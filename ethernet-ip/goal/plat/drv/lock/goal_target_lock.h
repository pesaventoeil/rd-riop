/** @file
 *
 * @brief
 * GOAL Generic Locking Implementation
 *
 * @details
 * This module implements locking mechanisms via interrupt disabling and
 * enabling. The target implementation must provide the two macros
 * CONFIG_TARGET_LOCK_INT_ENABLE and CONFIG_TARGET_LOCK_INT_DISABLE.
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
#ifndef GOAL_TARGETLOCK_H
#define GOAL_TARGETLOCK_H


/****************************************************************************/
/* Enums */
/****************************************************************************/
#if GOAL_CONFIG_DEBUG_CONTEXT_LOCKING == 1
/**< GOAL context definition */
typedef enum {
    GOAL_LOCK_CTX_NORMAL = 0,                   /**< normal/user context */
    GOAL_LOCK_CTX_TIMER = 1,                    /**< IRQ: timer context */
    GOAL_LOCK_CTX_ETHERNET = 2,                 /**< IRQ: ethernet context */
} GOAL_LOCK_CONTEXT_T;
#endif


/****************************************************************************/
/* Exported variables */
/****************************************************************************/
#if GOAL_CONFIG_DEBUG_CONTEXT_LOCKING == 1
extern volatile GOAL_LOCK_CONTEXT_T gLockContext; /**< global setable lock context */
#endif


/****************************************************************************/
/* Macros */
/****************************************************************************/
#if GOAL_CONFIG_DEBUG_CONTEXT_LOCKING == 1
#  define GOAL_LOCK_CTX_ENTER(ctx) \
        GOAL_LOCK_CONTEXT_T lockContext; \
        lockContext = gLockContext; \
        gLockContext = ctx
#  define GOAL_LOCK_CTX_LEAVE() \
        gLockContext = lockContext
#else
#  define GOAL_LOCK_CTX_ENTER(ctx)
#  define GOAL_LOCK_CTX_LEAVE()
#endif


#endif /* GOAL_TARGETLOCK_H */
