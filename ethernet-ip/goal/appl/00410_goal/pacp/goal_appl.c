/** @file
 *
 *
 * @brief PACP Example
 *
 * This example shows the initialization of PACP.
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
#include <goal_includes.h>
#include <protos/pacp/pacp.h>
#include <protos/radius/radius.h>


/****************************************************************************/
/* Local defines */
/****************************************************************************/
#define APPL_ADDR_IP        GOAL_NET_IPV4(172, 16, 100, 100)
#define APPL_ADDR_NETMASK   GOAL_NET_IPV4(255, 255, 255, 0)
#define APPL_ADDR_GATEWAY   GOAL_NET_IPV4(0, 0, 0, 0)
#define APPL_ADDR_RADIUS    GOAL_NET_IPV4(172, 16, 100, 150)

#define APPL_RADIUS_PORT        0               /**< port connected to radius server */
#define APPL_PORT_SUPPL_START   1               /**< first supplicant port */


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_TIMESTAMP_T tsNext;                 /**< next timestamp */
static uint32_t numPorts = 0;                   /**< count of ports */


/****************************************************************************/
/** Application Init
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T appl_init(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* initialize RADIUS module */
    res = goal_radiusInit();
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* initialize PACP module */
    return goal_pacpInit();
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
    GOAL_STATUS_T res;                          /* result */
    uint32_t radiusIp;                          /* test IP for RADIUS server */
    uint32_t radiusNetPort;                     /* test netPort for RADIUS server */
    uint32_t port;                              /* Ethernet port */
    uint8_t secret[8] = "goalsecr";             /* shared secret for testing */
    uint32_t secretLength = sizeof(secret);     /* size of shared secret */
    GOAL_BOOL_T reauthMode;                     /* reauthication mode on/off */

    /* get port count */
    res = goal_ethPortsGet(&numPorts);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("error while retrieving count of ports");
        return res;
    }

    /* check that at least APPL_PORT_SUPPL_START ports are available */
    if (APPL_PORT_SUPPL_START >= numPorts) {
        goal_logWarn("Less than 2 ports found. No Supplicant can be registered.");
        return GOAL_ERROR;
    }

    /* set static IP address for PACP */
    res = goal_netIpSet(APPL_ADDR_IP, APPL_ADDR_NETMASK, APPL_ADDR_GATEWAY, GOAL_TRUE);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("IP address configuration failed");
        return res;
    }

    /* configure RADIUS server IP */
    radiusIp = APPL_ADDR_RADIUS;
    res = goal_radiusServerIp(&radiusIp, GOAL_TRUE);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Failed to set RADIUS server IP.");
        return res;
    }

    /* configure RADIUS server netPort to port APPL_RADIUS_PORT */
    radiusNetPort = GOAL_ETH_PORT(APPL_RADIUS_PORT);
    res = goal_radiusServerPort(&radiusNetPort, GOAL_TRUE);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Failed to set RADIUS server port number.");
        return res;
    }

    /* configure shared secret for RADIUS server */
    res = goal_radiusCmdServerSecret(radiusNetPort, secret, secretLength, GOAL_TRUE);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Failed to set shared secret.");
        return res;
    }

    /* enable reauthentication supplicant ports */
    for (port = APPL_PORT_SUPPL_START; port < numPorts; port++) {
        reauthMode = GOAL_TRUE;
        res = goal_pacpCmdPortReauthMode(port, &reauthMode, GOAL_TRUE);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("Failed to enable re-authentication mode on port %"FMT_u32".", port);
            return res;
        }
    }

    /* start port access controle module */
    res = goal_pacpModuleEnable();
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Failed start PACP module.");
        return res;
    }

    /* reset all Ethernet statistics if supported (no return value check) */
    goal_statResetByListId(GOAL_ID_ETH);

    /* initialize next timestamp */
    tsNext = goal_timerTsGet() + GOAL_TIMER_SEC;

    return res;
}


/****************************************************************************/
/** Application Loop
 */
void appl_loop(
    void
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_TIMESTAMP_T tsCur = 0;                 /* current timestamp */
    uint32_t port;                              /* Ethernet port */
    uint32_t portAuthState;                     /* command assistance variable */

    /* get current time stamp */
    tsCur = goal_timerTsGet();

    /* get ethernet statistics if time expired */
    if (tsCur > tsNext) {

        /* update next timestamp */
        tsNext = tsCur + GOAL_TIMER_SEC * 5;

        /* iterate though external ports to display their hardware authentication state */
        for (port = 0; port < numPorts; port++) {

            /* get port authentication state from switch */
            res = goal_ethCmd(GOAL_ETH_CMD_PORT_AUTH, GOAL_FALSE, port, &portAuthState);
            if (GOAL_RES_ERR(res)) {
                goal_logErr("Failed to get authentication state of port %"FMT_u32".", port);
            }

            /* output current port authentication state */
            goal_logInfo("Port %"FMT_u32" - auth state: %"FMT_u32, port, portAuthState);
        }

        /* divider line */
        goal_logInfo("-------------------------------------------------------");
    }
}
