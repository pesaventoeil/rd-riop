/*
 * DCE NextGen slow temperature path (NAFE13388 universal temperature AFE).
 *
 * Independent of the load-cell path (Daniel 2026-11-09 10:06 ET): temperature runs
 * on its own slow schedule (1 SPS default, 1/min option) with heavy averaging.
 * Portable C11, no SDK: the NAFE SPI transport is injected through temp_afe_ops_t
 * so all logic is host-testable (logic-first directive, DECISIONS 2026-11-05/06).
 *
 * Register encodings follow NAFE13388 DS Rev 5.0 (17 Feb 2025) and match the NXP
 * rd-riop common/m7/NAFE_hw/nafe13388.c field shifts.
 */
#ifndef DCE_NG_TEMP_PATH_H
#define DCE_NG_TEMP_PATH_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TEMP_MAX_CHANNELS 4u
#define TEMP_MAX_OVERSAMPLE 64u

/* ---------------- NAFE13388 register map subset (DS Rev 5.0 §8) ---------------- */
#define TP_NAFE_REG_CH_CONFIG0 0x20u
#define TP_NAFE_REG_CH_CONFIG1 0x21u
#define TP_NAFE_REG_CH_CONFIG2 0x22u
#define TP_NAFE_REG_CH_CONFIG3 0x23u
#define TP_NAFE_REG_CH_CONFIG4 0x24u
#define TP_NAFE_REG_SYS_CONFIG0 0x30u
#define TP_NAFE_REG_SYS_STATUS0 0x31u
#define TP_NAFE_REG_DIE_TEMP 0x34u
#define TP_NAFE_REG_CH_STATUS0 0x35u
/* DS Rev 5.0 names. NOTE: the NXP rd-riop nafe13388.h labels 0x7C/0x7D as SERIAL0/1
 * and 0x7E/0x7F as PRODREV/PRODID; the data sheet has 0x7C=PN2, 0x7D=PN1,
 * 0x7E=PN0 (REVISION_ID[7:0]), 0x7F=CRC_TRIM_INT, serial at 0xAE/0xAF (24-bit). */
#define TP_NAFE_REG_PN2 0x7Cu
#define TP_NAFE_REG_PN1 0x7Du
#define TP_NAFE_REG_PN0 0x7Eu
#define TP_NAFE_REG_CRC_TRIM_INT 0x7Fu
#define TP_NAFE_REG_SERIAL1 0xAEu
#define TP_NAFE_REG_SERIAL0 0xAFu
#define TP_NAFE_REG_OPT_COEF3 0xA3u /* RTD_OFFSET1, G16 */
#define TP_NAFE_REG_OPT_COEF4 0xA4u /* RTD_GAIN1,   G16 */
#define TP_NAFE_REG_OPT_COEF5 0xA5u /* RTD_OFFSET2, G8  */
#define TP_NAFE_REG_OPT_COEF6 0xA6u /* RTD_GAIN2,   G8  */

#define TP_NAFE_CMD_CH0 0x0000u
#define TP_NAFE_CMD_ABORT 0x0010u
#define TP_NAFE_CMD_RESET 0x0014u
#define TP_NAFE_CMD_SS 0x2000u
#define TP_NAFE_CMD_MS 0x2004u

#define NAFE_EXPECT_PN2 0x1338u /* DS example for NAFE13388B40BS */
#define NAFE_EXPECT_PN1 0x8B40u

/* HV mux selections (CH_CONFIG0 HV_AIP[15:12] / HV_AIN[11:8]) */
typedef enum {
    NAFE_HV_GND = 0, NAFE_HV_AI1 = 1, NAFE_HV_AI2 = 2, NAFE_HV_AI3 = 3, NAFE_HV_AI4 = 4,
    NAFE_HV_REFCAL_H = 5, NAFE_HV_REFCAL_L = 6, NAFE_HV_AICOM = 7, NAFE_HV_VEXC = 8
} nafe_hv_sel_t;

/* LVSIG_IN[3:1] */
typedef enum {
    NAFE_LV_HALFREF = 0, NAFE_LV_GPIO0_GPIO1 = 1, NAFE_LV_HALFREF_COARSE = 2,
    NAFE_LV_VADD = 3, NAFE_LV_VHDD = 4, NAFE_LV_VHSS = 5
} nafe_lv_sel_t;

typedef enum { NAFE_G0P2 = 0, NAFE_G0P4, NAFE_G0P8, NAFE_G1, NAFE_G2, NAFE_G4, NAFE_G8, NAFE_G16 } nafe_gain_t;

/* VIEX_MAG codes (DS Table 16, current mode) */
typedef enum {
    NAFE_IEX_OFF = 0, NAFE_IEX_125UA = 8, NAFE_IEX_250UA = 9, NAFE_IEX_375UA = 10,
    NAFE_IEX_500UA = 11, NAFE_IEX_750UA = 12, NAFE_IEX_1MA = 13, NAFE_IEX_1P5MA = 14, NAFE_IEX_2MA = 15
} nafe_iex_t;

/* Excitation pin: VIEX_AIP_EN (AIxP) or VIEX_AIN_EN (AIxN), x = 1..4 */
typedef enum { NAFE_EXC_NONE = 0, NAFE_EXC_AIP = 1, NAFE_EXC_AIN = 2 } nafe_exc_side_t;

/* Raw per-logical-channel register words (16-bit registers) */
typedef struct {
    uint16_t cfg0, cfg1, cfg2, cfg3;
} nafe_ch_regs_t;

/* ---------------- Per-channel temperature config table ---------------- */
typedef enum {
    TEMP_SENSOR_OFF = 0,
    TEMP_SENSOR_PT100,
    TEMP_SENSOR_PT1000,
    TEMP_SENSOR_TC_K,
    TEMP_SENSOR_TC_J,
    TEMP_SENSOR_TC_T,
    TEMP_SENSOR_VOLTAGE, /* raw mV: shorted-input noise or mV-source check */
} temp_sensor_t;

typedef enum { TEMP_WIRE_2 = 2, TEMP_WIRE_3 = 3, TEMP_WIRE_4 = 4 } temp_wiring_t;

typedef enum {
    TEMP_CJC_DIE = 0,    /* NAFE DIE_TEMP (bring-up only: board temperature, not TC block) */
    TEMP_CJC_LVSIG_RTD,  /* PT100 on LVSIG GPIO0/GPIO1 (DS Fig. 44); RIOP R347 unconfirmed */
    TEMP_CJC_FIXED,      /* fixed value in cjc_fixed_c */
} temp_cjc_src_t;

typedef struct {
    temp_sensor_t sensor;
    temp_wiring_t wiring;
    uint8_t sense_p;        /* nafe_hv_sel_t for ADC +input */
    uint8_t sense_n;        /* nafe_hv_sel_t for ADC -input */
    /* Force (excitation) path. The design assumes exactly ONE excitation source (NAFE VIEX), sequenced
     * per channel by the scheduler: never two channels' excitation active at once.
     *  - exc_side/exc_pin: which NAFE AI pin VIEX (or ODC) leaves from (default 2/3-wire-per-cell layout).
     *  - exc_route: abstract force route for future layouts (external 1:N switch position, or a second
     *    NAFE index). 0 = no external routing; the board layer maps it through temp_afe_ops_t.select_force. */
    nafe_exc_side_t exc_side;
    uint8_t exc_pin;        /* 1..4 */
    uint8_t exc_route;      /* 0 = none, 1..N = board-specific force route */
    nafe_iex_t exc_current;
    bool viex_chop;         /* reverse excitation, 2 conversions, cancels offsets */
    nafe_gain_t gain;
    uint8_t data_rate_code; /* CH_CONFIG1 ADC_DATA_RATE[4:0] */
    uint8_t sinc;           /* CH_CONFIG1 ADC_SINC[2:0] */
    bool ch_chop;
    bool open_detect;       /* 65 nA ODC on a separate check cycle (not during measurement) */
    uint8_t lead_p, lead_n; /* 3-wire only: sense pair across one lead (same excitation) */
    float lead_ohms;        /* 2-wire: total lead + series R to subtract (bring-up RIOP: 2x2.4k only if in loop) */
    float r0_ohms;          /* RTD nominal; 0 -> 100/1000 from sensor */
    float user_gain;        /* trim, default 1.0 */
    float user_offset;      /* trim in engineering units (ohm or mV) */
} temp_ch_cfg_t;

typedef struct {
    const char *name;
    uint32_t period_ms;     /* 1000 (1 SPS) or 60000 (1/min) */
    uint8_t oversample;     /* conversions averaged per channel per period */
    uint8_t trim;           /* drop this many highest and lowest before averaging */
    float iir_alpha;        /* 0 or 1 = off; else output = a*new + (1-a)*old */
    temp_cjc_src_t cjc_src;
    float cjc_fixed_c;
    float lvsig_ref_v;      /* LVSIG CJC divider: REF_BYP volts (RIOP 2.5) */
    float lvsig_r_top;      /* sum of series resistors other than the RTD (RIOP R346+R349 = 5880) */
    temp_ch_cfg_t ch[TEMP_MAX_CHANNELS];
} temp_profile_t;

/* First test profile: EI Electrical Dev RIOP B2 bench hookup (AFE_LC_RTD_SPLIT_ANALYSIS §4.7). */
extern const temp_profile_t temp_profile_riop_b2_bench1;
/* Same pins, PT1000 resistor at 250 uA / G8 */
extern const temp_profile_t temp_profile_riop_b2_bench1_pt1000;
/* 1/min option: slow filter, more averaging, IIR across scans */
extern const temp_profile_t temp_profile_slow_1pm;
/* Bench with nothing wired: no excitation anywhere. Internal GND/GND short (chop off/on), open AI3, R347 + DIE_TEMP. */
extern const temp_profile_t temp_profile_riop_b2_bench0;
double temp_scan_time_ms(const temp_profile_t *p);

/* ---------------- NAFE encoding helpers ---------------- */
uint16_t nafe_cmd_reg(uint8_t dev_addr, uint16_t reg, bool read);
uint16_t nafe_cmd_instr(uint8_t dev_addr, uint16_t instr);
int temp_cfg_to_regs(const temp_ch_cfg_t *c, bool odc_cycle, nafe_ch_regs_t *out);
float nafe_gain_value(nafe_gain_t g);
double tp_exp(double x); /* compact exp (thermocouple.c) */
double nafe_code_to_volts(int32_t code24, nafe_gain_t g);
int32_t nafe_sign_extend24(uint32_t raw);
double nafe_die_temp_c(uint16_t raw);
double nafe_iex_amps(nafe_iex_t code);
double nafe_scs_rate_sps(uint8_t data_rate_code);

typedef struct {
    uint16_t pn2, pn1, pn0, crc_trim;
    uint32_t serial1, serial0;
    uint8_t revision;
    bool id_ok;
} nafe_id_t;
void nafe_decode_id(uint16_t pn2, uint16_t pn1, uint16_t pn0, nafe_id_t *id);

/* ---------------- Conversions ---------------- */
double rtd_cvd_ohms(double t_c, double r0);          /* IEC 60751 Callendar-Van Dusen */
double rtd_cvd_temp_c(double r_ohms, double r0);     /* inverse, -200..850 C */
/* 3-wire: r_a = force-lead + RTD (sense P vs return), r_b = lead only. RTD = r_a - r_b. */
double rtd_lead_compensate(temp_wiring_t w, double r_a, double r_b, double lead_ohms);

typedef enum { TC_K, TC_J, TC_T } tc_type_t;
int tc_temp_to_mv(tc_type_t t, double t_c, double *mv);  /* NIST ITS-90 forward */
int tc_mv_to_temp(tc_type_t t, double mv, double *t_c);  /* NIST ITS-90 inverse */
/* Hot-junction temperature from measured mV and cold-junction temperature */
int tc_compensated_temp(tc_type_t t, double meas_mv, double cjc_c, double *t_c);

/* ---------------- Averaging + scheduler ---------------- */
double temp_trimmed_mean(const double *x, unsigned n, unsigned trim, double *stdev_out);

typedef struct {
    /* Program one logical channel (slot 0..15) with register words */
    int (*program)(void *ctx, uint8_t slot, const nafe_ch_regs_t *regs);
    /* Single-channel single conversion on slot; returns signed 24-bit code */
    int (*convert)(void *ctx, uint8_t slot, int32_t *code);
    int (*read_reg16)(void *ctx, uint16_t reg, uint16_t *val);
    /* Optional: connect force route `route` (0 = disconnect all). Called before an excited channel's
     * conversions and with 0 right after them. NULL on a single-NAFE board with no switch. */
    int (*select_force)(void *ctx, uint8_t route);
    void *ctx;
} temp_afe_ops_t;

typedef enum { TEMP_ST_OK = 0, TEMP_ST_OFF, TEMP_ST_OPEN, TEMP_ST_RANGE, TEMP_ST_COMM } temp_status_t;

typedef struct {
    temp_status_t status;
    double value;     /* degC, or mV for TEMP_SENSOR_VOLTAGE */
    double raw;       /* averaged ohms (RTD) or mV (TC/voltage) */
    double noise_rms; /* stdev of the oversampled raw values */
    uint32_t scans;
} temp_reading_t;

typedef struct {
    const temp_profile_t *prof;
    const temp_afe_ops_t *ops;
    uint32_t next_due_ms;
    bool started;
    double cjc_c;
    double die_c;          /* NAFE DIE_TEMP, read every scan when read_reg16 is provided */
    double lvsig_ohms;     /* LVSIG GPIO0-GPIO1 element (RIOP R347), when cjc_src == TEMP_CJC_LVSIG_RTD */
    temp_status_t lvsig_status;
    int exc_owner;         /* channel slot currently holding the excitation, -1 = none */
    int32_t last_code, mean_code;  /* scratch from the most recent sample() */
    nafe_ch_regs_t last_regs;
    struct { int32_t last_code, mean_code; nafe_ch_regs_t regs; } dbg[TEMP_MAX_CHANNELS]; /* as applied */
    temp_reading_t out[TEMP_MAX_CHANNELS];
} temp_sched_t;

void temp_sched_init(temp_sched_t *s, const temp_profile_t *p, const temp_afe_ops_t *ops);
/* Call from a low-priority task; runs one full scan when due. Returns true if a scan ran. */
bool temp_sched_poll(temp_sched_t *s, uint32_t now_ms);

/* ---------------- R347 self-check (RIOP B2) ----------------
 * LVSIG GPIO0-GPIO1 across R347 in REF_BYP -> R346 (2.94k) -> R347 -> R349 (2.94k) -> AGND (topology assumed).
 * R = V * (R346 + R349) / (Vref - V). Classify by slope vs DIE_TEMP: PT100 ~0.385 ohm/C, fixed 100 R ~0. */
extern const temp_profile_t temp_profile_riop_r347_check;
double lvsig_divider_ohms(double v, double vref, double r_top);
typedef enum { R347_UNKNOWN = 0, R347_NEED_SPAN, R347_RTD_PT100, R347_FIXED_100R } r347_class_t;
typedef struct {
    unsigned n;
    double sx, sy, sxx, sxy, xmin, xmax;
} r347_acc_t;
void r347_acc_init(r347_acc_t *a);
void r347_acc_add(r347_acc_t *a, double die_c, double ohms);
double r347_slope(const r347_acc_t *a);   /* ohm / C, least squares */
double r347_mean_ohms(const r347_acc_t *a);
/* min_span_c: die-temp span needed before the slope is trusted (e.g. 3 C). */
r347_class_t r347_classify(const r347_acc_t *a, double min_span_c);
/* Single-point plausibility: PT100 at room temperature reads ~107-112 ohm */
bool r347_room_plausible_pt100(double ohms);
const char *r347_class_name(r347_class_t c);
/* CSV log line: "ms,die_c,r347_ohms,status" */
int temp_r347_log_line(const temp_sched_t *s, uint32_t now_ms, char *buf, unsigned len);

#ifdef __cplusplus
}
#endif
#endif
