/** @file
 *
 * @brief CTC wrapper for DD
 *
 * @details
 * This module provided RPC functionality for DD
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
#ifndef GOAL_DD_RPC_CC_H_
#define GOAL_DD_RPC_CC_H_


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_ddRpcInit(
    void
);

#define goal_ddInit goal_ddRpcInit
#define goal_ddNew goal_ddRpcNew

#endif /* GOAL_DD_RPC_CC_H */
