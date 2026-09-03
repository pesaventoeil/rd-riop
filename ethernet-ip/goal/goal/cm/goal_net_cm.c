/** @file
 *
 * @brief NET configuration
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
#include "goal_net_cm.h"


#if GOAL_CONFIG_TCPIP_STACK == 1


/****************************************************************************/
/* Local Config */
/****************************************************************************/
#if GOAL_CONFIG_NUM_INTERFACES == 1
# define GOAL_CM_NET_VAR_LIST GOAL_CM_NET_VARS(0)
#elif GOAL_CONFIG_NUM_INTERFACES == 2
# define GOAL_CM_NET_VAR_LIST GOAL_CM_NET_VARS(0),  GOAL_CM_NET_VARS(1)
#elif GOAL_CONFIG_NUM_INTERFACES == 3
# define GOAL_CM_NET_VAR_LIST GOAL_CM_NET_VARS(0),  GOAL_CM_NET_VARS(1), GOAL_CM_NET_VARS(2)
#elif GOAL_CONFIG_NUM_INTERFACES == 4
# define GOAL_CM_NET_VAR_LIST GOAL_CM_NET_VARS(0),  GOAL_CM_NET_VARS(1), GOAL_CM_NET_VARS(2), GOAL_CM_NET_VARS(3)
#else
# error "Please extend this macro"
#endif /* GOAL_CONFIG_NUM_INTERFACES */


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/
static GOAL_STATUS_T goal_netCmModReg(
    void
);

static GOAL_STATUS_T goal_netCmModAdd(
    void
);


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_STAGE_HANDLER_T stageCmModReg;      /**< CM module register stage */
static GOAL_STAGE_HANDLER_T stageCmModAdd;      /**< CM module add stage */
static GOAL_CM_MODDEF_T cmMod = GOAL_CM_MODDEF(GOAL_CM_NET_MOD_ID, GOAL_NET); /**< CM module definition */

/* generate 'GOAL_CM_VARENTRY_T cmVars[]' array that maps the above table */
#include <goal_cm_t.h>
GOAL_CM_VARLIST(cmVars, GOAL_CM_NET_VAR_LIST);


/****************************************************************************/
/** net cm variables registration
 *
 */
GOAL_STATUS_T goal_netRegCmVars(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* add callback to configuration management module register stage */
    res = goal_mainStageReg(GOAL_STAGE_CM_MOD_REG, &stageCmModReg, GOAL_STAGE_INIT, goal_netCmModReg);

    /* add callback to configuration management module add stage */
    if (GOAL_RES_OK(res)) {
        res = goal_mainStageReg(GOAL_STAGE_CM_MOD_ADD, &stageCmModAdd, GOAL_STAGE_INIT, goal_netCmModAdd);
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
static GOAL_STATUS_T goal_netCmModReg(
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
static GOAL_STATUS_T goal_netCmModAdd(
    void
)
{
    GOAL_STATUS_T res;                          /* result */
    uint32_t cnt;                               /* loop counter */
    uint32_t offset;                            /* instance offset */

    /* add application variables */
    res = goal_cmAddModule(&cmMod, cmVars, NULL, NULL, NULL);

    offset = (uint32_t) (GOAL_CM_NET_VIRT_VAR_LAST - NET_CM_VAR_COMMIT);

    /* add virtual variables */
    for (cnt = 0; (cnt < GOAL_CONFIG_NUM_INTERFACES) && GOAL_RES_OK(res); cnt++) {
        res = goal_cmRegVarVirtual(12, NET_CM_VAR_COMMIT + (cnt * offset), GOAL_CM_UINT8, 1, NULL, goal_miNetCmIpCb);
    }

    return res;
}

#endif /* GOAL_CONFIG_TCPIP_STACK */

