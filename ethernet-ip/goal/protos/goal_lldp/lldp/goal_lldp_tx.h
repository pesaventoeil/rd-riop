/** @file
 *
 * @brief GOAL LLDP Tx
 *
 * This module implements sending mechanism of the GOAL LLDP implementation.
 *
 * The start of sending LLDP frames is triggered by a timer populated with
 * the Message Tx interval in seconds. If it is started cyclically it is tried
 * to send a LLDP frame on the next port, until on all active ports one frame was sent.
 *
 * The TLV data to send are stored directly in a prepared LLDP frame.
 * Directly before sending for all stored TLVs a TLV callback is send, allowing
 * to change data. If the length will be not changed, the data in the callback
 * can be modified directly, otherweise the corresponding setter API must be used.
 *
 * @copyright
 * Copyright 2023 port GmbH
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


#ifndef GOAL_LLDP_TX_H
#define GOAL_LLDP_TX_H


/****************************************************************************/
/* Data Types */
/****************************************************************************/
/**< Tx TLV list entry */
typedef struct GOAL_LLDP_TX_TLV_LIST_T {
    struct GOAL_LLDP_TX_TLV_LIST_T *pNext;      /**< next entry */
    uint8_t tlvId;                              /**< TLV id */
    uint32_t tlvOuid;                           /**< TLV OUID */
    uint8_t tlvSubtypeId;                       /**< TLV subtype */
    uint8_t tlvNum;                             /**< number of this TLV over TLVs sharing same ID (OUID and subtype), starting with 0 */
    uint8_t tlvMultMax;                         /**< number of TLV sharing same ID (OUID and subtype), starting with 0 */
    uint16_t pos;                               /**< current position of TLV in frame */
    uint8_t subHeaderLen;                       /**< sub header length (used for organizationally specific TLVs only) */
    uint16_t dataLen;                           /**< data length (excluding header and subheader) */
    GOAL_BOOL_T isActive;                       /**< TLV is active flag */
    GOAL_LLDP_TLV_SEND_CB_T txCb;               /**< Tx callback handler */
} GOAL_LLDP_TX_TLV_LIST_T;


typedef struct GOAL_LLDP_TX_INSTANCE_T {
    /* LLDP buffer to send */
    GOAL_BUFFER_T *pLldpBuf;                    /**< LLDP send buffer */
    GOAL_LLDP_TX_TLV_LIST_T *pTlvList;          /**< transmit TLV list - ordered by occurrences in LLDP frame to be sent, excluding END TLV */

    /* options */
    uint8_t msgTxHold;                          /**< Tx hold value */
    uint16_t msgTxInterval;                     /**< Tx interval */

    /* port specific data */
    uint32_t lldpDisableTx;                     /**< Tx disable flags */

    /* LLDP MAC address callback */
    GOAL_LLDP_MACGEN_CB_T macGenCb;             /**< Source MAC address creation callback */

    /* internal data */
    GOAL_BOOL_T flgRun;                         /**< sending of LLDP frames is active */
    GOAL_BOOL_T flgTx;                          /**< sending current burst of LLDP frames is active */
    GOAL_ETH_PORT_T txPortId;                   /**< Id of next Tx port */
    GOAL_TIMER_T *pTmrSend;                     /**< broadcast timer */
    GOAL_BOOL_T vlanTag;                        /**< VLAN tag is inserted in Tx frame */
} GOAL_LLDP_TX_INSTANCE_T;


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_lldpTxNew(
    GOAL_LLDP_INSTANCE_T *pLldp                 /**< [in] GOAL LLDP instance */
);

GOAL_STATUS_T goal_lldpTxShutdown(
    GOAL_LLDP_INSTANCE_T *pLldp                 /**< [in] GOAL LLDP instance */
);

GOAL_STATUS_T goal_lldpTxStart(
    GOAL_LLDP_INSTANCE_T *pLldp                 /**< [in] GOAL LLDP instance */
);

GOAL_STATUS_T goal_lldpTxStop(
    GOAL_LLDP_INSTANCE_T *pLldp                 /**< [in] GOAL LLDP instance */
);

GOAL_STATUS_T goal_lldpTxVlanSet(
    GOAL_LLDP_INSTANCE_T *pLldp,                /**< [in] GOAL LLDP instance */
    GOAL_BOOL_T vlanEnable,                     /**< enable/disable VLAN */
    uint16_t vlanId,                            /**< VLAN ID (VID) */
    uint8_t vlanPrio                            /**< priority for event messages (PCP) */
);

GOAL_STATUS_T goal_lldpTxLoop(
    GOAL_LLDP_INSTANCE_T *pLldp                 /**< [in] GOAL LLDP instance */
);

GOAL_STATUS_T goal_lldpTxTlvAdd(
    GOAL_LLDP_INSTANCE_T *pLldp,                /**< [in] GOAL LLDP instance */
    uint8_t tlvId,                              /**< TLV type id */
    uint32_t tlvOuid,                           /**< TLV ouid if type id is 127 */
    uint8_t tlvSubtypeId,                       /**< subtype id if type id is 127 */
    uint8_t tlvMultMax,                         /**< maximum allowed number of multiple TLVs with this ID (OUID and subtype) */
    GOAL_LLDP_TLV_SEND_CB_T txCb                /**< send calback */
);

GOAL_STATUS_T goal_lldpTxTlvValueSet(
    GOAL_LLDP_INSTANCE_T *pLldp,                /**< [in] GOAL LLDP instance */
    uint8_t tlvId,                              /**< TLV type id */
    uint32_t tlvOuid,                           /**< TLV ouid if type id is 127 */
    uint8_t tlvSubtypeId,                       /**< subtype id if type id is 127 */
    uint8_t tlvNum,                             /**< number of TLV sharing same ID (OUID, subtype), starting with 0 */
    uint8_t *pData,                             /**< [in] data to set */
    uint16_t tlvDataLen                         /**< length of TLV data */
);

GOAL_STATUS_T goal_lldpTxTlvValueGet(
    GOAL_LLDP_INSTANCE_T *pLldp,                /**< [in] GOAL LLDP instance */
    uint8_t tlvId,                              /**< TLV type id */
    uint32_t tlvOuid,                           /**< TLV ouid if type id is 127 */
    uint8_t tlvSubtypeId,                       /**< subtype id if type id is 127 */
    uint8_t tlvNum,                             /**< number of TLV sharing same ID (OUID, subtype), starting with 0 */
    GOAL_ETH_PORT_T portIdx,                    /**< Ethernet port index */
    uint8_t **ppData,                           /**< [out] data of TLV (without OUI and subtype) */
    uint16_t *pDataLen                          /**< [out] length of data of TLV (without OUI and subtype) */
);

GOAL_STATUS_T goal_lldpTxTlvActivationSet(
    GOAL_LLDP_INSTANCE_T *pLldp,                /**< [in] GOAL LLDP instance */
    uint8_t tlvId,                              /**< TLV type id */
    uint32_t tlvOuid,                           /**< TLV ouid if type id is 127 */
    uint8_t tlvSubtypeId,                       /**< subtype id if type id is 127 */
    uint8_t tlvNum,                             /**< number of TLV sharing same ID (OUID, subtype), starting with 0 */
    GOAL_BOOL_T activate                        /**< activate TLV */
);

GOAL_STATUS_T goal_lldpTxHoldSet(
    GOAL_LLDP_INSTANCE_T *pLldp,                /**< [in] GOAL LLDP instance */
    uint8_t msgTxHold                           /**< Tx hold time value */
);

GOAL_STATUS_T goal_lldpTxIntervalSet(
    GOAL_LLDP_INSTANCE_T *pLldp,                /**< [in] GOAL LLDP instance */
    uint16_t msgTxInterval                      /**< Tx interval time value in seconds */
);

GOAL_STATUS_T goal_lldpTxDisablePortFlagsSet(
    GOAL_LLDP_INSTANCE_T *pLldp,                /**< [in] GOAL LLDP instance */
    uint32_t txDisableFlags                     /**< Tx disable flags */
);

GOAL_STATUS_T goal_lldpTxDisablePortFlagsGet(
    GOAL_LLDP_INSTANCE_T *pLldp,                /**< [in] GOAL LLDP instance */
    uint32_t *pTxDisableFlags                   /**< [out] Tx disable flags */
);

GOAL_STATUS_T goal_lldpTxMacGenCbSet(
    GOAL_LLDP_INSTANCE_T *pLldp,                /**< [in] GOAL LLDP instance */
    GOAL_LLDP_MACGEN_CB_T macGenCb              /**< MAC address generation callback */
);

#endif /* GOAL_LLDP_TX_H */
