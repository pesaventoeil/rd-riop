/** @file
 *
 * @brief https configuration
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
#ifndef GOAL_HTTPS_CM_H
#define GOAL_HTTPS_CM_H

#include <goal_includes.h>


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_httpsRegCmVars(
    void
);


/****************************************************************************/
/* Configuration Variables Management */
/****************************************************************************/

#define GOAL_CM_HTTPS_MOD_ID      93

#define GOAL_CM_HTTPS_VARS \
/*              Name,              Data type,       Max. size,        Validation Cb, Change Cb */   \
    GOAL_CM_VAR(HTTPS_CM_VAR_TLS_SERVER_CERTIFICATE, GOAL_CM_GENERIC, 1024, NULL, NULL), \
    GOAL_CM_VAR(HTTPS_CM_VAR_TLS_PRIVATE_KEY, GOAL_CM_GENERIC, 1024, NULL, NULL), \
    GOAL_CM_VAR(HTTPS_CM_VAR_TLS_SRV_CERT_CA_CN, GOAL_CM_STRING, 128, NULL, NULL), \
    GOAL_CM_VAR(HTTPS_CM_VAR_TLS_SRV_CERT_CA_O, GOAL_CM_STRING, 128, NULL, NULL), \
    GOAL_CM_VAR(HTTPS_CM_VAR_TLS_SRV_CERT_CA_C, GOAL_CM_STRING, 8, NULL, NULL), \
    GOAL_CM_VAR(HTTPS_CM_VAR_TLS_SRV_CERT_CN, GOAL_CM_STRING, 128, NULL, NULL), \
    GOAL_CM_VAR(HTTPS_CM_VAR_TLS_SRV_CERT_O, GOAL_CM_STRING, 128, NULL, NULL), \
    GOAL_CM_VAR(HTTPS_CM_VAR_TLS_SRV_CERT_C, GOAL_CM_STRING, 8, NULL, NULL), \
    GOAL_CM_VAR(HTTPS_CM_VAR_TLS_SRV_CERT_NOT_BEFORE, GOAL_CM_STRING, 20, NULL, NULL), \
    GOAL_CM_VAR(HTTPS_CM_VAR_TLS_SRV_CERT_NOT_AFTER, GOAL_CM_STRING, 20, NULL, NULL)

enum GOAL_CM_HTTPS_VAR_IDS {
    HTTPS_CM_VAR_TLS_SERVER_CERTIFICATE = 0,
    HTTPS_CM_VAR_TLS_PRIVATE_KEY = 1,
    HTTPS_CM_VAR_TLS_SRV_CERT_CA_CN = 2,
    HTTPS_CM_VAR_TLS_SRV_CERT_CA_O = 3,
    HTTPS_CM_VAR_TLS_SRV_CERT_CA_C = 4,
    HTTPS_CM_VAR_TLS_SRV_CERT_CN = 5,
    HTTPS_CM_VAR_TLS_SRV_CERT_O = 6,
    HTTPS_CM_VAR_TLS_SRV_CERT_C = 7,
    HTTPS_CM_VAR_TLS_SRV_CERT_NOT_BEFORE = 8,
    HTTPS_CM_VAR_TLS_SRV_CERT_NOT_AFTER = 9,
    GOAL_CM_HTTPS_VAR_LAST
};

enum GOAL_CM_HTTPS_VIRT_VAR_IDS {
    GOAL_CM_HTTPS_VIRT_VAR_LAST
};

#endif /* GOAL_HTTPS_CM_H */

