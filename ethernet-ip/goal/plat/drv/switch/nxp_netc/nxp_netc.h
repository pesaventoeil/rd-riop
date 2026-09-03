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

#ifndef GOAL_NXP_NETC_H
#define GOAL_NXP_NETC_H


/****************************************************************************/
/* Configuration */
/****************************************************************************/


/****************************************************************************/
/* Global variables */
/****************************************************************************/


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_targetSwitchNXPNetcReg(
    GOAL_DRV_ETH_NETC_PORT_INFO_T *pPortInfo,   /**< port info data table */
    uint32_t numEntries                         /**< entries in port info data table */
);

GOAL_STATUS_T goal_targetSwitchNXPNetcOpen(
    GOAL_MA_ETH_T *pMaEth,                      /**< ethernet ma handle */
    void *pSwitchHandle,                        /**< bsp switch handle */
    GOAL_ETH_IFACE_T *pIfaceSwitch              /**< interface handle */
);

GOAL_STATUS_T goal_targetSwitchNXPNetcClose(
    GOAL_MA_ETH_T *pMaEth                       /**< ethernet ma handle */
);

GOAL_STATUS_T goal_targetSwitchNXPNetcCmdHandler(
    GOAL_ETH_CMD_DATA_T *pCmdData               /**< command handler */
);

#endif /* GOAL_NXP_NETC_H */
