/** @file
 *
 * @brief application template
 *
 * This module receives the Ethernet statistics of the CC ports and shows them.
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
/* Defines */
/****************************************************************************/
#define GOAL_APPL_RPC_STACK_SIZE 250            /**< byte size of RPC stack */
#define GOAL_APPL_RPC_FCT_STATS (0)             /**< user function ID */
#define GOAL_APPL_PORTS_MAX 4                   /**< maximal number of ports */


/****************************************************************************/
/* Local struct */
/****************************************************************************/
typedef struct {
    uint32_t port;                              /**< number of port */
    uint64_t outOctets;                         /**< valid outgoing frames */
    uint64_t inOctets;                          /**< valid incoming frames */
    uint32_t linkState;                         /**< port link state */
    uint32_t linkSpeed;                         /**< port speeds state */
} GOAL_APPL_ETH_STATS_T;


/****************************************************************************/
/* Local variables */
/****************************************************************************/
GOAL_APPL_ETH_STATS_T *pEthPeerStats;           /**< eth states of peer core port */


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/
static void goal_applUserFct(
    uint32_t numPorts,                          /**< number of ports */
    GOAL_APPL_ETH_STATS_T *pEthStats            /**< eth states */
);

static GOAL_STATUS_T goal_rpcUserFunctionServer(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< RPC handle */
);


/****************************************************************************/
/** Application User Function
 *
 * This function logs the stats of a selected port.
 */
static void goal_applUserFct(
    uint32_t numPorts,                          /**< number of ports */
    GOAL_APPL_ETH_STATS_T *pEthStats            /**< eth states */
)
{
    /* show link state and load of selected port */
    goal_logInfo("============== Port %"FMT_u32" ===============", pEthStats->port);
    /* Link state not provided for host port */
    if (pEthStats->port != numPorts) {
        goal_logInfo("Link state port %"FMT_u32": %s", pEthStats->port, (GOAL_ETH_STATE_UP == (pEthStats->linkState)) ? "up" : "down");
        if (GOAL_ETH_STATE_UP == (pEthStats->linkState)) {
            goal_logInfo("Link speed port %"FMT_u32": %"FMT_u32" Mbit/s", pEthStats->port, pEthStats->linkSpeed);
        }
    } else {
        goal_logInfo("Link host port: up");
    }

    goal_logInfo("Rx load %"FMT_u64" bytes/s", pEthStats->inOctets);
    goal_logInfo("Tx load %"FMT_u64" bytes/s", pEthStats->outOctets);
}


/****************************************************************************/
/** RPC User Function
 *
 * This function will be called, once a RPC message is received.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_rpcUserFunctionServer(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< RPC handle */
)
{
    uint64_t val64_be;                          /* value in big endianness */
    uint32_t numPorts = 0;                      /* number of ports */
    uint32_t cntPorts = 0;                      /* counter of ports */
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    /* pop the port ID */
    GOAL_RPC_POP(numPorts, uint32_t);

    /* check the number of ports. consider the host ID */
    if (GOAL_RES_OK(res)) {
        if ((GOAL_APPL_PORTS_MAX + 1) <= numPorts) {
            goal_logErr("received invalid number of ports %"FMT_u32, numPorts);
            return GOAL_ERR_OUT_OF_RANGE;
        }
    }

    /* pop the data of the ports - consider host port */
    for (cntPorts = 0; GOAL_RES_OK(res) && (cntPorts <= numPorts); cntPorts++) {
        GOAL_MEMSET(pEthPeerStats, 0, sizeof(GOAL_APPL_ETH_STATS_T));
        /* pop the port ID */
        GOAL_RPC_POP(pEthPeerStats->port, uint32_t);

        /* check the number of ports. consider the host ID */
        if (GOAL_RES_OK(res)) {
            /* host port has no link state and speed */
            if (pEthPeerStats->port != numPorts) {
                /* poop the link state */
                GOAL_RPC_POP(pEthPeerStats->linkState, uint32_t);

                /* poop the link speed */
                GOAL_RPC_POP(pEthPeerStats->linkSpeed, uint32_t);
            }
        }

        /* pop the incoming octets and transform them into host format */
        if (GOAL_RES_OK(res)) {
            res = goal_rpcArgPop(pHdlRpc, (uint8_t *) &val64_be, sizeof(uint64_t)); \
            if (GOAL_RES_ERR(res)) {
                goal_logDbg("RPC pop result: 0x%"FMT_x32, res);
                return res;
            }
            pEthPeerStats->inOctets = GOAL_be64toh(val64_be);
        }

        /* pop the outcomming octets and transform them into host format */
        if (GOAL_RES_OK(res)) {
            res = goal_rpcArgPop(pHdlRpc, (uint8_t *) &val64_be, sizeof(uint64_t)); \
            if (GOAL_RES_ERR(res)) {
                goal_logDbg("RPC pop result: 0x%"FMT_x32, res);
                return res;
            }
            pEthPeerStats->outOctets = GOAL_be64toh(val64_be);
        }

        if (GOAL_RES_OK(res)) {
            goal_applUserFct(numPorts, pEthPeerStats);
        }
    }


    return res;
}


/****************************************************************************/
/** Application initialization
 *
 * @result GOAL_STATUS_T result
 */
GOAL_STATUS_T appl_init(
    void
)
{
    /* configure the RPC stack size */
    return goal_rpcSetStackMax(GOAL_APPL_RPC_STACK_SIZE);
}


/****************************************************************************/
/** Application Setup
 *
 * @result GOAL_STATUS_T result
 */
GOAL_STATUS_T appl_setup(
    void
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    /* setup the service function */
    GOAL_RPC_USER_REGISTER_SERVICE(GOAL_APPL_RPC_FCT_STATS, &goal_rpcUserFunctionServer);

    if (GOAL_RES_OK(res)) {
        res = goal_memAlloc(&pEthPeerStats, sizeof(GOAL_APPL_ETH_STATS_T));
    }

    return res;
}
