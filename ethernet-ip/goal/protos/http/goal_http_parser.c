/** @file
 *
 * @brief Implementation of the Goal http resource manager
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

#include "goal_includes.h"
#include "goal_http.h"
#include "goal_http_resmgr.h"
#include "goal_http_parser.h"
#include "goal_http_auth.h"
#include "cm/goal_http_cm.h"
#include "b64/cdecode.h"


/****************************************************************************/
/* Local types */
/****************************************************************************/


/****************************************************************************/
/* Local variables */
/****************************************************************************/


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/

static GOAL_STATUS_T httpParse_ParseInfoCheck(
    GOAL_HTTP_PARSE_T *pInfoParse               /**< pointer to parser information struct */
);

static GOAL_STATUS_T goal_httpSubStr(
    uint8_t *strMes,                            /**< string of message to parse */
    uint32_t lenMes,                            /**< length of message */
    uint32_t startChar,                         /**< number of char where substring starts */
    uint32_t *pEndChar,                         /**< pointer to number of char, where substring ends */
    char symbol                                 /**< divider symbol */
);

static GOAL_STATUS_T httpParse_ParseGet(
    GOAL_HTTP_T         *pInst,                 /**< instance pointer */
    GOAL_HTTP_PARSE_T   *pInfoParse,            /**< pointer to parse info struct */
    uint32_t            methOffset              /**< method string offset */
);

static GOAL_STATUS_T httpParse_ParsePost(
    GOAL_HTTP_T         *pInst,                 /**< instance pointer */
    GOAL_HTTP_PARSE_T   *pInfoParse,            /**< pointer to parse info struct */
    uint32_t            methOffset              /**< method string offset */
);

static GOAL_STATUS_T httpParse_HandleMultipart(
    GOAL_HTTP_T         *pInst,                 /**< instance pointer */
    GOAL_HTTP_PARSE_T   *pInfoParse             /**< pointer to parser info */
);

static GOAL_STATUS_T httpParse_ParsePut(
    GOAL_HTTP_PARSE_T   *pInfoParse,            /**< pointer to parse info struct */
    uint32_t            methOffset              /**< method string offset */
);

static GOAL_STATUS_T httpParse_ParseDelete(
    GOAL_HTTP_PARSE_T   *pInfoParse,            /**< pointer to parse info struct */
    uint32_t            methOffset              /**< method string offset */
);

static GOAL_STATUS_T httpParse_checkAuth(
    GOAL_HTTP_T         *pInst,                 /**< instance pointer */
    GOAL_HTTP_HDL_T     hdlRes,                 /**< resource handle */
    uint32_t            authReq                 /**< authentication required */
);

static GOAL_STATUS_T httpParse_checkAccRights(
    GOAL_HTTP_T         *pInst,                 /**< instance pointer */
    GOAL_HTTP_HDL_T     hdlRes,                 /**< resource handle */
    uint32_t            access                  /**< desired access */
);

static GOAL_STATUS_T httpParse_ParseDestination(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    uint8_t     *pPath,                         /**< resource path */
    uint32_t    lenPath,                        /**< length of path */
    GOAL_HTTP_PARSE_T *pInfoParse               /**< pointer to parse info struct */
);

static GOAL_STATUS_T httpParse_searchString(
    char *pMes,                                 /**< message string */
    char *strSearch,                            /**< string to look for */
    uint32_t *pStart,                           /**< start character */
    uint32_t *pEnd                              /**< end character */
);

static GOAL_STATUS_T httpParse_searchMimeType(
    uint8_t *str,                               /**< pointer to message */
    uint32_t startChar,                         /**< offset start character */
    uint32_t endChar,                           /**< offset end character */
    GOAL_HTTP_POST_MIME_T *pMime                /**< [out] pointer to mime type */
);

static uint32_t httpParse_convertDez(
    uint8_t     *str,                           /**< string to convert */
    uint32_t    lenStr                          /**< string length */
);


/****************************************************************************/
/** Parses the HTTP request.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_httpParse(
    GOAL_HTTP_T         *pInst,                 /**< instance pointer */
    GOAL_HTTP_PARSE_T   *pInfoParse             /**< pointer to parser information struct */
)
{
    uint32_t startChar = 0;                     /* number of start char of substring */
    uint32_t currentChar = 0;                   /* number of current char of substring */
    uint32_t cnt;                               /* count */
    GOAL_STATUS_T res;                          /* result */
    GOAL_HTTP_METHODS_T method;                 /* http method */
    GOAL_HTTP_REQFRAG_T reqFrag;                /* request fragmentation info */

    /* check input information */
    res = httpParse_ParseInfoCheck(pInfoParse);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* reset return information */
    pInfoParse->out.method = GOAL_HTTP_METHOD_NONE;
    pInfoParse->out.fileExt = GOAL_HTTP_FILE_EXT_UNK;
    pInfoParse->out.hdlRes = 0;

    /* get channel info */
    res = goal_httpChnGetReqInfo(pInst, pInfoParse->in.pHttpChan, &reqFrag);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* in case request fragmentation is already recognized, do not search method string */
    if ((GOAL_FALSE == reqFrag.flag) || (GOAL_FALSE == reqFrag.frstBound)) {
        /* find first substring = method */
        res = goal_httpSubStr(pInfoParse->in.strMes, pInfoParse->in.lenMes, startChar, &currentChar, ' ');
        if (GOAL_RES_ERR(res)) {
            return res;
        }
    }

    /* get method from substring */
    method = GOAL_HTTP_METHOD_NONE;
    for (cnt = GOAL_HTTP_METHOD_NONE + 1; (cnt < pInfoParse->in.lenMethTable) && (GOAL_HTTP_METHOD_NONE == method); cnt++) {
        if (0 == GOAL_MEMCMP((char *) pInfoParse->in.strMes, pInfoParse->in.pMethTable[cnt].strMes, pInfoParse->in.pMethTable[cnt].lenMes)) {
            method = (GOAL_HTTP_METHODS_T)  cnt;
            goal_logDbg("http method found: %s", pInfoParse->in.pMethTable[cnt].strMes);
        }
    }

    /* check if method is supported by http */
    switch (method) {
        case GOAL_HTTP_METHOD_GET:
            goal_logDbg("GET method found");
            /* set method */
            pInfoParse->out.method = method;
            /* parse get request */
            res = httpParse_ParseGet(pInst, pInfoParse, currentChar);
            break;

        case GOAL_HTTP_METHOD_POST:
            goal_logDbg("POST method found");
            /* set method */
            pInfoParse->out.method = method;
            /* parse request */
            res = httpParse_ParsePost(pInst, pInfoParse, currentChar);
            /* change type name to garbage value if parsed message exists */
            if (0 < pInfoParse->out.lenParsed) {
                for (cnt = 0; cnt <= currentChar; cnt++) {
                    pInfoParse->in.strMes[cnt] = '-';
                }
            }
            break;

        case GOAL_HTTP_METHOD_NONE:
            goal_logDbg("No method found, checking for multipart request");
            /* check for multipart request */
            res = httpParse_HandleMultipart(pInst, pInfoParse);
            break;

        case GOAL_HTTP_METHOD_PUT:
            goal_logDbg("PUT method found");
            /* set method */
            pInfoParse->out.method = method;
            /* parse put request */
            res = httpParse_ParsePut(pInfoParse, currentChar);
            break;

        case GOAL_HTTP_METHOD_DELETE:
            goal_logDbg("DELETE method found");
            /* set method */
            pInfoParse->out.method = method;
            /* parse delete request */
            res = httpParse_ParseDelete(pInfoParse, currentChar);
            break;

        default:
            goal_logErr("Unsuported http method");
            return GOAL_ERR_UNSUPPORTED;
    }

    return res;
}


/****************************************************************************/
/** This function parses a received PUT request.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T httpParse_ParsePut(
    GOAL_HTTP_PARSE_T   *pInfoParse,            /**< pointer to parse info struct */
    uint32_t            methOffset              /**< method string offset */
)
{
    GOAL_STATUS_T ret = GOAL_ERROR;             /* return value */

    UNUSED_ARG(pInfoParse);
    UNUSED_ARG(methOffset);

    goal_logErr("PUT method not implemented");

    return ret;
}


/****************************************************************************/
/** This function parses a received DELETE request.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T httpParse_ParseDelete(
    GOAL_HTTP_PARSE_T   *pInfoParse,            /**< pointer to parse info struct */
    uint32_t            methOffset              /**< method string offset */
)
{
    GOAL_STATUS_T ret = GOAL_ERROR;             /* return value */

    UNUSED_ARG(pInfoParse);
    UNUSED_ARG(methOffset);

    goal_logErr("DELETE method not implemented");

    return ret;
}


/****************************************************************************/
/** This function parses a received mulitpart request.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T httpParse_HandleMultipart(
    GOAL_HTTP_T         *pInst,                 /**< instance pointer */
    GOAL_HTTP_PARSE_T   *pInfoParse             /**< pointer to parser info */
)
{
    GOAL_STATUS_T           ret = GOAL_OK;      /* return value */
    GOAL_HTTP_REQFRAG_T     reqFrag;            /* request fragmentation info */

    uint32_t startChar = 0;                     /* start character index */
    uint32_t currentChar = 0;                   /* current character index */
    uint32_t lenBound = 0;                      /* boundary length */
    uint32_t startHead;                         /* MIME header start index */
    uint32_t dataHdrOffset;                     /* data header string offset */
    uint32_t dataOffset;                        /* data string offset */

    /* get channel info */
    ret = goal_httpChnGetReqInfo(pInst, pInfoParse->in.pHttpChan, &reqFrag);
    if (GOAL_RES_ERR(ret)) {
        return ret;
    }

    /* reset channel flags */
    goal_httpChnState_reset(pInst, pInfoParse->in.pHttpChan, (GOAL_HTTP_CHNFLAG_FORWARD
                            | GOAL_HTTP_CHNFLAG_SENDRES
                            | GOAL_HTTP_CHNFLAG_CLOSCHN));

    /* check mulitpart flag */
    if (GOAL_TRUE == reqFrag.flag) {
        /* find data header string offset */
        for (dataHdrOffset = 0; dataHdrOffset < pInfoParse->in.lenMes; dataHdrOffset++) {
            if (0 == pInfoParse->in.strMes[dataHdrOffset]) {
                break;
            }
        }
        if (dataHdrOffset == pInfoParse->in.lenMes) {
            return GOAL_ERROR;
        }
        dataHdrOffset++;

        /* first boundary not found, search it now */
        if (GOAL_FALSE == reqFrag.frstBound) {
            /* find first boundary */
            startChar = 0;
            currentChar = pInfoParse->in.lenMes;
            ret = httpParse_searchString((char *) (pInfoParse->in.strMes), (char *) (reqFrag.boundary), &startChar, &currentChar);
            if (GOAL_RES_ERR(ret)) {
                /* boundary not found do nothing until next chunk arrives */
                return GOAL_OK;
            }

            /* set MIME header start index to first boundary: 2 '-' are added to boundary in MIME header */
            startHead = startChar - 2;

            /* search header end relative to first boundary */
            currentChar = pInfoParse->in.lenMes;
            ret = httpParse_searchString((char *) (pInfoParse->in.strMes), GOAL_HTTP_HDREND_STR, &startChar, &currentChar);
            if (GOAL_RES_ERR(ret)) {
                /* MIME header and not found, do nothing until next chunk arrives */
                return GOAL_OK;
            }

            /* set data string offset */
            dataOffset = currentChar;

            /* set first boundary flag */
            reqFrag.frstBound = GOAL_TRUE;

            /* set header and footer length */
            reqFrag.lenHead = currentChar - startHead;
            reqFrag.lenFoot = (uint32_t) GOAL_STRLEN((char *) (reqFrag.boundary)) + 8;
        } else {
            /* set data string offset to data header offset, since no data header exists more */
            dataOffset = dataHdrOffset;
        }

        /* check if, last part has arrived */
        if (pInfoParse->in.lenMes - dataHdrOffset <= reqFrag.lenRemain) {
            /* set outputs */
            pInfoParse->out.hdlRes = reqFrag.hdlRes;
            pInfoParse->out.pfnCb = reqFrag.pfnAppCb;
            pInfoParse->out.pData = pInfoParse->in.strMes + dataOffset;
            pInfoParse->out.lenData = pInfoParse->in.lenMes - dataOffset;
            pInfoParse->out.lenParsed = pInfoParse->in.lenMes;
            /* update remaining length */
            reqFrag.lenRemain -= (pInfoParse->in.lenMes - dataHdrOffset);

        } else {
            /* last part */
            reqFrag.lenRemain = 0;
            /* set outputs */
            pInfoParse->out.hdlRes = reqFrag.hdlRes;
            pInfoParse->out.pfnCb = reqFrag.pfnAppCb;
            pInfoParse->out.pData = pInfoParse->in.strMes + dataOffset;
            pInfoParse->out.lenData = pInfoParse->in.lenMes - dataOffset;
            pInfoParse->out.lenParsed = pInfoParse->in.lenMes;
        }

        /* search last boundary string relative to first boundary */
        if (GOAL_FALSE == reqFrag.lastBound) {
            if (0 < reqFrag.lenHead) {
                currentChar = pInfoParse->in.lenMes;
                ret = httpParse_searchString((char *) (pInfoParse->in.strMes), (char *) (reqFrag.boundary), &startChar, &currentChar);
                if (GOAL_RES_OK(ret)) {
                    /* last boundary found */
                    reqFrag.lastBound = GOAL_TRUE;

                    /* zero remaining length, thus ignoring any additional whitespace
                     * characters */
                    lenBound = 8 + (uint32_t) GOAL_STRLEN((char *) reqFrag.boundary);

                    if (lenBound < pInfoParse->out.lenData) {
                        pInfoParse->out.lenData -= lenBound;
                    }

                    reqFrag.lenRemain = 0;
                }
            } else {
                if (0 == reqFrag.lenRemain) {
                    /* last bound found */
                    reqFrag.lastBound = GOAL_TRUE;
                }
            }
        }
    }

    /* set application forwarding active */
    ret = goal_httpChnState_set(pInst, pInfoParse->in.pHttpChan, GOAL_HTTP_CHNFLAG_FORWARD);
    if (GOAL_RES_ERR(ret)) {
        return ret;
    }

    /* set updated channel info */
    ret = goal_httpChnSetReqInfo(pInst, pInfoParse->in.pHttpChan, &reqFrag);
    if (GOAL_RES_ERR(ret)) {
        return ret;
    }

    return ret;
}


/****************************************************************************/
/** This function parses a received POST request.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T httpParse_ParsePost(
    GOAL_HTTP_T         *pInst,                 /**< instance pointer */
    GOAL_HTTP_PARSE_T   *pInfoParse,            /**< pointer to parse info struct */
    uint32_t            methOffset              /**< method string offset */
)
{
    GOAL_STATUS_T ret = GOAL_ERROR;             /* return value */
    uint32_t startChar;                         /* start character index */
    uint32_t currentChar;                       /* current character index */
    uint32_t hdrEndChar;                        /* header end character */
    char     *strUrl;                           /* URL string */
    char     *strTemp;                          /* temp string */
    uint32_t lenUrl;                            /* URL string length */
    uint32_t lenTemp;                           /* temp string length */
    uint32_t lenBound;                          /* boundary string length */
    uint32_t lenContStr;                        /* content length string length */
    uint32_t startHead;                         /* MIME header start index */
    GOAL_HTTP_REQFRAG_T reqFrag;                /* request fragmentation info */
    GOAL_HTTP_POST_MIME_T mimeType;             /* MIME type identifier */
    uint32_t hdrOffset;                         /* header string offset */

    /* init fragemtation info */
    GOAL_MEMSET(&reqFrag, 0, sizeof(GOAL_HTTP_REQFRAG_T));

    /* init output with input message */
    pInfoParse->out.lenData = pInfoParse->in.lenMes;
    pInfoParse->out.pData = pInfoParse->in.strMes;

    /* find second substring = resource path */
    startChar = methOffset + 2;
    ret = goal_httpSubStr(pInfoParse->in.strMes, pInfoParse->in.lenMes, startChar, &currentChar, ' ');
    if (GOAL_RES_ERR(ret)) {
        return ret;
    }

    /* special case handling: change default "/ " to default path */
    lenUrl = currentChar - startChar + 1;
    strUrl = (char *) (pInfoParse->in.strMes + startChar);
    strUrl[lenUrl] = 0;
    if ((1 == lenUrl) && (strUrl[0] == '/')) {
        /* determine resource and destination */
        ret = httpParse_ParseDestination(pInst, (uint8_t *) GOAL_HTTP_STANDARD_INDEX_NAME, GOAL_HTTP_STANDARD_INDEX_NAME_LEN, pInfoParse);
        if (GOAL_RES_ERR(ret)) {
            return ret;
        }
        /* log info */
        goal_logInfo("POST URL requested: %s", GOAL_HTTP_STANDARD_INDEX_NAME);
    } else {
        /* determine resource and destination */
        ret = httpParse_ParseDestination(pInst, (uint8_t *) strUrl, lenUrl, pInfoParse);
        if (GOAL_RES_ERR(ret)) {
            return ret;
        }
        /* log info */
        goal_logInfo("POST URL requested: %s", strUrl);
    }

    /* set first index of POST header */
    hdrOffset = currentChar + 2;

    /* search header end */
    startChar = hdrOffset;
    currentChar = pInfoParse->in.lenMes;
    ret = httpParse_searchString((char *) (pInfoParse->in.strMes),
                                  GOAL_HTTP_HDREND_STR,
                                  &startChar, &currentChar);
    if (GOAL_RES_ERR(ret)) {
        return ret;
    }

    /* set header end char */
    hdrEndChar = startChar;
    /* set parsed length */
    pInfoParse->out.lenParsed = currentChar;
    /* check authentication required */
    ret = httpParse_checkAuth(pInst, pInfoParse->out.hdlRes, GOAL_HTTP_AUTH);
    if (GOAL_RES_ERR(ret)) {
        /* search authentication specifier string */
        startChar = hdrOffset;
        currentChar = hdrEndChar;
        ret = httpParse_searchString((char *) (pInfoParse->in.strMes),
                                      GOAL_HTTP_AUTH_BAS_STR,
                                      &startChar, &currentChar);
        if (GOAL_RES_ERR(ret)) {
            return GOAL_ERR_ACCESS;
        }
        /* get encoded authentication string */
        startChar = currentChar + 1;
        currentChar = hdrEndChar;
        ret = goal_httpSubStr(pInfoParse->in.strMes, pInfoParse->in.lenMes, startChar, &currentChar, '\r');
        if (GOAL_RES_OK(ret)) {
            /* set temp string */
            lenTemp = currentChar - startChar + 1;
            strTemp = (char *) &(pInfoParse->in.strMes[startChar]);
            /* check authentication */
            ret = goal_httpAuthBasCheck(pInst, pInfoParse->out.hdlRes, strTemp, lenTemp);
            if (GOAL_RES_ERR(ret)) {
                return GOAL_ERR_ACCESS;
            }
        }
    }

    /* check access rights */
    ret = httpParse_checkAccRights(pInst, pInfoParse->out.hdlRes,
                                    GOAL_HTTP_METHOD_ALLW_POST);
    if (GOAL_RES_ERR(ret)) {
       return GOAL_ERR_UNSUPPORTED;
    }

    /* search content length string */
    startChar = hdrOffset;
    currentChar = pInfoParse->in.lenMes;
    ret = httpParse_searchString((char *) (pInfoParse->in.strMes), "Content-Length:", &startChar, &currentChar);
    if (GOAL_RES_ERR(ret)) {
        return ret;
    }
    /* get content length string length */
    startChar = currentChar + 1;
    ret = goal_httpSubStr(pInfoParse->in.strMes, pInfoParse->in.lenMes, startChar, &currentChar, '\r');
    if (GOAL_RES_ERR(ret)) {
        return ret;
    }
    lenContStr = currentChar - startChar + 1;
    /* convert content length */
    reqFrag.lenContent = httpParse_convertDez(&(pInfoParse->in.strMes[startChar]), lenContStr);

    /* handle content length of 0 */
    if (0 == reqFrag.lenContent) {
        mimeType = GOAL_HTTP_POST_M_URLENC;
    }
    else {
        /* search MIME type */
        startChar = hdrOffset;
        currentChar = pInfoParse->in.lenMes;
        ret = httpParse_searchMimeType((pInfoParse->in.strMes), startChar, currentChar, &mimeType);
        if (GOAL_RES_ERR(ret)) {
            return ret;
        }
    }

    /* parse according to MIME type */
    switch (mimeType)
    {
    case GOAL_HTTP_POST_M_URLENC:
    case GOAL_HTTP_POST_M_JSON:
        /* search header end */
        currentChar = pInfoParse->in.lenMes;
        ret = httpParse_searchString((char *) (pInfoParse->in.strMes), GOAL_HTTP_HDREND_STR, &startChar, &currentChar);
        if (GOAL_RES_ERR(ret)) {
            return ret;
        }
        /* calculate remaining data length */
        reqFrag.lenRemain = reqFrag.lenContent - (pInfoParse->in.lenMes - currentChar);
        if (0 == reqFrag.lenRemain) {
            goal_logDbg("Oneshot POST calculated");
            /* only one data chunk in this POST request, no fragmentation */
            reqFrag.flag = GOAL_FALSE;
            /* data pointer and length */
            pInfoParse->out.lenData = reqFrag.lenContent - reqFrag.lenHead - reqFrag.lenFoot;
            pInfoParse->out.pData = &(pInfoParse->in.strMes[currentChar]);
        } else {
            /* set fragmentation info */
            reqFrag.flag = GOAL_TRUE;
            reqFrag.frstBound = GOAL_TRUE;
            reqFrag.pfnAppCb = pInfoParse->out.pfnCb;
            reqFrag.hdlRes = pInfoParse->out.hdlRes;
            reqFrag.lenRemain = reqFrag.lenContent;
            /* store info for later use */
            ret = goal_httpChnSetReqInfo(pInst, pInfoParse->in.pHttpChan, &reqFrag);
            if (GOAL_RES_OK(ret)) {
                ret = GOAL_ERR_DELAYED;
            }
            return ret;
        }
        /* set forwarding active */
        ret = goal_httpChnState_set(pInst, pInfoParse->in.pHttpChan, GOAL_HTTP_CHNFLAG_FORWARD);
        if (GOAL_RES_ERR(ret)) {
            return ret;
        }
        /* set channel information */
        ret = goal_httpChnSetReqInfo(pInst, pInfoParse->in.pHttpChan, &reqFrag);
        break;

    case GOAL_HTTP_POST_M_MULTIPART:
        /* search boundary string */
        startChar = hdrOffset;
        currentChar = pInfoParse->in.lenMes;
        ret = httpParse_searchString((char *) (pInfoParse->in.strMes), "boundary=", &startChar, &currentChar);
        if (GOAL_RES_ERR(ret)) {
            return ret;
        }
        /* get boundary string length */
        startChar = currentChar;
        ret = goal_httpSubStr(pInfoParse->in.strMes, pInfoParse->in.lenMes, startChar, &currentChar, '\r');
        if (GOAL_RES_ERR(ret)) {
            return ret;
        }
        lenBound = currentChar - startChar + 1;
        /* reset boundary string */
        GOAL_MEMSET(reqFrag.boundary, 0, HTTP_POST_MULTIP_BOUND);
        /* store boundary string */
        GOAL_MEMCPY(reqFrag.boundary, &(pInfoParse->in.strMes[startChar]), lenBound);

        /* expect string found ? */
        startChar = hdrOffset;
        currentChar = pInfoParse->in.lenMes;
        ret = httpParse_searchString((char *) (pInfoParse->in.strMes), "Expect: 100", &startChar, &currentChar);
        if (GOAL_RES_OK(ret)) {
            /* set remaining data length */
            /* set fragmentation info */
            reqFrag.flag = GOAL_TRUE;
            reqFrag.lenRemain = reqFrag.lenContent;
            reqFrag.pfnAppCb = pInfoParse->out.pfnCb;
            reqFrag.hdlRes = pInfoParse->out.hdlRes;
            /* store info for later use */
            goal_httpChnSetReqInfo(pInst, pInfoParse->in.pHttpChan, &reqFrag);
            return GOAL_ERR_DELAYED;
        }

        /* search header end */
        startChar = hdrOffset;
        currentChar = pInfoParse->in.lenMes;
        ret = httpParse_searchString((char *) (pInfoParse->in.strMes), GOAL_HTTP_HDREND_STR, &startChar, &currentChar);
        if (GOAL_RES_ERR(ret)) {
            return ret;
        }

        /* check if message ends after header end */
        if (currentChar >= pInfoParse->in.lenMes) {
            /* no data in this frame, expect later */
            /* set fragmentation info */
            reqFrag.flag = GOAL_TRUE;
            reqFrag.lenRemain = reqFrag.lenContent;
            reqFrag.pfnAppCb = pInfoParse->out.pfnCb;
            reqFrag.hdlRes = pInfoParse->out.hdlRes;
            /* store info for later use */
            goal_httpChnSetReqInfo(pInst, pInfoParse->in.pHttpChan, &reqFrag);
        } else {
            /* find first boundary */
            currentChar = pInfoParse->in.lenMes;
            ret = httpParse_searchString((char *) (pInfoParse->in.strMes), (char *) (reqFrag.boundary), &startChar, &currentChar);
            if (GOAL_RES_ERR(ret)) {
                /* first boundary does not exist, expect later */
                /* set fragmentation info */
                reqFrag.flag = GOAL_TRUE;
                reqFrag.lenRemain = reqFrag.lenContent;
                reqFrag.pfnAppCb = pInfoParse->out.pfnCb;
                reqFrag.hdlRes = pInfoParse->out.hdlRes;
                /* store info for later use */
                ret = goal_httpChnSetReqInfo(pInst, pInfoParse->in.pHttpChan, &reqFrag);
                return ret;
            }
            /* set request fragment flag */
            reqFrag.frstBound = GOAL_TRUE;

            /* set MIME header start index to first boundary: 2 '-' are added to boundary in MIME header */
            startHead = startChar - 2;

            /* find end of MIME header */
            currentChar = pInfoParse->in.lenMes;
            ret = httpParse_searchString((char *) (pInfoParse->in.strMes), GOAL_HTTP_HDREND_STR, &startChar, &currentChar);
            if (GOAL_RES_ERR(ret)) {
                /* header end not found, suspend MIME header parsing */
                reqFrag.flag = GOAL_TRUE;
                reqFrag.frstBound = GOAL_FALSE;
                reqFrag.lenRemain = reqFrag.lenContent;
                reqFrag.pfnAppCb = pInfoParse->out.pfnCb;
                reqFrag.hdlRes = pInfoParse->out.hdlRes;
                /* data pointer and length */
                pInfoParse->out.lenData -= startHead;
                pInfoParse->out.pData = &(pInfoParse->in.strMes[startHead]);
                /* set channel information */
                ret = goal_httpChnSetReqInfo(pInst, pInfoParse->in.pHttpChan, &reqFrag);
                return ret;
            }

            /* set parsed length */
            pInfoParse->out.lenParsed = pInfoParse->in.lenMes;

            /* set header and footer length */
            reqFrag.lenHead = currentChar - startHead;
            reqFrag.lenFoot = (uint32_t) GOAL_STRLEN((char *) (reqFrag.boundary)) + 8;

            /* calculate remaining data length */
            reqFrag.lenRemain = reqFrag.lenContent - (pInfoParse->in.lenMes - startHead);
            if (0 == reqFrag.lenRemain) {
                goal_logDbg("Oneshot POST calculated");
                /* only one data chunk in this POST request, no fragmentation */
                reqFrag.flag = GOAL_FALSE;
                /* data pointer and length */
                pInfoParse->out.lenData = reqFrag.lenContent - reqFrag.lenHead - reqFrag.lenFoot;
                pInfoParse->out.pData = &(pInfoParse->in.strMes[currentChar]);
            } else {
                goal_logDbg("Multipart POST calculated");
                /* POST request consists of more than one data chunk */
                reqFrag.flag = GOAL_TRUE;
                reqFrag.pfnAppCb = pInfoParse->out.pfnCb;
                reqFrag.hdlRes = pInfoParse->out.hdlRes;
                /* data pointer and length */
                pInfoParse->out.lenData = pInfoParse->in.lenMes - startHead - reqFrag.lenHead;
                pInfoParse->out.pData = &(pInfoParse->in.strMes[currentChar]);
            }

            /* set forwarding active */
            ret = goal_httpChnState_set(pInst, pInfoParse->in.pHttpChan, GOAL_HTTP_CHNFLAG_FORWARD);
            if (GOAL_RES_ERR(ret)) {
                return ret;
            }

            /* set channel information */
            ret = goal_httpChnSetReqInfo(pInst, pInfoParse->in.pHttpChan, &reqFrag);
        }
        break;

    default:
        break;
    }

    return ret;
}


/****************************************************************************/
/** This function searches the MIME type of a POST request and returns Id
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T httpParse_searchMimeType(
    uint8_t *str,                               /**< pointer to message */
    uint32_t startChar,                         /**< offset start character */
    uint32_t endChar,                           /**< offset end character */
    GOAL_HTTP_POST_MIME_T *pMime                /**< [out] pointer to mime type */
)
{
    GOAL_STATUS_T ret = GOAL_OK;                /* return value */
    uint32_t start = 0;                         /* start character */
    uint32_t end = 0;                           /* end character */

    if (NULL == str) {
        /* error */
        return GOAL_ERR_NULL_POINTER;
    }

    /* init mime type return */
    *pMime = GOAL_HTTP_POST_M_NO_SUPP;

    /* search MIME type = multipart/form-data */
    start = startChar;
    end = endChar;
    ret = httpParse_searchString((char *) str, "Content-Type: multipart/form-data", &start, &end);
    if (GOAL_RES_OK(ret)) {
        /* set mime type and return value */
        *pMime = GOAL_HTTP_POST_M_MULTIPART;
        return GOAL_OK;
    }

    /* search MIME type = multipart/form-data */
    start = startChar;
    end = endChar;
    ret = httpParse_searchString((char *) str, "Content-type: multipart/form-data", &start, &end);
    if (GOAL_RES_OK(ret)) {
        /* set mime type and return value */
        *pMime = GOAL_HTTP_POST_M_MULTIPART;
        return GOAL_OK;
    }

    /* search MIME type = application/x-www-form-urlencoded */
    start = startChar;
    end = endChar;
    ret = httpParse_searchString((char *) str, "Content-Type: application/x-www-form-urlencoded", &start, &end);
    if (GOAL_RES_OK(ret)) {
        /* set mime type and return value */
        *pMime = GOAL_HTTP_POST_M_URLENC;
        return GOAL_OK;
    }

    /* search MIME type = application/x-www-form-urlencoded */
    start = startChar;
    end = endChar;
    ret = httpParse_searchString((char *) str, "Content-type: application/x-www-form-urlencoded", &start, &end);
    if (GOAL_RES_OK(ret)) {
        /* set mime type and return value */
        *pMime = GOAL_HTTP_POST_M_URLENC;
        return GOAL_OK;
    }

    /* search MIME type = application/json */
    start = startChar;
    end = endChar;
    ret = httpParse_searchString((char *) str, "Content-Type: application/json", &start, &end);
    if (GOAL_RES_OK(ret)) {
        /* set mime type and return value */
        *pMime = GOAL_HTTP_POST_M_JSON;
        return GOAL_OK;
    }

    /* search MIME type = application/json */
    start = startChar;
    end = endChar;
    ret = httpParse_searchString((char *) str, "Content-Type: application/json", &start, &end);
    if (GOAL_RES_OK(ret)) {
        /* set mime type and return value */
        *pMime = GOAL_HTTP_POST_M_JSON;
        return GOAL_OK;
    }

    return GOAL_ERR_NOT_FOUND;
}


/****************************************************************************/
/** This function converts a string into a decimal.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static uint32_t httpParse_convertDez(
    uint8_t     *str,                           /**< string to convert */
    uint32_t    lenStr                          /**< string length */
)
{
    uint32_t cnt1;                              /* loop counter */
    uint32_t cnt2;                              /* loop counter */
    uint32_t tmp = 1;                           /* temporarily used for calculating */
    uint32_t ret = 0;                           /* final return */

    for (cnt1 = 0; cnt1 < lenStr; cnt1++) {
        tmp = 1;
        for (cnt2 = 0; cnt2 < cnt1; cnt2++) {
            tmp *= 10;
        }
        ret += (uint32_t) ((str[lenStr-cnt1-1] - (uint8_t) '0') * tmp);
    }
    return ret;
}


/****************************************************************************/
/** This function searches a string within a message.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 *
 * The string is searched within the given start and end border values.
 * When the function returns GOAL_OK, the string has been found and
 * start and end variables contain the offset of the string in the message.
 */
static GOAL_STATUS_T httpParse_searchString(
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


/****************************************************************************/
/** This function parses a received GET request.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T httpParse_ParseGet(
    GOAL_HTTP_T         *pInst,                 /**< instance pointer */
    GOAL_HTTP_PARSE_T   *pInfoParse,            /**< pointer to parse info struct */
    uint32_t            methOffset              /**< method string offset */
)
{
    GOAL_STATUS_T           ret = GOAL_ERROR;   /* return value */
    GOAL_HTTP_REQFRAG_T     reqFrag;            /* request fragmentation info */
    uint32_t                startChar;          /* start character */
    uint32_t                currentChar;        /* current character */
    uint32_t                hdrEndChar;         /* header end character */
    char                    *strUrl;            /* URL string */
    char                    *strTemp;           /* temp string */
    uint32_t                lenUrl;             /* length of URL string */
    uint32_t                lenTemp;            /* length of temporary string */

    /* find second substring = resource URL */
    startChar = methOffset + 2;
    ret = goal_httpSubStr(pInfoParse->in.strMes, pInfoParse->in.lenMes, startChar, &currentChar, ' ');
    if (GOAL_RES_ERR(ret)) {
        return ret;
    }

    /* special case handling: change default "/ " to default path */
    lenUrl = currentChar - startChar + 1;
    strUrl = (char *) (pInfoParse->in.strMes + startChar);
    if ((1 == lenUrl) && (strUrl[0] == '/')) {
        /* client requests index page */
        strUrl = GOAL_HTTP_STANDARD_INDEX_NAME;
        lenUrl = GOAL_HTTP_STANDARD_INDEX_NAME_LEN;
    } else {
        strUrl[lenUrl] = 0;
    }

    /* log info */
    goal_logInfo("GET URL requested: %s", strUrl);

    /* determine resource and destination */
    ret = httpParse_ParseDestination(pInst, (uint8_t *) strUrl, lenUrl, pInfoParse);
    if (GOAL_RES_ERR(ret)) {
       return ret;
    }

    /* search header end */
    startChar = currentChar;
    currentChar = pInfoParse->in.lenMes;
    ret = httpParse_searchString((char *) (pInfoParse->in.strMes),
                                  GOAL_HTTP_HDREND_STR,
                                  &startChar, &currentChar);
    if (GOAL_RES_ERR(ret)) {
        return ret;
    }

    /* set parsed length */
    pInfoParse->out.lenParsed = pInfoParse->in.lenMes;

    /* set header end char */
    hdrEndChar = startChar;
    /* check authentication required */
    ret = httpParse_checkAuth(pInst, pInfoParse->out.hdlRes, GOAL_HTTP_AUTH);
    if (GOAL_RES_ERR(ret)) {
        /* search authentication specifier string */
        startChar = methOffset + lenUrl;
        currentChar = hdrEndChar;
        ret = httpParse_searchString((char *) (pInfoParse->in.strMes),
                                      GOAL_HTTP_AUTH_BAS_STR,
                                      &startChar, &currentChar);
        if (GOAL_RES_ERR(ret)) {
            return GOAL_ERR_ACCESS;
        }
        /* get encoded authentication string */
        startChar = currentChar + 1;
        currentChar = hdrEndChar;
        ret = goal_httpSubStr(pInfoParse->in.strMes, pInfoParse->in.lenMes, startChar, &currentChar, '\r');
        if (GOAL_RES_OK(ret)) {
            /* set temp string */
            lenTemp = currentChar - startChar + 1;
            strTemp = (char *) &(pInfoParse->in.strMes[startChar]);
            /* check authentication */
            ret = goal_httpAuthBasCheck(pInst, pInfoParse->out.hdlRes, strTemp, lenTemp);
            if (GOAL_RES_ERR(ret)) {
                return GOAL_ERR_ACCESS;
            }
        }
    }

    /* check access rights */
    ret = httpParse_checkAccRights(pInst, pInfoParse->out.hdlRes,
                                    GOAL_HTTP_METHOD_ALLW_GET);
    if (GOAL_RES_ERR(ret)) {
       return GOAL_ERR_UNSUPPORTED;
    }

    /* set request fragmentation information */
    reqFrag.flag = GOAL_FALSE;
    reqFrag.pfnAppCb = pInfoParse->out.pfnCb;
    reqFrag.hdlRes = pInfoParse->out.hdlRes;
    ret = goal_httpChnSetReqInfo(pInst, pInfoParse->in.pHttpChan, &reqFrag);
    if (GOAL_RES_ERR(ret)) {
        return ret;
    }

    /* set forwarding flag */
    ret = goal_httpChnState_set(pInst, pInfoParse->in.pHttpChan, GOAL_HTTP_CHNFLAG_FORWARD);

    return ret;
}


/****************************************************************************/
/** This function checks the access rigthts for a resource
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T httpParse_checkAuth(
    GOAL_HTTP_T         *pInst,                 /**< instance pointer */
    GOAL_HTTP_HDL_T     hdlRes,                 /**< resource handle */
    uint32_t            authReq                 /**< authentication required */
)
{
    GOAL_STATUS_T ret = GOAL_ERROR;             /* return value */
    uint32_t auth = 0;                          /* authentication */

    /* get allowed access */
    ret = goal_httpGetResAcc(pInst, hdlRes, &auth);
    if (GOAL_RES_ERR(ret)) {
        return ret;
    }

    /* check access */
    if (auth & authReq) {
        /* not allowed */
        return GOAL_ERR_ACCESS;
    }

    return ret;
}


/****************************************************************************/
/** This function checks the access rigthts for a resource
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T httpParse_checkAccRights(
    GOAL_HTTP_T         *pInst,                 /**< instance pointer */
    GOAL_HTTP_HDL_T     hdlRes,                 /**< resource handle */
    uint32_t            access                  /**< desired access */
)
{
    GOAL_STATUS_T ret = GOAL_ERROR;             /* return value */
    uint32_t allowAcc = 0;                      /* allowed access */

    /* get allowed access */
    ret = goal_httpGetResAcc(pInst, hdlRes, &allowAcc);
    if (GOAL_RES_ERR(ret)) {
        return ret;
    }

    /* check access */
    if (0 == (access & allowAcc)) {
        /* not allowed */
        return GOAL_ERR_ACCESS;
    }

    return ret;
}


/****************************************************************************/
/** This function checks the input information for http resource manager
 *  parser.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T httpParse_ParseDestination(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    uint8_t     *pPath,                         /**< resource path */
    uint32_t    lenPath,                        /**< length of path */
    GOAL_HTTP_PARSE_T *pInfoParse               /**< pointer to parse info struct */
)
{
    GOAL_STATUS_T ret = GOAL_OK;                /* return value */
    uint32_t      lenMatch;                     /* match length */
    HTTP_RESMGR_LL_RES_T *pRes;                 /* resource pointer */

    /* search path and resource name */
    ret = goal_httpResMgrSearch(pInst, pPath, lenPath, &pRes, &lenMatch);
    if (GOAL_RES_ERR(ret)) {
        /* not found */
        return GOAL_ERR_NODATA;
    }

    /* set parser info */
    pInfoParse->out.strUrl = (char *) pPath;
    pInfoParse->out.lenUrl = lenPath;
    pInfoParse->out.lenMatch = lenMatch;

    /* get resource info */
    pInfoParse->out.hdlRes = pRes->hdlRes;
    pInfoParse->out.pfnCb = (void *) (pRes->pfnCbData);

    return ret;
}


/****************************************************************************/
/** This function checks the input information for http resource manager
 *  parser.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T httpParse_ParseInfoCheck(
    GOAL_HTTP_PARSE_T *pInfoParse               /**< pointer to parser information struct */
)
{
    GOAL_STATUS_T ret = GOAL_OK;                /* return value */

    /* check input string pointer */
    if (NULL == pInfoParse->in.strMes) {
        return GOAL_ERR_NULL_POINTER;
    }
    /* check string length */
    if (0 == pInfoParse->in.strMes) {
        return GOAL_ERROR;
    }
    /* check input pointer to method table */
    if (NULL == pInfoParse->in.pMethTable) {
        return GOAL_ERR_NULL_POINTER;
    }
    /* check method table length */
    if (0 == pInfoParse->in.lenMethTable) {
        return GOAL_ERROR;
    }
    /* check input pointer to method table */
    if (NULL == pInfoParse->in.pFileExtTable) {
        return GOAL_ERR_NULL_POINTER;
    }
    /* check method table length */
    if (0 == pInfoParse->in.lenFileExtTable) {
        return GOAL_ERROR;
    }


    return ret;
}


/****************************************************************************/
/** Get next substring of http request.
 *
 * This functions looks for the next divider symbol in the given message
 * after the start character and returns the number of the last character
 * via pointer. If no ' ' is found, the very last character of the message
 * is returned.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_httpSubStr(
    uint8_t *strMes,                            /**< string of message to parse */
    uint32_t lenMes,                            /**< length of message */
    uint32_t startChar,                         /**< number of char where substring starts */
    uint32_t *pEndChar,                         /**< pointer to number of char, where substring ends */
    char symbol                                 /**< divider symbol */
)
{
    uint32_t cnt;                               /* count */

    if (lenMes <= startChar) {
        return GOAL_ERROR;
    }

    for (cnt = startChar; cnt < lenMes; cnt++) {
        if (strMes[cnt] == symbol) {
            *pEndChar = cnt-1;
            return GOAL_OK;
        }
    }
    *pEndChar = cnt;
    return GOAL_OK;
}

