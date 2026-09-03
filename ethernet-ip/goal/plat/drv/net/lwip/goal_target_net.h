/** @file
 *
 * @brief Low Level Interface for GOAL Network Handling
 *
 * This module allows the usage of the BSD socket API in GOAL.
 *
 * @copyright
 * Copyright 2010-2021 port GmbH Halle/Saale.
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


#ifndef GOAL_TARGET_NET_H
#define GOAL_TARGET_NET_H

#include "lwip/sockets.h"


/****************************************************************************/
/* Public prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_drvNetLwipReg(
    uint32_t idNetMa,                           /**< Net MA ID */
    uint32_t idEthMi                            /**< Eth MI ID */
);

#if GOAL_FEAT_LWIP_INIT == 0
GOAL_STATUS_T goal_drvLwipNetIfHdlSet(
    uint32_t idMa,                              /**< MA ID */
    struct netif *pNetIfNew                     /**< lwIP iface handle */
);
#endif /* !GOAL_FEAT_LWIP_INIT */

#endif /* GOAL_TARGET_NET_H */
