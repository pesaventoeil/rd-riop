/** @file
 *
 * @brief LED demo application
 *
 * This is a simple LED chase lights application, which uses alternating
 * ascending or descending LED order.
 *
 * @copyright
 * Copyright 2010-2017.
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

#include "goal_includes.h"


/****************************************************************************/
/* Local defines */
/****************************************************************************/
#define GOAL_LEDS_NUM 8                         /**<  number of LEDs */
#define GOAL_LED_BLINK_TIME_MS 25               /**<  LED blinks every 25ms */


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_TIMER_T *pLedTimer = GOAL_TIMER_INVALID; /**< Timer for led demo */
static uint32_t ledActive = 0;                  /**< current activated LED */
static GOAL_BOOL_T flgLedOrderAsc = GOAL_TRUE;  /**< flag for ascending LED activation order */


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/
static void goal_applLedCallback(
    void *pArg                                  /**< unused argument */
);


/****************************************************************************/
/** Application Init
 *
 * Initialize required GOAL components like protocols.
 *
 * Note: If the function body is empty the whole function can be removed.
 */
GOAL_STATUS_T appl_init(
    void
)
{
    return GOAL_OK;
}


/****************************************************************************/
/** Application Setup
 *
 * Configure GOAL and components.
 *
 * Note: If the function body is empty the whole function can be removed.
 */
GOAL_STATUS_T appl_setup(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* set up a timer for demo mode */
    res = goal_timerCreate(&pLedTimer, GOAL_TIMER_HIGH);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Failed to create LED timer");
        return res;
    }

    /* setup and start timer */
    res = goal_timerSetup(pLedTimer, GOAL_TIMER_PERIODIC,
                          GOAL_LED_BLINK_TIME_MS, goal_applLedCallback,
                          NULL, GOAL_TRUE);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Failed to start LED timer");
        return res;
    }

    /* inform user */
    goal_logInfo("Chase Lights application started.");

    return GOAL_OK;

}


/****************************************************************************/
/** Application Loop
 *
 * Perform cyclic application tasks.
 *
 * Note: If the function body is empty the whole function can be removed.
 */
void appl_loop(
    void
)
{
}


/****************************************************************************/
/** Test mode run leds
 *
 * This function is called every GOAL_LED_BLINK_TIME_MS seconds and turns on
 * one LED. The LEDs are activated in ascending or descending order respectly.
 * The order is changing if the first or last LED was activated.
 */
static void goal_applLedCallback(
    void *pArg                                  /**< unused argument */
)
{
    UNUSEDARG(pArg);

    /* find next LED in ascending or descending order respectivly */
    ledActive = (GOAL_TRUE == flgLedOrderAsc) ? ledActive + 1 : ledActive - 1;

    /* check for LED turnaround */
    if (GOAL_LEDS_NUM <= ledActive + 1) {
        flgLedOrderAsc = GOAL_FALSE;
    }
    else if (0 == ledActive) {
        flgLedOrderAsc = GOAL_TRUE;
    }

    /* set LED */
    goal_targetSetLeds(1 << ledActive);
}
