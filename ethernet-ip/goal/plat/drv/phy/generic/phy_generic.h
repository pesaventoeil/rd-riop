/** @file
 *
 * @brief Generic PHY Driver
 *
 * @copyright
 * Copyright 2010-2017.
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

#ifndef GOAL_PHY_GENERIC_H
#define GOAL_PHY_GENERIC_H


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_targetPhyGenericOpen(
    GOAL_ETH_IFACE_T *pIface,                   /**< interface data */
    uint32_t maxSpeed                           /**< maximum speed of Phy in Mbit/s */
);

GOAL_STATUS_T goal_targetPhyGenericClose(
    GOAL_ETH_IFACE_T *pIface                    /**< interface data */
);

GOAL_STATUS_T goal_targetPhyGenericId(
    GOAL_BUS_T *pBus,                           /**< PHY bus */
    uint32_t addr,                              /**< bus address */
    uint32_t *pIdPhy                            /**< PHY id ref */
);

#if GOAL_CONFIG_PHY_DETECTION == 1
GOAL_STATUS_T goal_targetPhyGenericDetect(
    GOAL_ETH_IFACE_T **ppIface,                 /**< interface ptr ref */
    uint32_t *pNumPhys,                         /**< number of found PHYs */
    GOAL_BUS_T *pBus,                           /**< bus handle */
    uint32_t addrMin,                           /**< start at address */
    uint32_t addrMax                            /**< end at address */
);
#endif


#endif /* GOAL_PHY_GENERIC_H */
