/** @file
 *
 * @brief
 * GOAL Generic Locking Implementation
 *
 * @details
 * This module implements locking mechanisms via interrupt disabling and
 * enabling. The target implementation must provide the two macros
 * GOAL_TARGET_LOCK_INT_ENABLE and GOAL_TARGET_LOCK_INT_DISABLE.
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

#define GOAL_ID GOAL_ID_TGT
#include <goal_includes.h>

#if !defined(GOAL_TARGET_LOCK_INT_ENABLE) || !defined(GOAL_TARGET_LOCK_INT_DISABLE)
#  error "Error, don't compile this file without providing interrupt handling macros."
#endif


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static volatile uint32_t g_lockCnt = 0;         /**< global lock counter */

#if GOAL_CONFIG_DEBUG_CONTEXT_LOCKING == 1
volatile GOAL_LOCK_CONTEXT_T gLockContext = GOAL_LOCK_CTX_NORMAL; /**< global setable lock context */
static volatile GOAL_LOCK_CONTEXT_T g_lockCtx;  /**< context of last taken lock */
#endif


/****************************************************************************/
/** Initialize Locking
 *
 * Initialize target locking.
 *
 * @return GOAL_OK - success
 * @return other - fail
 */
GOAL_STATUS_T goal_targetLockInit(
    void
)
{
    return GOAL_OK;
}


/****************************************************************************/
/** Shutdown Locking
 *
 * Shutdown target locking.
 *
 * @return GOAL_OK - success
 * @return other - fail
 */
GOAL_STATUS_T goal_targetLockShutdown(
    void
)
{
    return GOAL_OK;
}


/****************************************************************************/
/** Create a Binary or Counting Lock
 *
 * A binary lock represents a mutex, where a counting lock represents a
 * semaphore. When a counting lock is created, the inital an maximal value can
 * be set. For example if 3 instances of a ressource are available and 2 should
 * be marked as already in use, set the valInit to 2 and the valMax to 3.
 *
 * It is not necessary to set pLock->type to lockType, because this will be
 * done in goal_lock.c.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_targetLockCreate(
    GOAL_LOCK_TYPE_T lockType,                  /**< GOAL_LOCK_BINARY or GOAL_LOCK_COUNT */
    GOAL_LOCK_T *pLock,                         /**< pointer to store the lock data */
    uint32_t valInit,                           /**< initial lock value */
    uint32_t valMax                             /**< maximal lock value */
)
{
    UNUSEDARG(lockType);
    UNUSEDARG(pLock);
    UNUSEDARG(valInit);
    UNUSEDARG(valMax);

    return GOAL_OK;
}


/****************************************************************************/
/** Aquire a Lock
 *
 * Enter a binary or counting lock.
 * If this is not possible, the function will sleep until the timeout (in ms)
 * is reached. Setting the timeout to zero, will result in an infinite wait.
 *
 * Make sure the lock can be called recursively by the same thread (or when in
 * IRQ context, by the currently running IRQ). This is for example necessary,
 * because the timer handler locks the timer structure while calling the timer
 * callback and the timer callback must be able to create new or stop active
 * timers.
 *
 * @retval GOAL_OK success
 * @retval GOAL_ERR_LOCK_GET_TIMEOUT timeout reached
 * @retval other fail
 */
GOAL_STATUS_T goal_targetLockGet(
    GOAL_LOCK_T *pLock,                         /**< lock data */
    uint32_t timeout                            /**< lock timeout */
)
{
    UNUSEDARG(pLock);
    UNUSEDARG(timeout);

    /* timeout is not supported on this implementation */
    GOAL_ASSERT(0 == timeout);

    GOAL_TARGET_LOCK_INT_DISABLE;

#if GOAL_CONFIG_DEBUG_CONTEXT_LOCKING == 1
    if (g_lockCnt) {
        if (g_lockCtx != gLockContext) {
            goal_targetHalt();
        }
    } else {
        g_lockCtx = gLockContext;
    }
#endif

    g_lockCnt++;

    return GOAL_OK;
}


/****************************************************************************/
/** Release a Lock
 *
 * Leave a binary or counting lock.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_targetLockPut(
    GOAL_LOCK_T *pLock                          /**< lock data */
)
{
    UNUSEDARG(pLock);

#if GOAL_CONFIG_DEBUG_CONTEXT_LOCKING == 1
    if (g_lockCtx != gLockContext) {
        goal_targetHalt();
    }
#endif

    if (g_lockCnt > 0) {
        g_lockCnt--;
    }

    if (g_lockCnt == 0) {
        GOAL_TARGET_LOCK_INT_ENABLE;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Free Lock Resources
 *
 * Cleans up the locks resources.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_targetLockDelete(
    GOAL_LOCK_T *pLock                          /**< lock data */
)
{
    UNUSEDARG(pLock);
    return GOAL_OK;
}
