/** @file
 *
 * @brief QUEUE configuration
 *
 * @copyright
 * Copyright 2021 port GmbH Halle/Saale.
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
#include "goal_queue_cm.h"


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/
static GOAL_STATUS_T goal_queueCmModReg(
    void
);

static GOAL_STATUS_T goal_queueCmModAdd(
    void
);


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_STAGE_HANDLER_T stageCmModReg;      /**< CM module register stage */
static GOAL_STAGE_HANDLER_T stageCmModAdd;      /**< CM module add stage */
static GOAL_CM_MODDEF_T cmMod = GOAL_CM_MODDEF(GOAL_CM_QUEUE_MOD_ID, GOAL_QUEUE); /**< CM module definition */

/* generate 'GOAL_CM_VARENTRY_T cmVars[]' array that maps the above table */
#include <goal_cm_t.h>
GOAL_CM_VARLIST(cmVars, GOAL_CM_QUEUE_VARS);


/****************************************************************************/
/** queue cm variables registration
 *
 */
GOAL_STATUS_T goal_queueRegCmVars(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* add callback to configuration management module register stage */
    res = goal_mainStageReg(GOAL_STAGE_CM_MOD_REG, &stageCmModReg, GOAL_STAGE_INIT, goal_queueCmModReg);

    /* add callback to configuration management module add stage */
    if (GOAL_RES_OK(res)) {
        res = goal_mainStageReg(GOAL_STAGE_CM_MOD_ADD, &stageCmModAdd, GOAL_STAGE_INIT, goal_queueCmModAdd);
    }

    return res;
}


/****************************************************************************/
/** Register Application Configuration Variables
 *
 * This function is called by GOAL init-stage system to register its
 * configuration management variables.
 *
 * API functions from earlier stages are allowed to be used here.
 */
static GOAL_STATUS_T goal_queueCmModReg(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* register application variables */
    res = goal_cmRegModule(cmVars);

    return res;
}


/****************************************************************************/
/** Add Application Configuration Module
 *
 * This function is called by GOAL init-stage system to add the application
 * module to configuration mangement.
 *
 * API functions from earlier stages are allowed to be used here.
 */
static GOAL_STATUS_T goal_queueCmModAdd(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* add application variables */
    res = goal_cmAddModule(&cmMod, cmVars, NULL, NULL, NULL);

    /* add virtual variables */
    return res;
}


