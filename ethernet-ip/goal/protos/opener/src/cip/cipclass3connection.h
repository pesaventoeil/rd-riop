/** @file
 *
 * @brief Class 3 Connections
 *
 * This module handles Class 3 connections (COnnected Explicit Messages).
 *
 * @copyright
 * This software consists of the OpENer by Rockwell Automation, Inc. and
 * extensive fixes, changes and improvements by port GmbH.
 *
 * Copyright (c) 2019, port GmbH for all fixes, changes and improvements in
 * this software, added to the original, unchanged OpENer
 *
 * Copyright (c) 2009, Rockwell Automation, Inc. for the original, unchanged
 * OpENer source code
 *
 * All rights reserved.
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

#ifndef CIPCLASS3CONNECTION_H_
#define CIPCLASS3CONNECTION_H_


/****************************************************************************/
/* Public functions */
/****************************************************************************/
EIP_STATUS eip_explConnectionsInit(
    EIP_INSTANCE_T *pEip                        /**< EtherNet/IP instance */
);

EIP_UINT8 eip_explConnectionOpen(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    struct CIP_ConnectionObject *pNewCon,       /**< data of new connection */
    EIP_UINT16 *pExtError                       /**< extended error code */
);


#endif /* CIPCLASS3CONNECTION_H_ */
