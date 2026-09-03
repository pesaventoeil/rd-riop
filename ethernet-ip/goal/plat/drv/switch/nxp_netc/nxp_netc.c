/** @file
 *
 * @brief NXP NETC Switch Driver.
 *
 * @copyright
 * Copyright 2022 port GmbH Halle/Saale.
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
#include "nxp_netc_bsp.h"
#include "nxp_netc.h"
#include "nxp_netc_int.h"


#if GOAL_DLR == 1
# include <goal_dlr.h>
#endif

#if GOAL_CONFIG_EPL == 1
# include "goal_epl.h"
#endif


/****************************************************************************/
/* Configuration */
/****************************************************************************/
#ifndef GOAL_CONFIG_SWITCH_NXP_DEBUG
# define GOAL_CONFIG_SWITCH_NXP_DEBUG 0
#endif

#ifndef GOAL_CONFIG_SWITCH_NXP_VERBOSE
# define GOAL_CONFIG_SWITCH_NXP_VERBOSE 0
#endif

#ifndef GOAL_CONFIG_SWITCH_NXP_LOG_FDB_TABLE
# define GOAL_CONFIG_SWITCH_NXP_LOG_FDB_TABLE 0
#endif


/****************************************************************************/
/* Local typedefs */
/****************************************************************************/
typedef struct {
    GOAL_BUFFER_T *pBufNet;
} GOAL_EPL_HANDLE_T;


/****************************************************************************/
/* Constants */
/****************************************************************************/
#if GOAL_DLR == 1 && DLR_CONFIG_BEACON_RINGNODE == 0
#define GOAL_DRV_DLR_MAC_BASE {0x01, 0x21, 0x6C, 0x00, 0x00, 0x00} /**< DLR Destination MAC address base */
#define GOAL_DRV_DLR_MAC_BEACON    0x01         /**< last Byte of DLR Dst MAC of Beacon frames */
#define GOAL_DRV_DLR_MAC_SIGNON    0x02         /**< last Byte of DLR Dst MAC of SignOn & NeighborCheck frames */
#define GOAL_DRV_DLR_MAC_ANNOUNCE  0x03         /**< last Byte of DLR Dst MAC of Announce frames */
#define GOAL_DRV_DLR_MAC_ADVERTISE 0x04         /**< last Byte of DLR Dst MAC of Advertise frames */
#define GOAL_DRV_DLR_MAC_LEARNING  0x05         /**< last Byte of DLR Dst MAC of Learning_Update frames */
#endif

#if GOAL_CONFIG_EPL == 1
#define GOAL_DRV_EPL_MAC_BASE {0x01, 0x11, 0x1e, 0x00, 0x00, 0x00} /**< EPL Destination MAC address base */
#define GOAL_DRV_EPL_MAC_SOC       0x01         /**< last Byte of EPL Dst MAC of Start of Cycle frames */
#define GOAL_DRV_EPL_MAC_PRES      0x02         /**< last Byte of EPL Dst MAC of Poll Resonse frames */
#define GOAL_DRV_EPL_MAC_SOA       0x03         /**< last Byte of EPL Dst MAC of Start of Asynchronous frames */
#define GOAL_DRV_EPL_MAC_ASND      0x04         /**< last Byte of EPL Dst MAC of Asynchrounous frames */
#define GOAL_DRV_EPL_MAC_AMNI      0x05         /**< last Byte of EPL Dst MAC of Active Managing Node frames */
#endif


/****************************************************************************/
/* Table of supported Ethernet commands */
/****************************************************************************/
#define GOAL_ETH_SWITCH_NETC_FUNCS                                                            \
    /* Ethernet command,                              Function name,                 Flags */ \
    GOAL_ETH_FUNC(GOAL_ETH_CMD_PORT_STATE,            netcPortState,              0           ), \
    GOAL_ETH_FUNC(GOAL_ETH_CMD_MACTAB_SET,            netcMacTableEntrySet,       0           ), \
    GOAL_ETH_FUNC(GOAL_ETH_CMD_MACTAB_GET,            netcMacTableEntryGet,       0           ), \
    GOAL_ETH_FUNC(GOAL_ETH_CMD_MACTAB_CLR,            netcMacTableEntryClear,     0           ), \
    GOAL_ETH_FUNC(GOAL_ETH_CMD_EPL,                   netcEpl,                    0           ), \
    GOAL_ETH_FUNC(GOAL_ETH_CMD_SETUP_DONE,            netcSetupDone,              0           )


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
static GOAL_STATUS_T netcPortState(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T netcMacTableEntrySet(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T netcMacTableEntryGet(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T netcMacTableEntryClear(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T convertPortGoalToTgt(
    uint32_t *pPortGoal,                        /**< pointer to port */
    uint32_t *pPortSw                           /**< pointer to port */
);

static GOAL_STATUS_T netcEpl(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T netcSetupDone(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

#if GOAL_CONFIG_SWITCH_NXP_DEBUG == 1
static GOAL_STATUS_T netcDebugFunctions(
    void
);
#endif

#if GOAL_CONFIG_SWITCH_NXP_LOG_FDB_TABLE == 1
void netcLoop(
    void
);
#endif

#if GOAL_DLR == 1  && DLR_CONFIG_BEACON_RINGNODE == 0
static GOAL_STATUS_T dlrInitFDB(
    void
);
#endif


/****************************************************************************/
/* Local variables */
/****************************************************************************/

/**< connect target and board Ethernet functions tables */
#define GOAL_ETH_FUNCS    GOAL_ETH_SWITCH_NETC_FUNCS

#if GOAL_DLR == 1 && DLR_CONFIG_BEACON_RINGNODE == 0
/**< DLR Dst MAC forwarding table */
typedef struct {
    uint8_t dstMacLsb;                          /**< last byte of DLR Destination MAC */
    uint32_t flags;                             /**< forwarding flags */
} GOAL_DRV_DLR_FWD_TABLE_T;

static uint8_t dlrDstMac[MAC_ADDR_LEN] = GOAL_DRV_DLR_MAC_BASE;
static GOAL_DRV_DLR_FWD_TABLE_T dlrFwdTable[] = {
    {GOAL_DRV_DLR_MAC_BEACON,    GOAL_ETH_CMD_PORT_EXT}, /**< Beacon frames are not received */
    {GOAL_DRV_DLR_MAC_SIGNON,    GOAL_ETH_CMD_PORT_HOST}, /**< SignOn frames are Peer-to-Peer */
    {GOAL_DRV_DLR_MAC_ANNOUNCE,  GOAL_ETH_CMD_PORT_HOST | GOAL_ETH_CMD_PORT_EXT}, /**< Announce frame are received and forwarded */
    {GOAL_DRV_DLR_MAC_ADVERTISE, GOAL_ETH_CMD_PORT_EXT}, /**< Advertise frames are not received */
    {GOAL_DRV_DLR_MAC_LEARNING,  GOAL_ETH_CMD_PORT_EXT}, /**< Learning frames are not received */
};

static GOAL_BOOL_T flgDlrInit;                  /**< DLR init flag */
#endif

static GOAL_SW_NETC_INST_T *pInstance;          /**< switch driver instance */

#if GOAL_CONFIG_EPL == 1
# if GOAL_CONFIG_EPL_EXTERN_BUF == 1
static GOAL_EPL_HANDLE_T *pHdlPres;             /**< network buffer */
static GOAL_EPL_HANDLE_T *pHdlIdent;            /**< network buffer */
static GOAL_EPL_HANDLE_T *pHdlStatus;           /**< network buffer */
static GOAL_EPL_HANDLE_T *pHdlUnspec;           /**< network buffer */
# endif
#endif

static uint8_t portEnableMask;                  /**< port mask for switch configuration */
#if GOAL_CONFIG_SWITCH_NXP_DEBUG == 1
static GOAL_STAGE_HANDLER_T stageDebug;         /**< stage handler for Debug mode */
#endif


/****************************************************************************/
/** Register switch driver in Ethernet handlers
 */
GOAL_STATUS_T goal_targetSwitchNXPNetcReg(
    GOAL_DRV_ETH_NETC_PORT_INFO_T *pPortInfo,   /**< port info data table */
    uint32_t numEntries                         /**< entries in port info data table */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    uint8_t switchPortMask = 0;                 /* switch port mask */
    uint32_t cnt;                               /* loop counter */

    /* mask all enabled switch ports */
    for (cnt = 0; cnt < numEntries; cnt++) {
        switch (pPortInfo[cnt].portId) {
            case kNETC_SWITCH0EthPort0:
                switchPortMask |= (1 << kNETC_SWITCH0Port0);
                break;

            case kNETC_SWITCH0EthPort1:
                switchPortMask |= (1 << kNETC_SWITCH0Port1);
                break;

            case kNETC_SWITCH0EthPort2:
                switchPortMask |= (1 << kNETC_SWITCH0Port2);
                break;

            case kNETC_SWITCH0EthPort3:
                switchPortMask |= (1 << kNETC_SWITCH0Port3);
                break;

            case kNETC_ENETC0EthPort:
                /* this config is for the Endpoint only */
                return GOAL_OK;

            default:
                break;
        }
    }

#if GOAL_CONFIG_SWITCH_NXP_DEBUG == 1
    res = goal_mainStageReg(GOAL_STAGE_MODULES, &stageDebug, GOAL_STAGE_INIT, netcDebugFunctions);
#endif

    /* remember enabled external ports and host port */
    portEnableMask = switchPortMask | (1 << kNETC_SWITCH0Port4);

    return res;
}


/****************************************************************************/
/** Open switch driver
 *
 */
GOAL_STATUS_T goal_targetSwitchNXPNetcOpen(
    GOAL_MA_ETH_T *pMaEth,                      /**< ethernet ma handle */
    void *pSwitchHandle,                        /**< bsp switch handle */
    GOAL_ETH_IFACE_T *pIfaceSwitch              /**< interface handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    res = goal_memCalloc(&pInstance, sizeof(GOAL_SW_NETC_INST_T));

    /* store mac address */
    if (GOAL_RES_OK(res)) {
        GOAL_MEMCPY(pInstance->mDevMac, pMaEth->cfg.macAddr, MAC_ADDR_LEN);
    }

    if (GOAL_RES_OK(res)) {
        pInstance->pDrvInst = NULL;
        res = netcOpen(&pInstance->pDrvInst, pSwitchHandle);
    }

#if GOAL_CONFIG_SWITCH_NXP_LOG_FDB_TABLE == 1
    if (GOAL_RES_OK(res)) {
        res = goal_mainLoopReg(netcLoop);
    }
#endif

    if (GOAL_RES_OK(res)) {
        pInstance->pMiInst = (GOAL_MI_ETH_T *) pMaEth->pMiInst;
    }

    if (NULL != pIfaceSwitch) {
        pIfaceSwitch->pCmd = goal_targetSwitchNXPNetcCmdHandler;
    }

#if GOAL_DLR == 1 && DLR_CONFIG_BEACON_RINGNODE == 0
    if (GOAL_RES_OK(res)) {
        if (GOAL_TRUE == flgDlrInit) {
            res = dlrInitFDB();
        }
    }
#endif

    pInstance->portEnableMask = portEnableMask;

    /* enable switching */
    return res;
}


/****************************************************************************/
/** Close switch driver
 *
 */
GOAL_STATUS_T goal_targetSwitchNXPNetcClose(
    GOAL_MA_ETH_T *pMaEth                       /**< ethernet ma handle */
)
{
    UNUSEDARG(pMaEth);

    return GOAL_OK;
}


/****************************************************************************/
/** Process switch function table and execute proper command if available
 *
 */
GOAL_STATUS_T goal_targetSwitchNXPNetcCmdHandler(
    GOAL_ETH_CMD_DATA_T *pCmdData               /**< command handler */
)
{
    GOAL_STATUS_T res;                          /* result */

    res = GOAL_ERR_UNSUPPORTED;

    if (NULL == pInstance) {
        return GOAL_ERR_TARGET_INIT;
    }

    if (NULL == pCmdData) {
        return GOAL_ERR_NULL_POINTER;
    }

    switch (pCmdData->cmd) {
        case GOAL_ETH_CMD_PORT_STATE:
            res = netcPortState(pCmdData);
            break;
        case GOAL_ETH_CMD_MACTAB_SET:
            res = netcMacTableEntrySet(pCmdData);
            break;
        case GOAL_ETH_CMD_MACTAB_GET:
            res = netcMacTableEntryGet(pCmdData);
            break;
        case GOAL_ETH_CMD_MACTAB_CLR:
            res = netcMacTableEntryClear(pCmdData);
            break;
        case GOAL_ETH_CMD_EPL:
            res = netcEpl(pCmdData);
            break;
        case GOAL_ETH_CMD_SETUP_DONE:
            res = netcSetupDone(pCmdData);
            break;
        case GOAL_ETH_CMD_TIMESTAMP:
            /* no additional settings required */
            res = GOAL_OK;
            break;
        default:
            res = GOAL_ERR_UNSUPPORTED;
    }

    return res;
}


/****************************************************************************/
/** Ethernet command handler
 *
 */
static GOAL_STATUS_T netcPortState(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    GOAL_STATUS_T res;                          /* result */
    uint32_t val;                               /* value */
    uint32_t port;                              /* port number */

    res = convertPortGoalToTgt(&pCmd->port, &port);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("port conversion failed");
        return GOAL_ERR_UNSUPPORTED;
    }

    if (pCmd->flgSet) {
#if GOAL_CONFIG_SWITCH_NXP_VERBOSE == 1
        goal_logInfo("setting port state for port %"FMT_u32" to %"FMT_u32, port, *pCmd->data.pStatePort);
#endif
        if ((GOAL_ETH_STATE_DEFAULT == *pCmd->data.pStatePort) ||
            (GOAL_ETH_STATE_UP == *pCmd->data.pStatePort)) {
            val = 0;
        }
        else {
            val = 1;
        }

        /* TODO: enable/disable port */
    }
    else {
        /* TODO: read enable status of port */
        val = GOAL_ETH_STATE_UP;
        *pCmd->data.pStateLink = (val) ? GOAL_ETH_STATE_UP : GOAL_ETH_STATE_DOWN;
    }

    return res;
}


/****************************************************************************/
/** Ethernet command handler
 *
 */
static GOAL_STATUS_T netcMacTableEntrySet(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    uint32_t forwardPorts;                      /* forward ports */

    if (NULL == pInstance  || NULL == pCmd || NULL == pCmd->data.pMacTab) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (pCmd->flgSet) {

#if 0
        /* error if port mask bit is not set */
        if (!(pCmd->data.pMacTab->portmask & GOAL_ETH_PORT_MASK)) {
            goal_logErr("Only port masks are supported.");
            return GOAL_ERROR;
        }

        /* check if not all ports are requested */
        if (!(UINT32_MAX == pCmd->data.pMacTab->portmask)) {
            /* check if port mask has unsupported port bits set */
            if (0 != (pCmd->data.pMacTab->portmask & ~(GOAL_ETH_PORT_HOST |  GOAL_ETH_PORT_BIT(0) | GOAL_ETH_PORT_BIT(1) | GOAL_ETH_PORT_BIT(2) | GOAL_ETH_PORT_BIT(3) | GOAL_ETH_PORT_MASK))) {
                goal_logErr("Invalid ports set.");
                return GOAL_ERROR;
            }
        }
#endif

        forwardPorts = 0;

        if (pCmd->data.pMacTab->portmask & GOAL_SW_NETC_BITMASK_FORWARD_HOST) {
            forwardPorts = GOAL_SW_NETC_BITMASK_FORWARD_HOST; /* forward to host */
        }
        if (GOAL_ETH_PORT(pCmd->data.pMacTab->portmask & GOAL_ETH_PORT_BIT(0))) {
            forwardPorts |= GOAL_SW_NETC_BITMASK_FORWARD_EXT1;
        }
        if (GOAL_ETH_PORT(pCmd->data.pMacTab->portmask & GOAL_ETH_PORT_BIT(1))) {
            forwardPorts |= GOAL_SW_NETC_BITMASK_FORWARD_EXT2;
        }
        if (GOAL_ETH_PORT(pCmd->data.pMacTab->portmask & GOAL_ETH_PORT_BIT(2))) {
            forwardPorts |= GOAL_SW_NETC_BITMASK_FORWARD_EXT3;
        }
        if (GOAL_ETH_PORT(pCmd->data.pMacTab->portmask & GOAL_ETH_PORT_BIT(3))) {
            forwardPorts |= GOAL_SW_NETC_BITMASK_FORWARD_EXT4;
        }

        if (forwardPorts) {
            netcMacTabAdd(pInstance->pDrvInst, forwardPorts, (uint8_t *) pCmd->data.pMacTab->addr);
        }
        else {
            netcMacTabDel(pInstance->pDrvInst, (uint8_t *) pCmd->data.pMacTab->addr);
        }

    }
    else {
        return GOAL_ERR_UNSUPPORTED;
    }

#if GOAL_CONFIG_SWITCH_NXP_VERBOSE == 1
    netcFDBTableDump(pInstance->pDrvInst);
#endif

    return GOAL_OK;
}


/****************************************************************************/
/** Ethernet command handler
 *
 */
static GOAL_STATUS_T netcMacTableEntryGet(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    goal_logInfo("called");

    if (!pCmd->flgSet) {
        /* check if there is a free entry */
        return GOAL_ERR_PARAM;
    }
    else {
        /* set not supported */
        return GOAL_ERR_UNSUPPORTED;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Ethernet command handler
 *
 */
static GOAL_STATUS_T netcMacTableEntryClear(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{

    goal_logInfo("called");

    if (NULL == pInstance) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (pCmd->flgSet) {
        netcMacTabDel(pInstance->pDrvInst, (uint8_t *) pCmd->data.pMacTab->addr);
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Convert GOAL port number to switch port number
 *
 * This function converts the given GOAL port number to the corresponding
 * port number of the switch.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
static GOAL_STATUS_T convertPortGoalToTgt(
    uint32_t * pPortGoal,                       /**< pointer to port */
    uint32_t * pPortSw                          /**< pointer to port */
)
{
    /* check mask pointer */
    if ((NULL == pPortGoal) || (NULL == pPortSw)) {
        goal_logErr("Port mask pointer is null.");
        return GOAL_ERR_NULL_POINTER;
    }

    /* check if management port is demanded */
    if (GOAL_ETH_PORT_HOST == *pPortGoal) {
        /* return CPU port */
        *pPortSw = GOAL_SW_NETC_PORT_HOST;
        return GOAL_OK;
    }

    switch (*pPortGoal) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            *pPortSw = *pPortGoal;
            break;
        default:
             goal_logErr("Invalid port number.");
             return GOAL_ERR_PARAM;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Configure Switch for Ethernet POWERLINK
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T netcEpl(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    GOAL_STATUS_T res;                          /* result */
#if GOAL_CONFIG_EPL == 1
    GOAL_SW_CMD_EPL_T *pSubCmd;                 /* sub command structure */

    if (NULL == pInstance || NULL == pCmd || NULL == pCmd->data.pRaw) {
        return GOAL_ERR_NULL_POINTER;
    }

    pSubCmd = (GOAL_SW_CMD_EPL_T *) (pCmd->data.pRaw);

    switch (pSubCmd->id) {
        case GOAL_SW_CMD_EPL_INIT:
            res = netcEplSetup(pInstance->pDrvInst, pInstance->portEnableMask);
            break;
        case GOAL_SW_CMD_EPL_PATTERN_INIT:
            res = netcEplUpdateNodeId(pInstance->pDrvInst, pSubCmd->data.nodeId);
            break;
        case GOAL_SW_CMD_EPL_PATTERN_UPDATE:
            res = netcEplUpdateNodeId(pInstance->pDrvInst, pSubCmd->data.nodeId);
            break;
        default:
            res = GOAL_ERR_UNSUPPORTED;
            break;
    }
#else
    UNUSEDARG(pCmd);
    res = GOAL_ERR_UNSUPPORTED;
#endif

    return res;
}


/****************************************************************************/
/** Configure Switch when setup is done
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
static GOAL_STATUS_T netcSetupDone(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    UNUSEDARG(pCmd);

    goal_logInfo("GOAL_ETH_CMD_SETUP_DONE");

    return GOAL_OK;
}


#if GOAL_CONFIG_SWITCH_NXP_LOG_FDB_TABLE == 1
/****************************************************************************/
/** Switch loop function
 *
 * @retval GOAL_OK - successful
 * @retval other - failed
 */
void netcLoop(
    void
)
{
    static GOAL_TIMESTAMP_T tsTout;             /* timeout */
    GOAL_TIMESTAMP_T tsTime;                    /* timestamp */

    tsTime = goal_timerTsGet();

    if (tsTout <= tsTime) {
        tsTout = tsTime + 10 * GOAL_TIMER_SEC;

        netcFDBTableDump(pInstance->pDrvInst);
    }
}
#endif



#if GOAL_CONFIG_SWITCH_NXP_DEBUG == 1
/****************************************************************************/
/** stage handler which executes some tests for the switch driver
 */
static GOAL_STATUS_T netcDebugFunctions(
    void
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MA_ETH_T *pMaEth;                      /* ethernet handle */
    GOAL_ETH_CMD_DATA_T data;                   /* command handler data */
    uint8_t mac[] = { 0x02, 0x11, 0x22, 0x33, 0x44, 0x01 }; /* test data */
    GOAL_ETH_MAC_ADDR_T *pMac;                  /* test data */

    /* get ethernet ma handle */
    goal_maEthGetById(&pMaEth, GOAL_ID_DEFAULT);

    /* open switch driver */
    res = goal_targetSwitchNXPNetcOpen(pMaEth);
    if (GOAL_RES_ERR(res)) {
        goal_logInfo("failed to open switch driver");
    }

#if 0
    pMac = (GOAL_ETH_MAC_ADDR_T *) &mac[0];

    res = goal_miEthMcastAdd(pMaEth->pMiInst, GOAL_ETH_PORT_MASK & GOAL_ETH_PORT_BIT(2) & GOAL_ETH_PORT_BIT(1) & GOAL_ETH_PORT_BIT(0), pMac);
    goal_logInfo("res = %s", (GOAL_RES_OK(res)) ? "OK" : "ERROR");

    res = goal_miEthMcastDel(pMaEth->pMiInst, GOAL_ETH_PORT_MASK & GOAL_ETH_PORT_BIT(0), pMac);
    goal_logInfo("res = %s", (GOAL_RES_OK(res)) ? "OK" : "ERROR");
#endif

    netcMacTabAdd(pInstance->pDrvInst, 0x001f, &mac[0]);
    netcFDBTableDump(pInstance->pDrvInst);
    netcMacTabAdd(pInstance->pDrvInst, 0x0001, &mac[0]);
    netcFDBTableDump(pInstance->pDrvInst);
    netcMacTabDel(pInstance->pDrvInst, &mac[0]);
    netcFDBTableDump(pInstance->pDrvInst);

    /* close switch driver */
    res = goal_targetSwitchNXPNetcClose(pMaEth);
    if (GOAL_RES_ERR(res)) {
        goal_logInfo("failed to close switch driver");
    }

    return res;
}
#endif


#if GOAL_DLR == 1 && DLR_CONFIG_BEACON_RINGNODE == 0
/****************************************************************************/
/** Setup device for DLR
 *
 * This function adds the required mac table entries for proper DLR functionality
 */
GOAL_STATUS_T goal_targetDlrInit(
    GOAL_DLR_T *pHdlDlr                         /**< DLR handle */
)
{
    flgDlrInit = GOAL_TRUE;

    UNUSEDARG(pHdlDlr);

    if (NULL != pInstance) {
        return dlrInitFDB();
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Setup device for DLR
 *
 * This function adds the required mac table entries for proper DLR functionality
 */
static GOAL_STATUS_T dlrInitFDB(
    void
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* GOAL result */
    uint32_t cnt;                               /* loop counter */
    uint32_t tblSize;                           /* size of forwarding table */
    uint32_t forwardPorts;                      /* forwarding ports */
    uint32_t portMask;                          /* GOAL port mask */

    if (NULL == pInstance) {
        return GOAL_ERR_TARGET_INIT;
    }

    tblSize = ARRAY_ELEMENTS(dlrFwdTable);

    for (cnt = 0; (cnt < tblSize) && GOAL_RES_OK(res); cnt++) {
        dlrDstMac[5] = dlrFwdTable[cnt].dstMacLsb;

        forwardPorts = 0;

        if (dlrFwdTable[cnt].flags & GOAL_ETH_CMD_PORT_HOST) {
            forwardPorts |= GOAL_SW_NETC_BITMASK_FORWARD_HOST;
        }

        if (dlrFwdTable[cnt].flags & GOAL_ETH_CMD_PORT_EXT) {
            forwardPorts |= GOAL_SW_NETC_BITMASK_FORWARD_EXT;
        }

        portMask = GOAL_ETH_PORT_MASK;;
        if (forwardPorts & GOAL_SW_NETC_BITMASK_FORWARD_HOST) {
            portMask |= GOAL_ETH_PORT_HOST;
        }
        if (forwardPorts & GOAL_SW_NETC_BITMASK_FORWARD_EXT) {
            portMask |= GOAL_ETH_PORT_BIT(0) | GOAL_ETH_PORT_BIT(1) | GOAL_ETH_PORT_BIT(2) | GOAL_ETH_PORT_BIT(3);
        }

        res = goal_ethMcastAdd(portMask,(GOAL_ETH_MAC_ADDR_T *) &dlrDstMac[0]);
    }

#if GOAL_CONFIG_SWITCH_NXP_VERBOSE == 1
        netcFDBTableDump(pInstance->pDrvInst);
#endif

    return res;
}
#endif


#if GOAL_CONFIG_EPL == 1
/****************************************************************************/
/** Update NMT state in EPL driver
 *
 * When the NMT state change, this callback handles switch specific
 * reconfiguration.
 * The MAC queues didn't provide the EPL frames for transmission anytime.
 * After sending, the queue is empty and has to be reloaded. Additionally,
 * updating a flag results in a flush operation causing a short duration of
 * unavailability. Thus, in basic ethernet mode and reduced EPL cycle the
 * frames have to be triggered "manually". Only in full EPL mode, the
 * auto-responses are triggered by pattern.
 */
void goal_tgtEplNmtStateUpdate(
    GOAL_EPL_NODE_STATE_T newState              /**< new NMT state */
)
{
    if (NULL == pInstance) {
        return;
    }

    switch (newState) {
        case GOAL_EPL_NMT_CS_PRE_OPERATIONAL_1:
            /* configuring switch for EPL mode */
# if GOAL_CONFIG_EPL_EXTERN_BUF == 1
            netcEplEnableMode(pInstance->pDrvInst);
# endif
            break;

        case GOAL_EPL_NMT_CS_BASIC_ETHERNET:
            /* configuring switch for basic ehternet mode */
# if GOAL_CONFIG_EPL_EXTERN_BUF == 1
            netcEplDisableMode(pInstance->pDrvInst);
# endif
            break;
        default:
            break;
    }

    goal_logInfo("state: %s",
                 (newState == GOAL_EPL_NMT_CS_NOT_ACTIVE) ? "GOAL_EPL_NMT_CS_NOT_ACTIVE" : \
                 (newState == GOAL_EPL_NMT_GS_OFF) ? "GOAL_EPL_NMT_GS_OFF" : \
                 (newState == GOAL_EPL_NMT_GS_INITIALISING) ? "GOAL_EPL_NMT_GS_INITIALIZING" : \
                 (newState == GOAL_EPL_NMT_GS_RESET_APPLICATION) ? "GOAL_EPL_NMT_GS_RESET_APPLICATION" : \
                 (newState == GOAL_EPL_NMT_GS_RESET_COMMUNICATION) ? "GOAL_EPL_NMT_GS_RESET_COMMUNICATION" : \
                 (newState == GOAL_EPL_NMT_GS_RESET_CONFIGURATION) ? "GOAL_EPL_NMT_GS_RESET_CONFIGURATION" : \
                 (newState == GOAL_EPL_NMT_CS_PRE_OPERATIONAL_1) ? "GOAL_EPL_NMT_CS_PRE_OPERATIONAL_1" : \
                 (newState == GOAL_EPL_NMT_CS_PRE_OPERATIONAL_2) ? "GOAL_EPL_NMT_CS_PRE_OPERATIONAL_2" : \
                 (newState == GOAL_EPL_NMT_CS_READY_TO_OPERATE) ? "GOAL_EPL_NMT_CS_READY_TO_OPERATE" : \
                 (newState == GOAL_EPL_NMT_CS_OPERATIONAL) ? "GOAL_EPL_NMT_CS_OPERATIONAL" : \
                 (newState == GOAL_EPL_NMT_CS_STOPPED) ? "GOAL_EPL_NMT_CS_STOPPED" : \
                 (newState == GOAL_EPL_NMT_CS_BASIC_ETHERNET) ? "GOAL_EPL_NMT_CS_BASIC_ETHERNET" : \
                 (newState == GOAL_EPL_NMT_UNKNOWN) ? "GOAL_EPL_NMT_UNKNOWN" : "unknown");
}

# if GOAL_CONFIG_EPL_EXTERN_BUF == 1
/****************************************************************************/
/** Enable/Disable automatic reply in general
 *
 * @returns GOAL_STATUS_T result
 */
void goal_tgtEplMacAutoReplyEnable(
    GOAL_BOOL_T flgEnable                       /**< enable flag */
)
{
    if (NULL == pInstance) {
        return;
    }

    netcEplAutoreplySet(pInstance->pDrvInst, flgEnable);
}


/****************************************************************************/
/** Enable/Disable automatic reply for Pres frames
 *
 * @returns GOAL_STATUS_T result
 */
void goal_tgtEplMacPresAutoReplyEnable(
    GOAL_BOOL_T flgEnable                       /**< enable flag */
)
{
    if (NULL == pInstance) {
        return;
    }

    netcEplPresAutoreplySet(pInstance->pDrvInst, flgEnable);
}


/****************************************************************************/
/** Initialization for auto reply
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_targetEplInitIsr(
    void
)
{
    return GOAL_OK;
}


/****************************************************************************/
/** Reset all unspec buffers for reuse
 *
 * @returns GOAL_STATUS_T result
 */
void goal_tgtEplUnspecReset(
    void
)
{
    /* TODO */
}
# endif


/****************************************************************************/
/** Initialize Ethernet Transmission Buffer
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_tgtEplTxHdlGet(
    void **ppHdl,                               /**< [out] handle reference */
    uint8_t msgType,                            /**< message type */
    uint8_t requestedServiceID,                 /**< RequestedServiceID */
    uint16_t dataLen                            /**< buffer data length */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_EPL_HANDLE_T *pHdlTx;                  /* handle */

    UNUSEDARG(msgType);
    UNUSEDARG(requestedServiceID);

    goal_logInfo("msgType: %d, requestedServiceId: %d", msgType, requestedServiceID);

    /* allocate a handle */
    res = goal_memCalloc(&pHdlTx, sizeof(GOAL_EPL_HANDLE_T));
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Failed to allocate a EPL transmission handle");
        return res;
    }

    /* get a network buffer */
    res = goal_ethGetNetBuf((GOAL_BUFFER_T **) &pHdlTx->pBufNet);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Failed to allocate a EPL poll response buffer");
        return res;
    }

    /* verifying buffer size */
    if (dataLen > pHdlTx->pBufNet->bufSize) {
        return GOAL_ERR_PARAM;
    }

    /* set the length of the buffer */
    pHdlTx->pBufNet->dataLen = dataLen;

    /* return buffer */
    *ppHdl = pHdlTx;

# if GOAL_CONFIG_EPL_EXTERN_BUF == 1
    if (msgType == EPLMSG_TYPE_PREQ) {
        goal_queueFlagsSet(pHdlTx->pBufNet, GOAL_QUEUE_FLG_KEEP_CONTENT | GOAL_QUEUE_FLG_NO_RELEASE);
        pHdlPres = pHdlTx;
    }

    if (msgType == EPLMSG_TYPE_SOA) {
        switch (requestedServiceID) {
        case EPLMSG_ASND_IDENT:
            goal_queueFlagsSet(pHdlTx->pBufNet, GOAL_QUEUE_FLG_KEEP_CONTENT | GOAL_QUEUE_FLG_NO_RELEASE);
            pHdlIdent = pHdlTx;
            break;
        case EPLMSG_ASND_STATE:
            goal_queueFlagsSet(pHdlTx->pBufNet, GOAL_QUEUE_FLG_KEEP_CONTENT | GOAL_QUEUE_FLG_NO_RELEASE);
            pHdlStatus = pHdlTx;
            break;
        case ASYNC_UNSPEC_REQ:
            goal_queueFlagsSet(pHdlTx->pBufNet, GOAL_QUEUE_FLG_KEEP_CONTENT | GOAL_QUEUE_FLG_NO_RELEASE);
            pHdlUnspec = pHdlTx;
            break;
        default:
            break;
        }
    }
# endif

    return res;
}


/****************************************************************************/
/** prepare an Ethernet Transmission Buffer
 */
void goal_tgtEplBufPrepare(
    void *pHdl,                                 /**< handle */
    void *pBuf,                                 /**< buffer pointer */
    int lenData                                 /**< buffer length */
)
{
    GOAL_EPL_HANDLE_T *pHdlTx;                  /* transmission handle */

    /* validation of null pointer and enabled HUB */
    if (NULL == pHdl) {
        return;
    }

    /* cast the handle type */
    pHdlTx = (GOAL_EPL_HANDLE_T *) pHdl;

    /* verifying buffer size */
    if (lenData > pHdlTx->pBufNet->bufSize) {
        return;
    }

    /* copy EPL buffer to GOAL TX buffer */
    GOAL_MEMCPY(pHdlTx->pBufNet->ptrData, pBuf, (unsigned int) lenData);

    /* set frame length (not copied length) */
    pHdlTx->pBufNet->dataLen = (uint16_t) lenData;
}


/****************************************************************************/
/** send an Ethernet Transmission Buffer
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_tgtEplBufSend(
    void *pHdl                                  /**< handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_EPL_HANDLE_T *pHdlTx;                  /* network buffer */

    /* Null pointer validation */
    if (NULL == pHdl) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* cast the handle type */
    pHdlTx = (GOAL_EPL_HANDLE_T *) pHdl;

# if GOAL_CONFIG_EPL_EXTERN_BUF == 1
    if (NULL == pInstance) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* update buffer to send buffer in hardware mode */
    if ((pHdl == pHdlPres)) {
        netcEplPresUpdate(pInstance->pDrvInst, pHdlTx->pBufNet);
    }
    else if (pHdl == pHdlIdent) {
        netcEplIdentUpdate(pInstance->pDrvInst, pHdlTx->pBufNet);
    }
    else if (pHdl == pHdlStatus) {
        netcEplStatusUpdate(pInstance->pDrvInst, pHdlTx->pBufNet);
    }
    else if (pHdl == pHdlUnspec) {
        netcEplUnspecUpdate(pInstance->pDrvInst, pHdlTx->pBufNet);
    }

    if (allowBasicEthernet) {
# endif
        /* set port, frames are sent on all "ports" */
        pHdlTx->pBufNet->netPort = GOAL_ETH_PORT_HOST;

        /* send frame for software mode - the buffer will be released */

        /* send GOAL TX buffers */
        res = goal_ethSend((void **) &pHdlTx->pBufNet, GOAL_NET_TX_RT);
        if (GOAL_RES_ERR(res)) {
            goal_logWarn("TX buffer dropped");
        }

        /* get a new net buffer for the next frame */
        res = goal_ethGetNetBuf(&pHdlTx->pBufNet);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to get EPL TX buffer");
        }

# if GOAL_CONFIG_EPL_EXTERN_BUF == 1
    }
# endif
    return res;
}

#endif /* GOAL_CONFIG_EPL */
