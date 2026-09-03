/** @file
 *
 * @brief Implementation of the Goal HTTP RPC wrapper for AC
 *
 * Implementation of the HTTP RPC wrapper for data transmission
 * between AC and CC.
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
#include <goal_media/goal_mi_mctc.h>


/****************************************************************************/
/* Local types */
/****************************************************************************/

/**< id to function mapping */
typedef struct {
    unsigned int id;                            /**< function id */
} GOAL_HTTP_FUNC_PTR_T;

/* id to function mapping */
#undef GOAL_HTTP_FUNC_ENTRY
#define GOAL_HTTP_FUNC_ENTRY(id, name, func) { id },
static GOAL_HTTP_FUNC_PTR_T goal_httpTblFunc[] = {
    GOAL_HTTP_FUNC_LIST
};

/**< id to function mapping */
typedef struct {
    unsigned int id;                            /**< function id */
    GOAL_RPC_FUNC_T pFunc;                      /**< function ptr */
} GOAL_HTTP_CB_FUNC_PTR_T;

/* id to function mapping */
#undef GOAL_HTTP_FUNC_CB_ENTRY
#define GOAL_HTTP_FUNC_CB_ENTRY(id, name, func) { id, func },
static GOAL_HTTP_CB_FUNC_PTR_T goal_httpTblCbFunc[] = {
    GOAL_HTTP_FUNC_CB_LIST
};


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_INSTANCE_LIST_T *pListCtcInst = NULL; /**< HTTP instance list */
static GOAL_STAGE_HANDLER_T mStageMod;          /**< modules stage handler */


/****************************************************************************/
/** This function resets channel information for the specified channel.
 *
 * @retval GOAL_OK on success
 * @retval other failed
 */
GOAL_STATUS_T goal_httpDataCloseChnAc(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< RPC handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_HTTP_INSTANCE_CTC_T *pHttpCtc = NULL;  /* CTC instance */
    GOAL_HTTP_HDL_T hdlTransm = 0;              /* transmission handle */
    GOAL_HTTP_CHN_AC_LL_T *pChn;                /* channel pointer */
    uint32_t clearOff = 0;                      /* clearing offset */

    /* get rpc data */
    GOAL_HTTP_RPC_POP_INST(pHttpCtc);
    GOAL_RPC_POP(hdlTransm, GOAL_HTTP_HDL_T);

    /* find transmission handle */
    GOAL_LL_FOREACH(pHttpCtc->pChn, pChn) {
        if (pChn->hdlTransm == hdlTransm) {
            /* clear channel, excluding next pointer */
            clearOff = sizeof(GOAL_HTTP_CHN_AC_LL_T *);
            GOAL_MEMSET((((char *) pChn) + clearOff), 0, (sizeof(GOAL_HTTP_CHN_AC_LL_T) - clearOff));
            res = GOAL_OK;
            break;
        }
    }

    return res;
}


/****************************************************************************/
/** This function is used when CTC fragmentation is necessary.
 *
 * @retval GOAL_OK on success
 * @retval other failed
 */
GOAL_STATUS_T goal_httpDataContAc(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< RPC handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_HTTP_INSTANCE_CTC_T *pHttpCtc = NULL;  /* CTC instance */
    GOAL_HTTP_HDL_T hdlTransm = 0;              /* transmission handle */
    GOAL_HTTP_CHN_AC_LL_T *pChn;                /* channel pointer */
    const uint8_t* pData;                       /* data pointer */
    uint32_t lenData;                           /* data length */
    uint32_t lenHeader = 0;                     /* length of header */
    uint16_t lenFree = 0;                       /* free RPC stack data size */

    /* get rpc data */
    GOAL_HTTP_RPC_POP_INST(pHttpCtc);
    GOAL_RPC_POP(hdlTransm, GOAL_HTTP_HDL_T);

    /* find transmission handle */
    res = GOAL_ERR_NOT_FOUND;
    GOAL_LL_FOREACH(pHttpCtc->pChn, pChn) {
        if (pChn->hdlTransm == hdlTransm) {
            res = GOAL_OK;
            break;
        }
    }

    if (GOAL_RES_OK(res)) {
        /* increase fragment counter */
        pChn->ctcFrag.cntFrag++;
        /* set transmission info */
        lenHeader = sizeof(GOAL_BOOL_T) + sizeof(lenData);
        pData = pChn->pData + pChn->offset;
        /* TODO: check if there is a substitute */
        //lenFree = GOAL_RPC_STACK_SIZE_FREE_GET(GOAL_TRUE);
        lenFree = GOAL_HTTP_RPC_STACK_SIZE_USABLE;
        if (pChn->ctcFrag.cntFrag < pChn->ctcFrag.cntFragTot) {
            pChn->ctcFrag.getFrag = GOAL_TRUE;
            lenData = lenFree - lenHeader;
            pChn->offset += lenData;
        } else
        {
            pChn->ctcFrag.getFrag = GOAL_FALSE;
            lenData = pChn->lenData - pChn->offset;
            if (lenFree < (lenData + lenHeader)) {
                /* due to header transmission in every cycle, stack size is exceeded */
                pChn->ctcFrag.cntFragTot++;
                pChn->ctcFrag.getFrag = GOAL_TRUE;
                lenData = lenFree - lenHeader;
                pChn->offset += lenData;
            }
        }

        GOAL_RPC_PUSH_PTR(pData, (uint16_t) lenData);
        GOAL_RPC_PUSH(lenData);
        GOAL_RPC_PUSH(pChn->ctcFrag.getFrag);
    }

    return res;
}


/****************************************************************************/
/** This function is used as application data callback function when CTC is
 * active. The HTTP request data is received from the CC via RPC.
 *
 * @retval GOAL_OK on success
 * @retval other failed
 */
GOAL_STATUS_T goal_httpCbDataAcPost(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< RPC handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_HTTP_INSTANCE_CTC_T *pHttpCtc = NULL;  /* CTC instance */
    GOAL_HTTP_APPLCB_DATA_T cbInfo;             /* callback information struct */
    GOAL_HTTP_AC_RES_LL_T *pRes = NULL;         /* resource pointer */
    GOAL_HTTP_STATUS_T httpStatus = GOAL_HTTP_STATUS_OK; /* HTTP status */
    GOAL_HTTP_CONTENT_T httpContent = GOAL_HTTP_CONTENT_UNK; /* HTTP content type */
    GOAL_HTTP_REQCB_T pfnCbData = NULL;         /* callback function pointer */

    /* init callback information struct */
    GOAL_MEMSET(&cbInfo, 0, sizeof(cbInfo));

    /* get rpc data */
    GOAL_HTTP_RPC_POP_INST(pHttpCtc);
    GOAL_RPC_POP(cbInfo.cs.lenMatch, uint32_t);
    GOAL_RPC_POP(cbInfo.cs.lenUrl, uint32_t);
    if (cbInfo.cs.lenUrl) {
        /* copy url to temp buffer */
        GOAL_MEMSET(&pHttpCtc->strUrl[0], 0, GOAL_HTTP_URL_MAX_LENGTH);
        GOAL_RPC_POP_PTR(&pHttpCtc->strUrl[0], (uint16_t) (cbInfo.cs.lenUrl));
        /* set pointer to url */
        cbInfo.cs.pUrl = &pHttpCtc->strUrl[0];
    }
    GOAL_RPC_POP(cbInfo.hdlTransm, GOAL_HTTP_HDL_T);
    GOAL_RPC_POP(cbInfo.hdlRes, GOAL_HTTP_HDL_T);
    GOAL_RPC_POP(cbInfo.reqType, GOAL_HTTP_FWCB_T);
    GOAL_RPC_POP(cbInfo.cs.lenData, uint32_t);
    if (cbInfo.cs.lenData) {
        /* copy data */
        GOAL_RPC_POP_PTR((pHttpCtc->pCtcTemp), (uint16_t) (cbInfo.cs.lenData));
        /* set data pointer */
        cbInfo.cs.pData = pHttpCtc->pCtcTemp;
    };

    if (GOAL_RES_OK(res)) {
        /* search resource by handle */
        res = GOAL_ERR_NOT_FOUND;
        GOAL_LL_FOREACH(pHttpCtc->pResList, pRes) {
            if (pRes->hdlRes == cbInfo.hdlRes) {
                /* set callback function pointer */
                pfnCbData = pRes->pfnCbData;
                res = GOAL_OK;
                break;
            }
        }
    }

    if (GOAL_RES_OK(res)) {
        /* use callback function */
        if (NULL == pfnCbData) {
            /* error */
            res = GOAL_ERR_NULL_POINTER;
        } else {
            /* set callback information return pointer */
            cbInfo.sc.pContType = &httpContent;
            cbInfo.pRet = &httpStatus;
            res = (*pfnCbData)(&cbInfo);
        }
    }

    /* set rpc return data */
    GOAL_RPC_PUSH(httpStatus);

    return res;
}


/****************************************************************************/
/** This function is used as application data callback function when CTC is
 * active. The HTTP request data is received from the CC via RPC.
 *
 * @retval GOAL_OK on success
 * @retval other failed
 */
GOAL_STATUS_T goal_httpCbDataAcGet(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< RPC handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_HTTP_INSTANCE_CTC_T *pHttpCtc = NULL;  /* CTC instance */
    GOAL_HTTP_APPLCB_DATA_T cbInfo;             /* callback information struct */
    GOAL_HTTP_AC_RES_LL_T *pRes = NULL;         /* resource pointer */
    GOAL_HTTP_REQCB_T pfnCbData = NULL;         /* callback function pointer */
    GOAL_HTTP_STATUS_T httpStatus = GOAL_HTTP_STATUS_OK; /* HTTP status */
    GOAL_HTTP_CONTENT_T httpContent = GOAL_HTTP_CONTENT_UNK; /* HTTP content type */
    uint32_t dataLen = 0;                       /* data length */
    const uint8_t *pData = NULL;                /* return data pointer */
    uint32_t lenHeader;                         /* length of header data for fragmentation calculation */
    GOAL_HTTP_CTC_FRAG_T ctcFrag;               /* CTC fragmentation (return data) */
    GOAL_HTTP_CHN_AC_LL_T *pChn;                /* pointer to channel */
    uint16_t lenFree = 0;                       /* free stack data length */

    /* init callback information struct */
    GOAL_MEMSET(&cbInfo, 0, sizeof(cbInfo));
    GOAL_MEMSET(&ctcFrag, 0, sizeof(ctcFrag));

    /* get rpc data */
    GOAL_HTTP_RPC_POP_INST(pHttpCtc);
    GOAL_RPC_POP(cbInfo.cs.lenMatch, uint32_t);
    GOAL_RPC_POP(cbInfo.cs.lenUrl, uint32_t);
    if (cbInfo.cs.lenUrl) {
        GOAL_MEMSET(&pHttpCtc->strUrl[0], 0, GOAL_HTTP_URL_MAX_LENGTH);
        GOAL_RPC_POP_PTR(&pHttpCtc->strUrl[0], (uint16_t) (cbInfo.cs.lenUrl));
        cbInfo.cs.pUrl = &pHttpCtc->strUrl[0];
    }
    GOAL_RPC_POP(cbInfo.hdlTransm, GOAL_HTTP_HDL_T);
    GOAL_RPC_POP(cbInfo.hdlRes, GOAL_HTTP_HDL_T);
    GOAL_RPC_POP(cbInfo.reqType, GOAL_HTTP_FWCB_T);

    if (GOAL_RES_OK(res)) {
        /* search resource by handle */
        res = GOAL_ERR_NOT_FOUND;
        GOAL_LL_FOREACH(pHttpCtc->pResList, pRes) {
            if (pRes->hdlRes == cbInfo.hdlRes) {
                /* set callback function pointer */
                pfnCbData = pRes->pfnCbData;
                res = GOAL_OK;
                break;
            }
        }
    }

    if (GOAL_RES_OK(res)) {
        /* use callback function */
        if (NULL == pfnCbData) {
            /* error */
            res = GOAL_ERR_NULL_POINTER;
        } else {
            /* set callback information return pointer */
            cbInfo.sc.pLenData = &dataLen;
            cbInfo.sc.ppData = &pData;
            cbInfo.sc.pContType = &httpContent;
            cbInfo.pRet = &httpStatus;
            res = (*pfnCbData)(&cbInfo);
        }
    }

    /* set size of header information */
    lenHeader = sizeof(dataLen) + sizeof(httpContent) + sizeof(httpStatus) + sizeof(GOAL_BOOL_T);
    //lenFree = GOAL_RPC_STACK_SIZE_FREE_GET(GOAL_TRUE);
    lenFree = GOAL_HTTP_RPC_STACK_SIZE_USABLE;
    /* calculate fragmentation */
    if (lenFree < (dataLen + lenHeader)) {
        /* set flag */
        ctcFrag.getFrag = GOAL_TRUE;
        ctcFrag.hdlRes = cbInfo.hdlRes;
        ctcFrag.cntFrag = 1;
        ctcFrag.cntFragTot = (dataLen + lenHeader) / lenFree;
        if ((dataLen + lenHeader) % lenFree) {
            ctcFrag.cntFragTot++;
        }

        /* search free channel */
        res = GOAL_ERR_NOT_FOUND;
        GOAL_LL_FOREACH(pHttpCtc->pChn, pChn) {
            if ((0 == pChn->hdlTransm) || (pChn->hdlTransm == cbInfo.hdlTransm)) {
                /* found free slot, set CTC fragmentation info */
                GOAL_MEMCPY(&(pChn->ctcFrag), &ctcFrag, sizeof(ctcFrag));
                pChn->hdlTransm = cbInfo.hdlTransm;
                pChn->pData = pData;
                pChn->offset = lenFree - lenHeader;
                pChn->lenData = dataLen;
                res = GOAL_OK;
                break;
            }
        }

        if (GOAL_RES_OK(res)) {
            /* fragmentation necessary */
            GOAL_RPC_PUSH_PTR(pData, (uint16_t) (lenFree - lenHeader));
            GOAL_RPC_PUSH((lenFree - lenHeader));
            GOAL_RPC_PUSH(ctcFrag.getFrag);
            GOAL_RPC_PUSH(httpContent);
            GOAL_RPC_PUSH(httpStatus);
        }
    } else {
        if (GOAL_RES_OK(res)) {
            /* set rpc return data */
            GOAL_RPC_PUSH_PTR(pData, (uint16_t) dataLen);
            GOAL_RPC_PUSH(dataLen);
            GOAL_RPC_PUSH(GOAL_FALSE);
            GOAL_RPC_PUSH(httpContent);
            GOAL_RPC_PUSH(httpStatus);
        }
    }

    return res;
}


/****************************************************************************/
/** This function is used as application data callback function when CTC is
 * active. The HTTP request data is received from the CC via RPC.
 *
 * @retval GOAL_OK on success
 * @retval other failed
 */
GOAL_STATUS_T goal_httpCbTransDoneAc(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< RPC handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_HTTP_INSTANCE_CTC_T *pHttpCtc = NULL;  /* CTC instance */
    GOAL_HTTP_APPLCB_DATA_T cbInfo;             /* callback information struct */
    GOAL_HTTP_AC_RES_LL_T *pRes = NULL;         /* resource pointer */
    GOAL_HTTP_REQCB_T pfnCbData = NULL;         /* callback function pointer */
    GOAL_HTTP_STATUS_T httpStatus = GOAL_HTTP_STATUS_OK; /* HTTP status */
    GOAL_HTTP_CONTENT_T httpContent = GOAL_HTTP_CONTENT_UNK; /* HTTP content type */
    uint32_t dataLen = 0;                       /* data length */
    const uint8_t *pData = NULL;                /* return data pointer */
    GOAL_HTTP_CTC_FRAG_T ctcFrag;               /* CTC fragmentation (return data) */

    /* init callback information struct */
    GOAL_MEMSET(&cbInfo, 0, sizeof(cbInfo));
    GOAL_MEMSET(&ctcFrag, 0, sizeof(ctcFrag));

    /* get rpc data */
    GOAL_HTTP_RPC_POP_INST(pHttpCtc);
    GOAL_RPC_POP(cbInfo.hdlTransm, GOAL_HTTP_HDL_T);
    GOAL_RPC_POP(cbInfo.hdlRes, GOAL_HTTP_HDL_T);
    GOAL_RPC_POP(cbInfo.reqType, GOAL_HTTP_FWCB_T);

    if (GOAL_RES_OK(res)) {
        /* search resource by handle */
        res = GOAL_ERR_NOT_FOUND;
        GOAL_LL_FOREACH(pHttpCtc->pResList, pRes) {
            if (pRes->hdlRes == cbInfo.hdlRes) {
                /* set callback function pointer */
                pfnCbData = pRes->pfnCbData;
                res = GOAL_OK;
                break;
            }
        }
    }

    if (GOAL_RES_OK(res)) {
        /* use callback function */
        if (NULL == pfnCbData) {
            /* error */
            res = GOAL_ERR_NULL_POINTER;
        } else {
            /* set callback information return pointer */
            cbInfo.sc.pLenData = &dataLen;
            cbInfo.sc.ppData = &pData;
            cbInfo.sc.pContType = &httpContent;
            cbInfo.pRet = &httpStatus;
            if (cbInfo.reqType != GOAL_HTTP_FW_REQ_DONE_OK) {
                res = (*pfnCbData)(&cbInfo);
            }
        }
    }

    if (GOAL_RES_OK(res)) {
        /* set rpc return data */
        GOAL_RPC_PUSH(GOAL_FALSE);
        GOAL_RPC_PUSH(httpContent);
        GOAL_RPC_PUSH(httpStatus);
    }

    return res;
}


/****************************************************************************/
/** This function is used as application template callback function
 * when CTC is active. The HTTP request data is received from the CC via RPC.
 *
 * @retval GOAL_OK on success
 * @retval other failed
 */
GOAL_STATUS_T goal_httpCbTemplListAc(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< RPC handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_HTTP_INSTANCE_CTC_T *pHttpCtc = NULL;  /* CTC instance */
    GOAL_HTTP_APPLCB_TEMPL_T cbInfo;            /* callback information struct */
    uint32_t lenName = 0;                       /* length of name */
    GOAL_HTTP_TMPCB_T pfnCbTempl = NULL;        /* pointer to template callback */
    GOAL_HTTP_AC_RES_LL_T *pRes = NULL;         /* pointer to resource list element */
    uint32_t lenRet = 0;                        /* length of return */
    GOAL_HTTP_TMPMGR_L_PATH_T path;             /* list path */
    uint32_t cnt;                               /* loop counter */

    /* init string */
    GOAL_MEMSET(&cbInfo, 0, sizeof(cbInfo));
    GOAL_MEMSET(&path, 0, sizeof(path));

    /* get rpc data */
    GOAL_HTTP_RPC_POP_INST(pHttpCtc);
    GOAL_RPC_POP(cbInfo.hdlRes, GOAL_HTTP_HDL_T);
    GOAL_RPC_POP(cbInfo.in.retLenMax, uint32_t);
    GOAL_RPC_POP(lenName, uint32_t);
    if (lenName) {
        GOAL_MEMSET(&pHttpCtc->strUrl[0], 0, GOAL_HTTP_URL_MAX_LENGTH);
        GOAL_RPC_POP_PTR(&pHttpCtc->strUrl[0], (uint16_t) lenName);
    }

    GOAL_RPC_POP(path.pathLen, uint32_t);
    if (path.pathLen) {
        for (cnt = path.pathLen; cnt > 0; cnt--) {
            GOAL_RPC_POP(path.path[cnt - 1].offset, uint32_t);
            GOAL_RPC_POP(path.path[cnt - 1].listId, uint32_t);
            GOAL_RPC_POP(path.path[cnt - 1].index, uint32_t);
        }
        /* set pointer */
        cbInfo.in.pPath = &path;
    }

    if (GOAL_RES_OK(res)) {
        /* search resource by handle */
        res = GOAL_ERR_NOT_FOUND;
        GOAL_LL_FOREACH(pHttpCtc->pResList, pRes) {
            if (pRes->hdlRes == cbInfo.hdlRes) {
                /* set callback function pointer */
                pfnCbTempl = pRes->pfnCbTempl;
                res = GOAL_OK;
                break;
            }
        }
    }

    /* use callback function */
    if (GOAL_RES_OK(res)) {
        if (NULL == pfnCbTempl) {
            /* error */
            res = GOAL_ERR_NULL_POINTER;
        } else {
            /* init buffer */
            GOAL_MEMSET(pHttpCtc->strRet, 0, sizeof(pHttpCtc->strRet));

            /* set callback info */
            cbInfo.in.name = (char *) &pHttpCtc->strUrl[0];
            cbInfo.out.strReturn = &pHttpCtc->strRet[0];
            /* use callback */
            res = (*pfnCbTempl)(&cbInfo);
        }
    }

    if (GOAL_RES_OK(res)) {
        /* get return string length */
        lenRet = (uint32_t) GOAL_STRLEN(&pHttpCtc->strRet[0]);

    }

    GOAL_RPC_PUSH_PTR(&pHttpCtc->strRet[0], (uint16_t) lenRet);
    GOAL_RPC_PUSH(lenRet);

    return res;
}


/****************************************************************************/
/** This function is used as application template callback function
 * when CTC is active. The HTTP request data is received from the CC via RPC.
 *
 * @retval GOAL_OK on success
 * @retval other failed
 */
GOAL_STATUS_T goal_httpCbTemplSmplAc(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< RPC handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_HTTP_INSTANCE_CTC_T *pHttpCtc = NULL;  /* CTC instance */
    GOAL_HTTP_APPLCB_TEMPL_T cbInfo;            /* callback information struct */
    uint32_t lenName = 0;                       /* length of name */
    GOAL_HTTP_TMPCB_T pfnCbTempl = NULL;        /* pointer to template callback */
    GOAL_HTTP_AC_RES_LL_T *pRes = NULL;         /* pointer to resource list element */
    uint32_t lenRet = 0;                        /* length of return */

    /* init string */
    GOAL_MEMSET(&cbInfo, 0, sizeof(cbInfo));

    /* get rpc data */
    GOAL_HTTP_RPC_POP_INST(pHttpCtc);
    GOAL_RPC_POP(cbInfo.hdlRes, GOAL_HTTP_HDL_T);
    GOAL_RPC_POP(cbInfo.in.retLenMax, uint32_t);
    GOAL_RPC_POP(lenName, uint32_t);
    if (lenName) {
        GOAL_MEMSET(&pHttpCtc->strUrl[0], 0, GOAL_HTTP_URL_MAX_LENGTH);
        GOAL_RPC_POP_PTR(&pHttpCtc->strUrl[0], (uint16_t) lenName);
    }

    if (GOAL_RES_OK(res)) {
        /* search resource by handle */
        res = GOAL_ERR_NOT_FOUND;
        GOAL_LL_FOREACH(pHttpCtc->pResList, pRes) {
            if (pRes->hdlRes == cbInfo.hdlRes) {
                /* set callback function pointer */
                pfnCbTempl = pRes->pfnCbTempl;
                res = GOAL_OK;
                break;
            }
        }
    }

    /* use callback function */
    if (GOAL_RES_OK(res)) {
        if (NULL == pfnCbTempl) {
            /* error */
            res = GOAL_ERR_NULL_POINTER;
        } else {
            /* set callback info */
            cbInfo.in.name = (char *) &pHttpCtc->strUrl[0];
            cbInfo.out.strReturn = &pHttpCtc->strRet[0];
            /* use callback */
            res = (*pfnCbTempl)(&cbInfo);
        }
    }

    if (GOAL_RES_OK(res)) {
        /* get return string length */
        lenRet = (uint32_t) GOAL_STRLEN(&pHttpCtc->strRet[0]);
    }

    GOAL_RPC_PUSH_PTR(&pHttpCtc->strRet[0], (uint16_t) lenRet);
    GOAL_RPC_PUSH(lenRet);

    return res;
}


/****************************************************************************/
/** This function sets basic authentication information.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_httpAuthBasSetUserInfoAc(
    GOAL_HTTP_T         *pInst,                 /**< instance pointer */
    uint32_t            userLevel,              /**< user level */
    char                *strUser,               /**< user name string */
    char                *strPw                  /**< password string */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */
    uint16_t lenUser = 0;                       /* user string length */
    uint16_t lenPw = 0;                         /* password string length */

    /* get new rpc handle */
    GOAL_HTTP_RPC_NEW(pInst);

    if (GOAL_RES_OK(res)) {
        if (strUser) {
            /* get string length */
            lenUser = (uint16_t) GOAL_STRLEN(strUser);
        } else
        {
            /* error */
            res = GOAL_ERR_NULL_POINTER;
        }
    }

    if (GOAL_RES_OK(res)) {
        if (strPw) {
            /* get string length */
            lenPw = (uint16_t) GOAL_STRLEN(strPw);
        } else
        {
            /* error */
            res = GOAL_ERR_NULL_POINTER;
        }
    }

    /* set rpc data */
    if (lenPw) {
        GOAL_RPC_PUSH_PTR(strPw, lenPw);
    }
    GOAL_RPC_PUSH(lenPw);
    if (lenUser) {
        GOAL_RPC_PUSH_PTR(strUser, lenUser);
    }
    GOAL_RPC_PUSH(lenUser);
    GOAL_RPC_PUSH(userLevel);

    /* call */
    GOAL_HTTP_RPC_CALL(pInst, GOAL_HTTP_FUNC_AUTHBAS_SET_INFO);

    /* close */
    GOAL_HTTP_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** This function creates a new template list for template replacement.
 *
 * @retval GOAL_OK on success
 * @retval other failed
 */
GOAL_STATUS_T goal_httpTmpMgrNewListAc(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_TEMPLATE_LIST_INIT_T *pInit       /**< pointer to list init data */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */
    uint32_t lenName = 0;                       /* list name length */

    /* get new rpc handle */
    GOAL_HTTP_RPC_NEW(pInst);

    /* get string length */
    lenName = (uint32_t) GOAL_STRLEN(pInit->listName);

    /* set rpc data */
    if (lenName) {
        GOAL_RPC_PUSH_PTR(pInit->listName, (uint16_t) lenName);
    }
    GOAL_RPC_PUSH(lenName);
    GOAL_RPC_PUSH(pInit->cntMemb);
    GOAL_RPC_PUSH(pInit->listId);

    /* call */
    GOAL_HTTP_RPC_CALL(pInst, GOAL_HTTP_FUNC_TEMPL_NEW_LIST);

    /* close */
    GOAL_HTTP_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** Registers a new resource at the HTTP server instance on the CC via CTC.
 *
 * @returns GOAL_OK on success
 * @returns error code otherwise
 *
 */
GOAL_STATUS_T goal_httpResRegAc(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    uint8_t *strUrl,                            /**< URL string */
    uint32_t allowMeth,                         /**< allowed methods for resource */
    GOAL_HTTP_REQCB_T pfnCbData,                /**< application callback */
    GOAL_HTTP_TMPCB_T pfnCbTemplate,            /**< template handler callback */
    GOAL_HTTP_HDL_T *pHdl                       /**< resource handle return */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_HTTP_CB_DATA_LL_T *pCbDataElem = NULL; /* data cb element pointer */
    GOAL_HTTP_CB_TEMPL_LL_T *pCbTemplElem = NULL; /* template cb element pointer */
    GOAL_BOOL_T flagFound = GOAL_FALSE;         /* search bool */
    uint32_t id = 0;                            /* temporary id value */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */
    uint32_t strLen = 0;                        /* string length */
    GOAL_HTTP_HDL_T retHdl = 0;                 /* return http handle */
    GOAL_HTTP_AC_RES_LL_T *pRes = NULL;         /* pointer to resource */

    /* if callback function pointer set, do the following */
    if (pfnCbData) {
        /* check if list is already in use */
        if (NULL == pInst->pCbDataList) {
            /* start data cb function list */
            res = goal_memCalloc(&pCbDataElem, sizeof(GOAL_HTTP_CB_DATA_LL_T));
            if (GOAL_RES_OK(res)) {
                /* set values */
                pCbDataElem->pfnCbData = pfnCbData;
                pCbDataElem->id = 1;
                /* set list pointer */
                pInst->pCbDataList = pCbDataElem;
            }
        } else {
            /* reset find flag */
            flagFound = GOAL_FALSE;
            GOAL_LL_FOREACH(pInst->pCbDataList, pCbDataElem) {
                /* update id for every element */
                id = pCbDataElem->id;
                /* search callback pointer */
                if (pCbDataElem->pfnCbData == pfnCbData) {
                    /* set found flag and leave search */
                    flagFound = GOAL_TRUE;
                    break;
                }
            }
            /* check find flag */
            if (GOAL_FALSE == flagFound) {
                /* alloc memory for new element */
                res = goal_memCalloc(&pCbDataElem, sizeof(GOAL_HTTP_CB_DATA_LL_T));
                if (GOAL_RES_OK(res)) {
                    /* set values */
                    pCbDataElem->pfnCbData = pfnCbData;
                    pCbDataElem->id = id + 1;
                    /* append to list */
                    GOAL_LL_APPEND(pInst->pCbDataList, pCbDataElem);
                }
            }
        }
    }

    /* if callback function pointer set, do the following */
    if (pfnCbTemplate) {
    /* check if list is already in use */
    if (NULL == pInst->pCbTemplList) {
        /* start data cb function list */
        res = goal_memCalloc(&pCbTemplElem, sizeof(GOAL_HTTP_CB_TEMPL_LL_T));
        if (GOAL_RES_OK(res)) {
            /* set values */
            pCbTemplElem->pfnCbTempl = pfnCbTemplate;
            pCbTemplElem->id = 1;
            /* set list pointer */
            pInst->pCbTemplList = pCbTemplElem;
        }
    } else {
            /* reset find flag */
            flagFound = GOAL_FALSE;
            GOAL_LL_FOREACH(pInst->pCbTemplList, pCbTemplElem) {
                /* update id for every element */
                id = pCbTemplElem->id;
                /* search callback pointer */
                if (pCbTemplElem->pfnCbTempl == pfnCbTemplate) {
                    /* set found flag and leave search */
                    flagFound = GOAL_TRUE;
                    break;
                }
            }
            /* check find flag */
            if (GOAL_FALSE == flagFound) {
                /* alloc memory for new element */
                res = goal_memCalloc(&pCbTemplElem, sizeof(GOAL_HTTP_CB_TEMPL_LL_T));
                if (GOAL_RES_OK(res)) {
                    /* set values */
                    pCbTemplElem->pfnCbTempl = pfnCbTemplate;
                    pCbTemplElem->id = id + 1;
                    /* append to list */
                    GOAL_LL_APPEND(pInst->pCbTemplList, pCbTemplElem);
                }
            }
        }
    }

    if (GOAL_RES_OK(res)) {
        /* set rpc handle */
        GOAL_HTTP_RPC_NEW(pInst);
    }

    /* get URL string length */
    strLen = (uint32_t) GOAL_STRLEN((char *) strUrl);
    if (0 == strLen) {
        /* error */
        res = GOAL_ERROR;
    } else {
        /* push URL string */
        GOAL_RPC_PUSH_PTR(strUrl, (uint16_t) strLen);
        /* push string length */
        GOAL_RPC_PUSH(strLen);
    }

    /* push allowed methods identifier */
    GOAL_RPC_PUSH(allowMeth);

    if (pfnCbData) {
        /* push data callback id */
        GOAL_RPC_PUSH(pCbDataElem->id);
    } else {
        GOAL_RPC_PUSH(0);
    }

    if (pfnCbTemplate) {
        /* push template callback id */
        GOAL_RPC_PUSH(pCbTemplElem->id);
    } else {
        GOAL_RPC_PUSH(0);
    }

    /* call function */
    GOAL_HTTP_RPC_CALL(pInst, GOAL_HTTP_FUNC_RES_REG);

    /* pull returned value */
    GOAL_RPC_POP(retHdl, GOAL_HTTP_HDL_T);
    GOAL_HTTP_RPC_CLOSE();

    if (GOAL_RES_OK(res)) {
        if (0 == retHdl) {
            /* error */
            res = GOAL_ERROR;
        } else {
            /* alloc memory for resource list element */
            res = goal_memCalloc(&pRes, sizeof(GOAL_HTTP_AC_RES_LL_T));
            if (GOAL_RES_OK(res)) {
                if (NULL == pInst->pResList) {
                    /* set values */
                    pRes->hdlRes = retHdl;
                    pRes->pfnCbData = pfnCbData;
                    pRes->pfnCbTempl = pfnCbTemplate;
                    /* start list */
                    pInst->pResList = pRes;
                } else {
                    /* set values */
                    pRes->hdlRes = retHdl;
                    pRes->pfnCbData = pfnCbData;
                    pRes->pfnCbTempl = pfnCbTemplate;
                    /* apped to list */
                    GOAL_LL_APPEND(pInst->pResList, pRes);
                }
            }
        }
    }

    if (GOAL_RES_OK(res)) {
        /* set resource handle return */
        *pHdl = retHdl;
    }

    return res;
}


/****************************************************************************/
/** Adds a new HTTP server instance on the AC and the CC via CTC.
 *
 * @returns GOAL_OK on success
 * @returns error code otherwise
 *
 */
GOAL_STATUS_T goal_httpNewAc(
    GOAL_HTTP_T **ppInst,                       /**< pointer to instance pointer */
    uint16_t port,                              /**< port to link instance to */
    uint16_t cntChn                             /**< channel count */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    uint32_t cntInst = 22;                      /* instance count */
    GOAL_HTTP_INSTANCE_CTC_T *pHttpCtc = NULL;  /* CTC instance */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */
    GOAL_HTTP_CHN_AC_LL_T *pChn = NULL;         /* channel pointer */
    uint32_t cnt;                               /* loop count */

    /* new http instance */
    res = goal_instNew((GOAL_INSTANCE_T **) &pHttpCtc, sizeof(GOAL_HTTP_INSTANCE_CTC_T), GOAL_ID_HTTP, cntInst,
                    "HTTP Instance AC");

    /* setup RPC channel for http data exchange */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcSetupChannel(&(pHttpCtc->pHdlRpcChn), GOAL_ID_MI_CTC_DEFAULT);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to setup RPC channel");
        } else {
            /* set handle variable */
            GOAL_HTTP_RPC_NEW(pHttpCtc);
        }
    }

    /* alloc memory for channel list */
    for (cnt = 0; cnt < cntChn; cnt++) {
        if (GOAL_RES_OK(res)) {
            res = goal_memCalloc(&pChn, sizeof(GOAL_HTTP_CHN_AC_LL_T));
            if (GOAL_RES_OK(res)) {
                if (NULL == pHttpCtc->pChn) {
                    /* start list */
                    pHttpCtc->pChn = pChn;
                } else {
                    /* apped to list */
                    GOAL_LL_APPEND(pHttpCtc->pChn, pChn);
                }
            }
        } else {
            break;
        }
    }

    /* alloc memory for CTC temp buffer */
    if (GOAL_RES_OK(res)) {
        res = goal_memCalloc(&(pHttpCtc->pCtcTemp), GOAL_HTTP_RPC_STACK_SIZE);
    }

    if (GOAL_RES_OK(res)) {
        /* push channel count and port */
        GOAL_RPC_PUSH(port);
        GOAL_RPC_PUSH(cntChn);
        /* call HTTP new on CC */
        GOAL_HTTP_RPC_CALL(pHttpCtc, GOAL_HTTP_FUNC_NEW);
        GOAL_HTTP_RPC_CLOSE();
    }

    /* info */
    if (GOAL_RES_OK(res)) {
        *ppInst = pHttpCtc;
        goal_logInfo("HTTP Application Core successfully started");
    }

    return res;
}


#if GOAL_CONFIG_HTTPS == 1
/****************************************************************************/
/** Adds a new secure HTTP server instance on the AC and the CC via CTC.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_httpsNewAc(
    GOAL_HTTP_T **ppInst,                       /**< pointer to instance pointer */
    uint16_t port,                              /**< port to link instance to */
    uint16_t cntChn                             /**< channel count */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    uint32_t cntInst = 22;                      /* instance count */
    GOAL_HTTP_INSTANCE_CTC_T *pHttpCtc = NULL;  /* CTC instance */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */
    GOAL_HTTP_CHN_AC_LL_T *pChn = NULL;         /* channel pointer */
    uint32_t cnt;                               /* loop count */

    /* new http instance */
    res = goal_instNew((GOAL_INSTANCE_T **) &pHttpCtc, sizeof(GOAL_HTTP_INSTANCE_CTC_T), GOAL_ID_HTTP, cntInst,
                    "HTTPS Instance AC");

    /* setup RPC channel for http data exchange */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcSetupChannel(&(pHttpCtc->pHdlRpcChn), GOAL_ID_MI_CTC_DEFAULT);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to setup RPC channel");
        } else {
            /* set handle variable */
            GOAL_HTTP_RPC_NEW(pHttpCtc);
        }
    }

    /* alloc memory for channel list */
    for (cnt = 0; cnt < cntChn; cnt++) {
        if (GOAL_RES_OK(res)) {
            res = goal_memCalloc(&pChn, sizeof(GOAL_HTTP_CHN_AC_LL_T));
            if (GOAL_RES_OK(res)) {
                if (NULL == pHttpCtc->pChn) {
                    /* start list */
                    pHttpCtc->pChn = pChn;
                } else {
                    /* apped to list */
                    GOAL_LL_APPEND(pHttpCtc->pChn, pChn);
                }
            }
        } else {
            break;
        }
    }

    /* alloc memory for CTC temp buffer */
    if (GOAL_RES_OK(res)) {
        res = goal_memCalloc(&(pHttpCtc->pCtcTemp), GOAL_HTTP_RPC_STACK_SIZE);
    }

    if (GOAL_RES_OK(res)) {
        /* push channel count and port */
        GOAL_RPC_PUSH(port);
        GOAL_RPC_PUSH(cntChn);
        /* call HTTP new on CC */
        GOAL_HTTP_RPC_CALL(pHttpCtc, GOAL_HTTP_FUNC_NEW_HTTPS);
        GOAL_HTTP_RPC_CLOSE();
    }

    /* info */
    if (GOAL_RES_OK(res)) {
        *ppInst = pHttpCtc;
        goal_logInfo("HTTPS Application Core successfully started");
    }

    return res;
}
#endif /* GOAL_CONFIG_HTTPS == 1 */


/****************************************************************************/
/** Initialize HTTP RPC
 *
 * @retval GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_httpRpcInit(
    void
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    unsigned int cnt;                           /* counter */
    GOAL_RPC_HDL_CHN_T *pHdlRpcChn = GOAL_RPC_HDL_NONE; /* acyclic transmission handle */

    /* create the HTTP CTC instance list */
    res = goal_instListNew(&pListCtcInst, GOAL_ID_HTTP);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create HTTP CTC instance list");
    }

    /* register RPC functions */
    for (cnt = 0; cnt < ARRAY_ELEMENTS(goal_httpTblFunc); cnt++) {
        res = goal_rpcRegisterService(GOAL_ID_HTTP, (GOAL_RPC_FUNC_ID) goal_httpTblFunc[cnt].id, NULL);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to register HTTP RPC function nr %u", goal_httpTblFunc[cnt].id);
            return res;
        }
    }

    /* register RPC CB functions */
    for (cnt = 0; cnt < ARRAY_ELEMENTS(goal_httpTblCbFunc); cnt++) {
        res = goal_rpcRegisterService(GOAL_ID_HTTP, (GOAL_RPC_FUNC_ID) goal_httpTblCbFunc[cnt].id, goal_httpTblCbFunc[cnt].pFunc);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to register HTTP RPC function nr %u", goal_httpTblCbFunc[cnt].id);
            return res;
        }
    }

    /* setup a channel for RPC */
    res = goal_rpcSetupChannel(&pHdlRpcChn, GOAL_ID_MI_CTC_DEFAULT);

    return res;
}


/****************************************************************************/
/** Initializes the HTTP server on the AC.
 *
 * @returns GOAL_OK on success
 * @returns error code otherwise
 *
 */
GOAL_STATUS_T goal_httpInitAc(
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

    /* add stage handler to register HTTP RPC functions */
    res = goal_mainStageReg(GOAL_STAGE_MODULES, &mStageMod, GOAL_STAGE_INIT, goal_httpRpcInit);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to register HTTP RPC init in modules stage");
    }

    return res;
}


