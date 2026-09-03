/** @file
 *
 * @brief
 * Task Example Application
 *
 * @details
 * This application shows how to set up and use tasks.
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
#define GOAL_APPL_MAIN_LOOP_TIMER_PERIOD        (5 * GOAL_TIMER_SEC)
#define GOAL_APPL_TASK_LOOP_SLEEP_DURATION      (5 * GOAL_TIMER_SEC)


/****************************************************************************/
/* Local Variables */
/****************************************************************************/
static GOAL_TIMER_T *pTimerMainLoop = NULL;     /**< main loop timer */
static GOAL_TASK_T *pTskLoop = NULL;            /**< GOAL loop task */


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
static GOAL_STATUS_T main_taskLoop(
    void *pArg                                  /**< task argument */
) GOAL_TARGET_NORETURN;

static void appl_timerMainLoopCb(
    void *pArg                                  /**< timer argument */
);


/****************************************************************************/
/** Application Setup
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T appl_setup(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* put GOAL loop into extra task */
    res = goal_taskCreate(&pTskLoop,
                           "GOAL Loop Task",
                           main_taskLoop,
                           NULL,
                           GOAL_TASK_PRIO_DEFAULT,
                           GOAL_TASK_STACK_DEFAULT,
                           GOAL_FALSE);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create loop task");
        return res;
    }

    /* As the task was created with the boolean false,
     * it will not run automaticly - therefore we have
     * to start the task explicit. */
    res = goal_taskStart(pTskLoop);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to start loop task");
        return res;
    }

    /* create main loop timer */
    res = goal_timerCreate(&pTimerMainLoop, GOAL_TIMER_LOW);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("error during eth stats timer creation");
        return res;
    }

    /* setup and start main loop timer */
    res = goal_timerSetup(pTimerMainLoop, GOAL_TIMER_PERIODIC, GOAL_APPL_MAIN_LOOP_TIMER_PERIOD, appl_timerMainLoopCb, NULL, GOAL_TRUE);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("error while starting eth stats timer");
        return res;
    }

    return res;
}


/****************************************************************************/
/** Application Loop Task
 *
 * This task executes GOAL loop periodically.
 */
static GOAL_STATUS_T main_taskLoop(
    void *pArg                                  /**< task argument */
)
{
    UNUSEDARG(pArg);

    while (GOAL_TRUE) {
        goal_logInfo("task: task loop");
        goal_taskMsSleep(GOAL_APPL_TASK_LOOP_SLEEP_DURATION, NULL);
    }
}


/****************************************************************************/
/** Application Loop
 */
static void appl_timerMainLoopCb(
    void *pArg                                  /**< timer argument */
)
{
    UNUSEDARG(pArg);

    goal_logInfo("task: main loop");
}
