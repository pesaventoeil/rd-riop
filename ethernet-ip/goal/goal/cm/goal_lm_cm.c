/** @file
 *
 * @brief LM configuration
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
#include "goal_lm_cm.h"

#if GOAL_CONFIG_LOGGING == 1


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/
static GOAL_STATUS_T goal_lmCmModReg(
    void
);

static GOAL_STATUS_T goal_lmCmModAdd(
    void
);


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_STAGE_HANDLER_T stageCmModReg;      /**< CM module register stage */
static GOAL_STAGE_HANDLER_T stageCmModAdd;      /**< CM module add stage */
static GOAL_CM_MODDEF_T cmMod = GOAL_CM_MODDEF(GOAL_CM_LM_MOD_ID, GOAL_LM); /**< CM module definition */

/* generate 'GOAL_CM_VARENTRY_T cmVars[]' array that maps the above table */
#include <goal_cm_t.h>
GOAL_CM_VARLIST(cmVars, GOAL_CM_LM_VARS);


/****************************************************************************/
/** lm cm variables registration
 *
 */
GOAL_STATUS_T goal_lmRegCmVars(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* add callback to configuration management module register stage */
    res = goal_mainStageReg(GOAL_STAGE_CM_MOD_REG, &stageCmModReg, GOAL_STAGE_INIT, goal_lmCmModReg);

    /* add callback to configuration management module add stage */
    if (GOAL_RES_OK(res)) {
        res = goal_mainStageReg(GOAL_STAGE_CM_MOD_ADD, &stageCmModAdd, GOAL_STAGE_INIT, goal_lmCmModAdd);
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
static GOAL_STATUS_T goal_lmCmModReg(
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
static GOAL_STATUS_T goal_lmCmModAdd(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* add application variables */
    res = goal_cmAddModule(&cmMod, cmVars, NULL, NULL, NULL);

    /* add virtual variables */
    if (GOAL_RES_OK(res)) {
        res = goal_cmRegVarVirtual(35, LM_CM_VAR_READBUFFER, GOAL_CM_GENERIC, 128, NULL, NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_cmRegVarVirtual(35, LM_CM_VAR_CNT, GOAL_CM_UINT16, 2, NULL, LM_onLogAccess);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_cmRegVarVirtual(35, LM_CM_VAR_EXLOG_READBUFFER, GOAL_CM_GENERIC, 128, NULL, NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_cmRegVarVirtual(35, LM_CM_VAR_EXLOG_CNT, GOAL_CM_UINT16, 2, NULL, LM_exLogAccess);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_cmRegVarVirtual(35, LM_CM_VAR_EXLOG_SIZE, GOAL_CM_UINT32, 4, NULL, NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_cmRegVarVirtual(35, LM_CM_VAR_EXLOG_USAGE, GOAL_CM_UINT8, 1, NULL, NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_cmRegVarVirtual(35, LM_CM_VAR_EXLOG_ERASE, GOAL_CM_UINT8, 1, NULL, LM_exLogErase);
    }

    return res;
}

#endif /* GOAL_CONFIG_LOGGING == 1 */
