/** @file
 *
 * @brief net configuration
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
#ifndef GOAL_NET_CM_H
#define GOAL_NET_CM_H

#include <goal_includes.h>


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_netRegCmVars(
    void
);


/****************************************************************************/
/* Configuration Variables Management */
/****************************************************************************/

#define GOAL_CM_NET_MOD_ID      12

#define GOAL_CM_NET_VARS(instId) \
/*              Name,              Data type,       Max. size,        Validation Cb, Change Cb */   \
    GOAL_CM_INST_VAR(NET_CM_VAR_IP, instId, GOAL_CM_NET_VAR_LAST, GOAL_CM_IPV4, 4, NULL, NULL), \
    GOAL_CM_INST_VAR(NET_CM_VAR_NETMASK, instId, GOAL_CM_NET_VAR_LAST, GOAL_CM_IPV4, 4, NULL, NULL), \
    GOAL_CM_INST_VAR(NET_CM_VAR_GW, instId, GOAL_CM_NET_VAR_LAST, GOAL_CM_IPV4, 4, NULL, NULL), \
    GOAL_CM_INST_VAR(NET_CM_VAR_VALID, instId, GOAL_CM_NET_VAR_LAST, GOAL_CM_UINT8, 1, NULL, NULL), \
    GOAL_CM_INST_VAR(NET_CM_VAR_DHCP_ENABLED, instId, GOAL_CM_NET_VAR_LAST, GOAL_CM_UINT8, 1, NULL, NULL), \
    GOAL_CM_INST_VAR(NET_CM_VAR_DHCP_STATE, instId, GOAL_CM_NET_VAR_LAST, GOAL_CM_UINT8, 1, NULL, NULL), \
    GOAL_CM_INST_VAR(NET_CM_VAR_DNS0, instId, GOAL_CM_NET_VAR_LAST, GOAL_CM_IPV4, 4, NULL, NULL), \
    GOAL_CM_INST_VAR(NET_CM_VAR_DNS1, instId, GOAL_CM_NET_VAR_LAST, GOAL_CM_IPV4, 4, NULL, NULL), \
    GOAL_CM_INST_VAR(NET_CM_VAR_HOSTNAME, instId, GOAL_CM_NET_VAR_LAST, GOAL_CM_STRING, 20, NULL, NULL)

enum GOAL_CM_NET_VAR_IDS {
    NET_CM_VAR_IP = 0,
    NET_CM_VAR_NETMASK = 1,
    NET_CM_VAR_GW = 2,
    NET_CM_VAR_VALID = 3,
    NET_CM_VAR_DHCP_ENABLED = 4,
    NET_CM_VAR_DHCP_STATE = 5,
    NET_CM_VAR_DNS0 = 6,
    NET_CM_VAR_DNS1 = 7,
    NET_CM_VAR_HOSTNAME = 8,
    GOAL_CM_NET_VAR_LAST
};

enum GOAL_CM_NET_VIRT_VAR_IDS {
    NET_CM_VAR_COMMIT = 1000,
    GOAL_CM_NET_VIRT_VAR_LAST
};

#endif /* GOAL_NET_CM_H */

