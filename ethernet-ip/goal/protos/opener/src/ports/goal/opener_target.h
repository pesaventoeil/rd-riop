/** @file
 *
 * @brief OpENer adaptation to GOAL
 *
 * This module connects the OpENer EtherNet/IP stack to GOAL.
 *
 * @copyright
 * Copyright 2010-2019 port GmbH Halle/Saale.
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

#ifndef OPENER_TARGET_H
#define OPENER_TARGET_H

#include "goal_config.h"
#include "goal_includes.h"
#include "opener_includes.h"

#include "cm/goal_eip_cm.h"


/****************************************************************************/
/* External variables */
/****************************************************************************/
extern EIP_CFG_T eipCfg;                        /**< default configuration of EtherNet/IP */


/****************************************************************************/
/* Public Functions */
/****************************************************************************/
GOAL_STATUS_T opener_init(
    void
);

GOAL_STATUS_T opener_new(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    uint32_t ethMiId,                           /**< ID of assigned ETH MI */
    uint32_t netMiId                            /**< ID of assigned NET MI */
);

GOAL_STATUS_T opener_shutdown(
    void
);

EIP_STATUS opener_NWHsend(
    EIP_SOCKT_T sock,                           /**< socket */
    GOAL_BUFFER_T *pTxBuf,                      /**< tx buffer */
    S_CIP_Net_Addr *pAddr                       /**< remote address */
);

void opener_NWHshutdown(
    EIP_INSTANCE_T *pEip                        /**< EtherNet/IP instance */
);

void opener_reset(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EResetState pa_enResetState                 /**< type of reset */
);

GOAL_STATUS_T opener_acdLastConflictDetectedReset(
    EIP_INSTANCE_T *pEip                        /**< EtherNet/IP instance */
);

GOAL_STATUS_T opener_acdLastConflictDetectedGet(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    GOAL_ACD_LAST_CONFLICT_DETECTED_T *pAttribute /**< ACD attribute */
);

GOAL_STATUS_T eip_coreCmVarValueSet(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    uint32_t modId,                             /**< Module ID of the var */
    uint32_t varId,                             /**< Variable ID of the var */
    void *pData,                                /**< Pointer to the value to set */
    uint32_t size,                              /**< Size of the data to set */
    GOAL_BOOL_T temp,                           /**< If true, the value is marked as temporary */
    goal_cm_changed exclude                     /**< If not NULL, the given changed callback is not invoked */
);

GOAL_STATUS_T eip_coreCmVarValueGet(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    uint32_t modId,                             /**< Module ID of the var */
    uint32_t varId,                             /**< Variable ID of the var */
    void *pData,                                /**< [out] variable buffer */
    uint32_t size                               /**< Size of the data to read */
);

#endif /* OPENER_TARGET_H */
