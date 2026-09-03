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

#define GOAL_ID GOAL_ID_MA_MBOX
#include <goal_includes.h>
#include <goal_media/goal_ma_mbox.h>


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_MA_MBOX_T *pListMbox = NULL;        /**< mailbox list */
static unsigned int cntMbox = 0;                /**< count of registered mailboxes */


/****************************************************************************/
/** Register mailbox driver
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maMboxReg(
    GOAL_MA_MBOX_T **ppMbox,                    /**< mailbox handle ref ptr */
    GOAL_MA_MBOX_POLL_T funcPoll,               /**< poll function */
    GOAL_MA_MBOX_SEND_T funcSend,               /**< send function */
    GOAL_MA_MBOX_ALLOC funcAlloc,               /**< alloc function */
    GOAL_MA_MBOX_FREE funcFree,                 /**< free function */
    void *pData                                 /**< driver specific data */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MA_MBOX_T **ppElem;                    /* mailbox element */

    /* allocate mailbox handle */
    res = goal_memCalloc(ppMbox, sizeof(GOAL_MA_MBOX_T));
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to allocate mailbox handle");
        return res;
    }

    /* setup mailbox handle */
    (*ppMbox)->funcPoll = funcPoll;
    (*ppMbox)->funcSend = funcSend;
    (*ppMbox)->pData = pData;
    (*ppMbox)->funcAlloc = funcAlloc;
    (*ppMbox)->funcFree = funcFree;

    /* iterate to list end */
    for (ppElem = &pListMbox; *ppElem; ppElem = &(*ppElem)->pNext);

    /* store mailbox handle */
    *ppElem = *ppMbox;
    cntMbox++;

    return GOAL_OK;
}


/****************************************************************************/
/** Get mailbox by index
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maMboxGetByIdx(
    GOAL_MA_MBOX_T **ppMbox,                    /**< ptr ref for mailbox driver handle */
    unsigned int idx                            /**< mailbox index */
)
{
    GOAL_MA_MBOX_T *pElem;                      /* mailbox element */
    unsigned int cnt;                           /* mailbox count */

    /* verify mailbox count */
    if (cntMbox <= idx) {
        goal_logErr("mailbox index not available");
        return GOAL_ERR_OUT_OF_RANGE;
    }

    /* return mailbox by index */
    for (pElem = pListMbox, cnt = 0; (pElem) && (cnt < idx); pElem = pElem->pNext, cnt++);

    if (NULL == pElem) {
        goal_logErr("failed to retrieve mailbox by index");
        return GOAL_ERR_NULL_POINTER;
    }

    /* return mailbox handle */
    *ppMbox = pElem;

    return GOAL_OK;
}


/****************************************************************************/
/** Allocate mailbox driver
 *
 * Allocation of a mailbox driver. The destination ID reference and receive
 * callback function define the transmit or receive functionality of the
 * mailbox.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maMboxAlloc(
    GOAL_MA_MBOX_T *pMbox,                      /**< mailbox driver handle */
    GOAL_MA_MBOX_RECV_T funcRecv                /**< receive callback function */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    if (NULL == pMbox) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* quit if the handle is used for send and receive or one of both option would be reconfigured */
    if (((NULL != pMbox->funcRecv) && (GOAL_FALSE != pMbox->flgTxUsed))
        || ((NULL == pMbox->funcRecv) && (GOAL_TRUE == pMbox->flgTxUsed))
        || ((NULL != pMbox->funcRecv) && (NULL != funcRecv))) {
        return GOAL_ERR_ALREADY_USED;
    }

    if (pMbox->funcAlloc) {
        res = pMbox->funcAlloc(pMbox, funcRecv);
    }

    if (GOAL_RES_OK(res)) {
        if (funcRecv) {
            pMbox->funcRecv = funcRecv;
        }
        else {
            pMbox->flgTxUsed = GOAL_TRUE;
        }
    }

    return res;
}


/****************************************************************************/
/** Release mailbox driver
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maMboxFree(
    GOAL_MA_MBOX_T *pMbox                       /**< mailbox driver handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    if (NULL == pMbox) {
        return GOAL_ERR_NULL_POINTER;
    }

    if ((GOAL_FALSE == pMbox->flgTxUsed) && (NULL == pMbox->funcRecv)) {
        return GOAL_ERR_WRONG_STATE;
    }

    if (pMbox->funcFree) {
        res = pMbox->funcFree(pMbox);
    }

    if (GOAL_RES_OK(res)) {
        pMbox->flgTxUsed = GOAL_FALSE;
        pMbox->funcRecv = NULL;
    }

    return res;
}


/****************************************************************************/
/** Check if mailbox data is available
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maMboxPoll(
    GOAL_MA_MBOX_T *pMbox                       /**< mailbox driver handle */
)
{
    if ((NULL != pMbox) && (NULL != pMbox->funcPoll)) {
        return pMbox->funcPoll(pMbox);
    }
    return GOAL_ERR_NULL_POINTER;
}


/****************************************************************************/
/** Call receive callback
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maMboxRecv(
    GOAL_MA_MBOX_T *pMbox,                      /**< mailbox driver handle */
    uint8_t *pData,                             /**< data ptr */
    unsigned int len                            /**< data length in byte */
)
{
    if ((NULL != pMbox) && (NULL != pMbox->funcRecv)) {
        return pMbox->funcRecv(pMbox, pData, len);
    }
    return GOAL_ERR_NULL_POINTER;
}


/****************************************************************************/
/** Send data via mailbox
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maMboxSend(
    GOAL_MA_MBOX_T *pMbox,                      /**< mailbox driver handle */
    uint8_t *pData,                             /**< data ptr */
    unsigned int len                            /**< data length */
)
{
    if ((NULL != pMbox) && (NULL != pMbox->funcSend)) {
        if (GOAL_TRUE == pMbox->flgTxUsed) {
            return pMbox->funcSend(pMbox, pData, len);
        }
        return GOAL_ERR_UNSUPPORTED;

    }
    return GOAL_ERR_NULL_POINTER;
}
