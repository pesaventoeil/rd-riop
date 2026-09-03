/** @file
 *
 * @brief Generic MA for TCP channel
 *
 * @details
 * This module provides a MA for a TCP channel
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
#ifndef GOAL_MA_CHAN_TCP_H
#define GOAL_MA_CHAN_TCP_H


/****************************************************************************/
/* Defines */
/****************************************************************************/
#ifndef GOAL_CONFIG_BUF_NUM
# define GOAL_CONFIG_BUFFERS 10
#endif

#define goal_maChanTcpNew(pChanTcpHdl, ppChanHandle, pChanOut, pAddr, type, callback) goal_maChanTcpNewNonBlockingActivate(pChanTcpHdl, ppChanHandle, pChanOut, pAddr, type, callback, GOAL_FALSE, GOAL_FALSE, GOAL_FALSE)


/****************************************************************************/
/* Forward declaration */
/****************************************************************************/
struct GOAL_MA_CHAN_TCP_T;                      /**< MA CHAN_TCP handle */


/****************************************************************************/
/* Typedefs */
/****************************************************************************/
/** network receive callback */
typedef void (*GOAL_MA_CHAN_TCP_CB_T)(
    struct GOAL_MA_CHAN_TCP_T *pMaChanTcp,      /**< net ma handle */
    GOAL_NET_CB_TYPE_T cbType,                  /**< callback type */
    struct GOAL_NET_CHAN_T *pChan,              /**< channel descriptor */
    struct GOAL_BUFFER_T *pBuf                  /**< GOAL buffer */
);

/**< net channel new function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_CHAN_TCP_NEW) (
    struct GOAL_MA_CHAN_TCP_T *pChanTcpHdl,     /**< pointer to store CHAN_TCP handler */
    GOAL_NET_CHAN_T **ppChanHandle,             /**< pointer to channel handle */
    GOAL_NET_CHAN_T *pChanOut,                  /**< pointer to channel handle for output */
    GOAL_NET_ADDR_T *pAddr,                     /**< remote address */
    GOAL_NET_TYPE_T type,                       /**< connection type */
    GOAL_MA_CHAN_TCP_CB_T callback,             /**< channel callback */
    GOAL_BOOL_T flgOption,                      /**< flag for socket option */
    GOAL_BOOL_T optVal,                         /**< value of socket option */
    GOAL_BOOL_T flgImmediateActivation          /**< flag for immediate activation */
);

/**< net channel close function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_CHAN_TCP_CLOSE) (
    struct GOAL_MA_CHAN_TCP_T *pChanTcpHdl,     /**< pointer to store CHAN_TCP handler */
    GOAL_NET_CHAN_T *pChanHandle                /**< pointer to channel handle */
);

/**< net channel set channel to non blocking prototype */
typedef GOAL_STATUS_T (*GOAL_MA_CHAN_TCP_SET_NONBLOCKING) (
    struct GOAL_MA_CHAN_TCP_T *pChanTcpHdl,     /**< pointer to store CHAN_TCP handler */
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_BOOL_T flgOption                       /**< non blocking state */
);

/**< net channel activate function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_CHAN_TCP_ACTIVATE) (
    struct GOAL_MA_CHAN_TCP_T *pChanTcpHdl,     /**< pointer to store CHAN_TCP handler */
    GOAL_NET_CHAN_T *pChanHandle                /**< channel handle */
);

/**< net channel activate function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_CHAN_TCP_GET_REMOTE_ADDR) (
    struct GOAL_MA_CHAN_TCP_T *pChanTcpHdl,     /**< pointer to store CHAN_TCP handler */
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_NET_ADDR_T *pAddr                      /**< remote address */
);

/**< net channel activate function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_CHAN_TCP_SEND) (
    struct GOAL_MA_CHAN_TCP_T *pChanTcpHdl,     /**< pointer to store CHAN_TCP handler */
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_BUFFER_T *pBuf                         /**< buffer with data to send */
);

/**< CHAN_TCP handle instance struct */
typedef struct GOAL_MA_CHAN_TCP_T {
    struct GOAL_MA_CHAN_TCP_T *pNext;           /**< next entry */
    uint32_t id;                                /**< MA ID */
    GOAL_BOOL_T flgUsed;                        /**< used flag */
    GOAL_MA_CHAN_TCP_NEW fctChanNew;            /**< new CHAN_TCP channel function */
    GOAL_MA_CHAN_TCP_CLOSE fctChanClose;        /**< close CHAN_TCP channel function */
    GOAL_MA_CHAN_TCP_SET_NONBLOCKING fctChanSetNonBlocking; /**< set CHAN_TCP channel to non blocking function */
    GOAL_MA_CHAN_TCP_GET_REMOTE_ADDR fctChanGetRemoteAddr; /**< get channel remote address function */
    GOAL_MA_CHAN_TCP_ACTIVATE fctChanActivate;  /**< CHAN_TCP channel activate function */
    GOAL_MA_CHAN_TCP_SEND fctChanSend;          /**< CHAN_TCP channel send function */
    void *pFctNotifyArg;                        /**< notification function argument */
    void *pDrvData;                             /**< driver specific data */
} GOAL_MA_CHAN_TCP_T;


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_maChanTcpDrvReg(
    uint32_t id,                                /**< ID */
    GOAL_MA_CHAN_TCP_T **ppHdlMaChanTcp,        /**< CHAN_TCP handle ref ptr */
    GOAL_MA_CHAN_TCP_NEW fChanNew,              /**< new CHAN_TCP channel function */
    GOAL_MA_CHAN_TCP_CLOSE fChanClose,          /**< close CHAN_TCP channel function */
    GOAL_MA_CHAN_TCP_SET_NONBLOCKING fChanSetNonBlocking, /**< set CHAN_TCP channel options function */
    GOAL_MA_CHAN_TCP_GET_REMOTE_ADDR fctChanGetRemoteAddr, /**< get channel remote address function */
    GOAL_MA_CHAN_TCP_ACTIVATE fChanActivate,    /**< CHAN_TCP channel activate function */
    GOAL_MA_CHAN_TCP_SEND fChanSend,            /**< CHAN_TCP channel send function */
    void *pDrvData                              /**< driver specific data */
);

GOAL_STATUS_T goal_maChanTcpGetById(
    GOAL_MA_CHAN_TCP_T **ppHdlMaChanTcp,        /**< CHAN_TCP handle ref ptr */
    uint32_t id                                 /**< MA id */
);

GOAL_STATUS_T goal_maChanTcpClose(
    GOAL_MA_CHAN_TCP_T *pChanTcpHdl,            /**< pointer to store CHAN_TCP handler */
    GOAL_NET_CHAN_T *pChanHandle                /**< pointer to channel handle */
);

GOAL_STATUS_T goal_maChanTcpOpen(
    uint32_t id,                                /**< MA id */
    GOAL_MA_CHAN_TCP_T **ppHdlMaChanTcp         /**< CHAN_TCP handle ref ptr */
);

GOAL_STATUS_T goal_maChanTcpNewNonBlockingActivate(
    GOAL_MA_CHAN_TCP_T *pChanTcpHdl,            /**< pointer to store CHAN_TCP handler */
    GOAL_NET_CHAN_T **ppChanHandle,             /**< pointer to channel handle */
    GOAL_NET_CHAN_T *pChanOut,                  /**< pointer to channel handle for output */
    GOAL_NET_ADDR_T *pAddr,                     /**< remote address */
    GOAL_NET_TYPE_T type,                       /**< connection type */
    GOAL_MA_CHAN_TCP_CB_T callback,             /**< channel callback */
    GOAL_BOOL_T flgOption,                      /**< flag for socket option */
    GOAL_BOOL_T optVal,                         /**< value of socket option */
    GOAL_BOOL_T flgImmediateActivation          /**< flag for immediate activation */
);

GOAL_STATUS_T goal_maChanTcpClose(
    GOAL_MA_CHAN_TCP_T *pChanTcpHdl,            /**< pointer to store CHAN_TCP handler */
    GOAL_NET_CHAN_T *pChanHandle                /**< pointer to channel handle */
);

GOAL_STATUS_T goal_maChanTcpSetNonBlocking(
    GOAL_MA_CHAN_TCP_T *pChanTcpHdl,            /**< pointer to store CHAN_TCP handler */
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_BOOL_T flgOption                       /**< non blocking state */
);

GOAL_STATUS_T goal_maChanTcpActivate(
    GOAL_MA_CHAN_TCP_T *pChanTcpHdl,            /**< pointer to store CHAN_TCP handler */
    GOAL_NET_CHAN_T *pChanHandle                /**< channel handle */
);

GOAL_STATUS_T goal_maChanTcpReopen(
    struct GOAL_MA_CHAN_TCP_T *pChanTcpHdl,     /**< pointer to store CHAN_TCP handler */
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_BOOL_T immediateActivation             /**< flag for immediate activation */
);

GOAL_STATUS_T goal_maChanTcpSend(
    GOAL_MA_CHAN_TCP_T *pChanTcpHdl,            /**< pointer to store CHAN_TCP handler */
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_BUFFER_T *pBuf                         /**< buffer with data to send */
);

GOAL_STATUS_T goal_maChanTcpGetRemoteAddr(
    GOAL_MA_CHAN_TCP_T *pChanTcpHdl,            /**< pointer to store CHAN_TCP handler */
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_NET_ADDR_T *pAddr                      /**< remote address */
);

#endif /* GOAL_MA_CHAN_TCP_H */
