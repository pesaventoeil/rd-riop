/** @file
 *
 * @brief GOAL - target specific source file
 *
 * This file implements the target specific low level implementation.
 *
 * @copyright
 * Copyright 2010-2021.
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

#define GOAL_ID GOAL_ID_TGT

#include <goal_includes.h>
#include "goal_target_common.h"
#if GOAL_CONFIG_MCTC_CC == 1
# include "fsl_ele_base_api.h"
#endif /* GOAL_CONFIG_MCTC_CC == 1 */


/****************************************************************************/
/* Local Defines */
/****************************************************************************/
#define GOAL_ARCH_ELE_RESET_TIME 23 * 60 * 60 * 1000UL /**< 23h Reset time for EdgeLock Secure Enclave Ping */


/****************************************************************************/
/* extern variables */
/****************************************************************************/
extern volatile GOAL_BOOL_T flgIsr;


/****************************************************************************/
/* local variables */
/****************************************************************************/
/* Stage handler */
static GOAL_STAGE_HANDLER_T mStageInit;         /**< init stage handler */
static GOAL_STAGE_HANDLER_T mStageShutdown;     /**< shutdown stage handler */
#if GOAL_CONFIG_MCTC_CC == 1
static GOAL_STAGE_HANDLER_T mStageTimer;        /**< timer stage handler */
static GOAL_TIMER_T *mpTimerEdgeLock = NULL;    /**< Edgelock timer */
#endif /* GOAL_CONFIG_MCTC_CC == 1 */


/****************************************************************************/
/* local prototypes */
/****************************************************************************/
static GOAL_STATUS_T goal_targetCommonInit(
    void
);

static GOAL_STATUS_T goal_targetCommonShutdown(
    void
);

#if GOAL_CONFIG_MCTC_CC == 1
static GOAL_STATUS_T goal_targetCommonTimerInit(
    void
);

static void goal_targetPingECE(
    void *pArg                                  /**< unused arg */
);
#endif /* GOAL_CONFIG_MCTC_CC == 1 */


/****************************************************************************/
/** Register stage handler
 *
 * This function registers the primary target initialization function and the
 * shutdown function to the GOAL stage system.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_targetCommonInitPre(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* init stage */
    res = goal_mainStageReg(GOAL_STAGE_BOARD_PRE, &mStageInit, GOAL_STAGE_INIT, goal_targetCommonInit);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Failed to register target init function.");
    }

#if GOAL_CONFIG_MCTC_CC == 1
    /* Timer stage */
    if (GOAL_RES_OK(res)) {
        res = goal_mainStageReg(GOAL_STAGE_TIMER, &mStageTimer, GOAL_STAGE_INIT, goal_targetCommonTimerInit);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("Failed to register target timer function.");
        }
    }
#endif /* GOAL_CONFIG_MCTC_CC == 1 */

    /* shutdown stage */
    if (GOAL_RES_OK(res)) {
        res = goal_mainStageReg(GOAL_STAGE_BOARD_PRE, &mStageShutdown, GOAL_STAGE_SHUTDOWN, goal_targetCommonShutdown);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("Failed to register target shutdown function.");
        }
    }

    return res;
}


/****************************************************************************/
/** Target Initialization
 *
 * This function initializes the low level drivers used by GOAL.
 *
 * @retval GOAL_OK success
 * @retval other failed
 */
static GOAL_STATUS_T goal_targetCommonInit(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* board init */
    res = goal_targetBoardInit();

    return res;
}


#if GOAL_CONFIG_MCTC_CC == 1
/****************************************************************************/
/** Target Timer Initialization
 *
 * This function initializes the EdgeLock Timer.
 *
 * @retval GOAL_OK success
 * @retval other failed
 */
static GOAL_STATUS_T goal_targetCommonTimerInit(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* Create and setup timer to ping EdgeLock Secure Enclave every 23h */
    res = goal_timerCreate(&mpTimerEdgeLock, GOAL_TIMER_LOW);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("error during EdgeLock Ping timer creation");
        return res;
    }

    /* setup and start EdgeLock Secure Enclave timer */
    res = goal_timerSetup(mpTimerEdgeLock, GOAL_TIMER_PERIODIC, GOAL_ARCH_ELE_RESET_TIME, goal_targetPingECE, NULL, GOAL_TRUE);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("error while starting EdgeLock Ping timer");
        return res;
    }

    return res;
}


/****************************************************************************/
/** Ping EdgeLock Secure Enclave
 *
 * Pings the Edgelock Secure Enclave every 23h.
 * Otherwise the Board will reset after 24h.
 *
 * @returns void
 */
static void goal_targetPingECE(
    void *pArg                                  /**< unused arg */
)
{
    UNUSEDARG(pArg);
    goal_logDbg("Ping EdgeLock Secure Enclave IP FROM TIMER CB");
    ELE_BaseAPI_Ping(MU_RT_S3MUA);
}
#endif /* #if GOAL_CONFIG_MCTC_CC == 1 */


/****************************************************************************/
/** Shutdown the low level module
 *
 * Should do all necessary steps to shut down the low level module.
 *
 * @note On some architectures this halts the device.
 */
static GOAL_STATUS_T goal_targetCommonShutdown(
    void
)
{
    goal_targetHalt();

    return GOAL_OK;
}


/****************************************************************************/
/** Reset the system
 *
 * This function performs a soft reset of the platform.
 */
void goal_targetReset(
    void
)
{

}


/****************************************************************************/
/** Global Stack Halt Function
 *
 * This functions makes it easier to focus on one breakpoint while debugging.
 * See callstack for details of the caller.
 */
void goal_targetHalt(
    void
)
{
    for (;;);
}


/****************************************************************************/
/** Check if content is called from ISR
 *
 * @retval GOAL_TRUE if this function is called out of an ISR
 * @retval GOAL_FALSE otherwise
 */
GOAL_BOOL_T goal_targetCommonIsrCheck(
    void
)
{
    if (__get_IPSR())
    {
        return GOAL_TRUE;
    }

    return (GOAL_FALSE);
}
