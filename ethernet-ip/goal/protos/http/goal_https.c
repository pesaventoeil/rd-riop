/** @file
 *
 * @brief Implementation of the HTTPS protocol
 *
 * Implementation of the HTTPS protocol.
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

#include "goal_includes.h"
#include "goal_http.h"
#include "goal_https.h"
#include "cm/goal_https_cm.h"


/****************************************************************************/
/** Gets certificate from the CM.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_httpsGetCert(
    GOAL_MI_TLS_CERT_T  *pHttpsCert             /**< pointer to certificate info */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_CM_VAR_T *pCmVar = NULL;               /* pointer to CM var */
    uint32_t lenCert = 0;                       /* certificate string length */
    uint32_t lenPkey = 0;                       /* length of private key */

    /* get certificate length */
    res = goal_cmGetVarById(GOAL_CM_HTTPS_MOD_ID, HTTPS_CM_VAR_TLS_SERVER_CERTIFICATE, &pCmVar);
    if (GOAL_RES_OK(res)) {
        GOAL_CM_GETVARLEN_CURR(pCmVar, lenCert);
    }

    /* get private key length */
    if (GOAL_RES_OK(res)) {
        res = goal_cmGetVarById(GOAL_CM_HTTPS_MOD_ID, HTTPS_CM_VAR_TLS_PRIVATE_KEY, &pCmVar);
    }
    if (GOAL_RES_OK(res)) {
       GOAL_CM_GETVARLEN_CURR(pCmVar, lenPkey);
    }

    if (lenCert && lenPkey) {
        /* use certificate from CM */
    } else {
        /* initialize with 0 values, TLS will use default certificate */
        pHttpsCert->pCert = NULL;
        pHttpsCert->pKeyPriv = NULL;
        pHttpsCert->lenCert = 0;
        pHttpsCert->lenKeyPriv = 0;
    }

    return res;
}


/****************************************************************************/
/** Initializes the certifacte information in the CM.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_httpsSetCertInfoCm(
    GOAL_MI_TLS_T *pMiTls                       /**< TLS MI handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    char strTemp[128];                          /* temporary string */
    GOAL_CM_VAR_T *pCmVar = NULL;               /* pointer to CM var */
    uint32_t lenMax = 0;                        /* maximum var length */

    /* init temp string */
    GOAL_MEMSET(strTemp, 0, sizeof(strTemp));

    if (GOAL_RES_OK(res)) {
        /* get pointer to CM var */
        res = goal_cmGetVarById(GOAL_CM_HTTPS_MOD_ID, HTTPS_CM_VAR_TLS_SRV_CERT_CA_CN, &pCmVar);
    }
    /* get maximum length of CM var */
    if (GOAL_RES_OK(res)) {
        GOAL_CM_GETVARLEN_MAX(pCmVar, lenMax);
    }
    /* read cert info */
    if (GOAL_RES_OK(res)) {
        res = goal_miTlsReadInfo(pMiTls, GOAL_MI_TLS_CERTINFO_CA_CN, strTemp, lenMax);
    }
    /* set var in CM */
    if (GOAL_RES_OK(res)) {
        GOAL_CM_SETVAR_GENERIC(pCmVar, strTemp, (uint32_t) GOAL_STRLEN(strTemp));
        GOAL_MEMSET(strTemp, 0, sizeof(strTemp));
    }

    if (GOAL_RES_OK(res)) {
        /* get pointer to CM var */
        res = goal_cmGetVarById(GOAL_CM_HTTPS_MOD_ID, HTTPS_CM_VAR_TLS_SRV_CERT_CA_O, &pCmVar);
    }
    /* get maximum length of CM var */
    if (GOAL_RES_OK(res)) {
        GOAL_CM_GETVARLEN_MAX(pCmVar, lenMax);
    }
    /* read cert info */
    if (GOAL_RES_OK(res)) {
        res = goal_miTlsReadInfo(pMiTls, GOAL_MI_TLS_CERTINFO_CA_O, strTemp, lenMax);
    }
    /* set var in CM */
    if (GOAL_RES_OK(res)) {
        GOAL_CM_SETVAR_GENERIC(pCmVar, strTemp, (uint32_t) GOAL_STRLEN(strTemp));
        GOAL_MEMSET(strTemp, 0, sizeof(strTemp));
    }

    if (GOAL_RES_OK(res)) {
        /* get pointer to CM var */
        res = goal_cmGetVarById(GOAL_CM_HTTPS_MOD_ID, HTTPS_CM_VAR_TLS_SRV_CERT_CA_C, &pCmVar);
    }
    /* get maximum length of CM var */
    if (GOAL_RES_OK(res)) {
        GOAL_CM_GETVARLEN_MAX(pCmVar, lenMax);
    }
    /* read cert info */
    if (GOAL_RES_OK(res)) {
        res = goal_miTlsReadInfo(pMiTls, GOAL_MI_TLS_CERTINFO_CA_C, strTemp, lenMax);
    }
    /* set var in CM */
    if (GOAL_RES_OK(res)) {
        GOAL_CM_SETVAR_GENERIC(pCmVar, strTemp, (uint32_t) GOAL_STRLEN(strTemp));
        GOAL_MEMSET(strTemp, 0, sizeof(strTemp));
    }

    if (GOAL_RES_OK(res)) {
        /* get pointer to CM var */
        res = goal_cmGetVarById(GOAL_CM_HTTPS_MOD_ID, HTTPS_CM_VAR_TLS_SRV_CERT_CN, &pCmVar);
    }
    /* get maximum length of CM var */
    if (GOAL_RES_OK(res)) {
        GOAL_CM_GETVARLEN_MAX(pCmVar, lenMax);
    }
    /* read cert info */
    if (GOAL_RES_OK(res)) {
        res = goal_miTlsReadInfo(pMiTls, GOAL_MI_TLS_CERTINFO_CN, strTemp, lenMax);
    }
    /* set var in CM */
    if (GOAL_RES_OK(res)) {
        GOAL_CM_SETVAR_GENERIC(pCmVar, strTemp, (uint32_t) GOAL_STRLEN(strTemp));
        GOAL_MEMSET(strTemp, 0, sizeof(strTemp));
    }

    if (GOAL_RES_OK(res)) {
        /* get pointer to CM var */
        res = goal_cmGetVarById(GOAL_CM_HTTPS_MOD_ID, HTTPS_CM_VAR_TLS_SRV_CERT_O, &pCmVar);
    }
    /* get maximum length of CM var */
    if (GOAL_RES_OK(res)) {
        GOAL_CM_GETVARLEN_MAX(pCmVar, lenMax);
    }
    /* read cert info */
    if (GOAL_RES_OK(res)) {
        res = goal_miTlsReadInfo(pMiTls, GOAL_MI_TLS_CERTINFO_O, strTemp, lenMax);
    }
    /* set var in CM */
    if (GOAL_RES_OK(res)) {
        GOAL_CM_SETVAR_GENERIC(pCmVar, strTemp, (uint32_t) GOAL_STRLEN(strTemp));
        GOAL_MEMSET(strTemp, 0, sizeof(strTemp));
    }

    if (GOAL_RES_OK(res)) {
        /* get pointer to CM var */
        res = goal_cmGetVarById(GOAL_CM_HTTPS_MOD_ID, HTTPS_CM_VAR_TLS_SRV_CERT_C, &pCmVar);
    }
    /* get maximum length of CM var */
    if (GOAL_RES_OK(res)) {
        GOAL_CM_GETVARLEN_MAX(pCmVar, lenMax);
    }
    /* read cert info */
    if (GOAL_RES_OK(res)) {
        res = goal_miTlsReadInfo(pMiTls, GOAL_MI_TLS_CERTINFO_C, strTemp, lenMax);
    }
    /* set var in CM */
    if (GOAL_RES_OK(res)) {
        GOAL_CM_SETVAR_GENERIC(pCmVar, strTemp, (uint32_t) GOAL_STRLEN(strTemp));
        GOAL_MEMSET(strTemp, 0, sizeof(strTemp));
    }

    if (GOAL_RES_OK(res)) {
        /* get pointer to CM var */
        res = goal_cmGetVarById(GOAL_CM_HTTPS_MOD_ID, HTTPS_CM_VAR_TLS_SRV_CERT_NOT_BEFORE, &pCmVar);
    }
    /* get maximum length of CM var */
    if (GOAL_RES_OK(res)) {
        GOAL_CM_GETVARLEN_MAX(pCmVar, lenMax);
    }
    /* read cert info */
    if (GOAL_RES_OK(res)) {
        res = goal_miTlsReadInfo(pMiTls, GOAL_MI_TLS_CERTINFO_NBF, strTemp, lenMax);
    }
    /* set var in CM */
    if (GOAL_RES_OK(res)) {
        GOAL_CM_SETVAR_GENERIC(pCmVar, strTemp, (uint32_t) GOAL_STRLEN(strTemp));
        GOAL_MEMSET(strTemp, 0, sizeof(strTemp));
    }

    if (GOAL_RES_OK(res)) {
        /* get pointer to CM var */
        res = goal_cmGetVarById(GOAL_CM_HTTPS_MOD_ID, HTTPS_CM_VAR_TLS_SRV_CERT_NOT_AFTER, &pCmVar);
    }
    /* get maximum length of CM var */
    if (GOAL_RES_OK(res)) {
        GOAL_CM_GETVARLEN_MAX(pCmVar, lenMax);
    }
    /* read cert info */
    if (GOAL_RES_OK(res)) {
        res = goal_miTlsReadInfo(pMiTls, GOAL_MI_TLS_CERTINFO_NAF, strTemp, lenMax);
    }
    /* set var in CM */
    if (GOAL_RES_OK(res)) {
        GOAL_CM_SETVAR_GENERIC(pCmVar, strTemp, (uint32_t) GOAL_STRLEN(strTemp));
        GOAL_MEMSET(strTemp, 0, sizeof(strTemp));
    }

    return res;
}
