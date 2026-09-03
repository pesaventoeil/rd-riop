/** @file
 *
 * @brief Internal Header for device detection
 *
 * @copyright
 * Copyright 2013-2017 port GmbH Halle/Saale.
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

#ifndef GOAL_DD_INT_H
#define GOAL_DD_INT_H


/****************************************************************************/
/* Dependencies */
/****************************************************************************/
#include "goal_dd.h"


/****************************************************************************/
/* Defines */
/****************************************************************************/

/* Commands */
#define DD_COMMAND_HELLOREQ 1                   /**< Hello Request */
#define DD_COMMAND_HELLOREP 2                   /**< Hello Reply */
#define DD_COMMAND_SETPERMIP 3                  /**< Set IP Command */
#define DD_COMMAND_SETPERMIPOK 4                /**< Set IP Command Reply */
#define DD_COMMAND_SETCONFIG 5                  /**< Set Config Command */
#define DD_COMMAND_SETCONFIGOK 6                /**< Set Config Command Reply */
#define DD_COMMAND_WINK 7                       /**< WINK Command */
#define DD_COMMAND_GETCONFIG 8                  /**< GET CONFIG Item */
#define DD_COMMAND_GETCONFIGOK 9                /**< GET CONFIG Reply */
#define DD_COMMAND_GETLISTREQ 10                /**< GET LIST Request */
#define DD_COMMAND_GETLISTRES 11                /**< GET LIST Request */
#define DD_COMMAND_WINKREP 12                   /**< WINK Reply */

/* TLV Types */
#define DD_TLVT_IP 1                            /**< IP Address */
#define DD_TLVT_NETMASK 2                       /**< IP Netmask Address */
#define DD_TLVT_DNS 3                           /**< DNS Address */
#define DD_TLVT_GW 4                            /**< GW Address */
#define DD_TLVT_DHCP 5                          /**< DHCP option */
#define DD_TLVT_HOSTNAME 6                      /**< Hostname */
#define DD_TLVT_STRING 7                        /**< Generic String */
#define DD_TLVT_U32 8                           /**< Generic UINT32 */
#define DD_TLVT_MAC 9                           /**< MAC Address */
#define DD_TLVT_TID 10                          /**< Transaction ID */
#define DD_TLVT_BOB 11                          /**< Binary Object */
#define DD_TLVT_CMMID 12                        /**< GOAL-CM Addressing */
#define DD_TLVT_CMVID 13                        /**< GOAL-CM Addressing */
#define DD_TLVT_CMT 14                          /**< GOAL-CM Type */
#define DD_TLVT_CMV 15                          /**< GOAL-CM variable */
#define DD_TLVT_CID 16                          /**< Customer ID */
#define DD_TLVT_U8 17                           /**< Generic UINT8 */
#define DD_TLVT_CMA 18                          /**< GOAL-CM access rights */

/* Sizes */
#define DD_SIZE_COMMAND 1                       /**< Size of Command field */
#define DD_SIZE_NUMTLVS 1                       /**< Size of Number of TLS field */
#define DD_SIZE_TLVTYPE 2                       /**< Size of TLV type field */
#define DD_SIZE_TLVLENGH 2                      /**< Size of TLV length field */
#define DD_SIZE_MAC 6                           /**< Size of MAC Address */
#define DD_SIZE_CMT 4                           /**< Size of CMT field */
#define DD_SIZE_CID 4                           /**< Size of CID field */
#define DD_SIZE_TID 4                           /**< Size of TID value */
#define DD_SIZE_DHCP 1                          /**< Size of DHCP value */
#define DD_SIZE_IP 4                            /**< Size of IP value */
#define DD_SIZE_NETMASK 4                       /**< Size of IP value */
#define DD_SIZE_GW 4                            /**< Size of IP value */
#define DD_SIZE_DNS 4                           /**< Size of IP value */
#define DD_SIZE_U32 4                           /**< Size of U32 */
#define DD_SIZE_CMMID 4                         /**< Size of Module ID */
#define DD_SIZE_CMVID 4                         /**< Size of Variable ID */
#define DD_SIZE_U8 1                            /**< Size of U8 */
#define DD_SIZE_CMA 4                           /**< Size of CM access */

/* Network constants */
#define DD_UDP_PORT_SRV 19010                   /**< GOAL DD UDP Server Port */
#define DD_UDP_PORT_CLIENT 19011                /**< GOAL DD UDP Client Port */

/* Customer ID default */
#define DD_CID_DEFAULT 0x00000000               /**< Default Customer ID */

/**< Maximum buffer size of response frame */
#define DD_MESSAGE_BUFFER_SIZE GOAL_NETBUF_SIZE

#define GOAL_DD_GETCONF_TLVS (9)                /**< GETCONF tlv count */
#define GOAL_DD_SETCONF_TLVS (7)                /**< SETCONF tlv count */
#define GOAL_DD_GETLIST_TLVS (9)                /**< GETLIST tlv count */
#define GOAL_DD_SETIP_TLVS (9)                  /**< SETIP tlv count */
#define GOAL_DD_HELLO_TLVS (11)                 /**< HELLO tlv count */
#define GOAL_DD_WINK_TLVS (3)                   /**< WINK tlv count */


/****************************************************************************/
/* Data Types */
/****************************************************************************/

/**< GOAL DD header */
GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct {
    uint8_t command;                            /**< message command */
    uint8_t tlvCount;                           /**< number of TLVs */
    uint8_t flgsCryptInfo;                      /**< crypto information flags */
    uint8_t reserved;                           /**< reserved byte */
} GOAL_TARGET_PACKED GOAL_DD_MESSAGE_HEADER_T;
GOAL_TARGET_PACKED_STRUCT_POST


/**< GOAL DD TLV header */
GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct {
    uint16_t type_be16;                         /**< type of TLV */
    uint16_t length_be16;                       /**< length of TLV */
} GOAL_TARGET_PACKED GOAL_DD_TLV_HEADER_T;
GOAL_TARGET_PACKED_STRUCT_POST


/**< GOAL DD minimal message */
GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct {
    GOAL_DD_MESSAGE_HEADER_T header;            /**< message header */

    GOAL_DD_TLV_HEADER_T tlvTransactionId;      /**< header for TID TLV */
    uint32_t tid_be32;                          /**< Transaction Id */

    GOAL_DD_TLV_HEADER_T tlvCustomerId;         /**< header for CID TLV */
    uint32_t cid_be32;                          /**< customer Id */

    GOAL_DD_TLV_HEADER_T tlvMacAddress;         /**< header for MAC TLV */
    GOAL_ETH_MAC_ADDR_T mac_be;                 /**< MAC address */
} GOAL_TARGET_PACKED GOAL_DD_BASE_MESSAGE_T;
GOAL_TARGET_PACKED_STRUCT_POST


/**< GOAL HELLO REQUEST message */
GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct {
    GOAL_DD_MESSAGE_HEADER_T header;            /**< message header */

    GOAL_DD_TLV_HEADER_T tlvTransactionId;      /**< header for TID TLV */
    uint32_t tid_be32;                          /**< Transaction Id */

    GOAL_DD_TLV_HEADER_T tlvCustomerId;         /**< header for CID TLV */
    uint32_t cid_be32;                          /**< customer Id */

} GOAL_TARGET_PACKED GOAL_DD_HELLO_REQUEST_MESSAGE_T;
GOAL_TARGET_PACKED_STRUCT_POST


/**< GOAL HELLO REPLY message */
GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct {
    GOAL_DD_MESSAGE_HEADER_T header;            /**< message header */

    GOAL_DD_TLV_HEADER_T tlvTransactionId;      /**< header for TID TLV */
    uint32_t tid_be32;                          /**< Transaction Id */

    GOAL_DD_TLV_HEADER_T tlvCustomerId;         /**< header for CID TLV */
    uint32_t cid_be32;                          /**< customer Id */

    GOAL_DD_TLV_HEADER_T tlvMacAddress;         /**< header for MAC TLV */
    GOAL_ETH_MAC_ADDR_T mac_be;                 /**< MAC address */

    GOAL_DD_TLV_HEADER_T tlvIpAddress;          /**< header for IP TLV */
    uint32_t ip_be32;                           /**< IP address */

    GOAL_DD_TLV_HEADER_T tlvNetmask;            /**< header for Netmask TLV */
    uint32_t netmask_be32;                      /**< netmask */

    GOAL_DD_TLV_HEADER_T tlvGateway;            /**< header for Gateway TLV */
    uint32_t gateway_be32;                      /**< gateway */

    GOAL_DD_TLV_HEADER_T tlvDns;                /**< header for DNS TLV */
    uint32_t dns_be32;                          /**< DNS */

    GOAL_DD_TLV_HEADER_T tlvDhcp;               /**< header for DHCP TLV */
    uint8_t dhcp;                               /**< DHCP flag */

    GOAL_DD_TLV_HEADER_T tlvModState;           /**< header for module State TLV */
    uint32_t modState_be32;                     /**< module state */

    /* additional TLVs with variable length :
        TLV_HOSTNAME
        TLV_STRING (Module Name)
    */
} GOAL_TARGET_PACKED GOAL_DD_HELLO_REPLY_MESSAGE_T;
GOAL_TARGET_PACKED_STRUCT_POST


/**< GOAL DD SET IP REPLY and RESPONSE message */
GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct {
    GOAL_DD_MESSAGE_HEADER_T header;            /**< message header */

    GOAL_DD_TLV_HEADER_T tlvTransactionId;      /**< header for TID TLV */
    uint32_t tid_be32;                          /**< Transaction Id */

    GOAL_DD_TLV_HEADER_T tlvCustomerId;         /**< header for CID TLV */
    uint32_t cid_be32;                          /**< customer Id */

    GOAL_DD_TLV_HEADER_T tlvMacAddress;         /**< header for MAC TLV */
    GOAL_ETH_MAC_ADDR_T mac_be;                 /**< MAC address */

    GOAL_DD_TLV_HEADER_T tlvIpAddress;          /**< header for IP TLV */
    uint32_t ip_be32;                           /**< IP address */

    GOAL_DD_TLV_HEADER_T tlvNetmask;            /**< header for Netmask TLV */
    uint32_t netmask_be32;                      /**< netmask */

    GOAL_DD_TLV_HEADER_T tlvGateway;            /**< header for Gateway TLV */
    uint32_t gateway_be32;                      /**< gateway */

    GOAL_DD_TLV_HEADER_T tlvDns;                /**< header for DNS TLV */
    uint32_t dns_be32;                          /**< DNS */

    GOAL_DD_TLV_HEADER_T tlvDhcp;               /**< header for DHCP TLV */
    uint8_t dhcp;                               /**< DHCP flag */

    GOAL_DD_TLV_HEADER_T tlvHostname;           /**< header for DHCP TLV */
    uint8_t startHostname;
} GOAL_TARGET_PACKED GOAL_DD_SETIP_MESSAGE_T;
GOAL_TARGET_PACKED_STRUCT_POST


/**< GOAL DD WINK REQUEST message */
GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct {
    GOAL_DD_MESSAGE_HEADER_T header;            /**< message header */

    GOAL_DD_TLV_HEADER_T tlvTransactionId;      /**< header for TID TLV */
    uint32_t tid_be32;                          /**< Transaction Id */

    GOAL_DD_TLV_HEADER_T tlvCustomerId;         /**< header for CID TLV */
    uint32_t cid_be32;                          /**< customer Id */

    GOAL_DD_TLV_HEADER_T tlvMacAddress;         /**< header for MAC TLV */
    GOAL_ETH_MAC_ADDR_T mac_be;                 /**< MAC address */

    GOAL_DD_TLV_HEADER_T tlvTime;               /**< header for U32 TLV */
    uint32_t winktime_be32;                     /**< wink time */
} GOAL_TARGET_PACKED GOAL_DD_WINK_REQUEST_MESSAGE_T;
GOAL_TARGET_PACKED_STRUCT_POST


/**< GOAL DD GET CONFIG REQUEST message */
GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct {
    GOAL_DD_MESSAGE_HEADER_T header;            /**< message header */

    GOAL_DD_TLV_HEADER_T tlvTransactionId;      /**< header for TID TLV */
    uint32_t tid_be32;                          /**< Transaction Id */

    GOAL_DD_TLV_HEADER_T tlvCustomerId;         /**< header for CID TLV */
    uint32_t cid_be32;                          /**< customer Id */

    GOAL_DD_TLV_HEADER_T tlvMacAddress;         /**< header for MAC TLV */
    GOAL_ETH_MAC_ADDR_T mac_be;                 /**< MAC address */

    GOAL_DD_TLV_HEADER_T tlvModId;              /**< header for Mod Id TLV */
    uint32_t modId_be32;                        /**< module Id */

    GOAL_DD_TLV_HEADER_T tlvVarId;              /**< header for Var Id TLV */
    uint32_t varId_be32;                        /**< variable Id */
} GOAL_TARGET_PACKED GOAL_DD_GET_CONFIG_REQUEST_MESSAGE_T;
GOAL_TARGET_PACKED_STRUCT_POST


/**< union for type dependent content in message */
GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE union {
    uint8_t val8;                               /**< byte value */
    uint16_t val_be16;                          /**< 16 bit word value */
    uint32_t val_be32;                          /**< 32 bit word value */
} GOAL_TARGET_PACKED GOAL_DD_TLV_VALUE_T;
GOAL_TARGET_PACKED_STRUCT_POST

/**< GOAL dd GET CONFIG RESPONSE message */
GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct {
    GOAL_DD_MESSAGE_HEADER_T header;            /**< message header */

    GOAL_DD_TLV_HEADER_T tlvTransactionId;      /**< header for TID TLV */
    uint32_t tid_be32;                          /**< Transaction Id */

    GOAL_DD_TLV_HEADER_T tlvCustomerId;         /**< header for CID TLV */
    uint32_t cid_be32;                          /**< customer Id */

    GOAL_DD_TLV_HEADER_T tlvMacAddress;         /**< header for MAC TLV */
    GOAL_ETH_MAC_ADDR_T mac_be;                 /**< MAC address */

    GOAL_DD_TLV_HEADER_T tlvModId;              /**< header for Mod Id TLV */
    uint32_t modId_be32;                        /**< module Id */

    GOAL_DD_TLV_HEADER_T tlvVarId;              /**< header for Var Id TLV */
    uint32_t varId_be32;                        /**< variable Id */

    GOAL_DD_TLV_HEADER_T tlvType;               /**< header for Type TLV */
    uint32_t type_be32;                         /**< type */

    GOAL_DD_TLV_HEADER_T tlvMaxLength;          /**< header for Maximum Length TLV */
    uint32_t maxLength_be32;                    /**< maximum length */

    GOAL_DD_TLV_HEADER_T tlvAccess;             /**< header for CM access */
    uint32_t access_be32;                       /**< access rights */

    GOAL_DD_TLV_HEADER_T tlvValue;              /**< header for Value TLV */
    GOAL_DD_TLV_VALUE_T value;

} GOAL_TARGET_PACKED GOAL_DD_GET_CONFIG_RESPONSE_MESSAGE_T;
GOAL_TARGET_PACKED_STRUCT_POST


/**< GOAL DD GET LIST REQUEST message */
GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct {
    GOAL_DD_MESSAGE_HEADER_T header;            /**< message header */

    GOAL_DD_TLV_HEADER_T tlvTransactionId;      /**< header for TID TLV */
    uint32_t tid_be32;                          /**< Transaction Id */

    GOAL_DD_TLV_HEADER_T tlvCustomerId;         /**< header for CID TLV */
    uint32_t cid_be32;                          /**< customer Id */

    GOAL_DD_TLV_HEADER_T tlvMacAddress;         /**< header for MAC TLV */
    GOAL_ETH_MAC_ADDR_T mac_be;                 /**< MAC address */

    GOAL_DD_TLV_HEADER_T tlvListType;           /**< header for Mod Id TLV */
    uint32_t listType_be32;                     /**< module Id */

    GOAL_DD_TLV_HEADER_T tlvModId;              /**< header for Mod Id TLV */
    uint32_t modId_be32;                        /**< module Id */

    GOAL_DD_TLV_HEADER_T tlvVarId;              /**< header for Var Id TLV */
    uint32_t varId_be32;                        /**< variable Id */
} GOAL_TARGET_PACKED GOAL_DD_GET_LIST_REQUEST_MESSAGE_T;
GOAL_TARGET_PACKED_STRUCT_POST


/**< GOAL DD SET CONFIG REQUEST message */
GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct {
    GOAL_DD_MESSAGE_HEADER_T header;            /**< message header */

    GOAL_DD_TLV_HEADER_T tlvTransactionId;      /**< header for TID TLV */
    uint32_t tid_be32;                          /**< Transaction Id */

    GOAL_DD_TLV_HEADER_T tlvCustomerId;         /**< header for CID TLV */
    uint32_t cid_be32;                          /**< customer Id */

    GOAL_DD_TLV_HEADER_T tlvMacAddress;         /**< header for MAC TLV */
    GOAL_ETH_MAC_ADDR_T mac_be;                 /**< MAC address */

    GOAL_DD_TLV_HEADER_T tlvModId;              /**< header for Mod Id TLV */
    uint32_t modId_be32;                        /**< module Id */

    GOAL_DD_TLV_HEADER_T tlvVarId;              /**< header for Var Id TLV */
    uint32_t varId_be32;                        /**< variable Id */

    GOAL_DD_TLV_HEADER_T tlvType;               /**< header for Type TLV */
    uint32_t type_be32;                         /**< type */

    GOAL_DD_TLV_HEADER_T tlvValue;              /**< header for Value TLV */
    GOAL_DD_TLV_VALUE_T value;
} GOAL_TARGET_PACKED GOAL_DD_SET_CONFIG_REQUEST_MESSAGE_T;
GOAL_TARGET_PACKED_STRUCT_POST


/**< GOAL DD SET CONFIG RESPONSE message */
GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct {
    GOAL_DD_MESSAGE_HEADER_T header;            /**< message header */

    GOAL_DD_TLV_HEADER_T tlvTransactionId;      /**< header for TID TLV */
    uint32_t tid_be32;                          /**< Transaction Id */

    GOAL_DD_TLV_HEADER_T tlvCustomerId;         /**< header for CID TLV */
    uint32_t cid_be32;                          /**< customer Id */

    GOAL_DD_TLV_HEADER_T tlvMacAddress;         /**< header for MAC TLV */
    GOAL_ETH_MAC_ADDR_T mac_be;                 /**< MAC address */

    GOAL_DD_TLV_HEADER_T tlvModId;              /**< header for Mod Id TLV */
    uint32_t modId_be32;                        /**< module Id */

    GOAL_DD_TLV_HEADER_T tlvVarId;              /**< header for Var Id TLV */
    uint32_t varId_be32;                        /**< variable Id */

    GOAL_DD_TLV_HEADER_T tlvType;               /**< header for Type TLV */
    uint32_t type_be32;                         /**< type */

    GOAL_DD_TLV_HEADER_T tlvValue;              /**< header for Value TLV */
    GOAL_DD_TLV_VALUE_T value;
} GOAL_TARGET_PACKED GOAL_DD_SET_CONFIG_RESPONSE_MESSAGE_T;
GOAL_TARGET_PACKED_STRUCT_POST


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_ddFilterCheck(
    GOAL_DD_T *pHdlDd,                          /**< dd handle */
    uint32_t modId,                             /**< module id */
    uint32_t varId,                             /**< variable id */
    GOAL_DD_ACCESS_T access                     /**< varible access */
);

#endif /* GOAL_DD_INT_H */

