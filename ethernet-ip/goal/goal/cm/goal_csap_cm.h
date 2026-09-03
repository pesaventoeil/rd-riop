/** @file
 *
 * @brief csap configuration
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
#ifndef GOAL_CSAP_CM_H
#define GOAL_CSAP_CM_H

#include <goal_includes.h>


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_csapRegCmVars(
    void
);


/****************************************************************************/
/* Configuration Variables Management */
/****************************************************************************/

#define GOAL_CM_CSAP_MOD_ID      41

#define GOAL_CM_CSAP_VARS \
/*              Name,              Data type,       Max. size,        Validation Cb, Change Cb */   \
    GOAL_CM_VAR(CSAP_CM_VAR_CONFIG, GOAL_CM_GENERIC, 1024, NULL, NULL), \
    GOAL_CM_VAR(CSAP_CM_VAR_MODE, GOAL_CM_UINT16, 2, NULL, NULL), \
    GOAL_CM_VAR(CSAP_CM_VAR_CONFIG1, GOAL_CM_GENERIC, 1024, NULL, NULL), \
    GOAL_CM_VAR(CSAP_CM_VAR_CONFIG2, GOAL_CM_GENERIC, 1024, NULL, NULL), \
    GOAL_CM_VAR(CSAP_CM_VAR_CONFIG3, GOAL_CM_GENERIC, 1024, NULL, NULL)

enum GOAL_CM_CSAP_VAR_IDS {
    CSAP_CM_VAR_CONFIG = 0,
    CSAP_CM_VAR_MODE = 1,
    CSAP_CM_VAR_CONFIG1 = 2,
    CSAP_CM_VAR_CONFIG2 = 3,
    CSAP_CM_VAR_CONFIG3 = 4,
    GOAL_CM_CSAP_VAR_LAST
};

enum GOAL_CM_CSAP_VIRT_VAR_IDS {
    GOAL_CM_CSAP_VIRT_VAR_LAST
};

#endif /* GOAL_CSAP_CM_H */

