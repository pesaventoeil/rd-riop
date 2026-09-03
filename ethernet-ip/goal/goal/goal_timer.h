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

#ifndef GOAL_TIMER_H
#define GOAL_TIMER_H


/****************************************************************************/
/* defines */
/****************************************************************************/
/**< 1 millisecond */
#define GOAL_TIMER_MSEC 1

/**< 1 second */
#define GOAL_TIMER_SEC (1000 * GOAL_TIMER_MSEC)


/****************************************************************************/
/* Configuration */
/****************************************************************************/
/** hard timer run period */
#ifndef GOAL_CONFIG_TIMER_PERIOD_HARD
#  define GOAL_CONFIG_TIMER_PERIOD_HARD GOAL_TIMER_MS
#endif

#define GOAL_TIMER_SAFETY_MARGIN    10      /**< timer rescue safety margin */


/****************************************************************************/
/* enums */
/****************************************************************************/
/** timer state */
typedef enum {
    GOAL_TIMER_STATE_UNKNOWN = 0,               /**< unknown */
    GOAL_TIMER_STATE_SETUP,                     /**< configured */
    GOAL_TIMER_STATE_ACTIVE,                    /**< active */
    GOAL_TIMER_STATE_RUN,                       /**< currently executed */
    GOAL_TIMER_STATE_DELETE,                    /**< wait for deletion */
} GOAL_TIMER_STATE_T;


/****************************************************************************/
/* structures */
/****************************************************************************/
/** timer data */
typedef struct GOAL_TIMER_T {
    struct GOAL_TIMER_T *pPrev;                 /**< prev list element */
    struct GOAL_TIMER_T *pNext;                 /**< next list element */

    GOAL_TIMER_STATE_T  state;                  /**< timer state */
    GOAL_TIMER_STATE_T  stateNext;              /**< timer state next */
    GOAL_TIMER_CB_T     pFunc;                  /**< timer function */
    void               *pArg;                   /**< timer function argument */
    GOAL_TIMESTAMP_T    expire;                 /**< timeout */
    GOAL_TIMESTAMP_T    period;                 /**< period */
    GOAL_TIMER_TYPE_T   type;                   /**< type */
    GOAL_FUNC_NORET_T   pCbFunc;                /**< stop callback function */
    void               *pCbArg;                 /**< stop callback argument */
    GOAL_TIMER_PRIO_T   prio;                   /**< timer priority */
    GOAL_LOCK_T        *pLock;                  /**< timer lock */
    void               *pTgt;                   /**< target handle */
} GOAL_TIMER_T;


/****************************************************************************/
/* prototypes */
/****************************************************************************/

GOAL_STATUS_T goal_timerInitPre(
    void
);

GOAL_STATUS_T goal_timerCreate(
    GOAL_TIMER_T **ppTmr,                       /**< timer ptr ref */
    GOAL_TIMER_PRIO_T prio                      /**< timer priority */
);

GOAL_STATUS_T goal_timerSetup(
    GOAL_TIMER_T *pTmr,                         /**< timer */
    GOAL_TIMER_TYPE_T type,                     /**< single or periodic type */
    uint32_t period,                            /**< period in ms */
    GOAL_TIMER_CB_T pFunc,                      /**< timer callback */
    void *pArg,                                 /**< timer argument */
    GOAL_BOOL_T startFlag                       /**< start after setup */
);

GOAL_STATUS_T goal_timerStart(
    GOAL_TIMER_T *pTmr                          /**< timer */
);

GOAL_STATUS_T goal_timerStop(
    GOAL_TIMER_T *pTmr                          /**< timer */
);

GOAL_STATUS_T goal_timerStopCb(
    GOAL_TIMER_T *pTmr,                         /**< timer */
    GOAL_FUNC_NORET_T pFunc,                    /**< callback function */
    void *pArg                                  /**< function argument */
);

void goal_timerDelete(
    GOAL_TIMER_T **ppTmr                        /**< timer ptr ref */
);

void goal_timerRun(
    GOAL_BOOL_T flgPrioHard                     /**< hard flag */
);

void goal_timerExec(
    GOAL_TIMER_T *pTmr                          /**< timer */
);

#if GOAL_CONFIG_TASK == 1
void goal_timerLoadThread(
    void
);

uint32_t goal_timerLoadAvg(
    void
);
#endif /* GOAL_CONFIG_TASK == 1 */

GOAL_STATUS_T goal_timerSleep(
    uint32_t delay                              /**< sleep delay in ms */
);

GOAL_STATUS_T goal_timerInitCli(
    void
);

GOAL_TIMESTAMP_T goal_timerTsGet(
    void
);


#endif /* GOAL_TIMER_H */
