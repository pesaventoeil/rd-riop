/** @file
 *
 * @brief Generic Media Adapter for Ethernet Frame Handling
 *
 * @details
 * This module implements generic handling functions for Ethernet frames. Its
 * functions manage Ethernet Instances with assigned Media Adapter and
 * registered drivers for e. g. Ethernet or EoE. The Instances contains
 * Ethernet Queues that can be used to prioritize specific Frame types.
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

#define GOAL_ID GOAL_ID_ETH

#include <goal_includes.h>
#include "cm/goal_eth_cm.h"


/****************************************************************************/
/* Constants */
/****************************************************************************/
#define GOAL_ETH_CM_LINK_MASK_SIZE       (2)
#define GOAL_ETH_CM_SPEED_MASK_SIZE      (3)
#define GOAL_ETH_CM_DUPLEX_MASK_SIZE     (2)

#define GOAL_ETH_CM_SPEED_10             (2)
#define GOAL_ETH_CM_SPEED_100            (3)
#define GOAL_ETH_CM_SPEED_1000           (4)

#define GOAL_ETH_CM_LINK_MASK            (3)
#define GOAL_ETH_CM_SPEED_MASK           (7)
#define GOAL_ETH_CM_DUPLEX_MASK          (3)

#if GOAL_CONFIG_DD != 1
#define goal_miEthCmLinkStateSet(...)
#define goal_miEthCmDuplexSet(...)
#define goal_miEthCmSpeedSet(...)
#endif /* GOAL_CONFIG_DD */


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_INSTANCE_LIST_T *mpListInst;        /**< mi eth instance list */
static GOAL_STAGE_HANDLER_T stageInit;          /**< init stage handler */
static GOAL_STAGE_HANDLER_T stageShutdown;      /**< shutdown stage handler */
static GOAL_MA_ETH_CFG_T mCfg;                  /**< module global config */
static GOAL_BOOL_T mFlgCfgSet = GOAL_FALSE;     /**< user specified config flag */
static uint32_t m_cmVarIdMask = 0;              /**< mask of used CM Var instance IDs */
#if GOAL_CONFIG_MEDIA_MI_ETH == 0
static GOAL_STAGE_HANDLER_T stageMiOpen;        /**< mi open stage handler */
#endif
#if GOAL_CONFIG_LOGGING_TARGET_SYSLOG == 1
static GOAL_QUEUE_T *gSyslogTxQueue;            /**< Syslog TX queue memory descriptors */
#endif
#if GOAL_CONFIG_CLI == 1
static GOAL_STAGE_HANDLER_T stageCli;           /**< cli stage handler */
#endif
#if GOAL_CONFIG_DD == 1
static const uint32_t m_virtCmVarOffset = (uint32_t) (GOAL_CM_ETH_VIRT_VAR_LAST - ETH_CM_VAR_LINK);
#endif /* GOAL_CONFIG_DD */

#if GOAL_ETH_NAMES == 1
#  undef GOAL_ETH_COMMAND
#  define GOAL_ETH_COMMAND(x) #x

/**< Ethernet command names */
static const char *goal_miEthCmdNames[] = {
    GOAL_ETH_COMMANDS
};
#endif

#if GOAL_CONFIG_ETH_STATS_INTERNAL == 1
#  undef GOAL_ETH_STAT_RV
#  define GOAL_ETH_STAT_RV(id, val, desc) id,

/**< Ethernet statistics list */
static const uint32_t mStatIds[] = {
    GOAL_ETH_STAT_LIST_RV
};
#endif /* GOAL_CONFIG_ETH_STATS_INTERNAL */


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/
static GOAL_STATUS_T goal_miEthInit(
    void
);

#if GOAL_CONFIG_MEDIA_MI_ETH == 0
static GOAL_STATUS_T goal_miEthOpenReg(
    void
);
#endif

static GOAL_STATUS_T goal_miEthShutdown(
    void
);

static void goal_miEthLoop(
    void *pArg                                  /**< loop argument */
);

static void goal_miEthPortStateUpdate(
    GOAL_MI_ETH_T *pMiInst                      /**< MI instance */
);

#if GOAL_CONFIG_DD == 1
static GOAL_STATUS_T goal_miEthCmLinkStateSet(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t port,                              /**< port */
    uint32_t state                              /**< link state */
);

static GOAL_STATUS_T goal_miEthCmDuplexSet(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t port,                              /**< port */
    uint32_t mode                               /**< duplex mode */
);

static GOAL_STATUS_T goal_miEthCmSpeedSet(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t port,                              /**< port */
    uint32_t speed                              /**< speed */
);

static GOAL_STATUS_T goal_miEthCmVarsInit(
    GOAL_MI_ETH_T *pMiInst                      /**< [in] MI instance */
);
#endif /* GOAL_CONFIG_DD */

static GOAL_STATUS_T goal_miEthPortStateGenericCbReg(
    GOAL_MI_ETH_T *pMiInst,                     /**< [in] MI instance */
    uint32_t id,                                /**< port id */
    GOAL_ETH_PORT_STATE_CB_UNION_T cb,          /**< callback union */
    GOAL_BOOL_T hasArg,                         /**< callback uses additional callback argument */
    void *pArg                                  /**< [in] additional callback argument (NULL if unused) */
);

#if GOAL_CONFIG_ETH_STATS_INTERNAL == 1
static GOAL_STATUS_T goal_miEthStatGet(
    uint64_t *pVal,                             /**< [out] value */
    struct GOAL_STAT_T *pStat                   /**< statistics data */
);

static GOAL_STATUS_T goal_miEthStatReset(
    uint64_t *pVal,                             /**< [out] value */
    struct GOAL_STAT_T *pStat                   /**< statistics data */
);
#endif /* GOAL_CONFIG_ETH_STATS_INTERNAL */


#if GOAL_CONFIG_DD == 1
/****************************************************************************/
/** Get CM Variable for a specific Eth MI instance
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static inline GOAL_STATUS_T goal_miEthVirtCmVarGetById(
    GOAL_MI_ETH_T *pMiEthHdl,                   /**< MI handle */
    uint32_t modId,                             /**< Module ID of the var to search for */
    uint32_t varId,                             /**< Variable ID of the var to search for */
    GOAL_CM_VAR_T **ppVar                       /**< Pointer to the var to search for */
)
{
    if (NULL == pMiEthHdl) {
        return GOAL_ERR_NULL_POINTER;
    }

    return goal_cmGetVarById(modId, varId + (pMiEthHdl->cmVarInstId * m_virtCmVarOffset), ppVar);
}
#endif /* GOAL_CONFIG_DD */


/****************************************************************************/
/** Register stage handler
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_miEthReg(
    void
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    /* init stage */
    res = goal_mainStageReg(GOAL_STAGE_ETH_PRE, &stageInit, GOAL_STAGE_INIT, goal_miEthInit);

    /* shutdown stage */
    if (GOAL_RES_OK(res)) {
        res = goal_mainStageReg(GOAL_STAGE_ETH_PRE, &stageShutdown, GOAL_STAGE_SHUTDOWN, goal_miEthShutdown);
    }

#if GOAL_CONFIG_MEDIA_MI_ETH == 0
    /* register stage to open default handle */
    if (GOAL_RES_OK(res)) {
        res = goal_mainStageReg(GOAL_STAGE_ETH_PRE, &stageMiOpen, GOAL_STAGE_INIT, goal_miEthOpenReg);
    }
#endif

#if GOAL_CONFIG_CLI == 1
    if (GOAL_RES_OK(res)) {
        res = goal_mainStageReg(GOAL_STAGE_CLI, &stageCli, GOAL_STAGE_INIT, goal_ethInitCli);
    }
#endif

    goal_ethRegCmVars();

    return res;
}


/****************************************************************************/
/** Ethernet Handling Initialization
 *
 * This function initializes the Ethernet send lock, the Ethernet Queues
 * and every in the board registered driver through Media Adapter.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_miEthInit(
    void
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* return value */
    GOAL_MA_ETH_T *pMaHdl;                      /* MA hdl */
    GOAL_INSTANCE_LIST_T *pListInst = NULL;     /* instance list */
    GOAL_INSTANCE_T *pEntry;                    /* instance entry */


#if GOAL_CONFIG_LOGGING_TARGET_SYSLOG == 1
    /* initialize syslog buffers */
    res = goal_queueInit(&gSyslogTxQueue, GOAL_CONFIG_LOGGING_TARGET_SYSLOG_BUF,
                         GOAL_CONFIG_LOGGING_TARGET_SYSLOG_BUF,
                         GOAL_CONFIG_LOGGING_TARGET_SYSLOG_SIZE);
    if (GOAL_RES_ERR(res)) {
        return res;
    }
#endif

    /* NOTE: all registered ETH MA's are organized in
     * a linked list. The first MA is fetched
     * and initialized, then the following and so on.
     */
    if (GOAL_RES_OK(res)) {
        res = goal_instListGetByType(&pListInst, GOAL_ID_MA_ETH);
    }

    if (GOAL_RES_OK(res)) {
        GOAL_INSTANCE_FOREACH(pListInst->pInst, pEntry) {
            /* get instance */
            pMaHdl = (GOAL_MA_ETH_T *) pEntry;

            /* initialize driver */
            res = goal_maEthInit(pMaHdl);
            if (GOAL_RES_ERR(res)) {
                break;
            }
        }
    }

    return res;
}


#if GOAL_CONFIG_MEDIA_MI_ETH == 0
/****************************************************************************/
/** Open default Ethernet Instance
 *
 * This function opens a default Ethernet instance and assign it to
 * the board specific Ethernet driver. This function is necessary to
 * keep the old API usable.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_miEthOpenReg(
    void
)
{
    return goal_miEthOpen(GOAL_ID_DEFAULT, GOAL_ID_DEFAULT);
}
#endif


/****************************************************************************/
/** Open Ethernet Instance
 *
 * Creates a new Ethernet instance with the given Media Interface
 * ID. This function additionally registers callback functions
 * in the Media Adapter and opens the driver belonging
 * to the ID.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_miEthOpen(
    uint32_t idMi,                              /**< MI id */
    uint32_t idMa                               /**< MA id */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_MI_ETH_T *pMiEthHdl = NULL;            /* MI handle */
    GOAL_ETH_MAC_ADDR_T macAddr = GOAL_ETH_MAC_ADDR; /* default MAC address */
    uint32_t cntCmVarId = UINT32_MAX;           /* counter of available CM Var instance IDs */
#if GOAL_CONFIG_DD == 1
    uint32_t portCnt;                           /* port count */
#endif /* GOAL_CONFIG_DD */

    /* initially create instance list */
    if (NULL == mpListInst) {
        res = goal_instListNew(&mpListInst, GOAL_ID);
    }

    /* check if instance was already created */
    if (GOAL_RES_OK(res)) {
        res = goal_instGetById((GOAL_INSTANCE_T **) &pMiEthHdl, GOAL_ID, idMi);
    }

    /* allocate memory and create instance */
    if (GOAL_RES_ERR(res)) {
        res = goal_instNew((GOAL_INSTANCE_T **) &pMiEthHdl, sizeof(GOAL_MI_ETH_T), GOAL_ID, idMi, "Ethernet Instance");
    }

    if (GOAL_RES_OK(res) && (GOAL_TRUE == pMiEthHdl->initFlag)) {
        /* instance already internalized */
        return GOAL_OK;
    }

    if (GOAL_RES_OK(res)) {
        pMiEthHdl->pMaEth = NULL;
        pMiEthHdl->pStatePorts = NULL;
        pMiEthHdl->pCmdLock = NULL;
#if GOAL_PTP == 1
        pMiEthHdl->pPortTsCbFunc = NULL;
#endif
    }

    if (GOAL_RES_OK(res)) {
        res = goal_queueInit(&pMiEthHdl->pEthRxLowQueue, GOAL_ETH_RX_LOW_QUEUE_NUM, 0, 0);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_queueInit(&pMiEthHdl->pEthTxLowQueue, GOAL_ETH_TX_LOW_QUEUE_NUM, 0, 0);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_queueInit(&pMiEthHdl->pEthTxHighQueue, GOAL_ETH_TX_HIGH_QUEUE_NUM, 0, 0);
    }

    if (GOAL_RES_OK(res)) {
        /* bind MA to MI */
        res = goal_maEthGetById(&pMiEthHdl->pMaEth, idMa);
        if (GOAL_RES_OK(res)) {
            pMiEthHdl->pMaEth->pMiInst = (void *) pMiEthHdl;
        }
    }

    if (GOAL_RES_OK(res)) {
        /* create Ethernet access lock */
        res = goal_lockCreate(GOAL_LOCK_BINARY, &pMiEthHdl->pCmdLock, 0, 1, GOAL_ID_ETH_SEND);
        if (GOAL_RES_ERR(res)) {
            /* GG_LOG ID:110: */
            goal_lmLog(GOAL_ID_ETH, 110, 81, 0, 0, GOAL_LOG_SEV_ERROR, "failed to create Ethernet access lock");
        }
    }

    if (GOAL_RES_OK(res)) {
        for (cntCmVarId = 0; cntCmVarId < GOAL_CONFIG_NUM_INTERFACES; cntCmVarId++) {
            if (0 == (m_cmVarIdMask & (1 << cntCmVarId))) {
                m_cmVarIdMask |= (1 << cntCmVarId);
                pMiEthHdl->cmVarInstId = cntCmVarId;
                break;
            }
        }

        if (GOAL_CONFIG_NUM_INTERFACES <= cntCmVarId) {
            goal_logErr("no free set of CM variables available. Increase GOAL_CONFIG_NUM_INTERFACES.");
            res = GOAL_ERROR;
        }
    }

#if GOAL_CONFIG_DD == 1
    if (GOAL_RES_OK(res)) {
        res = goal_miEthCmVarsInit(pMiEthHdl);
    }
#endif /* GOAL_CONFIG_DD */

    if (GOAL_RES_OK(res)) {
        /* add callbacks to ethernet MA handle */
        res = goal_maEthCbReg(pMiEthHdl->pMaEth, GOAL_MA_ETH_RECV, (GOAL_MA_ETH_CB_T) { .fCbRecv = goal_miEthRecv});
    }

    if (GOAL_RES_OK(res)) {
        res = goal_maEthCbReg(pMiEthHdl->pMaEth, GOAL_MA_ETH_SEND_FRAME_GET, (GOAL_MA_ETH_CB_T) { .fCbSendFrameGet = goal_miEthSendFrameGet});
    }

    if (GOAL_RES_OK(res)) {
        res = goal_maEthCbReg(pMiEthHdl->pMaEth, GOAL_MA_ETH_SEND_DONE, (GOAL_MA_ETH_CB_T) { .fCbSendDone = goal_miEthSendDone});
    }

    if (GOAL_RES_OK(res)) {
        /* copy module global config into Media Adapter */
        if (GOAL_TRUE == mFlgCfgSet) {
            GOAL_MEMCPY(pMiEthHdl->pMaEth->cfg.macAddr, mCfg.macAddr, MAC_ADDR_LEN);
        }
        else {
            GOAL_MEMCPY(pMiEthHdl->pMaEth->cfg.macAddr, macAddr, MAC_ADDR_LEN);
            pMiEthHdl->pMaEth->cfg.macAddr[5] += pMiEthHdl->id;
        }

        /* open MA and driver */
        res = goal_maEthOpen(pMiEthHdl->pMaEth);
    }

    if (GOAL_RES_OK(res)) {
        /* get actual MAC address, if driver ignored config */
        res = goal_miEthMacAddrGet(pMiEthHdl, GOAL_ETH_PORT_HOST, (GOAL_ETH_MAC_ADDR_T *) pMiEthHdl->pMaEth->cfg.macAddr);
    }

#if GOAL_CONFIG_DD == 1
    if (GOAL_RES_OK(res)) {
        /* update virtual CM Var that shows number of ports */
        res = goal_miEthPortsGet(pMiEthHdl, &portCnt);
        if (GOAL_RES_OK(res)) {
            GOAL_CM_SETVAR_UINT32(pMiEthHdl->pVarPortCnt, portCnt);
        }
    }
#endif /* GOAL_CONFIG_DD */

    /* register Ethernet loop */
    if (GOAL_RES_OK(res)) {
        res = goal_mainLoopParamReg(goal_miEthLoop, pMiEthHdl);
    }

    /* set init done flag */
    if (GOAL_RES_OK(res)) {
        pMiEthHdl->initFlag = GOAL_TRUE;
    }
    else if (UINT32_MAX != cntCmVarId) {
        /* flag was already set, clear it */
        m_cmVarIdMask &= ~(1 << cntCmVarId);
    }

    return res;
}


/****************************************************************************/
/** Ethernet Handling Shutdown
 *
 * This function shuts the Ethernet handling down.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
static GOAL_STATUS_T goal_miEthShutdown(
    void
)
{
    /* GG_LOG ID:114: */
    goal_lmLog(GOAL_ID_ETH, 114, 85, 0, 0, GOAL_LOG_SEV_INFO, "Ethernet shutdown");

    return GOAL_OK;
}


/****************************************************************************/
/** Generic Ethernet Receive Callback for unspecific Frames
 *
 * This function is called by the target and Media Adapter when an Ethernet
 * frame was received.
 */
void goal_miEthRecv(
    GOAL_MA_ETH_T *pMaEthHdl,                   /**< [in] MA handle */
    uint32_t portIdx,                           /**< Ethernet port index */
    GOAL_BUFFER_T **ppBuf                       /**< [out] OS Ethernet buffer */
)
{
    GOAL_STATUS_T res;                          /* return result */
    GOAL_BUFFER_T *pBuf = NULL;                 /* RX queue buffer */
    GOAL_BUFFER_T *pBufTmp;                     /* temporary exchange buffer */
    uint8_t *pData;                             /* frame data pointer */
    GOAL_ETH_BUF_HANDLER_T *pHandler;           /* buffer handler */
    GOAL_ETH_MAC_ADDR_T macAny = GOAL_ETH_MAC_ADDR_ANY; /* any Mac address */
    GOAL_MI_ETH_T *pMiInst;                     /* instance */

    /* get mi binded to ma */
    if (NULL != pMaEthHdl) {
        pMiInst = (GOAL_MI_ETH_T *) pMaEthHdl->pMiInst;
    }
    else {
        /* otherwise get mi by id */
        res = goal_instGetById((GOAL_INSTANCE_T **) &pMiInst, GOAL_ID, GOAL_ID_DEFAULT);
        if (GOAL_RES_ERR(res)) {
            return;
        }
        pMaEthHdl = pMiInst->pMaEth;
    }

    /* check if Ethernet handling is initialized */
    if (GOAL_FALSE == pMiInst->initFlag) {
        return;
    }

    /* check port index range */
    if ((pMiInst->numPorts <= portIdx) && (GOAL_ETH_PORT_HOST != portIdx)) {
        /* GG_LOG ID:115: */
        goal_lmLog(GOAL_ID_ETH, 115, 86, 4, 0, GOAL_LOG_SEV_ERROR, "port index out of range: $1");
        goal_lmLogParamUINT32(portIdx);
        goal_lmLogFinish();
        return;
    }

    /* statistics: IfInOctets */
    GOAL_ETH_STAT_ADD(GOAL_STAT_ID_ETH_IFINOCTETS, portIdx, (*ppBuf)->dataLen);

    /* check that buffer contains data */
    if (0 == (*ppBuf)->dataLen) {

        /* statistics: IfInErrors */
        GOAL_ETH_STAT_INC(GOAL_STAT_ID_ETH_IFINERRORS, portIdx);

        return;
    }

    /* set port ID */
    (*ppBuf)->netPort = portIdx;

#if GOAL_CONFIG_ETHERNET_DUMP_MAC == 1
    /* dump destination and source MAC address */

    /* GG_LOG ID:24: */
    goal_lmLog(GOAL_ID_ETH, 24, 15, 6, 6, GOAL_LOG_SEV_INFO, "src $1 : dst $2");
    goal_lmLogParamMAC(&(*ppBuf)->ptrData[6]);
    goal_lmLogParamMAC(&(*ppBuf)->ptrData[0]);
    goal_lmLogFinish();
#endif

#if GOAL_CONFIG_MAC_ADDR_FILTER == 1
    /* allow broadcast, multicast and own MAC address to pass */
    if (!((*ppBuf)->ptrData[0] & GOAL_ETH_MAC_MULTICAST) &&
        !(!GOAL_MEMCMP(pMaEthHdl->cfg.macAddr, (*ppBuf)->ptrData, MAC_ADDR_LEN))) {

        /* statistics: IfInDiscards */
        GOAL_ETH_STAT_INC(GOAL_STAT_ID_ETH_IFINDISCARDS, portIdx);

        return;
    }
#endif

    /* check for high priority destination MAC address handlers */
    for (pHandler = pMiInst->pHandlerMacHigh; pHandler; pHandler = pHandler->pNext) {
        if ((!GOAL_MEMCMP(pHandler->type.pAddrMac, &macAny, MAC_ADDR_LEN)) ||
            (!GOAL_MEMCMP(pHandler->type.pAddrMac, (*ppBuf)->ptrData, MAC_ADDR_LEN))) {
            if (GOAL_RES_OK(pHandler->pFunc(ppBuf, pHandler->pArg))) {
                return;
            }
        }
    }

    /* check for low priority destination MAC address handlers */
    for (pHandler = pMiInst->pHandlerMacLow; pHandler; pHandler = pHandler->pNext) {
        if (!GOAL_MEMCMP(pHandler->type.pAddrMac, (*ppBuf)->ptrData, MAC_ADDR_LEN)) {
            /* assign low priority buffer handler */
            (*ppBuf)->pEthBufHdlr = pHandler;
            break;
        }
    }

    /* only handle EtherType if low priority handling wasn't found (pHandler is NULL) */
    if (NULL == pHandler) {

        /* set EtherType */
        pData = (*ppBuf)->ptrData + (MAC_ADDR_LEN * 2);
        (*ppBuf)->etherType = GOAL_be16toh_p(pData);

        /* if EtherType is VLAN, extract real EtherType */
        if (GOAL_ETH_ETHERTYPE_VLAN == (*ppBuf)->etherType) {
            pData += VLAN_TAG_LEN;
            (*ppBuf)->etherType = GOAL_be16toh_p(pData);
            goal_queueFlagsSet(*ppBuf, GOAL_QUEUE_FLG_VLAN);
        }
        else {
            /* clear VLAN flag if still enabled in exchanged frame */
            goal_queueFlagsClr(*ppBuf, GOAL_QUEUE_FLG_VLAN);
        }

        /* call high priority RX handlers for buffer */
        for (pHandler = pMiInst->pHandlerHigh; pHandler; pHandler = pHandler->pNext) {
            if ((GOAL_ETH_ETHERTYPE_ANY == pHandler->type.etherType) ||
                (pHandler->type.etherType == (*ppBuf)->etherType)) {

                if (GOAL_RES_OK(pHandler->pFunc(ppBuf, pHandler->pArg))) {
                    return;
                }
            }
        }
    }

    /* non-cyclic frame: check if the RX queue isn't full */
    if (goal_queueIsFull(pMiInst->pEthRxLowQueue)) {

        /* statistics: IfInDiscards */
        GOAL_ETH_STAT_INC(GOAL_STAT_ID_ETH_IFINDISCARDS, portIdx);

        return;
    }

    /* allocate a new buffer for the low level Ethernet handler */
    if (GOAL_OK != goal_ethGetNetBuf(&pBuf)) {

        /* statistics: IfInDiscards */
        GOAL_ETH_STAT_INC(GOAL_STAT_ID_ETH_IFINDISCARDS, portIdx);

        return;
    }

    /* pass the new buffer to the low level handler */
    pBufTmp = *ppBuf;
    *ppBuf = pBuf;
    pBuf = pBufTmp;

    /* add current buffer to RX queue */
    if (GOAL_OK != goal_queueAddElem(pMiInst->pEthRxLowQueue, pBuf)) {

        /* statistics: IfInDiscards */
        GOAL_ETH_STAT_INC(GOAL_STAT_ID_ETH_IFINDISCARDS, portIdx);

        goal_queueReleaseBuf(&pBuf);
    }
}


/****************************************************************************/
/** Generic Ethernet Send Function
 *
 * This function is called when an Ethernet frame that is embedded in a
 * GOAL Buffer (GOAL_BUFFER_T) is ready to be sent. Frames of the type
 * @em GOAL_NET_TX_RT will be put into a queue with a higher priority.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_miEthSend(
    GOAL_MI_ETH_T *pMiEthHdl,                   /**< [in] MI handle */
    GOAL_NET_TX_TYPE_T type,                    /**< frame channel type */
    void **ppBuf                                /**< [in,out] pointer to PROFINET buffer */
)
{
    GOAL_STATUS_T res;                          /* return result */

    /* get instance by id */
    if (NULL == pMiEthHdl) {
        res = goal_instGetById((GOAL_INSTANCE_T **) &pMiEthHdl, GOAL_ID, GOAL_ID_DEFAULT);
        if (GOAL_RES_ERR(res)) {
            /* release buffer if an error occurs */
            goal_miEthSendDone(NULL, (GOAL_BUFFER_T **) ppBuf, GOAL_FALSE);
            return res;
        }
    }

    /* make sure buffer isn't in transmit state */
    if (GOAL_TRUE == goal_queueFlagsGet(*ppBuf, GOAL_QUEUE_FLG_TX)) {
        /* GG_LOG ID:116: */
        goal_lmLog(GOAL_ID_ETH, 116, 87, 0, 0, GOAL_LOG_SEV_ERROR, "frame still in transmit state");
        return GOAL_ERR_FRAME_IN_TRANSMISSION;
    }

    /* statistics: IfOutOctets */
    GOAL_ETH_STAT_ADD(GOAL_STAT_ID_ETH_IFOUTOCTETS, (*((GOAL_BUFFER_T **) ppBuf))->netPort, ((GOAL_BUFFER_T *) (*ppBuf))->dataLen);

    /* set transmission flag */
    goal_queueFlagsSet(*ppBuf, GOAL_QUEUE_FLG_TX);

    /* check if Ethernet handling is initialized */
    if (GOAL_FALSE == pMiEthHdl->initFlag) {

        /* statistics: IfOutDiscards */
        GOAL_ETH_STAT_INC(GOAL_STAT_ID_ETH_IFOUTDISCARDS, (*((GOAL_BUFFER_T **) ppBuf))->netPort);

        /* free buffer and call buffer callback if available */
        goal_miEthSendDone(pMiEthHdl->pMaEth, (GOAL_BUFFER_T **) ppBuf, GOAL_FALSE);

        return GOAL_ERR_FRAME_SEND_FAILED;
    }

    /* make sure frame isn't already released */
    GOAL_ASSERT(goal_queueFlagsGet((GOAL_BUFFER_T *) *ppBuf, GOAL_QUEUE_FLG_USED));

    /* enqueue frame or free frame on error */
    res = goal_queueAddElem((GOAL_NET_TX_RT == type) ? (pMiEthHdl->pEthTxHighQueue) : (pMiEthHdl->pEthTxLowQueue), *ppBuf);
    if (GOAL_RES_ERR(res)) {

        /* statistics: IfOutDiscards */
        GOAL_ETH_STAT_INC(GOAL_STAT_ID_ETH_IFOUTDISCARDS, (*((GOAL_BUFFER_T **) ppBuf))->netPort);

        goal_miEthSendDone(pMiEthHdl->pMaEth, (GOAL_BUFFER_T **) ppBuf, GOAL_FALSE);
        return res;
    }

    /* send through MA */
    res = goal_maEthSend(pMiEthHdl->pMaEth);
    if (GOAL_RES_OK(res)) {
        /* clear pointer ref from caller */
        *ppBuf = NULL;
    }

    return res;
}


/****************************************************************************/
/** Get Frame From Send Queue
 *
 * This function is called by the MA if a frame should be sended.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miEthSendFrameGet(
    GOAL_MA_ETH_T *pMaEthHdl,                   /**< [in] MA handle */
    GOAL_BUFFER_T **ppBuf                       /**< [out] GOAL buffer ptr ref */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MI_ETH_T *pMiInst = NULL;              /* instance */

    /* check if ma hdl accessable */
    if (NULL != pMaEthHdl) {
        pMiInst = (GOAL_MI_ETH_T *) pMaEthHdl->pMiInst;
    }
    else {
        /* otherwise get mi by id */
        res = goal_instGetById((GOAL_INSTANCE_T **) &pMiInst, GOAL_ID, GOAL_ID_DEFAULT);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
    }

    /* return high priority frames prior low priority frames */
    res = goal_queueGetElem(pMiInst->pEthTxHighQueue, (void **) ppBuf);
    if (GOAL_RES_ERR(res)) {
        res = goal_queueGetElem(pMiInst->pEthTxLowQueue, (void **) ppBuf);
    }

#if GOAL_CONFIG_ETHERNET_DUMP_MAC == 1
    if (GOAL_RES_OK(res)) {
        /* dump destination and source MAC address */
        /* GG_LOG ID:25: */
        goal_lmLog(GOAL_ID_ETH, 25, 15, 6, 6, GOAL_LOG_SEV_INFO, "src $1 : dst $2");
        goal_lmLogParamMAC(&(*ppBuf)->ptrData[6]);
        goal_lmLogParamMAC(&(*ppBuf)->ptrData[0]);
        goal_lmLogFinish();
    }
#endif

    return res;
}


/****************************************************************************/
/** Send Ethernet Frame Done Handler
 *
 * This callback is called when a buffer was send and can be reused. If the
 * doneFlag isn't set to GOAL_TRUE the frame wasn't sent.
 */
void goal_miEthSendDone(
    GOAL_MA_ETH_T *pMaEthHdl,                   /**< [in] MA handle */
    GOAL_BUFFER_T **ppBuf,                      /**< [out]frame buffer */
    GOAL_BOOL_T doneFlag                        /**< done flag */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MI_ETH_T *pMiInst;                     /* MI instance */

    /* check if ma hdl accessable */
    if (NULL != pMaEthHdl) {
        pMiInst = (GOAL_MI_ETH_T *) pMaEthHdl->pMiInst;
    }
    else {
        /* otherwise get mi by id */
        res = goal_instGetById((GOAL_INSTANCE_T **) &pMiInst, GOAL_ID, GOAL_ID_DEFAULT);
        if (GOAL_RES_ERR(res)) {
            return;
        }
    }

    /* clear TX flag */
    goal_queueFlagsClr(*ppBuf, GOAL_QUEUE_FLG_TX);

    /* statistics: IfOutDiscards */
    if (GOAL_FALSE == doneFlag) {
        GOAL_ETH_STAT_INC(GOAL_STAT_ID_ETH_IFOUTDISCARDS, (*((GOAL_BUFFER_T **) ppBuf))->netPort);
    }

    /* only print a failure if syslog is disabled to avoid recursion */
#if (GOAL_CONFIG_LOGGING == 1) && (GOAL_CONFIG_LOGGING_TARGET_SYSLOG == 0)
    if (GOAL_FALSE == doneFlag) {
        /* GG_LOG ID:117: */
        goal_lmLog(GOAL_ID_ETH, 117, 88, 0, 0, GOAL_LOG_SEV_WARNING, "failed to send Ethernet frame");
    }
#else
    UNUSEDARG(doneFlag);
#endif /* (GOAL_CONFIG_LOGGING == 1) && (GOAL_CONFIG_LOGGING_TARGET_SYSLOG == 0) */

    /* only release buffer if NO_RELEASE isn't set */
    goal_queueReleaseBuf(ppBuf);

    /* check if next frame needs to be send */
    if ((!goal_queueIsEmpty(pMiInst->pEthTxHighQueue)) || (!goal_queueIsEmpty(pMiInst->pEthTxLowQueue))) {
        goal_maEthSend(pMiInst->pMaEth);
    }
}


/****************************************************************************/
/** Main Loop Network Data Handler
 *
 * This handles non-RT network data and must be called in the main loop.
 * It checks the Rx Queue for new packets and passes them to the appropriate
 * modules.
 */
static void goal_miEthLoop(
    void *pArg                                  /**< loop argument */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_BUFFER_T *pBuf = NULL;                 /* Ethernet buffer */
    GOAL_ETH_BUF_HANDLER_T *pHandler;           /* buffer handler */
    GOAL_MI_ETH_T *pMiInst;                     /* MI instance */

    pMiInst = (GOAL_MI_ETH_T *) pArg;

    /* update port states */
    goal_miEthPortStateUpdate(pMiInst);

    /* read an element from the receive queue */
    while (GOAL_OK == goal_queueGetElem(pMiInst->pEthRxLowQueue, (void **) &pBuf)) {

        /* check for low priority destination MAC address handlers */
        if (pBuf->pEthBufHdlr) {
            pHandler = pBuf->pEthBufHdlr;
        } else {
            /* call low priority EtherType RX handlers for buffer */
            for (pHandler = pMiInst->pHandlerLow; pHandler; pHandler = pHandler->pNext) {
                if (pHandler->type.etherType == pBuf->etherType) {
                    break;
                }
            }
        }

        /* if handler was found, handover buffer to handler */
        if (pHandler) {

            /* release buffer if handler returned error */
            res = pHandler->pFunc(&pBuf, pHandler->pArg);
            if (GOAL_RES_ERR(res)) {
                goal_queueReleaseBuf(&pBuf);
            }

            continue;
        }

        /* release unhandled buffer */
        goal_queueReleaseBuf(&pBuf);
    }
}


#if GOAL_CONFIG_LOGGING_TARGET_SYSLOG == 1
/****************************************************************************/
/** Output Syslog Message via Ethernet
 *
 * Take a buffer from the Syslog queue, copy the message into it and transfer
 * it to the given target.
 */
void goal_miEthSendSyslog(
    GOAL_MI_ETH_T *pMiEthHdl,                   /**< MI handle */
    const char *str,                            /**< log string */
    unsigned int len                            /**< string length */
)
{
    GOAL_BUFFER_T *pBuf = NULL;                 /* buffer descriptor */
    unsigned int clen;                          /* copy length */

    if (pMiEthHdl->initFlag == GOAL_FALSE) {
        return;
    }

    /* try to get syslog buffer */
    if (GOAL_OK != goal_queueGetNewBuf(&pBuf, gSyslogTxQueue, GOAL_ID_LOG_SYSLOG)) {

        /* all buffers in transmission, drop message */
        return;
    }

    /* copy message to buffer */
    clen = (len < GOAL_CONFIG_LOGGING_TARGET_SYSLOG_SIZE) ? len : GOAL_CONFIG_LOGGING_TARGET_SYSLOG_SIZE;
    GOAL_MEMCPY(pBuf->ptrData, str, clen);
    pBuf->dataLen = GOAL_UINT2U16(clen);

    /* transmit the message */
    goal_miEthSend(pMiEthHdl, GOAL_NET_TX_LOW, (void **) &pBuf);
}
#endif /* GOAL_CONFIG_LOGGING_TARGET_SYSLOG */


/****************************************************************************/
/** Add Ethernet protocol handler with optional position parameter
 *
 * Register a low or high priority Ethernet protocol handler. High priority
 * protocol handlers are executed directly in the Ethernet interrupt if the
 * driver supports it. Low priority handlers are processed in the main loop,
 * for example the TCP/IP stack.
 *
 * This function registers either an EtherType handler or an MAC address
 * handler but not both. Low priority MAC address handlers are also
 * fast-checked in the high priority processing to avoid false-positives if the
 * EtherType of another handler accidentally matches.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_miEthProtoAddPos(
    GOAL_MI_ETH_T *pMiEthHdl,                   /**< MI handle */
    GOAL_BOOL_T highPrio,                       /**< high priority flag */
    uint16_t etherType,                         /**< EtherType */
    GOAL_ETH_MAC_ADDR_T *pAddrMac,              /**< MAC address ptr */
    GOAL_ETH_BUF_FUNC_T pFunc,                  /**< protocol handler function */
    void *pArg,                                 /**< callback argument */
    GOAL_BOOL_T beforeSame                      /**< add handler before handlers using same ether type */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_ETH_BUF_HANDLER_T *pHandler;           /* buffer handler */
    GOAL_ETH_BUF_HANDLER_T **ppCur = NULL;      /* current buffer handler ref */
    GOAL_BOOL_T isEtherType = GOAL_FALSE;       /* handler is for ether type instead of Mac */


    /* check if function is set */
    if (NULL == pFunc) {
        /* GG_LOG ID:118: */
        goal_lmLog(GOAL_ID_ETH, 118, 89, 0, 0, GOAL_LOG_SEV_ERROR, "handler function not set");
        return GOAL_ERR_NULL_POINTER;
    }

    if (NULL == pMiEthHdl) {
        /* get inst by id */
        res = goal_instGetById((GOAL_INSTANCE_T **) &pMiEthHdl, GOAL_ID, GOAL_ID_DEFAULT);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
    }

    /* check if ethertype is set */
    if (0 == etherType) {
        return GOAL_ERR_PARAM;
    }

    /* allocate protocol handler and set function pointer */
    res = goal_memCalloc(&pHandler, sizeof(GOAL_ETH_BUF_HANDLER_T));
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:119: */
        goal_lmLog(GOAL_ID_ETH, 119, 90, 0, 0, GOAL_LOG_SEV_ERROR, "failed to allocate protocol handler");
    }
    else {
        pHandler->pFunc = pFunc;
        pHandler->pArg = pArg;
    }

    /* allocate and copy MAC address if set */
    if (GOAL_RES_OK(res) && *pAddrMac) {
        res = goal_memCalloc(&pHandler->type.pAddrMac, MAC_ADDR_LEN);
        if (GOAL_RES_ERR(res)) {
            /* GG_LOG ID:120: */
            goal_lmLog(GOAL_ID_ETH, 120, 91, 0, 0, GOAL_LOG_SEV_ERROR, "failed to allocate MAC address in protocol handler");
        }
        else {
            GOAL_MEMCPY(pHandler->type.pAddrMac, *pAddrMac, MAC_ADDR_LEN);

            /* assign MAC address list */
            ppCur = (highPrio) ? &(pMiEthHdl->pHandlerMacHigh) : &(pMiEthHdl->pHandlerMacLow);
        }
    }
    else {
        /* otherwise set EtherType and normal list */
        pHandler->type.etherType = etherType;
        ppCur = (highPrio) ? &(pMiEthHdl->pHandlerHigh) : &(pMiEthHdl->pHandlerLow);
        isEtherType = GOAL_TRUE;
    }

    /* add handler to list */
    if (GOAL_RES_OK(res)) {

        /* check if element is first in queue */
        if (NULL == *ppCur) {
            /* add handler */
            *ppCur = pHandler;
            pHandler->pNext = NULL;
        }
        else if (GOAL_TRUE == isEtherType) {
            /* check if ethertype is smaller than first in list */
            if ((etherType < (*ppCur)->type.etherType) || ((GOAL_TRUE == beforeSame) && (etherType == (*ppCur)->type.etherType))) {
                    /* add entry to list start */
                    pHandler->pNext = *ppCur;
                    *ppCur = pHandler;
            }
            else {
                /* find first entry with bigger etherType in list */
                for (; NULL != (*ppCur)->pNext; ppCur = &(*ppCur)->pNext) {
                    if ((etherType < ((*ppCur)->pNext)->type.etherType) || ((GOAL_TRUE == beforeSame) && (etherType == ((*ppCur)->pNext)->type.etherType))) {
                        /* found bigger one */
                        break;
                    }
                }
                /* add handler to list */
                pHandler->pNext = (*ppCur)->pNext;
                (*ppCur)->pNext = pHandler;
            }
        }
        else {

            /* check if MAC is smaller than first in list */
            if (((GOAL_FALSE == beforeSame) && (0 > GOAL_MEMCMP(*pAddrMac, *(*ppCur)->type.pAddrMac, MAC_ADDR_LEN))) ||
                ((GOAL_TRUE == beforeSame) && (0 >= GOAL_MEMCMP(*pAddrMac, *(*ppCur)->type.pAddrMac, MAC_ADDR_LEN)))) {
                    /* add entry to list start */
                    pHandler->pNext = *ppCur;
                    *ppCur = pHandler;
            }
            else {
                /* find first entry with bigger MAC in list */
                for (; NULL != (*ppCur)->pNext; ppCur = &(*ppCur)->pNext) {
                    if (((GOAL_FALSE == beforeSame) && (0 > GOAL_MEMCMP(*pAddrMac, *((*ppCur)->pNext)->type.pAddrMac, MAC_ADDR_LEN))) ||
                        ((GOAL_TRUE == beforeSame) && (0 >= GOAL_MEMCMP(*pAddrMac, *((*ppCur)->pNext)->type.pAddrMac, MAC_ADDR_LEN)))) {
                        /* found bigger one */
                        break;
                    }
                }
                /* add handler to list */
                pHandler->pNext = (*ppCur)->pNext;
                (*ppCur)->pNext = pHandler;
            }
        }
    }

    /* inform platform about "raw" Ethernet handler */
    if (GOAL_RES_OK(res)) {
        /* ignore result if command is unsupported */
        (void) goal_miEthCmd(pMiEthHdl, GOAL_ETH_CMD_RAW_INIT, GOAL_FALSE, GOAL_ETH_PORT_HOST, &etherType);
    }

    /* handle errors */
    if (GOAL_RES_ERR(res)) {

        /* free handler if allocated */
        if (pHandler) {

            /* free MAC address if allocated */
            if (pHandler->type.pAddrMac) {
                goal_memFree(&pHandler->type.pAddrMac);
            }

            /* free handler */
            goal_memFree(&pHandler);
        }
    }

    return res;
}


/****************************************************************************/
/** Get external interface/port count
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_miEthPortsGet(
    GOAL_MI_ETH_T *pMiEthHdl,                   /**< [in] MI handle */
    uint32_t *pPorts                            /**< port count ref */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    uint32_t numOfPorts = 0;                    /* number of ethernet ports */

    /* get number of ethernet ports */
    res = goal_miEthCmd(pMiEthHdl, GOAL_ETH_CMD_PORT_COUNT, GOAL_FALSE, GOAL_ETH_PORT_HOST, &numOfPorts);

    if (GOAL_RES_OK(res)) {
        /* set number of external ports */
        *pPorts = numOfPorts;
    };

    return res;
}


/****************************************************************************/
/** Generic interface command caller
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_miEthCmd(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    GOAL_ETH_CMD_T cmd,                         /**< command */
    GOAL_BOOL_T wrFlag,                         /**< write flag */
    uint32_t port,                              /**< port ID */
    void *pArg                                  /**< argument */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    if (NULL == pMiInst) {
        /* get inst by id */
        res = goal_instGetById((GOAL_INSTANCE_T **) &pMiInst, GOAL_ID, GOAL_ID_DEFAULT);
    }

    if (GOAL_RES_ERR(res)) {
        if (GOAL_ETH_CMD_MAC_ADDR == cmd) {
            /* use default module mac as fallback */
            GOAL_MEMCPY(pArg, mCfg.macAddr, MAC_ADDR_LEN);
            return GOAL_OK;
        }
        else {
            return GOAL_ERR_UNSUPPORTED;
        }
    }

    /* verify argument pointer */
    if (NULL == pArg) {
        switch (cmd) {
            case GOAL_ETH_CMD_DLR_INIT:
            case GOAL_ETH_CMD_DLR_ENABLE:
            case GOAL_ETH_CMD_DLR_SHUTDOWN:
            case GOAL_ETH_CMD_DLR_FORWARD:
            case GOAL_ETH_CMD_MAC_UNI_FLUSH:
            case GOAL_ETH_CMD_MAC_MULTI_FLUSH:
            case GOAL_ETH_CMD_PORT_FWD_ADD:
            case GOAL_ETH_CMD_PORT_FWD_DEL:
            case GOAL_ETH_CMD_RAW_INIT:
            case GOAL_ETH_CMD_AUTONEG_RESTART:
            case GOAL_ETH_CMD_SETUP_DONE:
            case GOAL_ETH_CMD_DLR_RESET:
                break;

            default:
                /* GG_LOG ID:121: */
                goal_lmLog(GOAL_ID_ETH, 121, 92, 0, 0, GOAL_LOG_SEV_ERROR, "NULL pointer argument not allowed");
                return GOAL_ERR_NULL_POINTER;
        }
    }

    /* acquire Ethernet access lock */
    res = goal_lockGet(pMiInst->pCmdLock, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:122: */
        goal_lmLog(GOAL_ID_ETH, 122, 93, 0, 0, GOAL_LOG_SEV_ERROR, "failed to aquire Ethernet access lock");
        return res;
    }

    /* call interface command handler */
    res = goal_miEthCmdTable(pMiInst, cmd, pMiInst->pEthTableCmd, wrFlag, port, pArg);

    /* release Ethernet access lock */
    goal_lockPut(pMiInst->pCmdLock);

    return res;
}


/****************************************************************************/
/** Ethernet Command Table Handler
 *
 * Generic Ethernet command table handler.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_miEthCmdTable(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    GOAL_ETH_CMD_T cmd,                         /**< command */
    GOAL_ETH_FUNCTION_ENTRY_T *pTableFunc,      /**< table of Ethernet functions */
    GOAL_BOOL_T wrFlag,                         /**< write flag */
    uint32_t port,                              /**< port ID */
    void *pArg                                  /**< argument */
)
{
    uint32_t cnt;                               /* count */
    GOAL_STATUS_T res;                          /* result */
    GOAL_ETH_CMD_DATA_T data;                   /* command data */
    uint32_t numOfPorts = 0;                    /* number of ethernet ports */

    numOfPorts = pMiInst->numPorts;
    /* verify port argument */
    if (!(port & GOAL_ETH_PORT_MASK)) {
        if ((GOAL_ETH_PORT_HOST != port) && (numOfPorts <= port)) {
            /* GG_LOG ID:123: */
            goal_lmLog(GOAL_ID_ETH, 123, 94, 4, 0, GOAL_LOG_SEV_ERROR, "invalid port argument $1");
            goal_lmLogParamUINT32(port);
            goal_lmLogFinish();
            return GOAL_ERR_PARAM;
        }
    }
    else {
        if (port & (~pMiInst->portMaskValidAll)) {
            /* GG_LOG ID:124: */
            goal_lmLog(GOAL_ID_ETH, 124, 95, 4, 0, GOAL_LOG_SEV_ERROR, "port argument $1 contains invalid bits");
            goal_lmLogParamUINT32(port);
            goal_lmLogFinish();
            return GOAL_ERR_PARAM;
        }
    }

    /* try target specific cmd without using table */
    res = goal_maEthCmd(pMiInst->pMaEth, (uint32_t) cmd, wrFlag, port, pArg);
    if (GOAL_ERR_UNSUPPORTED != res) {
        return res;
    }

    /* check if a command table is defined */
    if (NULL == pTableFunc) {
        goal_logDbg("No Ethernet functions found in table.");
        return GOAL_ERR_UNSUPPORTED;
    }

    /* check if command is in function list and execute in this case */
    for (cnt = 0; pTableFunc[cnt].cmd < GOAL_ETH_CMD_END; cnt++) {

        /* skip unmatched commands */
        if (pTableFunc[cnt].cmd != cmd) {
            continue;
        }

        /* check if command is only for external ports and host port is selected */
        if ((pTableFunc[cnt].param & GOAL_ETH_CMD_PORT_EXT) && (port & GOAL_ETH_PORT_HOST)) {
#if GOAL_ETH_NAMES == 1
            /* GG_LOG ID:125: */
            goal_lmLog(GOAL_ID_ETH, 125, 96, GOAL_STRLEN((const char *) goal_miEthCmdNameGet(cmd)), 0, GOAL_LOG_SEV_ERROR, "command $1 only supports external ports");
            goal_lmLogParamSTRING0(goal_miEthCmdNameGet(cmd));
            goal_lmLogFinish();
#else
            /* GG_LOG ID:126: */
            goal_lmLog(GOAL_ID_ETH, 126, 97, 4, 0, GOAL_LOG_SEV_ERROR, "command $1 only supports external ports");
            goal_lmLogParamUINT32((uint32_t) cmd);
            goal_lmLogFinish();
#endif
            return GOAL_ERR_UNSUPPORTED;
        }

        /* check if command is only for internal port and external ports are selected */
        if ((pTableFunc[cnt].param & GOAL_ETH_CMD_PORT_HOST) && (port & GOAL_ETH_PORT_BITS)) {
#if GOAL_ETH_NAMES == 1
            /* GG_LOG ID:127: */
            goal_lmLog(GOAL_ID_ETH, 127, 98, GOAL_STRLEN((const char *) goal_miEthCmdNameGet(cmd)), 0, GOAL_LOG_SEV_ERROR, "command $1 only supports internal port");
            goal_lmLogParamSTRING0(goal_miEthCmdNameGet(cmd));
            goal_lmLogFinish();
#else
            /* GG_LOG ID:128: */
            goal_lmLog(GOAL_ID_ETH, 128, 99, 4, 0, GOAL_LOG_SEV_ERROR, "command $1 only supports internal port");
            goal_lmLogParamUINT32((uint32_t) cmd);
            goal_lmLogFinish();
#endif
            return GOAL_ERR_UNSUPPORTED;
        }

        break;
    }

    /* call default forward function if defined */
    if (pTableFunc[cnt].func) {

        /* initialize command data */
        data.pMaEth = pMiInst->pMaEth;
        data.pIface = NULL;
        data.cmd = cmd;
        data.flgSet = wrFlag;
        data.port = port;
        data.data.pRaw = pArg;

        res = pTableFunc[cnt].func(&data);

        /* Commands where default values are set when the command is not
         * directly supported by the underlying platform
         */
        if (GOAL_ERR_UNSUPPORTED == res) {
            switch (cmd) {
            case GOAL_ETH_CMD_QUEUES_PER_PORT:
                *((uint32_t *) pArg) = GOAL_ETH_DEFAULT_QUEUES_PER_PORT;
                res = GOAL_OK;
                break;
            default:
                break;
            }
        }

        return res;
    }

    return GOAL_ERR_UNSUPPORTED;
}


/****************************************************************************/
/** Setup command data structure
 */
void goal_miEthCmdData(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    GOAL_ETH_CMD_DATA_T *pDataCmd,              /**< data structure */
    GOAL_ETH_CMD_T cmd,                         /**< command */
    GOAL_BOOL_T flgSet,                         /**< set flag */
    uint32_t port,                              /**< port or port mask */
    void *pArg                                  /**< argument ptr */
)
{
    if (NULL == pMiInst) {
        /* get inst by id */
        goal_instGetById((GOAL_INSTANCE_T **) &pMiInst, GOAL_ID, GOAL_ID_DEFAULT);
    }

    pDataCmd->pMaEth = pMiInst->pMaEth;
    pDataCmd->cmd = cmd;
    pDataCmd->flgSet = flgSet;
    pDataCmd->port = port;
    pDataCmd->data.pRaw = pArg;
}


/****************************************************************************/
/** Table registration
 *
 * Register table of commands and belonging to them
 *
 * @retval GOAL_OK table is registered
 * @retval other table to register was empty
 */
GOAL_STATUS_T goal_miEthTableReg(
    GOAL_MI_ETH_T *pMiEthHdl,                   /**< MI handle */
    GOAL_ETH_FUNCTION_ENTRY_T *pEthTableCmd     /**< table of Ethernet functions to register */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    uint32_t cnt;                               /* counter */
#if GOAL_CONFIG_ETH_STATS_INTERNAL == 1
    unsigned int cntPort;                       /* port counter */
    GOAL_ETH_PORT_T idPort;                     /* port id */
    uint32_t cntPortMax;                        /* maximum number of ports */
#endif /* GOAL_CONFIG_ETH_STATS_INTERNAL */

    if (NULL == pEthTableCmd) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (NULL == pMiEthHdl) {
        /* get handle of default interface */
        res = goal_miEthHdlGet(GOAL_ID_DEFAULT, &pMiEthHdl);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
    }

    /* register given table in MI handle */
    pMiEthHdl->pEthTableCmd = pEthTableCmd;

    /* get number of ethernet ports */
    res = goal_miEthCmd(pMiEthHdl, GOAL_ETH_CMD_PORT_COUNT, GOAL_FALSE, GOAL_ETH_PORT_HOST, &pMiEthHdl->numPorts);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to get number of ethernet ports from ETH driver");
    }

    /* set port mask containing all valid port mask bits */
    pMiEthHdl->portMaskValidAll = GOAL_ETH_PORT_MASK | GOAL_ETH_PORT_HOST;
    for (cnt = 0; cnt < pMiEthHdl->numPorts; cnt++) {
        pMiEthHdl->portMaskValidAll |= GOAL_ETH_PORT_BIT(cnt);
    }

    /* allocate Ethernet port states for all external ports */
    if (GOAL_RES_OK(res)) {
        res = goal_memCalloc(&pMiEthHdl->pStatePorts, pMiEthHdl->numPorts * sizeof(GOAL_ETH_PORT_STATE_T));
    }

#if GOAL_CONFIG_ETH_STATS_INTERNAL == 1
    cntPortMax = (1 == pMiEthHdl->numPorts) ? 1 : pMiEthHdl->numPorts + GOAL_ETH_PORT_HOST_CNT;

    /* register internal Ethernet statistics */
    for (cnt = 0; GOAL_RES_OK(res) && (cnt < GOAL_ETH_STAT_ID_END); cnt++) {

        /* iterate through each available port plus host port */
        for (cntPort = 0; GOAL_RES_OK(res) && (cntPort < cntPortMax); cntPort++) {

            /* check if statistics entry is not used */
            res = goal_statIsUnused(GOAL_ID_ETH, mStatIds[cnt], cntPort);
            if (GOAL_RES_ERR(res)) {

                /* skip to next statistics entry */
                break;
            }
        }

        /* leave if statistics already exist */
        if (GOAL_RES_ERR(res)) {

            /* mark situation as no-problem */
            res = GOAL_OK;

            break;
        }

        /* allocate values after entry was registered to not waste memory */
        res = goal_memCalloc(&(pMiEthHdl->statBuf[cnt]), sizeof(uint64_t) * cntPortMax);
        if (GOAL_RES_ERR(res)) {
            /* GG_LOG ID:111: */
            goal_lmLog(GOAL_ID_ETH, 111, 82, 0, 0, GOAL_LOG_SEV_ERROR, "failed to allocate statistics values");
            break;
        }

        /* iterate through each available port plus host port */
        for (cntPort = 0; GOAL_RES_OK(res) && (cntPort < cntPortMax); cntPort++) {

            /* assign port id */
            idPort = (pMiEthHdl->numPorts > cntPort) ? cntPort : GOAL_ETH_PORT_HOST;

            /* register statistics entry */
            res = goal_statNew(NULL, (GOAL_ID_T) pMiEthHdl->id, mStatIds[cnt], idPort, GOAL_TRUE, goal_miEthStatGet, goal_miEthStatReset, &(pMiEthHdl->statBuf[cnt][cntPort]));
            if (GOAL_RES_ERR(res)) {
                /* GG_LOG ID:112: */
                goal_lmLog(GOAL_ID_ETH, 112, 83, 0, 0, GOAL_LOG_SEV_ERROR, "failed to register statistics value");
                break;
            }
        }

        /* mark statistics counter as available */
        if (GOAL_RES_OK(res)) {
            /* GG_LOG ID:113: */
            goal_lmLog(GOAL_ID_ETH, 113, 84, 4, 0, GOAL_LOG_SEV_INFO, "internally providing statistics counter: $1");
            goal_lmLogParamUINT32(mStatIds[cnt]);
            goal_lmLogFinish();
        }
    }
#endif /* GOAL_CONFIG_ETH_STATS_INTERNAL */


    return res;
}


/****************************************************************************/
/** Ethernet Interface Command Check
 *
 * Look up if input command is supported, found in registered table
 *
 * @retval GOAL_OK command is supported
 * @retval other command is not supported
 */
GOAL_STATUS_T goal_miEthCmdExist(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    GOAL_ETH_CMD_T cmd                          /**< command */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    if (NULL == pMiInst) {
        /* get inst by id */
        res = goal_instGetById((GOAL_INSTANCE_T **) &pMiInst, GOAL_ID, GOAL_ID_DEFAULT);
    }

    res = goal_miEthCmdExistTable(pMiInst, cmd, pMiInst->pEthTableCmd);

    return res;
}


/****************************************************************************/
/** Ethernet Interface Command Check
 *
 * Looks up if input command is supported by input function table.
 *
 * @retval GOAL_OK command is supported
 * @retval other command is not supported
 */
GOAL_STATUS_T goal_miEthCmdExistTable(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    GOAL_ETH_CMD_T cmd,                         /**< command */
    GOAL_ETH_FUNCTION_ENTRY_T *pTableFunc       /**< Ethernet command function table */
)
{
    uint32_t cnt;                               /* count */

    UNUSEDARG(pMiInst);

    if (NULL == pTableFunc) {
        /* GG_LOG ID:129: */
        goal_lmLog(GOAL_ID_ETH, 129, 100, 0, 0, GOAL_LOG_SEV_ERROR, "No Ethernet functions registered.");
        return GOAL_ERROR;
    }

    /* looking up the list for cmd */
    for (cnt = 0; pTableFunc[cnt].cmd < GOAL_ETH_CMD_END; cnt++) {
        if (pTableFunc[cnt].cmd == cmd) {
            return GOAL_OK;
        }
    }
    return GOAL_ERR_UNSUPPORTED;
}


#if GOAL_ETH_NAMES == 1
/****************************************************************************/
/** Returns the name of input command.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
const char * goal_miEthCmdNameGet(
    uint32_t cmd                                /**< command id */
)
{
    if (GOAL_ETH_CMD_END <= cmd) {
        return NULL;
    }

    return goal_miEthCmdNames[cmd];
}
#endif /* GOAL_ETH_NAMES */


/****************************************************************************/
/** Get interface link state
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_miEthLinkStateGet(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t port,                              /**< port ID */
    uint32_t *pState                            /**< link state ref */
)
{
    return goal_miEthCmd(pMiInst, GOAL_ETH_CMD_LINK_STATE, GOAL_FALSE, port, pState);
}


/****************************************************************************/
/** Get port state
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_miEthPortStateGet(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t port,                              /**< port ID */
    uint32_t *pState                            /**< port state ref */
)
{
    return goal_miEthCmd(pMiInst, GOAL_ETH_CMD_PORT_STATE, GOAL_FALSE, port, pState);
}


/****************************************************************************/
/** Set port state
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_miEthPortStateSet(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t port,                              /**< port ID */
    uint32_t state                              /**< port state */
)
{
    return goal_miEthCmd(pMiInst, GOAL_ETH_CMD_PORT_STATE, GOAL_TRUE, port, &state);
}


/****************************************************************************/
/** Get interface MAC address
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_miEthMacAddrGet(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t port,                              /**< port ID */
    GOAL_ETH_MAC_ADDR_T *pMac                   /**< MAC address ptr */
)
{
    return goal_miEthCmd(pMiInst, GOAL_ETH_CMD_MAC_ADDR, GOAL_FALSE, port, pMac);
}


/****************************************************************************/
/** Configurate interface MAC address
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_miEthCfgMacAddrImpl(
    uint32_t port,                              /**< port ID */
    uint32_t idMa,                              /**< associated MA */
    GOAL_ETH_MAC_ADDR_T *pMac                   /**< MAC address ptr */
)
{
    if (*pMac == NULL) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* unused */
    UNUSEDARG(idMa);

    /* copy mac and port into module */
    mCfg.port = port;
    GOAL_MEMCPY(mCfg.macAddr, *pMac, MAC_ADDR_LEN);
    mFlgCfgSet = GOAL_TRUE;

    return GOAL_OK;
}


/****************************************************************************/
/** Set interface MAC address
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_miEthMacAddrSet(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t port,                              /**< port ID */
    GOAL_ETH_MAC_ADDR_T *pMac                   /**< MAC address ptr */
)
{
    return goal_miEthCmd(pMiInst, GOAL_ETH_CMD_MAC_ADDR, GOAL_TRUE, port, pMac);
}


/****************************************************************************/
/** Flush unicast MAC address table
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_miEthMacUniFlush(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t port                               /**< port ID */
)
{
    return goal_miEthCmd(pMiInst, GOAL_ETH_CMD_MAC_UNI_FLUSH, GOAL_FALSE, port, NULL);
}


/****************************************************************************/
/** Flush multicast MAC address table
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_miEthMacMultiFlush(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t port                               /**< port ID */
)
{
    return goal_miEthCmd(pMiInst, GOAL_ETH_CMD_MAC_MULTI_FLUSH, GOAL_FALSE, port, NULL);
}


/****************************************************************************/
/** Add multicast MAC address
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_miEthMcastAdd(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    GOAL_ETH_PORT_T port,                       /**< port or mask */
    GOAL_ETH_MAC_ADDR_T *pMac                   /**< MAC address */
)
{
    GOAL_STATUS_T res;                          /* Result */
    GOAL_ETH_MACTAB_ENTRY_T entry;              /* MAC table entry */

    /* make sure we use a port mask */
    if (!(port & GOAL_ETH_PORT_MASK)) {
        /* GG_LOG ID:130: */
        goal_lmLog(GOAL_ID_ETH, 130, 101, 0, 0, GOAL_LOG_SEV_ERROR, "need a portmask to proceed");
        return GOAL_ERR_PARAM;
    }

    /* get current port config for MAC address */
    entry.portmask = 0;
    entry.addr = pMac;

    /* get isn't implemented on all platforms, ignore result */
    goal_miEthCmd(pMiInst, GOAL_ETH_CMD_MACTAB_GET, GOAL_FALSE, GOAL_ETH_PORT_HOST, &entry);

    /* add requested port(s) to current port mask */
    entry.portmask |= port;
    res = goal_miEthCmd(pMiInst, GOAL_ETH_CMD_MACTAB_SET, GOAL_TRUE, GOAL_ETH_PORT_HOST, &entry);

    if (GOAL_RES_OK(res)) {
        /* GG_LOG ID:26: */
        goal_lmLog(GOAL_ID_ETH, 26, 16, 6, 0, GOAL_LOG_SEV_INFO, "added multicast address: $1");
        goal_lmLogParamMAC(&(*pMac)[0]);
        goal_lmLogFinish();
    } else {
        /* GG_LOG ID:27: */
        goal_lmLog(GOAL_ID_ETH, 27, 17, 6, 0, GOAL_LOG_SEV_ERROR, "failed to add multicast address: $1");
        goal_lmLogParamMAC(&(*pMac)[0]);
        goal_lmLogFinish();
    }

    return res;
}


/****************************************************************************/
/** Delete multicast MAC address
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_miEthMcastDel(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    GOAL_ETH_PORT_T port,                       /**< port or mask */
    GOAL_ETH_MAC_ADDR_T *pMac                   /**< MAC address */
)
{
    GOAL_STATUS_T res;                          /* Result */
    GOAL_ETH_MACTAB_ENTRY_T entry;              /* MAC table entry */

    /* prepare entry structure */
    entry.portmask = 0;
    entry.addr = pMac;

    /* get isn't implemented on all platforms, ignore result */
    goal_miEthCmd(pMiInst, GOAL_ETH_CMD_MACTAB_GET, GOAL_FALSE, GOAL_ETH_PORT_HOST, &entry);

    /* remove portmask */
    entry.portmask = GOAL_ETH_PORT_MASK | (entry.portmask & ~port);
    res = goal_miEthCmd(pMiInst, GOAL_ETH_CMD_MACTAB_SET, GOAL_TRUE, GOAL_ETH_PORT_HOST, &entry);

    if (GOAL_RES_OK(res)) {
        /* GG_LOG ID:28: */
        goal_lmLog(GOAL_ID_ETH, 28, 18, 6, 0, GOAL_LOG_SEV_INFO, "removed multicast address: $1");
        goal_lmLogParamMAC(&(*pMac)[0]);
        goal_lmLogFinish();
    }

    return res;
}


/****************************************************************************/
/** Get interface speed
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_miEthLinkSpeedGet(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t port,                              /**< port ID */
    uint32_t *pSpeed                            /**< speed ref */
)
{
    return goal_miEthCmd(pMiInst, GOAL_ETH_CMD_SPEED, GOAL_FALSE, port, pSpeed);
}


/****************************************************************************/
/** Set interface speed
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_miEthLinkSpeedSet(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t port,                              /**< port ID */
    uint32_t speed                              /**< speed */
)
{
    return goal_miEthCmd(pMiInst, GOAL_ETH_CMD_SPEED, GOAL_TRUE, port, &speed);
}


/****************************************************************************/
/** Get interface duplex
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_miEthLinkDuplexGet(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t port,                              /**< port ID */
    uint32_t *pDuplex                           /**< duplex ref */
)
{
    return goal_miEthCmd(pMiInst, GOAL_ETH_CMD_DUPLEX, GOAL_FALSE, port, pDuplex);
}


/****************************************************************************/
/** Set interface duplex
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_miEthLinkDuplexSet(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t port,                              /**< port ID */
    uint32_t duplex                             /**< duplex */
)
{
    return goal_miEthCmd(pMiInst, GOAL_ETH_CMD_DUPLEX, GOAL_TRUE, port, &duplex);
}


/****************************************************************************/
/** Get interface MDI mode
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miEthMdiModeGet(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t port,                              /**< port id */
    uint32_t *pModeMdi                          /**< MDI mode */
)
{
    return goal_miEthCmd(pMiInst, GOAL_ETH_CMD_MDI_MODE, GOAL_FALSE, port, pModeMdi);
}


/****************************************************************************/
/** Set interface MDI mode
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miEthMdiModeSet(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t port,                              /**< port id */
    uint32_t modeMdi                            /**< MDI mode */
)
{
    return goal_miEthCmd(pMiInst, GOAL_ETH_CMD_MDI_MODE, GOAL_TRUE, port, &modeMdi);
}


/****************************************************************************/
/** Get interface MDI state
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miEthMdiStateGet(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t port,                              /**< port id */
    uint32_t *pStateMdi                         /**< MDI state */
)
{
    return goal_miEthCmd(pMiInst, GOAL_ETH_CMD_MDI_STATE, GOAL_FALSE, port, pStateMdi);
}


/****************************************************************************/
/** Set interface MDI state
 *
 * Setting the MDI state with this API enforces the MDI mode 'manual'.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miEthMdiStateSet(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t port,                              /**< port id */
    uint32_t stateMdi                           /**< MDI state */
)
{
    /* switch mode to 'manual' if supported by the platform */
    goal_miEthMdiModeSet(pMiInst, port, GOAL_ETH_MDI_MODE_MANUAL);

    return goal_miEthCmd(pMiInst, GOAL_ETH_CMD_MDI_STATE, GOAL_TRUE, port, &stateMdi);
}


/****************************************************************************/
/** Get interface autoneg state
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_miEthAutonegGet(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t port,                              /**< port ID */
    uint32_t *pAutoneg                          /**< autoneg ref */
)
{
    return goal_miEthCmd(pMiInst, GOAL_ETH_CMD_AUTONEG, GOAL_FALSE, port, pAutoneg);
}


/****************************************************************************/
/** Set interface autoneg state
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_miEthAutonegSet(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t port,                              /**< port ID */
    uint32_t autoneg                            /**< autoneg */
)
{
    return goal_miEthCmd(pMiInst, GOAL_ETH_CMD_AUTONEG, GOAL_TRUE, port, &autoneg);
}


/****************************************************************************/
/** Get Autonegotiation progress
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_miEthAutonegProgressGet(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t port,                              /**< port ID */
    uint32_t *pAutoneg                          /**< autoneg ref */
)
{
    return goal_miEthCmd(pMiInst, GOAL_ETH_CMD_AUTONEG_PROGRESS, GOAL_FALSE, port, pAutoneg);
}


/****************************************************************************/
/** Get type of physics for port
 *
 */
GOAL_STATUS_T goal_miEthPhysicsGet(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t port,                              /**< port ID */
    uint32_t *pPhysics                          /**< physics ref */
)
{
    return goal_miEthCmd(pMiInst, GOAL_ETH_CMD_PHYSICS, GOAL_FALSE, port, pPhysics);
}


/****************************************************************************/
/** Update all port states for all external ports
 *
 * A toggling link state initiates an update of the link speed.
 * Autonegotiation process requires an enabled autonegotiation for update.
 */
static void goal_miEthPortStateUpdate(
    GOAL_MI_ETH_T *pMiInst                      /**< MI instance */
)
{
    GOAL_STATUS_T res;                          /* result */
    unsigned int id;                            /* port id */
    uint32_t val;                               /* value storage */
    uint32_t maskChg;                           /* state change mask */
    GOAL_ETH_PORT_STATE_CB_T *pHdlCb;           /* callback handle */
    uint32_t numOfPorts = 0;                    /* number of ethernet ports */

    numOfPorts = pMiInst->numPorts;

    /* iterate through all external ports */
    for (id = 0; id < numOfPorts; id++) {

        /* clear change mask */
        maskChg = 0;

        /* link state */
        res = goal_miEthLinkStateGet(pMiInst, id, &val);
        if (GOAL_RES_OK(res) && (pMiInst->pStatePorts[id].stateLink != val)) {
            maskChg |= GOAL_ETH_PORT_STATE_LINK;
            pMiInst->pStatePorts[id].stateLink = val;
            goal_miEthCmLinkStateSet(pMiInst, id, val);
        }

        /* get duplex mode */
        res = goal_miEthLinkDuplexGet(pMiInst, id, &val);
        if (GOAL_RES_OK(res) && (pMiInst->pStatePorts[id].modeDuplex != val)) {
            maskChg |= GOAL_ETH_PORT_STATE_DUPLEX;
            pMiInst->pStatePorts[id].modeDuplex = val;
            goal_miEthCmDuplexSet(pMiInst, id, val);
        }

        /* get link speed */
        res = goal_miEthLinkSpeedGet(pMiInst, id, &val);
        if (GOAL_RES_OK(res) && (pMiInst->pStatePorts[id].speed != val)) {
            maskChg |= GOAL_ETH_PORT_STATE_SPEED;
            pMiInst->pStatePorts[id].speed = val;
            goal_miEthCmSpeedSet(pMiInst, id, val);
        }

        /* autonegotiation */
        res = goal_miEthAutonegGet(pMiInst, id, &val);
        if (GOAL_RES_OK(res) && (pMiInst->pStatePorts[id].stateAutoNeg != val)) {
            maskChg |= GOAL_ETH_PORT_STATE_AUTONEG;
            pMiInst->pStatePorts[id].stateAutoNeg = val;
        }

        /* autonegotiation progress - get progress state only, if autoneg is enabled */
        if (GOAL_ETH_AUTONEG_ON == pMiInst->pStatePorts[id].stateAutoNeg) {
            res = goal_miEthAutonegProgressGet(pMiInst, id, &val);
            if (GOAL_RES_OK(res) && (pMiInst->pStatePorts[id].stateAutoNegProgress != val)) {
                maskChg |= GOAL_ETH_PORT_STATE_AUTONEG_PROGRESS;
                pMiInst->pStatePorts[id].stateAutoNegProgress = val;
            }
        }
        else {
           /* in case of autoneg being disabled set the proper progress value and change mask bit */
            val = GOAL_ETH_AUTONEG_SKIPPED;
            if (val != pMiInst->pStatePorts[id].stateAutoNegProgress) {
                maskChg |= GOAL_ETH_PORT_STATE_AUTONEG_PROGRESS;
                pMiInst->pStatePorts[id].stateAutoNegProgress = val;
            }
        }

        /* if mask bits are set call callback handlers */
        if (maskChg) {
            goal_logDbg("state change on port: %u, state: 0x%"FMT_x32, id, maskChg);

            for (pHdlCb = pMiInst->pStatePorts[id].pCb; pHdlCb; pHdlCb = pHdlCb->pNext) {
                if (GOAL_TRUE == pHdlCb->hasArg) {
                    pHdlCb->cb.pFuncArg(id, maskChg, &pMiInst->pStatePorts[id], pHdlCb->pArg);
                }
                else {
                    pHdlCb->cb.pFuncNoArg(id, maskChg, &pMiInst->pStatePorts[id]);
                }
            }
        }
    }
}


/****************************************************************************/
/** Register Ethernet port change handler
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_miEthPortStateCbReg(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t id,                                /**< port id */
    GOAL_ETH_PORT_STATE_CB_FUNC_T pFunc         /**< callback function */
)
{
    GOAL_ETH_PORT_STATE_CB_UNION_T cb;          /* callback union */

    cb.pFuncNoArg = pFunc;

    return goal_miEthPortStateGenericCbReg(pMiInst, id, cb, GOAL_FALSE, NULL);
}


/****************************************************************************/
/** Register Ethernet port change handler with additional argument
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_miEthPortStateArgCbReg(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t id,                                /**< port id */
    GOAL_ETH_PORT_STATE_ARG_CB_FUNC_T pFunc,    /**< callback function */
    void *pArg                                  /**< [in] callback argument to register */
)
{
    GOAL_ETH_PORT_STATE_CB_UNION_T cb;          /* callback union */

    cb.pFuncArg = pFunc;

    return goal_miEthPortStateGenericCbReg(pMiInst, id, cb, GOAL_TRUE, pArg);
}


#if GOAL_CONFIG_DD == 1
/****************************************************************************/
/** Initializes the CM vars of the Ethernet module
 *
 */
static GOAL_STATUS_T goal_miEthCmVarsInit(
    GOAL_MI_ETH_T *pMiInst                      /**< [in] MI instance */
)
{
    GOAL_STATUS_T res;                          /* result */

    res = goal_miEthVirtCmVarGetById(pMiInst, GOAL_CM_ETH_MOD_ID, ETH_CM_VAR_LINK, &pMiInst->pVarLink);

    if (GOAL_RES_OK(res)) {
        res = goal_miEthVirtCmVarGetById(pMiInst, GOAL_CM_ETH_MOD_ID, ETH_CM_VAR_SPEED, &pMiInst->pVarSpeed);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_miEthVirtCmVarGetById(pMiInst, GOAL_CM_ETH_MOD_ID, ETH_CM_VAR_DUPLEX, &pMiInst->pVarDuplex);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_miEthVirtCmVarGetById(pMiInst, GOAL_CM_ETH_MOD_ID, ETH_CM_VAR_PORTCNT, &pMiInst->pVarPortCnt);
    }

    return res;
}


/****************************************************************************/
/** Set value of CM variable for the link state
 *
 */
static GOAL_STATUS_T goal_miEthCmLinkStateSet(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t port,                              /**< port */
    uint32_t state                              /**< link state */
)
{
    uint32_t val = 0;                           /* value */
    GOAL_CM_GETVAR_UINT32(pMiInst->pVarLink, val);
    val &= ~((uint32_t)(GOAL_ETH_CM_LINK_MASK << (port * GOAL_ETH_CM_LINK_MASK_SIZE)));
    val |= state << (port * GOAL_ETH_CM_LINK_MASK_SIZE);
    GOAL_CM_SETVAR_UINT32(pMiInst->pVarLink, val);

    return GOAL_OK;
}


/****************************************************************************/
/** Set the value of CM variable for the duplex state
 *
 */
static GOAL_STATUS_T goal_miEthCmDuplexSet(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t port,                              /**< port */
    uint32_t mode                               /**< duplex mode */
)
{
    uint32_t val = 0;                           /* value */
    GOAL_CM_GETVAR_UINT32(pMiInst->pVarDuplex, val);
    val &= ~((uint32_t) (GOAL_ETH_CM_DUPLEX_MASK << (port * GOAL_ETH_CM_DUPLEX_MASK_SIZE)));
    val |= (mode) << (port * GOAL_ETH_CM_DUPLEX_MASK_SIZE);
    GOAL_CM_SETVAR_UINT32(pMiInst->pVarDuplex, val);

    return GOAL_OK;
}


/****************************************************************************/
/** Set value of CM variable for the link speed
 *
 */
static GOAL_STATUS_T goal_miEthCmSpeedSet(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t port,                              /**< port */
    uint32_t speed                              /**< speed */
)
{
    uint32_t val = 0;                           /* value */
    uint32_t cspeed;                            /* converted speed value */
    switch (speed) {
        case GOAL_ETH_SPEED_10:
            cspeed = GOAL_ETH_CM_SPEED_10;
            break;
        case GOAL_ETH_SPEED_100:
            cspeed = GOAL_ETH_CM_SPEED_100;
            break;
        case GOAL_ETH_SPEED_1000:
            cspeed = GOAL_ETH_CM_SPEED_1000;
            break;
        default:
            cspeed = speed;
    }

    GOAL_CM_GETVAR_UINT32(pMiInst->pVarSpeed, val);
    val &= ~((uint32_t) (GOAL_ETH_CM_SPEED_MASK << (port * GOAL_ETH_CM_SPEED_MASK_SIZE)));
    val |= (cspeed) << (port * GOAL_ETH_CM_SPEED_MASK_SIZE);
    GOAL_CM_SETVAR_UINT32(pMiInst->pVarSpeed, val);

    return GOAL_OK;
}
#endif /* GOAL_CONFIG_DD */


/****************************************************************************/
/** Register Ethernet port change handler with or without additional argument
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_miEthPortStateGenericCbReg(
    GOAL_MI_ETH_T *pMiInst,                     /**< [in] MI instance */
    uint32_t id,                                /**< port id */
    GOAL_ETH_PORT_STATE_CB_UNION_T cb,          /**< callback union */
    GOAL_BOOL_T hasArg,                         /**< callback uses additional callback argument */
    void *pArg                                  /**< [in] additional callback argument (NULL if unused) */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_ETH_PORT_STATE_CB_T **ppHdlCb;         /* callback handle ref */
    uint32_t maskChg;                           /* state change mask */

    if (NULL == pMiInst) {
        /* get handle of default interface */
        res = goal_miEthHdlGet(GOAL_ID_DEFAULT, &pMiInst);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
    }

    /* find a free entry */
    for (ppHdlCb = &pMiInst->pStatePorts[id].pCb; *ppHdlCb; ppHdlCb = &(*ppHdlCb)->pNext);

    /* allocate callback structure */
    res = goal_memCalloc(ppHdlCb, sizeof(GOAL_ETH_PORT_STATE_CB_T));
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* fill callback structure */
    (*ppHdlCb)->cb = cb;
    (*ppHdlCb)->hasArg = hasArg;
    (*ppHdlCb)->pArg = pArg;

    /* initially call callback */
    maskChg = (
        GOAL_ETH_PORT_STATE_LINK |
        GOAL_ETH_PORT_STATE_DUPLEX |
        GOAL_ETH_PORT_STATE_SPEED |
        GOAL_ETH_PORT_STATE_AUTONEG |
        GOAL_ETH_PORT_STATE_AUTONEG_PROGRESS);

    if (GOAL_TRUE == hasArg) {
        cb.pFuncArg(id, maskChg, &pMiInst->pStatePorts[id], pArg);
    }
    else {
        cb.pFuncNoArg(id, maskChg, &pMiInst->pStatePorts[id]);
    }

    return res;
}


#if GOAL_CONFIG_ETH_STATS_INTERNAL == 1
/****************************************************************************/
/** Statistics Get Callback
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_miEthStatGet(
    uint64_t *pVal,                             /**< [out] value */
    struct GOAL_STAT_T *pStat                   /**< statistics data */
)
{
    *pVal = *((const uint64_t *) pStat->pPriv);

    return GOAL_OK;
}


/****************************************************************************/
/** Statistics Reset Callback
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_miEthStatReset(
    uint64_t *pVal,                             /**< [out] value */
    struct GOAL_STAT_T *pStat                   /**< statistics data */
)
{
    /* return last stored value */
    *pVal = *((const uint64_t *) pStat->pPriv);

    /* access 'const' pointer content */
    *((uint64_t *)(uintptr_t) pStat->pPriv) = 0;

    return GOAL_OK;
}


/****************************************************************************/
/** Increment an internal statistic counter
 */
void goal_miEthStatInternalInc(
    GOAL_MI_ETH_T *pMiEthHdl,                   /**< [in] MI handle */
    GOAL_ETH_STAT_ENUM_T statId,                /**< statistic type */
    uint32_t portId,                            /**< port Index */
    uint64_t incVal                             /**< increment value */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    uint32_t idxPort;                           /* port index */

    if (NULL == pMiEthHdl) {
        /* get inst by id */
        res = goal_instGetById((GOAL_INSTANCE_T **) &pMiEthHdl, GOAL_ID, GOAL_ID_DEFAULT);
    }

    if (GOAL_RES_OK(res) && (statId < GOAL_ETH_STAT_ID_END)) {
        if (1 == pMiEthHdl->numPorts) {
            idxPort = 0;
        }
        else if (portId < pMiEthHdl->numPorts) {
            idxPort = portId;
        }
        else {
            idxPort = pMiEthHdl->numPorts;
        }

        pMiEthHdl->statBuf[portId][idxPort] += incVal;
    }
}
#endif /* GOAL_CONFIG_ETH_STATS_INTERNAL */


/****************************************************************************/
/** Query MTU size
 *
 * @retval MTU size in bytes
 */
GOAL_TARGET_WEAK uint32_t goal_miEthMtuGet(
    void
)
{
    return GOAL_TGT_MTU_SIZE;
}


#if GOAL_PTP == 1
/****************************************************************************/
/** Register a timestamp capture callback
 *
 * The callback will be invoked by the Ethernet driver whenever an Egress
 * timestamp has been captured. This function can be
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_miEthTimestampCaptureCbReg(
    GOAL_MI_ETH_T *pMiInst,                     /**< Eth MI instance */
    GOAL_ETH_TIMESTAMP_CB_T pCb,                /**< callback function */
    void *pArg                                  /**< callback argument */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    if (NULL == pMiInst) {
        res = goal_instGetById((GOAL_INSTANCE_T **) &pMiInst, GOAL_ID, GOAL_ID_DEFAULT);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
    }

    if (GOAL_RES_OK(res)) {
        if ((NULL != pMiInst->pPortTsCbFunc) && (NULL != pCb) && (pMiInst->pPortTsCbFunc != pCb)) {
            /* deny overload of callback */
            res = GOAL_ERR_ALREADY_USED;
        }
    }

    if (GOAL_RES_OK(res)) {
        /* allow setting and clearing of callback */
        pMiInst->pPortTsCbFunc = pCb;
        pMiInst->pPortTsCbArg = pArg;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Register a timestamp capture callback
 *
 * The function must be called by the Ethernet driver if a egress timestamp has
 * been captured for a frame. The function invokes the registered callback
 * handler to pass the data to another module.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
void goal_miEthTimestampCaptureCbExec(
    GOAL_MA_ETH_T *pMaEthHdl,                   /**< [in] MA handle */
    uint64_t sec,                               /**< full seconds */
    uint32_t nsec,                              /**< sub-seconds in ns */
    uint32_t port                               /**< sending port */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MI_ETH_T *pMiInst;                     /* instance */

    /* get mi bound to ma */
    if (NULL != pMaEthHdl) {
        pMiInst = (GOAL_MI_ETH_T *) pMaEthHdl->pMiInst;
    }
    else {
        /* otherwise get mi by id */
        res = goal_instGetById((GOAL_INSTANCE_T **) &pMiInst, GOAL_ID, GOAL_ID_DEFAULT);
        if (GOAL_RES_ERR(res)) {
            return;
        }
    }

    if ((NULL != pMiInst) && (NULL != pMiInst->pPortTsCbFunc)) {
        pMiInst->pPortTsCbFunc(pMiInst->pPortTsCbArg, sec, nsec, port);
    }
}
#endif /* GOAL_PTP == 1 */
