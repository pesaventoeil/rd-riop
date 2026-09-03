/** @file
 *
 * @brief TCP/IP Interface class
 *
 * This module implements the TCP/IP Interface class.
 *
 * @copyright
 * This software consists of the OpENer by Rockwell Automation, Inc. and
 * extensive fixes, changes and improvements by port GmbH.
 *
 * Copyright (c) 2021, port GmbH for all fixes, changes and improvements in
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

#ifndef CIPTCPIPINTERFACE_H
#define CIPTCPIPINTERFACE_H


/****************************************************************************/
/* TCP/IP Attribute ID */
/****************************************************************************/
#define CIP_TCPIP_STATUS                1
#define CIP_TCPIP_CONFIG_CAPA           2
#define CIP_TCPIP_CONFIG_CNTRL          3
#define CIP_TCPIP_PHY_LINK_OBJECT       4
#define CIP_TCPIP_IFACE_CONFIG          5
#define CIP_TCPIP_HOST_NAME             6
#define CIP_TCPIP_TTL_VALUE             8
#define CIP_TCPIP_MC_CFG                9
#define CIP_TCPIP_SELECT_ACD            10
#define CIP_TCPIP_LAST_CONFL_DETECTED   11
#define CIP_TCPIP_ENCAPTIMEOUT          13


/****************************************************************************/
/* Defines */
/****************************************************************************/
#define CIP_TCPIPINTERFACE_CLASS_CODE 0xF5

#define CIP_TCPIP_DOMAINNAME_LEN 48
#define CIP_TCPIP_HOSTNAME_LEN 64

#define CIP_TCPIP_ENCAPTIMEOUT_DEFAULT 120

/* instance attribute #1: Status */
#define CIP_TCPIP_STATUS_VALIDCONFIG_SW (1u<<0)
#define CIP_TCPIP_STATUS_VALIDCONFIG_HW (1u<<1)
#define CIP_TCPIP_STATUS_MCAST_PENDING  (1u<<4)
#define CIP_TCPIP_STATUS_CONFIG_PENDING (1u<<5)
#define CIP_TCPIP_STATUS_ACD_STATUS     (1u<<6)

/* instance attribute #2: Configuration Capability */
#define CIP_TCPIP_CONFIG_CAP_BOOTP    (1<<0)
#define CIP_TCPIP_CONFIG_CAP_DNS      (1<<1)
#define CIP_TCPIP_CONFIG_CAP_DHCP     (1<<2)
#define CIP_TCPIP_CONFIG_CAP_SETTABLE (1<<4)
#define CIP_TCPIP_CONFIG_CAP_HW       (1<<5)
#define CIP_TCPIP_CONFIG_CAP_RESET    (1<<6)
#define CIP_TCPIP_CONFIG_CAP_ACD      (1<<7)

/* instance attribute #3: Configuration Control */
#define CIP_TCPIP_CONFIG_CNTRL_DNS_ENABLE 0x00000010
#define CIP_TCPIP_CONFIG_CNTRL_METHOD_gp  0x0000000F
#define CIP_TCPIP_CONFIG_CNTRL_STATIC_IP  0x00000000
#define CIP_TCPIP_CONFIG_CNTRL_DHCP       0x00000002

/* instance attribute #8 TTL Value */
#define CIP_TCPIP_TTL_DEFAULT 1

/* instance attribute #9 Mcast Config */
#define CIP_TCPIP_MC_CFG_DEFAULT 0
#define CIP_TCPIP_MC_CFG_NUM_DEFAULT 32

/* no Multicast IP address available */
#define CIP_TCPIP_MCAST_IP_ERROR   0xFFFFFFFF


/****************************************************************************/
/* Public Functions */
/****************************************************************************/
EIP_STATUS eip_tcpIpIfInit(
    EIP_INSTANCE_T *pEip                        /**< EtherNet/IP instance */
);

void eip_tcpIpIfShutdown(
    EIP_INSTANCE_T *pEip                        /**< EtherNet/IP instance */
);

void eip_tcpIpIfIPSettingsUpdate(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_UINT32 *pIpAddr,                        /**< IP address */
    EIP_UINT32 *pMask,                          /**< net mask */
    EIP_UINT32 *pGateway,                       /**< gateway address */
    EIP_UINT32 *pDNS1,                          /**< name server 1 */
    EIP_UINT32 *pDNS2,                          /**< name server 2 */
    char *pDomainName,                          /**< domain name string */
    char *pHostName                             /**< host name string */
);

EIP_UINT32 eip_tcpIpIfMcastAddrGet(
    EIP_INSTANCE_T *pEip                        /**< EtherNet/IP instance */
);

void eip_tcpIpIfMcastAddrRelease(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_UINT32 mcIpAddr                         /**< closed multicast address */
);

#endif /* CIPTCPIPINTERFACE_H */
