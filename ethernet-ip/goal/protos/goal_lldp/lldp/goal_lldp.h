/** @file
 *
 * @brief GOAL LLDP API
 *
 * This module implements the public API for the GOAL LLDP implementation.
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

#ifndef GOAL_LLDP_H
#define GOAL_LLDP_H


/****************************************************************************/
/* Defines */
/****************************************************************************/
/* instances */
#define GOAL_LLDP_INSTANCE_DEFAULT            0 /**< default instance Id */

/* TLV types */
#define GOAL_LLDP_TLV_TYPE_EOF                0 /**< ID of TLV End of LLDPDU */
#define GOAL_LLDP_TLV_TYPE_CHASSISID          1 /**< ID of TLV Chassis ID */
#define GOAL_LLDP_TLV_TYPE_PORTID             2 /**< ID of TLV Port ID */
#define GOAL_LLDP_TLV_TYPE_TTL                3 /**< ID of TLV Time To Live */
#define GOAL_LLDP_TLV_TYPE_PORTDESC           4 /**< ID of TLV Port Description */
#define GOAL_LLDP_TLV_TYPE_SYSNAME            5 /**< ID of TLV System Name */
#define GOAL_LLDP_TLV_TYPE_SYSDESC            6 /**< ID of TLV System Description */
#define GOAL_LLDP_TLV_TYPE_SYSCAP             7 /**< ID of TLV System Capabiities */
#define GOAL_LLDP_TLV_TYPE_MANADDR            8 /**< ID of TLV Management Address */
/* 9 -126 are reserved for future */
#define GOAL_LLDP_TLV_TYPE_OS_EXT           127 /**< ID of Organizationally Specific TLVs */

/* TLV multiplicity */
#define GOAL_LLDP_TLV_MULTIPLICITY_NUM_FIRST  0 /**< smallest number of TLV sharing same ID (or OUID and subtype) */
#define GOAL_LLDP_TLV_MULTIPLICITY_SINGLE     1 /**< allows only one TLV sharing same ID (or OUID and subtype) */

/* subtype values for GOAL_LLDP_TLV_TYPE_CHASSISID */
#define GOAL_LLDP_CHASSISID_SUBTYPE_CHASSIS_COMPONENT 1 /**< Chassis ID Subtype Chassis Component */
#define GOAL_LLDP_CHASSISID_SUBTYPE_IF_ALIAS          2 /**< Chassis ID Subtype Interface Alias */
#define GOAL_LLDP_CHASSISID_SUBTYPE_PORT_COMPONENT    3 /**< Chassis ID Subtype Port Component */
#define GOAL_LLDP_CHASSISID_SUBTYPE_MAC               4 /**< Chassis ID Subtype MAC address */
#define GOAL_LLDP_CHASSISID_SUBTYPE_NET               5 /**< Chassis ID Subtype Network Address */
#define GOAL_LLDP_CHASSISID_SUBTYPE_IF_NAME           6 /**< Chassis ID Subtype Interface Name */
#define GOAL_LLDP_CHASSISID_SUBTYPE_LOCAL             7 /**< Chassis ID Subtype Locally Assigned */

/* subtype values for GOAL_LLDP_TLV_TYPE_PORTID */
#define GOAL_LLDP_PORTID_SUBTYPE_IF_ALIAS        1 /**< Port ID Subtype Interface Alias */
#define GOAL_LLDP_PORTID_SUBTYPE_PORT_COMPONENT  2 /**< Port ID Subtype Port COmponent */
#define GOAL_LLDP_PORTID_SUBTYPE_MAC             3 /**< Port ID Subtype MAC Address */
#define GOAL_LLDP_PORTID_SUBTYPE_NET             4 /**< Port ID Subtype Network Address */
#define GOAL_LLDP_PORTID_SUBTYPE_IF_NAME         5 /**< Port ID Subtype Interface Name */
#define GOAL_LLDP_PORTID_SUBTYPE_AGENT_ID        6 /**< Port ID Subtype Agent circuit ID */
#define GOAL_LLDP_PORTID_SUBTYPE_LOCAL           7 /**< Port ID Subtype Locally Assigned */

/* TTL */
#define GOAL_LLDP_TTL_SHUTDOWN                   0 /**< TTL value to indicate a shutdown LLDP */

/* System capabilities */
#define GOAL_LLDP_SYSCAP_BIT_OTHER         (1 << 0) /**< System Capability: other */
#define GOAL_LLDP_SYSCAP_BIT_REPEATER      (1 << 1) /**< System Capability: Repeater */
#define GOAL_LLDP_SYSCAP_BIT_BRIDGE        (1 << 2) /**< System Capability: Bridge */
#define GOAL_LLDP_SYSCAP_BIT_WLAN_AP       (1 << 3) /**< System Capability: WLAN access point */
#define GOAL_LLDP_SYSCAP_BIT_ROUTER        (1 << 4) /**< System Capability: Router */
#define GOAL_LLDP_SYSCAP_BIT_TELEPHONE     (1 << 5) /**< System Capability: Telephone */
#define GOAL_LLDP_SYSCAP_BIT_DOCSIS        (1 << 6) /**< System Capability: DOCSIS cable device */
#define GOAL_LLDP_SYSCAP_BIT_STATION_ONLY  (1 << 7) /**< System Capability: Station only */

/* Management Address field values */
#define GOAL_LLDP_MANADDR_IPV4_STRINGLEN      5 /**< String Length of Managment Address, Subtype IPV4 */
#define GOAL_LLDP_MANADDR_IPV4_SUBTYPE        1 /**< Subtype of Managment Address: IPV4 */
#define GOAL_LLDP_MANADDR_OFF_STRLEN          0 /**< offset of Management Address string length field */
#define GOAL_LLDP_MANADDR_OFF_SUBTYPE         1 /**< offset of Management Address subtype field */
#define GOAL_LLDP_MANADDR_OFF_ADDRESS         2 /**< offset of Management */
#define GOAL_LLDP_MANADDR_OFF_IFSUBTYPE       1 /**< length of Management Address interface subtype field */
#define GOAL_LLDP_MANADDR_802_STRINGLEN       7 /**< String Length of Managment Address, Subtype 802 */
#define GOAL_LLDP_MANADDR_802_SUBTYPE         6 /**< Subtype of Managment Address: 802 */
#define GOAL_LLDP_MANADDR_IF_SUBTYPE          2 /**< Subtype of Managment Address: Interface */
#define GOAL_LLDP_MANADDR_LEN_STRLEN          1 /**< length of Management Address string length field */
#define GOAL_LLDP_MANADDR_LEN_SUBTYPE         1 /**< length of Management Address subtype field */
#define GOAL_LLDP_MANADDR_LEN_IFSUBTYPE       1 /**< length of Management Address interface subtype field */

/* miscellaneous */
#define GOAL_LLDP_OUID_NONE                   0 /**< TLV has no OUID */
#define GOAL_LLDP_SUBTYPE_NONE                0 /**< TLV has no subtype */

/* generic TLV length */
#define GOAL_LLDP_TLV_LEN_MAX                    511 /**< maximum total TLV information string length in bytes */
#define GOAL_LLDP_TLV_LEN_UNLIMITED          0xFFFFu /**< use maximum possible TLV length, independent of TLV type */
#define GOAL_LLDP_TLV_LEN_IGNORE                   0 /**< Ignore TLV */

/* Chassis ID TLV length */
#define GOAL_LLDP_TLV_OFFSET_CHASSISID_SUBTYPE     0 /**< offset of chassis ID subtype in TLV */
#define GOAL_LLDP_TLV_LEN_CHASSISID_SUBTYPE        1 /**< size of chassis ID subtype in bytes */
#define GOAL_LLDP_TLV_OFFSET_CHASSISID_STRING      1 /**< offset of chassis ID string in TLV */
#define GOAL_LLDP_TLV_LEN_CHASSISID_STRING_MIN     1 /**< minimum chassis ID string length in bytes */
#define GOAL_LLDP_TLV_LEN_CHASSISID_STRING_MAX   255 /**< maximum chassis ID string length in bytes */
#define GOAL_LLDP_TLV_LEN_CHASSISID_MIN          (sizeof(uint8_t) + GOAL_LLDP_TLV_LEN_CHASSISID_STRING_MIN) /**< minimum chassis ID (subtype + string) length in bytes */
#define GOAL_LLDP_TLV_LEN_CHASSISID_MAX          (sizeof(uint8_t) + GOAL_LLDP_TLV_LEN_CHASSISID_STRING_MAX) /**< maximum chassis ID (subtype + string) length in bytes */

/* Port ID TLV length */
#define GOAL_LLDP_TLV_LEN_PORTID_STRING_MIN        1 /**< minimum port ID string length in bytes */
#define GOAL_LLDP_TLV_LEN_PORTID_STRING_MAX      255 /**< maximum port ID string length in bytes */
#define GOAL_LLDP_TLV_LEN_PORTID_MIN             (sizeof(uint8_t) + GOAL_LLDP_TLV_LEN_PORTID_STRING_MIN) /**< minimum port ID (subtype + string) string length in bytes */
#define GOAL_LLDP_TLV_LEN_PORTID_MAX             (sizeof(uint8_t) + GOAL_LLDP_TLV_LEN_PORTID_STRING_MAX) /**< maximum port ID (subtype + string) string length in bytes */

/* Time to Live TLV length */
#define GOAL_LLDP_TLV_LEN_TTL                      2 /**< length of time to live in bytes */
#define GOAL_LLDP_TLV_LEN_TTL_MIN                  2 /**< minimum length of time to live in bytes */
#define GOAL_LLDP_TLV_LEN_TTL_MAX                  GOAL_LLDP_TLV_LEN_MAX /**< maximum length of time to live in bytes - all except the first two bytes are ignored */

/* Port Description TLV length */
#define GOAL_LLDP_TLV_LEN_PORTDESC_STRING_MAX    255 /**< maximum port descriptor string length in bytes */
#define GOAL_LLDP_TLV_LEN_PORTDESC_MAX           GOAL_LLDP_TLV_LEN_PORTDESC_STRING_MAX /**< maximum port descriptor length in bytes */

/* System Name TLV length */
#define GOAL_LLDP_TLV_LEN_SYSNAME_STRING_MAX     255 /**< maximum system name string length in bytes */
#define GOAL_LLDP_TLV_LEN_SYSNAME_MAX            GOAL_LLDP_TLV_LEN_SYSNAME_STRING_MAX /**< maximum system name length */

/* System Descriptor TLV length */
#define GOAL_LLDP_TLV_LEN_SYSDESC_STRING_MAX     255 /**< maximum system descriptor string length in bytes */
#define GOAL_LLDP_TLV_LEN_SYSDESC_MAX            GOAL_LLDP_TLV_LEN_SYSDESC_STRING_MAX /**< maximum system descriptor length in bytes */

/* System Capabilities TLV length */
#define GOAL_LLDP_TLV_LEN_SYSCAP                   4 /**< length of system capabilities in bytes */

/* Management Address TLV length */
#define GOAL_LLDP_TLV_LEN_MANADDR_MIN              9 /**< minimum management address string length in bytes */
#define GOAL_LLDP_TLV_LEN_MANADDR_MAX            167 /**< maximum management address string length in bytes */

/* Organizationally Specific TLV length */
#define GOAL_LLDP_TLV_LEN_OUI                      3 /**< length of organizationally unique identifyer (OUI) in bytes */
#define GOAL_LLDP_TLV_LEN_SUBTYPE                  1 /**< length of organizationally subtypes in bytes */
#define GOAL_LLDP_TLV_LEN_OS_DATA_STRING_MAX     507 /**< maximum organizationally information string length in bytes */


/****************************************************************************/
/* Macros */
/****************************************************************************/
/* Chassis ID string length vs. Chassis ID TLV length */
#define GOAL_LLDP_CHASSISID_LEN_TLV_TO_STR(len) ((len) - sizeof(uint8_t))
#define GOAL_LLDP_CHASSISID_LEN_STR_TO_TLV(len) ((len) + sizeof(uint8_t))

/* Port ID string length vs. Port ID TLV length */
#define GOAL_LLDP_PORTID_LEN_TLV_TO_STR(len) ((len) - sizeof(uint8_t))
#define GOAL_LLDP_PORTID_LEN_STR_TO_TLV(len) ((len) + sizeof(uint8_t))


/****************************************************************************/
/* data types */
/****************************************************************************/
typedef void GOAL_LLDP_HANDLE_T;                /**< GOAL LLDP instance handle */

/** basic data from LLDP frames of remote devices */
typedef struct GOAL_LLDP_REMOTE_T {
    GOAL_BOOL_T active;                         /**< remote device */
    GOAL_ETH_PORT_T portIdx;                    /**< port Index */
    GOAL_ETH_MAC_ADDR_T macAddr;                /**< MAC address of remote device (only valid, if device is active) */
    uint32_t lastChange;                        /**< sysUpTime of last change */
} GOAL_LLDP_REMOTE_T;

/**< remote data callback ID */
typedef enum GOAL_LLDP_REMOTE_CB_TYPE_T {
    /* remote device - called directly before TLV data update - old TLV data can be gotten here one last time */
    GOAL_LLDP_REMOTE_CB_TYPE_DEVICE_NEW,        /**< received LLDP data from new remote device */
    GOAL_LLDP_REMOTE_CB_TYPE_DEVICE_CHANGED,    /**< received LLDP data from another remote device (Spec: Too many neighbors) - data from old device, new device sends a device new callback directly after this */
    GOAL_LLDP_REMOTE_CB_TYPE_DEVICE_UNCHANGED,  /**< received LLDP data from same remote device as last one */

    /* remote device - called without any TLV data update - TLV data can be gotten here one last time */
    GOAL_LLDP_REMOTE_CB_TYPE_DEVICE_SHUTDOWN,   /**< received LLDP shutdown frame - stored LLDP remote data will be removed after this callback */
    GOAL_LLDP_REMOTE_CB_TYPE_DEVICE_LINKDOWN,   /**< Link is down, LLDP remote data will be removed after this callback */
    GOAL_LLDP_REMOTE_CB_TYPE_DEVICE_EXPIRED,    /**< received no further LLDP frame in TTL window - stored LLDP remote data will be removed after this callback */

    /* remote data - called directly after TLV data update - new TLV data are stored */
    GOAL_LLDP_REMOTE_CB_TYPE_DATA_INITIAL,      /**< initial data of remote */
    GOAL_LLDP_REMOTE_CB_TYPE_DATA_UPDATE,       /**< any remote data changed (Spec: rxChanges is TRUE) */
    GOAL_LLDP_REMOTE_CB_TYPE_DATA_UNCHANGED,    /**< updated remote data (Spec: rxChanges is FALSE) */
} GOAL_LLDP_REMOTE_CB_TYPE_T;

/**< remote TLV callback ID */
typedef enum GOAL_LLDP_REMOTE_TLV_CB_TYPE_T {
    GOAL_LLDP_REMOTE_TLV_CB_NEW,                /**< TLV was not received before by the current remote LLDP frames */
    GOAL_LLDP_REMOTE_TLV_CB_UPDATE,             /**< TLV changed within remote LLDP frames */
    GOAL_LLDP_REMOTE_TLV_CB_UNCHANGED,          /**< TLV is identical to the last received remote LLDP frames */
    GOAL_LLDP_REMOTE_TLV_CB_REMOVED,            /**< TLV of the last remote LLDP frame was removed in the current one */
} GOAL_LLDP_REMOTE_TLV_CB_TYPE_T;

/**< Remote callback handler */
typedef void (* GOAL_LLDP_REMOTE_UPDATE_CB_T) (
    GOAL_LLDP_HANDLE_T *pGlldp,                 /**< [in] GOAL LLDP handle */
    GOAL_LLDP_REMOTE_T *pRemote,                /**< [in] remote structure */
    GOAL_LLDP_REMOTE_CB_TYPE_T type             /**< callback type */
);

/**< Receive callback handler */
typedef GOAL_STATUS_T (*GOAL_LLDP_TLV_RECEIVE_CB_T)(
    GOAL_LLDP_HANDLE_T *pGlldp,                 /**< [in] GOAL LLDP handle */
    GOAL_ETH_PORT_T portIdx,                    /**< Ethernet port index */
    uint8_t tlvId,                              /**< TLV type id */
    uint32_t tlvOuid,                           /**< organization specific id */
    uint8_t tlvSubtypeId,                       /**< organization specific subtype */
    uint8_t tlvNum,                             /**< number of TLV sharing same ID (OUID and subtype) before this TLV in same frame */
    uint8_t *pTlvData,                          /**< [in] TLV content */
    uint16_t tlvLen,                            /**< length of TLV content */
    GOAL_LLDP_REMOTE_TLV_CB_TYPE_T type         /**< callback type */
);

/**< Update Tx TLV callback */
typedef GOAL_STATUS_T (*GOAL_LLDP_TLV_SEND_CB_T)(
    GOAL_LLDP_HANDLE_T *pGLldp,                 /**< [in] GOAL LLDP handle */
    GOAL_ETH_PORT_T portIdx,                    /**< Ethernet port index */
    uint8_t tlvId,                              /**< TLV type id */
    uint32_t tlvOuid,                           /**< organization specific id */
    uint8_t tlvSubtypeId,                       /**< organization specific subtype */
    uint8_t tlvNum,                             /**< number of TLV sharing same ID (OUID and subtype) before this TLV in same frame */
    uint8_t *pTlvData,                          /**< [in] poiner to TLV content in LLDP frame */
    uint16_t tlvLen                             /**< length of TLV content */
);

/**< Tx Source MAC address creation callback */
typedef GOAL_STATUS_T (*GOAL_LLDP_MACGEN_CB_T)(
    GOAL_LLDP_HANDLE_T *pGlldp,                 /**< [in] GOAL LLDP handle */
    GOAL_ETH_PORT_T portIdx,                    /**< Ethernet port index */
    GOAL_ETH_MAC_ADDR_T *pMacAddr               /**< [in] MAC address buffer to be written */
);


/****************************************************************************/
/* TLV data types (directly usable by frames) */
/****************************************************************************/
GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct GOAL_LLDP_TLV_CHASSISID_T {
    uint8_t subtype;                            /**< Chassis ID subtype subtype */
    char str[GOAL_LLDP_TLV_LEN_CHASSISID_STRING_MAX]; /**< Chassis ID */
} GOAL_TARGET_PACKED GOAL_LLDP_TLV_CHASSISID_T;
GOAL_TARGET_PACKED_STRUCT_POST

GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct GOAL_LLDP_TLV_PORTID_T {
    uint8_t subtype;                            /**< Port ID subtype subtype */
    char str[GOAL_LLDP_TLV_LEN_PORTID_STRING_MAX]; /**< Port ID */
} GOAL_TARGET_PACKED GOAL_LLDP_TLV_PORTID_T;
GOAL_TARGET_PACKED_STRUCT_POST

GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct GOAL_LLDP_TLV_TTL_T {
    uint16_t ttl_be16;                          /**< time to tive */
} GOAL_TARGET_PACKED GOAL_LLDP_TLV_TTL_T;
GOAL_TARGET_PACKED_STRUCT_POST

GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct GOAL_LLDP_TLV_PORTDESC_T {
    char str[GOAL_LLDP_TLV_LEN_PORTDESC_STRING_MAX]; /**< Port Descriptor */
} GOAL_TARGET_PACKED GOAL_LLDP_TLV_PORTDESC_T;
GOAL_TARGET_PACKED_STRUCT_POST

GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct GOAL_LLDP_TLV_SYSNAME_T {
    char str[GOAL_LLDP_TLV_LEN_SYSNAME_STRING_MAX]; /**< System Name */
} GOAL_TARGET_PACKED GOAL_LLDP_TLV_SYSNAME_T;
GOAL_TARGET_PACKED_STRUCT_POST

GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct GOAL_LLDP_TLV_SYSDESC_T {
    char str[GOAL_LLDP_TLV_LEN_SYSDESC_STRING_MAX]; /**< system description */
} GOAL_TARGET_PACKED GOAL_LLDP_TLV_SYSDESC_T;
GOAL_TARGET_PACKED_STRUCT_POST

GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct GOAL_LLDP_TLV_SYSCAP_T {
    uint16_t sysCap_be16;                       /**< system capabilities */
    uint16_t enabledCap_be16;                   /**< enabled capabilities */
} GOAL_TARGET_PACKED GOAL_LLDP_TLV_SYSCAP_T;
GOAL_TARGET_PACKED_STRUCT_POST

GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct GOAL_LLDP_TLV_MAN_ADDR_T {
    uint8_t data[GOAL_LLDP_TLV_LEN_MAX];        /**< generic data */
} GOAL_TARGET_PACKED GOAL_LLDP_TLV_MAN_ADDR_T;
GOAL_TARGET_PACKED_STRUCT_POST

GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct GOAL_LLDP_TLV_OS_T {
    uint8_t oui[GOAL_LLDP_TLV_LEN_OUI];         /**< organizationally unique identifyer (OUI) */
    uint8_t subtype;                            /**< organizationally defined subtype */
    uint8_t data[GOAL_LLDP_TLV_LEN_OS_DATA_STRING_MAX]; /**< organizationally information string */
} GOAL_TARGET_PACKED GOAL_LLDP_TLV_OS_T;
GOAL_TARGET_PACKED_STRUCT_POST

GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct GOAL_LLDP_TLV_OS_STRING_T {
    uint8_t data[GOAL_LLDP_TLV_LEN_OS_DATA_STRING_MAX]; /**< organizationally information string */
} GOAL_TARGET_PACKED GOAL_LLDP_TLV_OS_STRING_T;
GOAL_TARGET_PACKED_STRUCT_POST

GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct GOAL_LLDP_TLV_GENERIC_T {
    uint8_t data[GOAL_LLDP_TLV_LEN_MAX];        /**< generic data */
} GOAL_TARGET_PACKED GOAL_LLDP_TLV_GENERIC_T;
GOAL_TARGET_PACKED_STRUCT_POST


/****************************************************************************/
/* Public Functions */
/****************************************************************************/
GOAL_STATUS_T goal_lldpInit(
    void
);

GOAL_STATUS_T goal_lldpNew(
    GOAL_LLDP_HANDLE_T **ppGlldp,               /**< [out] GOAL LLDP instance */
    uint32_t id,                                /**< instance id */
    GOAL_MI_ETH_T *pMiEthHdl                    /**< Eth Mi handle of interface */
);

GOAL_STATUS_T goal_lldpStart(
    GOAL_LLDP_HANDLE_T *pGlldp                  /**< [in] GOAL LLDP handle */
);

GOAL_STATUS_T goal_lldpStop(
    GOAL_LLDP_HANDLE_T *pGlldp                  /**< [in] GOAL LLDP handle */
);

GOAL_STATUS_T goal_lldpVlanTxSet(
    GOAL_LLDP_HANDLE_T *pGlldp,                 /**< [in] GOAL LLDP handle */
    GOAL_BOOL_T vlanEnable,                     /**< enable/disable VLAN */
    uint16_t vlanId,                            /**< VLAN ID (VID) */
    uint8_t vlanPrio                            /**< priority for event messages (PCP) */
);

GOAL_STATUS_T goal_lldpTlvTxAdd(
    GOAL_LLDP_HANDLE_T *pGlldp,                 /**< [in] GOAL LLDP handle */
    uint8_t tlvId,                              /**< TLV type id */
    GOAL_LLDP_TLV_SEND_CB_T txCb                /**< send calback (may be NULL) */
);

GOAL_STATUS_T goal_lldpTlvTxMultipleAdd(
    GOAL_LLDP_HANDLE_T *pGlldp,                 /**< [in] GOAL LLDP handle */
    uint8_t tlvMultMax,                         /**< maximum allowed number of multiple TLVs with this OUID and subtype */
    uint8_t tlvId,                              /**< TLV type id */
    GOAL_LLDP_TLV_SEND_CB_T txCb                /**< send calback (may be NULL) */
);

GOAL_STATUS_T goal_lldpTlvTxOsAdd(
    GOAL_LLDP_HANDLE_T *pGlldp,                 /**< [in] GOAL LLDP handle */
    uint32_t tlvOuid,                           /**< OUI of custom TLV */
    uint8_t tlvSubtypeId,                       /**< subtype id of custom TLV */
    GOAL_LLDP_TLV_SEND_CB_T txCb                /**< send calback (may be NULL) */
);

GOAL_STATUS_T goal_lldpTlvTxOsMultipleAdd(
    GOAL_LLDP_HANDLE_T *pGlldp,                 /**< [in] GOAL LLDP handle */
    uint32_t tlvOuid,                           /**< OUI of custom TLV */
    uint8_t tlvSubtypeId,                       /**< subtype id of custom TLV */
    uint8_t tlvMultMax,                         /**< maximum allowed number of multiple TLVs with this OUID and subtype */
    GOAL_LLDP_TLV_SEND_CB_T txCb                /**< send calback (may be NULL) */
);

GOAL_STATUS_T goal_lldpTlvTxValueSet(
    GOAL_LLDP_HANDLE_T *pGlldp,                 /**< [in] GOAL LLDP handle */
    uint8_t tlvId,                              /**< TLV type id */
    uint8_t *pData,                             /**< [in] data to set */
    uint16_t tlvDataLen                         /**< length of TLV data */
);

GOAL_STATUS_T goal_lldpTlvTxValueWithNumSet(
    GOAL_LLDP_HANDLE_T *pGlldp,                 /**< [in] GOAL LLDP handle */
    uint8_t tlvId,                              /**< TLV type id */
    uint8_t tlvNum,                             /**< number of TLV sharing same ID */
    uint8_t *pData,                             /**< [in] data to set */
    uint16_t tlvDataLen                         /**< length of TLV data */
);

GOAL_STATUS_T goal_lldpTlvTxValueGet(
    GOAL_LLDP_HANDLE_T *pGlldp,                 /**< [in] GOAL LLDP handle */
    GOAL_ETH_PORT_T portIdx,                    /**< Ethernet port index */
    uint8_t tlvId,                              /**< TLV type id */
    uint8_t **ppData,                           /**< [out] data of TLV (without OUI and subtype) */
    uint16_t *ptlvDataLen                       /**< [out] length of data of TLV (without OUI and subtype) */
);

GOAL_STATUS_T goal_lldpTlvTxValueByNumGet(
    GOAL_LLDP_HANDLE_T *pGlldp,                 /**< [in] GOAL LLDP handle */
    GOAL_ETH_PORT_T portIdx,                    /**< Ethernet port index */
    uint8_t tlvId,                              /**< TLV type id */
    uint8_t tlvNum,                             /**< number of TLV sharing same ID */
    uint8_t **ppData,                           /**< [out] data of TLV (without OUI and subtype) */
    uint16_t *ptlvDataLen                       /**< [out] length of data of TLV (without OUI and subtype) */
);

GOAL_STATUS_T goal_lldpTlvTxDeactivate(
    GOAL_LLDP_HANDLE_T *pGlldp,                 /**< [in] GOAL LLDP handle */
    uint8_t tlvId                               /**< TLV type id */
);

GOAL_STATUS_T goal_lldpTlvTxByNumDeactivate(
    GOAL_LLDP_HANDLE_T *pGlldp,                 /**< [in] GOAL LLDP handle */
    uint8_t tlvId,                              /**< TLV type id */
    uint8_t tlvNum                              /**< number of TLV sharing same ID */
);

GOAL_STATUS_T goal_lldpTlvTxActivate(
    GOAL_LLDP_HANDLE_T *pGlldp,                 /**< [in] GOAL LLDP handle */
    uint8_t tlvId                               /**< TLV type id */
);

GOAL_STATUS_T goal_lldpTlvTxByNumActivate(
    GOAL_LLDP_HANDLE_T *pGlldp,                 /**< [in] GOAL LLDP handle */
    uint8_t tlvId,                              /**< TLV type id */
    uint8_t tlvNum                              /**< number of TLV sharing same ID */
);

GOAL_STATUS_T goal_lldpTlvTxOsValueSet(
    GOAL_LLDP_HANDLE_T *pGlldp,                 /**< [in] GOAL LLDP handle */
    uint32_t tlvOuid,                           /**< OUI of custom TLV */
    uint8_t tlvSubtypeId,                       /**< subtype id of custom TLV */
    uint8_t *pData,                             /**< [in] data to set */
    uint16_t tlvDataLen                         /**< length of TLV data */
);

GOAL_STATUS_T goal_lldpTlvTxOsValueWithNumSet(
    GOAL_LLDP_HANDLE_T *pGlldp,                 /**< [in] GOAL LLDP handle */
    uint32_t tlvOuid,                           /**< OUI of custom TLV */
    uint8_t tlvSubtypeId,                       /**< subtype id of custom TLV */
    uint8_t tlvNum,                             /**< number of TLV sharing same OUID and subtype */
    uint8_t *pData,                             /**< [in] data to set */
    uint16_t tlvDataLen                         /**< length of TLV data */
);

GOAL_STATUS_T goal_lldpTlvTxOsValueGet(
    GOAL_LLDP_HANDLE_T *pGlldp,                 /**< [in] GOAL LLDP handle */
    uint32_t tlvOuid,                           /**< OUI of custom TLV */
    uint8_t tlvSubtypeId,                       /**< subtype id of custom TLV */
    GOAL_ETH_PORT_T portIdx,                    /**< Ethernet port index */
    uint8_t **ppData,                           /**< [out] data of TLV (without OUI and subtype) */
    uint16_t *ptlvDataLen                       /**< [out] length of data of TLV (without OUI and subtype) */
);

GOAL_STATUS_T goal_lldpTlvTxOsValueByNumGet(
    GOAL_LLDP_HANDLE_T *pGlldp,                 /**< [in] GOAL LLDP handle */
    uint32_t tlvOuid,                           /**< OUI of custom TLV */
    uint8_t tlvSubtypeId,                       /**< subtype id of custom TLV */
    uint8_t tlvNum,                             /**< number of TLV sharing same OUID and subtype */
    GOAL_ETH_PORT_T portIdx,                    /**< Ethernet port index */
    uint8_t **ppData,                           /**< [out] data of TLV (without OUI and subtype) */
    uint16_t *ptlvDataLen                       /**< [out] length of data of TLV (without OUI and subtype) */
);

GOAL_STATUS_T goal_lldpTlvTxOsDeactivate(
    GOAL_LLDP_HANDLE_T *pGlldp,                 /**< [in] GOAL LLDP handle */
    uint32_t tlvOuid,                           /**< OUI of custom TLV */
    uint8_t tlvSubtypeId                        /**< subtype id of custom TLV */
);

GOAL_STATUS_T goal_lldpTlvTxOsByNumDeactivate(
    GOAL_LLDP_HANDLE_T *pGlldp,                 /**< [in] GOAL LLDP handle */
    uint32_t tlvOuid,                           /**< OUI of custom TLV */
    uint8_t tlvSubtypeId,                       /**< subtype id of custom TLV */
    uint8_t tlvNum                              /**< number of TLV sharing same OUID and subtype */
);

GOAL_STATUS_T goal_lldpTlvTxOsActivate(
    GOAL_LLDP_HANDLE_T *pGlldp,                 /**< [in] GOAL LLDP handle */
    uint32_t tlvOuid,                           /**< OUI of custom TLV */
    uint8_t tlvSubtypeId                        /**< subtype id of custom TLV */
);

GOAL_STATUS_T goal_lldpTlvTxOsByNumActivate(
    GOAL_LLDP_HANDLE_T *pGlldp,                 /**< [in] GOAL LLDP handle */
    uint32_t tlvOuid,                           /**< OUI of custom TLV */
    uint8_t tlvSubtypeId,                       /**< subtype id of custom TLV */
    uint8_t tlvNum                              /**< number of TLV sharing same OUID and subtype */
);

GOAL_STATUS_T goal_lldpTlvRxAdd(
    GOAL_LLDP_HANDLE_T *pGlldp,                 /**< [in] GOAL LLDP handle */
    uint8_t tlvId,                              /**< TLV type id */
    uint16_t tlvsizeMax,                        /**< maximum size of TLV */
    GOAL_LLDP_TLV_RECEIVE_CB_T rxCb             /**< receive callback */
);

GOAL_STATUS_T goal_lldpTlvRxMultipleAdd(
    GOAL_LLDP_HANDLE_T *pGlldp,                 /**< [in] GOAL LLDP handle */
    uint8_t tlvId,                              /**< TLV type id */
    uint8_t tlvMultMax,                         /**< maximum allowed number of multiple TLVs with this ID */
    uint16_t tlvsizeMax,                        /**< maximum size of TLV */
    GOAL_LLDP_TLV_RECEIVE_CB_T rxCb             /**< receive callback */
);

GOAL_STATUS_T goal_lldpTlvRxOsAdd(
    GOAL_LLDP_HANDLE_T *pGlldp,                 /**< [in] GOAL LLDP handle */
    uint32_t tlvOuid,                           /**< OUI of custom TLV */
    uint8_t tlvSubtypeId,                       /**< subtype id of custom TLV */
    uint16_t tlvsizeMax,                        /**< maximum size of TLV */
    GOAL_LLDP_TLV_RECEIVE_CB_T rxCb             /**< receive call back */
);

GOAL_STATUS_T goal_lldpTlvRxOsMultipleAdd(
    GOAL_LLDP_HANDLE_T *pGlldp,                 /**< [in] GOAL LLDP handle */
    uint32_t tlvOuid,                           /**< OUI of custom TLV */
    uint8_t tlvSubtypeId,                       /**< subtype id of custom TLV */
    uint8_t tlvMultMax,                         /**< maximum allowed number of multiple TLVs with this OUID and subtype */
    uint16_t tlvsizeMax,                        /**< maximum size of TLV */
    GOAL_LLDP_TLV_RECEIVE_CB_T rxCb             /**< receive call back */
);

GOAL_STATUS_T goal_lldpPeerInfoGet(
    GOAL_LLDP_HANDLE_T *pGlldp,                 /**< [in] GOAL LLDP handle */
    GOAL_ETH_PORT_T portIdx,                    /**< Ethernet port index */
    GOAL_LLDP_REMOTE_T **ppRemote               /**< [out] remote device information */
);

GOAL_STATUS_T goal_lldpTlvRxValueGet(
    GOAL_LLDP_HANDLE_T *pGlldp,                 /**< [in] GOAL LLDP handle */
    uint8_t tlvId,                              /**< TLV type id */
    GOAL_ETH_PORT_T portIdx,                    /**< Ethernet port index */
    uint8_t **ppData,                           /**< [out] data of TLV (withoit OUI and subtype) */
    uint16_t *pDataLen                          /**< [out] length of data of TLV (withoit OUI and subtype) */
);

GOAL_STATUS_T goal_lldpTlvRxValueByNumGet(
    GOAL_LLDP_HANDLE_T *pGlldp,                 /**< [in] GOAL LLDP handle */
    uint8_t tlvId,                              /**< TLV type id */
    uint8_t tlvNum,                             /**< number of TLV sharing same ID */
    GOAL_ETH_PORT_T portIdx,                    /**< Ethernet port index */
    uint8_t **ppData,                           /**< [out] data of TLV (without OUI and subtype) */
    uint16_t *pDataLen                          /**< [out] length of data of TLV (without OUI and subtype) */
);

GOAL_STATUS_T goal_lldpTlvRxOsValueGet(
    GOAL_LLDP_HANDLE_T *pGlldp,                 /**< [in] GOAL LLDP handle */
    uint32_t tlvOuid,                           /**< OUI of custom TLV */
    uint8_t tlvSubtypeId,                       /**< subtype id */
    GOAL_ETH_PORT_T portIdx,                    /**< Ethernet port index */
    uint8_t **ppData,                           /**< [out] data of TLV (withoit OUI and subtype) */
    uint16_t *pDataLen                          /**< [out] length of data of TLV (withoit OUI and subtype) */
);

GOAL_STATUS_T goal_lldpTlvRxOsValueByNumGet(
    GOAL_LLDP_HANDLE_T *pGlldp,                 /**< [in] GOAL LLDP handle */
    uint32_t tlvOuid,                           /**< OUI of custom TLV */
    uint8_t tlvSubtypeId,                       /**< subtype id */
    uint8_t tlvNum,                             /**< number of TLV sharing same OUID and subtype */
    GOAL_ETH_PORT_T portIdx,                    /**< Ethernet port index */
    uint8_t **ppData,                           /**< [out] data of TLV (withoit OUI and subtype) */
    uint16_t *pDataLen                          /**< [out] length of data of TLV (withoit OUI and subtype) */
);

GOAL_STATUS_T goal_lldpRemoteUpdateHandlerRegister(
    GOAL_LLDP_HANDLE_T *pGlldp,                 /**< [in] GOAL LLDP handle */
    GOAL_LLDP_REMOTE_UPDATE_CB_T remoteUpdateCb /**< remote Update Callback handler */
);

GOAL_STATUS_T goal_lldpMsgTxHoldSet(
    GOAL_LLDP_HANDLE_T *pGlldp,                 /**< [in] GOAL LLDP handle */
    uint8_t msgTxHold                           /**< Tx hold time value */
);

GOAL_STATUS_T goal_lldpMsgTxIntervalSet(
    GOAL_LLDP_HANDLE_T *pGlldp,                 /**< [in] GOAL LLDP handle */
    uint16_t msgTxInterval                      /**< Tx interval time value in seconds */
);

GOAL_STATUS_T goal_lldpUserDataSet(
    GOAL_LLDP_HANDLE_T *pGlldp,                 /**< [in] GOAL LLDP handle */
    void *pUserData                             /**< [in] arbitrary user data */
);

GOAL_STATUS_T goal_lldpUserDataGet(
    GOAL_LLDP_HANDLE_T *pGlldp,                 /**< [in] GOAL LLDP handle */
    void **ppUserData                           /**< [out] arbitrary user data */
);

GOAL_STATUS_T goal_lldpTxDisableFlagsSet(
    GOAL_LLDP_HANDLE_T *pGlldp,                 /**< [in] GOAL LLDP handle */
    uint32_t txDisableFlags                     /**< Tx disable flags */
);

GOAL_STATUS_T goal_lldpPortMacGenCbSet(
    GOAL_LLDP_HANDLE_T *pGlldp,                 /**< [in] GOAL LLDP handle */
    GOAL_LLDP_MACGEN_CB_T macGenCb              /**< MAC address generation callback */
);

GOAL_STATUS_T goal_lldpSysUpTimeGet(
    GOAL_LLDP_HANDLE_T *pGlldp,                 /**< [in] GOAL LLDP handle */
    uint32_t *pSysUpTime                        /**< [out] SysUpTime */
);

#endif /* GOAL_LLDP_H */
