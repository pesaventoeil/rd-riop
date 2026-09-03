/** @file
 *
 * @brief Generic MA for NET
 *
 * @details
 * This module provides a MA for NET
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
#ifndef GOAL_MA_NET_H
#define GOAL_MA_NET_H


/****************************************************************************/
/* Forward declarations */
/****************************************************************************/
struct GOAL_MA_NET_T;                           /**< MA NET handle */
struct GOAL_MI_NET_T;                           /**< MI NET handle */


/****************************************************************************/
/* Typedefs */
/****************************************************************************/
/** network receive callback */
typedef void (*GOAL_MA_NET_CB_T)(
    struct GOAL_MA_NET_T *pMaNet,               /**< net ma handle */
    GOAL_NET_CB_TYPE_T cbType,                  /**< callback type */
    struct GOAL_NET_CHAN_T *pChan,              /**< channel descriptor */
    struct GOAL_BUFFER_T *pBuf                  /**< GOAL buffer */
);

/**< open function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_NET_OPEN) (
    struct GOAL_MA_NET_T *pHdlMaNet             /**< MA handle for NET  */
);

/**< close function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_NET_CLOSE) (
    struct GOAL_MA_NET_T *pHdlMaNet             /**< MA handle for NET  */
);

/**< net ip set function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_NET_IP_SET) (
    struct GOAL_MA_NET_T *pHdlMaNet,            /**< MA handle for NET  */
    uint32_t addrIp,                            /**< IP address */
    uint32_t addrMask,                          /**< subnet mask */
    uint32_t addrGw,                            /**< gateway */
    GOAL_BOOL_T flgTemp                         /**< temporary IP config flag */
);

/**< net ip get function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_NET_IP_GET) (
    struct GOAL_MA_NET_T *pHdlMaNet,            /**< MA handle for NET  */
    uint32_t *pAddrIp,                          /**< IP address */
    uint32_t *pAddrMask,                        /**< subnet mask */
    uint32_t *pAddrGw,                          /**< gateway */
    GOAL_BOOL_T *pFlgTemp                       /**< temporary IP config flag */
);

/**< net command function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_NET_CMD) (
    struct GOAL_MA_NET_T *pHdlMaNet,            /**< MA handle for NET  */
    GOAL_NET_CMD_T id,                          /**< command ID */
    GOAL_BOOL_T wrFlag,                         /**< write flag */
    void *pArg                                  /**< argument */
);

/**< NET handle instance struct */
typedef struct GOAL_MA_NET_T {
    struct GOAL_MA_NET_T *pNext;                /**< next entry */
    uint32_t id;                                /**< MA ID */
    uint32_t idEthMi;                           /**< ID of corresponding Ethernet interface */
    struct GOAL_MI_NET_T *pMiNet;               /**< MI Net handle */
    GOAL_BOOL_T flgUsed;                        /**< used flag */
    GOAL_MA_NET_OPEN fctOpen;                   /**< open NET function */
    GOAL_MA_NET_IP_SET fctSetIp;                /**< ip set function */
    GOAL_MA_NET_IP_GET fctGetIp;                /**< ip get function */
    GOAL_MA_NET_CMD fctCmd;                     /**< execute net command */
    GOAL_MA_NET_CLOSE fctClose;                 /**< close NET function */
    void *pDrvData;                             /**< driver specific data */
} GOAL_MA_NET_T;


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_maNetDrvReg(
    uint32_t id,                                /**< ID */
    GOAL_MA_NET_T **ppHdlMaNet,                 /**< NET handle ref ptr */
    GOAL_MA_NET_OPEN fOpen,                     /**< open function */
    GOAL_MA_NET_IP_SET fSetIp,                  /**< ip set function */
    GOAL_MA_NET_IP_GET fGetIp,                  /**< ip Get function */
    GOAL_MA_NET_CMD fCmd,                       /**< net command function */
    GOAL_MA_NET_CLOSE fClose,                   /**< close function */
    void *pDrvData                              /**< driver specific data */
);

GOAL_STATUS_T goal_maNetOpen(
    GOAL_MA_NET_T *pNetHdl                      /**< NET MA Handle */
);

GOAL_STATUS_T goal_maNetClose(
    GOAL_MA_NET_T *pNetHdl                      /**< pointer to store NET handler */
);

GOAL_STATUS_T goal_maNetGetById(
    GOAL_MA_NET_T **ppHdlMaNet,                 /**< NET handle ref ptr */
    uint32_t id                                 /**< MA id */
);

GOAL_STATUS_T goal_maNetIpSet(
    GOAL_MA_NET_T *pNetHdl,                     /**< pointer to store NET handler */
    uint32_t addrIp,                            /**< IP address */
    uint32_t addrMask,                          /**< subnet mask */
    uint32_t addrGw,                            /**< gateway */
    GOAL_BOOL_T flgTemp                         /**< temporary IP config flag */
);

GOAL_STATUS_T goal_maNetIpGet(
    GOAL_MA_NET_T *pNetHdl,                     /**< pointer to store NET handler */
    uint32_t *pAddrIp,                          /**< IP address */
    uint32_t *pAddrMask,                        /**< subnet mask */
    uint32_t *pAddrGw,                          /**< gateway */
    GOAL_BOOL_T *pFlgTemp                       /**< temporary IP config flag */
);

GOAL_STATUS_T goal_maNetCmd(
    GOAL_MA_NET_T *pNetHdl,                     /**< pointer to store NET handler */
    GOAL_NET_CMD_T id,                          /**< command ID */
    GOAL_BOOL_T wrFlag,                         /**< write flag */
    void *pArg                                  /**< argument */
);

#endif /* GOAL_MA_NET_H */
