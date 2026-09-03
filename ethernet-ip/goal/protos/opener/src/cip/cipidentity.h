/** @file
 *
 * @brief Identity class
 *
 * This module implements the Identity class.
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

#ifndef CIPIDENTITY_H
#define CIPIDENTITY_H


/****************************************************************************/
/* Public defines */
/****************************************************************************/
/** Class Id */
#define CIP_IDENTITY_CLASS_CODE 0x01

/** Status attribute values */
#define CIP_IDENTITY_STATUS_OWNED 0x0001
#define CIP_IDENTITY_STATUS_CONFIGURED 0x0004
#define CIP_IDENTITY_STATUS_MINOR_RECOV_FLT 0x0100
#define CIP_IDENTITY_STATUS_MINOR_UNRECOV_FLT 0x0200
#define CIP_IDENTITY_STATUS_MAJOR_RECOV_FLT 0x0400
#define CIP_IDENTITY_STATUS_MAJOR_UNRECOV_FLT 0x0800

/** Extended Status attribute values */
#define CIP_IDENTITY_EXTENDED_STATUS_SELFTESTING_UNKNOWN                                     0x0000
#define CIP_IDENTITY_EXTENDED_STATUS_FIRMEWARE_UPDATE_IN_PROGRESS                            0x0010
#define CIP_IDENTITY_EXTENDED_STATUS_AT_LEAST_ONE_FAULTED_IO_CONNECTION                      0x0020
#define CIP_IDENTITY_EXTENDED_STATUS_NO_IO_CONNECTIONS_ESTABLISHED                           0x0030
#define CIP_IDENTITY_EXTENDED_STATUS_NON_VOLATILE_CONFIGURATION_BAD                          0x0040
#define CIP_IDENTITY_EXTENDED_STATUS_MAJOR_FAULT                                             0x0050
#define CIP_IDENTITY_EXTENDED_STATUS_AT_LEAST_ONE_IO_CONNECTION_IN_RUN_MODE                  0x0060
#define CIP_IDENTITY_EXTENDED_STATUS_AT_LEAST_ONE_IO_CONNECTION_ESTABLISHED_ALL_IN_IDLE_MODE 0x0070


/****************************************************************************/
/* Public functions */
/****************************************************************************/
EIP_STATUS eip_idInit(
    EIP_INSTANCE_T *pEip                        /**< EtherNet/IP instance */
);

void eip_idShutdown(
    EIP_INSTANCE_T *pEip                        /**< EtherNet/IP instance */
);

void eip_idIdentityGet(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    S_CIP_IdentityAtributes *pIdAttributes      /**< [out] ID attributes buffer */
);

EIP_STATUS eip_idIdentitySet(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    S_CIP_IdentityAtributes *pIdAttributes      /**< [in] new ID attributes */
);

void eip_idDeviceStatusSet(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_UINT16 newStatus                        /**< new device status */
);

#endif /* CIPIDENTITY_H */
