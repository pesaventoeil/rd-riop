/** @file
 *
 * @brief Configuration Management Demonstration
 *
 * This application shows how to use the configuration management.
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
#include "goal_appl.h"


/****************************************************************************/
/* Local Prototypes */
/****************************************************************************/
static GOAL_STATUS_T appl_cmModReg(
    void
);

static GOAL_STATUS_T appl_cmModAdd(
    void
);


/****************************************************************************/
/* Local Variables */
/****************************************************************************/
static GOAL_STAGE_HANDLER_T stageCmModReg;      /**< CM module register stage */
static GOAL_STAGE_HANDLER_T stageCmModAdd;      /**< CM module add stage */
static GOAL_CM_MODDEF_T cmMod = GOAL_CM_MODDEF(APPL_CM_MOD_ID, "APPL_MODULE"); /**< CM module definition */

/* generate 'GOAL_CM_VARENTRY_T cmVars[]' array that maps the above table */
#include <goal_cm_t.h>
GOAL_CM_VARLIST(cmVars, APPL_CM_VARS);


/****************************************************************************/
/** Register Application Configuration Variables
 *
 * This function is called by GOAL init-stage system to register its
 * configuration management variables.
 *
 * API functions from earlier stages are allowed to be used here.
 */
static GOAL_STATUS_T appl_cmModReg(
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
static GOAL_STATUS_T appl_cmModAdd(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* add application variables */
    res = goal_cmAddModule(&cmMod, cmVars, NULL, NULL, NULL);

    return res;
}


/****************************************************************************/
/** Handle Configuration Variable Changes
 *
 * This function is called by the configuration management when a variable was
 * changed.
 */
GOAL_STATUS_T appl_cmChg(
    uint32_t modId,                             /**< module ID */
    uint32_t varId,                             /**< variable Id */
    GOAL_CM_VAR_T *pVar                         /**< variable pointer */
)
{
    UNUSEDARG(pVar);

#if GOAL_CONFIG_LOGGING == 0
    UNUSEDARG(modId);
    UNUSEDARG(varId);
#endif

    goal_logInfo("variable %" FMT_u32 ":%" FMT_u32 " was changed", modId, varId);

    return GOAL_OK;
}


/****************************************************************************/
/** Application Init
 *
 * Initialize required GOAL components like protocols.
 *
 * Note: If the function body is empty the whole function can be removed.
 */
GOAL_STATUS_T appl_init(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* add callback to CM module register stage */
    res = goal_mainStageReg(GOAL_STAGE_CM_MOD_REG, &stageCmModReg, GOAL_STAGE_INIT, appl_cmModReg);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add CM register stage handle");
        return res;
    }

    /* add callback to CM module add stage */
    res = goal_mainStageReg(GOAL_STAGE_CM_MOD_ADD, &stageCmModAdd, GOAL_STAGE_INIT, appl_cmModAdd);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add CM add stage handler");
        return res;
    }

    return res;
}


/****************************************************************************/
/** Application Setup
 *
 * Configure GOAL and components.
 *
 * Note: If the function body is empty the whole function can be removed.
 */
GOAL_STATUS_T appl_setup(
    void
)
{
    GOAL_STATUS_T res;                          /* result */
    uint32_t val32;                             /* 32-bit value */
    GOAL_CM_VAR_T *pCmVar;                      /* CM var handle */

    /* print variables meta data */
    goal_cmPrintVarList();

    /* change 32-bit variable */
    val32 = 0xdeadbeaf;
    res = goal_cmSetVarValue(APPL_CM_MOD_ID, APPL_CM_VAR_UINT32, &val32, sizeof(val32), GOAL_TRUE, NULL);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("error updating 32-bit variable");
    }

    /* read 32-bit variable */
    val32 = 0;
    res = goal_cmGetVarById(APPL_CM_MOD_ID, APPL_CM_VAR_UINT32, &pCmVar);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("error reading 32-bit variable");
    } else {
        goal_logInfo("content of 32-bit variable: 0x%08" FMT_x32, GOAL_CM_VAR_UINT32(pCmVar));
    }

    return res;
}
