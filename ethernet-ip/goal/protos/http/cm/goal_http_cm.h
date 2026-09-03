/** @file
 *
 * @brief http configuration
 *
 * @copyright
 * Copyright 2020 port GmbH Halle/Saale.
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
#ifndef GOAL_HTTP_CM_H
#define GOAL_HTTP_CM_H

#include <goal_includes.h>


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_httpRegCmVars(
    void
);


/****************************************************************************/
/* Configuration Variables Management */
/****************************************************************************/

#define GOAL_CM_HTTP_MOD_ID      25

#define GOAL_CM_HTTP_VARS \
/*              Name,              Data type,       Max. size,        Validation Cb, Change Cb */   \
    GOAL_CM_VAR(HTTP_CM_VAR_HTTP_CHANNELS_MAX, GOAL_CM_UINT16, 2, NULL, NULL), \
    GOAL_CM_VAR(HTTP_CM_VAR_HTTPS_CHANNELS_MAX, GOAL_CM_UINT16, 2, NULL, NULL), \
    GOAL_CM_VAR(HTTP_CM_VAR_USERLEVEL0, GOAL_CM_STRING, 32, NULL, NULL), \
    GOAL_CM_VAR(HTTP_CM_VAR_USERLEVEL1, GOAL_CM_STRING, 32, NULL, NULL), \
    GOAL_CM_VAR(HTTP_CM_VAR_USERLEVEL2, GOAL_CM_STRING, 32, NULL, NULL), \
    GOAL_CM_VAR(HTTP_CM_VAR_USERLEVEL3, GOAL_CM_STRING, 32, NULL, NULL)

enum GOAL_CM_HTTP_VAR_IDS {
    HTTP_CM_VAR_HTTP_CHANNELS_MAX = 0,
    HTTP_CM_VAR_HTTPS_CHANNELS_MAX = 1,
    HTTP_CM_VAR_USERLEVEL0 = 2,
    HTTP_CM_VAR_USERLEVEL1 = 3,
    HTTP_CM_VAR_USERLEVEL2 = 4,
    HTTP_CM_VAR_USERLEVEL3 = 5,
    GOAL_CM_HTTP_VAR_LAST
};

enum GOAL_CM_HTTP_VIRT_VAR_IDS {
    GOAL_CM_HTTP_VIRT_VAR_LAST
};

#endif /* GOAL_HTTP_CM_H */

