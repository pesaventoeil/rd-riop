/** @file
 *
 * @brief GOAL Task Module
 *
 * This module implements task handling. It doesn't directly enforce a scheme
 * like threads or separate processes. The target is to provide an API to put
 * work into a separate/parallel timeline.
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
#ifndef GOAL_TASK_H
#define GOAL_TASK_H


/****************************************************************************/
/* Defines */
/****************************************************************************/

#define GOAL_TASK_STATE_WAIT    0               /**< wait for execution */
#define GOAL_TASK_STATE_RUN     1               /**< in execution */
#define GOAL_TASK_STATE_EXIT    2               /**< exit */

#define GOAL_TASK_STACK_DEFAULT 0               /**< default stack size */

#define GOAL_TASK_PRIO_LOWEST   0               /**< lowest priority */
#define GOAL_TASK_PRIO_HIGHEST  100             /**< highest priority */
#define GOAL_TASK_PRIO_DEFAULT  50              /**< default priority */


/****************************************************************************/
/* Typedefs */
/****************************************************************************/

/**< GOAL task function */
typedef GOAL_STATUS_T (* GOAL_TASK_FUNC_T)(void *);


/****************************************************************************/
/* Structures */
/****************************************************************************/

/**< GOAL task data */
typedef struct GOAL_TASK_T {
    const char * pName;                         /**< task name */
    GOAL_TASK_FUNC_T func;                      /**< task function */
    void *pArg;                                 /**< task function argument */
    uint32_t stack;                             /**< task stack size */
    uint32_t prio;                              /**< task priority */

    uint32_t join;                              /**< join value */
    unsigned int state;                         /**< state */

    void *pTgt;                                 /**< target data */
    struct GOAL_TASK_T *pNext;                  /**< next task */

#if GOAL_CONFIG_TASK_STACK_MONITOR == 1
    uint8_t *pStack;                            /**< task stack pointer */
#endif
} GOAL_TASK_T;


/****************************************************************************/
/* Prototypes */
/****************************************************************************/

GOAL_STATUS_T goal_taskInitPre(
    void
);

GOAL_STATUS_T goal_taskShutdown(
    void
);

GOAL_STATUS_T goal_taskCreate(
    GOAL_TASK_T **ppTask,                       /**< task handle */
    const char * const strName,                 /**< task name */
    GOAL_TASK_FUNC_T func,                      /**< task function */
    void *pArg,                                 /**< task argument */
    uint32_t prio,                              /**< task priority */
    uint32_t stack,                             /**< task stack size */
    GOAL_BOOL_T flgStart                        /**< task start flag */
);

GOAL_STATUS_T goal_taskExit(
    void
);

GOAL_STATUS_T goal_taskStart(
    GOAL_TASK_T *pTask                          /**< task handle */
);

GOAL_STATUS_T goal_taskPrioGet(
    GOAL_TASK_T *pTask,                         /**< task handle */
    uint32_t *pPrio                             /**< priority ref */
);

GOAL_STATUS_T goal_taskPrioSet(
    GOAL_TASK_T *pTask,                         /**< task handle */
    uint32_t prio                               /**< priority */
);

GOAL_STATUS_T goal_taskSuspend(
    GOAL_TASK_T *pTask                          /**< task handle */
);

GOAL_STATUS_T goal_taskResume(
    GOAL_TASK_T *pTask                          /**< task handle */
);

GOAL_STATUS_T goal_taskYield(
    void
);

GOAL_STATUS_T goal_taskMsSleep(
    uint32_t msReq,                             /**< requested sleep */
    uint32_t *pMsRem                            /**< remaining sleep */
);

GOAL_STATUS_T goal_taskSelf(
    GOAL_TASK_T **ppTask                        /**< task handle */
);

GOAL_STATUS_T goal_taskJoin(
    void
);

GOAL_STATUS_T goal_taskSigWait(
    unsigned int timeout                        /**< timeout */
);

GOAL_STATUS_T goal_taskSig(
    void
);

GOAL_STATUS_T goal_taskSigBroadcast(
    void
);

GOAL_STATUS_T goal_taskGoalMainCheck(
    void
);

#endif /* GOAL_TASK_H */
