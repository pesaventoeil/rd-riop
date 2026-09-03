/** @file
 *
 * @brief NETC Ethernet driver
 *
 * @copyright
 * Copyright 2010-2022 port GmbH.
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

#define GOAL_ID GOAL_ID_TGT
#include <goal_includes.h>
#include <goal_target_eth.h>
#include <goal_media/goal_mi_eth.h>
#if GOAL_PTP == 1
# include "goal_target_ptp.h"
#endif
#if GOAL_DLR == 1
# include <plat/drv/dlr/nxp_netc/dlr_netc.h>
#endif
#include "fsl_msgintr.h"
#include "fsl_netc_endpoint.h"
#include "fsl_netc_switch.h"


/****************************************************************************/
/* Local defines */
/****************************************************************************/
#define GOAL_DRV_ETH_PRIO_NUM                 8 /**< number of priorities */

#define GOAL_DRV_ETH_TX_MSIX_ENTRY_IDX        0 /**< Tx interrupt setting index */
#define GOAL_DRV_ETH_RX_MSIX_ENTRY_IDX        1 /**< Rx interrupt setting index */
#define GOAL_DRV_ETH_TX_INTR_CHAN             0 /**< MSI message channel for Tx interrupts */
#define GOAL_DRV_ETH_RX_INTR_CHAN             1 /**< MSI message channel for Rx interrupts */

#define GOAL_DRV_ETH_SWITCH_PORTS_NUM         5 /**< number of available Switch ports incl. host port */
#define GOAL_DRV_ETH_SWITCH_HOST_PORT_ID      4 /**< host port of Switch (Pseudo MAX) */

#define GOAL_DRV_ETH_SEC_IN_NSEC     1000000000 /**< 1 s in ns */

#define GOAL_DRV_ETH_TASK_PRIO_RX            75 /**< task priority for Eth Rx Task */
#define GOAL_DRV_ETH_TASK_PRIO_TX            85 /**< task priority for Eth Tx Task */
#define GOAL_DRV_ETH_TASK_STACK_SIZE       1024 /**< stack size for Eth Tx/Rx Tasks */

#define GOAL_DRV_ETH_RX_LOOP_CNT              3 /**< number of Rx loop iteration */

#define GOAL_DRV_ETH_VLAN_PRIO_NUM_MAX        8 /**< number of vlan priority mappings */
#define GOAL_DRV_ETH_VLAN_PRIO_UNUSED      0xFF /**< vlan priority mapping unused entry */
#define GOAL_DRV_ETH_ETYPE_PRIO_UNUSED     0xFF /**< ethertype priority mapping unused entry */

#define GOAL_DRV_TDMA_CB_OFFSET_MIN      300000 /**< minimum offset between Qbv callback and cycle time in ns */

#define GOAL_DRV_ETH_PFPCR_MASK      0x000000ff /**< Port frame preemption configuration register mask */
#if GOAL_DLR == 1
# define GOAL_DRV_ETH_L2_PARSER_PL_MAX       24 /**< maximum number of bytes the L2 Parser has to check */
#else /* GOAL_DLR */
# define GOAL_DRV_ETH_L2_PARSER_PL_MAX        8 /**< maximum number of bytes the L2 Parser has to check */
#endif /* GOAL_DLR */

#define GOAL_DRV_ETH_RXBDR_NUM_DEFAULT        0 /**< Rx BDR number for regular frames  */

#define GOAL_DRV_ETH_DLR_MAC_BASE {0x01, 0x21, 0x6C, 0x00, 0x00, 0x00} /**< DLR Destination MAC address base */
#define GOAL_DRV_ETH_DLR_MAC_NUM              5 /**< number of DLR Destination MAC addresses */
#define GOAL_DRV_ETH_DLR_MAC_BEACON        0x01 /**< last Byte of DLR Dst MAC of Beacon frames */
#define GOAL_DRV_ETH_DLR_MAC_SIGNON        0x02 /**< last Byte of DLR Dst MAC of SignOn & NeighborCheck frames */
#define GOAL_DRV_ETH_DLR_MAC_ANNOUNCE      0x03 /**< last Byte of DLR Dst MAC of Announce frames */
#define GOAL_DRV_ETH_DLR_MAC_ADVERTISE     0x04 /**< last Byte of DLR Dst MAC of Advertise frames */
#define GOAL_DRV_ETH_DLR_MAC_LEARNING      0x05 /**< last Byte of DLR Dst MAC of Learning_Update frames */

#define GOAL_DRV_ETH_MBOX_NUM                10 /**< number of Rx IRQ Mailbox entries */
#define GOAL_DRV_ETH_MBOX_SIZE                1 /**< size of Rx IRQ Mailbox entry */

#define GOAL_DRV_ETH_MA_ID_MAX (31 / GOAL_DRV_ETH_TC_MAX + 1) /**< maximum MA ID supported by this driver */


/****************************************************************************/
/* Local functions */
/****************************************************************************/
static GOAL_STATUS_T goal_drvEthNetcInit(
    GOAL_MA_ETH_T *pMaHdl                       /**< MA Hdl */
);

static GOAL_STATUS_T goal_drvEthNetcOpen(
    GOAL_MA_ETH_T *pMaHdl                       /**< MA Hdl */
);

static void goal_drvEthNetcSend(
    GOAL_MA_ETH_T *pMaHdl                       /**< MA Hdl */
);

static GOAL_STATUS_T goal_drvEthNetcCmd(
    GOAL_MA_ETH_T *pMaHdl,                      /**< MA handle */
    uint32_t id,                                /**< command ID */
    GOAL_BOOL_T wrFlag,                         /**< write flag */
    uint32_t port,                              /**< port ID */
    void *pArg                                  /**< argument */
);

#if GOAL_CONFIG_TASK == 1
static GOAL_STATUS_T goal_drvEthNetcRxTask(
    void *pArg                                  /**< task argument */
);

static GOAL_STATUS_T goal_drvEthNetcTxTask(
    void *pArg                                  /**< task argument */
);
#endif /* GOAL_CONFIG_TASK */

static void goal_drvEthNetcCbExec(
    MSGINTR_Type *pBase,                        /**< MSI controller base */
    uint8_t channel,                            /**< channel ID */
    uint32_t pendingIntr                        /**< interrupt message data */
);

static void goal_drvEthNetcRxHandler(
    GOAL_MA_ETH_T *pMaEthHdl,                   /**< MA Hdl */
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData          /**< private driver data */
);

static void goal_drvEthNetcTxHandler(
    GOAL_MA_ETH_T *pMaEthHdl,                   /**< MA Hdl */
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData          /**< private driver data */
);

static void goal_drvEthNetcTxDescRingGet(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_BUFFER_T *pBuf,                        /**< GOAL network buffer */
    uint8_t *pRingIdx                           /**< [out] index of Tx ring */
);

static status_t goal_drvEthNetcSwitchTxDoneCbExec(
    swt_handle_t *pSwHandle,                    /**< switch driver handle */
    netc_tx_frame_info_t *pFrameInfo,           /**< frame info data */
    void *pUserData                             /**< user data */
);

static status_t goal_drvEthNetcTxDoneCbExec(
    ep_handle_t *pEpHandle,                     /**< Endpoint driver handle */
    uint8_t ring,                               /**< descriptor ring */
    netc_tx_frame_info_t *pFrameInfo,           /**< frame info data */
    void *pUserData                             /**< user data */
);

#if GOAL_PTP == 1
static void goal_drvEthNetcTxTimestampIdPortAdd(
    GOAL_MA_ETH_T *pMaEthHdl,                   /**< Eth MA handle */
    uint16_t txTsId,                            /**< Tx Timestamp ID */
    uint32_t txPort                             /**< egress port */
);

static void goal_drvEthNetcTxTimestampIdValueAdd(
    GOAL_MA_ETH_T *pMaEthHdl,                   /**< Eth MA handle */
    uint16_t txTsId,                            /**< Tx Timestamp ID */
    uint32_t txTimeStamp                        /**< Tx Timestamp */
);
#endif /* GOAL_PTP */

static void goal_drvEthNetcPhyStateCbExec(
    GOAL_ETH_PORT_T port,                       /**< GOAL port id */
    uint32_t maskChg,                           /**< change mask */
    GOAL_ETH_PORT_STATE_T *pState,              /**< port state ptr */
    void *pArg                                  /**< [in] registered callback argument */
);

static GOAL_STATUS_T goal_drvEthNetcCmdMacAddr(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T goal_drvEthNetcCmdPhy(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T goal_drvEthNetcCmdNumPorts(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T goal_drvEthNetcPortToTargetConvert(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    uint32_t goalPort,                          /**< GOAL port ID/mask */
    uint32_t *pTgtPort                          /**< target port ID/mask */
);

static GOAL_STATUS_T goal_drvEthNetcPortFromTargetConvert(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    uint32_t tgtPort,                           /**< target port ID/mask */
    uint32_t *pGoalPort                         /**< GOAL port ID/mask */
);

static GOAL_STATUS_T goal_drvEthNetcCmdMacTabSet(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T goal_drvEthNetcCmdMacTabGet(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T goal_drvEthNetcCmdMacTabClear(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T goal_drvEthNetcCmdQoSMode(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T goal_drvEthNetcCmdVlanPrio(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T goal_drvEthNetcCmdEthTypePrio(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

#if GOAL_CONFIG_TDMA == 1
static GOAL_STATUS_T goal_drvEthNetcCmdTdmaConfig(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T goal_drvEthNetcCmdTdmaTsltConfig(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T goal_drvEthNetcCmdTdmaEnable(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static void goal_drvEthNetcPtpCbExec(
    void *pArg                                  /**< callback argument */
);
#endif /* GOAL_CONFIG_TDMA */

#if GOAL_CONFIG_DRV_ETH_NETC_IPFILTER == 1
static GOAL_STATUS_T goal_drvEthNetcIPFInit(
    GOAL_MA_ETH_T *pMaHdl                       /**< MA Hdl */
);
#endif

static GOAL_STATUS_T goal_drvEthNetcCmdFramePreemptSupportGet(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T goal_drvEthNetcCmdFramePreemptEnable(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T goal_drvEthNetcCmdFramePreemptVerificationStatusGet(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T goal_drvEthNetcCmdFramePreemptAddFragSizeLocalGet(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T goal_drvEthNetcCmdFramePreemptAddFragSizeRemoteSet(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T goal_drvEthNetcCmdFramePreemptStatusGet(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T goal_drvEthNetcCmdFramePreemptTrafficClassConfigAssign(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T goal_drvEthNetcCmdDlrDefaultForwardingSet(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);;


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static netc_msix_entry_t msixEntry[(2 * GOAL_DRV_ETH_TC_MAX) + 2]; /**< MSI-X interrupt config */


/****************************************************************************/
/* Table of supported Ethernet commands */
/****************************************************************************/
/**< Ethernet function table */
#define GOAL_ETH_TARGET_FUNCS                                                                               \
    /*            Ethernet command,                   Function name,              Flags */                  \
    GOAL_ETH_FUNC(GOAL_ETH_CMD_MAC_ADDR,              goal_drvEthNetcCmdMacAddr,  0 ), \
    GOAL_ETH_FUNC(GOAL_ETH_CMD_AUTONEG,               goal_drvEthNetcCmdPhy,      GOAL_ETH_CMD_PORT_EXT  ), \
    GOAL_ETH_FUNC(GOAL_ETH_CMD_AUTONEG_ADVERTISEMENT, goal_drvEthNetcCmdPhy,      GOAL_ETH_CMD_PORT_EXT  ), \
    GOAL_ETH_FUNC(GOAL_ETH_CMD_AUTONEG_PROGRESS,      goal_drvEthNetcCmdPhy,      GOAL_ETH_CMD_PORT_EXT  ), \
    GOAL_ETH_FUNC(GOAL_ETH_CMD_AUTONEG_RESTART,       goal_drvEthNetcCmdPhy,      GOAL_ETH_CMD_PORT_EXT  ), \
    GOAL_ETH_FUNC(GOAL_ETH_CMD_DUPLEX,                goal_drvEthNetcCmdPhy,      GOAL_ETH_CMD_PORT_EXT  ), \
    GOAL_ETH_FUNC(GOAL_ETH_CMD_HW_FAULT,              goal_drvEthNetcCmdPhy,      GOAL_ETH_CMD_PORT_EXT  ), \
    GOAL_ETH_FUNC(GOAL_ETH_CMD_LINK_CAPABILITIES,     goal_drvEthNetcCmdPhy,      GOAL_ETH_CMD_PORT_EXT  ), \
    GOAL_ETH_FUNC(GOAL_ETH_CMD_LINK_STATE,            goal_drvEthNetcCmdPhy,      GOAL_ETH_CMD_PORT_EXT  ), \
    GOAL_ETH_FUNC(GOAL_ETH_CMD_MDI_MODE,              goal_drvEthNetcCmdPhy,      GOAL_ETH_CMD_PORT_EXT  ), \
    GOAL_ETH_FUNC(GOAL_ETH_CMD_MDI_STATE,             goal_drvEthNetcCmdPhy,      GOAL_ETH_CMD_PORT_EXT  ), \
    GOAL_ETH_FUNC(GOAL_ETH_CMD_PHY_REG_GET,           goal_drvEthNetcCmdPhy,      GOAL_ETH_CMD_PORT_EXT  ), \
    GOAL_ETH_FUNC(GOAL_ETH_CMD_PHY_REG_SET,           goal_drvEthNetcCmdPhy,      GOAL_ETH_CMD_PORT_EXT  ), \
    GOAL_ETH_FUNC(GOAL_ETH_CMD_PORT_STATE,            goal_drvEthNetcCmdPhy,      GOAL_ETH_CMD_PORT_EXT  ), \
    GOAL_ETH_FUNC(GOAL_ETH_CMD_SPEED,                 goal_drvEthNetcCmdPhy,      GOAL_ETH_CMD_PORT_EXT  ), \
    GOAL_ETH_FUNC(GOAL_ETH_CMD_SPEED_MAX,             goal_drvEthNetcCmdPhy,      GOAL_ETH_CMD_PORT_EXT  ), \
    GOAL_ETH_FUNC(GOAL_ETH_CMD_PORT_COUNT,            goal_drvEthNetcCmdNumPorts, 0)

/* connect target and board Ethernet functions tables */
#define GOAL_ETH_FUNCS     GOAL_ETH_TARGET_FUNCS

/**< List of Ethernet functions */
static GOAL_ETH_FUNCTION_ENTRY_T targetFunc[] = GOAL_ETH_FUNC_LIST(
    GOAL_ETH_FUNCS
);


#if GOAL_PTP == 1
/****************************************************************************/
/** Convert a hardware timestamp to GOAL timestamp format
 *
 * The provided timestamp only contains the lower 32 bits of the 64-bit value.
 * These lower bits can represent more than a second.
 */
static inline void goal_drvEthNetcTimestampConvert(
    uint32_t tsValLow,                          /**< lower 32 bit of timestamp in ns */
    uint64_t *pSec,                             /**< full seconds of timestamp */
    uint32_t *pNsec                             /**< subseconds of timestamp */
)
{
    static uint64_t now;                        /* current time in ns */

    /* get current time in ns */
    goal_targetPtpGetTime(pSec, pNsec);
    now = (*pSec * GOAL_DRV_ETH_SEC_IN_NSEC) + *pNsec;

    if (tsValLow > (now & 0xffffffff)) {
        /* timestamp was taken in the previous second */
        now -= GOAL_DRV_ETH_SEC_IN_NSEC;
    }

    /* build old timestamp */
    now = (now & 0xffffffff00000000) | tsValLow;

    *pSec = now / GOAL_DRV_ETH_SEC_IN_NSEC;

    *pNsec = (uint32_t) (now - (*pSec * GOAL_DRV_ETH_SEC_IN_NSEC));
}
#endif /* GOAL_PTP */


/****************************************************************************/
/** Get the MAC Port Register Set for a GOAL port
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static inline GOAL_STATUS_T goal_drvEthNetcMacPortRegGet(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    uint32_t port,                              /**< GOAL port */
    NETC_ETH_LINK_Type **ppReg                  /**< MAC Port registers */
)
{
    if (GOAL_TRUE == pDrvData->hasSwitch) {
        if ((port & GOAL_ETH_PORT_MASK) ||
            (port == GOAL_ETH_PORT_HOST) ||
            (port >= pDrvData->numPorts)) {
            return GOAL_ERR_PARAM;
        }

        *ppReg = pDrvData->swHandle.hw.ports[pDrvData->pPortInfo[port].portId - 1].eth;
    }
    else {
        *ppReg = pDrvData->epHandle.hw.portGroup.eth;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Register Ethernet Driver
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_drvEthNetcReg(
    uint32_t idMa,                              /**< id ma */
    GOAL_DRV_ETH_NETC_PORT_INFO_T *pPortInfo,   /**< port info data table */
    uint32_t numEntries,                        /**< entries in port info data table */
    GOAL_DRV_ETH_NETC_BUFDESC_INFO_T *pBufDescInfo /**< buffers and descriptors */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_MA_ETH_T *pMaEthHdl;                   /* ma handle */
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData;         /* private driver data */
    uint32_t cnt;                               /* loop counter */

    if (GOAL_DRV_ETH_MA_ID_MAX <= idMa) {
        goal_logErr("MA ID exceeds supported maximum value (%"FMT_u32" >= %"FMT_u32")", idMa, (uint32_t) GOAL_DRV_ETH_MA_ID_MAX);
        res = GOAL_ERR_PARAM;
    }

    if ((GOAL_DRV_ETH_TC_MAX < pBufDescInfo->numTc) || (0 == pBufDescInfo->numTc)) {
        goal_logErr("unsupported number of traffic classes (%"FMT_u32" != 1 .. %"FMT_u32")", pBufDescInfo->numTc, (uint32_t) GOAL_DRV_ETH_TC_MAX);
        res = GOAL_ERR_PARAM;
    }

    if (GOAL_RES_OK(res)) {
        res = goal_memCalloc(&pDrvData, sizeof(GOAL_DRV_ETH_NETC_DATA_T));
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to allocate driver data");
        }
    }

    if (GOAL_RES_OK(res)) {
        pDrvData->pPortInfo = pPortInfo;
        pDrvData->numPorts = numEntries;
        pDrvData->numTc = pBufDescInfo->numTc;
        for (cnt = 0; cnt < pDrvData->numTc; cnt++) {
            pDrvData->rxBuffDesc[cnt] = pBufDescInfo->rxBuffDesc[cnt];
            pDrvData->txBuffDesc[cnt] = pBufDescInfo->txBuffDesc[cnt];
            pDrvData->rxBuffers[cnt] = pBufDescInfo->rxBuffers[cnt];
        }
        pDrvData->swTxBuffDesc = pBufDescInfo->swTxBuffDesc;
        pDrvData->switchCmdDesc = pBufDescInfo->switchCmdDesc;
        pDrvData->epCmdDesc = pBufDescInfo->epCmdDesc;

        /* register ma */
        res = goal_maEthDrvReg(idMa, &pMaEthHdl,
                               goal_drvEthNetcInit,
                               goal_drvEthNetcOpen,
                               goal_drvEthNetcSend,
                               goal_drvEthNetcCmd,
                               pDrvData);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to register Ethernet MA");
        }
    }

    return res;
}


/****************************************************************************/
/** Target Ethernet initialization
 *
 * This function is called during initialization stage.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_drvEthNetcInit(
    GOAL_MA_ETH_T *pMaHdl                       /**< MA Hdl */
)
{
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData;         /* private driver data */
    uint32_t cnt;                               /* loop counter */
    uint32_t cntEntry;                          /* table entry counter */
    GOAL_STATUS_T res;                          /* result */

    pDrvData = (GOAL_DRV_ETH_NETC_DATA_T *) pMaHdl->pData;

    if (1 == pDrvData->numPorts) {
        pDrvData->hasSwitch = (kNETC_ENETC0EthPort == pDrvData->pPortInfo[0].portId) ? GOAL_FALSE : GOAL_TRUE;
    }
    else {
        /* if there are multiple ports, only switch ports are allowed */
        pDrvData->hasSwitch = GOAL_TRUE;
        for (cnt = 0; cnt < pDrvData->numPorts; cnt++) {
            if (kNETC_ENETC0EthPort == pDrvData->pPortInfo[cnt].portId) {
                goal_logErr("mixture of MAC Controller and Switch ports is not allowed");
                return GOAL_ERROR;
            }
        }
    }

    pDrvData->qosMode.mode = 0;
    pDrvData->qosMode.defprio = 0;

    /* allocate vlan prio mapping buffer */
    pDrvData->vlanPrioMap = NULL;
    res = goal_memCalloc(&pDrvData->vlanPrioMap, sizeof(uint8_t *) * pDrvData->numPorts);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to allocate VLAN prio mapping buffer array");
        return res;
    }

    for (cnt = 0; cnt < pDrvData->numPorts; cnt++) {
        res = goal_memCalloc(&pDrvData->vlanPrioMap[cnt], sizeof(uint8_t) * GOAL_DRV_ETH_VLAN_PRIO_NUM_MAX);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to allocate VLAN prio mapping buffer for port %"FMT_u32, cnt);
            return res;
        }

        for (cntEntry = 0; cntEntry < GOAL_DRV_ETH_VLAN_PRIO_NUM_MAX; cntEntry++) {
            pDrvData->vlanPrioMap[cnt][cntEntry] = GOAL_DRV_ETH_VLAN_PRIO_UNUSED;
        }
    }

    GOAL_MEMSET(pDrvData->etypePrioMap, GOAL_DRV_ETH_ETYPE_PRIO_UNUSED, sizeof(GOAL_ETH_QOS_PRIO_TYPE_T) * GOAL_DRV_ETH_ETYPE_PRIO_NUM_MAX);
#if GOAL_CONFIG_TDMA == 1
    pDrvData->maxValidTslt = 0;
    pDrvData->tdmaEnabled = GOAL_FALSE;
#endif /* GOAL_CONFIG_TDMA */

#if GOAL_DLR == 1
    if (GOAL_TRUE == pDrvData->hasSwitch) {
        res = goal_drvDlrNetcInit(pDrvData);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to initialize DLR driver");
            return res;
        }
    }
#endif /* GOAL_DLR */

    return GOAL_OK;
}


/****************************************************************************/
/** Initialize Hardware and driver
 *
 * If this function succeeds the device will be able to send and receive
 * Ethernet frames.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_drvEthNetcOpen(
    GOAL_MA_ETH_T *pMaHdl                       /**< MA Hdl */
)
{
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData;         /* private driver data */
    GOAL_MI_ETH_T *pMiEthHdl;                   /* ETH MI handle */
    GOAL_STATUS_T res = GOAL_OK;                /* GOAL result */
    uint32_t cntRing;                           /* ring loop counter */
    uint32_t cntElem;                           /* element loop counter */
    status_t resSdk;                            /* SDK result */
    ep_config_t epConfig;                       /* EP config data */
    uint32_t msgAddrRx = 0;                     /* message interrupt address for Rx interrupts */
    uint32_t msgAddrTx = 0;                     /* message interrupt address for Tx interrupts */
    uint32_t bitOffset;                         /* bit offset for MSIX signals between instances */
    netc_bdr_config_t bdrConfig;                /* Descriptor configuration */
    uint32_t cntPrio;                           /* priority loop counter */
    uint32_t portIdx;                           /* port array index */
    netc_si_l2mf_config_t l2Filter;             /* Layer 2 filter settings */
#if GOAL_CONFIG_DRV_ETH_NETC_IPFILTER == 1
    netc_ep_ipf_config_t  ipFilter;             /* IP filter settings */
#endif
#if GOAL_DLR == 1
    netc_port_ipf_config_t swIpfCfg;            /* switch port IP filter settings */
#endif /* GOAL_DLR */

    pDrvData = (GOAL_DRV_ETH_NETC_DATA_T *) pMaHdl->pData;
    pMiEthHdl = (GOAL_MI_ETH_T *) pMaHdl->pMiInst;

    /* initialize board specific Ethernet handling */
    goal_targetBoardEthInit(pDrvData->pPortInfo, pDrvData->numPorts);

#if GOAL_CONFIG_TASK == 1
    /* create Tx Status lock as a binary counting semaphore */
    if (GOAL_RES_OK(res)) {
        pDrvData->pTxStatusLock = NULL;
        res = goal_lockCreate(GOAL_LOCK_COUNT, &(pDrvData->pTxStatusLock), 1, 1, GOAL_ID_TGT);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to create Tx Status lock");
        }
    }
#endif /* GOAL_CONFIG_TASK */

    /* allocate Tx frame info array */
    if (GOAL_RES_OK(res)) {
        res = goal_memCallocAlign(&pDrvData->txFrameInfo, sizeof(netc_tx_frame_info_t *) * pDrvData->numTc, GOAL_TARGET_MEM_ALIGN_CPU);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to allocate Tx frame info ring array");
        }
    }

    for (cntRing = 0; cntRing < pDrvData->numTc && GOAL_RES_OK(res); cntRing++) {
        res = goal_memCallocAlign(&(pDrvData->txFrameInfo[cntRing]), sizeof(netc_tx_frame_info_t) * GOAL_DRV_ETH_DESC_NUM, GOAL_TARGET_MEM_ALIGN_CPU);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to allocate Tx frame info array for ring %"FMT_u32, cntRing);
        }
    }

    /* allocate Rx buffer address array */
    if (GOAL_RES_OK(res)) {
        res = goal_memCallocAlign(&pDrvData->rxBuffAddrArray, sizeof(uint64_t *) * pDrvData->numTc, GOAL_TARGET_MEM_ALIGN_CPU);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to allocate Rx Buffer address array");
        }
    }

    for (cntRing = 0; cntRing < pDrvData->numTc && GOAL_RES_OK(res); cntRing++) {
        res = goal_memCallocAlign(&(pDrvData->rxBuffAddrArray[cntRing]), sizeof(uint64_t) * GOAL_DRV_ETH_BUF_NUM, GOAL_TARGET_MEM_ALIGN_CPU);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to allocate Rx Buffer address array for ring %"FMT_u32, cntRing);
        }
    }

    /* assign Rx buffer addresses to address array */
    for (cntRing = 0; cntRing < pDrvData->numTc && GOAL_RES_OK(res); cntRing++) {
        for (cntElem = 0; cntElem < GOAL_DRV_ETH_BUF_NUM; cntElem++) {
            pDrvData->rxBuffAddrArray[cntRing][cntElem] = (uint64_t) (PtrCast) &(pDrvData->rxBuffers[cntRing][cntElem]);
        }
    }

    /* allocate Rx descriptor config array */
    if (GOAL_RES_OK(res)) {
        res = goal_memCallocAlign(&pDrvData->rxBdrConfig, sizeof(netc_rx_bdr_config_t) * pDrvData->numTc, GOAL_TARGET_MEM_ALIGN_CPU);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to allocate Rx Buffer descriptor config array");
        }
    }

    /* allocate Tx descriptor config array */
    if (GOAL_RES_OK(res)) {
        res = goal_memCallocAlign(&pDrvData->txBdrConfig, sizeof(netc_tx_bdr_config_t) * pDrvData->numTc, GOAL_TARGET_MEM_ALIGN_CPU);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to allocate Tx Buffer descriptor config array");
        }
    }

    if (GOAL_RES_OK(res) && (GOAL_TRUE == pDrvData->hasSwitch)) {
        /* allocate switch Tx frame info array */
        if (GOAL_RES_OK(res)) {
            res = goal_memCallocAlign(&pDrvData->swTxFrameInfo, sizeof(netc_tx_frame_info_t) * GOAL_DRV_ETH_DESC_NUM, GOAL_TARGET_MEM_ALIGN_CPU);
            if (GOAL_RES_ERR(res)) {
                goal_logErr("failed to allocate Switch Tx info array");
            }
        }
    }

    /* allocate Phy bus handle array */
    if (GOAL_RES_OK(res)) {
        res = goal_memCallocAlign(&pDrvData->pIfacePhys, sizeof(GOAL_ETH_IFACE_T) * pDrvData->numPorts, GOAL_TARGET_MEM_ALIGN_CPU);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to allocate Phy bus handle array");
        }
    }

#if GOAL_PTP == 1
    if (GOAL_RES_OK(res)) {
        /* prepare Tx Timstamp buffers */
        for (cntElem = 0; cntElem < GOAL_DRV_ETH_TXTS_NUM; cntElem++) {
            pDrvData->txTsStore[cntElem].txPort = UINT32_MAX;
        }
    }
#endif /* GOAL_PTP */

    if (GOAL_RES_OK(res)) {
        bitOffset = pMaHdl->id * (GOAL_DRV_ETH_TC_MAX + 1);
        msgAddrRx = MSGINTR_GetIntrSelectAddr(MSGINTR1, GOAL_DRV_ETH_RX_INTR_CHAN);
        msgAddrTx = MSGINTR_GetIntrSelectAddr(MSGINTR1, GOAL_DRV_ETH_TX_INTR_CHAN);

        /* MSIX and interrupt configuration. */
        if (GOAL_RES_OK(res)) {
            resSdk = MSGINTR_Init(MSGINTR1, &goal_drvEthNetcCbExec);
            if (kStatus_Success != resSdk) {
                goal_logErr("failed to register IRQ handler");
                res = GOAL_ERROR;
            }
        }
    }

    for (cntRing = 0, cntElem = 0; cntRing < pDrvData->numTc && GOAL_RES_OK(res); cntRing++) {
        /* prepare MSI-X vector table */
        msixEntry[GOAL_DRV_ETH_TX_MSIX_ENTRY_IDX + cntElem].control = kNETC_MsixIntrMaskBit;
        msixEntry[GOAL_DRV_ETH_TX_MSIX_ENTRY_IDX + cntElem].msgAddr = msgAddrTx;
        msixEntry[GOAL_DRV_ETH_TX_MSIX_ENTRY_IDX + cntElem].msgData = cntRing + bitOffset;
        msixEntry[GOAL_DRV_ETH_RX_MSIX_ENTRY_IDX + cntElem].control = kNETC_MsixIntrMaskBit;
        msixEntry[GOAL_DRV_ETH_RX_MSIX_ENTRY_IDX + cntElem].msgAddr = msgAddrRx;
        msixEntry[GOAL_DRV_ETH_RX_MSIX_ENTRY_IDX + cntElem].msgData = cntRing + bitOffset;

        /* RX Descriptor config */
        pDrvData->rxBdrConfig[cntRing].bdArray = &pDrvData->rxBuffDesc[cntRing][0];
        pDrvData->rxBdrConfig[cntRing].len = GOAL_DRV_ETH_DESC_NUM;
        pDrvData->rxBdrConfig[cntRing].extendDescEn = true;
        pDrvData->rxBdrConfig[cntRing].buffAddrArray = &pDrvData->rxBuffAddrArray[cntRing][0];
        pDrvData->rxBdrConfig[cntRing].buffSize = GOAL_DRV_ETH_BUF_SIZE;
        pDrvData->rxBdrConfig[cntRing].enThresIntr = true;
        pDrvData->rxBdrConfig[cntRing].enCoalIntr = true;
        pDrvData->rxBdrConfig[cntRing].intrThreshold = 1;
        pDrvData->rxBdrConfig[cntRing].msixEntryIdx = GOAL_DRV_ETH_RX_MSIX_ENTRY_IDX + cntElem;

        /* TX Descriptor config */
        pDrvData->txBdrConfig[cntRing].bdArray = &pDrvData->txBuffDesc[cntRing][0];
        pDrvData->txBdrConfig[cntRing].priority = cntRing;
        pDrvData->txBdrConfig[cntRing].len = GOAL_DRV_ETH_DESC_NUM;
        pDrvData->txBdrConfig[cntRing].dirtyArray = &pDrvData->txFrameInfo[cntRing][0];
        pDrvData->txBdrConfig[cntRing].msixEntryIdx = GOAL_DRV_ETH_TX_MSIX_ENTRY_IDX + cntElem;
        pDrvData->txBdrConfig[cntRing].enIntr = true;
        pDrvData->txBdrConfig[cntRing].enCoalIntr = true;
        pDrvData->txBdrConfig[cntRing].intrThreshold = 1;

        cntElem += 2;
    }

    if (GOAL_RES_OK(res) && GOAL_TRUE == pDrvData->hasSwitch) {
        /* prepare interrupt entries for switch management frames */
        msixEntry[GOAL_DRV_ETH_TX_MSIX_ENTRY_IDX + cntElem].control = kNETC_MsixIntrMaskBit;
        msixEntry[GOAL_DRV_ETH_TX_MSIX_ENTRY_IDX + cntElem].msgAddr = msgAddrTx;
        msixEntry[GOAL_DRV_ETH_TX_MSIX_ENTRY_IDX + cntElem].msgData = cntRing + bitOffset;
        msixEntry[GOAL_DRV_ETH_RX_MSIX_ENTRY_IDX + cntElem].control = kNETC_MsixIntrMaskBit;
        msixEntry[GOAL_DRV_ETH_RX_MSIX_ENTRY_IDX + cntElem].msgAddr = msgAddrRx;
        msixEntry[GOAL_DRV_ETH_RX_MSIX_ENTRY_IDX + cntElem].msgData = cntRing + bitOffset;
    }

    if (GOAL_RES_OK(res)) {
        /* Endpoint configuration. */
        EP_GetDefaultConfig(&epConfig);
        epConfig.si = (GOAL_TRUE == pDrvData->hasSwitch) ? kNETC_ENETC1PSI0 : kNETC_ENETC0PSI0;
        epConfig.siConfig.txRingUse = pDrvData->numTc;
        epConfig.siConfig.rxRingUse = pDrvData->numTc;
#if GOAL_DLR == 1
        if (GOAL_TRUE == pDrvData->hasSwitch) {
            /* enable mapping of IPV to RxBDR number, ignore VLAN tags */
            epConfig.siConfig.rxBdrGroupNum = 1;
            epConfig.siConfig.valnToIpvEnable = 0;
            epConfig.siConfig.ringPerBdrGroup = pDrvData->numTc;
            for (cntRing = 0; cntRing < pDrvData->numTc; cntRing++) {
                epConfig.siConfig.ipvToRingMap[cntRing] = (uint8_t) cntRing;
            }
        }
#endif /* GOAL_DLR */
        epConfig.reclaimCallback = goal_drvEthNetcTxDoneCbExec;
        epConfig.msixEntry = &msixEntry[0];
        epConfig.entryNum = 2 * pDrvData->numTc;
        if (GOAL_TRUE == pDrvData->hasSwitch) {
            epConfig.entryNum += 1;
        }
        epConfig.userData = pMaHdl;
        if (GOAL_FALSE == pDrvData->hasSwitch) {
            epConfig.port.ethMac.miiMode = pDrvData->pPortInfo[0].miiType;
            epConfig.port.ethMac.miiSpeed = (GOAL_ETH_SPEED_1000 == pDrvData->pPortInfo[0].maxSpeed) ? kNETC_MiiSpeed1000M : kNETC_MiiSpeed100M;
            epConfig.port.ethMac.miiDuplex = kNETC_MiiFullDuplex;
        }

        bdrConfig.rxBdrConfig = pDrvData->rxBdrConfig;
        bdrConfig.txBdrConfig = pDrvData->txBdrConfig;

        /* allow sending of vlan tagged frames */
        epConfig.siConfig.enSIBaseVlan = true;
        epConfig.siConfig.vlanCtrl = kNETC_ENETC_StanCVlan;
        epConfig.siConfig.vlanInsertEnable = false;

        /* Endpoint command buffer ring */
        epConfig.cmdBdrConfig.bdBase = pDrvData->epCmdDesc;
        epConfig.cmdBdrConfig.bdLength = GOAL_DRV_ETH_DESC_NUM;
        epConfig.cmdBdrConfig.enCompInt = false;

        /* enable l2/l3/l4 payload parser */
        epConfig.port.common.parser.l2PloadCount = GOAL_DRV_ETH_L2_PARSER_PL_MAX;
        epConfig.port.common.parser.l3PayloadCount = 0;
        epConfig.port.common.parser.enableL3Parser = false;
        epConfig.port.common.parser.l4PayloadCount = 0;
        epConfig.port.common.parser.enableL4Parser = false;

#ifdef RIOP
        /* Enable Tx buffer cache maintain in driver */
        epConfig.txCacheMaintain = true;
#endif

        /* apply endpoint settings */
        resSdk = EP_Init(&pDrvData->epHandle, pMaHdl->cfg.macAddr, &epConfig, &bdrConfig);
        if (kStatus_Success != resSdk) {
            goal_logErr("failed to initialize endpoint");
            res = GOAL_ERROR;
        }
    }

    if (GOAL_RES_OK(res)) {
        l2Filter.macUCPromis = false;
#if GOAL_CONFIG_DRV_ETH_NETC_IPFILTER == 1
        /* the filter requires all multicast to reach the filter in the first place */
        l2Filter.macMCPromis = true;
#else
        l2Filter.macMCPromis = false;
#endif
        l2Filter.rejectUC = false;
        l2Filter.rejectMC = false;
        l2Filter.rejectBC = false;

        /* when switch is used, multicast traffic needs to be passed by the MAC */
        if (GOAL_TRUE == pDrvData->hasSwitch) {
            l2Filter.macMCPromis = true;
        }

        resSdk = EP_RxL2MFInit(&pDrvData->epHandle, &l2Filter);
        if (kStatus_Success != resSdk) {
            goal_logErr("failed to apply L2 filters");
            res = GOAL_ERROR;
        }
    }

#if GOAL_CONFIG_DRV_ETH_NETC_IPFILTER == 1
    /* configure ingress filtering to filter unwanted traffic from host port */
    if (GOAL_RES_OK(res)) {
        ipFilter.dosCfg.l2DiscardMCSmac = true;
        ipFilter.dosCfg.l3DiscardSipEquDip = true;
        ipFilter.dosCfg.l2DiscardMCSmac = true;
        ipFilter.portConfig.enL2Dos = true;
        ipFilter.portConfig.enL3Dos = true;
        ipFilter.portConfig.enIPFTable = true;
        resSdk = EP_RxIPFInit(&pDrvData->epHandle, &ipFilter);
        if (kStatus_Success != resSdk) {
            res = GOAL_ERROR;
        }
    }

    if (GOAL_RES_OK(res)) {
       res = goal_drvEthNetcIPFInit(pMaHdl);
    }
#endif

    if (GOAL_RES_OK(res) && (GOAL_TRUE == pDrvData->hasSwitch)) {
        /* initialize switch */
        SWT_GetDefaultConfig(&pDrvData->swConfig);

        /* disable all ports */
        for (cntElem = 0; cntElem < GOAL_DRV_ETH_SWITCH_PORTS_NUM; cntElem++) {
            pDrvData->swConfig.ports[cntElem].enTxRx = false;
        }

        /* enable ports specified by board driver */
        for (cntElem = 0; cntElem < pDrvData->numPorts; cntElem++) {
            /* the config data only contains external switch ports */
            portIdx = ((uint32_t) pDrvData->pPortInfo[cntElem].portId) - 1;
            pDrvData->swConfig.ports[portIdx].enTxRx = true;
#if GOAL_CONFIG_EPL == 1
            /* enable cut through on switch ports */
            pDrvData->swConfig.ports[portIdx].inCutThrough = true;
            pDrvData->swConfig.ports[portIdx].outCutThrough = true;
#endif
            pDrvData->swConfig.ports[portIdx].commonCfg.qosMode.vlanQosMap = 0;
            pDrvData->swConfig.ports[portIdx].commonCfg.qosMode.defaultIpv = 0;
            pDrvData->swConfig.ports[portIdx].commonCfg.qosMode.defaultDr = 2;
            pDrvData->swConfig.ports[portIdx].commonCfg.qosMode.enVlanInfo = true;
            pDrvData->swConfig.ports[portIdx].commonCfg.qosMode.vlanTagSelect = true;
            pDrvData->swConfig.ports[portIdx].commonCfg.ipfCfg.enIPFTable = true;
            pDrvData->swConfig.ports[portIdx].bridgeCfg.isRxVlanAware = false;
            pDrvData->swConfig.ports[portIdx].bridgeCfg.enMacStationMove = true;

            /* map each priority to a dedicated buffer pool */
            for (cntPrio = 0, cntRing = 0; cntPrio < GOAL_DRV_ETH_PRIO_NUM; cntPrio++) {
                pDrvData->swConfig.ports[portIdx].ipvToBP[cntPrio] = cntRing;
                if ((pDrvData->numTc - 1) > cntRing) {
                    cntRing++;
                }
            }

            pDrvData->swConfig.ports[portIdx].ethMac.miiMode = pDrvData->pPortInfo[cntElem].miiType;;
            pDrvData->swConfig.ports[portIdx].ethMac.miiSpeed = (GOAL_ETH_SPEED_1000 == pDrvData->pPortInfo[cntElem].maxSpeed) ? kNETC_MiiSpeed1000M : kNETC_MiiSpeed100M;;
            pDrvData->swConfig.ports[portIdx].ethMac.miiDuplex = kNETC_MiiFullDuplex;
        }

        /* enable host board */
        pDrvData->swConfig.ports[GOAL_DRV_ETH_SWITCH_HOST_PORT_ID].enTxRx = true;
        pDrvData->swConfig.ports[GOAL_DRV_ETH_SWITCH_HOST_PORT_ID].commonCfg.qosMode.vlanQosMap = 0;
        pDrvData->swConfig.ports[GOAL_DRV_ETH_SWITCH_HOST_PORT_ID].commonCfg.qosMode.defaultIpv = 0;
        pDrvData->swConfig.ports[GOAL_DRV_ETH_SWITCH_HOST_PORT_ID].commonCfg.qosMode.defaultDr = 2;
        pDrvData->swConfig.ports[GOAL_DRV_ETH_SWITCH_HOST_PORT_ID].commonCfg.qosMode.enVlanInfo = true;
        pDrvData->swConfig.ports[GOAL_DRV_ETH_SWITCH_HOST_PORT_ID].commonCfg.qosMode.vlanTagSelect = true;
        pDrvData->swConfig.ports[GOAL_DRV_ETH_SWITCH_HOST_PORT_ID].bridgeCfg.isRxVlanAware = false;
        pDrvData->swConfig.ports[GOAL_DRV_ETH_SWITCH_HOST_PORT_ID].bridgeCfg.enMacStationMove = true;

        /* map each priority to a dedicated buffer pool */
        for (cntPrio = 0, cntRing = 0; cntPrio < GOAL_DRV_ETH_PRIO_NUM; cntPrio++) {
            pDrvData->swConfig.ports[GOAL_DRV_ETH_SWITCH_HOST_PORT_ID].ipvToBP[cntPrio] = cntRing;
            if ((pDrvData->numTc - 1) > cntRing) {
                cntRing++;
            }
        }

        /* Switch table command buffer ring */
        pDrvData->swConfig.cmdRingUse = 1;
        pDrvData->swConfig.cmdBdrCfg[0].bdBase = pDrvData->switchCmdDesc;
        pDrvData->swConfig.cmdBdrCfg[0].bdLength = GOAL_DRV_ETH_BUF_NUM;

        /* enable l2/l3/l4 payload parser and enable isi first step for ports */
        for (cntElem = 0; cntElem < pDrvData->numPorts; cntElem++) {
            portIdx = ((uint32_t) pDrvData->pPortInfo[cntElem].portId) - 1;
            /* enable l2/l3/l4 payload parser */
            pDrvData->swConfig.ports[portIdx].commonCfg.parser.l2PloadCount = GOAL_DRV_ETH_L2_PARSER_PL_MAX;
            pDrvData->swConfig.ports[portIdx].commonCfg.parser.l3PayloadCount = 0;
            pDrvData->swConfig.ports[portIdx].commonCfg.parser.enableL3Parser = false;
            pDrvData->swConfig.ports[portIdx].commonCfg.parser.l4PayloadCount = 0;
            pDrvData->swConfig.ports[portIdx].commonCfg.parser.enableL4Parser = false;
        }

        /* apply switch settings */
        resSdk = SWT_Init(&pDrvData->swHandle, &pDrvData->swConfig);
        if (kStatus_Success != resSdk) {
            goal_logErr("failed to initialize switch");
            res = GOAL_ERROR;
        }

        if (GOAL_RES_OK(res)) {
            /* prepare switch transfer settings */
            pDrvData->swRxTxConfig.rxZeroCopy = false;
            pDrvData->swRxTxConfig.reclaimCallback = goal_drvEthNetcSwitchTxDoneCbExec;
            pDrvData->swRxTxConfig.userData = pMaHdl;
            pDrvData->swRxTxConfig.enUseMgmtRxBdRing = false;

            pDrvData->swRxTxConfig.enUseMgmtTxBdRing = true;
            pDrvData->swRxTxConfig.mgmtTxBdrConfig.bdArray = &pDrvData->swTxBuffDesc[0],
            pDrvData->swRxTxConfig.mgmtTxBdrConfig.priority = pDrvData->numTc - 1;
            pDrvData->swRxTxConfig.mgmtTxBdrConfig.len = GOAL_DRV_ETH_DESC_NUM;
            pDrvData->swRxTxConfig.mgmtTxBdrConfig.dirtyArray = &pDrvData->swTxFrameInfo[0];
            pDrvData->swRxTxConfig.mgmtTxBdrConfig.msixEntryIdx = 2 * pDrvData->numTc;
            pDrvData->swRxTxConfig.mgmtTxBdrConfig.enIntr = true;

            /* apply switch transfer settings */
            resSdk = SWT_ManagementTxRxConfig(&pDrvData->swHandle, &pDrvData->epHandle, &pDrvData->swRxTxConfig);
            if (kStatus_Success != resSdk) {
                goal_logErr("failed to apply switch transfer settings");
                res = GOAL_ERROR;
            }
        }

#if GOAL_DLR == 1
        if (GOAL_RES_OK(res)) {
            swIpfCfg.enL2Dos = 1;
            swIpfCfg.enL3Dos = 1;
            swIpfCfg.enIPFTable = 1;
            for (cntElem = 0; cntElem < pDrvData->numPorts; cntElem++) {
                portIdx = ((uint32_t) pDrvData->pPortInfo[cntElem].portId) - 1;
                SWT_RxIPFConfigPort(&pDrvData->swHandle, (netc_hw_port_idx_t) portIdx, &swIpfCfg);
            }
        }
#endif /* GOAL_DLR */
    }

    /* allocate pool buffer */
    if (GOAL_RES_OK(res)) {
        res = goal_queuePoolBufsReq(GOAL_ID, GOAL_NETBUF_SIZE, 1, 0);
    }

    /* get a receive buffer */
    if (GOAL_RES_OK(res)) {
        pDrvData->pRxBuf = NULL;
        res = goal_queuePoolGetBuf(&(pDrvData->pRxBuf), GOAL_NETBUF_SIZE, GOAL_ID_ETH_RECV);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to get Rx buffer");
        }
    }

    /* register featured table */
    if (GOAL_RES_OK(res)) {
        res = goal_miEthTableReg(pMiEthHdl, targetFunc);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to register command table");
        }
    }

    for (cntElem = 0; cntElem < pDrvData->numPorts && GOAL_RES_OK(res); cntElem++) {
        /* register PHY cb handler for each GOAL port */
        res = goal_miEthPortStateArgCbReg(pMiEthHdl, cntElem, goal_drvEthNetcPhyStateCbExec, pDrvData);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to register Phy cb handler for port %"FMT_u32, cntElem);
        }
    }

    if (GOAL_RES_OK(res)) {
        if (pDrvData->hasSwitch) {
            res = goal_targetBoardEthPostInit(pMaHdl, pDrvData->pPortInfo, pDrvData->numPorts, &pDrvData->ifaceSwitch, pDrvData->pIfacePhys, &pDrvData->swHandle);
        }
        else {
            res = goal_targetBoardEthPostInit(pMaHdl, pDrvData->pPortInfo, pDrvData->numPorts, NULL, pDrvData->pIfacePhys, NULL);
        }
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to register switch and phy command handlers");
        }
    }

    if (GOAL_RES_OK(res)) {
        /* unmask MSIX message interrupt */
        for (cntElem = 0; cntElem < (2 * pDrvData->numTc); cntElem++) {
            EP_MsixSetEntryMask(&pDrvData->epHandle, cntElem, false);
        }
        if (GOAL_TRUE == pDrvData->hasSwitch) {
            EP_MsixSetEntryMask(&pDrvData->epHandle, cntElem, false);
        }
    }

#if GOAL_DLR == 1
    if (GOAL_RES_OK(res) && (GOAL_TRUE == pDrvData->hasSwitch)) {
        res = goal_drvDlrNetcOpen(pDrvData);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to open DLR NETC driver");
        }
    }
#endif /* GOAL_DLR */

#if GOAL_CONFIG_TASK == 1
    if (GOAL_RES_OK(res)) {
        /* create task for Ethernet send finish handling */
        pDrvData->pTskEthSend = NULL;
        GOAL_SNPRINTF(pDrvData->txTaskName, GOAL_DRV_ETH_TASK_NAME_MAX, "Eth TX %"FMT_u32, pMaHdl->id);
        res = goal_taskCreate(&pDrvData->pTskEthSend,
                              pDrvData->txTaskName,
                              goal_drvEthNetcTxTask,
                              pMaHdl,
                              GOAL_DRV_ETH_TASK_PRIO_TX,
                              GOAL_DRV_ETH_TASK_STACK_SIZE,
                              GOAL_TRUE);
    }

    if (GOAL_RES_OK(res)) {
        /* create task for Ethernet receive handling */
        pDrvData->pTskEthRecv = NULL;
        GOAL_SNPRINTF(pDrvData->rxTaskName, GOAL_DRV_ETH_TASK_NAME_MAX, "Eth RX %"FMT_u32, pMaHdl->id);
        res = goal_taskCreate(&pDrvData->pTskEthRecv,
                              pDrvData->rxTaskName,
                              goal_drvEthNetcRxTask,
                              pMaHdl,
                              GOAL_DRV_ETH_TASK_PRIO_RX,
                              GOAL_DRV_ETH_TASK_STACK_SIZE,
                              GOAL_TRUE);
    }

    if (GOAL_RES_OK(res)) {
        /* create Rx IRQ Mailbox */
        pDrvData->pMbxRxIrq = NULL;
        res = goal_mboxLockFreeCreate(&pDrvData->pMbxRxIrq, GOAL_DRV_ETH_MBOX_NUM, GOAL_DRV_ETH_MBOX_SIZE);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to create Rx IRQ Mailbox");
        }
    }
#endif

    if (GOAL_RES_ERR(res)) {
        goal_drvEthNetcShutdown(pMaHdl);
    }

    SDK_DelayAtLeastUs(1000000, SystemCoreClock);

    return res;
}


/****************************************************************************/
/** Shut down Hardware and driver
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_drvEthNetcShutdown(
    GOAL_MA_ETH_T *pMaHdl                       /**< MA Hdl */
)
{
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData;         /* private driver data */
    uint32_t cntRing;                           /* ring loop counter */
    uint32_t cntElem;                           /* element loop counter */

    if ((NULL == pMaHdl) || (NULL == pMaHdl->pData)) {
        return GOAL_ERR_NULL_POINTER;
    }

    pDrvData = (GOAL_DRV_ETH_NETC_DATA_T *) pMaHdl->pData;

    for (cntElem = 0; cntElem < (2 * pDrvData->numTc); cntElem++) {
        EP_MsixSetEntryMask(&pDrvData->epHandle, cntElem, true);
    }
    if (GOAL_TRUE == pDrvData->hasSwitch) {
        EP_MsixSetEntryMask(&pDrvData->epHandle, cntElem, true);
        SWT_Deinit(&pDrvData->swHandle);
    }

    EP_Deinit(&pDrvData->epHandle);
    MSGINTR_Deinit(MSGINTR1);

    if (GOAL_TRUE == pDrvData->hasSwitch) {
        goal_memFree(&pDrvData->swTxFrameInfo);
    }

    goal_memFree(&pDrvData->txBdrConfig);
    goal_memFree(&pDrvData->rxBdrConfig);

    for (cntRing = 0; cntRing < pDrvData->numTc; cntRing++) {
        goal_memFree(&pDrvData->rxBuffAddrArray[cntRing]);
    }
    goal_memFree(&pDrvData->rxBuffAddrArray);

    for (cntRing = 0; cntRing < pDrvData->numTc; cntRing++) {
        goal_memFree(&pDrvData->txFrameInfo[cntRing]);
    }
    goal_memFree(&pDrvData->txFrameInfo);

    goal_memFree(&pDrvData->pIfacePhys);

    goal_queueReleaseBuf(&(pDrvData->pRxBuf));

    for (cntElem = 0; cntElem < pDrvData->numPorts; cntElem++) {
        goal_memFree(&pDrvData->vlanPrioMap[cntElem]);
    }
    goal_memFree(&pDrvData->vlanPrioMap);
#if GOAL_CONFIG_TASK == 1
    goal_mboxDestroy(&pDrvData->pMbxRxIrq);
#endif
    goal_memFree(&pDrvData);

    return GOAL_OK;
}


#if GOAL_CONFIG_TASK == 1
/****************************************************************************/
/** Ethernet receive task
 *
 * This task is woken up by the RTOS when an Ethernet frame was received.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_drvEthNetcRxTask(
    void *pArg                                  /**< task argument */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MA_ETH_T *pMaEthHdl;                   /* eth ma handle */
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData;         /* private driver data */

    pMaEthHdl = (GOAL_MA_ETH_T *) pArg;
    pDrvData = (GOAL_DRV_ETH_NETC_DATA_T *) pMaEthHdl->pData;

    /* loop until GOAL shutdown */
    while (!GOAL_flagShutdown) {
        /* suspend task until it is resumed by ISR */
        res = goal_taskSuspend(pDrvData->pTskEthRecv);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("Error suspending ETH receive task");
            continue;
        }
        goal_drvEthNetcRxHandler(pMaEthHdl, pDrvData);
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Ethernet send task
 *
 * This task is woken up by the RTOS when an Ethernet frame shall be transmitted.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_drvEthNetcTxTask(
    void *pArg                                  /**< task argument */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MA_ETH_T *pMaEthHdl;                   /* eth ma handle */
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData;         /* private driver data */

    pMaEthHdl = (GOAL_MA_ETH_T *) pArg;
    pDrvData = (GOAL_DRV_ETH_NETC_DATA_T *) pMaEthHdl->pData;

    /* loop until GOAL shutdown */
    while (!GOAL_flagShutdown) {
        /* suspend task until it is resumed by ISR */
        res = goal_taskSuspend(pDrvData->pTskEthSend);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("Error suspending ETH receive task");
            continue;
        }
        goal_drvEthNetcTxHandler(pMaEthHdl, pDrvData);
    }

    return GOAL_OK;
}
#endif /* GOAL_CONFIG_TASK */


/****************************************************************************/
/** NETC interrupt handler
 *
 * This handler processes the MSI messages from the NETC module.
 */
static void goal_drvEthNetcCbExec(
    MSGINTR_Type *pBase,                        /**< MSI controller base */
    uint8_t channel,                            /**< channel ID */
    uint32_t pendingIntr                        /**< interrupt message data */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MA_ETH_T *pMaEthHdl;                   /* eth ma handle */
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData;         /* private driver data */
    uint8_t ringId;                             /* ring that caused interrupt */
    uint32_t maId;                              /* ID of Eth MA */
    uint32_t clearBits;                         /* status bits to clear */
#if GOAL_CONFIG_TASK == 1
    bool portMasquerading = false;              /* switch tx mode */
#endif /* GOAL_CONFIG_TASK */

    UNUSEDARG(pBase);

#if GOAL_CONFIG_TGT_RT1180_TRACE == 1
# if GOAL_CONFIG_TASK == 0
    goal_tgtTaskTrace(1, 3);
# endif
#endif

    while (0 != pendingIntr) {
        res = goal_utilFindFirstSetU32(pendingIntr, &ringId);
        if (GOAL_RES_ERR(res)) {
            goal_logDbg("failed to get ring ID");
            return;
        }

        pendingIntr &= ~(1 << ringId);

        /* get MA ID from bit position and calculate Ring ID from Ma ID */
        maId = ringId / (GOAL_DRV_ETH_TC_MAX + 1);
        ringId -= (GOAL_DRV_ETH_TC_MAX + 1) * maId;

        res = goal_maEthGetById(&pMaEthHdl, maId);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to get ETH MA handle for ID %"FMT_u32, maId);
            return;
        }

        pDrvData = (GOAL_DRV_ETH_NETC_DATA_T *) pMaEthHdl->pData;

        if (GOAL_DRV_ETH_TX_INTR_CHAN == channel) {
            /* Transmit interrupt */
            if (GOAL_TRUE == pDrvData->hasSwitch) {
                if (GOAL_DRV_ETH_TC_MAX == ringId) {
                    /* used Switch management Tx descriptor ring and HW Ring 0 */
                    clearBits = (1 << 0);
#if GOAL_CONFIG_TASK == 1
                    portMasquerading = false;
                    ringId = 0;
#endif /* GOAL_CONFIG_TASK */
                }
                else {
                    /* used EP Tx Descriptor Ring and HW Ring (ring + 1) */
#if GOAL_CONFIG_TASK == 1
                    portMasquerading = true;
#endif /* GOAL_CONFIG_TASK */
                    clearBits = 1 << (ringId + 1);
                }
            }
            else {
                clearBits = (1 << ringId);
            }

            EP_CleanTxIntrFlags(&pDrvData->epHandle, clearBits, clearBits);

#if GOAL_CONFIG_TASK == 1
            if (GOAL_TRUE == pDrvData->hasSwitch) {
                SWT_ReclaimTxDescriptor(&pDrvData->swHandle, portMasquerading, ringId);
            }
            else {
                EP_ReclaimTxDescriptor(&pDrvData->epHandle, ringId);
            }

            goal_lockPut(pDrvData->pTxStatusLock);
#endif /* GOAL_CONFIG_TASK */
        }
        else if (GOAL_DRV_ETH_RX_INTR_CHAN == channel) {
            /* Receive interrupt */
            EP_CleanRxIntrFlags(&pDrvData->epHandle, 1 << ringId);
#if GOAL_DLR == 1
            if ((GOAL_TRUE == pDrvData->hasSwitch) && (GOAL_DRV_ETH_RXBDR_NUM_DEFAULT != ringId)) {
                goal_drvDlrNetcRxIrqExec(pDrvData);
            }
#endif /* GOAL_DLR */
#if GOAL_CONFIG_TASK == 1
            goal_mboxMsgPut(pDrvData->pMbxRxIrq, &ringId);
            goal_taskResume(pDrvData->pTskEthRecv);
#else /* GOAL_CONFIG_TASK */
            pDrvData->rxRing = ringId;
            goal_drvEthNetcRxHandler(pMaEthHdl, pDrvData);
#endif /* GOAL_CONFIG_TASK */
        }
    }

#if GOAL_CONFIG_TGT_RT1180_TRACE == 1
# if GOAL_CONFIG_TASK == 0
    goal_tgtTaskTrace(0, 3);
# endif
#endif
}


/****************************************************************************/
/** Ethernet frame Reception handler
 */
static void goal_drvEthNetcRxHandler(
    GOAL_MA_ETH_T *pMaEthHdl,                   /**< MA Hdl */
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData          /**< private driver data */
)
{
    status_t resSdk;                            /* SDK result */
    uint32_t frameLen;                          /* frame length */
    netc_frame_attr_t attr;                     /* frame attributes */
    uint32_t cntPort;                           /* port loop counter */
    uint32_t rxLoopCntr = GOAL_DRV_ETH_RX_LOOP_CNT; /* maximum number of loop iterations */
    uint8_t rxRing = 0;                         /* Rx BDR Id */
#if GOAL_PTP == 1
    swt_tsr_resp_t txTsData;                    /* Tx timestamp data */
#endif /* GOAL_PTP */

#if GOAL_CONFIG_TASK == 1
    if (GOAL_OK != goal_mboxMsgGet(pDrvData->pMbxRxIrq, &rxRing)) {
        return;
    }
#else /* GOAL_CONFIG_TASK */
    rxRing = pDrvData->rxRing;
#endif /* GOAL_CONFIG_TASK */


    while (rxLoopCntr) {
        resSdk = EP_GetRxFrameSize(&pDrvData->epHandle, rxRing, &frameLen);
        if (kStatus_Success == resSdk) {
#if GOAL_DLR == 1
            if ((GOAL_TRUE == pDrvData->hasSwitch) && (GOAL_DRV_ETH_RXBDR_NUM_DEFAULT != rxRing)) {
                goal_drvDlrNetcRxHandler(pDrvData, rxRing, frameLen);
                return;
            }
#endif /* GOAL_DLR */
            resSdk = EP_ReceiveFrameCopy(&pDrvData->epHandle, rxRing, pDrvData->pRxBuf->ptrData, pDrvData->pRxBuf->bufSize, &attr);
            if (kStatus_Success == resSdk) {
                if (GOAL_TRUE == pDrvData->hasSwitch) {
                    /* determine source port */
                    for (cntPort = 0; cntPort < pDrvData->numPorts; cntPort++) {
                        if (pDrvData->pPortInfo[cntPort].portId == attr.srcPort + 1) {
                            break;
                        }
                    }
                    if (cntPort > pDrvData->numPorts) {
                        goal_logDbg("no GOAL port found for srcPort %u", attr.srcPort);
                        cntPort = GOAL_ETH_PORT_HOST;
                    }
                }
                else {
                    cntPort = GOAL_ETH_PORT(0);
                }
            }
            else {
                goal_logErr("failed to read frame data: %"FMT_i32, (int32_t) resSdk);
            }
        }
        else if (kStatus_NETC_RxFrameError == resSdk) {
            EP_ReceiveFrameCopy(&pDrvData->epHandle, rxRing, NULL, 0, NULL);
        }
        else if ((kStatus_NETC_RxHRNotZeroFrame == resSdk) && (GOAL_TRUE == pDrvData->hasSwitch)) {
            resSdk = SWT_GetRxFrameSize(&pDrvData->swHandle, &frameLen);
            if (kStatus_Success == resSdk) {
                resSdk = SWT_ReceiveFrameCopy(&pDrvData->swHandle, pDrvData->pRxBuf->ptrData, pDrvData->pRxBuf->bufSize, &attr);
                if (kStatus_Success == resSdk) {
                    /* determine source port */
                    for (cntPort = 0; cntPort < pDrvData->numPorts; cntPort++) {
                        if (pDrvData->pPortInfo[cntPort].portId == attr.srcPort + 1) {
                            break;
                        }
                    }
                    if (cntPort > pDrvData->numPorts) {
                        goal_logDbg("no GOAL port found for srcPort %u", attr.srcPort);
                        cntPort = GOAL_ETH_PORT_HOST;
                    }
                }
                else {
                    goal_logErr("failed to read frame data: %"FMT_i32, (int32_t) resSdk);
                }
            }
            else if (kStatus_NETC_RxFrameError == resSdk) {
                SWT_ReceiveFrameCopy(&pDrvData->swHandle, NULL, 0, NULL);
            }
            else {
                goal_logErr("failed to read frame length: %"FMT_i32, (int32_t) resSdk);
                break;
            }
        }

        if (kStatus_Success == resSdk) {
            pDrvData->pRxBuf->dataLen = (uint16_t) frameLen;
#if GOAL_PTP == 1
            if (true == attr.isTsAvail) {
                goal_drvEthNetcTimestampConvert(attr.timestamp, &pDrvData->pRxBuf->tsSec, &pDrvData->pRxBuf->tsNsec);
            }
#endif /* GOAL_PTP */
            goal_maEthRecvCb(pMaEthHdl, cntPort, &(pDrvData->pRxBuf));
        }
#if GOAL_PTP == 1
        else if ((kStatus_NETC_RxTsrResp == resSdk) && (GOAL_TRUE == pDrvData->hasSwitch)) {
            resSdk = SWT_GetTimestampRefResp(&pDrvData->swHandle, &txTsData);
            if (kStatus_Success == resSdk) {
                goal_drvEthNetcTxTimestampIdValueAdd(pMaEthHdl, txTsData.txtsid, txTsData.timestamp);
            }
            else {
                goal_logErr("failed to get Tx timestamp");
            }
        }
#endif /* GOAL_PTP */
        else if (kStatus_NETC_RxFrameEmpty == resSdk) {
            /* ring is empty */
            break;
        }
        else if (kStatus_NETC_RxFrameError == resSdk) {
            goal_logDbg("dropped frame due to reception error");
        }
        else {
            goal_logErr("failed to read frame length: %"FMT_i32, (int32_t) resSdk);
            break;
        }
        rxLoopCntr--;
    }
}


/****************************************************************************/
/** Send frame via Ethernet
 *
 * This function triggers the send process.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static void goal_drvEthNetcSend(
    GOAL_MA_ETH_T *pMaHdl                       /**< MA Hdl */
)
{
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData;         /* private driver data */

    pDrvData = (GOAL_DRV_ETH_NETC_DATA_T *) pMaHdl->pData;
#if GOAL_CONFIG_TASK == 1
    /* resume Ethernet send task */
    goal_taskResume(pDrvData->pTskEthSend);
#else
    goal_drvEthNetcTxHandler(pMaHdl, pDrvData);
#endif
}


/****************************************************************************/
/** Ethernet frame Transmission handler
 */
static void goal_drvEthNetcTxHandler(
    GOAL_MA_ETH_T *pMaEthHdl,                   /**< MA Hdl */
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData          /**< private driver data */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_BUFFER_T *pBuf = NULL;                 /* GOAL buffer */
    status_t resSdk = kStatus_Fail;             /* SDK result */
    netc_frame_struct_t txBufData;              /* netc Tx buffer data */
    netc_buffer_struct_t txBuf;                 /* netc Tx buffer */
    ep_tx_opt epTxOps;                          /* endpoint Tx options */
    uint8_t ring;                               /* Tx descriptor ring */
    netc_hw_port_idx_t swPort = kNETC_SWITCH0Port4; /* switch port */
    bool portMasquerading = false;              /* switch tx mode */
    swt_mgmt_tx_arg_t priority;                 /* tx priority */
    swt_tx_opt swTxOps;                         /* switch Tx options */
    void *pContext = NULL;                      /* sending context */

#if GOAL_CONFIG_TGT_RT1180_TRACE == 1
# if GOAL_CONFIG_TASK == 0
    goal_tgtTaskTrace(1, 1);
# endif
#endif

    res = goal_maEthSendFrameGetCb(pMaEthHdl, &pBuf);
    if (GOAL_RES_ERR(res)) {
        goal_logDbg("no Tx buffer for sending - queue cleared previously?");
        return;
    }

    txBuf.buffer = pBuf->ptrData;
    txBuf.length = pBuf->dataLen;
    txBufData.buffArray = &txBuf;
    txBufData.length = 1;

    goal_drvEthNetcTxDescRingGet(pDrvData, pBuf, &ring);
    if (GOAL_TRUE == pDrvData->hasSwitch) {
        if ((GOAL_ETH_PORT_HOST == pBuf->netPort) || (pBuf->netPort >= pDrvData->numPorts)) {
            /* let switch decide egress port */
            swPort = kNETC_SWITCH0Port4;
            pContext = NULL;
            portMasquerading = true;
            priority.ring = ring;
            if (0 != (pBuf->flags & GOAL_QUEUE_FLG_TIMESTAMP)) {
                /* request Tx timestamp */
                swTxOps.flags = kSWT_TX_OPT_REQ_TS;
                pBuf->flags &= ~GOAL_QUEUE_FLG_TIMESTAMP;
            }
            else {
                swTxOps.flags = 0;
            }
        }
        else {
            /* enforce a specific egress port */
            swPort = pDrvData->pPortInfo[pBuf->netPort].portId - 1;
            pContext = &pDrvData->pPortInfo[pBuf->netPort];
            portMasquerading = false;
            priority.ipv = ring;
            if (0 != (pBuf->flags & GOAL_QUEUE_FLG_TIMESTAMP)) {
                /* request Tx timestamp */
                swTxOps.flags = kSWT_TX_OPT_DIRECT_ENQUEUE_REQ_TSR;
                pBuf->flags &= ~GOAL_QUEUE_FLG_TIMESTAMP;
            }
            else {
                swTxOps.flags = 0;
            }
        }
    }
    else {
        if (0 != (pBuf->flags & GOAL_QUEUE_FLG_TIMESTAMP)) {
            /* request Tx timestamp */
            epTxOps.flags = kEP_TX_OPT_REQ_TS;
            pBuf->flags &= ~GOAL_QUEUE_FLG_TIMESTAMP;
        }
        else {
            epTxOps.flags = 0;
        }
    }

#if GOAL_CONFIG_TASK == 1
    /* check if previous transmission is not done yet */
    goal_lockGet(pDrvData->pTxStatusLock, GOAL_LOCK_INFINITE);
#endif /* GOAL_CONFIG_TASK */

    if (GOAL_TRUE == pDrvData->hasSwitch) {
        resSdk = SWT_SendFrame(&pDrvData->swHandle, priority, swPort, portMasquerading,
                               &txBufData, pContext, &swTxOps);
    }
    else {
        resSdk = EP_SendFrame(&pDrvData->epHandle, ring, &txBufData, NULL, &epTxOps);
    }

    if (kStatus_Success != resSdk) {
        goal_logErr("failed to send frame");
        goal_maEthSendDoneCb(pMaEthHdl, &pBuf, GOAL_FALSE);
        return;
    }

#if GOAL_CONFIG_TASK == 0
    if (GOAL_TRUE == pDrvData->hasSwitch) {
        SWT_WaitUnitilTxComplete(&pDrvData->swHandle, portMasquerading, ring);
        SWT_ReclaimTxDescriptor(&pDrvData->swHandle, portMasquerading, ring);
    }
    else {
        EP_WaitUnitilTxComplete(&pDrvData->epHandle, ring);
        EP_ReclaimTxDescriptor(&pDrvData->epHandle, ring);
    }
#endif /* !GOAL_CONFIG_TASK */

    goal_maEthSendDoneCb(pMaEthHdl, &pBuf, GOAL_TRUE);

#if GOAL_CONFIG_TGT_RT1180_TRACE == 1
# if GOAL_CONFIG_TASK == 0
    goal_tgtTaskTrace(0, 1);
# endif
#endif

}


/****************************************************************************/
/** Get the frame Tx Ring for an Ethernet frame
 *
 * The frame index corresponds to the priority of the frame
 */
static void goal_drvEthNetcTxDescRingGet(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_BUFFER_T *pBuf,                        /**< GOAL network buffer */
    uint8_t *pRingIdx                           /**< [out] index of Tx ring */
)
{
    uint16_t ethType;                           /* ethertype of current frame */
    uint8_t pcp;                                /* PCP value of VLAN tag */
    uint32_t cnt;                               /* loop counter */
    uint32_t idx;                               /* table index */

    *pRingIdx = pDrvData->qosMode.defprio;

    if (pDrvData->qosMode.mode & (GOAL_ETH_QOS_MODE_VLAN | GOAL_ETH_QOS_MODE_TYPE)) {
        ethType = GOAL_be16toh_p(&pBuf->ptrData[GOAL_ETH_ETHTYPE_OFFSET]);
    }

    /* VLAN mapping takes precedence over Ethertype mapping */
    if ((pDrvData->qosMode.mode & GOAL_ETH_QOS_MODE_VLAN) && (GOAL_ETH_ETHERTYPE_VLAN == ethType)) {
        pcp = goal_miEthVlanPcpExtract(pBuf);

        if ((GOAL_ETH_PORT_HOST == pBuf->netPort) || (pDrvData->numPorts <= pBuf->netPort)) {
            idx = 0;
        }
        else {
            idx = pBuf->netPort;
        }

        if (GOAL_DRV_ETH_VLAN_PRIO_UNUSED != pDrvData->vlanPrioMap[idx][pcp]) {
            *pRingIdx = pDrvData->vlanPrioMap[idx][pcp];
            return;
        }
    }

    if (pDrvData->qosMode.mode & GOAL_ETH_QOS_MODE_TYPE) {
        if (GOAL_ETH_ETHERTYPE_VLAN == ethType) {
            /* get actual EtherType */
            ethType = GOAL_be16toh_p(&pBuf->ptrData[GOAL_ETH_ETHTYPE_OFFSET + VLAN_TAG_LEN]);
        }

        for (cnt = 0; cnt < GOAL_DRV_ETH_ETYPE_PRIO_NUM_MAX; cnt++) {
            if (ethType == pDrvData->etypePrioMap[cnt].ethType) {
                *pRingIdx = pDrvData->etypePrioMap[cnt].prio;
                return;
            }
        }
    }
}


/****************************************************************************/
/** Tx Done Callback handler for Switch driver
 *
 * This handler is called after a frame has been transmitted by the Endpoint
 * driver.
 *
 * @retval kStatus_Success successful
 * @retval other failed
 */
static status_t goal_drvEthNetcSwitchTxDoneCbExec(
    swt_handle_t *pSwHandle,                    /**< switch driver handle */
    netc_tx_frame_info_t *pFrameInfo,           /**< frame info data */
    void *pUserData                             /**< user data */
)
{
#if GOAL_PTP == 1
    GOAL_MA_ETH_T *pMaEthHdl;                   /* eth ma handle */
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData;         /* private driver data */
    uint64_t sec;                               /* full seconds */
    uint32_t nsec;                              /* sub-seconds in ns */
    GOAL_DRV_ETH_NETC_PORT_INFO_T *pTxPort;     /* tx port info */
    uint32_t cntPort;                           /* port loop counter */

    UNUSEDARG(pSwHandle);

    pMaEthHdl = (GOAL_MA_ETH_T *) pUserData;
    pDrvData = (GOAL_DRV_ETH_NETC_DATA_T *) pMaEthHdl->pData;

    if (pFrameInfo && (pFrameInfo->isTsAvail || pFrameInfo->isTxTsIdAvail)) {
        /* determine egress port */
        pTxPort = (GOAL_DRV_ETH_NETC_PORT_INFO_T *) pFrameInfo->context;
        for (cntPort = 0; cntPort < pDrvData->numPorts; cntPort++) {
            if (&pDrvData->pPortInfo[cntPort] == pTxPort) {
                if (pFrameInfo->isTsAvail) {
                    goal_drvEthNetcTimestampConvert(pFrameInfo->timestamp, &sec, &nsec);
                    goal_miEthTimestampCaptureCbExec(pMaEthHdl, sec, nsec, cntPort);
                }
                else if (pFrameInfo->isTxTsIdAvail) {
                    goal_drvEthNetcTxTimestampIdPortAdd(pMaEthHdl, pFrameInfo->txtsid, cntPort);
                }
                break;
            }
        }
        if (cntPort >= pDrvData->numPorts) {
            goal_logErr("failed to get egress port for timestamp");
            return kStatus_Fail;
        }
    }

#else /* GOAL_PTP */
    UNUSEDARG(pSwHandle);
    UNUSEDARG(pFrameInfo);
    UNUSEDARG(pUserData);
#endif /* GOAL_PTP */

    return kStatus_Success;
}


/****************************************************************************/
/** Tx Done Callback handler for Endpoint driver
 *
 * This handler is called after a frame has been transmitted by the Endpoint
 * driver.
 *
 * @retval kStatus_Success successful
 * @retval other failed
 */
static status_t goal_drvEthNetcTxDoneCbExec(
    ep_handle_t *pEpHandle,                     /**< Endpoint driver handle */
    uint8_t ring,                               /**< descriptor ring */
    netc_tx_frame_info_t *pFrameInfo,           /**< frame info data */
    void *pUserData                             /**< user data */
)
{
#if GOAL_PTP == 1
    GOAL_MA_ETH_T *pMaEthHdl;                   /* eth ma handle */
    uint64_t sec;                               /* full seconds */
    uint32_t nsec;                              /* sub-seconds in ns */

    UNUSEDARG(pEpHandle);
    UNUSEDARG(ring);

    if (pFrameInfo && pFrameInfo->isTsAvail) {
        pMaEthHdl = (GOAL_MA_ETH_T *) pUserData;
        goal_drvEthNetcTimestampConvert(pFrameInfo->timestamp, &sec, &nsec);
        goal_miEthTimestampCaptureCbExec(pMaEthHdl, sec, nsec, 0);
    }
#else /* GOAL_PTP */
    UNUSEDARG(pEpHandle);
    UNUSEDARG(ring);
    UNUSEDARG(pFrameInfo);
    UNUSEDARG(pUserData);
#endif /* GOAL_PTP */

    return kStatus_Success;
}


#if GOAL_PTP == 1
/****************************************************************************/
/** Store the Tx Timestamp ID and the Egress port
 *
 * This function checks if a timestamp for the given ID is already available.
 * In that case the timestamp is passed to GOAL. Otherwise the Tx port is
 * stored in private driver data.
 */
static void goal_drvEthNetcTxTimestampIdPortAdd(
    GOAL_MA_ETH_T *pMaEthHdl,                   /**< Eth MA handle */
    uint16_t txTsId,                            /**< Tx Timestamp ID */
    uint32_t txPort                             /**< egress port */
)
{
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData;         /* private driver data */
    uint32_t cnt;                               /* loop counter */
    uint64_t sec;                               /* full seconds */
    uint32_t nsec;                              /* sub-seconds in ns */
    uint32_t tsVal;                             /* timestamp value */
    GOAL_BOOL_T entryFound = GOAL_FALSE;        /* matching entry found */

    pDrvData = (GOAL_DRV_ETH_NETC_DATA_T *) pMaEthHdl->pData;

    /* check if matching timestamp was already received */
    for (cnt = 0; cnt < GOAL_DRV_ETH_TXTS_NUM; cnt++) {
        if ((UINT32_MAX != pDrvData->txTsStore[cnt].txPort) &&
            (txTsId == pDrvData->txTsStore[cnt].txTsId)) {
            /* matching ID found */
            tsVal = pDrvData->txTsStore[cnt].tsVal;
            pDrvData->txTsStore[cnt].tsVal = 0;
            pDrvData->txTsStore[cnt].txPort = UINT32_MAX;
            entryFound = GOAL_TRUE;
            break;
        }
    }

    if (GOAL_DRV_ETH_TXTS_NUM <= cnt) {
        /* no timestamp available, yet, store Tx port in free entry */
        for (cnt = 0; cnt < GOAL_DRV_ETH_TXTS_NUM; cnt++) {
            if (UINT32_MAX == pDrvData->txTsStore[cnt].txPort) {
                pDrvData->txTsStore[cnt].txPort = txPort;
                pDrvData->txTsStore[cnt].txTsId = txTsId;
                break;
            }
        }
    }

    if (GOAL_TRUE == entryFound) {
        goal_drvEthNetcTimestampConvert(tsVal, &sec, &nsec);
        goal_miEthTimestampCaptureCbExec(pMaEthHdl, sec, nsec, txPort);
    }
    else if (GOAL_DRV_ETH_TXTS_NUM <= cnt) {
        goal_logErr("no free entry in Tx Timestamp store");
    }
}


/****************************************************************************/
/** Store the Tx Timestamp ID and the Timestamp value
 *
 * This function checks if a egress port for the given ID is already available.
 * In that case the timestamp is passed to GOAL. Otherwise the timestamp is
 * stored in private driver data.
 */
static void goal_drvEthNetcTxTimestampIdValueAdd(
    GOAL_MA_ETH_T *pMaEthHdl,                   /**< Eth MA handle */
    uint16_t txTsId,                            /**< Tx Timestamp ID */
    uint32_t txTimeStamp                        /**< Tx Timestamp */
)
{
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData;         /* private driver data */
    uint32_t cnt;                               /* loop counter */
    uint64_t sec;                               /* full seconds */
    uint32_t nsec;                              /* sub-seconds in ns */
    uint32_t txPort = UINT32_MAX;               /* tx port ID */

    pDrvData = (GOAL_DRV_ETH_NETC_DATA_T *) pMaEthHdl->pData;

    /* MSGINTR1_IRQHandler may try to access this data inside the interrupt
       FreeRTOS cannot handle an infinite lock in this context, so instead
       of using locks, disable the interrupts here */
    __disable_irq();

    /* check if matching egress port was already received */
    for (cnt = 0; cnt < GOAL_DRV_ETH_TXTS_NUM; cnt++) {
        if ((UINT32_MAX != pDrvData->txTsStore[cnt].txPort) &&
            (txTsId == pDrvData->txTsStore[cnt].txTsId)) {
            /* matching ID found */
            txPort = pDrvData->txTsStore[cnt].txPort;
            pDrvData->txTsStore[cnt].txPort = UINT32_MAX;
            break;
        }
    }

    if (GOAL_DRV_ETH_TXTS_NUM <= cnt) {
        /* no egress port available, yet, store Tx port in free entry */
        for (cnt = 0; cnt < GOAL_DRV_ETH_TXTS_NUM; cnt++) {
            if (UINT32_MAX == pDrvData->txTsStore[cnt].txPort) {
                /* use dummy port to mark entry as used */
                pDrvData->txTsStore[cnt].txPort = pDrvData->numPorts;
                pDrvData->txTsStore[cnt].txTsId = txTsId;
                pDrvData->txTsStore[cnt].tsVal = txTimeStamp;
                break;
            }
        }
    }

    /* reenable all interrupts */
    __enable_irq();

    if (UINT32_MAX != txPort) {
        goal_drvEthNetcTimestampConvert(txTimeStamp, &sec, &nsec);
        goal_miEthTimestampCaptureCbExec(pMaEthHdl, sec, nsec, txPort);
    }
    else if ((GOAL_DRV_ETH_TXTS_NUM <= cnt)) {
        goal_logErr("no free entry in Tx Timestamp store");
    }
}
#endif /* GOAL_PTP */


/****************************************************************************/
/** Adjust MAC Controller to speed & duplex mode of external Phy
 *
 * This function is called by goal if a port state has changed.
 */
static void goal_drvEthNetcPhyStateCbExec(
    GOAL_ETH_PORT_T port,                       /**< GOAL port id */
    uint32_t maskChg,                           /**< change mask */
    GOAL_ETH_PORT_STATE_T *pState,              /**< port state ptr */
    void *pArg                                  /**< [in] registered callback argument */
)
{
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData;         /* private driver data */
    netc_hw_mii_speed_t speed;                  /* link speed */
    netc_hw_mii_duplex_t duplex;                /* duplex mode */

    UNUSEDARG(maskChg);

    pDrvData = (GOAL_DRV_ETH_NETC_DATA_T *) pArg;

    if (GOAL_ETH_STATE_UP == pState->stateLink) {
        /* set port configuration according to PHY configuration */
        if (GOAL_ETH_SPEED_1000 == pState->speed) {
            speed = kNETC_MiiSpeed1000M;
        }
        else if (GOAL_ETH_SPEED_100 == pState->speed) {
            speed = kNETC_MiiSpeed100M;
        }
        else {
            speed = kNETC_MiiSpeed10M;
        }

        duplex = (GOAL_ETH_DUPLEX_FULL == pState->modeDuplex) ? kNETC_MiiFullDuplex : kNETC_MiiHalfDuplex;

        if (pDrvData->hasSwitch) {
            SWT_SetEthPortMII(&pDrvData->swHandle, pDrvData->pPortInfo[port].portId - 1, speed, duplex);
        }
        else {
            EP_Up(&pDrvData->epHandle, speed, duplex);
        }

        goal_logInfo("port %"FMT_u32": speed: %s, duplicity: %s", port,
            (speed == kNETC_MiiSpeed1000M) ? "1000M" : \
            (speed == kNETC_MiiSpeed100M) ? "100M" : \
            (speed == kNETC_MiiSpeed10M) ? "10M" : "unknown", \
            (duplex == kNETC_MiiFullDuplex) ? "full" : \
            (duplex == kNETC_MiiHalfDuplex) ? "half" : "unknown");
    }

#if GOAL_DLR == 1
    if ((GOAL_TRUE == pDrvData->hasSwitch) && (maskChg & GOAL_ETH_PORT_STATE_LINK) && ((0 == port) || (1 == port))) {
        goal_drvDlrNetcLinkChangeExec(pDrvData, port, pState->stateLink);
    }
#endif /* GOAL_DLR */
}


/****************************************************************************/
/** Generic command handler
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
static GOAL_STATUS_T goal_drvEthNetcCmd(
    GOAL_MA_ETH_T *pMaHdl,                      /**< MA handle */
    uint32_t id,                                /**< command ID */
    GOAL_BOOL_T wrFlag,                         /**< write flag */
    uint32_t port,                              /**< port ID */
    void *pArg                                  /**< argument */
)
{
    GOAL_ETH_CMD_DATA_T cmd;                    /* command data */
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData;         /* private driver data */
    uint32_t orgMask = 0;                       /* original port mask for certain commands */
    GOAL_BOOL_T useSwitchCmd = GOAL_FALSE;      /* use Switch Command handler */

    pDrvData = (GOAL_DRV_ETH_NETC_DATA_T *) pMaHdl->pData;

    cmd.pIface = NULL;
    cmd.cmd = id;
    cmd.flgSet = wrFlag;
    cmd.port = port;
    cmd.data.pRaw = pArg;

    switch (id) {
        case GOAL_ETH_CMD_MACTAB_SET:
            if (pDrvData->hasSwitch) {
                useSwitchCmd = GOAL_TRUE;
            }
            res = goal_drvEthNetcCmdMacTabSet(pDrvData, &cmd);
            break;

        case GOAL_ETH_CMD_MACTAB_GET:
            if (pDrvData->hasSwitch) {
                useSwitchCmd = GOAL_TRUE;
            }
            res = goal_drvEthNetcCmdMacTabGet(pDrvData, &cmd);
            break;

        case GOAL_ETH_CMD_MACTAB_CLR:
            if (pDrvData->hasSwitch) {
                useSwitchCmd = GOAL_TRUE;
            }
            res = goal_drvEthNetcCmdMacTabClear(pDrvData, &cmd);
            break;

        case GOAL_ETH_CMD_QOS_MODE:
            /* command is always handled by Ethernet driver */
            res = goal_drvEthNetcCmdQoSMode(pDrvData, &cmd);
            break;

        case GOAL_ETH_CMD_QOS_PRIO_VLAN:
            /* command is always handled by Ethernet driver */
            res = goal_drvEthNetcCmdVlanPrio(pDrvData, &cmd);
            break;

        case GOAL_ETH_CMD_QOS_PRIO_TYPE:
            /* command is always handled by Ethernet driver */
            res = goal_drvEthNetcCmdEthTypePrio(pDrvData, &cmd);
            break;

        case GOAL_ETH_CMD_TIMESTAMP:
            if (pDrvData->hasSwitch) {
                useSwitchCmd = GOAL_TRUE;
            }
            else {
                /* no additional settings required */
                res = GOAL_OK;
            }
            break;

        case GOAL_ETH_CMD_EPL:
            if (pDrvData->hasSwitch) {
                useSwitchCmd = GOAL_TRUE;
            }
            else {
                res = GOAL_OK;
            }
            break;

#if GOAL_CONFIG_TDMA == 1
        case GOAL_ETH_CMD_TDMA_CONFIG:
            /* command is always handled by Ethernet driver */
            res = goal_drvEthNetcCmdTdmaConfig(pDrvData, &cmd);
            break;

        case GOAL_ETH_CMD_TDMA_TSLT_CFG:
            /* command is always handled by Ethernet driver */
            res = goal_drvEthNetcCmdTdmaTsltConfig(pDrvData, &cmd);
            break;

        case GOAL_ETH_CMD_TDMA_ENABLE:
            /* command is always handled by Ethernet driver */
            res = goal_drvEthNetcCmdTdmaEnable(pDrvData, &cmd);
            break;
#endif /* GOAL_CONFIG_TDMA */

        case GOAL_ETH_CMD_LATENCY_EGRESS:
            if (pDrvData->hasSwitch) {
                useSwitchCmd = GOAL_TRUE;
            }
            else {
                *cmd.data.pLatencyEgress = GOAL_TGT_BOARD_LATENCY_EGRESS;
                res = GOAL_OK;
            }
            break;

        case GOAL_ETH_CMD_LATENCY_INGRESS:
            if (pDrvData->hasSwitch) {
                useSwitchCmd = GOAL_TRUE;
            }
            else {
                *cmd.data.pLatencyIngress = GOAL_TGT_BOARD_LATENCY_INGRESS;
                res = GOAL_OK;
            }
            break;

        case GOAL_ETH_CMD_FP_SUPPORTED:
            /* command is always handled by Ethernet driver */
            res = goal_drvEthNetcCmdFramePreemptSupportGet(pDrvData, &cmd);
            break;

        case GOAL_ETH_CMD_FP_ENABLE:
            /* command is always handled by Ethernet driver */
            res = goal_drvEthNetcCmdFramePreemptEnable(pDrvData, &cmd);
            break;

        case GOAL_ETH_CMD_FP_VERIFICATION_STATUS:
            /* command is always handled by Ethernet driver */
            res = goal_drvEthNetcCmdFramePreemptVerificationStatusGet(pDrvData, &cmd);
            break;

        case GOAL_ETH_CMD_FP_LOCAL_ADD_FRAG_SIZE:
            /* command is always handled by Ethernet driver */
            res = goal_drvEthNetcCmdFramePreemptAddFragSizeLocalGet(pDrvData, &cmd);
            break;

        case GOAL_ETH_CMD_FP_REMOTE_ADD_FRAG_SIZE:
            /* command is always handled by Ethernet driver */
            res = goal_drvEthNetcCmdFramePreemptAddFragSizeRemoteSet(pDrvData, &cmd);
            break;

        case GOAL_ETH_CMD_FP_LOCAL_STATUS:
            /* command is always handled by Ethernet driver */
            res = goal_drvEthNetcCmdFramePreemptStatusGet(pDrvData, &cmd);
            break;

        case GOAL_ETH_CMD_FP_TC_ASSIGN:
            /* command is always handled by Ethernet driver */
            res = goal_drvEthNetcCmdFramePreemptTrafficClassConfigAssign(pDrvData, &cmd);
            break;

#if GOAL_DLR == 1
        case GOAL_ETH_CMD_DLR_ENABLE:
            GOAL_TARGET_FALLTHROUGH;

        case GOAL_ETH_CMD_DLR_INIT:
            GOAL_TARGET_FALLTHROUGH;

        case GOAL_ETH_CMD_DLR_LAST_BEACON:
            GOAL_TARGET_FALLTHROUGH;

        case GOAL_ETH_CMD_DLR_NODE_STATE:
            GOAL_TARGET_FALLTHROUGH;

        case GOAL_ETH_CMD_DLR_RING_STATE:
            GOAL_TARGET_FALLTHROUGH;

        case GOAL_ETH_CMD_DLR_SHUTDOWN:
            GOAL_TARGET_FALLTHROUGH;

        case GOAL_ETH_CMD_DLR_SV_IP:
            GOAL_TARGET_FALLTHROUGH;

        case GOAL_ETH_CMD_DLR_SV_MAC:
            GOAL_TARGET_FALLTHROUGH;

        case GOAL_ETH_CMD_DLR_SV_PREC:
            GOAL_TARGET_FALLTHROUGH;

        case GOAL_ETH_CMD_DLR_VLAN:
            if (GOAL_TRUE == pDrvData->hasSwitch) {
                res = goal_drvDlrNetcCmdExec(pDrvData, &cmd);
            }
            else {
                res = GOAL_ERR_UNSUPPORTED;
            }
            break;
#endif /* GOAL_DLR */

        case GOAL_ETH_CMD_DLR_FORWARD:
            if (pDrvData->hasSwitch) {
                res = goal_drvEthNetcCmdDlrDefaultForwardingSet(pDrvData, &cmd);
            }
            else {
                res = GOAL_ERR_UNSUPPORTED;
            }
            break;

        default:
            /* command either not supported or will be handled by command table */
            res = GOAL_ERR_UNSUPPORTED;
            break;
    }

    /* if the switch is available the commands are handled by the switch driver */
    if (useSwitchCmd) {

        if (GOAL_RES_ERR(res)) {
            goal_logInfo("error occured before configuring switch");
            return res;
        }

        /* convert port ID and masks */
        res = goal_drvEthNetcPortToTargetConvert(pDrvData, port, &cmd.port);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to convert GOAL port 0x%08"FMT_x32" to target port", port);
            return res;
        }
        if (GOAL_ETH_CMD_MACTAB_SET == cmd.cmd) {
            orgMask = cmd.data.pMacTab->portmask;
            res = goal_drvEthNetcPortToTargetConvert(pDrvData, orgMask, &cmd.data.pMacTab->portmask);
            if (GOAL_RES_ERR(res)) {
                goal_logErr("failed to convert cmd port mask 0x%08"FMT_x32" to target port mask", orgMask);
                return res;
            }
        }
#if GOAL_CONFIG_TDMA
        else if (GOAL_ETH_CMD_TDMA_CONFIG == cmd.cmd) {
            orgMask = cmd.data.pTdmaConfig->tdmaPorts;
            res = goal_drvEthNetcPortToTargetConvert(pDrvData, orgMask, &cmd.data.pTdmaConfig->tdmaPorts);
            if (GOAL_RES_ERR(res)) {
                goal_logErr("failed to convert cmd port mask 0x%08"FMT_x32" to target port mask", orgMask);
                return res;
            }
        }
#endif /* GOAL_CONFIG_TDMA */

        /* pass command to switch driver */
        cmd.pIface = &pDrvData->ifaceSwitch;
        res = GOAL_ERR_UNSUPPORTED;
        if (NULL != cmd.pIface->pCmd) {
            res = cmd.pIface->pCmd(&cmd);
        }
        if (GOAL_RES_ERR(res)) {
            return res;
        }

        /* convert target port ID/mask to GOAL port ID/mask */
        if (GOAL_ETH_CMD_MACTAB_SET == cmd.cmd) {
            cmd.data.pMacTab->portmask = orgMask;
        }
#if GOAL_CONFIG_TDMA
        else if (GOAL_ETH_CMD_TDMA_CONFIG == cmd.cmd) {
            cmd.data.pTdmaConfig->tdmaPorts = orgMask;
        }
#endif /* GOAL_CONFIG_TDMA */
        else if (GOAL_ETH_CMD_MACTAB_GET == cmd.cmd) {
            res = goal_drvEthNetcPortFromTargetConvert(pDrvData, cmd.data.pMacTab->portmask, &cmd.data.pMacTab->portmask);
            if (GOAL_RES_ERR(res)) {
                goal_logErr("failed to convert target port mask 0x%08"FMT_x32" to cmd port mask", cmd.data.pMacTab->portmask);
                return res;
            }
        }
    }

    return res;
}


/****************************************************************************/
/** Read MAC address
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvEthNetcCmdMacAddr(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    uint8_t *pMac;                              /* assistance pointer */

    /* deny MAC address set */
    if (pCmd->flgSet) {
        goal_logErr("MAC address set not supported");
        return GOAL_ERR_MAC_SET;
    }

    /* set assistance pointer to MAC target variable */
    pMac = *pCmd->data.ppAddrMac;

    GOAL_MEMCPY(pMac, pCmd->pMaEth->cfg.macAddr, MAC_ADDR_LEN);

    return GOAL_OK;
}


/****************************************************************************/
/** Forward request to PHY
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
static GOAL_STATUS_T goal_drvEthNetcCmdPhy(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData;         /* private driver data */

    pDrvData = (GOAL_DRV_ETH_NETC_DATA_T *) pCmd->pMaEth->pData;

    /* assign and call PHY specific command handler */
    pCmd->pIface = &pDrvData->pIfacePhys[pCmd->port];

    return pCmd->pIface->pCmd(pCmd);
}


/****************************************************************************/
/** Get the number of eternal ports
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvEthNetcCmdNumPorts(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData;         /* private driver data */

    /* deny MAC address set */
    if (pCmd->flgSet) {
        goal_logErr("set not supported");
        return GOAL_ERR_ACCESS;
    }

    pDrvData = (GOAL_DRV_ETH_NETC_DATA_T *) pCmd->pMaEth->pData;

    *pCmd->data.pCntPorts = pDrvData->numPorts;

    return GOAL_OK;
}


/****************************************************************************/
/** Convert a GOAL port ID/mask to a target port ID/mask
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvEthNetcPortToTargetConvert(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    uint32_t goalPort,                          /**< GOAL port ID/mask */
    uint32_t *pTgtPort                          /**< target port ID/mask */
)
{
    uint32_t cnt;                               /* loop counter */
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    if (goalPort & GOAL_ETH_PORT_MASK) {
        /* port mask */
       *pTgtPort = GOAL_ETH_PORT_MASK;
        if (goalPort & GOAL_ETH_PORT_HOST) {
            *pTgtPort |= (1 << kNETC_SWITCH0Port4);
        }
        for (cnt = 0; cnt < pDrvData->numPorts; cnt++) {
            if (goalPort & (1 << cnt)) {
                *pTgtPort |= (1 << (pDrvData->pPortInfo[cnt].portId - 1));
            }
        }
    }
    else if (GOAL_ETH_PORT_HOST == goalPort) {
        /* port ID, host port */
        *pTgtPort = kNETC_SWITCH0Port4;
    }
    else if (goalPort < pDrvData->numPorts) {
        /* port ID, external port */
        *pTgtPort = pDrvData->pPortInfo[goalPort].portId - 1;
    }
    else {
        /* port ID, out of range */
        res = GOAL_ERR_PARAM;
    }

    return res;
}


/****************************************************************************/
/** Convert a target port ID/mask to a GOAL port ID/mask
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvEthNetcPortFromTargetConvert(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    uint32_t tgtPort,                           /**< target port ID/mask */
    uint32_t *pGoalPort                         /**< GOAL port ID/mask */
)
{
    uint32_t cnt;                               /* loop counter */
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    if (tgtPort & GOAL_ETH_PORT_MASK) {
        /* port mask */
       *pGoalPort = GOAL_ETH_PORT_MASK;
        if (tgtPort & (1 << kNETC_SWITCH0Port4)) {
            *pGoalPort |= GOAL_ETH_PORT_HOST;
        }
        for (cnt = 0; cnt < pDrvData->numPorts; cnt++) {
            if (tgtPort & (1 << (pDrvData->pPortInfo[cnt].portId - 1))) {
                *pGoalPort |= (1 << cnt);
            }
        }
    }
    else if (kNETC_SWITCH0Port4 == tgtPort) {
        /* port ID, host port */
        *pGoalPort = GOAL_ETH_PORT_HOST;
    }
    else {
        for (cnt = 0; cnt < pDrvData->numPorts; cnt++) {
            if (tgtPort == (uint32_t) (pDrvData->pPortInfo[cnt].portId - 1)) {
                *pGoalPort = cnt;
            }
        }

        if (cnt >= pDrvData->numPorts) {
            /* port ID, out of range */
            res = GOAL_ERR_PARAM;
        }
    }

    return res;
}


/****************************************************************************/
/** Enable or disable reception of a Multicast MAC address
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvEthNetcCmdMacTabSet(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
#if GOAL_CONFIG_DRV_ETH_NETC_IPFILTER == 1
    status_t resSdk;                            /* SDK result */
    netc_tb_ipf_config_t ipfConfig;             /* IPF config */
    uint32_t entryId;                           /* entry id */
    uint8_t mask[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; /* mask array */
    uint8_t addrZero[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; /* entry address */
    uint32_t cntEntry;                          /* entry iterator */

    GOAL_BOOL_T entryExists = GOAL_FALSE;       /* entry already exists */

    /* check if address should be added or removed */
    if (GOAL_ETH_PORT_HOST & (pCmd->data.pMacTab->portmask & ~GOAL_ETH_PORT_MASK)) {
        /* if forwared to host is enabled, enter IPF rule */
        GOAL_MEMSET(&ipfConfig, 0, sizeof(ipfConfig));
        GOAL_MEMCPY(ipfConfig.keye.dmac, pCmd->data.pMacTab->addr, sizeof(GOAL_ETH_MAC_ADDR_T));
        GOAL_MEMCPY(ipfConfig.keye.dmacMask, mask, sizeof(mask));
        ipfConfig.cfge.flta = kNETC_IPFNoAction;
        ipfConfig.cfge.fltfa = kNETC_IPFForwardPermit;
        ipfConfig.cfge.hr = kNETC_SoftwareDefHR0;
        ipfConfig.keye.precedence = GOAL_CONFIG_DRV_ETH_NETC_IPFILTER_PRECEDENSE;

        /* check if entry with this MAC already exists */
        for (cntEntry = 0; cntEntry < GOAL_CONFIG_DRV_ETH_NETC_IPFILTER_ENTRIES; cntEntry++) {
            /* search for mac address */
            if (GOAL_MEMCMP(&pDrvData->ipfEntries[cntEntry].addr, pCmd->data.pMacTab->addr, sizeof(GOAL_ETH_MAC_ADDR_T)) == 0) {
                /* entry already exists */
                entryExists = GOAL_TRUE;
                break;
            }
        }

        if (GOAL_FALSE == entryExists) {
            resSdk = EP_RxIPFAddTableEntry(&pDrvData->epHandle, &ipfConfig, &entryId);
            if (kStatus_Success != resSdk) {
                goal_logErr("failed to add multicast address %lu", resSdk);
                res = GOAL_ERROR;
            }
            else {
                /* find free entry in managing structure */
                for (cntEntry = 0; cntEntry < GOAL_CONFIG_DRV_ETH_NETC_IPFILTER_ENTRIES; cntEntry++) {
                    /* try to update entry */
                    if (GOAL_MEMCMP(&pDrvData->ipfEntries[cntEntry].addr, pCmd->data.pMacTab->addr, sizeof(GOAL_ETH_MAC_ADDR_T)) == 0) {
                        pDrvData->ipfEntries[cntEntry].ipfEntryId = entryId;
                        break;
                    }
                }

                if (cntEntry == GOAL_CONFIG_DRV_ETH_NETC_IPFILTER_ENTRIES) {
                    /* add new entry */
                    for (cntEntry = 0; cntEntry < GOAL_CONFIG_DRV_ETH_NETC_IPFILTER_ENTRIES; cntEntry++) {
                        if (GOAL_MEMCMP(&pDrvData->ipfEntries[cntEntry].addr, addrZero, sizeof(GOAL_ETH_MAC_ADDR_T)) == 0) {
                            GOAL_MEMCPY(&pDrvData->ipfEntries[cntEntry].addr, pCmd->data.pMacTab->addr, sizeof(GOAL_ETH_MAC_ADDR_T));
                            pDrvData->ipfEntries[cntEntry].ipfEntryId = entryId;
                            break;
                        }
                    }
                }

                if (cntEntry == GOAL_CONFIG_DRV_ETH_NETC_IPFILTER_ENTRIES) {
                    goal_logErr("IPF table is full");
                    res = GOAL_ERR_OVERFLOW;
                }
            }
        }
    }
    else {
        for (cntEntry = 0; cntEntry < GOAL_CONFIG_DRV_ETH_NETC_IPFILTER_ENTRIES; cntEntry++) {
            /* search for mac address */
            if (GOAL_MEMCMP(&pDrvData->ipfEntries[cntEntry].addr, pCmd->data.pMacTab->addr, sizeof(GOAL_ETH_MAC_ADDR_T)) == 0) {
                /* remove entry */
                resSdk = EP_RxIPFDelTableEntry(&pDrvData->epHandle, pDrvData->ipfEntries[cntEntry].ipfEntryId);
                if (kStatus_Success != resSdk) {
                    goal_logErr("failed to remove multicast address");
                    res = GOAL_ERROR;
                }
                else {
                    /* clear entry */
                    GOAL_MEMCPY(pDrvData->ipfEntries[cntEntry].addr, addrZero, sizeof(GOAL_ETH_MAC_ADDR_T));
                    pDrvData->ipfEntries[cntEntry].ipfEntryId = 0;
                    res = GOAL_OK;
                }
                break;
            }
        }
    }
#else
    UNUSEDARG(pDrvData);
    UNUSEDARG(pCmd);
#endif

    return res;
}


/****************************************************************************/
/** Get enabled Multicast MAC addresses
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvEthNetcCmdMacTabGet(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    /* functionality not implemented as an API for reading the MAC table wasn't
     * available */
    pCmd->data.pMacTab->portmask = 0;
    UNUSEDARG(pDrvData);

    return GOAL_OK;
}


/****************************************************************************/
/** Clear Multicast MAC address filters
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvEthNetcCmdMacTabClear(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
#if GOAL_CONFIG_DRV_ETH_NETC_IPFILTER == 1
    status_t resSdk;                            /* SDK result */
    uint8_t addrZero[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; /* emtry address */
    uint32_t cntEntry;                          /* entry iterator */

    /* deny read flag */
    if (!(pCmd->flgSet)) {
        goal_logErr("Only write access is allowed for clearing the MAC table.");
        return GOAL_ERROR;
    }

    for (cntEntry = 0; cntEntry < GOAL_CONFIG_DRV_ETH_NETC_IPFILTER_ENTRIES; cntEntry++) {
        /* search for mac address with valid entry */
        if (GOAL_MEMCMP(&pDrvData->ipfEntries[cntEntry].addr, addrZero, sizeof(GOAL_ETH_MAC_ADDR_T)) != 0) {
            /* remove entry */
            resSdk = EP_RxIPFDelTableEntry(&pDrvData->epHandle, pDrvData->ipfEntries[cntEntry].ipfEntryId);
            if (kStatus_Success != resSdk) {
                goal_logErr("failed to remove multicast address");
                res = GOAL_ERROR;
            }
            else {
                /* clear entry */
                GOAL_MEMCPY(pDrvData->ipfEntries[cntEntry].addr, addrZero, sizeof(GOAL_ETH_MAC_ADDR_T));
                pDrvData->ipfEntries[cntEntry].ipfEntryId = 0;
                res = GOAL_OK;
            }
            break;
        }
    }
#else
    UNUSEDARG(pDrvData);
    UNUSEDARG(pCmd);
#endif

    return res;
}


/****************************************************************************/
/** Set/Get QoS settings
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvEthNetcCmdQoSMode(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    if (GOAL_FALSE == pCmd->flgSet) {
        pCmd->data.pModeQos->mode = pDrvData->qosMode.mode;
        pCmd->data.pModeQos->defprio = pDrvData->qosMode.defprio;
    }
    else {
        /* check prio value and supported modes */
        if ((pDrvData->numTc <= pCmd->data.pModeQos->defprio) ||
            (0 != (pCmd->data.pModeQos->mode & ~(GOAL_ETH_QOS_MODE_VLAN | GOAL_ETH_QOS_MODE_TYPE)))) {
            res = GOAL_ERR_PARAM;
        }
        else {
            pDrvData->qosMode.mode = pCmd->data.pModeQos->mode;
            pDrvData->qosMode.defprio = pCmd->data.pModeQos->defprio;
        }
    }

    return res;
}


/****************************************************************************/
/** Map a VLAN PCP value to QoS Priority
 *
 * This function enables or disables QoS mapping features
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvEthNetcCmdVlanPrio(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    /* check port ID */
    if ((GOAL_ETH_PORT_MASK & pCmd->port) ||
        (GOAL_ETH_PORT_HOST == pCmd->port) ||
        (pDrvData->numPorts <= pCmd->port)) {
        return GOAL_ERR_PARAM;
    }

    /* check PCP value */
    if (GOAL_DRV_ETH_VLAN_PRIO_NUM_MAX <= pCmd->data.pPrioQosVlan->pcp) {
        return GOAL_ERR_PARAM;
    }

    if (GOAL_FALSE == pCmd->flgSet) {
        /* get priority for a pcp value */
        if (GOAL_DRV_ETH_VLAN_PRIO_UNUSED == pDrvData->vlanPrioMap[pCmd->port][pCmd->data.pPrioQosVlan->pcp]) {
            goal_logErr("no entry for PCP 0x%02x on port %"FMT_u32, pCmd->data.pPrioQosVlan->pcp, pCmd->port);
            res = GOAL_ERROR;
        }
        else {
            pCmd->data.pPrioQosVlan->prio = pDrvData->vlanPrioMap[pCmd->port][pCmd->data.pPrioQosVlan->pcp];
        }
    }
    else {
        /* set priority for a pcp value */
        if (pDrvData->numTc <= pCmd->data.pPrioQosVlan->prio) {
            goal_logErr("supported priority values: 0 .. %"FMT_u32, pDrvData->numTc - 1);
            res = GOAL_ERR_PARAM;
        }
        else {
            pDrvData->vlanPrioMap[pCmd->port][pCmd->data.pPrioQosVlan->pcp] = pCmd->data.pPrioQosVlan->prio;
        }
    }

    return res;
}


/****************************************************************************/
/** Map an EtherType to QoS Priority
 *
 * This function enables or disables QoS mapping features
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvEthNetcCmdEthTypePrio(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    int cnt;                                    /* loop counter */
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    if (GOAL_FALSE == pCmd->flgSet) {
        if (0 == pCmd->data.pPrioQosEtype->ethType) {
            /* get ethertype for a priority */
            for (cnt = 0; cnt < GOAL_DRV_ETH_ETYPE_PRIO_NUM_MAX; cnt++) {
                if (pCmd->data.pPrioQosEtype->prio == pDrvData->etypePrioMap[cnt].prio) {
                    pCmd->data.pPrioQosEtype->ethType = pDrvData->etypePrioMap[cnt].ethType;
                    break;
                }
            }
            if (GOAL_DRV_ETH_ETYPE_PRIO_NUM_MAX <= cnt) {
                goal_logErr("no entry for priority %u", pCmd->data.pPrioQosEtype->prio);
                res = GOAL_ERR_PARAM;
            }
        }
        else {
            /* get priority for an EtherType */
            for (cnt = 0; cnt < GOAL_DRV_ETH_ETYPE_PRIO_NUM_MAX; cnt++) {
                if (pCmd->data.pPrioQosEtype->ethType == pDrvData->etypePrioMap[cnt].ethType) {
                    pCmd->data.pPrioQosEtype->prio = pDrvData->etypePrioMap[cnt].prio;
                    break;
                }
            }
            if (GOAL_DRV_ETH_ETYPE_PRIO_NUM_MAX <= cnt) {
                goal_logErr("no entry for EtherType 0x%04x", pCmd->data.pPrioQosEtype->ethType);
                res = GOAL_ERR_PARAM;
            }
        }
    }
    else {
        /* look if ethertype already has a mapping */
        for (cnt = 0; cnt < GOAL_DRV_ETH_ETYPE_PRIO_NUM_MAX; cnt++) {
            if (pCmd->data.pPrioQosEtype->ethType == pDrvData->etypePrioMap[cnt].ethType) {
                break;
            }
        }

        if (GOAL_DRV_ETH_ETYPE_PRIO_NUM_MAX <= cnt) {
            /* no mapping yet, look for free entry */
            for (cnt = 0; cnt < GOAL_DRV_ETH_ETYPE_PRIO_NUM_MAX; cnt++) {
                if (pDrvData->etypePrioMap[cnt].prio == GOAL_DRV_ETH_ETYPE_PRIO_UNUSED) {
                    break;
                }
            }
            if (GOAL_DRV_ETH_ETYPE_PRIO_NUM_MAX <= cnt) {
                goal_logErr("no free entry for EtherType 0x%04x", pCmd->data.pPrioQosEtype->ethType);
                res = GOAL_ERR_OVERFLOW;
            }
        }

        if (GOAL_RES_OK(res)) {
            pDrvData->etypePrioMap[cnt].ethType = pCmd->data.pPrioQosEtype->ethType;
            pDrvData->etypePrioMap[cnt].prio = pCmd->data.pPrioQosEtype->prio;
        }
    }

    return res;
}


#if GOAL_CONFIG_TDMA == 1
/****************************************************************************/
/** Get or Set the general TDMA configuration
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvEthNetcCmdTdmaConfig(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    if (GOAL_TRUE == pCmd->flgSet) {
        if ((NULL != pCmd->data.pTdmaConfig->cb) && (0 == pCmd->data.pTdmaConfig->eventMask)) {
            goal_logErr("no events for callback");
            return GOAL_ERR_PARAM;
        }

        if (0 == pCmd->data.pTdmaConfig->tdmaCycleTime) {
            goal_logErr("invalid cycle time");
            return GOAL_ERR_PARAM;
        }

        GOAL_MEMCPY(&pDrvData->tdmaConfig, pCmd->data.pTdmaConfig, sizeof(GOAL_ETH_TDMA_CONFIG_T));
    }
    else {
        GOAL_MEMCPY(pCmd->data.pTdmaConfig, &pDrvData->tdmaConfig, sizeof(GOAL_ETH_TDMA_CONFIG_T));
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Get or Set the timeslot TDMA configuration
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvEthNetcCmdTdmaTsltConfig(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    if (GOAL_DRV_ETH_TSLT_NUM <= pCmd->data.pTdmaTsltCfg->timeslotNum) {
        return GOAL_ERR_PARAM;
    }

    if (GOAL_FALSE == pCmd->flgSet) {
        if (pCmd->data.pTdmaTsltCfg->timeslotNum <= pDrvData->maxValidTslt) {
            pCmd->data.pTdmaTsltCfg->startOffset = GOAL_TIME_NS_TO_US(pDrvData->tdmaTsltCfg[pCmd->data.pTdmaTsltCfg->timeslotNum].startOffset);
            pCmd->data.pTdmaTsltCfg->queueMsk = pDrvData->tdmaTsltCfg[pCmd->data.pTdmaTsltCfg->timeslotNum].queueMsk;
            pCmd->data.pTdmaTsltCfg->guardBand = GOAL_TIME_NS_TO_US(pDrvData->tdmaTsltCfg[pCmd->data.pTdmaTsltCfg->timeslotNum].guardBand);;
        }
        else {
            pCmd->data.pTdmaTsltCfg->startOffset = 0;
            pCmd->data.pTdmaTsltCfg->queueMsk = 0;
            pCmd->data.pTdmaTsltCfg->guardBand = 0;
        }
    }
    else {
        /* write access */
        pDrvData->tdmaTsltCfg[pCmd->data.pTdmaTsltCfg->timeslotNum].startOffset = GOAL_TIME_US_TO_NS(pCmd->data.pTdmaTsltCfg->startOffset);
        pDrvData->tdmaTsltCfg[pCmd->data.pTdmaTsltCfg->timeslotNum].guardBand = GOAL_TIME_US_TO_NS(pCmd->data.pTdmaTsltCfg->guardBand);
        pDrvData->tdmaTsltCfg[pCmd->data.pTdmaTsltCfg->timeslotNum].queueMsk = pCmd->data.pTdmaTsltCfg->queueMsk;
        if (pCmd->data.pTdmaTsltCfg->timeslotNum > pDrvData->maxValidTslt) {
            pDrvData->maxValidTslt = pCmd->data.pTdmaTsltCfg->timeslotNum;
        }
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Enable or disable TDMA scheduling
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvEthNetcCmdTdmaEnable(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* GOAL result */
    status_t resSdk = kStatus_Fail;             /* SDK result */
    uint32_t cntTslt;                           /* timeslot loop counter */
    uint32_t cntEntry = 0;                      /* list entry counter */
    netc_tb_tgs_gcl_t qbvSettings;              /* Qbv settings */
    uint32_t cycleTimeNs;                       /* Qbv cycle time in ns */
    uint32_t cycleOffsetNs;                     /* Qbv cycle time offset in ns */

    if (GOAL_FALSE == pCmd->flgSet) {
        /* read access */
        pCmd->data.pTdmaEnable->enable = pDrvData->tdmaEnabled;

        return GOAL_OK;
    }

    cycleTimeNs = GOAL_TIME_US_TO_NS(pDrvData->tdmaConfig.tdmaCycleTime);

    if (GOAL_TRUE == pCmd->data.pTdmaEnable->enable) {
        qbvSettings.baseTime = pCmd->data.pTdmaEnable->tdmaStartTime;
        qbvSettings.cycleTime = cycleTimeNs;
        qbvSettings.extTime = 0;
        qbvSettings.gcList = pDrvData->tsltCfg;

        for (cntTslt = 0; cntTslt <= pDrvData->maxValidTslt; cntTslt++) {
            /* calculate duration of each timeslot */
            if (cntTslt < pDrvData->maxValidTslt) {
                qbvSettings.gcList[cntEntry].interval = pDrvData->tdmaTsltCfg[cntTslt + 1].startOffset - pDrvData->tdmaTsltCfg[cntTslt].startOffset - pDrvData->tdmaTsltCfg[cntTslt].guardBand;
            }
            else {
                qbvSettings.gcList[cntEntry].interval = cycleTimeNs - pDrvData->tdmaTsltCfg[cntTslt].startOffset - pDrvData->tdmaTsltCfg[cntTslt].guardBand;
            }
            /* create schedule entry */
            qbvSettings.gcList[cntEntry].tcGateState = pDrvData->tdmaTsltCfg[cntTslt].queueMsk;
            qbvSettings.gcList[cntEntry].operType = kNETC_SetGateStates;
            cntEntry++;
        }

        qbvSettings.numEntries = cntEntry;

        if (pDrvData->hasSwitch) {
            for (cntEntry = 0; (cntEntry < pDrvData->numPorts) && GOAL_RES_OK(res); cntEntry++) {
                qbvSettings.entryID = (netc_tb_tgs_entry_id_t) (pDrvData->pPortInfo[cntEntry].portId - 1);
                resSdk = SWT_TxPortTGSEnable(&pDrvData->swHandle, (netc_hw_port_idx_t) qbvSettings.entryID, true, 0xFF);
                if (kStatus_Success != resSdk) {
                    goal_logErr("failed to enable time gating for port %"FMT_u32, cntEntry);
                    res = GOAL_ERROR;
                }
                else {
                    resSdk = SWT_TxTGSConfigAdminGcl(&pDrvData->swHandle, &qbvSettings);
                    if (kStatus_Success != resSdk) {
                        goal_logErr("failed to apply timeslot configuration for port %"FMT_u32, cntEntry);
                        res = GOAL_ERROR;
                    }
                }
            }
        }
        else {
            qbvSettings.entryID = kNETC_TGSEnetc0Port;
            resSdk = EP_TxPortTGSEnable(&pDrvData->epHandle, true, 0xFF);
            if (kStatus_Success != resSdk) {
                goal_logErr("failed to enable time gating");
                res = GOAL_ERROR;
            }
            else {
                resSdk = EP_TxTGSConfigAdminGcl(&pDrvData->epHandle, &qbvSettings);
                if (kStatus_Success != resSdk) {
                    goal_logErr("failed to apply timeslot configuration");
                    res = GOAL_ERROR;
                }
            }
        }
    }
    else {
        if (pDrvData->hasSwitch) {
            for (cntEntry = 0; (cntEntry < pDrvData->numPorts) && GOAL_RES_OK(res); cntEntry++) {
                resSdk = SWT_TxPortTGSEnable(&pDrvData->swHandle, (netc_hw_port_idx_t) (pDrvData->pPortInfo[cntEntry].portId - 1), false, 0);
                if (kStatus_Success != resSdk) {
                    goal_logErr("failed to disable time gating for port %"FMT_u32, cntEntry);
                    res = GOAL_ERROR;
                }
            }
        }
        else {
            resSdk = EP_TxPortTGSEnable(&pDrvData->epHandle, false, 0);
            if (kStatus_Success != resSdk) {
                goal_logErr("failed to disable time gating");
                res = GOAL_ERROR;
            }
        }
    }

    if (kStatus_Success == resSdk) {
        pDrvData->tdmaEnabled = pCmd->data.pTdmaEnable->enable;
        if (GOAL_TRUE == pDrvData->tdmaEnabled) {
            cycleOffsetNs = cycleTimeNs / 2;
            if (GOAL_DRV_TDMA_CB_OFFSET_MIN < cycleOffsetNs) {
                cycleOffsetNs = GOAL_DRV_TDMA_CB_OFFSET_MIN;
            }
            pCmd->data.pTdmaEnable->tdmaStartTime -= cycleOffsetNs;
            pCmd->data.pTdmaEnable->tdmaStartOffset = -1 * (int64_t) cycleOffsetNs;
            res = goal_targetPtpCycleCallbackStart(pCmd->data.pTdmaEnable->tdmaStartTime, cycleTimeNs,
                                                   &goal_drvEthNetcPtpCbExec, pDrvData);
        }
        else {
            res = goal_targetPtpCycleCallbackStop();
        }
    }

    return res;
}


/****************************************************************************/
/** PTP Timer callback
 *
 * This callback is used to mark the start of the TDMA cycle
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static void goal_drvEthNetcPtpCbExec(
    void *pArg                                  /**< callback argument */
)
{
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData;         /* private driver data */

    pDrvData = (GOAL_DRV_ETH_NETC_DATA_T *) pArg;

    pDrvData->tdmaConfig.cb(pDrvData->tdmaConfig.pArg, pDrvData->tdmaConfig.eventMask);
}
#endif /* GOAL_CONFIG_TDMA */


#if GOAL_CONFIG_DRV_ETH_NETC_IPFILTER == 1
/****************************************************************************/
/** Initial configuration of IPF ingress port filtering
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvEthNetcIPFInit(
    GOAL_MA_ETH_T *pMaHdl                       /**< MA Hdl */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData;         /* private driver data */
    netc_tb_ipf_config_t ipfConfig;             /* ipf config entry */
    uint32_t entryId;                           /* ipf entry id */
    uint8_t dmac_mask[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; /* broadcast MAC address and mask */
    status_t resSdk;                            /* SDK result */

    pDrvData = (GOAL_DRV_ETH_NETC_DATA_T *) pMaHdl->pData;

    /* try to manually add filters to allow normal commmunication */
    do {
        if (GOAL_RES_OK(res)) {
            /* add a catch all rule which drops all frames not covered by any rule with higher
             * precedense */
            GOAL_MEMSET(&ipfConfig, 0, sizeof(ipfConfig));
            ipfConfig.cfge.flta = kNETC_IPFNoAction;
            ipfConfig.cfge.fltfa = kNETC_IPFForwardDiscard;
            ipfConfig.cfge.hr = kNETC_SoftwareDefHR0;
            ipfConfig.keye.precedence = 0;

            resSdk = EP_RxIPFAddTableEntry(&pDrvData->epHandle, &ipfConfig, &entryId);
            if (kStatus_Success != resSdk) {
                res = GOAL_ERROR;
                break;
            }

            /* add a rule to allow broadcast */
            GOAL_MEMSET(&ipfConfig, 0, sizeof(ipfConfig));
            GOAL_MEMCPY(ipfConfig.keye.dmac, dmac_mask, sizeof(dmac_mask));
            GOAL_MEMCPY(ipfConfig.keye.dmacMask, dmac_mask, sizeof(dmac_mask));
            ipfConfig.cfge.flta = kNETC_IPFNoAction;
            ipfConfig.cfge.fltfa = kNETC_IPFForwardPermit;
            ipfConfig.cfge.hr = kNETC_SoftwareDefHR0;
            ipfConfig.keye.precedence = GOAL_CONFIG_DRV_ETH_NETC_IPFILTER_PRECEDENSE;

            resSdk = EP_RxIPFAddTableEntry(&pDrvData->epHandle, &ipfConfig, &pDrvData->ipfEntryBroadcast);
            if (kStatus_Success != resSdk) {
                res = GOAL_ERROR;
                break;
            }

            /* add a rule to allow unicast */
            GOAL_MEMSET(&ipfConfig, 0, sizeof(ipfConfig));
            GOAL_MEMCPY(ipfConfig.keye.dmac, pMaHdl->cfg.macAddr, sizeof(dmac_mask));
            GOAL_MEMCPY(ipfConfig.keye.dmacMask, dmac_mask, sizeof(dmac_mask));
            ipfConfig.cfge.flta = kNETC_IPFNoAction;
            ipfConfig.cfge.fltfa = kNETC_IPFForwardPermit;
            ipfConfig.cfge.hr = kNETC_SoftwareDefHR0;
            ipfConfig.keye.precedence = GOAL_CONFIG_DRV_ETH_NETC_IPFILTER_PRECEDENSE;

            resSdk = EP_RxIPFAddTableEntry(&pDrvData->epHandle, &ipfConfig, &pDrvData->ipfEntryUnicast);
            if (kStatus_Success != resSdk) {
                res = GOAL_ERROR;
                break;
            }

        }
    } while (0);

    return res;
}
#endif


/****************************************************************************/
/** Get Support flag for Frame Preemption of a port
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvEthNetcCmdFramePreemptSupportGet(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    GOAL_STATUS_T res;                          /* result */
    NETC_ETH_LINK_Type *pMacPortRegs;           /* MAC Port Register sets */

    if (GOAL_TRUE == pCmd->flgSet) {
        return GOAL_ERR_PARAM;
    }

    res = goal_drvEthNetcMacPortRegGet(pDrvData, pCmd->port, &pMacPortRegs);

    if (GOAL_RES_OK(res)) {
        *pCmd->data.pFramePreemptSupport = (pMacPortRegs->MAC_MERGE_MMCSR & NETC_ETH_LINK_MAC_MERGE_MMCSR_LPS_MASK) ? GOAL_TRUE : GOAL_FALSE;
    }

    return res;
}


/****************************************************************************/
/** Get or Set Frame Preemption enable status of a port
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvEthNetcCmdFramePreemptEnable(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    GOAL_STATUS_T res;                          /* result */
    NETC_ETH_LINK_Type *pMacPortRegs;           /* MAC Port Register sets */
    uint32_t regVal;                            /* register value */

    res = goal_drvEthNetcMacPortRegGet(pDrvData, pCmd->port, &pMacPortRegs);

    if (GOAL_RES_OK(res)) {
        if (GOAL_TRUE == pCmd->flgSet) {
            if (GOAL_TRUE == *pCmd->data.pFramePreemptEnable) {
                regVal = pMacPortRegs->MAC_MERGE_MMCSR & ~NETC_ETH_LINK_MAC_MERGE_MMCSR_ME_MASK;
                regVal &= ~NETC_ETH_LINK_MAC_MERGE_MMCSR_VT_MASK;
                regVal |= NETC_ETH_LINK_MAC_MERGE_MMCSR_VT(10);
                regVal |= NETC_ETH_LINK_MAC_MERGE_MMCSR_ME(1);
                regVal &= ~NETC_ETH_LINK_MAC_MERGE_MMCSR_VDIS_MASK;
                pMacPortRegs->MAC_MERGE_MMCSR = regVal | NETC_ETH_LINK_MAC_MERGE_MMCSR_ME(1);
            }
            else {
                regVal = pMacPortRegs->MAC_MERGE_MMCSR & ~NETC_ETH_LINK_MAC_MERGE_MMCSR_ME_MASK;
                regVal |= NETC_ETH_LINK_MAC_MERGE_MMCSR_VDIS_MASK;
                pMacPortRegs->MAC_MERGE_MMCSR = regVal;
            }
        }
        else {
            *pCmd->data.pFramePreemptEnable = (pMacPortRegs->MAC_MERGE_MMCSR & NETC_ETH_LINK_MAC_MERGE_MMCSR_ME_MASK) ? GOAL_TRUE : GOAL_FALSE;
        }
    }

    return res;
}


/****************************************************************************/
/** Get verification status for Frame Preemption of a port
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvEthNetcCmdFramePreemptVerificationStatusGet(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    GOAL_STATUS_T res;                          /* result */
    NETC_ETH_LINK_Type *pMacPortRegs;           /* MAC Port Register sets */
    uint32_t regVal;                            /* register value */
    netc_port_preemption_verify_status_t status; /* verification status; */

    if (GOAL_TRUE == pCmd->flgSet) {
        return GOAL_ERR_PARAM;
    }

    res = goal_drvEthNetcMacPortRegGet(pDrvData, pCmd->port, &pMacPortRegs);

    if (GOAL_RES_OK(res)) {
        regVal = pMacPortRegs->MAC_MERGE_MMCSR & NETC_ETH_LINK_MAC_MERGE_MMCSR_VSTS_MASK;
        status = (netc_port_preemption_verify_status_t) (regVal >> NETC_ETH_LINK_MAC_MERGE_MMCSR_VSTS_SHIFT);

        switch (status) {
            case kNETC_VerifyDisable:
                *pCmd->data.pFramePreemptVerificationStatus = GOAL_ETH_FP_VERIFICATION_DISABLED;
                break;

            case kNETC_VerifyInProgress:
                *pCmd->data.pFramePreemptVerificationStatus = GOAL_ETH_FP_VERIFICATION_IN_PROGRESS;
                break;

            case kNETC_VerifySuccess:
                *pCmd->data.pFramePreemptVerificationStatus = GOAL_ETH_FP_VERIFICATION_SUCCESS;
                break;

            case kNETC_VerifyFaile:
                *pCmd->data.pFramePreemptVerificationStatus = GOAL_ETH_FP_VERIFICATION_FAIL;
                break;

            default:
                *pCmd->data.pFramePreemptVerificationStatus = GOAL_ETH_FP_VERIFICATION_UNKNOWN;
                break;
        }
    }

    return res;
}


/****************************************************************************/
/** Get Local minimum additional fragmentation size of a port
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvEthNetcCmdFramePreemptAddFragSizeLocalGet(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    GOAL_STATUS_T res;                          /* result */
    NETC_ETH_LINK_Type *pMacPortRegs;           /* MAC Port Register sets */

    if (GOAL_TRUE == pCmd->flgSet) {
        return GOAL_ERR_PARAM;
    }

    res = goal_drvEthNetcMacPortRegGet(pDrvData, pCmd->port, &pMacPortRegs);

    if (GOAL_RES_OK(res)) {
        *pCmd->data.pFramePreemptAddFragSize = pMacPortRegs->MAC_MERGE_MMCSR & NETC_ETH_LINK_MAC_MERGE_MMCSR_LAFS_MASK;
        *pCmd->data.pFramePreemptAddFragSize >>= NETC_ETH_LINK_MAC_MERGE_MMCSR_LAFS_SHIFT;
    }

    return res;
}


/****************************************************************************/
/** Set Remote minimum additional fragmentation size for a port
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvEthNetcCmdFramePreemptAddFragSizeRemoteSet(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    GOAL_STATUS_T res;                          /* result */
    NETC_ETH_LINK_Type *pMacPortRegs;           /* MAC Port Register sets */
    uint32_t regVal;                            /* register value */

    if (GOAL_FALSE == pCmd->flgSet) {
        return GOAL_ERR_PARAM;
    }

    res = goal_drvEthNetcMacPortRegGet(pDrvData, pCmd->port, &pMacPortRegs);

    if (GOAL_RES_OK(res)) {
        regVal = pMacPortRegs->MAC_MERGE_MMCSR & ~NETC_ETH_LINK_MAC_MERGE_MMCSR_RAFS_MASK;
        pMacPortRegs->MAC_MERGE_MMCSR = regVal | NETC_ETH_LINK_MAC_MERGE_MMCSR_RAFS(*pCmd->data.pFramePreemptAddFragSize);
    }

    return res;
}


/****************************************************************************/
/** Get Activation Status of Frame Preemption for a port
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvEthNetcCmdFramePreemptStatusGet(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    GOAL_STATUS_T res;                          /* result */
    NETC_ETH_LINK_Type *pMacPortRegs;           /* MAC Port Register sets */

    if (GOAL_TRUE == pCmd->flgSet) {
        return GOAL_ERR_PARAM;
    }

    res = goal_drvEthNetcMacPortRegGet(pDrvData, pCmd->port, &pMacPortRegs);

    if (GOAL_RES_OK(res)) {
        *pCmd->data.pFramePreemptActive = (pMacPortRegs->MAC_MERGE_MMCSR & NETC_ETH_LINK_MAC_MERGE_MMCSR_LPA_MASK) ? GOAL_TRUE : GOAL_FALSE;
    }

    return res;
}


/****************************************************************************/
/** Mark Traffic Classes as preemptable for a port
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvEthNetcCmdFramePreemptTrafficClassConfigAssign(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    volatile uint32_t *pReg;                    /* config register */

    if (GOAL_TRUE == pDrvData->hasSwitch) {
        if ((pCmd->port & GOAL_ETH_PORT_MASK) ||
            (pCmd->port == GOAL_ETH_PORT_HOST) ||
            (pCmd->port >= pDrvData->numPorts)) {
            return GOAL_ERR_PARAM;
        }

        pReg = &(pDrvData->swHandle.hw.ports[pDrvData->pPortInfo[pCmd->port].portId - 1].port->PFPCR);
    }
    else {
        pReg = &(pDrvData->epHandle.hw.portGroup.port->PFPCR);
    }

    if (GOAL_TRUE == pCmd->flgSet) {
        *pReg = *pCmd->data.pFramePreemptTrafficClassAssign & GOAL_DRV_ETH_PFPCR_MASK;
    }
    else {
        *pCmd->data.pFramePreemptTrafficClassAssign = *pReg & GOAL_DRV_ETH_PFPCR_MASK;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Set default forwarding of DLR frames
 *
 * This command is used to disable reception of DLR frames. All DLR frames will
 * be forwarded to external ports only.
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvEthNetcCmdDlrDefaultForwardingSet(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* GOAL result */
    uint32_t cnt;                               /* loop counter */
    uint32_t portMask;                          /* GOAL port mask */
    uint8_t dlrDstMac[MAC_ADDR_LEN] = GOAL_DRV_ETH_DLR_MAC_BASE;

    if (GOAL_FALSE == pCmd->flgSet) {
        return GOAL_ERR_ACCESS;
    }

    /* mask all external ports */
    portMask = (1 << pDrvData->numPorts) - 1;
    portMask |= GOAL_ETH_PORT_MASK;

    for (cnt = GOAL_DRV_ETH_DLR_MAC_BEACON; (cnt <= GOAL_DRV_ETH_DLR_MAC_LEARNING) && GOAL_RES_OK(res); cnt++) {
        /* DLR MAC addresses differ in last byte: 0x01 .. 0x05 */
        dlrDstMac[5] = cnt;

        res = goal_miEthMcastAdd((GOAL_MI_ETH_T *) pCmd->pMaEth->pMiInst, portMask, (GOAL_ETH_MAC_ADDR_T *) &dlrDstMac[0]);
    }

    return res;
}
