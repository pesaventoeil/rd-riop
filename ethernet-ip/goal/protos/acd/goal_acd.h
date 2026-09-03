/** @file
 *
 * @brief GOAL ACD
 *
 * This module provides a address conflict detection.
 *
 * @copyright
 * Copyright 2010-2020.
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


#ifndef GOAL_ACD_H
#define GOAL_ACD_H


/****************************************************************************/
/* Public Defines */
/****************************************************************************/
#define GOAL_ACD_NO_CONFLICTED_DETECTED 0       /**< No conflict has been detected since this attribute was last cleared. */
#define GOAL_ACD_PROBE_IPV4_ADDRESS     1       /**< Last conflict detected during ProbeIpv4Address state. */
#define GOAL_ACD_ONGOING_DETECTION      2       /**< Last conflict detected during OngoingDetection */
#define GOAL_ACD_SEMI_ACTIVE_PROBE      3       /**< Last conflict detected during SemiActiveProbe */

#define GOAL_ACD_INSTANCE_DEFAULT             0 /**< ID of ACD default instance */


/****************************************************************************/
/* Public data types */
/****************************************************************************/
/**< ARP PDU */
GOAL_TARGET_PACKED_STRUCT_PRE
typedef struct {
    uint16_t hwAddressType_be16;                /**< 1 for Ethernet H/W */
    uint16_t protoAddressType_be16;             /**< 0x800 for IP */
    uint8_t hwAddrLen;                          /**< 6 for Ethernet h/w */
    uint8_t protoAddrLen;                       /**< 4 for IP */
    uint16_t operation_be16;                    /**< 1 for Req or 2 for Rsp */
    uint8_t senderHwAddress[6];                 /**< Sender's h/w addr */
    uint8_t senderProtoAddress[4];              /**< Sender's proto addr */
    uint8_t targetHwAddress[6];                 /**< Target's h/w addr */
    uint8_t targetProtodAdress[4];              /**< Target's proto addr */
} GOAL_TARGET_PACKED GOAL_ACD_ARP_PDU_T;

/**< last ACD conflict data */
typedef GOAL_TARGET_PACKED_PRE struct {
    uint8_t acdActivity;                        /**< State of ACD activity when last conflict detected */
    uint8_t remoteMac[6];                       /**< MAC address of remote node from the ARP PDU in which a conflict was detected */
    GOAL_ACD_ARP_PDU_T arpPru;                  /**< Copy of the raw ARP PDU in which a conflict was detected. */
} GOAL_TARGET_PACKED GOAL_ACD_LAST_CONFLICT_DETECTED_T;

/** callback types */
typedef enum {
    GOAL_ACD_CB_IP_PROBED,                      /**< callback: ip probed */
    GOAL_ACD_CB_IP_CONFLICT,                    /**< callback: ip conflict detected, stop using ip */
    GOAL_ACD_CB_SET_LASTCONFLICTDETECTED,       /**< callback: set LastConflictDetected Attr */
} GOAL_ACD_CB_T;

/** ACD Instance handle */
typedef void GOAL_ACD_HANDLE_T;


/****************************************************************************/
/* Callback Helpers */
/****************************************************************************/
#define GOAL_ACD_CB_ID_IP 0                      /**< acd ip callback */


typedef GOAL_STATUS_T (* GOAL_ACD_CB_IP_PROBED_T)(
    GOAL_ACD_HANDLE_T *pAcd,                    /**< GOAL ACD instance */
    GOAL_ACD_CB_T acdStatus                     /**< acd status flag */
);


/****************************************************************************/
/* Public Functions */
/****************************************************************************/
GOAL_STATUS_T goal_acdInit(
    void
);

GOAL_STATUS_T goal_acdNew(
    GOAL_ACD_HANDLE_T **ppAcd,                  /**< GOAL ACD instance reference */
    const uint32_t id,                          /**< instance id */
    GOAL_MI_ETH_T *pMiEthHdl,                   /**< Eth Mi handle of interface */
    GOAL_MI_NET_T *pMiNetHdl                    /**< Net MI handle of interface */
);

GOAL_STATUS_T goal_acdReset(
    GOAL_ACD_HANDLE_T *pAcd                     /**< GOAL ACD instance */
);

GOAL_STATUS_T goal_acdEnable(
    GOAL_ACD_HANDLE_T *pAcd                     /**< GOAL ACD instance */
);

GOAL_STATUS_T goal_acdDisable(
    GOAL_ACD_HANDLE_T *pAcd                     /**< GOAL ACD instance */
);

GOAL_STATUS_T goal_acdCfgProbeWaitSet(
    uint32_t probeWait                          /**< probe delay in ms */
);

GOAL_STATUS_T goal_acdCfgProbeNumSet(
    uint8_t probeNum                            /**< number of probe packets */
);

GOAL_STATUS_T goal_acdCfgMinProbeSet(
    uint32_t probeMin                           /**< minimum probe delay in ms */
);

GOAL_STATUS_T goal_acdCfgMaxProbeSet(
    uint32_t probeMax                           /**< maximum probe delay in ms */
);

GOAL_STATUS_T goal_acdCfgAnnounceWaitSet(
    uint32_t announceWait                       /**< Announce delay in ms */
);

GOAL_STATUS_T goal_acdCfgAnnounceNumSet(
    uint8_t announceNum                         /**< number of Announce packets */
);

GOAL_STATUS_T goal_acdCfgAnnounceIntervalSet(
    uint32_t announceInt                        /**< Announce interval in ms */
);

GOAL_STATUS_T goal_acdCfgDefendIntervalSet(
    uint32_t defendInt                          /**< Defend interval in ms */
);

GOAL_STATUS_T goal_acdCfgEipArpParsingEnable(
    GOAL_BOOL_T enable                          /**< enable feature */
);

GOAL_STATUS_T goal_acdCfgKeepAnnouncedIpEnable(
    GOAL_BOOL_T enable                          /**< enable feature */
);

GOAL_STATUS_T goal_acdCfgLinkupWaitRandom(
    uint32_t probeWaitRandom                    /**< maximum random probe delay part in ms */
);

GOAL_STATUS_T goal_acdCfgLinkupWaitOffset(
    uint32_t probeWaitFixed                     /**< fixed probe delay in ms */
);

GOAL_STATUS_T  goal_acdLastConflictDetectedGet(
    GOAL_ACD_HANDLE_T *pAcd,                    /**< GOAL ACD instance */
    GOAL_ACD_LAST_CONFLICT_DETECTED_T *pLastConflict /**< [out] last conflict info */
);

GOAL_STATUS_T goal_acdLastConflictDetectedReset(
    GOAL_ACD_HANDLE_T *pAcd                     /**< GOAL ACD instance */
);

GOAL_STATUS_T goal_acdUserDataSet(
    GOAL_ACD_HANDLE_T *pAcd,                    /**< GOAL ACD instance */
    void *pUserData                             /**< arbitrary user data */
);

GOAL_STATUS_T goal_acdUserDataGet(
    GOAL_ACD_HANDLE_T *pAcd,                    /**< GOAL ACD instance */
    void **ppUserData                           /**< [out] arbitrary user data */
);

#endif /* GOAL_ACD_H */
