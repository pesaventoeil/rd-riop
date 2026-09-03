/** @file
 *
 * @brief  CTC wrapper for DD RPC functions
 *
 * @details
 * This module provides a CTC wrapper for DD specific functions
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


/****************************************************************************/
/* Defines */
/****************************************************************************/
#define GOAL_ID GOAL_ID_DD


/****************************************************************************/
/* Includes */
/****************************************************************************/
#include <goal_includes.h>

#include <goal_dd.h>
#include "goal_dd_rpc_ac.h"
#include "goal_dd_rpc.h"

#include <goal_media/goal_mi_mctc.h>


/****************************************************************************/
/* Structs */
/****************************************************************************/


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_STAGE_HANDLER_T stageInit;          /**< init stage handler */

static GOAL_RPC_HDL_CHN_T *pHdlRpcChn = GOAL_RPC_HDL_NONE; /**< acyclic transmission handle */
static GOAL_DD_T *pHdlDdInst;                   /**< local instance of DD */


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
static GOAL_STATUS_T goal_ddRpcPastInit(
    void
);

static GOAL_STATUS_T goal_ddRpcSetupChannels(
    void
);

static GOAL_STATUS_T goal_ddRpcRegisterServices(
    void
);

static GOAL_STATUS_T goal_ddCbServer(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< rpc handle */
);


/****************************************************************************/
/** RPC initialization
 *
 * This function initialize all necessary function to use NET via RPC.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_ddRpcInit(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* set  the size of the RPC stack */
    res = goal_rpcSetStackMax(RPC_BUFF_SIZE_DD);

    /* register the NET via RPC init function */
    if (GOAL_RES_OK(res)) {
        res = goal_mainStageReg(GOAL_STAGE_MODULES, &stageInit, GOAL_STAGE_INIT, goal_ddRpcPastInit);
    }

    return res;
}


/****************************************************************************/
/** Create instance of goal dd
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_ddRpcNew(
    GOAL_DD_T **ppHdlDd,                        /**< DD handle */
    uint32_t bitmaskFeatures                    /**< initial features */
)
{
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE;/* call handle */
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    /* create local instance */
    res = goal_memCalloc(ppHdlDd, sizeof(GOAL_DD_T));
    pHdlDdInst = *ppHdlDd;

    /* unused until extended functionality of wrapper */
    UNUSEDARG(pHdlDdInst);

    GOAL_RPC_NEW();
    GOAL_RPC_PUSH(bitmaskFeatures);
    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_DD, GOAL_RPC_FUNC_DD_NEW);
    }
    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** NET via RPC past initialization
 *
 * This function is called during the init stage of the GOAL_STAGE_CTC_PRE.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_ddRpcPastInit(
    void
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    /* setup the service function */
    if (GOAL_RES_OK(res)) {
        res = goal_ddRpcRegisterServices();
    }

    /* setup all necessary RPC channels */
    if (GOAL_RES_OK(res)) {
        res = goal_ddRpcSetupChannels();
    }

    return res;
}


/****************************************************************************/
/** Register server function
 *
 * This function registers all server functions.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_ddRpcRegisterServices(
    void
)
{
    GOAL_STATUS_T res;                          /* result */


    res = goal_rpcRegisterService(GOAL_ID_DD, GOAL_RPC_FUNC_DD_CB, &goal_ddCbServer);


    return res;
}


/****************************************************************************/
/** Setup channels for NET via RPC
 *
 * This function initialize the ctc channels for RPC.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_ddRpcSetupChannels(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* setup a channel for RPC */
    res = goal_rpcSetupChannel(&pHdlRpcChn, GOAL_ID_MI_CTC_DEFAULT);

    return res;
}


/** goal dd callback - server
 *
 * Pop the data from the RPC stack, call the selected function and send a
 * response to the client.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_ddCbServer(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< rpc handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    UNUSEDARG(pHdlRpc);

    return res;
}


/****************************************************************************/
/** set dd customer ID
 *
 * @retval GOAL_OK successful
 * @retval other failed
 *
 */
GOAL_STATUS_T goal_ddCustomerIdSet(
    GOAL_DD_T *pHdlDd,                          /**< dd handle */
    uint32_t cid                                /**< customer ID */
)
{
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE;/* call handle */
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    UNUSEDARG(pHdlDd);
    GOAL_RPC_NEW();
    GOAL_RPC_PUSH(cid);
    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_DD, GOAL_RPC_FUNC_DD_CFG_CUSTOMER_ID);
    }
    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** Add dd filter set to dd instance
 *
 * @retval GOAL_OK successful
 * @retval other failed
 *
 */
GOAL_STATUS_T goal_ddFilterAdd(
    GOAL_DD_T *pHdlDd,                          /**< dd handle */
    GOAL_DD_ACCESS_FILTER_SET_T setId           /**< set id */
)
{
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE;/* call handle */
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    UNUSEDARG(pHdlDd);
    GOAL_RPC_NEW();
    GOAL_RPC_PUSH(setId);
    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_DD, GOAL_RPC_FUNC_DD_FILTER_ADD);
    }
    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** set dd module name
 *
 * @retval GOAL_OK successful
 * @retval other failed
 *
 */
GOAL_STATUS_T goal_ddModuleNameSet(
    GOAL_DD_T *pHdlDd,                          /**< dd handle */
    uint8_t *str                                /**< module name */
)
{
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE;/* call handle */
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    uint16_t lenStr;                            /* string length */

    UNUSEDARG(pHdlDd);
    lenStr = (uint16_t) GOAL_STRLEN((const char *) str);

    if (lenStr > GOAL_DD_MODULE_NAME_LENGTH) {
        return GOAL_ERR_OVERFLOW;
    }

    GOAL_RPC_NEW();

    if (GOAL_RES_OK(res)) {
        res = goal_rpcArgPush(pHdlRpc, (void *) str, lenStr);
    }
    GOAL_RPC_PUSH(lenStr);

    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_DD, GOAL_RPC_FUNC_DD_CFG_MODULE_NAME);
    }
    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** set dd features
 *
 * @retval GOAL_OK successful
 * @retval other failed
 *
 */
GOAL_STATUS_T goal_ddFeaturesSet(
    GOAL_DD_T *pHdlDd,                          /**< dd handle */
    uint32_t bitmaskFeatures                    /**< bitmask with feature disable bits set */
)
{
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE;/* call handle */
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    UNUSEDARG(pHdlDd);
    GOAL_RPC_NEW();
    GOAL_RPC_PUSH(bitmaskFeatures);
    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_DD, GOAL_RPC_FUNC_DD_FEATURES_SET);
    }
    GOAL_RPC_CLOSE();

    return res;
}


#if 1 == GOAL_CONFIG_DD_CRYPT
/****************************************************************************/
/** set dd crypto activated flag
 *
 * @returns GOAL_STATUS_T value
 */
GOAL_STATUS_T goal_ddCryptActivationSet(
    GOAL_DD_T *pHdlDd,                          /**< dd handle */
    GOAL_BOOL_T flgActivate                     /**< activation flag for device detection encryption */
)
{
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE;/* call handle */
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    UNUSEDARG(pHdlDd);
    GOAL_RPC_NEW();
    GOAL_RPC_PUSH(flgActivate);
    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_DD, GOAL_RPC_FUNC_DD_CRYPT_ACTIVATION_SET);
    }
    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** set dd crypto password
 *
 * @returns GOAL_STATUS_T value
 */
GOAL_STATUS_T goal_ddCryptPasswordSet(
    GOAL_DD_T *pHdlDd,                          /**< dd handle */
    uint8_t *str                                /**< password */
)
{
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE;/* call handle */
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    uint16_t lenStr;                            /* string length */

    UNUSEDARG(pHdlDd);
    lenStr = (uint16_t) GOAL_STRLEN((const char *) str);

    if (lenStr > GOAL_DD_CRYPT_PASSWORD_LENGTH) {
        return GOAL_ERR_OVERFLOW;
    }

    GOAL_RPC_NEW();

    if (GOAL_RES_OK(res)) {
        res = goal_rpcArgPush(pHdlRpc, (void *) str, lenStr);
    }
    GOAL_RPC_PUSH(lenStr);

    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_DD, GOAL_RPC_FUNC_DD_CRYPT_PASSWORD_SET);
    }
    GOAL_RPC_CLOSE();

    return res;
}
#endif /* 1 == GOAL_CONFIG_DD_CRYPT */


/****************************************************************************/
/** set dd features
 *
 * @retval GOAL_OK successful
 * @retval other failed
 *
 */
GOAL_STATUS_T goal_ddCallbackReg(
    GOAL_DD_T *pHdlDd,                          /**< dd handle */
    GOAL_DD_FUNC_CB_T pFunc                     /**< callback function */
)
{
    UNUSEDARG(pHdlDd);
    UNUSEDARG(pFunc);

    return GOAL_OK;
}


/****************************************************************************/
/** Configure dd features for this session
 *
 * Settings in CM are not touched, therfore after reboot the initial settings
 * are reactivated
 *
 * Session settings overwrite CM settings
 *
 * @retval GOAL_OK successful
 * @retval other failed
 *
 */
GOAL_STATUS_T goal_ddSessionFeatureActivate(
    GOAL_DD_T *pHdlDd,                          /**< dd handle */
    uint32_t bitmaskFeatures                    /**< bitmask with feature enable bits set */
)
{
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE;/* call handle */
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    UNUSEDARG(pHdlDd);

    GOAL_RPC_NEW();
    GOAL_RPC_PUSH(bitmaskFeatures);
    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_DD, GOAL_RPC_FUNC_DD_SESSION_FEATURES_ACTIVATE);
    }
    GOAL_RPC_CLOSE();

    return res;
}
