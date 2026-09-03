/** @file
 *
 * @brief Generic MA for EVENT
 *
 * @details
 * This module provides a MA for EVENT access
 *
 *
 * @copyright
 * Copyright 2010-2020.
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
#ifndef GOAL_MA_EVENT_H
#define GOAL_MA_EVENT_H


/****************************************************************************/
/* Defines */
/****************************************************************************/


/****************************************************************************/
/* Forward declaration */
/****************************************************************************/
struct GOAL_MA_EVENT_T;                          /**< MA EVENT handle */


/****************************************************************************/
/* Typedefs */
/****************************************************************************/

/**< open function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_EVENT_OPEN)(
    struct GOAL_MA_EVENT_T *pHdlMaEvent,        /**< MA handle for EVENT  */
    GOAL_BOOL_T flgAutoStart                    /**< auto start of event generation */
);

/**< start function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_EVENT_ENABLE) (
    struct GOAL_MA_EVENT_T *pHdlMaEvent         /**< MA handle for EVENT  */
);

/**< clear function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_EVENT_DISABLE)(
    struct GOAL_MA_EVENT_T *pHdlMaEvent         /**< MA handle for EVENT  */
);

/**< set pending function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_EVENT_SET_PENDING)(
    struct GOAL_MA_EVENT_T *pHdlMaEvent         /**< MA handle for EVENT  */
);

/**< poll function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_EVENT_POLL)(
    struct GOAL_MA_EVENT_T *pHdlMaEvent,        /**< MA handle for EVENT */
    GOAL_BOOL_T *pState                         /**< event state */
);

/**< event flag and configuation mode */
typedef enum {
    GOAL_MA_EVENT_RISING_EDGE = 1,              /**< rising edge */
    GOAL_MA_EVENT_FALLING_EDGE = 2              /**< falling edge */
} GOAL_MA_EVENT_EVENT_T;

/**< notify function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_EVENT_NOTIFY)(
    struct GOAL_MA_EVENT_T *pEventHdl,          /**< handle where the event occurred */
    uint32_t id,                                /**< event id */
    GOAL_MA_EVENT_EVENT_T event                 /**< event type */
);

/**< EVENT handle instance struct */
typedef struct GOAL_MA_EVENT_T {
    GOAL_INSTANCE_HEADER(GOAL_MA_EVENT_T);      /**< instance header */
    GOAL_BOOL_T flgUsed;                        /**< used flag */
    GOAL_BOOL_T flgEnable;                      /**< enable flag */
    GOAL_MA_EVENT_OPEN fctOpen;                 /**< open EVENT function */
    GOAL_MA_EVENT_ENABLE fctEnable;             /**< start EVENT function */
    GOAL_MA_EVENT_DISABLE fctDisable;           /**< clear EVENT function */
    GOAL_MA_EVENT_POLL fctPoll;                 /**< poll EVENT function */
    GOAL_MA_EVENT_SET_PENDING fctSetPending;    /**< set EVENT pending function */
    GOAL_MA_EVENT_NOTIFY fctNotify;             /**< application cb */
    void *pDrvData;                             /**< driver specific data */
} GOAL_MA_EVENT_T;


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_maEventDrvReg(
    uint32_t id,                                /**< ID */
    GOAL_MA_EVENT_T **ppHdlMaEvent,             /**< EVENT handle ref ptr */
    GOAL_MA_EVENT_OPEN fOpen,                   /**< open function */
    GOAL_MA_EVENT_ENABLE fEnable,               /**< start function */
    GOAL_MA_EVENT_DISABLE fDisable,             /**< clear function */
    GOAL_MA_EVENT_SET_PENDING fSetPending,      /**< set pending function */
    GOAL_MA_EVENT_POLL fPoll,                   /**< poll function */
    void *pDrvData                              /**< driver specific data */
);

GOAL_STATUS_T goal_maEventOpen(
    uint32_t id,                                /**< id of EVENT handler to use */
    GOAL_MA_EVENT_T **ppEventHdl,               /**< pointer to store EVENT handler */
    GOAL_BOOL_T flgAutoStart,                   /**< auto start of event generation */
    GOAL_MA_EVENT_NOTIFY fNotify                /**< notify callback */
);

GOAL_STATUS_T goal_maEventEnable(
    GOAL_MA_EVENT_T *pEventHdl                  /**< MA handle for EVENT */
);

GOAL_STATUS_T goal_maEventDisable(
    GOAL_MA_EVENT_T *pEventHdl                  /**< MA handle for EVENT */
);

GOAL_STATUS_T goal_maEventPoll(
    GOAL_MA_EVENT_T *pHdlMaEvent,               /**< MA handle for EVENT */
    GOAL_BOOL_T *pState                         /**< event state */
);

GOAL_STATUS_T goal_maEventSetPending(
    GOAL_MA_EVENT_T *pEventHdl                  /**< MA handle for EVENT */
);

GOAL_STATUS_T goal_maEventGetById(
    GOAL_MA_EVENT_T **ppHdlMaEvent,             /**< EVENT handle ref ptr */
    uint32_t id                                 /**< MA id */
);

GOAL_STATUS_T goal_maEventDrvNotifyCb(
    GOAL_MA_EVENT_T *pHdlMaEvent,               /**< MA handle for EVENT */
    uint32_t id,                                /**< id of event */
    GOAL_MA_EVENT_EVENT_T event                 /**< event type */
);

#endif /* GOAL_MA_EVENT_H */
