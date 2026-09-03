/** @file
 *
 * @brief RSTP Example Application
 *
 * This application starts the RSTP protocol stack.
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
#include <protos/rstp/goal_rstp.h>


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_RSTP_BRIDGE_ADDRESS_T addrBridge = { 0xef, 0x34, 0x56, 0x78, 0x9a, 0xbc }; /**< Bridge address */


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

    /* initialize CLI */
    res = goal_cliInit(NULL);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* initialize RSTP */
    res = goal_rstpInit();
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    return res;
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
    uint32_t numPortsExt = 0;                   /* number of external ports */
    uint32_t cnt;                               /* counter */

    /* get external port count */
    res = goal_ethPortsGet(&numPortsExt);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* set Bridge address */
    goal_rstpBridgeAddrSet(&addrBridge);

    /* set RSTP port states */
    for (cnt = 0; cnt < numPortsExt; cnt++) {
        goal_rstpBridgePortForcePortState(cnt, GOAL_TRUE);
    }

    return res;
}
