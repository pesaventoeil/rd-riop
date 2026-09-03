/** @file
 *
 * @brief Class 1 Connections
 *
 * This module handles CLass 1 connections (Implicit Messages).
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

#ifndef CIPIOCONNECTION_H_
#define CIPIOCONNECTION_H_


/****************************************************************************/
/* Public functions */
/****************************************************************************/
EIP_UINT8 eip_ioConnctionOpen(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    struct CIP_ConnectionObject *pNewCon,       /**< new IO connection */
    EIP_UINT16 *pExtError                       /**< extended error code buffer */
);

EIP_UINT16 eip_ioConfigDataReceived(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_CIP_CLASS_T *pAsmClass,                 /**< CIP Assembly Class */
    S_CIP_ConnectionObject *pConObj             /**< new IO connection */
);

EIP_STATUS eip_ioExclusiveOwnerConnectionAdd(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    unsigned int outputAsm,                     /**< output Assembly (consuming endpoint) */
    unsigned int inputAsm,                      /**< input assembly (producing endpoint) */
    unsigned int cfgAsm                         /**< configuration assembly (configuration endpoint) */
);

EIP_STATUS eip_ioInputOnlyConnectionAdd(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    unsigned int numCon,                        /**< number of connections per application path */
    unsigned int outputAsm,                     /**< output Assembly (consuming endpoint) */
    unsigned int inputAsm,                      /**< input assembly (producing endpoint) */
    unsigned int cfgAsm                         /**< configuration assembly (configuration endpoint) */
);

EIP_STATUS eip_ioListenOnlyConnectionAdd(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    unsigned int numCon,                        /**< number of connections per application path */
    unsigned int outputAsm,                     /**< output Assembly (consuming endpoint) */
    unsigned int inputAsm,                      /**< input assembly (producing endpoint) */
    unsigned int cfgAsm                         /**< configuration assembly (configuration endpoint) */
);

void eip_ioShutdown(
    EIP_INSTANCE_T *pEip                        /**< EtherNet/IP instance */
);

EIP_BOOL8 eip_ioProducingMcastActiveCheck(
    EIP_INSTANCE_T *pEip                        /**< EtherNet/IP instance */
);

#endif /* CIPIOCONNECTION_H_ */
