/** @file
 *
 * @brief UDP Server Example
 *
 * This application demonstrates the reception of UDP packets.
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
#define MAIN_APPL_UDP_PORT      1234
#define MAIN_APPL_IP            GOAL_NET_IPV4(192, 168, 0, 10)
#define MAIN_APPL_NM            GOAL_NET_IPV4(255, 255, 255, 0)
#define MAIN_APPL_GW            GOAL_NET_IPV4(0, 0, 0, 0)


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/
static GOAL_STATUS_T udpCallback(
    void *pArg,                                 /**< callback argument */
    GOAL_NET_CB_TYPE_T cbType,                  /**< callback type */
    GOAL_NET_CHAN_T *pChan,                     /**< channel descriptor */
    GOAL_BUFFER_T *pBuf                         /**< GOAL buffer */
);

static void udpChanOpen(
    void *pArg                                  /**< timer argument */
);


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_NET_CHAN_T *pUdpChan = NULL;        /**< UDP channel */
static GOAL_NET_ADDR_T addr;                    /**< net address */
static GOAL_TIMER_T *pTmr = NULL;               /**< channel restart timer */
static GOAL_BOOL_T chanOpened = GOAL_FALSE;     /**< UDP channel was opened */


/****************************************************************************/
/** Application Setup
 *
 * This function is called by the GOAL init-stage system to open UDP channels.
 *
 * API functions from earlier stages are allowed to be used here.
 */
GOAL_STATUS_T appl_setup(
    void
)
{
    GOAL_STATUS_T res;                          /* result */
    uint32_t ip = MAIN_APPL_IP;                 /* IP address */
    uint32_t nm = MAIN_APPL_NM;                 /* netmask */
    uint32_t gw = MAIN_APPL_GW;                 /* gateway */

    /* set IP address */
    res = goal_netIpSet(ip, nm, gw, GOAL_FALSE);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("error while setting IP address");
        return res;
    }

    /* register UDP server */
    res = goal_netChanCreate(&pUdpChan, GOAL_NET_UDP_SERVER, 0);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("error while creating UDP server channel");
        return res;
    }

    GOAL_MEMSET(&addr, 0, sizeof(GOAL_NET_ADDR_T));
    addr.localPort = MAIN_APPL_UDP_PORT;

    res = goal_timerCreate(&pTmr, GOAL_TIMER_LOW);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("error while creating Timer");
        return res;
    }

    res = goal_timerSetup(pTmr, GOAL_TIMER_SINGLE, 10, udpChanOpen, NULL, GOAL_FALSE);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("error while configuring Timer");
        return res;
    }

    udpChanOpen(NULL);

    if (GOAL_FALSE == chanOpened) {
        goal_logErr("failed to open UDP channel");
        return res;
    }

    /* greet */
    goal_logInfo("waiting for UDP connections on port %u", MAIN_APPL_UDP_PORT);

    return GOAL_OK;
}


/****************************************************************************/
/** UDP Server Callback
 *
 * Print a hexdump of the received data.
 */
static GOAL_STATUS_T udpCallback(
    void *pArg,                                 /**< callback argument */
    GOAL_NET_CB_TYPE_T cbType,                  /**< callback type */
    GOAL_NET_CHAN_T *pChan,                     /**< channel descriptor */
    GOAL_BUFFER_T *pBuf                         /**< GOAL buffer */
)
{
    GOAL_STATUS_T res;                          /* result */

    UNUSEDARG(pArg);
    UNUSEDARG(cbType);

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
    goal_logInfo("UDP message: '%s'", pBuf->ptrData);

    if (GOAL_CMP_EQUAL == GOAL_STRCMP((char *) pBuf->ptrData, "restart")) {
        goal_logInfo("reopening UDP socket %p", (void *) pUdpChan);
        goal_netChanClose(pUdpChan);
        res = goal_timerStart(pTmr);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("error while configuring Timer");
        }
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Open the UDP channel
 *
 * This function initializes the UDP channel.
 * It is called during initialization and after a restart command was received.
 */
static void udpChanOpen(
    void *pArg                                  /**< timer argument */
)
{
    GOAL_STATUS_T res;                          /* result */
    uint32_t optVal;                            /* option value */

    UNUSEDARG(pArg);

    /* open UDP channel */
    res = goal_netChanOpen(pUdpChan, &addr, udpCallback, NULL);
    if (GOAL_OK != res) {
        goal_logErr("error while opening UDP channel on port %"FMT_u32, (uint32_t) MAIN_APPL_UDP_PORT);
    }

    if (GOAL_RES_OK(res)) {
        /* set UDP channel to non-blocking */
        optVal = 1;
        res = goal_netChanOptionSet(pUdpChan, GOAL_NET_OPTION_NONBLOCK, &optVal);
        if (GOAL_OK != res) {
            goal_logErr("error while setting UDP channel to non-blocking");
        }
    }

    if (GOAL_RES_OK(res)) {
        /* enable broadcast reception */
        optVal = 1;
        res = goal_netChanOptionSet(pUdpChan, GOAL_NET_OPTION_BROADCAST, &optVal);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("error while setting UDP channel to receive broadcasts");
        }
    }

    if (GOAL_RES_OK(res)) {
        /* activate channel */
        res = goal_netChanActivate(pUdpChan);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("error while enabling UDP channel");
        }
    }

    chanOpened = GOAL_RES_OK(res) ? GOAL_TRUE : GOAL_FALSE;
}
