/** @file
 *
 * @brief GOAL Statistics
 *
 * This module manages generic GOAL statistics and is mainly used by the
 * Ethernet subsystem.
 *
 * @copyright
 * Copyright 2010-2018.
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

#ifndef GOAL_STAT_H
#define GOAL_STAT_H

#include "goal_includes.h"

#if GOAL_CONFIG_STATS == 1
/****************************************************************************/
/* Data types */
/****************************************************************************/
/**< Statistics Entry Table */
#define GOAL_STAT_ENTRY_RV_LIST \
    /* GOAL_STAT_ENTRY_RV(list macro name, statistics id, statistics macro name, description) */ \
    GOAL_STAT_ENTRY_RV(GOAL_ID_STAT, 0, GOAL_STAT_ID_NONE, "no entry") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 1, GOAL_STAT_ID_ETH_TOTAL_DISC, "total discarded frames") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 2, GOAL_STAT_ID_ETH_TOTAL_BYTE_DISC, "total discarded bytes") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 3, GOAL_STAT_ID_ETH_TOTAL_FRAMES, "total processed frames") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 4, GOAL_STAT_ID_ETH_TOTAL_BYTE_FRAMES, "total processed bytes") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 5, GOAL_STAT_ID_ETH_ODISC, "discarded outgoing frames") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 6, GOAL_STAT_ID_ETH_IDISC_VLAN, "discarded wrong/missing VLAN ids") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 7, GOAL_STAT_ID_ETH_IDISC_UNTAGGED, "discarded missing VLAN tags") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 8, GOAL_STAT_ID_ETH_IDISC_BLOCK, "discarded due to blocking mode") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 9, GOAL_STAT_ID_ETH_LEARN_CNT, "number of learned MAC addresses") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 10, GOAL_STAT_ID_ETH_AFRAMES_RECEIVED_OK, "received valid frames (including pause)") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 11, GOAL_STAT_ID_ETH_AFRAMES_CRC_ERRORS, "received frames with CRC errors") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 12, GOAL_STAT_ID_ETH_AALIGNMENT_ERRORS, "received frames with alignment errors") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 13, GOAL_STAT_ID_ETH_IFOUTOCTETS, "valid outgoing bytes") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 14, GOAL_STAT_ID_ETH_IFINOCTETS, "valid incoming bytes") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 15, GOAL_STAT_ID_ETH_ATX_PAUSE_CTRL_FRAMES, "transmitted pause frames") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 16, GOAL_STAT_ID_ETH_ARX_PAUSE_CTRL_FRAMES, "received pause frames") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 17, GOAL_STAT_ID_ETH_IFINERRORS, "erroneous incoming frames") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 18, GOAL_STAT_ID_ETH_IFOUTERRORS, "erroneous outgoing frames") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 19, GOAL_STAT_ID_ETH_IFIN_UCAST_PKTS, "received unicast frames") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 20, GOAL_STAT_ID_ETH_IFIN_MCAST_PKTS, "received multicast frames") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 21, GOAL_STAT_ID_ETH_IFIN_BCAST_PKTS, "received broadcast frames") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 22, GOAL_STAT_ID_ETH_IFOUTDISCARDS, "discarded outgoing frames") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 23, GOAL_STAT_ID_ETH_IFOUT_UCAST_PKTS, "transmitted unicast frames") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 24, GOAL_STAT_ID_ETH_IFOUT_MCAST_PKTS, "transmitted multicast frames") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 25, GOAL_STAT_ID_ETH_IFOUT_BCAST_PKTS, "transmitted broadcast frames") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 26, GOAL_STAT_ID_ETH_ETHERSTATS_OCTETS, "all bytes (good and bad)") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 27, GOAL_STAT_ID_ETH_ETHERSTATS_PKTS, "all frames (good and bad)") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 28, GOAL_STAT_ID_ETH_ETHERSTATS_UNDERSIZE, "frames too short") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 29, GOAL_STAT_ID_ETH_ETHERSTATS_OVERSIZE, "frames too long") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 30, GOAL_STAT_ID_ETH_ETHERSTATS_PKTS64, "frames with size of 64 bytes") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 31, GOAL_STAT_ID_ETH_ETHERSTATS_PKTS65TO127, "frames with size of 65-127 bytes") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 32, GOAL_STAT_ID_ETH_ETHERSTATS_PKTS128TO255, "frames with size of 128-255 bytes") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 33, GOAL_STAT_ID_ETH_ETHERSTATS_PKTS256TO511, "frames with size of 256-511 bytes") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 34, GOAL_STAT_ID_ETH_ETHERSTATS_PKTS512TO1023, "frames with size of 512-1023 bytes") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 35, GOAL_STAT_ID_ETH_ETHERSTATS_PKTS1024TO1518, "frames with size of 1024-1518 bytes") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 36, GOAL_STAT_ID_ETH_ETHERSTATS_PKTS1519TOX, "frames with size of >= 1519 bytes") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 37, GOAL_STAT_ID_ETH_ETHERSTATS_JABBERS, "jabbers") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 38, GOAL_STAT_ID_ETH_ETHERSTATS_FRAGS, "fragments") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 39, GOAL_STAT_ID_ETH_VLAN_RECV_OK, "received valid VLANs") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 40, GOAL_STAT_ID_ETH_VLAN_TRANS_OK, "transmitted valid VLANs") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 41, GOAL_STAT_ID_ETH_FRAMES_RETRANS, "retransmitted collision frames") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 42, GOAL_STAT_ID_ETH_ADEFERRED, "deferred at begin") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 43, GOAL_STAT_ID_ETH_AMULTIPLE_COLL, "frames transmitted after multiple collisions") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 44, GOAL_STAT_ID_ETH_ASINGLE_COLL, "frames transmitted after single collisions") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 45, GOAL_STAT_ID_ETH_ALATE_COLL, "too late collisions") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 46, GOAL_STAT_ID_ETH_AEXCESS_COLL, "frames discarded due to 16 consecutive collisions") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 47, GOAL_STAT_ID_ETH_ACARR_SENSE_ERR, "PHY carrier sense errors") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 48, GOAL_STAT_ID_ETH_IFINDISCARDS, "discarded incoming frames") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 49, GOAL_STAT_ID_ETH_IFIN_UNKNOWN_PROTO, "received unknown protocols") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 50, GOAL_STAT_ID_ETH_SQE_ERR, "SQE Test error") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 51, GOAL_STAT_ID_ETH_MAC_TX_ERR, "internal MAC TX errors") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 52, GOAL_STAT_ID_ETH_MAC_RX_ERR, "internal MAC RX errors") \
    GOAL_STAT_ENTRY_RV(GOAL_ID_ETH, 53, GOAL_STAT_ID_ETH_SYMBOL_ERR, "symbol errors")


/**< Statistics Id Mapping */
#undef GOAL_STAT_ENTRY_RV
#define GOAL_STAT_ENTRY_RV(nameList, id, name, desc) name = id,

typedef enum {
    GOAL_STAT_ENTRY_RV_LIST
    GOAL_STAT_ID_LAST
} GOAL_STAT_ID_ENUM_T;


/**< Statistics Id */
typedef uint32_t GOAL_STAT_ID_T;


/**< Statistics Index */
struct GOAL_STAT_T;


/**< value function */
typedef GOAL_STATUS_T (* GOAL_STAT_FUNC_T)(
    uint64_t *pVal,                             /**< [out] value */
    struct GOAL_STAT_T *pStat                   /**< statistics data */
);


/**< Statistics List Entry */
typedef struct GOAL_STAT_LIST_T {
    struct GOAL_STAT_LIST_T *pNext;             /**< next statistics list entry */
    struct GOAL_STAT_ENTRY_T *pListEntry;       /**< statistics entry list */

    uint16_t id;                                /**< statistics GOAL ID */
} GOAL_STAT_LIST_T;


/**< Statistics Entry */
typedef struct GOAL_STAT_ENTRY_T {
    struct GOAL_STAT_ENTRY_T *pNext;            /**< next statistics entry */
    struct GOAL_STAT_T *pListStat;              /**< statistics list */

    uint32_t id;                                /**< statistics entry id */
} GOAL_STAT_ENTRY_T;


/**< Statistics Index Entry */
typedef struct GOAL_STAT_T {
    struct GOAL_STAT_T *pNext;                  /**< next statistics entry */

    uint32_t id;                                /**< statistics id */
    GOAL_BOOL_T flgWeak;                        /**< weak entry */
    GOAL_STAT_FUNC_T funcGet;                   /**< get function */
    GOAL_STAT_FUNC_T funcReset;                 /**< reset function */
    const void *pPriv;                          /**< private pointer */
} GOAL_STAT_T;


/**< Statistics Name Mapping */
typedef struct {
    GOAL_ID_T idList;                           /**< list id */
    uint32_t id;                                /**< statistics id */
    const char *strName;                        /**< name */
} GOAL_STAT_NAME_T;


/****************************************************************************/
/* Exported variables */
/****************************************************************************/
extern GOAL_STAT_LIST_T *gpGoalStatList;        /**< global GOAL statistics list */


/****************************************************************************/
/* List of public functions */
/****************************************************************************/
GOAL_STATUS_T goal_statNew(
    GOAL_STAT_T **ppStat,                       /**< [out] statistics ptr storage */
    GOAL_ID_T idList,                           /**< statistics list id */
    uint32_t idEntry,                           /**< statistics entry id */
    uint32_t idStat,                            /**< statistics id */
    GOAL_BOOL_T flgWeak,                        /**< weak flag */
    GOAL_STAT_FUNC_T funcGet,                   /**< get function */
    GOAL_STAT_FUNC_T funcReset,                 /**< reset function */
    const void *pPriv                           /**< private function data */
);

GOAL_STATUS_T goal_statGetById(
    GOAL_STAT_T **ppStat,                       /**< [out] statistics ptr storage */
    GOAL_ID_T idList,                           /**< statistics list id */
    uint32_t idEntry,                           /**< statistics entry id */
    uint32_t id                                 /**< statistics id */
);

GOAL_STATUS_T goal_statValGet(
    uint64_t *pVal,                             /**< [out] statistics value storage */
    GOAL_STAT_T *pStat                          /**< statistics entry */
);

GOAL_STATUS_T goal_statValGetById(
    uint64_t *pVal,                             /**< [out] statistics value storage */
    GOAL_ID_T idList,                           /**< statistics list id */
    uint32_t idEntry,                           /**< statistics entry id */
    uint32_t id                                 /**< statistics id */
);

GOAL_STATUS_T goal_statReset(
    uint64_t *pVal,                             /**< [out] statistics last value */
    GOAL_STAT_T *pStat                          /**< statistics entry */
);

GOAL_STATUS_T goal_statResetById(
    GOAL_ID_T idList,                           /**< statistics list id */
    uint32_t idEntry,                           /**< statistics entry id */
    uint32_t id                                 /**< statistics id */
);

GOAL_STATUS_T goal_statResetByListId(
    GOAL_ID_T idList                            /**< statistics list id */
);

GOAL_STATUS_T goal_statNameGetById(
    const char **pStrName,                      /**< [out] statistics name */
    GOAL_ID_T idList,                           /**< statistics list id */
    uint32_t idEntry                            /**< statistics entry id */
);

GOAL_STATUS_T goal_statIsUnused(
    GOAL_ID_T idList,                           /**< statistics list id */
    uint32_t idEntry,                           /**< statistics entry id */
    uint32_t id                                 /**< statistics id */
);


#else /* GOAL_CONFIG_STATS == 1 */

#define goal_statInit(...)

#endif /* GOAL_CONFIG_STATS == 1 */

#endif /* GOAL_STAT_H */
