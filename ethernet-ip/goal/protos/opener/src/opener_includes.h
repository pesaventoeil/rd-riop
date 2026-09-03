/** @file
 *
 * @brief common header for OpENer
 *
 * This header file must be included by all files that are using OpENer.
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

#ifndef OPENER_INCLUDES_H
#define OPENER_INCLUDES_H


/****************************************************************************/
/* Default Config */
/****************************************************************************/
#ifndef OPENER_CONFIG_QOS_VLAN_SUPPORT
# define OPENER_CONFIG_QOS_VLAN_SUPPORT 0
#endif /* OPENER_CONFIG_QOS_VLAN_SUPPORT */


/****************************************************************************/
/* Includes */
/****************************************************************************/
#include "opener_target_types.h"
#include "goal_eip.h"
#include "eip_types.h"
#include "opener_target.h"
#include "opener_led.h"
#include "opener_api.h"
#include "cipassembly.h"
#include "cipclass3connection.h"
#include "cipcommon.h"
#include "cipconnectionmanager.h"
#include "cipdlr.h"
#include "ciperror.h"
#include "cipethernetlink.h"
#include "cipidentity.h"
#include "cipioconnection.h"
#include "ciplldp.h"
#include "cipmessagerouter.h"
#include "cipqos.h"
#include "ciptcpipinterface.h"
#include "encap.h"
#include "endianconv.h"


/****************************************************************************/
/* Public defines */
/****************************************************************************/
/* default ACD conflict configuration: switch to static IP,
 * as the DHCP flag is GOAL_FALSE
 */
#define ACD_CFG_DFLT  { GOAL_CONFIG_NET_ADDR_IP_DEFAULT, \
                        GOAL_CONFIG_NET_ADDR_MASK_DEFAULT, \
                        GOAL_CONFIG_NET_ADDR_GATEWAY_DEFAULT }

/* default configurations */
#define EIP_CFG_DFLT_VENDORID            1114   /**< default value: ODVA Vendor ID */
#define EIP_CFG_DFLT_DEVICETYPE          0x2B   /**< default value: CIP device type */
#define EIP_CFG_DFLT_PRODUCTCODE         1      /**< default value: vendor specific product code */
#define EIP_CFG_DFLT_REVISIONMAJOR       1      /**< default value: major revision */
#define EIP_CFG_DFLT_REVISIONMINOR       1      /**< default value: minor revision */
#define EIP_CFG_DFLT_SERIALNUM           1      /**< default value: serial number */
#define EIP_CFG_DFLT_PRODUCTNAME         "EtherNet/IP Adapter" /**< default value: product name */
#ifndef EIP_CFG_DFLT_DOMAINNAME
# define EIP_CFG_DFLT_DOMAINNAME         "port.de" /**< default value: domain name */
#endif
#ifndef EIP_CFG_DFLT_HOSTNAME
# define EIP_CFG_DFLT_HOSTNAME           "eipdevice" /**< default value: host name */
#endif
#define EIP_CFG_DFLT_NUMEXPLICITCON      6      /**< default value: number of explicit connections */
#define EIP_CFG_DFLT_ETHCOUNTERS         GOAL_TRUE /**< default value: support of Ethernet Link counters */
#define EIP_CFG_DFLT_ETHIFCONTROL        GOAL_TRUE /**< default value: support of Ethernet Link Interface Control */
#define EIP_CFG_DFLT_ETHCHANGEAFTERRESET GOAL_FALSE /**< default value: change of interface config only after reset */
#define EIP_CFG_DFLT_IPCHANGEAFTERRESET  GOAL_FALSE /**< default value: change of IP address only after reset */
#define EIP_CFG_DFLT_NUMSESSIONS         5      /**< default value: number of supported encapsulation sessions */
#define EIP_CFG_DFLT_O2TRUNIDLEHEADER    GOAL_TRUE /**< default value: consumed data has Run/Idle Header */
#define EIP_CFG_DFLT_T2ORUNIDLEHEADER    GOAL_FALSE /**< default value: produced data has Run/Idle Header */
#define EIP_CFG_DFLT_QOSSUPPORT          GOAL_TRUE /**< default value: QoS object is supported */
#define EIP_CFG_DFLT_NUMDELAYEDMSG       2      /**< default value: size of Delayed Encapsulation Message buffer */
#define EIP_CFG_DFLT_DHCPSUPPORT         GOAL_FALSE /**< default value: device is DHCP client */
#define EIP_CFG_DFLT_DLRSUPPORT          GOAL_FALSE /**< default value: device is DLR node */
#define EIP_CFG_DFLT_ACDSUPPORT          GOAL_FALSE /**< default value: no ACD support */
#define EIP_CFG_DFLT_ACDCONFIG           ACD_CFG_DFLT /**< default value: switch to static IP */
#define EIP_CFG_DFLT_NUMLLDPTABLEENTRIES      9 /**< default value: number of LLDP Data Table entries */
#define EIP_CFG_DFLT_HWCONFIGSUPPORT     GOAL_FALSE /**< default value: no support */

#endif /* OPENER_INCLUDES_H */
