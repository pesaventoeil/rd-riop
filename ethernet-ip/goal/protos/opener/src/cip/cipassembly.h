/** @file
 *
 * @brief Assembly Class
 *
 * This module implements the Assembly class.
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

#ifndef CIPASSEMBLY_H
#define CIPASSEMBLY_H


/****************************************************************************/
/* Public defines */
/****************************************************************************/
#define CIP_ASSEMBLY_CLASS_CODE 0x04            /**< CIP class ID for Assembly objects */
#define CIP_ASSEMBLY_ATTR_ID_DATA 0x03          /**< CIP Assembly Attribute for Data */


/****************************************************************************/
/* Public functions */
/****************************************************************************/
EIP_STATUS eip_asmInit(
    EIP_INSTANCE_T *pEip                        /**< EtherNet/IP instance */
);

void eip_asmShutdown(
    EIP_INSTANCE_T *pEip                        /**< EtherNet/IP instance */
);

EIP_CIP_INSTANCE_T *eip_asmInstanceCreate(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_UINT32 instanceId,                      /**< number of new instance */
    EIP_BYTE *pData,                            /**< assembly data buffer (attribute 3) */
    EIP_UINT16 dataLen                          /**< size of assembly data buffer */
);

EIP_STATUS eip_asmDataUpdate(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_CIP_INSTANCE_T *pInstance,              /**< CIP instance handle */
    EIP_UINT8 *pData,                           /**< received data */
    EIP_UINT16 dataLen                          /**< length of received data */
);

#endif /* CIPASSEMBLY_H */
