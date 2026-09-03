/** @file
 *
 * @brief NXP NETC Switch Driver BSP glue code
 *
 * @copyright
 * Copyright 2023 port GmbH Halle/Saale.
 * This software is protected Intellectual Property and may only be used
 * according to the license agreement.
 */

#include "nxp_netc_bsp_int.h"
#include <fsl_netc.h>
#include <fsl_netc_switch.h>


/****************************************************************************/
/* Configuration */
/****************************************************************************/
#ifndef GOAL_CONFIG_SWITCH_NXP_STATISTICS_LOG
# define GOAL_CONFIG_SWITCH_NXP_STATISTICS_LOG 0
#endif

#define ISC_ENTRY_ID_PRES 0x112
#define ISC_ENTRY_ID_ASND 0x113

#define EPL_MAX_FRAME_LENGTH 1500

/* CC-Link IE TSN detection frame filter */
#define GOAL_DRV_SWITCH_CCLIETSN_ETYPE  0x890f  /**< CCLIETSN: EtherType */
#define GOAL_DRV_SWITCH_CCLIETSN_CMD_DETECTION  0xC1 /**< CCLIETSN: Frame Type: Detection */
#define GOAL_DRV_SWITCH_CCLIETSN_PRECEDENCE 200 /**< CCLIETSN: Filter Precedence */
#define GOAL_BROADCAST_MAC_ADDR { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }


/****************************************************************************/
/* Powerlink frame filters */
/****************************************************************************/
#if GOAL_CONFIG_EPL == 1
#define GOAL_SW_CTRL_EPL_LIST \
    /*                     name,                        pattern,
     *                                                  mask,
     *                                                  posNodeId */ \
    GOAL_SW_CTRL_EPL_ENTRY(GOAL_EPL_PATTERN_SOA_IDENT,  0x05, 0xff, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, \
                                                    /*  Mtyp  Dest  Src   NMT   flags res   sId   Target */ \
                                                        0x7f, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, \
                                                        7, \
                                                        0x0001, 0x0010, 0x0001, 0x0000), /* length = 60 bytes */ \
    GOAL_SW_CTRL_EPL_ENTRY(GOAL_EPL_PATTERN_SOA_STATUS, 0x05, 0xff, 0x00, 0x00, 0x00, 0x00, 0x02, 0x01, \
                                                    /*  Mtyp  Dest  Src   NMT   flags res   sId   Target */ \
                                                        0x7f, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, \
                                                        7, \
                                                        0x0002, 0x0020, 0x0002, 0x0008), \
    GOAL_SW_CTRL_EPL_ENTRY(GOAL_EPL_PATTERN_SOA_NNT,    0x05, 0xff, 0x00, 0x00, 0x00, 0x00, 0x03, 0x01, \
                                                    /*  Mtyp  Dest  Src   NMT   flags res   sId   Target */ \
                                                        0x7f, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, \
                                                        7, \
                                                        0x0004, 0x0040, 0x0004, 0x0008), \
    GOAL_SW_CTRL_EPL_ENTRY(GOAL_EPL_PATTERN_SOA_UNSPEC, 0x05, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0x01, \
                                                    /*  Mtyp  Dest  Src   NMT   flags res   sId   Target */ \
                                                        0x7f, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, \
                                                        7, \
                                                        0x0003, 0x0030, 0x0003, 0x0040), \
    GOAL_SW_CTRL_EPL_ENTRY(GOAL_EPL_PATTERN_PREQ,       0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                                                        0x7f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                                                        1, \
                                                        0x0005, 0x0050, 0x0005, 0x080)

/* create function id enum from list */
#undef GOAL_SW_CTRL_EPL_ENTRY
#define GOAL_SW_CTRL_EPL_ENTRY(name, ...) name
typedef enum {
    GOAL_SW_CTRL_EPL_LIST,
    GOAL_EPL_PATTERN_USAGE_VAL_END  /**< End of list marker */
} GOAL_SW_PATTERN_ID;

/* configuration structure for switch hub and pattern */
typedef struct {
    uint8_t pattern[8];                         /**< pattern data */
    uint8_t mask[8];                            /**< mask data */
    int posNodeId;                              /**< node id position; -1 if no node ID needed */
    uint32_t streamId;                          /**< stream ID */
    uint32_t etId;                              /**< egress treament ID */
    uint32_t efmId;                             /**< egress frame modification ID */
    uint32_t fmdId;                             /**< frame data ID */
    uint32_t entryIdIPF;                        /**< entry id for IPF */
} GOAL_SW_EPL_CONFIG_T;

/* create hub configuration structure */
#undef GOAL_SW_CTRL_EPL_ENTRY
#define GOAL_SW_CTRL_EPL_ENTRY(name, p0, p1, p2, p3, p4, p5, p6, p7, m0, m1, m2, m3, m4, m5, m6, m7, posNodeId, sid, etid, efmid, fmdid) \
    { {p0, p1, p2, p3, p4, p5, p6, p7}, {m0, m1, m2, m3, m4, m5, m6, m7}, posNodeId, sid, etid, efmid, fmdid, GOAL_SW_NETC_ENTRY_NULL }

static GOAL_SW_EPL_CONFIG_T goal_swEplConfigTbl[] = {
    GOAL_SW_CTRL_EPL_LIST
};
#endif


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static NETC_BSP_INST_T netcBSP;
#if GOAL_CONFIG_EPL_EXTERN_BUF == 1
static netc_tb_fmd_update_config_t *pFmdUpdateConfig; /**< frame modification update config */
static GOAL_BOOL_T flgAutoEnable;               /**< autoreply enabled */
static GOAL_BOOL_T flgAutoPresEnable;           /**< autoreply enabled */
#endif


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/
#if GOAL_CONFIG_SWITCH_NXP_STATISTICS_LOG == 1
static const char *netcRxReason1String(
    uint32_t reason1                            /**< reson */
);

static const char *netcRxReason0String(
    uint32_t reason0                            /**< reson */
);

static const char *netcTxReason1String(
    uint32_t reason1                            /**< reson */
);

static const char *netcTxReason0String(
    uint32_t reason0                            /**< reson */
);

static const char *netcBridgeReason1String(
    uint32_t reason1                            /**< reson */
);

static const char *netcBridgeReason0String(
    uint32_t reason0                            /**< reson */
);
#endif

#if GOAL_CONFIG_CC_LINK_IE_TSN == 1
static GOAL_STATUS_T netcCclietsnFilterEnable(
    NETC_BSP_INST_T *pNetcBSP,                  /**< driver instance */
    uint32_t *pIpfEntryId                       /**< [out] IP filter entry id */
);
#endif


#if GOAL_CONFIG_EPL == 1
# if GOAL_CONFIG_EPL_EXTERN_BUF == 1
static GOAL_STATUS_T netcEplFilterEnable(
    NETC_BSP_INST_T *pNetcBSP,                  /**< driver instance */
    uint8_t *pFilterData,                       /**< filter data */
    uint8_t *pFilterMask,                       /**< filter mask */
    uint8_t filterLen,                          /**< filter length */
    uint32_t isStreamId,                        /**< is stream id */
    uint32_t *pIpfEntryId                       /**< [out] IP filter entry id */
);

static GOAL_STATUS_T netcEplFilterDisable(
    NETC_BSP_INST_T *pNetcBSP,                  /**< driver instance */
    uint32_t ipfEntryId                         /**< [out] IP filter entry id */
);

static GOAL_STATUS_T netcEplFilterAdd(
    NETC_BSP_INST_T *pNetcBSP,                  /**< driver instance */
    uint32_t isStreamId,                        /**< is stream id */
    uint32_t etId,                              /**< et stream id */
    uint32_t efmId,                             /**< efm stream id */
    uint32_t fmdId,                             /**< fmd stream id */
    uint32_t scId                               /**< optional stream count id or GOAL_SW_NETC_ENTRY_NULL */
);

static GOAL_STATUS_T netcEplAutoUpdate(
    NETC_BSP_INST_T *pNetcBSP,                  /**< driver instance */
    GOAL_SW_PATTERN_ID id,                      /**< pattern id */
    GOAL_BUFFER_T *pBuf                         /**< buffer */
);

static GOAL_STATUS_T goal_tgtEplRx(
    GOAL_BUFFER_T **ppBuf,                      /**< GOAL buffer */
    void *pArg                                  /**< callback argument */
);
# endif
#endif


/****************************************************************************/
/** Open BSP switch driver
 *
 */
GOAL_STATUS_T netcOpen(
    NETC_BSP_INST_T **ppNetcBSP,                /**< driver instance */
    void *pSwitchHandle                         /**< switch handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
#if GOAL_CONFIG_CC_LINK_IE_TSN == 1
    uint32_t num;                               /* filter number */
#endif

#if GOAL_CONFIG_SWITCH_NXP_STATISTICS_LOG == 1 && GOAL_CONFIG_LOGGING == 0
    /* make it compile with logging disabled */
    UNUSEDARG(netcRxReason0String);
    UNUSEDARG(netcRxReason1String);
    UNUSEDARG(netcTxReason0String);
    UNUSEDARG(netcTxReason1String);
    UNUSEDARG(netcBridgeReason0String);
    UNUSEDARG(netcBridgeReason1String);
#endif

#if GOAL_CONFIG_EPL_EXTERN_BUF == 1
    res = goal_memCalloc(&pFmdUpdateConfig, sizeof(netc_tb_fmd_update_config_t) + EPL_MAX_FRAME_LENGTH);
#endif

    if (res == GOAL_OK) {
        netcBSP.pSwitchHandle = pSwitchHandle;
        *ppNetcBSP = &netcBSP;
    }

#if GOAL_CONFIG_CC_LINK_IE_TSN == 1
    if (res == GOAL_OK) {
        res = netcCclietsnFilterEnable(*ppNetcBSP, &num);
    }
#endif

    if (GOAL_RES_OK(res)) {
        res = goal_lockCreate(GOAL_LOCK_BINARY, &netcBSP.pLockEpl, 0, 1, GOAL_ID);
    }

    return res;
}


/****************************************************************************/
/** Add entry to static mac address table
 *
 */
GOAL_STATUS_T netcMacTabAdd(
    NETC_BSP_INST_T *pNetcBSP,                  /**< driver instance */
    uint8_t forwardPorts,                       /**< forwarding ports */
    uint8_t *pMac                               /**< MAC address */
)
{
    status_t result = kStatus_Success;          /* bsp result */
    netc_tb_fdb_config_t fdbConfig;             /* fdb entry config */
    uint32_t entryId;                           /* fdb entry id */
    GOAL_STATUS_T res;                          /* result */

    res = goal_lockGet(pNetcBSP->pLockEpl, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(res)) {
        return GOAL_ERR_LOCK_GET_TIMEOUT;
    }

    /* set forwarding ports */
    fdbConfig.cfge.portBitmap = forwardPorts;
    /* set override ET_EID option */
    fdbConfig.cfge.oETEID = kNETC_MulitPortPackedETAccess;
    fdbConfig.cfge.ePort = 0;
    fdbConfig.cfge.iMirE = 0;
    fdbConfig.cfge.ctd = kNETC_FDBNoCTD;
    fdbConfig.cfge.dynamic = 0;
    fdbConfig.cfge.timeCapE = 0;
    fdbConfig.cfge.etEID = GOAL_SW_NETC_ENTRY_NULL;

    /* set mac address */
    GOAL_MEMCPY(&fdbConfig.keye.macAddr, pMac, 6);
    /* set fid */
    fdbConfig.keye.fid = 0;

    /* add entry */
    result = SWT_BridgeAddFDBTableEntry(pNetcBSP->pSwitchHandle, &fdbConfig, &entryId);

    goal_lockPut(pNetcBSP->pLockEpl);

    return goal_targetResToGoal(result);
}


/****************************************************************************/
/** delete entry from static mac address table
 *
 */
GOAL_STATUS_T netcMacTabDel(
    NETC_BSP_INST_T *pNetcBSP,                  /**< driver instance */
    uint8_t *pMac                               /**< MAC address */
)
{
    status_t status;                            /* sdk result */
    netc_tb_fdb_search_criteria_t searchCriteria; /* search criteria */
    netc_tb_fdb_rsp_data_t rsp;                 /* search response */
    GOAL_STATUS_T res;                          /* result */

    res = goal_lockGet(pNetcBSP->pLockEpl, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(res)) {
        return GOAL_ERR_LOCK_GET_TIMEOUT;
    }

    /* start search with NULL */
    GOAL_MEMSET(&searchCriteria, 0, sizeof(searchCriteria));
    GOAL_MEMCPY(&searchCriteria.keye.macAddr, pMac, 6);
    searchCriteria.resumeEntryId = GOAL_SW_NETC_ENTRY_NULL;

    /* perform query */
    status = SWT_BridgeSearchFDBTableEntry(pNetcBSP->pSwitchHandle, &searchCriteria, &rsp);
    if (status != kStatus_Success) {
        goal_lockPut(pNetcBSP->pLockEpl);
        return 1;
    }

    /* if found delete entry with given entryID */
    status = SWT_BridgeDelFDBTableEntry(pNetcBSP->pSwitchHandle, rsp.entryID);

    goal_lockPut(pNetcBSP->pLockEpl);

    return goal_targetResToGoal(status);
}


#if GOAL_CONFIG_SWITCH_NXP_STATISTICS_LOG == 1
static const char *netcRxReason0String(
    uint32_t reason0                            /**< reson */
)
{
    static char buffer[16];

    if (NETC_PORT_PRXDCRR0_ITEDR_MASK & reason0) {
        return "ITEDR : Invalid Table Entry Discard reason";
    }
    else if (NETC_PORT_PRXDCRR0_PCDR_MASK & reason0) {
        return "PCDR : Pre-Classification Discard Reason";
    }
    else if (NETC_PORT_PRXDCRR0_NODESTDR_MASK & reason0) {
        return "NODESTDR : No Destination Discard Reason";
    }
    else if (NETC_PORT_PRXDCRR0_FMMEDR_MASK & reason0) {
        return "FMMEDRT : Frame Modification Misconfiguration Error Discard Reason";
    }
    else {
        GOAL_SNPRINTF(&buffer[0], sizeof(buffer), "unknown: %"FMT_x32, reason0);
        return buffer;
    }
}


static const char *netcRxReason1String(
    uint32_t reason1                            /**< reson */
)
{
    static char buffer[64];
    uint32_t tableId;
    uint16_t entryId;

    const char *strTableName[] = {
        "Table Type None",
        "Ingress port filter table",
        "Rate Policer table",
        "Ingress Stream Identifier table",
        "Ingress Stream table",
        "5",
        "Ingress Sequence Generation table",
        "Stream Gating Instance table",
        "Frame modification table",
        "9",
        "10",
        "11",
        "12",
        "13",
        "14",
        "Congestion Management where ENTRYID represents Buffer Pool ID"
    };

    tableId = (reason1 & NETC_PORT_PRXDCRR1_TT_MASK) >> NETC_PORT_PRXDCRR1_TT_SHIFT;
    entryId = (reason1 & 0xffff);

    GOAL_SNPRINTF(&buffer[0], sizeof(buffer), "table %s, entryId: %d", strTableName[tableId], entryId);
    return buffer;
}


static const char *netcTxReason0String(
    uint32_t reason0                            /**< reson */
)
{
    static char buffer[16];

    if (NETC_PORT_PTXDCRR0_ITEDR_MASK & reason0) {
        return "ITEDR : Invalid Table Entry Discard reason";
    }
    else {
        GOAL_SNPRINTF(&buffer[0], sizeof(buffer), "unknown: %"FMT_x32, reason0);
        return buffer;
    }
}


static const char *netcTxReason1String(
    uint32_t reason1                            /**< reson */
)
{
    static char buffer[64];
    uint32_t tableId;
    uint16_t entryId;

    const char *strTableName[] = {
        "Table Type None",
        "Egress Treatment table",
        "Sequence Recovery table",
        "Egress Count table",
        "4",
        "5",
        "6",
        "7",
        "8",
        "9",
        "10",
        "11",
        "12",
        "13",
        "14",
        "15"
    };

    tableId = (reason1 & NETC_PORT_PTXDCRR1_TT_MASK) >> NETC_PORT_PTXDCRR1_TT_SHIFT;
    entryId = (reason1 & 0xffff);

    GOAL_SNPRINTF(&buffer[0], sizeof(buffer), "table %s, entryId: %d", strTableName[tableId], entryId);
    return buffer;
}

static const char *netcBridgeReason0String(
    uint32_t reason0                            /**< reson */
)
{
    static char buffer[16];

    if (NETC_PORT_BPDCRR0_NODESTDR_MASK & reason0) {
        return "NODESTDR : No Destination Discard reason";
    }
    else {
        GOAL_SNPRINTF(&buffer[0], sizeof(buffer), "unknown: %"FMT_x32, reason0);
        return buffer;
    }
}


static const char *netcBridgeReason1String(
    uint32_t reason1                            /**< reson */
)
{
    static char buffer[64];
    uint32_t tableId;
    uint16_t entryId;

    const char *strTableName[] = {
        "None",
        "VLAN Filter table",
        "FDB table",
        "L2 IPV MC Filter table - any source multicast",
        "L2 IPV MC Filter table - source specific multicast",
        "Rate Policer table due to storm control",
        "6",
        "7",
        "8",
        "9",
        "10",
        "11",
        "12",
        "13",
        "14",
        "15"
    };

    tableId = (reason1 & NETC_PORT_BPDCRR1_TT_SHIFT) >> NETC_PORT_BPDCRR1_TT_SHIFT;
    entryId = (reason1 & NETC_PORT_BPDCRR1_ENTRYID_MASK) >> NETC_PORT_BPDCRR1_ENTRYID_SHIFT;

    GOAL_SNPRINTF(&buffer[0], sizeof(buffer), "table %s, entryId: %d", strTableName[tableId], entryId);
    return buffer;
}

#endif


/****************************************************************************/
/** Dump FDB mac address table
 *
 */
void netcFDBTableDump(
    NETC_BSP_INST_T *pNetcBSP                   /**< driver instance */
)
{
    uint32_t nrRemain;                          /* number of remaining entries */
    uint32_t nrInUse;                           /* number of entries in fdb */
    status_t status;                            /* sdk result */
    netc_tb_fdb_search_criteria_t searchCriteria; /* search criteria */
    netc_tb_fdb_rsp_data_t rsp;                 /* search response */
    uint32_t index = 1;                         /* timeout */
    uint32_t resumeIdLast = 0;                  /* previous resume Id */
    netc_switch_inuse_fdb_statistic_t statistics; /* fdb statistics */
#if GOAL_CONFIG_SWITCH_NXP_STATISTICS_LOG == 1
    netc_hw_port_idx_t portIdx;                 /* port index */
    netc_port_discard_tpye_t discardType;       /* discard type */
    netc_port_discard_statistic_t discardStatistic; /* statistics structure */
    uint32_t entryId;                           /* statistics entry id */
    netc_tb_ec_stse_t ecStatistic;              /* ec statistics */
#endif
#if GOAL_CONFIG_SWITCH_NXP_STATISTICS_LOG == 1 && GOAL_CONFIG_EPL_EXTERN_BUF == 1
    uint64_t cntMatch;                          /* ipf match count */
    netc_tb_isc_stse_t iscStatistic;            /* isc statistics */
#endif

    /* without logging this variable is never read */
    UNUSEDARG(nrRemain);

    /* check number of entries in FDB */
    nrRemain = SWT_BridgeGetFDBTableRemainEntryNum(pNetcBSP->pSwitchHandle);
    status = SWT_GetFDBInUseEntriesNumber(pNetcBSP->pSwitchHandle, &statistics);
    if (status != kStatus_Success) {
        return;
    }

    nrInUse = statistics.staticEntries + statistics.camEntries + statistics.dynamicEntries + statistics.dynamicEntriesHWM;
    if (nrInUse == 0) {
        goal_logInfo("FDB MAC Address Table is EMPTY");
        return;
    }

    /* start search with NULL */
    GOAL_MEMSET(&searchCriteria, 0, sizeof(searchCriteria));
    searchCriteria.resumeEntryId = GOAL_SW_NETC_ENTRY_NULL;

    goal_logInfo("FDB MAC Address Table (free slots %"FMT_u32")", nrRemain);
    goal_logInfo("+-------+-------------------+------------+------------+--------+----------+");
    goal_logInfo("| index | MAC address       | forwarding | entry type | oETEID | etEID    |");
    goal_logInfo("+-------+-------------------+------------+------------+--------+----------+");
    do {
        /* remember last id to check for end of query */
        resumeIdLast = searchCriteria.resumeEntryId;

        /* perform query */
        status = SWT_BridgeSearchFDBTableEntry(pNetcBSP->pSwitchHandle, &searchCriteria, &rsp);
        if (status != kStatus_Success) {
            break;
        }

        /* update resume id for next query */
        searchCriteria.resumeEntryId = rsp.status;

        /* skip if this entry is last */
        if (resumeIdLast == searchCriteria.resumeEntryId) {
            break;
        }

        goal_logInfo("| %03"FMT_u32"   | %02x:%02x:%02x:%02x:%02x:%02x | "
                     "%04x       | %s    | %02x     | %08"FMT_x32" |",
                     index, rsp.keye.macAddr[0], rsp.keye.macAddr[1], rsp.keye.macAddr[2], rsp.keye.macAddr[3], rsp.keye.macAddr[4], rsp.keye.macAddr[5],
                     rsp.cfge.portBitmap, ((rsp.cfge.dynamic == 0) ? "static " : "dynamic"), rsp.cfge.oETEID, rsp.cfge.etEID);
        index++;

    }
    while (index < 1024);

    goal_logInfo("+-------+-------------------+------------+------------+--------+----------+");

#if GOAL_CONFIG_SWITCH_NXP_STATISTICS_LOG == 1
    goal_logInfo("kNETC_RxDisard");
    discardType = kNETC_RxDiscard;
    for (portIdx = 0; portIdx < 5; portIdx++) {
        SWT_GetPortDiscardStatistic(pNetcBSP->pSwitchHandle, portIdx, discardType, &discardStatistic);
        if (discardStatistic.count) {
            goal_logInfo("Port: %"FMT_u32", count:   %"FMT_x32, (uint32_t) portIdx, discardStatistic.count);
            goal_logInfo("Port: %"FMT_u32", reason0: %s", (uint32_t) portIdx, netcRxReason0String(discardStatistic.reason0));
            goal_logInfo("Port: %"FMT_u32", reason1: %s", (uint32_t) portIdx, netcRxReason1String(discardStatistic.reason1));
        }
        SWT_ClearPortDiscardReason(pNetcBSP->pSwitchHandle, portIdx, discardType, discardStatistic.reason0, discardStatistic.reason1);
    }

    goal_logInfo("kNETC_TxDisard");
    discardType = kNETC_TxDiscard;
    for (portIdx = 0; portIdx < 5; portIdx++) {
        SWT_GetPortDiscardStatistic(pNetcBSP->pSwitchHandle, portIdx, discardType, &discardStatistic);
        if (discardStatistic.count) {
            goal_logInfo("Port: %"FMT_u32", count:   %"FMT_x32, (uint32_t) portIdx, discardStatistic.count);
            goal_logInfo("Port: %"FMT_u32", reason0: %s", (uint32_t) portIdx, netcTxReason0String(discardStatistic.reason0));
            goal_logInfo("Port: %"FMT_u32", reason1: %s", (uint32_t) portIdx, netcTxReason1String(discardStatistic.reason1));
        }
        SWT_ClearPortDiscardReason(pNetcBSP->pSwitchHandle, portIdx, discardType, discardStatistic.reason0, discardStatistic.reason1);
    }

    goal_logInfo("kNETC_BridgeDiscard");
    discardType = kNETC_BridgeDiscard;
    for (portIdx = 0; portIdx < 5; portIdx++) {
        SWT_GetPortDiscardStatistic(pNetcBSP->pSwitchHandle, portIdx, discardType, &discardStatistic);
        if (discardStatistic.count) {
            goal_logInfo("Port: %"FMT_u32", count:   %"FMT_x32, (uint32_t) portIdx, discardStatistic.count);
            goal_logInfo("Port: %"FMT_u32", reason0: %s", (uint32_t) portIdx, netcBridgeReason0String(discardStatistic.reason0));
            goal_logInfo("Port: %"FMT_u32", reason1: %s", (uint32_t) portIdx, netcBridgeReason1String(discardStatistic.reason1));
        }
        SWT_ClearPortDiscardReason(pNetcBSP->pSwitchHandle, portIdx, discardType, discardStatistic.reason0, discardStatistic.reason1);
    }
    entryId = SWT_GetECEntryNum(pNetcBSP->pSwitchHandle);
    status = SWT_GetECStatistic(pNetcBSP->pSwitchHandle, entryId, &ecStatistic);
    if (status == kStatus_Success) {
        goal_logInfo("enqFrameCount: %"FMT_u32", %"FMT_u32, ecStatistic.enqFrmCnt[0], ecStatistic.enqFrmCnt[1]);
        goal_logInfo("reqFrameCount: %"FMT_u32", %"FMT_u32, ecStatistic.rejFrmCnt[0], ecStatistic.rejFrmCnt[1]);
    }

# if GOAL_CONFIG_EPL_EXTERN_BUF == 1
    status = SWT_RxPSFPGetISCStatistic(pNetcBSP->pSwitchHandle, ISC_ENTRY_ID_ASND, &iscStatistic);
    if (status == kStatus_Success) {
        goal_logInfo("ISC_ENTRY_ID_ASND");
        goal_logInfo("isc: rxCount: %"FMT_u32, iscStatistic.rxCount);
        goal_logInfo("isc: msduDropCount: %"FMT_u32, iscStatistic.msduDropCount);
        goal_logInfo("isc: policerDropCount: %"FMT_u32, iscStatistic.policerDropCount);
        goal_logInfo("isc: sgDropCount: %"FMT_u32, iscStatistic.sgDropCount);

        SWT_RxPSFPResetISCStatistic(pNetcBSP->pSwitchHandle, ISC_ENTRY_ID_ASND);
    }

    status = SWT_RxPSFPGetISCStatistic(pNetcBSP->pSwitchHandle, ISC_ENTRY_ID_PRES, &iscStatistic);
    if (status == kStatus_Success) {
        goal_logInfo("ISC_ENTRY_ID_PRES");
        goal_logInfo("isc: rxCount: %"FMT_u32, iscStatistic.rxCount);
        goal_logInfo("isc: msduDropCount: %"FMT_u32, iscStatistic.msduDropCount);
        goal_logInfo("isc: policerDropCount: %"FMT_u32, iscStatistic.policerDropCount);
        goal_logInfo("isc: sgDropCount: %"FMT_u32, iscStatistic.sgDropCount);

        SWT_RxPSFPResetISCStatistic(pNetcBSP->pSwitchHandle, ISC_ENTRY_ID_PRES);
    }

    for (entryId = 0; entryId < 1024; entryId++) {
        status = SWT_RxIPFGetMatchedCount(pNetcBSP->pSwitchHandle, entryId, &cntMatch);
        if (cntMatch && status == kStatus_Success) {
            goal_logInfo("IPF match (%"FMT_u32"): %"FMT_u64, entryId, cntMatch);
            SWT_RxIPFResetMatchCounter(pNetcBSP->pSwitchHandle, entryId);
        }
    }
# endif

#endif
}


/****************************************************************************/
/** Dump dynamic mac address table
 *
 */
void netcL2MCTableDump(
    NETC_BSP_INST_T *pNetcBSP                   /**< driver instance */
)
{
    uint32_t nrRemain;                          /* number of remaining entries */
    status_t status;                            /* sdk result */
    netc_tb_l2mcf_search_criteria_t searchCriteria; /* search criteria */
    netc_tb_l2mcf_rsp_data_t rsp;               /* search response */
    uint32_t index = 1;                         /* timeout */
    uint32_t resumeIdLast = 0;                  /* previous resume Id */

    /* without logging this variable is never read */
    UNUSEDARG(nrRemain);

    /* check number of entries in L2MCF-Table */
    nrRemain = SWT_BridgeGetL2MCFTableRemainEntryNum(pNetcBSP->pSwitchHandle);

    /* start search with NULL */
    GOAL_MEMSET(&searchCriteria, 0, sizeof(searchCriteria));
    searchCriteria.resumeEntryId = GOAL_SW_NETC_ENTRY_NULL;

    goal_logInfo("L2MC MAC Address Table (free slots %"FMT_u32")", nrRemain);
    goal_logInfo("+-------+-------------------+");
    goal_logInfo("| index | IP addresses      |");
    goal_logInfo("+-------+-------------------+");
    do {
        /* remember last id to check for end of query */
        resumeIdLast = searchCriteria.resumeEntryId;

        /* perform query */
        status = SWT_BridgeSearchL2MCFTableEntry(pNetcBSP->pSwitchHandle, &searchCriteria, &rsp);
        if (status != kStatus_Success) {
            break;
        }

        /* update resume id for next query */
        searchCriteria.resumeEntryId = rsp.status;

        /* skip if this entry is last */
        if (resumeIdLast == searchCriteria.resumeEntryId) {
            break;
        }

        goal_logInfo("| %03"FMT_u32"   | %"FMT_x32", %"FMT_x32" |",
                index, rsp.keye.ipv4DestAddr, rsp.keye.ipv4SrcAddr);
        index++;

    }
    while (index < 1024);

    goal_logInfo("+-------+-------------------+------------+------------+");
}


#if GOAL_CONFIG_EPL == 1
/****************************************************************************/
/** Dump dynamic mac address table
 *
 */
GOAL_STATUS_T netcEplSetup(
    NETC_BSP_INST_T *pNetcBSP,                  /**< driver instance */
    uint8_t portEnableMask                      /**< port enable mask */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
#if GOAL_CONFIG_EPL_EXTERN_BUF == 1
    uint32_t cntFilter;                         /* filter iterator */
#endif

    pNetcBSP->portEnableMask = portEnableMask;

#if GOAL_CONFIG_EPL_EXTERN_BUF == 1

    for (cntFilter = 0; cntFilter < GOAL_EPL_PATTERN_USAGE_VAL_END; cntFilter++) {
        res = netcEplFilterAdd(
                pNetcBSP,
                goal_swEplConfigTbl[cntFilter].streamId,
                goal_swEplConfigTbl[cntFilter].etId,
                goal_swEplConfigTbl[cntFilter].efmId,
                goal_swEplConfigTbl[cntFilter].fmdId,
                GOAL_SW_NETC_ENTRY_NULL);

        if (GOAL_RES_ERR(res)) {
            goal_logErr("error configuring EPL filters");
        }
    }

    /* register EtherType in GOAL before actual handler of EPL stack */
    if (GOAL_RES_OK(res)) {
        res = goal_ethProtoAddPos(GOAL_TRUE, (PROTOCOL_EPL_B0 << 8) | PROTOCOL_EPL_B1, NULL, goal_tgtEplRx, NULL, GOAL_TRUE);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to register RX handler for low priority frames");
        }
    }
#else
    UNUSEDARG(goal_swEplConfigTbl);
#endif

    return res;
}
#endif


#if GOAL_CONFIG_EPL_EXTERN_BUF == 1
/****************************************************************************/
/** Update the autoreply buffer for poll response in the Switch
 *
 */
GOAL_STATUS_T netcEplPresUpdate(
    NETC_BSP_INST_T *pNetcBSP,                  /**< driver instance */
    GOAL_BUFFER_T *pBuf                         /**< buffer */
)
{
    return netcEplAutoUpdate(pNetcBSP, GOAL_EPL_PATTERN_PREQ, pBuf);
}


/****************************************************************************/
/** Update the autoreply buffer for ident response in the Switch
 *
 */
GOAL_STATUS_T netcEplIdentUpdate(
    NETC_BSP_INST_T *pNetcBSP,                  /**< driver instance */
    GOAL_BUFFER_T *pBuf                         /**< buffer */
)
{
    return netcEplAutoUpdate(pNetcBSP, GOAL_EPL_PATTERN_SOA_IDENT, pBuf);
}


/****************************************************************************/
/** Update the autoreply buffer for status response in the Switch
 *
 */
GOAL_STATUS_T netcEplStatusUpdate(
    NETC_BSP_INST_T *pNetcBSP,                  /**< driver instance */
    GOAL_BUFFER_T *pBuf                         /**< buffer */
)
{
    return netcEplAutoUpdate(pNetcBSP, GOAL_EPL_PATTERN_SOA_STATUS, pBuf);
}


/****************************************************************************/
/** Update the autoreply buffer for status response in the Switch
 *
 */
GOAL_STATUS_T netcEplUnspecUpdate(
    NETC_BSP_INST_T *pNetcBSP,                  /**< driver instance */
    GOAL_BUFFER_T *pBuf                         /**< buffer */
)
{
    return netcEplAutoUpdate(pNetcBSP, GOAL_EPL_PATTERN_SOA_UNSPEC, pBuf);
}


/****************************************************************************/
/** Update the autoreply buffer for poll response in the Switch
 *
 */
static GOAL_STATUS_T netcEplAutoUpdate(
    NETC_BSP_INST_T *pNetcBSP,                  /**< driver instance */
    GOAL_SW_PATTERN_ID id,                      /**< pattern id */
    GOAL_BUFFER_T *pBuf                         /**< buffer */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    status_t resSdk;                            /* SDK result */
    netc_tb_fm_config_t fmEntryConfig;          /* frame modification config */

    res = goal_lockGet(pNetcBSP->pLockEpl, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(res)) {
        return GOAL_ERR_LOCK_GET_TIMEOUT;
    }

    GOAL_MEMSET(&fmEntryConfig, 0, sizeof(fmEntryConfig));
    fmEntryConfig.entryID = goal_swEplConfigTbl[id].efmId;               /* this is the entry id of the frame modification table entry */
    fmEntryConfig.cfge.l2Act = kNETC_UseSpecPlayload; /* replace whole payload */
    fmEntryConfig.cfge.fmdBytes = pBuf->dataLen; /* modification length */
    fmEntryConfig.cfge.fmdEID = goal_swEplConfigTbl[id].fmdId;           /* this is the entry id for the frame data table entry */
    resSdk = SWT_FMUpdateTableEntry(pNetcBSP->pSwitchHandle, &fmEntryConfig);
    if (resSdk != kStatus_Success) {
        res = GOAL_ERROR;
    }


    GOAL_MEMSET(pFmdUpdateConfig, 0, sizeof(netc_tb_fmd_update_config_t));
    pFmdUpdateConfig->entryID = goal_swEplConfigTbl[id].fmdId;
    GOAL_MEMCPY(pFmdUpdateConfig->cfge, pBuf->ptrData, pBuf->dataLen);

    resSdk = SWT_FMDUpdateTableEntry(pNetcBSP->pSwitchHandle, pFmdUpdateConfig, sizeof(netc_tb_fmd_update_config_t) + pBuf->dataLen);
    if (resSdk != kStatus_Success) {
        res = GOAL_ERROR;
    }

    goal_lockPut(pNetcBSP->pLockEpl);

    return res;
}


/****************************************************************************/
/** Enable autoreply filter
 *
 */
static GOAL_STATUS_T netcEplFilterEnable(
    NETC_BSP_INST_T *pNetcBSP,                  /**< driver instance */
    uint8_t *pFilterData,                       /**< filter data */
    uint8_t *pFilterMask,                       /**< filter mask */
    uint8_t filterLen,                          /**< filter length */
    uint32_t isStreamId,                        /**< is stream id */
    uint32_t *pIpfEntryId                       /**< [out] IP filter entry id */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    netc_tb_ipf_config_t ipfConfig;             /* IPF config */
    uint16_t cntFilter;                         /* filter counter */
    status_t resSdk;                            /* SDK result */

    res = goal_lockGet(pNetcBSP->pLockEpl, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(res)) {
        return GOAL_ERR_LOCK_GET_TIMEOUT;
    }

    /* add IPF entry to associate EPL frame ident request with stream id ISE_EGRESS_STREAM_ID */
    GOAL_MEMSET(&ipfConfig, 0, sizeof(ipfConfig));
    /* filter for unicast, thus our own mac address */
    /* filter for ethertype of POWERLINK */
    ipfConfig.keye.etherType = GOAL_htobe16(0x88ab);
    ipfConfig.keye.etherTypeMask = 0xffff;

    /* set filter parameters */
    for (cntFilter = 0; cntFilter < filterLen; cntFilter++) {
        ipfConfig.keye.payload[cntFilter].data = pFilterData[cntFilter];
        ipfConfig.keye.payload[cntFilter].mask = pFilterMask[cntFilter];
    }

    /* configure stream */
    ipfConfig.keye.precedence = 80;
    ipfConfig.cfge.hr = kNETC_SoftwareDefHR0;

    if (goal_swEplConfigTbl[GOAL_EPL_PATTERN_SOA_UNSPEC].streamId == isStreamId) {
        ipfConfig.cfge.hr = kNETC_SoftwareDefHR7;
    }

    ipfConfig.cfge.fltfa = kNETC_IPFForwardPermit;
    ipfConfig.cfge.flta = kNETC_IPFWithIngressStream;
    ipfConfig.cfge.fltaTgt = isStreamId;
    ipfConfig.cfge.rpr = 0x00;
    resSdk = SWT_RxIPFAddTableEntry(pNetcBSP->pSwitchHandle, &ipfConfig, pIpfEntryId);
    if (kStatus_Success != resSdk) {
        res = GOAL_ERROR;
    }

    goal_lockPut(pNetcBSP->pLockEpl);

    return res;
}


/****************************************************************************/
/** Disable autoreply filter
 *
 */
static GOAL_STATUS_T netcEplFilterDisable(
    NETC_BSP_INST_T *pNetcBSP,                  /**< driver instance */
    uint32_t ipfEntryId                         /**< [out] IP filter entry id */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    status_t resSdk;                            /* SDK result */

    res = goal_lockGet(pNetcBSP->pLockEpl, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(res)) {
        return GOAL_ERR_LOCK_GET_TIMEOUT;
    }

    resSdk = SWT_RxIPFDelTableEntry(pNetcBSP->pSwitchHandle, ipfEntryId);
    if (kStatus_Success != resSdk) {
        res = GOAL_ERROR;
    }

    goal_lockPut(pNetcBSP->pLockEpl);

    return res;
}


/****************************************************************************/
/** Configure IPF filter for POWERLINK autoreply
 *
 */
static GOAL_STATUS_T netcEplFilterAdd(
    NETC_BSP_INST_T *pNetcBSP,                  /**< driver instance */
    uint32_t isStreamId,                        /**< is stream id */
    uint32_t etId,                              /**< et stream id */
    uint32_t efmId,                             /**< efm stream id */
    uint32_t fmdId,                             /**< fmd stream id */
    uint32_t scId                               /**< stream count id */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    netc_tb_is_config_t isEntryConfig;          /* ingress stream config */
    netc_tb_et_config_t etEntryConfig;          /* egress treatment config */
    netc_tb_fm_config_t fmEntryConfig;          /* frame modification config */
    status_t resSdk;                            /* SDK result */

    res = goal_lockGet(pNetcBSP->pLockEpl, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(res)) {
        return GOAL_ERR_LOCK_GET_TIMEOUT;
    }

#if GOAL_CONFIG_LOGGING == 1
    uint32_t words;                             /* word number */

    /* get number of available words */
    words = SWT_FMDGetTableWordsNum(pNetcBSP->pSwitchHandle);
    goal_logInfo("FMD size is : %"FMT_u32, words * 24);
#endif

    /* create a ingress stream for processing/forwarding */
    GOAL_MEMSET(&isEntryConfig, 0, sizeof(isEntryConfig));
    isEntryConfig.entryID = isStreamId;      /* stream id for specific treatment */
    isEntryConfig.cfge.sfe = false;              /* enable filtering */
    isEntryConfig.cfge.ctd = false;
    isEntryConfig.cfge.isqa = kNETC_ISNotPerformFRER;
    isEntryConfig.cfge.sduType = kNETC_PDU;
    isEntryConfig.cfge.isqEID = GOAL_SW_NETC_ENTRY_NULL;
    isEntryConfig.cfge.rpEID = GOAL_SW_NETC_ENTRY_NULL;
    isEntryConfig.cfge.sgiEID = GOAL_SW_NETC_ENTRY_NULL;
    isEntryConfig.cfge.ifmEID = GOAL_SW_NETC_ENTRY_NULL;
    isEntryConfig.cfge.etEID = etId;            /* this is the entry id of the egress treatment table entry */
    isEntryConfig.cfge.eport = 0x0;             /* unused in this mode */
    isEntryConfig.cfge.oETEID = kNETC_MulitPortAbsETAccess; /* this specifies single port egress treatment */
    isEntryConfig.cfge.ePortBitmap = 0x10 | 0x02 | 0x08;      /* only forward to ingress port and egress port (port 0x02 and 0x08)  and the host */
    isEntryConfig.cfge.ePortBitmap = pNetcBSP->portEnableMask;
    isEntryConfig.cfge.iscEID = scId;   /* stream count entry */
    /*
     * enabling source port pruning globally and disabling it for this stream does not work.
     * fa needs to be 2 or 4 (kNETC_ISStreamForward, kNETC_ISCopyToMgmtPortAndStream)
     */
    isEntryConfig.cfge.sppd = true;             /* disable source port pruning for this frame */
    isEntryConfig.cfge.fa = kNETC_ISStreamForward;

    resSdk = SWT_RxPSFPAddISTableEntry(pNetcBSP->pSwitchHandle, &isEntryConfig);
    if (resSdk != kStatus_Success) {
        res = GOAL_ERROR;
    }

    /* create a egress treatment table entry for port 0 */
    GOAL_MEMSET(&etEntryConfig, 0, sizeof(etEntryConfig));
    etEntryConfig.entryID = etId;               /* this is the entry id of the egress treatment table entry */
    etEntryConfig.cfge.efmEID = efmId;           /* this is the entry id of the frame modification table entry */
    resSdk = SWT_TxEPPAddETTableEntry(pNetcBSP->pSwitchHandle, &etEntryConfig);
    if (resSdk != kStatus_Success) {
        res = GOAL_ERROR;
    }

    /* create a egress treatment table entry for port 1 */
    GOAL_MEMSET(&etEntryConfig, 0, sizeof(etEntryConfig));
    etEntryConfig.entryID = etId + 1;               /* this is the entry id of the egress treatment table entry */
    etEntryConfig.cfge.efmEID = efmId;           /* this is the entry id of the frame modification table entry */
    resSdk = SWT_TxEPPAddETTableEntry(pNetcBSP->pSwitchHandle, &etEntryConfig);
    if (resSdk != kStatus_Success) {
        res = GOAL_ERROR;
    }

    /* create a egress treatment table entry for port 2 */
    GOAL_MEMSET(&etEntryConfig, 0, sizeof(etEntryConfig));
    etEntryConfig.entryID = etId + 2;               /* this is the entry id of the egress treatment table entry */
    etEntryConfig.cfge.efmEID = efmId;           /* this is the entry id of the frame modification table entry */
    resSdk = SWT_TxEPPAddETTableEntry(pNetcBSP->pSwitchHandle, &etEntryConfig);
    if (resSdk != kStatus_Success) {
        res = GOAL_ERROR;
    }
    /* create a egress treatment table entry for port 3 */
    GOAL_MEMSET(&etEntryConfig, 0, sizeof(etEntryConfig));
    etEntryConfig.entryID = etId + 3;               /* this is the entry id of the egress treatment table entry */
    etEntryConfig.cfge.efmEID = efmId;           /* this is the entry id of the frame modification table entry */
    resSdk = SWT_TxEPPAddETTableEntry(pNetcBSP->pSwitchHandle, &etEntryConfig);
    if (resSdk != kStatus_Success) {
        res = GOAL_ERROR;
    }

    /* create a egress treatment table entry for port 4 */
    GOAL_MEMSET(&etEntryConfig, 0, sizeof(etEntryConfig));
    etEntryConfig.entryID = etId + 4;               /* this is the entry id of the egress treatment table entry */
    etEntryConfig.cfge.efmEID = 0xffffffff;           /* this is the entry id of the frame modification table entry */
    resSdk = SWT_TxEPPAddETTableEntry(pNetcBSP->pSwitchHandle, &etEntryConfig);
    if (resSdk != kStatus_Success) {
        res = GOAL_ERROR;
    }

    /* create a frame modification entry */
    GOAL_MEMSET(&fmEntryConfig, 0, sizeof(fmEntryConfig));
    fmEntryConfig.entryID = efmId;               /* this is the entry id of the frame modification table entry */
    fmEntryConfig.cfge.l2Act = kNETC_UseSpecPlayload; /* replace whole payload */
    fmEntryConfig.cfge.fmdBytes = EPL_MAX_FRAME_LENGTH; /* modification length */
    fmEntryConfig.cfge.fmdEID = fmdId;           /* this is the entry id for the frame data table entry */
    resSdk = SWT_FMAddTableEntry(pNetcBSP->pSwitchHandle, &fmEntryConfig);
    if (resSdk != kStatus_Success) {
        res = GOAL_ERROR;
    }

    goal_lockPut(pNetcBSP->pLockEpl);

    return res;
}


/****************************************************************************/
/** Enable POWERLINK mode
 *
 */
GOAL_STATUS_T netcEplEnableMode(
    NETC_BSP_INST_T *pNetcBSP                   /**< driver instance */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* return value */
    uint32_t cntFilter;                         /* filter iterator */
    uint32_t entryId;                           /* entry id */

    goal_logInfo("enabling EPL mode");

    if (flgAutoEnable == GOAL_TRUE) {
        return GOAL_OK;
    }

    res = goal_lockGet(pNetcBSP->pLockEpl, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(res)) {
        return GOAL_ERR_LOCK_GET_TIMEOUT;
    }

    for (cntFilter = 0; GOAL_RES_OK(res) && cntFilter < GOAL_EPL_PATTERN_USAGE_VAL_END; cntFilter++) {

        if (cntFilter != GOAL_EPL_PATTERN_PREQ) {
            res = netcEplFilterEnable(
                pNetcBSP,
                goal_swEplConfigTbl[cntFilter].pattern,
                goal_swEplConfigTbl[cntFilter].mask,
                sizeof(goal_swEplConfigTbl[cntFilter].pattern),
                goal_swEplConfigTbl[cntFilter].streamId,
                &entryId);

            if (GOAL_RES_OK(res)) {
                goal_swEplConfigTbl[cntFilter].entryIdIPF = entryId;
            }
        }
    }

    flgAutoEnable = GOAL_TRUE;

    goal_lockPut(pNetcBSP->pLockEpl);

    return res;
}


/****************************************************************************/
/** Disable POWERLINK mode
 *
 */
GOAL_STATUS_T netcEplDisableMode(
    NETC_BSP_INST_T *pNetcBSP                   /**< driver instance */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* return value */
    uint32_t cntFilter;                         /* filter iterator */

    goal_logInfo("disabling EPL mode");

    if (flgAutoEnable == GOAL_FALSE) {
        return GOAL_OK;
    }

    res = goal_lockGet(pNetcBSP->pLockEpl, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(res)) {
        return GOAL_ERR_LOCK_GET_TIMEOUT;
    }

    for (cntFilter = 0; GOAL_RES_OK(res) && cntFilter < GOAL_EPL_PATTERN_USAGE_VAL_END; cntFilter++) {
        if (cntFilter != GOAL_EPL_PATTERN_PREQ) {
            res = netcEplFilterDisable(
                pNetcBSP,
                goal_swEplConfigTbl[cntFilter].entryIdIPF);
        }
    }

    flgAutoEnable = GOAL_FALSE;

    goal_lockPut(pNetcBSP->pLockEpl);

    return res;
}


/****************************************************************************/
/** Disable/Enable POWERLINK auto reply
 *
 */
GOAL_STATUS_T netcEplAutoreplySet(
    NETC_BSP_INST_T *pNetcBSP,                  /**< driver instance */
    GOAL_BOOL_T flagActive                      /**< active flag */
)
{
    netc_port_parser_config_t config;           /* config structure */
    uint32_t portIdx;                           /* port iterator */
    NETC_PORT_Type *port;                       /* ethernet port handle */
    GOAL_STATUS_T res;                          /* result */

    res = goal_lockGet(pNetcBSP->pLockEpl, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(res)) {
        return GOAL_ERR_LOCK_GET_TIMEOUT;
    }

    GOAL_MEMSET(&config, 0, sizeof(netc_port_parser_config_t));
    if (flagActive) {
        config.l2PloadCount = 8;
        config.l3PayloadCount = 0;
        config.l4PayloadCount = 0;
        config.enableL3Parser = false;
        config.enableL4Parser = false;
    }

    for (portIdx = 0; portIdx < 5U; portIdx++) {
        port = pNetcBSP->pSwitchHandle->hw.ports[portIdx].port;
        NETC_PortSetParser(port, &config);
    }

    goal_lockPut(pNetcBSP->pLockEpl);

    return GOAL_OK;
}


/****************************************************************************/
/** Disable/Enable POWERLINK Preq/Pres auto reply
 *
 */
GOAL_STATUS_T netcEplPresAutoreplySet(
    NETC_BSP_INST_T *pNetcBSP,                  /**< driver instance */
    GOAL_BOOL_T flagActive                      /**< active flag */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* return value */
    uint32_t cntFilter;                         /* filter iterator */
    uint32_t entryId;                           /* entry id */

    if (flagActive == flgAutoPresEnable) {
        return GOAL_OK;
    }

    res = goal_lockGet(pNetcBSP->pLockEpl, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(res)) {
        return GOAL_ERR_LOCK_GET_TIMEOUT;
    }

    cntFilter = GOAL_EPL_PATTERN_PREQ;
    if (flagActive) {
        res = netcEplFilterEnable(
            pNetcBSP,
            goal_swEplConfigTbl[cntFilter].pattern,
            goal_swEplConfigTbl[cntFilter].mask,
            sizeof(goal_swEplConfigTbl[cntFilter].pattern),
            goal_swEplConfigTbl[cntFilter].streamId,
            &entryId);

        if (GOAL_RES_OK(res)) {
            goal_swEplConfigTbl[cntFilter].entryIdIPF = entryId;
        }
    }
    else {
        res = netcEplFilterDisable(
            pNetcBSP,
            goal_swEplConfigTbl[cntFilter].entryIdIPF);
    }

    flgAutoPresEnable = flagActive;

    goal_lockPut(pNetcBSP->pLockEpl);

    return res;
}


/****************************************************************************/
/** Configure nodeId in filter patterns
 *
 */
GOAL_STATUS_T netcEplUpdateNodeId(
    NETC_BSP_INST_T *pNetcBSP,                  /**< driver instance */
    uint8_t nodeId                              /**< nodeId */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* return value */
    uint32_t cntFilter;                         /* filter iterator */

    UNUSEDARG(pNetcBSP);

    res = goal_lockGet(pNetcBSP->pLockEpl, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(res)) {
        return GOAL_ERR_LOCK_GET_TIMEOUT;
    }

    for (cntFilter = 0; GOAL_RES_OK(res) && cntFilter < GOAL_EPL_PATTERN_USAGE_VAL_END; cntFilter++) {
        goal_swEplConfigTbl[cntFilter].pattern[goal_swEplConfigTbl[cntFilter].posNodeId] = nodeId;
    }

    goal_lockPut(pNetcBSP->pLockEpl);

    return res;
}


/****************************************************************************/
/** Ethernet POWERLINK handler for high priority frames
 *
 * @retval GOAL_OK handled
 * @retval other unhandled
 */
static GOAL_STATUS_T goal_tgtEplRx(
    GOAL_BUFFER_T **ppBuf,                      /**< GOAL buffer */
    void *pArg                                  /**< callback argument */
)
{
    UNSIGNED8 bMsgType = 0;                     /* EPL message type identifier */
    UNSIGNED8 bReqTarget = 0;                   /* EPL requested node id */
    UNSIGNED8 bReqSid = 0;                      /* EPL requested service id */
    uint8_t *pBuffer;                           /* frame data */

    UNUSEDARG(pArg);

    pBuffer = (*ppBuf)->ptrData;

    bMsgType = pBuffer[14];
    bReqTarget = pBuffer[21];
    bReqSid = pBuffer[20];

    if ((bMsgType == EPLMSG_TYPE_SOA) && (bReqTarget == eplNodeId) && (bReqSid == ASYNC_UNSPEC_REQ)) {
        goal_eplUnspecTransmitCb();
    }

    /* return ERROR so frame is handles later */
    return GOAL_ERROR;
}
#endif


#if GOAL_CONFIG_CC_LINK_IE_TSN == 1
/****************************************************************************/
/** Enable CC-Link IE TSN Detection filter
 *
 * Detection broadcast frame must only be forwarded to the host port.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T netcCclietsnFilterEnable(
    NETC_BSP_INST_T *pNetcBSP,                  /**< driver instance */
    uint32_t *pIpfEntryId                       /**< [out] IP filter entry id */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    netc_tb_ipf_config_t ipfConfig;             /* IPF config */
    status_t resSdk;                            /* SDK result */

    uint8_t mac[MAC_ADDR_LEN] = GOAL_BROADCAST_MAC_ADDR;

    GOAL_MEMSET(&ipfConfig, 0, sizeof(ipfConfig));

    /* DMAC is broadcast */
    GOAL_MEMCPY(ipfConfig.keye.dmac, mac, MAC_ADDR_LEN);
    GOAL_MEMSET(ipfConfig.keye.dmacMask, 0xff, MAC_ADDR_LEN);

    /* EtherType CCLIETSN */
    ipfConfig.keye.etherType = GOAL_htobe16(GOAL_DRV_SWITCH_CCLIETSN_ETYPE);
    ipfConfig.keye.etherTypeMask = 0xffff;

    /* set frame type */
    ipfConfig.keye.payload[0].data = GOAL_DRV_SWITCH_CCLIETSN_CMD_DETECTION;
    ipfConfig.keye.payload[0].mask = 0xff;
    ipfConfig.keye.precedence = GOAL_DRV_SWITCH_CCLIETSN_PRECEDENCE;

    /* on match: redirect frame to Management port */
    ipfConfig.cfge.hr = kNETC_SoftwareDefHR0;
    ipfConfig.cfge.fltfa = kNETC_IPFRedirectToMgmtPort;
    ipfConfig.cfge.flta = kNETC_IPFNoAction;

    resSdk = SWT_RxIPFAddTableEntry(pNetcBSP->pSwitchHandle, &ipfConfig, pIpfEntryId);
    if (kStatus_Success != resSdk) {
        res = GOAL_ERROR;
    }
    return res;
}
#endif /* GOAL_CONFIG_CC_LINK_IE_TSN */
