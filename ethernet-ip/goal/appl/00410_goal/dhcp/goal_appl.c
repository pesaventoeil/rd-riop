/** @file
 *
 * @brief DHCP Example
 *
 * This application demonstrates the reception of IP, netmask and gateway
 * via DHCP.
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
/* Local constants */
/****************************************************************************/
/**< DHCP states strings */
static char *strStates[] = {
    "unknown",                                  /* unknown DHCP state */
    "disabled",                                 /* DHCP disabled */
    "in progress",                              /* DHCP in progress */
    "bound"                                     /* DHCP address bound */
};


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

    /* start DHCP */
    res = goal_netCmd(GOAL_NET_CMD_DHCP_START, GOAL_FALSE, NULL);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to initialize DHCP");
    }

    return res;
}


/****************************************************************************/
/** Main Loop
 *
 * This function must implement the application logic and must not block. It is
 * called in a loop (the GOAL loop) and if state tracking is necessary it
 * should use static or global variables.
 */
void appl_loop(
    void
)
{
    static uint32_t statePrev = GOAL_NET_DHCP_STATE_UNKNOWN; /* previous DHCP state */
    static uint32_t state = GOAL_NET_DHCP_STATE_UNKNOWN; /* current DHCP state */
    GOAL_STATUS_T res;                          /* result */
    GOAL_NET_DHCP_T dhcpData;                   /* addresses from DHCP server */

    /* retrieve DHCP state */
    res = goal_netCmd(GOAL_NET_CMD_DHCP_STATE, GOAL_FALSE, &state);
    if (GOAL_RES_ERR(res)) {
        return;
    }

    /* compare current and previous states */
    if (statePrev == state) {
        return;
    }

    goal_logInfo("DHCP state change: '%s' to '%s'", strStates[statePrev], strStates[state]);

    /* update previous state */
    statePrev = state;

    /* show IP address if state changed to bound */
    if (GOAL_NET_DHCP_STATE_BOUND != state) {
        return;
    }

    /* request DHCP address settings */
    res = goal_netCmd(GOAL_NET_CMD_DHCP_ADDR, GOAL_FALSE, &dhcpData);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to request DHCP address settings");
        return;
    }

    /* update IP address config */
    res = goal_netIpSet(dhcpData.addrIp, dhcpData.addrNetmask, dhcpData.addrGateway, GOAL_FALSE);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to set IP address config");
        return;
    }

    /* show IP configuration */
    goal_logInfo("ip addr: %"FMT_u32".%"FMT_u32".%"FMT_u32".%"FMT_u32,
                 (dhcpData.addrIp >> 24) & 0xff, (dhcpData.addrIp >> 16) & 0xff, (dhcpData.addrIp >> 8) & 0xff, dhcpData.addrIp & 0xff);
    goal_logInfo("netmask: %"FMT_u32".%"FMT_u32".%"FMT_u32".%"FMT_u32,
                 (dhcpData.addrNetmask >> 24) & 0xff, (dhcpData.addrNetmask >> 16) & 0xff, (dhcpData.addrNetmask >> 8) & 0xff, dhcpData.addrNetmask & 0xff);
    goal_logInfo("gateway: %"FMT_u32".%"FMT_u32".%"FMT_u32".%"FMT_u32,
                 (dhcpData.addrGateway >> 24) & 0xff, (dhcpData.addrGateway >> 16) & 0xff, (dhcpData.addrGateway >> 8) & 0xff, dhcpData.addrGateway & 0xff);
    goal_logInfo("DNS server 1: %"FMT_u32".%"FMT_u32".%"FMT_u32".%"FMT_u32,
                 (dhcpData.addrDns1 >> 24) & 0xff, (dhcpData.addrDns1 >> 16) & 0xff, (dhcpData.addrDns1 >> 8) & 0xff, dhcpData.addrDns1 & 0xff);
    goal_logInfo("DNS server 2: %"FMT_u32".%"FMT_u32".%"FMT_u32".%"FMT_u32,
                 (dhcpData.addrDns2 >> 24) & 0xff, (dhcpData.addrDns2 >> 16) & 0xff, (dhcpData.addrDns2 >> 8) & 0xff, dhcpData.addrDns2 & 0xff);
    goal_logInfo("Domain Name: %s", dhcpData.domainName);
}
