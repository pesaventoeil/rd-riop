/** @file
 *
 * @brief  CTC wrapper for DD specific functions
 *
 * @details
 * This module provides a RPC functions
 *
 * @copyright
 * Copyright 2020 port GmbH Halle/Saale.
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
#ifndef GOAL_DD_RPC_H_
#define GOAL_DD_RPC_H_


/****************************************************************************/
/* Defines */
/****************************************************************************/
#define RPC_BUFF_SIZE_DD (64)

#define GOAL_RPC_FUNC_DD_NEW (1)
#define GOAL_RPC_FUNC_DD_CFG_CUSTOMER_ID (2)
#define GOAL_RPC_FUNC_DD_CFG_MODULE_NAME (3)
#define GOAL_RPC_FUNC_DD_FEATURES_SET (4)
#define GOAL_RPC_FUNC_DD_SESSION_FEATURES_ACTIVATE (5)
#define GOAL_RPC_FUNC_DD_CB (6)
#define GOAL_RPC_FUNC_DD_FILTER_ADD (7)
#define GOAL_RPC_FUNC_DD_CRYPT_ACTIVATION_SET (8)
#define GOAL_RPC_FUNC_DD_CRYPT_PASSWORD_SET (9)

#endif /* GOAL_DD_RPC_H */
