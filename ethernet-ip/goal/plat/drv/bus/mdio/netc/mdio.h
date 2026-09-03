/** @file
 *
 * @brief NETC MDIO bus handler
 *
 * This module implements a bus handler for NETC MDIO access.
 *
 * @copyright
 * Copyright 2010-2022 port GmbH.
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

#ifndef GOAL_DRV_MDIO_NETC_H
#define GOAL_DRV_MDIO_NETC_H


/****************************************************************************/
/* Public functions */
/****************************************************************************/
GOAL_STATUS_T goal_drvBusMdioNetcOpen(
    GOAL_BUS_T *pBus                            /**< bus handle */
);


#endif /* GOAL_DRV_MDIO_NETC_H */
