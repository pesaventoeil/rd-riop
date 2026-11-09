/* Test profiles. First profile = EI Electrical Dev RIOP B2 bench hookup
 * (meeting_20261111/AFE_LC_RTD_SPLIT_ANALYSIS.md §4.7), NOT yet confirmed/wired by Daniel:
 *   ch0: 100 R 0.01% 4-wire: force AI1P (VIEX 1 mA), sense AI2P(+)/AI2N(-), bottom -> AGND, G16, VIEX chop
 *   ch1: shorted input AI3P/AI3N (noise), G16
 *   ch2: off (reserved: mV source later on AI3P/N)
 *   ch3: type K on AI4P/AI4N, G16, ODC open detect
 * CJC: NAFE DIE_TEMP (bring-up only: board temperature, not the TC terminal).
 * 1 SPS: 4 conversions/channel at 50 SPS single-cycle settling (code 20, SINC4+SINC1); chop doubles
 * conversion time. Scan budget checked in tests (temp_scan_time_ms < period). Heavy averaging comes from the
 * NAFE digital filter + trimmed mean; slow profile adds IIR. */
#include "dce_ng/temp_path.h"

#define RTD4_AI1P_AI2(sensor_, iex_, gain_) \
    {.sensor = sensor_, .wiring = TEMP_WIRE_4, .sense_p = NAFE_HV_AI2, .sense_n = NAFE_HV_AI2, \
     .exc_side = NAFE_EXC_AIP, .exc_pin = 1, .exc_current = iex_, .viex_chop = true, .gain = gain_, \
     .data_rate_code = 20, .sinc = 1, .ch_chop = false, .user_gain = 1.0f}

const temp_profile_t temp_profile_riop_b2_bench1 = {
    .name = "riop_b2_bench1_pt100",
    .period_ms = 1000,
    .oversample = 4,
    .trim = 0,
    .iir_alpha = 0.0f,
    .cjc_src = TEMP_CJC_DIE,
    .cjc_fixed_c = 25.0f,
    .lvsig_ref_v = 2.5f,
    .lvsig_r_top = 5880.0f,
    .ch = {
        RTD4_AI1P_AI2(TEMP_SENSOR_PT100, NAFE_IEX_1MA, NAFE_G16),
        {.sensor = TEMP_SENSOR_VOLTAGE, .sense_p = NAFE_HV_AI3, .sense_n = NAFE_HV_AI3, .gain = NAFE_G16,
         .data_rate_code = 20, .sinc = 1, .ch_chop = true, .user_gain = 1.0f},
        {.sensor = TEMP_SENSOR_OFF},
        {.sensor = TEMP_SENSOR_TC_K, .sense_p = NAFE_HV_AI4, .sense_n = NAFE_HV_AI4, .gain = NAFE_G16,
         .data_rate_code = 20, .sinc = 1, .ch_chop = true, .open_detect = true, .user_gain = 1.0f},
    },
};

/* PT1000 at 250 uA: 0 C -> 250 mV, above the G16 nominal (+/-250 mV) range; use G8 (+/-625 mV). */
const temp_profile_t temp_profile_riop_b2_bench1_pt1000 = {
    .name = "riop_b2_bench1_pt1000",
    .period_ms = 1000,
    .oversample = 4,
    .trim = 0,
    .iir_alpha = 0.0f,
    .cjc_src = TEMP_CJC_DIE,
    .cjc_fixed_c = 25.0f,
    .lvsig_ref_v = 2.5f,
    .lvsig_r_top = 5880.0f,
    .ch = {
        RTD4_AI1P_AI2(TEMP_SENSOR_PT1000, NAFE_IEX_250UA, NAFE_G8),
        {.sensor = TEMP_SENSOR_OFF},
        {.sensor = TEMP_SENSOR_OFF},
        {.sensor = TEMP_SENSOR_OFF},
    },
};

const temp_profile_t temp_profile_slow_1pm = {
    .name = "slow_1pm_4xpt1000_3w",
    .period_ms = 60000,
    .oversample = 32,
    .trim = 4,
    .iir_alpha = 0.5f,
    .cjc_src = TEMP_CJC_DIE,
    .cjc_fixed_c = 25.0f,
    .lvsig_ref_v = 2.5f,
    .lvsig_r_top = 5880.0f,
    .ch = {
        /* placeholder pin map for a 4-cell design: needs Electrical Dev's final mapping */
        {.sensor = TEMP_SENSOR_PT1000, .wiring = TEMP_WIRE_2, .sense_p = NAFE_HV_AI1, .sense_n = NAFE_HV_GND,
         .exc_side = NAFE_EXC_AIP, .exc_pin = 1, .exc_current = NAFE_IEX_250UA, .viex_chop = true,
         .gain = NAFE_G8, .data_rate_code = 25, .sinc = 1, .user_gain = 1.0f},
        {.sensor = TEMP_SENSOR_PT1000, .wiring = TEMP_WIRE_2, .sense_p = NAFE_HV_AI2, .sense_n = NAFE_HV_GND,
         .exc_side = NAFE_EXC_AIP, .exc_pin = 2, .exc_current = NAFE_IEX_250UA, .viex_chop = true,
         .gain = NAFE_G8, .data_rate_code = 25, .sinc = 1, .user_gain = 1.0f},
        {.sensor = TEMP_SENSOR_PT1000, .wiring = TEMP_WIRE_2, .sense_p = NAFE_HV_AI3, .sense_n = NAFE_HV_GND,
         .exc_side = NAFE_EXC_AIP, .exc_pin = 3, .exc_current = NAFE_IEX_250UA, .viex_chop = true,
         .gain = NAFE_G8, .data_rate_code = 25, .sinc = 1, .user_gain = 1.0f},
        {.sensor = TEMP_SENSOR_TC_K, .sense_p = NAFE_HV_AI4, .sense_n = NAFE_HV_AI4, .gain = NAFE_G16,
         .data_rate_code = 25, .sinc = 1, .ch_chop = true, .open_detect = true, .user_gain = 1.0f},
    },
};

/* R347 self-check (RIOP B2, no wiring): GPIO0-GPIO1 LVSIG across R347, ratiometric to REF_BYP through
 * R346/R349 2.94k 0.1 %, logged with DIE_TEMP every second. All AI channels off, no excitation. */
const temp_profile_t temp_profile_riop_r347_check = {
    .name = "riop_r347_selfcheck",
    .period_ms = 1000,
    .oversample = 8,
    .trim = 1,
    .iir_alpha = 0.0f,
    .cjc_src = TEMP_CJC_LVSIG_RTD,
    .cjc_fixed_c = 25.0f,
    .lvsig_ref_v = 2.5f,
    .lvsig_r_top = 5880.0f,
    .ch = {{.sensor = TEMP_SENSOR_OFF}, {.sensor = TEMP_SENSOR_OFF}, {.sensor = TEMP_SENSOR_OFF},
           {.sensor = TEMP_SENSOR_OFF}},
};

/* Bench0: safe with unknown wiring. No VIEX/ODC on any pin. Shorted-input noise via the internal
 * HV mux GND/GND (true short at the PGA input, no terminal wiring). AI3P/AI3N is informational only
 * (unwired = floating behind 2.4k + 1 nF). R347 + DIE_TEMP every scan. 1 SPS. */
const temp_profile_t temp_profile_riop_b2_bench0 = {
    .name = "riop_b2_bench0_noexc",
    .period_ms = 1000,
    .oversample = 4,
    .trim = 0,
    .iir_alpha = 0.0f,
    .cjc_src = TEMP_CJC_DIE,
    .cjc_fixed_c = 25.0f,
    .lvsig_ref_v = 2.5f,
    .lvsig_r_top = 5880.0f,
    .ch = {
        {.sensor = TEMP_SENSOR_VOLTAGE, .sense_p = NAFE_HV_GND, .sense_n = NAFE_HV_GND, .gain = NAFE_G16,
         .data_rate_code = 20, .sinc = 1, .ch_chop = false, .user_gain = 1.0f},
        {.sensor = TEMP_SENSOR_VOLTAGE, .sense_p = NAFE_HV_GND, .sense_n = NAFE_HV_GND, .gain = NAFE_G16,
         .data_rate_code = 20, .sinc = 1, .ch_chop = true, .user_gain = 1.0f},
        {.sensor = TEMP_SENSOR_VOLTAGE, .sense_p = NAFE_HV_AI3, .sense_n = NAFE_HV_AI3, .gain = NAFE_G16,
         .data_rate_code = 20, .sinc = 1, .ch_chop = true, .user_gain = 1.0f},
        {.sensor = TEMP_SENSOR_OFF},
    },
};
