/** @file
 *
 * @brief eip configuration
 *
 * @copyright
 * Copyright 2010-2022 port GmbH Halle/Saale.
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
#ifndef GOAL_EIP_CM_H
#define GOAL_EIP_CM_H

#include <goal_includes.h>


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_eipRegCmVars(
    void
);


/****************************************************************************/
/* Configuration Variables Management */
/****************************************************************************/

#define GOAL_CM_EIP_MOD_ID      23

#define GOAL_CM_EIP_VARS(instId) \
/*              Name,              Data type,       Max. size,        Validation Cb, Change Cb */   \
    GOAL_CM_INST_VAR(EIP_CM_VAR_INCARNATIONID, instId, GOAL_CM_EIP_VAR_LAST, GOAL_CM_UINT32, 4, NULL, NULL), \
    GOAL_CM_INST_VAR(EIP_CM_VAR_DOMAIN, instId, GOAL_CM_EIP_VAR_LAST, GOAL_CM_GENERIC, EIP_CM_DOMAIN_LEN, NULL, NULL), \
    GOAL_CM_INST_VAR(EIP_CM_VAR_HOST, instId, GOAL_CM_EIP_VAR_LAST, GOAL_CM_GENERIC, EIP_CM_HOST_LEN, NULL, NULL), \
    GOAL_CM_INST_VAR(EIP_CM_VAR_ENCAPTIMEOUT, instId, GOAL_CM_EIP_VAR_LAST, GOAL_CM_UINT16, 2, NULL, NULL), \
    GOAL_CM_INST_VAR(EIP_CM_VAR_PORTCFG, instId, GOAL_CM_EIP_VAR_LAST, GOAL_CM_GENERIC, sizeof(EIP_PORT_CFG_T), NULL, NULL), \
    GOAL_CM_INST_VAR(EIP_CM_VAR_QOS_VLAN, instId, GOAL_CM_EIP_VAR_LAST, GOAL_CM_UINT8, 1, NULL, NULL), \
    GOAL_CM_INST_VAR(EIP_CM_VAR_QOS_URGENT, instId, GOAL_CM_EIP_VAR_LAST, GOAL_CM_UINT8, 1, NULL, NULL), \
    GOAL_CM_INST_VAR(EIP_CM_VAR_QOS_SCHEDULED, instId, GOAL_CM_EIP_VAR_LAST, GOAL_CM_UINT8, 1, NULL, NULL), \
    GOAL_CM_INST_VAR(EIP_CM_VAR_QOS_HIGH, instId, GOAL_CM_EIP_VAR_LAST, GOAL_CM_UINT8, 1, NULL, NULL), \
    GOAL_CM_INST_VAR(EIP_CM_VAR_QOS_LOW, instId, GOAL_CM_EIP_VAR_LAST, GOAL_CM_UINT8, 1, NULL, NULL), \
    GOAL_CM_INST_VAR(EIP_CM_VAR_QOS_EXPLICIT, instId, GOAL_CM_EIP_VAR_LAST, GOAL_CM_UINT8, 1, NULL, NULL), \
    GOAL_CM_INST_VAR(EIP_CM_VAR_TTL, instId, GOAL_CM_EIP_VAR_LAST, GOAL_CM_UINT8, 1, NULL, NULL), \
    GOAL_CM_INST_VAR(EIP_CM_VAR_MC_CTRL, instId, GOAL_CM_EIP_VAR_LAST, GOAL_CM_UINT8, 1, NULL, NULL), \
    GOAL_CM_INST_VAR(EIP_CM_VAR_MC_NUM, instId, GOAL_CM_EIP_VAR_LAST, GOAL_CM_UINT16, 2, NULL, NULL), \
    GOAL_CM_INST_VAR(EIP_CM_VAR_MC_START, instId, GOAL_CM_EIP_VAR_LAST, GOAL_CM_UINT32, 4, NULL, NULL), \
    GOAL_CM_INST_VAR(EIP_CM_VAR_ACD_ENABLED, instId, GOAL_CM_EIP_VAR_LAST, GOAL_CM_UINT8, 1, NULL, NULL), \
    GOAL_CM_INST_VAR(EIP_CM_VAR_ACD_LAST_CONFLICT, instId, GOAL_CM_EIP_VAR_LAST, GOAL_CM_GENERIC, sizeof(S_CIP_ACDLastConflictDetected), NULL, NULL), \
    GOAL_CM_INST_VAR(EIP_CM_VAR_LLDP_ENABLE, instId, GOAL_CM_EIP_VAR_LAST, GOAL_CM_UINT32, 4, NULL, NULL), \
    GOAL_CM_INST_VAR(EIP_CM_VAR_LLDP_TX_INT, instId, GOAL_CM_EIP_VAR_LAST, GOAL_CM_UINT16, 2, NULL, NULL), \
    GOAL_CM_INST_VAR(EIP_CM_VAR_LLDP_TX_HOLD, instId, GOAL_CM_EIP_VAR_LAST, GOAL_CM_UINT8, 1, NULL, NULL)

enum GOAL_CM_EIP_VAR_IDS {
    EIP_CM_VAR_INCARNATIONID = 0,
    EIP_CM_VAR_DOMAIN = 1,
    EIP_CM_VAR_HOST = 2,
    EIP_CM_VAR_ENCAPTIMEOUT = 3,
    EIP_CM_VAR_PORTCFG = 4,
    EIP_CM_VAR_QOS_VLAN = 5,
    EIP_CM_VAR_QOS_URGENT = 6,
    EIP_CM_VAR_QOS_SCHEDULED = 7,
    EIP_CM_VAR_QOS_HIGH = 8,
    EIP_CM_VAR_QOS_LOW = 9,
    EIP_CM_VAR_QOS_EXPLICIT = 10,
    EIP_CM_VAR_TTL = 11,
    EIP_CM_VAR_MC_CTRL = 12,
    EIP_CM_VAR_MC_NUM = 13,
    EIP_CM_VAR_MC_START = 14,
    EIP_CM_VAR_ACD_ENABLED = 15,
    EIP_CM_VAR_ACD_LAST_CONFLICT = 16,
    EIP_CM_VAR_LLDP_ENABLE = 17,
    EIP_CM_VAR_LLDP_TX_INT = 18,
    EIP_CM_VAR_LLDP_TX_HOLD = 19,
    GOAL_CM_EIP_VAR_LAST
};

enum GOAL_CM_EIP_VIRT_VAR_IDS {
    GOAL_CM_EIP_VIRT_VAR_LAST
};

#endif /* GOAL_EIP_CM_H */

