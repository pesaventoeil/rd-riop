/** @file
 *
 * @brief Implementation of the Goal http template manager
 *
 * Implementation of the goal webserver template manager.
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
#include "goal_http_tmpmgr.h"
#include "goal_http_resmgr.h"
#include "cm/goal_http_cm.h"

#include "goal_cm.h"


/****************************************************************************/
/* Local defines */
/****************************************************************************/


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/

static GOAL_STATUS_T http_tmpMgrReplace(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_BUFFER_T *pBuf,                        /**< goal buffer */
    GOAL_HTTP_HDL_T hdlRes,                     /**< resource handle */
    const uint8_t *pData,                       /**< replacement data pointer */
    uint32_t offset,                            /**< data pointert offset */
    uint32_t lenTag,                            /**< length tag */
    GOAL_BOOL_T *pBool,                         /**< length bool pointer */
    uint32_t *pRetOff                           /**< return offset pointer */
);

static GOAL_STATUS_T http_tmpMgrSearchListPtr(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    uint32_t listId,                            /**< list identifier */
    GOAL_HTTP_TMPMGR_LL_T **pList               /**< return pointer */
);

static GOAL_STATUS_T http_tmpMgrSearchListId(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    char* strName,                              /**< list name string */
    uint32_t *pListId                           /**< return pointer */
);

static GOAL_STATUS_T http_tmpMgrReplaceListItem(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_BUFFER_T *pBuf,                        /**< pointer to message buffer */
    GOAL_HTTP_HDL_T hdlRes,                     /**< resource handle */
    char* varName,                              /**< variable name */
    GOAL_BOOL_T *pBool                          /**< pointer to length bool */
);

static GOAL_STATUS_T http_tmpMgrReplaceVar(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_BUFFER_T *pBuf,                        /**< pointer to message buffer */
    GOAL_HTTP_HDL_T hdlRes,                     /**< resource handle */
    char* varName,                              /**< variable name */
    GOAL_BOOL_T *pBool                          /**< pointer to length bool */
);

static GOAL_STATUS_T http_tmpMgrReplaceCm(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_BUFFER_T *pBuf,                        /**< pointer to message buffer */
    uint32_t modId,                             /**< module id */
    uint32_t varId,                             /**< variable id */
    GOAL_BOOL_T *pBool                          /**< pointer to length bool */
);

static GOAL_STATUS_T http_tmpMgrStringify(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_CM_VAR_T *pVar,                        /**< pointer to CM variable */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_BUFFER_T *pBuf,                        /**< pointer to message buffer */
    uint32_t lenMax,                            /**< maximum string length */
    GOAL_BOOL_T *pBool                          /**< return bool */
);

static GOAL_STATUS_T http_convertIP(
    uint32_t val,                               /**< IP address value */
    char* pRet                                  /**< return string */
);

static GOAL_BOOL_T http_tmpMgrFindTok(
    char tok,                                   /**< token */
    const uint8_t *pBuf,                        /**< data buffer */
    uint32_t lenBuf,                            /**< data buffer length */
    uint32_t *pOff                              /**< return token offset */
);


/****************************************************************************/
/* Local variables */
/****************************************************************************/


/****************************************************************************/
/** Add a list for template replacement
 *
 * @returns GOAL_OK on success
 * @returns error code otherwise
 *
 */
GOAL_STATUS_T goal_httpTmpMgrNewListImpl(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_TEMPLATE_LIST_INIT_T *pInit       /**< pointer to list init data */
)
{
    GOAL_STATUS_T ret = GOAL_OK;                /* return value */
    GOAL_HTTP_TMPMGR_LL_T *pElem = NULL;        /* element pointer */
    GOAL_HTTP_TMPMGR_LL_T **ppList = NULL;      /* pointer to template list */

    if (GOAL_RES_OK(ret)) {
        /* allocate memory for list entry */
        ret = goal_memCalloc(&pElem, sizeof(GOAL_HTTP_TMPMGR_LL_T));
        /* set list pointer */
        ppList = &(pInst->pTemplateList);
    }

    if (GOAL_RES_OK(ret)) {
        /* append element to list */
        GOAL_LL_APPEND(*ppList, pElem);
        /* set element data */
        pElem->listId = pInit->listId;
        pElem->cntMemb = pInit->cntMemb;
        GOAL_MEMCPY(pElem->listName, pInit->listName, GOAL_STRLEN(pInit->listName));
    }

    return ret;
}


/****************************************************************************/
/** Parses a given buffer for replaceable templates
 *
 * @returns GOAL_OK on success
 * @returns error code otherwise
 *
 */
GOAL_STATUS_T goal_httpTmpMgrParse(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_BUFFER_T *pBuf,                        /**< goal buffer */
    GOAL_HTTP_HDL_T hdlRes,                     /**< resource handle */
    const uint8_t *pData,                       /**< data pointer */
    uint32_t lenData,                           /**< data length */
    uint32_t bufOffset                          /**< buffer offset */
)
{
    GOAL_STATUS_T ret = GOAL_OK;                /* return value */
    uint32_t offStart = 0;                      /* start offset of tag */
    uint32_t offEnd = 0  ;                      /* end offset of tag */
    GOAL_BOOL_T retBool = GOAL_FALSE;           /* return bool */
    uint32_t offset = bufOffset;                /* data buffer offset */
    GOAL_BOOL_T lenBool = GOAL_TRUE;            /* length bool */
    GOAL_HTTP_RESFRAG_T resFrag;                /* response fragmentation info */
    uint32_t retOff = 0;                        /* return offset */

    /* check pointers */
    if (NULL == pBuf) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* search buffer */
    do {
        /* find tag open token */
        retBool = http_tmpMgrFindTok(GOAL_HTTP_TMPMGR_TOKEN_OPEN, &(pData[offset]), (lenData - offset), &offStart);
        if (GOAL_FALSE == retBool) {
            /* no tag found, copy and send all */
            ret = goal_httpAddToBuffer(pInst, pBuf, pHttpChan, &(pData[offset]), (lenData - offset));
        } else {
            /* find tag close token */
            retBool = http_tmpMgrFindTok(GOAL_HTTP_TMPMGR_TOKEN_CLOSE, &(pData[offset + offStart]), (lenData - offset), &offEnd);
            if (GOAL_TRUE == retBool) {
                offEnd += offStart;
                /* open and close found -> check for double open token */
                if (GOAL_HTTP_TMPMGR_TOKEN_OPEN == pData[offset + offStart + 1]) {
                    /* double end token needed */
                    if (GOAL_HTTP_TMPMGR_TOKEN_CLOSE != pData[offset + offEnd + 1]) {
                        /* error */
                        ret = GOAL_ERROR;
                    } else {
                        /* double end token found, copy until start token */
                        ret = goal_httpAddToBuffer(pInst, pBuf, pHttpChan, &(pData[offset]), (offStart - offset));
                        if (GOAL_RES_OK(ret)) {
                            offStart++;
                            /* copy span to buffer */
                            ret = goal_httpAddToBuffer(pInst, pBuf, pHttpChan, &(pData[offset + offStart]), (offEnd - offStart + 1));
                        }
                        if (GOAL_RES_OK(ret)) {
                            /* skip end token */
                            offEnd++;
                        }
                    }
                } else {
                    /* copy until replacement */
                    ret = goal_httpAddToBuffer(pInst, pBuf, pHttpChan, &(pData[offset]), offStart);
                    if (GOAL_RES_OK(ret)) {
                        /* single token only, needs replacement */
                        ret = http_tmpMgrReplace(pInst, pHttpChan, pBuf, hdlRes, pData, (offset + offStart), (offEnd - offStart), &lenBool, &retOff);
                    }
                }
                /* check returned offset and length bool */
                if (retOff) {
                    /* use returned offset */
                    offset = retOff;
                } else if (GOAL_TRUE == lenBool) {
                    /* udpate offset */
                    offset += offEnd + 1;
                }
            }
        }
    } while ((GOAL_TRUE == retBool) && (GOAL_OK == ret) && (GOAL_TRUE == lenBool));

    if (GOAL_FALSE == lenBool) {
        /* set fragmentetation info */
        resFrag.flag = GOAL_TRUE;
        resFrag.lenData = lenData;
        resFrag.pData = pData;
        resFrag.srcOff = offset;
        ret = goal_httpChnSetResInfo(pInst, pHttpChan, pBuf, &resFrag);
    }

    return ret;
}


/****************************************************************************/
/** Replaces a template tag
 *
 * @returns GOAL_OK on success
 * @returns error code otherwise
 *
 */
static GOAL_STATUS_T http_tmpMgrReplace(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_BUFFER_T *pBuf,                        /**< goal buffer */
    GOAL_HTTP_HDL_T hdlRes,                     /**< resource handle */
    const uint8_t *pData,                       /**< replacement data pointer */
    uint32_t offset,                            /**< data pointert offset */
    uint32_t lenTag,                            /**< length tag */
    GOAL_BOOL_T *pBool,                         /**< length bool pointer */
    uint32_t *pRetOff                           /**< return offset pointer */
)
{
    GOAL_STATUS_T ret = GOAL_OK;                /* return value */
    uint32_t modId = 0;                         /* module id */
    uint32_t varId = 0;                         /* variable id */
    uint32_t cntVar = 0;                        /* variable count */
    char varName[32];                           /* variable name */
    char listName[32];                          /* list name */
    const uint8_t* pTag;                        /* pointer to tag */
    GOAL_HTTP_TMPMGR_LL_T *pList;               /* pointer to list */
    GOAL_HTTP_TMPMGR_L_PATH_T *pTemplPath;      /* pointer to template path */

    UNUSEDARG(lenTag);

    /* get pointer to template path */
    pTemplPath = (GOAL_HTTP_TMPMGR_L_PATH_T *) pInst->pTemplatePath;
    if (NULL == pTemplPath) {
        /* error */
        return GOAL_ERR_NULL_POINTER;
    }

    /* set return offset */
    *pRetOff = 0;
    /* set tag pointer */
    pTag = pData + offset;

    /* scan tag */
    cntVar = (uint32_t) (GOAL_SSCANF((const char *) pTag, "[CM:%"FMT_u32", %"FMT_u32"]", &modId, &varId));
    if (2 == cntVar) {
        /* replace CM variable */
        ret = http_tmpMgrReplaceCm(pInst, pHttpChan, pBuf, modId, varId, pBool);
    } else if (1 == GOAL_SSCANF((const char *) pTag,"[VAR:%s]", varName)) {
        if (0 == pTemplPath->pathLen) {
            /* replace application variable */
            ret = http_tmpMgrReplaceVar(pInst, pHttpChan, pBuf, hdlRes, varName, pBool);
        } else {
            /* replace list variable */
            ret = http_tmpMgrReplaceListItem(pInst, pHttpChan, pBuf, hdlRes, varName, pBool);
        }
    } else if (1 == GOAL_SSCANF((const char *) pTag,"[FOREACH:%s]", listName)) {
        /* ENTER LIST: search list id */
        ret = http_tmpMgrSearchListId(pInst, listName, &(pTemplPath->path[(pTemplPath->pathLen)].listId));
        if (GOAL_RES_OK(ret)) {
            /* set offset of the starting point of this list */
            pTemplPath->path[(pTemplPath->pathLen)].offset = offset + lenTag + 1;
            /* increase path length */
            pTemplPath->pathLen++;
        }
    }
    else if ((0 < pTemplPath->pathLen)
    && (0 == GOAL_MEMCMP((const char *) pTag, "[/FOREACH]", GOAL_STRLEN("[/FOREACH]")))) {
        /* increase list index */
        pTemplPath->path[(pTemplPath->pathLen-1)].index++;
        /* search list by id */
        ret = http_tmpMgrSearchListPtr(pInst, pTemplPath->path[(pTemplPath->pathLen-1)].listId, &pList);
        if (GOAL_RES_OK(ret)) {
            if (pTemplPath->path[(pTemplPath->pathLen-1)].index < (pList->cntMemb)) {
                /* set return offset */
                *pRetOff = pTemplPath->path[(pTemplPath->pathLen-1)].offset;
            } else {
                /* LEAVE LIST: reset path entry */
                pTemplPath->path[(pTemplPath->pathLen-1)].index = 0;
                pTemplPath->path[(pTemplPath->pathLen-1)].listId = 0;
                pTemplPath->path[(pTemplPath->pathLen-1)].offset = 0;
                /* decrease path length */
                pTemplPath->pathLen--;
            }
        }
    }

    return ret;
}


/****************************************************************************/
/** Returns list pointer
 *
 * @returns GOAL_OK on success
 * @returns error code otherwise
 *
 */
static GOAL_STATUS_T http_tmpMgrSearchListPtr(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    uint32_t listId,                            /**< list identifier */
    GOAL_HTTP_TMPMGR_LL_T **pList               /**< return pointer */
)
{
    GOAL_STATUS_T ret = GOAL_ERR_NOT_FOUND;     /* return value */
    GOAL_HTTP_TMPMGR_LL_T *pStart;              /* start of list */

    /* set start of list */
    pStart = (pInst->pTemplateList);

    GOAL_LL_FOREACH(pStart, *pList) {
        if (listId == (*pList)->listId) {
            ret = GOAL_OK;
            break;
        }
    }
    return ret;
}


/****************************************************************************/
/** Returns list identifier
 *
 * @returns GOAL_OK on success
 * @returns error code otherwise
 *
 */
static GOAL_STATUS_T http_tmpMgrSearchListId(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    char* strName,                              /**< list name string */
    uint32_t *pListId                           /**< return pointer */
)
{
    GOAL_STATUS_T ret = GOAL_ERR_NOT_FOUND;     /* return value */
    GOAL_HTTP_TMPMGR_LL_T *pStart;              /* start of list */
    GOAL_HTTP_TMPMGR_LL_T *pElem;               /* recent list element */

    /* set start of list */
    pStart = (pInst->pTemplateList);

    GOAL_LL_FOREACH(pStart, pElem) {
        if (0 == GOAL_MEMCMP(strName, pElem->listName, GOAL_STRLEN(pElem->listName))) {
            *pListId = pElem->listId;
            ret = GOAL_OK;
            break;
        }
    }
    return ret;
}


/****************************************************************************/
/** Replaces a template tag with list handling.
 *
 * @returns GOAL_OK on success
 * @returns error code otherwise
 *
 */
static GOAL_STATUS_T http_tmpMgrReplaceListItem(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_BUFFER_T *pBuf,                        /**< pointer to message buffer */
    GOAL_HTTP_HDL_T hdlRes,                     /**< resource handle */
    char* varName,                              /**< variable name */
    GOAL_BOOL_T *pBool                          /**< pointer to length bool */
)
{
    GOAL_STATUS_T ret = GOAL_OK;                /* return value */
    uint32_t newLen = 0;                        /* new length */
    GOAL_HTTP_TMPCB_T pfnCallback = NULL;       /* callback function pointer */
    GOAL_HTTP_APPLCB_TEMPL_T cbInfo;            /* callback information struct */

    if (GOAL_RES_OK(ret)) {
        /* init buffer */
        GOAL_MEMSET(pHttpChan->templateBuffer, 0, HTTP_CHN_TEMPL_BUFLEN);

        /* set callback information struct */
        GOAL_MEMSET(&cbInfo, 0, sizeof(GOAL_HTTP_APPLCB_TEMPL_T));
        cbInfo.pInst = pInst;
        cbInfo.hdlRes = hdlRes;
        cbInfo.in.pPath = (GOAL_HTTP_TMPMGR_L_PATH_T *) pInst->pTemplatePath;
        cbInfo.in.name = varName;
        cbInfo.in.retLenMax = HTTP_CHN_TEMPL_BUFLEN;
        cbInfo.out.strReturn = (char *) (pHttpChan->templateBuffer);
    }

    /* get callback function pointer */
    ret = goal_httpResGetTemplCb(pInst, hdlRes, &pfnCallback);
    if (GOAL_RES_OK(ret)) {
        ret = (*pfnCallback)(&cbInfo);
    }

    if (GOAL_RES_ERR(ret)) {
        /* set error string */
        GOAL_MEMCPY(pHttpChan->templateBuffer, GOAL_HTTP_TMPMGR_ERR_STRING, GOAL_STRLEN(GOAL_HTTP_TMPMGR_ERR_STRING));
    }

    /* check length */
    newLen = (uint32_t) (GOAL_STRLEN((char *) (pHttpChan->templateBuffer)));
    if (newLen > (uint32_t) (pBuf->bufSize - pBuf->dataLen)) {
        /* bad length */
        *pBool = GOAL_FALSE;
    } else {
        /* copy text into buffer */
        ret = goal_httpAddToBuffer(pInst, pBuf, pHttpChan, pHttpChan->templateBuffer, newLen);
    }

    return ret;
}


/****************************************************************************/
/** Replaces a template tag directly with a CM variable.
 *
 * @returns GOAL_OK on success
 * @returns error code otherwise
 *
 */
static GOAL_STATUS_T http_tmpMgrReplaceVar(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_BUFFER_T *pBuf,                        /**< pointer to message buffer */
    GOAL_HTTP_HDL_T hdlRes,                     /**< resource handle */
    char* varName,                              /**< variable name */
    GOAL_BOOL_T *pBool                          /**< pointer to length bool */
)
{
    GOAL_STATUS_T ret = GOAL_OK;                /* return value */
    uint32_t newLen = 0;                        /* new length */
    GOAL_HTTP_TMPCB_T pfnCallback = NULL;       /* callback function pointer */
    GOAL_HTTP_APPLCB_TEMPL_T cbInfo;            /* callback info struct */

    if (GOAL_RES_OK(ret)) {
        /* init buffer */
        GOAL_MEMSET(pHttpChan->templateBuffer, 0, HTTP_CHN_TEMPL_BUFLEN);

        /* set callback information struct */
        GOAL_MEMSET(&cbInfo, 0, sizeof(GOAL_HTTP_APPLCB_TEMPL_T));
        cbInfo.pInst = pInst;
        cbInfo.hdlRes = hdlRes;
        cbInfo.in.name = varName;
        cbInfo.in.retLenMax = HTTP_CHN_TEMPL_BUFLEN;
        cbInfo.out.strReturn = (char *) (pHttpChan->templateBuffer);

        /* get callback function pointer */
        ret = goal_httpResGetTemplCb(pInst, hdlRes, &pfnCallback);
    }

    /* check for valid function pointer before calling */
    if (NULL == pfnCallback) {
        ret = GOAL_ERR_NULL_POINTER;
    }

    if (GOAL_RES_OK(ret)) {
        /* use callback function */
        ret = (*pfnCallback)(&cbInfo);
    }

    if (GOAL_RES_ERR(ret)) {
        /* set error string */
        GOAL_MEMCPY(pHttpChan->templateBuffer, GOAL_HTTP_TMPMGR_ERR_STRING, \
                    GOAL_STRLEN(GOAL_HTTP_TMPMGR_ERR_STRING));
    }

    /* check length */
    newLen = (uint32_t) (GOAL_STRLEN((char *) (pHttpChan->templateBuffer)));
    if (newLen > (uint32_t) (pBuf->bufSize - pBuf->dataLen)) {
        /* bad length */
        *pBool = GOAL_FALSE;
    } else {
        /* copy text into buffer */
        ret = goal_httpAddToBuffer(pInst, pBuf, pHttpChan, pHttpChan->templateBuffer, newLen);
    }

    return ret;
}


/****************************************************************************/
/** Replaces a template tag directly with a CM variable.
 *
 * @returns GOAL_OK on success
 * @returns error code otherwise
 *
 */
static GOAL_STATUS_T http_tmpMgrReplaceCm(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_BUFFER_T *pBuf,                        /**< pointer to message buffer */
    uint32_t modId,                             /**< module id */
    uint32_t varId,                             /**< variable id */
    GOAL_BOOL_T *pBool                          /**< pointer to length bool */
)
{
    GOAL_STATUS_T ret = GOAL_OK;                /* return values */
    GOAL_CM_VAR_T *pVar = NULL;                 /* pointer to CM variable */
    uint32_t newLen = 0;                        /* new length of tag */

    /* get CM variable */
    ret = goal_cmGetVarById(modId, varId, &pVar);

    if (GOAL_RES_OK(ret)) {
        /* stringify variable values */
        ret = http_tmpMgrStringify(pInst, pVar, pHttpChan, pBuf, (pBuf->bufSize - pBuf->dataLen), pBool);
    }

    if (GOAL_RES_ERR(ret)) {
        /* check length */
        newLen = (uint32_t)(GOAL_STRLEN(GOAL_HTTP_TMPMGR_ERR_STRING));
        if (newLen > (uint32_t) (pBuf->bufSize - pBuf->dataLen)) {
            /* bad length */
            *pBool = GOAL_FALSE;
        } else {
            /* copy text into buffer */
            ret = goal_httpAddToBuffer(pInst, pBuf, pHttpChan, (uint8_t *) GOAL_HTTP_TMPMGR_ERR_STRING, newLen);
        }
    }

    return ret;
}


/****************************************************************************/
/** Creates a character string from the value of a CM variable.
 *
 * @returns GOAL_OK on success
 * @returns error code otherwise
 *
 */
static GOAL_STATUS_T http_tmpMgrStringify(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_CM_VAR_T *pVar,                        /**< pointer to CM variable */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_BUFFER_T *pBuf,                        /**< pointer to message buffer */
    uint32_t lenMax,                            /**< maximum string length */
    GOAL_BOOL_T *pBool                          /**< return bool */
)
{
    GOAL_STATUS_T ret = GOAL_OK;                /* return value */
    GOAL_CM_DATATYPE_T dataType = (GOAL_CM_DATATYPE_T) (pVar->type); /* data type */
    char temp[15];                              /* temporary buffer */
    uint32_t val = 0;                           /* CM var value */
    uint32_t newLen = 0;                        /* new length */

    /* init buffer */
    GOAL_MEMSET(temp, 0, sizeof(temp));

    /* check data type */
    switch (dataType)
    {
    case GOAL_CM_UINT8:
    case GOAL_CM_INT8:
        GOAL_CM_GETVAR_UINT8(pVar, val);
        GOAL_SNPRINTF(temp, 4, "%"FMT_u32, val);
        break;

    case GOAL_CM_UINT16:
    case GOAL_CM_INT16:
        GOAL_CM_GETVAR_UINT16(pVar, val);
        GOAL_SNPRINTF(temp, 6, "%"FMT_u32, val);
        break;

    case GOAL_CM_UINT32:
    case GOAL_CM_INT32:
        GOAL_CM_GETVAR_UINT32(pVar, val);
        GOAL_SNPRINTF(temp, 11, "%"FMT_u32, val);
        break;

    case GOAL_CM_IPV4:
        GOAL_CM_GETVAR_UINT32(pVar, val);
        ret = http_convertIP(val, temp);
        break;

    case GOAL_CM_STRING:
        GOAL_SNPRINTF(temp, sizeof(temp), "%s", GOAL_CM_VAR_STRING(pVar));
        break;

    case GOAL_CM_GENERIC:
    case GOAL_CM_NONE:
    default:
        /* error */
        ret = GOAL_ERROR;
        break;
    }

    if (GOAL_RES_OK(ret)) {
        /* check length */
        newLen = (uint32_t)(GOAL_STRLEN(temp));
        if (newLen > lenMax) {
            /* bad length */
            *pBool = GOAL_FALSE;
        } else {
            /* copy text into buffer */
            ret = goal_httpAddToBuffer(pInst, pBuf, pHttpChan, (uint8_t *) temp, newLen);
        }
    }

    return ret;
}


/****************************************************************************/
/** Converts an IP adress to a string.
 *
 * @returns GOAL_TRUE on success
 * @returns GOAL_FALSE otherwise
 *
 */
static GOAL_STATUS_T http_convertIP(
    uint32_t val,                               /**< IP address value */
    char* pRet                                  /**< return string */
)
{
    GOAL_STATUS_T ret = GOAL_OK;                /* return value */
    uint8_t temp;                               /* temporary value */
    uint32_t offset = 0;                        /* string offset */
    uint32_t loop;                              /* loop count */

    for (loop = 0; loop < 4; loop++) {
        if (offset) {
            pRet[offset] = '.';
            offset++;
        }
        temp = (uint8_t) ((val >> (24 - (loop * 8))) & 0xFF);
        GOAL_SNPRINTF(&pRet[offset], 4, "%u", temp);
        offset = (uint32_t) GOAL_STRLEN(pRet);
    }

    return ret;
}


/****************************************************************************/
/** Searches a token in a given buffer with maximum buffer length.
 *
 * @returns GOAL_TRUE on success
 * @returns GOAL_FALSE otherwise
 *
 */
static GOAL_BOOL_T http_tmpMgrFindTok(
    char tok,                                   /**< token */
    const uint8_t *pBuf,                        /**< data buffer */
    uint32_t lenBuf,                            /**< data buffer length */
    uint32_t *pOff                              /**< return token offset */
)
{
    GOAL_BOOL_T ret = GOAL_FALSE;               /* return value */
    uint32_t cnt = 0;                           /* loop counter */

    /* search token */
    for (cnt = 0; ((cnt < lenBuf) && (GOAL_FALSE == ret)); cnt++) {
        if (tok == pBuf[cnt]) {
            /* token found */
            ret = GOAL_TRUE;
            /* set return offset */
            *pOff = cnt;
        }
    }
    return ret;
}

