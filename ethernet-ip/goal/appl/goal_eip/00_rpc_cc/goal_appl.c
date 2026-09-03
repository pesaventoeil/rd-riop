/** @file
 *
 * @brief EtherNet/IP + DLR for communication core
 *
 * This application provides the communication core of the EtherNet/IP stack.
 * It supports DLR if enabled and the platform supports it.
 *
 * @copyright
 * Copyright 2020 port GmbH Halle/Saale.
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
#include "goal_eip.h"
#include "protos/opener/src/ports/goal/wrapper/cc/goal_eip_rpc.h"
#include <goal_dd.h>


/****************************************************************************/
/** Application
 *
 * This function initializes the CTC and GOAL.
 *
 * @result GOAL_STATUS_T result
 */
GOAL_STATUS_T appl_init(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* initialize device detection module */
    res = goal_ddInit();
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Initialization of GOAL dd failed");
        return res;
    }


    res = goal_eipInit();
    if (GOAL_RES_ERR(res)) {
       goal_logErr("Initialization of Ethernet/IP failed");
       return res;
    }


    return res;
}
