/** @file
 *
 * @brief Timer Management Module - CLI Interface
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

#define GOAL_ID GOAL_ID_TMR
#include <goal_includes.h>


/****************************************************************************/
/* Local constants */
/****************************************************************************/
static const char strTimer[] = "time";          /**< 'tm' command */
static const char strTimerHelp[] = "timer management"; /**< 'tm' command help */
static const char strTimestamp[] = "current";   /**< 'ts' sub-command */
static const char strShow[] = "show";           /**< 'ts' sub-command */
static const char strOs[] = "os";               /**< 'ts' sub-command */


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/
static void goal_timerCliCmd(
    GOAL_CLI_DATA_T *pData                      /**< CLI data */
);

static void goal_timerCliTimestamp(
    void
);


/****************************************************************************/
/** Initialize Timer CLI Commands
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_timerInitCli(
    void
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_CLI_CMD_T *pHdlTimer = NULL;           /* CLI 'timer' command handle */
    GOAL_CLI_CMD_T *pHdlCurrent = NULL;         /* CLI 'timer current' command handle */
    GOAL_CLI_CMD_T *pHdlTmp = NULL;             /* tmp command handle */

    /* register root 'timer' command */
    res = goal_cliCmdReg(strTimer, strTimerHelp, goal_timerCliCmd, NULL, &pHdlTimer);

    /* register 'timer os' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(strOs, NULL, NULL, pHdlTimer, &pHdlCurrent);
    }

    /* register 'timer os current' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(strTimestamp, NULL, NULL, pHdlCurrent, &pHdlTmp);
    }

    /* register 'timer os current show' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(strShow, NULL, NULL, pHdlTmp, NULL);
    }

    /* show error info */
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to initialize timer CLI");
    }

    return res;
}


/****************************************************************************/
/** Timer CLI 'timer' handler
 */
static void goal_timerCliCmd(
    GOAL_CLI_DATA_T *pData                      /**< CLI data */
)
{
    GOAL_STATUS_T res;                          /* result */
    const char *pStr = NULL;                    /* command string */
    unsigned int len = 0;                       /* command length */

    /* handle main parameter */
    res = goal_cliParamGet(pData, GOAL_CLI_PARAM(1), &pStr, &len);
    if (GOAL_RES_OK(res)) {

        /* 'timer os' */
        if (strOs == pStr) {
            goal_timerCliTimestamp();
            return;
        }
    }

    goal_cliPrintf("unknown command\n");
}


/****************************************************************************/
/** Timer CLI 'timer os' handler
 */
static void goal_timerCliTimestamp(
    void
)
{
    /* show current timestamp */
    goal_cliPrintf("Current timestamp: %" FMT_u64 "\n", goal_timerTsGet());
}
