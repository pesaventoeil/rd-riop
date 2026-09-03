/** @file
 *
 * @brief CLI via UDP Example
 *
 * This application tests the reception of cli commands via UDP packets.
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
#define MAIN_APPL_IP            GOAL_NET_IPV4(192, 168, 0, 100)
#define MAIN_APPL_NM            GOAL_NET_IPV4(255, 255, 255, 0)
#define MAIN_APPL_GW            GOAL_NET_IPV4(0, 0, 0, 0)


/****************************************************************************/
/** Application
 *
 * This function must initialize the GOAL and all used protocol stacks.
 * Furthermore application specific resources must be initialized.
 */
GOAL_STATUS_T appl_init(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* initialize CLI */
    res = goal_cliInit(NULL);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to initialize CLI");
    }

    return res;
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
    GOAL_STATUS_T res;                          /* result */
    uint32_t ip;                                /* IP address */
    uint32_t nm;                                /* netmask */
    uint32_t gw;                                /* gateway */

    /* set IP address */
    ip = MAIN_APPL_IP;
    nm = MAIN_APPL_NM;
    gw = MAIN_APPL_GW;
    res = goal_netIpSet(ip, nm, gw, GOAL_FALSE);
    if (GOAL_OK != res) {
        goal_logErr("failed to set IP address");
    }

    return res;
}
