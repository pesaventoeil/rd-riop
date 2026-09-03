/** @file
 *
 * @brief GOAL Network Channel Handling
 *
 * This module implements the interface for managing TCP and UDP connections.
 *
 * @copyright
 * Copyright 2010-2025.
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

#ifndef GOAL_NET_CHAN_H
#define GOAL_NET_CHAN_H

#include "goal_includes.h"


/****************************************************************************/
/* Public Defines */
/****************************************************************************/
#define GOAL_NET_FLAGS_USED             (1u<<0) /**< net channel flag: channel is used */
#define GOAL_NET_FLAGS_ACTIVE           (1u<<1) /**< net channel flag: channel is active */
#define GOAL_NET_FLAGS_CLOSE_MAN        (1u<<2) /**< net channel flag: channel can only be manually closed */
#define GOAL_NET_FLAGS_DYN_PORT         (1u<<3) /**< net channel flag: dynamically assigned local port */
#define GOAL_NET_FLAGS_TUNNELING        (1u<<4) /**< net channel flag:tunneling mode enabled */


/****************************************************************************/
/* Public Functions */
/****************************************************************************/
GOAL_STATUS_T goal_netChanInitPre(
    void
);

GOAL_STATUS_T goal_netChanCreate(
    GOAL_NET_CHAN_T **ppChan,                   /**< [out] net channel reference */
    GOAL_NET_TYPE_T type,                       /**< channel type */
    uint32_t numDataChannels                    /**< number of data channels for TCP Listener */
);

GOAL_STATUS_T goal_netChanDelete(
    GOAL_NET_CHAN_T *pChan                      /**< channel handle */
);

GOAL_STATUS_T goal_netChanOpen(
    GOAL_NET_CHAN_T *pChan,                     /**< channel handle */
    GOAL_NET_ADDR_T *pAddr,                     /**< remote address */
    GOAL_NET_CB_T callback,                     /**< channel callback */
    void *pArg                                  /**< callback argument */
);

GOAL_STATUS_T goal_netChanTunnelingEnable(
    GOAL_NET_CHAN_T *pChanHandle,               /**< tunnel channel handle */
    GOAL_NET_CB_T tunnelCallback,               /**< [in] tunnel callback */
    void *pTunnelCbArg,                         /**< [in] tunnel callback argument */
    GOAL_NET_CB_T *pApplCb,                     /**< [out] application callback */
    void **ppApplCbArg                          /**< [out] application callback argument */
);

GOAL_STATUS_T goal_netChanCloseManSet(
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle structure */
    GOAL_BOOL_T flgCloseMan                     /**< manually close flag if */
);

GOAL_STATUS_T goal_netChanFreeDataChanGet(
    GOAL_NET_CHAN_T *pListener,                 /**< listener handle */
    GOAL_NET_CHAN_T **ppDataChan                /**< [out] data channel */
);

GOAL_STATUS_T goal_netChanOpenTgt(
    GOAL_NET_CHAN_T *pChan,                     /**< data channel handle */
    GOAL_NET_ADDR_T *pAddr                      /**< remote address */
);

GOAL_STATUS_T goal_netChanConnectedTgt(
    GOAL_NET_CHAN_T *pChanHandle                /**< channel handle */
);

GOAL_STATUS_T goal_netChanClose(
    GOAL_NET_CHAN_T *pChanHandle                /**< channel handle */
);

GOAL_STATUS_T goal_netChanCloseTgt(
    GOAL_NET_CHAN_T *pChanHandle                /**< channel handle */
);

GOAL_STATUS_T goal_netChanReopen(
    GOAL_NET_CHAN_T *pChanHandle                /**< channel handle */
);

GOAL_STATUS_T goal_netChanActivate(
    GOAL_NET_CHAN_T *pChanHandle                /**< channel handle */
);

GOAL_STATUS_T goal_netChanRemoteAddrGet(
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_NET_ADDR_T *pAddr                      /**< pointer to store address */
);

GOAL_STATUS_T goal_netChanSend(
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_BUFFER_T *pBuf                         /**< buffer with data to send */
);

GOAL_STATUS_T goal_netChanTunnelSend(
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_BUFFER_T *pBuf                         /**< buffer with data to send */
);

GOAL_STATUS_T goal_netChanOptionSet(
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_NET_OPTION_T option,                   /**< GOAL option ID */
    void *pValue                                /**< option value */
);

void goal_netChanRecv(
    GOAL_NET_CHAN_T *pChan,                     /**< channel handle */
    GOAL_NET_ADDR_T *pAddr,                     /**< remote address */
    GOAL_BUFFER_T *pBuf                         /**< buffer with rx data */
);

GOAL_STATUS_T goal_netChanIpAddrStrToU32Convert(
    const char *pStr,                           /**< ASCII string */
    uint32_t strLen,                            /**< string buffer length */
    uint32_t *pNetAddr                          /**< net address */
);

int goal_netChanIpAddrU32ToStrConvert(
    const uint32_t netAddr,                     /**< net address */
    char *pBuf,                                 /**< ASCII buffer */
    uint32_t bufLen                             /**< buffer length */
);

GOAL_STATUS_T goal_netChanDataGet(
    GOAL_NET_CHAN_T **ppChanData                /**< [out] channel data */
);


#endif /* GOAL_NET_CHAN_H */
