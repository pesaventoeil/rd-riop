/*
 * NAFE13388 slow temperature task for the RIOP DCE gateway image (M7).
 *  - Own FreeRTOS task at priority 2: below dce_task (4), mbpoll (5) and the GOAL EIP tasks, so the
 *    Modbus master and EIP timing are not preempted by this task. All waits are vTaskDelay (no busy
 *    DRDY polling), conversions are started by SPI command and read from CH_DATA by register access.
 *  - Uses NXP's own driver/HAL handle (gNafe_Hdl, LPSPI1). Skips a scan if NXP AFE_task is sampling.
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

static void cmd(uint16_t instr) { NAFE_HAL_sendCmdBlock(gNafe_Hdl.halHdl, nafe_cmd_instr((uint8_t)gNafe_Hdl.devAddr, instr), 1u); }

static int op_program(void *ctx, uint8_t slot, const nafe_ch_regs_t *r)
{
    (void)ctx;
    cmd((uint16_t)(TP_NAFE_CMD_CH0 + slot));
    NAFE_writeRegBlock(&gNafe_Hdl, TP_NAFE_REG_CH_CONFIG0, r->cfg0, 16u);
    NAFE_writeRegBlock(&gNafe_Hdl, TP_NAFE_REG_CH_CONFIG1, r->cfg1, 16u);
    NAFE_writeRegBlock(&gNafe_Hdl, TP_NAFE_REG_CH_CONFIG2, r->cfg2, 16u);
    NAFE_writeRegBlock(&gNafe_Hdl, TP_NAFE_REG_CH_CONFIG3, r->cfg3, 16u);
    s_slot[slot & 15u] = *r;
    return 0;
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
    cmd((uint16_t)(TP_NAFE_CMD_CH0 + slot));
    cmd(TP_NAFE_CMD_SS);                       /* single channel, single reading */
    vTaskDelay(pdMS_TO_TICKS(conv_ms(&s_slot[slot & 15u])));
    heartbeat_mid_scan();
    NAFE_readRegBlock(&gNafe_Hdl, (uint16_t)(0x40u + slot), &raw, 24u); /* CH_DATA<slot> */
    *code = nafe_sign_extend24(raw);
    return 0;
}

static int op_read16(void *ctx, uint16_t reg, uint16_t *val)
{
    (void)ctx;
    uint32_t v = 0;
    NAFE_readRegBlock(&gNafe_Hdl, reg, &v, 16u);
    *val = (uint16_t)v;
    return 0;
}

static const temp_afe_ops_t s_ops = {op_program, op_convert, op_read16, NULL, NULL};

static void take_nafe_for_scan(void)
{
    /* NXP AFE_Init leaves 16-bit output + SYNC-pin trigger; this path needs 24-bit + SPI-command trigger.
     * Re-asserted every scan (NXP's own AFE path rewrites it whenever it reconfigures). */
    uint32_t v = 0;
    cmd(TP_NAFE_CMD_ABORT);
    NAFE_readRegBlock(&gNafe_Hdl, TP_NAFE_REG_SYS_CONFIG0, &v, 16u);
    v &= ~((1u << 14) | (1u << 5));
    NAFE_writeRegBlock(&gNafe_Hdl, TP_NAFE_REG_SYS_CONFIG0, v, 16u);
}

static void probe_id(void)
{
    uint32_t pn2 = 0, pn1 = 0, pn0 = 0, crc = 0, st = 0, sn1 = 0, sn0 = 0;
    NAFE_readRegBlock(&gNafe_Hdl, TP_NAFE_REG_PN2, &pn2, 16u);
    NAFE_readRegBlock(&gNafe_Hdl, TP_NAFE_REG_PN1, &pn1, 16u);
    NAFE_readRegBlock(&gNafe_Hdl, TP_NAFE_REG_PN0, &pn0, 16u);
    NAFE_readRegBlock(&gNafe_Hdl, TP_NAFE_REG_CRC_TRIM_INT, &crc, 16u);
    NAFE_readRegBlock(&gNafe_Hdl, TP_NAFE_REG_SYS_STATUS0, &st, 16u);
    NAFE_readRegBlock(&gNafe_Hdl, TP_NAFE_REG_SERIAL1, &sn1, 24u);
    NAFE_readRegBlock(&gNafe_Hdl, TP_NAFE_REG_SERIAL0, &sn0, 24u);
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
    vTaskDelay(pdMS_TO_TICKS(3000)); /* let AFE_Init / GOAL / Modbus come up first */
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
                s_rt.sched.next_due_ms = now + s_rt.live.period_ms;
                (void)temp_rt_heartbeat(&s_rt, now);
            } else {
                s_rt.hdr_flags &= (uint8_t)~TEMP_HF_SCAN_SKIPPED;
                take_nafe_for_scan();
                TickType_t t0 = xTaskGetTickCount();
                (void)temp_rt_poll(&s_rt, now);
                s_rt.scan_time_us = (uint32_t)((xTaskGetTickCount() - t0) * portTICK_PERIOD_MS * 1000u);
                s_rt.now_ms = now_ms();
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

void nafe_tempTaskInit(void)
{
    temp_rt_init(&s_rt, NAFE_TEMP_DEFAULT_PROFILE, &s_ops);
    memset(s_174, 0, sizeof s_174);
    s_174[0] = TEMP_EIP_LAYOUT_VERSION;
    s_174[2] = TEMP_EIP_RELEASE_LEN;
    (void)xTaskCreate(nafe_tempTask, "nafe_temp", NAFE_TEMP_TASK_STACK_WORDS, NULL, NAFE_TEMP_TASK_PRIORITY, NULL);
}
