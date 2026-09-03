/** @file
 *
 * @brief GOAL Micro Core To Core
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
#define GOAL_ID GOAL_ID_CTC
#include "goal_mctc.h"


/* TODO: use GOAL_ERR_CTC_* instead of common error codes */


/****************************************************************************/
/* Local variables */
/****************************************************************************/
/**< MCTC RPC function list */
static GOAL_MCTC_RPC_T *mpRpc;

/**< MCTC RPC catch-all function */
static GOAL_MCTC_RPC_T *mpRpcAll;

/**< MCTC message id counter */
static uint8_t mIdCtc = 1;

/**< configured timeout value */
static uint32_t cfgToutValue = GOAL_MCTC_TIMEOUT_RX;


/****************************************************************************/
/** GOAL MCTC - Create MCTC instance
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_mctcInstNew(
    GOAL_MCTC_INST_T **ppInst,                  /**< [out] MCTC instance */
    GOAL_MCTC_TGT_T *pTgtRx,                    /**< RX target handle */
    GOAL_MCTC_TGT_T *pTgtTx,                    /**< TX target handle */
    unsigned int cntHdl,                        /**< handle count */
    unsigned int sizeData                       /**< data size per handle */
)
{
    GOAL_STATUS_T res;                          /* result */
    unsigned int cnt;                           /* counter */

    /* allocate instance */
    res = GOAL_MEM_CALLOC(ppInst, sizeof(GOAL_MCTC_INST_T));
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* allocate CTC handles */
    res = GOAL_MEM_CALLOC(&(*ppInst)->pHdls, sizeof(GOAL_MCTC_T) * cntHdl);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* allocate CTC handle data */
    for (cnt = 0; cnt < cntHdl; cnt++) {
        res = GOAL_MEM_CALLOC(&(*ppInst)->pHdls[cnt].pData, sizeData);
        if (GOAL_RES_ERR(res)) {
            break;
        }

        /* initialize locks */
        GOAL_LOCK_CREATE((*ppInst)->pHdls[cnt].pLock);
    }

    /* assign instance data */
    (*ppInst)->cntHdl = cntHdl;
    (*ppInst)->sizeData = sizeData;
    (*ppInst)->pTgtRx = pTgtRx;
    (*ppInst)->pTgtTx = pTgtTx;

    /* set default value for timeout */
    (*ppInst)->cfgToutValue = cfgToutValue;

    return res;
}


/****************************************************************************/
/** GOAL MCTC - Register MCTC Remote Procedure Callback
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_mctcRpcReg(
    uint32_t idRpc,                             /**< RPC id */
    uint32_t idFunc,                            /**< function id */
    GOAL_MCTC_RPC_FUNC_T func                   /**< RPC function */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MCTC_RPC_T **ppRpc;                    /* RPC element ptr ref */

    /* check if catch-all handler must be registered */
    if ((0 == idRpc) && (0 == idFunc)) {
        if (NULL != mpRpcAll) {
            return GOAL_ERR_EXISTS;
        }

        ppRpc = &mpRpcAll;
    } else {
        /* check if function is already registered */
        for (ppRpc = &mpRpc; *ppRpc; ppRpc = &(*ppRpc)->pNext) {
            if (((*ppRpc)->idRpc == idRpc) && ((*ppRpc)->idFunc == idFunc)) {
                return GOAL_ERR_EXISTS;
            }
        }
    }

    /* function doesn't exist and ppRpc already set to last list element */
    res = GOAL_MEM_CALLOC(ppRpc, sizeof(GOAL_MCTC_RPC_T));
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* assign RPC data */
    (*ppRpc)->idRpc = idRpc;
    (*ppRpc)->idFunc = idFunc;
    (*ppRpc)->func = func;

    return GOAL_OK;
}


/****************************************************************************/
/** GOAL MCTC - Allocate CTC Handle
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_mctcNew(
    GOAL_MCTC_T **ppHdl,                        /**< [out] MCTC handle */
    GOAL_MCTC_INST_T *pInst                     /**< instance */
)
{
    unsigned int cnt;                           /* counter */
    GOAL_STATUS_T res;                          /* result */

    /* is this the GOAL loop? Sending messages is only allowed from there */
    res = goal_utilGoalLoopCheck();
    if (GOAL_RES_ERR(res)) {
        goal_logErr("sending RPC message is only allowed from GOAL loop!");
        return res;
    }

    /* find free CTC transfer handle */
    for (cnt = 0; cnt < pInst->cntHdl; cnt++) {

        /* lock handle */
        GOAL_LOCK_GET(pInst->pHdls[cnt].pLock);

        /* check if handle is used */
        if (GOAL_TRUE == pInst->pHdls[cnt].flgUsed) {

            /* unlock handle */
            GOAL_LOCK_PUT(pInst->pHdls[cnt].pLock);

            continue;
        }

        /* assign metadata */
        pInst->pHdls[cnt].flgUsed = GOAL_TRUE;
        pInst->pHdls[cnt].flgRes = GOAL_FALSE;
        pInst->pHdls[cnt].flgRemote = GOAL_FALSE;

        /* assign default timeout */
        pInst->pHdls[cnt].tsTout = pInst->cfgToutValue;

        /* remove RPC request info from usable size */
        pInst->pHdls[cnt].sizeData = pInst->sizeData - sizeof(GOAL_MCTC_HDR_RPC_REQ_T);

        /* assign instance */
        pInst->pHdls[cnt].pInst = pInst;

        /* return CTC handle */
        *ppHdl = &pInst->pHdls[cnt];

        /* unlock handle */
        GOAL_LOCK_PUT(pInst->pHdls[cnt].pLock);

        return GOAL_OK;
    }

    return GOAL_ERR_ALLOC;
}


/****************************************************************************/
/** GOAL MCTC - Push Data Onto CTC Stack
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_mctcPush(
    GOAL_MCTC_T *pCtc,                          /**< CTC handle */
    const uint8_t *pData,                       /**< data pointer */
    unsigned int lenData                        /**< data length */
)
{
    /* lock handle */
    GOAL_LOCK_GET(pCtc->pLock);

    if ((pCtc->sizeData - pCtc->idxData) < lenData) {

        /* unlock handle */
        GOAL_LOCK_PUT(pCtc->pLock);

        return GOAL_ERR_OVERFLOW;
    }

    /* TODO: add datatype and length check for debugging */

    /* push data onto CTC stack */
    GOAL_MEMCPY(&pCtc->pData[pCtc->idxData], pData, lenData);
    pCtc->idxData += lenData;

    /* unlock handle */
    GOAL_LOCK_PUT(pCtc->pLock);

    return GOAL_OK;
}


/****************************************************************************/
/** GOAL MCTC - Pop Data From CTC Stack
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_mctcPop(
    GOAL_MCTC_T *pCtc,                          /**< CTC handle */
    uint8_t *pData,                             /**< data pointer */
    unsigned int lenData                        /**< data length */
)
{
    /* lock handle */
    GOAL_LOCK_GET(pCtc->pLock);

    if (pCtc->idxData < lenData) {

        /* unlock handle */
        GOAL_LOCK_PUT(pCtc->pLock);

        return GOAL_ERR_UNDERFLOW;
    }

    /* TODO: add datatype and length check for debugging */

    /* pop data from CTC stack */
    GOAL_MEMCPY(pData, &pCtc->pData[pCtc->idxData - lenData], lenData);
    pCtc->idxData -= lenData;

    /* unlock handle */
    GOAL_LOCK_PUT(pCtc->pLock);

    return GOAL_OK;
}


/****************************************************************************/
/** GOAL MCTC - Get CTC Stack Length
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_mctcLenGet(
    GOAL_MCTC_T *pCtc,                          /**< CTC handle */
    unsigned int *pLen                          /**< [out] stack length */
)
{
    if (NULL == pCtc) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* lock handle */
    GOAL_LOCK_GET(pCtc->pLock);

    /* return current data length */
    *pLen = pCtc->idxData;

    /* unlock handle */
    GOAL_LOCK_PUT(pCtc->pLock);

    return GOAL_OK;
}


/****************************************************************************/
/** GOAL MCTC - Get Free CTC Data Length
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_mctcLenFreeGet(
    GOAL_MCTC_T *pCtc,                          /**< CTC handle */
    unsigned int *pLenFree                      /**< [out] free data size */
)
{
    if (NULL == pCtc) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* lock handle */
    GOAL_LOCK_GET(pCtc->pLock);

    /* return free data size */
    *pLenFree = pCtc->sizeData - pCtc->idxData;

    /* unlock handle */
    GOAL_LOCK_PUT(pCtc->pLock);

    return GOAL_OK;
}


/****************************************************************************/
/** GOAL MCTC - Call Remote Function
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_mctcSendInfo(
    GOAL_MCTC_T *pCtc,                          /**< CTC handle */
    uint32_t idRpc,                             /**< RPC id */
    uint32_t idFunc                             /**< function id */
)
{
    GOAL_MCTC_HDR_RPC_REQ_T *pHdr;              /* RPC request header */

    /* lock handle */
    GOAL_LOCK_GET(pCtc->pLock);

    /* assign CTC transfer id */
    pCtc->idCtc = mIdCtc++;

    /* add RPC request header */
    pHdr = (GOAL_MCTC_HDR_RPC_REQ_T *) &pCtc->pData[pCtc->idxData];
    pHdr->common.flags = GOAL_MCTC_FLG_INFO;
    pHdr->common.idCtc = pCtc->idCtc;
    GOAL_htole32_p(&pHdr->idRpc_le32, idRpc);
    GOAL_htole32_p(&pHdr->idFunc_le32, idFunc);
    pCtc->idxData += sizeof(GOAL_MCTC_HDR_RPC_REQ_T);

    /* unlock handle */
    GOAL_LOCK_PUT(pCtc->pLock);

    /* send message */
    GOAL_LOG("  * sending information\n");
    return pCtc->pInst->pTgtTx->funcSend(pCtc, pCtc->pInst->pTgtTx);
}


/****************************************************************************/
/** GOAL MCTC - Call Remote Function
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_mctcSendReq(
    GOAL_MCTC_T *pCtc,                          /**< CTC handle */
    uint32_t idRpc,                             /**< RPC id */
    uint32_t idFunc                             /**< function id */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_STATUS_T resRemote = GOAL_OK;          /* remote response */
    GOAL_MCTC_HDR_RPC_REQ_T *pHdr;              /* RPC request header */
    GOAL_TIMESTAMP_T tsTout;                    /* timeout timestamp */
    GOAL_TIMESTAMP_T tsStart;                   /* start timestamp */

    /* lock handle */
    GOAL_LOCK_GET(pCtc->pLock);

    /* assign CTC transfer id */
    pCtc->idCtc = mIdCtc++;

    /* add RPC request header */
    pHdr = (GOAL_MCTC_HDR_RPC_REQ_T *) &pCtc->pData[pCtc->idxData];
    pHdr->common.flags = GOAL_MCTC_FLG_REQUEST;
    pHdr->common.idCtc = pCtc->idCtc;
    GOAL_htole32_p(&pHdr->idRpc_le32, idRpc);
    GOAL_htole32_p(&pHdr->idFunc_le32, idFunc);
    pCtc->idxData += sizeof(GOAL_MCTC_HDR_RPC_REQ_T);

    /* unlock handle */
    GOAL_LOCK_PUT(pCtc->pLock);

    /* send message and wait for a response */
    GOAL_LOG("  * sending request\n");
    res = pCtc->pInst->pTgtTx->funcSend(pCtc, pCtc->pInst->pTgtTx);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* calculate timeout */
    tsStart = GOAL_TIMER_TS_GET();
    tsTout = tsStart + pCtc->tsTout;

    /* statistics */
    pCtc->pInst->cntRpc++;

    /* wait until timeout is reached */
    GOAL_LOG("    * waiting for response\n");
    while (GOAL_FALSE == pCtc->flgRes) {

        /* run cyclic tasks during receive-wait */
        GOAL_LOOP();

        /* leave on timeout */
        if ((0 != pCtc->tsTout) && (tsTout < GOAL_TIMER_TS_GET())) {
            pCtc->pInst->cntRpcTimeout++;
            return GOAL_ERR_TIMEOUT;
        }
    }

    /* check rpc time statistics */
    tsTout = GOAL_TIMER_TS_GET() - tsStart;
    if ((pCtc->pInst->tsRpcMin == 0) || (pCtc->pInst->tsRpcMin > tsTout)) {
        pCtc->pInst->tsRpcMin = (uint32_t) tsTout;
    }
    if ((pCtc->pInst->tsRpcMax == 0) || (pCtc->pInst->tsRpcMax < tsTout)) {
        pCtc->pInst->tsRpcMax = (uint32_t) tsTout;
    }
    pCtc->pInst->tsRpcMean += ((tsTout - pCtc->pInst->tsRpcMean) / 10);

    /* return response */
    res = goal_mctcPop(pCtc, (uint8_t *) &resRemote, sizeof(GOAL_STATUS_T));
    if (GOAL_RES_OK(res)) {
        GOAL_LOG("    * remote result: %"FMT_u32, resRemote);
        res = resRemote;
    }

    /* return response */
    return res;
}


/****************************************************************************/
/** GOAL MCTC - Release CTC Handle
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_mctcRelease(
    GOAL_MCTC_T *pCtc                           /**< CTC handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    /* lock handle */
    GOAL_LOCK_GET(pCtc->pLock);

    /* check if already marked as unused */
    if (GOAL_FALSE == pCtc->flgUsed) {

        /* unlock handle */
        GOAL_LOCK_PUT(pCtc->pLock);

        GOAL_LOG_ERR("  * CTC handle not in use\n");

        return GOAL_ERR_WRONG_STATE;
    }

    /* check if all pushed variables where handled */
    if ((GOAL_TRUE == pCtc->flgRes) && (0 != pCtc->idxData)) {

        GOAL_LOG_ERR("  * CTC stack not empty: %u bytes\n", pCtc->idxData);

        res = GOAL_ERR_NOT_EMPTY;
    }

    /* clear CTC data buffer */
    pCtc->idxData = 0;

    /* mark CTC handle as unused */
    pCtc->flgUsed = GOAL_FALSE;

    /* unlock handle */
    GOAL_LOCK_PUT(pCtc->pLock);

    return res;
}


/****************************************************************************/
/** GOAL MCTC - Parse Received CTC Data
 */
void goal_mctcMsgParse(
    GOAL_MCTC_INST_T *pInst,                    /**< instance */
    uint8_t *pData,                             /**< data pointer */
    unsigned int lenData                        /**< data length */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MCTC_T *pCtc = NULL;                   /* CTC handle */
    GOAL_MCTC_RPC_T *pRpc;                      /* RPC function */
    GOAL_MCTC_HDR_RPC_T *pHdr;                  /* RPC header */
    GOAL_MCTC_HDR_RPC_REQ_T *pHdrReq;           /* RPC request header */
    unsigned int cnt;                           /* counter */

    /* assign header */
    pHdr = (GOAL_MCTC_HDR_RPC_T *) &pData[lenData - sizeof(GOAL_MCTC_HDR_RPC_T)];

    /* TODO: check minimal data length depending on flags */

    /* handle request and info messages */
    switch (pHdr->flags & GOAL_MCTC_FLG_TYPE) {

        case GOAL_MCTC_FLG_REQUEST:
        case GOAL_MCTC_FLG_INFO:

            GOAL_LOG("  * found request\n");

            /* allocate CTC handle */
            res = goal_mctcNew(&pCtc, pInst);
            if (GOAL_RES_ERR(res)) {
                GOAL_LOG_ERR("    * no free handle found\n");
                return;
            }

            /* lock handle */
            GOAL_LOCK_GET_NO_RES(pCtc->pLock);

            /* mark handle as remote */
            pCtc->flgRemote = GOAL_TRUE;

            /* copy received data to CTC handle */
            /* TODO: can this be done in a more efficient way? maybe allocate
             * buffers separately from CTC handles and than assign the RX buffer to
             * the CTC handle? */
            GOAL_MEMCPY(pCtc->pData, pData, lenData - sizeof(GOAL_MCTC_HDR_RPC_REQ_T));
            pCtc->idxData = lenData - sizeof(GOAL_MCTC_HDR_RPC_REQ_T);

            /* reserve space for response header */
            pCtc->sizeData -= sizeof(GOAL_MCTC_HDR_RPC_T);

            /* assign CTC transfer id */
            pCtc->idCtc = pHdr->idCtc;

            /* assign flags */
            pCtc->flags = pHdr->flags;

            /* extract RPC and function id */
            pHdrReq = (GOAL_MCTC_HDR_RPC_REQ_T *) &pData[lenData - sizeof(GOAL_MCTC_HDR_RPC_REQ_T)];
            pCtc->idRpc = GOAL_le32toh_p(&pHdrReq->idRpc_le32);
            pCtc->idFunc = GOAL_le32toh_p(&pHdrReq->idFunc_le32);
            GOAL_LOG("  * idRpc: %"FMT_u32, pCtc->idRpc);
            GOAL_LOG("  * idFunc: %"FMT_u32, pCtc->idFunc);

            /* unlock handle */
            GOAL_LOCK_PUT(pCtc->pLock);

            /* find matching RPC function */
            for (pRpc = mpRpc; pRpc; pRpc = pRpc->pNext) {
                if ((pRpc->idRpc == pCtc->idRpc) && (pRpc->idFunc == pCtc->idFunc)) {

                    /* call registered RPC function and store the result */
                    res = pRpc->func(pCtc);

                    break;
                }
            }

            /* if no handler was found try to use catch-all */
            if (NULL == pRpc) {
                if (NULL == mpRpcAll) {
                    GOAL_LOG_ERR("    * no handler found for %"FMT_u32":%"FMT_u32, pCtc->idRpc, pCtc->idFunc);
                    res = GOAL_ERR_NOT_FOUND;
                } else {
                    res = mpRpcAll->func(pCtc);
                }
            }

            /* check if a response is awaited */
            if (GOAL_MCTC_FLG_INFO == (pCtc->flags & GOAL_MCTC_FLG_TYPE)) {

                /* release handle */
                goal_mctcRelease(pCtc);

                break;
            }

            /* return result to caller */
            GOAL_LOG("  * sending response\n");
            goal_mctcPush(pCtc, (uint8_t *) &res, sizeof(GOAL_STATUS_T));

            /* lock handle */
            GOAL_LOCK_GET_NO_RES(pCtc->pLock);

            /* add response header */
            pHdr = (GOAL_MCTC_HDR_RPC_T *) &pCtc->pData[pCtc->idxData];
            pHdr->flags = GOAL_MCTC_FLG_RESPONSE;
            pHdr->idCtc = pCtc->idCtc;
            pCtc->idxData += sizeof(GOAL_MCTC_HDR_RPC_T);

            /* unlock handle */
            GOAL_LOCK_PUT(pCtc->pLock);

            /* send response */
            pInst->pTgtTx->funcSend(pCtc, pInst->pTgtTx);

            /* release handle */
            goal_mctcRelease(pCtc);

            break;

        case GOAL_MCTC_FLG_RESPONSE:

            /* handle response */
            GOAL_LOG("  * found response\n");

            /* find matching CTC handle for CTC id */
            for (cnt = 0; cnt < pInst->cntHdl; cnt++) {

                /* assign handle */
                pCtc = &pInst->pHdls[cnt];

                /* lock handle */
                GOAL_LOCK_GET_NO_RES(pCtc->pLock);

                /* compare id */
                if ((GOAL_TRUE == pCtc->flgUsed) && (GOAL_FALSE == pCtc->flgRemote) && (pCtc->idCtc == pHdr->idCtc)) {

                    GOAL_LOG("    * response matches request id: %u\n", pHdr->idCtc);

                    /* copy response data into CTC handle */
                    lenData -= sizeof(GOAL_MCTC_HDR_RPC_T);
                    GOAL_MEMCPY(pCtc->pData, pData, lenData);
                    pCtc->idxData = lenData;

                    /* mark request as answered */
                    pCtc->flgRes = GOAL_TRUE;

                    /* unlock handle */
                    GOAL_LOCK_PUT(pCtc->pLock);

                    break;

                }

                /* unlock handle */
                GOAL_LOCK_PUT(pCtc->pLock);
            }

            break;
    }
}


/****************************************************************************/
/** GOAL MCTC - Configure instance timeout behaviour
 *
 * tOut configures the RPC timeout in seconds
 *
 * a value of 0 disables timeout detection (debug mode)
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_mctcToutSet(
    GOAL_MCTC_INST_T *pInst,                    /**< instance */
    uint32_t tOut                               /**< timeout value */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    if (NULL == pInst) {
        res = GOAL_ERR_NULL_POINTER;
    }

    if (GOAL_RES_OK(res)) {
        pInst->cfgToutValue = tOut;
    }

    return res;
}


/****************************************************************************/
/** GOAL MCTC - Configure timeout behaviour for newly created instances
 *
 * tOut configures the RPC timeout in seconds
 *
 * a value of 0 disables timeout detection (debug mode)
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_mctcCfgTout(
    uint32_t tOut                               /**< timeout value */
)
{
    cfgToutValue = tOut;

    return GOAL_OK;
}
