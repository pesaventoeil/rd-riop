/** @file
 *
 * @brief cm configuration
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
#ifndef GOAL_CM_CM_H
#define GOAL_CM_CM_H

#include <goal_includes.h>


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_cmRegCmVars(
    void
);


/****************************************************************************/
/* Configuration Variables Management */
/****************************************************************************/

#define GOAL_CM_CM_MOD_ID      2

#define GOAL_CM_CM_VARS \
/*              Name,              Data type,       Max. size,        Validation Cb, Change Cb */   \
    GOAL_CM_VAR(CM_CM_VAR_VERSION, GOAL_CM_UINT32, 4, NULL, NULL)

enum GOAL_CM_CM_VAR_IDS {
    CM_CM_VAR_VERSION = 0,
    GOAL_CM_CM_VAR_LAST
};

enum GOAL_CM_CM_VIRT_VAR_IDS {
    CM_CM_VAR_SAVE = 1000,
    GOAL_CM_CM_VIRT_VAR_LAST
};

#endif /* GOAL_CM_CM_H */

