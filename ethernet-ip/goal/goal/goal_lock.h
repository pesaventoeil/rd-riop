/** @file
 *
 * @brief GOAL Locking Mechanisms
 *
 * This module implements binary and counting lock functions.
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

#ifndef GOAL_LOCK_H
#define GOAL_LOCK_H


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_lockInitPre(
    void
);

GOAL_STATUS_T goal_lockCreate(
    GOAL_LOCK_TYPE_T lockType,                  /**< GOAL_LOCK_BINARY or GOAL_LOCK_COUNT */
    GOAL_LOCK_T **ppLock,                       /**< pointer to lock ref */
    uint32_t valInit,                           /**< initial lock value */
    uint32_t valMax,                            /**< maximal lock value */
    GOAL_ID_T usage                             /**< usage indicator */
);

GOAL_STATUS_T goal_lockGet(
    GOAL_LOCK_T *pLock,                         /**< lock data */
    uint32_t timeout                            /**< lock timeout */
);

void goal_lockPut(
    GOAL_LOCK_T *pLock                          /**< lock data */
);

void goal_lockDelete(
    GOAL_LOCK_T *pLockId                        /**< lock ID */
);

GOAL_STATUS_T goal_lockGetIfTrue(
    GOAL_LOCK_T *pLock,                         /**< lock */
    uint32_t timeout,                           /**< lock timeout */
    volatile GOAL_BOOL_T *pVal                  /**< value to check */
);

GOAL_STATUS_T goal_lockGetIfFalse(
    GOAL_LOCK_T *pLock,                         /**< lock */
    uint32_t timeout,                           /**< lock timeout */
    volatile GOAL_BOOL_T *pVal                  /**< value to check */
);


#endif /* GOAL_LOCK_H */
