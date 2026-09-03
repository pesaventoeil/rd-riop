/** @file
 *
 * @brief Implementation of the Goal http authentication module
 *
 * Implementation of the HTTP authentication handling.
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
#include "goal_http_auth.h"
#include "goal_http_resmgr.h"
#include "cm/goal_http_cm.h"
#include "b64/cdecode.h"


/****************************************************************************/
/* Local types */
/****************************************************************************/


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static char decOut[32];


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/

static GOAL_BOOL_T goal_httpAuthBasSetCheck(
    uint32_t cmVarId                            /**< CM variable Id */
);


/****************************************************************************/
/** This function checks if basic authentication variable is set.
 *
 * @retval GOAL_TRUE if set
 * @retval GOAL_FALSE otherwise
 */
static GOAL_BOOL_T goal_httpAuthBasSetCheck(
    uint32_t cmVarId                            /**< CM variable Id */
)
{
    GOAL_STATUS_T ret = GOAL_OK;                /* return value */
    GOAL_BOOL_T retBool = GOAL_TRUE;            /* return bool */
    GOAL_CM_VAR_T *pCmVar = NULL;               /* pointer to CM var */

    /* load CM var */
    ret = goal_cmGetVarById(GOAL_CM_HTTP_MOD_ID, cmVarId, &pCmVar);

    if (GOAL_RES_OK(ret)) {
        /* check, if variable is set */
        if (pCmVar->curSize) {
            /* set */
            retBool = GOAL_TRUE;
        } else {
            /* not set */
            retBool = GOAL_FALSE;
        }
    }

    return retBool;
}


/****************************************************************************/
/** This function checks basic authentication.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_httpAuthBasCheck(
    GOAL_HTTP_T         *pInst,                 /**< instance pointer */
    GOAL_HTTP_HDL_T     hdlRes,                 /**< resource handle */
    char                *strAuth,               /**< authentication string */
    uint32_t            lenAuth                 /**< length of string */
)
{
    GOAL_STATUS_T ret = GOAL_OK;                /* return value */
    int32_t retDec = 0;                         /* decode return */
    base64_decodestate decState;                /* decode struct */
    GOAL_CM_VAR_T *pCmVar = NULL;               /* pointer to CM var */
    char *pCmString;                            /* pointer to string in CM */
    uint32_t cmVarId = 0;                       /* CM variable id */
    uint32_t access = 0;                        /* access level */

    /* get resource access level */
    ret = goal_httpGetResAcc(pInst, hdlRes, &access);

    if (GOAL_RES_OK(ret)) {
        /* define access level */
        if (access & GOAL_HTTP_AUTH_USERLEVEL0) {
            cmVarId = HTTP_CM_VAR_USERLEVEL0;
        } else if (access & GOAL_HTTP_AUTH_USERLEVEL1) {
            cmVarId = HTTP_CM_VAR_USERLEVEL1;
        } else if (access & GOAL_HTTP_AUTH_USERLEVEL2) {
            cmVarId = HTTP_CM_VAR_USERLEVEL2;
        } else if (access & GOAL_HTTP_AUTH_USERLEVEL3) {
            cmVarId = HTTP_CM_VAR_USERLEVEL3;
        }
    }

    if (GOAL_RES_OK(ret)) {
        /* check if authentication variable is set */
        if (GOAL_TRUE == goal_httpAuthBasSetCheck(cmVarId)) {
            /* init decode */
            base64_init_decodestate(&decState);
            GOAL_MEMSET(decOut, 0, ARRAY_ELEMENTS(decOut));

            /* decode base64 string */
            retDec = base64_decode_block(strAuth, (int) lenAuth, decOut, &decState);
            if (0 > retDec) {
                /* error */
                ret = GOAL_ERROR;
            }

            /* load CM var */
            if (GOAL_RES_OK(ret)) {
                ret = goal_cmGetVarById(GOAL_CM_HTTP_MOD_ID, cmVarId, &pCmVar);
            }

            /* check length */
            if (GOAL_RES_OK(ret)) {
                if (GOAL_STRLEN(decOut) != pCmVar->curSize) {
                    /* bad length */
                    ret = GOAL_ERROR;
                }
            }

            /* check string */
            if (GOAL_RES_OK(ret)) {
                /* get string */
                pCmString = GOAL_CM_VAR_STRING(pCmVar);
                if (0 != GOAL_MEMCMP(pCmString, decOut, pCmVar->curSize)) {
                    /* string does not match */
                    ret = GOAL_ERROR;
                }
            }
        }
    }

    return ret;
}


/****************************************************************************/
/** This function sets basic authentication information.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_httpAuthBasSetUserInfoImpl(
    GOAL_HTTP_T         *pInst,                 /**< instance pointer */
    uint32_t            userLevel,              /**< user level */
    char                *strUser,               /**< user name string */
    char                *strPw                  /**< password string */
)
{
    GOAL_STATUS_T ret = GOAL_OK;                /* return value */
    uint32_t lenUser = 0;                       /* length of user name */
    uint32_t lenPw = 0;                         /* length of password */
    uint32_t cmVarId = 0;                       /* CM variable id */
    GOAL_CM_VAR_T *pCmVar = NULL;               /* pointer to CM var */
    char temp[128];                             /* temp string */
    uint32_t offset = 0;                        /* string offset */

    UNUSEDARG(pInst);

    GOAL_MEMSET(temp, 0, sizeof(temp));

    if (strUser) {
        /* get user name length */
        lenUser = (uint32_t) GOAL_STRLEN(strUser);
    } else {
        ret = GOAL_ERR_NULL_POINTER;
    }

    if (strPw) {
        /* get user name length */
        lenPw = (uint32_t) GOAL_STRLEN(strPw);
    } else {
        ret = GOAL_ERR_NULL_POINTER;
    }

    if (GOAL_RES_OK(ret)) {
        /* define access level */
        if (userLevel & GOAL_HTTP_AUTH_USERLEVEL0) {
            cmVarId = HTTP_CM_VAR_USERLEVEL0;
        } else if (userLevel & GOAL_HTTP_AUTH_USERLEVEL1) {
            cmVarId = HTTP_CM_VAR_USERLEVEL1;
        } else if (userLevel & GOAL_HTTP_AUTH_USERLEVEL2) {
            cmVarId = HTTP_CM_VAR_USERLEVEL2;
        } else if (userLevel & GOAL_HTTP_AUTH_USERLEVEL3) {
            cmVarId = HTTP_CM_VAR_USERLEVEL3;
        }
    }

    /* load CM var */
    if (GOAL_RES_OK(ret)) {
        ret = goal_cmGetVarById(GOAL_CM_HTTP_MOD_ID, cmVarId, &pCmVar);
    }

    if (GOAL_RES_OK(ret)) {
        /* check string length */
        if (pCmVar->totalSize < (lenUser + lenPw + 1)) {
            /* error */
            ret = GOAL_ERR_OVERFLOW;
        } else {
            GOAL_MEMCPY(&temp[offset], strUser, lenUser);
            offset += lenUser;
            temp[offset] = ':';
            offset++;
            GOAL_MEMCPY(&temp[offset], strPw, lenPw);
            /* set variable */
            GOAL_CM_SETVAR_GENERIC(pCmVar, temp, (uint32_t) GOAL_STRLEN(temp));
        }
    }

    return ret;
}


