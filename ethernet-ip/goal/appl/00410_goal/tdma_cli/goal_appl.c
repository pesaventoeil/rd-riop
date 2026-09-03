/** @file
 *
 * @brief Command Line Interface Example
 *
 * This application basically initializes the Command Line interface (CLI)
 * for TDMA usage.
 *
 * @copyright
 * Copyright 2010-2025.
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

#include "goal_includes.h"
#include <goal_ts.h>
#include <goal_dd.h>
#include <goal_appl.h>
#include "goal_dd_config.h"


/****************************************************************************/
/* Local defines */
/****************************************************************************/
#define MAIN_APPL_IP            GOAL_NET_IPV4(192, 168, 0, 10)
#define MAIN_APPL_NM            GOAL_NET_IPV4(255, 255, 255, 0)
#define MAIN_APPL_GW            GOAL_NET_IPV4(0, 0, 0, 0)
#define MAIN_APPL_UDP_PORT      44444
#define MAIN_APPL_TOS           0xE0

/* TDMA config values */
#define TDMA_CYCLE_TIME         400             /**< TDMA cycle time in us */
#define TDMA_TSLT_NUM             4             /**< number of configured timeslots */
#define TDMA_OFFSET_TS0           0             /**< start of time slot 0 in us */
#define TDMA_OFFSET_TS1         100             /**< start of time slot 1 in us */
#define TDMA_OFFSET_TS2         200             /**< start of time slot 2 in us */
#define TDMA_OFFSET_TS3         300             /**< start of time slot 3 in us */
#define TDMA_QUEUE_0       (1 << 0)             /**< mask for queue 0 */
#define TDMA_QUEUE_1       (1 << 1)             /**< mask for queue 1 */
#define TDMA_QUEUE_2       (1 << 2)             /**< mask for queue 2 */
#define TDMA_QUEUE_3       (1 << 3)             /**< mask for queue 3 */
#define TDMA_QUEUES_TS0 (TDMA_QUEUE_0)          /**< queues allowed to transmit during time slot 0 */
#define TDMA_QUEUES_TS1 (TDMA_QUEUE_0 | TDMA_QUEUE_1) /**< queues allowed to transmit during time slot 1 */
#define TDMA_QUEUES_TS2 (TDMA_QUEUE_0 | TDMA_QUEUE_2) /**< queues allowed to transmit during time slot 2 */
#define TDMA_QUEUES_TS3 (TDMA_QUEUE_0 | TDMA_QUEUE_3) /**< queues allowed to transmit during time slot 3 */
#define TDMA_PORTS (GOAL_ETH_PORT_BIT(0) | GOAL_ETH_PORT_BIT(1)) /**< ports used for TDAM scheduling */
#define TDMA_START_CYCLE_OFFSET 10              /**< number of cycles to wait before starting scheduling */

/* TS config values */
#define GOAL_APPL_PRIO1             246         /**< default value of priority1 */
#define GOAL_APPL_PRIO2             248         /**< default value of priority2 */
#define GOAL_APPL_ANNOUNCEINT_MIN  -124         /**< minimum Announce message interval */
#define GOAL_APPL_ANNOUNCEINT_DEF     0         /**< default Announce message interval */
#define GOAL_APPL_ANNOUNCEINT_MAX   123         /**< maximum Announce message interval */
#define GOAL_APPL_ART_MIN             0         /**< minimum Announce Receipt Timeout */
#define GOAL_APPL_ART_DEF             3         /**< default Announce Receipt Timeout */
#define GOAL_APPL_ART_MAX            10         /**< maximum Announce Receipt Timeout */
#define GOAL_APPL_SYNC_MIN         -124         /**< minimum Sync message interval */
#define GOAL_APPL_SYNC_DEF           -3         /**< default Sync message interval */
#define GOAL_APPL_SYNC_MAX          123         /**< maximum Sync message interval */
#define GOAL_APPL_PDELAYREQ_MIN    -124         /**< minimum PDelay_Req message interval */
#define GOAL_APPL_PDELAYREQ_DEF       0         /**< default PDelay_Req message interval */
#define GOAL_APPL_PDELAYREQ_MAX     123         /**< maximum PDelay_Req message interval */
#define GOAL_APPL_PROFILE_ID {0x00, 0x80, 0xC2, 0x00, 0x01, 0x00} /**< PTP profile ID */

#define GOAL_APPL_UTC_OFFSET         33         /**< offset between UTC and TAI timescales */


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_TS_HANDLE_T *pHdlGts = NULL;        /**< GOAL TS instance handle */
static GOAL_TIMESTAMP_T tsNext;                 /**< next iteration time */
static char vendorName[] = "port GmbH";         /**< manufacturerName of productDescription */
static char modelName[] = "PTP P2P demo";       /**< modelNumber of productDescription */
static char revisionData[] = "1.0.0";           /**< dummy version for Hardware, Firmware and Software */
static uint8_t profileId[6] = GOAL_APPL_PROFILE_ID; /**< PTP profile ID */

static GOAL_STAGE_HANDLER_T stageCmModReg;      /**< CM module register stage */
static GOAL_STAGE_HANDLER_T stageCmModAdd;      /**< CM module add stage */
static GOAL_CM_MODDEF_T cmMod = GOAL_CM_MODDEF(APPL_CM_MOD_ID, "APPL_MODULE"); /**< CM module definition */
/** timeslot start offsets */
static uint32_t tsltStart[TDMA_TSLT_NUM] = {
    TDMA_OFFSET_TS0,
    TDMA_OFFSET_TS1,
    TDMA_OFFSET_TS2,
    TDMA_OFFSET_TS3
};
/** timeslot queue assignments */
static uint32_t tsltQueues[TDMA_TSLT_NUM] = {
    TDMA_QUEUES_TS0,
    TDMA_QUEUES_TS1,
    TDMA_QUEUES_TS2,
    TDMA_QUEUES_TS3
};
static GOAL_NET_CHAN_T *pChanUdp = NULL;        /**< UDP channel */


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/
static GOAL_STATUS_T appl_goalTsCb(
    GOAL_TS_HANDLE_T *pGts,                     /**< GOAL TS handle */
    GOAL_TS_CB_ID_T cbId,                       /**< callback ID */
    void *pCbData                               /**< callback data */
);

static GOAL_STATUS_T appl_setupTs(
    void
);

static GOAL_STATUS_T ddCallback(
    GOAL_DD_T *pHdlDd,                          /**< DD handle */
    GOAL_DD_CB_ID_T cbId,                       /**< callback ID */
    GOAL_DD_CB_DATA_T *pCbData                  /**< callback data */
);

static GOAL_STATUS_T appl_cmModReg(
    void
);

static GOAL_STATUS_T appl_cmModAdd(
    void
);

GOAL_STATUS_T appl_cmChg(
    uint32_t modId,                             /**< module ID */
    uint32_t varId,                             /**< variable Id */
    GOAL_CM_VAR_T *pVar                         /**< variable pointer */
);

static GOAL_STATUS_T appl_qosInit(
    void
);

static GOAL_STATUS_T goal_udpServerCallback(
    void *pArg,                                 /**< callback argument */
    GOAL_NET_CB_TYPE_T cbType,                  /**< callback type */
    GOAL_NET_CHAN_T *pChan,                     /**< channel descriptor */
    GOAL_BUFFER_T *pBuf                         /**< GOAL buffer */
);

static GOAL_STATUS_T goal_applEthCallback(
    GOAL_BUFFER_T **ppBuf,                      /**< Ethernet buffer */
    void *pArg                                  /**< callback argument */
);

static GOAL_STATUS_T goal_tdmaCb(
    void *pArg,                                 /**< user specific argument */
    uint32_t tdmaEventMask                      /**< TMDA event mask */
);

static GOAL_STATUS_T goal_tdmaInit(
    void
);

static GOAL_STATUS_T goal_tdmaStart(
    void
);

/* generate 'GOAL_CM_VARENTRY_T cmVars[]' array that maps the above table */
#include <goal_cm_t.h>
GOAL_CM_VARLIST(cmVars, APPL_CM_VARS);


/****************************************************************************/
/** Register Application Configuration Variables
 *
 * This function is called by GOAL init-stage system to register its
 * configuration management variables.
 *
 * API functions from earlier stages are allowed to be used here.
 */
static GOAL_STATUS_T appl_cmModReg(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* register application variables */
    res = goal_cmRegModule(cmVars);

    return res;
}


/****************************************************************************/
/** Add Application Configuration Module
 *
 * This function is called by GOAL init-stage system to add the application
 * module to configuration mangement.
 *
 * API functions from earlier stages are allowed to be used here.
 */
static GOAL_STATUS_T appl_cmModAdd(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* add application variables */
    res = goal_cmAddModule(&cmMod, cmVars, NULL, NULL, NULL);

    return res;
}


/****************************************************************************/
/** Handle Configuration Variable Changes
 *
 * This function is called by the configuration management when a variable was
 * changed.
 */
GOAL_STATUS_T appl_cmChg(
    uint32_t modId,                             /**< module ID */
    uint32_t varId,                             /**< variable Id */
    GOAL_CM_VAR_T *pVar                         /**< variable pointer */
)
{
    UNUSEDARG(pVar);

#if GOAL_CONFIG_LOGGING == 0
    UNUSEDARG(modId);
    UNUSEDARG(varId);
#endif

    goal_logInfo("variable %" FMT_u32 ":%" FMT_u32 " was changed", modId, varId);

    return GOAL_OK;
}


/****************************************************************************/
/** Application Init
 *
 * This function initializes the DD, CLI, PTP and GOAL.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T appl_init(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* register CM reg stage handler */
    res = goal_mainStageReg(GOAL_STAGE_CM_MOD_REG, &stageCmModReg, GOAL_STAGE_INIT, appl_cmModReg);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to register CM reg stage handler");
        return res;
    }

    /* register CM add stage handler */
    res = goal_mainStageReg(GOAL_STAGE_CM_MOD_ADD, &stageCmModAdd, GOAL_STAGE_INIT, appl_cmModAdd);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to register CM add stage handler");
        return res;
    }

    /* Init DD */
    res = goal_ddInit();
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Initialization of GOAL dd failed");
        return res;
    }

    /* initialize CLI */
    res = goal_cliInit(NULL);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to initialize CLI");
        return res;
    }

    /* register GOAL_TS stack */
    res = goal_tsInit();
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    return res;
}


/****************************************************************************/
/** TDMA scheduler initialization
 *
 * This function is called after GOAL initialization to setup the TDMA
 * scheduler incl. cycle time and an event callback
 */
static GOAL_STATUS_T goal_tdmaInit(
    void
)
{
    GOAL_ETH_TDMA_CONFIG_T config;              /* TDMA config */
    GOAL_ETH_TDMA_TSLT_CFG_T tsltCfg;           /* timeslot configuration */
    GOAL_STATUS_T res;                          /* result */
    uint8_t cnt;                                /* loop counter */

    /* Set TDMA configuration */
    config.tdmaPorts = TDMA_PORTS | GOAL_ETH_PORT_MASK;
    config.tdmaCycleTime = TDMA_CYCLE_TIME;
    config.eventMask = GOAL_ETH_TDMA_EVENT_T1 | GOAL_ETH_TDMA_EVENT_T2 | GOAL_ETH_TDMA_EVENT_T3 | GOAL_ETH_TDMA_EVENT_EOC;
    config.cb = goal_tdmaCb;
    config.pArg = NULL;

    res = goal_ethCmd(GOAL_ETH_CMD_TDMA_CONFIG, GOAL_TRUE, GOAL_ETH_PORT_HOST, &config);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Setting config of TDMA failed");
    }

    for (cnt = 0; (cnt < TDMA_TSLT_NUM) && GOAL_RES_OK(res); cnt++) {
        tsltCfg.timeslotNum = cnt;
        tsltCfg.startOffset = tsltStart[cnt];
        tsltCfg.queueMsk = tsltQueues[cnt];
        tsltCfg.guardBand = 0;

        res = goal_ethCmd(GOAL_ETH_CMD_TDMA_TSLT_CFG, GOAL_TRUE, GOAL_ETH_PORT_HOST, &tsltCfg);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("Setting timeslot %u of TDMA failed", cnt);
        }
    }

    return res;
}


/****************************************************************************/
/** Start TDMA scheduler
 *
 * This function calculates the start time and enables the TDMA scheduler
 */
static GOAL_STATUS_T goal_tdmaStart(
    void
)
{
    GOAL_TS_TIMESTAMP_T ptpTime;                /* PTP timestamp */
    GOAL_STATUS_T res;                          /* result */
    GOAL_ETH_TDMA_ENABLE_T tdmaEnable;          /* TDMA enable settings */
    uint64_t cycleTimeNs;                       /* TDAM cycle time in ns */

    cycleTimeNs = GOAL_TIME_US_TO_NS(TDMA_CYCLE_TIME);

    res = goal_tsTimeGet(pHdlGts, &ptpTime.sec, &ptpTime.nsec);

    /* adjust start time to start of a global cycle */
    tdmaEnable.tdmaStartTime = ptpTime.nsec / cycleTimeNs;
    tdmaEnable.tdmaStartTime = (tdmaEnable.tdmaStartTime + TDMA_START_CYCLE_OFFSET) * cycleTimeNs;
    tdmaEnable.enable = GOAL_TRUE;

    res = goal_ethCmd(GOAL_ETH_CMD_TDMA_ENABLE, GOAL_TRUE, GOAL_ETH_PORT_HOST, &tdmaEnable);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to start TDMA scheduler");
    }

    return res;
}


/****************************************************************************/
/** TDMA Event callback
 *
 * This callback is invoked every time a TDMA event occurs (end of T1, end of
 * T2, end of T3, end of cycle).
 */
static GOAL_STATUS_T goal_tdmaCb(
    void *pArg,                                 /**< user specific argument */
    uint32_t tdmaEventMask                      /**< TMDA event mask */
)
{
    UNUSEDARG(pArg);

    /* End of T1 */
    if (GOAL_ETH_TDMA_EVENT_T1 & tdmaEventMask) {
        /* Process end of T1 here */
    }

    /* End of T2 */
    if (GOAL_ETH_TDMA_EVENT_T2 & tdmaEventMask) {
        /* Process end of T2 here */
    }

    /* End of T3 */
    if (GOAL_ETH_TDMA_EVENT_T3 & tdmaEventMask) {
        /* Process end of T3 here */
    }

    /* End of Cycle */
    if (GOAL_ETH_TDMA_EVENT_EOC & tdmaEventMask) {
        /* Process end of cycle here */
    }

    return GOAL_OK;
}


/****************************************************************************/
/** goal dd callback
 *
 */
static GOAL_STATUS_T ddCallback(
    GOAL_DD_T *pHdlDd,                          /**< DD handle */
    GOAL_DD_CB_ID_T cbId,                       /**< callback ID */
    GOAL_DD_CB_DATA_T *pCbData                  /**< callback data */
)
{
    UNUSEDARG(pCbData);
    UNUSEDARG(pHdlDd);

    switch (cbId) {
        case GOAL_DD_CB_ID_WINK:
            break;
        default:
            break;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** GOAL TS application callback
 *
 * This function is called by the GOAL TS stack to inform the application about
 * special events that might require the application's attention.
 */
static GOAL_STATUS_T appl_goalTsCb(
    GOAL_TS_HANDLE_T *pGts,                     /**< GOAL TS handle */
    GOAL_TS_CB_ID_T cbId,                       /**< callback ID */
    void *pCbData                               /**< callback data */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_TS_TIMESTAMP_T *pTs;                   /* new system time */
    GOAL_TS_CLOCK_ID_T *pClkId;                 /* clock ID of new Grandmaster */
    GOAL_TS_UTC_PROPERTIES_T *pUtcProps;        /* new UTC properties */

    switch (cbId) {
        case GOAL_TS_CB_TIME_UPDATE:
            /* the local clock was updated by the PTP stack */
            pTs = (GOAL_TS_TIMESTAMP_T *) pCbData;
            goal_logInfo("new system time: %"FMT_u64" s %"FMT_u32" ns",
                         pTs->sec, pTs->nsec);
            UNUSEDARG(pTs); /* if logging is disabled */
            break;

        case GOAL_TS_CB_GRANDMASTER_UPDATE:
            /* local clock has new Grandmaster */
            pClkId = (GOAL_TS_CLOCK_ID_T *) pCbData;
            goal_logInfo("new Grandmaster has Id: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",
                         pClkId->octet[0], pClkId->octet[1], pClkId->octet[2], pClkId->octet[3],
                         pClkId->octet[4], pClkId->octet[5], pClkId->octet[6], pClkId->octet[7]);
            UNUSEDARG(pClkId); /* if logging is disabled */

            break;

        case GOAL_TS_CB_GRANDMASTER:
            goal_logInfo("Local clock became Grandmaster");
            break;

        case GOAL_TS_CB_SYNC:
            /* The local clock is synchronized to a master. The application
             * should do all necessary steps to synchronize to the new time and
             * then call goal_tsAppSyncFinished().
             */
            goal_logInfo("clock synced to master");
            goal_tsAppSyncFinished(pGts);
            break;

        case GOAL_TS_CB_UTC_PROP_UPDATE:
            /* a management node requested to change the device's UTC properties,
             * application must determine if the new values are applicable
             */
            pUtcProps = (GOAL_TS_UTC_PROPERTIES_T *) pCbData;
            if ((GOAL_TRUE == pUtcProps->leap61) && (GOAL_TRUE == pUtcProps->leap59)) {
                /* deny this setting */
                res = GOAL_ERROR;
            }
            break;

        case GOAL_TS_CB_TRACABLE_UPDATE:
            /* a management node requested to change the device's traceability
             * properties, application must determine if the new values are
             * applicable
             */
            /* accept all settings */
            goal_logInfo("Traceability properties updated");
            break;

        case GOAL_TS_CB_TIMESCALE_UPDATE:
            /* a management node requested to change the device's time scale
             * properties, application must determine if the new values are
             * applicable
             */
            /* accept all settings */
            goal_logInfo("Timescale properties updated");
            break;

        case GOAL_TS_CB_IN_SYNC:
            goal_logInfo("synchronization successful");
            goal_tdmaStart();
            break;

        case GOAL_TS_CB_OUT_OF_SYNC:
            goal_logInfo("lost synchronization");
            break;
        default:
            goal_logErr("unknown callback ID %d", (int) cbId);
            res = GOAL_ERROR;
            break;
    }

    return res;
}


/****************************************************************************/
/** GOAL TS setup
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T appl_setupTs(
    void
)
{
    GOAL_STATUS_T res;                          /* result */
    const char *pVersion;                       /* GOAL TS version string */

    /* create instance of GOAL TS stack */
    res = goal_tsNew(&pHdlGts, GOAL_TS_INSTANCE_DEFAULT, appl_goalTsCb);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Failed to instantiate GOAL TS stack");
        return res;
    }

    /* configure properties of local clock */
    res = goal_tsLocalClockPropertiesSet(pHdlGts,
                                         GOAL_TS_CLOCK_CLASS_DEFAULT,
                                         GOAL_TS_CLOCK_ACCURACY_25ns,
                                         GOAL_TS_CLOCK_VARIANCE_UNKNOWN,
                                         GOAL_APPL_PRIO1,
                                         GOAL_APPL_PRIO2,
                                         GOAL_TS_DOAMIN_DEFAULT);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Failed to set properties of local clock");
        return res;
    }

    /* initialize time properties, these values are application specific */
    res = goal_tsLocalTimePropertiesSet(pHdlGts,
                                        GOAL_APPL_UTC_OFFSET,
                                        GOAL_TRUE,
                                        GOAL_FALSE,
                                        GOAL_FALSE,
                                        GOAL_FALSE,
                                        GOAL_FALSE,
                                        GOAL_TRUE,
                                        GOAL_TS_TIME_SRC_INTERNAL_OSC);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Failed to set time properties of local clock");
        return res;
    }

    /* set profile specific values */
    res = goal_tsLogAnnounceIntervalSet(pHdlGts,
                                        GOAL_APPL_ANNOUNCEINT_MIN,
                                        GOAL_APPL_ANNOUNCEINT_DEF,
                                        GOAL_APPL_ANNOUNCEINT_MAX);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Failed to set Announce Message interval");
        return res;
    }

    res = goal_tsAnnounceRxTimeoutSet(pHdlGts,
                                      GOAL_APPL_ART_MIN,
                                      GOAL_APPL_ART_DEF,
                                      GOAL_APPL_ART_MAX);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Failed to set Announce Receipt Timeout");
        return res;
    }

    res = goal_tsLogSyncIntervalSet(pHdlGts,
                                    GOAL_APPL_SYNC_MIN,
                                    GOAL_APPL_SYNC_DEF,
                                    GOAL_APPL_SYNC_MAX);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Failed to set Sync Message interval");
        return res;
    }

    res = goal_tsLogPdelayReqIntervalSet(pHdlGts,
                                         GOAL_APPL_PDELAYREQ_MIN,
                                         GOAL_APPL_PDELAYREQ_DEF,
                                         GOAL_APPL_PDELAYREQ_MAX);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Failed to set PDelay_Req Message interval");
        return res;
    }

    res = goal_tsDelayMechSet(pHdlGts, GOAL_TS_DELAY_MECHANISM_P2P);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Failed to set delay mechanism");
        return res;
    }

    res = goal_tsProductDescriptionSet(pHdlGts, vendorName, modelName);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Failed to set productDescription");
        return res;
    }

    /* use same dummy string for Hardware, Firmware and Software revision */
    res = goal_tsRevisionDataSet(pHdlGts, revisionData, revisionData, revisionData);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Failed to set revisionData");
        return res;
    }

    res = goal_tsGptpEnableSet(pHdlGts, GOAL_TRUE);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Failed to enable support for IEEE802.1AS");
        return res;
    }

    goal_tsProfileIdentitySet(pHdlGts, profileId);

    res = goal_tsStart(pHdlGts);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Failed to start GOAL TS instance");
        return res;
    }

    res = goal_tsVersionGet(pHdlGts, &pVersion);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Failed to get GOAL TS version string");
    }
    else {
        goal_logInfo("Initialized GOAL TS stack %s", pVersion);
    }

    /* initialize timestamp */
    tsNext = goal_timerTsGet() + GOAL_TIMER_SEC;

    return GOAL_OK;
}


/****************************************************************************/
/** Initialize QoS
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T appl_qosInit(
    void
)
{
    GOAL_ETH_QOS_PRIO_TYPE_T etherTypeCfg;      /* EtherType priority config */
    GOAL_ETH_QOS_MODE_T qosMode;                /* priority resolution mode settings */
    GOAL_STATUS_T res;                          /* result */

    /* setup TS1 for IPV4 */
    etherTypeCfg.prio = (uint8_t) 2;
    etherTypeCfg.ethType = GOAL_ETH_ETHERTYPE_IPV4;
    res = goal_ethCmd(GOAL_ETH_CMD_QOS_PRIO_TYPE, GOAL_TRUE, GOAL_ETH_PORT_HOST, &etherTypeCfg);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to assign EtherType %x", etherTypeCfg.ethType);
        return res;
    }

    /* all other traffic will be transmitted in TS1 (queue1) */
    qosMode.mode = GOAL_ETH_QOS_MODE_TYPE;
    qosMode.defprio = TDMA_QUEUES_TS1;
    res = goal_ethCmd(GOAL_ETH_CMD_QOS_MODE, GOAL_TRUE, GOAL_ETH_PORT_HOST, &qosMode);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to set QOS mode");
        return res;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Application Setup
 *
 * This function is called by the GOAL init-stage system to open the UDP
 * channel and register the default Ethernet handler.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T appl_setup(
    void
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_NET_ADDR_T addr;                       /* net address */
    uint32_t optVal;                            /* option value */
    uint8_t tosVal;                             /* TOS value */
    uint32_t ip;                                /* ip */
    uint32_t nm;                                /* netmask */
    uint32_t gw;                                /* gateway */
    GOAL_DD_T *pHdlDd;                          /* dd handle */

    res = appl_setupTs();
    if (GOAL_OK != res) {
        goal_logErr("failed to setup GOAL TS");
        return res;
    }

     /* set IP address */
    ip = MAIN_APPL_IP;
    nm = MAIN_APPL_NM;
    gw = MAIN_APPL_GW;
    res = goal_netIpSet(ip, nm, gw, GOAL_FALSE);
    if (GOAL_OK != res) {
        goal_logErr("failed to set IP address");
        return res;
    }

    /* Init DD */
    res = goal_ddNew(&pHdlDd, GOAL_DD_FEAT_ALL);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("error creating dd instance");
        return res;
    }

    res = goal_ddCustomerIdSet(pHdlDd, GOAL_DD_CUSTOMERID);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("error configuring customer id");
        return res;
    }

    res = goal_ddModuleNameSet(pHdlDd, (uint8_t *) GOAL_DD_MODULENAME);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("error configuring module name");
        return res;
    }

    goal_ddCallbackReg(pHdlDd, (GOAL_DD_FUNC_CB_T) ddCallback);

    /* Init TDMA */
    res = goal_tdmaInit();
    if (GOAL_RES_ERR(res)) {
        goal_logErr("error while initializing TDMA scheduler");
        return res;
    }

    /* create NET channel */
    res = goal_netChanCreate(&pChanUdp, GOAL_NET_UDP_SERVER, 0);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create UDP channel");
        return res;
    }

    /* initializing QOS */
    res = appl_qosInit();
    if (GOAL_RES_ERR(res)) {
        goal_logErr("error while initializing TDMA scheduler");
        return res;
    }

    /* register UDP server */
    GOAL_MEMSET(&addr, 0, sizeof(GOAL_NET_ADDR_T));
    addr.localPort = MAIN_APPL_UDP_PORT;
    res = goal_netChanOpen(pChanUdp, &addr, goal_udpServerCallback, NULL);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("error while opening UDP server channel on port %"FMT_u32, (uint32_t) MAIN_APPL_UDP_PORT);
        return res;
    }

    /* set UDP channel to non-blocking */
    optVal = 1;
    res = goal_netChanOptionSet(pChanUdp, GOAL_NET_OPTION_NONBLOCK, &optVal);
    if (GOAL_OK != res) {
        goal_logErr("error while setting UDP channel to non-blocking");
        return res;
    }

    /* activate channel */
    res = goal_netChanActivate(pChanUdp);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("error while enabling UDP channel");
        return res;
    }

    /* Set TOS value for channel */
    tosVal = MAIN_APPL_TOS;
    res = goal_netChanOptionSet(pChanUdp, GOAL_NET_OPTION_TOS, &tosVal);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("error while setting TOS value");
        return res;
    }

    /* Register default Ethernet handler */
    res = goal_ethProtoAddPos(GOAL_TRUE, 0, NULL, &goal_applEthCallback, NULL, GOAL_TRUE);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to register default ETH handler");
        return res;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Ethernet Callback
 *
 * This callback is invoked for every received Ethernet frame. It MUST return
 * an error so that the frame is forwarded to the other network stacks such as
 * the TCP/IP stack.
 */
static GOAL_STATUS_T goal_applEthCallback(
    GOAL_BUFFER_T **ppBuf,                      /**< Ethernet buffer */
    void *pArg                                  /**< callback argument */
)
{
    uint32_t tsNsec;                            /* PPT Timestamp nsec */
    uint64_t tsSec;                             /* PPT Timestamp sec */

    UNUSEDARG(pArg);

    /* Get the timestamp from PTP */
    tsNsec = (*ppBuf)->tsNsec;
    tsSec = (*ppBuf)->tsSec;

    /*
     * Keep the compiler happy. You may replace the next two lines
     * with the processing code or your choice.
     */
    UNUSEDARG(tsNsec);
    UNUSEDARG(tsSec);

    /* Return an error to forward the frame */
    return GOAL_ERR_NODATA;
}


/****************************************************************************/
/** UDP Server Callback
 *
 * Mirror data back to the sender
 */
static GOAL_STATUS_T goal_udpServerCallback(
    void *pArg,                                 /**< callback argument */
    GOAL_NET_CB_TYPE_T cbType,                  /**< callback type */
    GOAL_NET_CHAN_T *pChan,                     /**< channel descriptor */
    GOAL_BUFFER_T *pBuf                         /**< GOAL buffer */
)
{
    UNUSEDARG(pArg);
    UNUSEDARG(cbType);

    /* echo message */
    goal_netChanSend(pChan, pBuf);

    return GOAL_OK;
}
