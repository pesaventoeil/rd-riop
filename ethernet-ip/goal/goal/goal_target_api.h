/** @file
 *
 * @brief Low Level Interface of GOAL
 *
 * This provides the API to the target specific functions used by GOAL.
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

#ifndef GOAL_TARGET_API_H
#define GOAL_TARGET_API_H

#include <goal_includes.h>


/****************************************************************************/
/* List of public functions */
/****************************************************************************/
GOAL_STATUS_T goal_targetInitPre(
    void
);

void goal_targetReset(
    void
);

void goal_targetHalt(
    void
);

GOAL_TIMESTAMP_T goal_targetGetTimestamp(
    void
);

uint32_t goal_targetGetButtons(
    void
);

void goal_targetSetLeds(
    uint32_t leds                               /**< LED bitmask */
);

void goal_targetClearLeds(
    uint32_t leds                               /**< LED bit field */
);

uint32_t goal_targetGetLeds(
    void
);

/* Non-volatile Memory interface */
unsigned int goal_targetNvsGetSize(
    void
);

GOAL_STATUS_T goal_targetNvsReadData(
    void *pData,                                /**< read buffer */
    unsigned int size                           /**< read length */
);

GOAL_STATUS_T goal_targetNvsWriteData(
    void *pData,                                /**< write buffer */
    unsigned int size                           /**< write length */
);

/* Lock interface */
GOAL_STATUS_T goal_targetLockInit(
    void
);

GOAL_STATUS_T goal_targetLockShutdown(
    void
);

GOAL_STATUS_T goal_targetLockCreate(
    GOAL_LOCK_TYPE_T lockType,                  /**< lock type */
    GOAL_LOCK_T *pLock,                         /**< lock handle */
    uint32_t valInit,                           /**< initial value */
    uint32_t valMax                             /**< maximum value */
);

GOAL_STATUS_T goal_targetLockGet(
    GOAL_LOCK_T *pLock,                         /**< lock handle */
    uint32_t timeout                            /**< timeout */
);

GOAL_STATUS_T goal_targetLockPut(
    GOAL_LOCK_T *pLock                          /**< lock handle */
);

GOAL_STATUS_T goal_targetLockDelete(
    GOAL_LOCK_T *pLock                          /**< lock handle */
);

/* timer interface */
GOAL_STATUS_T goal_targetTimerInit(
    void
);

GOAL_STATUS_T goal_targetTimerCreate(
    GOAL_TIMER_T *pTmr                          /**< timer */
);

GOAL_STATUS_T goal_targetTimerStart(
    GOAL_TIMER_T *pTmr                          /**< timer */
);

GOAL_STATUS_T goal_targetTimerStop(
    GOAL_TIMER_T *pTmr                          /**< timer */
);

GOAL_STATUS_T goal_targetTimerDelete(
    GOAL_TIMER_T *pTmr                          /**< timer */
);

/* logging interface */
#if GOAL_CONFIG_LOGGING_TARGET_RAW == 1
void goal_targetMsgRaw(
    const char *str,                            /**< message */
    unsigned int len                            /**< message length */
);
#endif /* GOAL_CONFIG_LOGGING_TARGET_RAW */

/* UART character get and put */
GOAL_STATUS_T goal_tgtCharGet(
    char *pBuf                                  /**< single character buffer */
);

GOAL_STATUS_T goal_tgtCharPut(
    char c                                      /**< character */
);

/* TCP/IP Stack interface */
#if GOAL_CONFIG_TCPIP_STACK == 1
GOAL_STATUS_T goal_targetNetChanCreate(
    GOAL_NET_CHAN_T *pChan,                     /**< GOAL Net channel */
    GOAL_NET_TYPE_T type                        /**< channel type */
);

GOAL_STATUS_T goal_targetNetChanDelete(
    GOAL_NET_CHAN_T *pChan                      /**< GOAL Net channel */
);

GOAL_STATUS_T goal_targetNetChanOpen(
    GOAL_NET_CHAN_T *pChan,                     /**< channel handle */
    GOAL_BOOL_T flgReopen                       /**< reopen GOAL channel */
);

GOAL_STATUS_T goal_targetNetChanClose(
    GOAL_NET_CHAN_T *pChan                      /**< channel handle */
);

GOAL_STATUS_T goal_targetNetChanActivate(
    GOAL_NET_CHAN_T *pChan                      /**< channel handle */
);

GOAL_STATUS_T goal_targetNetChanDeactivate(
    GOAL_NET_CHAN_T *pChan                      /**< channel handle */
);

GOAL_STATUS_T goal_targetNetChanSend(
    GOAL_NET_CHAN_T *pChan,                     /**< channel handle */
    GOAL_BUFFER_T *pBuf                         /**< send buffer */
);

GOAL_STATUS_T goal_targetNetChanOptSet(
    GOAL_NET_CHAN_T *pChan,                     /**< channel handle */
    GOAL_NET_OPTION_T option,                   /**< net option */
    void *pValue                                /**< net option value */
);

void goal_targetNetChanPoll(
    GOAL_NET_CHAN_T *pChanList                  /**< Net channel list */
);

GOAL_BOOL_T goal_targetNetChanDataPresentCheck(
    void
);
#endif /* GOAL_CONFIG_TCPIP_STACK */


#if GOAL_CONFIG_TASK == 1
GOAL_STATUS_T goal_tgtTaskCreate(
    GOAL_TASK_T *pTask                          /**< GOAL task handle */
);

GOAL_STATUS_T goal_tgtTaskStart(
    GOAL_TASK_T *pTask                          /**< GOAL task handle */
);

GOAL_STATUS_T goal_tgtTaskExit(
    void
);

GOAL_STATUS_T goal_tgtTaskMsSleep(
    uint32_t msReq,                             /**< ms requested to sleep */
    uint32_t *pMsRem                            /**< remaining ms if interrupted */
);

GOAL_STATUS_T goal_tgtTaskTestSelf(
    GOAL_TASK_T *pTask                          /**< task handle */
);

GOAL_STATUS_T goal_tgtTaskPrioGet(
    GOAL_TASK_T *pTask,                         /**< task handle */
    uint32_t *pPrio                             /**< priority ref */
);

GOAL_STATUS_T goal_tgtTaskPrioSet(
    GOAL_TASK_T *pTask,                         /**< task handle */
    uint32_t prio                               /**< priority */
);

GOAL_STATUS_T goal_tgtTaskSuspend(
    GOAL_TASK_T *pTask                          /**< task handle */
);

GOAL_STATUS_T goal_tgtTaskResume(
    GOAL_TASK_T *pTask                          /**< task handle */
);
#endif /* GOAL_CONFIG_TASK == 1 */

#endif /* GOAL_TARGET_API_H */
