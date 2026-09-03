/** @file
 *
 * @brief Low Level Interface of GOAL
 *
 * This module binds GOAL to a specific hardware or OS. It provides a generic
 * API for GOAL.
 *
 * @copyright
 * Copyright 2010-2022 port GmbH Halle/Saale.
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
#include "goal_target_ext.h"
#include "goal_target_freertos.h"

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"

#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"

#if GOAL_CONFIG_MM_EXT == 1
#  include <plat/drv/mem/heap/freertos/mem_heap_freertos.h>
#endif


/****************************************************************************/
/* Configuration */
/****************************************************************************/
#ifndef GOAL_CONFIG_TARGET_FREERTOS_TIMER_SUPPORT
#   define GOAL_CONFIG_TARGET_FREERTOS_TIMER_SUPPORT 0
#endif

#ifndef GOAL_TARGET_LOCK_INT_ENABLE
# define GOAL_TARGET_LOCK_INT_ENABLE
#endif

#ifndef GOAL_TARGET_LOCK_INT_DISABLE
# define GOAL_TARGET_LOCK_INT_DISABLE
#endif

#ifndef GOAL_CONFIG_TARGET_FREERTOS_TARGET_MAIN
# define GOAL_CONFIG_TARGET_FREERTOS_TARGET_MAIN 0
#endif

#ifndef GOAL_CONFIG_TARGET_FREERTOS_USE_GOAL_HEAP
# define GOAL_CONFIG_TARGET_FREERTOS_USE_GOAL_HEAP 0
#endif

#ifndef GOAL_CONFIG_TARGET_FREERTOS_TIMER_TICK_FROM_HOOK
# define GOAL_CONFIG_TARGET_FREERTOS_TIMER_TICK_FROM_HOOK 0
#endif


/****************************************************************************/
/* Local Defines */
/****************************************************************************/
#define GOAL_TARGET_INIT_TASK_PRIO              (configMAX_PRIORITIES - 1) /**< FreeRTOS priority of init task */
#define GOAL_TARGET_INIT_TASK_STACK_SIZE        (0x1000 / sizeof(StackType_t)) /**< stack size of init task (x * sizeof(StackType_t) bytes) */
#define GOAL_TARGET_MAIN_TASK_PRIO              1 /**< (GOAL) priority of main task */
#ifndef GOAL_TARGET_MAIN_TASK_STACK_SIZE
# define GOAL_TARGET_MAIN_TASK_STACK_SIZE       (0x2000 / sizeof(StackType_t)) /**< stack size of main task (x * sizeof(StackType_t) bytes) */
#endif
#define GOAL_TARGET_STACK_SIZE_DEFAULT          (configMINIMAL_STACK_SIZE) /**< default size of a new task in words */

#define GOAL_TARGET_TIMER_WAIT_NO_WAIT          ((TickType_t) 0) /**< number of ticks if a function shall not wait */

/* convert times in ms to ticks */
#if GOAL_TARGET_FREERTOS_FAST_BASETICK == 1
/* mathematically the same as below,
 * but considers integer division would result in 0 if configTICK_RATE_HZ > 1000
 */
# define GOAL_TARGET_MS_TO_TICKS(ms) (ms * configTICK_RATE_HZ / ( TickType_t ) 1000)
# define GOAL_TARGET_TICKS_TO_MS(ticks) (ticks * ( TickType_t ) 1000 / configTICK_RATE_HZ)
#else
/* default conversion if configTICK_RATE_HZ <= 1000 */
# define GOAL_TARGET_MS_TO_TICKS(ms) (ms / portTICK_PERIOD_MS)
# define GOAL_TARGET_TICKS_TO_MS(ticks) (ticks * portTICK_PERIOD_MS)
#endif /* GOAL_TARGET_FREERTOS_FAST_BASETICK */


/****************************************************************************/
/* Local Macros */
/****************************************************************************/
#define GOAL_TARGET_TASK_PRIO_TO_TARGET(prio)   ((prio * (configMAX_PRIORITIES - 1) / GOAL_TASK_PRIO_HIGHEST))
#define GOAL_TARGET_TASK_PRIO_TO_GOAL(prio)     ((prio * GOAL_TASK_PRIO_HIGHEST) / (configMAX_PRIORITIES - 1))

#define GOAL_TARGET_INTERRUPT_START             GOAL_TARGET_LOCK_INT_DISABLE; \
                                                flgInt = GOAL_TRUE;
#define GOAL_TARGET_INTERRUPT_END               GOAL_TARGET_LOCK_INT_ENABLE; \
                                                flgInt = GOAL_FALSE;


/****************************************************************************/
/* Local Prototypes */
/****************************************************************************/
#if GOAL_CONFIG_TARGET_FREERTOS_TARGET_MAIN == 1
static void goal_tgtInit(
    void *pArg                                  /**< task argument */
);
#endif

static GOAL_STATUS_T goal_targetTaskLoop(
    void *pArg                                  /**< task argument */
);

#if GOAL_CONFIG_MM_EXT == 1
static GOAL_STATUS_T goal_stageTgtMemPreInit(
    void
);
#endif

static GOAL_STATUS_T goal_stageTgtTaskInit(
    void
);

static GOAL_STATUS_T goal_stageTgtTaskShutdown(
    void
);

#if (GOAL_CONFIG_TARGET_FREERTOS_TIMER_SUPPORT == 1)
static void goal_tgtTimerCbWrapper(
    TimerHandle_t hdlTmr                        /**< timer handle */
);
#endif /* (GOAL_CONFIG_TARGET_FREERTOS_TIMER_SUPPORT == 1) */

static void goal_tgtTaskWrapper(
    void *pTask                                 /**< GOAL task handle */
);

GOAL_TARGET_WEAK GOAL_BOOL_T goal_targetCommonIsrCheck(
    void
);

GOAL_TARGET_WEAK GOAL_STATUS_T goal_targetMainPreOsCb(
    void
);


/****************************************************************************/
/* Local Variables */
/****************************************************************************/
#if GOAL_CONFIG_TARGET_FREERTOS_TARGET_MAIN == 1
static TaskHandle_t taskInit;                   /**< GOAL init task handle */
#endif
static GOAL_BOOL_T flgInt = GOAL_FALSE;         /**< Interrupt running flag */

/* Stage handler */
static GOAL_STAGE_HANDLER_T stageTgtTaskInit;      /**< target task init stage handler */
#if GOAL_CONFIG_MM_EXT == 1
static GOAL_STAGE_HANDLER_T stageTgtMemPreInit;    /**< target memory pre init stage handler */
#endif
static GOAL_STAGE_HANDLER_T stageTgtTaskShutdown;  /**< target task shutdown stage handler */

/* Timer related variables */
volatile GOAL_TIMESTAMP_T tickCounter = 0;      /**< global tick counter */

static GOAL_TASK_T *pGoalTask = NULL;           /**< GOAL task handle */

static GOAL_TGT_TASK_MAP_T *mpTaskMap;          /**< task map data */
static uint32_t mCntTaskMap = 0;                /**< task map data elements */


#if GOAL_CONFIG_TARGET_FREERTOS_TARGET_MAIN == 1
/****************************************************************************/
/** Global Main Entry
 *
 * This function creates a FreeRTOS task for GOAL initialization and starts
 * the scheduler afterwards to provide it's features (timer/ tasks) on init.
 * Due to missing GOAL tasks in this early state, GOAL init is called by a
 * FreeRTOS task.
 *
 * @returns System Exit Code (always 0)
 */
int main(
    int argc,                                   /**< argument count */
    char **argv                                 /**< arguments */
)
{
    BaseType_t resFreeRtos;                     /* freeRTOS result */

    UNUSEDARG(argc);
    UNUSEDARG(argv);

    goal_targetMainPreOsCb();

    /* create GOAL init task */
    resFreeRtos = xTaskCreate((TaskFunction_t) goal_tgtInit,
                                "GOAL init",
                                GOAL_TARGET_INIT_TASK_STACK_SIZE,
                                NULL,
                                GOAL_TARGET_INIT_TASK_PRIO,
                                &taskInit);

    if (pdPASS == resFreeRtos) {
        /* Start the scheduler, this function will return only in case of an error */
        vTaskStartScheduler();
    }
    return 0;
}


/****************************************************************************/
/** GOAL initialization Task
 *
 * This function is a FreeRTOS task and calls the GOAL initialization before
 * shutting itself down.
 * Running goal_loop in this task is not sufficient, because this function is
 * not a part of the linked list of GOAL tasks.
 *
 * @returns GOAL_STATUS_T result
 */
static void goal_tgtInit(
    void *pArg                                  /**< task argument */
)
{
    GOAL_STATUS_T res;                          /* result */

    UNUSEDARG(pArg);

    /* call goal_init and exit afterwards */
    res = goal_init();

    if (GOAL_RES_OK(res)) {
        /* give goal main task a signal to start it's loop */
        goal_taskResume(pGoalTask);

        /* delete the init task after GOAL has been created successful */
        goal_logDbg("deleting FreeRTOS init task");
        vTaskDelete(taskInit);
    }
    else {
       goal_targetHalt();
    }

    /* task should self-delete if end is reached */
    /* should not happen in normal execution */
    goal_logDbg("reached impossible state during FreeRTOS init");
    vTaskDelete(NULL);
}
#endif


/****************************************************************************/
/** main task (goal_loop)
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_targetTaskLoop(
    void *pArg                                  /**< task argument */
)
{
    GOAL_TASK_T *pMainTask;                     /* main goal task */

    pMainTask = (GOAL_TASK_T *) pArg;

    /* sleep until further notice */
    goal_taskSuspend(pMainTask);

    /* loop until the shutdown flag is set */
    while (!GOAL_flagShutdown) {
        goal_loop();
        taskYIELD();
    }

    /* exit this task */
    return goal_taskExit();
}


#if GOAL_CONFIG_TARGET_FREERTOS_TIMER_TICK_FROM_HOOK == 1

/* use Tick Hook as source for timestamp */
# if (configUSE_TICK_HOOK == 1)
/****************************************************************************/
/** FreeRtos Application Tick Hook
 *
 * The tick hook gets called at regular intervals, even if the scheduler is
 * locked.
 */
void vApplicationTickHook(
    void
)
{
    /* increas GOAL 64 bit tick counter */
    tickCounter++;
}
# else
#  error "Need configUSE_TICK_HOOK enabled in freertos configuration"
# endif
#else
/****************************************************************************/
/** FreeRtos Tick Timer Handler
 *
 * The tick timer handler is called by the architecture combined with freertos
 */
void goal_tgtSystickHandler(
    void
)
{
    tickCounter++;
    goal_tgtCommonSystickHandler();
}
#endif


#if (configUSE_IDLE_HOOK == 1)
/****************************************************************************/
/** FreeRtos Application Idle Hook
 *
 * Idle task callback. The user might add some background functionality.
 *
 * NOTE: vApplicationIdleHook() MUST NOT, UNDER ANY CIRCUMSTANCES,
 * CALL A FUNCTION THAT MIGHT BLOCK.
 *
 */
void vApplicationIdleHook(
    void
)
{
    /* not in use */
}
#endif


#if (configCHECK_FOR_STACK_OVERFLOW > 0)
/****************************************************************************/
/** FreeRtos Application Stack Overflow Hook
 *
 */
void vApplicationStackOverflowHook(
    TaskHandle_t pTask,                         /**< task handler */
    char *pcTaskName                            /**< task name */
)
{
    UNUSEDARG(pTask);
    UNUSEDARG(pcTaskName);

    goal_logErr("Stack overflow in %s", pcTaskName);
}
#endif


/****************************************************************************/
/** Register stage handler
 *
 * This function registers the primary target initialization function and the
 * shutdown function to the GOAL stage system.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_targetInitPre(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* init stages */
    res = goal_mainStageReg(GOAL_STAGE_TASK, &stageTgtTaskInit, GOAL_STAGE_INIT, goal_stageTgtTaskInit);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Failed to register target task init function.");
    }

#if GOAL_CONFIG_MM_EXT == 1
    if (GOAL_RES_OK(res)) {
        res = goal_mainStageReg(GOAL_STAGE_MEM_PRE, &stageTgtMemPreInit, GOAL_STAGE_INIT, goal_stageTgtMemPreInit);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("Failed to register target task init function.");
        }
    }
#endif

    /* shutdown stages */
    if (GOAL_RES_OK(res)) {
        res = goal_mainStageReg(GOAL_STAGE_TASK, &stageTgtTaskShutdown, GOAL_STAGE_SHUTDOWN, goal_stageTgtTaskShutdown);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("Failed to register target task shutdown function.");
        }
    }

    /* call common target init function */
    if (GOAL_RES_OK(res)) {
        res = goal_targetCommonInitPre();
    }
    return res;
}


#if GOAL_CONFIG_MM_EXT == 1
/****************************************************************************/
/** Target memory pre Initialization
 *
 * This function initializes the target memory by registration of the heap
 * wrapper.
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_stageTgtMemPreInit(
    void
)
{
    return goal_drvMemHeapCreate();
}
#endif /* GOAL_CONFIG_MM_EXT == 1 */


/****************************************************************************/
/** Target task Initialization
 *
 * This function initializes the target task by creating the GOAL loop task.
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_stageTgtTaskInit(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    res = goal_taskCreate(&pGoalTask, "GOAL Main", goal_targetTaskLoop, pGoalTask, GOAL_TARGET_MAIN_TASK_PRIO, GOAL_TARGET_MAIN_TASK_STACK_SIZE, GOAL_TRUE);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create main task");
        goal_targetHalt();
    }

    return res;
}


/****************************************************************************/
/** Target Task shutdown
 *
 * Halts the target.
 *
 * @note On some architectures this halts the device.
 */
static GOAL_STATUS_T goal_stageTgtTaskShutdown(
    void
)
{
    /* halt the target */
    goal_targetHalt();
    return GOAL_OK;
}


/****************************************************************************/
/** Create a new task
 *
 * Tasks are createdy dynamic by default. The user can set a task map by
 * goal_tgtTaskMapSet, which lists static tasks.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_tgtTaskCreate(
    GOAL_TASK_T *pTask                          /**< GOAL task handle */
)
{
    BaseType_t resFreeRtos;                     /* freeRTOS result */
    configSTACK_DEPTH_TYPE prio;                /* stack priority */
    uint32_t cnt;                               /* loop counter */

    goal_logDbg("free heap before task creation: %u bytes", xPortGetFreeHeapSize());

    prio = GOAL_TARGET_TASK_PRIO_TO_TARGET(pTask->prio);

    for (cnt = 0; cnt < mCntTaskMap; cnt++) {
        if (GOAL_CMP_EQUAL == GOAL_STRCMP(mpTaskMap[cnt].strName, pTask->pName)) {
            break;
        }
    }

    if (cnt >= mCntTaskMap) {
#if configSUPPORT_DYNAMIC_ALLOCATION == 1
        /* task not found in mapping table -> create dynamic */
        goal_logInfo("creating dynamic task %s on prio %"FMT_u32, pTask->pName, (uint32_t) prio);
        resFreeRtos = xTaskCreate((TaskFunction_t) goal_tgtTaskWrapper,
                                    pTask->pName,
                                    (GOAL_TASK_STACK_DEFAULT == pTask->stack) ? GOAL_TARGET_STACK_SIZE_DEFAULT : pTask->stack,
                                    pTask,
                                    prio,
                                    (TaskHandle_t *) &pTask->pTgt);
#else
        goal_logErr("dynamic task creation is not supported");
        resFreeRtos = pdFALSE;
#endif
    } else {
#if configSUPPORT_STATIC_ALLOCATION == 1
        /* task found in mapping table -> create static */
        goal_logInfo("creating static task %s on prio %"FMT_u32, pTask->pName, (uint32_t) prio);
        pTask->pTgt = xTaskCreateStatic((TaskFunction_t) goal_tgtTaskWrapper,
                                        pTask->pName,
                                        mpTaskMap[cnt].stackSize / sizeof(StackType_t),
                                        pTask,
                                        prio,
                                        (StackType_t *) mpTaskMap[cnt].pStack,
                                        &mpTaskMap[cnt].taskHandle);
        resFreeRtos = (NULL != pTask->pTgt) ? pdPASS : pdFALSE;
#else
        goal_logErr("static task creation is not supported");
        resFreeRtos = pdFALSE;
#endif
    }

    goal_logDbg("free heap after task creation: %u bytes", xPortGetFreeHeapSize());

    return (pdPASS == resFreeRtos) ? GOAL_OK : GOAL_ERROR;
}


/****************************************************************************/
/** Set task map data
 *
 * Set a list of tasks, which shall be created as static tasks.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_tgtTaskMapSet(
    GOAL_TGT_TASK_MAP_T *pTaskMap,              /**< task map data */
    unsigned int cntTaskMap                     /**< task map data count */
)
{
    mpTaskMap = pTaskMap;
    mCntTaskMap = cntTaskMap;

    return GOAL_OK;
}


/****************************************************************************/
/** Start a task
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_tgtTaskStart(
    GOAL_TASK_T *pTask                          /**< GOAL task handle */
)
{
    return goal_taskResume(pTask);
}


/****************************************************************************/
/** Task wrapper
 *
 * @returns GOAL_STATUS_T result
 */
static void goal_tgtTaskWrapper(
    void *pTask                                 /**< GOAL task handle */
)
{
    GOAL_STATUS_T res;                          /* result */
#if configUSE_APPLICATION_TASK_TAG == 1
    static uint32_t taskId = 1;                 /* task id counter, id 0 is reserved for IDLE task */
#endif

#if configUSE_APPLICATION_TASK_TAG == 1
    vTaskSetApplicationTaskTag(NULL, (void *) taskId);
    taskId++;
#endif

    do {
        /* wait for task start signal */
        res = goal_taskSuspend((GOAL_TASK_T *) pTask);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to start task %s", ((GOAL_TASK_T *) pTask)->pName);
            return;
        }
    } while (GOAL_TASK_STATE_RUN != ((GOAL_TASK_T *) pTask)->state);

    /* run task until GOAL or the task is shut down */
    while ((!GOAL_flagShutdown) && (GOAL_TASK_STATE_RUN == ((GOAL_TASK_T *) pTask)->state)) {
        ((GOAL_TASK_T *) pTask)->func(((GOAL_TASK_T *) pTask)->pArg);
        taskYIELD();
    }

    /* task should self-delete if end is reached */
    /* should not happen in normal execution */
    vTaskDelete(NULL);
}


/****************************************************************************/
/** Initiate current task shutdown
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_tgtTaskExit(
    void
)
{
    /* tasks end after they are done */
    return GOAL_OK;
}


/****************************************************************************/
/** Put task to millisecond sleep
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_tgtTaskMsSleep(
    uint32_t msReq,                             /**< ms requested to sleep */
    uint32_t *pMsRem                            /**< remaining ms if interrupted */
)
{
    vTaskDelay(GOAL_TARGET_MS_TO_TICKS(msReq));

    if (NULL != pMsRem) {
        *pMsRem = 0;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Check if given task matches our task id
 *
 * @retval GOAL_OK task ids are the same
 * @retval other task ids differ
 */
GOAL_STATUS_T goal_tgtTaskTestSelf(
    GOAL_TASK_T *pTask                          /**< task handle */
)
{
    GOAL_STATUS_T res;                          /* result */
    TaskHandle_t task;                          /* current task handle */

    task = xTaskGetCurrentTaskHandle();

    if (task == pTask->pTgt) {
        res = GOAL_OK;
    }
    else {
        res = GOAL_ERR_NOT_FOUND;
    }

    return res;
}


/****************************************************************************/
/** Get task priority
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_tgtTaskPrioGet(
    GOAL_TASK_T *pTask,                         /**< task handle */
    uint32_t *pPrio                             /**< priority ref */
)
{
    UBaseType_t prio;                           /* priority of task */

    if (GOAL_TRUE == goal_targetCommonIsrCheck()) {
        prio = uxTaskPriorityGetFromISR(pTask->pTgt);
    }
    else {
        prio = uxTaskPriorityGet(pTask->pTgt);
    }

    *pPrio = (uint32_t) GOAL_TARGET_TASK_PRIO_TO_GOAL(prio);
    return GOAL_OK;
}


/****************************************************************************/
/** Set task priority
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_tgtTaskPrioSet(
    GOAL_TASK_T *pTask,                         /**< task handle */
    uint32_t prio                               /**< priority */
)
{
    vTaskPrioritySet(pTask->pTgt, (UBaseType_t) GOAL_TARGET_TASK_PRIO_TO_TARGET(prio));
    pTask->prio = prio;

    return GOAL_OK;
}


/****************************************************************************/
/** Suspend task execution
 *
 * The task suspends by waiting for a notification.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_tgtTaskSuspend(
    GOAL_TASK_T *pTask                          /**< task handle */
)
{
    UNUSEDARG(pTask);
    BaseType_t flgClearCountOnExit = pdFALSE;   /* clear the notify counter on exit */

    /* suspend until a notificatoin was received */

    /* If INCLUDE_vTaskSuspend is defined and the second argument
     * is GOAL_TARGET_TASK_MAX_DELAY, ulTaskNotifyTake blocks infinity. */
    ulTaskNotifyTake(flgClearCountOnExit, GOAL_TARGET_TASK_MAX_DELAY);

    return GOAL_OK;
}


/****************************************************************************/
/** Resume task execution
 *
 * The task is resumed by sending a notification.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_tgtTaskResume(
    GOAL_TASK_T *pTask                          /**< task handle */
)
{
    BaseType_t resFreeRtos = pdFALSE;           /* freeRTOS result */

    UNUSEDARG(resFreeRtos);
    if (GOAL_TRUE == goal_targetCommonIsrCheck()) {
        /* send notification */
        vTaskNotifyGiveFromISR((TaskHandle_t) pTask->pTgt, &resFreeRtos);
        /* perform a context switch */
        portYIELD_FROM_ISR(resFreeRtos);
    } else {
        xTaskNotifyGive((TaskHandle_t) pTask->pTgt);
    }

    return GOAL_OK;
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
    return (GOAL_TIMESTAMP_T) GOAL_TARGET_TICKS_TO_MS(tickCounter);
}


#if (GOAL_CONFIG_TARGET_FREERTOS_TIMER_SUPPORT == 1)
/****************************************************************************/
/** Timer Init
 *
 * Initializes low level timer management.
 *
 * @retval GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_targetTimerInit(
    void
)
{
    /* not implemented */
    return GOAL_OK;
}


/****************************************************************************/
/** Create Timer
 *
 * Creates a FreeRTOS timer for the given timer.
 *
 * The created FreeRTOS timer is declared as periodic timer and will be stoped
 * after first execution if GOAL timer is non-periodic.
 *
 * For each FreeRTOS timer an ID must be registered. In this case it is the
 * GOAL timer pointer belonging to the FreeRTOS timer.
 *
 * @retval GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_targetTimerCreate(
    GOAL_TIMER_T *pTmr                          /**< timer */
)
{
    TimerHandle_t pTgt;                         /* target timer handle */

    /* create empty timer */
    pTgt = xTimerCreate("",                     /* timer name */
                        (const TickType_t) portMAX_DELAY, /* timer period or time */
                        pdTRUE,                 /* periodic timer flag */
                        (void * const) pTmr,    /* timer ID which can be retrieved in callback */
                        goal_tgtTimerCbWrapper  /* timer callback */
                        );

    /* check for creation error */
    if (NULL == pTgt) {
        return GOAL_ERR_TIMER_CREATE;
    }

    /* store target timer handle */
    pTmr->pTgt = (void *) pTgt;

    return GOAL_OK;
}


/****************************************************************************/
/** Start Timer
 *
 * Start the given timer.
 *
 * @retval GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_targetTimerStart(
    GOAL_TIMER_T *pTmr                          /**< timer */
)
{
    BaseType_t resFreertos;                     /* FreeRTOS result */
    BaseType_t flgHigherPrioTaskWoken = pdFALSE; /* higher priority task will be woken after interrupt ends */

    /* setup period or timer time */
    if (GOAL_TRUE == goal_targetCommonIsrCheck()) {
        resFreertos = xTimerChangePeriodFromISR(pTmr->pTgt, GOAL_TARGET_MS_TO_TICKS((TickType_t) pTmr->period), &flgHigherPrioTaskWoken);
    }
    else {
        resFreertos = xTimerChangePeriodFromISR(pTmr->pTgt, GOAL_TARGET_MS_TO_TICKS((TickType_t) pTmr->period), &flgHigherPrioTaskWoken);
    }
    if (pdPASS != resFreertos) {
        return GOAL_ERR_TIMER_START;
    }

    /* start FreeRTOS timer */
    if (GOAL_TRUE == goal_targetCommonIsrCheck()) {
        resFreertos = xTimerStartFromISR(pTmr->pTgt, &flgHigherPrioTaskWoken);
    }
    else {
        resFreertos = xTimerStart(pTmr->pTgt, GOAL_TARGET_TIMER_WAIT_NO_WAIT);
    }
    if (pdPASS != resFreertos) {
        return GOAL_ERR_TIMER_START;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Stop Timer
 *
 * Stop the given timer.
 *
 * @retval GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_targetTimerStop(
    GOAL_TIMER_T *pTmr                          /**< timer */
)
{
    BaseType_t resFreertos;                     /* FreeRTOS result */
    BaseType_t flgHigherPrioTaskWoken = pdFALSE; /* higher priority task will be woken after interrupt ends */

    /* stop freeRTOS timer */
    if (GOAL_TRUE == goal_targetCommonIsrCheck()) {
        resFreertos = xTimerStopFromISR(pTmr->pTgt, &flgHigherPrioTaskWoken);
    }
    else {
        resFreertos = xTimerStop(pTmr->pTgt, GOAL_TARGET_TIMER_WAIT_NO_WAIT);
    }
    if (pdPASS != resFreertos) {
        return GOAL_ERR_TIMER_STOP;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Free Timer Resources
 *
 * Cleans up timer resources.
 *
 * @retval GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_targetTimerDelete(
    GOAL_TIMER_T *pTmr                          /**< timer */
)
{
    BaseType_t resFreertos;                     /* FreeRTOS result */

    /* delete freeRTOS timer */
    resFreertos = xTimerDelete(pTmr->pTgt, GOAL_TARGET_TIMER_WAIT_NO_WAIT);
    if (resFreertos != pdPASS) {
        return GOAL_ERR_TIMER_DELETE;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Timer wrapper
 *
 * This wrapper will be called from the FreeRTOS timer task if a registered
 * target timer starts to run.
 *
 * The argument is a FreeRTOS timer handle. The registered timer ID is the
 * GOAL timer handle belonging to the FreeRTOS timer handle.
 *
 * Because FreeRTOS does not allow changing the timer type (periodic or single)
 * after creation, a GOAL single timer will be simulated by stopping the timer
 * after calling the GOAL timer callback.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static void goal_tgtTimerCbWrapper(
    TimerHandle_t hdlTmr                        /**< timer handle */
)
{
    GOAL_TIMER_T *pTmr;                         /* GOAL timer handle */

    /* get registered timer handle from FreeRTOS timer handle */
    pTmr = (GOAL_TIMER_T *) pvTimerGetTimerID(hdlTmr);

    /* excute corresponding GOAL timer */
    goal_timerExec(pTmr);

    /* stop periodic FreeRtosTimer if GOAL timer is nonperiodic */
    if (GOAL_TIMER_SINGLE == pTmr->type) {
        xTimerStop(hdlTmr, GOAL_TARGET_TIMER_WAIT_NO_WAIT);
    }
}
#endif /* (GOAL_CONFIG_TARGET_FREERTOS_TIMER_SUPPORT == 1) */


/****************************************************************************/
/** Initialize Locking
 *
 * Initialize target locking.
 *
 * @return GOAL_OK - success
 * @return other - fail
 */
GOAL_STATUS_T goal_targetLockInit(
    void
)
{
    return GOAL_OK;
}


/****************************************************************************/
/** Shutdown Locking
 *
 * Shutdown target locking.
 *
 * @return GOAL_OK - success
 * @return other - fail
 */
GOAL_STATUS_T goal_targetLockShutdown(
    void
)
{
    return GOAL_OK;
}


/****************************************************************************/
/** Create a Binary or Counting Lock
 *
 * A binary lock represents a mutex, where a counting lock represents a
 * semaphore. When a counting lock is created, the initial and maximal value can
 * be set. For example if 3 instances of a resource are available and 2 should
 * be marked as already in use, set the valInit to 2 and the valMax to 3.
 *
 * It is not necessary to set pLock->type to lockType, because this will be
 * done in goal_lock.c.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_targetLockCreate(
    GOAL_LOCK_TYPE_T lockType,                  /**< GOAL_LOCK_BINARY or GOAL_LOCK_COUNT */
    GOAL_LOCK_T *pLock,                         /**< pointer to store the lock data */
    uint32_t valInit,                           /**< initial lock value */
    uint32_t valMax                             /**< maximal lock value */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    switch (lockType) {
        case GOAL_LOCK_BINARY:
            /* create binary semaphore */
            pLock->pTgt = xSemaphoreCreateRecursiveMutex();
            if (NULL == pLock->pTgt) {
                goal_logErr("failed to create binary semaphore");
                res = GOAL_ERR_ALLOC;
            }
            break;

        case GOAL_LOCK_COUNT:
            /* create counting semaphore */
            pLock->pTgt = xSemaphoreCreateCounting(valMax, valInit);
            if (NULL == pLock->pTgt) {
                goal_logErr("failed to create counting semaphore");
                res = GOAL_ERR_ALLOC;
            }
            break;

        default:
            /* unknown lock type */
            res = GOAL_ERR_UNSUPPORTED;
    }

    return res;
}


/****************************************************************************/
/** Acquire a Lock
 *
 * Enter a binary or counting lock.
 * If this is not possible, the function will sleep until the timeout (in ms)
 * is reached. Setting the timeout to zero, will result in an infinite wait.
 *
 * Make sure the lock can be called recursively by the same thread (or when in
 * IRQ context, by the currently running IRQ). This is for example necessary,
 * because the timer handler locks the timer structure while calling the timer
 * callback and the timer callback must be able to create new or stop active
 * timers.
 *
 * @retval GOAL_OK success
 * @retval GOAL_ERR_LOCK_GET_TIMEOUT timeout reached
 * @retval other fail
 */
GOAL_STATUS_T goal_targetLockGet(
    GOAL_LOCK_T *pLock,                         /**< lock data */
    uint32_t timeout                            /**< lock timeout */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* GOAL result */
    BaseType_t resFreertos;                     /* FreeRTOS result */
    BaseType_t highPrioTask = pdFALSE;          /* higher priority task needs switch */

    /* get FreeRTOS semaphore */
    if (GOAL_TRUE == goal_targetCommonIsrCheck()) {
        resFreertos = xSemaphoreTakeFromISR(pLock->pTgt, &highPrioTask);
        /* perform a context switch */
        portYIELD_FROM_ISR(highPrioTask);
    }
    else if (GOAL_LOCK_COUNT == pLock->type) {
        resFreertos = xSemaphoreTake(pLock->pTgt, (GOAL_LOCK_INFINITE == timeout) ? portMAX_DELAY : timeout);
    }
    else {
        resFreertos = xSemaphoreTakeRecursive(pLock->pTgt, (GOAL_LOCK_INFINITE == timeout) ? portMAX_DELAY : timeout);
    }
    if (pdFALSE == resFreertos) {
        /* goal_logErr("failed to take lock"); */
        res = GOAL_ERROR;
    }

    return res;
}


/****************************************************************************/
/** Release a Lock
 *
 * Leave a binary or counting lock.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_targetLockPut(
    GOAL_LOCK_T *pLock                          /**< lock data */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* GOAL result */
    BaseType_t resFreertos = pdFALSE;           /* FreeRTOS result */
    BaseType_t highPrioTask = pdFALSE;          /* higher priority task needs switch */

    /* release FreeRTOS semaphore */
    if (GOAL_TRUE == goal_targetCommonIsrCheck()) {
        /* TODO: check for xSemaphoreGiveFromISR usage */
        resFreertos = xSemaphoreGiveFromISR(pLock->pTgt, &highPrioTask);
        /* perform a context switch */
        portYIELD_FROM_ISR(highPrioTask);
    }
    else if (GOAL_LOCK_COUNT == pLock->type) {
        resFreertos = xSemaphoreGive(pLock->pTgt);
    }
    else {
        resFreertos = xSemaphoreGiveRecursive(pLock->pTgt);
    }
    if (pdFALSE == resFreertos) {
        goal_logErr("failed to put lock");
        res = GOAL_ERROR;
    }

    return res;
}


/****************************************************************************/
/** Free Lock Resources
 *
 * Cleans up the locks resources.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_targetLockDelete(
    GOAL_LOCK_T *pLock                          /**< lock data */
)
{
    /* delete FreeRTOS semaphore */
    vSemaphoreDelete(pLock->pTgt);
    pLock->pTgt = NULL;

    return GOAL_OK;
}


/****************************************************************************/
/** Check if content is called from ISR
 *
 * This function is used for compatibility.
 * The board has to identify, if an ISR is active or not. In consequence,
 * FreeRTOS can choose to call functionFROMISR or not.
 * For some
 *
 * @retval GOAL_TRUE if this function is called out of an ISR
 * @retval GOAL_FALSE otherwise
 */
GOAL_TARGET_WEAK GOAL_BOOL_T goal_targetCommonIsrCheck(
    void
)
{
    return flgInt;
}


GOAL_TARGET_WEAK GOAL_STATUS_T goal_targetMainPreOsCb(
    void
)
{
    return GOAL_OK;
}
