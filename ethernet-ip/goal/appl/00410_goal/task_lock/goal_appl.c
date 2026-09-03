/** @file
 *
 * @brief
 * Show synchronisation of two tasks via locking
 *
 * @details
 * This application shows how to set up and use the locking mechanismn with two
 * tasks.
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
/* Defines */
/****************************************************************************/
#define APPL_TASK_ONE_SLEEP     ((uint32_t) (1 * GOAL_TIMER_SEC)) /**< first task sleep time */
#define APPL_TASK_TWO_SLEEP     ((uint32_t) (1.5 * GOAL_TIMER_SEC)) /**< second task sleep time */
#define APPL_TASK_PRIO_HIGH     GOAL_TASK_PRIO_HIGHEST /**< high priority */
#define APPL_TASK_PRIO_LOW      GOAL_TASK_PRIO_LOWEST /**< low priority */


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_BOOL_T flgGoalReady = GOAL_FALSE;   /**< GOAL ready flag */
static GOAL_LOCK_T *gpLckSync = NULL;           /**< synchronisation lock */
static GOAL_TASK_T *pTskOne = NULL;             /**< first GOAL task */
static GOAL_TASK_T *pTskTwo = NULL;             /**< second GOAL task */


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
static GOAL_STATUS_T main_task(
    void *pArg                                  /**< task argument */
) GOAL_TARGET_NORETURN;


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

    /* initialize lock */
    res = goal_lockCreate(GOAL_LOCK_BINARY, &gpLckSync, 0, 1, GOAL_ID_APPL);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create sync lock");
        return res;
    }

    /* start first GOAL task */
    res = goal_taskCreate(&pTskOne,
                          "GOAL Task One",
                          main_task,
                          &pTskOne,
                          APPL_TASK_PRIO_HIGH,
                          GOAL_TASK_STACK_DEFAULT,
                          GOAL_TRUE);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* start second GOAL task */
    res = goal_taskCreate(&pTskTwo,
                          "GOAL Task Two",
                          main_task,
                          &pTskTwo,
                          APPL_TASK_PRIO_HIGH,
                          GOAL_TASK_STACK_DEFAULT,
                          GOAL_TRUE);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* set GOAL ready flag */
    flgGoalReady = GOAL_TRUE;

    return GOAL_OK;
}


/****************************************************************************/
/** Generic Task
 *
 * This task tries to take a lock, reports it, holds it a while and releases
 * it. The argument is the address of the local task pointer variable.
 */
static GOAL_STATUS_T main_task(
    void *pArg                                  /**< task argument */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_TASK_T *pTask = *(GOAL_TASK_T **) pArg; /* calling task */
    uint32_t timeSleep;                         /* sleep time of calling task */

    /* get sleep duration of calling task */
    timeSleep = (pTask == pTskOne) ? APPL_TASK_ONE_SLEEP : APPL_TASK_TWO_SLEEP;

    goal_logInfo("Task %s: task initialized -> waiting for GOAL ready flag", pTask->pName);
    while (!flgGoalReady) {
        goal_taskMsSleep(GOAL_TIMER_MSEC, NULL);
    }

    goal_logInfo("Task %s: ready flag detected, starting demo", pTask->pName);

    while (GOAL_TRUE) {

        /* request lock or restart request if lock wasn't available */
        res = goal_lockGet(gpLckSync, GOAL_LOCK_INFINITE);
        if (GOAL_RES_ERR(res)) {
            continue;
        }

        /* hold lock a while, set task priority to low */
        goal_logInfo("Task %s: lock aquired", pTask->pName);
        goal_logInfo("Task %s: priority will be decreased", pTask->pName);
        goal_taskPrioSet(pTask, APPL_TASK_PRIO_LOW);
        goal_logInfo("Task %s: priority is low", pTask->pName);
        goal_logInfo("Task %s: start waiting", pTask->pName);
        goal_taskMsSleep(timeSleep, NULL);

        /* release lock, set task priority to high */
        goal_logInfo("Task %s: lock will be released", pTask->pName);
        goal_lockPut(gpLckSync);
        goal_logInfo("Task %s: lock released", pTask->pName);
        goal_logInfo("Task %s: priority will be increased", pTask->pName);
        goal_taskPrioSet(pTask, APPL_TASK_PRIO_HIGH);
        goal_logInfo("Task %s: priority is high", pTask->pName);
        goal_logInfo("Task %s: try to get lock", pTask->pName);
    }
}
