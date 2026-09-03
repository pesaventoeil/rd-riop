/** @file
 *
 * @brief dd configuration
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
#ifndef GOAL_DD_CM_H
#define GOAL_DD_CM_H

#include <goal_includes.h>


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_ddRegCmVars(
    void
);


/****************************************************************************/
/* Configuration Variables Management */
/****************************************************************************/

#define GOAL_CM_DD_MOD_ID      34

#if 1 == GOAL_CONFIG_DD_CRYPT
#  define GOAL_CM_DD_VARS \
    /*          Name,                 Data type,      Max. size, Validation Cb, Change Cb */ \
    GOAL_CM_VAR(DD_CM_VAR_MODULENAME, GOAL_CM_STRING, 20, NULL, NULL), \
    GOAL_CM_VAR(DD_CM_VAR_CUSTOMERID, GOAL_CM_UINT32, 4, NULL, NULL), \
    GOAL_CM_VAR(DD_CM_VAR_RESERVED, GOAL_CM_UINT8, 1, NULL, NULL), \
    GOAL_CM_VAR(DD_CM_VAR_FEATURE_DISABLE, GOAL_CM_UINT32, 4, NULL, NULL), \
    GOAL_CM_VAR(DD_CM_VAR_CRYPT_ACTIVATED, GOAL_CM_UINT8, 1, goal_ddCryptActivatedValidateCb, NULL), \
    GOAL_CM_VAR(DD_CM_VAR_CRYPT_PASSWORD, GOAL_CM_STRING, 32, NULL, NULL)
#else /* 1 == GOAL_CONFIG_DD_CRYPT */
#  define GOAL_CM_DD_VARS \
    /*          Name,                 Data type,      Max. size, Validation Cb, Change Cb */ \
    GOAL_CM_VAR(DD_CM_VAR_MODULENAME, GOAL_CM_STRING, 20, NULL, NULL), \
    GOAL_CM_VAR(DD_CM_VAR_CUSTOMERID, GOAL_CM_UINT32, 4, NULL, NULL), \
    GOAL_CM_VAR(DD_CM_VAR_RESERVED, GOAL_CM_UINT8, 1, NULL, NULL), \
    GOAL_CM_VAR(DD_CM_VAR_FEATURE_DISABLE, GOAL_CM_UINT32, 4, NULL, NULL)
#endif /* 1 == GOAL_CONFIG_DD_CRYPT */

enum GOAL_CM_DD_VAR_IDS {
    DD_CM_VAR_MODULENAME = 0,
    DD_CM_VAR_CUSTOMERID = 1,
    DD_CM_VAR_RESERVED = 2,
    DD_CM_VAR_FEATURE_DISABLE = 3,
#if 1 == GOAL_CONFIG_DD_CRYPT
    DD_CM_VAR_CRYPT_ACTIVATED = 4,
    DD_CM_VAR_CRYPT_PASSWORD = 5,
#endif
    GOAL_CM_DD_VAR_LAST
};

enum GOAL_CM_DD_VIRT_VAR_IDS {
    GOAL_CM_DD_VIRT_VAR_LAST
};

#endif /* GOAL_DD_CM_H */

