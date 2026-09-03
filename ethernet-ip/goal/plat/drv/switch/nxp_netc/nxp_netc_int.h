/** @file
 *
 * @brief NXP NETC Switch Driver.
 *
 * @copyright
 * Copyright 2022 port GmbH Halle/Saale.
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

#ifndef GOAL_NXP_NETC_INT_H
#define GOAL_NXP_NETC_INT_H

#include <goal_includes.h>
#include "nxp_netc_bsp.h"


/****************************************************************************/
/* Structures */
/****************************************************************************/
typedef struct GOAL_SW_NETC_INST {
    GOAL_MI_ETH_T *pMiInst;                     /**< local instance handle */
    GOAL_ETH_MAC_ADDR_T mDevMac;                /**< default GOAL MAC address */
    NETC_BSP_INST_T *pDrvInst;                  /**< driver instance */
    uint8_t portEnableMask;                     /**< mask of enabled switch ports */
} GOAL_SW_NETC_INST_T;


/****************************************************************************/
/* Defines */
/****************************************************************************/
#define GOAL_SW_NETC_PORT_HOST            0x00  /**< port 0 is management port */
#define GOAL_SW_NETC_CNT_PORT_EXT         4     /**< number of external ports */

#define GOAL_SW_NETC_BITMASK_FORWARD_HOST 0x10  /**< bitmask for forwarding to host port */
#define GOAL_SW_NETC_BITMASK_FORWARD_EXT  0x0f  /**< bitmask for forwarding to external port */

#define GOAL_SW_NETC_BITMASK_FORWARD_EXT1 0x01  /**< bitmask for forwarding to first external port */
#define GOAL_SW_NETC_BITMASK_FORWARD_EXT2 0x02  /**< bitmask for forwarding to first second port */
#define GOAL_SW_NETC_BITMASK_FORWARD_EXT3 0x04  /**< bitmask for forwarding to first third port */
#define GOAL_SW_NETC_BITMASK_FORWARD_EXT4 0x08  /**< bitmask for forwarding to first fourth port */


#endif /* GOAL_NXP_NETC_INT_H */
