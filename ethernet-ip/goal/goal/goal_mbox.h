/** @file
 *
 * @brief Mailbox Management
 *
 * This module implements MAilboxes to exchange arbitrary messages between
 * tasks. All callers of these functions must know the size of a message for
 * each mailbox.
 *
 * @copyright
 * Copyright 2010-2021.
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

#ifndef GOAL_MBOX_H
#define GOAL_MBOX_H

#include "goal_includes.h"


/****************************************************************************/
/* Public Data types */
/****************************************************************************/
/** Mailbox handle */
typedef struct {
    GOAL_LOCK_T *pMutex;                        /**< mbox lock */
    uint16_t rdIdx;                             /**< read index */
    uint16_t wrIdx;                             /**< write index */
    uint16_t numMsg;                            /**< number of messages per mailbox */
    uint16_t sizeMsg;                           /**< size of each message */
    uint16_t flags;                             /**< flags */
    uint8_t *pMsgData;                          /**< message data area */
} GOAL_MBOX_T;


/****************************************************************************/
/* Public Functions */
/****************************************************************************/
GOAL_STATUS_T goal_mboxCreate(
    GOAL_MBOX_T **ppMbox,                       /**< [out] Mailbox pointer reference */
    uint16_t numMsg,                            /**< number of messages */
    uint16_t sizeMsg                            /**< size of each message */
);

GOAL_STATUS_T goal_mboxLockFreeCreate(
    GOAL_MBOX_T **ppMbox,                       /**< [out] Mailbox pointer reference */
    uint16_t numMsg,                            /**< number of messages */
    uint16_t sizeMsg                            /**< size of each message */
);

GOAL_STATUS_T goal_mboxDestroy(
    GOAL_MBOX_T **ppMbox                        /**< [in out] Mailbox pointer reference */
);

GOAL_STATUS_T goal_mboxMsgGet(
    GOAL_MBOX_T *pMbox,                         /**< mailbox */
    uint8_t *pMsg                               /**< [out] message buffer */
);

GOAL_STATUS_T goal_mboxMsgPut(
    GOAL_MBOX_T *pMbox,                         /**< mailbox */
    uint8_t *pMsg                               /**< [in] message buffer */
);

uint8_t goal_mboxIsFull(
    GOAL_MBOX_T *pMbox                          /**< mailbox */
);

uint8_t goal_mboxIsEmpty(
    GOAL_MBOX_T *pMbox                          /**< mailbox */
);


#endif /* GOAL_MBOX_H */
