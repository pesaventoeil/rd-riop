/** @file
 *
 * @brief OpENer API
 *
 * This module contains all public functions that can be used by an application.
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


#ifndef CIP_API_H
#define CIP_API_H


/**  @defgroup CIP_CALLBACK_API Callback Functions Demanded by OpENer
 * @ingroup CIP_API
 *
 * These functions have to implemented in order to give the OpENer a
 * method to inform the application on certain state changes.
 */

/** Callback for the application initialization
 *
 * @ingroup CIP_CALLBACK_API
 *
 * This function will be called by the CIP stack after it has finished its
 * initialization. In this function the user can setup all CIP objects she
 * likes to have.
 *
 * This function is provided for convenience reasons. After the void CIP_Init(void)
 * function has finished it is okay to also generate your CIP objects.
 *  return status EIP_ERROR .. error
 *                EIP_OK ... successful finish
 */
EIP_STATUS IApp_Init(
    EIP_INSTANCE_T *pEip                        /**< EtherNet/IP instance */
);

/** Inform the application on changes occurred for a connection
 *
 * @ingroup CIP_CALLBACK_API
 */
void IApp_IOConnectionEvent(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    unsigned int pa_unOutputAssembly,           /**< output assembly connection point */
    unsigned int pa_unInputAssembly,            /**< input assembly connection point */
    EIOConnectionEvent pa_eIOConnectionEvent    /**< event ID */
);

/** Call back function to inform application on received data for an assembly object.
 *
 * @ingroup CIP_CALLBACK_API
 *
 * This function has to be implemented by the user of the CIP-stack.
 * @return Information if the data could be processed
 *     - EIP_OK the received data was ok
 *     - EIP_ERROR the received data was wrong (especially needed for configuration data assembly
 *                 objects)
 *
 * Assembly Objects for Configuration Data:
 * The CIP-stack uses this function to inform on received configuration data. The length of the data
 * is already checked within the stack. Therefore the user only has to check if the data is valid.
 */
EIP_STATUS IApp_AfterAssemblyDataReceived(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_UINT16 instanceId,                      /**< instance ID */
    S_CIP_Byte_Array *pData                     /**< assembly data */
);

/** Inform the application that the data of an assembly object will be sent
 *
 * @ingroup CIP_CALLBACK_API
 *
 * Within this function the user can update the data of the assembly object before it
 * gets sent. The application can inform the application if data has changed.
 *
 * @return data has changed:
 *          - true assembly data has changed
 *          - false assembly data has not changed
 */
EIP_BOOL8 IApp_BeforeAssemblyDataSend(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_UINT16 instanceId,                      /**< instance ID */
    void *pData                                 /**< attribute data */
);

/** Inform Application that device will be reset
 *
 * @ingroup CIP_CALLBACK_API
 */
EIP_STATUS IApp_ResetDevice(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EResetState pa_enResetState                 /**< type of reset */
);

/** Allocate memory for the cip stack
 *
 * @ingroup CIP_CALLBACK_API
 *
 * emulate the common c-library function calloc
 * In OpENer allocation only happens on application startup and on class/instance creation
 * and configuration not on during operation (processing messages)
 *
 * return pointer to the allocated memory, 0 on error
 */
void *IApp_CipCalloc(
    unsigned pa_nNumberOfElements,              /**< number of elements to allocate */
    unsigned pa_nSizeOfElement                  /**< size in bytes of one element */
);


/** Free memory allocated by the OpENer
 *
 * @ingroup CIP_CALLBACK_API
 *
 * emulate the common c-library function free
 * return
 */
void IApp_CipFree(
    void *pa_poData                             /**< pointer to the allocated memory */
);

/** Inform the application that the Run/Idle State has been changed by the originator.
 *
 * @ingroup CIP_CALLBACK_API
 */
void IApp_RunIdleChanged(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    unsigned int pa_unOutputAssembly,           /**< output assembly connection point */
    unsigned int pa_unInputAssembly,            /**< input assembly connection point */
    EIP_UINT32 pa_nRunIdleValue                 /**< current value of the run/idle flag */
);

/** Set the TTL value of produced UDP packets
 *
 * @ingroup CIP_CALLBACK_API
 *
 * @retval EIP_OK success
 * @retval EIP_ERROR error
 */
EIP_STATUS IApp_UdpSetTTL(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_SOCKT_T pa_sockfd,                      /**< UDP socket */
    EIP_UINT8 pa_TTLValue                       /**< TTL value */
);


/** Close the given socket and clean up the stack
 *
 * @ingroup CIP_CALLBACK_API
 */
void IApp_CloseSocket(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_SOCKT_T pa_nSockFd                      /**< socket descriptor to close */
);


/** Check if Minor Revision is acceptable
 *
 * @ingroup CIP_CALLBACK_API
 *
 * If the compatibility bit is set in the Electronic Key Segment the application
 * can decide whether a Minor Revision is supported or not.
 *
 * @retval EIP_OK Revision is supported
 * @retval EIP_ERROR Revision is not supported
 */
EIP_STATUS IApp_CheckMinorRevision(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_UINT8 minorRevision                     /**< requested Minor Revision */
);


void IApp_LedEvent(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_UINT32 leds                             /**< active LEDs */
);

void opener_nwhMcastGroupLeave(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_SOCKT_T pa_sockfd,                      /**< UDP socket */
    EIP_UINT32 mcastAddr                        /**< IP multicast address */
);

EIP_STATUS opener_nwhMcastGroupJoin(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_SOCKT_T pa_sockfd,                      /**< UDP socket */
    EIP_UINT32 mcastAddr                        /**< IP multicast address */
);

EIP_SOCKT_T opener_nwhIoSocketGet(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_UINT8 priority                          /**< priority of the connection */
);

EIP_STATUS opener_nwhTcpSockPrepare(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_UINT32 numSessions                      /**< number of Encapsulation sessions */
);

EIP_STATUS opener_nwhTcpSockDestroy(
    EIP_INSTANCE_T *pEip                        /**< EtherNet/IP instance */
);

#endif /* CIP_API_H */

