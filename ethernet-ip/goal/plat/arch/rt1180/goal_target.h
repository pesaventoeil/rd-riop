/** @file
 *
 * @brief Low Level Interface of GOAL
 *
 * This module binds GOAL to a specific hardware or OS. It provides a generic
 * API for GOAL.
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

#ifndef GOAL_TARGET_H
#define GOAL_TARGET_H

#include <goal_includes.h>
#include <goal_target_board.h>


/****************************************************************************/
/* Public constant definitions */
/****************************************************************************/
#define GOAL_TARGET_LOCK_INT_ENABLE __enable_irq()


/****************************************************************************/
/* function definitions */
/****************************************************************************/
#define goal_tgtSystickHandler timer_callback


/****************************************************************************/
/* Defines */
/****************************************************************************/
/* AC needs less heap due to lack of industrial protocols */
#ifndef GOAL_CONFIG_HEAP_SIZE
# if GOAL_CONFIG_MCTC_AC == 1
#  define GOAL_CONFIG_HEAP_SIZE (120 * 1024)    /**< target HEAP size */
# else
#  define GOAL_CONFIG_HEAP_SIZE (200 * 1024)    /**< target HEAP size */
# endif
#endif

#ifndef GOAL_LM_BUFFER_SIZE
#  define GOAL_LM_BUFFER_SIZE (15 * 1024)       /**< target log buffer size */
#endif

#endif /* GOAL_TARGET_H */
