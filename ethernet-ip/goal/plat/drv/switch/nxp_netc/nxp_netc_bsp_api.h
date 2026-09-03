/** @file
 *
 * @brief NXP NETC Switch Driver BSP glue code
 *
 * @copyright
 * Copyright 2022 port GmbH Halle/Saale.
 * This software is protected Intellectual Property and may only be used
 * according to the license agreement.
 */

GOAL_STATUS_T netcOpen(
    NETC_BSP_INST_T **ppNetcBSP,                /**< driver instance */
    void *pSwitchHandle                         /**< switch handle */
);

GOAL_STATUS_T netcMacTabAdd(
    NETC_BSP_INST_T *pNetcBSP,                  /**< driver instance */
    uint8_t forwardPorts,                       /**< forwarding ports */
    uint8_t *pMac                               /**< MAC address */
);

GOAL_STATUS_T netcMacTabDel(
    NETC_BSP_INST_T *pNetcBSP,                  /**< driver instance */
    uint8_t *pMac                               /**< MAC address */
);

void netcFDBTableDump(
    NETC_BSP_INST_T *pNetcBSP                   /**< driver instance */
);

void netcL2MCTableDump(
    NETC_BSP_INST_T *pNetcBSP                   /**< driver instance */
);

GOAL_STATUS_T netcEplSetup(
    NETC_BSP_INST_T *pNetcBSP,                  /**< driver instance */
    uint8_t portEnableMask                      /**< port enable mask */
);

GOAL_STATUS_T netcEplPresUpdate(
    NETC_BSP_INST_T *pNetcBSP,                  /**< driver instance */
    GOAL_BUFFER_T *pBuf                         /**< buffer */
);

GOAL_STATUS_T netcEplIdentUpdate(
    NETC_BSP_INST_T *pNetcBSP,                  /**< driver instance */
    GOAL_BUFFER_T *pBuf                         /**< buffer */
);

GOAL_STATUS_T netcEplStatusUpdate(
    NETC_BSP_INST_T *pNetcBSP,                  /**< driver instance */
    GOAL_BUFFER_T *pBuf                         /**< buffer */
);

GOAL_STATUS_T netcEplUnspecUpdate(
    NETC_BSP_INST_T *pNetcBSP,                  /**< driver instance */
    GOAL_BUFFER_T *pBuf                         /**< buffer */
);

GOAL_STATUS_T netcEplEnableMode(
    NETC_BSP_INST_T *pNetcBSP                   /**< driver instance */
);

GOAL_STATUS_T netcEplDisableMode(
    NETC_BSP_INST_T *pNetcBSP                   /**< driver instance */
);

GOAL_STATUS_T netcEplAutoreplySet(
    NETC_BSP_INST_T *pNetcBSP,                  /**< driver instance */
    GOAL_BOOL_T flagActive                      /**< active flag */
);

GOAL_STATUS_T netcEplPresAutoreplySet(
    NETC_BSP_INST_T *pNetcBSP,                  /**< driver instance */
    GOAL_BOOL_T flagActive                      /**< active flag */
);

GOAL_STATUS_T netcEplUpdateNodeId(
    NETC_BSP_INST_T *pNetcBSP,                  /**< driver instance */
    uint8_t nodeId                              /**< nodeId */
);
