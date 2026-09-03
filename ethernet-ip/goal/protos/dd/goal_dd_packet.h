/** @file
 *
 * @brief Header for device detection packets
 *
 * @copyright
 * Copyright 2013-2021 port GmbH Halle/Saale.
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


#ifndef GOAL_DD_PACKET_H
#define GOAL_DD_PACKET_H


/****************************************************************************/
/* Data Types */
/****************************************************************************/

/**< Base meta packet structure */
struct Packet {
    uint32_t command;                           /**< command */
    GOAL_BOOL_T isBroadcast;                    /**< broadcast flag for response */
    uint8_t *payload;                           /**< request payload buffer */
    uint16_t payloadLength;                     /**< request payload buffer length */
};

/**< TLV structure */
struct TLV {
    uint16_t type;                              /**< type of TLV */
    uint16_t length;                            /**< data length */
    uint8_t *data;                              /**< data */
};


/****************************************************************************/
/* Public functions */
/****************************************************************************/
void dd_readPacket(
    struct Packet *pPacket,                     /**< target packet description */
    uint8_t *pBuffer,                           /**< packet data */
    uint16_t size                               /**< packet size */
);

uint8_t *dd_tlvToPacket(
    struct TLV *pTlv,                           /**< pointer to TLV */
    uint8_t *pos                                /**< destination pointer */
);

uint8_t *dd_getTlvByType(
    int type,                                   /**< TLV type to search */
    struct Packet *packet,                      /**< packet to scan */
    struct TLV *pTlv                            /**< return TLV */
);

GOAL_BOOL_T dd_checkCustomerID(
    uint32_t tlv_cid,                           /**< check value */
    uint32_t customerID                         /**< customer ID */
);

uint8_t *dd_writeCustomerID(
    uint8_t *wPtr,                              /**< buffer write pointer */
    uint32_t customerID                         /**< customer ID */
);

#endif /* GOAL_DD_PACKET_H */
