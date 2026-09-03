/** @file
 *
 * @brief Implementation of the QoS Object
 *
 * This module defines the QoS Object for EtherNet/IP. It implements the
 * attributes and services used to set the DSCP value in the IP Header or
 * activate VLAN tagging.
 *
 *
 * @copyright
 * Copyright 2010-2019 port GmbH Halle/Saale.
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

#ifndef CIP_QOS_H
#define CIP_QOS_H


/****************************************************************************/
/* public definitions */
/****************************************************************************/
/* initial value of QoS attributes according to EIP Spec. Vol.2, 3-7.5 */
#define CIP_QOS_DEFAULT_VLANENABLE      0
#define CIP_QOS_DEFAULT_DSCPURGENT     55
#define CIP_QOS_DEFAULT_DSCPSCHEDULED  47
#define CIP_QOS_DEFAULT_DSCPHIGH       43
#define CIP_QOS_DEFAULT_DSCPLOW        31
#define CIP_QOS_DEFAULT_DSCEXPLICIT    27

/* Priority values */
#define QOS_PRIORITY_LOW                      0 /**< priority: low */
#define QOS_PRIORITY_HIGH                     1 /**< priority: high */
#define QOS_PRIORITY_SCHEDULED                2 /**< priority: scheduled */
#define QOS_PRIORITY_URGENT                   3 /**< priority: urgent */
#define QOS_PRIORITY_OTHER                    4 /**< priority: other */


/****************************************************************************/
/* public functions */
/****************************************************************************/
EIP_STATUS eip_qosInitInit(
    EIP_INSTANCE_T *pEip                        /**< EtherNet/IP instance */
);

EIP_UINT8 eip_qosDscpValGet(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_UINT8 pa_nPriority                      /**< priority of the connection */
);

void eip_qosAttributesActiveSet(
    EIP_INSTANCE_T *pEip                        /**< EtherNet/IP instance */
);

#endif /* CIP_QOS_H */
