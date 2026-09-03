/** @file
 *
 * @brief UDP Server Example
 *
 * This application demonstrates the activation and handling of the
 * jumbo frame feature if supported by the hardware.
 * This application is based on the "udp_receive" demo.
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


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_NET_CHAN_T *pChanUdp = NULL;        /**< UDP channel */


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
    GOAL_NET_ADDR_T addr;                       /* net address */
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
        goal_logErr("error while setting IP address");
        return res;
    }

    /* create NET channel */
    res = goal_netChanCreate(&pChanUdp, GOAL_NET_UDP_SERVER, 0);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create UDP channel");
        return res;
    }

    /* register UDP server */
    GOAL_MEMSET(&addr, 0, sizeof(GOAL_NET_ADDR_T));
    addr.localPort = MAIN_APPL_UDP_PORT;
    res = goal_netChanOpen(pChanUdp, &addr, udpCallback, NULL);
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

    /* enable broadcast reception */
    optVal = 1;
    res = goal_netChanOptionSet(pChanUdp, GOAL_NET_OPTION_BROADCAST, &optVal);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("error while setting UDP channel to receive broadcasts");
        return res;
    }

    /* activate channel */
    res = goal_netChanActivate(pChanUdp);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("error while enabling UDP channel");
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

    return GOAL_OK;
}
