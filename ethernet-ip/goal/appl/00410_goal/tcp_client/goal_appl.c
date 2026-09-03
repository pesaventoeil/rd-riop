/** @file
 *
 * @brief Simple TCP client
 *
 * This application demonstrates the connection of a TCP client to a TCP server.
 * After establishing a connection to given server a fixed number of test TCP
 * frames will be sent. Then the client will close the connection.
 *
 * The client waits until a server is found. If the TCP closed the channel
 * (e.g. by timeout) the client tries to reopen it.
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
#define MAIN_APPL_IP            GOAL_NET_IPV4(192, 168, 0, 100)
#define MAIN_APPL_IP_REMOTE     GOAL_NET_IPV4(192, 168, 0, 10)
#define MAIN_APPL_NM            GOAL_NET_IPV4(255, 255, 255, 0)
#define MAIN_APPL_GW            GOAL_NET_IPV4(0, 0, 0, 0)

#define GOAL_APPL_RECON_TIME        GOAL_TIMER_SEC /**< reconnection time */
#define GOAL_APPL_SEND_INT          GOAL_TIMER_SEC /**< interval between two sending actions */
#define GOAL_APPL_SEND_CNT_CLOSE    5           /**< number of sending actions before closing */

#define GOAL_APPL_CNT_REPEAT        3           /**< count of repeating this test for each execution mode */

#define TCP_MSL                 60000           /**< TCP MSL in ms */


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/
static GOAL_STATUS_T tcpCallback(
    void *pArg,                                 /**< callback argument */
    GOAL_NET_CB_TYPE_T cbType,                  /**< callback type */
    GOAL_NET_CHAN_T *pChan,                     /**< channel descriptor */
    struct GOAL_BUFFER_T *pBuf                  /**< GOAL buffer */
);

static void appReconnect(
    void *pArg                                  /**< timer argument */
);

static void appReset(
    void *pArg                                  /**< timer argument */
);

static void appConnect(
    uint16_t lport                              /**< local port */
);


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_BOOL_T flgExeDynLocPortMode = GOAL_FALSE; /**< execution in dynamic local port mode flag */
static GOAL_BOOL_T flgExeSttLocPortMode = GOAL_FALSE; /**< execution in static local port mode flag */
static GOAL_NET_CHAN_T *pChanTcp = NULL;        /**< GOAL TCP channel */
static GOAL_TIMESTAMP_T tsNext = 0;             /**< next iteration time */
static GOAL_BOOL_T connected = GOAL_FALSE;      /**< connected to server */
static GOAL_TIMER_T *pTmrReconnect = NULL;      /**< Timer for next connection attempt */
static GOAL_TIMER_T *pTmrReset = NULL;          /**< Reset timer */
static uint32_t cntTried = 0;                   /**< count of test tried */
static uint16_t lastDynLocPort;                 /**< local port number at last test in dynamic local port mode */


/****************************************************************************/
/** Application Setup
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T appl_setup(
    void
)
{
    GOAL_STATUS_T res;                          /* result */
    uint32_t ip;                                /* IP address */
    uint32_t nm;                                /* netmask */
    uint32_t gw;                                /* gateway */

    /* prepare reconnection timer */
    res = goal_timerCreate(&pTmrReconnect, GOAL_TIMER_LOW);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create reconnect timer");
        return res;
    }

    /* prepare reset timer */
    res = goal_timerCreate(&pTmrReset, GOAL_TIMER_LOW);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create reset timer");
        return res;
    }

    /* prepare GOAL channel */
    res = goal_netChanCreate(&pChanTcp, GOAL_NET_TCP_CLIENT, 0);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("error while creating TCP client channel");
        return res;
    }

    /* set IP address */
    ip = MAIN_APPL_IP;
    nm = MAIN_APPL_NM;
    gw = MAIN_APPL_GW;
    res = goal_netIpSet(ip, nm, gw, GOAL_FALSE);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Set IP failed");
        return res;
    }

    return res;
}


/****************************************************************************/
/** Create TCP client socket & connect to server
 */
static void appConnect(
    uint16_t lport                              /**< local port */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_NET_ADDR_T addr;                       /* net address */
    uint32_t optVal;                            /* option value */

    GOAL_MEMSET(&addr, 0, sizeof(GOAL_NET_ADDR_T));
    addr.localPort = lport;
    addr.remoteIp = MAIN_APPL_IP_REMOTE;
    addr.remotePort = MAIN_APPL_TCP_PORT;

    /* register TCP client */
    res = goal_netChanOpen(pChanTcp, &addr, tcpCallback, NULL);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("error while opening TCP client channel on port %"FMT_u32, (uint32_t) lport);
    }

    if (GOAL_RES_OK(res)) {
        /* set TCP channel to non-blocking */
        optVal = 1;
        res = goal_netChanOptionSet(pChanTcp, GOAL_NET_OPTION_NONBLOCK, &optVal);
        if (GOAL_RES_ERR(res)) {
            goal_netChanClose(pChanTcp);
            goal_logErr("error while setting TCP channel to non-blocking");
        }
    }


    if (GOAL_RES_OK(res)) {
        /* activate channel */
        res = goal_netChanActivate(pChanTcp);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("error while enabling TCP channel");
            return;
        }
    }

    if (GOAL_RES_ERR(res)) {
        res = goal_timerSetup(pTmrReset, GOAL_TIMER_SINGLE, GOAL_APPL_RECON_TIME, appReset, NULL, GOAL_TRUE);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to start reset timer");
        }
    }
}


/****************************************************************************/
/** TCP Server Callback
 *
 * Print a hexdump of the received data.
 */
static GOAL_STATUS_T tcpCallback(
    void *pArg,                                 /**< callback argument */
    GOAL_NET_CB_TYPE_T cbType,                  /**< callback type */
    GOAL_NET_CHAN_T *pChan,                     /**< channel descriptor */
    struct GOAL_BUFFER_T *pBuf                  /**< GOAL buffer */
)
{
    UNUSEDARG(pArg);

    if (cbType == GOAL_NET_CB_CONNECTED) {

        /* indicate succsessfull connection */
        connected = GOAL_TRUE;
    }
    else if (cbType == GOAL_NET_CB_NEW_DATA) {
        goal_logInfo("Data received on tcp socket %p", (void *) pChan);
        if (GOAL_TRUE == flgExeDynLocPortMode) {
            lastDynLocPort = pChan->addr.localPort;
            goal_logInfo("Local port number: %"FMT_u32, (uint32_t) lastDynLocPort);
        }

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
    }
    else if (cbType == GOAL_NET_CB_CLOSING) {
        if (!connected) {
            goal_logInfo("Connection to remote endpoint failed, try reconnect");

            /* start reconnect timer */
            if (GOAL_OK != goal_timerSetup(pTmrReconnect, GOAL_TIMER_SINGLE,
                                           GOAL_APPL_RECON_TIME, appReconnect,
                                           NULL, GOAL_TRUE)) {
                goal_logErr("failed to start reconnection timer");
            }

        }
        else {
            /* close active socket */
            goal_logInfo("Closing TCP socket %p", (void *) pChan);

            /* indicate succsessfull disconnection */
            connected = GOAL_FALSE;
        }
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Retry to connect to server
 */
static void appReconnect(
    void *pArg                                  /**< timer argument */
)
{
    GOAL_STATUS_T res;                          /* result */

    UNUSEDARG(pArg);

    goal_logInfo("Reopen TCP client channel");

    /* reopen channel */
    res = goal_netChanReopen(pChanTcp);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to reopen TCP client channel");
    }

    if (GOAL_RES_OK(res)) {
        /* activate channel */
        res = goal_netChanActivate(pChanTcp);
        if (GOAL_RES_ERR(res)) {
            goal_netChanClose(pChanTcp);
            goal_logErr("error while enabling TCP channel");
        }
    }

    if (GOAL_RES_ERR(res)) {
        res = goal_timerSetup(pTmrReset, GOAL_TIMER_SINGLE, GOAL_APPL_RECON_TIME, appReset, NULL, GOAL_TRUE);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to start reset timer");
        }
    }
    else {
        /* get current timestamp */
        tsNext = goal_timerTsGet();
    }
}


/****************************************************************************/
/** Reset all state variables to start a new test run
 */
static void appReset(
    void *pArg                                  /**< timer argument */
)
{
    UNUSEDARG(pArg);

    flgExeDynLocPortMode = GOAL_FALSE;
    flgExeSttLocPortMode = GOAL_FALSE;
    tsNext = 0;
    connected = GOAL_FALSE;
    cntTried = 0;
}


/****************************************************************************/
/** Application Loop
 */
void appl_loop(
    void
)
{
    static uint32_t cnt = 0;                    /* iteration counter */
    GOAL_BUFFER_T *pBuf = NULL;                 /* GOAL buffer */
    uint32_t waitingTime;                       /* waiting time in ms */
    GOAL_STATUS_T res;                          /* result */

    if (!connected) {
        if ((0 == tsNext) && (0 == cntTried)) {
            if ((GOAL_FALSE == flgExeDynLocPortMode) && (GOAL_FALSE == flgExeSttLocPortMode)) {
                /* set Execution in dynamic localport mode on */
                flgExeDynLocPortMode = GOAL_TRUE;

                /* create socket with local port parameter value of 0 */
                /* caution: When the local port parameter is zero, the local port of the socket is     */
                /*          set to a value from the dynamic local port pool and will be changes for    */
                /*          each reconnection. If the local port parameter is not zero, the local      */
                /*          port of the socket is fixed to the parameter value. In this case the port  */
                /*          can be changed manually on next connection by changing the parameter value */
                /*          in the channel handle before reconnecting.                                 */
                appConnect(0);

                /* greet */
                goal_logInfo("===== TCP client with dynamic local port =====");
                goal_logInfo("connecting to TCP server %"FMT_u32".%"FMT_u32".%"FMT_u32".%"FMT_u32" on port %"FMT_u32,
                             (uint32_t) ((MAIN_APPL_IP_REMOTE >> 24) & 0xff),
                             (uint32_t) ((MAIN_APPL_IP_REMOTE >> 16) & 0xff),
                             (uint32_t) ((MAIN_APPL_IP_REMOTE >> 8) & 0xff),
                             (uint32_t) (MAIN_APPL_IP_REMOTE & 0xff),
                             (uint32_t) MAIN_APPL_TCP_PORT);

               /* set next iteration time */
               tsNext = goal_timerTsGet() + GOAL_APPL_SEND_INT;
            } else if ((GOAL_TRUE == flgExeDynLocPortMode) && (GOAL_FALSE == flgExeSttLocPortMode)) {
                /* set Execution in dynamic localport mode off */
                flgExeDynLocPortMode = GOAL_FALSE;

                /* set Execution in static localport mode on */
                flgExeSttLocPortMode = GOAL_TRUE;

                /* retry connect to server with a static local port */
                goal_netChanClose(pChanTcp);
                connected = GOAL_FALSE;
                appConnect(MAIN_APPL_TCP_PORT);

                /* greet */
                goal_logInfo("===== TCP client with static local port =====");
                goal_logInfo("connecting to TCP server %"FMT_u32".%"FMT_u32".%"FMT_u32".%"FMT_u32" on port %"FMT_u32,
                             (uint32_t) ((MAIN_APPL_IP_REMOTE >> 24) & 0xff),
                             (uint32_t) ((MAIN_APPL_IP_REMOTE >> 16) & 0xff),
                             (uint32_t) ((MAIN_APPL_IP_REMOTE >> 8) & 0xff),
                             (uint32_t) (MAIN_APPL_IP_REMOTE & 0xff),
                             (uint32_t) MAIN_APPL_TCP_PORT);
            }
        }
        return;
    }

    /* send out data each time interval */
    if (goal_timerTsGet() < tsNext) {
        return;
    }

    tsNext = goal_timerTsGet() + GOAL_APPL_SEND_INT;

    if (GOAL_APPL_SEND_CNT_CLOSE > cnt) {
        /* get net buffer */
        res = goal_ethGetNetBuf(&pBuf);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("Did not get GOAL buffer");
            return;
        }

        /* fill buffer with message */
        GOAL_SNPRINTF((char *) pBuf->ptrData, pBuf->bufSize, "TESTSTRING %"FMT_u32, cnt);
        pBuf->dataLen = (uint16_t) GOAL_STRLEN((char *) pBuf->ptrData);

        /* send message */
        res = goal_netChanSend(pChanTcp, pBuf);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("Could not send message %s", pBuf->ptrData);
        }
        else {
            goal_logInfo("sent message %s", pBuf->ptrData);

            /* update counter */
            cnt++;
        }

        /* release buffer */
        res = goal_queueReleaseBuf(&pBuf);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("Could not release GOAL buffer");
        }
    } else {
        /* close connection & prepare next test */
        goal_logInfo("closing socket");
        goal_netChanClose(pChanTcp);
        connected = GOAL_FALSE;
        tsNext = (GOAL_TIMESTAMP_T) -1L;

        /* reset iteration counter */
        cnt = 0;

        /* increase count of test tried */
        cntTried++;

        if (GOAL_APPL_CNT_REPEAT > cntTried) {
            /* repeat this */
            /* caution: if a local port which is used within 120 seconds(= 2 * TCP_MSS) */
            /*          is reuses, a socket error will be occured.                      */
            waitingTime = (GOAL_TRUE == flgExeSttLocPortMode) ? 2 * TCP_MSL : GOAL_APPL_RECON_TIME;
            if (GOAL_OK != goal_timerSetup(pTmrReconnect, GOAL_TIMER_SINGLE,
                                           waitingTime, appReconnect,
                                           NULL, GOAL_TRUE)) {
                goal_logErr("failed to start reconnection timer");
            } else {
                goal_logInfo("try to reconnect in %"FMT_u32" sec", waitingTime / GOAL_TIMER_SEC);
            }
        } else {
            /* reset count of test tried and next iteration time */
            cntTried = 0;
            tsNext = 0;

            /* restore local port if test has been performed in dynamic local port mode */
            if ((GOAL_FALSE == flgExeDynLocPortMode) && (0 != lastDynLocPort)) {
                pChanTcp->addr.localPort = lastDynLocPort;
            }
        }
    }
}
