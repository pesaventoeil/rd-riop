/** @file
 *
 * @brief GOAL Linux Low Level Interface
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

#ifndef GOAL_TARGET_FREERTOS_H
#define GOAL_TARGET_FREERTOS_H
#include <FreeRTOS.h>


/****************************************************************************/
/* Public Data types */
/****************************************************************************/
/** custom stack mapping table */
typedef struct {
    const char *strName;                        /**< task name */
    StaticTask_t taskHandle;                    /**< target task handle */
    uint32_t stackSize;                         /**< size of stack in bytes */
    uint8_t *pStack;                            /**< task stack */
} GOAL_TGT_TASK_MAP_T;


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_targetMainPreOsCb(
    void
);

GOAL_STATUS_T goal_tgtTaskMapSet(
    GOAL_TGT_TASK_MAP_T *pTaskMap,              /**< task map data */
    unsigned int cntTaskMap                     /**< task map data count */
);


#endif /* GOAL_TARGET_FREERTOS_H */
