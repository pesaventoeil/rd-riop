/* N10: M33-side NETC / PHY / lwIP poller and RX-stall recover.
 * Publishes a snapshot at RIOP_DIAG_NET_ADDR for the M7 asm-177 assembler.
 * Does not write NAFE or DCE config. Does not re-init EMDIO (GOAL already owns it).
 */
#include "riop_diag.h"
#include "riop_diag_net.h"

#include "fsl_cache.h"
#include "fsl_device_registers.h"
#include "fsl_netc.h"
#include "netc_hw/fsl_netc_hw_port.h"
#include "netc_hw/fsl_netc_hw_si.h"

#include "FreeRTOS.h"
#include "task.h"

#include <string.h>

#if defined(__has_include)
#if __has_include("lwip/stats.h")
#include "lwip/opt.h"
#if LWIP_STATS
#include "lwip/stats.h"
#define RIOP_DIAG_HAS_LWIP_STATS 1
#endif
#endif
#if __has_include("lwip/netif.h")
#include "lwip/netif.h"
#define RIOP_DIAG_HAS_NETIF 1
#endif
#endif
#ifndef RIOP_DIAG_HAS_LWIP_STATS
#define RIOP_DIAG_HAS_LWIP_STATS 0
#endif
#ifndef RIOP_DIAG_HAS_NETIF
#define RIOP_DIAG_HAS_NETIF 0
#endif

#define RIOP_DIAG_POLL_MS   200u
#define RIOP_DIAG_NPORTS    4u
#define RIOP_DIAG_NSIS      3u

static NETC_ETH_LINK_Type *const k_ports[RIOP_DIAG_NPORTS] = {
	SW0_ETH_MAC_PORT0, SW0_ETH_MAC_PORT1, SW0_ETH_MAC_PORT2, SW0_ETH_MAC_PORT3
};

static ENETC_SI_Type *const k_sis[RIOP_DIAG_NSIS] = {
	ENETC0_SI0, ENETC1_SI0, ENETC1_SI1
};

#if defined(SW0_PORT0)
static NETC_PORT_Type *const k_sw_ports[RIOP_DIAG_NPORTS] = {
	SW0_PORT0, SW0_PORT1, SW0_PORT2, SW0_PORT3
};
#define RIOP_DIAG_HAS_SW_PORT 1
#else
#define RIOP_DIAG_HAS_SW_PORT 0
#endif

static volatile riop_diag_net_t g_riop_diag_net
	__attribute__((section(".noinit.$riop_diag_net"), used));

static uint32_t s_last_rx;
static uint32_t s_last_tx;
static uint32_t s_last_rx_ms;
static uint32_t s_last_tx_ms;
static uint8_t  s_last_link_mask;
static uint32_t s_last_reinit_s;
static uint32_t s_seq;
static uint32_t s_reinit_count;

static uint32_t diag_now_ms(void)
{
	return (uint32_t)((xTaskGetTickCount() * 1000u) / configTICK_RATE_HZ);
}

static void diag_publish(const riop_diag_net_t *img)
{
	riop_diag_net_t *dst = (riop_diag_net_t *)(void *)&g_riop_diag_net;

	/* If the linker placed the section at the reserved window, use it;
	 * otherwise fall back to the hardcoded address. */
	if ((uintptr_t)dst != (uintptr_t)RIOP_DIAG_NET_ADDR) {
		dst = (riop_diag_net_t *)RIOP_DIAG_NET_ADDR;
	}
	*dst = *img;
	DCACHE_CleanByRange((uint32_t)(uintptr_t)dst, sizeof(*dst));
}

static void diag_read_netc(uint32_t now_ms, riop_diag_net_t *img)
{
	uint32_t i;
	uint32_t rx = 0u, tx = 0u, rxerr = 0u, txerr = 0u, disc = 0u, stall = 0u;
	uint8_t mask = 0u;

	for (i = 0; i < RIOP_DIAG_NPORTS; i++) {
		netc_port_phy_mac_traffic_statistic_t rxs, txs;
		netc_port_phy_mac_discard_statistic_t d;

		memset(&rxs, 0, sizeof rxs);
		memset(&txs, 0, sizeof txs);
		memset(&d, 0, sizeof d);
		NETC_PortGetPhyMacRxStatistic(k_ports[i], kNETC_ExpressMAC, &rxs);
		NETC_PortGetPhyMacTxStatistic(k_ports[i], kNETC_ExpressMAC, &txs);
		NETC_PortGetPhyMacDiscardStatistic(k_ports[i], kNETC_ExpressMAC, &d);
		rx += (uint32_t)rxs.validFrame;
		tx += (uint32_t)txs.validFrame;
		rxerr += (uint32_t)d.rxError;
		txerr += (uint32_t)d.txErrorFCS;
		disc += (uint32_t)d.rxDiscard;
		/* MDIO is owned by GOAL on this core; do not open a second
		 * EMDIO handle. Link is "port has seen a good RX frame". */
		if (rxs.validFrame != 0u) {
			mask |= (uint8_t)(1u << i);
		}
	}

	for (i = 0; i < RIOP_DIAG_NSIS; i++) {
		netc_si_traffic_statistic_t st;
		netc_si_discard_statistic_t sd;
		uint32_t r;
		uint32_t nbd;

		memset(&st, 0, sizeof st);
		memset(&sd, 0, sizeof sd);
		NETC_SIGetTrafficStatistic(k_sis[i], &st);
		NETC_SIGetDiscardStatistic(k_sis[i], &sd);
		rx += (uint32_t)st.rxFrame;
		tx += (uint32_t)st.txFrame;
		nbd = (uint32_t)(sizeof(sd.lackBD) / sizeof(sd.lackBD[0]));
		for (r = 0; r < nbd; r++) {
			stall += sd.lackBD[r];
		}
		stall += sd.programError + sd.busError;
	}

	if (rx != s_last_rx) {
		s_last_rx_ms = now_ms;
		s_last_rx = rx;
	}
	if (tx != s_last_tx) {
		s_last_tx_ms = now_ms;
		s_last_tx = tx;
	}
	if (mask != s_last_link_mask) {
		if (img->link_change_count < 0xFFFFu) {
			img->link_change_count++;
		}
		s_last_link_mask = mask;
	}

	img->rx_frames = rx;
	img->tx_frames = tx;
	img->rx_errors = rxerr;
	img->tx_errors = txerr;
	img->dma_stall = stall;
	img->dma_overflow = disc;
	img->phy_link_mask = mask;
}

#if RIOP_ENET_RX_RECOVER
static void diag_bounce_mac_and_netif(void)
{
#if RIOP_DIAG_HAS_SW_PORT
	{
		uint32_t i;
		for (i = 0; i < RIOP_DIAG_NPORTS; i++) {
			NETC_PortEthMacGracefulStop(k_sw_ports[i]);
		}
		for (i = 0; i < 10000u; i++) {
			__NOP();
		}
	}
#endif
#if RIOP_DIAG_HAS_NETIF
	if (netif_default != NULL) {
		netif_set_link_down(netif_default);
		netif_set_down(netif_default);
		netif_set_up(netif_default);
		netif_set_link_up(netif_default);
	}
#endif
}

static void diag_maybe_recover(uint32_t now_ms, const riop_diag_net_t *img)
{
	uint32_t uptime_s = now_ms / 1000u;
	uint32_t quiet_ms;

	if (img->phy_link_mask == 0u) {
		return;
	}
	if (uptime_s < RIOP_ENET_RX_RECOVER_GRACE_SEC) {
		return;
	}
	if ((s_last_reinit_s != 0u) &&
	    ((uptime_s - s_last_reinit_s) < RIOP_ENET_RX_RECOVER_COOLDOWN_SEC)) {
		return;
	}
	if (s_last_rx_ms == 0u) {
		quiet_ms = now_ms;
	} else if (now_ms >= s_last_rx_ms) {
		quiet_ms = now_ms - s_last_rx_ms;
	} else {
		return;
	}
	if (quiet_ms < (RIOP_ENET_RX_RECOVER_SEC * 1000u)) {
		return;
	}
	diag_bounce_mac_and_netif();
	if (s_reinit_count < 0xFFFFFFFFu) {
		s_reinit_count++;
	}
	s_last_reinit_s = uptime_s;
}
#endif

static void diag_poll_once(void)
{
	riop_diag_net_t img;
	uint32_t now_ms = diag_now_ms();

	memset(&img, 0, sizeof img);
	/* Preserve link_change_count across polls (it lives in the last published image). */
	{
		const riop_diag_net_t *cur = (const riop_diag_net_t *)(void *)&g_riop_diag_net;
		if (cur->magic == RIOP_DIAG_NET_MAGIC) {
			img.link_change_count = cur->link_change_count;
		}
	}

	diag_read_netc(now_ms, &img);

	img.flags = 0u;
#if RIOP_DIAG_HAS_LWIP_STATS
	img.flags |= RIOP_DIAG_F_LWIP_STATS;
	img.lwip_pbuf_err = (uint32_t)lwip_stats.memp[MEMP_PBUF].err +
			    (uint32_t)lwip_stats.memp[MEMP_PBUF_POOL].err;
	img.lwip_mem_err = (uint32_t)lwip_stats.mem.err;
#endif
#if RIOP_ENET_RX_RECOVER
	img.flags |= RIOP_DIAG_F_RECOVER_EN;
	diag_maybe_recover(now_ms, &img);
#endif
	if (img.phy_link_mask != 0u) {
		img.flags |= RIOP_DIAG_F_PHY_LINK;
	}
	img.enet_reinit_count = s_reinit_count;
	if (s_reinit_count != 0u) {
		img.flags |= RIOP_DIAG_F_RECOVERED;
	}

	if (s_seq < 0xFFFFFFFFu) {
		s_seq++;
	}
	img.seq = s_seq;
	img.magic = RIOP_DIAG_NET_MAGIC;
	diag_publish(&img);
}

static void diag_m33_task(void *arg)
{
	(void)arg;
	s_last_rx = 0u;
	s_last_tx = 0u;
	s_last_rx_ms = 0u;
	s_last_tx_ms = 0u;
	s_last_link_mask = 0u;
	s_last_reinit_s = 0u;
	s_seq = 0u;
	s_reinit_count = 0u;
	for (;;) {
		diag_poll_once();
		vTaskDelay(pdMS_TO_TICKS(RIOP_DIAG_POLL_MS));
	}
}

void riop_diag_m33_start(void)
{
	/* Created from BOARD_InitHardware before the scheduler starts. */
	(void)xTaskCreate(diag_m33_task, "riop_diag", 512, NULL,
			  tskIDLE_PRIORITY + 1, NULL);
}
