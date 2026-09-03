/** @file
 *
 * @brief Low Level Interface of GOAL
 *
 * This module binds GOAL to a specific hardware or OS. It provides a generic
 * API for GOAL.
 *
 * @copyright
 * Copyright 2010-2023 port GmbH Halle/Saale.
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

#ifndef GOAL_TARGET_H
#define GOAL_TARGET_H

#include <goal_includes.h>
#include <goal_target_board.h>
#include <fsl_device_registers.h>


/****************************************************************************/
/* Constants */
/****************************************************************************/
/* NVS definitions */
#define GOAL_NVS_SIZE           0

/* Ethernet queue sizes */
#define GOAL_ETH_RX_LOW_QUEUE_NUM    4
#define GOAL_ETH_TX_LOW_QUEUE_NUM    4
#define GOAL_ETH_TX_HIGH_QUEUE_NUM   2

/* Recursive Mutex Support
 *
 * Allows mutexes to be locked multiple times by the same calling thread. To
 * use this feature, the target implementation will get access to the lock
 * elements 'owner' and 'ownerCnt'. Owner should be used to store and compare
 * the calling thread ID and ownerCnt should reflect the count of lock calls
 * for the first-calling thread.
 */
#define GOAL_TARGET_RECURSIVE_MUTEX_SUPPORT 1

/* Lock Handling Defines
 *
 * CONFIG_TARGET_LOCK_INT_ENABLE - command to enable interrupts
 * CONFIG_TARGET_LOCK_INT_DISABLE - command to disable interrupts
 */
#define GOAL_TARGET_LOCK_INT_ENABLE __enable_irq()
#define GOAL_TARGET_LOCK_INT_DISABLE __disable_irq()

/* Instruction Cacha handling defines
 *
 * Dummy definitions - not available on Cortex M4
 *
 * GOAL_TARGET_ICACHE_ENABLE - enable instruction cache
 * GOAL_TARGET_ICACHE_DISABLE - invalidate and disable instruction cache
 */
#define GOAL_TARGET_ICACHE_ENABLE
#define GOAL_TARGET_ICACHE_DISABLE

#ifndef PRIu64
# define PRIu64 "llu"
#endif

#ifndef PRIx64
# define PRIx64 "llx"
#endif

#ifndef PRIi64
# define PRIi64 "lli"
#endif


void SysTick_Handler(
    void
);

#endif /* GOAL_TARGET_H */
