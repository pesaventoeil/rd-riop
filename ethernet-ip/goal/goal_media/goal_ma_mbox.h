/** @file
 *
 * @brief GOAL Mailbox Media Adapter
 *
 * @details
 * Generic media adapter for mailboxes.
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

#ifndef GOAL_MA_MBOX_H
#define GOAL_MA_MBOX_H


/****************************************************************************/
/* Forward declarations */
/****************************************************************************/
struct GOAL_MA_MBOX_T;


/****************************************************************************/
/* Typedefs */
/****************************************************************************/
typedef GOAL_STATUS_T (*GOAL_MA_MBOX_POLL_T)(\
    struct GOAL_MA_MBOX_T *pMbox                /**< mailbox handle */  \
);

typedef GOAL_STATUS_T (*GOAL_MA_MBOX_RECV_T)(\
    struct GOAL_MA_MBOX_T *pMbox,               /**< mailbox handle */  \
    uint8_t *pData,                             /**< data ptr */        \
    unsigned int len                            /**< data length */     \
);

typedef GOAL_STATUS_T (*GOAL_MA_MBOX_SEND_T)(\
    struct GOAL_MA_MBOX_T *pMbox,               /**< mailbox handle */  \
    uint8_t *pData,                             /**< data ptr */        \
    unsigned int len                            /**< data length */     \
);

typedef GOAL_STATUS_T (*GOAL_MA_MBOX_ALLOC)(\
    struct GOAL_MA_MBOX_T *pMbox,               /**< mailbox driver handle */
    GOAL_MA_MBOX_RECV_T funcRecv                /**< receive callback function */
);

typedef GOAL_STATUS_T (*GOAL_MA_MBOX_FREE)(\
    struct GOAL_MA_MBOX_T *pHdlMbox             /**< mailbox MA handle */
);


/****************************************************************************/
/* Structures */
/****************************************************************************/
typedef struct GOAL_MA_MBOX_T {
    struct GOAL_MA_MBOX_T *pNext;               /**< next element */
    GOAL_BOOL_T flgTxUsed;                      /**< flag for transmission in use */
    GOAL_MA_MBOX_POLL_T funcPoll;               /**< poll data function */
    GOAL_MA_MBOX_RECV_T funcRecv;               /**< receive callback function */
    GOAL_MA_MBOX_SEND_T funcSend;               /**< send function */
    GOAL_MA_MBOX_ALLOC funcAlloc;               /**< allocation function */
    GOAL_MA_MBOX_FREE funcFree;                 /**< free function */
    void *pData;                                /**< driver specific data */
} GOAL_MA_MBOX_T;


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_maMboxReg(
    GOAL_MA_MBOX_T **ppMbox,                    /**< mailbox handle ref ptr */
    GOAL_MA_MBOX_POLL_T funcPoll,               /**< poll function */
    GOAL_MA_MBOX_SEND_T funcSend,               /**< send function */
    GOAL_MA_MBOX_ALLOC funcAlloc,               /**< alloc function */
    GOAL_MA_MBOX_FREE funcFree,                 /**< free function */
    void *pData                                 /**< driver specific data */
);

GOAL_STATUS_T goal_maMboxGetByIdx(
    GOAL_MA_MBOX_T **ppMbox,                    /**< ptr ref for mailbox driver handle */
    unsigned int idx                            /**< mailbox index */
);

GOAL_STATUS_T goal_maMboxAlloc(
    GOAL_MA_MBOX_T *pMbox,                      /**< mailbox driver handle */
    GOAL_MA_MBOX_RECV_T funcRecv                /**< receive callback function */
);

GOAL_STATUS_T goal_maMboxFree(
    GOAL_MA_MBOX_T *pMbox                       /**< mailbox driver handle */
);

GOAL_STATUS_T goal_maMboxPoll(
    GOAL_MA_MBOX_T *pMbox                       /**< mailbox driver handle */
);

GOAL_STATUS_T goal_maMboxRecv(
    GOAL_MA_MBOX_T *pMbox,                      /**< mailbox driver handle */
    uint8_t *pData,                             /**< data ptr */
    unsigned int len                            /**< data length */
);

GOAL_STATUS_T goal_maMboxSend(
    GOAL_MA_MBOX_T *pMbox,                      /**< mailbox driver handle */
    uint8_t *pData,                             /**< data ptr */
    unsigned int len                            /**< data length */
);


#endif /* GOAL_MA_MBOX_H */
