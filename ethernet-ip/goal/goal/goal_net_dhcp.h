/** @file
 *
 * @brief DHCP Client
 *
 * @details
 * This module implements a DHCPv4 client.
 *
 * @copyright
 * Copyright 2016-2020.
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

#ifndef GOAL_NET_DHCP_H
#define GOAL_NET_DHCP_H


/****************************************************************************/
/* Public functions */
/****************************************************************************/
GOAL_STATUS_T goal_netDhcpInit(
    GOAL_MI_NET_T *pMiNetHdl                    /**< MI NET handle */
);

GOAL_STATUS_T goal_netDhcpShutdown(
    GOAL_MI_NET_T *pMiNetHdl                    /**< MI NET handle */
);

GOAL_STATUS_T goal_netDhcpStart(
    GOAL_MI_NET_T *pMiNetHdl                    /**< MI NET handle */
);

GOAL_STATUS_T goal_netDhcpStop(
    GOAL_MI_NET_T *pMiNetHdl                    /**< MI NET handle */
);

GOAL_STATUS_T goal_netDhcpRelease(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI NET handle */
    GOAL_BOOL_T *pReleaseTx                     /**< send RELEASE message */
);

GOAL_STATUS_T goal_netDhcpGetAddr(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI NET handle */
    GOAL_NET_DHCP_T *pAddrData                  /**< DHCP address data */
);

GOAL_STATUS_T goal_netDhcpGetState(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI NET handle */
    uint32_t *pState                            /**< current state */
);

GOAL_STATUS_T goal_netDhcpClientIdSet(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI NET handle */
    char *pStrName,                             /**< client id */
    uint8_t lenName,                            /**< client id length */
    GOAL_BOOL_T flgPerm                         /**< permanent flag */
);


#endif /* GOAL_NET_DHCP_H */
