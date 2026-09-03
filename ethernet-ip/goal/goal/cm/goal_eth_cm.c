/** @file
 *
 * @brief ETH configuration
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
#include "goal_eth_cm.h"


#if GOAL_CONFIG_ETHERNET == 1


/****************************************************************************/
/* Local Config */
/****************************************************************************/
#if GOAL_CONFIG_NUM_INTERFACES == 1
# define GOAL_CM_ETH_VAR_LIST GOAL_CM_ETH_VARS(0)
#elif GOAL_CONFIG_NUM_INTERFACES == 2
# define GOAL_CM_ETH_VAR_LIST GOAL_CM_ETH_VARS(0),  GOAL_CM_ETH_VARS(1)
#elif GOAL_CONFIG_NUM_INTERFACES == 3
# define GOAL_CM_ETH_VAR_LIST GOAL_CM_ETH_VARS(0),  GOAL_CM_ETH_VARS(1), GOAL_CM_ETH_VARS(2)
#elif GOAL_CONFIG_NUM_INTERFACES == 4
# define GOAL_CM_ETH_VAR_LIST GOAL_CM_ETH_VARS(0),  GOAL_CM_ETH_VARS(1), GOAL_CM_ETH_VARS(2), GOAL_CM_ETH_VARS(3)
#else
# error "Please extend this macro"
#endif /* GOAL_CONFIG_NUM_INTERFACES */


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/
static GOAL_STATUS_T goal_ethCmModReg(
    void
);

static GOAL_STATUS_T goal_ethCmModAdd(
    void
);


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_STAGE_HANDLER_T stageCmModReg;      /**< CM module register stage */
static GOAL_STAGE_HANDLER_T stageCmModAdd;      /**< CM module add stage */
static GOAL_CM_MODDEF_T cmMod = GOAL_CM_MODDEF(GOAL_CM_ETH_MOD_ID, GOAL_ETH); /**< CM module definition */

/* generate 'GOAL_CM_VARENTRY_T cmVars[]' array that maps the above table */
#include <goal_cm_t.h>
GOAL_CM_VARLIST(cmVars, GOAL_CM_ETH_VAR_LIST);


/****************************************************************************/
/** eth cm variables registration
 *
 */
GOAL_STATUS_T goal_ethRegCmVars(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* add callback to configuration management module register stage */
    res = goal_mainStageReg(GOAL_STAGE_CM_MOD_REG, &stageCmModReg, GOAL_STAGE_INIT, goal_ethCmModReg);

    /* add callback to configuration management module add stage */
    if (GOAL_RES_OK(res)) {
        res = goal_mainStageReg(GOAL_STAGE_CM_MOD_ADD, &stageCmModAdd, GOAL_STAGE_INIT, goal_ethCmModAdd);
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
static GOAL_STATUS_T goal_ethCmModReg(
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
static GOAL_STATUS_T goal_ethCmModAdd(
    void
)
{
    GOAL_STATUS_T res;                          /* result */
#if GOAL_CONFIG_DD == 1
    uint32_t cnt;                               /* loop counter */
    uint32_t offset;                            /* instance offset */
#endif /* GOAL_CONFIG_DD */

    /* add application variables */
    res = goal_cmAddModule(&cmMod, cmVars, NULL, NULL, NULL);

#if GOAL_CONFIG_DD == 1
    /* add virtual variables */

    offset = (uint32_t) (GOAL_CM_ETH_VIRT_VAR_LAST - ETH_CM_VAR_LINK);

    for (cnt = 0; (cnt < GOAL_CONFIG_NUM_INTERFACES) && GOAL_RES_OK(res); cnt++) {
        if (GOAL_RES_OK(res)) {
            res = goal_cmRegVarVirtual(4, ETH_CM_VAR_LINK + (cnt * offset), GOAL_CM_UINT32, 4, NULL, NULL);
        }

        if (GOAL_RES_OK(res)) {
            res = goal_cmRegVarVirtual(4, ETH_CM_VAR_SPEED + (cnt * offset), GOAL_CM_UINT32, 4, NULL, NULL);
        }

        if (GOAL_RES_OK(res)) {
            res = goal_cmRegVarVirtual(4, ETH_CM_VAR_DUPLEX + (cnt * offset), GOAL_CM_UINT32, 4, NULL, NULL);
        }

        if (GOAL_RES_OK(res)) {
            res = goal_cmRegVarVirtual(4, ETH_CM_VAR_PORTCNT + (cnt * offset), GOAL_CM_UINT32, 4, NULL, NULL);
        }
    }
#endif /* GOAL_CONFIG_DD */

    return res;
}

#endif /* GOAL_CONFIG_ETHERNET */

