/* Shared M33->M7 NETC/lwIP diagnostic snapshot.
 * Lives at a reserved 256-byte OCRAM1 window just below the RPMSG region
 * (0x204FDF00 when __multicore__ shmem is 8K). M33 writes, M7 reads.
 * OCRAM1 is non-cacheable on the M33 (MPU) and cacheable on the M7 (M7 invalidates).
 */
#ifndef RIOP_DIAG_NET_H
#define RIOP_DIAG_NET_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RIOP_DIAG_NET_MAGIC  0x4E31304Eu /* 'N10N' */
#define RIOP_DIAG_NET_ADDR   0x204FDF00u
#define RIOP_DIAG_NET_LEN    64u

typedef struct {
	uint32_t magic;
	uint32_t seq;
	uint32_t rx_frames;
	uint32_t tx_frames;
	uint32_t rx_errors;
	uint32_t tx_errors;
	uint32_t dma_stall;
	uint32_t dma_overflow;
	uint32_t lwip_pbuf_err;
	uint32_t lwip_mem_err;
	uint32_t enet_reinit_count;
	uint32_t flags;
	uint8_t  phy_link_mask;
	uint8_t  reserved0;
	uint16_t link_change_count;
	uint32_t reserved1[3];
} riop_diag_net_t;

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
_Static_assert(sizeof(riop_diag_net_t) == RIOP_DIAG_NET_LEN, "riop_diag_net_t must be 64 bytes");
#endif

/* N10g: M33 fault record, 64 B at window +0x40. Written by the M33 fault
 * handlers just before NVIC_SystemReset(); survives the warm reset (neither
 * core's startup touches the window); read by the M7 into asm 177 v2. */
#define RIOP_DIAG_FAULT_ADDR   0x204FDF40u
#define RIOP_DIAG_FAULT_MAGIC  0x4E313046u /* 'N10F' */
#define RIOP_FAULT_HARD    1u
#define RIOP_FAULT_MEM     2u
#define RIOP_FAULT_BUS     3u
#define RIOP_FAULT_USAGE   4u
#define RIOP_FAULT_SECURE  5u

typedef struct {
	uint32_t magic;
	uint32_t count;      /* faults since this record was last valid (power-off clears) */
	uint32_t type;       /* RIOP_FAULT_* */
	uint32_t cfsr;
	uint32_t hfsr;
	uint32_t bfar;
	uint32_t mmfar;
	uint32_t sfsr;
	uint32_t sfar;
	uint32_t pc;
	uint32_t lr;
	uint32_t xpsr;
	uint32_t exc_return;
	uint32_t uptime_ms;
	uint32_t reserved0;
	uint32_t reserved1;
} riop_diag_fault_t;

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
_Static_assert(sizeof(riop_diag_fault_t) == 64, "riop_diag_fault_t must be 64 bytes");
#endif

/* N10w: M33 RTWDOG1 health watchdog (KEEP). Compile switch shared by the
 * M33 files; the M7 only reads the record. */
#ifndef RIOP_M33_WDOG
#define RIOP_M33_WDOG 1
#endif

/* Diag poller sequence (riop_diag_m33.c); 0 until the poller publishes. */
extern volatile uint32_t g_riop_diag_poll_seq;

/* N10w: watchdog record, 80 B at window +0x80 (after the N10F record).
 * N10w2: armed from the health task, not pre-OS (see riop_wdog_m33.c).
 * Written by the M33 at boot (summary of the previous boot) and every
 * 250 ms by the health task (breadcrumb); the RTWDOG1 IRQ adds PC/LR/task.
 * Survives the WDOG system reset like the fault record; re-initialised on
 * POR. The M7 copies the summary into asm 177 v3. */
#define RIOP_DIAG_WDOG_ADDR    0x204FDF80u
#define RIOP_DIAG_WDOG_LEN     80u
#define RIOP_DIAG_WDOG_MAGIC   0x4E313057u /* 'N10W' */

/* last_reason */
#define RIOP_WDOG_R_NONE        0u
#define RIOP_WDOG_R_RUN_ISR     1u  /* health checks stopped the kicks; IRQ ran (PC captured) */
#define RIOP_WDOG_R_RUN_NOISR   2u  /* no IRQ either: IRQs masked or the bus/core stalled */
#define RIOP_WDOG_R_BOOT_ISR    3u  /* init not complete within the boot timeout; IRQ ran */
#define RIOP_WDOG_R_BOOT_NOISR  4u  /* boot timeout and no IRQ */
#define RIOP_WDOG_R_UNKNOWN     5u  /* WDOG reset but the record was not retained */

/* state (this boot) */
#define RIOP_WDOG_S_EN          (1u << 0)  /* armed */
#define RIOP_WDOG_S_RUN         (1u << 1)  /* init complete, 4 s run timeout active */
#define RIOP_WDOG_S_SAFE        (1u << 2)  /* boot-loop guard: safe mode (diag poller off) */
#define RIOP_WDOG_S_CLK_BAD     (1u << 3)  /* clock check failed: NOT armed */
#define RIOP_WDOG_S_REARM       (1u << 4)  /* EN was found cleared and re-armed */
#define RIOP_WDOG_S_TEST        (1u << 5)  /* N10wt stall-test build */
#define RIOP_WDOG_S_WDOG_BOOT   (1u << 6)  /* this boot was caused by a watchdog (any RTWDOG1..5) */
#define RIOP_WDOG_S_FRESH       (1u << 7)  /* record re-initialised (POR or not retained) */

/* health flags (breadcrumb) */
#define RIOP_WDOG_H_GOAL_OK      (1u << 0)
#define RIOP_WDOG_H_POLL_OK      (1u << 1)
#define RIOP_WDOG_H_NETIF_OK     (1u << 2)
#define RIOP_WDOG_H_INIT_DONE    (1u << 3)
#define RIOP_WDOG_H_RUN          (1u << 4)
#define RIOP_WDOG_H_SAFE         (1u << 5)
#define RIOP_WDOG_H_POLL_ACTIVE  (1u << 6)
#define RIOP_WDOG_H_GOAL_HOOK    (1u << 7)
#define RIOP_WDOG_H_KICKED       (1u << 8)
#define RIOP_WDOG_H_HOOK_MISSING (1u << 9)
#define RIOP_WDOG_H_ISR          (1u << 15)

typedef struct {
	uint32_t magic;          /*  0 */
	uint32_t boot_srsr;      /*  4: raw SRSR of this boot (cleared after reading) */
	uint16_t wdog_count;     /*  8: watchdog resets (RTWDOG1..5) since POR */
	uint8_t  consec;         /* 10: consecutive quick WDOG resets (loop guard) */
	uint8_t  last_reason;    /* 11: RIOP_WDOG_R_* */
	uint16_t state;          /* 12: RIOP_WDOG_S_* */
	uint8_t  test_stage;     /* 14: N10wt progress (0..3) */
	uint8_t  reserved0;      /* 15 */
	uint16_t clk_hz;         /* 16: measured RTWDOG tick (expect ~128) */
	uint16_t reserved1;      /* 18 */
	uint32_t last_uptime_ms; /* 20: previous boot, last breadcrumb */
	uint32_t last_health;    /* 24 */
	uint32_t last_pc;        /* 28: IRQ-captured PC (0 = IRQ did not run) */
	uint32_t last_lr;        /* 32 */
	char     last_task[8];   /* 36 */
	uint32_t cur_uptime_ms;  /* 44: this boot breadcrumb */
	uint32_t cur_health;     /* 48 */
	uint32_t cur_pc;         /* 52 */
	uint32_t cur_lr;         /* 56 */
	char     cur_task[8];    /* 60 */
	uint32_t srsr_hist;      /* 68: N10w2: OR of every boot's SRSR since POR */
	uint16_t boot_seq;       /* 72: N10w2: boots since POR (1 = first) */
	uint16_t arm_s;          /* 74: N10w2: uptime (s) when RTWDOG1 was armed, 0 = not armed */
	uint32_t srmask_boot;    /* 76: N10w2: SRC SRMASK as found before arming */
} riop_diag_wdog_t;

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
_Static_assert(sizeof(riop_diag_wdog_t) == RIOP_DIAG_WDOG_LEN, "riop_diag_wdog_t must be 80 bytes");
_Static_assert((RIOP_DIAG_WDOG_ADDR + RIOP_DIAG_WDOG_LEN) <= (RIOP_DIAG_NET_ADDR + 0x100u), "wdog record must stay inside the 256 B diag window");
#endif

#ifdef __cplusplus
}
#endif

#endif /* RIOP_DIAG_NET_H */
