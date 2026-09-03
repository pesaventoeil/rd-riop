/** @file
 *
 * @brief Implementation of the Goal HTTP RPC wrapper for the CC.
 *
 * Implementation of the HTTP RPC wrapper on the CC.
 *
 * @copyright
 * Copyright 2010-2018 port GmbH Halle/Saale.
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
#define GOAL_ID GOAL_ID_HTTP_CTC
#include "goal_includes.h"
#include "goal_http.h"
#include "goal_http_resmgr.h"
#include "goal_http_idlist.h"


/****************************************************************************/
/* Local types */
/****************************************************************************/


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/

static GOAL_STATUS_T goal_httpRpcInit(
    void
);

static GOAL_STATUS_T goal_httpChnSetCtcFrag(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    uint32_t hdlTransm,                         /**< transmission handle */
    GOAL_BOOL_T frag                            /**< CTC fragmentation bool */
);

static GOAL_STATUS_T goal_httpCbDataCc(
    GOAL_HTTP_APPLCB_DATA_T *pCbInfo            /**< pointer to callback info */
);

static GOAL_STATUS_T goal_httpCbDataCcTransDone(
    GOAL_HTTP_APPLCB_DATA_T *pCbInfo            /**< pointer to callback info */
);

static GOAL_STATUS_T goal_httpCbDataCcGet(
    GOAL_HTTP_APPLCB_DATA_T *pCbInfo            /**< pointer to callback info */
);

static GOAL_STATUS_T goal_httpCbDataCcPost(
    GOAL_HTTP_APPLCB_DATA_T *pCbInfo            /**< pointer to callback info */
);

static GOAL_STATUS_T goal_httpCbTemplCc(
    GOAL_HTTP_APPLCB_TEMPL_T *pCbInfo           /**< pointer to callback info */
);

static GOAL_STATUS_T goal_httpCbTemplSmplCc(
    GOAL_HTTP_APPLCB_TEMPL_T *pCbInfo           /**< pointer to callback info */
);

static GOAL_STATUS_T goal_httpCbTemplListCc(
    GOAL_HTTP_APPLCB_TEMPL_T *pCbInfo           /**< pointer to callback info */
);


/****************************************************************************/
/* Function and parameter mappings */
/****************************************************************************/

/**< id to function mapping */
typedef struct {
    unsigned int id;                            /**< function id */
    GOAL_RPC_FUNC_T pFunc;                      /**< function ptr */
} GOAL_HTTP_FUNC_PTR_T;

/* id to function mapping */
#undef GOAL_HTTP_FUNC_ENTRY
#define GOAL_HTTP_FUNC_ENTRY(id, name, func) { id, func },
static GOAL_HTTP_FUNC_PTR_T goal_httpTblFunc[] = {
    GOAL_HTTP_FUNC_LIST
};

/**< id to function mapping */
typedef struct {
    unsigned int id;                            /**< function id */
} GOAL_HTTP_CB_FUNC_PTR_T;

/* id to function mapping */
#undef GOAL_HTTP_FUNC_CB_ENTRY
#define GOAL_HTTP_FUNC_CB_ENTRY(id, name, func) { id },
static GOAL_HTTP_CB_FUNC_PTR_T goal_httpTblCbFunc[] = {
    GOAL_HTTP_FUNC_CB_LIST
};


/****************************************************************************/
/* Local variables */
/****************************************************************************/

static GOAL_INSTANCE_LIST_T *pListCtcInst = NULL; /**< HTTP instance list */
static GOAL_STAGE_HANDLER_T mStageMod;          /**< modules stage handler */


/****************************************************************************/
/** This function is used to inform the AC when a channel is closed.
 *
 * @retval GOAL_OK on success
 * @retval other failed
 */
GOAL_STATUS_T goal_httpChnCloseCc(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan                  /**< channel pointer */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* resutlt */
    GOAL_INSTANCE_T *pInstIt = NULL;            /* list iterator */
    GOAL_HTTP_INSTANCE_CTC_T *pHttpCtc = NULL;  /* HTTP CTC instance */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */
    uint32_t chanId;                            /* channel id */

    /* find CTC instance */
    res = GOAL_ERR_NOT_FOUND;
    GOAL_INSTANCE_FOREACH((GOAL_INSTANCE_T *) pListCtcInst->pInst, pInstIt) {
        pHttpCtc = (GOAL_HTTP_INSTANCE_CTC_T *) pInstIt;

        if (pInst == pHttpCtc->pHttpInst) {
            res = GOAL_OK;
            break;
        }
    }

    if (GOAL_RES_ERR(res)) {
        /* there is nothing to do here */
        res = GOAL_OK;
    } else {
        /* set transmission handle */
        res = goal_httpChannelId(pInst, pHttpChan, &chanId);
        /* get new rpc handle */
        GOAL_HTTP_RPC_NEW(pHttpCtc);
        /* set rpc data */
        GOAL_RPC_PUSH(chanId);
        GOAL_HTTP_RPC_CALL(pHttpCtc, GOAL_HTTP_CB_CLOSECHN);
        /* close rpc channel */
        GOAL_HTTP_RPC_CLOSE();
    }
    return res;
}


/****************************************************************************/
/** This function is used when CTC data fragmentation is necessary.
 *
 * @retval GOAL_OK on success
 * @retval other failed
 */
GOAL_STATUS_T goal_httpDataContinueCc(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    const uint8_t **ppData,                     /**< address of data pointer */
    uint32_t *pLenData                          /**< length of data */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_INSTANCE_T *pInstIt = NULL;            /* list iterator */
    GOAL_HTTP_INSTANCE_CTC_T *pHttpCtc = NULL;  /* HTTP CTC instance */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */
    GOAL_BOOL_T getFrag = GOAL_FALSE;           /* fragmentation bool */
    uint32_t chanId;                            /* channel id */

    /* set transmission handle */
    res = goal_httpChannelId(pInst, pHttpChan, &chanId);
    /* find CTC instance */
    res = GOAL_ERR_NOT_FOUND;
    GOAL_INSTANCE_FOREACH((GOAL_INSTANCE_T *) pListCtcInst->pInst, pInstIt) {
        pHttpCtc = (GOAL_HTTP_INSTANCE_CTC_T *) pInstIt;

        if (pInst == pHttpCtc->pHttpInst) {
            res = GOAL_OK;
            break;
        }
    }

    /* get new rpc handle */
    GOAL_HTTP_RPC_NEW(pHttpCtc);

    /* set rpc data */
    GOAL_RPC_PUSH(chanId);
    GOAL_HTTP_RPC_CALL(pHttpCtc, GOAL_HTTP_CB_CONT);

    GOAL_RPC_POP(getFrag, GOAL_BOOL_T);
    GOAL_RPC_POP((*pLenData), uint32_t);
    GOAL_RPC_POP_PTR(pHttpCtc->pTempBuf, (uint16_t) (*pLenData));

    GOAL_HTTP_RPC_CLOSE();

    if (GOAL_RES_OK(res)) {
        /* set data pointer */
        *(ppData) = (const uint8_t *) (pHttpCtc->pTempBuf);
    }

    if (GOAL_RES_OK(res)) {
        /* set CTC fragmentation info */
        res = goal_httpChnSetCtcFrag(pInst, chanId, getFrag);
    }

    return res;
}


/****************************************************************************/
/** This function sets the fragmentation bool for CTC fragmentation.
 *
 * @retval GOAL_OK on success
 * @retval other failed
 */
static GOAL_STATUS_T goal_httpChnSetCtcFrag(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    uint32_t chanId,                            /**< transmission handle */
    GOAL_BOOL_T frag                            /**< CTC fragmentation bool */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_HTTP_CHN_T *pEntry = NULL;             /* channel list entry */

    /* lock mutex */
    res = goal_lockGet(pInst->lockChn, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("error acquiring channel lock");
    }

    /* find channel */
    res = goal_httpChannelEntryById(pInst, chanId, &pEntry);

    if (GOAL_RES_OK(res)) {
        pEntry->ctcFrag.getFrag = frag;
    }

    /* unlock mutex */
    goal_lockPut(pInst->lockChn);

    return res;
}


/****************************************************************************/
/** This function is used as application data callback function when CTC is
 * active. The HTTP request data is forwarded to the AC via RPC.
 *
 * @retval GOAL_OK on success
 * @retval other failed
 */
static GOAL_STATUS_T goal_httpCbDataCc(
    GOAL_HTTP_APPLCB_DATA_T *pCbInfo            /**< pointer to callback info */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    switch (pCbInfo->reqType)
    {
    case GOAL_HTTP_FW_GET:
        /* call CTC get handler */
        res = goal_httpCbDataCcGet(pCbInfo);
        break;

    case GOAL_HTTP_FW_POST_START:
    case GOAL_HTTP_FW_POST_DATA:
    case GOAL_HTTP_FW_POST_END:
        /* call CTC post handler */
        res = goal_httpCbDataCcPost(pCbInfo);
        break;

    case GOAL_HTTP_FW_REQ_DONE_OK:
    case GOAL_HTTP_FW_REQ_DONE_ERR:
        /* call CTC transmission end handler */
        res = goal_httpCbDataCcTransDone(pCbInfo);
        break;

    default:
        break;
    }

    return res;
}


/****************************************************************************/
/** This function is used as application data callback function when CTC is
 * active. The HTTP request data is forwarded to the AC via RPC.
 *
 * @retval GOAL_OK on success
 * @retval other failed
 */
static GOAL_STATUS_T goal_httpCbDataCcTransDone(
    GOAL_HTTP_APPLCB_DATA_T *pCbInfo            /**< pointer to callback info */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_INSTANCE_T *pInstIt = NULL;            /* list iterator */
    GOAL_HTTP_INSTANCE_CTC_T *pHttpCtc = NULL;  /* HTTP CTC instance */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */
    uint16_t resId;                             /* resource id */

    /* find CTC instance */
    res = GOAL_ERR_NOT_FOUND;
    GOAL_INSTANCE_FOREACH((GOAL_INSTANCE_T *) pListCtcInst->pInst, pInstIt) {
        pHttpCtc = (GOAL_HTTP_INSTANCE_CTC_T *) pInstIt;

        if (pCbInfo->pInst == pHttpCtc->pHttpInst) {
            res = GOAL_OK;
            break;
        }
    }

    /* get new rpc handle */
    GOAL_HTTP_RPC_NEW(pHttpCtc);

    /* set rpc data */
    GOAL_RPC_PUSH(pCbInfo->reqType);
    res = goal_httpIdByHdl(&resId, pCbInfo->hdlRes);
    GOAL_RPC_PUSH(resId);
    GOAL_RPC_PUSH((uint32_t) ((PtrCast) pCbInfo->hdlTransm));

    /* call function */
    GOAL_HTTP_RPC_CALL(pHttpCtc, GOAL_HTTP_CB_TRANS_DONE);

    /* close */
    GOAL_HTTP_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** This function is used as application data callback function when CTC is
 * active. The HTTP request data is forwarded to the AC via RPC.
 *
 * @retval GOAL_OK on success
 * @retval other failed
 */
static GOAL_STATUS_T goal_httpCbDataCcGet(
    GOAL_HTTP_APPLCB_DATA_T *pCbInfo            /**< pointer to callback info */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_INSTANCE_T *pInstIt = NULL;            /* list iterator */
    GOAL_HTTP_INSTANCE_CTC_T *pHttpCtc = NULL;  /* HTTP CTC instance */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */
    GOAL_BOOL_T getFrag = GOAL_FALSE;           /* fragmentation bool */
    uint16_t resId;                             /* resource id */

    /* find CTC instance */
    res = GOAL_ERR_NOT_FOUND;
    GOAL_INSTANCE_FOREACH((GOAL_INSTANCE_T *) pListCtcInst->pInst, pInstIt) {
        pHttpCtc = (GOAL_HTTP_INSTANCE_CTC_T *) pInstIt;

        if (pCbInfo->pInst == pHttpCtc->pHttpInst) {
            res = GOAL_OK;
            break;
        }
    }

    /* get new rpc handle */
    GOAL_HTTP_RPC_NEW(pHttpCtc);

    /* set rpc data */
    GOAL_RPC_PUSH(pCbInfo->reqType);
    res = goal_httpIdByHdl(&resId, pCbInfo->hdlRes);
    GOAL_RPC_PUSH(resId);
    GOAL_RPC_PUSH((uint32_t) ((PtrCast) pCbInfo->hdlTransm));
    GOAL_RPC_PUSH_PTR(pCbInfo->cs.pUrl, (uint16_t) (pCbInfo->cs.lenUrl));
    GOAL_RPC_PUSH(pCbInfo->cs.lenUrl);
    GOAL_RPC_PUSH(pCbInfo->cs.lenMatch);

    /* call function */
    GOAL_HTTP_RPC_CALL(pHttpCtc, GOAL_HTTP_CB_DATA_GET);

    /* get rpc return data */
    GOAL_RPC_POP(*(pCbInfo->pRet), GOAL_HTTP_STATUS_T);
    GOAL_RPC_POP(*(pCbInfo->sc.pContType), GOAL_HTTP_CONTENT_T);
    GOAL_RPC_POP(getFrag, GOAL_BOOL_T);
    if (pCbInfo->sc.pLenData) {
        /* data buffer defined for server -> client flow */
        GOAL_RPC_POP(*(pCbInfo->sc.pLenData), uint32_t);
        GOAL_RPC_POP_PTR(pHttpCtc->pTempBuf, (uint16_t) *(pCbInfo->sc.pLenData));
    }

    GOAL_HTTP_RPC_CLOSE();

    if (GOAL_RES_OK(res)) {
        /* set CTC fragmentation info */
        res = goal_httpChnSetCtcFrag(pCbInfo->pInst, (uint32_t) ((PtrCast) pCbInfo->hdlTransm), getFrag);
    }

    if (GOAL_RES_OK(res)) {
        /* set data pointer */
        *(pCbInfo->sc.ppData) = (const uint8_t *) (pHttpCtc->pTempBuf);
    }

    return res;
}


/****************************************************************************/
/** This function is used as application data callback function when CTC is
 * active. The HTTP request data is forwarded to the AC via RPC.
 *
 * @retval GOAL_OK on success
 * @retval other failed
 */
static GOAL_STATUS_T goal_httpCbDataCcPost(
    GOAL_HTTP_APPLCB_DATA_T *pCbInfo            /**< pointer to callback info */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_INSTANCE_T *pInstIt = NULL;            /* list iterator */
    GOAL_HTTP_INSTANCE_CTC_T *pHttpCtc = NULL;  /* HTTP CTC instance */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */
    GOAL_BOOL_T postFrag = GOAL_FALSE;          /* fragmentation bool */
    uint32_t postOff = 0;                       /* post offset */
    uint32_t postHeadLen = 0;                   /* post header length */
    uint32_t postDataLen = 0;                   /* post data length */
    uint8_t  *pData = NULL;                     /* post data pointer */
    GOAL_HTTP_STATUS_T httpStatus = GOAL_HTTP_STATUS_OK; /* HTTP return status */
    uint32_t lenFree = 0;                       /* free RPC stack size */
    uint16_t resId;                             /* resource id */

    /* find CTC instance */
    res = GOAL_ERR_NOT_FOUND;
    GOAL_INSTANCE_FOREACH((GOAL_INSTANCE_T *) pListCtcInst->pInst, pInstIt) {
        pHttpCtc = (GOAL_HTTP_INSTANCE_CTC_T *) pInstIt;

        if (pCbInfo->pInst == pHttpCtc->pHttpInst) {
            res = GOAL_OK;
            break;
        }
    }

    do {
        /* get new rpc handle */
        GOAL_HTTP_RPC_NEW(pHttpCtc);
        if (GOAL_RES_ERR(res)) {
            res++;
        }

        if ((GOAL_RES_OK(res)) && (pCbInfo->cs.lenData)) {
            /* calculate header length */
            postHeadLen = sizeof(pCbInfo->cs.lenData) + sizeof(pCbInfo->reqType) + sizeof(pCbInfo->hdlRes)  \
                        + sizeof(pCbInfo->hdlTransm) + pCbInfo->cs.lenUrl + sizeof(pCbInfo->cs.lenUrl)      \
                        + sizeof(pCbInfo->cs.lenMatch);
            /* check POST fragementation */
            //lenFree = GOAL_RPC_STACK_SIZE_FREE_GET(GOAL_TRUE) - 16;
            lenFree = GOAL_HTTP_RPC_STACK_SIZE_USABLE;
            if (lenFree < ((pCbInfo->cs.lenData - postOff) + postHeadLen)) {
                /* fragmentation necessary */
                postFrag = GOAL_TRUE;
                postDataLen = lenFree - postHeadLen;
            } else {
                postFrag = GOAL_FALSE;
                postDataLen = pCbInfo->cs.lenData - postOff;
            }
        }

        /* set rpc data */
        if (postDataLen) {
            pData = pCbInfo->cs.pData + postOff;
            GOAL_RPC_PUSH_PTR(pData, (uint16_t) postDataLen);
            if (GOAL_RES_ERR(res)) {
                res++;
            }
            postOff += postDataLen;
        }
        GOAL_RPC_PUSH(postDataLen);
        if (GOAL_RES_ERR(res)) {
            res++;
        }
        GOAL_RPC_PUSH(pCbInfo->reqType);
        if (GOAL_RES_ERR(res)) {
            res++;
        }
        res = goal_httpIdByHdl(&resId, pCbInfo->hdlRes);
        GOAL_RPC_PUSH(resId);
        if (GOAL_RES_ERR(res)) {
            res++;
        }
        GOAL_RPC_PUSH((uint32_t) ((PtrCast) pCbInfo->hdlTransm));
        if (GOAL_RES_ERR(res)) {
            res++;
        }
        if (pCbInfo->cs.lenUrl) {
            GOAL_RPC_PUSH_PTR(pCbInfo->cs.pUrl, (uint16_t) (pCbInfo->cs.lenUrl));
        }
        GOAL_RPC_PUSH(pCbInfo->cs.lenUrl);
        if (GOAL_RES_ERR(res)) {
            res++;
        }
        GOAL_RPC_PUSH(pCbInfo->cs.lenMatch);
        if (GOAL_RES_ERR(res)) {
            res++;
        }

        /* call function */
        GOAL_HTTP_RPC_CALL(pHttpCtc, GOAL_HTTP_CB_DATA_POST);
        if (GOAL_RES_ERR(res)) {
            res++;
        }

        /* get rpc return data */
        GOAL_RPC_POP(httpStatus, GOAL_HTTP_STATUS_T);
        if (GOAL_RES_ERR(res)) {
            res++;
        }
        GOAL_HTTP_RPC_CLOSE();
        if (GOAL_RES_ERR(res)) {
            res++;
        }
    } while ((GOAL_TRUE == postFrag) && (GOAL_RES_OK(res)));

    if (GOAL_RES_ERR(res)) {
        res++;
    }

    if ((GOAL_RES_OK(res)) && (pCbInfo->pRet)) {
        *(pCbInfo->pRet) = httpStatus;
    }

    return res;
}


/****************************************************************************/
/** This function is used as application template callback function when
 * CTC is active. The HTTP request data is forwarded to the AC via RPC.
 *
 * @retval GOAL_OK on success
 * @retval other failed
 */
static GOAL_STATUS_T goal_httpCbTemplCc(
    GOAL_HTTP_APPLCB_TEMPL_T *pCbInfo           /**< pointer to callback info */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    if (pCbInfo->in.pPath) {
        /* template list handling */
        res = goal_httpCbTemplListCc(pCbInfo);
    } else {
        /* simple template handling */
        res = goal_httpCbTemplSmplCc(pCbInfo);
    }
    return res;
}


/****************************************************************************/
/** This function is used as application template callback function when
 * CTC is active. The HTTP request data is forwarded to the AC via RPC.
 *
 * @retval GOAL_OK on success
 * @retval other failed
 */
static GOAL_STATUS_T goal_httpCbTemplSmplCc(
    GOAL_HTTP_APPLCB_TEMPL_T *pCbInfo           /**< pointer to callback info */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_INSTANCE_T *pInstIt = NULL;            /* list iterator */
    GOAL_HTTP_INSTANCE_CTC_T *pHttpCtc = NULL;  /* HTTP CTC instance */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */
    uint32_t lenName = 0;                       /* name length */
    uint32_t lenRet = 0;                        /* length of return string */
    uint16_t resId;                             /* resource id */

    /* find CTC instance */
    res = GOAL_ERR_NOT_FOUND;
    GOAL_INSTANCE_FOREACH((GOAL_INSTANCE_T *) pListCtcInst->pInst, pInstIt) {
        pHttpCtc = (GOAL_HTTP_INSTANCE_CTC_T *) pInstIt;

        if (pCbInfo->pInst == pHttpCtc->pHttpInst) {
            res = GOAL_OK;
            break;
        }
    }

    /* get template name length */
    lenName = (uint32_t) GOAL_STRLEN(pCbInfo->in.name);

    /* get new rpc handle */
    GOAL_HTTP_RPC_NEW(pHttpCtc);

    /* set rpc data */
    if (lenName) {
        GOAL_RPC_PUSH_PTR(pCbInfo->in.name, (uint16_t) lenName);
    }
    GOAL_RPC_PUSH(lenName);
    GOAL_RPC_PUSH(pCbInfo->in.retLenMax);
    res = goal_httpIdByHdl(&resId, pCbInfo->hdlRes);
    GOAL_RPC_PUSH(resId);
    /* call */
    GOAL_HTTP_RPC_CALL(pHttpCtc, GOAL_HTTP_CB_TEMPL_SMPL);
    /* get rpc data */
    GOAL_RPC_POP(lenRet, uint32_t);
    if (lenRet) {
        GOAL_RPC_POP_PTR(pCbInfo->out.strReturn, (uint16_t) lenRet);
    }

    GOAL_HTTP_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** This function sets basic authentication information.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_httpAuthBasSetUserInfoCc(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< RPC handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_HTTP_INSTANCE_CTC_T *pHttpCtc = NULL;  /* HTTP CTC instance */
    uint16_t lenUser = 0;                       /* user string length */
    uint16_t lenPw = 0;                         /* password string length */
    char strUser[128];                          /* user string */
    char strPw[128];                            /* password string */
    uint32_t userLevel = 0;                     /* user level */

    /* init strings */
    GOAL_MEMSET(strUser, 0, sizeof(strUser));
    GOAL_MEMSET(strPw, 0, sizeof(strPw));

    /* get rpc data */
    GOAL_HTTP_RPC_POP_INST(pHttpCtc);
    GOAL_RPC_POP(userLevel, uint32_t);
    GOAL_RPC_POP(lenUser, uint16_t);
    if (lenUser) {
        GOAL_RPC_POP_PTR(strUser, lenUser);
    }
    GOAL_RPC_POP(lenPw, uint16_t);
    if (lenPw) {
        GOAL_RPC_POP_PTR(strPw, lenPw);
    }

    if (GOAL_RES_OK(res)) {
        /* use internal function */
        res = goal_httpAuthBasSetUserInfoImpl(pHttpCtc->pHttpInst, userLevel, strUser, strPw);
    }

    return res;
}


/****************************************************************************/
/** This function is used as application template callback function when
 * CTC is active. The HTTP request data is forwarded to the AC via RPC.
 *
 * @retval GOAL_OK on success
 * @retval other failed
 */
static GOAL_STATUS_T goal_httpCbTemplListCc(
    GOAL_HTTP_APPLCB_TEMPL_T *pCbInfo           /**< pointer to callback info */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_INSTANCE_T *pInstIt = NULL;            /* list iterator */
    GOAL_HTTP_INSTANCE_CTC_T *pHttpCtc = NULL;  /* HTTP CTC instance */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */
    uint32_t lenName = 0;                       /* name length */
    uint32_t lenRet = 0;                        /* length of return string */
    uint32_t cnt;                               /* loop counter */
    uint16_t resId;                             /* resource id */

    /* find CTC instance */
    res = GOAL_ERR_NOT_FOUND;
    GOAL_INSTANCE_FOREACH((GOAL_INSTANCE_T *) pListCtcInst->pInst, pInstIt) {
        pHttpCtc = (GOAL_HTTP_INSTANCE_CTC_T *) pInstIt;

        if (pCbInfo->pInst == pHttpCtc->pHttpInst) {
            res = GOAL_OK;
            break;
        }
    }

    /* get template name length */
    lenName = (uint32_t) GOAL_STRLEN(pCbInfo->in.name);

    /* get new rpc handle */
    GOAL_HTTP_RPC_NEW(pHttpCtc);

    /* set rpc data */
    if ((pCbInfo->in.pPath) && (pCbInfo->in.pPath->pathLen)) {
        for (cnt = 0; cnt < pCbInfo->in.pPath->pathLen; cnt++) {
            GOAL_RPC_PUSH(pCbInfo->in.pPath->path[cnt].index);
            GOAL_RPC_PUSH(pCbInfo->in.pPath->path[cnt].listId);
            GOAL_RPC_PUSH(pCbInfo->in.pPath->path[cnt].offset);
        }
    }
    GOAL_RPC_PUSH(pCbInfo->in.pPath->pathLen);
    if (lenName) {
        GOAL_RPC_PUSH_PTR(pCbInfo->in.name, (uint16_t) lenName);
    }
    GOAL_RPC_PUSH(lenName);
    GOAL_RPC_PUSH(pCbInfo->in.retLenMax);
    res = goal_httpIdByHdl(&resId, pCbInfo->hdlRes);
    GOAL_RPC_PUSH(resId);
    /* call */
    GOAL_HTTP_RPC_CALL(pHttpCtc, GOAL_HTTP_CB_TEMPL_LIST);
    /* get rpc data */
    GOAL_RPC_POP(lenRet, uint32_t);
    if (lenRet) {
        GOAL_RPC_POP_PTR(pCbInfo->out.strReturn, (uint16_t) lenRet);
    }

    GOAL_HTTP_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** This function adds a new template list at the HTTP server by request of
 * the AC.
 *
 * @retval GOAL_OK on success
 * @retval other failed
 */
GOAL_STATUS_T goal_httpTmpMgrNewListCc(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< RPC handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_HTTP_INSTANCE_CTC_T *pHttpCtc = NULL;  /* CTC instance */
    GOAL_HTTP_TEMPLATE_LIST_INIT_T listInit;    /* list init struct */
    uint32_t lenName = 0;                       /* list name length */

    /* init init struct */
    GOAL_MEMSET(&listInit, 0, sizeof(listInit));

    /* get rpc data */
    GOAL_HTTP_RPC_POP_INST(pHttpCtc);
    GOAL_RPC_POP(listInit.listId, uint32_t);
    GOAL_RPC_POP(listInit.cntMemb, uint32_t);
    GOAL_RPC_POP(lenName, uint32_t);
    if (lenName) {
        GOAL_RPC_POP_PTR(listInit.listName, (uint16_t) lenName);
    }

    /* add new list */
    if (GOAL_RES_OK(res)) {
        res = goal_httpTmpMgrNewListImpl(pHttpCtc->pHttpInst, &listInit);
    }

    return res;
}


/****************************************************************************/
/** This function adds a new resource at the HTTP server by request of
 * the AC.
 *
 * @retval GOAL_OK on success
 * @retval other failed
 */
GOAL_STATUS_T goal_httpResRegCc(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< RPC handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_HTTP_INSTANCE_CTC_T *pHttpCtc = NULL;  /* CTC instance */
    uint32_t idCbTempl = 0;                     /* id for data callback (CTC) */
    uint32_t idCbData = 0;                      /* id for template callback (CTC) */
    uint32_t strLen = 0;                        /* URL string length */
    uint32_t allowMeth = 0;                     /* allowed methods */
    GOAL_HTTP_HDL_T hdlRes = 0;                 /* resource handle */
    uint8_t strUrl[128];                        /* URL string */
    GOAL_HTTP_REQCB_T pfnCbData = NULL;         /* data cb function pointer */
    GOAL_HTTP_TMPCB_T pfnCbTempl = NULL;        /* template cb function pointer */
    uint16_t resId;                             /* resource id */

    goal_logInfo("called");

    GOAL_MEMSET(strUrl, 0, 128);

    /* get rpc data */
    GOAL_HTTP_RPC_POP_INST(pHttpCtc);
    GOAL_RPC_POP(idCbTempl, uint32_t);
    GOAL_RPC_POP(idCbData, uint32_t);
    GOAL_RPC_POP(allowMeth, uint32_t);
    GOAL_RPC_POP(strLen, uint32_t);
    GOAL_RPC_POP_PTR(strUrl, (uint16_t) strLen);

    if ((GOAL_RES_OK(res)) && (pHttpCtc)) {
        /* set data callback pointer */
        if (idCbData) {
            pfnCbData = goal_httpCbDataCc;
        }

        /* set template data pointer */
        if (idCbTempl) {
            pfnCbTempl = goal_httpCbTemplCc;
        }

        /* register new resource */
        res = goal_httpResRegImpl(pHttpCtc->pHttpInst, strUrl, allowMeth,
                                  pfnCbData, pfnCbTempl, &hdlRes);
    }

    /* return resource handle to AC */
    res = goal_httpIdByHdl(&resId, hdlRes);
    GOAL_RPC_PUSH(resId);

    return res;
}


/****************************************************************************/
/** This function adds a new  HTTP server instance by request of the AC.
 *
 * @retval GOAL_OK on success
 * @retval other failed
 */
GOAL_STATUS_T goal_httpNewCc(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< RPC handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    uint16_t cntChn = 0;                        /* channel count */
    uint16_t port = 0;                          /* TCP port */
    uint32_t idInstCtc = 0;                     /* CTC instance id */
    GOAL_HTTP_INSTANCE_CTC_T *pHttpCtc = NULL;  /* HTTP CTC instance */
    GOAL_HTTP_T *pHttp = NULL;                  /* HTTP instance */

    UNUSEDARG(pHdlRpc);

#if GOAL_CONFIG_MEDIA_MI_ETH == 1
    /* it is required to start networking before stack initialization */

    /* open up Ethernet MI */
    if (GOAL_RES_OK(res)) {
        res = goal_miEthOpen(GOAL_MI_ID, GOAL_MA_ID_ETH);
    }

    /* open up NET */
    if (GOAL_RES_OK(res)) {
        res = goal_miNetOpen(GOAL_MI_ID, GOAL_MA_ID_ETH);
    }
#endif

    /* get rpc data */
    GOAL_RPC_POP(idInstCtc, uint32_t);
    GOAL_RPC_POP(cntChn, uint16_t);
    GOAL_RPC_POP(port, uint16_t);

    /* new HTTP CTC instance */
    res = goal_instNew((GOAL_INSTANCE_T **) &pHttpCtc, sizeof(GOAL_HTTP_INSTANCE_CTC_T), GOAL_ID_HTTP, idInstCtc,
                    "HTTP Instance CTC CC");

    /* setup RPC channel for http data exchange */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcSetupChannel(&(pHttpCtc->pHdlRpcChn), GOAL_ID_MI_CTC_DEFAULT);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to setup RPC channel");
        }
    }

    /* new HTTP instance */
    if (GOAL_RES_OK(res)) {
        res = goal_httpNewImpl(&pHttp, port, cntChn);
    }

    /* link HTTP instance to HTTP CTC instance */
    if (GOAL_RES_OK(res)) {
        pHttpCtc->pHttpInst = pHttp;
        goal_logInfo("HTTP Communication Core successfully started");
    }

    /* allocate memory for temporary buffer */
    if (GOAL_RES_OK(res)) {
        res = goal_memCalloc(&(pHttpCtc->pTempBuf), GOAL_HTTP_RPC_STACK_SIZE);
        if (GOAL_RES_OK (res)) {
            /* set buffer size */
            pHttpCtc->tempBufSize = GOAL_HTTP_RPC_STACK_SIZE;
        }
    }

    return res;
}


/****************************************************************************/
/** This function adds a new secure HTTP server instance by request of the AC.
 *
 * @retval GOAL_OK on success
 * @retval other failed
 */
GOAL_STATUS_T goal_httpsNewCc(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< RPC handle */
)
{
#if GOAL_CONFIG_HTTPS == 1
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    uint16_t cntChn = 0;                        /* channel count */
    uint16_t port = 0;                          /* TCP port */
    uint32_t idInstCtc = 0;                     /* CTC instance id */
    GOAL_HTTP_INSTANCE_CTC_T *pHttpCtc = NULL;  /* HTTP CTC instance */
    GOAL_HTTP_T *pHttp = NULL;                  /* HTTP instance */

    UNUSEDARG(pHdlRpc);

#if GOAL_CONFIG_MEDIA_MI_ETH == 1
    /* it is required to start networking before stack initialization */

    /* open up Ethernet MI */
    if (GOAL_RES_OK(res)) {
        res = goal_miEthOpen(GOAL_MI_ID, GOAL_MA_ID_ETH);
    }

    /* open up NET */
    if (GOAL_RES_OK(res)) {
        res = goal_miNetOpen();
    }
#endif

    /* get rpc data */
    GOAL_RPC_POP(idInstCtc, uint32_t);
    GOAL_RPC_POP(cntChn, uint16_t);
    GOAL_RPC_POP(port, uint16_t);

    /* new HTTP CTC instance */
    res = goal_instNew((GOAL_INSTANCE_T **) &pHttpCtc, sizeof(GOAL_HTTP_INSTANCE_CTC_T), GOAL_ID_HTTP, idInstCtc,
                    "HTTP Instance CTC CC");

    /* setup RPC channel for http data exchange */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcSetupChannel(&(pHttpCtc->pHdlRpcChn), GOAL_ID_MI_CTC_DEFAULT);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to setup RPC channel");
        }
    }

    /* new HTTP instance */
    if (GOAL_RES_OK(res)) {
        res = goal_httpsNewImpl(&pHttp, port, cntChn);
    }

    /* link HTTP instance to HTTP CTC instance */
    if (GOAL_RES_OK(res)) {
        pHttpCtc->pHttpInst = pHttp;
        goal_logInfo("HTTP Communication Core successfully started");
    }

    /* allocate memory for temporary buffer */
    if (GOAL_RES_OK(res)) {
        res = goal_memCalloc(&(pHttpCtc->pTempBuf), GOAL_HTTP_RPC_STACK_SIZE);
        if (GOAL_RES_OK (res)) {
            /* set buffer size */
            pHttpCtc->tempBufSize = GOAL_HTTP_RPC_STACK_SIZE;
        }
    }

    return res;
#else
    UNUSEDARG(pHdlRpc);
    return GOAL_ERR_UNSUPPORTED;
#endif /* GOAL_CONFIG_HTTPS == 1 */
}


/****************************************************************************/
/** Initialize HTTP RPC
 *
 * @retval GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_httpRpcInit(
    void
)
{
    GOAL_STATUS_T res;                          /* result */
    unsigned int cnt;                           /* counter */
    GOAL_RPC_HDL_CHN_T *pHdlRpcChn = GOAL_RPC_HDL_NONE; /* acyclic transmission handle */

    /* register RPC functions */
    for (cnt = 0; cnt < ARRAY_ELEMENTS(goal_httpTblFunc); cnt++) {
        res = goal_rpcRegisterService(GOAL_ID_HTTP, (GOAL_RPC_FUNC_ID) goal_httpTblFunc[cnt].id, goal_httpTblFunc[cnt].pFunc);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to register HTTP RPC function nr %u", goal_httpTblFunc[cnt].id);
            return res;
        }
    }

    /* register RPC CB functions */
    for (cnt = 0; cnt < ARRAY_ELEMENTS(goal_httpTblCbFunc); cnt++) {
        res = goal_rpcRegisterService(GOAL_ID_HTTP, (GOAL_RPC_FUNC_ID) goal_httpTblCbFunc[cnt].id, NULL);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to register HTTP RPC function nr %u", goal_httpTblCbFunc[cnt].id);
            return res;
        }
    }

    /* setup a channel for RPC */
    res = goal_rpcSetupChannel(&pHdlRpcChn, GOAL_ID_MI_CTC_DEFAULT);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to setup RPC channel");
    }

    /* create the HTTP CTC instance list */
    res = goal_instListNew(&pListCtcInst, GOAL_ID_HTTP);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create HTTP CTC instance list");
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Initialize HTTP on CC
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_httpInitCc(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* set size of RPC stack */
    res = goal_rpcSetStackMax(GOAL_HTTP_RPC_STACK_SIZE);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to set RPC stack size");
        return res;
    }

    /* initialize the HTTP server */
    res = goal_httpInitImpl();
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to initialize the HTTP server");
        return res;
    }

    /* add stage handler to register HTTP RPC functions */
    res = goal_mainStageReg(GOAL_STAGE_MODULES, &mStageMod, GOAL_STAGE_INIT, goal_httpRpcInit);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to register HTTP RPC init in modules stage");
    }

    return res;
}

