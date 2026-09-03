/** @file
 *
 * @brief mi_mctc configuration
 *
 * @copyright
 * Copyright 2021 port GmbH Halle/Saale.
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
#ifndef GOAL_MI_MCTC_CM_H
#define GOAL_MI_MCTC_CM_H

#include <goal_includes.h>


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_mi_mctcRegCmVars(
    void
);


/****************************************************************************/
/* Configuration Variables Management */
/****************************************************************************/

#define GOAL_CM_MI_MCTC_MOD_ID      94

#define GOAL_CM_MI_MCTC_VARS \
/*              Name,              Data type,       Max. size,        Validation Cb, Change Cb */   \
    GOAL_CM_VAR(MI_MCTC_CM_VAR_RPC_DISABLE, GOAL_CM_UINT8, 1, NULL, NULL)

enum GOAL_CM_MI_MCTC_VAR_IDS {
    MI_MCTC_CM_VAR_RPC_DISABLE = 0,
    GOAL_CM_MI_MCTC_VAR_LAST
};

enum GOAL_CM_MI_MCTC_VIRT_VAR_IDS {
    MI_MCTC_CM_VAR_ID = 1000,
    MI_MCTC_CM_VAR_STAT_RESET = 1001,
    MI_MCTC_CM_VAR_STAT_RPC_TIMEOUTS = 1002,
    MI_MCTC_CM_VAR_STAT_RPC_DELAY_MIN = 1003,
    MI_MCTC_CM_VAR_STAT_RPC_DELAY_MAX = 1004,
    MI_MCTC_CM_VAR_STAT_RPC_DELAY_MEAN = 1005,
    MI_MCTC_CM_VAR_STAT_RPC_COUNT = 1006,
    MI_MCTC_CM_VAR_STAT_FRAME_VALID_COUNT = 1007,
    MI_MCTC_CM_VAR_STAT_FRAME_INVALID_COUNT = 1008,
    GOAL_CM_MI_MCTC_VIRT_VAR_LAST
};

#endif /* GOAL_MI_MCTC_CM_H */

