/** @file
 *
 * @brief Phy driver for Realtek RTL8211FDI-CG
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

#ifndef GOAL_PHY_RTL8211_H
#define GOAL_PHY_RTL8211_H


/****************************************************************************/
/* Public functions */
/****************************************************************************/
GOAL_STATUS_T goal_targetPhyRtl8211Open(
    GOAL_ETH_IFACE_T *pIface                    /**< interface data */
);


#endif /* GOAL_PHY_RTL8211_H */
