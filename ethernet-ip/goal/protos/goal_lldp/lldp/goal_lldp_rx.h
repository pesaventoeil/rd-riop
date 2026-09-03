/** @file
 *
 * @brief GOAL LLDP Rx
 *
 * This module implements receiving mechanism of the GOAL LLDP implementation.
 *
 * Based on registered Rx TLV handling received data are stored and can gotten
 * by corresponding getter function. Cyclically it is checked for timeout remote
 * devices.
 *
 * On timeout or shutdown of any remote device, a remote device callback with that
 * information is sent.
 * On LLDP frame receiption first a remote device callback is done, indicating if
 * it is a new or known device (or if the device changed). After all TLV callbacks
 * are called followed by another device callback indicating if any TLV data changed.
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


#ifndef GOAL_LLDP_RX_H
#define GOAL_LLDP_RX_H


/****************************************************************************/
/* Data Types */
/****************************************************************************/
/**< Rx callback list entry */
typedef struct GOAL_LLDP_RX_TLV_LIST_T {
    struct GOAL_LLDP_RX_TLV_LIST_T *pNext;      /**< next receive TLV list entry */
    uint8_t tlvId;                              /**< TLV id */
    uint32_t tlvOuid;                           /**< TLV OUID */
    uint8_t tlvSubtypeId;                       /**< TLV subtype */
    uint8_t tlvNum;                             /**< number of this TLV over TLVs sharing same ID (OUID and subtype), starting with 0 */
    uint8_t tlvMultMax;                         /**< number of TLV sharing same ID (OUID and subtype), starting with 0 */
    uint8_t subHeaderLen;                       /**< sub header length (used for organizationally specific TLVs only) */
    uint8_t *pData;                             /**< data buffer for each remote device */
    uint16_t *pLen;                             /**< data lengths for each remote device */
    uint8_t *pFlags;                            /**< flags for each remote device */
    uint16_t dataLenListen;                     /**< maximum length of remote data to listen */
    uint16_t dataLenMax;                        /**< maximum length of remote data */
    GOAL_LLDP_TLV_RECEIVE_CB_T rxCb;            /**< Rx callback handler */
} GOAL_LLDP_RX_TLV_LIST_T;

typedef struct GOAL_LLDP_RX_REMOTE_INFO_T {
    GOAL_LLDP_REMOTE_T remDev;                  /* remote device information */
    uint16_t ttl;                               /* time to live value */
    GOAL_BOOL_T flgInitial;                     /* initial data of remote */
    GOAL_TIMESTAMP_T tsRxLast;                  /* timestamp of last received LLDP frame */
} GOAL_LLDP_RX_REMOTE_INFO_T;

typedef struct GOAL_LLDP_RX_INSTANCE_T {
    GOAL_BOOL_T flgRun;                         /**< instance is running */
    GOAL_LLDP_RX_REMOTE_INFO_T *pRemDevList;    /**< remote device list (one entry per port) */
    GOAL_LLDP_REMOTE_UPDATE_CB_T remoteUpdateCb; /**< remote Callback */
    GOAL_LLDP_RX_TLV_LIST_T *pTlvList;          /**< receive TLV list (one entry per TLV to listen) */
    GOAL_TIMER_T *pTmrRxInfoAge;                /**< rxInfoAge Timer */
} GOAL_LLDP_RX_INSTANCE_T;


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_lldpRxNew(
    GOAL_LLDP_INSTANCE_T *pLldp,                /**< [in] GOAL LLDP instance */
    uint32_t numPorts                           /**< number of ports */
);

GOAL_STATUS_T goal_lldpRxShutdown(
    GOAL_LLDP_INSTANCE_T *pLldp                 /**< [in] GOAL LLDP instance */
);

GOAL_STATUS_T goal_lldpRxStart(
    GOAL_LLDP_INSTANCE_T *pLldp                 /**< [in] GOAL LLDP instance */
);

GOAL_STATUS_T goal_lldpRxStop(
    GOAL_LLDP_INSTANCE_T *pLldp                 /**< [in] GOAL LLDP instance */
);

GOAL_STATUS_T goal_lldpRxTlvAdd(
    GOAL_LLDP_INSTANCE_T *pLldp,                /**< [in] GOAL LLDP instance */
    uint8_t tlvId,                              /**< TLV type id */
    uint32_t tlvOuid,                           /**< TLV ouid if type id is 127 */
    uint8_t tlvSubtypeId,                       /**< subtype id if type id is 127 */
    uint8_t tlvMultMax,                         /**< maximum allowed number of multiple TLVs with this ID (OUID and subtype) */
    uint16_t tlvSizeMax,                        /**< maximum size of TLV */
    GOAL_LLDP_TLV_RECEIVE_CB_T rxCb             /**< receive callback */
);

GOAL_STATUS_T goal_lldpRxPeerInfoGet(
    GOAL_LLDP_INSTANCE_T *pLldp,                /**< [in] GOAL LLDP instance */
    GOAL_ETH_PORT_T portIdx,                    /**< Ethernet port index */
    GOAL_LLDP_REMOTE_T **ppRemote               /**< [out] remote device information */
);

GOAL_STATUS_T goal_lldpRxTlvValueGet(
    GOAL_LLDP_INSTANCE_T *pLldp,                /**< [in] GOAL LLDP instance */
    uint8_t tlvId,                              /**< TLV type id */
    uint32_t tlvOuid,                           /**< TLV ouid if type id is 127 */
    uint8_t tlvSubtypeId,                       /**< subtype id if type id is 127 */
    uint8_t tlvMultNum,                         /**< number of TLV sharing same ID (OUID and subtype), starting with 0 */
    GOAL_ETH_PORT_T portIdx,                    /**< Ethernet port index */
    uint8_t **ppData,                           /**< [out] data of TLV (without OUI and subtype) */
    uint16_t *pDataLen                          /**< [out] length of data of TLV (without OUI and subtype) */
);

GOAL_STATUS_T goal_lldpRxRemoteUpdateHandlerRegister(
    GOAL_LLDP_INSTANCE_T *pLldp,                /**< [in] GOAL LLDP instance */
    GOAL_LLDP_REMOTE_UPDATE_CB_T remoteUpdateCb /**< Remote Update Callback handler */
);

GOAL_STATUS_T goal_lldpRx(
    GOAL_LLDP_INSTANCE_T *pLldp,                /**< [in] GOAL LLDP instance */
    GOAL_BUFFER_T *pBuf                         /**< [in] data received */
);

GOAL_STATUS_T goal_lldpRxRemoteDeviceRemove(
    GOAL_LLDP_INSTANCE_T *pLldp,                /**< [in] GOAL LLDP instance */
    GOAL_ETH_PORT_T portIdx,                    /**< Ethernet port index */
    GOAL_LLDP_REMOTE_CB_TYPE_T type             /**< callback type */
);

#endif /* GOAL_LLDP_RX_H */
