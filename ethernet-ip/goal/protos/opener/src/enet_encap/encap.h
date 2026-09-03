/** @file
 *
 * @brief Encapsulation Layer
 *
 * This module implements the Encapsulation Layer (CIP over TCP and UDP).
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

#ifndef ENCAP_H
#define ENCAP_H


/****************************************************************************/
/** @defgroup ENCAP OpENer Ethernet encapsulation layer
 *
 * The Ethernet encapsulation layer connects the TCP/IP layer and the CIP layer.
 */


/****************************************************************************/
/* Public Defines */
/****************************************************************************/
#define ENCAPSULATION_HEADER_LENGTH 24u
#define ENCAPSULATION_DATA_LENGTH 65511         /**< maximal size of encapsulation message */
#define ENCAP_EXPLICIT_MSG_PORT 0xAF12          /**< public default port for Explicit Messages */
#define ENCAP_IMPLICIT_MSG_PORT 0x08AE          /**< public default port for Implicit Messages */
#define ENCAP_AF_INET 0x0002
#define ENCAP_INADDR_ANY 0x00000000
#define ENCAP_INADDR_BROADCAST 0xFFFFFFFF
#define ENCAP_IP_IS_MCAST(ip) (0xE0000000 == ((ip) & 0xF0000000))

/* status codes */
#define OPENER_ENCAP_STATUS_SUCCESS                     0x0000
#define OPENER_ENCAP_STATUS_INVALID_COMMAND             0x0001
#define OPENER_ENCAP_STATUS_INSUFFICIENT_MEM            0x0002
#define OPENER_ENCAP_STATUS_INCORRECT_DATA              0x0003
#define OPENER_ENCAP_STATUS_INVALID_SESSION_HANDLE      0x0064
#define OPENER_ENCAP_STATUS_INVALID_LENGTH              0x0065
#define OPENER_ENCAP_STATUS_UNSUPPORTED_PROTOCOL        0x0069


/* define Item ID numbers used for address and data items in CPF structures */
#define CIP_ITEM_ID_NULL                                0x0000  /* Null Address Item */
#define CIP_ITEM_ID_LISTIDENTITY_RESPONSE               0x000C
#define CIP_ITEM_ID_CONNECTIONBASED                     0x00A1  /* Connected Address Item */
#define CIP_ITEM_ID_CONNECTIONTRANSPORTPACKET           0x00B1  /* Connected Data Item */
#define CIP_ITEM_ID_UNCONNECTEDMESSAGE                  0x00B2  /* Unconnected Data Item */
#define CIP_ITEM_ID_LISTSERVICE_RESPONSE                0x0100
#define CIP_ITEM_ID_SOCKADDRINFO_O_TO_T                 0x8000  /* Sockaddr info item originator to target (data) */
#define CIP_ITEM_ID_SOCKADDRINFO_T_TO_O                 0x8001  /* Sockaddr info item target to originator (data) */
#define CIP_ITEM_ID_SEQUENCEDADDRESS                    0x8002  /* Sequenced Address item */

#define ENCAP_OFFSET_MRRS_CONN 50               /**< start of MR response data in connected Explicit Message */
#define ENCAP_OFFSET_MRRS_UNCONN 44             /**< start of MR response data in unconnected Explicit Message */

#define ENCAP_OFFSET_COMM_BUFFER_POS (16 + SENDER_CONTEXT_SIZE) /**< offset to comm buffer */


/****************************************************************************/
/* Public functions */
/****************************************************************************/
EIP_STATUS encap_init(
    EIP_INSTANCE_T *pEip                        /**< EtherNet/IP instance */
);

void encap_shutdown(
    EIP_INSTANCE_T *pEip                        /**< EtherNet/IP instance */
);

EIP_STATUS encap_sessionOpen(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_SOCKT_T sock                            /**< socket in open state */
);

EIP_STATUS encap_sessionClose(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_SOCKT_T sock                            /**< socket belonging to session */
);

EIP_STATUS encap_sessionCloseRemote(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_SOCKT_T sock                            /**< socket belonging to session */
);

void encap_sessionCloseAll(
    EIP_INSTANCE_T *pEip                        /**< EtherNet/IP instance */
);

void encap_timeoutDeactivate(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_SOCKT_T sock                            /**< socket for a class 3 connection */
);

void encap_timeoutReactivate(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_SOCKT_T sock                            /**< socket for a class 3 connection */
);

void encap_timerLoop(
    EIP_INSTANCE_T *pEip                        /**< EtherNet/IP instance */
);

void ecap_delayedMessageCheck(
    EIP_INSTANCE_T *pEip                        /**< EtherNet/IP instance */
);

int encap_receivedExplictData(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_SOCKT_T rxSock,                         /**< rx socket */
    EIP_UINT8 *pRxBuf,                          /**< encapsulated data */
    unsigned int rxLen,                         /**< length of encapsulated data */
    int *pBytesRemain,                          /**< unprocessed data in pRxBuf */
    S_CIP_Net_Addr *pAddr,                      /**< remote address */
    EIP_BOOL8 isTCP                             /**< received via TCP, otherwise UDP */
);

EIP_STATUS encap_receivedImplicitData(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_UINT8 * pa_pnData,                      /**< rx data */
    int pa_nDataLength,                         /**< rx data length */
    S_CIP_Net_Addr *pa_pstFromAddr,             /**< originator addres */
    EIP_SOCKT_T pa_nSock                        /**< consuming socket */
);

#endif /* ENCAP_H */
