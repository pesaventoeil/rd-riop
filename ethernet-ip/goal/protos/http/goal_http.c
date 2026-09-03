/** @file
 *
 * @brief Implementation of the HTTP protocol
 *
 * Implementation of the HTTP protocol with basic features.
 *
 * @copyright
 * Copyright 2010-2017 port GmbH Halle/Saale.
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
#define GOAL_ID GOAL_ID_HTTP
#include "goal_includes.h"
#include "goal_http.h"
#include "goal_http_resmgr.h"
#include "goal_http_parser.h"
#include "goal_http_tmpmgr.h"
#include "cm/goal_http_cm.h"
#if GOAL_CONFIG_HTTPS == 1
#  include "goal_https.h"
#  include "cm/goal_https_cm.h"
#endif
#include "goal_http_idlist.h"


/****************************************************************************/
/* Local defines */
/****************************************************************************/
#define GOAL_HTTP_SEND_RETRY_MAX            200         /* maxmimum number of sending retries */


/****************************************************************************/
/* Local variables */
/****************************************************************************/

#undef GOAL_HTTP_NAME
#define GOAL_HTTP_NAME(name, nr, str) {name, str, GOAL_HTTP_STRLEN(str)}

#undef GOAL_HTTP_EXT_NAME
#define GOAL_HTTP_EXT_NAME(name, nr, str, cont) {name, str, GOAL_HTTP_STRLEN(str), cont}

static struct goal_httpTableEntry goal_httpStatusTable[] = {GOAL_HTTP_STATUS}; /**< table of possible http status */
static struct goal_httpTableEntry goal_httpContentTable[] = {GOAL_HTTP_CONTENT}; /**< table of possible http content types */
static struct goal_httpTableEntry goal_httpMethodTable[] = {GOAL_HTTP_METHOD}; /**< table of possible http methods (GET, POST, etc.) */
static struct goal_httpExtTableEntry goal_httpFileExtTable[] = {GOAL_HTTP_FILE_EXT}; /**< table of possible file extensions */


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/
#if GOAL_CONFIG_GEN_CTC_CC == 1
static GOAL_STATUS_T goal_httpSendNextCtcFrag(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan                  /**< channel pointer */
);
#endif

static GOAL_STATUS_T goal_httpBufRelease(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_BUFFER_T **ppBuf                       /**< pointer to released buffer reference */
);

static GOAL_STATUS_T goal_httpTransDone(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_STATUS_T state                         /**< goal status */
);

static GOAL_STATUS_T goal_httpInstanceToChannelAssign(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_NET_CHAN_T *pChan                      /**< net channel */
);

static GOAL_STATUS_T goal_httpChannelGet(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_NET_CHAN_T *pChan,                     /**< net channel */
    GOAL_HTTP_CHN_T **ppHttpChan                /**< channel pointer */
);

static GOAL_STATUS_T httpNetCb(
    void *pArg,                                 /**< callback argument */
    GOAL_NET_CB_TYPE_T cbType,                  /**< callback type */
    GOAL_NET_CHAN_T *pChan,                     /**< channel descriptor */
    struct GOAL_BUFFER_T *pBuf                  /**< GOAL buffer */
);

static GOAL_STATUS_T goal_httpProcessReq(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    struct GOAL_BUFFER_T *pBuf                  /**< GOAL buffer */
);

static GOAL_STATUS_T goal_httpCheckRet(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_STATUS_T retParser,                    /**< return from parser */
    GOAL_HTTP_APPLRET_T *pApplRet               /**< pointer to application return */
);

static GOAL_STATUS_T goal_httpSendRes(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_HTTP_APPLRET_T *pApplRet               /**< pointer to application return */
);

static GOAL_STATUS_T goal_httpAddHeader(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_BUFFER_T *pBuf,                        /**< pointer to buffer */
    GOAL_HTTP_APPLRET_T *pApplRet               /**< pointer to application info */
);

static GOAL_STATUS_T goal_httpSendFrag(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_BUFFER_T **ppBuf                       /**< pointer to buffer reference */
);

static GOAL_STATUS_T goal_httpAllocResBuf(
    GOAL_HTTP_T     *pInst,                     /**< instance pointer */
    GOAL_BUFFER_T   **ppBuf                     /**< pointer to buffer pointer */
);

static GOAL_STATUS_T goal_httpForwardReq(
    GOAL_HTTP_T             *pInst,             /**< instance pointer */
    GOAL_HTTP_PARSE_T      *pPrsInfo,           /**< pointer to parser info */
    GOAL_HTTP_APPLRET_T    *pApplRet            /**< application return pointer */
);

static GOAL_STATUS_T goal_httpChnSetResBuf(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_BUFFER_T *pBuf                         /**< buffer pointer */
);

static GOAL_STATUS_T goal_httpChnSetRdySend(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_BOOL_T flag                            /**< ready to send flag */
);

static GOAL_STATUS_T goal_httpChnGetRdySend(
    GOAL_HTTP_T *pInst,                         /**< [in] instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< [in] channel pointer */
    GOAL_BOOL_T *pFlag                          /**< [out] ready to send pointer */
);

static GOAL_STATUS_T goal_httpChnResetPend(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan                  /**< channel pointer */
);

static GOAL_STATUS_T goal_httpChnGetPend(
    GOAL_HTTP_T *pInst,                         /**< [in] instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< [in] channel pointer */
    GOAL_BOOL_T *pFlag                          /**< [out] pend flag pointer */
);

static GOAL_STATUS_T goal_httpChnClear(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan                  /**< channel pointer */
);

static GOAL_STATUS_T goal_httpChnChkWind(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_HTTP_PARSE_T *pInfoPrs                 /**< pointer to parser info */
);

static GOAL_STATUS_T goal_httpChnCpyChnk(
    GOAL_HTTP_T     *pInst,                     /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_BUFFER_T   *pBuf,                      /**< pointer to buffer from net module */
    uint8_t         **pData,                    /**< return pointer for buffered data */
    uint32_t        *pLenData                   /**< return pointer for buffered data length */
);

static GOAL_STATUS_T goal_httpChnGetResInfo(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_HTTP_RESFRAG_T *pResFrag,              /**< pointer to fragmentation info */
    GOAL_HTTP_CTC_FRAG_T *pCtcFrag              /**< pointer to CTC fragmentation info */
);

static GOAL_STATUS_T goal_httpChnGetResType(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_HTTP_RESTYP_T *pResType,               /**< pointer to response type, optional */
    GOAL_HTTP_STATUS_T *pHttpStatus             /**< pointer to http channel status, optional */
);

static GOAL_STATUS_T goal_httpChnSetResType(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_HTTP_RESTYP_T resType,                 /**< response type */
    GOAL_HTTP_STATUS_T httpStatus               /**< http status of channel */
);

static GOAL_STATUS_T goal_httpFwAppl(
    GOAL_HTTP_T         *pInst,                 /**< instance pointer */
    GOAL_HTTP_PARSE_T  *pPrs,                   /**< parser info */
    GOAL_HTTP_REQ_T    *pReq,                   /**< pointer to request */
    const uint8_t       **pBuf,                 /**< address of pointer to return buffer */
    uint32_t            *pLen,                  /**< return length */
    GOAL_HTTP_STATUS_T *pState,                 /**< return status */
    GOAL_HTTP_CONTENT_T *pContent               /**< return content type */
);

static GOAL_STATUS_T goal_httpFwApplOnePost(
    GOAL_HTTP_T        *pInst,                  /**< instance pointer */
    GOAL_HTTP_PARSE_T  *pPrs,                   /**< parser info */
    const uint8_t       **pBuf,                 /**< address of pointer to return buffer */
    uint32_t            *pLen,                  /**< return length */
    GOAL_HTTP_STATUS_T *pState,                 /**< return status */
    GOAL_HTTP_CONTENT_T *pContent,              /**< return content type */
    GOAL_HTTP_APPLCB_DATA_T *pCbInfo            /**< pointer to callback info */
);

static GOAL_STATUS_T goal_httpGetUrlStr(
    GOAL_HTTP_PARSE_T  *pPrs,                   /**< parser info */
    char               **ppUrl,                 /**< URL string */
    uint32_t           *pLenUrl,                /**< URL length */
    uint32_t           *pLenMatch               /**< match length */
);

static GOAL_STATUS_T http_searchString(
    char *pMes,                                 /**< message string */
    char *strSearch,                            /**< string to look for */
    uint32_t *pStart,                           /**< start character */
    uint32_t *pEnd                              /**< end character */
);

static void goal_httpMainLoop(
    void *pArg                                  /**< instance handle */
);


/****************************************************************************/
/** This function searches a string within a given buffer.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 *
 * The string is searched within the given start and end border values.
 * When the function returns GOAL_OK, the string has been found and
 * start and end variables contain the offset of the string in the message.
 */
static GOAL_STATUS_T http_searchString(
    char *pMes,                                 /**< message string */
    char *strSearch,                            /**< string to look for */
    uint32_t *pStart,                           /**< start character */
    uint32_t *pEnd                              /**< end character */
)
{
    GOAL_STATUS_T ret = GOAL_ERROR;             /* return value */
    uint32_t lenSearch;                         /* search string length */
    uint32_t cnt = 0;                           /* loop counter */

    /* check input params */
    if ((NULL == pStart)
    ||  (NULL == pEnd)
    ||  (NULL == pMes)
    ||  (NULL == strSearch)) {
        return GOAL_ERR_NULL_POINTER;
    } else if (*pStart >= *pEnd) {
        return GOAL_ERROR;
    }

    /* get search string length */
    lenSearch = (uint32_t) GOAL_STRLEN(strSearch);

    for (cnt = *pStart; (cnt < *pEnd) && (GOAL_OK != ret); cnt++) {
        /* check first character */
        if (strSearch[0] == pMes[cnt]) {
            if (0 == GOAL_MEMCMP(&pMes[cnt], strSearch, lenSearch)) {
                /* set border values */
                *pStart = cnt;
                *pEnd = cnt + lenSearch;
                /* set return value */
                ret = GOAL_OK;
            }
        }
    }
    return ret;
}


#if (GOAL_CONFIG_GEN_CTC_CC == 1)
/****************************************************************************/
/** Response buffer release callback function
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_httpSendNextCtcFrag(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan                  /**< channel pointer */
)
{
    GOAL_STATUS_T ret = GOAL_OK;                /* return value */
    const uint8_t *pData = NULL;                /* data pointer */
    uint32_t lenData = 0;                       /* data length */
    GOAL_BUFFER_T *pNew = NULL;                 /* new buffer pointer */

    if (GOAL_RES_OK(ret)) {
        /* use callback function */
        ret = goal_httpDataContinueCc(pInst, pHttpChan, &pData, &lenData);
    }

    if (GOAL_RES_OK(ret)) {
        /* get new buffer */
        ret = goal_httpAllocResBuf(pInst, &pNew);
    }

    if (GOAL_RES_OK(ret)) {
        /* set buffer for this channel */
        ret = goal_httpChnSetResBuf(pInst, pHttpChan, pNew);
    }

    if (GOAL_RES_OK(ret)) {
        /* add next fragment to buffer */
        ret = goal_httpAddToBuffer(pInst, pNew, pHttpChan, pData, lenData);
    }

    /* release buffer if any error occurred */
    if (GOAL_RES_ERR(ret) && (NULL != pNew)) {
        goal_queueReleaseBuf(&pNew);
    }

    if (GOAL_RES_OK(ret)) {
        /* set ready to send flag */
        ret = goal_httpChnSetRdySend(pInst, pHttpChan, GOAL_TRUE);
    }

    return ret;
}
#endif


/****************************************************************************/
/** Response buffer release callback function
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_httpBufRelease(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_BUFFER_T **ppBuf                       /**< pointer to released buffer reference */
)
{
    GOAL_STATUS_T ret = GOAL_ERROR;             /* return value */
    GOAL_HTTP_RESFRAG_T resFrag;                /* response fragmentation info */
    GOAL_HTTP_CTC_FRAG_T ctcFrag;               /* CTC fragmentation info */
    GOAL_BUFFER_T *pNew = NULL;                 /* buffer pointer */
    GOAL_BOOL_T rdySnd = GOAL_FALSE;            /* ready to send flag */
    GOAL_BOOL_T pend = GOAL_FALSE;              /* send buffer pending flag */
    GOAL_NET_CHAN_T *pChan;                     /* net channel */

    /* check pointer */
    if ((NULL == pInst) || (NULL == pHttpChan) || (NULL == ppBuf)) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* get fragmentation info */
    ret = goal_httpChnGetResInfo(pInst, pHttpChan, &resFrag, &ctcFrag);
    if (GOAL_RES_ERR(ret)) {
        return ret;
    }

    /* get ready to send flag */
    ret = goal_httpChnGetRdySend(pInst, pHttpChan, &rdySnd);
    if (GOAL_RES_ERR(ret)) {
        return ret;
    }

    /* get send buffer pending flag */
    ret = goal_httpChnGetPend(pInst, pHttpChan, &pend);
    if (GOAL_RES_ERR(ret)) {
        return ret;
    }

    /* check ready to send */
    if (GOAL_TRUE == rdySnd) {
        /* check for valid buffer */
        if (NULL == *ppBuf) {
            return GOAL_ERR_NULL_POINTER;
        }

        /* send */
        ret = goal_httpSendFrag(pInst, pHttpChan, ppBuf);
        /* return if sending is waiting for next loop iteration or failed */
        if ((GOAL_RES_ERR(ret)) || (GOAL_OK_DELAYED == ret)) {
            return ret;
        }

        /* release buffer */
        ret = goal_queueReleaseBuf(ppBuf);
        if (GOAL_RES_ERR(ret)) {
            goal_logErr("Buffer could not be released");
        }

        /* reset ready to send flag */
        ret = goal_httpChnSetRdySend(pInst, pHttpChan, GOAL_FALSE);
        if (GOAL_RES_ERR(ret)) {
            return ret;
        }

        /* reset pending flag */
        goal_httpChnResetPend(pInst, pHttpChan);
        if (GOAL_RES_ERR(ret)) {
            return ret;
        }

        if (GOAL_FALSE == goal_httpChnState_get(pInst, pHttpChan, GOAL_HTTP_CHNFLAG_CONTINUE)) {
            /* close channel after sending */
            goal_httpChnState_set(pInst, pHttpChan, GOAL_HTTP_CHNFLAG_CLOSCHN);
        }

    /* check fragmentation info */
    } else if ((GOAL_TRUE == resFrag.flag) && (GOAL_FALSE == pend)) {
        /* get new buffer */
        ret = goal_httpAllocResBuf(pInst, &pNew);
        if (GOAL_RES_ERR(ret)) {
            return ret;
        }

        if (GOAL_RES_OK(ret)) {
            /* set response buffer for this channel */
            ret = goal_httpChnSetResBuf(pInst, pHttpChan, pNew);

            /* release buffer if any error occurred */
            if (GOAL_RES_ERR(ret)) {
                goal_queueReleaseBuf(&pNew);
            }
        }

        if (GOAL_RES_OK(ret)) {
            /* add next fragment to buffer */
            ret = goal_httpAddToBuffer(pInst, pNew, pHttpChan,
                                    (const uint8_t *) (resFrag.pData + resFrag.srcOff),
                                    (resFrag.lenData - resFrag.srcOff));

            /* release buffer if any error occurred */
            if (GOAL_RES_ERR(ret)) {
                goal_queueReleaseBuf(&pNew);
            }
        }

        if (GOAL_RES_OK(ret)) {
            /* set ready to send flag */
            ret = goal_httpChnSetRdySend(pInst, pHttpChan, GOAL_TRUE);
        }
#if (GOAL_CONFIG_GEN_CTC_CC == 1)
    } else if ((GOAL_TRUE == ctcFrag.getFrag) && (GOAL_FALSE == pend)) {
        /* get and send next data fragment */
        ret = goal_httpSendNextCtcFrag(pInst, pHttpChan);
#endif
    } else {
        if (GOAL_TRUE == goal_httpChnState_get(pInst, pHttpChan, GOAL_HTTP_CHNFLAG_CLOSCHN)) {
#if (GOAL_CONFIG_GEN_CTC_CC == 1)
            /* inform AC to close channel */
            goal_httpChnCloseCc(pInst, pHttpChan);
#endif

            /* inform application that transaction is done */
            ret = goal_httpTransDone(pInst, pHttpChan, GOAL_OK);
            /* clear channel */
            pChan = pHttpChan->pChan;
            ret = goal_httpChnClear(pInst, pHttpChan);

            /* no fragments to send, close channel */
            goal_netChanClose(pChan);
        }
    }
    /* leave loop */
    return ret;
}


/****************************************************************************/
/** Informs application via callback that transmission is done
 *
 * @retval GOAL_OK successful
 *
 */
static GOAL_STATUS_T goal_httpTransDone(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_STATUS_T state                         /**< goal status */
)
{
    GOAL_STATUS_T ret = GOAL_OK;                /* return value */
    GOAL_HTTP_REQFRAG_T reqFrag;                /* request fragmentation info */
    GOAL_HTTP_REQCB_T   pfnApplCb;              /* application callback */
    GOAL_HTTP_APPLCB_DATA_T infoApplCb;         /* application callback info */
    uint32_t id = 0;                            /* channel ID */

    /* init struct */
    GOAL_MEMSET(&infoApplCb, 0, sizeof(infoApplCb));

    /* get channel request info */
    ret = goal_httpChnGetReqInfo(pInst, pHttpChan, &reqFrag);
    if (GOAL_RES_ERR(ret)) {
        return ret;
    }

    /* set callback function pointer */
    pfnApplCb = (GOAL_HTTP_REQCB_T) (reqFrag.pfnAppCb);
    if (pfnApplCb) {
        if (GOAL_RES_OK(state)) {
            /* state is OK */
            infoApplCb.reqType = GOAL_HTTP_FW_REQ_DONE_OK;
        } else {
            /* state is not ok */
            infoApplCb.reqType = GOAL_HTTP_FW_REQ_DONE_ERR;
        }
        /* set application callback info struct */
        infoApplCb.pInst = pInst;
        infoApplCb.hdlRes = reqFrag.hdlRes;

        ret = goal_httpChannelId(pInst, pHttpChan, &id);
        if (GOAL_RES_OK(ret)) {
            infoApplCb.hdlTransm = (GOAL_HTTP_HDL_T) (PtrCast) id;

            /* use callback */
            ret = (*pfnApplCb)(&infoApplCb);
        }
    }
    return ret;
}


/****************************************************************************/
/** Assign a TCP data channel to a free HTTP channel
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
static GOAL_STATUS_T goal_httpInstanceToChannelAssign(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_NET_CHAN_T *pChan                      /**< net channel */
)
{
    uint32_t cntChan;                           /* channel loop counter */

    /* search free channel */
    for (cntChan = 0; cntChan < pInst->cntChn; cntChan++) {
        if (NULL == pInst->pChanHdl[cntChan].pChan) {
            /* found free channel */
            pInst->pChanHdl[cntChan].pChan = pChan;
            break;
        }
    }

    return (cntChan >= pInst->cntChn) ? GOAL_ERR_NOT_FOUND : GOAL_OK;
}


/****************************************************************************/
/** Get a HTTP channel that has a given TCP data channel
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
static GOAL_STATUS_T goal_httpChannelGet(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_NET_CHAN_T *pChan,                     /**< net channel */
    GOAL_HTTP_CHN_T **ppHttpChan                /**< channel pointer */
)
{
    uint32_t cntChan;                           /* channel loop counter */

    for (cntChan = 0; cntChan < pInst->cntChn; cntChan++) {
        if (pChan == pInst->pChanHdl[cntChan].pChan) {
            /* found matching channel */
            *ppHttpChan = &(pInst->pChanHdl[cntChan]);
            break;
        }
    }

    return (cntChan >= pInst->cntChn) ? GOAL_ERR_NOT_FOUND : GOAL_OK;
}


/****************************************************************************/
/** HTTP Callback for net module
 *
 * Sends received data to http module, which handles the incoming request.
 *
 */
static GOAL_STATUS_T httpNetCb(
    void *pArg,                                 /**< callback argument */
    GOAL_NET_CB_TYPE_T cbType,                  /**< callback type */
    GOAL_NET_CHAN_T *pChan,                     /**< channel descriptor */
    struct GOAL_BUFFER_T *pBuf                  /**< GOAL buffer */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_HTTP_T *pInstance;                     /* instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan;                 /* HTTP channel */

    pInstance = (GOAL_HTTP_T *) pArg;

    if (GOAL_NET_CB_NEW_SOCKET == cbType) {
        /* channel opened */
        goal_logDbg("HTTP connection opened");
        /* activate channel */
        res = goal_netChanActivate(pChan);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("error while activating TCP channel");
        }

        if (GOAL_RES_OK(res)) {
            /* assign GOAL channel to HTTP channel */
            res = goal_httpInstanceToChannelAssign(pInstance, pChan);
            if (GOAL_RES_ERR(res)) {
                goal_logErr("failed to assign new TCP channel to HTTP channel");
            }
        }

        return res;
    }
    else if (GOAL_NET_CB_NEW_DATA == cbType) {
        /* data received */
        goal_logDbg("HTTP request received");

        res = goal_httpChannelGet(pInstance, pChan, &pHttpChan);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("Failed to find HTTP channel");
        }
        else {
            /* process message */
            res = goal_httpProcessReq(pHttpChan->pInst, pHttpChan, pBuf);
            if (GOAL_RES_ERR(res)) {
                goal_logErr("Failed to process message");
            }
        }
    }
    else if (GOAL_NET_CB_CLOSING == cbType) {
        /* channel closed */
        goal_logDbg("HTTP connection closed");

        res = goal_httpChannelGet(pInstance, pChan, &pHttpChan);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("Failed to find HTTP channel");
        }
        else {
            /* set close channel flag (application callback will be used via main) */
            goal_httpChnState_set(pHttpChan->pInst, pHttpChan, GOAL_HTTP_CHNFLAG_CLOSCHN);
        }
    }

    return GOAL_OK;
}


/****************************************************************************/
/** This functions processes a received request message from http client.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_httpProcessReq(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    struct GOAL_BUFFER_T *pBuf                  /**< GOAL buffer */
)
{
    GOAL_STATUS_T ret = GOAL_ERROR;             /* return value */
    GOAL_HTTP_PARSE_T      infoPrs;             /* parser information struct */
    GOAL_HTTP_APPLRET_T    applRet;             /* application return */
    uint8_t     *pDataIn = NULL;                /* incoming data after chunk handling */
    uint32_t    lenDataIn = 0;                  /* incoming data length after chunk handling */
    GOAL_NET_CHAN_T *pChan;                     /* net Channel */

    GOAL_MEMSET(&applRet, 0, sizeof(applRet));
    GOAL_MEMSET(&infoPrs, 0, sizeof(infoPrs));

    /* check instance pointer */
    if ((NULL == pInst) || (NULL == pHttpChan) || (NULL == pBuf)) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* copy data chunk to channel buffer */
    ret = goal_httpChnCpyChnk(pInst, pHttpChan, pBuf, &pDataIn, &lenDataIn);
    if (GOAL_RES_ERR(ret)) {
        goal_logErr("Data chunk buffering failed");
        return ret;
    }

    /* check for parsing */
    if (GOAL_TRUE == goal_httpChnState_get(pInst, pHttpChan, GOAL_HTTP_CHNFLAG_PARSE)) {
        /* set parser input information */
        GOAL_MEMSET(&infoPrs, 0, sizeof(GOAL_HTTP_PARSE_T));
        infoPrs.in.pHttpChan = pHttpChan;
        infoPrs.in.strMes = pDataIn;
        infoPrs.in.lenMes = lenDataIn;
        infoPrs.in.pMethTable = &goal_httpMethodTable[0];
        infoPrs.in.lenMethTable = ARRAY_ELEMENTS(goal_httpMethodTable);
        infoPrs.in.pFileExtTable = &goal_httpFileExtTable[0];
        infoPrs.in.lenFileExtTable = ARRAY_ELEMENTS(goal_httpFileExtTable);

        /* parse message */
        ret = goal_httpParse(pInst, &infoPrs);
        if (GOAL_RES_ERR(ret)) {
            /* check parser return value */
            goal_httpCheckRet(pInst, pHttpChan, ret, &applRet);
            if (GOAL_HTTP_STATUS_BAD_REQ_11 == applRet.retState) {
                /* error */
                goal_logErr("Http parser returned error");
            }
        }
        /* check for forwarding */
        if (GOAL_FALSE == goal_httpChnState_get(pInst, pHttpChan, GOAL_HTTP_CHNFLAG_FORWARD)) {
            /* wind chunk buffer forward */
            goal_httpChnChkWind(pInst, pHttpChan, &infoPrs);
        }
    }

    /* check for forwarding */
    if (GOAL_TRUE == goal_httpChnState_get(pInst, pHttpChan, GOAL_HTTP_CHNFLAG_FORWARD)) {
        /* forward request to application and get/put user data */
        ret = goal_httpForwardReq(pInst, &infoPrs, &applRet);
        if (GOAL_RES_ERR(ret)) {
            goal_logErr("Http request forwarding to application failed");
            goal_httpChnState_set(pInst, pHttpChan, GOAL_HTTP_CHNFLAG_CLOSCHN);
        }
        /* wind receive chunk buffer forward */
        goal_httpChnChkWind(pInst, pHttpChan, &infoPrs);
    }

    /* check for sending response */
    if (GOAL_TRUE == goal_httpChnState_get(pInst, pHttpChan, GOAL_HTTP_CHNFLAG_SENDRES)) {
        /* send response */
        ret = goal_httpSendRes(pInst, pHttpChan, &applRet);
        if (GOAL_RES_ERR(ret)) {
            goal_logErr("Http response sending failed");
            goal_httpChnState_set(pInst, pHttpChan, GOAL_HTTP_CHNFLAG_CLOSCHN);
        }
    }

    /* check for channel closing */
    if (GOAL_RES_ERR(ret)) {
        pChan = pHttpChan->pChan;
        /* clear channel */
        goal_httpChnClear(pInst, pHttpChan);
        /* close channel */
        goal_netChanClose(pChan);
    }

    return ret;
}


/****************************************************************************/
/** This function prepares and sends a reponse message.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_httpCheckRet(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_STATUS_T retParser,                    /**< return from parser */
    GOAL_HTTP_APPLRET_T *pApplRet               /**< pointer to application return */
)
{
    GOAL_STATUS_T ret = GOAL_OK;                /* return value */

    /* set application return */
    pApplRet->lenData = 0;
    pApplRet->retContent = GOAL_HTTP_CONTENT_UNK;

    /* set return state for http response */
    switch (retParser) {
    case GOAL_ERR_ACCESS:
        pApplRet->retState = GOAL_HTTP_STATUS_UNAUTH_11;
        break;

    case GOAL_ERR_UNSUPPORTED:
        pApplRet->retState = GOAL_HTTP_STATUS_FORBID_11;
        break;

    case GOAL_ERR_NODATA:
        pApplRet->retState = GOAL_HTTP_STATUS_NOT_FOUND_11;
        break;

    case GOAL_ERR_DELAYED:
        pApplRet->retState = GOAL_HTTP_STATUS_CONTINUE_11;
        break;

    default:
        pApplRet->retState = GOAL_HTTP_STATUS_BAD_REQ_11;
        break;
    }

    /* allow sending */
    goal_httpChnState_set(pInst, pHttpChan, GOAL_HTTP_CHNFLAG_SENDRES);

    return ret;
}


/****************************************************************************/
/** This function prepares and sends a response message.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_httpSendRes(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_HTTP_APPLRET_T *pApplRet               /**< pointer to application return */
)
{
    GOAL_STATUS_T ret = GOAL_ERROR;             /* return value */
    GOAL_BUFFER_T *pBuf = NULL;                 /* pointer to response buffer */

    /* get buffer from net module */
    ret = goal_httpAllocResBuf(pInst, &pBuf);

    if (GOAL_RES_OK(ret)) {
        /* set response buffer for this channel */
        ret = goal_httpChnSetResBuf(pInst, pHttpChan, pBuf);
    }

    if (GOAL_RES_OK(ret)) {
        /* add header info */
        ret = goal_httpAddHeader(pInst, pHttpChan, pBuf, pApplRet);
    }

    /* add user data */
    if (GOAL_RES_OK(ret) && (0 != pApplRet->lenData) && (NULL != pApplRet->pData)) {
        if ((GOAL_HTTP_CONTENT_HTML == pApplRet->retContent) ||
            (GOAL_HTTP_CONTENT_JS == pApplRet->retContent)) {
            /* replace templates for html and java script */
            ret = goal_httpTmpMgrParse(pInst, pHttpChan, pBuf, pApplRet->hdlRes, pApplRet->pData, pApplRet->lenData, 0);
        } else {
            /* set buffer without template parsing */
            ret = goal_httpAddToBuffer(pInst, pBuf, pHttpChan, pApplRet->pData, pApplRet->lenData);
        }
    }

    /* release buffer if any error occurred */
    if (GOAL_RES_ERR(ret) && (NULL != pBuf)) {
        goal_queueReleaseBuf(&pBuf);
    }

    if (GOAL_RES_OK(ret)) {
        /* set ready to send flag */
        ret = goal_httpChnSetRdySend(pInst, pHttpChan, GOAL_TRUE);
    }

    if (GOAL_HTTP_STATUS_CONTINUE_11 == pApplRet->retState) {
        /* in case of sending 101 continue response, leave channel open */
        goal_httpChnState_set(pInst, pHttpChan, GOAL_HTTP_CHNFLAG_CONTINUE);
    }
    else {
        /* close channel after sending */
        goal_httpChnState_reset(pInst, pHttpChan, GOAL_HTTP_CHNFLAG_CONTINUE);
    }

    return ret;
}


/****************************************************************************/
/** This adds http header information to a net buffer.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_httpAddHeader(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_BUFFER_T *pBuf,                        /**< pointer to buffer */
    GOAL_HTTP_APPLRET_T *pApplRet               /**< pointer to application info */
)
{
    GOAL_STATUS_T ret = GOAL_OK;                /* return value */

    /* add http status header */
    if (GOAL_RES_OK(ret)) {
        ret = goal_httpAddToBuffer(pInst, pBuf, pHttpChan,
                                   (const uint8_t *) (goal_httpStatusTable[pApplRet->retState].strMes),
                                   goal_httpStatusTable[pApplRet->retState].lenMes);
    }

    /* add sub-header */
    if (GOAL_HTTP_STATUS_UNAUTH_11 == pApplRet->retState) {
        if (GOAL_RES_OK(ret)) {
            ret = goal_httpAddToBuffer(pInst, pBuf, pHttpChan,
                                       (const uint8_t *) (goal_httpStatusTable[GOAL_HTTP_STATUS_AUTH_CHALLENGE].strMes),
                                       goal_httpStatusTable[GOAL_HTTP_STATUS_AUTH_CHALLENGE].lenMes);
        }
    }

    /* add http content type */
    if ((0 != pApplRet->lenData) && (NULL != pApplRet->pData)) {
        if ((GOAL_RES_OK(ret)) && (GOAL_HTTP_CONTENT_UNK != pApplRet->retContent)) {
            /* get content type from request */
            ret = goal_httpAddToBuffer(pInst, pBuf, pHttpChan,
                                       (const uint8_t *) (goal_httpContentTable[pApplRet->retContent].strMes),
                                       goal_httpContentTable[pApplRet->retContent].lenMes);
        }
    }

    /* add http connection close */
    if (GOAL_RES_OK(ret)) {
        ret = goal_httpAddToBuffer(pInst, pBuf, pHttpChan,
                                   (const uint8_t *) (goal_httpStatusTable[GOAL_HTTP_STATUS_CONN_CLOSE].strMes),
                                   goal_httpStatusTable[GOAL_HTTP_STATUS_CONN_CLOSE].lenMes);
    }

    /*  add header end */
    if (GOAL_RES_OK(ret)) {
        ret = goal_httpAddToBuffer(pInst, pBuf, pHttpChan,
                                   (const uint8_t *) GOAL_HTTP_HDREND_STR,
                                   (uint32_t) GOAL_STRLEN((const char *) GOAL_HTTP_HDREND_STR));
    }

    return ret;
}


/****************************************************************************/
/** This function gets a buffer from net module.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_httpAllocResBuf(
    GOAL_HTTP_T     *pInst,                     /**< instance pointer */
    GOAL_BUFFER_T   **ppBuf                     /**< pointer to buffer pointer */
)
{
    GOAL_STATUS_T ret = GOAL_ERROR;             /* return value */
    uint16_t retry = 0;                         /* number of retries */

    UNUSEDARG(pInst);

    for (retry = 0; retry < 5; retry++) {
        /* get buffer from net module */
        ret = goal_ethGetNetBuf(ppBuf);
        if (GOAL_RES_OK(ret)) {
            /* leave loop */
            break;
        }
    }

    if (GOAL_RES_ERR(ret)) {
        /* error */
        goal_logErr("No response buffer available");
    }

    return ret;
}


/****************************************************************************/
/** This function forwards a client request to the user application.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_httpForwardReq(
    GOAL_HTTP_T             *pInst,             /**< instance pointer */
    GOAL_HTTP_PARSE_T       *pPrsInfo,          /**< pointer to parser info */
    GOAL_HTTP_APPLRET_T     *pApplRet           /**< application return pointer */
)
{
    GOAL_STATUS_T   ret = GOAL_ERROR;           /* return value */
    GOAL_HTTP_REQ_T        request;             /* request info */
    GOAL_HTTP_RESTYP_T     resType;             /* response type */

    /* fill request data */
    request.method = pPrsInfo->out.method;
    request.fileExt = pPrsInfo->out.fileExt;
    request.hdlRes = pPrsInfo->out.hdlRes;

    /* forward request to application */
    ret = goal_httpFwAppl(pInst, pPrsInfo, &request, &(pApplRet->pData),
                          &(pApplRet->lenData), &(pApplRet->retState),
                          &(pApplRet->retContent));
    if (GOAL_RES_ERR(ret)) {
        return ret;
    }

    /* set resource handle */
    pApplRet->hdlRes = pPrsInfo->out.hdlRes;

    /* get response type */
    ret = goal_httpChnGetResType(pInst, pPrsInfo->in.pHttpChan, &resType, NULL);
    if (GOAL_RES_ERR(ret)) {
        return ret;
    }

    /* check response type */
    if (GOAL_HTTP_RES_IMMEDT == resType) {
        /* set channel flag */
        ret = goal_httpChnState_set(pInst, pPrsInfo->in.pHttpChan, GOAL_HTTP_CHNFLAG_SENDRES);
        if (GOAL_RES_ERR(ret)) {
            return ret;
        }
    }

    return ret;
}


/****************************************************************************/
/** This function forwards the client request to the application.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_httpFwAppl(
    GOAL_HTTP_T        *pInst,                  /**< instance pointer */
    GOAL_HTTP_PARSE_T  *pPrs,                   /**< parser info */
    GOAL_HTTP_REQ_T    *pReq,                   /**< pointer to request */
    const uint8_t       **pBuf,                 /**< address of pointer to return buffer */
    uint32_t            *pLen,                  /**< return length */
    GOAL_HTTP_STATUS_T *pState,                 /**< return status */
    GOAL_HTTP_CONTENT_T *pContent               /**< return content type */
)
{
    GOAL_STATUS_T   ret = GOAL_ERROR;           /* return value */
    GOAL_HTTP_REQFRAG_T reqFrag;                /* request fragment information */
    GOAL_HTTP_REQCB_T   pfnApplCb;              /* application callback */
    GOAL_HTTP_APPLCB_DATA_T infoApplCb;         /* application callback info */

    static GOAL_BOOL_T writeAct = GOAL_FALSE;   /* active write flag */
    uint32_t id = 0;                            /* channel id */

    /* init application callback info struct */
    GOAL_MEMSET(&infoApplCb, 0, sizeof(GOAL_HTTP_APPLCB_DATA_T));

    if ((NULL == pBuf) || (NULL == pLen) || (NULL == pReq)) {
        /* NULL pointer error */
        return GOAL_ERR_NULL_POINTER;
    }

    /* set application callback pointer */
    pfnApplCb = (GOAL_HTTP_REQCB_T) (pPrs->out.pfnCb);

    /* get channel request info */
    ret = goal_httpChnGetReqInfo(pInst, pPrs->in.pHttpChan, &reqFrag);
    if (GOAL_RES_ERR(ret)) {
        return ret;
    }

    /* check for multipart POST request */
    if (GOAL_TRUE == reqFrag.flag) {
        if ((0 == reqFrag.lenRemain) && (GOAL_FALSE == writeAct)) {
            /* multipart request with one data chunk handle as oneshot */
            ret = goal_httpFwApplOnePost(pInst, pPrs, pBuf, pLen,
                                             pState, pContent, &infoApplCb);

        } else if ((0 == reqFrag.lenRemain) && (GOAL_TRUE == writeAct)) {
            /* check URL string */
            if (NULL == pPrs->out.strUrl) {
                ret = goal_httpGetUrlStr(pPrs, &pPrs->out.strUrl, &pPrs->out.lenUrl, &pPrs->out.lenMatch);
                if (GOAL_RES_ERR(ret)) {
                    return ret;
                }
            }

            /* get channel state */
            ret = goal_httpChnGetResType(pInst, pPrs->in.pHttpChan, NULL, pState);
            /* use application callback */
            if ((NULL != pfnApplCb) && (pPrs->out.lenData) &&
                (GOAL_RES_OK(ret)) && (GOAL_HTTP_STATUS_OK(*pState))) {
                /* set application callback info struct for POST DATA */
                infoApplCb.pInst = pInst;
                infoApplCb.reqType = GOAL_HTTP_FW_POST_DATA;
                infoApplCb.hdlRes = pPrs->out.hdlRes;

                ret = goal_httpChannelId(pInst, pPrs->in.pHttpChan, &id);
                if (GOAL_RES_OK(ret)) {
                    infoApplCb.hdlTransm = (GOAL_HTTP_HDL_T) (PtrCast) id;
                }

                infoApplCb.pRet = pState;
                infoApplCb.cs.pData = pPrs->out.pData;
                infoApplCb.cs.lenData = pPrs->out.lenData;
                infoApplCb.cs.pUrl = pPrs->out.strUrl;
                infoApplCb.cs.lenUrl = pPrs->out.lenUrl;
                infoApplCb.cs.lenMatch = pPrs->out.lenMatch;
                infoApplCb.sc.ppData = pBuf;
                infoApplCb.sc.pLenData = pLen;
                infoApplCb.sc.pContType = pContent;
                /* use callback */
                ret = (*pfnApplCb)(&infoApplCb);

                if ((GOAL_RES_OK(ret)) && (GOAL_HTTP_STATUS_OK(*pState))) {
                    /* set application callback info struct for POST END */
                    infoApplCb.pInst = pInst;
                    infoApplCb.reqType = GOAL_HTTP_FW_POST_END;
                    infoApplCb.hdlRes = pPrs->out.hdlRes;

                    ret = goal_httpChannelId(pInst, pPrs->in.pHttpChan, &id);
                    if (GOAL_RES_OK(ret)) {
                        infoApplCb.hdlTransm = (GOAL_HTTP_HDL_T) (PtrCast) id;
                    }

                    infoApplCb.pRet = pState;
                    infoApplCb.cs.pData = NULL;
                    infoApplCb.cs.lenData = 0;
                    infoApplCb.cs.pUrl = pPrs->out.strUrl;
                    infoApplCb.cs.lenUrl = pPrs->out.lenUrl;
                    infoApplCb.cs.lenMatch = pPrs->out.lenMatch;
                    infoApplCb.sc.ppData = pBuf;
                    infoApplCb.sc.pLenData = pLen;
                    infoApplCb.sc.pContType = pContent;
                    /* use callback */
                    ret = (*pfnApplCb)(&infoApplCb);
                }
            }
            /* last part of data received, set response type */
            ret = goal_httpChnSetResType(pInst, pPrs->in.pHttpChan, GOAL_HTTP_RES_IMMEDT, *pState);
            if ((GOAL_RES_OK(ret)) && (GOAL_HTTP_STATUS_ERR(*pState))) {
                /* set return length and content */
                *pLen = 0;
                *pContent = GOAL_HTTP_CONTENT_UNK;
            }
            /* reset write flag */
            writeAct = GOAL_FALSE;
        } else {
            /* check URL string */
            if (NULL == pPrs->out.strUrl) {
                ret = goal_httpGetUrlStr(pPrs, &pPrs->out.strUrl, &pPrs->out.lenUrl, &pPrs->out.lenMatch);
                if (GOAL_RES_ERR(ret)) {
                    return ret;
                }
            }

            /* first data chunk */
            if (GOAL_FALSE == writeAct) {
                /* use application callback */
                if (NULL != pfnApplCb) {
                    /* set application callback info struct for POST START */
                    infoApplCb.pInst = pInst;
                    infoApplCb.reqType = GOAL_HTTP_FW_POST_START;
                    infoApplCb.hdlRes = pPrs->out.hdlRes;

                    ret = goal_httpChannelId(pInst, pPrs->in.pHttpChan, &id);
                    if (GOAL_RES_OK(ret)) {
                        infoApplCb.hdlTransm = (GOAL_HTTP_HDL_T) (PtrCast) id;
                    }

                    infoApplCb.pRet = pState;
                    infoApplCb.cs.pData = NULL;
                    infoApplCb.cs.lenData = 0;
                    infoApplCb.cs.pUrl = pPrs->out.strUrl;
                    infoApplCb.cs.lenUrl = pPrs->out.lenUrl;
                    infoApplCb.cs.lenMatch = pPrs->out.lenMatch;
                    infoApplCb.sc.ppData = pBuf;
                    infoApplCb.sc.pLenData = pLen;
                    infoApplCb.sc.pContType = pContent;
                    /* use callback */
                    ret = (*pfnApplCb)(&infoApplCb);

                    if ((GOAL_RES_OK(ret)) && (GOAL_HTTP_STATUS_OK(*pState)) && (pPrs->out.lenData)) {
                        /* set application callback info struct for POST DATA */
                        infoApplCb.pInst = pInst;
                        infoApplCb.reqType = GOAL_HTTP_FW_POST_DATA;
                        infoApplCb.hdlRes = pPrs->out.hdlRes;

                        ret = goal_httpChannelId(pInst, pPrs->in.pHttpChan, &id);
                        if (GOAL_RES_OK(ret)) {
                            infoApplCb.hdlTransm = (GOAL_HTTP_HDL_T) (PtrCast) id;
                        }

                        infoApplCb.pRet = pState;
                        infoApplCb.cs.pData = pPrs->out.pData;
                        infoApplCb.cs.lenData = pPrs->out.lenData;
                        infoApplCb.cs.pUrl = pPrs->out.strUrl;
                        infoApplCb.cs.lenUrl = pPrs->out.lenUrl;
                        infoApplCb.cs.lenMatch = pPrs->out.lenMatch;
                        infoApplCb.sc.ppData = pBuf;
                        infoApplCb.sc.pLenData = pLen;
                        infoApplCb.sc.pContType = pContent;
                        /* use callback */
                        ret = (*pfnApplCb)(&infoApplCb);
                    }
                }
                /* set response type */
                ret = goal_httpChnSetResType(pInst, pPrs->in.pHttpChan, GOAL_HTTP_RES_LATER, *pState);
                if (GOAL_RES_ERR(ret)) {
                    return ret;
                }
                /* set flag */
                writeAct = GOAL_TRUE;
            } else {
                /* get channel state */
                ret = goal_httpChnGetResType(pInst, pPrs->in.pHttpChan, NULL, pState);
                /* use application callback */
                if ((NULL != pfnApplCb) && (GOAL_RES_OK(ret)) && (GOAL_HTTP_STATUS_OK(*pState))) {
                    /* set application callback info struct */
                    infoApplCb.pInst = pInst;
                    infoApplCb.reqType = GOAL_HTTP_FW_POST_DATA;
                    infoApplCb.hdlRes = pPrs->out.hdlRes;

                    ret = goal_httpChannelId(pInst, pPrs->in.pHttpChan, &id);
                    if (GOAL_RES_OK(ret)) {
                        infoApplCb.hdlTransm = (GOAL_HTTP_HDL_T) (PtrCast) id;
                    }

                    infoApplCb.pRet = pState;
                    infoApplCb.cs.pData = pPrs->out.pData;
                    infoApplCb.cs.lenData = pPrs->out.lenData;
                    infoApplCb.cs.pUrl = pPrs->out.strUrl;
                    infoApplCb.cs.lenUrl = pPrs->out.lenUrl;
                    infoApplCb.cs.lenMatch = pPrs->out.lenMatch;
                    infoApplCb.sc.ppData = pBuf;
                    infoApplCb.sc.pLenData = pLen;
                    infoApplCb.sc.pContType = pContent;
                    /* use callback */
                    ret = (*pfnApplCb)(&infoApplCb);
                }
            }
        }
    } else {
        switch (pReq->method)
        {
            case GOAL_HTTP_METHOD_GET:
                /* use application callback */
                if (NULL != pfnApplCb) {
                    /* set application callback info struct */
                    infoApplCb.pInst = pInst;
                    infoApplCb.reqType = GOAL_HTTP_FW_GET;
                    infoApplCb.hdlRes = pPrs->out.hdlRes;

                    ret = goal_httpChannelId(pInst, pPrs->in.pHttpChan, &id);
                    if (GOAL_RES_OK(ret)) {
                        infoApplCb.hdlTransm = (GOAL_HTTP_HDL_T) (PtrCast) id;
                    }

                    infoApplCb.pRet = pState;
                    infoApplCb.cs.pUrl = pPrs->out.strUrl;
                    infoApplCb.cs.lenUrl = pPrs->out.lenUrl;
                    infoApplCb.cs.lenMatch = pPrs->out.lenMatch;
                    infoApplCb.cs.lenData = 0;
                    infoApplCb.sc.ppData = pBuf;
                    infoApplCb.sc.pLenData = pLen;
                    infoApplCb.sc.pContType = pContent;
                    /* use callback */
                    ret = (*pfnApplCb)(&infoApplCb);
                } else {
                  /* error */
                  ret = GOAL_ERR_NULL_POINTER;
                }

                break;

            case GOAL_HTTP_METHOD_POST:
                /* oneshot post */
                ret = goal_httpFwApplOnePost(pInst, pPrs, pBuf, pLen,
                                             pState, pContent, &infoApplCb);

                break;

            default:
                break;
        }
    }
    return ret;
}


/****************************************************************************/
/** Forwards a oneshot POST to the application
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_httpFwApplOnePost(
    GOAL_HTTP_T        *pInst,                  /**< instance pointer */
    GOAL_HTTP_PARSE_T  *pPrs,                   /**< parser info */
    const uint8_t       **pBuf,                 /**< address of pointer to return buffer */
    uint32_t            *pLen,                  /**< return length */
    GOAL_HTTP_STATUS_T *pState,                 /**< return status */
    GOAL_HTTP_CONTENT_T *pContent,              /**< return content type */
    GOAL_HTTP_APPLCB_DATA_T *pCbInfo            /**< pointer to callback info */
)
{
    GOAL_STATUS_T ret = GOAL_OK;                /* return value */
    GOAL_HTTP_REQCB_T pfnApplCb = NULL;         /* application callback */
    uint32_t id = 0;                            /* channel id */

    /* set application callback pointer */
    pfnApplCb = (GOAL_HTTP_REQCB_T) (pPrs->out.pfnCb);

    /* use application callback */
    if (NULL == pfnApplCb) {
        ret = GOAL_ERR_NULL_POINTER;
    } else {
        /* check URL string */
        if (NULL == pPrs->out.strUrl) {
            ret = goal_httpGetUrlStr(pPrs, &pPrs->out.strUrl, &pPrs->out.lenUrl, &pPrs->out.lenMatch);
            if (GOAL_RES_ERR(ret)) {
                return ret;
            }
        }

        /* set application callback info struct for POST START */
        pCbInfo->pInst = pInst;
        pCbInfo->reqType = GOAL_HTTP_FW_POST_START;
        pCbInfo->hdlRes = pPrs->out.hdlRes;

        ret = goal_httpChannelId(pInst, pPrs->in.pHttpChan, &id);
        if (GOAL_RES_OK(ret)) {
            pCbInfo->hdlTransm = (GOAL_HTTP_HDL_T) (PtrCast) id;
        }

        pCbInfo->pRet = pState;
        pCbInfo->cs.pUrl = pPrs->out.strUrl;
        pCbInfo->cs.lenUrl = pPrs->out.lenUrl;
        pCbInfo->cs.lenMatch = pPrs->out.lenMatch;
        pCbInfo->cs.pData = NULL;
        pCbInfo->cs.lenData = 0;
        pCbInfo->sc.ppData = pBuf;
        pCbInfo->sc.pLenData = pLen;
        pCbInfo->sc.pContType = pContent;
        /* use callback */
        ret = (*pfnApplCb)(pCbInfo);

        if ((GOAL_RES_OK(ret)) && (GOAL_HTTP_STATUS_OK(*pState))) {
            /* set application callback info struct for POST DATA */
            pCbInfo->pInst = pInst;
            pCbInfo->reqType = GOAL_HTTP_FW_POST_DATA;
            pCbInfo->hdlRes = pPrs->out.hdlRes;

            ret = goal_httpChannelId(pInst, pPrs->in.pHttpChan, &id);
            if (GOAL_RES_OK(ret)) {
                pCbInfo->hdlTransm = (GOAL_HTTP_HDL_T) (PtrCast) id;
            }

            pCbInfo->pRet = pState;
            pCbInfo->cs.pUrl = pPrs->out.strUrl;
            pCbInfo->cs.lenUrl = pPrs->out.lenUrl;
            pCbInfo->cs.lenMatch = pPrs->out.lenMatch;
            pCbInfo->cs.pData = pPrs->out.pData;
            pCbInfo->cs.lenData = pPrs->out.lenData;
            pCbInfo->sc.ppData = pBuf;
            pCbInfo->sc.pLenData = pLen;
            pCbInfo->sc.pContType = pContent;
            /* use callback */
            ret = (*pfnApplCb)(pCbInfo);
        }

        if ((GOAL_RES_OK(ret)) && (GOAL_HTTP_STATUS_OK(*pState))) {
            /* set application callback info struct for POST END */
            pCbInfo->pInst = pInst;
            pCbInfo->reqType = GOAL_HTTP_FW_POST_END;
            pCbInfo->hdlRes = pPrs->out.hdlRes;

            ret = goal_httpChannelId(pInst, pPrs->in.pHttpChan, &id);
            if (GOAL_RES_OK(ret)) {
                pCbInfo->hdlTransm = (GOAL_HTTP_HDL_T) (PtrCast) id;
            }

            pCbInfo->pRet = pState;
            pCbInfo->cs.pUrl = pPrs->out.strUrl;
            pCbInfo->cs.lenUrl = pPrs->out.lenUrl;
            pCbInfo->cs.lenMatch = pPrs->out.lenMatch;
            pCbInfo->cs.pData = NULL;
            pCbInfo->cs.lenData = 0;
            pCbInfo->sc.ppData = pBuf;
            pCbInfo->sc.pLenData = pLen;
            pCbInfo->sc.pContType = pContent;
            /* use callback */
            ret = (*pfnApplCb)(pCbInfo);
        }
    }
    return ret;
}


/****************************************************************************/
/** Find URL String from parser info
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_httpGetUrlStr(
    GOAL_HTTP_PARSE_T  *pPrs,                   /**< parser info */
    char               **ppUrl,                 /**< URL string */
    uint32_t           *pLenUrl,                /**< URL length */
    uint32_t           *pLenMatch               /**< match length */
)
{
    GOAL_STATUS_T ret = GOAL_ERROR;             /* return value */
    uint32_t len = 0;                           /* length */
    uint32_t cnt;                               /* loop count */

    /* find end of request type string */
    for (cnt = 0; cnt < pPrs->in.lenMes; cnt++) {
        if (' ' == pPrs->in.strMes[cnt]) {
            ret = GOAL_OK;
            break;
        }
    }

    /* set URL string */
    if (GOAL_RES_OK(ret)) {
        *ppUrl = (char *) &pPrs->in.strMes[cnt + 1];
        ret = GOAL_ERROR;
        len = 1;
        for (cnt = cnt + 2; cnt < pPrs->in.lenMes; cnt++) {
            if (0 != pPrs->in.strMes[cnt]) {
                len++;
            } else {
                ret = GOAL_OK;
                break;
            }
        }
    }
    if (GOAL_RES_ERR(ret)) {
        *ppUrl = NULL;
        *pLenUrl = 0;
    } else if ((1 == len) && ('/' == (*ppUrl)[0])) {
        *ppUrl = GOAL_HTTP_STANDARD_INDEX_NAME;
        *pLenUrl = GOAL_HTTP_STANDARD_INDEX_NAME_LEN;
    } else {
        *pLenUrl = len;
    }

    /* set match length */
    *pLenMatch = *pLenUrl;

    return ret;
}


/****************************************************************************/
/** This function sends fragments of a prepared client response.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_httpSendFrag(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_BUFFER_T **ppBuf                       /**< pointer to buffer reference */
)
{
    GOAL_STATUS_T ret = GOAL_OK;                /* return value */

    /* check pointer */
    if ((NULL == pInst) || (NULL == pHttpChan) || (NULL == ppBuf) || (NULL == *ppBuf)) {
        return GOAL_ERR_NULL_POINTER;
    }

    ret = goal_netChanSend(pHttpChan->pChan, *ppBuf);

    if (GOAL_RES_OK(ret)) {
        pHttpChan->cntSendTry = 0;
    }
    else {
        pHttpChan->cntSendTry++;
    }

    /* report error */
    if (GOAL_RES_ERR(ret)) {
        if ((pHttpChan->cntSendTry < GOAL_HTTP_SEND_RETRY_MAX) && (!(GOAL_ERR_NET_SEND_CHAN_DISABLED == ret))) {
            /* retry in next loop iteration */
            ret = GOAL_OK_DELAYED;
        }
        else {
            goal_logErr("Buffer could not be sent");
            /* release buffer in case of error */
            goal_queueReleaseBuf(ppBuf);
            /* reset pending flag */
            goal_httpChnResetPend(pInst, pHttpChan);
        }
    }

    return ret;
}


/****************************************************************************/
/** Get http channel request information
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_httpChnGetReqInfo(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_HTTP_REQFRAG_T *pReqFrag               /**< pointer to fragmentation info */
)
{
    GOAL_STATUS_T ret = GOAL_ERROR;             /* return value */

    /* check pointer */
    if (NULL == pReqFrag) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* lock mutex */
    ret = goal_lockGet(pInst->lockChn, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(ret)) {
        goal_logErr("error acquiring channel lock");
        return ret;
    }

    /* copy fragmentation info */
    GOAL_MEMCPY(pReqFrag, &(pHttpChan->reqFrag), sizeof (GOAL_HTTP_REQFRAG_T));

    /* unlock mutex */
    goal_lockPut(pInst->lockChn);

    return ret;
}


/****************************************************************************/
/** Get http channel response information
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_httpChnGetResInfo(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_HTTP_RESFRAG_T *pResFrag,              /**< pointer to fragmentation info */
    GOAL_HTTP_CTC_FRAG_T *pCtcFrag              /**< pointer to CTC fragmentation info */
)
{
    GOAL_STATUS_T ret = GOAL_ERROR;             /* return value */

    if ((NULL == pInst) || (NULL == pHttpChan) || (NULL == pResFrag) || (NULL == pResFrag)) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* lock mutex */
    ret = goal_lockGet(pInst->lockChn, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(ret)) {
        goal_logErr("error acquiring channel lock");
        return ret;
    }

    /* copy fragmentation info */
    GOAL_MEMCPY(pResFrag, &(pHttpChan->resFrag), sizeof (GOAL_HTTP_RESFRAG_T));
    GOAL_MEMCPY(pCtcFrag, &(pHttpChan->ctcFrag), sizeof (GOAL_HTTP_CTC_FRAG_T));

    /* unlock mutex */
    goal_lockPut(pInst->lockChn);

    return ret;
}


/****************************************************************************/
/** Clear http channel information
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_httpChnClear(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan                  /**< channel pointer */
)
{
    GOAL_STATUS_T ret = GOAL_ERROR;             /* return value */

    if ((NULL == pInst) || (NULL == pHttpChan)) {
        /* error */
        return GOAL_ERR_NULL_POINTER;
    }

    /* lock mutex */
    ret = goal_lockGet(pInst->lockChn, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(ret)) {
        goal_logErr("error acquiring channel lock");
        return ret;
    }

    if (NULL != pHttpChan->pRes) {
        /* release buffer before its pointer is cleared */
        goal_queueReleaseBuf(&(pHttpChan->pRes));
    }

    /* reset channel information */
    GOAL_MEMSET(pHttpChan, 0, sizeof (GOAL_HTTP_CHN_T));
    /* restore instance pointer */
    pHttpChan->pInst = pInst;

    /* unlock mutex */
    goal_lockPut(pInst->lockChn);

    return ret;
}


/****************************************************************************/
/** Resets specified channel flags
 *
 * @retval GOAL_TRUE if flag is set
 * @retval GOAL_FALSE if flag is not set
 */
GOAL_STATUS_T goal_httpChnState_reset(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    uint32_t flag                               /**< channel flag */
)
{
    GOAL_STATUS_T ret = GOAL_ERROR;             /* return value */

    /* lock mutex */
    ret = goal_lockGet(pInst->lockChn, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(ret)) {
        goal_logErr("error acquiring channel lock");
        return ret;
    }

    /* reset flag */
    pHttpChan->chnState &= ~(flag);

    /* unlock mutex */
    goal_lockPut(pInst->lockChn);

    return ret;
}


/****************************************************************************/
/** Sets specified channel flags
 *
 * @retval GOAL_TRUE if flag is set
 * @retval GOAL_FALSE if flag is not set
 */
GOAL_STATUS_T goal_httpChnState_set(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    uint32_t flag                               /**< channel flag */
)
{
    GOAL_STATUS_T ret = GOAL_ERROR;             /* return value */

    /* lock mutex */
    ret = goal_lockGet(pInst->lockChn, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(ret)) {
        goal_logErr("error acquiring channel lock");
        return ret;
    }

    pHttpChan->chnState |= flag;

    /* unlock mutex */
    goal_lockPut(pInst->lockChn);

    return ret;
}


/****************************************************************************/
/** Returns state of channel flag
 *
 * @retval GOAL_TRUE if flag is set
 * @retval GOAL_FALSE if flag is not set
 */
GOAL_BOOL_T goal_httpChnState_get(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    uint32_t flag                               /**< channel flag */
)
{
    GOAL_BOOL_T retFlag = GOAL_FALSE;           /* return flag */
    GOAL_STATUS_T ret;                          /* return value */

    /* lock mutex */
    ret = goal_lockGet(pInst->lockChn, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(ret)) {
        goal_logErr("error acquiring channel lock");
        return retFlag;
    }

    retFlag = (flag & pHttpChan->chnState) ?  GOAL_TRUE : GOAL_FALSE;

    /* unlock mutex */
    goal_lockPut(pInst->lockChn);

    return retFlag;
}


/****************************************************************************/
/** Copyies data chunk to channel buffer
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_httpChnChkWind(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_HTTP_PARSE_T *pInfoPrs                 /**< pointer to parser info */
)
{
    GOAL_STATUS_T ret = GOAL_ERROR;             /* return value */
    uint32_t dataOffset = 0;                    /* data string offset */
    uint32_t offset;                            /* offset */
    GOAL_HTTP_REQFRAG_T *pReqFrag;              /* request fragmentation info */

    /* lock mutex */
    ret = goal_lockGet(pInst->lockChn, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(ret)) {
        goal_logErr("error acquiring channel lock");
        return ret;
    }

    /* get fragmentation info */
    pReqFrag = &pHttpChan->reqFrag;

    /* check if not all chunks are received */
    if ((GOAL_TRUE == pReqFrag->flag) && (GOAL_FALSE == pReqFrag->lastBound)) {
        /* find data string offset */
        for (dataOffset = 0; dataOffset < pHttpChan->lenData; dataOffset++) {
            if (0 == pHttpChan->tmpBuf[dataOffset]) {
                break;
            }
        }
        if (dataOffset == pHttpChan->lenData) {
            return GOAL_ERROR;
        }
        dataOffset++;
    }

    /* get offset from parsed message */
    offset = pInfoPrs->out.lenParsed;
    if ((offset >= dataOffset) && (offset <= pHttpChan->lenData)) {
        /* calculate new length */
        pHttpChan->lenData -= (offset - dataOffset);
        /* copy chunk to beginning of buffer */
        GOAL_MEMCPY(&(pHttpChan->tmpBuf[dataOffset]), &(pHttpChan->tmpBuf[offset]), pHttpChan->lenData - dataOffset);
        /* clear unused buffer */
        offset = pHttpChan->lenData;
        GOAL_MEMSET(&(pHttpChan->tmpBuf[offset]), 0, (HTTP_CHN_TMPBUF_LEN - offset));
    }

    /* unlock mutex */
    goal_lockPut(pInst->lockChn);

    return ret;
}


/****************************************************************************/
/** Copyies data chunk to channel buffer
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_httpChnCpyChnk(
    GOAL_HTTP_T     *pInst,                     /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_BUFFER_T   *pBuf,                      /**< pointer to buffer from net module */
    uint8_t         **pData,                    /**< return pointer for buffered data */
    uint32_t        *pLenData                   /**< return pointer for buffered data length */
)
{
    GOAL_STATUS_T ret = GOAL_ERROR;             /* return value */
    uint32_t offset = 0;                        /* offset */
    uint8_t *pDest;                             /* pointer to destination */
    uint32_t start = 0;                         /* start character */
    uint32_t end = 0;                           /* end character */

    /* lock mutex */
    ret = goal_lockGet(pInst->lockChn, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(ret)) {
        goal_logErr("error acquiring channel lock");
        return ret;
    }

    /* get recent data offset */
    offset = pHttpChan->lenData;

    if ((HTTP_CHN_TMPBUF_LEN - offset) < pBuf->dataLen) {
        ret = GOAL_ERROR;
    } else {
        /* set destination pointer */
        pDest = &(pHttpChan->tmpBuf[offset]);
        /* copy data chunk */
        GOAL_MEMCPY(pDest, pBuf->ptrData, pBuf->dataLen);
        /* update data length */
        pHttpChan->lenData += pBuf->dataLen;
        /* set returns */
        *pData = &(pHttpChan->tmpBuf[0]);
        *pLenData = pHttpChan->lenData;
        /* search header end */
        start = 0;
        end = pHttpChan->lenData;
        ret = http_searchString((char *) &(pHttpChan->tmpBuf[0]), GOAL_HTTP_HDREND_STR,
                                 &start, &end);
        if (GOAL_RES_OK(ret)) {
            /* set parse flag */
            pHttpChan->chnState |= GOAL_HTTP_CHNFLAG_PARSE;
        }
        /*set return value */
        ret = GOAL_OK;
    }

    /* unlock mutex */
    goal_lockPut(pInst->lockChn);

    return ret;
}


/****************************************************************************/
/** Sets http channel response type
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_httpChnGetResType(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_HTTP_RESTYP_T *pResType,               /**< pointer to response type, optional */
    GOAL_HTTP_STATUS_T *pHttpStatus             /**< pointer to http channel status, optional */
)
{
    GOAL_STATUS_T ret = GOAL_ERROR;             /* return value */

    /* lock mutex */
    ret = goal_lockGet(pInst->lockChn, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(ret)) {
        goal_logErr("error acquiring channel lock");
        return ret;
    }

    /* set return for response type and http channel status */
    if (pResType) {
        *pResType = pHttpChan->resType;
    }
    if (pHttpStatus) {
        *pHttpStatus = pHttpChan->httpStatus;
    }

    /* unlock mutex */
    goal_lockPut(pInst->lockChn);

    return ret;
}


/****************************************************************************/
/** Gets http channel response type
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_httpChnSetResType(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_HTTP_RESTYP_T resType,                 /**< response type */
    GOAL_HTTP_STATUS_T httpStatus               /**< http status of channel */
)
{
    GOAL_STATUS_T ret = GOAL_ERROR;             /* return value */

    /* lock mutex */
    ret = goal_lockGet(pInst->lockChn, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(ret)) {
        goal_logErr("error acquiring channel lock");
        return ret;
    }

    /* set response type and http channel status */
    pHttpChan->resType = resType;
    pHttpChan->httpStatus = httpStatus;

    /* unlock mutex */
    goal_lockPut(pInst->lockChn);

    return ret;
}


/****************************************************************************/
/** Sets http channel response buffer pointer
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_httpChnSetResBuf(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_BUFFER_T *pBuf                         /**< buffer pointer */
)
{
    GOAL_STATUS_T ret = GOAL_ERROR;             /* return value */

    /* lock mutex */
    ret = goal_lockGet(pInst->lockChn, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(ret)) {
        goal_logErr("error acquiring channel lock");
        return ret;
    }

    pHttpChan->pRes = pBuf;
    pHttpChan->pend = GOAL_TRUE;

    /* unlock muetx */
    goal_lockPut(pInst->lockChn);

    return ret;
}


/****************************************************************************/
/** Sets http channel request information
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_httpChnSetReqInfo(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_HTTP_REQFRAG_T *pReqFrag               /**< pointer to request information */
)
{
    GOAL_STATUS_T ret = GOAL_ERROR;             /* return value */

    /* lock mutex */
    ret = goal_lockGet(pInst->lockChn, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(ret)) {
        goal_logErr("error acquiring channel lock");
        return ret;
    }

    /* set request fragmentation info */
    GOAL_MEMCPY(&(pHttpChan->reqFrag), pReqFrag, sizeof(GOAL_HTTP_REQFRAG_T));

    /* unlock mutex */
    goal_lockPut(pInst->lockChn);

    return ret;
}


/****************************************************************************/
/** Sets http channel response information
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_httpChnSetResInfo(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_BUFFER_T *pBuf,                        /**< buffer pointer */
    GOAL_HTTP_RESFRAG_T *pResFrag               /**< pointer to response info */
)
{
    GOAL_STATUS_T ret = GOAL_ERROR;             /* return value */

    /* lock mutex */
    ret = goal_lockGet(pInst->lockChn, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(ret)) {
        goal_logErr("error acquiring channel lock");
        return ret;
    }

    if ((NULL == pHttpChan->pRes) || (pBuf == pHttpChan->pRes)) {
        /* set buffer */
        pHttpChan->pRes = pBuf;
        /* set response fragmentation info */
        GOAL_MEMCPY(&(pHttpChan->resFrag), pResFrag, sizeof(GOAL_HTTP_RESFRAG_T));
        ret = GOAL_OK;
    }
    else {
        /* skip channels with other active response buffers */
        ret = GOAL_ERROR;
    }

    /* unlock mutex */
    goal_lockPut(pInst->lockChn);

    return ret;
}


/****************************************************************************/
/** Sets http channel ready to send
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_httpChnSetRdySend(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_BOOL_T flag                            /**< ready to send flag */
)
{
    GOAL_STATUS_T ret = GOAL_ERROR;             /* return value */

    /* lock mutex */
    ret = goal_lockGet(pInst->lockChn, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(ret)) {
        goal_logErr("error acquiring channel lock");
        return ret;
    }

    /* find channel */
    pHttpChan->rdySend = flag;

    /* unlock mutex */
    goal_lockPut(pInst->lockChn);

    return ret;
}


/****************************************************************************/
/** Gets http channel ready to send
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_httpChnGetRdySend(
    GOAL_HTTP_T *pInst,                         /**< [in] instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< [in] channel pointer */
    GOAL_BOOL_T *pFlag                          /**< [out] ready to send pointer */
)
{
    GOAL_STATUS_T ret = GOAL_ERROR;             /* return value */

    /* lock mutex */
    ret = goal_lockGet(pInst->lockChn, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(ret)) {
        goal_logErr("error acquiring channel lock");
        return ret;
    }

    /* set flag */
    *pFlag = pHttpChan->rdySend;

    /* unlock mutex */
    goal_lockPut(pInst->lockChn);

    return ret;
}


/****************************************************************************/
/** Resets buffer pending flag
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_httpChnResetPend(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan                  /**< channel pointer */
)
{
    GOAL_STATUS_T ret = GOAL_ERROR;             /* return value */

    /* lock mutex */
    ret = goal_lockGet(pInst->lockChn, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(ret)) {
        goal_logErr("error acquiring channel lock");
        return ret;
    }

    /* set flag */
    pHttpChan->pend = GOAL_FALSE;

    /* unlock mutex */
    goal_lockPut(pInst->lockChn);

    return ret;
}


/****************************************************************************/
/** Gets http channel send buffer pending flag
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_httpChnGetPend(
    GOAL_HTTP_T *pInst,                         /**< [in] instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< [in] channel pointer */
    GOAL_BOOL_T *pFlag                          /**< [out] pend flag pointer */
)
{
    GOAL_STATUS_T ret = GOAL_ERROR;             /* return value */

    /* lock mutex */
    ret = goal_lockGet(pInst->lockChn, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(ret)) {
        goal_logErr("error acquiring channel lock");
        return ret;
    }

    /* set flag */
    *pFlag = pHttpChan->pend;

    /* unlock mutex */
    goal_lockPut(pInst->lockChn);

    return ret;
}


/****************************************************************************/
/** Registers the http module for GOAL.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_httpNewImpl(
    GOAL_HTTP_T **ppInst,                       /**< pointer to instance pointer */
    uint16_t port,                              /**< port to link instance to */
    uint16_t cntChn                             /**< channel count */
)
{
    GOAL_STATUS_T ret = GOAL_OK;                /* return value */
    GOAL_HTTP_CHN_T *pChanHdl = NULL;           /* pointer to channel handler */
    GOAL_NET_ADDR_T addr;                       /* net address */
    uint32_t optVal;                            /* option value */
    uint32_t cnt;                               /* loop counter */
    uint32_t cntInst = 0;                       /* instance count */
    GOAL_INSTANCE_LIST_T *pList = NULL;         /* instance list entry */
    GOAL_INSTANCE_T *pInst;                     /* instance entry */

    /* get instance list entry */
    ret = goal_instListGetByType(&pList, GOAL_ID_HTTP);

    if (GOAL_RES_OK(ret) && (NULL != pList)) {
        /* find instance entry */
        GOAL_LL_FOREACH(pList->pInst, pInst) {
            if (((GOAL_HTTP_T *) pInst)->localPort == port) {
                /* port is already in use */
                ret = GOAL_ERR_ALREADY_USED;
            }
            cntInst++;
        }
    }
    else {
        /* instance does not exist yet */
        ret = GOAL_OK;
    }

    if (GOAL_RES_OK(ret)) {
        /* new http instance, use recent instance count as id */
        ret = goal_instNew((GOAL_INSTANCE_T **) ppInst, sizeof(GOAL_HTTP_T), GOAL_ID_HTTP, cntInst,
                            "HTTP Instance");
    }

    /* store channel count and port number */
    if (GOAL_RES_OK(ret)) {
        /* set TCP port in instance */
        (*ppInst)->localPort = port;
        (*ppInst)->cntChn = cntChn;
        /* allocate memory for the channel handler */
        ret = goal_memCalloc(&pChanHdl, (cntChn * sizeof(GOAL_HTTP_CHN_T)));
    }

    /* set channel handler pointer */
    if (GOAL_RES_OK(ret)) {
        (*ppInst)->pChanHdl = pChanHdl;
        /* create lock for channel handler */
        ret = goal_lockCreate(GOAL_LOCK_BINARY, &((*ppInst)->lockChn), 0, 1, GOAL_ID_HTTP);
    }

    if (GOAL_RES_OK(ret)) {
        /* allocate memory for template path */
        ret = goal_memCalloc(&((*ppInst)->pTemplatePath), sizeof(GOAL_HTTP_TMPMGR_L_PATH_T));
    }

    if (GOAL_RES_OK(ret)) {
        /* init and set address struct */
        GOAL_MEMSET(&addr, 0, sizeof(GOAL_NET_ADDR_T));
        addr.localPort = port;

        /* create NET channel */
        ret = goal_netChanCreate(&((*ppInst)->pTcpListener), GOAL_NET_TCP_LISTENER, cntChn);
        if (GOAL_RES_ERR(ret)) {
            goal_logErr("failed to create TCP Listener");
        }
    }

    if (GOAL_RES_OK(ret)) {
        /* register TCP server */
        ret = goal_netChanOpen((*ppInst)->pTcpListener, &addr, httpNetCb, *ppInst);
        if (GOAL_RES_ERR(ret)) {
            goal_logErr("error while opening TCP server channel on port %"FMT_u32, (uint32_t) (addr.localPort));
        }
    }

    if (GOAL_RES_OK(ret)) {
        /* set TCP channel to non-blocking */
        optVal = 1;
        ret = goal_netChanOptionSet((*ppInst)->pTcpListener, GOAL_NET_OPTION_NONBLOCK, &optVal);
        if (GOAL_RES_ERR(ret)) {
            goal_logErr("error while setting TCP channel to non-blocking");
        }
    }

    if (GOAL_RES_OK(ret)) {
        /* init HTTP channels */
        for (cnt = 0; cnt < cntChn; cnt++) {
            (*ppInst)->pChanHdl[cnt].pChan = NULL;
            (*ppInst)->pChanHdl[cnt].id = cnt;
            (*ppInst)->pChanHdl[cnt].pInst = *ppInst;
        }
    }

    /* register main loop function */
    ret = goal_mainLoopParamReg(goal_httpMainLoop, *ppInst);
    if (GOAL_RES_ERR(ret)) {
        goal_logErr("Registration of main loop function failed");
    }

    return ret;
}


/****************************************************************************/
/** Sends buffer fragments in case there are some pending.
 *
 */
static void goal_httpMainLoop(
    void *pArg                                  /**< instance handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_HTTP_T *pInst = NULL;                  /* instance pointer */
    GOAL_NET_CHAN_T *pChan;                     /* Net channel of HTTP channel */
    GOAL_BUFFER_T **ppBuf = NULL;               /* buffer pointer reference */
    uint32_t cnt;                               /* loop counter */

    if (NULL == pArg) {
        goal_logErr("instance is NULL");
        return;
    }

    pInst = (GOAL_HTTP_T *) pArg;

    for (cnt = 0; cnt < pInst->cntChn; cnt++) {
        /* get lock */
        res = goal_lockGet(pInst->lockChn, GOAL_LOCK_INFINITE);
        if (GOAL_RES_OK(res)) {
            /* get channel pointer */
            pChan = pInst->pChanHdl[cnt].pChan;
            /* get buffer pointer */
            ppBuf = &pInst->pChanHdl[cnt].pRes;
            /* release lock */
            goal_lockPut(pInst->lockChn);
            if (NULL != pChan) {
                /* release buffer or send next fragment */
                res = goal_httpBufRelease(pInst, &pInst->pChanHdl[cnt], ppBuf);
                if (GOAL_RES_ERR(res)) {
                    goal_logErr("error during release of buffer or sending next fragment");
                    /* clear channel */
                    res = goal_httpChnClear(pInst, &pInst->pChanHdl[cnt]);
                    if (GOAL_RES_ERR(res)) {
                        goal_logErr("failed to clear channel");
                    }
                }
            }
        }
    }
}


/****************************************************************************/
/** Registers the http module for GOAL.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_httpInitImpl(
    void
)
{
    GOAL_STATUS_T ret = GOAL_OK;                /* return value */
    static GOAL_BOOL_T initFlag = GOAL_FALSE;   /* init flag for this function */

    /* check init flag */
    if (GOAL_TRUE == initFlag) {
        /* error */
        goal_logErr("Already initialized");
        ret = GOAL_ERROR;
    } else {
        /* set init flag */
        initFlag = GOAL_TRUE;

        /* init HTTP CM module */
        ret = goal_httpRegCmVars();
        if (GOAL_RES_ERR(ret)) {
            return ret;
        }

#if GOAL_CONFIG_HTTPS == 1
        /* init HTTPS CM module */
        ret = goal_httpsRegCmVars();
        if (GOAL_RES_ERR(ret)) {
            return ret;
        }
#endif /* GOAL_CONFIG_HTTPS == 1 */
    }

    return ret;
}


/****************************************************************************/
/** Initializes a new instance of the https server.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_httpsNewImpl(
    GOAL_HTTP_T **ppInst,                       /**< HTTPS instance */
    uint16_t port,                              /**< port to link instance to */
    uint16_t cntChn                             /**< channel count */
)
{
#if GOAL_CONFIG_HTTPS == 1
    GOAL_MI_TLS_CERT_T miTlsCert;               /* certificate info struct */
    GOAL_MI_TLS_T *pMiTls = NULL;               /* MA TLS handle */
    GOAL_STATUS_T res;                          /* result */

    res = goal_httpNewImpl(ppInst, port, cntChn);

    /* get certificate information */
    if (GOAL_RES_OK(res)) {
        GOAL_MEMSET(&miTlsCert, 0, sizeof(GOAL_MI_TLS_CERT_T));
        res = goal_httpsGetCert(&miTlsCert);
    }

    if (GOAL_RES_OK(res)) {
        /* open TLS channel */
        res = goal_miTlsOpen(GOAL_MI_TLS_ID_DEFAULT, GOAL_MA_TLS_ID_DEFAULT, &miTlsCert);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("error while opening TLS channel");
        }
    }

    if (GOAL_RES_OK(res)) {
        /* get MI TLS handle */
        res = goal_miTlsGetById(&pMiTls, GOAL_MI_TLS_ID_DEFAULT);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("Could not get MI TLS handle from given ID");
        }
    }

    if (GOAL_RES_OK(res)) {
        /* open TLS channel */
        res = goal_miTlsSessOpen(pMiTls, (*ppInst)->pTcpListener, (*ppInst)->cntChn);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("error while opening TLS session");
        }
    }

    return res;
#else
    UNUSEDARG(ppInst);
    UNUSEDARG(port);
    UNUSEDARG(cntChn);

    return GOAL_ERR_UNSUPPORTED;
#endif /* GOAL_CONFIG_HTTPS == 1 */
}


/****************************************************************************/
/** Adds the current message strMes to the buffer ppBuf. If message is too long,
 * the buffer will be send with part of the message and a new one is requested,
 * which is repeated until message is added completely or no buffer is free.
 *
 * If a buffer could not be sent, it is stored as pAnswBuf and *ppBuf is set
 * to NULL. If not the complete message could be sent, the rest is stored in
 * strMesMem and lenMesMem.
 *
 * @retval GOAL_OK successful
 * @retval GOAL_ERR_NET_SEND could not get new buffer, message is stored
 * @retval other failed
 */
GOAL_STATUS_T goal_httpAddToBuffer(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_BUFFER_T *pBuf,                        /**< buffer pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    const uint8_t *strMes,                      /**< message for sending */
    uint32_t len                                /**< length of message */
)
{
    GOAL_STATUS_T ret = GOAL_OK;                /* return value */
    uint32_t      bufSize = pBuf->bufSize;      /* send buffer size */
    uint32_t      offset;                       /* data copy offset */
    uint32_t      copylen;                      /* data copy length */
    GOAL_HTTP_RESFRAG_T resFrag;                /* response fragmentation info */

    /* if message fits in current buffer, add it */
    if ((pBuf->dataLen + len) <= bufSize) {
        /* calculate data offset and copy length */
        offset = pBuf->dataLen;
        copylen = len;
        /* set fragmentation info */
        resFrag.flag = GOAL_FALSE;
        resFrag.pData = NULL;
        resFrag.lenData = 0;
        resFrag.srcOff = 0;
    }
    /* if message does not fit, copy part */
    else {
        /* calculate data offset and copy length */
        offset = pBuf->dataLen;
        copylen = bufSize - offset;
        /* set fragmentation info */
        resFrag.flag = GOAL_TRUE;
        resFrag.pData = (const uint8_t *) strMes;
        resFrag.lenData = len;
        resFrag.srcOff = copylen;
    }

    /* set fragmentation info */
    ret = goal_httpChnSetResInfo(pInst, pHttpChan, pBuf, &resFrag);
    if (GOAL_RES_ERR(ret)) {
        return ret;
    }

    /* copy data into send buffer */
    GOAL_MEMCPY((char *)(pBuf->ptrData + offset), strMes, copylen);
    /* update data length */
    pBuf->dataLen += copylen;

    return ret;
}


