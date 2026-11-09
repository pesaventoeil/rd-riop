/*
 * DCE NextGen NAFE temperature path <-> EtherNet/IP assemblies (RIOP gateway image).
 *   asm 174: read-only input (RIOP -> PC), explicit Get_Attribute_Single class 0x04 attr 3.
 *   asm 175: NAFE-only config output (PC -> RIOP), Set_Attribute_Single; compiled only with DEBUG_NAFE.
 * Spec: /workspace/dce_nextgen/NAFE_TEMP_EIP_LAYOUT.md (layout version 1). All fields little-endian,
 * packed, REAL32 = IEEE-754 single. Release offsets are identical in release and DEBUG_NAFE builds.
 */
#ifndef DCE_NG_TEMP_EIP_H
#define DCE_NG_TEMP_EIP_H

#include <stddef.h>
#include <stdint.h>
#include "dce_ng/temp_path.h"

#if defined(RIOP) && RIOP
#include "nafe_temp_cfg.h" /* RIOP gateway build: DEBUG_NAFE, NAFE_LOG_STREAM */
#endif
#ifndef DEBUG_NAFE
#define DEBUG_NAFE 0
#endif

#define TEMP_EIP_ASM_IN 174u
#define TEMP_EIP_ASM_CFG 175u
#define TEMP_EIP_LAYOUT_VERSION 1u

#define TEMP_EIP_HDR_LEN 32u
#define TEMP_EIP_CH_LEN 16u
#define TEMP_EIP_RELEASE_LEN (TEMP_EIP_HDR_LEN + TEMP_MAX_CHANNELS * TEMP_EIP_CH_LEN) /* 96 */
#define TEMP_EIP_DBG_HDR_LEN 40u
#define TEMP_EIP_DBG_CH_LEN 24u
#define TEMP_EIP_DBG_LEN (TEMP_EIP_DBG_HDR_LEN + TEMP_MAX_CHANNELS * TEMP_EIP_DBG_CH_LEN) /* 136 */
#if DEBUG_NAFE
#define TEMP_EIP_IN_LEN (TEMP_EIP_RELEASE_LEN + TEMP_EIP_DBG_LEN) /* 232 */
#else
#define TEMP_EIP_IN_LEN TEMP_EIP_RELEASE_LEN /* 96 */
#endif
#define TEMP_EIP_CFG_LEN 32u
/* N9: asm 176 = lossless scan log (read-only, explicit Get_Attribute_Single class 0x04 attr 3; DEBUG_NAFE builds).
 * Ring of the last TEMP_EIP_LOG_SLOTS completed scans, slot = scan_count % SLOTS. Record = the 96 B release image of
 * that scan (asm 174 bytes 0..95) with bytes 0..1 = 175 applied_count, 2..3 = skip_count, 4..7 = scan_count.
 * Header: u16 version 1, u16 rec_len 96, u8 slots, u8 valid records, u16 0, u32 newest scan_count, u32 frame. */
#ifndef NAFE_LOG_STREAM
#define NAFE_LOG_STREAM DEBUG_NAFE
#endif
#define TEMP_EIP_ASM_LOG 176u
#define TEMP_EIP_LOG_VERSION 1u
#define TEMP_EIP_LOG_HDR_LEN 16u
#define TEMP_EIP_LOG_REC_LEN TEMP_EIP_RELEASE_LEN
#define TEMP_EIP_LOG_SLOTS 4u
#define TEMP_EIP_LOG_LEN (TEMP_EIP_LOG_HDR_LEN + TEMP_EIP_LOG_SLOTS * TEMP_EIP_LOG_REC_LEN) /* 400 */

/* per-channel status byte (174) */
#define TEMP_ST_BIT_VALID 0x01u
#define TEMP_ST_BIT_OPEN_SHORT 0x02u
#define TEMP_ST_BIT_RANGE 0x04u
#define TEMP_ST_BIT_STALE 0x08u
#define TEMP_ST_BIT_COMM 0x10u
#define TEMP_ST_BIT_OFF 0x20u
#define TEMP_ST_BIT_OVERRIDE 0x40u /* channel runs a 175 RAM override (debug builds only) */

/* header flags byte (174 offset 22) */
#define TEMP_HF_NAFE_ID_OK 0x01u
#define TEMP_HF_TASK_RUNNING 0x02u
#define TEMP_HF_SCAN_SKIPPED 0x04u /* NXP AFE_task owned the NAFE this period */
#define TEMP_HF_DEBUG_PRESENT 0x08u
#define TEMP_HF_OVERRIDES_ACTIVE 0x10u
#define TEMP_HF_DIE_VALID 0x20u /* last DIE_TEMP read OK and fresh (age <= 3 x period + 1 s) */

/* R347 flags (174 offset 20) */
#define TEMP_R347_PASS 0x01u  /* valid and inside the 95..140 ohm plausibility window */
#define TEMP_R347_VALID 0x02u /* latest LVSIG read converted OK */
#define TEMP_R347_FAIL 0x04u  /* read attempted and invalid, or outside the window */
#define TEMP_R347_STALE 0x08u /* no valid read for > 3 x period + 1 s */
#define TEMP_R347_WIN_LO 95.0
#define TEMP_R347_WIN_HI 140.0
#define TEMP_HEARTBEAT_MS 1000u

/* 175 ops */
enum { TEMP_OP_NONE = 0, TEMP_OP_SET_CH = 1, TEMP_OP_CLEAR_CH = 2, TEMP_OP_RESTORE_DEFAULTS = 3,
       TEMP_OP_SET_BOARD = 4, TEMP_OP_SELECT_PROFILE = 5 };
/* 175 SET_CH field mask bits (also the reject-mask bits) */
enum {
    TEMP_F_SENSOR = 1u << 0, TEMP_F_WIRING = 1u << 1, TEMP_F_SENSE_P = 1u << 2, TEMP_F_SENSE_N = 1u << 3,
    TEMP_F_EXC_SIDE = 1u << 4, TEMP_F_EXC_PIN = 1u << 5, TEMP_F_EXC_CUR = 1u << 6, TEMP_F_GAIN = 1u << 7,
    TEMP_F_RATE = 1u << 8, TEMP_F_SINC = 1u << 9, TEMP_F_FLAGS = 1u << 10, TEMP_F_LEAD = 1u << 11,
    /* board (SET_BOARD) */
    TEMP_F_OVERSAMPLE = 1u << 12, TEMP_F_TRIM = 1u << 13, TEMP_F_PERIOD = 1u << 14, TEMP_F_IIR = 1u << 15,
};
/* reject-mask extra bits (not in field mask) */
#define TEMP_REJ_OP 0x00010000u
#define TEMP_REJ_CHANNEL 0x00020000u
#define TEMP_REJ_CHECKSUM 0x00040000u
#define TEMP_REJ_COMBINATION 0x00080000u /* fields valid alone but channel config invalid together */
#define TEMP_REJ_BUDGET 0x00100000u      /* scan would not fit the period */
#define TEMP_REJ_PROFILE 0x00200000u

/* 175 ack codes (174 debug block) */
enum { TEMP_ACK_NONE = 0, TEMP_ACK_OK = 1, TEMP_ACK_REJECTED = 2, TEMP_ACK_DISABLED = 3 };

/* Built-in profiles selectable by id (174 header byte 27, 175 SELECT_PROFILE) */
enum { TEMP_PROF_BENCH1_PT100 = 0, TEMP_PROF_BENCH1_PT1000 = 1, TEMP_PROF_SLOW_1PM = 2, TEMP_PROF_R347_CHECK = 3,
       TEMP_PROF_BENCH0_NOEXC = 4, TEMP_PROF_COUNT = 5 };
const temp_profile_t *temp_builtin_profile(unsigned id);

typedef struct {
    unsigned default_id;       /* built-in id the live profile came from */
    temp_profile_t live;       /* RAM working copy (175 overrides land here; never persisted) */
    uint8_t ch_override;       /* bit i = channel i differs from its built-in default */
    bool board_override;
    temp_sched_t sched;
    uint32_t frame;            /* 1 Hz publisher heartbeat, ticks in every profile (also mid-scan) */
    uint32_t hb_ms;            /* time of last heartbeat tick */
    bool hb_started;
    uint32_t now_ms;           /* latest time seen (stale ages) */
    uint32_t scan_count;       /* completed scans */
    uint32_t scan_tick_ms;     /* time of the last completed scan: payload changes only when this does */
    uint32_t last_ok_ms[TEMP_MAX_CHANNELS];
    bool ever_ok[TEMP_MAX_CHANNELS];
    uint32_t r347_ok_ms, die_ok_ms;
    bool die_ever_ok;
    bool r347_ever_ok, r347_tried;
    uint16_t samples[TEMP_MAX_CHANNELS];
    r347_acc_t r347;
    uint8_t hdr_flags;
    nafe_id_t id;
    uint16_t crc_trim, sys_status0;
    uint32_t serial1, serial0;
    uint16_t skip_count;
    uint32_t scan_time_us;
    /* 175 handshake */
    uint16_t last_seq, ack_seq, ack_code, applied_count;
    uint32_t reject_mask;
    uint8_t ack_op, ack_ch;
} temp_rt_t;

void temp_rt_init(temp_rt_t *rt, unsigned profile_id, const temp_afe_ops_t *ops);
/* Run the scheduler; on a completed scan updates counters/R347 accumulator. Returns true if scanned. */
bool temp_rt_poll(temp_rt_t *rt, uint32_t now_ms);
/* Advance the 1 Hz heartbeat (frame counter). Returns true if it ticked. Called by temp_rt_poll too. */
bool temp_rt_heartbeat(temp_rt_t *rt, uint32_t now_ms);
/* Stale age limit for this profile: 3 x period + 1 s. */
uint32_t temp_rt_stale_ms(const temp_rt_t *rt);
/* Serialize asm 174 into buf (len >= TEMP_EIP_IN_LEN). Returns bytes written. */
size_t temp_eip_pack174(const temp_rt_t *rt, uint8_t *buf, size_t len);
/* Process one asm 175 write. No-op (ACK_DISABLED) unless DEBUG_NAFE. Returns ack code. */
int temp_eip_apply175(temp_rt_t *rt, const uint8_t *buf, size_t len);
uint16_t temp_eip_checksum(const uint8_t *buf, size_t n); /* 16-bit byte sum over bytes 0..27 */

/* N9: add the scan packed in img174 to the asm 176 ring if rt->scan_count is new; always refresh the header.
 * Returns 1 if a record was added. */
int temp_eip_log_push(uint8_t *ring, const temp_rt_t *rt, const uint8_t *img174);

#endif
