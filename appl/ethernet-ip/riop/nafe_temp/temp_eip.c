/* asm 174 packer and asm 175 NAFE-only RAM config handler. See temp_eip.h / NAFE_TEMP_EIP_LAYOUT.md. */
#include <math.h>
#include <string.h>
#include "dce_ng/temp_eip.h"

#if defined(__GNUC__)
#define TE_MAYBE_UNUSED __attribute__((unused))
#else
#define TE_MAYBE_UNUSED
#endif

const temp_profile_t *temp_builtin_profile(unsigned id)
{
    switch (id) {
    case TEMP_PROF_BENCH1_PT100: return &temp_profile_riop_b2_bench1;
    case TEMP_PROF_BENCH1_PT1000: return &temp_profile_riop_b2_bench1_pt1000;
    case TEMP_PROF_SLOW_1PM: return &temp_profile_slow_1pm;
    case TEMP_PROF_R347_CHECK: return &temp_profile_riop_r347_check;
    case TEMP_PROF_BENCH0_NOEXC: return &temp_profile_riop_b2_bench0;
    default: return NULL;
    }
}

void temp_rt_init(temp_rt_t *rt, unsigned profile_id, const temp_afe_ops_t *ops)
{
    const temp_profile_t *p = temp_builtin_profile(profile_id);
    if (!p) { profile_id = TEMP_PROF_BENCH1_PT100; p = temp_builtin_profile(profile_id); }
    memset(rt, 0, sizeof(*rt));
    rt->default_id = profile_id;
    rt->live = *p;
    temp_sched_init(&rt->sched, &rt->live, ops);
    r347_acc_init(&rt->r347);
}

bool temp_rt_heartbeat(temp_rt_t *rt, uint32_t now_ms)
{
    rt->now_ms = now_ms;
    if (!rt->hb_started || (uint32_t)(now_ms - rt->hb_ms) >= TEMP_HEARTBEAT_MS) {
        rt->hb_started = true;
        rt->hb_ms = now_ms;
        rt->frame++;
        return true;
    }
    return false;
}

uint32_t temp_rt_stale_ms(const temp_rt_t *rt) { return 3u * rt->live.period_ms + 1000u; }

bool temp_rt_poll(temp_rt_t *rt, uint32_t now_ms)
{
    temp_rt_heartbeat(rt, now_ms);
    if (!temp_sched_poll(&rt->sched, now_ms)) return false;
    rt->scan_count++;
    rt->scan_tick_ms = now_ms;
    for (unsigned i = 0; i < TEMP_MAX_CHANNELS; i++) {
        temp_status_t st = rt->sched.out[i].status;
        if (st == TEMP_ST_OFF) continue;
        rt->samples[i]++;
        if (st == TEMP_ST_OK) { rt->last_ok_ms[i] = now_ms; rt->ever_ok[i] = true; }
    }
    if (rt->sched.die_valid) { rt->die_ok_ms = now_ms; rt->die_ever_ok = true; }
    if (rt->live.lvsig_r_top > 0.0f) rt->r347_tried = true;
    if (rt->sched.lvsig_status == TEMP_ST_OK) {
        r347_acc_add(&rt->r347, rt->sched.die_c, rt->sched.lvsig_ohms);
        rt->r347_ok_ms = now_ms;
        rt->r347_ever_ok = true;
    }
    return true;
}

/* little-endian writers */
static void w8(uint8_t *b, size_t o, uint8_t v) { b[o] = v; }
static void w16(uint8_t *b, size_t o, uint16_t v) { b[o] = (uint8_t)v; b[o + 1] = (uint8_t)(v >> 8); }
static void w32(uint8_t *b, size_t o, uint32_t v) { for (int i = 0; i < 4; i++) b[o + i] = (uint8_t)(v >> (8 * i)); }
static void wf(uint8_t *b, size_t o, double d)
{
    float f = (float)d;
    uint32_t u;
    if (isnan(d) || isinf(d)) f = 0.0f; /* never publish NaN: validity is the status byte */
    memcpy(&u, &f, 4);
    w32(b, o, u);
}
TE_MAYBE_UNUSED static uint16_t r16(const uint8_t *b, size_t o) { return (uint16_t)(b[o] | (b[o + 1] << 8)); }
TE_MAYBE_UNUSED static uint32_t r32(const uint8_t *b, size_t o) { return (uint32_t)b[o] | ((uint32_t)b[o + 1] << 8) | ((uint32_t)b[o + 2] << 16) | ((uint32_t)b[o + 3] << 24); }
TE_MAYBE_UNUSED static float rf(const uint8_t *b, size_t o) { uint32_t u = r32(b, o); float f; memcpy(&f, &u, 4); return f; }

static uint8_t status_byte(const temp_rt_t *rt, unsigned i)
{
    const temp_reading_t *o = &rt->sched.out[i];
    uint8_t b = 0;
    switch (o->status) {
    case TEMP_ST_OK: b |= TEMP_ST_BIT_VALID; break;
    case TEMP_ST_OPEN: b |= TEMP_ST_BIT_OPEN_SHORT; break;
    case TEMP_ST_RANGE: b |= TEMP_ST_BIT_RANGE; break;
    case TEMP_ST_COMM: b |= TEMP_ST_BIT_COMM; break;
    case TEMP_ST_OFF: return TEMP_ST_BIT_OFF;
    }
    /* stale (set here, by the RIOP): no valid result for > 3 x period + 1 s, or never */
    if (!rt->ever_ok[i] || (uint32_t)(rt->now_ms - rt->last_ok_ms[i]) > temp_rt_stale_ms(rt)) {
        b |= TEMP_ST_BIT_STALE;
        b &= (uint8_t)~TEMP_ST_BIT_VALID;
    }
    if (rt->ch_override & (1u << i)) b |= TEMP_ST_BIT_OVERRIDE;
    return b;
}

size_t temp_eip_pack174(const temp_rt_t *rt, uint8_t *b, size_t len)
{
    if (len < TEMP_EIP_IN_LEN) return 0;
    memset(b, 0, TEMP_EIP_IN_LEN);
    /* ---- header, 32 bytes ---- */
    w16(b, 0, TEMP_EIP_LAYOUT_VERSION);
    w16(b, 2, TEMP_EIP_RELEASE_LEN);
    w32(b, 4, rt->frame);
    w32(b, 8, rt->scan_tick_ms);
    wf(b, 12, rt->sched.die_c);
    bool rv = rt->sched.lvsig_status == TEMP_ST_OK;
    wf(b, 16, rv ? rt->sched.lvsig_ohms : 0.0);
    r347_class_t cls = r347_classify(&rt->r347, 3.0);
    uint8_t rfl = 0;
    /* PASS/FAIL judge the divider reading only (plausibility window); the RTD-vs-fixed class is
     * separate (byte 21) and may still be NEED_SPAN while PASS is set. Neither bit before a first read. */
    bool inwin = rv && rt->sched.lvsig_ohms > TEMP_R347_WIN_LO && rt->sched.lvsig_ohms < TEMP_R347_WIN_HI;
    if (rv) rfl |= TEMP_R347_VALID;
    if (inwin) rfl |= TEMP_R347_PASS;
    else if (rt->r347_tried) rfl |= TEMP_R347_FAIL;
    if (rt->r347_tried && (!rt->r347_ever_ok || (uint32_t)(rt->now_ms - rt->r347_ok_ms) > temp_rt_stale_ms(rt))) rfl |= TEMP_R347_STALE;
    w8(b, 20, rfl);
    w8(b, 21, (uint8_t)cls);
    uint8_t hf = rt->hdr_flags & (uint8_t)~(TEMP_HF_DEBUG_PRESENT | TEMP_HF_OVERRIDES_ACTIVE | TEMP_HF_DIE_VALID);
    if (rt->sched.die_valid && rt->die_ever_ok && (uint32_t)(rt->now_ms - rt->die_ok_ms) <= temp_rt_stale_ms(rt)) hf |= TEMP_HF_DIE_VALID;
    if (DEBUG_NAFE) hf |= TEMP_HF_DEBUG_PRESENT;
    if (rt->ch_override || rt->board_override) hf |= TEMP_HF_OVERRIDES_ACTIVE;
    w8(b, 22, hf);
    w8(b, 23, TEMP_MAX_CHANNELS);
    w16(b, 24, (uint16_t)TEMP_EIP_DBG_LEN * (DEBUG_NAFE ? 1u : 0u));
    w8(b, 26, DEBUG_NAFE ? 1u : 0u);
    w8(b, 27, (uint8_t)rt->default_id);
    w32(b, 28, rt->live.period_ms);
    /* ---- channels, 16 bytes each from offset 32 ---- */
    for (unsigned i = 0; i < TEMP_MAX_CHANNELS; i++) {
        size_t o = TEMP_EIP_HDR_LEN + i * TEMP_EIP_CH_LEN;
        const temp_reading_t *r = &rt->sched.out[i];
        uint8_t st = status_byte(rt, i);
        bool keep = (st & (TEMP_ST_BIT_VALID | TEMP_ST_BIT_STALE)) != 0 && r->status == TEMP_ST_OK;
        wf(b, o + 0, keep ? r->value : 0.0);
        wf(b, o + 4, keep ? r->raw : 0.0);
        w8(b, o + 8, st);
        w8(b, o + 9, (uint8_t)rt->live.ch[i].sensor);
        w16(b, o + 10, rt->samples[i]);
        wf(b, o + 12, keep ? r->noise_rms : 0.0);
    }
#if DEBUG_NAFE
    /* ---- debug block, after the release subset (offset 96) ---- */
    size_t d = TEMP_EIP_RELEASE_LEN;
    w16(b, d + 0, rt->ack_seq);
    w8(b, d + 2, (uint8_t)rt->ack_code);
    w8(b, d + 3, rt->ack_op);
    w32(b, d + 4, rt->reject_mask);
    w8(b, d + 8, rt->ack_ch);
    w8(b, d + 9, rt->ch_override);
    w16(b, d + 10, rt->applied_count);
    w16(b, d + 12, rt->id.pn2);
    w16(b, d + 14, rt->id.pn1);
    w16(b, d + 16, rt->id.pn0);
    w16(b, d + 18, rt->skip_count);
    w32(b, d + 20, rt->scan_time_us);
    w32(b, d + 24, rt->serial1);
    w32(b, d + 28, rt->serial0);
    w16(b, d + 32, rt->crc_trim);
    w16(b, d + 34, rt->sys_status0);
    w32(b, d + 36, rt->scan_count);
    for (unsigned i = 0; i < TEMP_MAX_CHANNELS; i++) {
        size_t o = d + TEMP_EIP_DBG_HDR_LEN + i * TEMP_EIP_DBG_CH_LEN;
        const temp_ch_cfg_t *c = &rt->live.ch[i];
        w32(b, o + 0, (uint32_t)rt->sched.dbg[i].last_code);
        w32(b, o + 4, (uint32_t)rt->sched.dbg[i].mean_code);
        w16(b, o + 8, rt->sched.dbg[i].regs.cfg0);
        w16(b, o + 10, rt->sched.dbg[i].regs.cfg1);
        w16(b, o + 12, rt->sched.dbg[i].regs.cfg2);
        w16(b, o + 14, rt->sched.dbg[i].regs.cfg3);
        w8(b, o + 16, (uint8_t)((rt->sched.dbg[i].regs.cfg0 >> 5) & 7u)); /* gain as applied */
        w8(b, o + 17, (uint8_t)((rt->sched.dbg[i].regs.cfg3 >> 10) & 0xFu)); /* VIEX_MAG as applied */
        w8(b, o + 18, (uint8_t)((rt->sched.dbg[i].regs.cfg3 >> 3) & 7u) | (uint8_t)((rt->sched.dbg[i].regs.cfg3 & 7u) << 4));
        w8(b, o + 19, (uint8_t)(c->wiring));
        w8(b, o + 20, (uint8_t)((rt->sched.dbg[i].regs.cfg1 >> 3) & 0x1Fu)); /* data rate as applied */
        w8(b, o + 21, (uint8_t)(rt->sched.dbg[i].regs.cfg1 & 7u));          /* sinc as applied */
        w8(b, o + 22, (uint8_t)((c->viex_chop ? 1u : 0u) | (c->ch_chop ? 2u : 0u) | (c->open_detect ? 4u : 0u)));
        w8(b, o + 23, rt->live.oversample);
    }
#endif
    return TEMP_EIP_IN_LEN;
}

uint16_t temp_eip_checksum(const uint8_t *b, size_t n)
{
    uint16_t s = 0;
    for (size_t i = 0; i < n; i++) s = (uint16_t)(s + b[i]);
    return s;
}

TE_MAYBE_UNUSED static void ack(temp_rt_t *rt, uint16_t seq, uint8_t op, uint8_t ch, uint16_t code, uint32_t rej)
{
    rt->ack_seq = seq;
    rt->ack_op = op;
    rt->ack_ch = ch;
    rt->ack_code = code;
    rt->reject_mask = rej;
}

TE_MAYBE_UNUSED static bool same_cfg(const temp_ch_cfg_t *a, const temp_ch_cfg_t *b) { return memcmp(a, b, sizeof(*a)) == 0; }

int temp_eip_apply175(temp_rt_t *rt, const uint8_t *b, size_t len)
{
#if !DEBUG_NAFE
    (void)b; (void)len;
    rt->ack_code = TEMP_ACK_DISABLED;
    return TEMP_ACK_DISABLED;
#else
    if (len < TEMP_EIP_CFG_LEN) return TEMP_ACK_NONE;
    uint16_t seq = r16(b, 0);
    uint8_t op = b[2], ch = b[3];
    uint16_t mask = r16(b, 4);
    if (seq == 0 || op == TEMP_OP_NONE) return TEMP_ACK_NONE;      /* all-zero = no command */
    if (seq == rt->last_seq) return rt->ack_code;                   /* already handled: idempotent */
    rt->last_seq = seq;
    if (temp_eip_checksum(b, 28) != r16(b, 28)) { ack(rt, seq, op, ch, TEMP_ACK_REJECTED, TEMP_REJ_CHECKSUM); return TEMP_ACK_REJECTED; }

    const temp_profile_t *def = temp_builtin_profile(rt->default_id);
    temp_profile_t np = rt->live;
    uint32_t rej = 0;

    switch (op) {
    case TEMP_OP_RESTORE_DEFAULTS:
        np = *def;
        break;
    case TEMP_OP_SELECT_PROFILE: {
        const temp_profile_t *p = temp_builtin_profile(b[6]);
        if (!p) { rej |= TEMP_REJ_PROFILE; break; }
        rt->default_id = b[6];
        def = p;
        np = *p;
        break;
    }
    case TEMP_OP_CLEAR_CH:
        if (ch >= TEMP_MAX_CHANNELS) { rej |= TEMP_REJ_CHANNEL; break; }
        np.ch[ch] = def->ch[ch];
        break;
    case TEMP_OP_SET_CH: {
        if (ch >= TEMP_MAX_CHANNELS) { rej |= TEMP_REJ_CHANNEL; break; }
        temp_ch_cfg_t c = np.ch[ch];
        if (mask & TEMP_F_SENSOR) { if (b[6] > TEMP_SENSOR_VOLTAGE) rej |= TEMP_F_SENSOR; else c.sensor = (temp_sensor_t)b[6]; }
        if (mask & TEMP_F_WIRING) { if (b[7] < 2 || b[7] > 4) rej |= TEMP_F_WIRING; else c.wiring = (temp_wiring_t)b[7]; }
        if (mask & TEMP_F_SENSE_P) { if (b[8] > NAFE_HV_VEXC) rej |= TEMP_F_SENSE_P; else c.sense_p = b[8]; }
        if (mask & TEMP_F_SENSE_N) { if (b[9] > NAFE_HV_VEXC) rej |= TEMP_F_SENSE_N; else c.sense_n = b[9]; }
        if (mask & TEMP_F_EXC_SIDE) { if (b[10] > NAFE_EXC_AIN) rej |= TEMP_F_EXC_SIDE; else c.exc_side = (nafe_exc_side_t)b[10]; }
        if (mask & TEMP_F_EXC_PIN) { if (b[11] > 4) rej |= TEMP_F_EXC_PIN; else c.exc_pin = b[11]; }
        if (mask & TEMP_F_EXC_CUR) { if (b[12] != 0 && (b[12] < NAFE_IEX_125UA || b[12] > NAFE_IEX_2MA)) rej |= TEMP_F_EXC_CUR; else c.exc_current = (nafe_iex_t)b[12]; }
        if (mask & TEMP_F_GAIN) { if (b[13] > NAFE_G16) rej |= TEMP_F_GAIN; else c.gain = (nafe_gain_t)b[13]; }
        if (mask & TEMP_F_RATE) { if (b[14] > 28) rej |= TEMP_F_RATE; else c.data_rate_code = b[14]; }
        if (mask & TEMP_F_SINC) { if (b[15] > 4) rej |= TEMP_F_SINC; else c.sinc = b[15]; }
        if (mask & TEMP_F_FLAGS) { if (b[16] & 0xF8u) rej |= TEMP_F_FLAGS; else { c.viex_chop = b[16] & 1u; c.ch_chop = (b[16] >> 1) & 1u; c.open_detect = (b[16] >> 2) & 1u; } }
        if (mask & TEMP_F_LEAD) { if (b[17] > NAFE_HV_VEXC || b[18] > NAFE_HV_VEXC) rej |= TEMP_F_LEAD; else { c.lead_p = b[17]; c.lead_n = b[18]; } }
        if (mask & 0xF000u) rej |= (mask & 0xF000u); /* board fields not valid in SET_CH */
        if (!rej && c.sensor != TEMP_SENSOR_OFF) {
            nafe_ch_regs_t tr;
            if (temp_cfg_to_regs(&c, false, &tr)) rej |= TEMP_REJ_COMBINATION;
        }
        if (!rej) np.ch[ch] = c;
        break;
    }
    case TEMP_OP_SET_BOARD: {
        if (mask & 0x0FFFu) rej |= (mask & 0x0FFFu);
        uint8_t ov = b[17] ? b[17] : np.oversample;
        if (mask & TEMP_F_OVERSAMPLE) { if (b[17] < 1 || b[17] > TEMP_MAX_OVERSAMPLE) rej |= TEMP_F_OVERSAMPLE; else np.oversample = b[17]; }
        if (mask & TEMP_F_TRIM) { if (2u * b[18] >= ov) rej |= TEMP_F_TRIM; else np.trim = b[18]; }
        if (mask & TEMP_F_PERIOD) { uint32_t pm = r32(b, 20); if (pm < 1000u || pm > 600000u) rej |= TEMP_F_PERIOD; else np.period_ms = pm; }
        if (mask & TEMP_F_IIR) { float a = rf(b, 24); if (!(a >= 0.0f && a <= 1.0f)) rej |= TEMP_F_IIR; else np.iir_alpha = a; }
        break;
    }
    default:
        rej |= TEMP_REJ_OP;
    }
    if (!rej && temp_scan_time_ms(&np) > 0.9 * np.period_ms) rej |= TEMP_REJ_BUDGET;
    if (rej) { ack(rt, seq, op, ch, TEMP_ACK_REJECTED, rej); return TEMP_ACK_REJECTED; }

    /* apply in RAM only; scheduler picks it up next scan (same struct, single writer = temp task) */
    rt->live = np;
    rt->ch_override = 0;
    for (unsigned i = 0; i < TEMP_MAX_CHANNELS; i++)
        if (!same_cfg(&np.ch[i], &def->ch[i])) rt->ch_override |= (uint8_t)(1u << i);
    rt->board_override = np.oversample != def->oversample || np.trim != def->trim || np.period_ms != def->period_ms ||
                         np.iir_alpha != def->iir_alpha;
    for (unsigned i = 0; i < TEMP_MAX_CHANNELS; i++) { rt->samples[i] = 0; rt->sched.out[i].scans = 0; }
    rt->applied_count++;
    ack(rt, seq, op, ch, TEMP_ACK_OK, 0);
    return TEMP_ACK_OK;
#endif
}
