/** @file
 *
 * @brief Command Line Interface Example
 *
 * This application basically initializes the Command Line interface (CLI).
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
/* Local prototypes */
/****************************************************************************/
static void appl_cliDo(
    GOAL_CLI_DATA_T *pData                      /**< CLI data */
);


/****************************************************************************/
/** Application Setup
 *
 * This function registers the CLI "do" command function.
 *
 * @retval GOAL_STATUS_T result
 */
GOAL_STATUS_T appl_setup(
    void
)
{
    return goal_cliCmdReg("do", "does things", appl_cliDo, NULL, NULL);
}


/****************************************************************************/
/** CLI function 'do'
 *
 * This function parses the parameters from the given data and prints them.
 */
static void appl_cliDo(
    GOAL_CLI_DATA_T *pData                      /**< CLI data */
)
{
    GOAL_STATUS_T res;                          /* result */
    unsigned int cnt;                           /* parameter index */
    unsigned int cntMax;                        /* parameter max index */
    const char *pStr = NULL;                    /* parameter string */
    unsigned int len = 0;                       /* parameter length */

    /* get parameter count */
    cntMax = goal_cliParamCount(pData);

    /* print parameters */
    for (cnt = 0; cnt < cntMax; cnt++) {
        res = goal_cliParamGet(pData, cnt, &pStr, &len);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("error reading parameter");
            break;
        }

        goal_cliPrintf("Param %u: %.*s\n", cnt, len, pStr);
    }
}


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
    /* initialize CLI */
    return goal_cliInit(NULL);
}
