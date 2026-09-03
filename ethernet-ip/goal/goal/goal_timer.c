/** @file
 *
 * @brief Timer Management Module
 *
 * This module provides generic timer functions that can be used by the other
 * modules.
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

#define GOAL_ID GOAL_ID_TMR
#include <goal_includes.h>


/****************************************************************************/
/* Local defines */
/****************************************************************************/
#define GOAL_TIMER_LOAD_CNT  10                 /**< load track slots */

/** warn value if timer to to far in the future */
#define GOAL_TIMER_PERIOD_WARN 120000


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_STAGE_HANDLER_T stageInit;          /**< init stage handler */
#if GOAL_CONFIG_CLI == 1
static GOAL_STAGE_HANDLER_T stageCli;           /**< CLI init stage handler */
#endif
static GOAL_BOOL_T gTimerInit = GOAL_FALSE;     /**< timer init flag */
static GOAL_LOCK_T *gMtxTimer = NULL;           /**< timer structure mutex */
static GOAL_TIMER_T *gListTmrSoft = NULL;       /**< soft timer list */
static GOAL_TIMER_T *gListTmrHard = NULL;       /**< hard timer list */
#if GOAL_CONFIG_TASK == 1
static uint64_t loadRef = 0;                    /**< load reference */
static uint64_t load[GOAL_TIMER_LOAD_CNT];      /**< load slots */
static unsigned int idxLoad = 0;                /**< load slot index */
#endif
#if GOAL_CONFIG_TIMER_TS_NO_STUCK == 1
static GOAL_TIMESTAMP_T gCntTsRescue = 0;       /**< rescue timestamp counter */
#endif


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/
static GOAL_STATUS_T goal_timerInit(
    void
);

#if GOAL_CONFIG_TIMER_TS_NO_STUCK == 1
static GOAL_STATUS_T goal_timerInitTsRescue(
    void
);
#endif

static void goal_timerRunSoft(
    void
);


/****************************************************************************/
/** Register stage handler
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_timerInitPre(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* timer pre-stage */
    res = goal_mainStageReg(GOAL_STAGE_TIMER_PRE, &stageInit, GOAL_STAGE_INIT, goal_timerInit);

#if GOAL_CONFIG_CLI == 1
    /* CLI stage */
    if (GOAL_RES_OK(res)) {
        res = goal_mainStageReg(GOAL_STAGE_CLI, &stageCli, GOAL_STAGE_INIT, goal_timerInitCli);
    }
#endif

    return res;
}


/****************************************************************************/
/** Initialize timer management
 *
 * This function sets up the timer management.
 *
 * @retval GOAL_OK successful
 * @retval other fail
 */
static GOAL_STATUS_T goal_timerInit(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* create timer mutex */
    res = goal_lockCreate(GOAL_LOCK_BINARY, &gMtxTimer, 0, 1, GOAL_ID_TMR);
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:239: */
        goal_lmLog(GOAL_ID_TMR, 239, 194, 0, 0, GOAL_LOG_SEV_ERROR, "failed to create timer lock");
        return res;
    }

#if GOAL_CONFIG_TASK == 1
    /* initialize load slots */
    GOAL_MEMSET(load, 0, sizeof(uint64_t) * ARRAY_ELEMENTS(load));
#endif

    /* initialize target specific timer */
    res = goal_targetTimerInit();
    if (GOAL_OK != res) {
        return res;
    }

#if GOAL_CONFIG_TIMER_TS_NO_STUCK == 1
    /* initialize timer rescue counter */
    if (GOAL_RES_OK(res)) {
        res = goal_timerInitTsRescue();
    }
#endif

    /* register soft timer handler */
    if (GOAL_RES_OK(res)) {
        res = goal_mainLoopReg(goal_timerRunSoft);
    }

    /* set timer init flag */
    if (GOAL_RES_OK(res)) {
        gTimerInit = GOAL_TRUE;
    }

    return res;
}


/****************************************************************************/
/** Soft Timer Loop Function
 *
 * This function executes the Cyclic Timer Management for low priority timers
 * (softtimers).
 */
static void goal_timerRunSoft(
    void
)
{
    /* don't run without registered softtimers */
    if (NULL != gListTmrSoft) {
        goal_timerRun(GOAL_FALSE);
    }
}


/****************************************************************************/
/** Cyclic timer management
 *
 * This function must be cyclically called every 1 ms for hard timers and at
 * maximum every 1 ms for soft timers.
 */
void goal_timerRun(
    GOAL_BOOL_T flgPrioHard                     /**< hard flag */
)
{
    static GOAL_TIMESTAMP_T ts;                 /* timestamp */
    GOAL_TIMER_T *pElem;                        /* timer element */

    /* check timer init flag */
    if (GOAL_TRUE != gTimerInit) {
        return;
    }

    /* assign selected list */
    pElem = (GOAL_TRUE == flgPrioHard) ? gListTmrHard : gListTmrSoft;

    /* store timestamp */
    ts = goal_timerTsGet();

    /* iterate through all timers */
    for (; pElem; pElem = pElem->pNext) {

        /* check if timer is active */
        if (GOAL_TIMER_STATE_ACTIVE != pElem->state) {
            continue;
        }

        /* check if timer is expired
         *
         * Note: No timer lock is aquired here so it's possible that expire is
         * true but timer isn't active anymore - but for this case,
         * goal_timerExec needs to check the ACTIVE flag anyway so an extra
         * lock only costs time.
         */
        if (pElem->expire > ts) {
            continue;
        }

        /* run timer */
        goal_timerExec(pElem);

        /* lock timer */
        goal_lockGet(pElem->pLock, GOAL_LOCK_INFINITE);

        /* update expiration if timer state is active */
        if (GOAL_TIMER_STATE_ACTIVE == pElem->state) {
            pElem->expire = ts + pElem->period;
        }

        /* unlock timer */
        goal_lockPut(pElem->pLock);
    }
}


/****************************************************************************/
/** Execute Timer
 *
 * This function executes a given timer and can be called directly even by the
 * target.
 */
void goal_timerExec(
    GOAL_TIMER_T *pTmr                          /**< timer */
)
{
    GOAL_TIMER_CB_T pFunc;                      /* timer function */
    GOAL_FUNC_NORET_T pFuncCb;                  /* timer function */
    void *pArg;                                 /* timer argument */

    /* lock timer access */
    goal_lockGet(pTmr->pLock, GOAL_LOCK_INFINITE);

    /* check if timer is active */
    if (GOAL_TIMER_STATE_ACTIVE != pTmr->state) {
        goal_lockPut(pTmr->pLock);
        return;
    }

    /* set state to run */
    pTmr->state = GOAL_TIMER_STATE_RUN;

    /* temporary store callback function and its argument for
     * the case that they change between releasing the lock and
     * calling it */
    pFunc = pTmr->pFunc;
    pArg = pTmr->pArg;

    /* execute timer with unlocked timer mutex */
    goal_lockPut(pTmr->pLock);
    pFunc(pArg);
    goal_lockGet(pTmr->pLock, GOAL_LOCK_INFINITE);

    /* check if timer has changed while running */
    if (pTmr->stateNext) {
        pTmr->state = pTmr->stateNext;
        pTmr->stateNext = GOAL_TIMER_STATE_UNKNOWN;

        if (GOAL_TIMER_STATE_SETUP == pTmr->state) {
            /* run stop callback if available */
            if (pTmr->pCbFunc) {
                /* cache timer callback and argument to allow overwrite in callback itself */
                pFuncCb = pTmr->pCbFunc;
                pArg = pTmr->pCbArg;

                pTmr->pCbFunc = NULL;
                pTmr->pCbArg = NULL;

                goal_lockPut(pTmr->pLock);
                pFuncCb(pArg);
                goal_lockGet(pTmr->pLock, GOAL_LOCK_INFINITE);
            }
        }
        /* delete timer if requested */
        else if (GOAL_TIMER_STATE_DELETE == pTmr->state) {
            goal_timerDelete(&pTmr);
        }
    } else {
        if (GOAL_TIMER_SINGLE == pTmr->type) {
            pTmr->state = GOAL_TIMER_STATE_SETUP;
        } else {
            pTmr->state = GOAL_TIMER_STATE_ACTIVE;
        }
    }

    /* unlock timer access */
    goal_lockPut(pTmr->pLock);
}


/****************************************************************************/
/** Create Timer
 *
 * This function is used to create ("allocate") a new timer.
 * Timer IDs always start at 1, because timer ID 0 is reserved for
 * GOAL_TIMER_INVALID.
 *
 * @returns GOAL_OK - success
 * @returns other - fail
 */
GOAL_STATUS_T goal_timerCreate(
    GOAL_TIMER_T **ppTmr,                       /**< timer ptr ref */
    GOAL_TIMER_PRIO_T prio                      /**< timer priority */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_TIMER_T **ppList;                      /* timer list ptr ref */

    /* check timer init flag */
    if (GOAL_TRUE != gTimerInit) {
        /* GG_LOG ID:240: */
        goal_lmLog(GOAL_ID_TMR, 240, 195, 0, 0, GOAL_LOG_SEV_ERROR, "timer not initialized");
        return GOAL_ERR_INIT;
    }

    /* timer must be unused */
    if ((NULL == ppTmr) || (NULL != *ppTmr)) {
        /* GG_LOG ID:241: */
        goal_lmLog(GOAL_ID_TMR, 241, 196, 0, 0, GOAL_LOG_SEV_ERROR, "timer pointer not properly initialized");
        return GOAL_ERR_PARAM;
    }

    /* allocate timer */
    res = goal_memCalloc(ppTmr, sizeof(GOAL_TIMER_T));
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:242: */
        goal_lmLog(GOAL_ID_TMR, 242, 197, 0, 0, GOAL_LOG_SEV_ERROR, "failed to allocate timer");
    }

    /* create timer mutex */
    if (GOAL_RES_OK(res)) {
        res = goal_lockCreate(GOAL_LOCK_BINARY, &(*ppTmr)->pLock, 0, 1, GOAL_ID_TMR);
        if (GOAL_RES_ERR(res)) {
            /* GG_LOG ID:243: */
            goal_lmLog(GOAL_ID_TMR, 243, 194, 0, 0, GOAL_LOG_SEV_ERROR, "failed to create timer lock");
        }
    }

    /* assign priority */
    if (GOAL_RES_OK(res)) {
        (*ppTmr)->prio = prio;
    }

    /* create target timer */
    if (GOAL_RES_OK(res) && (GOAL_TIMER_HIGH == prio)) {
        res = goal_targetTimerCreate(*ppTmr);
        if (GOAL_RES_ERR(res)) {
            /* GG_LOG ID:244: */
            goal_lmLog(GOAL_ID_TMR, 244, 198, 0, 0, GOAL_LOG_SEV_ERROR, "failed to create target timer");
        }
    }

    /* put timer at the end of the given list */
    if (GOAL_RES_OK(res)) {

        /* iterate to list end */
        ppList = (GOAL_TIMER_HIGH == prio) ? &gListTmrHard : &gListTmrSoft;
        for (; *ppList; ppList = &(*ppList)->pNext);

        /* attach timer and update timers previous element */
        if (*ppList) {
            (*ppList)->pNext = *ppTmr;
            (*ppTmr)->pPrev = *ppList;
        } else {
            *ppList = *ppTmr;
        }
    }

    /* cleanup on error */
    if (GOAL_RES_ERR(res)) {
        goal_memFree(ppTmr);
    }

    return res;
}


/****************************************************************************/
/** Timer Setup
 *
 * Setup a timer by setting its type, its period and its callback function.
 *
 * @returns IOD_OK - success
 * @returns others - fail
 */
GOAL_STATUS_T goal_timerSetup(
    GOAL_TIMER_T *pTmr,                         /**< timer */
    GOAL_TIMER_TYPE_T type,                     /**< single or periodic type */
    uint32_t period,                            /**< period in ms */
    GOAL_TIMER_CB_T pFunc,                      /**< timer callback */
    void *pArg,                                 /**< timer argument */
    GOAL_BOOL_T startFlag                       /**< start after setup */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    /* check pointer */
    if (NULL == pTmr) {
        /* GG_LOG ID:245: */
        goal_lmLog(GOAL_ID_TMR, 245, 199, 0, 0, GOAL_LOG_SEV_ERROR, "timer is null");
        return GOAL_ERR_NULL_POINTER;
    }

    /* check period */
    if (GOAL_TIMER_PERIOD_WARN < period) {
        /* GG_LOG ID:246: */
        goal_lmLog(GOAL_ID_TMR, 246, 200, 4, 0, GOAL_LOG_SEV_WARNING, "timer period larger than $1");
        goal_lmLogParamUINT32(GOAL_TIMER_PERIOD_WARN);
        goal_lmLogFinish();
    }

    /* check callback */
    if (NULL == pFunc) {
        /* GG_LOG ID:247: */
        goal_lmLog(GOAL_ID_TMR, 247, 201, 0, 0, GOAL_LOG_SEV_ERROR, "timer callback not set");
        return GOAL_ERR_NULL_POINTER;
    }

    /* lock timer access */
    goal_lockGet(pTmr->pLock, GOAL_LOCK_INFINITE);

    /* if timer is in run state, postpone state change */
    if (GOAL_TIMER_STATE_RUN == pTmr->state) {
        pTmr->stateNext = GOAL_TIMER_STATE_SETUP;
    } else {
        pTmr->state = GOAL_TIMER_STATE_SETUP;
    }

    pTmr->expire = (uint64_t) 0;
    pTmr->period = (uint64_t) period;
    pTmr->type = type;
    pTmr->pArg = pArg;
    pTmr->pFunc = pFunc;
    pTmr->pCbFunc = NULL;
    pTmr->pCbArg = NULL;

    /* if flag is set start timer */
    if (GOAL_TRUE == startFlag) {
        res = goal_timerStart(pTmr);
    }

    /* unlock timer */
    goal_lockPut(pTmr->pLock);

    return res;
}


/****************************************************************************/
/** Start Timer
 *
 * Start a timer by marking it as active.
 *
 * @returns GOAL_OK - success
 * @returns GOAL_TIMER_NOT_USED - unused timer handle
 * @returns GOAL_TIMER_NOT_SET - timer not set
 */
GOAL_STATUS_T goal_timerStart(
    GOAL_TIMER_T *pTmr                          /**< timer */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_TIMESTAMP_T ts;                        /* timestamp */
    GOAL_TIMER_STATE_T *pState;                 /* state ptr */

    /* check pointer */
    if (NULL == pTmr) {
        /* GG_LOG ID:248: */
        goal_lmLog(GOAL_ID_TMR, 248, 199, 0, 0, GOAL_LOG_SEV_ERROR, "timer is null");
        return GOAL_ERR_NULL_POINTER;
    }

    /* do not start timer if the timer is not set up */
    if (GOAL_TIMER_STATE_UNKNOWN == pTmr->state) {
        return GOAL_ERR_TIMER_START;
    }

    /* lock timer */
    goal_lockGet(pTmr->pLock, GOAL_LOCK_INFINITE);

    if (GOAL_TIMER_STATE_SETUP == pTmr->state) {
        pState = &pTmr->state;
    }
    else if (GOAL_TIMER_STATE_SETUP == pTmr->stateNext) {
        pState = &pTmr->stateNext;
    }
    else {
        goal_lockPut(pTmr->pLock);
        /* GG_LOG ID:249: */
        goal_lmLog(GOAL_ID_TMR, 249, 202, 0, 0, GOAL_LOG_SEV_ERROR, "timer already started");
        return GOAL_ERR_TIMER_STARTED;
    }

    ts = goal_timerTsGet();

    pTmr->expire = ts + pTmr->period;
    *pState = GOAL_TIMER_STATE_ACTIVE;

    /* start target specific timer */
    if (GOAL_TIMER_HIGH == pTmr->prio) {
        res = goal_targetTimerStart(pTmr);
    }

    /* unlock timer */
    goal_lockPut(pTmr->pLock);

    return res;
}


/****************************************************************************/
/** Stop Timer
 *
 * Stop a timer from being executed.
 *
 * @returns GOAL_OK success
 * @returns GOAL_OK_TIMER_RUN timer will be stopped after it's done
 * @returns GOAL_ERR_TIMER_NOT_USED unused timer handle
 */
GOAL_STATUS_T goal_timerStop(
    GOAL_TIMER_T *pTmr                          /**< timer ID */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    /* check pointer */
    if (NULL == pTmr) {
        /* GG_LOG ID:250: */
        goal_lmLog(GOAL_ID_TMR, 250, 199, 0, 0, GOAL_LOG_SEV_ERROR, "timer is null");
        return GOAL_ERR_NULL_POINTER;
    }

    /* lock timer */
    goal_lockGet(pTmr->pLock, GOAL_LOCK_INFINITE);

    /* if timer is in run state, postpone state change */
    if (GOAL_TIMER_STATE_RUN == pTmr->state) {
        pTmr->stateNext = GOAL_TIMER_STATE_SETUP;
        res = GOAL_OK_TIMER_RUN;
    } else if (GOAL_TIMER_STATE_UNKNOWN != pTmr->state) {
        pTmr->state = GOAL_TIMER_STATE_SETUP;
    }

    /* stop target specifc timer */
    if (GOAL_TIMER_HIGH == pTmr->prio) {
        goal_targetTimerStop(pTmr);
    }

    /* unlock timer */
    goal_lockPut(pTmr->pLock);

    return res;
}


/****************************************************************************/
/** Stop Timer with Callback
 *
 * To always make sure that timer specific resources are only freed after the
 * timer is halted this function provides a safe way to do so. First it tries
 * to stop the timer before it is entering the RUN state. If that is possible,
 * the callback is called immediately. If the current timer state is RUN than
 * the callback will be called _after_ the timer has been run and was stopped
 * afterwards.
 *
 * @returns GOAL_OK success
 * @returns GOAL_OK_TIMER_RUN timer will be stopped after it's done
 * @returns GOAL_ERR_TIMER_NOT_USED unused timer handle
 */
GOAL_STATUS_T goal_timerStopCb(
    GOAL_TIMER_T *pTmr,                         /**< timer ID */
    GOAL_FUNC_NORET_T pFunc,                    /**< callback function */
    void *pArg                                  /**< function argument */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    /* check pointer */
    if (NULL == pTmr) {
        /* GG_LOG ID:251: */
        goal_lmLog(GOAL_ID_TMR, 251, 199, 0, 0, GOAL_LOG_SEV_ERROR, "timer is null");
        return GOAL_ERR_NULL_POINTER;
    }

    /* lock timer */
    goal_lockGet(pTmr->pLock, GOAL_LOCK_INFINITE);

    /* if timer is in run state, postpone state change */
    if (GOAL_TIMER_STATE_RUN == pTmr->state) {
        pTmr->stateNext = GOAL_TIMER_STATE_SETUP;
        pTmr->pCbFunc = pFunc;
        pTmr->pCbArg = pArg;
        res = GOAL_OK_TIMER_RUN;
    } else if (GOAL_TIMER_STATE_UNKNOWN != pTmr->state) {
        pTmr->state = GOAL_TIMER_STATE_SETUP;
    }

    /* stop target specifc timer */
    if (GOAL_TIMER_HIGH == pTmr->prio) {
        goal_targetTimerStop(pTmr);
    }

    /* unlock timer */
    goal_lockPut(pTmr->pLock);

    /* check if callback must be run immediately */
    if (GOAL_OK == res) {
        pFunc(pArg);
    }

    return res;
}


/****************************************************************************/
/** Delete Timer
 *
 * Clean up timer resources.
 */
void goal_timerDelete(
    GOAL_TIMER_T **ppTmr                        /**< timer */
)
{
    GOAL_STATUS_T res;                          /* result */

    /* timer must be set */
    if ((NULL == ppTmr) || (NULL == *ppTmr)) {
        /* GG_LOG ID:252: */
        goal_lmLog(GOAL_ID_TMR, 252, 203, 0, 0, GOAL_LOG_SEV_ERROR, "timer not properly created");
        return;
    }

    /* lock timer */
    goal_lockGet((*ppTmr)->pLock, GOAL_LOCK_INFINITE);

    /* check if timer can be deleted instantly */
    if ((GOAL_TIMER_STATE_SETUP == (*ppTmr)->state) ||
        (GOAL_TIMER_STATE_UNKNOWN == (*ppTmr)->state) ||
        (GOAL_TIMER_STATE_DELETE == (*ppTmr)->state)) {

        /* remove timer from list */
        if ((*ppTmr)->pPrev) {
            (*ppTmr)->pPrev->pNext = (*ppTmr)->pNext;
        }

        /* delete timer in hardware */
        if (GOAL_TIMER_HIGH == (*ppTmr)->prio) {
            res = goal_targetTimerDelete(*ppTmr);
            if (GOAL_RES_ERR(res)) {
                /* GG_LOG ID:253: */
                goal_lmLog(GOAL_ID_TMR, 253, 204, 0, 0, GOAL_LOG_SEV_ERROR, "timer could not be deleted in target");
            }
        }

        /* unlock timer */
        goal_lockPut((*ppTmr)->pLock);

        /* free lock resources */
        goal_lockDelete((*ppTmr)->pLock);

        /* free timer resources */
        goal_memFree(ppTmr);

    } else {
        /* postpone timer deletion */
        (*ppTmr)->stateNext = GOAL_TIMER_STATE_DELETE;

        /* unlock timer */
        goal_lockPut((*ppTmr)->pLock);
    }
}


#if GOAL_CONFIG_TASK == 1
/****************************************************************************/
/** Thread for device load detection
 */
void goal_timerLoadThread(
    void
)
{
    GOAL_TIMESTAMP_T tsPrev;                    /* previous timestamp */
    GOAL_TIMESTAMP_T tsCur;                     /* current timestamp */
    uint64_t loadCur;                           /* current load */

    while (!gTimerInit) {
        goal_taskMsSleep(GOAL_TIMER_MSEC, NULL);
    }

    while (!GOAL_flagShutdown) {

        /* wait for next millisecond tick have a clean start */
        tsPrev = goal_timerTsGet();
        tsCur = tsPrev;
        for (; tsPrev == tsCur; tsCur = goal_timerTsGet());

        /* store current timestamp and start measurement */
        tsPrev = tsCur;
        for (loadCur = 0; tsPrev == tsCur; tsCur = goal_timerTsGet(), loadCur++);

        /* if load reference is less than this value, calibrate it */
        if (loadCur > loadRef) {
            loadRef = loadCur;
        }

        /* store current load in ringbuffer */
        load[idxLoad++] = loadCur;
        if (idxLoad >= ARRAY_ELEMENTS(load)) {
            idxLoad = 0;
        }

        /* switch to next task */
        goal_taskMsSleep(0, NULL);
    }
}


/****************************************************************************/
/** Calculate average load
 *
 * @retval average load in percent (0 - 100)
 */
uint32_t goal_timerLoadAvg(
    void
)
{
    unsigned int cnt;                           /* counter */
    uint64_t loadAvg = 0;                       /* average load */
    unsigned int cntLoadAvg = 0;                /* average load elements */

    /* summarize all load counters */
    for (cnt = 0; cnt < ARRAY_ELEMENTS(load); cnt++) {
        if (load[cnt]) {
            loadAvg += load[cnt];
            cntLoadAvg++;
        }
    }

    /* get load counter average */
    loadAvg /= cntLoadAvg;

    /* get percent value from load reference counter */
    return (uint32_t) (100 - ((loadAvg * 100) / loadRef));
}
#endif /* GOAL_CONFIG_TASK == 1 */


/****************************************************************************/
/** Blocking sleep (busy loop)
 *
 * Blocks the caller until a given delay has passed. On non-multitasking
 * architectures this blocks the whole device.
 *
 * Note: As GOAL uses a resolution of 1 ms it is possible that this call
 * returns instantly if the current millisecond increases just after tsTout was
 * initialized.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_timerSleep(
    uint32_t delay                              /**< sleep delay in ms */
)
{
    GOAL_TIMESTAMP_T tsTout;                    /* timeout */

    tsTout = goal_timerTsGet() + delay;
    while (tsTout > goal_timerTsGet());

    return GOAL_OK;
}


#if GOAL_CONFIG_TIMER_TS_NO_STUCK == 1
/****************************************************************************/
/** Initialize Timer Rescue
 *
 * Measures how often a counter can be increased to get a resolution of nearly
 * 1 ms.
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_timerInitTsRescue(
    void
)
{
    GOAL_TIMESTAMP_T tsLast;                    /* last timestamp */
    GOAL_TIMESTAMP_T tsCur;                     /* current timestamp */

    /* wait for start of next millisecond */
    for (tsLast = tsCur = goal_targetGetTimestamp(); tsLast == tsCur; tsCur = goal_targetGetTimestamp());

    /* start measurement */
    for (tsLast = tsCur; tsLast == tsCur; gCntTsRescue++, tsCur = goal_targetGetTimestamp());

    /* add 10 percent safety margin */
    gCntTsRescue += ((gCntTsRescue * (GOAL_TIMESTAMP_T) GOAL_TIMER_SAFETY_MARGIN) / (GOAL_TIMESTAMP_T) GOAL_NUM_PERCENT_100);

    /* GG_LOG ID:254: */
    goal_lmLog(GOAL_ID_TMR, 254, 205, 8, 0, GOAL_LOG_SEV_INFO, "rescue timeout counter calibrated: $1 counts");
    goal_lmLogParamUINT64(gCntTsRescue);
    goal_lmLogFinish();

    return GOAL_OK;
}
#endif /* GOAL_CONFIG_TIMER_TS_NO_STUCK == 1 */


/****************************************************************************/
/** Return Current Timestamp in ms
 *
 * If for any reason the timestamp is not updated this function makes sure that
 * the timer tick gets increment if the measured rescue counter matches the
 * query counter.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_TIMESTAMP_T goal_timerTsGet(
    void
)
{
#if GOAL_CONFIG_TIMER_TS_NO_STUCK == 1
    static GOAL_TIMESTAMP_T cntQuery = 0;       /* function query counter */
    static GOAL_TIMESTAMP_T tsLast = 0;         /* last timestamp */
    GOAL_TIMESTAMP_T tsNow;                     /* current timestamp */

    /* read current timestamp */
    tsNow = goal_targetGetTimestamp();

    /* check if timestamp was increased */
    if (tsLast != tsNow) {

        /* reset query counter */
        cntQuery = 0;

        /* update last timestamp */
        tsLast = tsNow;

    } else {

        /* increment query counter */
        cntQuery++;

        /* if rescue counter is reached, update timestamp */
        if ((gCntTsRescue) && (gCntTsRescue <= cntQuery)) {
            tsLast++;

            /* TODO: update timestamp in target */
            /* goal_tgtTsSet(tsLast); */
        }
    }

    return tsLast;

#else

    return goal_targetGetTimestamp();

#endif /* GOAL_CONFIG_TIMER_TS_NO_STUCK == 1 */
}


/****************************************************************************/
/** Timer Init
 *
 * Initializes low level timer management.
 *
 * @retval GOAL_STATUS_T result
 */
GOAL_TARGET_WEAK GOAL_STATUS_T goal_targetTimerInit(
    void
)
{
    /* not implemented */
    return GOAL_OK;
}


/****************************************************************************/
/** Create Timer
 *
 * Creates a timer for the given timer.
 *
 * @retval GOAL_STATUS_T result
 */
GOAL_TARGET_WEAK GOAL_STATUS_T goal_targetTimerCreate(
    GOAL_TIMER_T *pTmr                          /**< timer */
)
{
    UNUSEDARG(pTmr);

    /* not implemented */
    return GOAL_OK;
}


/****************************************************************************/
/** Start Timer
 *
 * Start the given timer.
 *
 * @retval GOAL_STATUS_T result
 */
GOAL_TARGET_WEAK GOAL_STATUS_T goal_targetTimerStart(
    GOAL_TIMER_T *pTmr                          /**< timer */
)
{
    UNUSEDARG(pTmr);

    /* not implemented */
    return GOAL_OK;
}


/****************************************************************************/
/** Stop Timer
 *
 * Stop the given timer.
 *
 * @retval GOAL_STATUS_T result
 */
GOAL_TARGET_WEAK GOAL_STATUS_T goal_targetTimerStop(
    GOAL_TIMER_T *pTmr                          /**< timer */
)
{
    UNUSEDARG(pTmr);

    /* not implemented */
    return GOAL_OK;
}


/****************************************************************************/
/** Free Timer Resources
 *
 * Cleans up timer resources.
 *
 * @retval GOAL_STATUS_T result
 */
GOAL_TARGET_WEAK GOAL_STATUS_T goal_targetTimerDelete(
    GOAL_TIMER_T *pTmr                          /**< timer */
)
{
    UNUSEDARG(pTmr);

    /* not implemented */
    return GOAL_OK;
}
