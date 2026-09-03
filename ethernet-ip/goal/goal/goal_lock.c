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

#define GOAL_ID GOAL_ID_LOCK
#include <goal_includes.h>


/****************************************************************************/
/* Local defined variables */
/****************************************************************************/
static GOAL_STAGE_HANDLER_T stageInit;          /**< init stage handler */
static GOAL_STAGE_HANDLER_T stageShutdown;      /**< shutdown stage handler */
static GOAL_LOCK_T *mpLockSoft;                 /**< real lock for soft locking */


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/
static GOAL_STATUS_T goal_lockInit(
    void
);

static GOAL_STATUS_T goal_lockShutdown(
    void
);


/****************************************************************************/
/** Register init and shutdown handlers.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_lockInitPre(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* initialize locking */
    res = goal_mainStageReg(GOAL_STAGE_LOCK_PRE, &stageInit, GOAL_STAGE_INIT, goal_lockInit);

    /* shutdown locking */
    if (GOAL_RES_OK(res)) {
        res = goal_mainStageReg(GOAL_STAGE_LOG_PRE, &stageShutdown, GOAL_STAGE_SHUTDOWN, goal_lockShutdown);
    }

    return res;
}


/****************************************************************************/
/** Lock init handler
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_lockInit(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* initialize platform locks */
    res = goal_targetLockInit();
    GOAL_RES_ERR_RET(res, "goal_targetLockInit");

    /* create real lock for soft locking */
    res = goal_lockCreate(GOAL_LOCK_BINARY, &mpLockSoft, 0, 1, GOAL_ID_LOCK);
    GOAL_RES_ERR_RET(res, "goal_lockCreate");

    return GOAL_OK;
}


/****************************************************************************/
/** Lock shutdown handler
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_lockShutdown(
    void
)
{
    return goal_targetLockShutdown();
}


/****************************************************************************/
/** Create a Binary or Counting Lock
 *
 * A binary lock represents a mutex, where a counting lock represents a
 * semaphore. When a counting lock is created, the initial and maximal value can
 * be set. For example if 3 instances of a resource are available and 2 should
 * be marked as already in use, set valInit to 2 and valMax to 3.
 *
 * This function halts the device via goal_targetHalt() if something went wrong.
 */
GOAL_STATUS_T goal_lockCreate(
    GOAL_LOCK_TYPE_T lockType,                  /**< GOAL_LOCK_BINARY or GOAL_LOCK_COUNT */
    GOAL_LOCK_T **ppLock,                       /**< pointer to lock ref */
    uint32_t valInit,                           /**< initial lock value */
    uint32_t valMax,                            /**< maximal lock value */
    GOAL_ID_T usage                             /**< usage indicator */
)
{
    GOAL_STATUS_T res;                          /* result */

    GOAL_HALT_IF_NULL(ppLock);
    GOAL_ASSERT(NULL == *ppLock);

    /* override init and maximum value for binary locks */
    if ((GOAL_LOCK_BINARY == lockType) || (GOAL_LOCK_BINARY_SOFT == lockType)) {
        valInit = 0;
        valMax = 1;
    }

    /* allocate lock data */
    res = goal_memCalloc(ppLock, sizeof(GOAL_LOCK_T));
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:135: */
        goal_lmLog(GOAL_ID_LOCK, 135, 106, 0, 0, GOAL_LOG_SEV_ERROR, "allocating lock structure failed");
        return res;
    }

    /* initialize lock data */
    (*ppLock)->type = lockType;
    (*ppLock)->usage = usage;

    /* create target lock */
    if ((GOAL_LOCK_BINARY == lockType) || (GOAL_LOCK_COUNT == lockType)) {
        res = goal_targetLockCreate(lockType, *ppLock, valInit, valMax);
        if (GOAL_RES_ERR(res)) {
            /* GG_LOG ID:136: */
            goal_lmLog(GOAL_ID_LOCK, 136, 107, 0, 0, GOAL_LOG_SEV_ERROR, "failed to create lock on target");
            return res;
        }
    }

    /* enable lock */
    (*ppLock)->active = GOAL_TRUE;

    return GOAL_OK;
}


/****************************************************************************/
/** Acquire a Lock
 *
 * Enter a binary or counting lock.
 * If this is not possible, the function will sleep until the timeout (in ms)
 * is reached. Setting the timeout to zero, will result in an infinite wait.
 *
 * @returns GOAL_OK - success
 * @returns GOAL_ERR_LOCK_GET_TIMEOUT - timeout reached
 * @returns other - fail
 */
GOAL_STATUS_T goal_lockGet(
    GOAL_LOCK_T *pLock,                         /**< lock data */
    uint32_t timeout                            /**< lock timeout */
)
{
    GOAL_STATUS_T res;                          /* locking result */

    GOAL_HALT_IF_NULL(pLock);
    GOAL_HALT_IF_INACTIVE(pLock);

    /* use general lock for soft locks */
    if (GOAL_LOCK_BINARY_SOFT == pLock->type) {
        pLock = mpLockSoft;
    }

    /* check if lock got a timeout */
    res = goal_targetLockGet(pLock, timeout);
    if (GOAL_OK != res) {
        if (timeout && (GOAL_ERR_LOCK_GET_TIMEOUT == res)) {
            goal_logDbg("Lock timeout");
            return res;
        }

        /* GG_LOG ID:137: */
        goal_lmLog(GOAL_ID_LOCK, 137, 108, 0, 0, GOAL_LOG_SEV_ERROR, "Could not get lock, halting device.");
        goal_targetHalt();
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Release a Lock
 *
 * Leave a binary or counting lock.
 */
void goal_lockPut(
    GOAL_LOCK_T *pLock                          /**< lock data */
)
{
    GOAL_HALT_IF_NULL(pLock);
    GOAL_HALT_IF_INACTIVE(pLock);

    /* use general lock for soft locks */
    if (GOAL_LOCK_BINARY_SOFT == pLock->type) {
        pLock = mpLockSoft;
    }

    if (GOAL_OK != goal_targetLockPut(pLock)) {
        /* GG_LOG ID:138: */
        goal_lmLog(GOAL_ID_LOCK, 138, 109, 0, 0, GOAL_LOG_SEV_ERROR, "Could not release lock, halting device.");
        goal_targetHalt();
    }
}


/****************************************************************************/
/** Free Lock Resources
 *
 * Cleans up the lock resources.
 *
 * This function halts the device via goal_targetHalt() if something went wrong.
 */
void goal_lockDelete(
    GOAL_LOCK_T *pLock                          /**< lock data */
)
{
    GOAL_HALT_IF_NULL(pLock);
    GOAL_HALT_IF_INACTIVE(pLock);

    /* only delete real locks */
    if (GOAL_LOCK_BINARY_SOFT != pLock->type) {
        if (GOAL_OK != goal_targetLockDelete(pLock)) {
            /* GG_LOG ID:139: */
            goal_lmLog(GOAL_ID_LOCK, 139, 110, 0, 0, GOAL_LOG_SEV_ERROR, "Lock could not be deleted - halt");
            goal_targetHalt();
        }
    }

    pLock->active = GOAL_FALSE;
}


/****************************************************************************/
/** Aquire Lock if given Boolean is True without and after Locking
 *
 * This function performs a lock-free check if a given boolean variable is true
 * and if that's the case, the given lock is locked and the variable is
 * verified again. If it's still true the lock is hold otherwise it is freed.
 *
 * Advantage: Checks where a lot of elements have to be iterated to find a used
 * item can be performed without having to much locks.
 */
GOAL_STATUS_T goal_lockGetIfTrue(
    GOAL_LOCK_T *pLock,                         /**< lock */
    uint32_t timeout,                           /**< lock timeout */
    volatile GOAL_BOOL_T *pVal                  /**< value to check */
)
{
    if (GOAL_TRUE == *pVal) {
        if (GOAL_OK == goal_lockGet(pLock, timeout)) {
            if (GOAL_TRUE == *pVal) {
                return GOAL_OK;
            } else {
                goal_lockPut(pLock);
            }
        }
    }

    return GOAL_ERR_LOCK_WRONG_STATE;
}


/****************************************************************************/
/** Aquire Lock if given Boolean is False without and after Locking
 *
 * This function performs a lock-free check if a given boolean variable is
 * false and if that's the case, the given lock is locked and the variable is
 * verified again. If it's still false the lock is hold otherwise it is freed.
 *
 * Advantage: Checks where a lot of elements have to be iterated to find a used
 * item can be performed without having to much locks.
 */
GOAL_STATUS_T goal_lockGetIfFalse(
    GOAL_LOCK_T *pLock,                         /**< lock */
    uint32_t timeout,                           /**< lock timeout */
    volatile GOAL_BOOL_T *pVal                  /**< value to check */
)
{
    if (GOAL_FALSE == *pVal) {
        if (GOAL_OK == goal_lockGet(pLock, timeout)) {
            if (GOAL_FALSE == *pVal) {
                return GOAL_OK;
            } else {
                goal_lockPut(pLock);
            }
        }
    }

    return GOAL_ERR_LOCK_WRONG_STATE;
}
