/* Slow temperature scheduler: one scan per period (1 s default, 60 s option),
 * N conversions per channel, trimmed mean, optional IIR across scans.
 * Runs in its own low-priority task; never touches the load-cell ADCs. */
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "dce_ng/temp_path.h"

#define CODE_CLIP 8388000 /* within ~600 LSB of +/-2^23 -> over/under range */

static void sort_d(double *x, unsigned n)
{
    for (unsigned i = 1; i < n; i++) {
        double v = x[i];
        unsigned j = i;
        while (j > 0 && x[j - 1] > v) { x[j] = x[j - 1]; j--; }
        x[j] = v;
    }
}

double temp_trimmed_mean(const double *in, unsigned n, unsigned trim, double *sd)
{
    double x[TEMP_MAX_OVERSAMPLE];
    if (n == 0 || n > TEMP_MAX_OVERSAMPLE) return NAN;
    memcpy(x, in, n * sizeof(double));
    if (2 * trim >= n) trim = 0;
    sort_d(x, n);
    double m = 0.0;
    unsigned k = n - 2 * trim;
    for (unsigned i = trim; i < n - trim; i++) m += x[i];
    m /= k;
    if (sd) {
        double v = 0.0;
        for (unsigned i = 0; i < n; i++) v += (in[i] - m) * (in[i] - m);
        *sd = n > 1 ? sqrt(v / (n - 1)) : 0.0;
    }
    return m;
}

void temp_sched_init(temp_sched_t *s, const temp_profile_t *p, const temp_afe_ops_t *ops)
{
    memset(s, 0, sizeof(*s));
    s->prof = p;
    s->ops = ops;
    s->cjc_c = p->cjc_fixed_c;
    s->exc_owner = -1;
    s->lvsig_ohms = NAN;
    s->lvsig_status = TEMP_ST_OFF;
    for (unsigned i = 0; i < TEMP_MAX_CHANNELS; i++) s->out[i].status = TEMP_ST_OFF;
}

static bool is_rtd(temp_sensor_t t) { return t == TEMP_SENSOR_PT100 || t == TEMP_SENSOR_PT1000; }

static bool regs_excite(const nafe_ch_regs_t *r)
{
    /* current/voltage source with magnitude > 0, VEXC_EN, or ODC */
    return (((r->cfg3 >> 10) & 0xFu) != 0u && (r->cfg3 & 0x3Fu) != 0u) || (r->cfg3 & (1u << 9)) ||
           ((r->cfg3 & (1u << 8)) && (r->cfg3 & 0x3Fu));
}

/* Average `n` conversions of a programmed slot, in volts. Owns the single excitation source for the
 * duration: routes the force path, converts, then turns the excitation off again (cfg3 = 0) and
 * disconnects the route, so no two channels are ever excited together. */
static temp_status_t sample_routed(temp_sched_t *s, uint8_t slot, const nafe_ch_regs_t *r, nafe_gain_t g,
                                   unsigned n, uint8_t route, double *mean_v, double *sd_v)
{
    double v[TEMP_MAX_OVERSAMPLE];
    int64_t csum = 0;
    temp_status_t st = TEMP_ST_OK;
    bool exc = regs_excite(r);
    if (exc && s->exc_owner >= 0) return TEMP_ST_COMM; /* invariant guard: should be unreachable */
    if (exc && route && s->ops->select_force && s->ops->select_force(s->ops->ctx, route)) return TEMP_ST_COMM;
    if (s->ops->program(s->ops->ctx, slot, r)) { st = TEMP_ST_COMM; goto release; }
    if (exc) s->exc_owner = slot;
    for (unsigned i = 0; i < n; i++) {
        int32_t code;
        if (s->ops->convert(s->ops->ctx, slot, &code)) { st = TEMP_ST_COMM; break; }
        s->last_code = code;
        if (code > CODE_CLIP || code < -CODE_CLIP) { st = TEMP_ST_RANGE; break; }
        v[i] = nafe_code_to_volts(code, g);
        csum += code;
    }
    if (n) s->mean_code = (int32_t)(csum / (int64_t)n);
    s->last_regs = *r;
    if (st == TEMP_ST_OK) *mean_v = temp_trimmed_mean(v, n, s->prof->trim, sd_v);
release:
    if (exc) {
        nafe_ch_regs_t off = *r;
        off.cfg3 = 0;
        s->ops->program(s->ops->ctx, slot, &off);
        s->exc_owner = -1;
        if (route && s->ops->select_force) s->ops->select_force(s->ops->ctx, 0);
    }
    return st;
}

static temp_status_t sample(temp_sched_t *s, uint8_t slot, const nafe_ch_regs_t *r, nafe_gain_t g,
                            unsigned n, double *mean_v, double *sd_v)
{
    return sample_routed(s, slot, r, g, n, 0, mean_v, sd_v);
}

double lvsig_divider_ohms(double v, double vref, double r_top)
{
    v = fabs(v); /* GPIO0/GPIO1 polarity across R347 not confirmed */
    if (v >= vref) return NAN;
    return v * r_top / (vref - v);
}

static void update_cjc(temp_sched_t *s)
{
    const temp_profile_t *p = s->prof;
    uint16_t raw;
    if (s->ops->read_reg16 && !s->ops->read_reg16(s->ops->ctx, TP_NAFE_REG_DIE_TEMP, &raw)) {
        s->die_c = nafe_die_temp_c(raw);
        if (p->cjc_src == TEMP_CJC_DIE) s->cjc_c = s->die_c;
    }
    /* R347 / LVSIG element is read every scan when the profile describes the divider (board-wide value
     * for asm 174); it is used as CJC only when cjc_src == TEMP_CJC_LVSIG_RTD. */
    if (p->lvsig_r_top > 0.0f) {
        /* LVSIG GPIO0-GPIO1 (HV_SEL = 0, LVSIG_IN = 1), G16 -> +/-312 mV; ~46 mV expected for 110 R.
         * RIOP B2 R346/R347/R349 topology is UNCONFIRMED. */
        nafe_ch_regs_t r = {(uint16_t)(((unsigned)NAFE_G16 << 5) | (NAFE_LV_GPIO0_GPIO1 << 1) | 1u),
                            (uint16_t)(((unsigned)NAFE_G16 << 12) | (20u << 3) | 1u), (uint16_t)((11u << 10) | (1u << 7)), 0};
        double v, sd;
        s->lvsig_status = sample(s, 15, &r, NAFE_G16, p->oversample, &v, &sd);
        if (s->lvsig_status == TEMP_ST_OK) {
            s->lvsig_ohms = lvsig_divider_ohms(v, p->lvsig_ref_v, p->lvsig_r_top);
            if (isnan(s->lvsig_ohms)) s->lvsig_status = TEMP_ST_RANGE;
            else if (p->cjc_src == TEMP_CJC_LVSIG_RTD) s->cjc_c = rtd_cvd_temp_c(s->lvsig_ohms, 100.0);
        }
    }
}

int temp_r347_log_line(const temp_sched_t *s, uint32_t now_ms, char *buf, unsigned len)
{
    return snprintf(buf, len, "%lu,%.4f,%.4f,%d", (unsigned long)now_ms, s->die_c, s->lvsig_ohms,
                    (int)s->lvsig_status);
}

void r347_acc_init(r347_acc_t *a)
{
    memset(a, 0, sizeof(*a));
    a->xmin = 1e9;
    a->xmax = -1e9;
}

void r347_acc_add(r347_acc_t *a, double x, double y)
{
    if (isnan(x) || isnan(y)) return;
    a->n++;
    a->sx += x; a->sy += y; a->sxx += x * x; a->sxy += x * y;
    if (x < a->xmin) a->xmin = x;
    if (x > a->xmax) a->xmax = x;
}

double r347_slope(const r347_acc_t *a)
{
    double d = a->n * a->sxx - a->sx * a->sx;
    return (a->n < 2 || d <= 0.0) ? NAN : (a->n * a->sxy - a->sx * a->sy) / d;
}

double r347_mean_ohms(const r347_acc_t *a) { return a->n ? a->sy / a->n : NAN; }

bool r347_room_plausible_pt100(double ohms) { return ohms >= 107.0 && ohms <= 112.0; }

r347_class_t r347_classify(const r347_acc_t *a, double min_span_c)
{
    if (a->n < 2 || (a->xmax - a->xmin) < min_span_c) return R347_NEED_SPAN;
    double k = r347_slope(a), m = r347_mean_ohms(a);
    /* PT100 alpha 0.00385 -> 0.385 ohm/C; accept 0.30..0.47 (die temp != R347 temp exactly) */
    if (k >= 0.30 && k <= 0.47 && m > 80.0 && m < 140.0) return R347_RTD_PT100;
    /* fixed resistor: < 0.02 ohm/C (200 ppm/C) and near 100 R */
    if (fabs(k) < 0.02 && m > 95.0 && m < 105.0) return R347_FIXED_100R;
    return R347_UNKNOWN;
}

const char *r347_class_name(r347_class_t c)
{
    switch (c) {
    case R347_RTD_PT100: return "RTD_PT100";
    case R347_FIXED_100R: return "FIXED_100R_GAIN_CHECK";
    case R347_NEED_SPAN: return "NEED_TEMP_SPAN";
    default: return "UNKNOWN";
    }
}

static void finish(temp_sched_t *s, unsigned i, temp_status_t st, double raw, double val, double sd)
{
    temp_reading_t *o = &s->out[i];
    float a = s->prof->iir_alpha;
    o->status = st;
    o->noise_rms = sd;
    if (st != TEMP_ST_OK) return;
    if (a > 0.0f && a < 1.0f && o->scans > 0 && o->status == TEMP_ST_OK) {
        o->raw = a * raw + (1.0 - a) * o->raw;
        o->value = a * val + (1.0 - a) * o->value;
    } else {
        o->raw = raw;
        o->value = val;
    }
    o->scans++;
}

static void scan_channel(temp_sched_t *s, unsigned i)
{
    const temp_ch_cfg_t *c = &s->prof->ch[i];
    nafe_ch_regs_t r;
    double v, sd;
    if (c->sensor == TEMP_SENSOR_OFF) { s->out[i].status = TEMP_ST_OFF; return; }
    if (temp_cfg_to_regs(c, false, &r)) { s->out[i].status = TEMP_ST_COMM; return; }
    temp_status_t st = sample_routed(s, (uint8_t)i, &r, c->gain, s->prof->oversample, c->exc_route, &v, &sd);
    s->dbg[i].last_code = s->last_code;
    s->dbg[i].mean_code = s->mean_code;
    s->dbg[i].regs = r;
    if (st != TEMP_ST_OK) { finish(s, i, st, 0, 0, 0); return; }

    float ug = c->user_gain != 0.0f ? c->user_gain : 1.0f;
    if (is_rtd(c->sensor)) {
        double amps = nafe_iex_amps(c->exc_current);
        double ohms = v / amps;
        double lead = 0.0;
        if (c->wiring == TEMP_WIRE_3) {
            temp_ch_cfg_t lc = *c;
            nafe_ch_regs_t lr;
            double lv, lsd;
            lc.sense_p = c->lead_p;
            lc.sense_n = c->lead_n;
            lc.wiring = TEMP_WIRE_2; /* lead pair may share the force pin */
            if (temp_cfg_to_regs(&lc, false, &lr) ||
                sample_routed(s, (uint8_t)(i + 4), &lr, c->gain, s->prof->oversample, c->exc_route, &lv, &lsd) != TEMP_ST_OK) {
                finish(s, i, TEMP_ST_COMM, 0, 0, 0);
                return;
            }
            lead = lv / amps;
        }
        ohms = rtd_lead_compensate(c->wiring, ohms, lead, c->lead_ohms) * ug + c->user_offset;
        double r0 = c->r0_ohms > 0 ? c->r0_ohms : (c->sensor == TEMP_SENSOR_PT100 ? 100.0 : 1000.0);
        if (ohms < 0.18 * r0 || ohms > 4.0 * r0) { finish(s, i, TEMP_ST_RANGE, ohms, 0, sd / amps); return; }
        finish(s, i, TEMP_ST_OK, ohms, rtd_cvd_temp_c(ohms, r0), sd / amps);
    } else {
        double mv = v * 1000.0 * ug + c->user_offset;
        if (c->sensor == TEMP_SENSOR_VOLTAGE) { finish(s, i, TEMP_ST_OK, mv, mv, sd * 1000.0); return; }
        tc_type_t t = c->sensor == TEMP_SENSOR_TC_K ? TC_K : c->sensor == TEMP_SENSOR_TC_J ? TC_J : TC_T;
        double tc;
        if (tc_compensated_temp(t, mv, s->cjc_c, &tc)) { finish(s, i, TEMP_ST_RANGE, mv, 0, sd * 1000.0); return; }
        finish(s, i, TEMP_ST_OK, mv, tc, sd * 1000.0);
    }

    /* Open detect on its own cycle (ODC x 2.4k would bias the measurement). ODC is an excitation too,
     * so it goes through the same single-source sequencing. */
    if (c->open_detect && !is_rtd(c->sensor)) {
        double ov, osd;
        if (!temp_cfg_to_regs(c, true, &r) &&
            sample_routed(s, (uint8_t)i, &r, c->gain, 1, c->exc_route, &ov, &osd) == TEMP_ST_RANGE)
            s->out[i].status = TEMP_ST_OPEN;
    }
}

bool temp_sched_poll(temp_sched_t *s, uint32_t now_ms)
{
    if (s->started && (int32_t)(now_ms - s->next_due_ms) < 0) return false;
    s->started = true;
    s->next_due_ms = now_ms + s->prof->period_ms;
    update_cjc(s);
    for (unsigned i = 0; i < TEMP_MAX_CHANNELS; i++) scan_channel(s, i);
    return true;
}
