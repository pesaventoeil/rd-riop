/** @file
 *
 * @brief lm configuration
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
#ifndef GOAL_LM_CM_H
#define GOAL_LM_CM_H

#include <goal_includes.h>


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_lmRegCmVars(
    void
);


/****************************************************************************/
/* Configuration Variables Management */
/****************************************************************************/

#define GOAL_CM_LM_MOD_ID      35

#define GOAL_CM_LM_VARS \
/*              Name,              Data type,       Max. size,        Validation Cb, Change Cb */   \
    GOAL_CM_VAR(LM_CM_VAR_VERSION, GOAL_CM_UINT8, 1, NULL, NULL)

enum GOAL_CM_LM_VAR_IDS {
    LM_CM_VAR_VERSION = 0,
    GOAL_CM_LM_VAR_LAST
};

enum GOAL_CM_LM_VIRT_VAR_IDS {
    LM_CM_VAR_READBUFFER = 1000,
    LM_CM_VAR_CNT = 1001,
    LM_CM_VAR_EXLOG_READBUFFER = 1002,
    LM_CM_VAR_EXLOG_CNT = 1003,
    LM_CM_VAR_EXLOG_SIZE = 1004,
    LM_CM_VAR_EXLOG_USAGE = 1005,
    LM_CM_VAR_EXLOG_ERASE = 1006,
    GOAL_CM_LM_VIRT_VAR_LAST
};

#endif /* GOAL_LM_CM_H */

