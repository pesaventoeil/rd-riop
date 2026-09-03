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

#define GOAL_ID GOAL_ID_MBOX
#include <goal_includes.h>


/****************************************************************************/
/* Local Defines */
/****************************************************************************/
#define GOAL_MBOX_FLAG_EMPTY             (1<<0) /**< mailbox is empty */
#define GOAL_MBOX_FLAG_FULL              (1<<1) /**< mailbox is full */
#define GOAL_MBOX_FLAG_LOCK              (1<<2) /**< mailbox has a lock */


/****************************************************************************/
/* Local functions */
/****************************************************************************/
static GOAL_STATUS_T goal_mboxInternalCreate(
    GOAL_MBOX_T **ppMbox,                       /**< [out] Mailbox pointer reference */
    uint16_t numMsg,                            /**< number of messages */
    uint16_t sizeMsg,                           /**< size of each message */
    GOAL_BOOL_T isLockFree                      /**< mailbox is lock free */
);


/****************************************************************************/
/** Create a mailbox for concurrent access
 *
 * This function allocates and initializes a mailbox that is thread-safe.
 * Its data area is allocated to store @em numMsg messages. Each messages is
 * expected to be @em sizeMsg bytes.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_mboxCreate(
    GOAL_MBOX_T **ppMbox,                       /**< [out] Mailbox pointer reference */
    uint16_t numMsg,                            /**< number of messages */
    uint16_t sizeMsg                            /**< size of each message */
)
{
    return goal_mboxInternalCreate(ppMbox, numMsg, sizeMsg, GOAL_FALSE);
}


/****************************************************************************/
/** Create a lock free mailbox
 *
 * This function allocates and initializes a mailbox that is lock free.
 * Only one consumer or producer can use this mailbox at a time.
 * Its data area is allocated to store @em numMsg messages. Each messages is
 * expected to be @em sizeMsg bytes.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_mboxLockFreeCreate(
    GOAL_MBOX_T **ppMbox,                       /**< [out] Mailbox pointer reference */
    uint16_t numMsg,                            /**< number of messages */
    uint16_t sizeMsg                            /**< size of each message */
)
{
    return goal_mboxInternalCreate(ppMbox, numMsg, sizeMsg, GOAL_TRUE);
}


/****************************************************************************/
/** Common implementation of mailbox creation function
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_mboxInternalCreate(
    GOAL_MBOX_T **ppMbox,                       /**< [out] Mailbox pointer reference */
    uint16_t numMsg,                            /**< number of messages */
    uint16_t sizeMsg,                           /**< size of each message */
    GOAL_BOOL_T isLockFree                      /**< mailbox is lock free */
)
{
    GOAL_STATUS_T res;                          /* result */

    if ((NULL == ppMbox) || (0 == numMsg) || (0 == sizeMsg)) {
        return GOAL_ERR_PARAM;
    }

    /* allocate mbox */
    res = goal_memCalloc(ppMbox, sizeof(GOAL_MBOX_T));
    if (GOAL_RES_ERR(res)) {
        goal_logErr("could not create mailbox");
    }

    if (GOAL_RES_OK(res)) {
        /* allocate data area */
        res = goal_memCalloc(&((*ppMbox)->pMsgData), numMsg * sizeMsg);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("could not create mailbox data area");
        }
    }

    if (GOAL_RES_OK(res) && (GOAL_FALSE == isLockFree)) {
        /* create mutex */
        res = goal_lockCreate(GOAL_LOCK_BINARY, &((*ppMbox)->pMutex), 0, 1, GOAL_ID);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to create mailbox lock");
        }
    }

    if (GOAL_RES_OK(res)) {
        /* initialize members that are not zero */
        (*ppMbox)->flags = GOAL_MBOX_FLAG_EMPTY;
        (*ppMbox)->numMsg = numMsg;
        (*ppMbox)->sizeMsg = sizeMsg;
    }

    if (GOAL_FALSE == isLockFree) {
        (*ppMbox)->flags |= GOAL_MBOX_FLAG_LOCK;
    }

    return res;
}


/****************************************************************************/
/** Destroy a mailbox
 *
 * This function frees the mailbox and its data. After calling this function
 * @em ppMbox is set to NULL.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_mboxDestroy(
    GOAL_MBOX_T **ppMbox                        /**< [in out] Mailbox pointer reference */
)
{
    GOAL_STATUS_T res;                          /* result */

    if (NULL == ppMbox) {
        return GOAL_ERR_PARAM;
    }

    if ((*ppMbox)->flags & GOAL_MBOX_FLAG_LOCK) {
        goal_lockDelete((*ppMbox)->pMutex);
    }

    res = goal_memFree(&((*ppMbox)->pMsgData));

    if (GOAL_RES_OK(res)) {
        res = goal_memFree(ppMbox);
    }

    if (GOAL_RES_OK(res)) {
        *ppMbox = NULL;
    }

    return res;
}


/****************************************************************************/
/** Return first new message from a given mailbox
 *
 * This function is thread-safe. The message size is the value specified with
 * @em goal_mboxCreate.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_mboxMsgGet(
    GOAL_MBOX_T *pMbox,                         /**< mailbox */
    uint8_t *pMsg                               /**< [out] message buffer */
)
{
    GOAL_STATUS_T retVal = GOAL_OK;             /* return value */
    uint16_t rdIdxLocal;                        /* local read index */

    if ((NULL == pMbox) || (NULL == pMsg)) {
        return GOAL_ERR_PARAM;
    }

    /* fast-return if mailbox is empty */
    if (pMbox->flags & GOAL_MBOX_FLAG_EMPTY) {
        return GOAL_ERR_QUEUE_EMPTY;
    }

    if (pMbox->flags & GOAL_MBOX_FLAG_LOCK) {
        goal_lockGet(pMbox->pMutex, GOAL_LOCK_INFINITE);
    }

    rdIdxLocal = pMbox->rdIdx;

    /* get message if mailbox isn't empty */
    if (0 == (pMbox->flags & GOAL_MBOX_FLAG_EMPTY)) {
        GOAL_MEMCPY(pMsg, &pMbox->pMsgData[rdIdxLocal * pMbox->sizeMsg], pMbox->sizeMsg);
        rdIdxLocal = (rdIdxLocal + 1) % pMbox->numMsg;

        /* set empty flag if mailbox is now empty */
        if (rdIdxLocal == pMbox->wrIdx) {
            pMbox->flags |= GOAL_MBOX_FLAG_EMPTY;
        }

        /* remove full flag */
        pMbox->flags &= ~GOAL_MBOX_FLAG_FULL;
    }
    else {
        retVal = GOAL_ERR_QUEUE_EMPTY;
        goal_logDbg("mailbox empty: %p", (void *) pMbox);
    }

    pMbox->rdIdx = rdIdxLocal;

    if (pMbox->flags & GOAL_MBOX_FLAG_LOCK) {
        /* release mutex */
        goal_lockPut(pMbox->pMutex);
    }

    return retVal;
}


/****************************************************************************/
/** Add a message to a given mailbox
 *
 * This function is thread-safe. The message size is the value specified with
 * @em goal_mboxCreate.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_mboxMsgPut(
    GOAL_MBOX_T *pMbox,                         /**< mailbox */
    uint8_t *pMsg                               /**< [in] message buffer */
)
{
    GOAL_STATUS_T retVal = GOAL_OK;             /* return value */
    uint16_t wrIdxLocal;                        /* local write index */

    if ((NULL == pMbox) || (NULL == pMsg)) {
        return GOAL_ERR_PARAM;
    }

    /* fast-return if mailbox is full */
    if (pMbox->flags & GOAL_MBOX_FLAG_FULL) {
        return GOAL_ERR_QUEUE_FULL;
    }

    if (pMbox->flags & GOAL_MBOX_FLAG_LOCK) {
        goal_lockGet(pMbox->pMutex, GOAL_LOCK_INFINITE);
    }

    wrIdxLocal = pMbox->wrIdx;

    /* only add message if mailbox isn't full */
    if (0 == (pMbox->flags & GOAL_MBOX_FLAG_FULL)) {

        GOAL_MEMCPY(&pMbox->pMsgData[wrIdxLocal * pMbox->sizeMsg], pMsg, pMbox->sizeMsg);
        wrIdxLocal = (wrIdxLocal + 1) % pMbox->numMsg;

        /* set full flag if mailbox is now full */
        if (pMbox->rdIdx == wrIdxLocal) {
            pMbox->flags |= GOAL_MBOX_FLAG_FULL;
        }

        /* remove empty flag */
        pMbox->flags &= ~GOAL_MBOX_FLAG_EMPTY;
    }
    else {
        retVal = GOAL_ERR_QUEUE_FULL;
        goal_logDbg("mailbox full: %p", (void *) pMbox);
    }

    pMbox->wrIdx = wrIdxLocal;

    if (pMbox->flags & GOAL_MBOX_FLAG_LOCK) {
        /* release mutex */
        goal_lockPut(pMbox->pMutex);
    }

    return retVal;
}


/****************************************************************************/
/** Check if a mailbox is full
 *
 * @retval 0 mailbox is not full
 * @retval 1 mailbox is full
 */
uint8_t goal_mboxIsFull(
    GOAL_MBOX_T *pMbox                          /**< mailbox */
)
{
    return (pMbox->flags & GOAL_MBOX_FLAG_FULL) ? 1 : 0;
}


/****************************************************************************/
/** Check if a mailbox is empty
 *
 * @retval 0 mailbox is not empty
 * @retval 1 mailbox is empty
 */
uint8_t goal_mboxIsEmpty(
    GOAL_MBOX_T *pMbox                          /**< mailbox */
)
{
    return (pMbox->flags & GOAL_MBOX_FLAG_EMPTY) ? 1 : 0;
}
