/** @file
 *
 * @brief Generic Media Adapter for Ethernet
 *
 *
 * @copyright
 * Copyright 2010-2020.
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

#ifndef GOAL_MA_ETH_H
#define GOAL_MA_ETH_H


/****************************************************************************/
/* Forward declaration */
/****************************************************************************/
struct GOAL_MA_ETH_T;                           /**< MA Eth handle */


/****************************************************************************/
/* Typedefs */
/****************************************************************************/

/**< init function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_ETH_INIT_T)(
    struct GOAL_MA_ETH_T *pHdlMaEth             /**< [in] MA handle for Eth */
);

/**< open function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_ETH_OPEN_T)(
    struct GOAL_MA_ETH_T *pHdlMaEth             /**< [in] MA handle for Eth */
);

/**< send function prototype */
typedef void (*GOAL_MA_ETH_SEND_T)(
    struct GOAL_MA_ETH_T *pHdlMaEth             /**< [in] MA handle for Eth */
);

/**< ethernet command function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_ETH_CMD_T)(
    struct GOAL_MA_ETH_T *pHdlMaEth,            /**< [in] MA handle for Eth */
    uint32_t cmd,                               /**< command */
    GOAL_BOOL_T wrFlag,                         /**< write flag */
    uint32_t port,                              /**< port ID */
    void *pArg                                  /**< argument */
);

/**< eth received a frame cb function prototype */
typedef void (*GOAL_MA_ETH_CB_RECV_T)(
    struct GOAL_MA_ETH_T *pHdlMaEth,            /**< [in] MA handle for Eth */
    uint32_t portIdx,                           /**< Ethernet port index */
    GOAL_BUFFER_T **ppBuf                       /**< [out] OS Ethernet buffer */
);

/**< eth get frame to send cb function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_ETH_CB_SEND_FRAME_GET_T)(
    struct GOAL_MA_ETH_T *pHdlMaEth,            /**< [in] MA handle for Eth */
    GOAL_BUFFER_T **ppBuf                       /**< [out] OS Ethernet buffer */
);

/**< eth send done cb function prototype */
typedef void (*GOAL_MA_ETH_CB_SEND_DONE_T)(
    struct GOAL_MA_ETH_T *pHdlMaEth,            /**< [in] MA handle */
    GOAL_BUFFER_T **ppBuf,                      /**< [out] buffer ref */
    GOAL_BOOL_T doneFlag                        /**< succesfully send? */
);


/****************************************************************************/
/* Cb specific datatypes */
/****************************************************************************/

/**< Cb types */
typedef union {
    GOAL_MA_ETH_CB_RECV_T fCbRecv;              /**< cb func to receive */
    GOAL_MA_ETH_CB_SEND_FRAME_GET_T fCbSendFrameGet; /**< cb func to get frame to send */
    GOAL_MA_ETH_CB_SEND_DONE_T fCbSendDone;     /**< cb func send done */
} GOAL_MA_ETH_CB_T;

/**< Cb identifier */
typedef enum {
    GOAL_MA_ETH_RECV,                           /**< cb type received frame */
    GOAL_MA_ETH_SEND_FRAME_GET,                 /**< cb type get frame to send */
    GOAL_MA_ETH_SEND_DONE                       /**< cb type send done */
} GOAL_MA_ETH_CB_TYPE_T;


/****************************************************************************/
/* Structures */
/****************************************************************************/

/**< cb function */
typedef struct GOAL_MA_ETH_CB_LIST_T {
    struct GOAL_MA_ETH_CB_LIST_T *pNext;        /**< next pointer */
    GOAL_MA_ETH_CB_TYPE_T typeCb;               /**< cb type eg send, sendDone etc. */
    GOAL_MA_ETH_CB_T fCb;                       /**< cb takes void pointer */
} GOAL_MA_ETH_CB_LIST_T;


/**< MA Eth cfg structure */
typedef struct {
    uint32_t port;                              /**< port */
    uint8_t macAddr[6];                         /**< MAC */
} GOAL_MA_ETH_CFG_T;


/**< MA Eth handle instance struct */
typedef struct GOAL_MA_ETH_T {
    struct GOAL_MA_ETH_T *pNext;                /**< next */
    uint32_t id;                                /**< id */
    GOAL_MA_ETH_INIT_T fInit;                   /**< init function */
    GOAL_MA_ETH_OPEN_T fOpen;                   /**< open function */
    GOAL_MA_ETH_SEND_T fSend;                   /**< send function */
    GOAL_MA_ETH_CMD_T fCmd;                     /**< ethernet cmd function */
    GOAL_MA_ETH_CB_LIST_T *pCbList;             /**< list of registered cb */
    GOAL_MA_ETH_CFG_T cfg;                      /**< config */
    void *pMiInst;                              /**< belonging mi */
    void *pData;                                /**< reserved data */
    GOAL_BOOL_T flgOpen;                        /**< open flag */
} GOAL_MA_ETH_T;


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_maEthDrvReg(
    uint32_t id,                                /**< ID */
    GOAL_MA_ETH_T **ppMaEthHdl,                 /**< [out] MA handle ref ptr */
    GOAL_MA_ETH_INIT_T fInit,                   /**< init function */
    GOAL_MA_ETH_OPEN_T fOpen,                   /**< open function */
    GOAL_MA_ETH_SEND_T fSend,                   /**< send function */
    GOAL_MA_ETH_CMD_T fCmd,                     /**< ethernet cmd function */
    void *pData                                 /**< [in] reserved data */
);

GOAL_STATUS_T goal_maEthInit(
    GOAL_MA_ETH_T *pMaEthHdl                    /**< [in] pointer handler */
);

GOAL_STATUS_T goal_maEthOpen(
    GOAL_MA_ETH_T *pMaEthHdl                    /**< [in] pointer handler */
);

GOAL_STATUS_T goal_maEthSend(
    GOAL_MA_ETH_T *pMaEthHdl                    /**< [in] pointer handler */
);

GOAL_STATUS_T goal_maEthCmd(
    GOAL_MA_ETH_T *pMaEthHdl,                   /**< [in] pointer handler */
    uint32_t cmd,                               /**< command */
    GOAL_BOOL_T wrFlag,                         /**< write flag */
    uint32_t port,                              /**< port ID */
    void *pArg                                  /**< argument */
);

GOAL_STATUS_T goal_maEthGetById(
    GOAL_MA_ETH_T **ppMaEthHdl,                 /**< [out] handle ref ptr */
    uint32_t idMa                               /**< MA id */
);

GOAL_STATUS_T goal_maEthCfgMacAddrGet(
    GOAL_MA_ETH_T *pMaHdl,                      /**< [in,out] MAC ptr */
    uint8_t *pMacAddr                           /**< MA id */
);


/****************************************************************************/
/* Callback Prototypes */
/****************************************************************************/

GOAL_STATUS_T goal_maEthCbReg(
    GOAL_MA_ETH_T *pMaEthHdl,                   /**< [in] MA handle */
    GOAL_MA_ETH_CB_TYPE_T typeCb,               /**< cb type eg send, sendDone etc. */
    GOAL_MA_ETH_CB_T fCb                        /**< cb takes void pointer */
);

void goal_maEthRecvCb(
    GOAL_MA_ETH_T *pMaEthHdl,                   /**< [in] MA handle */
    uint32_t portIdx,                           /**< port index */
    GOAL_BUFFER_T **ppBuf                       /**< [out] frame containing buffer */
);

GOAL_STATUS_T goal_maEthSendFrameGetCb(
    GOAL_MA_ETH_T *pMaEthHdl,                   /**< [in] MA handle */
    GOAL_BUFFER_T **ppBuf                       /**< [out] buffer ref */
);

void goal_maEthSendDoneCb(
    GOAL_MA_ETH_T *pMaEthHdl,                   /**< [in] MA handle */
    GOAL_BUFFER_T **ppBuf,                      /**< [out] buffer ref */
    GOAL_BOOL_T doneFlag                        /**< succesfully send? */
);

#endif /* GOAL_MA_ETH_H */
