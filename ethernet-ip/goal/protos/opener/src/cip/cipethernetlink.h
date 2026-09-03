/** @file
 *
 * @brief Ethernet Link class
 *
 * This module implements the Ethernet Link class.
 *
 * @copyright
 * This software consists of the OpENer by Rockwell Automation, Inc. and
 * extensive fixes, changes and improvements by port GmbH.
 *
 * Copyright (c) 2019, port GmbH for all fixes, changes and improvements in
 * this software, added to the original, unchanged OpENer
 *
 * Copyright (c) 2009, Rockwell Automation, Inc. for the original, unchanged
 * OpENer source code
 *
 * All rights reserved.
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

#ifndef CIPETHERNETLINK_H
#define CIPETHERNETLINK_H


/****************************************************************************/
/* Public defines */
/****************************************************************************/
#define CIP_ETHERNETLINK_CLASS_CODE        0xF6 /**< CIP class ID for Ethernet Link objects */

/* Ethernet Link Instance Attribute 2: Interface Flags */
#define CIP_ETHERNETLINK_FLAG_LINKUP       0x01 /* Link is up */
#define CIP_ETHERNETLINK_FLAG_LINKDOWN     0x00 /* Link is down */
#define CIP_ETHERNETLINK_FLAG_HD           0x00 /* Half Duplex */
#define CIP_ETHERNETLINK_FLAG_FD           0x02 /* Full Duplex */
#define CIP_ETHERNETLINK_FLAG_NEG_PROG     0x00 /* Autonegotiation in progress */
#define CIP_ETHERNETLINK_FLAG_NEG_FAIL     0x04 /* Autonegotiation failed */
#define CIP_ETHERNETLINK_FLAG_NEG_ND       0x08 /* Autonegotiation failed, no Duplex */
#define CIP_ETHERNETLINK_FLAG_NEG_OK       0x0C /* Autonegotiation successfull */
#define CIP_ETHERNETLINK_FLAG_NEG_OFF      0x10 /* Autonegotiation off */
#define CIP_ETHERNETLINK_FLAG_RESET        0x20 /* changes need resetting of device */
#define CIP_ETHERNETLINK_FLAG_FAULT        0x40 /* Hardware Fault detected */

/* Attribute 6: Interface Control, control bits */
#define CIP_ETH_CTRL_AUTONEG_ENBL        0x0001 /* Autonegotiation enabled */
#define CIP_ETH_CTRL_DUPLEX_FULL         0x0002 /* force full duplex mode */

/* Attribute 9: Admin State */
#define CIP_ETH_ADMINSTATE_ENABLE          0x01 /* port enabled */
#define CIP_ETH_ADMINSTATE_DISABLE         0x02 /* port disabled */

/* Attribute  10: Interface Label */
#define CIP_ETH_LABEL_SIZE                   65 /* size of lable buffer */
#define CIP_ETH_LABEL_TAMPLATE       "Port %hu" /* port label template */

/* Attribute 11: Interface Capability */
#define CIP_ETH_CAPABILITY_RESET_FLAG    (1<<0) /* Setting requires reset */
#define CIP_ETH_CAPABILITY_AUTONEG       (1<<1) /* Auto Negotiation supported */
#define CIP_ETH_CAPABILITY_AUTOMDIX      (1<<2) /* Auto-MDIX supported */
#define CIP_ETH_CAPABILITY_MAN_SETTING   (1<<3) /* manual setting of speed & duplex */


/****************************************************************************/
/* Public functions */
/****************************************************************************/
EIP_STATUS eip_ethInit(
    EIP_INSTANCE_T *pEip                        /**< EtherNet/IP instance */
);

void eip_ethShutdown(
    EIP_INSTANCE_T *pEip                        /**< EtherNet/IP instance */
);

EIP_STATUS eip_ethConfigApply(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_UINT16 instance                         /**< Ethernet Link instance */
);

#endif /* CIPETHERNETLINK_H */
