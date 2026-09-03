/** @file
 *
 * @brief GOAL LLDP core
 *
 * This module implements the core of the GOAL LLDP implementation.
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

#ifndef GOAL_LLDP_CORE_H
#define GOAL_LLDP_CORE_H


/****************************************************************************/
/* Defines */
/****************************************************************************/
#define GOAL_LLDP_ETH_TYPE_LLDP            0x88CC /**< Ethernet Frame Type: LLDP */
#define GOAL_LLDP_MAC_ADDR {0x01, 0x80, 0xC2, 0x00, 0x00, 0x0E} /**< Destination MAC address */

/* TLV masks */
#define GOAL_LLDP_TLV_LEN_MASK_HIGH        0x01 /**< TLV length high byte mask */
#define GOAL_LLDP_TLV_LEN_SHIFT_HIGH       8    /**< TLV length high byte shift */
#define GOAL_LLDP_TLV_LEN_MASK_LOW         0xff /**< TLV length low byte mask */
#define GOAL_LLDP_TLV_ID_SHIFT             1    /**< TLV ID byte shift */

/* TLV lengths */
#define GOAL_LLDP_TLV_HEADER_SIZE          2    /**< size of TLV header in bytes */
#define GOAL_LLDP_TLV_END_SIZE             GOAL_LLDP_TLV_HEADER_SIZE /**< size of TLV header in bytes */

/* TLV ID and length */
#define GOAL_LLDP_TLV_ID_GET(ptr)   (*ptr >> GOAL_LLDP_TLV_ID_SHIFT) /**< get TLV ID from start of TLV */
#define GOAL_LLDP_TLV_LEN_GET(ptr)  (uint16_t) ((((ptr)[0] & GOAL_LLDP_TLV_LEN_MASK_HIGH) << GOAL_LLDP_TLV_LEN_SHIFT_HIGH) | ((ptr)[1])) /**< get TLV length from start of TLV */
#define GOAL_LLDP_TLV_HEADER_SET(ptr, id, len)  (ptr)[0] = (uint8_t) ((id) << GOAL_LLDP_TLV_ID_SHIFT);                                                             \
                                                (ptr)[0] |= (uint8_t) (((len) >> GOAL_LLDP_TLV_LEN_SHIFT_HIGH) & GOAL_LLDP_TLV_LEN_MASK_HIGH); \
                                                (ptr)[1] = (uint8_t) ((len) & GOAL_LLDP_TLV_LEN_MASK_LOW); /**< set TLV ID and length (without header) to start of TLV */
#define GOAL_LLDP_TLV_END_SET(ptr) GOAL_LLDP_TLV_HEADER_SET((ptr), GOAL_LLDP_TLV_TYPE_EOF, 0) /**< End Of LLDPDU TLV */


/****************************************************************************/
/* Forward Declarations */
/****************************************************************************/
struct GOAL_LLDP_RX_INSTANCE_T;
struct GOAL_LLDP_TX_INSTANCE_T;


/****************************************************************************/
/* Data Types */
/****************************************************************************/
typedef struct GOAL_LLDP_CORE_INSTANCE_T {
    GOAL_BOOL_T flgRun;                         /**< instance is running */
    GOAL_TIMESTAMP_T startTime;                 /**< start time mark for calculating agents sysUpTime */
    uint32_t numPorts;                          /**< number pf ports */
    void *pUserData;                            /**< arbitrary user data */
    uint32_t portLinkUp;                        /**< link state mask */
} GOAL_LLDP_CORE_INSTANCE_T;

typedef struct GOAL_LLDP_INSTANCE_T {
    GOAL_INSTANCE_HEADER(GOAL_LLDP_INSTANCE_T); /**< instance header */
    GOAL_MI_ETH_T *pMiEthHdl;                   /**< Eth Mi handle of interface */
    GOAL_LLDP_CORE_INSTANCE_T core;             /**< LLDP core instance */
    struct GOAL_LLDP_RX_INSTANCE_T *pRx;        /**< LLDP Receive state machine instance */
    struct GOAL_LLDP_TX_INSTANCE_T *pTx;        /**< LLDP Transmit (and Transmit Timer) state machine instance */
} GOAL_LLDP_INSTANCE_T;

GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct GOAL_LLDP_ETH_HEADER_T {
    GOAL_ETH_MAC_ADDR_T macDst;                 /**< destination MAC address */
    GOAL_ETH_MAC_ADDR_T macSrc;                 /**< source MAC address */
    uint16_t ethType_be16;                      /**< Ethertype */
} GOAL_TARGET_PACKED GOAL_LLDP_ETH_HEADER_T;
GOAL_TARGET_PACKED_STRUCT_POST

GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct GOAL_LLDP_ETH_HEADER_WITH_VLAN_T {
    GOAL_ETH_MAC_ADDR_T macDst;                 /**< destination MAC address */
    GOAL_ETH_MAC_ADDR_T macSrc;                 /**< source MAC address */
    uint16_t vlanTpid_be16;                     /**< VLAN tag protocol identifier */
    uint16_t vlanTci_be16;                      /**< VLAN tag control information */
    uint16_t ethType_be16;                      /**< Ethertype */
} GOAL_TARGET_PACKED GOAL_LLDP_ETH_HEADER_WITH_VLAN_T;
GOAL_TARGET_PACKED_STRUCT_POST


/****************************************************************************/
/* Public Functions */
/****************************************************************************/
GOAL_STATUS_T goal_lldpCoreNew(
    GOAL_LLDP_INSTANCE_T *pLldp                 /**< [in] GOAL LLDP instance */
);

GOAL_STATUS_T goal_lldpCoreShutdown(
    GOAL_LLDP_INSTANCE_T *pLldp                 /**< [in] GOAL LLDP instance */
);

GOAL_STATUS_T goal_lldpCoreStart(
    GOAL_LLDP_INSTANCE_T *pLldp                 /**< [in] GOAL LLDP instance */
);

GOAL_STATUS_T goal_lldpCoreStop(
    GOAL_LLDP_INSTANCE_T *pLldp                 /**< [in] GOAL LLDP instance */
);

GOAL_STATUS_T goal_lldpCoreUserDataSet(
    GOAL_LLDP_INSTANCE_T *pLldp,                /**< [in] GOAL LLDP instance */
    void *pUserData                             /**< [in] arbitrary user data */
);

GOAL_STATUS_T goal_lldpCoreUserDataGet(
    GOAL_LLDP_INSTANCE_T *pLldp,                /**< [in] GOAL LLDP instance */
    void **ppUserData                           /**< [out] arbitrary user data */
);

GOAL_STATUS_T goal_ldpCorePortNextGet(
    GOAL_LLDP_INSTANCE_T *pLldp,                /**< [in] GOAL LLDP instance */
    GOAL_ETH_PORT_T *pPortId                    /**< [in,out] index of port */
);

GOAL_STATUS_T goal_ldpCorePortNumGet(
    GOAL_LLDP_INSTANCE_T *pLldp,                /**< [in] GOAL LLDP instance */
    uint32_t *pPortNum                          /**< [out] number of ports */
);

uint32_t goal_lldpCoreSysUpTimeGet(
    GOAL_LLDP_INSTANCE_T *pLldp                 /**< [in] GOAL LLDP instance */
);

#endif /* GOAL_LLDP_CORE_H */
