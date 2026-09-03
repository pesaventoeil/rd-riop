/** @file
 *
 * @brief NETC DLR driver
 *
 * This module configures the NETC to handle DLR Beacon frames.
 *
 * @copyright
 * Copyright 2010-2023 port GmbH.
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

#ifndef DLR_NXP_NETC_H
#define DLR_NXP_NETC_H


/****************************************************************************/
/* Public functions */
/****************************************************************************/
GOAL_STATUS_T goal_drvDlrNetcInit(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData          /**< private driver data */
);

GOAL_STATUS_T goal_drvDlrNetcOpen(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData          /**< private driver data */
);

void goal_drvDlrNetcLinkChangeExec(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    uint32_t port,                              /**< port ID */
    uint16_t linkState                          /**< new link state */
);

GOAL_STATUS_T goal_drvDlrNetcCmdExec(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

void goal_drvDlrNetcRxHandler(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    uint8_t rxRingNum,                          /**< RxBDR ID */
    uint32_t frameLen                           /**< frame length */
);

void goal_drvDlrNetcRxIrqExec(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData          /**< private driver data */
);

#endif /* DLR_NXP_NETC_H */
