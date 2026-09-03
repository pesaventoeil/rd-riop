/** @file
 *
 * @brief  CTC wrapper for net
 *
 * @details
 * This module provides a MA for networking over CTC
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
#ifndef PLAT_DRV_CTC_NET_CC_H_
#define PLAT_DRV_CTC_NET_CC_H_

#include <plat/drv/ctc/ctc_net_rpc.h>


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_netRpcInit(
    uint32_t id                                 /**< id for MA instance */
);

#endif /* PLAT_DRV_CTC_NET_CC_H_ */
