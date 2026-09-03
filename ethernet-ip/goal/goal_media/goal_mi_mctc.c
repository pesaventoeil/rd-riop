/** @file
 *
 * @brief GOAL MCTC - Common Media Interface
 *
 * @copyright
 * Copyright 2010-2018.
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

#define GOAL_ID GOAL_ID_MI_MCTC
#include "goal_includes.h"
#include <goal_media/goal_mi_mctc.h>
#include <protos/goal_mctc/src/goal_mctc.h>
#include <protos/goal_mctc/src/goal_mctc_rb.h>
#include "goal_media/cm/goal_mi_mctc_cm.h"


/****************************************************************************/
/* Defines */
/****************************************************************************/
#define GOAL_MI_MCTC_HS_DELAY   (4 * GOAL_TIMER_SEC) /**< delay between handshake requests */
#define GOAL_MI_MCTC_RPC_RB_SIZE(x) (2 * x)     /**< ringbuffer size */


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/
static GOAL_STATUS_T goal_miMctcRpc(
    GOAL_MCTC_T *pMctc                          /**< MCTC handle */
);

static GOAL_STATUS_T goal_miMctcCbSetupStateGet(
    GOAL_MI_MCTC_T *pHdlMctcRx                  /**< MCTC handle */
);

static GOAL_STATUS_T goal_miMctcCbSetupDone(
    GOAL_MI_MCTC_T *pHdlMctcRx                  /**< MCTC handle */
);

static GOAL_STATUS_T goal_miMctcCbSetupCfgTimeout(
    GOAL_MI_MCTC_T *pHdlMctcRx                  /**< MCTC handle */
);

static GOAL_STATUS_T goal_miMctcDmCbRead(
    GOAL_MI_DM_GROUP_T *pGroup,                 /**< [in] MI DM group handle */
    void *pPriv                                 /**< [in] private pointer */
);

static void goal_miMctcRpcSyncLoop(
    void *pParam                                /**< loop parameter */
);

static GOAL_STATUS_T goal_miMctcRpcSend(
    GOAL_MI_MCTC_INST_T *pMiMctc,               /**< MI MCTC handle */
    uint8_t *pBuf,                              /**< buffer pointer */
    unsigned int len                            /**< buffer length */
);

static GOAL_BOOL_T goal_miMctcRpcSendAllow(
    GOAL_MI_MCTC_INST_T *pMiMctc                /**< MI MCTC handle */
);

#if (1 == GOAL_CONFIG_GEN_CTC_CC)
static GOAL_STATUS_T goal_miMctcEthCfgMacAddrServer(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< rpc handle */
);
#endif


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_MI_MCTC_FUNC_RPC_LIST_T *mpListRpc; /**< RPC function list */
static GOAL_INSTANCE_LIST_T *mpListInst;        /**< instance list */
static unsigned int mSizeXfer = GOAL_MI_MCTC_XFER_SIZE; /**< transfer size */
static unsigned int mCntHdl = GOAL_MI_MCTC_HDL_CNT; /**< handle count */
static uint32_t toutRpc = GOAL_MCTC_TIMEOUT_RX; /**< default rpc timeout */


/****************************************************************************/
/** Register stage handler
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_mctcInitPre(
    void
)
{
    /* stage registration for MCTC CM variables */
    return goal_mi_mctcRegCmVars();
}


/****************************************************************************/
/** MCTC MI - Registration
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miMctcReg(
    GOAL_MI_MCTC_INST_T **ppInst,               /**< [out] MCTC instance */
    unsigned int id,                            /**< instance id */
    const char *strDesc,                        /**< description */
    GOAL_MI_MCTC_FUNC_OPEN_T funcOpen,          /**< function open */
    GOAL_MI_MCTC_FUNC_NOTIFY_T funcNotifyCb     /**< function notify callback */
)
{
    GOAL_STATUS_T res;                          /* result */

    /* check */
    if (NULL == ppInst) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* create instance */
    res = goal_instNew((GOAL_INSTANCE_T **) ppInst, sizeof(GOAL_MI_MCTC_INST_T), GOAL_ID_MI_MCTC, id, strDesc);
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:271: */
        goal_lmLog(GOAL_ID_MI_MCTC, 271, 221, 4, GOAL_STRLEN((const char *) strDesc), GOAL_LOG_SEV_ERROR, "failed to create MCTC instance: $1 ($2)");
        goal_lmLogParamUINT32(id);
        goal_lmLogParamSTRING0(strDesc);
        goal_lmLogFinish();
        return res;
    }

    /* create instance lock */
    res = goal_lockCreate(GOAL_LOCK_BINARY, &(*ppInst)->pLock, 0, 1, GOAL_ID_MI_MCTC);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create MCTC instance lock");
        return res;
    }

    /* initialize MCTC handle */
    (*ppInst)->funcOpen = funcOpen;
    (*ppInst)->funcNotifyCb = funcNotifyCb;

    /* use instance list as indicator for needed initialization */
    if (NULL != mpListInst) {
        return GOAL_OK;
    }

    /* register generic MCTC function handler (ignore error if already registered) */
    res = goal_mctcRpcReg(0, 0, goal_miMctcRpc);
    if (GOAL_RES_ERR(res) && (GOAL_ERR_EXISTS != res)) {
        /* GG_LOG ID:272: */
        goal_lmLog(GOAL_ID_MI_MCTC, 272, 222, 0, 0, GOAL_LOG_SEV_ERROR, "failed to register generic RPC handler");
        return res;
    }

    /* get instance list */
    res = goal_instListGetByType(&mpListInst, GOAL_ID_MI_MCTC);
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:273: */
        goal_lmLog(GOAL_ID_MI_MCTC, 273, 223, 0, 0, GOAL_LOG_SEV_ERROR, "failed to fetch instance list");
        return res;
    }

    /* register loop handlers */
    res = goal_mainLoopParamReg(goal_miMctcRpcProcessLoop, *ppInst);
    if (GOAL_RES_ERR(res)) {
        goal_logDbg("failed to register MI MCTC RPC process loop");
    }

    res = goal_mainLoopParamReg(goal_miMctcRpcSyncLoop, *ppInst);
    if (GOAL_RES_ERR(res)) {
        goal_logDbg("failed to register MI MCTC RPC sync loop");
    }

    /* register RPC callbacks */
    res = goal_miMctcRpcReg(GOAL_ID_MI_MCTC, GOAL_MCTC_RPC_FUNC_SETUP_DONE, goal_miMctcCbSetupDone);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to register setup done callback");
        return res;
    }

    res = goal_miMctcRpcReg(GOAL_ID_MI_MCTC, GOAL_MCTC_RPC_FUNC_SETUP_STATE_GET, goal_miMctcCbSetupStateGet);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to register setup state get callback");
        return res;
    }

    res = goal_miMctcRpcReg(GOAL_ID_MI_MCTC, GOAL_MCTC_RPC_FUNC_SETUP_CFG_TOUT, goal_miMctcCbSetupCfgTimeout);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to register setup configuration of timeout callback");
        return res;
    }

#if (1 == GOAL_CONFIG_GEN_CTC_CC)
    res = goal_miMctcRpcReg(GOAL_ID_MI_MCTC, GOAL_MCTC_RPC_FUNC_CFG_ETH_MAC_ADDRESS_SET, goal_miMctcEthCfgMacAddrServer);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to register setup configuration of timeout callback");
        return res;
    }
#endif

    if (GOAL_RES_OK(res)) {
        res = goal_cmGetVarById(GOAL_ID_MI_MCTC, MI_MCTC_CM_VAR_STAT_RPC_COUNT, &(*ppInst)->pCmVarRpcCount);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_cmGetVarById(GOAL_ID_MI_MCTC, MI_MCTC_CM_VAR_STAT_RPC_TIMEOUTS, &(*ppInst)->pCmVarRpcTimeouts);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_cmGetVarById(GOAL_ID_MI_MCTC, MI_MCTC_CM_VAR_STAT_RPC_DELAY_MIN, &(*ppInst)->pCmVarRpcDelayMin);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_cmGetVarById(GOAL_ID_MI_MCTC, MI_MCTC_CM_VAR_STAT_RPC_DELAY_MEAN, &(*ppInst)->pCmVarRpcDelayMean);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_cmGetVarById(GOAL_ID_MI_MCTC, MI_MCTC_CM_VAR_STAT_RPC_DELAY_MAX, &(*ppInst)->pCmVarRpcDelayMax);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_cmGetVarById(GOAL_ID_MI_MCTC, MI_MCTC_CM_VAR_STAT_FRAME_VALID_COUNT, &(*ppInst)->pCmVarFrameValidCount);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_cmGetVarById(GOAL_ID_MI_MCTC, MI_MCTC_CM_VAR_STAT_FRAME_INVALID_COUNT, &(*ppInst)->pCmVarFrameInvalidCount);
    }

    return GOAL_OK;
}


/****************************************************************************/
/** MCTC MI - RPC Registration
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miMctcRpcReg(
    uint32_t idRpc,                             /**< RPC id */
    uint32_t idFunc,                            /**< function id */
    GOAL_MI_MCTC_FUNC_RPC_T funcRpc             /**< RPC function */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MI_MCTC_FUNC_RPC_LIST_T **ppItem;      /* list item ref */

    /* check if function is already registered */
    for (ppItem = &mpListRpc; *ppItem; ppItem = &(*ppItem)->pNext) {
        if (((*ppItem)->idRpc == idRpc) && ((*ppItem)->idFunc == idFunc)) {
            return GOAL_ERR_EXISTS;
        }
    }

    /* allocate function entry */
    res = goal_memCalloc(ppItem, sizeof(GOAL_MI_MCTC_FUNC_RPC_LIST_T));
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:278: */
        goal_lmLog(GOAL_ID_MI_MCTC, 278, 228, 0, 0, GOAL_LOG_SEV_ERROR, "failed to allocate RPC function item");
        return res;
    }

    /* initialize function entry */
    (*ppItem)->idRpc = idRpc;
    (*ppItem)->idFunc = idFunc;
    (*ppItem)->func = funcRpc;

    return GOAL_OK;
}


/****************************************************************************/
/** MCTC MI - Generic RPC Callback
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_miMctcRpc(
    GOAL_MCTC_T *pMctc                          /**< MCTC handle */
)
{
    GOAL_MI_MCTC_FUNC_RPC_LIST_T *pItem;        /* list item */

    UNUSEDARG(pMctc);

    /* find function */
    for (pItem = mpListRpc; pItem; pItem = pItem->pNext) {
        if ((pItem->idRpc == pMctc->idRpc) && (pItem->idFunc == pMctc->idFunc)) {
            return pItem->func(pMctc);
        }
    }

    /* GG_LOG ID:279: */
    goal_lmLog(GOAL_ID_MI_MCTC, 279, 229, 4, 4, GOAL_LOG_SEV_ERROR, "unregistered function called: $1 $2");
    goal_lmLogParamUINT32(pMctc->idRpc);
    goal_lmLogParamUINT32(pMctc->idFunc);
    goal_lmLogFinish();
    return GOAL_ERR_NOT_FOUND;
}


/****************************************************************************/
/** MCTC MI - Set maximum RPC transfer size
 *
 * This function defines the maximal size of the RPC stack in bytes. As long
 * as the RPC channel is not setup, this function can be called. If this
 * function is not called, the stack size is set to GOAL_MI_MCTC_XFER_SIZE.
 *
 * This function works after the "highest value wins" principle.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miMctcCfgXferSize(
    unsigned int sizeXfer                       /**< maximum transfer size */
)
{
    if (mSizeXfer < sizeXfer) {
        mSizeXfer = sizeXfer;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** MCTC MI - Get CTC transfer size
 *
 * @returns CTC transfer size
 */
GOAL_STATUS_T goal_miMctcXferSizeGet(
    unsigned int *pSizeXfer,                    /**< [out] transfer size */
    GOAL_MI_MCTC_INST_T *pInst                  /**< MCTC instance */
)
{
    UNUSEDARG(pInst);

    *pSizeXfer = mSizeXfer;

    return GOAL_OK;
}


/****************************************************************************/
/** MCTC MI - Set maximum RPC handle count
 *
 * This function configures the maximal count of RPC handles. As long as the
 * RPC channel is not setup, this function can be called. If this function is
 * not called, the stack size is set to GOAL_MI_MCTC_HDL_CNT.
 *
 * This function works after the "highest value wins" principle.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miMctcCfgHdlCnt(
    unsigned int cntHdl                         /**< maximum handle count */
)
{
    if (mCntHdl < cntHdl) {
        mCntHdl = cntHdl;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** MCTC MI - Open
 *
 * Open a registered MI MCTC instance.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miMctcOpen(
    GOAL_MI_MCTC_INST_T **ppInst,               /**< MCTC instance */
    unsigned int id                             /**< instance id */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MCTC_TGT_T *pHdlTgtRx = NULL;          /* MCTC RX target handle */
    GOAL_MCTC_TGT_T *pHdlTgtTx = NULL;          /* MCTC TX target handle */

    /* check */
    if (NULL == ppInst) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* get instance */
    res = goal_instGetById((GOAL_INSTANCE_T **) ppInst, GOAL_ID_MI_MCTC, id);
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:280: */
        goal_lmLog(GOAL_ID_MI_MCTC, 280, 230, 0, 0, GOAL_LOG_SEV_ERROR, "MCTC instance not found");
        return res;
    }

    /* check if instance is already initialized */
    if (GOAL_TRUE == (*ppInst)->flgInit) {
        return GOAL_OK;
    }
    (*ppInst)->flgInit = GOAL_TRUE;

    /* create MCTC RX ringbuffer */
    res = goal_mctcRbNew(&pHdlTgtRx, GOAL_MI_MCTC_RPC_RB_SIZE(mSizeXfer), mSizeXfer, NULL);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* create MCTC TX ringbuffer */
    res = goal_mctcRbNew(&pHdlTgtTx, GOAL_MI_MCTC_RPC_RB_SIZE(mSizeXfer), mSizeXfer, NULL);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* create MCTC instance */
    res = goal_mctcInstNew(&(*ppInst)->pInstMctc, pHdlTgtRx, pHdlTgtTx, mCntHdl, mSizeXfer);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* initialize private handle */
    (*ppInst)->pInstMctc->pPriv = *ppInst;

    /* open handler with maximal requested CTC transfer size */
    return (*ppInst)->funcOpen(*ppInst, mSizeXfer);
}


/****************************************************************************/
/** MCTC MI - Get Handle
 *
 * Allocate a MCTC transfer handle.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miMctcNew(
    GOAL_MI_MCTC_T **ppHdlMctc,                 /**< [out] MCTC handle */
    GOAL_MI_MCTC_INST_T *pInst                  /**< MCTC instance */
)
{
    /* check */
    if ((NULL == ppHdlMctc) || (NULL == pInst)) {
        return GOAL_ERR_NULL_POINTER;
    }

    return goal_mctcNew(ppHdlMctc, pInst->pInstMctc);
}


/****************************************************************************/
/** MCTC MI - Release Handle
 *
 * Free an allocated MCTC transfer handle.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miMctcRelease(
    GOAL_MI_MCTC_T *pHdlMctc                    /**< MCTC handle */
)
{
    /* check */
    if (NULL == pHdlMctc) {
        return GOAL_ERR_NULL_POINTER;
    }

    return goal_mctcRelease(pHdlMctc);
}


/****************************************************************************/
/** MCTC MI - Push data to MCTC stack
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miMctcPush(
    GOAL_MI_MCTC_T *pHdl,                       /**< MCTC handle */
    const uint8_t *pData,                       /**< data */
    unsigned int len                            /**< data length */
)
{
    GOAL_STATUS_T res;                          /* result */

    /* check */
    if (NULL == pHdl) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* push data */
    res = goal_mctcPush(pHdl, pData, len);
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:283: */
        goal_lmLog(GOAL_ID_MI_MCTC, 283, 233, 0, 0, GOAL_LOG_SEV_ERROR, "failed to push data to MCTC stack");
        return res;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** MCTC MI - Pop data from MCTC stack
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miMctcPop(
    GOAL_MI_MCTC_T *pHdl,                       /**< MCTC handle */
    uint8_t *pData,                             /**< [out] data */
    unsigned int len                            /**< data length */
)
{
    GOAL_STATUS_T res;                          /* result */

    /* check */
    if (NULL == pHdl) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* pop data */
    res = goal_mctcPop(pHdl, pData, len);
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:284: */
        goal_lmLog(GOAL_ID_MI_MCTC, 284, 234, 0, 0, GOAL_LOG_SEV_ERROR, "failed to pop data to MCTC stack");
        return res;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** MCTC MI - Call remote function and wait for a result
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miMctcSendReq(
    GOAL_MI_MCTC_T *pHdl,                       /**< MCTC handle */
    uint32_t idRpc,                             /**< RPC id */
    uint32_t idFunc                             /**< function id */
)
{
    GOAL_STATUS_T res;                          /* result */
    struct GOAL_MI_MCTC_INST_T *pMiMctc;        /* MI MCTC handle */

    /* check */
    if (NULL == pHdl) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* refer the MI handle */
    pMiMctc = (struct GOAL_MI_MCTC_INST_T *) pHdl->pInst->pPriv;

    /* remote call */
    res = goal_mctcSendReq(pHdl, idRpc, idFunc);
    if (GOAL_ERR_TIMEOUT == res) {
        if ((GOAL_MI_MCTC_RPC_STATE_RUN == pMiMctc->stateRpc) && (GOAL_TRUE == pMiMctc->flgStateSync)) {

            goal_logErr("request failed");
        }
    }

    return res;
}


/****************************************************************************/
/** MCTC MI - Call remote function without waiting for a result
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miMctcSendInfo(
    GOAL_MI_MCTC_T *pHdl,                       /**< MCTC handle */
    uint32_t idRpc,                             /**< RPC id */
    uint32_t idFunc                             /**< function id */
)
{
    /* check */
    if (NULL == pHdl) {
        return GOAL_ERR_NULL_POINTER;
    }

    goal_logDbg("MCTC info");

    /* remote call */
    return goal_mctcSendInfo(pHdl, idRpc, idFunc);
}


/****************************************************************************/
/** MCTC MI - RPC Setup Done Response Callback
 *
 * Gets called when a setup done was received.
 * This function is only executed on the CC after AC is done with
 * his setup routine.
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_miMctcCbSetupDone(
    GOAL_MI_MCTC_T *pHdlMctcRx                  /**< MCTC handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_MI_MCTC_INST_T *pMiMctc = NULL;        /* MI MCTC */
    uint32_t idInst = 0;                        /* instance id */

    /* instance id */
    GOAL_MCTC_POP_CB(idInst, uint32_t);
    if (GOAL_RES_OK(res)) {
        res = goal_instGetById((GOAL_INSTANCE_T **) &pMiMctc, GOAL_ID_MI_MCTC, idInst);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
    }

    /* lock flag access */
    res = goal_lockGet(pMiMctc->pLock, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* check if local setup is already done */
    if (GOAL_TRUE == pMiMctc->flgSetupLocal) {

        /* mark partner setup as done */
        pMiMctc->flgSetupRemote = GOAL_TRUE;

        /* lock memory allocation */
        goal_memInitDone();
    } else {
        /* mark the setup remote flag as received
         * the evaluation will be done in goal_miMctcRpcSyncLoop */
        pMiMctc->flgSetupRemoteRx = GOAL_TRUE;
    }

    /* unlock flag access */
    goal_lockPut(pMiMctc->pLock);

    return res;
}


/****************************************************************************/
/** MCTC MI - RPC Setup State Get Callback
 *
 * Request the current initialization state.
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_miMctcCbSetupStateGet(
    GOAL_MI_MCTC_T *pHdlMctcRx                  /**< MCTC handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_MI_MCTC_INST_T *pMiMctc = NULL;        /* MI MCTC */
    uint32_t idInst = 0;                        /* instance id */

    /* instance id */
    GOAL_MCTC_POP_CB(idInst, uint32_t);
    if (GOAL_RES_OK(res)) {
        res = goal_instGetById((GOAL_INSTANCE_T **) &pMiMctc, GOAL_ID_MI_MCTC, idInst);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
    }

    /* lock flag access */
    res = goal_lockGet(pMiMctc->pLock, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* return initialization state */
    GOAL_MCTC_PUSH_CB(pMiMctc->flgSetupLocal);
    GOAL_MCTC_PUSH_CB(pMiMctc->flgSetupRemote);

    /* unlock flag access */
    goal_lockPut(pMiMctc->pLock);

    return res;
}


/****************************************************************************/
/** MCTC MI - RPC Setup Configure Timeout Callback
 *
 * Configure timeout values for media and rpc by the peer
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_miMctcCbSetupCfgTimeout(
    GOAL_MI_MCTC_T *pHdlMctcRx                  /**< MCTC handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_MI_MCTC_INST_T *pMiMctc = NULL;        /* MI MCTC */
    uint32_t idInst = 0;                        /* instance id */
    uint32_t cfgTimeoutValue = 0;               /* configured timeout value */

    /* instance id */
    GOAL_MCTC_POP_CB(idInst, uint32_t);
    if (GOAL_RES_OK(res)) {
        res = goal_instGetById((GOAL_INSTANCE_T **) &pMiMctc, GOAL_ID_MI_MCTC, idInst);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
    }

    /* lock flag access */
    res = goal_lockGet(pMiMctc->pLock, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* get timeout value for media timeout */
    GOAL_MCTC_POP_CB(cfgTimeoutValue, uint32_t);

    /* set mi timeout (SPI = 1 second) */
    if (GOAL_RES_OK(res)) {
        goal_logInfo("setting media timeout to %"FMT_u32" ", cfgTimeoutValue);
        pMiMctc->cfgToutValue = cfgTimeoutValue;
    }

    /* get timeout value for rpc timeout */
    GOAL_MCTC_POP_CB(cfgTimeoutValue, uint32_t);
    /* set rpc timeout (RPC = 3 seconds) */
    if (GOAL_RES_OK(res)) {
        goal_logInfo("setting RPC timeout to %"FMT_u32" ", cfgTimeoutValue);
        res = goal_mctcToutSet(pMiMctc->pInstMctc, cfgTimeoutValue);
    }

    if (0 == pMiMctc->cfgToutValue) {
        goal_logWarn("media timeout disabled, peer loss will not be detected. Don\'t use in productive system!");
    }

    /* unlock flag access */
    goal_lockPut(pMiMctc->pLock);

    return res;
}


/****************************************************************************/
/** MCTC MI - Configure initial timeout values
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miMctcCfgTout(
    uint32_t toutRpcValue                       /**< initial rpc timeout */
)
{
    GOAL_STATUS_T res;                          /* result */

    toutRpc = toutRpcValue;

    res = goal_mctcCfgTout(toutRpc);

    return res;
}


/****************************************************************************/
/** MCTC MI - RPC Status
 *
 * This function checks, whether a suitable partner for communication was
 * found and the exchange of data is possible.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miMctcStatusGet(
    unsigned int id                             /**< instance id */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MI_MCTC_INST_T *pInst = NULL;          /* MCTC instance */

    res = goal_instGetById((GOAL_INSTANCE_T **) &pInst, GOAL_ID_MI_MCTC, id);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    if ((GOAL_MI_MCTC_RPC_STATE_RUN == pInst->stateRpc) && (GOAL_TRUE == pInst->flgStateSync)) {
        return GOAL_OK;
    }

    return GOAL_ERR_BUSY;
}


/****************************************************************************/
/** MCTC MI - Instance Get By Id
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miMctcInstGetById(
    GOAL_MI_MCTC_INST_T **ppMiMctcInst,         /**< [out] MCTC MI instance */
    uint32_t id                                 /**< MCTC MI instance id */
)
{
    static uint32_t idCache = UINT32_MAX;       /* cached id */
    static GOAL_MI_MCTC_INST_T *pMiMctcInst = NULL; /* cached instance */
    GOAL_STATUS_T res;                          /* result */

    if (idCache == id) {
        *ppMiMctcInst = pMiMctcInst;
        return GOAL_OK;
    }

    res = goal_instGetById((GOAL_INSTANCE_T **) ppMiMctcInst, GOAL_ID_MI_MCTC, id);
    if (GOAL_RES_OK(res)) {
        idCache = id;
        pMiMctcInst = *ppMiMctcInst;
    }

    return res;
}


/****************************************************************************/
/** MCTC MI - RPC to MI DM Mapping
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miMctcRpcDmMap(
    GOAL_MI_MCTC_INST_T *pMiMctc,               /**< [in] MI MCTC instance */
    GOAL_MI_DM_PART_T *pPartRead,               /**< [in] read partition */
    GOAL_MI_DM_PART_T *pPartWrite               /**< [in] write partition */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MI_DM_T *pDm;                          /* DM handle */
    unsigned int cnt;                           /* counter */

    /* retrieve write partition size */
    pMiMctc->sizeRpc = goal_miDmPartSizeGet(pPartWrite);

    /* allocate RPC TX buffers */
    for (cnt = 0; cnt < GOAL_MI_MCTC_RPC_CNT_RESEND; cnt++) {
        res = goal_memCalloc(&pMiMctc->pBufRpcTx[cnt], pMiMctc->sizeRpc);
        if (GOAL_RES_ERR(res)) {
            /* GG_LOG ID:302: */
            goal_lmLog(GOAL_ID_MI_CTC_SPI, 302, 252, 0, 0, GOAL_LOG_SEV_ERROR, "failed to allocate RX/TX resend buffer");
            return res;
        }
    }

    /* subtract RPC header from available payload size */
    pMiMctc->sizeRpc -= sizeof(GOAL_MI_MCTC_RPC_TX_T);

    /* store DM handles */
    pMiMctc->pDmPartRead = pPartRead;
    pMiMctc->pDmPartWrite = pPartWrite;

    /* retrieve DM handle */
    res = goal_miDmInstGetByPart(&pDm, pPartRead);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* add read callback */
    res = goal_miDmCbReg(NULL, pPartRead->pGroup, GOAL_MI_DM_CB_READ, goal_miMctcDmCbRead, pMiMctc);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** MCTC MI - DM Partition Read Update Callback
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_miMctcDmCbRead(
    GOAL_MI_DM_GROUP_T *pGroup,                 /**< [in] MI DM group handle */
    void *pPriv                                 /**< [in] private pointer */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MI_MCTC_INST_T *pMiMctc;               /* MI MCTC instance */
    uint8_t *pBufRpc = NULL;                    /* RPC buffer */
    uint32_t len = 0;                           /* RPC buffer length */
    GOAL_MI_MCTC_RPC_RX_T *pHdr;                /* RPC header */
    GOAL_MI_MCTC_RPC_DATA_LENGTH_T lenData;     /* RPCA data length in host format */
    unsigned int valDiff;                       /* difference value */

    UNUSEDARG(pGroup);

    /* assign instance */
    pMiMctc = (GOAL_MI_MCTC_INST_T *) pPriv;

    /* get partition buffer address */
    res = goal_miDmSingleReadBufGet(&pBufRpc, &len, pMiMctc->pDmPartRead);
    if (GOAL_RES_ERR(res)) {
        return GOAL_OK;
    }

    /* assign frame header */
    pHdr = (GOAL_MI_MCTC_RPC_RX_T *) pBufRpc;

    /* get the data length information from RPC data buffer */
    lenData = GOAL_MI_MCTC_RPC_DATA_LENGTH_letoh(pHdr->lenData_le);

    /* check frame CRC */
    res = goal_miMctcCrc(((uint8_t *) pHdr) + sizeof(GOAL_MI_MCTC_RPC_RX_T), lenData, GOAL_le16toh_p(&pHdr->crc_le16));
    if (GOAL_RES_ERR(res)) {
        return GOAL_OK;
    }

    goal_logDbg("received frame: received local ack: %u, local seq: %u, remote seq: %u, my remote ack: %u, flags: 0x%x", pHdr->seqLocalAck, pMiMctc->seqRpcLocal, pHdr->seqRemote, pMiMctc->seqRpcRemoteAck, pHdr->flags);

    /* store remote flags */
    pMiMctc->flgRpcRemote = pHdr->flags;

    /* remove an active item on receive */
    if (0 < pMiMctc->cntRpcActive) {
        pMiMctc->cntRpcActive--;
    }

    /* skip processing if sync mode is requested */
    if (GOAL_MI_MCTC_RPC_FLG_SYNC_REQ & pHdr->flags) {
        if (GOAL_MI_MCTC_RPC_STATE_STOP != pMiMctc->stateRpc) {
            pMiMctc->stateRpc = GOAL_MI_MCTC_RPC_STATE_SYNC_INIT;
        }
        return GOAL_OK;
    }

    /* check if an acknowledge must be sent only on new frames */
    if (pMiMctc->seqRpcRemoteAck != pHdr->seqRemote) {
        if (GOAL_MI_MCTC_RPC_FLG_REQ_ACK & pHdr->flags) {
            /* lock flag access */
            res = goal_lockGet(pMiMctc->pLock, GOAL_LOCK_INFINITE);
            if (GOAL_RES_ERR(res)) {
                return res;
            }
            /* send acknowledge */
            pMiMctc->flgRpcAck = (GOAL_MI_MCTC_RPC_STATE_STOP != pMiMctc->stateRpc) ? GOAL_TRUE : GOAL_FALSE;
            /* unlock flag access */
            goal_lockPut(pMiMctc->pLock);
        }
    }

    /* accept remote frame if sequence is ascending */
    if (pHdr->seqRemote == ((pMiMctc->seqRpcRemoteAck + 1) % GOAL_MI_MCTC_RPC_MAX_SEQ)) {

        goal_logDbg("remote seq accepted: %u", pHdr->seqRemote);

        /* update remote sequence tracker */
        pMiMctc->seqRpcRemoteAck = pHdr->seqRemote;

        /* put RX data into ringbuffer */
        goal_logDbg("added %u bytes", lenData);
        res = goal_mctcRbPut(pMiMctc->pInstMctc->pTgtRx->pPriv, ((uint8_t *) pHdr) + sizeof(GOAL_MI_MCTC_RPC_RX_T), lenData);
        if (GOAL_RES_ERR(res)) {
            /* GG_LOG ID:314: */
            goal_lmLog(GOAL_ID_MI_CTC, 314, 264, 0, 0, GOAL_LOG_SEV_ERROR, "failed to put data into RX ringbuffer");
            return GOAL_OK;
        }

        /* lock flag access */
        res = goal_lockGet(pMiMctc->pLock, GOAL_LOCK_INFINITE);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
        /* send acknowledge */
        pMiMctc->flgRpcAck = GOAL_TRUE;
        /* unlock flag access */
        goal_lockPut(pMiMctc->pLock);
    }

    /* track local sequence acknowledge */
    valDiff = (pHdr->seqLocalAck >= pMiMctc->seqRpcLocalAck) ? pHdr->seqLocalAck - pMiMctc->seqRpcLocalAck : pHdr->seqLocalAck + GOAL_MI_MCTC_RPC_MAX_SEQ - pMiMctc->seqRpcLocalAck;
    goal_logDbg("current local ack: %u, new local ack: %u, diff: %u", pMiMctc->seqRpcLocalAck, pHdr->seqLocalAck, valDiff);
    if (GOAL_MI_MCTC_RPC_CNT_RESEND >= valDiff) {
        pMiMctc->seqRpcLocalAck = pHdr->seqLocalAck;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** MCTC MI - Check calculated vs given CRC
 *
 * @returns GOAL_STATUS_T
 */
GOAL_STATUS_T goal_miMctcCrc(
    uint8_t *pData,                             /**< data */
    unsigned int len,                           /**< data length */
    uint16_t valCrc                             /**< expected CRC */
)
{
    uint16_t valCrcCalc;                        /* calculated CRC */

    valCrcCalc = goal_utilCrcFletcher16(pData, (int) len) + GOAL_MI_MCTC_FLETCHER16_OFFSET;

    return (valCrc == valCrcCalc) ? GOAL_OK : GOAL_ERROR;
}


/****************************************************************************/
/** MCTC MI - RPC Send Processing
 *
 * @returns GOAL_STATUS_T
 */
GOAL_STATUS_T goal_miMctcRpcTxProcess(
    GOAL_MI_MCTC_INST_T *pMiMctc                /**< MI MCTC handle */
)
{
    GOAL_STATUS_T res;                          /* result */
    unsigned int valDiff;                       /* difference value */
    unsigned int lenData;                       /* data length */
    GOAL_BOOL_T flgRpcAck;                      /* local RPC acknowledge flag */

    /* check if remote got all our frames */
    valDiff = (pMiMctc->seqRpcLocal >= pMiMctc->seqRpcLocalAck) ? pMiMctc->seqRpcLocal - pMiMctc->seqRpcLocalAck : pMiMctc->seqRpcLocal + GOAL_MI_MCTC_RPC_MAX_SEQ - pMiMctc->seqRpcLocalAck;
    if (GOAL_MI_MCTC_RPC_CNT_RESEND <= valDiff) {

        /* update remote sequence */
        GOAL_MASK_SET(pMiMctc->flgRpc, GOAL_MI_MCTC_RPC_FLG_RESEND);
        GOAL_MASK_SET(pMiMctc->flgRpc, GOAL_MI_MCTC_RPC_FLG_REQ_ACK);
        goal_miMctcRpcSend(pMiMctc, pMiMctc->pBufRpcTx[pMiMctc->cntResend], pMiMctc->lenBufRpcTx[pMiMctc->cntResend]);
        return GOAL_OK;
    }

    /* get data length from TX rb */
    lenData = goal_mctcRbLenGet(pMiMctc->pInstMctc->pTgtTx->pPriv);
    if (lenData > pMiMctc->sizeRpc) {
        lenData = pMiMctc->sizeRpc;
    }

    /* lock flag access */
    res = goal_lockGet(pMiMctc->pLock, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* get acknowlege flag and remove it from the handle afterwards */
    flgRpcAck = pMiMctc->flgRpcAck;

    /* remove acknowledge flag */
    pMiMctc->flgRpcAck = GOAL_FALSE;

    /* unlock flag access */
    goal_lockPut(pMiMctc->pLock);

    /* skip processing if no data is available, no ACK is needed and sequence is confirmed */
    if ((0 == lenData) && (GOAL_FALSE == flgRpcAck) && (pMiMctc->seqRpcLocal == pMiMctc->seqRpcLocalAck)) {
        return GOAL_OK;
    }

    /* get TX data from ringbuffer */
    if (0 != lenData) {
        res = goal_mctcRbPeek(pMiMctc->pInstMctc->pTgtTx->pPriv, pMiMctc->pBufRpcTx[pMiMctc->cntResend], 0, lenData);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
    }

    /* build RPC header */
    pMiMctc->lenBufRpcTx[pMiMctc->cntResend] = lenData;

    /* if previous and this frame are empty, don't increase the sequence counter */
    if (!((GOAL_TRUE == pMiMctc->flgRpcPrevEmpty) && (0 == lenData))) {
        pMiMctc->seqRpcLocal = (pMiMctc->seqRpcLocal + 1) % GOAL_MI_MCTC_RPC_MAX_SEQ;
    } else {
        goal_logDbg("previous frame == empty, keeping seq");
    }

    GOAL_MASK_CLR(pMiMctc->flgRpc, GOAL_MI_MCTC_RPC_FLG_RESEND);
    GOAL_MASK_CLR(pMiMctc->flgRpc, GOAL_MI_MCTC_RPC_FLG_REQ_ACK);
    goal_miMctcRpcSend(pMiMctc, pMiMctc->pBufRpcTx[pMiMctc->cntResend], pMiMctc->lenBufRpcTx[pMiMctc->cntResend]);

    /* update resend */
    pMiMctc->cntResend = (pMiMctc->cntResend + 1) % GOAL_MI_MCTC_RPC_CNT_RESEND;

    /* remove data from ringbuffer */
    if (0 != lenData) {
        goal_mctcRbRemove(pMiMctc->pInstMctc->pTgtTx->pPriv, lenData);
    }

    /* get statistics */
    if (pMiMctc->pCmVarRpcTimeouts) {
        GOAL_CM_SETVAR_UINT32(pMiMctc->pCmVarRpcTimeouts, pMiMctc->pInstMctc->cntRpcTimeout);
    }

    if (pMiMctc->pCmVarRpcCount) {
        GOAL_CM_SETVAR_UINT32(pMiMctc->pCmVarRpcCount, pMiMctc->pInstMctc->cntRpc);
    }

    if (pMiMctc->pCmVarRpcDelayMin) {
        GOAL_CM_SETVAR_UINT32(pMiMctc->pCmVarRpcDelayMin, pMiMctc->pInstMctc->tsRpcMin);
    }

    if (pMiMctc->pCmVarRpcDelayMax) {
        GOAL_CM_SETVAR_UINT32(pMiMctc->pCmVarRpcDelayMax, pMiMctc->pInstMctc->tsRpcMax);
    }

    if (pMiMctc->pCmVarRpcDelayMean) {
        GOAL_CM_SETVAR_UINT32(pMiMctc->pCmVarRpcDelayMean, pMiMctc->pInstMctc->tsRpcMean);
    }

    return GOAL_OK;
}


/****************************************************************************/
/** MCTC MI - RPC Process Loop
 */
void goal_miMctcRpcProcessLoop(
    void *pParam                                /**< loop parameter */
)
{
    GOAL_MI_MCTC_INST_T *pMiMctc = pParam;      /* MI MCTC handle */

    /* process RX data */
    pMiMctc->pInstMctc->pTgtRx->funcRecv(pMiMctc->pInstMctc, pMiMctc->pInstMctc->pTgtRx);

    /* leave if not in run-mode */
    if (GOAL_MI_MCTC_RPC_STATE_RUN != pMiMctc->stateRpc) {
        return;
    }

    /* check if send is permitted */
    if (GOAL_TRUE != goal_miMctcRpcSendAllow(pMiMctc)) {
        return;
    }

    /* process TX data */
    goal_miMctcRpcTxProcess(pMiMctc);
}


/****************************************************************************/
/** MCTC MI - RPC Sync Loop
 */
static void goal_miMctcRpcSyncLoop(
    void *pParam                                /**< loop parameter */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_MI_MCTC_INST_T *pMiMctc = pParam;      /* MI MCTC handle */
    GOAL_BOOL_T flgSetupLocal = GOAL_FALSE;     /* local setup done flag */
    GOAL_BOOL_T flgSetupRemote = GOAL_FALSE;    /* remote setup done flag */
    GOAL_RPC_HDL_T *pHdlMctcTx = GOAL_RPC_HDL_NONE; /* call handle */

    /* leave if not in sync-mode */
    if ((GOAL_MI_MCTC_RPC_STATE_RUN == pMiMctc->stateRpc)
        || (GOAL_MI_MCTC_RPC_STATE_STOP == pMiMctc->stateRpc)) {
        return;
    }

    /* check if send is permitted */
    if (GOAL_TRUE != goal_miMctcRpcSendAllow(pMiMctc)) {
        return;
    }

    /* handle state machine */
    switch (pMiMctc->stateRpc) {

        case GOAL_MI_MCTC_RPC_STATE_RUN_ONCE:

            goal_logDbg("state: RUN_ONCE");

            pMiMctc->stateRpc = GOAL_MI_MCTC_RPC_STATE_RUN;

            /* check device state of partner device */
            GOAL_MCTC_NEW(pMiMctc);
            GOAL_MCTC_PUSH_REQ(pMiMctc->idInst);
            GOAL_MCTC_SEND_REQ(GOAL_ID_MI_MCTC, GOAL_MCTC_RPC_FUNC_SETUP_STATE_GET);
            GOAL_MCTC_POP_REQ(flgSetupLocal, GOAL_BOOL_T);
            GOAL_MCTC_POP_REQ(flgSetupRemote, GOAL_BOOL_T);
            GOAL_MCTC_RELEASE();

            /* reset to sync state as partner isn't ready for setup */
            if (GOAL_RES_ERR(res)) {
                pMiMctc->stateRpc = GOAL_MI_MCTC_RPC_STATE_SYNC_INIT;
                break;
            }

            /* compare current local state vs reported by partner */
            if (pMiMctc->flgSetupLocal != flgSetupLocal) {

                /* local device needs a reset */
                if (GOAL_TRUE == pMiMctc->flgSetupLocal) {

                    goal_logWarn("sync needs local reset to proceed");

                    /* halt RPC */
                    pMiMctc->stateRpc = GOAL_MI_MCTC_RPC_STATE_STOP;

                    /* inform callback listeners */
                    GOAL_MI_MCTC_CB_RESET(pMiMctc);

                    break;
                }

                goal_logWarn("sync needs remote reset to proceed");

                /* remote reset is needed, stay in sync mode */
                pMiMctc->stateRpc = GOAL_MI_MCTC_RPC_STATE_SYNC_INIT;

                break;
            }

            /* compare current remote state vs reported by partner */
            if (pMiMctc->flgSetupRemote != flgSetupRemote) {

                /* local device needs a reset */
                if (GOAL_TRUE == pMiMctc->flgSetupRemote) {

                    goal_logWarn("sync needs local reset to proceed");

                    /* halt RPC */
                    pMiMctc->stateRpc = GOAL_MI_MCTC_RPC_STATE_STOP;

                    /* inform callback listeners */
                    GOAL_MI_MCTC_CB_RESET(pMiMctc);

                    break;
                }

                /* remote device needs a reset */
                if (GOAL_FALSE == pMiMctc->flgSetupRemoteRx) {
                    goal_logWarn("sync needs remote reset to proceed");

                    /* remote reset is needed, stay in sync mode */
                    pMiMctc->stateRpc = GOAL_MI_MCTC_RPC_STATE_SYNC_INIT;

                    break;
                }
            }

            /* leave if both devices are fully initialized */
            if ((GOAL_TRUE == flgSetupLocal) && (GOAL_TRUE == flgSetupRemote)) {
                break;
            }

            /* mark local setup as done */
            goal_logInfo("RPC state synchronized, running appl_setup");
            pMiMctc->flgStateSync = GOAL_TRUE;

#if GOAL_CONFIG_MCTC_AC == 1
            goal_logInfo("configuring timeout values for peer");

            /* the application controller may configure remote timeouts */
            GOAL_MCTC_NEW(pMiMctc);
            /* push rpc timeout */
            GOAL_MCTC_PUSH_REQ(toutRpc);
            /* push media timeout */
            GOAL_MCTC_PUSH_REQ(pMiMctc->cfgToutValue);
            GOAL_MCTC_PUSH_REQ(pMiMctc->idInst);
            GOAL_MCTC_SEND_REQ(GOAL_ID_MI_MCTC, GOAL_MCTC_RPC_FUNC_SETUP_CFG_TOUT);
            /* pop rpc timeout */
            GOAL_MCTC_RELEASE();
#endif

            /* call application setup */
            res = appl_setup();
            if (GOAL_RES_ERR(res)) {
                break;
            }

            /* register application loop */
            res = goal_mainLoopReg(appl_loop);
            if (GOAL_RES_ERR(res)) {
                /* GG_LOG ID:147: */
                goal_lmLog(GOAL_ID_MAIN, 147, 117, 0, 0, GOAL_LOG_SEV_ERROR, "registering application loop failed");
                return;
            }

#if GOAL_CONFIG_ETHERNET == 1
            /* signalize finish of application setup, only required by special platforms */
            goal_ethCmd(GOAL_ETH_CMD_SETUP_DONE, GOAL_FALSE, 0, NULL);
#endif

            /* inform partner that appl_setup is finished */
            GOAL_MCTC_NEW(pMiMctc);
            GOAL_MCTC_PUSH_REQ(pMiMctc->idInst);
            GOAL_MCTC_SEND_INFO(GOAL_ID_MI_MCTC, GOAL_MCTC_RPC_FUNC_SETUP_DONE);
            GOAL_MCTC_RELEASE();

            /* lock flag access */
            res = goal_lockGet(pMiMctc->pLock, GOAL_LOCK_INFINITE);
            if (GOAL_RES_ERR(res)) {
                return;
            }

            /* mark local setup as done */
            goal_logInfo("local setup done");
            pMiMctc->flgSetupLocal = GOAL_TRUE;

            /* check if remote setup flag has been received */
            if (GOAL_TRUE == pMiMctc->flgSetupRemoteRx) {
                pMiMctc->flgSetupRemote = GOAL_TRUE;

                /* reset the setup received flag */
                pMiMctc->flgSetupRemoteRx = GOAL_FALSE;
            }

            /* check if memory is already locked or needs to be locked here */
            if (GOAL_TRUE == pMiMctc->flgSetupRemote) {

                /* lock memory allocation */
                goal_memInitDone();
            }

            /* unlock flag access */
            goal_lockPut(pMiMctc->pLock);

            break;

        /* initialize synchronisation */
        case GOAL_MI_MCTC_RPC_STATE_SYNC_INIT:

            goal_logDbg("state: SYNC_INIT");

            /* reset sync state */
            GOAL_MASK_SET(pMiMctc->flgRpc, GOAL_MI_MCTC_RPC_FLG_SYNC_REQ);
            GOAL_MASK_CLR(pMiMctc->flgRpc, GOAL_MI_MCTC_RPC_FLG_SYNC_ACK);
            pMiMctc->seqRpcLocal = 0;
            pMiMctc->seqRpcLocalAck = 0;
            pMiMctc->seqRpcRemoteAck = 0;
            pMiMctc->stateRpc = GOAL_MI_MCTC_RPC_STATE_SYNC_REQ;

            /* intended fallthrough */
            GOAL_TARGET_FALLTHROUGH;

        /* request synchronisation from partner */
        case GOAL_MI_MCTC_RPC_STATE_SYNC_REQ:

            goal_logDbg("state: SYNC_REQ");

            /* send RPC sync frame until ACK is received */
            if (!(GOAL_MI_MCTC_RPC_FLG_SYNC_ACK & pMiMctc->flgRpcRemote)) {

                /* send sync request frame */
                goal_miMctcRpcSend(pMiMctc, NULL, 0);

                break;
            }

            /* request acknowledge of first sequence */
            GOAL_MASK_CLR(pMiMctc->flgRpc, GOAL_MI_MCTC_RPC_FLG_SYNC_REQ);
            GOAL_MASK_SET(pMiMctc->flgRpc, GOAL_MI_MCTC_RPC_FLG_REQ_ACK);
            pMiMctc->seqRpcLocal = 1;
            pMiMctc->stateRpc = GOAL_MI_MCTC_RPC_STATE_SYNC_ACK_LOCAL;

            /* intended fallthrough */
            GOAL_TARGET_FALLTHROUGH;

        case GOAL_MI_MCTC_RPC_STATE_SYNC_ACK_LOCAL:

            goal_logDbg("state: SYNC_ACK_LOCAL");

            /* send empty sequence 1 until ACK is received and we confirmed remote sequence */
            if (1 != pMiMctc->seqRpcLocalAck) {

                /* send empty RPC request */
                goal_miMctcRpcSend(pMiMctc, NULL, 0);

                break;
            }

            /* wait for remote sync */
            pMiMctc->stateRpc = GOAL_MI_MCTC_RPC_STATE_SYNC_ACK_REMOTE;

            break;

        case GOAL_MI_MCTC_RPC_STATE_SYNC_ACK_REMOTE:

            goal_logDbg("state: SYNC_ACK_REMOTE");

            if (0 == pMiMctc->seqRpcRemoteAck) {

                /* send empty RPC request */
                goal_miMctcRpcSend(pMiMctc, NULL, 0);

                break;
            }

            /* enter run mode */
            GOAL_MASK_CLR(pMiMctc->flgRpc, GOAL_MI_MCTC_RPC_FLG_REQ_ACK);
            pMiMctc->stateRpc = GOAL_MI_MCTC_RPC_STATE_RUN_ONCE;

            break;
    }
}


/****************************************************************************/
/** MCTC MI - RPC Send
 *
 * @returns GOAL_STATUS_T
 */
static GOAL_STATUS_T goal_miMctcRpcSend(
    GOAL_MI_MCTC_INST_T *pMiMctc,               /**< MI MCTC handle */
    uint8_t *pBuf,                              /**< buffer pointer */
    unsigned int len                            /**< buffer length */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MI_MCTC_RPC_TX_T *pRpc;                /* RPC header */
    uint8_t *pBufPart = NULL;                   /* partition buffer */
    uint32_t lenBufPart = 0;                    /* partition buffer length */

    /* check if partition was assigned */
    if (NULL == pMiMctc->pDmPartWrite) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* check data length */
    if (len != (unsigned int) ((GOAL_MI_MCTC_RPC_DATA_LENGTH_T) len)) {
        goal_logErr("unable to send %u bytes", len);
        return GOAL_ERR_OVERFLOW;
    }

    /* initiate group start */
    res = goal_miDmGroupWriteStart(pMiMctc->pDmPartWrite->pGroup);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* directly access partition buffer */
    res = goal_miDmGroupWriteBufGet(&pBufPart, &lenBufPart, pMiMctc->pDmPartWrite);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* validate the provided buffer length */
    if (lenBufPart < len + sizeof(GOAL_MI_MCTC_RPC_TX_T)) {
        return GOAL_ERR_OVERFLOW;
    }

    /* build header */
    pRpc = (GOAL_MI_MCTC_RPC_TX_T *) pBufPart;
    pRpc->flags = pMiMctc->flgRpc;
    pRpc->seqLocal = pMiMctc->seqRpcLocal;
    pRpc->seqRemoteAck = pMiMctc->seqRpcRemoteAck;
    pRpc->lenData_le = GOAL_MI_MCTC_RPC_DATA_LENGTH_htole(len);

    /* acknowledge sync if remote requests it */
    if (GOAL_MI_MCTC_RPC_FLG_SYNC_REQ & pMiMctc->flgRpcRemote) {
        pRpc->flags |= GOAL_MI_MCTC_RPC_FLG_SYNC_ACK;
    }

    /* attach data */
    if ((NULL != pBuf) && (0 != len)) {
        GOAL_MEMCPY(&pBufPart[sizeof(GOAL_MI_MCTC_RPC_TX_T)], pBuf, len);
        pMiMctc->flgRpcPrevEmpty = GOAL_FALSE;
    } else {
        pMiMctc->flgRpcPrevEmpty = GOAL_TRUE;
    }

    /* calculate CRC */
    pRpc->crc_le16 = GOAL_htole16(goal_utilCrcFletcher16(&pBufPart[sizeof(GOAL_MI_MCTC_RPC_TX_T)], (int) len) + GOAL_MI_MCTC_FLETCHER16_OFFSET);

    /* commit data */
    res = goal_miDmGroupWriteEnd(pMiMctc->pDmPartWrite->pGroup);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    goal_logDbg("local %stransmit: l:%u, lack:%u, rack:%u, buf_idx: %u, flags: 0x%x",
                 (GOAL_MI_MCTC_RPC_FLG_RESEND & pRpc->flags) ? "re" : "",
                 pRpc->seqLocal,
                 pMiMctc->seqRpcLocalAck,
                 pRpc->seqRemoteAck,
                 pMiMctc->cntResend,
                 pRpc->flags);

    /* add frame to send-active counter */
    pMiMctc->cntRpcActive++;

    /* update receive timeout timestamp */
    pMiMctc->tsRpcTout = goal_timerTsGet() + GOAL_MI_MCTC_RPC_RECV_TOUT;

    return GOAL_OK;
}


/****************************************************************************/
/** MCTC MI - RPC Send Allow Check
 *
 * @returns GOAL_STATUS_T
 */
static GOAL_BOOL_T goal_miMctcRpcSendAllow(
    GOAL_MI_MCTC_INST_T *pMiMctc                /**< MI MCTC handle */
)
{
    /* check active send counter */
    if (GOAL_MI_MCTC_RPC_CNT_RESEND > pMiMctc->cntRpcActive) {
        return GOAL_TRUE;
    }

    /* check send timeout */
    if (pMiMctc->tsRpcTout < goal_timerTsGet()) {
        pMiMctc->cntRpcActive = 0;

        return GOAL_TRUE;
    }

    return GOAL_FALSE;
}


/****************************************************************************/
/** MCTC MI - Signalize RX Timeout
 */
void goal_miMctcMonitorRx(
    GOAL_MI_MCTC_INST_T *pMiMctc,               /**< MI MCTC handle */
    GOAL_BOOL_T flgConn                         /**< connection flag */
)
{
    /* if connection is up reset timeout info */
    if (GOAL_TRUE == flgConn) {
        if (GOAL_TRUE == pMiMctc->flgInfoToutRx) {
            goal_logInfo("data channel online: %s", pMiMctc->strInstDesc);
            pMiMctc->flgInfoToutRx = GOAL_FALSE;
            GOAL_MI_MCTC_CB_ONLINE(pMiMctc);
        }

        return;
    }

    /* connection down, inform callback handlers */
    if (GOAL_FALSE == pMiMctc->flgInfoToutRx) {

        /* only propage an error if a peer was available */
        if (pMiMctc->stateRpc != GOAL_MI_MCTC_RPC_STATE_SYNC_REQ) {
            pMiMctc->flgInfoToutRx = GOAL_TRUE;
            GOAL_MI_MCTC_CB_TIMEOUT_RX(pMiMctc);
            goal_logErr("data channel offline: %s", pMiMctc->strInstDesc);
            goal_logInfo("state : %"FMT_u32":", (uint32_t) pMiMctc->stateRpc);
        }
    }

    /* enter sync state */
    pMiMctc->stateRpc = GOAL_MI_MCTC_RPC_STATE_SYNC_INIT;
}


/****************************************************************************/
/** MCTC MI - Notify function
 *
 * @returns GOAL_STATUS_T
 */
GOAL_STATUS_T goal_miMctcNotify(
    void *pArg,                                 /**< argument */
    uint8_t *pData,                             /**< notify data */
    uint16_t len                                /**< notify data length */
)
{
    struct GOAL_MI_MCTC_INST_T *pMiMctc;        /* MI MCTC handle */

    /* refer the MI handle */
    pMiMctc = (struct GOAL_MI_MCTC_INST_T *) pArg;

    /* null pointer verification */
    if (NULL == pMiMctc->funcNotifyCb) {
        return GOAL_ERR_NULL_POINTER;
    }

    return pMiMctc->funcNotifyCb(pMiMctc, pData, len);
}


#if (1 == GOAL_CONFIG_GEN_CTC_AC)
/****************************************************************************/
/** Configure device MAC address - client
 *
 * Note: This function must be called before any other network related calls
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_miMctcEthCfgMacAddr(
    uint32_t port,                              /**< port ID */
    uint32_t idMa,                              /**< associated MA */
    GOAL_ETH_MAC_ADDR_T *pMacAddress            /**< new device MAC address */
)
{
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_MI_MCTC_INST_T *pHdlRpcChn = NULL;     /* default MI Mctc instance */

    /* get default instance */
    res = goal_instGetById((GOAL_INSTANCE_T **) &pHdlRpcChn, GOAL_ID_MI_MCTC, GOAL_ID_MI_CTC_DEFAULT);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    GOAL_RPC_NEW();

    /* push data in reversed order */
    GOAL_RPC_PUSH_PTR(pMacAddress, MAC_ADDR_LEN);
    GOAL_RPC_PUSH(idMa);
    GOAL_RPC_PUSH(port);

    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_MI_MCTC, GOAL_MCTC_RPC_FUNC_CFG_ETH_MAC_ADDRESS_SET);
    }

    GOAL_RPC_CLOSE();

    return res;
}


#elif (1 == GOAL_CONFIG_GEN_CTC_CC)
/****************************************************************************/
/** Configure device MAC address - server
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_miMctcEthCfgMacAddrServer(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< rpc handle */
)
{
#if 1 == GOAL_CONFIG_ETHERNET
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_ETH_MAC_ADDR_T macAddress = GOAL_ETH_MAC_ADDR; /* mac address */
    uint32_t idMa = 0;                          /* associated MA */
    uint32_t port = 0;                          /* port ID */

    /* pop data */
    GOAL_RPC_POP(port, uint32_t);
    GOAL_RPC_POP(idMa, uint32_t);
    GOAL_RPC_POP_PTR(&macAddress, MAC_ADDR_LEN);

    if (GOAL_RES_OK(res)) {
        res = goal_miEthCfgMacAddr(port, idMa, &macAddress);
    }

    return res;
#else
    UNUSEDARG(pHdlRpc);

    return GOAL_ERR_UNSUPPORTED;
#endif
}
#endif
