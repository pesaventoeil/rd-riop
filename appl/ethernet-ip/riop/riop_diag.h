/* N10: read-only RIOP Ethernet / uptime diagnostics (asm 177).
 * Little-endian packed image. Does not write NAFE or DCE config.
 */
#ifndef RIOP_DIAG_H
#define RIOP_DIAG_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RIOP_DIAG_VERSION          3u
#define RIOP_DIAG_LEN              112u /* v3 (N10w): +16 B watchdog fields; first 96 B = v2 */

/* Compile-time self-recovery (implemented on the M33, which owns NETC
 * and lwIP): if a switch port has seen RX and then no RX arrives for
 * RIOP_ENET_RX_RECOVER_SEC, bounce the lwIP netif and graceful-stop
 * the switch-port MACs. Does not touch NAFE / SET_CH / EMDIO. Set to
 * 0 to compile the path out.
 *
 * fix/n10-boot: default 0. NETC_PortEthMacGracefulStop() has no restart
 * call here, so one trigger leaves every switch MAC stopped for good, and
 * netif_set_down/up are called from a non-tcpip task (lwIP is not thread
 * safe). Do not set to 1 until it re-enables the MACs and posts the netif
 * bounce through tcpip_callback(). */
#ifndef RIOP_ENET_RX_RECOVER
#define RIOP_ENET_RX_RECOVER       0
#endif
#ifndef RIOP_ENET_RX_RECOVER_SEC
#define RIOP_ENET_RX_RECOVER_SEC    5u
#endif
#ifndef RIOP_ENET_RX_RECOVER_GRACE_SEC
#define RIOP_ENET_RX_RECOVER_GRACE_SEC 15u
#endif
#ifndef RIOP_ENET_RX_RECOVER_COOLDOWN_SEC
#define RIOP_ENET_RX_RECOVER_COOLDOWN_SEC 30u
#endif

/* flags */
#define RIOP_DIAG_F_LWIP_STATS     (1u << 0)
#define RIOP_DIAG_F_RECOVER_EN     (1u << 1)
#define RIOP_DIAG_F_PHY_LINK       (1u << 2)
#define RIOP_DIAG_F_RECOVERED      (1u << 3)
#define RIOP_DIAG_F_MDIO_OK        (1u << 4)
#define RIOP_DIAG_F_FAULT_SEEN     (1u << 5)  /* N10g: an M33 fault record is present */
#define RIOP_DIAG_F_MAC_CNT        (1u << 6)  /* N10g: rx/tx_errors + dma_overflow come from the ENETC0 MAC */
#define RIOP_DIAG_F_WDOG_EN        (1u << 7)  /* N10w: M33 RTWDOG1 armed */
#define RIOP_DIAG_F_WDOG_BOOT      (1u << 8)  /* N10w: this boot was caused by the watchdog */
#define RIOP_DIAG_F_SAFE_MODE      (1u << 9)  /* N10w: boot-loop guard tripped, diag poller off */

typedef struct {
	uint16_t version;            /* 0: RIOP_DIAG_VERSION */
	uint16_t flags;              /* 2 */
	uint32_t uptime_s;           /* 4 */
	uint32_t boot_count;         /* 8 */
	uint32_t reset_cause;        /* 12: raw SRC_GENERAL SRSR */
	uint32_t rx_frames;          /* 16 */
	uint32_t tx_frames;          /* 20 */
	uint32_t rx_errors;          /* 24 */
	uint32_t tx_errors;          /* 28 */
	uint32_t dma_stall;          /* 32: SI lack of Rx BDs */
	uint32_t dma_overflow;       /* 36: MAC rxDiscard */
	uint32_t last_rx_tick_ms;    /* 40 */
	uint32_t last_tx_tick_ms;    /* 44 */
	uint8_t  phy_link_mask;      /* 48: N10g bit0 = ENETC0 endpoint link (RIOP has no switch) */
	uint8_t  reserved0;          /* 49 */
	uint16_t link_change_count;  /* 50 */
	uint32_t lwip_pbuf_err;      /* 52 */
	uint32_t lwip_mem_err;       /* 56 */
	uint32_t eip_session_count;  /* 60: cumulative I/O Forward_Open */
	uint32_t last_cip_tick_ms;   /* 64 */
	uint32_t nafe_scan_count;    /* 68 */
	uint32_t enet_reinit_count;  /* 72 */
	uint32_t reserved1;          /* 76 */
	uint32_t fault_count;        /* 80: v2 M33 fault record count (0 = none) */
	uint32_t fault_cfsr;         /* 84 */
	uint32_t fault_pc;           /* 88 */
	uint32_t fault_bfar;         /* 92 */
	uint16_t wdog_reset_count;   /* 96: v3 WDOG1 resets since POR */
	uint8_t  wdog_last_reason;   /* 98: RIOP_WDOG_R_* of the last WDOG reset */
	uint8_t  wdog_consec;        /* 99: loop-guard count */
	uint16_t wdog_state;         /* 100: RIOP_WDOG_S_* (this boot) */
	uint8_t  wdog_test_stage;    /* 102: N10wt progress */
	uint8_t  reserved2;          /* 103 */
	uint32_t wdog_last_pc;       /* 104: PC at the watchdog IRQ (0 = IRQ did not run) */
	uint32_t wdog_last_uptime_ms;/* 108: M33 uptime at the last breadcrumb before the reset */
} riop_diag_t;

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
_Static_assert(sizeof(riop_diag_t) == RIOP_DIAG_LEN, "riop_diag_t must be RIOP_DIAG_LEN bytes");
#endif

void riop_diag_init(void);
void riop_diag_poll(void);
void riop_diag_note_cip(void);
void riop_diag_note_session_opened(void);
void riop_diag_get(uint8_t *buf, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif /* RIOP_DIAG_H */
