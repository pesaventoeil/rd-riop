/* dce_task.c - CentriFeeder DCE Modbus RTU master
 *
 * Location: appl\ethernet-ip\riop\dce_task.c
 *
 * FreeModbus is always linked into M7FOLLOWER. Runtime is gated by
 * DCE_MODBUS_RUNTIME (default 0 in dce_task.h):
 *   0 = idle: clear published floats, do not touch LPUART10/TPM6 or create
 *       Modbus tasks. Use this until PC EIP Class 1 / MCTC are healthy again.
 *   1 = live FC03 poll of holding regs 504..507 over RS-485 on LPUART10.
 *
 * When runtime is on, two tasks run:
 *   mbpoll  - eMBMasterPoll() forever; owns master init/enable.
 *   dce     - one FC03 every DCE_POLL_PERIOD_MS; FlowRate/Totalization + stale.
 *
 * Bench diagnostics: GDB "p dceDiag".
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include "dce_task.h"

#if DCE_MODBUS_RUNTIME
#include "mb.h"
#include "mb_m.h"
#include "mbport.h"

#include "dce_app.h"
#include "modbus_cb.h"

#define DCE_POLL_PERIOD_MS      100u
#define DCE_TASK_PRIORITY       4u
#define DCE_TASK_STACK_SIZE     (configMINIMAL_STACK_SIZE + 256)

/* Priority notes (M7 FreeRTOS, verified against this tree):
 *   GOAL Main maps to FreeRTOS ~0 (GOAL prio 1, scale 0..100 → 0..31).
 *   EIP Eth Rx/Tx (GOAL 75/85) run on CM33, not here.
 *   Local RIOP: AFE=7, SIGGEN=6, GPIO=5.
 * mbpoll must outrank the dce requester so responses are pumped promptly,
 * and stay at/below GPIO so a busy Modbus path cannot starve AFE sampling.
 * It will preempt GOAL Main on M7 — keep poll work short (queue wait yields). */
#define MB_POLL_TASK_PRIORITY   5u
#define MB_POLL_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE + 1024)

/* How long a request may wait for the stack to become free. The response
 * timeout itself is MB_MASTER_TIMEOUT_US_RESPOND, enforced by the TPM. */
#define DCE_REQ_WAIT_MS         500u

/* Publish zero and clear valid after this long without a good poll. Silence
 * is a NORMAL state: the DCE only speaks Modbus when enabled from its front
 * panel, and drops out of eMBSPoll() the moment that is switched off. */
#define DCE_STALE_TIMEOUT_MS    1000u
#endif /* DCE_MODBUS_RUNTIME */

static DCE_DATA_T dceData;

/* Deliberately NOT static: GDB can then find it by name without needing
 * the file scope ("p dceDiag"). */
DCE_DIAG_T dceDiag;

#if DCE_MODBUS_RUNTIME
/* Set true by the poll task once eMBMasterInit()/Enable() have run.
 * The requester task waits on this so it never issues a request
 * before the master stack exists. */
static volatile bool dceMbReady = false;

/* Rate-from-delta state for totalization (see dce_task). Cleared on stale. */
static float s_prevTotal     = 0.0f;
static bool  s_havePrevTotal = false;
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

#if DCE_MODBUS_RUNTIME
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
    /* Comms-lost: zero rate and clear valid, but keep last totalization so
     * EIP does not flash 0 before the next good Count sample restores it.
     * Drop delta state so the first sample after recovery does not spike. */
    taskENTER_CRITICAL();
    dceData.flowRate = 0.0f;
    dceData.valid    = false;
    taskEXIT_CRITICAL();
    s_havePrevTotal = false;
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

static void dce_task(
    void *pvParameters
)
{
    TickType_t tsWake;
    TickType_t tsPrevSample = 0;
    uint16_t   regs[DCE_REG_BLOCK_NREGS];

    (void) pvParameters;

    /* Wait until the poll task has initialised the master stack. */
    while (!dceMbReady) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    tsWake = xTaskGetTickCount();

    for (;;) {
        eMBMasterReqErrCode eErr;

        /* A failed transaction must not leave the previous reply looking
         * fresh to mbcb_rawGet(). */
        mbcb_rawInvalidate();
        dceDiag.polls++;

        /* >>> BENCH CHECK: confirm 0x01F8 appears on the wire, not 0x01F7 or
         * 0x01F9. Some FreeModbus forks apply a +/-1 conversion to the
         * request address. Reading one register off gives EFS and Count
         * instead of Count and CWratio, and the numbers will look almost
         * plausible. <<< */
        eErr = eMBMasterReqReadHoldingRegister(DCE_MODBUS_SLAVE_ADDR,
                                               DCE_REG_COUNT_ADDR,
                                               DCE_REG_BLOCK_NREGS,
                                               pdMS_TO_TICKS(DCE_REQ_WAIT_MS));
        dce_diagCountErr(eErr);

        if (MB_MRE_NO_ERR == eErr) {
            if (mbcb_rawGet(regs, DCE_REG_BLOCK_NREGS)) {
                uint32_t count   = dce_u32Get(&regs[0]);   /* usMBReg_Count       */
                uint32_t cwratio = dce_u32Get(&regs[2]);   /* usMBReg_ReadCWratio */

                dceDiag.good++;
                memcpy(dceDiag.lastRegs, regs, sizeof(dceDiag.lastRegs));

                if (0u != cwratio) {
                    /* Count is treated as an accumulating counter:
                     * totalization = Count/CWratio; flowRate is the
                     * per-minute delta of that total. If Count is instead a
                     * rate numerator, swap these on the bench. */
                    TickType_t now      = xTaskGetTickCount();
                    float      totalNow = (float) count / (float) cwratio;
                    float      flowRate = 0.0f;

                    if (s_havePrevTotal) {
                        uint32_t dtMs   = (uint32_t)(now - tsPrevSample);
                        float    dTotal = totalNow - s_prevTotal;

                        if (dTotal < 0.0f) {
                            dTotal = 0.0f; /* counter reset/wrap */
                        }
                        if ((dtMs > 0u) &&
                            (dtMs <= (DCE_STALE_TIMEOUT_MS * 2u))) {
                            flowRate = dTotal * (60000.0f / (float) dtMs);
                        }
                    }
                    s_prevTotal     = totalNow;
                    s_havePrevTotal = true;
                    tsPrevSample    = now;

                    dce_dataSet(flowRate, totalNow);
                } else {
                    /* cwratio == 0 means the DCE is uncalibrated. Hold the
                     * previous values and let the staleness timeout decide. */
                    dceDiag.cwratioZero++;
                }
            } else {
                dceDiag.rawMismatch++;
            }
        }

        /* Staleness check runs regardless of this poll's outcome. */
        {
            bool     valid;
            uint32_t tsLastGood;

            taskENTER_CRITICAL();
            valid      = dceData.valid;
            tsLastGood = dceData.tsLastGood;
            taskEXIT_CRITICAL();

            if (valid) {
                uint32_t age = (uint32_t) xTaskGetTickCount() - tsLastGood;

                if (age > pdMS_TO_TICKS(DCE_STALE_TIMEOUT_MS)) {
                    dce_dataInvalidate();
                    dceDiag.staleEvents++;
                }
            }
        }

        vTaskDelayUntil(&tsWake, pdMS_TO_TICKS(DCE_POLL_PERIOD_MS));
    }
}
#endif /* DCE_MODBUS_RUNTIME */

void dce_taskInit(void)
{
    memset(&dceData, 0, sizeof(dceData));
    memset(&dceDiag, 0, sizeof(dceDiag));

#if !DCE_MODBUS_RUNTIME
    /* Linked-but-idle: adc15/adc16 stay 0.0 via dce_dataGet(); no HW/tasks.
     * Matches 71ce78e behaviour (Modbus in image, EIP still connects). */
    dceDiag.mbState = DCE_MB_IDLE_LINKED;
#else
    dceMbReady = false;
    dceDiag.mbState = DCE_MB_NOT_STARTED;

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
