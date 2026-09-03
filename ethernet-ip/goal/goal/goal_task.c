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

#define GOAL_ID GOAL_ID_TASK
#include <goal_includes.h>


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/
static GOAL_STATUS_T goal_taskInit(
    void
);

#if GOAL_CONFIG_TASK_STACK_MONITOR == 1
static GOAL_STATUS_T goal_taskStackMonitor(
    void *pArg                                  /**< task argument */
);
#endif


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_TASK_T *lstTasks = NULL;            /**< task list */
static GOAL_STAGE_HANDLER_T stageInit;          /**< init stage handler */
#if GOAL_CONFIG_TASK_STACK_MONITOR == 1
static GOAL_TASK_T *pTaskStackMonitor;          /**< stack monitor task */
#endif


/****************************************************************************/
/** Task Pre-Initialization
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_taskInitPre(
    void
)
{
    /* task pre-stage */
    return goal_mainStageReg(GOAL_STAGE_TASK_PRE, &stageInit, GOAL_STAGE_INIT, goal_taskInit);
}


/****************************************************************************/
/** Initialize task module
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_taskInit(
    void
)
{
#if GOAL_CONFIG_TASK_STACK_MONITOR == 1
    GOAL_STATUS_T res;                          /* result */

    /* start stack monitor task */
    res = goal_taskCreate(&pTaskStackMonitor, "GOAL Task Stack Monitor", goal_taskStackMonitor, NULL, GOAL_TASK_PRIO_LOWEST, GOAL_TASK_STACK_DEFAULT, GOAL_TRUE);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create task stack monitor task");
        return res;
    }
#endif

    return GOAL_OK;
}


/****************************************************************************/
/** Shutdown task module
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_taskShutdown(
    void
)
{
    return GOAL_OK;
}


/****************************************************************************/
/** Create a new task
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_taskCreate(
    GOAL_TASK_T **ppTask,                       /**< task handle */
    const char * const strName,                 /**< task name */
    GOAL_TASK_FUNC_T func,                      /**< task function */
    void *pArg,                                 /**< task argument */
    uint32_t prio,                              /**< task priority */
    uint32_t stack,                             /**< task stack size */
    GOAL_BOOL_T flgStart                        /**< task start flag */
)
{
    GOAL_STATUS_T res;                          /* result */

    GOAL_ASSERT(NULL != ppTask);
    GOAL_ASSERT(NULL == *ppTask);

    /* GG_LOG ID:236: */
    goal_lmLog(GOAL_ID_TASK, 236, 191, GOAL_STRLEN((const char *) strName), 0, GOAL_LOG_SEV_INFO, "creating task: $1");
    goal_lmLogParamSTRING0(strName);
    goal_lmLogFinish();

    /* allocate task structure */
    res = goal_memCalloc(ppTask, sizeof(GOAL_TASK_T));
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:237: */
        goal_lmLog(GOAL_ID_TASK, 237, 192, 0, 0, GOAL_LOG_SEV_ERROR, "error allocating task structure");
        return res;
    }

    /* assign task data */
    (*ppTask)->pName = strName;
    (*ppTask)->func = func;
    (*ppTask)->pArg = pArg;
    (*ppTask)->prio = prio;
    (*ppTask)->stack = stack;

    /* set initial task state to wait */
    (*ppTask)->state = GOAL_TASK_STATE_WAIT;

    /* initialize target task */
    res = goal_tgtTaskCreate(*ppTask);
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:238: */
        goal_lmLog(GOAL_ID_TASK, 238, 193, 0, 0, GOAL_LOG_SEV_ERROR, "error creating task in target driver");
        goal_memFree(ppTask);
        return res;
    }

    /* add task to task list */
    GOAL_LL_APPEND(lstTasks, *ppTask);

    /* if start flag is set, start task */
    if (flgStart) {
        (*ppTask)->state = GOAL_TASK_STATE_RUN;
        goal_tgtTaskStart(*ppTask);
    }


    /* output task creation summary */
    goal_logInfo("Created task %s with priority %" FMT_u32 "/100 and stack %" FMT_u32 " byte successfully.",
        (*ppTask)->pName, (*ppTask)->prio, (*ppTask)->stack);

    return GOAL_OK;
}


/****************************************************************************/
/** Initiate current task shutdown
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_taskExit(
    void
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_TASK_T *pTask;                         /* task data */

    /* get current task handle */
    res = goal_taskSelf(&pTask);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* exit current task */
    res = goal_tgtTaskExit();
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* set task state to exit */
    pTask->state = GOAL_TASK_STATE_EXIT;

    return GOAL_OK;
}


/****************************************************************************/
/** Start a task
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_taskStart(
    GOAL_TASK_T *pTask                          /**< GOAL task handle */
)
{
    /* set task state to run and inform target */
    pTask->state = GOAL_TASK_STATE_RUN;

    return goal_tgtTaskStart(pTask);
}


/****************************************************************************/
/** Get task priority
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_taskPrioGet(
    GOAL_TASK_T *pTask,                         /**< task handle */
    uint32_t *pPrio                             /**< priority ref */
)
{
    if ((NULL == pTask) || (NULL == pPrio)) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* get task priority */
    return goal_tgtTaskPrioGet(pTask, pPrio);
}


/****************************************************************************/
/** Set task priority
 *
 * This overrides a possibly existing mapping table in the target.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_taskPrioSet(
    GOAL_TASK_T *pTask,                         /**< task handle */
    uint32_t prio                               /**< priority */
)
{
    if (NULL == pTask) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* set task priority */
    return goal_tgtTaskPrioSet(pTask, prio);
}


/****************************************************************************/
/** Suspend task execution
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_taskSuspend(
    GOAL_TASK_T *pTask                          /**< task handle */
)
{
    /* null pointer validation */
    if (NULL == pTask) {
        return GOAL_ERR_NULL_POINTER;
    }

    return goal_tgtTaskSuspend(pTask);
}


/****************************************************************************/
/** Resume task execution
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_taskResume(
    GOAL_TASK_T *pTask                          /**< task handle */
)
{
    /* null pointer validation */
    if (NULL == pTask) {
        return GOAL_ERR_NULL_POINTER;
    }

    return goal_tgtTaskResume(pTask);
}


/****************************************************************************/
/** Force task switch
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_taskYield(
    void
)
{
    return GOAL_OK;
}


/****************************************************************************/
/** Put task to millisecond sleep
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_taskMsSleep(
    uint32_t msReq,                             /**< requested sleep */
    uint32_t *pMsRem                            /**< remaining sleep */
)
{
    return goal_tgtTaskMsSleep(msReq, pMsRem);
}


/****************************************************************************/
/** Get current task ID
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_taskSelf(
    GOAL_TASK_T **ppTask                        /**< task handle */
)
{
    GOAL_STATUS_T res;                          /* result */

    /* find matching task structure */
    GOAL_LL_FOREACH(lstTasks, *ppTask) {
        res = goal_tgtTaskTestSelf(*ppTask);
        if (GOAL_RES_OK(res)) {
            return GOAL_OK;
        }
    }

    *ppTask = NULL;
    return GOAL_ERR_TASK_SELF;
}


/****************************************************************************/
/** Wait for task result
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_taskJoin(
    void
)
{
    return GOAL_OK;
}


/****************************************************************************/
/** Wait for signal
 *
 * If no timeout is needed it must be set to zero.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_taskSigWait(
    unsigned int timeout                        /**< timeout */
)
{
    UNUSEDARG(timeout);

    return GOAL_OK;
}


/****************************************************************************/
/** Send signal to task
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_taskSig(
    void
)
{
    return GOAL_OK;
}


/****************************************************************************/
/** Send signal to all tasks
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_taskSigBroadcast(
    void
)
{
    return GOAL_OK;
}


#if GOAL_CONFIG_TASK_STACK_MONITOR == 1
/****************************************************************************/
/** Show the stack usage of the created tasks
 *
 * The report will also include this task.
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_taskStackMonitor(
    void *pArg                                  /**< task argument */
)
{
    GOAL_TASK_T *pTask;                         /* task handle */
    unsigned int cnt;                           /* unused bytes counter */

    UNUSEDARG(pArg);

    while (GOAL_FALSE == GOAL_flagShutdown) {

        /* delay check */
        goal_taskMsSleep(GOAL_CONFIG_TASK_STACK_MONITOR_SLEEP_MS, NULL);

        /* iterate through tasks */
        GOAL_LL_FOREACH(lstTasks, pTask) {

            /* skip if stack pointer isn't set */
            if (NULL == pTask->pStack) {
                continue;
            }

            /* count unused stack memory */
            for (cnt = 0; (cnt < pTask->stack) && (GOAL_CONFIG_TASK_STACK_PAINT == pTask->pStack[cnt]); cnt++);

            /* report used stack */
            goal_logInfo("Task: %s, Stack: %"FMT_u32", Usage: %"FMT_u32" ", pTask->pName, pTask->stack, pTask->stack - cnt);
        }
    }

    return GOAL_OK;
}
#endif /* GOAL_CONFIG_TASK_STACK_MONITOR == 1 */


/****************************************************************************/
/** Test for GOAL main task
 *
 * Test if this function is called from the GOAL main task "GOAL Main".
 * If this main task was not created by goal_taskCreate(), this function will
 * return GOAL_ERROR always.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_taskGoalMainCheck(
    void
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_TASK_T *pTask;                         /* task data */

    /* find matching task structure */
    res = goal_taskSelf(&pTask);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* compare name to name of GOAL Main task */
    if (0 != GOAL_STRCMP(pTask->pName, "GOAL Main")) {
        /* not the GOAL Main task */
        return GOAL_ERROR;
    }

    /* the GOAL Main task */
    return GOAL_OK;
}
