/** @file
 *
 * @brief GOAL - target specific header file
 *
 * This header contains data type definitions which are used by GOAL and
 * external OS-dependend modules.
 *
 * @copyright
 * Copyright 2010-2023
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
#ifndef GOAL_TARGET_COMMON_H
#define GOAL_TARGET_COMMON_H

#include "goal_includes.h"
#include "fsl_common.h"


/****************************************************************************/
/* Defines */
/****************************************************************************/
#ifndef GOAL_CONFIG_TARGET_EXECUTE_TIMER_IN_TICK_INTERRUPT
#   define GOAL_CONFIG_TARGET_EXECUTE_TIMER_IN_TICK_INTERRUPT 0
#endif


/****************************************************************************/
/* Inline functions */
/****************************************************************************/
static inline GOAL_STATUS_T goal_targetResToGoal(
    int32_t err                                 /**<  error value */
)
{
    switch (err) {
        case kStatus_Success:
            return GOAL_OK;
        case kStatus_InvalidArgument:
            return GOAL_ERR_UNSUPPORTED;
        case kStatus_Busy:
            return GOAL_ERR_BUSY;
        default:
            return GOAL_ERROR;
    }
}


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_targetCommonInitPre(
    void
);

GOAL_BOOL_T goal_targetCommonIsrCheck(
    void
);
#endif /* GOAL_TARGET_COMMON_H */
