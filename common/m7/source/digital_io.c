/*
* Copyright 2025-2026 NXP
*
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly
* in accordance with the applicable license terms.  By expressly accepting such terms or by downloading,
* installing, activating and/or otherwise using the software, you are agreeing that you have read,
* and that you agree to comply with and are bound by, such license terms.  If you do not agree to be bound by
* the applicable license terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "FreeRTOS.h"
#include "task.h"

#if GOAL
#if GOAL_CONFIG_ECAT_AC
#include "goal_appl_ecat.h"
#include "goal_appl_ecat_objects.h"
#elif GOAL_CONFIG_EIP
#include "goal_app_eip.h"
#endif
#endif

#include "board.h"
#include "clock_config.h"
#include "fsl_debug_console.h"
#include "mcmgr.h"
#include "pin_mux.h"

#include "gl.h"

#include "api_riop.h"

/* Well done: Main function moved here from "flash zone". */
#include "mcmgr.h"

/* N8: NAFE temp path build switches (only present in the EIP gateway M7 build). */
#if defined(__has_include)
#if __has_include("nafe_temp_cfg.h")
#include "nafe_temp_cfg.h"
#endif
#endif
#ifndef NAFE_TEMP_TASK_ENABLE
#define NAFE_TEMP_TASK_ENABLE 0
#endif
#ifndef NAFE_TEMP_OWNS_AFE
#define NAFE_TEMP_OWNS_AFE 0
#endif

uint32_t fsm_mode;
unsigned char LED_status;
unsigned char di;
UINT32 adc_1;

uint8_t led_green = 0;
extern riop_board_status_t g_boardStatus;
riop_command_t riop_command;

#ifdef DEBUG_PRINT
DEBUG_PRINT
#endif
void dwt_init(void)
{
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->CYCCNT = 0;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}
// Inline makes a problem under Debug
#define dwt_get() DWT->CYCCNT

// static riop_status_t last_err = 0;
static uint32_t time;
static uint32_t tim;
static int tim_mode = 200;
static int tim_first = 0;
static volatile uint16_t RPMsgRemoteReadyEventData = 0;


/* N10: D10 RGB (OVSARGB4R8) status LED on RIOP B2.
 *   Red   GPIO_AON_03  MCU C8  RGPIO1.3   (TPM1_CH2 mux exists, unused)
 *   Green GPIO_AON_15  MCU B3  RGPIO1.15  (no PWM mux)
 *   Blue  GPIO_AON_10  MCU B5  RGPIO1.10  (no PWM mux)
 * Active-high (LOGIC_LED_ON=1). Green and blue have no timer mux, so
 * brightness is a 2 kHz software PWM from DWT in this already-polled
 * appl_loop. Breath period is 2750 ms. No new task or ISR, so this
 * cannot steal time from the 1 Hz NAFE scan or EIP.
 *
 * States keep the N9 meanings; only the look changes:
 *   led_green==0  (default / no EIP led_in) : teal breath   (was 5 Hz red blink)
 *   led_green!=0  (EIP led_in commanded)    : green breath  (was 5 Hz green blink)
 *   fault (reserved, not driven today)      : steady red, never blink
 *   warning/degraded (reserved)             : amber breath
 */
#define RIOP_LED_BUILD        10
const char riop_fw_build[] = "N10a";
#define LED_PWM_HZ            2000u
#define LED_PWM_STEPS         64u
#define LED_BREATH_MS         2750u
#define LED_DUTY_MIN          3u
#define LED_DUTY_MAX          52u

static const uint8_t k_led_sin_q[17] = {
	0, 25, 50, 74, 98, 121, 142, 162, 181, 198, 213, 226, 237, 245, 251, 254, 255
};

static uint8_t led_breath_duty(uint32_t phase_ms)
{
	uint32_t ph, x, i, f, s;

	phase_ms %= LED_BREATH_MS;
	ph = (phase_ms * 256u) / LED_BREATH_MS; /* 0..255 */
	x = (ph < 128u) ? ph : (256u - ph);     /* 0..128, up then down */
	i = x / 8u;                             /* 0..16 */
	f = x % 8u;
	s = k_led_sin_q[i];
	if (i < 16u) {
		s += ((uint32_t)(k_led_sin_q[i + 1u] - k_led_sin_q[i]) * f) / 8u;
	}
	return (uint8_t)(LED_DUTY_MIN + (s * (LED_DUTY_MAX - LED_DUTY_MIN)) / 255u);
}

static void led_write_rgb(uint8_t r, uint8_t g, uint8_t b)
{
	RGPIO_PinWrite(BOARD_USER_LED_RED_GPIO, BOARD_USER_LED_RED_GPIO_PIN,
		       r ? LOGIC_LED_ON : LOGIC_LED_OFF);
	RGPIO_PinWrite(BOARD_USER_LED_GREEN_GPIO, BOARD_USER_LED_GREEN_GPIO_PIN,
		       g ? LOGIC_LED_ON : LOGIC_LED_OFF);
	RGPIO_PinWrite(BOARD_USER_LED_BLUE_GPIO, BOARD_USER_LED_BLUE_GPIO_PIN,
		       b ? LOGIC_LED_ON : LOGIC_LED_OFF);
}

/* Scale a 0..STEPS master duty by a 0..255 color mix, then compare to PWM phase. */
static uint8_t led_chan_on(uint32_t t, uint32_t period, uint8_t duty, uint8_t mix)
{
	uint32_t d;

	if ((mix == 0u) || (duty == 0u)) {
		return 0u;
	}
	d = ((uint32_t)duty * (uint32_t)mix + 127u) / 255u;
	if (d > LED_PWM_STEPS) {
		d = LED_PWM_STEPS;
	}
	return ((t * LED_PWM_STEPS) < (period * d)) ? 1u : 0u;
}

static void led_animate(void)
{
	static uint32_t last_cyc;
	static uint32_t cyc_frac;
	static uint32_t breath_ms;
	static uint8_t inited;
	uint32_t now, dc, period, t, ms_add;
	uint8_t duty, r, g, b;

	(void)riop_fw_build[0];
	now = dwt_get();
	if (!inited) {
		last_cyc = now;
		inited = 1u;
	}
	dc = now - last_cyc;
	last_cyc = now;
	cyc_frac += dc;
	ms_add = cyc_frac / (SystemCoreClock / 1000u);
	cyc_frac -= ms_add * (SystemCoreClock / 1000u);
	breath_ms += ms_add;

	duty = led_breath_duty(breath_ms);
	period = SystemCoreClock / LED_PWM_HZ;
	if (period == 0u) {
		period = 1u;
	}
	t = now % period;

	if (led_green) {
		/* Commanded: calm green pulse (G full, a little blue). */
		r = led_chan_on(t, period, duty, 0u);
		g = led_chan_on(t, period, duty, 255u);
		b = led_chan_on(t, period, duty, 40u);
	} else {
		/* Normal heartbeat: teal (blue + some green). */
		r = led_chan_on(t, period, duty, 0u);
		g = led_chan_on(t, period, duty, 140u);
		b = led_chan_on(t, period, duty, 255u);
	}
	led_write_rgb(r, g, b);
}

void digital_io_init(void)
{
	dwt_init();

	// uint16_t counter = 0;
	memset(&riop_command, 0, sizeof(riop_command));

	// riop_command.afe_command.afe_gpio_command.gpio_output_value = 4;
	riop_command.afe_command.afe_gpio_command.gpio_output_value = 0;
	// riop_command.afe_command.afe_gpio_command.gpio_pin_direction = 0x3FC;
	riop_command.afe_command.afe_gpio_command.gpio_pin_direction =
		0x3C0; // 4 outputs and 4 inputs, set via ecat
	riop_command.afe_command.afe_gpio_command.is_dirty = 0;

	riop_command.afe_command.afe_hvsig_command.channel_enable[0] = 1;
	riop_command.afe_command.afe_hvsig_command.channel_enable[1] = 1;
	riop_command.afe_command.afe_hvsig_command.channel_enable[2] = 1;
	riop_command.afe_command.afe_hvsig_command.is_dirty = 0;

#if 1
	riop_command.afe_command.afe_lvsig_command.is_dirty = 0;
	riop_command.afe_command.afe_lvsig_command.channel_enable[0] = 1;
	riop_command.afe_command.afe_lvsig_command.channel_enable[1] = 1;
	riop_command.afe_command.afe_lvsig_command.channel_enable[2] = 1;
	riop_command.afe_command.afe_lvsig_command.channel_enable[3] = 1;
	riop_command.afe_command.afe_lvsig_command.channel_enable[4] = 1;
	riop_command.afe_command.afe_lvsig_command.samples_amt = 500;
#else

	riop_command.siggen_control_command.enable = 1;
	riop_command.siggen_control_command.frequency = 10;
	riop_command.siggen_control_command.is_dirty = 1;
#endif
	riop_command.afe_command.afe_current_command.is_dirty = 0;
	riop_command.afe_command.afe_current_command.shunt_value = kShunt100;
	riop_command.afe_command.afe_current_command.measurement_enabled = 1;

	riop_command.afe_command.afe_temperature_command.is_dirty = 0;
	riop_command.afe_command.afe_temperature_command.measurement_enabled = 1;
	riop_command.afe_command.afe_temperature_command.samples_amt =
		128; // up to 6sec!!

	tim = dwt_get();
}

#if GOAL_CONFIG_PNIO
void riop_appl_loop(void)
#else
void appl_loop(void)
#endif
{
	if (tim_first) {
		time = SystemCoreClock / 1000 * 200;
		tim_first = 0;
	} else {
		time = SystemCoreClock / 1000 * tim_mode;
	}

	led_animate();
#ifdef DEBUG_PRINT2
	DEBUG_PRINT2
#endif

	uint32_t dd;
	dd = dwt_get();
	dd -= tim;
	if (dd > time) // max 4 sec
	{
		tim += time; // tim=dwt_get();

		riop_command.afe_command.afe_hvsig_command.is_dirty = 0;
		riop_command.afe_command.afe_current_command.is_dirty = 0;
		riop_command.siggen_control_command.is_dirty = 0;
		riop_command.afe_command.afe_lvsig_command.is_dirty = 0;
		riop_command.afe_command.afe_gpio_command.is_dirty = 0;
		riop_command.mcu_digital_output_command.is_dirty = 0;
		riop_command.afe_command.afe_temperature_command.is_dirty = 0;
#if 0
		//gpio toggling
		{
		static int lled = 0;
		if(lled)
			riop_command.afe_command.afe_gpio_command.gpio_output_value =0x3F0;
		else
			riop_command.afe_command.afe_gpio_command.gpio_output_value =0x0;

		if(lled)
			riop_command.mcu_digital_output_command.mcu_digital_output =0x00;
		else
			riop_command.mcu_digital_output_command.mcu_digital_output =0xF1;
		}
#endif

		static int fsm = 0;
		uint32_t *pfsm_mode;
		pfsm_mode = &fsm_mode;
		static int last_mode = 0;
		if (last_mode != *pfsm_mode) {
			fsm = 0;
			tim_first = 1;
			last_mode = *pfsm_mode;
			switch (*pfsm_mode) {
			default:
			case 0:
				tim_mode = 200;
				break;
			case 1:
				tim_mode = 20;
				riop_command.afe_command.afe_hvsig_command.is_dirty = 1;
				break;
			case 2:
				tim_mode = 20;
				riop_command.afe_command.afe_current_command.is_dirty = 1;
				break;
			case 3:
				tim_mode = 200;
				riop_command.afe_command.afe_lvsig_command.is_dirty = 1;
				break;
			case 4:
				tim_mode = 20;
				riop_command.afe_command.afe_gpio_command.is_dirty = 1;
				break;
			case 5:
				tim_mode = 20;
				riop_command.mcu_digital_output_command.is_dirty = 1;
				break;
			case 6:
				tim_mode = 3000;
				riop_command.afe_command.afe_temperature_command.is_dirty = 1;
				break;
			}
			//!!!!
			// tim_mode*=4;
		}
		switch (*pfsm_mode) {
		case 0:
		{
			// Automatic switching modes
			switch (++fsm) {
			default:
				fsm = 0;
			case 0:
				riop_command.afe_command.afe_gpio_command.is_dirty = 1;
				break;
			case 1:
				break;
			case 2:
				riop_command.afe_command.afe_hvsig_command.is_dirty = 1;
				break;
			case 3:
				riop_command.mcu_digital_output_command.is_dirty = 1;
				break;
			case 4:
				riop_command.afe_command.afe_current_command.is_dirty = 1;
				break;
			case 5:
			{
				static uint32_t pass = 0;
				pass++;
				if (!(pass & 0x07))
					riop_command.afe_command.afe_lvsig_command.is_dirty = 1;
			} break;
				/*
         * This delays the loop for a couple of seconds.
case 6:
        {
                riop_command.afe_command.afe_temperature_command.is_dirty = 1;
        }
        break;
case 7:
        break;
case 8:
        break;
case 9:
        break;
case 10:
        break;
case 11:
        break;
case 12:
        break;
case 13:
        break;
case 14:
        break;
case 15:
        break;
case 16:
        break;
case 17:
        break;
case 18:
        break;
case 19:
        break;
        */
			}
		} break;
		case 1:
			break;
		case 2:
			break;
		case 3:
		{
			static uint32_t pass = 0;
			pass++;
			if (!(pass & 0x07))
				riop_command.afe_command.afe_lvsig_command.is_dirty = 1;
		} break;
		case 4:
			// Change detector with validator.
			static uint16_t afe_last;
			static uint8_t afe_last_val = 0;
			if (!afe_last_val) {
				afe_last = riop_command.afe_command.afe_gpio_command.gpio_output_value;
				afe_last_val = 1;
			}
			if (afe_last !=
			    riop_command.afe_command.afe_gpio_command.gpio_output_value) {
				afe_last = riop_command.afe_command.afe_gpio_command.gpio_output_value;
				riop_command.afe_command.afe_gpio_command.is_dirty = 1;
			}
			static uint16_t afe_last_d;
			static uint8_t afe_last_d_val = 0;
			// Another change detector
			if (!afe_last_d_val) {
				afe_last_d =
					riop_command.afe_command.afe_gpio_command.gpio_pin_direction;
				afe_last_d_val = 1;
			}
			if (afe_last_d !=
			    riop_command.afe_command.afe_gpio_command.gpio_pin_direction) {
				afe_last_d =
					riop_command.afe_command.afe_gpio_command.gpio_pin_direction;
				riop_command.afe_command.afe_gpio_command.is_dirty = 1;
			}
			break;
		case 5:
			// Change detector with validator.
			static uint8_t mcu_last;
			static uint8_t mcu_last_val = 0;
			if (!mcu_last_val) {
				mcu_last = riop_command.mcu_digital_output_command.mcu_digital_output;
				mcu_last_val = 1;
			}
			if (mcu_last !=
			    riop_command.mcu_digital_output_command.mcu_digital_output) {
				mcu_last = riop_command.mcu_digital_output_command.mcu_digital_output;
				riop_command.mcu_digital_output_command.is_dirty = 1;
			}
			break;
		case 6:
			riop_command.afe_command.afe_temperature_command.is_dirty = 1;
			break;
		}

#if NAFE_TEMP_TASK_ENABLE && NAFE_TEMP_OWNS_AFE
		/* N8: U38 belongs to the NAFE temp task; never start NXP AFE sampling (see nafe_temp_cfg.h). */
		riop_command.afe_command.afe_hvsig_command.is_dirty = 0;
		riop_command.afe_command.afe_current_command.is_dirty = 0;
		riop_command.afe_command.afe_lvsig_command.is_dirty = 0;
		riop_command.afe_command.afe_temperature_command.is_dirty = 0;
		riop_command.afe_command.afe_calibration_command.is_dirty = 0;
#endif
		ICC_DataHandleTask(NULL);
	}
}
