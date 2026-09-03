/** @file
 *
 * @brief eth configuration
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
#ifndef GOAL_ETH_CM_H
#define GOAL_ETH_CM_H

#include <goal_includes.h>


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_ethRegCmVars(
    void
);


/****************************************************************************/
/* Configuration Variables Management */
/****************************************************************************/

#define GOAL_CM_ETH_MOD_ID      4

#define GOAL_CM_ETH_VARS(instId) \
/*              Name,              Data type,       Max. size,        Validation Cb, Change Cb */   \
    GOAL_CM_INST_VAR(ETH_CM_VAR_MAC, instId, GOAL_CM_ETH_VAR_LAST, GOAL_CM_GENERIC, 6, NULL, NULL)

enum GOAL_CM_ETH_VAR_IDS {
    ETH_CM_VAR_MAC = 0,
    GOAL_CM_ETH_VAR_LAST
};

enum GOAL_CM_ETH_VIRT_VAR_IDS {
    ETH_CM_VAR_LINK = 1000,
    ETH_CM_VAR_SPEED = 1001,
    ETH_CM_VAR_DUPLEX = 1002,
    ETH_CM_VAR_PORTCNT = 1003,
    GOAL_CM_ETH_VIRT_VAR_LAST
};

#endif /* GOAL_ETH_CM_H */

