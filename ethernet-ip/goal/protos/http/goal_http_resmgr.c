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

#define GOAL_ID GOAL_ID_HTTP
#include "goal_includes.h"
#include "goal_http.h"
#include "goal_http_resmgr.h"
#include "goal_http_tmpmgr.h"


/****************************************************************************/
/* Local types */
/****************************************************************************/


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static const char strUrlDelimeters[]="/? ";     /**< list of url delimeters */


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/


/****************************************************************************/
/** This functions searches a data callback function by resource handle.
 *
 * @retval callback function pointer on success
 * @retval NULL otherwise
 */
GOAL_STATUS_T goal_httpResGetDataCb(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_HDL_T hdlRes,                     /**< resource handle */
    GOAL_HTTP_REQCB_T *pFcnRet                  /**< return for function pointer */
)
{
    GOAL_STATUS_T ret = GOAL_ERROR;             /* return value */
    HTTP_RESMGR_LL_RES_T *pRes;                 /* resource pointer */

    UNUSEDARG(pInst);

    /* get pointer to list */
    pRes = (HTTP_RESMGR_LL_RES_T *) hdlRes;

    *pFcnRet = pRes->pfnCbData;
    ret = GOAL_OK;

    return ret;
}


/****************************************************************************/
/** This functions searches a template callback function by resource handle.
 *
 * @retval callback function pointer on success
 * @retval NULL otherwise
 */
GOAL_STATUS_T goal_httpResGetTemplCb(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_HDL_T hdlRes,                     /**< resource handle */
    GOAL_HTTP_TMPCB_T *pFcnRet                  /**< return for function pointer */
)
{
    GOAL_STATUS_T ret = GOAL_ERROR;             /* return value */
    HTTP_RESMGR_LL_RES_T *pRes;                 /* resource pointer */

    UNUSEDARG(pInst);

    /* get pointer to list */
    pRes = (HTTP_RESMGR_LL_RES_T *) hdlRes;

    *pFcnRet = (pRes->pfnCbTemplate);
    ret = GOAL_OK;

    return ret;
}


/****************************************************************************/
/** This functions prints the resource list.
 *
 */
void goal_httpResMgrPrintListSort(
    GOAL_HTTP_T *pInst                          /**< instance pointer */
)
{
    UNUSEDARG(pInst);
}


/****************************************************************************/
/** This functions searches a resource path and returns a handle.
 *
 * @retval not '0' successful
 * @retval equals 0 failed
 */
GOAL_STATUS_T goal_httpResMgrSearch(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    uint8_t *strUrl,                            /**< URL string */
    uint32_t lenUrl,                            /**< URL string length */
    HTTP_RESMGR_LL_RES_T **ppResRet,            /**< return pointer for resource */
    uint32_t *pLenMatched                       /**< pointer to matching length var */
)
{
    GOAL_STATUS_T ret = GOAL_ERR_NOT_FOUND;     /* return value */
    HTTP_RESMGR_LL_RES_T *pList;                /* resource list */
    HTTP_RESMGR_LL_RES_T *pEntry;               /* resource entry */
    HTTP_RESMGR_LL_RES_T *pFound;               /* found entry */
    int cntPos;                                 /* position counter */

    /* any paths, application, resource registered? */
    if (0 == pInst->cntResHdlInUse) {
        /* error */
        return ret;
    }

    /* reset return values */
    *pLenMatched = 0;
    pFound = NULL;

    cntPos = 0;

    /* set starting point */
    pList = (HTTP_RESMGR_LL_RES_T *) pInst->pResMgrRes;

    do {
        /* skip leading delimeter */
        if (strUrl[0] == '/') {
            strUrl++;
            cntPos++;
            lenUrl--;
        }

        /* stop parsing on additional parameters */
        if (strUrl[0] == '?') {
            break;
        }

        /* match token */
        pFound = NULL;
        GOAL_LL_FOREACH(pList, pEntry) {

            /* check for delimeter and length */
            if ((pEntry->lenUrl == GOAL_STRLEN((const char *) strUrl)) || (strchr(strUrlDelimeters, strUrl[pEntry->lenUrl]) != NULL)) {
                /* compare string */
                if (0 == strncmp((const char *) strUrl, (char *) pEntry->strUrl, (unsigned long) pEntry->lenUrl)) {
                    pFound = pEntry;
                    pList = pEntry->pChild;
                    cntPos += pEntry->lenUrl;
                    strUrl += pEntry->lenUrl;
                    break;
                }
            }
        }

        /* stop if no match */
        if (NULL == pFound) {
            break;
        }

    } while (cntPos < (int) lenUrl);

    /* handle remaining unmatched url */
    if (0 != GOAL_STRLEN((const char *) strUrl)) {
        if (0 == strcmp("/", (const char *) strUrl)) {
        /* for now accept '/' and string end delimeter */
        }
        else if ('?' == strUrl[0]) {
          /* pass additional arguments to the application */
        }
        else {
            pFound = NULL;
        }
    }

    /* propagate result */
    if (NULL != pFound) {
        *ppResRet = pFound;
        *pLenMatched = lenUrl;

        ret = GOAL_OK;
    }
    else {
        ret = GOAL_ERR_NOT_FOUND;
    }

    return ret;
}


/****************************************************************************/
/** This functions registers a resource.
 *
 * @retval GOAL_OK on success
 * @retval error code otherwise
 */
GOAL_STATUS_T goal_httpResRegImpl(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    uint8_t *strUrl,                            /**< URL string */
    uint32_t allowMeth,                         /**< allowed methods for resource */
    GOAL_HTTP_REQCB_T pfnCbData,                /**< application callback */
    GOAL_HTTP_TMPCB_T pfnCbTemplate,            /**< template handler callback */
    GOAL_HTTP_HDL_T *pHdl                       /**< resource handle return */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* return value */
    HTTP_RESMGR_LL_RES_T **ppList = NULL;       /* list */
    HTTP_RESMGR_LL_RES_T *pEntry = NULL;        /* list entry */
    HTTP_RESMGR_LL_RES_T *pEntryFound = NULL;   /* list entry for iteration */
    char *strToken;                             /* url token */
    int cntPos;                                 /* position counter */

    /* check for valid parameters */
    if (NULL == strUrl || GOAL_STRLEN((char *) strUrl) == 0) {
        return GOAL_ERR_PARAM;
    }

    /* check method allow identifier, at least one method should be set */
    if (0 == allowMeth) {
        /* error */
        return GOAL_ERR_PARAM;
    }

    /* return pointer set? */
    if (NULL == pHdl) {
        /* error */
        return GOAL_ERR_NULL_POINTER;
    }

    /* set start entry */
    ppList = &pInst->pResMgrRes;

    /* split registered resource into parts */
    strToken = (char *) strUrl;

    do {
        cntPos = 0;

        /* skip leading delimeter */
        if (strToken[cntPos] == '/') {
            strToken++;
        }

        /* search end of token */
        while (strToken[cntPos] != '/' && strToken[cntPos] != '\0') {
            cntPos++;
        }

        if (cntPos > 0) {

            /* token found with length of cntPos */

            /* iterate existing list */
            pEntryFound = NULL;
            GOAL_LL_FOREACH(*ppList, pEntry) {
                if (0 == strncmp(strToken, (char *) pEntry->strUrl, (unsigned long) cntPos)) {
                    pEntryFound = pEntry;
                    break;
                }
            }

            if (NULL == pEntryFound) {
                res = goal_memCalloc(&pEntry, sizeof(HTTP_RESMGR_LL_RES_T));
                if (GOAL_RES_OK(res)) {
                    res = goal_memCalloc(&pEntry->strUrl, (uint32_t) cntPos + 1);
                }

                if (GOAL_RES_OK(res)) {
                    GOAL_MEMCPY((char *) pEntry->strUrl, strToken, (unsigned long) cntPos);
                    pEntry->strUrl[cntPos] = '\0';
                    pEntry->lenUrl = (uint32_t) cntPos;

                    GOAL_LL_APPEND(*ppList, pEntry);
                }
            }
        }

        /* set list to child list */
        ppList = &pEntry->pChild;

        /* set new token start */
        strToken += cntPos;

    } while (*strToken != '\0');

    /* populate created entry */
    pEntry->accMeth = allowMeth;
    pEntry->pfnCbData = pfnCbData;
    pEntry->pfnCbTemplate = pfnCbTemplate;

    /* increase hdl counter */
    pInst->cntResHdlInUse++;

    /* update hdl */
    pEntry->id = (uint16_t) pInst->cntResHdlInUse;
    pEntry->hdlRes = (GOAL_HTTP_HDL_T) pEntry;
    *pHdl = pEntry;

    return res;
}


/****************************************************************************/
/** This function gets resource information for later processing.
 *
 * @retval GOAL_OK on success
 * @retval error code otherwise
 */
GOAL_STATUS_T goal_httpGetResAcc(
    GOAL_HTTP_T         *pInst,                 /**< instance pointer */
    GOAL_HTTP_HDL_T     hdlRes,                 /**< resource handle */
    uint32_t            *pAccess                /**< pointer to access rights */
)
{
    UNUSEDARG(pInst);

    GOAL_STATUS_T ret = GOAL_ERR_NOT_FOUND;     /* return value */
    HTTP_RESMGR_LL_RES_T *pRes;                 /* resource pointer */

    /* get resource pointer */
    pRes = (HTTP_RESMGR_LL_RES_T *) hdlRes;

    /* set acces return */
    *pAccess = pRes->accMeth;

    /* set return value */
    ret = GOAL_OK;

    return ret;
}


/****************************************************************************/
/** Get id for HTTP handle
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_httpIdByHdl(
    uint16_t *pId,                              /**< id of handle */
    GOAL_HTTP_HDL_T hdl                         /**< http handle */
)
{
    HTTP_RESMGR_LL_RES_T *pEntry;               /* resource entry */

    pEntry = (HTTP_RESMGR_LL_RES_T *) hdl;
    if (NULL != pEntry) {
        *pId = pEntry->id;
        return GOAL_OK;
    }

    return GOAL_ERR_NULL_POINTER;
}


/****************************************************************************/
/** Get HTTP handle for id
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_httpHdlById(
    GOAL_HTTP_HDL_T *pHdl,                      /**< http handle */
    uint16_t id                                 /**< id of handle */
)
{
    /* TODO */
    *pHdl = NULL;
    UNUSEDARG(id);

    return GOAL_OK;
}
