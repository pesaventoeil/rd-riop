/** @file
 *
 * @brief Message Router
 *
 * This module implements the Message Router.
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

#ifndef CIPMESSAGEROUTER_H
#define CIPMESSAGEROUTER_H


/****************************************************************************/
/* Public defines */
/****************************************************************************/
#define CIP_MESSAGE_ROUTER_CLASS_CODE 0x02


/****************************************************************************/
/* Public functions */
/****************************************************************************/
EIP_STATUS eip_mrInit(
    EIP_INSTANCE_T *pEip                        /**< EtherNet/IP instance */
);

EIP_STATUS eip_mrReceive(
    EIP_INSTANCE_T *pEip                        /**< EtherNet/IP instance */
);

EIP_STATUS eip_mrVirtualClassRegister(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_UINT16 classId,                         /**< Id of virtual class */
    TCIPVirtClassHandlerFunc pFunc              /**< request handler */
);

EIP_CIP_CLASS_T *eip_mrCipClassGetById(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_UINT32 classId                          /**< class Id */
);

void eip_mrCipClassRegister(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_CIP_CLASS_T *pClass                     /**< CIP class to be registered */
);


#endif /* CIPMESSAGEROUTER_H */
