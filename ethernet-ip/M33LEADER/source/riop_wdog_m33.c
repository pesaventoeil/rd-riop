/* N10w: M33-owned RTWDOG1 health watchdog, 'N10W' record, boot-loop guard.
 *
 * Instance: RTWDOG1 (AONMIX, IRQ 38). NXP's RT1180 SDK pairs RTWDOG1 with
 * the CM33 and RTWDOG3 with the CM7. A WDOG1 timeout is a SRC global
 * *system* reset (SRMASK WDOG1_MASK = 0): both cores, NETC and the
 * M7 TCMs restart, the ROM boots the M33 from flash and the M33 reloads and
 * releases the M7 exactly as after power-on. Nothing on the M7 has to be
 * recovered separately.
 *
 * Conflicts handled:
 *  - The SDK SystemInit() of BOTH cores disables all five RTWDOGs when
 *    DISABLE_WDOG=1. The M33 one runs at reset, before we arm. The M7 one
 *    would run after we arm (the M33 releases the M7 later), so the M7 is
 *    built with -DDISABLE_WDOG=0 (M7FOLLOWER/CMakeLists.txt). The health
 *    task also re-arms RTWDOG1 if it ever finds EN cleared (state REARM).
 *  - Debugger: CS[DBG]=0, so the counter stops while the core is halted.
 *    Under ISP straps the ROM serial downloader runs instead of this image,
 *    so SWD flashing with LinkServer never sees an armed watchdog.
 *  - CS[UPDATE]=1 is kept, so a later image (or the ROM path above) can
 *    still reconfigure or disable it without a reset.
 *  - ERR051578 (NXP RT1180 rtwdog example): with CS[INT]=1 the first
 *    timeout only raises the IRQ; the SRC catches the backup reset one more
 *    TOVAL later. TOVAL is therefore half the reset time: run 2 s -> IRQ at
 *    2 s, reset at ~4 s; boot 15 s -> IRQ at 15 s, reset at ~30 s.
 *
 * Kicks: only from riop_wdog_task (tskIDLE_PRIORITY + 1, never an ISR),
 * and only when the GOAL main loop is progressing (it runs at FreeRTOS
 * priority 0, so any starvation shows up there first), the diag poller is
 * publishing (if it started) and the lwIP default netif is up. The ISR
 * never kicks; once it fires the task stops kicking, so the reset is
 * certain.
 *
 * Record: 64 B at RIOP_DIAG_WDOG_ADDR (OCRAM1 diag window +0x80, next to
 * the N10F fault record). The task writes a breadcrumb every 250 ms; the
 * ISR adds the interrupted PC/LR/task. On the next boot the M33 moves the
 * breadcrumb to last_* and the M7 copies it into asm 177 v3.
 *
 * Boot-loop guard (TEMP tuning): RIOP_WDOG_LOOP_N consecutive watchdog
 * resets, each within RIOP_WDOG_LOOP_WINDOW_S of boot, start the next boot
 * in safe mode: diag poller off, watchdog still on, state/flag SAFE.
 */
#include "riop_diag_net.h"

#include "fsl_device_registers.h"
#include "fsl_common.h"
#include "fsl_clock.h"

#include "FreeRTOS.h"
#include "task.h"

#if defined(__has_include)
#if __has_include("lwip/netif.h")
#include "lwip/netif.h"
#define RIOP_WDOG_HAS_NETIF 1
#endif
#endif
#ifndef RIOP_WDOG_HAS_NETIF
#define RIOP_WDOG_HAS_NETIF 0
#endif

/* GOAL hooks: always built so goal_appl.c links with RIOP_M33_WDOG=0. */
volatile uint32_t g_riop_goal_loop_cnt;
volatile uint8_t g_riop_goal_ready;

void riop_wdog_goal_loop(void)
{
	g_riop_goal_loop_cnt++;
}

void riop_wdog_note_goal_ready(void)
{
	g_riop_goal_ready = 1u;
}

#if RIOP_M33_WDOG

#ifndef RIOP_WDOG_BOOT_TOVAL_MS
#define RIOP_WDOG_BOOT_TOVAL_MS   15000u  /* IRQ 15 s, reset ~30 s */
#endif
#ifndef RIOP_WDOG_RUN_TOVAL_MS
#define RIOP_WDOG_RUN_TOVAL_MS    2000u   /* IRQ 2 s, reset ~4 s */
#endif
#define RIOP_WDOG_TASK_MS         250u
#define RIOP_WDOG_GOAL_MAX_MS     1000u
#define RIOP_WDOG_POLL_MAX_MS     1500u
#define RIOP_WDOG_NETIF_MAX_MS    3000u
#define RIOP_WDOG_SAFE_RUN_S      25u     /* safe mode: run phase after this even without GOAL ready */
#define RIOP_WDOG_HOOK_GRACE_MS   5000u   /* GOAL ready but loop hook silent -> HOOK_MISSING */

/* TEMP: boot-loop guard tuning (see DIAGNOSTICS_REGISTER.md). */
#ifndef RIOP_WDOG_LOOP_N
#define RIOP_WDOG_LOOP_N          3u
#endif
#ifndef RIOP_WDOG_LOOP_WINDOW_S
#define RIOP_WDOG_LOOP_WINDOW_S   600u
#endif

/* N10wt only: deliberate stalls after power-on, no write needed. */
#ifndef RIOP_WDOG_STALL_TEST
#define RIOP_WDOG_STALL_TEST      0
#endif
#ifndef RIOP_WDOG_TEST_AT_S
#define RIOP_WDOG_TEST_AT_S       60u
#endif

#define RIOP_WDOG_NOMINAL_HZ      128u    /* 32.768 kHz LPO / 256 */
#define RIOP_WDOG_UNLOCK_KEY      0xD928C520u
#define RIOP_WDOG_REFRESH_KEY     0xB480A602u
#define RIOP_WDOG_SPIN_MAX        200000u

static uint32_t s_tick_hz;
static uint32_t s_cs_base;
static uint32_t s_toval;
static uint8_t s_safe;
static volatile uint8_t s_run;
static volatile uint8_t s_isr_fired;    /* run-phase IRQ seen: stop kicking, reset is committed */
static volatile uint8_t s_boot_irq;     /* boot-phase IRQ seen (not committed) */
static volatile uint8_t s_task_alive;   /* scheduler running: pcTaskGetName() is safe in the ISR */

static volatile riop_diag_wdog_t *wrec(void)
{
	return (volatile riop_diag_wdog_t *)RIOP_DIAG_WDOG_ADDR;
}

static uint32_t wdog_ms(void)
{
	return (uint32_t)((xTaskGetTickCount() * 1000u) / configTICK_RATE_HZ);
}

static uint32_t ms_to_ticks(uint32_t ms)
{
	uint32_t t = (ms * s_tick_hz) / 1000u;
	if (t < 2u) {
		t = 2u;
	}
	if (t > 0xFFFFu) {
		t = 0xFFFFu;
	}
	return t;
}

/* Unlock + TOVAL + CS within one IRQ-off burst, then wait for RCS.
 * Returns 0 if the unlock or reconfiguration did not complete. */
static int wdog_write_cfg(uint32_t toval, uint32_t cs)
{
	uint32_t pm = DisableGlobalIRQ();
	uint32_t n;
	int ok = 0;

	RTWDOG1->CNT = RIOP_WDOG_UNLOCK_KEY;
	for (n = 0u; n < RIOP_WDOG_SPIN_MAX; n++) {
		if ((RTWDOG1->CS & RTWDOG_CS_ULK_MASK) != 0u) {
			break;
		}
	}
	if (n < RIOP_WDOG_SPIN_MAX) {
		RTWDOG1->WIN = 0u;
		RTWDOG1->TOVAL = toval;
		RTWDOG1->CS = cs;
		for (n = 0u; n < RIOP_WDOG_SPIN_MAX; n++) {
			if ((RTWDOG1->CS & RTWDOG_CS_RCS_MASK) != 0u) {
				ok = 1;
				break;
			}
		}
	}
	EnableGlobalIRQ(pm);
	return ok;
}

static void wdog_kick(void)
{
	RTWDOG1->CNT = RIOP_WDOG_REFRESH_KEY; /* CMD32EN: one 32-bit write */
}

/* Measure the LPO/256 tick against the core clock (100 ms busy wait,
 * pre-OS). Guards against a clock mux surprise turning 30 s into 30 ms. */
static uint32_t wdog_calibrate(void)
{
	uint32_t cs = RTWDOG_CS_CMD32EN_MASK | RTWDOG_CS_PRES_MASK | RTWDOG_CS_CLK(1u) |
		      RTWDOG_CS_UPDATE_MASK | RTWDOG_CS_WAIT_MASK | RTWDOG_CS_EN_MASK;
	uint32_t c0, c1;

	if (wdog_write_cfg(0xFFFFu, cs) == 0) {
		return 0u;
	}
	wdog_kick();
	c0 = RTWDOG1->CNT & 0xFFFFu;
	SDK_DelayAtLeastUs(100000u, SystemCoreClock);
	c1 = RTWDOG1->CNT & 0xFFFFu;
	return (c1 >= c0) ? ((c1 - c0) * 10u) : 0u;
}

static void wdog_state_set(uint16_t bits)
{
	wrec()->state = (uint16_t)(wrec()->state | bits);
}

static int wdog_arm(uint32_t toval_ms)
{
	s_toval = ms_to_ticks(toval_ms);
	if (wdog_write_cfg(s_toval, s_cs_base) == 0) {
		return 0;
	}
	wdog_kick();
	return 1;
}

/* Called by the naked RTWDOG1_IRQHandler with the stacked frame. Never
 * kicks; records where the core was and lets the backup reset happen. */
__attribute__((used))
void riop_wdog_isr(uint32_t *frame)
{
	volatile riop_diag_wdog_t *w = wrec();
	const char *nm;
	uint32_t i;

	/* Boot phase: only record; init may still finish before the backup
	 * reset and the switch to the run timeout refreshes the counter.
	 * Run phase: commit, the task stops kicking. */
	if (s_run) {
		s_isr_fired = 1u;
	} else {
		s_boot_irq = 1u;
	}
	if (frame != NULL) {
		w->cur_pc = frame[6];
		w->cur_lr = frame[5];
	}
	nm = s_task_alive ? pcTaskGetName(NULL) : NULL;
	for (i = 0u; i < sizeof(w->cur_task); i++) {
		char c = (nm != NULL) ? nm[i] : '\0';
		w->cur_task[i] = c;
		if (c == '\0') {
			nm = NULL;
		}
	}
	w->cur_health |= RIOP_WDOG_H_ISR;
	__DSB();
	RTWDOG1->CS |= RTWDOG_CS_FLG_MASK;
	__DSB();
}

__attribute__((naked))
void RTWDOG1_IRQHandler(void)
{
	__asm(".syntax unified\n"
	      "MOVS   R0, #4           \n"
	      "MOV    R1, LR           \n"
	      "TST    R0, R1           \n"
	      "BEQ    1f               \n"
	      "MRS    R0, PSP          \n"
	      "B      2f               \n"
	      "1:                      \n"
	      "MRS    R0, MSP          \n"
	      "2:                      \n"
	      "LDR    R2,=riop_wdog_isr \n"
	      "BX     R2               \n"
	      ".syntax divided\n");
}

int riop_wdog_safe_mode(void)
{
	return (int)s_safe;
}

/* BOARD_InitHardware, pre-OS, before the M7 is released and before the
 * diag poller is created. */
void riop_wdog_boot(void)
{
	volatile riop_diag_wdog_t *w = wrec();
	uint32_t srsr = SRC_GENERAL_REG->SRSR;
	int fresh = (w->magic != RIOP_DIAG_WDOG_MAGIC) ||
		    ((srsr & (SRC_GENERAL_SRSR_POR_RST_MASK | SRC_GENERAL_SRSR_IPP_POR_B_MASK)) != 0u);
	int wdog = (srsr & SRC_GENERAL_SRSR_WDOG1_RST_B_MASK) != 0u;
	uint16_t state = 0u;
	uint32_t i, hz;

	/* SRSR is sticky; clear it so the next boot sees only its own cause.
	 * This boot's raw value is kept in boot_srsr (asm 177 reset_cause). */
	SRC_GENERAL_REG->SRSR = srsr;

	w->magic = 0u;
	if (fresh) {
		volatile uint32_t *p = (volatile uint32_t *)RIOP_DIAG_WDOG_ADDR;
		for (i = 1u; i < (sizeof(riop_diag_wdog_t) / 4u); i++) {
			p[i] = 0u;
		}
		state |= RIOP_WDOG_S_FRESH;
	}
	w->boot_srsr = srsr;

	if (wdog) {
		state |= RIOP_WDOG_S_WDOG_BOOT;
		if (w->wdog_count < 0xFFFFu) {
			w->wdog_count++;
		}
		if (!fresh) {
			uint32_t h = w->cur_health;
			uint8_t isr = (h & RIOP_WDOG_H_ISR) ? 1u : 0u;
			uint8_t run = (h & RIOP_WDOG_H_RUN) ? 1u : 0u;
			w->last_reason = run ? (isr ? RIOP_WDOG_R_RUN_ISR : RIOP_WDOG_R_RUN_NOISR)
					     : (isr ? RIOP_WDOG_R_BOOT_ISR : RIOP_WDOG_R_BOOT_NOISR);
			w->last_uptime_ms = w->cur_uptime_ms;
			w->last_health = h;
			w->last_pc = isr ? w->cur_pc : 0u;
			w->last_lr = isr ? w->cur_lr : 0u;
			for (i = 0u; i < sizeof(w->last_task); i++) {
				w->last_task[i] = isr ? w->cur_task[i] : '\0';
			}
			if ((w->cur_uptime_ms / 1000u) < RIOP_WDOG_LOOP_WINDOW_S) {
				if (w->consec < 0xFFu) {
					w->consec++;
				}
			} else {
				w->consec = 1u;
			}
		} else {
			w->last_reason = RIOP_WDOG_R_UNKNOWN; /* OCRAM not retained */
			w->consec = 1u;
		}
	} else {
		w->consec = 0u;
	}

	s_safe = (w->consec >= RIOP_WDOG_LOOP_N) ? 1u : 0u;
	if (s_safe) {
		state |= RIOP_WDOG_S_SAFE;
	}
#if RIOP_WDOG_STALL_TEST
	state |= RIOP_WDOG_S_TEST;
#endif
	w->cur_uptime_ms = 0u;
	w->cur_health = 0u;
	w->cur_pc = 0u;
	w->cur_lr = 0u;
	for (i = 0u; i < sizeof(w->cur_task); i++) {
		w->cur_task[i] = '\0';
	}
	w->state = state;
	__DSB();
	w->magic = RIOP_DIAG_WDOG_MAGIC;
	__DSB();

	/* Arm: WDOG1 unmasked as a system-reset source, LPO/256, no window,
	 * IRQ before reset, counter stops while the core is halted (DBG=0). */
	CLOCK_EnableClock(kCLOCK_Wdog1);
	SRC_GENERAL_REG->SRMASK &= ~SRC_GENERAL_SRMASK_WDOG1_MASK_MASK;

	hz = wdog_calibrate();
	w->clk_hz = (uint16_t)((hz > 0xFFFFu) ? 0xFFFFu : hz);
	if ((hz >= 96u) && (hz <= 160u)) {
		s_tick_hz = RIOP_WDOG_NOMINAL_HZ;
	} else if ((hz >= 16u) && (hz <= 2000u)) {
		s_tick_hz = hz;     /* unexpected but usable: use the measured rate */
	} else {
		s_tick_hz = 0u;     /* refuse to arm rather than reset-loop */
	}
	if (s_tick_hz == 0u) {
		(void)wdog_write_cfg(0xFFFFu, RTWDOG_CS_CMD32EN_MASK | RTWDOG_CS_UPDATE_MASK |
				     RTWDOG_CS_CLK(1u));
		wdog_state_set(RIOP_WDOG_S_CLK_BAD);
		return;
	}
	s_cs_base = RTWDOG_CS_CMD32EN_MASK | RTWDOG_CS_PRES_MASK | RTWDOG_CS_CLK(1u) |
		    RTWDOG_CS_UPDATE_MASK | RTWDOG_CS_WAIT_MASK | RTWDOG_CS_INT_MASK |
		    RTWDOG_CS_EN_MASK;
	NVIC_SetPriority(RTWDOG1_IRQn, 0u);
	NVIC_ClearPendingIRQ(RTWDOG1_IRQn);
	if (wdog_arm(RIOP_WDOG_BOOT_TOVAL_MS) != 0) {
		NVIC_EnableIRQ(RTWDOG1_IRQn);
		wdog_state_set(RIOP_WDOG_S_EN);
	} else {
		wdog_state_set(RIOP_WDOG_S_CLK_BAD);
	}
}

static uint8_t wdog_netif_up(void)
{
#if RIOP_WDOG_HAS_NETIF
	struct netif *nif = netif_default;
	return ((nif != NULL) && netif_is_up(nif)) ? 1u : 0u;
#else
	return 1u;
#endif
}

#if RIOP_WDOG_STALL_TEST
static void wdog_stall_test(volatile riop_diag_wdog_t *w, uint32_t now)
{
	uint8_t stage;

	if (s_safe || (w->test_stage >= 3u) || ((now / 1000u) < RIOP_WDOG_TEST_AT_S)) {
		return;
	}
	stage = (uint8_t)(w->test_stage + 1u);
	w->test_stage = stage;
	w->cur_uptime_ms = now;
	__DSB();
	if (stage == 2u) {
		/* PRIMASK: no IRQ can run, as in a stalled bus access.
		 * Expect reason RUN_NOISR, last_pc 0. */
		__disable_irq();
	} else {
		/* BASEPRI critical section: scheduler and tick stopped, the
		 * priority-0 watchdog IRQ still runs. Expect RUN_ISR + PC here. */
		taskDISABLE_INTERRUPTS();
	}
	for (;;) {
		__NOP();
	}
}
#endif

static void riop_wdog_task(void *arg)
{
	volatile riop_diag_wdog_t *w = wrec();
	uint32_t now = wdog_ms();
	uint32_t goal_cnt = g_riop_goal_loop_cnt, goal_seen = now;
	uint32_t poll_seq = 0u, poll_seen = now;
	uint32_t netif_seen = now;
	uint32_t ready_at = 0u;
	uint8_t hook_missing = 0u;

	(void)arg;
	s_task_alive = 1u;
	for (;;) {
		uint32_t h = 0u, c, s;
		uint8_t up, healthy;

		vTaskDelay(pdMS_TO_TICKS(RIOP_WDOG_TASK_MS));
		now = wdog_ms();

		c = g_riop_goal_loop_cnt;
		if (c != goal_cnt) {
			goal_cnt = c;
			goal_seen = now;
		}
		if (c != 0u) {
			h |= RIOP_WDOG_H_GOAL_HOOK;
		}
		if (g_riop_goal_ready && (ready_at == 0u)) {
			ready_at = now | 1u;
		}
		if ((ready_at != 0u) && (c == 0u) && ((now - ready_at) > RIOP_WDOG_HOOK_GRACE_MS)) {
			hook_missing = 1u; /* our hook, not GOAL, is broken: do not reset for it */
		}
		if (hook_missing) {
			h |= RIOP_WDOG_H_HOOK_MISSING;
		}
		if (hook_missing || (c == 0u && s_safe) || ((now - goal_seen) < RIOP_WDOG_GOAL_MAX_MS)) {
			h |= RIOP_WDOG_H_GOAL_OK;
		}

		s = g_riop_diag_poll_seq;
		if (s != 0u) {
			h |= RIOP_WDOG_H_POLL_ACTIVE;
			if (s != poll_seq) {
				poll_seq = s;
				poll_seen = now;
			}
			if ((now - poll_seen) < RIOP_WDOG_POLL_MAX_MS) {
				h |= RIOP_WDOG_H_POLL_OK;
			}
		} else {
			h |= RIOP_WDOG_H_POLL_OK;
			poll_seen = now;
		}

		up = wdog_netif_up();
		if (up) {
			netif_seen = now;
		}
		if (s_safe || ((now - netif_seen) < RIOP_WDOG_NETIF_MAX_MS)) {
			h |= RIOP_WDOG_H_NETIF_OK;
		}
		if (s_safe) {
			h |= RIOP_WDOG_H_SAFE;
		}

		if (!s_run) {
			uint8_t done = s_safe ? (uint8_t)(g_riop_goal_ready || ((now / 1000u) >= RIOP_WDOG_SAFE_RUN_S))
					      : (uint8_t)(g_riop_goal_ready && ((c != 0u) || hook_missing) && up);
			if (done && (s_tick_hz != 0u)) {
				if (wdog_arm(RIOP_WDOG_RUN_TOVAL_MS) != 0) {
					s_run = 1u;
					s_boot_irq = 0u;
					goal_seen = now;
					poll_seen = now;
					netif_seen = now;
					wdog_state_set(RIOP_WDOG_S_RUN);
					h |= RIOP_WDOG_H_GOAL_OK | RIOP_WDOG_H_POLL_OK | RIOP_WDOG_H_NETIF_OK;
				}
			}
		}
		if (s_run) {
			h |= RIOP_WDOG_H_RUN | RIOP_WDOG_H_INIT_DONE;
		}

		if ((s_tick_hz != 0u) && ((RTWDOG1->CS & RTWDOG_CS_EN_MASK) == 0u) && !s_isr_fired) {
			/* someone disabled it (e.g. an M7 image built with DISABLE_WDOG=1) */
			if (wdog_write_cfg(s_toval, s_cs_base) != 0) {
				wdog_state_set(RIOP_WDOG_S_REARM);
			}
		}

		healthy = (uint8_t)(((h & (RIOP_WDOG_H_GOAL_OK | RIOP_WDOG_H_POLL_OK | RIOP_WDOG_H_NETIF_OK)) ==
				     (RIOP_WDOG_H_GOAL_OK | RIOP_WDOG_H_POLL_OK | RIOP_WDOG_H_NETIF_OK)) ? 1u : 0u);
		if (s_run && healthy && !s_isr_fired) {
			wdog_kick();
			h |= RIOP_WDOG_H_KICKED;
		}

		if (s_isr_fired || (!s_run && s_boot_irq)) {
			h |= RIOP_WDOG_H_ISR;
		}
		w->cur_uptime_ms = now;
		w->cur_health = h;

		if (s_run && (w->consec != 0u) && ((now / 1000u) >= RIOP_WDOG_LOOP_WINDOW_S)) {
			w->consec = 0u; /* ran clean for the window: the next WDOG reset starts a new count */
		}
#if RIOP_WDOG_STALL_TEST
		if (s_run) {
			wdog_stall_test(w, now);
		}
#endif
	}
}

void riop_wdog_start(void)
{
	if ((wrec()->state & RIOP_WDOG_S_EN) == 0u) {
		return;
	}
	(void)xTaskCreate(riop_wdog_task, "riop_wdog", 512, NULL, tskIDLE_PRIORITY + 1, NULL);
}

#else /* RIOP_M33_WDOG */

void riop_wdog_boot(void)
{
}

void riop_wdog_start(void)
{
}

int riop_wdog_safe_mode(void)
{
	return 0;
}

#endif /* RIOP_M33_WDOG */
