/** @file
 *
 * @brief Phy driver for Realtek RTL8201FI-VC-CG
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

#ifndef GOAL_PHY_RTL8201_H
#define GOAL_PHY_RTL8201_H


/****************************************************************************/
/* Public defines */
/****************************************************************************/
#define GOAL_PHY_RTL8201_REG_PAGE_SELECT     31 /**< Page select register address */
#define GOAL_PHY_RTL8201_REG_RMSR            16 /**< RMII Mode Setting Register (Page 7) */
#define GOAL_PHY_RTL8201_REG_CUSTOM_LED      17 /**< Customized LEDs Register (Page 7) */
#define GOAL_PHY_RTL8201_REG_INT_LED         19 /**< Interrupt and LED Register (Page 7) */
#define GOAL_PHY_RTL8201_REG_MIITXISO        20 /**< MII TX Isolate Register (Page 7) */

#define GOAL_PHY_RTL8201_RMSR_RXDV_SEL (1u << 2) /**< RMII Mode Setting Register: RSDV select */
#define GOAL_PHY_RTL8201_MIITXISO_HD_FIX 0x39d5 /**< MII TX Isolate Register: Fix Half Duplex links */


/****************************************************************************/
/* Public functions */
/****************************************************************************/
GOAL_STATUS_T goal_targetPhyRtl8201Open(
    GOAL_ETH_IFACE_T *pIface                    /**< interface data */
);


#endif /* GOAL_PHY_RTL8201_H */
