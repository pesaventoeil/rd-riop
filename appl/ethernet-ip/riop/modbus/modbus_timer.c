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
static void prvTpmArm( tpm_clock_prescale_t prescale, uint32_t ulPeriodUs )
{
    tpm_config_t tpmInfo;

    TPM_StopTimer(BOARD_TPM);

    TPM_GetDefaultConfig(&tpmInfo);
    tpmInfo.prescale = prescale;
    TPM_Init(BOARD_TPM, &tpmInfo);

    TPM_SetTimerPeriod(BOARD_TPM,
        USEC_TO_COUNT(ulPeriodUs, TPM_SOURCE_CLOCK / (1U << prescale)));

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
    prvTpmArm(TPM_PRESCALER, TIME_PERIOD_US);   /* fix 1: reprogram, do not inherit */
}

void vMBMasterPortTimersDisable(void)
{
    TPM_ClearStatusFlags(BOARD_TPM, kTPM_TimeOverflowFlag);
    TPM_DisableInterrupts(BOARD_TPM, kTPM_TimeOverflowInterruptEnable);
    DisableIRQ(BOARD_TPM_IRQ_NUM);
    TPM_StopTimer(BOARD_TPM);
}

static void prvvTIMERExpiredISR( void )
{
    ( void )xMBMasterRTUTimerExpired(  );
}
