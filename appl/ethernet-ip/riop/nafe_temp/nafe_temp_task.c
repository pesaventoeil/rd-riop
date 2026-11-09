/*
 * NAFE13388 slow temperature task for the RIOP DCE gateway image (M7).
 *  - N7: FreeRTOS priority 0, same as GOAL Main (which carries the CM33<->M7 MCTC/RPC link for every
 *    EIP explicit request and never blocks). Time slicing shares the CPU; this task can never starve it.
 *  - N7: own bounded-wait LPSPI1 primitives (same register sequence as NXP nafe_hal.c, which spins
 *    forever on FCF/RDF). On timeout: CS high, FIFOs reset, op fails -> TEMP_ST_COMM, scan backs off.
 *  - N7: LPSPI1/NAFE bus is shared with NXP GPIO_ControlTask (prio 5, NAFE_gpioReadData every 50 ms) and
 *    AFE_task. Both skip the NAFE while gNafe_Hdl.sampleInProgress is set, so every transfer here
 *    claims that flag first (they cannot be mid-transfer while this lower-priority task runs).
 *  - N7: static stack/TCB (DTCM .bss), not the FreeRTOS heap GOAL also allocates from.
 *  - All waits are vTaskDelay (no busy DRDY polling); conversions are SPI-command triggered.
 *  - Publishes asm 174 from a snapshot; asm 175 (DEBUG_NAFE only) is queued and applied here, RAM only.
 *  - Read-only toward the NAFE trims/fuses; never touches DCE Modbus or the asm 171 mailbox.
 */
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "nafe13388.h"
#include "nafe_hal.h"
#include "afe_task.h"
#include "nafe_temp_task.h"

extern NAFE_hdl_t gNafe_Hdl;
extern AFE_ctx_t gAfe_ctx;

static temp_rt_t s_rt;
static uint8_t s_174[TEMP_EIP_IN_LEN];
static uint8_t s_175[TEMP_EIP_CFG_LEN];
static volatile uint8_t s_175_pending;
static nafe_ch_regs_t s_slot[16];
static volatile uint32_t s_spi_timeouts; /* lifetime count, also folded into skip_count */
static volatile uint32_t s_bus_busy;     /* transfers refused because NXP owned the NAFE */
static uint8_t s_fault;                  /* set by any failed primitive during the current scan */

/* ---- bounded LPSPI1 primitives (register sequence copied from NXP nafe_hal.c) ---- */
static int bus_take(void)
{
    int ok;
    taskENTER_CRITICAL();
    ok = !gNafe_Hdl.sampleInProgress && !gAfe_ctx.periodical_measurment_ongoing;
    if (ok) gNafe_Hdl.sampleInProgress = 1u;
    taskEXIT_CRITICAL();
    if (!ok) { s_bus_busy++; s_fault = 1u; }
    return ok ? 0 : -1;
}

static void bus_give(void) { gNafe_Hdl.sampleInProgress = 0u; }

static int spi_wait(NAFE_HAL_hdl_t *h, uint32_t srMask)
{
    for (uint32_t i = 0; i < NAFE_TEMP_SPI_SPIN_MAX; i++) {
        if (h->spi->SR & srMask) {
            if (srMask == LPSPI_SR_FCF_MASK) h->spi->SR = LPSPI_SR_FCF_MASK; /* W1C, as NXP does */
            return 0;
        }
    }
    NAFE_HAL_spiCsWrite(h, 1u);
    NAFE_HAL_spiClearInterruptFlags(h); /* reset FIFOs, clear FCF */
    s_spi_timeouts++;
    s_fault = 1u;
    return -1;
}

static int spi_cmd(uint16_t comb)
{
    NAFE_HAL_hdl_t *h = gNafe_Hdl.halHdl;
    int r;
    if (bus_take()) return -1;
    NAFE_HAL_spiClearInterruptFlags(h);
    h->spi->TCR = h->spiTcrBaseline | LPSPI_TCR_RXMSK_MASK | LPSPI_TCR_FRAMESZ(15u);
    NAFE_HAL_spiCsWrite(h, 0u);
    h->spi->TDR = comb;
    r = spi_wait(h, LPSPI_SR_FCF_MASK);
    NAFE_HAL_spiCsWrite(h, 1u);
    bus_give();
    return r;
}

static uint16_t reg_comb(uint16_t reg, uint16_t rd)
{
    return (uint16_t)(((gNafe_Hdl.devAddr & 1u) << 15) | ((rd & 1u) << 14) | ((reg & 0x1FFFu) << 1));
}

static int spi_wr(uint16_t reg, uint32_t data, uint32_t bits)
{
    NAFE_HAL_hdl_t *h = gNafe_Hdl.halHdl;
    int r;
    if (bus_take()) return -1;
    NAFE_HAL_spiClearInterruptFlags(h);
    h->spi->TCR = h->spiTcrBaseline | LPSPI_TCR_CONT(1u) | LPSPI_TCR_RXMSK_MASK | LPSPI_TCR_FRAMESZ(15u);
    NAFE_HAL_spiCsWrite(h, 0u);
    h->spi->TDR = reg_comb(reg, 0u);
    h->spi->TCR = h->spiTcrBaseline | LPSPI_TCR_CONTC(1u) | LPSPI_TCR_RXMSK_MASK | LPSPI_TCR_FRAMESZ(bits - 1u);
    h->spi->TDR = data;
    r = spi_wait(h, LPSPI_SR_FCF_MASK);
    NAFE_HAL_spiCsWrite(h, 1u);
    bus_give();
    return r;
}

static int spi_rd(uint16_t reg, uint32_t *data, uint32_t bits)
{
    NAFE_HAL_hdl_t *h = gNafe_Hdl.halHdl;
    int r;
    *data = 0u;
    if (bus_take()) return -1;
    NAFE_HAL_spiClearInterruptFlags(h);
    h->spi->TCR = h->spiTcrBaseline | LPSPI_TCR_CONT(1u) | LPSPI_TCR_RXMSK_MASK | LPSPI_TCR_FRAMESZ(15u);
    NAFE_HAL_spiCsWrite(h, 0u);
    h->spi->TDR = reg_comb(reg, 1u);
    h->spi->TCR = h->spiTcrBaseline | LPSPI_TCR_CONTC(1u) | LPSPI_TCR_TXMSK_MASK | LPSPI_TCR_FRAMESZ(bits - 1u);
    r = spi_wait(h, LPSPI_SR_RDF_MASK);
    NAFE_HAL_spiCsWrite(h, 1u);
    if (r == 0) *data = h->spi->RDR;
    bus_give();
    return r;
}

static int cmd(uint16_t instr) { return spi_cmd(nafe_cmd_instr((uint8_t)gNafe_Hdl.devAddr, instr)); }

static int op_program(void *ctx, uint8_t slot, const nafe_ch_regs_t *r)
{
    (void)ctx;
    int e = cmd((uint16_t)(TP_NAFE_CMD_CH0 + slot));
    if (!e) e = spi_wr(TP_NAFE_REG_CH_CONFIG0, r->cfg0, 16u);
    if (!e) e = spi_wr(TP_NAFE_REG_CH_CONFIG1, r->cfg1, 16u);
    if (!e) e = spi_wr(TP_NAFE_REG_CH_CONFIG2, r->cfg2, 16u);
    if (!e) e = spi_wr(TP_NAFE_REG_CH_CONFIG3, r->cfg3, 16u);
    s_slot[slot & 15u] = *r;
    return e ? -1 : 0;
}

static uint32_t conv_ms(const nafe_ch_regs_t *r)
{
    double sps = nafe_scs_rate_sps((uint8_t)((r->cfg1 >> 3) & 0x1Fu));
    double ms = sps > 0.0 ? 1000.0 / sps : 100.0;
    if ((r->cfg2 & (1u << 7)) || (r->cfg3 & (1u << 6))) ms *= 2.0; /* CH_CHOP / VIEX_CHOP: 2 conversions */
    return (uint32_t)ms + 2u;
}

static uint32_t now_ms(void) { return (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS); }

/* Mid-scan heartbeat: a long scan (1/min profile: ~27 s of oversampling) must not freeze the frame
 * counter, or the PC's 3.5 s stale check trips. Only bytes 4..7 (frame) of the published snapshot change;
 * the payload stays the last completed scan, so a frame never mixes two scans. */
static void heartbeat_mid_scan(void)
{
    if (temp_rt_heartbeat(&s_rt, now_ms())) {
        uint32_t f = s_rt.frame;
        taskENTER_CRITICAL();
        s_174[4] = (uint8_t)f; s_174[5] = (uint8_t)(f >> 8); s_174[6] = (uint8_t)(f >> 16); s_174[7] = (uint8_t)(f >> 24);
        taskEXIT_CRITICAL();
    }
}

static int op_convert(void *ctx, uint8_t slot, int32_t *code)
{
    (void)ctx;
    uint32_t raw = 0;
    if (cmd((uint16_t)(TP_NAFE_CMD_CH0 + slot))) return -1;
    if (cmd(TP_NAFE_CMD_SS)) return -1;        /* single channel, single reading */
    vTaskDelay(pdMS_TO_TICKS(conv_ms(&s_slot[slot & 15u])));
    heartbeat_mid_scan();
    if (spi_rd((uint16_t)(0x40u + slot), &raw, 24u)) return -1; /* CH_DATA<slot> */
    *code = nafe_sign_extend24(raw);
    return 0;
}

static int op_read16(void *ctx, uint16_t reg, uint16_t *val)
{
    (void)ctx;
    uint32_t v = 0;
    if (spi_rd(reg, &v, 16u)) return -1;
    *val = (uint16_t)v;
    return 0;
}

static const temp_afe_ops_t s_ops = {op_program, op_convert, op_read16, NULL, NULL};

static int take_nafe_for_scan(void)
{
    /* NXP AFE_Init leaves 16-bit output + SYNC-pin trigger; this path needs 24-bit + SPI-command trigger.
     * Re-asserted every scan (NXP's own AFE path rewrites it whenever it reconfigures). */
    uint32_t v = 0;
    if (cmd(TP_NAFE_CMD_ABORT)) return -1;
    if (spi_rd(TP_NAFE_REG_SYS_CONFIG0, &v, 16u)) return -1;
    v &= ~((1u << 14) | (1u << 5));
    return spi_wr(TP_NAFE_REG_SYS_CONFIG0, v, 16u);
}

static void probe_id(void)
{
    uint32_t pn2 = 0, pn1 = 0, pn0 = 0, crc = 0, st = 0, sn1 = 0, sn0 = 0;
    int e = 0;
    e |= spi_rd(TP_NAFE_REG_PN2, &pn2, 16u);
    e |= spi_rd(TP_NAFE_REG_PN1, &pn1, 16u);
    e |= spi_rd(TP_NAFE_REG_PN0, &pn0, 16u);
    e |= spi_rd(TP_NAFE_REG_CRC_TRIM_INT, &crc, 16u);
    e |= spi_rd(TP_NAFE_REG_SYS_STATUS0, &st, 16u);
    e |= spi_rd(TP_NAFE_REG_SERIAL1, &sn1, 24u);
    e |= spi_rd(TP_NAFE_REG_SERIAL0, &sn0, 24u);
    if (e) return; /* id_ok stays false; retried after the next good scan */
    nafe_decode_id((uint16_t)pn2, (uint16_t)pn1, (uint16_t)pn0, &s_rt.id);
    s_rt.crc_trim = (uint16_t)crc;
    s_rt.sys_status0 = (uint16_t)st;
    s_rt.serial1 = sn1 & 0xFFFFFFu;
    s_rt.serial0 = sn0 & 0xFFFFFFu;
    if (s_rt.id.id_ok) s_rt.hdr_flags |= TEMP_HF_NAFE_ID_OK;
}

static void publish(void)
{
    uint8_t tmp[TEMP_EIP_IN_LEN];
    temp_eip_pack174(&s_rt, tmp, sizeof tmp);
    taskENTER_CRITICAL();
    memcpy(s_174, tmp, sizeof tmp);
    taskEXIT_CRITICAL();
}

static void nafe_tempTask(void *arg)
{
    (void)arg;
    vTaskDelay(pdMS_TO_TICKS(NAFE_TEMP_START_DELAY_MS)); /* let AFE_Init / GOAL MCTC / Modbus come up first */
    probe_id();
    s_rt.hdr_flags |= TEMP_HF_TASK_RUNNING;
    publish();
    for (;;) {
        if (s_175_pending) {
            uint8_t c[TEMP_EIP_CFG_LEN];
            taskENTER_CRITICAL();
            memcpy(c, s_175, sizeof c);
            s_175_pending = 0;
            taskEXIT_CRITICAL();
            temp_eip_apply175(&s_rt, c, sizeof c);
            publish();
        }
        uint32_t now = now_ms();
        if ((int32_t)(now - s_rt.sched.next_due_ms) >= 0 || !s_rt.sched.started) {
            if (gAfe_ctx.periodical_measurment_ongoing || gNafe_Hdl.sampleInProgress) {
                s_rt.skip_count++;
                s_rt.hdr_flags |= TEMP_HF_SCAN_SKIPPED;
                s_rt.sched.started = true;
                s_rt.sched.next_due_ms = now + s_rt.live.period_ms;
                (void)temp_rt_heartbeat(&s_rt, now);
            } else {
                s_fault = 0u;
                s_rt.hdr_flags &= (uint8_t)~TEMP_HF_SCAN_SKIPPED;
                if (take_nafe_for_scan() == 0) {
                    TickType_t t0 = xTaskGetTickCount();
                    (void)temp_rt_poll(&s_rt, now);
                    s_rt.scan_time_us = (uint32_t)((xTaskGetTickCount() - t0) * portTICK_PERIOD_MS * 1000u);
                    s_rt.now_ms = now_ms();
                }
                if (s_fault) {
                    /* SPI timeout or bus lost: channels already report TEMP_ST_COMM; back off. */
                    s_rt.skip_count++;
                    s_rt.hdr_flags |= TEMP_HF_SCAN_SKIPPED;
                    s_rt.sched.started = true;
                    s_rt.sched.next_due_ms = now_ms() + NAFE_TEMP_FAULT_BACKOFF_MS;
                    (void)temp_rt_heartbeat(&s_rt, now_ms());
                } else if (!s_rt.id.id_ok) {
                    probe_id();
                }
            }
            publish();
        } else if (temp_rt_heartbeat(&s_rt, now)) {
            publish(); /* 1 Hz heartbeat between scans (1/min profile); payload unchanged */
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void nafe_tempGet174(uint8_t *buf, uint32_t len)
{
    uint32_t n = len < TEMP_EIP_IN_LEN ? len : TEMP_EIP_IN_LEN;
    taskENTER_CRITICAL();
    memcpy(buf, s_174, n);
    taskEXIT_CRITICAL();
}

void nafe_tempPost175(const uint8_t *buf, uint32_t len)
{
#if DEBUG_NAFE
    if (len < TEMP_EIP_CFG_LEN) return;
    taskENTER_CRITICAL();
    memcpy(s_175, buf, TEMP_EIP_CFG_LEN);
    s_175_pending = 1;
    taskEXIT_CRITICAL();
#else
    (void)buf; (void)len;
#endif
}

#if NAFE_TEMP_TASK_ENABLE
static StackType_t s_nafeStack[NAFE_TEMP_TASK_STACK_WORDS];
static StaticTask_t s_nafeTcb;
#endif

void nafe_tempTaskInit(void)
{
    temp_rt_init(&s_rt, NAFE_TEMP_DEFAULT_PROFILE, &s_ops);
    memset(s_174, 0, sizeof s_174);
    s_174[0] = TEMP_EIP_LAYOUT_VERSION;
    s_174[2] = TEMP_EIP_RELEASE_LEN;
#if NAFE_TEMP_TASK_ENABLE
    (void)xTaskCreateStatic(nafe_tempTask, "nafe_temp", NAFE_TEMP_TASK_STACK_WORDS, NULL,
                            NAFE_TEMP_TASK_PRIORITY, s_nafeStack, &s_nafeTcb);
#endif
}
