/** @file
 *
 * @brief NETC DLR driver
 *
 * This module configures the NETC to handle DLR Beacon frames.
 *
 * @copyright
 * Copyright 2010-2023 port GmbH.
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
#include <plat/drv/dlr/nxp_netc/dlr_netc.h>
#include <goal_dlr.h>
#include <goal_target_dlr.h>

#include "fsl_netc_timer.h"
#include "fsl_netc_endpoint.h"


/****************************************************************************/
/* Local defines */
/****************************************************************************/
#define GOAL_DRV_DLR_IPF_PRECEDENCE_FILTER_BF_EXP 200 /**< precedence value of IPF Filter BF_EXP */
#define GOAL_DRV_DLR_IPF_PRECEDENCE_FILTER_BF_UNEX 180 /**< precedence value of IPF Filter BF_UNEX */
#define GOAL_DRV_DLR_IPF_PRECEDENCE_FILTER_OF 180 /**< precedence value of IPF Filter OF */
#define GOAL_DRV_DLR_IPF_PRECEDENCE_DLR_RX   100 /**< precedence value of IPF Filter for other DLR frames */

#define GOAL_DRV_DLR_FPLP_RING_SUBTYPE        0 /**< IPF Payload position: Ring Sub-Type */
#define GOAL_DRV_DLR_FPLP_PROTO_VERSION       1 /**< IPF Payload position: Protocol Version */
#define GOAL_DRV_DLR_FPLP_FRAME_TYPE          2 /**< IPF Payload position: Frame Type */
#define GOAL_DRV_DLR_FPLP_IP_ADDR             4 /**< IPF Payload position: Source IP address */
#define GOAL_DRV_DLR_FPLP_RING_STATE         12 /**< IPF Payload position: current Ring State */
#define GOAL_DRV_DLR_FPLP_PRECEDENCE         13 /**< IPF Payload position: Supervisor Precedence */
#define GOAL_DRV_DLR_FPLP_BEACON_INT         14 /**< IPF Payload position: Beacon Interval */
#define GOAL_DRV_DLR_FPLP_BEACON_TMO         18 /**< IPF Payload position: Beacon Timeout */

#define GOAL_DRV_DLR_MASK_VLAN_ID        0x0FFF /**< VLAN ID mask */

#define GOAL_DRV_DLR_TIMER_PERIOD             1 /**< DLR handler timer period in ms */
#define GOAL_DRV_DLR_ETHERTYPE           0x80E1 /**< DLR frame EtherType */
#define GOAL_DRV_DLR_SUBTYPE               0x02 /**< DLR Ring Sub-type */
#define GOAL_DRV_DLR_PROTO_VERSION         0x01 /**< DLR Ring Protocol Version */
#define GOAL_DRV_DLR_TYPE_BEACON           0x01 /**< DLR Frame Type: Beacon */

#define GOAL_DRV_DLR_PORT_TO_LINK_FLAG(p) (1u << p) /**< convert Port ID to link status flag */

#define GOAL_DRV_DLR_FLAGS_LINK_P0      (1u<<0) /**< internal DLR Flag: Link up for port 0 */
#define GOAL_DRV_DLR_FLAGS_LINK_P1      (1u<<1) /**< internal DLR Flag: Link up for port 1 */
#define GOAL_DRV_DLR_FLAGS_ENABLED      (1u<<2) /**< internal DLR Flag: initialization done */
#define GOAL_DRV_DLR_FLAGS_FILTER_BF_EXP (1u<<3) /**< internal DLR Flag: Filter for expected Beacon frames enabled */
#define GOAL_DRV_DLR_FLAGS_FILTER_BF_UNEX (1u<<4) /**< internal DLR Flag: Filter for unexpected Beacon frames enabled */
#define GOAL_DRV_DLR_FLAGS_FILTER_OF    (1u<<5) /**< internal DLR Flag: Filter for own frames enabled */
#define GOAL_DRV_DLR_FLAGS_BDR_POLL     (1u<<6) /**< internal DLR Flag: poll expected Beacon BDR */
#define GOAL_DRV_DLR_FLAGS_NEW_SUVI     (1u<<7) /**< internal DLR Flag: event "new Supervisor" is handled */
#define GOAL_DRV_DLR_FLAGS_NEW_STATE    (1u<<8) /**< internal DLR Flag: event "new Ring State" is handled */
#define GOAL_DRV_DLR_FLAGS_BTO_CHECK_P0 (1u<<9) /**< internal DLR Flag: check Beacon Timeout on Port 0 */
#define GOAL_DRV_DLR_FLAGS_BTO_CHECK_P1 (1u<<10) /**< internal DLR Flag: check Beacon Timeout on Port 1 */
#define GOAL_DRV_DLR_FLAGS_BTO_ALLPORTS (1u<<11) /**< internal DLR Flag: all ports have Beacon Timeout */

#define GOAL_DRV_DLR_NUM_EVENT_MSG          10 /**< number of DLR event messages */
#define GOAL_DRV_DLR_EVENT_MSG_MASK_EVENT 0x0000ffff /**< Event message mask: event Id */
#define GOAL_DRV_DLR_EVENT_MSG_MASK_PORT 0xff000000 /**< Event message mask: port Id */
#define GOAL_DRV_DLR_EVENT_MSG_SHIFT_PORT    24 /**< Event message position: port Id */

#define GOAL_DRV_DLR_RXBDR_DEFAULT            0 /**< Rx BDR for regular frames (IRQ) */
#define GOAL_DRV_DLR_RXBDR_BF_EXP             1 /**< Rx BDR for expected Beacon frames (polled) */
#define GOAL_DRV_DLR_RXBDR_BF_UNEXP           2 /**< Rx BDR for unexpected Beacon frames (IRQ) */

#define GOAL_DRV_DLR_RXBDR_NUM_TO_IRQ(n) (1 + (2 * (n))) /**< get Rx IRQ number from Rx BDR number */

#define GOAL_DRV_DLR_MAC_BASE {0x01, 0x21, 0x6C, 0x00, 0x00, 0x00} /**< DLR Destination MAC address base */
#define GOAL_DRV_DLR_MAC_NUM                  5 /**< number of DLR Destination MAC addresses */
#define GOAL_DRV_DLR_MAC_BEACON            0x01 /**< last Byte of DLR Dst MAC of Beacon frames */
#define GOAL_DRV_DLR_MAC_SIGNON            0x02 /**< last Byte of DLR Dst MAC of SignOn & NeighborCheck frames */
#define GOAL_DRV_DLR_MAC_ANNOUNCE          0x03 /**< last Byte of DLR Dst MAC of Announce frames */
#define GOAL_DRV_DLR_MAC_ADVERTISE         0x04 /**< last Byte of DLR Dst MAC of Advertise frames */
#define GOAL_DRV_DLR_MAC_LEARNING          0x05 /**< last Byte of DLR Dst MAC of Learning_Update frames */

#define GOAL_DRV_DLR_FRAME_LEN_MIN           40 /**< minimum length of Beacon frames */
#define GOAL_DRV_DLR_FRAME_BUF_SIZE         100 /**< size of Beacon frame buffer */

#define GOAL_DRV_DLR_POS_SMAC                 6 /**< DLR frame field position: Source MAC address */
#define GOAL_DRV_DLR_POS_VLAN_TCI            14 /**< DLR frame field position: VLAN Tag Control Information */
#define GOAL_DRV_DLR_POS_IP                  22 /**< DLR frame field position: Source IP address */
#define GOAL_DRV_DLR_POS_RING_STATE          30 /**< DLR frame field position: Ring State */
#define GOAL_DRV_DLR_POS_PRECEDENCE          31 /**< DLR frame field position: Supervisor Precedence */
#define GOAL_DRV_DLR_POS_INTERVAL            32 /**< DLR frame field position: Beacon Interval */
#define GOAL_DRV_DLR_POS_BTO                 36 /**< DLR frame field position: Beacon Timeout */

#define GOAL_DRV_DLR_INT_BTO_MIN       80000000 /**< minimum internal Beacon timeout in ns (80 ms) */

#define GOAL_DRV_DLR_BRX_FLAG_P0           0x01 /**< Beacon Rx Flag: Port 0 */
#define GOAL_DRV_DLR_BRX_FLAG_P1           0x02 /**< Beacon Rx Flag: Port 1 */
#define GOAL_DRV_DLR_BRX_FLAG_BOTH         0x03 /**< Beacon Rx Flag: both ports */

#define GOAL_DRV_DLR_NODESTATE_IDLE           0 /**< DLR Node State: IDLE */
#define GOAL_DRV_DLR_NODESTATE_NORMAL         1 /**< DLR Node State: NORMAL */
#define GOAL_DRV_DLR_NODESTATE_FAULT          2 /**< DLR Node State: FAULT */

#define GOAL_DRV_DLR_RINGSTATE_NORMAL         1 /**< DLR Ring State: Normal */
#define GOAL_DRV_DLR_RINGSTATE_FAULT          2 /**< DLR Ring State: Fault */


/****************************************************************************/
/* Local data types */
/****************************************************************************/
/** DLR state machine event */
typedef enum {
    GOAL_DRV_DLR_EVENT_POWER_UP = 0,            /**< DLR Event: initialize state machine */
    GOAL_DRV_DLR_EVENT_NEW_SUPERVISOR,          /**< DLR Event: New Supervisor detected */
    GOAL_DRV_DLR_EVENT_RING_STATE_CHANGED,      /**< DLR Event: Ring State has changed */
    GOAL_DRV_DLR_EVENT_BEACON_TIMEOUT,          /**< DLR Event: Beacon Rx Timeout */
    GOAL_DRV_DLR_EVENT_ALL_LINKS_DOWN,          /**< DLR Event: all links are down */
} GOAL_DRV_DLR_EVENT_T;

/** DLR state machine states */
typedef enum {
    GOAL_DRV_DLR_STATE_UNINITIALIZED,           /**< DLR State: uninitialized */
    GOAL_DRV_DLR_STATE_NO_SUPERVISOR,           /**< DLR State: no Supervisor */
    GOAL_DRV_DLR_STATE_ACTIVE_SUPERVISOR,       /**< DLR State: active Supervisor */
} GOAL_DRV_DLR_STATE_T;

/**< DLR Dst MAC forwarding table */
typedef struct {
    uint8_t dstMacLsb;                          /**< last byte of DLR Destination MAC */
    uint32_t flags;                             /**< forwarding flags */
} GOAL_DRV_DLR_FWD_TABLE_T;

/** DLR instance data */
typedef struct GOAL_DRV_DLR_DATA_T {
    GOAL_DLR_T *pDlr;                           /**< DLR instance handle */
    uint32_t flags;                             /**< internal DLR status fLags */
    GOAL_MBOX_T *pMboxEventHi;                  /**< message box for high priority event messages */
    GOAL_MBOX_T *pMboxEventLo;                  /**< message box for low priority event messages */
    GOAL_LOCK_T *pLockSm;                       /**< state machine access lock */
    uint32_t entryIdFilterBfExp;                /**< entry ID of IPF filter for expected Beacon frames */
    uint32_t entryIdFilterBfUnex;               /**< entry ID of IPF filter for unexpected Beacon frames */
    uint32_t entryIdFilterOf;                   /**< entry ID of IPF filter for own frames */
    uint32_t entryIdRx;                         /**< entry ID of IPF for DLR frame reception */
    uint64_t filterOfMatchCount;                /**< number of frames that matched Filter OF */
    uint32_t tgtPort0;                          /**< target port ID of DLR port 0 */
    uint32_t tgtPort1;                          /**< target port ID of DLR port 1 */
    netc_timer_handle_t timerHdl;               /**< Hardware timer handle */
    uint64_t lastRxTimeP1;                      /**< last Rx time of Beacon frame on port 1 */
    uint64_t lastRxTimeP0;                      /**< last Rx time of Beacon frame on port 0 */
    GOAL_TIMER_T *pTmr;                         /**< execution timer of DLR handler */
    GOAL_DRV_DLR_STATE_T smState;               /**< current state of internal state machine */
    uint32_t beaconFlags;                       /**< bitmap of ports that receive Beacon frames */
    uint8_t suViMac[MAC_ADDR_LEN];              /**< MAC address of current supervisor */
    uint32_t suViIp;                            /**< IP address of current supervisor */
    uint8_t suViPrecedence;                     /**< precedence of current supervisor */
    uint16_t suViVlanId;                        /**< VLAN ID of current supervisor */
    uint32_t ringState;                         /**< ring state from Supervisor */
    uint32_t beaconInterval;                    /**< Beacon Interval in us */
    uint32_t beaconTimeout;                     /**< Beacon Timeout in us */
    uint32_t beaconTimeoutInternal;             /**< internal Beacon Timeout in ns (min 1000000 ns) */
    uint8_t buf[GOAL_DRV_DLR_FRAME_BUF_SIZE];   /**< beacon frame buffer */
} GOAL_DRV_DLR_DATA_T;


/****************************************************************************/
/* Local functions */
/****************************************************************************/
static GOAL_STATUS_T goal_drvDlrNetcFwdInit(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData          /**< private driver data */
);

static GOAL_STATUS_T goal_drvDlrNetcRxEnable(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData          /**< private driver data */
);

static GOAL_STATUS_T goal_drvDlrNetcCmdEnable(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T goal_drvDlrNetcCmdInit(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T goal_drvDlrNetcCmdLastBeaconPortGet(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T goal_drvDlrNetcCmdNodeStateGet(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T goal_drvDlrNetcCmdRingStateGet(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T goal_drvDlrNetcCmdShutdown(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T goal_drvDlrNetcCmdSuViIpGet(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T goal_drvDlrNetcCmdSuViMacGet(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T goal_drvDlrNetcCmdSuViPrecedenceGet(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T goal_drvDlrNetcCmdSuViVlanGet(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T goal_drvDlrNetcHandlerExec(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_DRV_DLR_EVENT_T event,                 /**< DLR event */
    uint32_t port                               /**< sourc port of event */
);

static GOAL_STATUS_T goal_drvDlrNetcFilterBfExpUpdate(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData          /**< private driver data */
);

static GOAL_STATUS_T goal_drvDlrNetcFilterBfUnexCreate(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData          /**< private driver data */
);

static GOAL_STATUS_T goal_drvDlrNetcFilterOfCreate(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData          /**< private driver data */
);

static GOAL_STATUS_T goal_drvDlrNetcFilterBfExpDelete(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData          /**< private driver data */
);

static void goal_drvDlrNetcHandlerPoll(
    void *pArg                                  /**< private driver data */
);

static void goal_drvDlrNetcHiPrioTimerCbExec(
    void *pArg                                  /**< timer argument: private driver data */
);

static GOAL_STATUS_T goal_drvDlrNetcRxBdrPollAndDrop(
    ep_handle_t *pEpHandle,                     /**< endpoint handle */
    uint8_t ring,                               /**< descriptor ring number */
    uint32_t *pRxPort                           /**< [out] RX port id */
);

static GOAL_STATUS_T goal_drvDlrNetcBeaconTimeoutCheckEnable(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData          /**< private driver data */
);

static GOAL_STATUS_T goal_drvDlrNetcEventPush(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_DRV_DLR_EVENT_T event,                 /**< DLR event */
    uint32_t port,                              /**< sourc port of event */
    GOAL_BOOL_T prioHigh                        /**< high or low priority event */
);


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static uint8_t dlrDstMac[MAC_ADDR_LEN] = GOAL_DRV_DLR_MAC_BASE; /**< DLR destination MAC address */
static GOAL_DRV_DLR_FWD_TABLE_T dlrFwdTable[] = {
    {GOAL_DRV_DLR_MAC_BEACON,    GOAL_ETH_CMD_PORT_HOST | GOAL_ETH_CMD_PORT_EXT}, /**< Beacon frames are received and forwarded */
    {GOAL_DRV_DLR_MAC_SIGNON,    GOAL_ETH_CMD_PORT_HOST}, /**< SignOn frames are Peer-to-Peer */
    {GOAL_DRV_DLR_MAC_ANNOUNCE,  GOAL_ETH_CMD_PORT_HOST | GOAL_ETH_CMD_PORT_EXT}, /**< Announce frame are received and forwarded */
    {GOAL_DRV_DLR_MAC_ADVERTISE, GOAL_ETH_CMD_PORT_EXT}, /**< Advertise frames are not received */
    {GOAL_DRV_DLR_MAC_LEARNING,  GOAL_ETH_CMD_PORT_EXT}, /**< Learning frames are not received */
};


/****************************************************************************/
/* Local constants */
/****************************************************************************/
/** Destination MAC address of Beacon frames  */
static const GOAL_ETH_MAC_ADDR_T dlrBeaconMac = {
    0x01, 0x21, 0x6C, 0x00, 0x00, 0x01
};


/****************************************************************************/
/** Set a U32 value in a IPF payload entry
 */
static inline void goal_drvDlrNetcIpfPayloadU32Set(
    netc_tb_ipf_config_t *pIpfConfig,           /**< IPF config */
    uint32_t offset,                            /**< payload offset */
    uint32_t value                              /**< U32 value */
)
{
    pIpfConfig->keye.payload[offset + 0].data = (uint8_t) ((value >> 24) & 0xFF);
    pIpfConfig->keye.payload[offset + 0].mask = 0xff;
    pIpfConfig->keye.payload[offset + 1].data = (uint8_t) ((value >> 16) & 0xFF);
    pIpfConfig->keye.payload[offset + 1].mask = 0xff;
    pIpfConfig->keye.payload[offset + 2].data = (uint8_t) ((value >> 8) & 0xFF);
    pIpfConfig->keye.payload[offset + 2].mask = 0xff;
    pIpfConfig->keye.payload[offset + 3].data = (uint8_t) ((value >> 0) & 0xFF);
    pIpfConfig->keye.payload[offset + 3].mask = 0xff;
}


/****************************************************************************/
/** Initialize DLR driver
 *
 * This function is called during initialization stage.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_drvDlrNetcInit(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData          /**< private driver data */
)
{
    GOAL_STATUS_T res;                          /* result */

    pDrvData->pDlr = NULL;
    res = goal_memCalloc(&pDrvData->pDlr, sizeof(GOAL_DRV_DLR_DATA_T));
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to allocate private DLR driver data");
        return res;
    }

    pDrvData->pDlr->pMboxEventHi = NULL;
    res = goal_mboxCreate(&pDrvData->pDlr->pMboxEventHi, GOAL_DRV_DLR_NUM_EVENT_MSG, (uint16_t) sizeof(uint32_t));
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create high prio event message box");
        return res;
    }

    pDrvData->pDlr->pMboxEventLo = NULL;
    res = goal_mboxCreate(&pDrvData->pDlr->pMboxEventLo, GOAL_DRV_DLR_NUM_EVENT_MSG, (uint16_t) sizeof(uint32_t));
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create low prio event message box");
        return res;
    }

    pDrvData->pDlr->pLockSm = NULL;
    res = goal_lockCreate(GOAL_LOCK_BINARY, &(pDrvData->pDlr->pLockSm), 0, 1, GOAL_ID_TGT);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create SM access lock");
    }

    return res;
}


/****************************************************************************/
/** Enable NETC DLR driver
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_drvDlrNetcOpen(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData          /**< private driver data */
)
{
    status_t resSdk;                            /* SDK result */

    /* disable RX Interrupt for polled BDR */
    resSdk = EP_MsixSetEntryMask(&pDrvData->epHandle, GOAL_DRV_DLR_RXBDR_NUM_TO_IRQ(GOAL_DRV_DLR_RXBDR_BF_EXP), true);

    return (kStatus_Success == resSdk) ? GOAL_OK : GOAL_ERROR;
}


/****************************************************************************/
/** Link Status change handler
 */
void goal_drvDlrNetcLinkChangeExec(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    uint32_t port,                              /**< port ID */
    uint16_t linkState                          /**< new link state */
)
{
    if (GOAL_ETH_STATE_UP == linkState) {
        pDrvData->pDlr->flags |= GOAL_DRV_DLR_PORT_TO_LINK_FLAG(port);
        goal_dlrStateMachineExec(pDrvData->pDlr->pDlr, DLR_EVENT_LINKRESTORED, port);
    }
    else {
        pDrvData->pDlr->flags &= ~GOAL_DRV_DLR_PORT_TO_LINK_FLAG(port);
        goal_dlrStateMachineExec(pDrvData->pDlr->pDlr, DLR_EVENT_LINKLOST, port);
    }

    if (0 == (pDrvData->pDlr->flags & (GOAL_DRV_DLR_FLAGS_LINK_P0 | GOAL_DRV_DLR_FLAGS_LINK_P1))) {
        goal_drvDlrNetcEventPush(pDrvData, GOAL_DRV_DLR_EVENT_ALL_LINKS_DOWN, GOAL_ETH_PORT_HOST, GOAL_TRUE);
    }
}


/****************************************************************************/
/** Handle GOAL Ethernet commands
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_drvDlrNetcCmdExec(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    GOAL_STATUS_T res;                          /* result */

    switch (pCmd->cmd) {
        case GOAL_ETH_CMD_DLR_ENABLE:
            res = goal_drvDlrNetcCmdEnable(pDrvData, pCmd);
            break;

        case GOAL_ETH_CMD_DLR_INIT:
            res = goal_drvDlrNetcCmdInit(pDrvData, pCmd);
            break;

        case GOAL_ETH_CMD_DLR_LAST_BEACON:
            res = goal_drvDlrNetcCmdLastBeaconPortGet(pDrvData, pCmd);
            break;

        case GOAL_ETH_CMD_DLR_NODE_STATE:
            res = goal_drvDlrNetcCmdNodeStateGet(pDrvData, pCmd);
            break;

        case GOAL_ETH_CMD_DLR_RING_STATE:
            res = goal_drvDlrNetcCmdRingStateGet(pDrvData, pCmd);
            break;

        case GOAL_ETH_CMD_DLR_SHUTDOWN:
            res = goal_drvDlrNetcCmdShutdown(pDrvData, pCmd);
            break;

        case GOAL_ETH_CMD_DLR_SV_IP:
            res = goal_drvDlrNetcCmdSuViIpGet(pDrvData, pCmd);
            break;

        case GOAL_ETH_CMD_DLR_SV_MAC:
            res = goal_drvDlrNetcCmdSuViMacGet(pDrvData, pCmd);
            break;

        case GOAL_ETH_CMD_DLR_SV_PREC:
            res = goal_drvDlrNetcCmdSuViPrecedenceGet(pDrvData, pCmd);
            break;

        case GOAL_ETH_CMD_DLR_VLAN:
            res = goal_drvDlrNetcCmdSuViVlanGet(pDrvData, pCmd);
            break;

        default:
            res = GOAL_ERR_UNSUPPORTED;
            break;
    }

    return res;
}


/****************************************************************************/
/** Setup device for DLR
 *
 * This function adds the required mac table entries for proper DLR functionality
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
GOAL_STATUS_T goal_targetDlrInit(
    GOAL_DLR_T *pHdlDlr                         /**< DLR handle */
)
{
    GOAL_STATUS_T res;                          /* GOAL result */
    GOAL_MA_ETH_T *pMaEthHdl;                   /* eth ma handle */
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData;         /* private driver data */
    GOAL_ETH_CMD_DATA_T cmd;                    /* command data */

#if RIOP //Work around to disable DLR Init for RIOP
    return GOAL_OK;
#endif

    res = goal_maEthGetById(&pMaEthHdl, GOAL_MA_ID_ETH);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to get ETH MA handle");
        return res;
    }

    pDrvData = (GOAL_DRV_ETH_NETC_DATA_T *) pMaEthHdl->pData;

    pDrvData->pDlr->pDlr = pHdlDlr;
    pDrvData->pDlr->tgtPort0 = (uint32_t) pDrvData->pPortInfo[0].portId - 1;
    pDrvData->pDlr->tgtPort1 = (uint32_t) pDrvData->pPortInfo[1].portId - 1;

    res = goal_drvDlrNetcFwdInit(pDrvData);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to apply DLR forwarding settings");
    }

    cmd.flgSet = GOAL_TRUE;
    if (GOAL_RES_OK(res)) {
        /* initialize Switch DLR handling */
        res = goal_drvDlrNetcCmdInit(pDrvData, &cmd);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("error while initializing Switch DLR handling");
        }
    }

    if (GOAL_RES_OK(res)) {
        /* enable Switch DLR handling */
        res = goal_drvDlrNetcCmdEnable(pDrvData, &cmd);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("error while enabling Switch DLR handling");
        }
    }

    /* cleanup on error */
    if (GOAL_RES_ERR(res)) {
        /* shutdown Switch DLR handling */
        goal_drvDlrNetcCmdShutdown(pDrvData, &cmd);

        return res;
    }

    return res;
}


/****************************************************************************/
/** Process unexpected Beacon Frames
 */
void goal_drvDlrNetcRxHandler(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    uint8_t rxRingNum,                          /**< RxBDR ID */
    uint32_t frameLen                           /**< frame length */
)
{
    status_t resSdk;                            /* SDK result */
    netc_frame_attr_t attr;                     /* frame attributes */
    uint32_t port;                              /* port id */
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    uint32_t cnt;                               /* loop counter */
    uint16_t suViVlanId;                        /* Supervisor VLAN ID in current frame */
    uint32_t beaconInterval;                    /* Beacon Interval in current frame */
    uint32_t beaconTimeout;                     /* Beacon Timeout in current frame */
    uint32_t suViIp;                            /* Supervisor IP Address in current frame */

    if ((GOAL_DRV_DLR_FRAME_LEN_MIN > frameLen) || (GOAL_DRV_DLR_FRAME_BUF_SIZE < frameLen)) {
        return;
    }

    if (pDrvData->pDlr->flags & (GOAL_DRV_DLR_FLAGS_NEW_SUVI | GOAL_DRV_DLR_FLAGS_NEW_STATE)) {
        /* event is already handled, drop frame */
        EP_ReceiveFrameCopy(&pDrvData->epHandle, rxRingNum, NULL, 0, NULL);
        res = GOAL_ERROR;
    }
    else {
        resSdk = EP_ReceiveFrameCopy(&pDrvData->epHandle, rxRingNum, pDrvData->pDlr->buf, GOAL_DRV_DLR_FRAME_BUF_SIZE, &attr);
        if (kStatus_Success == resSdk) {
            if (pDrvData->pDlr->tgtPort0 == attr.srcPort) {
                port = 0;
            }
            else if (pDrvData->pDlr->tgtPort1 == attr.srcPort) {
                port = 1;
            }
            else {
                goal_logErr("unexpected port id %u", attr.srcPort);
                res = GOAL_ERROR;
            }
        }
        else {
            /* no frame available */
            res = GOAL_ERROR;
        }
    }

    if (GOAL_RES_OK(res)) {
        if ((GOAL_DRV_DLR_STATE_NO_SUPERVISOR == pDrvData->pDlr->smState) ||
            (GOAL_CMP_EQUAL != GOAL_MEMCMP(&pDrvData->pDlr->buf[GOAL_DRV_DLR_POS_SMAC], pDrvData->pDlr->suViMac, MAC_ADDR_LEN))) {
            pDrvData->pDlr->flags |= GOAL_DRV_DLR_FLAGS_NEW_SUVI;
            EP_MsixSetEntryMask(&pDrvData->epHandle, GOAL_DRV_DLR_RXBDR_NUM_TO_IRQ(GOAL_DRV_DLR_RXBDR_BF_UNEXP), true);
            res = GOAL_ERROR;
            if ((pDrvData->pDlr->buf[GOAL_DRV_DLR_POS_PRECEDENCE] > pDrvData->pDlr->suViPrecedence) || (GOAL_DRV_DLR_STATE_NO_SUPERVISOR == pDrvData->pDlr->smState)) {
                res = GOAL_OK;
            }
            else if (pDrvData->pDlr->buf[GOAL_DRV_DLR_POS_PRECEDENCE] == pDrvData->pDlr->suViPrecedence) {
                /* precedence is equal, compare MAC addresses */
                for (cnt = 0; cnt < MAC_ADDR_LEN; cnt++) {
                    if (pDrvData->pDlr->buf[GOAL_DRV_DLR_POS_SMAC + cnt] > pDrvData->pDlr->suViMac[cnt]) {
                        /* precedence higher */
                        res = GOAL_OK;
                        break;
                    }
                    else if (pDrvData->pDlr->buf[GOAL_DRV_DLR_POS_SMAC + cnt] < pDrvData->pDlr->suViMac[cnt]) {
                        /* precedence lower */
                        break;
                    }
                }
            }
            if (GOAL_RES_OK(res)) {
                /* stop timeout check because new supervisor will take over */
                pDrvData->pDlr->flags &= ~GOAL_DRV_DLR_FLAGS_BDR_POLL;
                /* new supervisor has higher precedence, parse frame data */
                GOAL_MEMCPY(pDrvData->pDlr->suViMac, &pDrvData->pDlr->buf[GOAL_DRV_DLR_POS_SMAC], MAC_ADDR_LEN);
                pDrvData->pDlr->suViIp = GOAL_be32toh_p(&pDrvData->pDlr->buf[GOAL_DRV_DLR_POS_IP]);
                pDrvData->pDlr->suViPrecedence = pDrvData->pDlr->buf[GOAL_DRV_DLR_POS_PRECEDENCE];
                pDrvData->pDlr->suViVlanId = GOAL_be16toh_p(&pDrvData->pDlr->buf[GOAL_DRV_DLR_POS_VLAN_TCI]) & GOAL_DRV_DLR_MASK_VLAN_ID;
                pDrvData->pDlr->ringState = pDrvData->pDlr->buf[GOAL_DRV_DLR_POS_RING_STATE];
                pDrvData->pDlr->beaconInterval = GOAL_be32toh_p(&pDrvData->pDlr->buf[GOAL_DRV_DLR_POS_INTERVAL]);
                pDrvData->pDlr->beaconTimeout = GOAL_be32toh_p(&pDrvData->pDlr->buf[GOAL_DRV_DLR_POS_BTO]);
                pDrvData->pDlr->beaconTimeoutInternal = GOAL_TIME_US_TO_NS(pDrvData->pDlr->beaconTimeout);
                if (GOAL_DRV_DLR_INT_BTO_MIN > pDrvData->pDlr->beaconTimeoutInternal) {
                    pDrvData->pDlr->beaconTimeoutInternal = GOAL_DRV_DLR_INT_BTO_MIN;
                }

                /* remember port that received first beacon of new supervisor */
                if (0 == port) {
                    pDrvData->pDlr->beaconFlags |= GOAL_DRV_DLR_BRX_FLAG_P0;
                }
                else {
                    pDrvData->pDlr->beaconFlags |= GOAL_DRV_DLR_BRX_FLAG_P1;
                }

                goal_drvDlrNetcEventPush(pDrvData, GOAL_DRV_DLR_EVENT_NEW_SUPERVISOR, port, GOAL_TRUE);
            }
            else {
                pDrvData->pDlr->flags &= ~GOAL_DRV_DLR_FLAGS_NEW_SUVI;
                pDrvData->pDlr->flags |= GOAL_DRV_DLR_FLAGS_BDR_POLL;
                EP_MsixSetEntryMask(&pDrvData->epHandle, GOAL_DRV_DLR_RXBDR_NUM_TO_IRQ(GOAL_DRV_DLR_RXBDR_BF_UNEXP), false);
            }
        }
        else {
            pDrvData->pDlr->flags |= GOAL_DRV_DLR_FLAGS_NEW_STATE;
            EP_MsixSetEntryMask(&pDrvData->epHandle, GOAL_DRV_DLR_RXBDR_NUM_TO_IRQ(GOAL_DRV_DLR_RXBDR_BF_UNEXP), true);
            suViIp = GOAL_be32toh_p(&pDrvData->pDlr->buf[GOAL_DRV_DLR_POS_IP]);
            suViVlanId = GOAL_be16toh_p(&pDrvData->pDlr->buf[GOAL_DRV_DLR_POS_VLAN_TCI]) & GOAL_DRV_DLR_MASK_VLAN_ID;
            beaconInterval = GOAL_be32toh_p(&pDrvData->pDlr->buf[GOAL_DRV_DLR_POS_INTERVAL]);
            beaconTimeout = GOAL_be32toh_p(&pDrvData->pDlr->buf[GOAL_DRV_DLR_POS_BTO]);

            /* Check if ring state is new. The RxBDR might still contain frames with the current state,
             * because changing the filters is relatively slow compared to the reception rate.
             */
            if ((pDrvData->pDlr->ringState != pDrvData->pDlr->buf[GOAL_DRV_DLR_POS_RING_STATE]) ||
                (pDrvData->pDlr->suViIp != suViIp) ||
                (pDrvData->pDlr->suViPrecedence != pDrvData->pDlr->buf[GOAL_DRV_DLR_POS_PRECEDENCE]) ||
                (pDrvData->pDlr->suViVlanId != suViVlanId) ||
                (pDrvData->pDlr->beaconInterval != beaconInterval) ||
                (pDrvData->pDlr->beaconTimeout != beaconTimeout)) {
                /* stop timeout check because beacon frames with new state are received
                 * by a different RxBDR until the filters have been reconfigured
                 */
                pDrvData->pDlr->flags &= ~GOAL_DRV_DLR_FLAGS_BDR_POLL;
                pDrvData->pDlr->ringState = pDrvData->pDlr->buf[GOAL_DRV_DLR_POS_RING_STATE];
                pDrvData->pDlr->suViIp = suViIp;
                pDrvData->pDlr->suViVlanId = suViVlanId;
                pDrvData->pDlr->beaconInterval = beaconInterval;
                pDrvData->pDlr->beaconTimeout = beaconTimeout;
                pDrvData->pDlr->suViPrecedence = pDrvData->pDlr->buf[GOAL_DRV_DLR_POS_PRECEDENCE];

                goal_drvDlrNetcEventPush(pDrvData, GOAL_DRV_DLR_EVENT_RING_STATE_CHANGED, port, GOAL_TRUE);
            }
            else {
                pDrvData->pDlr->flags &= ~GOAL_DRV_DLR_FLAGS_NEW_STATE;
                pDrvData->pDlr->flags |= GOAL_DRV_DLR_FLAGS_BDR_POLL;
                EP_MsixSetEntryMask(&pDrvData->epHandle, GOAL_DRV_DLR_RXBDR_NUM_TO_IRQ(GOAL_DRV_DLR_RXBDR_BF_UNEXP), false);
            }
        }
    }
}


/****************************************************************************/
/** Handle a Rx IRQ
 *
 * THis function is called if a BDR for unexpected Beacon frames received a
 * frame.
 */
void goal_drvDlrNetcRxIrqExec(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData          /**< private driver data */
)
{
    /* stop Beacon timeout detection */
    pDrvData->pDlr->flags &= ~GOAL_DRV_DLR_FLAGS_BDR_POLL;
}


/****************************************************************************/
/** Setup device for DLR
 *
 * This function adds the required mac table entries for proper DLR functionality
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvDlrNetcFwdInit(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData          /**< private driver data */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* GOAL result */
    uint32_t cnt;                               /* loop counter */
    uint32_t tblSize;                           /* size of forwarding table */
    GOAL_ETH_CMD_DATA_T cmd;                    /* command data */
    GOAL_ETH_MACTAB_ENTRY_T entry;              /* MAC table entry */

    cmd.pIface = &pDrvData->ifaceSwitch;
    cmd.cmd = GOAL_ETH_CMD_MACTAB_SET;
    cmd.flgSet = GOAL_TRUE;
    cmd.port = GOAL_ETH_PORT_HOST;
    cmd.data.pRaw = &entry;

    tblSize = ARRAY_ELEMENTS(dlrFwdTable);

    for (cnt = 0; (cnt < tblSize) && GOAL_RES_OK(res); cnt++) {
        dlrDstMac[5] = dlrFwdTable[cnt].dstMacLsb;

        cmd.data.pMacTab->portmask = GOAL_ETH_PORT_MASK;

        if (dlrFwdTable[cnt].flags & GOAL_ETH_CMD_PORT_HOST) {
            cmd.data.pMacTab->portmask |= (1 << kNETC_SWITCH0Port4);
        }

        if (dlrFwdTable[cnt].flags & GOAL_ETH_CMD_PORT_EXT) {
           cmd.data.pMacTab->portmask |= (1 << pDrvData->pDlr->tgtPort0) | (1 << pDrvData->pDlr->tgtPort1);
        }

        cmd.data.pMacTab->addr = (GOAL_ETH_MAC_ADDR_T *) dlrDstMac;

        /* only register MAC address in Switch and not in IPF table */
        if (NULL != cmd.pIface->pCmd) {
            res = cmd.pIface->pCmd(&cmd);
        }
        else {
            res = GOAL_ERROR;
        }
    }

    if (GOAL_RES_OK(res)) {
        res = goal_drvDlrNetcRxEnable(pDrvData);
    }

    return res;
}


/****************************************************************************/
/** Enable reception of DLR frames
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvDlrNetcRxEnable(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData          /**< private driver data */
)
{
    status_t resSdk;                            /* SDK result */
    netc_tb_ipf_config_t ipfConfig;             /* IPF config */

    GOAL_MEMSET(&ipfConfig, 0, sizeof(ipfConfig));
    GOAL_MEMCPY(ipfConfig.keye.dmac, dlrDstMac, sizeof(GOAL_ETH_MAC_ADDR_T));
    GOAL_MEMSET(ipfConfig.keye.dmacMask, 0xff, sizeof(GOAL_ETH_MAC_ADDR_T));
    ipfConfig.keye.dmacMask[5] = 0xf8;
    ipfConfig.cfge.flta = kNETC_IPFNoAction;
    ipfConfig.cfge.fltfa = kNETC_IPFForwardPermit;
    ipfConfig.cfge.hr = kNETC_SoftwareDefHR0;
    ipfConfig.keye.precedence = GOAL_DRV_DLR_IPF_PRECEDENCE_DLR_RX;

    resSdk = EP_RxIPFAddTableEntry(&pDrvData->epHandle, &ipfConfig, &pDrvData->pDlr->entryIdRx);
    if (kStatus_Success != resSdk) {
        goal_logErr("failed to enable reception of DLR frames");
    }

    return (kStatus_Success == resSdk) ? GOAL_OK : GOAL_ERROR;
}


/****************************************************************************/
/** Initialize DLR handler
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvDlrNetcCmdInit(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* GOAL result */
    status_t resSdk;                            /* SDK result */
    netc_timer_config_t timerConfig;            /* timer config */

    if (GOAL_FALSE == pCmd->flgSet) {
        return GOAL_ERR_ACCESS;
    }

    if (pDrvData->pDlr->flags & GOAL_DRV_DLR_FLAGS_ENABLED) {
        return GOAL_ERR_EXISTS;
    }

    GOAL_MEMSET(&timerConfig, 0, sizeof(netc_timer_config_t));
    timerConfig.clockSelect = kNETC_TimerExtRefClk;
    timerConfig.refClkHz = CLOCK_GetRootClockFreq(kCLOCK_Root_Tmr_1588);
    timerConfig.enableTimer = true;

    pDrvData->pDlr->ringState = GOAL_DRV_DLR_RINGSTATE_FAULT;
    resSdk = NETC_TimerInit(&pDrvData->pDlr->timerHdl, &timerConfig);
    if (kStatus_Success != resSdk) {
        goal_logErr("failed to enable PTP clock");
        res = GOAL_ERROR;
    }

    return res;
}


/****************************************************************************/
/** Enable DLR handler
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvDlrNetcCmdEnable(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    GOAL_STATUS_T res;                          /* result */

    if (GOAL_FALSE == pCmd->flgSet) {
        return GOAL_ERR_ACCESS;
    }

    if (pDrvData->pDlr->flags & GOAL_DRV_DLR_FLAGS_ENABLED) {
        return GOAL_ERR_EXISTS;
    }

    pDrvData->pDlr->smState = GOAL_DRV_DLR_STATE_UNINITIALIZED;

    res = goal_drvDlrNetcHandlerExec(pDrvData, GOAL_DRV_DLR_EVENT_POWER_UP, GOAL_ETH_PORT_HOST);

    if (GOAL_RES_OK(res)) {
        pDrvData->pDlr->pTmr = NULL;
        res = goal_timerCreate(&pDrvData->pDlr->pTmr, GOAL_TIMER_HIGH);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to create timer for DLR handler");
        }
    }

    if (GOAL_RES_OK(res)) {
        res = goal_timerSetup(pDrvData->pDlr->pTmr, GOAL_TIMER_PERIODIC, GOAL_DRV_DLR_TIMER_PERIOD,
                              goal_drvDlrNetcHiPrioTimerCbExec, pDrvData, GOAL_TRUE);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to set up timer for DLR handler");
        }
    }

    if (GOAL_RES_OK(res)) {
        res = goal_mainLoopParamReg(goal_drvDlrNetcHandlerPoll, pDrvData);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to register handler loop");
        }
    }

    if (GOAL_RES_OK(res)) {
        pDrvData->pDlr->flags |= GOAL_DRV_DLR_FLAGS_ENABLED;
    }

    return res;
}


/****************************************************************************/
/** Get Beacon reception status of a port
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvDlrNetcCmdLastBeaconPortGet(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    if (GOAL_TRUE == pCmd->flgSet) {
        return GOAL_ERR_ACCESS;
    }

    if (0 == pCmd->port) {
        *pCmd->data.pStateDlr = (pDrvData->pDlr->beaconFlags & GOAL_DRV_DLR_BRX_FLAG_P0) ? 1 : 0;
    }
    else if (1 == pCmd->port) {
        *pCmd->data.pStateDlr = (pDrvData->pDlr->beaconFlags & GOAL_DRV_DLR_BRX_FLAG_P1) ? 1 : 0;
    }
    else {
        res = GOAL_ERR_PARAM;
    }

    return res;
}


/****************************************************************************/
/** Get current node state
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvDlrNetcCmdNodeStateGet(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    if (GOAL_TRUE == pCmd->flgSet) {
        return GOAL_ERR_ACCESS;
    }

    /* if ring state is ok, node state is ok */
    *pCmd->data.pStateDlr = pDrvData->pDlr->ringState;

    return GOAL_OK;
}


/****************************************************************************/
/** Get current Ring State
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvDlrNetcCmdRingStateGet(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    if (GOAL_TRUE == pCmd->flgSet) {
        return GOAL_ERR_ACCESS;
    }

    *pCmd->data.pStateDlr = pDrvData->pDlr->ringState;

    return GOAL_OK;
}


/****************************************************************************/
/** Shut down DLR handler
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvDlrNetcCmdShutdown(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    GOAL_STATUS_T res;                          /* GOAL result */
    status_t resSdk;                            /* SDK result */

    if (GOAL_FALSE == pCmd->flgSet) {
        return GOAL_ERR_ACCESS;
    }

    resSdk = EP_RxIPFDelTableEntry(&pDrvData->epHandle, pDrvData->pDlr->entryIdRx);
    if (kStatus_Success != resSdk) {
        goal_logErr("failed to delete Rx Filter");
        res = GOAL_ERROR;
    }

    if (GOAL_RES_OK(res) && (pDrvData->pDlr->flags & GOAL_DRV_DLR_FLAGS_FILTER_BF_EXP)) {
        resSdk = EP_RxIPFDelTableEntry(&pDrvData->epHandle, pDrvData->pDlr->entryIdFilterBfExp);
        if (kStatus_Success != resSdk) {
            goal_logErr("failed to delete Filter BF_EXP");
            res = GOAL_ERROR;
        }
        else {
            pDrvData->pDlr->flags &= ~GOAL_DRV_DLR_FLAGS_FILTER_BF_EXP;
        }
    }

    if (GOAL_RES_OK(res) && (pDrvData->pDlr->flags & GOAL_DRV_DLR_FLAGS_FILTER_BF_UNEX)) {
        resSdk = EP_RxIPFDelTableEntry(&pDrvData->epHandle, pDrvData->pDlr->entryIdFilterBfUnex);
        if (kStatus_Success != resSdk) {
            goal_logErr("failed to delete Filter BF_UNEX");
            res = GOAL_ERROR;
        }
        else {
            pDrvData->pDlr->flags &= ~GOAL_DRV_DLR_FLAGS_FILTER_BF_UNEX;
        }
    }

    if (GOAL_RES_OK(res) && (pDrvData->pDlr->flags & GOAL_DRV_DLR_FLAGS_FILTER_OF)) {
        resSdk = SWT_RxIPFDelTableEntry(&pDrvData->swHandle, pDrvData->pDlr->entryIdFilterOf);
        if (kStatus_Success != resSdk) {
            goal_logErr("failed to delete Filter OF");
            res = GOAL_ERROR;
        }
        else {
            pDrvData->pDlr->flags &= ~GOAL_DRV_DLR_FLAGS_FILTER_OF;
        }
    }

    if (GOAL_RES_OK(res)) {
        res = goal_timerStop(pDrvData->pDlr->pTmr);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to stop timer");
        }
    }

    if (GOAL_RES_OK(res)) {
        res = goal_timerStop(pDrvData->pDlr->pTmr);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to stop timer");
        }
    }

    if (GOAL_RES_OK(res)) {
        goal_timerDelete(&pDrvData->pDlr->pTmr);
    }

    goal_mboxDestroy(&(pDrvData->pDlr->pMboxEventHi));
    goal_mboxDestroy(&(pDrvData->pDlr->pMboxEventLo));

    goal_lockDelete(pDrvData->pDlr->pLockSm);

    goal_memFree(&(pDrvData->pDlr));

    return res;;
}


/****************************************************************************/
/** Get active Supervisor's IP address
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvDlrNetcCmdSuViIpGet(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    if (GOAL_TRUE == pCmd->flgSet) {
        return GOAL_ERR_ACCESS;
    }

    *pCmd->data.pStateDlr = pDrvData->pDlr->suViIp;

    return GOAL_OK;
}


/****************************************************************************/
/** Get active Supervisor's MAC address
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvDlrNetcCmdSuViMacGet(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    if (GOAL_TRUE == pCmd->flgSet) {
        return GOAL_ERR_ACCESS;
    }

    GOAL_MEMCPY(*(pCmd->data.ppAddrMac), &pDrvData->pDlr->suViMac, MAC_ADDR_LEN);

    return GOAL_OK;
}


/****************************************************************************/
/** Get active Supervisor's precedence value
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvDlrNetcCmdSuViPrecedenceGet(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    if (GOAL_TRUE == pCmd->flgSet) {
        return GOAL_ERR_ACCESS;
    }

    *pCmd->data.pStateDlr = (uint32_t) pDrvData->pDlr->suViPrecedence;

    return GOAL_OK;
}


/****************************************************************************/
/** Get active Supervisor's VLAN ID
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvDlrNetcCmdSuViVlanGet(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    if (GOAL_TRUE == pCmd->flgSet) {
        return GOAL_ERR_ACCESS;
    }

    *pCmd->data.pStateDlr = (uint32_t) pDrvData->pDlr->suViVlanId;

    return GOAL_OK;
}


/****************************************************************************/
/** Create an event for the internal DLR state machine
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvDlrNetcHandlerExec(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_DRV_DLR_EVENT_T event,                 /**< DLR event */
    uint32_t port                               /**< sourc port of event */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_ETH_CMD_DATA_T cmd;                    /* command data */
    uint32_t linkState;                         /* link state */

    res = goal_lockGet(pDrvData->pDlr->pLockSm, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("could not get lock");
        return res;
    }

    switch (pDrvData->pDlr->smState) {
        case GOAL_DRV_DLR_STATE_UNINITIALIZED:
            switch (event) {
                case GOAL_DRV_DLR_EVENT_POWER_UP:
                    /* create filter for new supervisor's beacon frames */
                    res = goal_drvDlrNetcFilterBfUnexCreate(pDrvData);
                    if (GOAL_RES_OK(res)) {
                        /* create filter for own frames */
                        res = goal_drvDlrNetcFilterOfCreate(pDrvData);
                    }
                    if (GOAL_RES_OK(res)) {
                        pDrvData->pDlr->smState = GOAL_DRV_DLR_STATE_NO_SUPERVISOR;
                    }
                    break;

                default:
                    goal_logDbg("ignore event %d in state %d", event, pDrvData->pDlr->smState);
                    break;
            }
            break;

        case GOAL_DRV_DLR_STATE_NO_SUPERVISOR:
            switch (event) {
                case GOAL_DRV_DLR_EVENT_NEW_SUPERVISOR:
                    /* update filter for Expected Beacon frame */
                    res = goal_drvDlrNetcFilterBfExpUpdate(pDrvData);
                    if (GOAL_RES_OK(res)) {
                        /* start beacon timeout monitoring */
                        res = goal_drvDlrNetcBeaconTimeoutCheckEnable(pDrvData);
                    }
                    if (GOAL_RES_OK(res)) {
                        goal_dlrStateMachineExec(pDrvData->pDlr->pDlr, DLR_EVENT_NEWSTATE, GOAL_ETH_PORT_HOST);
                        pDrvData->pDlr->smState = GOAL_DRV_DLR_STATE_ACTIVE_SUPERVISOR;
                        pDrvData->pDlr->flags &= ~GOAL_DRV_DLR_FLAGS_NEW_SUVI;
                        EP_MsixSetEntryMask(&pDrvData->epHandle, GOAL_DRV_DLR_RXBDR_NUM_TO_IRQ(GOAL_DRV_DLR_RXBDR_BF_UNEXP), false);
                    }
                    break;

                default:
                    goal_logDbg("ignore event %d in state %d", event, pDrvData->pDlr->smState);
                    break;
            }
            break;

        case GOAL_DRV_DLR_STATE_ACTIVE_SUPERVISOR:
            switch (event) {
                case GOAL_DRV_DLR_EVENT_NEW_SUPERVISOR:
                    /* update filter for Expected Beacon frame */
                    res = goal_drvDlrNetcFilterBfExpUpdate(pDrvData);
                    if (GOAL_RES_OK(res)) {
                        /* start beacon timeout monitoring */
                        res = goal_drvDlrNetcBeaconTimeoutCheckEnable(pDrvData);
                    }
                    if (GOAL_RES_OK(res)) {
                        goal_dlrStateMachineExec(pDrvData->pDlr->pDlr, DLR_EVENT_NEWSUPERVISOR, port);
                        pDrvData->pDlr->flags &= ~GOAL_DRV_DLR_FLAGS_NEW_SUVI;
                        EP_MsixSetEntryMask(&pDrvData->epHandle, GOAL_DRV_DLR_RXBDR_NUM_TO_IRQ(GOAL_DRV_DLR_RXBDR_BF_UNEXP), false);
                    }
                    break;

                case GOAL_DRV_DLR_EVENT_RING_STATE_CHANGED:
                    /* update filter for Expected Beacon frame */
                    res = goal_drvDlrNetcFilterBfExpUpdate(pDrvData);
                    if (GOAL_RES_OK(res)) {
                        /* start beacon timeout monitoring */
                        res = goal_drvDlrNetcBeaconTimeoutCheckEnable(pDrvData);
                    }
                    if (GOAL_RES_OK(res)) {
                        goal_dlrStateMachineExec(pDrvData->pDlr->pDlr, DLR_EVENT_NEWSTATE, port);
                        pDrvData->pDlr->flags &= ~GOAL_DRV_DLR_FLAGS_NEW_STATE;
                        EP_MsixSetEntryMask(&pDrvData->epHandle, GOAL_DRV_DLR_RXBDR_NUM_TO_IRQ(GOAL_DRV_DLR_RXBDR_BF_UNEXP), false);
                    }
                    break;

                case GOAL_DRV_DLR_EVENT_BEACON_TIMEOUT:
                    if (pDrvData->pDlr->flags & GOAL_DRV_DLR_FLAGS_BDR_POLL) {
                        cmd.pIface = &pDrvData->pIfacePhys[port];
                        cmd.cmd = GOAL_ETH_CMD_LINK_STATE;
                        cmd.flgSet = GOAL_FALSE;
                        cmd.port = port;
                        cmd.data.pStateLink = &linkState;
                        res = cmd.pIface->pCmd(&cmd);
                        if (GOAL_RES_OK(res) && (GOAL_ETH_STATE_UP == linkState)) {
                            /* only throw event, if timeout check is still active */
                            goal_dlrStateMachineExec(pDrvData->pDlr->pDlr, DLR_EVENT_BEACONTIMEOUT, port);
                            if (pDrvData->pDlr->flags & GOAL_DRV_DLR_FLAGS_BTO_ALLPORTS) {
                                pDrvData->pDlr->flags &= ~GOAL_DRV_DLR_FLAGS_BTO_ALLPORTS;
                                goal_drvDlrNetcFilterBfExpDelete(pDrvData);
                                pDrvData->pDlr->smState = GOAL_DRV_DLR_STATE_NO_SUPERVISOR;
                            }
                        }
                    }
                    break;

                case GOAL_DRV_DLR_EVENT_ALL_LINKS_DOWN:
                    pDrvData->pDlr->flags &= ~GOAL_DRV_DLR_FLAGS_BDR_POLL;
                    goal_drvDlrNetcFilterBfExpDelete(pDrvData);
                    pDrvData->pDlr->smState = GOAL_DRV_DLR_STATE_NO_SUPERVISOR;
                    break;

                default:
                    goal_logDbg("ignore event %d in state %d", event, pDrvData->pDlr->smState);
                    break;
            }
            break;

        default:
            goal_logErr("ignore event %d in unknown state %d", event, pDrvData->pDlr->smState);
            res = GOAL_ERROR;
            break;
    }

    goal_lockPut(pDrvData->pDlr->pLockSm);

    return res;
}


/****************************************************************************/
/** Delete Filter for expected Beacon Frames
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvDlrNetcFilterBfExpDelete(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData          /**< private driver data */
)
{
    status_t resSdk = kStatus_Success;          /* SDK result */

    if (pDrvData->pDlr->flags & GOAL_DRV_DLR_FLAGS_FILTER_BF_EXP) {
        resSdk = EP_RxIPFDelTableEntry(&pDrvData->epHandle, pDrvData->pDlr->entryIdFilterBfExp);
        if (kStatus_Success != resSdk) {
            goal_logErr("failed to delete Filter BF_EXP");
        }
        else {
            pDrvData->pDlr->flags &= ~GOAL_DRV_DLR_FLAGS_FILTER_BF_EXP;
        }
    }

    return (kStatus_Success == resSdk) ? GOAL_OK : GOAL_ERROR;
}


/****************************************************************************/
/** Update Filter for expected Beacon frames
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvDlrNetcFilterBfExpUpdate(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData          /**< private driver data */
)
{
    netc_tb_ipf_config_t ipfConfig;             /* IPF config */
    status_t resSdk;                            /* SDK result */
    GOAL_STATUS_T res;                          /* result */

    res = goal_drvDlrNetcFilterBfExpDelete(pDrvData);

    if (GOAL_RES_OK(res)) {
        GOAL_MEMSET(&ipfConfig, 0, sizeof(ipfConfig));
        GOAL_MEMCPY(ipfConfig.keye.dmac, dlrBeaconMac, MAC_ADDR_LEN);
        GOAL_MEMSET(ipfConfig.keye.dmacMask, 0xff, MAC_ADDR_LEN);
        GOAL_MEMCPY(ipfConfig.keye.smac, pDrvData->pDlr->suViMac, MAC_ADDR_LEN);
        GOAL_MEMSET(ipfConfig.keye.smacMask, 0xff, MAC_ADDR_LEN);
        ipfConfig.keye.outerVlanTCI = GOAL_htobe16(pDrvData->pDlr->suViVlanId);
        ipfConfig.keye.outerVlanTCIMask = GOAL_htobe16(GOAL_DRV_DLR_MASK_VLAN_ID);
        ipfConfig.keye.etherType = GOAL_htobe16(GOAL_DRV_DLR_ETHERTYPE);;
        ipfConfig.keye.etherTypeMask = 0xffff;
        ipfConfig.keye.payload[GOAL_DRV_DLR_FPLP_RING_SUBTYPE].data = GOAL_DRV_DLR_SUBTYPE;
        ipfConfig.keye.payload[GOAL_DRV_DLR_FPLP_RING_SUBTYPE].mask = 0xff;
        ipfConfig.keye.payload[GOAL_DRV_DLR_FPLP_PROTO_VERSION].data = GOAL_DRV_DLR_PROTO_VERSION;
        ipfConfig.keye.payload[GOAL_DRV_DLR_FPLP_PROTO_VERSION].mask = 0xff;
        ipfConfig.keye.payload[GOAL_DRV_DLR_FPLP_FRAME_TYPE].data = GOAL_DRV_DLR_TYPE_BEACON;
        ipfConfig.keye.payload[GOAL_DRV_DLR_FPLP_FRAME_TYPE].mask = 0xff;
        goal_drvDlrNetcIpfPayloadU32Set(&ipfConfig, GOAL_DRV_DLR_FPLP_IP_ADDR, pDrvData->pDlr->suViIp);
        ipfConfig.keye.payload[GOAL_DRV_DLR_FPLP_RING_STATE].data = pDrvData->pDlr->ringState;
        ipfConfig.keye.payload[GOAL_DRV_DLR_FPLP_RING_STATE].mask = 0xff;
        ipfConfig.keye.payload[GOAL_DRV_DLR_FPLP_PRECEDENCE].data = pDrvData->pDlr->suViPrecedence;
        ipfConfig.keye.payload[GOAL_DRV_DLR_FPLP_PRECEDENCE].mask = 0xff;
        goal_drvDlrNetcIpfPayloadU32Set(&ipfConfig, GOAL_DRV_DLR_FPLP_BEACON_INT, pDrvData->pDlr->beaconInterval);
        goal_drvDlrNetcIpfPayloadU32Set(&ipfConfig, GOAL_DRV_DLR_FPLP_BEACON_TMO, pDrvData->pDlr->beaconTimeout);

        ipfConfig.cfge.flta = kNETC_IPFNoAction;
        ipfConfig.cfge.fltfa = kNETC_IPFForwardPermit;
        ipfConfig.cfge.ipv = GOAL_DRV_DLR_RXBDR_BF_EXP;
        ipfConfig.cfge.oipv = 1;
        ipfConfig.keye.precedence = GOAL_DRV_DLR_IPF_PRECEDENCE_FILTER_BF_EXP;

        resSdk = EP_RxIPFAddTableEntry(&pDrvData->epHandle, &ipfConfig, &pDrvData->pDlr->entryIdFilterBfExp);
        if (kStatus_Success != resSdk) {
            goal_logErr("failed to add Filter BF_EXP");
            res = GOAL_ERROR;
        }
        else {
            pDrvData->pDlr->flags |= GOAL_DRV_DLR_FLAGS_FILTER_BF_EXP;
            res = GOAL_OK;
        }
    }

    return res;
}


/****************************************************************************/
/** Create Filter for unexpected Beacon Frames
 *
 * THis filter filters all Beacon frames. Expected Beacon frames are filtered by
 * an IPF entry with higher precedence.
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvDlrNetcFilterBfUnexCreate(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData          /**< private driver data */
)
{
    netc_tb_ipf_config_t ipfConfig;             /* IPF config */
    status_t resSdk;                            /* SDK result */

    GOAL_MEMSET(&ipfConfig, 0, sizeof(ipfConfig));
    GOAL_MEMCPY(ipfConfig.keye.dmac, dlrBeaconMac, MAC_ADDR_LEN);
    GOAL_MEMSET(ipfConfig.keye.dmacMask, 0xff, MAC_ADDR_LEN);
    ipfConfig.keye.etherType = GOAL_htobe16(GOAL_DRV_DLR_ETHERTYPE);
    ipfConfig.keye.etherTypeMask = 0xffff;
    ipfConfig.keye.payload[GOAL_DRV_DLR_FPLP_RING_SUBTYPE].data = GOAL_DRV_DLR_SUBTYPE;
    ipfConfig.keye.payload[GOAL_DRV_DLR_FPLP_RING_SUBTYPE].mask = 0xff;
    ipfConfig.keye.payload[GOAL_DRV_DLR_FPLP_PROTO_VERSION].data = GOAL_DRV_DLR_PROTO_VERSION;
    ipfConfig.keye.payload[GOAL_DRV_DLR_FPLP_PROTO_VERSION].mask = 0xff;
    ipfConfig.keye.payload[GOAL_DRV_DLR_FPLP_FRAME_TYPE].data = GOAL_DRV_DLR_TYPE_BEACON;
    ipfConfig.keye.payload[GOAL_DRV_DLR_FPLP_FRAME_TYPE].mask = 0xff;

    ipfConfig.cfge.flta = kNETC_IPFNoAction;
    ipfConfig.cfge.fltfa = kNETC_IPFForwardPermit;
    ipfConfig.cfge.ipv = GOAL_DRV_DLR_RXBDR_BF_UNEXP;
    ipfConfig.cfge.oipv = 1;
    ipfConfig.keye.precedence = GOAL_DRV_DLR_IPF_PRECEDENCE_FILTER_BF_UNEX;

    resSdk = EP_RxIPFAddTableEntry(&pDrvData->epHandle, &ipfConfig, &pDrvData->pDlr->entryIdFilterBfUnex);
    if (kStatus_Success != resSdk) {
        goal_logErr("failed to add Filter C");
    }
    else {
        pDrvData->pDlr->flags |= GOAL_DRV_DLR_FLAGS_FILTER_BF_UNEX;
    }

    return (kStatus_Success == resSdk) ? GOAL_OK : GOAL_ERROR;
}


/****************************************************************************/
/** Create Filter for own frames
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvDlrNetcFilterOfCreate(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData          /**< private driver data */
)
{
    netc_tb_ipf_config_t ipfConfig;             /* IPF config */
    status_t resSdk;                            /* SDK result */
    GOAL_STATUS_T res;                          /* GOAL result */
    GOAL_MA_ETH_T *pMaEthHdl;                   /* eth ma handle */

    res = goal_maEthGetById(&pMaEthHdl, GOAL_MA_ID_ETH);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to get ETH MA handle");
        return res;
    }

    GOAL_MEMSET(&ipfConfig, 0, sizeof(ipfConfig));
    GOAL_MEMCPY(ipfConfig.keye.smac, pMaEthHdl->cfg.macAddr, MAC_ADDR_LEN);
    GOAL_MEMSET(ipfConfig.keye.smacMask, 0xff, MAC_ADDR_LEN);

    ipfConfig.cfge.flta = kNETC_IPFNoAction;
    ipfConfig.cfge.fltfa = kNETC_IPFForwardDiscard;
    ipfConfig.keye.precedence = GOAL_DRV_DLR_IPF_PRECEDENCE_FILTER_OF;

    resSdk = SWT_RxIPFAddTableEntry(&pDrvData->swHandle, &ipfConfig, &pDrvData->pDlr->entryIdFilterOf);
    if (kStatus_Success != resSdk) {
        goal_logErr("failed to add Filter OF");
    }
    else {
        pDrvData->pDlr->flags |= GOAL_DRV_DLR_FLAGS_FILTER_OF;
    }

    return (kStatus_Success == resSdk) ? GOAL_OK : GOAL_ERROR;
}


/****************************************************************************/
/** Check for low priority events
 */
static void goal_drvDlrNetcHandlerPoll(
    void *pArg                                  /**< private driver data */
)
{
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData;         /* private driver data */
    GOAL_STATUS_T res;                          /* result */
    uint32_t pollCnt = 0;                       /* number of BDR poll iterations */
    uint64_t now;                               /* current time in ns */
    status_t resSdk;                            /* SDK result */
    uint64_t matchCount;                        /* filter match count */
    uint32_t port;                              /* port Id */
    uint32_t eventMsg;                          /* event message */
    GOAL_DRV_DLR_EVENT_T dlrEvent;              /* DLR event */

    pDrvData = (GOAL_DRV_ETH_NETC_DATA_T *) pArg;

    if ((GOAL_DRV_DLR_FLAGS_BDR_POLL & pDrvData->pDlr->flags)) {
        /* poll Rx BDR (expected Beacon frames) */
        NETC_TimerGetCurrentTime(&pDrvData->pDlr->timerHdl, &now);
        do {
            res = goal_drvDlrNetcRxBdrPollAndDrop(&pDrvData->epHandle, GOAL_DRV_DLR_RXBDR_BF_EXP, &port);
            if (GOAL_RES_OK(res)) {
                if (port == pDrvData->pDlr->tgtPort0) {
                    pDrvData->pDlr->lastRxTimeP0 = now;
                    pDrvData->pDlr->flags |= GOAL_DRV_DLR_FLAGS_BTO_CHECK_P0;
                    pDrvData->pDlr->beaconFlags |= GOAL_DRV_DLR_BRX_FLAG_P0;
                }
                else {
                    pDrvData->pDlr->lastRxTimeP1 = now;
                    pDrvData->pDlr->flags |= GOAL_DRV_DLR_FLAGS_BTO_CHECK_P1;
                    pDrvData->pDlr->beaconFlags |= GOAL_DRV_DLR_BRX_FLAG_P1;
                }
            }
            pollCnt++;
        } while (GOAL_RES_OK(res) && (5 > pollCnt));

        if ((pDrvData->pDlr->flags & GOAL_DRV_DLR_FLAGS_BTO_CHECK_P0) &&
            (pDrvData->pDlr->beaconTimeoutInternal <= (now - pDrvData->pDlr->lastRxTimeP0))) {
            if (pDrvData->pDlr->flags & GOAL_DRV_DLR_FLAGS_LINK_P0) {
                goal_drvDlrNetcEventPush(pDrvData, GOAL_DRV_DLR_EVENT_BEACON_TIMEOUT, 0, GOAL_FALSE);
            }
            if (GOAL_DRV_DLR_BRX_FLAG_BOTH == pDrvData->pDlr->beaconFlags) {
                /* remember last beacon rx port, only clear flag if other port still receives beacons */
                pDrvData->pDlr->beaconFlags &= ~GOAL_DRV_DLR_BRX_FLAG_P0;
            }
            else {
                /* both ports have a beacon timeout, Supervisor is gone */
                pDrvData->pDlr->flags |= GOAL_DRV_DLR_FLAGS_BTO_ALLPORTS;
            }
            pDrvData->pDlr->flags &= ~GOAL_DRV_DLR_FLAGS_BTO_CHECK_P0;
        }
        if ((pDrvData->pDlr->flags & GOAL_DRV_DLR_FLAGS_BTO_CHECK_P1) &&
            (pDrvData->pDlr->beaconTimeoutInternal <= (now - pDrvData->pDlr->lastRxTimeP1))) {
            if (pDrvData->pDlr->flags & GOAL_DRV_DLR_FLAGS_LINK_P1) {
                goal_drvDlrNetcEventPush(pDrvData, GOAL_DRV_DLR_EVENT_BEACON_TIMEOUT, 1, GOAL_FALSE);
            }
            if (GOAL_DRV_DLR_BRX_FLAG_BOTH == pDrvData->pDlr->beaconFlags) {
                /* remember last beacon rx port, only clear flag if other port still receives beacons */
                pDrvData->pDlr->beaconFlags &= ~GOAL_DRV_DLR_BRX_FLAG_P1;
            }
            else {
                /* both ports have a beacon timeout, Supervisor is gone */
                pDrvData->pDlr->flags |= GOAL_DRV_DLR_FLAGS_BTO_ALLPORTS;
            }
            pDrvData->pDlr->flags &= ~GOAL_DRV_DLR_FLAGS_BTO_CHECK_P1;
        }
    }

    /* check match count of Filter OF (own frames) */
    resSdk = SWT_RxIPFGetMatchedCount(&pDrvData->swHandle, pDrvData->pDlr->entryIdFilterOf, &matchCount);
    if ((kStatus_Success == resSdk) && (matchCount != pDrvData->pDlr->filterOfMatchCount)) {
        pDrvData->pDlr->filterOfMatchCount = matchCount;
        goal_dlrStateMachineExec(pDrvData->pDlr->pDlr, DLR_EVENT_OWNFRAME, GOAL_ETH_PORT_HOST);
    }

    /* check for new low priority event for the internal DLR state machine */
    res = goal_mboxMsgGet(pDrvData->pDlr->pMboxEventLo, (uint8_t *) &eventMsg);
    if (GOAL_RES_OK(res)) {
        dlrEvent = (GOAL_DRV_DLR_EVENT_T) (eventMsg & GOAL_DRV_DLR_EVENT_MSG_MASK_EVENT);
        port = (eventMsg & GOAL_DRV_DLR_EVENT_MSG_MASK_PORT) >> GOAL_DRV_DLR_EVENT_MSG_SHIFT_PORT;
        goal_drvDlrNetcHandlerExec(pDrvData, dlrEvent, port);
    }
}


/****************************************************************************/
/** Check for high priority events
 *
 * Periodically check for DLR hardware events.
 */
static void goal_drvDlrNetcHiPrioTimerCbExec(
    void *pArg                                  /**< timer argument: private driver data */
)
{
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData;         /* private driver data */
    GOAL_STATUS_T res;                          /* result */
    uint32_t port;                              /* port Id */
    uint32_t eventMsg;                          /* event message */
    GOAL_DRV_DLR_EVENT_T dlrEvent;              /* DLR event */

    pDrvData = (GOAL_DRV_ETH_NETC_DATA_T *) pArg;

    /* check for new high priority event for the internal DLR state machine */
    res = goal_mboxMsgGet(pDrvData->pDlr->pMboxEventHi, (uint8_t *) &eventMsg);
    if (GOAL_RES_OK(res)) {
        dlrEvent = (GOAL_DRV_DLR_EVENT_T) (eventMsg & GOAL_DRV_DLR_EVENT_MSG_MASK_EVENT);
        port = (eventMsg & GOAL_DRV_DLR_EVENT_MSG_MASK_PORT) >> GOAL_DRV_DLR_EVENT_MSG_SHIFT_PORT;
        goal_drvDlrNetcHandlerExec(pDrvData, dlrEvent, port);
    }
}


/****************************************************************************/
/** Poll a Rx BDR and drop frames
 *
 * This function only extracts the Rx port Id. It only works with
 * unchained descriptors.
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvDlrNetcRxBdrPollAndDrop(
    ep_handle_t *pEpHandle,                     /**< endpoint handle */
    uint8_t ring,                               /**< descriptor ring number */
    uint32_t *pRxPort                           /**< [out] RX port id */
)
{
    netc_rx_bdr_t *pRxBdRing;                   /* buffer descriptor ring */
    netc_rx_bd_t *pRxDesc;                      /* RX Descriptor */
    uint16_t descPerFrame;                      /* descriptors per frame */

    if (NULL == pRxPort) {
        return GOAL_ERR_NULL_POINTER;
    }

    pRxBdRing = &pEpHandle->rxBdRing[ring];
    pRxDesc = &pRxBdRing->bdBase[pRxBdRing->index];

    if (0 == pRxDesc->writeback.isReady) {
        return GOAL_ERR_EMPTY;
    }

    *pRxPort = pRxDesc->writeback.srcPort;

    pRxDesc->writeback.isFinal = 0;
    pRxDesc->writeback.isReady = 0;

    if (pRxBdRing->extendDesc) {
        descPerFrame = 2;
    }
    else {
        descPerFrame = 1;
    }

    /* go to next descriptor */
    pRxBdRing->index = (pRxBdRing->index + descPerFrame) % (uint16_t) pRxBdRing->len;

    /* Update the Rx consumer index to free BD whose data is taken away. */
    NETC_SISetRxConsumer(pEpHandle->hw.si, ring, pRxBdRing->index / descPerFrame);

    return GOAL_OK;
}


/****************************************************************************/
/** Enable timeout check mechanism for Beacon frames
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvDlrNetcBeaconTimeoutCheckEnable(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData          /**< private driver data */
)
{
    NETC_TimerGetCurrentTime(&pDrvData->pDlr->timerHdl, &pDrvData->pDlr->lastRxTimeP0);

    pDrvData->pDlr->lastRxTimeP1 = pDrvData->pDlr->lastRxTimeP0;
    pDrvData->pDlr->flags |= GOAL_DRV_DLR_FLAGS_BDR_POLL;

    return GOAL_OK;
}


/****************************************************************************/
/** Push a DLR event to the event queue
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T goal_drvDlrNetcEventPush(
    GOAL_DRV_ETH_NETC_DATA_T *pDrvData,         /**< private driver data */
    GOAL_DRV_DLR_EVENT_T event,                 /**< DLR event */
    uint32_t port,                              /**< sourc port of event */
    GOAL_BOOL_T prioHigh                        /**< high or low priority event */
)
{
    GOAL_STATUS_T res;                          /* result */
    uint32_t eventMsg;                          /* event message */

    eventMsg = (uint32_t) (event & GOAL_DRV_DLR_EVENT_MSG_MASK_EVENT);
    eventMsg |= (port << GOAL_DRV_DLR_EVENT_MSG_SHIFT_PORT) & GOAL_DRV_DLR_EVENT_MSG_MASK_PORT;
    res = goal_mboxMsgPut((GOAL_TRUE == prioHigh) ? pDrvData->pDlr->pMboxEventHi : pDrvData->pDlr->pMboxEventLo, (uint8_t *) &eventMsg);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add %s priority event message", (GOAL_TRUE == prioHigh) ? "high" : "low");
    }

    return res;
}
