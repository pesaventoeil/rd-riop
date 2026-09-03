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
#ifndef PLAT_DRV_CTC_NET_RPC_H_
#define PLAT_DRV_CTC_NET_RPC_H_


/****************************************************************************/
/* Defines */
/****************************************************************************/
#define RPC_BUFF_SIZE_NET (GOAL_NETBUF_SIZE + 64)
#define GOAL_NET_ID_DEFAULT (0)

#define GOAL_RPC_FUNC_NET_CHAN_CB (1)
#define GOAL_RPC_FUNC_NET_CHAN_ACTIVATE (2)
#define GOAL_RPC_FUNC_NET_IP_SET (3)
#define GOAL_RPC_FUNC_NET_CHAN_NEW (4)
#define GOAL_RPC_FUNC_NET_CHAN_CLOSE (5)
#define GOAL_RPC_FUNC_NET_CHAN_SEND (6)
#define GOAL_RPC_FUNC_NET_SET_OPTION (7)
#define GOAL_RPC_FUNC_NET_CHAN_GET_REMOTE_ADDR (8)
#define GOAL_RPC_FUNC_NET_CHAN_REOPEN (9)

#define CHAN_UDP (1)
#define CHAN_TCP (2)

#endif /* PLAT_DRV_CTC_NET_RPC_H_ */
