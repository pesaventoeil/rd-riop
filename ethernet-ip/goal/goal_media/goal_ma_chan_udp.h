/** @file
 *
 * @brief Generic MA for CHAN_UDP
 *
 * @details
 * This module provides a MA for CHAN_UDP
 *
 *
 * @copyright
 * Copyright 2010-2018.
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
#ifndef GOAL_MA_CHAN_UDP_H
#define GOAL_MA_CHAN_UDP_H


/****************************************************************************/
/* Defines */
/****************************************************************************/
#ifndef GOAL_CONFIG_BUF_NUM
# define GOAL_CONFIG_BUFFERS 10
#endif

#define goal_maChanUdpNew(pChanTcpHdl, ppChanHandle, pChanOut, pAddr, type, callback) goal_maChanUdpNewNonBlockingActivate(pChanTcpHdl, ppChanHandle, pChanOut, pAddr, type, callback, GOAL_FALSE, GOAL_FALSE, GOAL_FALSE)


/****************************************************************************/
/* Forward declaration */
/****************************************************************************/
struct GOAL_MA_CHAN_UDP_T;                      /**< MA CHAN_UDP handle */


/****************************************************************************/
/* Typedefs */
/****************************************************************************/
/** network receive callback */
typedef void (*GOAL_MA_CHAN_UDP_CB_T)(
    struct GOAL_MA_CHAN_UDP_T *pMaChanUdp,      /**< net ma handle */
    GOAL_NET_CB_TYPE_T cbType,                  /**< callback type */
    struct GOAL_NET_CHAN_T *pChan,              /**< channel descriptor */
    struct GOAL_BUFFER_T *pBuf                  /**< GOAL buffer */
);

/**< net channel new function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_CHAN_UDP_NEW) (
    struct GOAL_MA_CHAN_UDP_T *pChanUdpHdl,     /**< pointer to store CHAN_UDP handler */
    GOAL_NET_CHAN_T **ppChanHandle,             /**< pointer to channel handle */
    GOAL_NET_CHAN_T *pChanOut,                  /**< pointer to channel handle for output */
    GOAL_NET_ADDR_T *pAddr,                     /**< remote address */
    GOAL_NET_TYPE_T type,                       /**< connection type */
    GOAL_MA_CHAN_UDP_CB_T callback,             /**< channel callback */
    GOAL_BOOL_T flgOption,                      /**< flag for socket option */
    GOAL_BOOL_T optVal,                         /**< value of socket option */
    GOAL_BOOL_T flgImmediateActivation          /**< flag for immediate activation */);

/**< net channel close function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_CHAN_UDP_CLOSE) (
    struct GOAL_MA_CHAN_UDP_T *pChanUdpHdl,     /**< pointer to store CHAN_UDP handler */
    GOAL_NET_CHAN_T *pChanHandle                /**< pointer to channel handle */
);

/**< net channel set non blocking function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_CHAN_UDP_SET_NONBLOCKING) (
    struct GOAL_MA_CHAN_UDP_T *pChanUdpHdl,     /**< pointer to store CHAN_UDP handler */
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_BOOL_T flgOption                       /**< option value */
);

/**< net channel set broadcast function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_CHAN_UDP_SET_BROADCAST) (
    struct GOAL_MA_CHAN_UDP_T *pChanUdpHdl,     /**< pointer to store CHAN_UDP handler */
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_BOOL_T flgOption                       /**< option value */
);

/**< net channel activate function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_CHAN_UDP_ACTIVATE) (
    struct GOAL_MA_CHAN_UDP_T *pChanUdpHdl,     /**< pointer to store CHAN_UDP handler */
    GOAL_NET_CHAN_T *pChanHandle                /**< channel handle */
);

/**< net channel activate function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_CHAN_UDP_GET_REMOTE_ADDR) (
    struct GOAL_MA_CHAN_UDP_T *pChanUdpHdl,     /**< pointer to store CHAN_UDP handler */
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_NET_ADDR_T *pAddr                      /**< remote address */
);

/**< net channel activate function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_CHAN_UDP_SEND) (
    struct GOAL_MA_CHAN_UDP_T *pChanUdpHdl,     /**< pointer to store CHAN_UDP handler */
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_BUFFER_T *pBuf                         /**< buffer with data to send */
);

/**< CHAN_UDP handle instance struct */
typedef struct GOAL_MA_CHAN_UDP_T {
    struct GOAL_MA_CHAN_UDP_T *pNext;           /**< next entry */
    uint32_t id;                                /**< MA ID */
    GOAL_BOOL_T flgUsed;                        /**< used flag */
    GOAL_MA_CHAN_UDP_NEW fctChanNew;            /**< new CHAN_UDP channel function */
    GOAL_MA_CHAN_UDP_CLOSE fctChanClose;        /**< close CHAN_UDP channel function */
    GOAL_MA_CHAN_UDP_SET_NONBLOCKING fctChanSetNonBlocking; /**< set CHAN_UDP channel options function */
    GOAL_MA_CHAN_UDP_SET_BROADCAST fctChanSetBroadcast; /**< set CHAN_UDP channel options function */
    GOAL_MA_CHAN_UDP_GET_REMOTE_ADDR fctChanGetRemoteAddr; /**< get channel remote address function */
    GOAL_MA_CHAN_UDP_ACTIVATE fctChanActivate;  /**< CHAN_UDP channel activate function */
    GOAL_MA_CHAN_UDP_SEND fctChanSend;          /**< CHAN_UDP channel send function */
    void *pFctNotifyArg;                        /**< notification function argument */
    void *pDrvData;                             /**< driver specific data */
} GOAL_MA_CHAN_UDP_T;


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_maChanUdpDrvReg(
    uint32_t id,                                /**< ID */
    GOAL_MA_CHAN_UDP_T **ppHdlMaChanUdp,        /**< CHAN_UDP handle ref ptr */
    GOAL_MA_CHAN_UDP_NEW fChanNew,              /**< new CHAN_UDP channel function */
    GOAL_MA_CHAN_UDP_CLOSE fChanClose,          /**< close CHAN_UDP channel function */
    GOAL_MA_CHAN_UDP_SET_NONBLOCKING fChanSetNonBlocking, /**< set CHAN_UDP channel options function */
    GOAL_MA_CHAN_UDP_SET_BROADCAST fChanSetBroadcast, /**< set CHAN_UDP channel options function */
    GOAL_MA_CHAN_UDP_GET_REMOTE_ADDR fctChanGetRemoteAddr, /**< get channel remote address function */
    GOAL_MA_CHAN_UDP_ACTIVATE fChanActivate,    /**< CHAN_UDP channel activate function */
    GOAL_MA_CHAN_UDP_SEND fChanSend,            /**< CHAN_UDP channel send function */
    void *pDrvData                              /**< driver specific data */
);

GOAL_STATUS_T goal_maChanUdpGetById(
    GOAL_MA_CHAN_UDP_T **ppHdlMaChanUdp,        /**< CHAN_UDP handle ref ptr */
    uint32_t id                                 /**< MA id */
);

GOAL_STATUS_T goal_maChanUdpOpen(
    uint32_t id,                                /**< MA id */
    GOAL_MA_CHAN_UDP_T **ppHdlMaChanUdp         /**< CHAN_UDP handle ref ptr */
);

GOAL_STATUS_T goal_maChanUdpIpSet(
    GOAL_MA_CHAN_UDP_T *pChanUdpHdl,            /**< pointer to store CHAN_UDP handler */
    uint32_t addrIp,                            /**< IP address */
    uint32_t addrMask,                          /**< subnet mask */
    uint32_t addrGw,                            /**< gateway */
    GOAL_BOOL_T flgTemp                         /**< temporary IP config flag */
);

GOAL_STATUS_T goal_maChanUdpNewNonBlockingActivate(
    GOAL_MA_CHAN_UDP_T *pChanUdpHdl,            /**< pointer to store CHAN_UDP handler */
    GOAL_NET_CHAN_T **ppChanHandle,             /**< pointer to channel handle */
    GOAL_NET_CHAN_T *pChanOut,                  /**< pointer to channel handle for output */
    GOAL_NET_ADDR_T *pAddr,                     /**< remote address */
    GOAL_NET_TYPE_T type,                       /**< connection type */
    GOAL_MA_CHAN_UDP_CB_T callback,             /**< channel callback */
    GOAL_BOOL_T flgOption,                      /**< flag for socket option */
    GOAL_BOOL_T optVal,                         /**< value of socket option */
    GOAL_BOOL_T flgImmediateActivation          /**< flag for immediate activation */
);

GOAL_STATUS_T goal_maChanUdpClose(
    GOAL_MA_CHAN_UDP_T *pChanUdpHdl,            /**< pointer to store CHAN_UDP handler */
    GOAL_NET_CHAN_T *pChanHandle                /**< pointer to channel handle */
);

GOAL_STATUS_T goal_maChanUdpSetNonBlocking(
    GOAL_MA_CHAN_UDP_T *pChanUdpHdl,            /**< pointer to store CHAN_UDP handler */
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_BOOL_T flgOption                       /**< option value */
);

GOAL_STATUS_T goal_maChanUdpSetBroadcast(
    GOAL_MA_CHAN_UDP_T *pChanUdpHdl,            /**< pointer to store CHAN_UDP handler */
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_BOOL_T flgOption                       /**< option value */
);

GOAL_STATUS_T goal_maChanUdpActivate(
    GOAL_MA_CHAN_UDP_T *pChanUdpHdl,            /**< pointer to store CHAN_UDP handler */
    GOAL_NET_CHAN_T *pChanHandle                /**< channel handle */
);

GOAL_STATUS_T goal_maChanUdpSend(
    GOAL_MA_CHAN_UDP_T *pChanUdpHdl,            /**< pointer to store CHAN_UDP handler */
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_BUFFER_T *pBuf                         /**< buffer with data to send */
);

GOAL_STATUS_T goal_maChanUdpGetRemoteAddr(
    GOAL_MA_CHAN_UDP_T *pChanUdpHdl,            /**< pointer to store CHAN_UDP handler */
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_NET_ADDR_T *pAddr                      /**< remote address */
);

#endif /* GOAL_MA_CHAN_UDP_H */
