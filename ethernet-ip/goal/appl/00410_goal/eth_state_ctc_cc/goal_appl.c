/** @file
 *
 * @brief Ethernet Statistics Example
 *
 * This application polls all Ethernet statistics every 4 seconds and
 * transmits all the values to AC on change.
 *
 * Note:    This demo application is meant to demonstrate the basic
 *          handling of this feature.
 *          It is not meant for performance benchmarking and it is
 *          not optimized for performance evaluation purposes.
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

#include "goal_includes.h"
#include <goal_media/goal_mi_mctc.h>


/****************************************************************************/
/* Local defines */
/****************************************************************************/
#define GOAL_APPL_ETH_STATS_TIMER_PERIOD        (4 * GOAL_TIMER_SEC)
#define GOAL_APPL_RPC_STACK_SIZE 250            /**< byte size of RPC stack */
#define GOAL_APPL_RPC_FCT_STATS (0)             /**< user function ID */
#define GOAL_APPL_CNT_STATS 2                   /**< number of observed states */


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static uint64_t **ppValShadow;                  /**< shadow value storage */
static uint32_t *pLinkStates;                   /**< port link states */
static uint32_t *pLinkSpeeds;                   /**< port speeds states */
static uint32_t *pObsState;                     /**< list of observed states */
static uint32_t numPorts = 0;                   /**< count of ports */
static GOAL_RPC_HDL_CHN_T *pHdlRpcChn = GOAL_RPC_HDL_NONE; /**< RPC channel handle */


/****************************************************************************/
/* Local Prototypes */
/****************************************************************************/
static void appl_timerEthStatsCb(
    void *pArg                                  /**< timer argument */
);

GOAL_STATUS_T appl_init(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* get port count */
    res = goal_ethPortsGet(&numPorts);

    /* configure the RPC stack size */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcSetStackMax(GOAL_APPL_RPC_STACK_SIZE);
    }

    if (GOAL_RES_ERR(res)) {
        goal_logErr("error on application initialization");
    }

    return res;
}


/****************************************************************************/
/** Application Setup
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T appl_setup(
    void
)
{
    unsigned int cnt;                           /* counter */
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_TIMER_T *pTimerEthStats = NULL;        /* eth stats timer */
    GOAL_STAT_LIST_T *pListStat;                /* statistics list */
    GOAL_STAT_ENTRY_T *pEntry;                  /* statistics entry */
    GOAL_STAT_T *pStat;                         /* statistics data */

    /* allocate memory for per-port link state shadow */
    if (GOAL_RES_OK(res)) {
        res = goal_memCalloc(&pLinkStates, numPorts * sizeof(uint32_t));
    }

    /* allocate memory for per-port link speed shadow */
    if (GOAL_RES_OK(res)) {
        res = goal_memCalloc(&pLinkSpeeds, numPorts * sizeof(uint32_t));
    }

    /* allocate memory for list of observed stats */
    if (GOAL_RES_OK(res)) {
        res = goal_memCalloc(&pObsState, GOAL_APPL_CNT_STATS * sizeof(uint32_t));
    }

    /* assign the observed ETH stats */
    pObsState[0] = GOAL_STAT_ID_ETH_IFOUTOCTETS;
    pObsState[1] = GOAL_STAT_ID_ETH_IFINOCTETS;

    /* create eth stats timer */
    res = goal_timerCreate(&pTimerEthStats, GOAL_TIMER_LOW);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("error during eth stats timer creation");
        return res;
    }

    if (GOAL_RES_OK(res)) {
        /* setup a channel for RPC init */
        res = goal_rpcSetupChannel(&pHdlRpcChn, GOAL_ID_MI_CTC_DEFAULT);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("Unable to setup RPC channel.");
            return res;
        }
    }

    /* setup and start eth stats timer */
    res = goal_timerSetup(pTimerEthStats, GOAL_TIMER_PERIODIC, GOAL_APPL_ETH_STATS_TIMER_PERIOD, appl_timerEthStatsCb, NULL, GOAL_TRUE);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("error while starting eth stats timer");
        return res;
    }


    /* reset all eth statistics */
    GOAL_LL_FOREACH(gpGoalStatList, pListStat) {

        /* skip non-Ethernet statistics */
        if (GOAL_ID_ETH != pListStat->id) {
            continue;
        }

        /* iterate through Ethernet statistics entries */
        GOAL_LL_FOREACH(pListStat->pListEntry, pEntry) {

            /* count Ethernet statistics */
            GOAL_LL_FOREACH(pEntry->pListStat, pStat) {

                /* reset statistics */
                goal_statReset(NULL, pStat);
            }
        }
    }

    goal_logInfo("observe %u statistics counters", GOAL_APPL_CNT_STATS);

    /* allocate shadow counters */
    res = goal_memCalloc(&ppValShadow, sizeof(uint64_t *) * GOAL_APPL_CNT_STATS);
    for (cnt = 0; GOAL_RES_OK(res) && (cnt < GOAL_APPL_CNT_STATS); cnt++) {
        res = goal_memCalloc(&ppValShadow[cnt], sizeof(uint64_t) * (numPorts + 1));
    }

    return res;
}


/****************************************************************************/
/** Application Loop
 */
static void appl_timerEthStatsCb(
    void *pArg                                  /**< timer argument */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    uint64_t val;                               /* value */
    uint64_t val64_be;                          /* value in big endianness */
    uint32_t port;                              /* Ethernet port */
    uint32_t linkState;                         /* link state */
    uint32_t linkSpeed;                         /* link speed */
    GOAL_ETH_PORT_T portGoal;                   /* GOAL port */
    int cntVal;                                 /* value position */
    GOAL_BOOL_T flgUpdate = GOAL_FALSE;         /* update flag */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */
    const char *strName;                        /* statistics name */

    UNUSEDARG(pArg);

    /* iterate through external port counters */
    for (port = 0, flgUpdate = GOAL_FALSE; port <= numPorts; port++) {
        if (numPorts != port) {
            portGoal = port;

            /* get the link state */
            linkState = 0;
            res = goal_ethLinkStateGet(port, &linkState);
            if (GOAL_RES_ERR(res)) {
                goal_logErr("Unable to get link state of port %"FMT_u32, port);
                return;
            }

            /* get the link speed */
            linkSpeed = 0;
            res = goal_ethLinkSpeedGet(port, &linkSpeed);
            if (GOAL_RES_ERR(res)) {
                goal_logErr("Unable to get link speed of port %"FMT_u32, port);
                return;
            }

            /* check for changes */
            if ((linkState != pLinkStates[port]) || (linkSpeed != pLinkSpeeds[port])) {
                pLinkStates[port] = linkState;
                pLinkSpeeds[port] = linkSpeed;
                goal_logDbg("Link stat for port %"FMT_u32" changed to %"FMT_u32" and speed %"FMT_u32" Mbit/s", port, linkState, linkSpeed);
                flgUpdate = GOAL_TRUE;
            }
        } else {
            portGoal = GOAL_ETH_PORT_HOST;
        }

        for (cntVal = 0; GOAL_APPL_CNT_STATS > cntVal; cntVal++) {
            /* get stat */
            res = goal_statValGetById(&val, GOAL_ID_ETH, pObsState[cntVal], portGoal);
            if (GOAL_RES_ERR(res)) {
                goal_logErr("Unable to get stat %"FMT_u32" of port %"FMT_u32, pObsState[cntVal], portGoal);
                return;
            }

            /* compare value to shadow */
            if (ppValShadow[cntVal][port] == val) {
                continue;
            }

            /* clear name string */
            strName = NULL;
#if GOAL_CONFIG_STATS_NAMES == 1
            goal_statNameGetById(&strName, pObsState[cntVal], portGoal);
#endif
            if (NULL == strName) {
                if (GOAL_ETH_PORT_HOST == portGoal) {
                    goal_logDbg("port host, stat %"FMT_u32": %"FMT_u64, pObsState[cntVal], val);
                } else {
                    goal_logDbg("port %"FMT_u32", stat %"FMT_u32": %"FMT_u64, portGoal, pObsState[cntVal], val);
                }
            } else {
                if (GOAL_ETH_PORT_HOST == portGoal) {
                    goal_logDbg("port host, %s: %"FMT_u64, strName, val);
                } else {
                    goal_logDbg("port %"FMT_u32", %s: %"FMT_u64, portGoal, strName, val);
                }
            }

            /* set update flag */
            flgUpdate = GOAL_TRUE;

            /* update shadow */
            ppValShadow[cntVal][port] = val;
        }
    }

    /* send RPC request if a stat has changed */
    if (GOAL_TRUE == flgUpdate) {
        /* divider line */
        goal_logDbg("-------------------------------------------------------");

        /* wait till RPC is ready */
        if (GOAL_RES_ERR(goal_rpcStatus(GOAL_ID_MI_CTC_DEFAULT))) {
            return;
        }

        /* get a new rpc handle */
        GOAL_RPC_NEW();

        for (port = 0; GOAL_RES_OK(res) && (port <= numPorts); port++) {
            /* transform the stats to big endianness and push it to RPC stack */
            for (cntVal = 0; ((GOAL_RES_OK(res)) && GOAL_APPL_CNT_STATS > cntVal); cntVal++) {
                if (GOAL_RES_OK(res)) {
                    val64_be = GOAL_htobe64(ppValShadow[cntVal][port]);
                    res = goal_rpcArgPush(pHdlRpc, (uint8_t *) &val64_be, sizeof(uint64_t));
                    if (GOAL_RES_ERR(res)) {
                        goal_logDbg("RPC push result: 0x%"FMT_x32, res);
                    }
                }
            }

            if (numPorts != port) {
                /* push the link speed */
                GOAL_RPC_PUSH(pLinkSpeeds[port]);

                /* push the link state */
                GOAL_RPC_PUSH(pLinkStates[port]);
            }
            /* push the port ID */
            GOAL_RPC_PUSH(port);
        }
        /* push the number of ports */
        GOAL_RPC_PUSH(numPorts);

        /* call the procedure at the server */
        GOAL_RPC_USER_CALL(GOAL_APPL_RPC_FCT_STATS);

        GOAL_RPC_CLOSE();
    }
}
