/* NAFE13388 command/register encoding for the temperature path.
 * Field shifts mirror NXP rd-riop common/m7/NAFE_hw/nafe13388.c (NAFE_init) and DS Rev 5.0 Table 29. */
#include "dce_ng/temp_path.h"

uint16_t nafe_cmd_reg(uint8_t dev_addr, uint16_t reg, bool read)
{
    /* bit15 hw address, bit14 R=1/W=0, bits13..1 register address (NXP NAFE_combCmdForAccessRegs) */
    return (uint16_t)(((dev_addr & 1u) << 15) | ((read ? 1u : 0u) << 14) | ((reg & 0x1FFFu) << 1));
}

uint16_t nafe_cmd_instr(uint8_t dev_addr, uint16_t instr)
{
    return (uint16_t)(((dev_addr & 1u) << 15) | ((instr & 0x3FFFu) << 1));
}

float nafe_gain_value(nafe_gain_t g)
{
    static const float k[8] = {0.2f, 0.4f, 0.8f, 1.0f, 2.0f, 4.0f, 8.0f, 16.0f};
    return k[(unsigned)g & 7u];
}

int32_t nafe_sign_extend24(uint32_t raw)
{
    raw &= 0xFFFFFFu;
    return (raw & 0x800000u) ? (int32_t)(raw | 0xFF000000u) : (int32_t)raw;
}

double nafe_code_to_volts(int32_t code24, nafe_gain_t g)
{
    /* Full scale +/-(5 V / G) for +/-2^23 (same as rd-riop api_afe: 10/G/2^24). */
    return (double)code24 * 10.0 / ((double)nafe_gain_value(g) * 16777216.0);
}

double nafe_die_temp_c(uint16_t raw)
{
    return (double)(int16_t)raw / 64.0; /* DS: DIE_TEMP code/64 degC */
}

double nafe_iex_amps(nafe_iex_t code)
{
    /* DS Table 16 current column, codes 0..15 */
    static const double a[16] = {0, 977e-9, 2e-6, 3.9e-6, 7.8e-6, 15.6e-6, 31.3e-6, 62.5e-6,
                                 125e-6, 250e-6, 375e-6, 500e-6, 750e-6, 1e-3, 1.5e-3, 2e-3};
    return a[(unsigned)code & 15u];
}

void nafe_decode_id(uint16_t pn2, uint16_t pn1, uint16_t pn0, nafe_id_t *id)
{
    id->pn2 = pn2;
    id->pn1 = pn1;
    id->pn0 = pn0;
    id->revision = (uint8_t)(pn0 & 0xFFu);
    id->id_ok = (pn2 == NAFE_EXPECT_PN2) && (pn1 == NAFE_EXPECT_PN1);
}

static bool is_rtd(temp_sensor_t s) { return s == TEMP_SENSOR_PT100 || s == TEMP_SENSOR_PT1000; }

int temp_cfg_to_regs(const temp_ch_cfg_t *c, bool odc_cycle, nafe_ch_regs_t *o)
{
    if (!c || !o || c->sensor == TEMP_SENSOR_OFF) return -1;
    if (c->sense_p > NAFE_HV_VEXC || c->sense_n > NAFE_HV_VEXC) return -2;
    if (c->gain > NAFE_G16 || c->data_rate_code > 28u || c->sinc > 4u) return -3;
    if (is_rtd(c->sensor)) {
        if (c->exc_side == NAFE_EXC_NONE || c->exc_pin < 1u || c->exc_pin > 4u) return -4;
        if (c->exc_current < NAFE_IEX_125UA) return -5;
        /* Excitation pin must not be a sense input in 4-wire (force and sense separate). */
        if (c->wiring == TEMP_WIRE_4 &&
            ((c->exc_side == NAFE_EXC_AIP && c->sense_p == c->exc_pin) ||
             (c->exc_side == NAFE_EXC_AIN && c->sense_n == c->exc_pin)))
            return -6;
    }
    unsigned tcc_off = 1u; /* match NXP temp config: TCC off */
    o->cfg0 = (uint16_t)((c->sense_p << 12) | (c->sense_n << 8) | ((unsigned)c->gain << 5) |
                         (1u << 4) /* HV_SEL */ | tcc_off);
    /* CH_CAL_GAIN_OFFSET pointer = gain index (NXP default slot per gain) */
    o->cfg1 = (uint16_t)(((unsigned)c->gain << 12) | ((c->data_rate_code & 0x1Fu) << 3) | (c->sinc & 7u));
    /* Single-cycle settling (bit9 = 0): every scan switches the mux, so each result must be settled.
     * (NXP's rd-riop temp example uses normal settling on one fixed channel.) */
    o->cfg2 = (uint16_t)((11u << 10) /* CH_DELAY like NXP */ |
                         ((c->ch_chop ? 1u : 0u) << 7));
    uint16_t c3 = 0;
    if (odc_cycle) {
        c3 = (uint16_t)(1u << 8); /* ODC 65 nA supersedes VIEX_VI; route like excitation if RTD, else to sense P */
        if (c->exc_side == NAFE_EXC_AIP) c3 |= (uint16_t)(c->exc_pin << 3);
        else if (c->exc_side == NAFE_EXC_AIN) c3 |= c->exc_pin;
        else if (c->sense_p >= NAFE_HV_AI1 && c->sense_p <= NAFE_HV_AI4) c3 |= (uint16_t)(c->sense_p << 3);
    } else if (is_rtd(c->sensor)) {
        c3 = (uint16_t)((1u << 15) /* current */ | ((unsigned)c->exc_current << 10) |
                        ((c->viex_chop ? 1u : 0u) << 6));
        if (c->exc_side == NAFE_EXC_AIP) c3 |= (uint16_t)(c->exc_pin << 3);
        else c3 |= c->exc_pin;
    }
    o->cfg3 = c3;
    return 0;
}

/* DS Table 7, single-cycle settling, SINC4+SINC1 column (codes 12..28) / SINC4 (0..11), 4.608 MHz clock */
double nafe_scs_rate_sps(uint8_t code)
{
    static const double r[29] = {72000, 48000, 36000, 24000, 18000, 12000, 9000, 6000, 4500, 3000, 2250, 1500,
                                 2250, 1500, 1125, 562.5, 281.25, 200, 150, 100, 50, 30, 25, 15, 12.5, 10,
                                 7.5, 5, 3.75};
    return code <= 28 ? r[code] : 0.0;
}

double temp_scan_time_ms(const temp_profile_t *p)
{
    double ms = 0.0;
    for (unsigned i = 0; i < TEMP_MAX_CHANNELS; i++) {
        const temp_ch_cfg_t *c = &p->ch[i];
        if (c->sensor == TEMP_SENSOR_OFF) continue;
        double conv = 1000.0 / nafe_scs_rate_sps(c->data_rate_code);
        if (c->ch_chop || (c->viex_chop && (c->sensor == TEMP_SENSOR_PT100 || c->sensor == TEMP_SENSOR_PT1000)))
            conv *= 2.0;
        unsigned n = p->oversample;
        if (c->wiring == TEMP_WIRE_3) n *= 2;
        ms += conv * n + (c->open_detect ? conv : 0.0);
    }
    if (p->lvsig_r_top > 0.0f) ms += 2.0 * 1000.0 / nafe_scs_rate_sps(20) * p->oversample; /* R347 read, chop */
    return ms;
}
