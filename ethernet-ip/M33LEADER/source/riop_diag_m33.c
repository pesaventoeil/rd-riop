/* N10g: M33-side network diag poller + fault recorder for asm 177.
 *
 * RIOP B2 runs GOAL with ONLY the ENETC0 endpoint (GOAL_BOARD_ETH_PORT_4=1,
 * kNETC_ENETC0EthPort, RGMII, PHY 3). The NETC switch (SW0 ports 0-3) and
 * ENETC1 are never initialised by GOAL on this board. N10/N10f polled the
 * switch-port MACs and ENETC1 SIs; that access killed the board. This file
 * now reads only registers of the block GOAL itself brought up:
 *   - ENETC0_SI0 traffic counters (32-bit, read-only, no side effects)
 *   - optional (RIOP_DIAG_M33_MAC_COUNTERS=1): ENETC0 port MAC PM0 counters,
 *     low 32-bit word only (explicit 32-bit loads, never LDRD on a 64-bit
 *     counter)
 * and only after GOAL has the netif up with link.
 *
 * Fault recorder (KEEP): HardFault/MemManage/BusFault/UsageFault/SecureFault
 * write CFSR/HFSR/BFAR/MMFAR/SFSR/SFAR/PC/LR/xPSR into a magic-tagged 64 B
 * record at RIOP_DIAG_FAULT_ADDR (OCRAM1, non-cacheable on the M33, not
 * cleared by either core's startup), then NVIC_SystemReset(). The M7 copies
 * the record into asm 177 v2 on the next boot. After 2 recorded faults the
 * poller stays off until power is removed.
 *
 * Does not write NAFE or DCE config. Does not touch EMDIO, the switch, or
 * ENETC1. OCRAM1 is non-cacheable on the M33 (CACHE_MODE_WRITE_THROUGH MPU
 * workaround in board.c), so no cache maintenance is needed here.
 */
#include "riop_diag.h"
#include "riop_diag_net.h"

#include "fsl_device_registers.h"

#include "FreeRTOS.h"
#include "task.h"

#include <string.h>

#if defined(__has_include)
#if __has_include("lwip/netif.h")
#include "lwip/netif.h"
#define RIOP_DIAG_HAS_NETIF 1
#endif
#endif
#ifndef RIOP_DIAG_HAS_NETIF
#define RIOP_DIAG_HAS_NETIF 0
#endif

#ifndef RIOP_DIAG_M33_MAC_COUNTERS
#define RIOP_DIAG_M33_MAC_COUNTERS 0
#endif

#define RIOP_DIAG_POLL_MS      200u
#define RIOP_DIAG_START_MIN_S  20u
#define RIOP_DIAG_FAULT_STOP   2u   /* poller stays off after this many recorded faults */

static volatile riop_diag_net_t g_riop_diag_net
	__attribute__((section(".noinit.$riop_diag_net"), used));

static uint32_t s_seq;
static uint8_t  s_last_link;
static uint16_t s_link_changes;

static uint32_t diag_now_ms(void)
{
	return (uint32_t)((xTaskGetTickCount() * 1000u) / configTICK_RATE_HZ);
}

/* ---------------- fault recorder ---------------- */

#define RIOP_SCB_SFSR (*(volatile uint32_t *)0xE000EDE4u)
#define RIOP_SCB_SFAR (*(volatile uint32_t *)0xE000EDE8u)

static volatile riop_diag_fault_t *diag_fault_rec(void)
{
	return (volatile riop_diag_fault_t *)RIOP_DIAG_FAULT_ADDR;
}

static int diag_fault_valid(void)
{
	volatile riop_diag_fault_t *f = diag_fault_rec();
	return (f->magic == RIOP_DIAG_FAULT_MAGIC) && (f->count < 0x10000u);
}

/* Called from the naked fault stubs in semihost_hardfault.c.
 * frame = stacked R0..xPSR, excRet = EXC_RETURN, type = RIOP_FAULT_* */
__attribute__((noreturn, used))
void riop_fault_entry(uint32_t *frame, uint32_t excRet, uint32_t type)
{
	volatile riop_diag_fault_t *f = diag_fault_rec();
	uint32_t cnt = diag_fault_valid() ? (f->count + 1u) : 1u;

	f->magic      = 0u;          /* invalidate while writing */
	f->count      = cnt;
	f->type       = type;
	f->cfsr       = SCB->CFSR;
	f->hfsr       = SCB->HFSR;
	f->bfar       = SCB->BFAR;
	f->mmfar      = SCB->MMFAR;
	f->sfsr       = RIOP_SCB_SFSR;
	f->sfar       = RIOP_SCB_SFAR;
	f->exc_return = excRet;
	f->uptime_ms  = diag_now_ms();
	if (frame != NULL) {
		f->lr   = frame[5];
		f->pc   = frame[6];
		f->xpsr = frame[7];
	} else {
		f->lr = f->pc = f->xpsr = 0u;
	}
	f->reserved0 = 0u;
	__DSB();
	f->magic = RIOP_DIAG_FAULT_MAGIC;
	__DSB();
	NVIC_SystemReset();
	for (;;) {
	}
}

void riop_fault_init(void)
{
	/* Route MemManage/Bus/Usage/Secure faults to their own vectors so the
	 * recorder sees the precise cause instead of an escalated HardFault. */
	SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk | SCB_SHCSR_BUSFAULTENA_Msk |
		      SCB_SHCSR_USGFAULTENA_Msk | SCB_SHCSR_SECUREFAULTENA_Msk;
	__DSB();
	__ISB();
}

/* ---------------- network poller ---------------- */

static void diag_publish(const riop_diag_net_t *img)
{
	volatile uint32_t *dst = (volatile uint32_t *)RIOP_DIAG_NET_ADDR;
	const uint32_t *src = (const uint32_t *)(const void *)img;
	uint32_t i;

	dst[0] = 0u; /* magic off while the body changes */
	__DSB();
	for (i = 1u; i < (sizeof(*img) / 4u); i++) {
		dst[i] = src[i];
	}
	__DSB();
	dst[0] = src[0];
	__DSB();
}

static uint8_t diag_link_up(void)
{
#if RIOP_DIAG_HAS_NETIF
	struct netif *nif = netif_default;
	return ((nif != NULL) && netif_is_up(nif) && netif_is_link_up(nif)) ? 1u : 0u;
#else
	return 0u;
#endif
}

static uint32_t rd32(const volatile void *reg)
{
	return *(const volatile uint32_t *)reg; /* low word only, single 32-bit load */
}

static void diag_poll_once(void)
{
	riop_diag_net_t img;
	uint8_t link = diag_link_up();

	memset(&img, 0, sizeof img);

	/* ENETC0 PSI0: the SI GOAL configured (epConfig.si = kNETC_ENETC0PSI0). */
	img.rx_frames = rd32(&ENETC0_SI0->SIRFRM0);
	img.tx_frames = rd32(&ENETC0_SI0->SITFRM0);
#if RIOP_DIAG_M33_MAC_COUNTERS
	/* ENETC0 port MAC (RGMII to PHY 3), brought up by GOAL. */
	img.rx_errors    = rd32(&ENETC0_ETH_MAC_PORT->PM0_RERRN);
	img.tx_errors    = rd32(&ENETC0_ETH_MAC_PORT->PM0_TERRN);
	img.dma_overflow = rd32(&ENETC0_ETH_MAC_PORT->PM0_RDRPN);
	img.flags |= RIOP_DIAG_F_MAC_CNT;
#endif

	if (link != s_last_link) {
		if (s_link_changes < 0xFFFFu) {
			s_link_changes++;
		}
		s_last_link = link;
	}
	img.phy_link_mask = link;          /* bit0 = ENETC0 endpoint link */
	img.link_change_count = s_link_changes;
	if (link != 0u) {
		img.flags |= RIOP_DIAG_F_PHY_LINK;
	}
	img.enet_reinit_count = 0u;        /* recover path removed in N10g */

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
	s_seq = 0u;
	s_last_link = 0u;
	s_link_changes = 0u;

	/* No NETC access until GOAL has the netif up with link and >=20 s uptime.
	 * Unlike N10f there is no time fallback: no link, no register reads. */
	for (;;) {
		if ((diag_now_ms() / 1000u >= RIOP_DIAG_START_MIN_S) && (diag_link_up() != 0u)) {
			break;
		}
		vTaskDelay(pdMS_TO_TICKS(1000u));
	}
	for (;;) {
		diag_poll_once();
		vTaskDelay(pdMS_TO_TICKS(RIOP_DIAG_POLL_MS));
	}
}

void riop_diag_m33_start(void)
{
	/* Clear the snapshot (not the fault record) so the M7 never copies a
	 * stale image from before this boot. */
	((volatile uint32_t *)RIOP_DIAG_NET_ADDR)[0] = 0u;
	if (diag_fault_valid() && (diag_fault_rec()->count >= RIOP_DIAG_FAULT_STOP)) {
		return; /* repeated faults: keep the poller off until power-off */
	}
	(void)xTaskCreate(diag_m33_task, "riop_diag", 768, NULL,
			  tskIDLE_PRIORITY + 1, NULL);
}
