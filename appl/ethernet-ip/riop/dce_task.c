/* dce_task.c - CentriFeeder DCE Modbus RTU master
 *
 * Location: appl\ethernet-ip\riop\dce_task.c
 *
 * FreeModbus is always linked into M7FOLLOWER. Runtime is gated by
 * DCE_MODBUS_RUNTIME (default 1 in dce_task.h).
 *
 * G48: page/mode-selected scan. Two tasks run:
 *   mbpoll  - eMBMasterPoll() forever; owns master init/enable.
 *   dce     - back-to-back FC03 reads (no inter-request gap) following the
 *             poll set chosen by the PC through asm 173 (dce_poll_table.h is
 *             the ONE file that defines sets -> registers).
 *
 * The poll-set selector is a LOCAL setting. It never generates a Modbus
 * request. The only Modbus write this firmware can issue is the allowlisted
 * mailbox write 0x0076 -> register 1, executed by this task at a transaction
 * boundary (so a continuous back-to-back scan cannot starve it).
 *
 * Bench diagnostics: GDB "p dceDiag"; asm 172 diag block (96..127).
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include "dce_task.h"

#if DCE_GPIO_BITBANG_TEST && !DCE_GPIO_BITBANG_ON_M33
#include "fsl_clock.h"
#include "fsl_iomuxc.h"
#include "fsl_rgpio.h"
#endif

/* Selector via a reserved part of the mailbox asm 171 as a FALLBACK to asm 173:
 * flags bit1 = "selector valid", flags bit2 = Y wanted, flags bits 15..8 = set id.
 * A frame with bit1 set and bit0 (arm) clear touches NOTHING of the mailbox
 * (seq / ack / arm logic). Every such frame counts as a heartbeat. */
#ifndef DCE_POLLSEL_VIA_171
#define DCE_POLLSEL_VIA_171     1
#endif

#if DCE_MODBUS_RUNTIME && !DCE_GPIO_BITBANG_TEST
#include "mb.h"
#include "mb_m.h"
#include "mbport.h"

#include "dce_app.h"
#include "modbus_cb.h"
#include "dce_poll_table.h"
#include "dce_tag_table.h"

#define DCE_TASK_PRIORITY       4u
#define DCE_TASK_STACK_SIZE     (configMINIMAL_STACK_SIZE + 256)

/* Priority notes (M7 FreeRTOS, verified against this tree):
 *   GOAL Main maps to FreeRTOS ~0 (GOAL prio 1, scale 0..100 -> 0..31).
 *   EIP Eth Rx/Tx (GOAL 75/85) run on CM33, not here.
 *   Local RIOP: AFE=7, SIGGEN=6, GPIO=5.
 * mbpoll must outrank the dce requester so responses are pumped promptly,
 * and stay at/below GPIO so a busy Modbus path cannot starve AFE sampling.
 * It will preempt GOAL Main on M7 - keep poll work short (queue wait yields).
 * The dce task blocks on a semaphore for the whole of every transaction, so a
 * back-to-back scan leaves the CPU free for GOAL Main. */
#define MB_POLL_TASK_PRIORITY   5u
#define MB_POLL_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE + 1024)

/* How long a request may wait for the stack to become free. The response
 * timeout itself is MB_MASTER_TIMEOUT_US_RESPOND, enforced by the TPM. */
#define DCE_REQ_WAIT_MS         500u

/* G49: idle gap after every transaction, ms. G48 measured ~11 % failed
 * transactions with 0; runtime-adjustable through asm 173 `reserved`. */
#ifndef DCE_INTER_REQ_GAP_MS
#define DCE_INTER_REQ_GAP_MS    0u
#endif
#define DCE_TICKS_TO_MS(t)      ((uint32_t)(((uint64_t)(t) * 1000u) / (uint64_t)configTICK_RATE_HZ))
#define DCE_NOW_MS()            DCE_TICKS_TO_MS(xTaskGetTickCount())
#define DCE_MAX_READ_WORDS      32u

_Static_assert(DCE_FAST_WORDS <= 33u, "asm 172 data area holds 33 words (fastrun layout)");
_Static_assert(sizeof(dceTags) / sizeof(dceTags[0]) == GOAL_APP_DCE_MIRROR_TAG_COUNT, "tag table size");
_Static_assert(DCE_G_COUNT == 6, "asm 172 carries 6 group counters (F, M1..M5)");
#endif /* DCE_MODBUS_RUNTIME && !DCE_GPIO_BITBANG_TEST */

static DCE_DATA_T dceData;

/* Deliberately NOT static: GDB can then find it by name without needing
 * the file scope ("p dceDiag"). */
DCE_DIAG_T dceDiag;

static DCE_MIRROR_SNAPSHOT_T s_dceMirror;
static uint32_t s_mailboxLastArmedSeq;
static bool s_mailboxArmedSeqSeen;
static uint32_t s_mailboxAcceptedCount;
static uint32_t s_mailboxDeniedCount;

#define DCE_ALLOWED_SLAVE_ADDR  1u
#define DCE_ALLOWED_REG_ADDR    1u
#define DCE_ALLOWED_VALUE_WORD0 0x0076u
#define DCE_MB_EX_ILLEGAL_FUNCTION 0x01u
#define DCE_MB_EX_ILLEGAL_ADDRESS  0x02u
#define DCE_MB_EX_ILLEGAL_VALUE    0x03u
#define DCE_MB_EX_SLAVE_FAILURE    0x04u

static void dceMirrorQualitySetAll(bool valid)
{
    uint8_t fill = valid ? 0xFFu : 0x00u;
    memset(s_dceMirror.quality, fill, sizeof(s_dceMirror.quality));
    if (valid) {
        s_dceMirror.quality[GOAL_APP_DCE_MIRROR_QUALITY_BYTES - 1u] &= 0x03u;
    }
}

#if DCE_GPIO_BITBANG_TEST && !DCE_GPIO_BITBANG_ON_M33
/* AD_32 = GPIO5_IO00 â†’ U35 DI; AD_35 = GPIO5_IO03 â†’ U35 DE//RE.
 * AD_33 = GPIO5_IO01 â† U35 RO (input only â€” never drive).
 * R310 pulls DE low when the pad is Hi-Z â€” if you only ever see DE low, the
 * MCU is not actually driving (clock/mux/task), or the probe is past an open SJ6. */
#define DCE_BB_GPIO             RGPIO5
#define DCE_BB_PIN_DI           0u
#define DCE_BB_PIN_RO           1u
#define DCE_BB_PIN_DE           3u
#define DCE_BB_HALF_PERIOD_MS   500u
/* AD pad: FSEL=0, PUE=0 (no pull), ODE=0, DSE=1 (high drive) â€” typical EVK GPIO. */
#define DCE_BB_PAD_CFG          0x06u

static void dce_gpioBitbangTask(void *arg)
{
    rgpio_pin_config_t outCfg = { .pinDirection = kRGPIO_DigitalOutput, .outputLogic = 0u };
    rgpio_pin_config_t inCfg  = { .pinDirection = kRGPIO_DigitalInput,  .outputLogic = 0u };
    (void)arg;

    CLOCK_EnableClock(kCLOCK_Iomuxc1);
    CLOCK_EnableClock(kCLOCK_Gpio5);

    /* Steal pads from LPUART10 (boot pinmux). */
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_32_GPIO5_IO00, 0U);
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_33_GPIO5_IO01, 1U);
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_35_GPIO5_IO03, 0U);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_32_GPIO5_IO00, DCE_BB_PAD_CFG);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_35_GPIO5_IO03, DCE_BB_PAD_CFG);

    RGPIO_PinInit(DCE_BB_GPIO, DCE_BB_PIN_DI, &outCfg);
    RGPIO_PinInit(DCE_BB_GPIO, DCE_BB_PIN_DE, &outCfg);
    RGPIO_PinInit(DCE_BB_GPIO, DCE_BB_PIN_RO, &inCfg);

    /* Mark alive for GDB: polls increments ~1 Hz while this task runs. */
    dceDiag.mbState = DCE_MB_RUNNING;

    for (;;) {
        /* In-phase 1 Hz square on DI and DE â€” easiest to spot on a scope. */
        RGPIO_PinWrite(DCE_BB_GPIO, DCE_BB_PIN_DE, 1U);
        RGPIO_PinWrite(DCE_BB_GPIO, DCE_BB_PIN_DI, 1U);
        dceDiag.polls++;
        vTaskDelay(pdMS_TO_TICKS(DCE_BB_HALF_PERIOD_MS));
        RGPIO_PinWrite(DCE_BB_GPIO, DCE_BB_PIN_DE, 0U);
        RGPIO_PinWrite(DCE_BB_GPIO, DCE_BB_PIN_DI, 0U);
        vTaskDelay(pdMS_TO_TICKS(DCE_BB_HALF_PERIOD_MS));
    }
}
#endif /* DCE_GPIO_BITBANG_TEST && !DCE_GPIO_BITBANG_ON_M33 */

#if DCE_MODBUS_RUNTIME && !DCE_GPIO_BITBANG_TEST
/* Set true by the poll task once eMBMasterInit()/Enable() have run.
 * The requester task waits on this so it never issues a request
 * before the master stack exists. */
static volatile bool dceMbReady = false;

/* Rate-from-delta state for totalization (see dce_updateFlowFromMirror). */
static float    s_prevTotal     = 0.0f;
static bool     s_havePrevTotal = false;
static uint32_t s_lastFlowMs    = 0u;

/* ---- link / freshness state (dce task is the only writer) ---------------- */
static bool     s_linkUp;                       /* a good read within DCE_STALE_TIMEOUT_MS */
static uint32_t s_lastAnyGoodMs;
static uint32_t s_wordMs[GOAL_APP_DCE_MIRROR_REG_WORDS];
static uint8_t  s_wordSeen[GOAL_APP_DCE_MIRROR_REG_WORDS];

/* ---- asm 172 compact image ------------------------------------------------ */
static uint32_t s_pubSeq;
static uint32_t s_tickMs;
static uint8_t  s_gSeq[DCE_G_COUNT];
static uint32_t s_gMs[DCE_G_COUNT];
static uint8_t  s_gSeen[DCE_G_COUNT];
static uint16_t s_fastWords[DCE_FAST_WORDS];

typedef struct __attribute__((packed)) {
    uint32_t pubSeq;
    uint32_t tickMs;
    uint16_t status;
    uint16_t planVersion;
    uint8_t  groupSeq[6];
    uint16_t selSeqEcho;
    uint32_t ackSeq;
    uint16_t ackCode;
    uint16_t ackDetail;
    uint16_t words[33];
    uint16_t pad;
    uint16_t diag[16];
} DCE_FAST_ASM_T;
_Static_assert(sizeof(DCE_FAST_ASM_T) == GOAL_APP_ASM_SIZE_DCE_FAST_INPUT, "asm 172 size");

#define DCE_FAST_PLAN_VERSION   1u
#define DCE_FAST_ST_LINK        0x0001u
#define DCE_FAST_ST_F_VALID     0x0002u   /* RUN set only */
#define DCE_FAST_ST_M_VALID     0x0004u   /* RUN set only: all 5 M groups fresh */
#define DCE_FAST_ST_SET_SHIFT   4u        /* bits 5..4 = active set */
#define DCE_FAST_ST_YFLAG       0x0040u
#define DCE_FAST_ST_FELLBACK    0x0080u   /* selector timed out since last accept */
#define DCE_FAST_ST_SEL_REJECT  0x0100u   /* last selector write had an unknown set id */
#define DCE_FAST_ST_WR_PENDING  0x0200u
#define DCE_FAST_ST_PLAN_ERR    0x0400u

/* ---- poll-set selector (writer: GOAL task; consumer: dce task) ------------ */
static volatile uint8_t  s_selReqSet = DCE_POLLSET_DEFAULT;
static volatile uint8_t  s_selReqFlags;
static volatile uint16_t s_selSeqEcho;
static volatile uint16_t s_selLastSeq;
static volatile bool     s_selSeqSeen;
static volatile uint32_t s_selHbMs;
static volatile bool     s_selRejected;
static volatile bool     s_selFellBack;
static volatile uint16_t s_selFallbackCount;
static uint16_t          s_sel171Seq;
static volatile uint8_t  s_gapMs = DCE_INTER_REQ_GAP_MS;
static volatile uint8_t  s_activeSet = DCE_POLLSET_DEFAULT;
static volatile uint8_t  s_activeFlags;
static uint16_t          s_setSwitches;
static bool              s_planError;

/* schedule cursor */
static uint8_t s_phase;     /* 0..nFixed-1 = fixed reads, nFixed = the round-robin slot */
static uint8_t s_rrPos;

/* ---- timing statistics ----------------------------------------------------- */
typedef struct { uint32_t n, sum, last, max; } dce_stat_t;
static dce_stat_t s_lat;    /* DCE reply latency: TX done -> first reply byte, ms */
static dce_stat_t s_txn;    /* whole transaction: request start -> result, ms     */
static uint32_t   s_cycleStartMs, s_cycleLastMs, s_cycles;
static uint32_t   s_rxShort, s_rxLong, s_rxEq;   /* G50: REV_DATA classified by accepted length vs expected */

static void dceStatAdd(dce_stat_t *s, uint32_t v)
{
    s->last = v;
    if (v > s->max) { s->max = v; }
    s->sum += v;
    s->n++;
    if (s->n >= 4096u) { s->n >>= 1; s->sum >>= 1; }
}

/* ---- pending mailbox write (handed from GOAL task to dce task) ------------ */
static volatile bool s_wrPending;
static DCE_MIRROR_WRITE_REQ_T s_wrReq;
static uint16_t s_wrImageIndex;

static bool dceMirrorAddressToIndex(uint16_t address, uint16_t *pWordIndex)
{
    uint32_t i;
    for (i = 0u; i < (uint32_t)(sizeof(dceImageBlocks) / sizeof(dceImageBlocks[0])); i++) {
        const dce_image_blk_t *blk = &dceImageBlocks[i];
        if ((address >= blk->startAddr) && (address < (uint16_t)(blk->startAddr + blk->words))) {
            *pWordIndex = (uint16_t)(blk->baseIndex + (address - blk->startAddr));
            return true;
        }
    }
    return false;
}

static bool dceMirrorReadRegWords(uint16_t address, uint16_t nWords, uint16_t *pOut)
{
    uint16_t idx = 0u;
    if ((nWords == 0u) || !dceMirrorAddressToIndex(address, &idx)) {
        return false;
    }
    if ((uint16_t)(idx + nWords) > GOAL_APP_DCE_MIRROR_REG_WORDS) {
        return false;
    }

    taskENTER_CRITICAL();
    memcpy(pOut, &s_dceMirror.regWords[idx], nWords * sizeof(uint16_t));
    taskEXIT_CRITICAL();
    return true;
}

static void dce_diagCountErr(
    eMBMasterReqErrCode eErr
);
#endif

void dce_dataGet(
    DCE_DATA_T *pOut
)
{
    taskENTER_CRITICAL();
    *pOut = dceData;
    taskEXIT_CRITICAL();
}

void dce_diagGet(
    DCE_DIAG_T *pOut
)
{
    *pOut = dceDiag;
}

void dce_mirrorGetSnapshot(
    DCE_MIRROR_SNAPSHOT_T *pOut
)
{
    if (pOut == NULL) {
        return;
    }
    taskENTER_CRITICAL();
    *pOut = s_dceMirror;
    taskEXIT_CRITICAL();
}

#if DCE_MODBUS_RUNTIME && !DCE_GPIO_BITBANG_TEST
static void dce_dataSet(
    float flowRate,
    float totalization
)
{
    taskENTER_CRITICAL();
    dceData.flowRate     = flowRate;
    dceData.totalization = totalization;
    dceData.tsLastGood   = (uint32_t) xTaskGetTickCount();
    dceData.valid        = true;
    taskEXIT_CRITICAL();
}

static void dce_dataInvalidate(
    void
)
{
    /* Comms-lost (no good read at all for DCE_STALE_TIMEOUT_MS): zero rate and
     * clear valid, but keep last totalization so EIP does not flash 0 before
     * the next good Count sample restores it. Drop delta state so the first
     * sample after recovery does not spike. */
    taskENTER_CRITICAL();
    dceData.flowRate = 0.0f;
    dceData.valid    = false;
    s_dceMirror.commStatus = 1u;
    dceMirrorQualitySetAll(false);
    taskEXIT_CRITICAL();

    s_linkUp = false;
    s_havePrevTotal = false;
}

/* G48 per-word staleness: a tag's quality bit is set only while the link is up
 * AND every word of the tag was refreshed within DCE_STALE_TIMEOUT_MS. Words
 * the active poll set does not read age out individually; the rest of the image
 * stays valid. */
static void dceQualityRefresh(uint32_t nowMs)
{
    uint8_t q[GOAL_APP_DCE_MIRROR_QUALITY_BYTES];
    uint32_t i, w;

    memset(q, 0, sizeof(q));
    if (s_linkUp) {
        for (i = 0u; i < (uint32_t)GOAL_APP_DCE_MIRROR_TAG_COUNT; i++) {
            bool ok = true;
            for (w = 0u; w < dceTags[i].words; w++) {
                uint32_t k = (uint32_t)dceTags[i].imgIndex + w;
                if ((k >= GOAL_APP_DCE_MIRROR_REG_WORDS) || !s_wordSeen[k] ||
                    ((uint32_t)(nowMs - s_wordMs[k]) > DCE_STALE_TIMEOUT_MS)) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                q[i / 8u] |= (uint8_t)(1u << (i % 8u));
            }
        }
    }
    taskENTER_CRITICAL();
    memcpy(s_dceMirror.quality, q, sizeof(q));
    taskEXIT_CRITICAL();
}

/* FreeModbus master event pump. Must run in a task, not an ISR: the event
 * layer blocks on a queue. This is the change from NXP's demo, which called
 * eMBMasterPoll() from a TPM interrupt handler. */
static void dce_mbPollTask(void *pvParameters)
{
    (void) pvParameters;

    dceDiag.mbState = DCE_MB_TASK_ENTERED;

    if (MB_ENOERR != eMBMasterInit(MB_RTU, 0, DCE_MODBUS_BAUDRATE, MB_PAR_NONE)) {
        dceDiag.mbState = DCE_MB_INIT_FAILED;
        vTaskDelete(NULL);
        return;
    }

    if (MB_ENOERR != eMBMasterEnable()) {
        dceDiag.mbState = DCE_MB_ENABLE_FAILED;
        vTaskDelete(NULL);
        return;
    }

    dceDiag.mbState = DCE_MB_RUNNING;
    dceMbReady = true;

    for (;;) {
        (void) eMBMasterPoll();
    }
}

/* Assemble a big-endian 32-bit value from two consecutive registers.
 * The DCE's word ordering is High Then Low, so the first register is the
 * high half. */
static uint32_t dce_u32Get(
    const uint16_t *pRegs
)
{
    return ((uint32_t) pRegs[0] << 16) | (uint32_t) pRegs[1];
}

/* Legacy RIOP process values (adc15/16): Count/CWratio -> totalization/rate.
 * Called after every good read that covers 504..505. CWratio (506..507) comes
 * from the image (read by the SETUP scan; constant in practice). G48: rate is
 * only recomputed once >= 100 ms have passed since the last computation
 * (Run polls 504 at >10 Hz and Count is quantised); totalization and the
 * freshness timestamp update on every call. */
static void dce_updateFlowFromMirror(
    uint32_t nowMs
)
{
    uint16_t regs[4];
    uint32_t count;
    uint32_t cwratio;
    float totalNow;
    float flowRate = 0.0f;

    if (!dceMirrorReadRegWords(504u, 4u, regs)) {
        return;
    }

    count = dce_u32Get(&regs[0]);
    cwratio = dce_u32Get(&regs[2]);
    if (cwratio == 0u) {
        dceDiag.cwratioZero++;
        return;
    }

    totalNow = (float)count / (float)cwratio;

    if (s_havePrevTotal) {
        uint32_t dtMs = nowMs - s_lastFlowMs;
        if (dtMs < 100u) {
            dce_dataSet(dceData.flowRate, totalNow);
            return;
        }
        {
            float dTotal = totalNow - s_prevTotal;
            if (dTotal < 0.0f) {
                dTotal = 0.0f;
            }
            if (dtMs <= (DCE_STALE_TIMEOUT_MS * 2u)) {
                flowRate = dTotal * (60000.0f / (float)dtMs);
            }
        }
    }
    s_prevTotal = totalNow;
    s_lastFlowMs = nowMs;
    s_havePrevTotal = true;
    dce_dataSet(flowRate, totalNow);
}

static void dceMirrorAckPublish(uint32_t seq, uint16_t code, uint16_t detail)
{
    s_dceMirror.ackSeq = seq;
    s_dceMirror.ackCode = code;
    s_dceMirror.ackDetail = detail;
}

/* Sequence monotonicity with wrap handling on uint32 sequence IDs. */
static bool dceSeqIsNewer(uint32_t seq, uint32_t base)
{
    return ((int32_t)(seq - base) > 0);
}

/* G48: real wire exception byte if the DCE answered with an exception frame
 * (acc[] = addr, func|0x80, exception code); otherwise a LOCAL detail >= 0x80. */
static uint16_t dceAckDetailFromReqErr(eMBMasterReqErrCode eErr)
{
    if (eErr == MB_MRE_EXE_FUN) {
        if ((dceUartDiag.nAcc >= 3u) && ((dceUartDiag.acc[1] & 0x80u) != 0u) && (dceUartDiag.acc[2] != 0u)) {
            return dceUartDiag.acc[2];
        }
        return DCE_ACK_DETAIL_LOCAL_REJECT;
    }
    if (eErr == MB_MRE_TIMEDOUT) {
        return DCE_ACK_DETAIL_TIMEOUT;
    }
    if (eErr == MB_MRE_REV_DATA) {
        return DCE_ACK_DETAIL_BAD_FRAME;
    }
    return DCE_ACK_DETAIL_OTHER;
}

/* ------------------------------------------------------------------------ */
/* Poll-set selector. LOCAL setting only - nothing here can reach the DCE.   */
/* ------------------------------------------------------------------------ */
void dce_pollSelectorSet(uint16_t selSeq, uint8_t set, uint8_t flags)
{
    if (set >= DCE_POLLSET_COUNT) {
        s_selRejected = true;           /* unknown id: ignored, stay on current set */
        return;
    }

    taskENTER_CRITICAL();
    if (!s_selSeqSeen || (selSeq != s_selLastSeq)) {
        s_selLastSeq   = selSeq;
        s_selSeqSeen   = true;
        s_selSeqEcho   = selSeq;
        s_selHbMs      = DCE_NOW_MS();
        s_selReqSet    = set;
        s_selReqFlags  = (uint8_t)(flags & DCE_SELFLAG_Y);
        s_selRejected  = false;
        s_selFellBack  = false;
    }
    taskEXIT_CRITICAL();
}

void dce_pollGapSet(uint32_t reserved)
{
    if ((reserved & 0x80000000u) != 0u) {
        uint32_t g = reserved & 0xFFu;
        s_gapMs = (uint8_t)((g > 100u) ? 100u : g);
    }
    if ((reserved & 0x40000000u) != 0u) {
        /* G50: bits 23..16 = T3.5 in units of 100 us, accepted 10..100 (1..10 ms). */
        uint32_t t = (reserved >> 16) & 0xFFu;
        if ((t >= 10u) && (t <= 100u)) {
            dceT35Us = t * 100u;
        }
    }
}

/* dce task, once per transaction boundary: heartbeat timeout + set switch. */
static void dceSelectorService(uint32_t nowMs)
{
    uint8_t reqSet, reqFlags;

    taskENTER_CRITICAL();
    if ((s_selReqSet != DCE_POLLSET_DEFAULT) && ((uint32_t)(nowMs - s_selHbMs) > DCE_POLLSEL_TIMEOUT_MS)) {
        s_selReqSet   = DCE_POLLSET_DEFAULT;
        s_selReqFlags = 0u;
        s_selFellBack = true;
        s_selFallbackCount++;
    }
    reqSet   = s_selReqSet;
    reqFlags = s_selReqFlags;
    taskEXIT_CRITICAL();

    if ((reqSet != s_activeSet) || (reqFlags != s_activeFlags)) {
        s_activeSet   = reqSet;
        s_activeFlags = reqFlags;
        s_phase = 0u;
        s_rrPos = 0u;
        s_setSwitches++;
    }
}

/* Next read of the active set. A cycle = every FIXED read, then ONE round-robin
 * read (entries gated by a selector flag that is off are skipped). Returns the
 * read index; *pCycleEnd is set on the last read of a cycle; *pRrWrap when the
 * round-robin list has just completed one full lap. */
static uint8_t dceNextRead(bool *pCycleEnd, bool *pRrWrap)
{
    const dce_set_t *S = &dceSets[s_activeSet];
    uint8_t tries;

    *pCycleEnd = false;
    *pRrWrap = false;

    if (s_phase < S->nFixed) {
        uint8_t rd = S->fixed[s_phase].rd;
        s_phase++;
        if ((s_phase >= S->nFixed) && (S->nRr == 0u)) {
            s_phase = 0u;
            *pCycleEnd = true;
            *pRrWrap = true;            /* no round-robin list: one lap == one cycle */
        }
        return rd;
    }

    for (tries = 0u; tries < S->nRr; tries++) {
        const dce_sched_t *e = &S->rr[s_rrPos];
        s_rrPos++;
        if (s_rrPos >= S->nRr) {
            s_rrPos = 0u;
            *pRrWrap = true;
        }
        if ((e->needFlags & (uint8_t)~s_activeFlags) == 0u) {
            s_phase = 0u;
            *pCycleEnd = true;
            return e->rd;
        }
    }
    /* nothing selectable (should not happen with the shipped table) */
    s_phase = 0u;
    *pCycleEnd = true;
    return (S->nFixed > 0u) ? S->fixed[0].rd : (uint8_t)RD_F504;
}

/* Store one good read into the asm 170 image and the asm 172 compact image. */
static void dceStoreRead(const dce_rd_t *rd, const uint16_t *regs, uint32_t nowMs)
{
    uint32_t i;

    taskENTER_CRITICAL();
    for (i = 0u; i < rd->words; i++) {
        uint16_t idx;
        if (dceMirrorAddressToIndex((uint16_t)(rd->start + i), &idx)) {
            s_dceMirror.regWords[idx] = regs[i];
            s_wordMs[idx]   = nowMs;
            s_wordSeen[idx] = 1u;
        }
    }
    for (i = 0u; i < DCE_FAST_WORDS; i++) {
        uint16_t a = dceFastAddr[i];
        if ((a >= rd->start) && (a < (uint16_t)(rd->start + rd->words))) {
            s_fastWords[i] = regs[a - rd->start];
        }
    }
    if (rd->group != DCE_G_NONE) {
        s_gSeq[rd->group]++;
        s_gMs[rd->group]   = nowMs;
        s_gSeen[rd->group] = 1u;
    }
    s_pubSeq++;
    s_tickMs = nowMs;
    s_dceMirror.lastScanTicks = (uint32_t)xTaskGetTickCount();
    s_dceMirror.commStatus = 2u;
    taskEXIT_CRITICAL();
}

void dce_fastGetPayload(uint8_t *pOut, uint16_t len)
{
    DCE_FAST_ASM_T p;
    uint32_t nowMs = DCE_NOW_MS();
    uint8_t set = s_activeSet;
    uint16_t st = 0u;
    uint32_t g;
    bool fOk, mOk = true;

    if (pOut == NULL) {
        return;
    }
    memset(&p, 0, sizeof(p));

    taskENTER_CRITICAL();
    p.pubSeq = s_pubSeq;
    p.tickMs = s_tickMs;
    memcpy(p.groupSeq, s_gSeq, sizeof(p.groupSeq));
    memcpy(p.words, s_fastWords, sizeof(s_fastWords));
    p.ackSeq    = s_dceMirror.ackSeq;
    p.ackCode   = s_dceMirror.ackCode;
    p.ackDetail = s_dceMirror.ackDetail;
    if (s_dceMirror.commStatus == 2u) { st |= DCE_FAST_ST_LINK; }
    fOk = (s_gSeen[DCE_G_F] != 0u) && ((uint32_t)(nowMs - s_gMs[DCE_G_F]) <= DCE_STALE_TIMEOUT_MS);
    for (g = DCE_G_M1; g <= DCE_G_M5; g++) {
        if (!s_gSeen[g] || ((uint32_t)(nowMs - s_gMs[g]) > DCE_STALE_TIMEOUT_MS)) { mOk = false; }
    }
    taskEXIT_CRITICAL();

    p.planVersion = DCE_FAST_PLAN_VERSION;
    p.pad = (uint16_t)(s_gapMs | ((dceT35Us / 100u) << 8));                    /* G49: current idle gap, ms */
    p.selSeqEcho  = s_selSeqEcho;
    if (set == DCE_POLLSET_RUN) {
        if (fOk) { st |= DCE_FAST_ST_F_VALID; }
        if (mOk) { st |= DCE_FAST_ST_M_VALID; }
    }
    st |= (uint16_t)((set & 0x3u) << DCE_FAST_ST_SET_SHIFT);
    if (s_activeFlags & DCE_SELFLAG_Y) { st |= DCE_FAST_ST_YFLAG; }
    if (s_selFellBack)  { st |= DCE_FAST_ST_FELLBACK; }
    if (s_selRejected)  { st |= DCE_FAST_ST_SEL_REJECT; }
    if (s_wrPending)    { st |= DCE_FAST_ST_WR_PENDING; }
    if (s_planError)    { st |= DCE_FAST_ST_PLAN_ERR; }
    p.status = st;

    p.diag[0]  = (uint16_t)dceDiag.polls;
    p.diag[1]  = (uint16_t)dceDiag.good;
    p.diag[2]  = (uint16_t)dceDiag.timeouts;
    p.diag[3]  = (uint16_t)dceDiag.rxErrors;
    p.diag[4]  = (uint16_t)dceDiag.exceptions;
    p.diag[5]  = (uint16_t)s_rxEq;      /* G50: bad frame with the expected length (CRC/addr) */
    p.diag[6]  = (uint16_t)dceUartDiag.rxDropped;
    p.diag[7]  = (uint16_t)s_lat.last;
    p.diag[8]  = (uint16_t)(s_lat.n ? (s_lat.sum * 10u) / s_lat.n : 0u);   /* 0.1 ms units */
    p.diag[9]  = (uint16_t)s_lat.max;
    p.diag[10] = (uint16_t)s_txn.last;
    p.diag[11] = (uint16_t)(s_txn.n ? (s_txn.sum * 10u) / s_txn.n : 0u);   /* 0.1 ms units */
    p.diag[12] = (uint16_t)s_txn.max;
    p.diag[13] = (uint16_t)s_rxShort;   /* G50: bad frame SHORTER than expected (truncated) */
    p.diag[14] = (uint16_t)s_rxLong;    /* G50: bad frame LONGER than expected */
    p.diag[15] = (uint16_t)(((s_selFallbackCount & 0xFFu) << 8) | (s_setSwitches & 0xFFu));

    memcpy(pOut, &p, (len < sizeof(p)) ? len : sizeof(p));
}

/* ------------------------------------------------------------------------ */
/* Mailbox write (asm 171). Validation runs in the caller's (GOAL) context; the
 * Modbus write itself is executed by the dce task at the next transaction
 * boundary so a back-to-back scan cannot starve it.                          */
/* ------------------------------------------------------------------------ */
void dce_mirrorHandleWriteRequest(
    const DCE_MIRROR_WRITE_REQ_T *pReq
)
{
    uint16_t imageIndex = 0u;
    bool seqValid = true;
    bool armSet;

    if (pReq == NULL) {
        return;
    }

    armSet = ((pReq->flags & 0x0001u) != 0u);

#if DCE_POLLSEL_VIA_171
    /* Fallback selector path (see top of file). Never touches mailbox state. */
    if ((pReq->flags & 0x0002u) != 0u) {
        dce_pollSelectorSet(s_sel171Seq++, (uint8_t)(pReq->flags >> 8),
                            ((pReq->flags & 0x0004u) != 0u) ? DCE_SELFLAG_Y : 0u);
        if (!armSet) {
            return;
        }
    }
#endif

    if (!armSet) {
        /* Clearing arm with the same sequence must be a no-op, not an error. */
        taskENTER_CRITICAL();
        if (s_mailboxArmedSeqSeen && (pReq->seq == s_mailboxLastArmedSeq)) {
            taskEXIT_CRITICAL();
            return;
        }
        if (s_mailboxArmedSeqSeen && !dceSeqIsNewer(pReq->seq, s_mailboxLastArmedSeq)) {
            dceMirrorAckPublish(pReq->seq, DCE_ACK_SEQ_REPLAY, 0u);
            s_mailboxDeniedCount++;
            taskEXIT_CRITICAL();
            return;
        }
        dceMirrorAckPublish(pReq->seq, DCE_ACK_NEEDS_ARM, DCE_MB_EX_ILLEGAL_VALUE);
        s_mailboxDeniedCount++;
        taskEXIT_CRITICAL();
        return;
    }

    if (pReq->seq == 0u) {
        taskENTER_CRITICAL();
        dceMirrorAckPublish(pReq->seq, DCE_ACK_REJECTED, DCE_MB_EX_ILLEGAL_VALUE);
        s_mailboxDeniedCount++;
        taskEXIT_CRITICAL();
        return;
    }

    taskENTER_CRITICAL();
    if (s_mailboxArmedSeqSeen) {
        seqValid = dceSeqIsNewer(pReq->seq, s_mailboxLastArmedSeq);
    }
    if (!seqValid) {
        dceMirrorAckPublish(pReq->seq, DCE_ACK_SEQ_REPLAY, 0u);
        s_mailboxDeniedCount++;
        taskEXIT_CRITICAL();
        return;
    }
    s_mailboxLastArmedSeq = pReq->seq;
    s_mailboxArmedSeqSeen = true;
    taskEXIT_CRITICAL();

    /* DEV 2026-11-06 (Daniel): allow all mailbox op=1 writes during bring-up.
     * Still requires valid length and a mappable address (checked below).
     * Fuse/OTP not reachable via this mailbox path. */
    if ((pReq->op != 1u) ||
        (DCE_MODBUS_SLAVE_ADDR != DCE_ALLOWED_SLAVE_ADDR) ||
        (pReq->lengthWords < 1u) ||
        (pReq->lengthWords > GOAL_APP_DCE_MIRROR_MAX_WRITE_WORDS)) {
        taskENTER_CRITICAL();
        dceMirrorAckPublish(pReq->seq, DCE_ACK_REJECTED, DCE_MB_EX_ILLEGAL_FUNCTION);
        s_mailboxDeniedCount++;
        taskEXIT_CRITICAL();
        return;
    }

    if (!dceMirrorAddressToIndex(pReq->address, &imageIndex)) {
        taskENTER_CRITICAL();
        dceMirrorAckPublish(pReq->seq, DCE_ACK_INVALID_ADDR, DCE_MB_EX_ILLEGAL_ADDRESS);
        s_mailboxDeniedCount++;
        taskEXIT_CRITICAL();
        return;
    }

#if !DCE_MIRROR_WRITES
    taskENTER_CRITICAL();
    dceMirrorAckPublish(pReq->seq, DCE_ACK_WRITES_DISABLED, 0u);
    s_mailboxDeniedCount++;
    taskEXIT_CRITICAL();
    return;
#else
    taskENTER_CRITICAL();
    if (s_wrPending) {
        dceMirrorAckPublish(pReq->seq, DCE_ACK_REJECTED, DCE_ACK_DETAIL_WRITE_PENDING);
        s_mailboxDeniedCount++;
        taskEXIT_CRITICAL();
        return;
    }
    s_wrReq = *pReq;
    s_wrImageIndex = imageIndex;
    s_wrPending = true;
    taskEXIT_CRITICAL();
#endif
}

static void dce_diagCountErr(
    eMBMasterReqErrCode eErr
)
{
    dceDiag.lastErr = (int32_t) eErr;

    switch (eErr) {
        case MB_MRE_NO_ERR:      break;
        case MB_MRE_TIMEDOUT:    dceDiag.timeouts++;    break;
        case MB_MRE_REV_DATA:    dceDiag.rxErrors++;    break;
        case MB_MRE_EXE_FUN:     dceDiag.exceptions++;  break;
        default:                 dceDiag.otherErrors++; break; /* incl. MASTER_BUSY */
    }
}

#if DCE_MIRROR_WRITES
/* Runs in the dce task between two transactions. */
static void dceExecutePendingWrite(void)
{
    DCE_MIRROR_WRITE_REQ_T req;
    uint16_t idx;
    eMBMasterReqErrCode eErr;
    uint16_t writeBuf[GOAL_APP_DCE_MIRROR_MAX_WRITE_WORDS];

    if (!s_wrPending) {
        return;
    }
    taskENTER_CRITICAL();
    req = s_wrReq;
    idx = s_wrImageIndex;
    taskEXIT_CRITICAL();

    mbcb_rawInvalidate();
    dceUartDiag.nFirst = 0u;
    dceUartDiag.nAcc = 0u;
    if (req.lengthWords == 1u) {
        eErr = eMBMasterReqWriteHoldingRegister(
            DCE_MODBUS_SLAVE_ADDR,
            req.address,
            req.values[0],
            pdMS_TO_TICKS(DCE_REQ_WAIT_MS)
        );
    } else {
        writeBuf[0] = req.values[0];
        writeBuf[1] = req.values[1];
        eErr = eMBMasterReqWriteMultipleHoldingRegister(
            DCE_MODBUS_SLAVE_ADDR,
            req.address,
            req.lengthWords,
            writeBuf,
            pdMS_TO_TICKS(DCE_REQ_WAIT_MS)
        );
    }
    dce_diagCountErr(eErr);

    taskENTER_CRITICAL();
    if (MB_MRE_NO_ERR == eErr) {
        uint16_t i;
        /* Apply write image before publishing the ack state. */
        for (i = 0u; i < req.lengthWords; i++) {
            s_dceMirror.regWords[idx + i] = req.values[i];
            s_wordMs[idx + i]   = DCE_NOW_MS();
            s_wordSeen[idx + i] = 1u;
        }
        s_dceMirror.commStatus = 2u;
        s_dceMirror.lastScanTicks = (uint32_t)xTaskGetTickCount();
        dceMirrorAckPublish(req.seq, DCE_ACK_OK, 0u);
        s_mailboxAcceptedCount++;
    } else {
        dceMirrorAckPublish(req.seq, DCE_ACK_REJECTED, dceAckDetailFromReqErr(eErr));
        s_mailboxDeniedCount++;
    }
    s_wrPending = false;
    taskEXIT_CRITICAL();
}
#endif

static void dce_task(
    void *pvParameters
)
{
    uint16_t regs[DCE_MAX_READ_WORDS];

    (void) pvParameters;

    /* Wait until the poll task has initialised the master stack. */
    while (!dceMbReady) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    s_cycleStartMs = DCE_NOW_MS();

    for (;;) {
        eMBMasterReqErrCode eErr;
        const dce_rd_t *rd;
        bool cycleEnd, rrWrap;
        uint32_t nowMs, tReq, tEnd;
        uint8_t rdIdx;

        nowMs = DCE_NOW_MS();
        dceSelectorService(nowMs);
#if DCE_MIRROR_WRITES
        dceExecutePendingWrite();
#endif

        rdIdx = dceNextRead(&cycleEnd, &rrWrap);
        rd = &dceReads[rdIdx];
        if ((rdIdx >= RD_COUNT) || (rd->words == 0u) || (rd->words > DCE_MAX_READ_WORDS)) {
            s_planError = true;         /* table error: never issue a bad request */
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }

        /* A failed transaction must not leave the previous reply looking
         * fresh to mbcb_rawGet(). */
        mbcb_rawInvalidate();
        dceDiag.polls++;
        dceUartDiag.nFirst = 0u;
        dceUartDiag.nAcc = 0u;
        dceTiming.txDoneValid = 0u;
        dceTiming.rxFirstValid = 0u;
        tReq = (uint32_t)xTaskGetTickCount();

        eErr = eMBMasterReqReadHoldingRegister(DCE_MODBUS_SLAVE_ADDR,
                                               rd->start,
                                               rd->words,
                                               pdMS_TO_TICKS(DCE_REQ_WAIT_MS));
        tEnd = (uint32_t)xTaskGetTickCount();
        dce_diagCountErr(eErr);
        if (MB_MRE_REV_DATA == eErr) {
            uint32_t expect = 5u + 2u * rd->words;
            if (dceUartDiag.nAcc < expect) { s_rxShort++; }
            else if (dceUartDiag.nAcc > expect) { s_rxLong++; }
            else { s_rxEq++; }
        }
        dceStatAdd(&s_txn, DCE_TICKS_TO_MS(tEnd - tReq));
        if (dceTiming.txDoneValid && dceTiming.rxFirstValid) {
            dceStatAdd(&s_lat, DCE_TICKS_TO_MS(dceTiming.rxFirstTick - dceTiming.txDoneTick));
        }

        nowMs = DCE_NOW_MS();
        if (MB_MRE_NO_ERR == eErr) {
            if (mbcb_rawGet(regs, rd->words)) {
                dceDiag.good++;
                if ((rd->start <= 504u) && ((uint16_t)(rd->start + rd->words) >= 508u)) {
                    memcpy(dceDiag.lastRegs, &regs[504u - rd->start], sizeof(dceDiag.lastRegs));
                }

                s_linkUp = true;
                s_lastAnyGoodMs = nowMs;
                dceStoreRead(rd, regs, nowMs);

                if ((rd->start <= 504u) && ((uint16_t)(rd->start + rd->words) >= 506u)) {
                    dce_updateFlowFromMirror(nowMs);
                }
            } else {
                dceDiag.rawMismatch++;
            }
        }

        if (rrWrap) {
            taskENTER_CRITICAL();
            s_dceMirror.scanCounter++;      /* one full lap of the active set's round-robin list */
            taskEXIT_CRITICAL();
        }
        if (cycleEnd) {
            s_cycles++;
            s_cycleLastMs = nowMs - s_cycleStartMs;
            s_cycleStartMs = nowMs;
        }

        /* Link staleness: no good read at all for DCE_STALE_TIMEOUT_MS. Silence
         * is a NORMAL state (the DCE only speaks Modbus when enabled from its
         * front panel). Per-word staleness is handled by dceQualityRefresh(). */
        if (s_linkUp && ((uint32_t)(nowMs - s_lastAnyGoodMs) > DCE_STALE_TIMEOUT_MS)) {
            dce_dataInvalidate();
            dceDiag.staleEvents++;
        }
        dceQualityRefresh(nowMs);

        /* G49: optional idle gap (default DCE_INTER_REQ_GAP_MS, adjustable). */
        if (s_gapMs != 0u) {
            vTaskDelay(pdMS_TO_TICKS(s_gapMs));
        }
    }
}
#endif /* DCE_MODBUS_RUNTIME && !DCE_GPIO_BITBANG_TEST */

#if !DCE_MODBUS_RUNTIME || DCE_GPIO_BITBANG_TEST
void dce_mirrorHandleWriteRequest(
    const DCE_MIRROR_WRITE_REQ_T *pReq
)
{
    (void)pReq;
    taskENTER_CRITICAL();
    s_dceMirror.ackCode = DCE_ACK_WRITES_DISABLED;
    s_dceMirror.ackDetail = 0u;
    taskEXIT_CRITICAL();
}

void dce_pollSelectorSet(uint16_t selSeq, uint8_t set, uint8_t flags)
{
    (void)selSeq; (void)set; (void)flags;
}

void dce_pollGapSet(uint32_t reserved)
{
    (void)reserved;
}

void dce_fastGetPayload(uint8_t *pOut, uint16_t len)
{
    if (pOut != NULL) {
        memset(pOut, 0, len);
    }
}
#endif

void dce_taskInit(void)
{
    memset(&dceData, 0, sizeof(dceData));
    memset(&dceDiag, 0, sizeof(dceDiag));
    memset(&s_dceMirror, 0, sizeof(s_dceMirror));
    s_dceMirror.commStatus = 1u;
    s_mailboxLastArmedSeq = 0u;
    s_mailboxArmedSeqSeen = false;
    s_mailboxAcceptedCount = 0u;
    s_mailboxDeniedCount = 0u;
    s_dceMirror.ackSeq = 0u;
    s_dceMirror.ackDetail = 0u;
    dceMirrorQualitySetAll(false);

#if DCE_GPIO_BITBANG_TEST && !DCE_GPIO_BITBANG_ON_M33
    dceDiag.mbState = DCE_MB_NOT_STARTED;
    if (pdPASS != xTaskCreate(dce_gpioBitbangTask, "dce_bb",
                              configMINIMAL_STACK_SIZE + 256, NULL, 4u, NULL)) {
        dceDiag.mbState = DCE_MB_CREATE_FAILED;
    }
#elif DCE_GPIO_BITBANG_TEST && DCE_GPIO_BITBANG_ON_M33
    /* Bitbang runs on M33 (riop_EIP_M33LEADER.c); M7 stays idle-linked. */
    dceDiag.mbState = DCE_MB_IDLE_LINKED;
#elif !DCE_MODBUS_RUNTIME
    /* Linked-but-idle: adc15/adc16 stay 0.0 via dce_dataGet(); no HW/tasks.
     * Matches 71ce78e behaviour (Modbus in image, EIP still connects). */
    dceDiag.mbState = DCE_MB_IDLE_LINKED;
    s_dceMirror.commStatus = 1u;
#else
    dceMbReady = false;
    dceDiag.mbState = DCE_MB_NOT_STARTED;

    /* Boot default = SETUP (today's full scan). */
    s_selReqSet = DCE_POLLSET_DEFAULT;
    s_selReqFlags = 0u;
    s_activeSet = DCE_POLLSET_DEFAULT;
    s_activeFlags = 0u;
    s_phase = 0u;
    s_rrPos = 0u;

    if (pdPASS != xTaskCreate(dce_mbPollTask, "mbpoll", MB_POLL_TASK_STACK_SIZE,
                              NULL, MB_POLL_TASK_PRIORITY, NULL)) {
        dceDiag.mbState = DCE_MB_CREATE_FAILED;
        return;
    }

    if (pdPASS != xTaskCreate(dce_task, "dce", DCE_TASK_STACK_SIZE,
                              NULL, DCE_TASK_PRIORITY, NULL)) {
        dceDiag.mbState = DCE_MB_CREATE_FAILED;
        return;
    }
#endif
}