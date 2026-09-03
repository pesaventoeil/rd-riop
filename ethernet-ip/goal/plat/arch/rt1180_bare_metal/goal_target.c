/** @file
 *
 * @brief Low Level Interface of GOAL
 *
 * This module binds GOAL to a specific hardware or OS. It provides a generic
 * API for GOAL.
 *
 * @copyright
 * Copyright 2010-2023 port GmbH Halle/Saale.
 * This software is protected Intellectual Property and may only be used
 * according to the license agreement.
 *
 * Copyright 2025 NXP
 *
 * NXP Confidential and Proprietary. This software is owned or controlled by NXP
 * and may only be used strictly in accordance with the applicable license
 * terms. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you
 * have read, and that you agree to comply with and are bound by, such license
 * terms. If you do not agree to be bound by the applicable license terms, then
 * you may not retain, install, activate or otherwise use the software.
 */

#define GOAL_ID GOAL_ID_TGT
#include <goal_includes.h>
#include "goal_target.h"
#include "goal_target_common.h"


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
void time_isr(
    void
);

static GOAL_STATUS_T goal_targetInit(
    void
);

static GOAL_STATUS_T goal_targetShutdown(
    void
);


/****************************************************************************/
/* External variables */
/****************************************************************************/
#ifndef __CA7_REV
extern uint32_t SystemCoreClock;                /**< bsp clock information */
#endif


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static volatile uint64_t time_now = 0;          /**< systick counter */
static GOAL_STAGE_HANDLER_T stageInit;          /**< init stage handler */
static GOAL_STAGE_HANDLER_T stageShutdown;      /**< shutdown stage handler */


/****************************************************************************/
/** Global Main Entry
 *
 * @returns System Exit Code (always 0)
 */
int main(
    int argc,                                   /**< argument count */
    char **argv                                 /**< arguments */
)
{
    UNUSEDARG(argc);
    UNUSEDARG(argv);

    /* call main application */
    goal_init();

    /* shut down after appl finished */
    while (!GOAL_flagShutdown) {
        goal_loop();
    }

    goal_targetHalt();

    return 0;
}


/****************************************************************************/
/** Register stage handler
 *
 * This function registers the primary target initialization function and the
 * shutdown function to the GOAL stage system.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_targetInitPre(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* init stage */
    res = goal_mainStageReg(GOAL_STAGE_BOARD, &stageInit, GOAL_STAGE_INIT, goal_targetInit);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Failed to register target init function.");
    }

    /* shutdown stage */
    if (GOAL_RES_OK(res)) {
        res = goal_mainStageReg(GOAL_STAGE_BOARD, &stageShutdown, GOAL_STAGE_SHUTDOWN, goal_targetShutdown);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("Failed to register target shutdown function.");
        }
    }

    if (GOAL_RES_OK(res)) {
        res = goal_targetCommonInitPre();
    }

    return res;
}


/****************************************************************************/
/** Target Initialization
 *
 * This function initializes the low level drivers used by GOAL. If the function
 * sets @em pPostInitFunc GOAL will call this function after its initialization.
 *
 * @retval GOAL_OK - success
 * @retval other - failed
 */
static GOAL_STATUS_T goal_targetInit(
    void
)
{
    /* setup the systick interrupt */
#ifdef __CA7_REV
    /* special for i.mx6ul */
    SystemSetupSystick(1000U, (void *) time_isr, 0U);
    SystemClearSystickFlag();
#else
    /* Set SysTick period to 1 ms and enable its interrupts */
    SysTick_Config(USEC_TO_COUNT(1000U, SystemCoreClock));
    NVIC_EnableIRQ(SysTick_IRQn);
#endif

    return GOAL_OK;
}


/****************************************************************************/
/** Target Shutdown
 *
 * This function shuts down the target
 *
 * @retval GOAL_OK - success
 * @retval other - failed
 */
static GOAL_STATUS_T goal_targetShutdown(
    void
)
{
    goal_targetHalt();

    return GOAL_OK;
}



#ifndef __CA7_REV
/****************************************************************************/
/** Interrupt service for SysTick timer
 *
 */
void SysTick_Handler(
    void
)
{
    time_isr();
}
#endif


/****************************************************************************/
/** Interrupt service for SysTick timer
 *
 */
void time_isr(
    void
)
{
#ifdef __CA7_REV
    SystemClearSystickFlag();
#endif
    time_now++;

    goal_timerRun(GOAL_TRUE);
}


/****************************************************************************/
/** Global Stack Halt Function
 *
 * This functions makes it easier to focus on one breakpoint while debugging.
 * See callstack for details of the caller.
 */
void goal_targetHalt(
    void
)
{
    for (;;);
}


/****************************************************************************/
/** Get Current Tick Count - 64 Bit
 *
 * Returns the current tick counter in ms as GOAL_TIMESTAMP_T.
 * The 64 bit variant will overflow in 584942417 years.
 *
 * @retval GOAL_TIMESTAMP_T 64 bit timestamp structure
 */
GOAL_TIMESTAMP_T goal_targetGetTimestamp(
    void
)
{
    return (GOAL_TIMESTAMP_T) time_now;
}


/****************************************************************************/
/** Read the available Buttons
 *
 * Return the state of the available buttons as bits. If a bit is set, it means
 * the button is pressed.
 *
 * @retval status button status bit field
 */
uint32_t goal_targetGetButtons(
    void
)
{
    return 0;
}
