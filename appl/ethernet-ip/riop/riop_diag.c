/* N10: RIOP Ethernet / uptime diagnostics (M7 assembler for asm 177).
 * Local: uptime, boot_count, reset_cause, EIP session, last CIP, NAFE scan.
 * NETC / PHY / lwIP / recover live on the M33 (see riop_diag_m33.c) and are
 * copied from the reserved OCRAM1 snapshot. This file never includes the
 * NETC driver (not on the M7 image) and never writes NAFE or DCE config.
 */
#include "riop_diag.h"
#include "riop_diag_net.h"

#include "board.h"
#include "fsl_cache.h"
#include "fsl_device_registers.h"

#include "FreeRTOS.h"
#include "task.h"

#include <string.h>

#if defined(__has_include)
#if __has_include("nafe_temp_task.h")
#include "nafe_temp_task.h"
#define RIOP_DIAG_HAS_NAFE 1
#endif
#endif
#ifndef RIOP_DIAG_HAS_NAFE
#define RIOP_DIAG_HAS_NAFE 0
#endif

#define RIOP_DIAG_POLL_MS    200u
#define RIOP_DIAG_BOOT_MAGIC 0x4E313042u /* 'N10B' */

/* Survives a warm reset if the linker keeps .noinit; start at 1 after POR. */
static uint32_t s_boot_magic __attribute__((section(".noinit"), used));
static uint32_t s_boot_count __attribute__((section(".noinit"), used));

static riop_diag_t s_img;
static uint32_t s_last_poll_ms;
static uint32_t s_last_rx_frames;
static uint32_t s_last_tx_frames;
static uint32_t s_last_net_seq;
static uint8_t s_inited;

static uint32_t diag_now_ms(void)
{
	return (uint32_t)((xTaskGetTickCount() * 1000u) / configTICK_RATE_HZ);
}

static void diag_init_boot(void)
{
	if (s_boot_magic != RIOP_DIAG_BOOT_MAGIC) {
		s_boot_magic = RIOP_DIAG_BOOT_MAGIC;
		s_boot_count = 1u;
	} else if (s_boot_count < 0xFFFFFFFFu) {
		s_boot_count++;
	}
	s_img.boot_count = s_boot_count;
	s_img.reset_cause = SRC_GENERAL_REG->SRSR;
}

static int diag_copy_m33(uint32_t now_ms)
{
	riop_diag_net_t net;
	volatile riop_diag_net_t *src = (volatile riop_diag_net_t *)RIOP_DIAG_NET_ADDR;

	DCACHE_InvalidateByRange(RIOP_DIAG_NET_ADDR, sizeof(net));
	net = *src;
	if (net.magic != RIOP_DIAG_NET_MAGIC) {
		return 0;
	}

	s_img.rx_frames = net.rx_frames;
	s_img.tx_frames = net.tx_frames;
	s_img.rx_errors = net.rx_errors;
	s_img.tx_errors = net.tx_errors;
	s_img.dma_stall = net.dma_stall;
	s_img.dma_overflow = net.dma_overflow;
	s_img.lwip_pbuf_err = net.lwip_pbuf_err;
	s_img.lwip_mem_err = net.lwip_mem_err;
	s_img.enet_reinit_count = net.enet_reinit_count;
	s_img.phy_link_mask = net.phy_link_mask;
	s_img.link_change_count = net.link_change_count;

	/* Stamp last RX/TX ticks in M7 time so CIP Get and recover age agree. */
	if (net.rx_frames != s_last_rx_frames) {
		s_img.last_rx_tick_ms = now_ms;
		s_last_rx_frames = net.rx_frames;
	}
	if (net.tx_frames != s_last_tx_frames) {
		s_img.last_tx_tick_ms = now_ms;
		s_last_tx_frames = net.tx_frames;
	}

	/* Merge M33 flags; M7 also ORs RECOVER_* from the snapshot. */
	s_img.flags |= (uint16_t)(net.flags & (RIOP_DIAG_F_LWIP_STATS |
					       RIOP_DIAG_F_RECOVER_EN |
					       RIOP_DIAG_F_PHY_LINK |
					       RIOP_DIAG_F_RECOVERED |
					       RIOP_DIAG_F_MDIO_OK |
					       RIOP_DIAG_F_MAC_CNT));
	s_last_net_seq = net.seq;
	return 1;
}

/* N10g: last M33 fault record (survives the M33's NVIC_SystemReset). */
static void diag_copy_fault(void)
{
	riop_diag_fault_t f;
	volatile riop_diag_fault_t *src = (volatile riop_diag_fault_t *)RIOP_DIAG_FAULT_ADDR;

	DCACHE_InvalidateByRange(RIOP_DIAG_FAULT_ADDR, sizeof(f));
	f = *src;
	if ((f.magic != RIOP_DIAG_FAULT_MAGIC) || (f.count == 0u) || (f.count >= 0x10000u)) {
		return;
	}
	s_img.fault_count = f.count;
	s_img.fault_cfsr = f.cfsr;
	s_img.fault_pc = f.pc;
	s_img.fault_bfar = f.bfar;
	s_img.flags |= RIOP_DIAG_F_FAULT_SEEN;
}

void riop_diag_init(void)
{
	if (s_inited) {
		return;
	}
	memset(&s_img, 0, sizeof s_img);
	s_img.version = RIOP_DIAG_VERSION;
	diag_init_boot();
	s_last_poll_ms = 0u;
	s_last_rx_frames = 0u;
	s_last_tx_frames = 0u;
	s_last_net_seq = 0u;
	s_inited = 1u;
}

void riop_diag_note_cip(void)
{
	if (!s_inited) {
		riop_diag_init();
	}
	s_img.last_cip_tick_ms = diag_now_ms();
}

void riop_diag_note_session_opened(void)
{
	if (!s_inited) {
		riop_diag_init();
	}
	if (s_img.eip_session_count < 0xFFFFFFFFu) {
		s_img.eip_session_count++;
	}
	s_img.last_cip_tick_ms = diag_now_ms();
}

void riop_diag_poll(void)
{
	uint32_t now_ms;

	if (!s_inited) {
		riop_diag_init();
	}
	now_ms = diag_now_ms();
	if ((s_last_poll_ms != 0u) && ((now_ms - s_last_poll_ms) < RIOP_DIAG_POLL_MS)) {
		return;
	}
	s_last_poll_ms = now_ms;

	s_img.uptime_s = now_ms / 1000u;
	/* Keep session/CIP/boot; refresh network flags from the M33 snapshot. */
	s_img.flags = 0u;
#if RIOP_ENET_RX_RECOVER
	/* Recover itself runs on the M33; this bit still advertises it is compiled in. */
	s_img.flags |= RIOP_DIAG_F_RECOVER_EN;
#endif

	(void)diag_copy_m33(now_ms);
	diag_copy_fault();

#if RIOP_DIAG_HAS_NAFE
	s_img.nafe_scan_count = nafe_tempGetScanCount();
#endif
}

void riop_diag_get(uint8_t *buf, uint32_t len)
{
	uint32_t n;

	if ((buf == NULL) || (len == 0u)) {
		return;
	}
	if (!s_inited) {
		riop_diag_init();
	}
	s_img.uptime_s = diag_now_ms() / 1000u;
#if RIOP_DIAG_HAS_NAFE
	s_img.nafe_scan_count = nafe_tempGetScanCount();
#endif
	n = (len < RIOP_DIAG_LEN) ? len : RIOP_DIAG_LEN;
	memcpy(buf, &s_img, n);
	if (len > n) {
		memset(buf + n, 0, len - n);
	}
	riop_diag_note_cip();
}
