/*
 * Copyright 2025 NXP
 * RIOP retarget + two bug fixes.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * BUG 1 (serious): vMBMasterPortTimersT35Enable() in the stock file starts
 * the TPM without re-programming prescaler or period. FreeModbus master
 * calls vMBMasterPortTimersRespondTimeoutEnable() after sending a frame,
 * which re-inits the TPM to the RESPOND prescaler and a 200 ms period. The
 * first received byte then calls T35Enable(), which inherits those settings.
 * T3.5 therefore never expires anywhere near 1750 us and end-of-frame
 * detection is broken. Every arm must program its own prescaler and period.
 *
 * BUG 2: TPM_StopTimer() clears CMOD but leaves CNT at its previous value,
 * so the next interval starts from a stale count and expires early by an
 * arbitrary amount. Writing any value to CNT resets it; that write is now
 * part of the common arm path.
 *
 * Both are invisible against a cooperative simulator and show up against a
 * real slave as intermittent frame fragmentation.
 */

#include <stdint.h>
#include <stdbool.h>

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mb_m.h"
#include "mbport.h"
#include "mbrtu.h"

#include "fsl_tpm.h"
#include "fsl_clock.h"
#include "fsl_cache.h"
#include "fsl_lpuart.h"
#include "dce_app.h"
#include "port.h"

/* ----------------------- static functions ---------------------------------*/
static void prvvTIMERExpiredISR( void );

/* Program prescaler + period, reset the counter, and start.
 * Every arm path goes through here so the two bugs above cannot come back. */
volatile uint32_t dceTpmDiag[6];
/* G51: end-of-frame gap (T3.5) in us, runtime adjustable (asm 173 reserved).
 * The DCE leaves inter-character gaps of up to ~3.5 ms inside long replies, so the
 * spec's fixed 1750 us (>19200 baud) cuts replies short (G48: 11 % failed
 * transactions). Bench sweep G50: 1.75 ms 11 % bad, 3.0 ms 3 %, 3.5 ms ~1 %,
 * >=4.0 ms 0 % over 25 s runs. 5.0 ms = 25 % margin over the clean threshold. */
volatile uint32_t dceT35Us = 5000UL;

/* G45: TPM6 is NOT guaranteed to have a 32-bit counter (the SDK feature macro
 * says yes for every instance, but TPM3..TPM6 can be 16 bit). A 16-bit MOD
 * silently truncates 1750 us @ 80 MHz (140000 ticks) to 8928 ticks = 112 us and
 * 200 ms (16000000 ticks) to 9216 ticks = 115 us. Read the real width from
 * TPM_PARAM[WIDTH] and pick the smallest prescaler that fits the period. */
static uint32_t prvTpmMaxCount( void )
{
    uint32_t w = (BOARD_TPM->PARAM >> 16) & 0xFFu;
    return (w >= 32u) ? 0xFFFFFFFFu : ((1UL << w) - 1UL);
}

static void prvTpmArm( tpm_clock_prescale_t prescaleReq, uint32_t ulPeriodUs )
{
    tpm_config_t tpmInfo;
    static uint32_t cUs[4], cP[4], cTicks[4], cMax;
    uint32_t maxCnt = cMax ? cMax : (cMax = prvTpmMaxCount());
    uint32_t p = 0u;
    uint64_t ticks = 0u;
    uint32_t slot;
    bool hit = false;

    (void)prescaleReq;
    /* The T3.5 re-arm runs from the LPUART ISR on every byte, so memoise. */
    for (slot = 0u; slot < 4u; slot++) {
        if (cUs[slot] == ulPeriodUs) {
            p = cP[slot]; ticks = cTicks[slot]; hit = true;
            break;
        }
    }
    if (!hit) {
        for (p = 0u; p < 7u; p++) {
            ticks = ((uint64_t)ulPeriodUs * (uint64_t)(TPM_SOURCE_CLOCK / (1U << p))) / 1000000ULL;
            if (ticks < (uint64_t)maxCnt) {
                break;
            }
        }
        for (slot = 0u; slot < 4u; slot++) {
            if (cUs[slot] == 0u) {
                cUs[slot] = ulPeriodUs; cP[slot] = p; cTicks[slot] = (uint32_t)ticks;
                break;
            }
        }
    }

    TPM_StopTimer(BOARD_TPM);

    TPM_GetDefaultConfig(&tpmInfo);
    tpmInfo.prescale = (tpm_clock_prescale_t)p;
    TPM_Init(BOARD_TPM, &tpmInfo);

    TPM_SetTimerPeriod(BOARD_TPM, (uint32_t)ticks);

    dceTpmDiag[0] = BOARD_TPM->PARAM;
    dceTpmDiag[5] = maxCnt;
    if (ulPeriodUs == MB_MASTER_TIMEOUT_US_RESPOND) {
        dceTpmDiag[1] = BOARD_TPM->MOD;
        dceTpmDiag[2] = p;
    } else if (ulPeriodUs == dceT35Us) {
        dceTpmDiag[3] = BOARD_TPM->MOD;
        dceTpmDiag[4] = p;
    }

    BOARD_TPM->CNT = 0U;    /* fix 2: discard any residual count */

    TPM_ClearStatusFlags(BOARD_TPM, kTPM_TimeOverflowFlag);
    TPM_EnableInterrupts(BOARD_TPM, kTPM_TimeOverflowInterruptEnable);
    EnableIRQ(BOARD_TPM_IRQ_NUM);
    TPM_StartTimer(BOARD_TPM, kTPM_SystemClock);
}

/* ----------------------- Start implementation -----------------------------*/
void BOARD_TPM_HANDLER(void)
{
    TPM_ClearStatusFlags(BOARD_TPM, kTPM_TimeOverflowFlag);
    prvvTIMERExpiredISR();
    SDK_ISR_EXIT_BARRIER;
}

BOOL
xMBMasterPortTimersInit( USHORT usTimerT35_50us )
{
    /* Enable the TPM6 peripheral clock gate before any TPM register access.
     * Root is set in clock_config.c; the LPCG must also be on. */
    CLOCK_EnableClock(kCLOCK_Tpm6);	
	
    /* The stack passes the computed T3.5 in 50 us units, but T3.5 above
     * 19200 baud is the fixed 1750 us from app.h, so the argument is
     * deliberately unused -- same as the stock file, just now on purpose. */
    (void)usTimerT35_50us;

    /* Reset priority 0 is above configMAX_SYSCALL_INTERRUPT_PRIORITY and
     * asserts in vPortValidateInterruptPriority. See DCE_MB_IRQ_PRIO. */
    NVIC_SetPriority(BOARD_TPM_IRQ_NUM, DCE_MB_IRQ_PRIO);
    prvTpmArm(TPM_PRESCALER, TIME_PERIOD_US);
    TPM_StopTimer(BOARD_TPM);   /* armed but idle until the stack asks */

    return 1;
}

void vMBMasterPortTimersConvertDelayEnable(void)
{
    vMBMasterSetCurTimerMode(MB_TMODE_CONVERT_DELAY);
    prvTpmArm(TPM_PRESCALER_CONVERT, MB_MASTER_DELAY_US_CONVERT);
}

void vMBMasterPortTimersRespondTimeoutEnable(void)
{
    vMBMasterSetCurTimerMode(MB_TMODE_RESPOND_TIMEOUT);
    prvTpmArm(TPM_PRESCALER_RESPOND, MB_MASTER_TIMEOUT_US_RESPOND);
}

void vMBMasterPortTimersT35Enable(void)
{
    vMBMasterSetCurTimerMode(MB_TMODE_T35);
    prvTpmArm(TPM_PRESCALER, dceT35Us);   /* fix 1: reprogram, do not inherit */
}

void vMBMasterPortTimersDisable(void)
{
    TPM_ClearStatusFlags(BOARD_TPM, kTPM_TimeOverflowFlag);
    TPM_DisableInterrupts(BOARD_TPM, kTPM_TimeOverflowInterruptEnable);
    DisableIRQ(BOARD_TPM_IRQ_NUM);
    TPM_StopTimer(BOARD_TPM);
}

extern void dceRxFrameEnd( void );

static void prvvTIMERExpiredISR( void )
{
    dceRxFrameEnd();
    ( void )xMBMasterRTUTimerExpired(  );
}
