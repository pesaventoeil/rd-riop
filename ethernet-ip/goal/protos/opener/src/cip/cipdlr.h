/** @file
 *
 * @brief Implementation of the DLR Object
 *
 * This module defines the DLR Object for EtherNet/IP. It implements the
 * attributes and services used to access a DLR Stack via EtherNet/IP.
 *
 * @copyright
 * Copyright 2010-2023 port GmbH Halle/Saale.
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

#ifndef CIP_DLR_H
#define CIP_DLR_H

#if GOAL_DLR == 1
/****************************************************************************/
/* Public functions */
/****************************************************************************/
EIP_STATUS CIP_DLR_Init(
    EIP_INSTANCE_T *pEip                        /**< EtherNet/IP instance */
);
#endif /* GOAL_DLR */

#endif /* CIP_DLR_H */
