/** @file
 *
 * @brief GOAL Network Handling - CLI Interface
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

#define GOAL_ID GOAL_ID_NET
#include <goal_includes.h>


#if (GOAL_CONFIG_TCPIP_STACK == 1) && (GOAL_CONFIG_CLI == 1)


/****************************************************************************/
/* Local constants */
/****************************************************************************/
static const char strNet[] = "net";             /**< 'net' command */
static const char strNetHelp[] = "network configuration"; /**< 'net' command help */
static const char strIp[] = "ip";               /**< 'ip' sub-command */
static const char strShow[] = "show";           /**< 'show' sub-command */
static const char strSet[] = "set";             /**< 'set' sub-command */


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/
static void goal_netCliCmd(
    GOAL_CLI_DATA_T *pData                      /**< CLI data */
);

static void goal_netCliIp(
    GOAL_CLI_DATA_T *pData                      /**< CLI data */
);


/****************************************************************************/
/** Initialize Network CLI Commands
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_netCliInit(
    void
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_CLI_CMD_T *pHdlNet = NULL;             /* CLI 'net' command handle */
    GOAL_CLI_CMD_T *pHdlIp = NULL;              /* CLI 'ip' command handle */

    /* register root 'net' command */
    res = goal_cliCmdReg(strNet, strNetHelp, goal_netCliCmd, NULL, &pHdlNet);

    /* register 'net ip' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(strIp, NULL, NULL, pHdlNet, &pHdlIp);
    }

    /* register 'net ip show' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(strShow, NULL, NULL, pHdlIp, NULL);
    }

    /* register 'net ip set' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(strSet, NULL, NULL, pHdlIp, NULL);
    }

    /* show error info */
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to initialize net CLI");
    }

    return res;
}


/****************************************************************************/
/** Net CLI 'net' handler
 */
static void goal_netCliCmd(
    GOAL_CLI_DATA_T *pData                      /**< CLI data */
)
{
    GOAL_STATUS_T res;                          /* result */
    const char *pStr = NULL;                    /* command string */
    unsigned int len = 0;                       /* command length */

    /* handle main parameter */
    res = goal_cliParamGet(pData, GOAL_CLI_PARAM(1), &pStr, &len);
    if (GOAL_RES_OK(res)) {

        /* 'net ip' */
        if (strIp == pStr) {
            goal_netCliIp(pData);
            return;
        }
    }

    goal_cliPrintf("unknown command\n");
}


/****************************************************************************/
/** Net CLI 'net ip' handler
 */
static void goal_netCliIp(
    GOAL_CLI_DATA_T *pData                      /**< CLI data */
)
{
    GOAL_STATUS_T res;                          /* result */
    const char *pStr = NULL;                    /* command string */
    unsigned int len = 0;                       /* command length */
    uint32_t addrIp;                            /* IP address */
    uint32_t addrNm;                            /* subnet mask */
    uint32_t addrGw;                            /* gateway */
    GOAL_BOOL_T flgTemp;                        /* temporary IP config flag */

    /* retrieve IP configuration */
    res = goal_netIpGet(&addrIp, &addrNm, &addrGw, &flgTemp);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("could not read IP address configuration\n");
        return;
    }

    /* 'net ip' - set address */
    res = goal_cliParamGet(pData, GOAL_CLI_PARAM(2), &pStr, &len);
    if (GOAL_RES_OK(res)) {

        if (strSet == pStr) {

            res = goal_cliParamGet(pData, GOAL_CLI_PARAM(3), &pStr, &len);
            if (GOAL_RES_OK(res)) {

                /* ip address */
                if (GOAL_RES_ERR(goal_netChanIpAddrStrToU32Convert(pStr, len, &addrIp))) {
                    goal_cliPrintf("failed to retrieve IP address\n");
                    return;
                }

                /* netmask */
                res = goal_cliParamGet(pData, GOAL_CLI_PARAM(4), &pStr, &len);
                if (GOAL_RES_OK(res)) {
                    if (GOAL_RES_ERR(goal_netChanIpAddrStrToU32Convert(pStr, len, &addrNm))) {
                        goal_cliPrintf("failed to retrieve netmask\n");
                        return;
                    }
                }

                /* gateway */
                res = goal_cliParamGet(pData, GOAL_CLI_PARAM(5), &pStr, &len);
                if (GOAL_RES_OK(res)) {
                    if (GOAL_RES_ERR(goal_netChanIpAddrStrToU32Convert(pStr, len, &addrGw))) {
                        goal_cliPrintf("failed to retrieve gateway\n");
                        return;
                    }
                }

                /* set IP configuration */
                res = goal_netIpSet(addrIp, addrNm, addrGw, GOAL_TRUE);
                if (GOAL_RES_ERR(res)) {
                    goal_cliPrintf("failed to set IP address configuration\n");
                }

                return;
            }
        } else if (strShow == pStr) {
            /* print IP configuration */
            goal_cliPrintf("IP: %hhu.%hhu.%hhu.%hhu\n", ((char *) &addrIp)[3], ((char *) &addrIp)[2], ((char *) &addrIp)[1], ((char *) &addrIp)[0]);
            goal_cliPrintf("Netmask: %hhu.%hhu.%hhu.%hhu\n", ((char *) &addrNm)[3], ((char *) &addrNm)[2], ((char *) &addrNm)[1], ((char *) &addrNm)[0]);
            goal_cliPrintf("Gateway: %hhu.%hhu.%hhu.%hhu\n", ((char *) &addrGw)[3], ((char *) &addrGw)[2], ((char *) &addrGw)[1], ((char *) &addrGw)[0]);
            return;
        }
    }

    goal_cliPrintf("Syntax error\n");
}
#endif /* (GOAL_CONFIG_TCPIP_STACK == 1) && (GOAL_CONFIG_CLI == 1) */
