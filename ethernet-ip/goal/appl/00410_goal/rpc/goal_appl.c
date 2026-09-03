/** @file
 *
 * @brief simple RPC application
 *
 * This module provides a simple RPC application using GOAL. The variable A
 * and variable B is transmitted to the server for multiplication.
 * The client receives the result as variable C and checks it.
 *
 * Note:    This demo application is meant to demonstrate the basic
 *          handling of this feature.
 *          It is not meant for performance benchmarking and it is
 *          not optimized for performance evaluation purposes.
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
#include <goal_media/goal_mi_mctc.h>


/****************************************************************************/
/* Defines */
/****************************************************************************/
#define GOAL_RPC_FUNC_TEST (0)                  /**< user function ID */
#define GOAL_CTC_TX_TIME (GOAL_TIMER_SEC * 5)   /**< cycle time for sending messages */
#define VAR_A 0x0123                            /**< variable A */
#define VAR_B 0x4567                            /**< variable B */
#define VAR_C (VAR_A * VAR_B)                   /**< result C */


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_RPC_HDL_CHN_T *pHdlRpcChn = GOAL_RPC_HDL_NONE; /**< RPC channel handle */
static GOAL_TIMESTAMP_T tsNext;                 /**< next timestamp */


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
static GOAL_STATUS_T goal_rpcUserFunction(
    uint32_t *pVarC,                            /**< result C */
    uint16_t varA,                              /**< variable A */
    uint16_t varB                               /**< variable B */
);

static GOAL_STATUS_T goal_rpcUserFunctionServer(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< RPC handle */
);

static GOAL_STATUS_T goal_userFunction(
    uint32_t *pVarC,                            /**< result C */
    uint16_t varA,                              /**< variable A */
    uint16_t varB                               /**< variable A */
);


/****************************************************************************/
/** RPC User Function
 *
 * The client pushes the transmit variables to the RPC stack and calls the
 * procedure at the server.
 * If the server response is valid, the result can be poped from the RPC
 * stack.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_rpcUserFunction(
    uint32_t *pVarC,                            /**< sum C */
    uint16_t varA,                              /**< variable A */
    uint16_t varB                               /**< variable B */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */

    /* get a new rpc handle */
    GOAL_RPC_NEW();

    /* push the value of Var A */
    GOAL_RPC_PUSH(varA);

    /* push the value of Var B */
    GOAL_RPC_PUSH(varB);

    /* call the procedure at the server */
    GOAL_RPC_USER_CALL(GOAL_RPC_FUNC_TEST);

    /* pop the value of Var B */
    GOAL_RPC_POP(*pVarC, uint32_t);

    /* rpc is done successfully */
    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** User Function - Server functionality
 *
 * The server execute the selected function and pushes the result to the
 * stack.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_rpcUserFunctionServer(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< RPC handle */
)
{
    uint16_t varA = 0;                          /* variable A */
    uint16_t varB = 0;                          /* variable B */
    uint32_t varC = 0;                          /* result C */
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    /* pop the value of Var B */
    GOAL_RPC_POP(varB, uint16_t);

    /* pop the value of Var A */
    GOAL_RPC_POP(varA, uint16_t);

    /* call the server function */
    if (GOAL_RES_OK(res)) {
        res = goal_userFunction(&varC, varA, varB);
    }

    /* push the value of Var C  */
    GOAL_RPC_PUSH(varC);

    return res;
}


/****************************************************************************/
/** User function
 *
 * This function is called at the server side and corresponds to the selected
 * non - RPC function.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_userFunction(
    uint32_t *pVarC,                            /**< pointer to result C */
    uint16_t varA,                              /**< variable A */
    uint16_t varB                               /**< variable B */
)
{
    if (VAR_A != varA) {
        goal_logErr("A doesn't match 0x%x vs 0x%x", varA, VAR_A);
    }
    if (VAR_B != varB) {
        goal_logErr("B doesn't match 0x%x vs 0x%x", varB, VAR_B);
    }

    /* calculate C */
    *pVarC = ((uint32_t) varA) * ((uint32_t) varB);

    return GOAL_OK;
}


/****************************************************************************/
/** Application Setup
 *
 * @result GOAL_STATUS_T result
 */
GOAL_STATUS_T appl_setup(
    void
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    /* setup a channel for RPC init */
    res = goal_rpcSetupChannel(&pHdlRpcChn, GOAL_ID_MI_CTC_DEFAULT);

    /* setup the service function */
    GOAL_RPC_USER_REGISTER_SERVICE(GOAL_RPC_FUNC_TEST, &goal_rpcUserFunctionServer);

    return res;
}


/****************************************************************************/
/** Main Loop
 *
 * This function must implement the application logic and must not block. It is
 * called in a loop (the GOAL loop) and if state tracking is necessary it
 * should use static or global variables.
 */
void appl_loop(
    void
)
{
    static GOAL_TIMESTAMP_T ts;                 /* timestamp */
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    uint32_t varC = 0;                          /* variable B */

    ts = goal_timerTsGet();

    if (tsNext < ts) {
        /* call the user function */
        res = goal_rpcUserFunction(&varC, VAR_A, VAR_B);
        if (GOAL_RES_OK(res)) {
            if (VAR_C != varC) {
                goal_logErr("C doesn't match 0x%"FMT_x32" vs 0x%x", varC, VAR_C);
            }
            else {
                goal_logInfo("C matches.");
            }
        }

        /* update next timestamp */
        tsNext = goal_timerTsGet() + GOAL_CTC_TX_TIME;
    }
}
