/** @file
 *
 * @brief Simple TCP Echo Server
 *
 * Note:    This demo application is meant to demonstrate the basic
 *          handling of this feature.
 *          It is not meant for performance benchmarking and it is
 *          not optimized for performance evaluation purposes.
 *
 * @copyright
 * Copyright 2010-2017.
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


/****************************************************************************/
/* Local defines */
/****************************************************************************/
#define MAIN_APPL_TCP_PORT      1234
#define GOAL_APPL_NUM_TCP_DATA_CHANNELS       3 /**< number of simultaneously active connections of TCP server */
#define MAIN_APPL_IP            GOAL_NET_IPV4(192, 168, 0, 10)
#define MAIN_APPL_NM            GOAL_NET_IPV4(255, 255, 255, 0)
#define MAIN_APPL_GW            GOAL_NET_IPV4(0, 0, 0, 0)


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/
static GOAL_STATUS_T tcpCallback(
    void *pArg,                                 /**< callback argument */
    GOAL_NET_CB_TYPE_T cbType,                  /**< callback type */
    struct GOAL_NET_CHAN_T *pChan,              /**< channel descriptor */
    struct GOAL_BUFFER_T *pBuf                  /**< GOAL buffer */
);

static void tcpRestartTimerCb(
    void *pArg                                  /**< timer argument */
);

static void tcpCloseTimerCb(
    void *pArg                                  /**< timer argument */
);


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_NET_CHAN_T *pListenerChan = NULL;   /**< TCP Listener channel */
static GOAL_NET_ADDR_T addr;                    /**< net address */
static GOAL_TIMER_T *pTmr = NULL;               /**< listener restart timer */


/****************************************************************************/
/** Application Setup
 *
 * This function is called by the GOAL init-stage system to open TCP channels.
 *
 * API functions from earlier stages are allowed to be used here.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T appl_setup(
    void
)
{
    GOAL_STATUS_T res;                          /* result */
    uint32_t optVal;                            /* option value */
    uint32_t ip;                                /* IP address */
    uint32_t nm;                                /* netmask */
    uint32_t gw;                                /* gateway */

    /* set IP address */
    ip = MAIN_APPL_IP;
    nm = MAIN_APPL_NM;
    gw = MAIN_APPL_GW;
    res = goal_netIpSet(ip, nm, gw, GOAL_FALSE);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Set IP failed");
        return res;
    }

    /* register TCP server */
    GOAL_MEMSET(&addr, 0, sizeof(GOAL_NET_ADDR_T));
    addr.localPort = MAIN_APPL_TCP_PORT;

    res = goal_netChanCreate(&pListenerChan, GOAL_NET_TCP_LISTENER, GOAL_APPL_NUM_TCP_DATA_CHANNELS);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("error while creating TCP server channel");
        return res;
    }

    res = goal_netChanOpen(pListenerChan, &addr, tcpCallback, NULL);
    if (GOAL_OK != res) {
        goal_logErr("error while opening TCP server channel on port %"FMT_u32, (uint32_t) MAIN_APPL_TCP_PORT);
        return res;
    }

    /* set TCP channel to non-blocking */
    optVal = 1;
    res = goal_netChanOptionSet(pListenerChan, GOAL_NET_OPTION_NONBLOCK, &optVal);
    if (GOAL_OK != res) {
        goal_logErr("error while setting TCP channel to non-blocking");
        return res;
    }

    res = goal_timerCreate(&pTmr, GOAL_TIMER_LOW);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("error while creating Timer");
        return res;
    }

    /* greet */
    goal_logInfo("waiting for TCP connections on port %u", MAIN_APPL_TCP_PORT);

    return GOAL_OK;
}


/****************************************************************************/
/** TCP Server Callback
 *
 * Print a hexdump of the received data.
 */
static GOAL_STATUS_T tcpCallback(
    void *pArg,                                 /**< callback argument */
    GOAL_NET_CB_TYPE_T cbType,                  /**< callback type */
    struct GOAL_NET_CHAN_T *pChan,              /**< channel descriptor */
    struct GOAL_BUFFER_T *pBuf                  /**< GOAL buffer */
)
{
    GOAL_NET_ADDR_T remote;                     /* remote address */
    GOAL_STATUS_T res;                          /* result */

    UNUSEDARG(pArg);

    if (cbType == GOAL_NET_CB_NEW_SOCKET) {
        goal_logInfo("new TCP socket from TCPListener: %p", (void *) pChan);
    }
    else if (cbType == GOAL_NET_CB_NEW_DATA) {
        goal_logInfo("Data received on tcp socket %p", (void *) pChan);

        /* get IP Address of remote node */
        res = goal_netChanRemoteAddrGet(pChan, &remote);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("Failed to get Remote Address for socket %p", (void *) pChan);
            return res;
        }

        goal_logInfo("sender 0x%08"FMT_x32":%u", remote.remoteIp, remote.remotePort);

        /* echo message */
        goal_netChanSend(pChan, pBuf);

        /* strip linefeeds and carriage returns */
        while (pBuf->dataLen) {
            if (('\r' == pBuf->ptrData[pBuf->dataLen - 1])
                || ('\n' == pBuf->ptrData[pBuf->dataLen - 1])) {
                pBuf->dataLen--;
                continue;
            }

            pBuf->ptrData[pBuf->dataLen] = 0;
            break;
        }

        /* log message */
        goal_logInfo("TCP message: '%s'", pBuf->ptrData);

        if (GOAL_CMP_EQUAL == GOAL_STRCMP((char *) pBuf->ptrData, "close")) {
            goal_logInfo("Closing TCP data socket %p", (void *) pChan);
            goal_netChanClose(pChan);
        }
        if (GOAL_CMP_EQUAL == GOAL_STRCMP((char *) pBuf->ptrData, "close_delay")) {
            goal_logInfo("Closing TCP data socket %p in 5 seconds", (void *) pChan);
            res = goal_timerSetup(pTmr, GOAL_TIMER_SINGLE, 5000, tcpCloseTimerCb, pChan, GOAL_TRUE);
            if (GOAL_RES_ERR(res)) {
                goal_logErr("error while configuring Timer");
                return res;
            }
        }
        else if (GOAL_CMP_EQUAL == GOAL_STRCMP((char *) pBuf->ptrData, "restart")) {
            goal_logInfo("reopening TCP Listener socket %p", (void *) pListenerChan);
            goal_netChanClose(pListenerChan);
            res = goal_timerSetup(pTmr, GOAL_TIMER_SINGLE, 10, tcpRestartTimerCb, NULL, GOAL_TRUE);
            if (GOAL_RES_ERR(res)) {
                goal_logErr("error while configuring Timer");
                return res;
            }
        }
    }
    else if (cbType == GOAL_NET_CB_CLOSING) {
        goal_logInfo("Closing TCP socket %p", (void *) pChan);
    }

    return GOAL_OK;
}


/****************************************************************************/
/** TCP Lister Restart Callback
 *
 * This function opens the TCP Listener again.
 */
static void tcpRestartTimerCb(
    void *pArg                                  /**< timer argument */
)
{
    GOAL_STATUS_T res;                          /* result */

    UNUSEDARG(pArg);

    res = goal_netChanOpen(pListenerChan, &addr, tcpCallback, NULL);
    if (GOAL_OK != res) {
        goal_logErr("error while opening TCP server channel on port %"FMT_u32, (uint32_t) MAIN_APPL_TCP_PORT);
    }
}


/****************************************************************************/
/** CLose TCP data channel
 */
static void tcpCloseTimerCb(
    void *pArg                                  /**< timer argument */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_NET_CHAN_T *pChan;                     /* net channel */

    pChan = (GOAL_NET_CHAN_T *) pArg;
    goal_logInfo("Closing TCP data socket %p", (void *) pChan);

    res = goal_netChanClose(pChan);
    if (GOAL_OK != res) {
        goal_logErr("error while closing TCP data channel");
    }
}
