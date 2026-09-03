/** @file
 *
 * @brief CIP implementation of LLDP
 *
 * This module connects the EIP stack to the LLDP stack and provides the
 * necessary CIP objects.
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

#ifndef CIP_LLDP_H
#define CIP_LLDP_H


/****************************************************************************/
/* Public defines */
/****************************************************************************/
#define EIP_LLDP_ENABLE_DFLT(_portCount) ((1 << (_portCount + 2)) - 1) /**< default values for LLDP Management attribute 1 */
#define EIP_LLDP_TX_INT_DFLT                 30 /**< default values for LLDP Management attribute 2 */
#define EIP_LLDP_TX_HOLD_DFLT                 4 /**< default values for LLDP Management attribute 3 */


/* Attribute values of LLDP Management object */
#define EIP_LLDP_TX_INT_MIN                   1 /**< Minimum value of msgTxInterval */
#define EIP_LLDP_TX_INT_MAX                3600 /**< Maximum value of msgTxInterval */
#define EIP_LLDP_TX_HOLD_MIN                  1 /**< Minimum value of msgTxHold */
#define EIP_LLDP_TX_HOLD_MAX                100 /**< Maximum value of msgTxHold */


/****************************************************************************/
/* Public functions */
/****************************************************************************/
EIP_STATUS eip_lldpInit(
    EIP_INSTANCE_T *pEip                        /**< EtherNet/IP instance */
);

GOAL_STATUS_T eip_lldpCmVarsApply(
    EIP_INSTANCE_T *pEip                        /**< EtherNet/IP instance */
);

void eip_lldpShutdown(
    EIP_INSTANCE_T *pEip                        /**< EtherNet/IP instance */
);

EIP_STATUS eip_lldpDataStoreInit(
    EIP_INSTANCE_T *pEip                        /**< EtherNet/IP instance */
);

void eip_lldpDataStoreUpdate(
    EIP_INSTANCE_T *pEip                        /**< EtherNet/IP instance */
);

void eip_lldpDataStorePortDelete(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    GOAL_ETH_PORT_T port                        /**< GOAL port id */
);

void eip_lldpDataStoreReset(
    EIP_INSTANCE_T *pEip                        /**< EtherNet/IP instance */
);


#endif /* CIP_LLDP_H */
