/** @file
 *
 * @brief GOAL Serialization and Execution of configuration
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

#define GOAL_ID GOAL_ID_CSAP
#include "goal_includes.h"
#include "cm/goal_csap_cm.h"

#if GOAL_CONFIG_CSAP == 1


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/
static GOAL_STATUS_T goal_csapInit(
    void
);

static GOAL_STATUS_T goal_csapCfgStart(
    void
);

static GOAL_STATUS_T goal_csapCfgEnd(
    void
);

static GOAL_STATUS_T goal_csapExecute(
    void
);

static GOAL_STATUS_T goal_csapRun(
    void
);


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_STAGE_HANDLER_T stageInit;          /**< init stage handler */
static GOAL_LOCK_T *csapMtx;                    /**< locking mutex */

static GOAL_CSAP_FUNCTION_LIST_T pCsapFunctionList[] = { /**< list of registered functions */
    {GOAL_CSAP_CFG_START, goal_csapCfgStart, 1, GOAL_FALSE, &pCsapFunctionList[1]},
    {GOAL_CSAP_CFG_END,   goal_csapCfgEnd,   1, GOAL_FALSE, NULL},
};

static GOAL_CM_VAR_T *pConfigVar = NULL;        /**< CM config variable */
static GOAL_CSAP_HANDLE_T *pList;               /**< handle list */
static GOAL_CSAP_HANDLE_T *pHdlCsap;            /**< handle */

static GOAL_CSAP_TYPE *pCsapConfigBuffer;       /**< pointer to configuration buffer */
static uint32_t csapConfigPos = 0;              /**< configuration position */

static char csapStringBuffer[255];              /**< string buffer for string pop */

/**< helper array to determine bit shift for bytes within 32 bit values */
static uint8_t bitShift[] = {
    0,
    8,
    16,
    24
};


/****************************************************************************/
/** Register stage handler
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_csapInitPre(
    void
)
{
    GOAL_STATUS_T res;                          /* return value */

    goal_csapRegCmVars();
    res = goal_mainStageReg(GOAL_STAGE_CM, &stageInit, GOAL_STAGE_INIT, goal_csapInit);

    return res;
}


/****************************************************************************/
/** Initialize CSAP module
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_csapInit(
    void
)
{
    GOAL_STATUS_T res;                          /* return value */

    res = goal_csapReg(&pHdlCsap, GOAL_ID_CSAP, pCsapFunctionList);

    if (GOAL_RES_OK(res)) {
        res = goal_cmGetVarById(GOAL_ID_CSAP, CSAP_CM_VAR_CONFIG, &pConfigVar);
    }

    if (GOAL_RES_OK(res)) {
        pCsapConfigBuffer = (GOAL_CSAP_TYPE *) GOAL_CM_VAR_STRING(pConfigVar);
    }

    if (GOAL_RES_OK(res)) {
        csapConfigPos = pConfigVar->curSize / sizeof(GOAL_CSAP_TYPE);
    }

    if (GOAL_RES_OK(res)) {
        /* init mutex */
        res = goal_lockCreate(GOAL_LOCK_BINARY, &csapMtx, 0, 1, GOAL_ID_LOG);
    }

    return res;
}


/****************************************************************************/
/** Run modules configuration
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_csapSetup(
    void
)
{
    return goal_csapRun();
}


/****************************************************************************/
/** Register function list to the CSAP module
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_csapReg(
    GOAL_CSAP_HANDLE_T **ppHdlCsap,             /**< handle */
    uint32_t modId,                             /**< goal module id */
    GOAL_CSAP_FUNCTION_LIST_T *pFunctionList    /**< list of functions */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* return value */
    GOAL_CSAP_HANDLE_T **ppElem;                /* list element */

    res = goal_memCalloc(ppHdlCsap, sizeof(GOAL_CSAP_HANDLE_T));

    if (GOAL_RES_OK(res)) {
        (*ppHdlCsap)->modId = modId;
        (*ppHdlCsap)->pList = pFunctionList;
    }

    if (GOAL_RES_OK(res)) {
        for (ppElem = &pList; *ppElem; ppElem = &(*ppElem)->pNext);

        *ppElem = *ppHdlCsap;
    }

    return res;
}


/****************************************************************************/
/** Push a value to the configuration
 *
 */
void goal_csapPush(
    GOAL_CSAP_TYPE value                        /**< configurations value */
)
{
    pCsapConfigBuffer[csapConfigPos++] = value;
}


/****************************************************************************/
/** Pop a value from the configuration
 *
 */
GOAL_CSAP_TYPE goal_csapPop(
    void
)
{
    return pCsapConfigBuffer[csapConfigPos++];
}


/****************************************************************************/
/** Push a string to the configuration
 *
 */
void goal_csapPushString(
    const char *strValue                        /**< string value */
)
{
    unsigned int cntPos;                        /* string position */
    GOAL_CSAP_TYPE value;                       /* config buffer value */
    unsigned int cntConfigByte;                 /* position counter for config value */

    value = (GOAL_CSAP_TYPE) GOAL_STRLEN(strValue);
    cntConfigByte = 1;

    for (cntPos = 0; cntPos < (unsigned int) GOAL_STRLEN(strValue); cntPos++) {

        value |= (GOAL_CSAP_TYPE) strValue[cntPos] << bitShift[cntConfigByte++];

        /* emit value and restart */
        if (cntConfigByte > 3) {
            pCsapConfigBuffer[csapConfigPos++] = value;
            value = 0;
            cntConfigByte = 0;
        }
    }

    /* emit remaining bytes */
    if (cntConfigByte != 0) {
        pCsapConfigBuffer[csapConfigPos++] = value;
    }
}


/****************************************************************************/
/** Pop a string value from the configuration
 *
 */
void goal_csapPopString(
    char **pStrValue                            /**< return string */
)
{
    uint8_t strLen;                             /* string length */
    unsigned int cntPos;                        /* string position */
    GOAL_CSAP_TYPE value;                       /* config buffer value */
    unsigned int cntConfigByte;                 /* position counter for config value */

    value = pCsapConfigBuffer[csapConfigPos++];
    strLen = (uint8_t) value&0xff;
    cntConfigByte = 1;

    for (cntPos = 0; cntPos < (unsigned int) strLen; cntPos++) {
        csapStringBuffer[cntPos] = (char) (0xff&(value >> bitShift[cntConfigByte++]));

        /* fetch next value */
        if (cntConfigByte > 3) {
            value = pCsapConfigBuffer[csapConfigPos++];
            cntConfigByte = 0;
        }
    }

    /* terminate string */
    csapStringBuffer[cntPos] = '\0';

    *pStrValue = csapStringBuffer;
}


/****************************************************************************/
/** Skip a value in the configuration
 *
 */
void goal_csapSkip(
    GOAL_CSAP_TYPE cnt                          /**< number of values to skip */
)
{
    csapConfigPos += cnt; \
}


/****************************************************************************/
/** Update configuration size
 *
 */
void goal_csapConfigSizeSet(
    void
)
{
    pConfigVar->curSize = csapConfigPos * sizeof(GOAL_CSAP_TYPE);
}


/****************************************************************************/
/** Execute configuration
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_csapRun(
    void
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* return value */
    uint32_t cntLast = csapConfigPos;           /* command length */

    /* point to start of configuration */
    csapConfigPos = 0;

    while (GOAL_RES_OK(res) && csapConfigPos < cntLast) {
        res = goal_csapExecute();
    }

    /* restore pointer to end of configuration */
    csapConfigPos = cntLast;

    return res;
}


/****************************************************************************/
/** Execute a function from the stack
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_csapExecute(
    void
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* return value */
    uint32_t modId;                             /* mod ID */
    uint32_t fnctId;                            /* function ID */
    uint32_t cntParams;                         /* function parameters */
    GOAL_CSAP_HANDLE_T *pHdl;                   /* handle */
    GOAL_CSAP_FUNCTION_LIST_T *pFnElem = NULL;  /* function list entry */

    GOAL_CSAP_POP(modId, uint32_t);
    GOAL_CSAP_POP(fnctId, uint32_t);
    GOAL_CSAP_POP(cntParams, uint32_t);

    if (GOAL_RES_OK(res)) {

        /* iterate handles of module to find module */
        for (pHdl = pList; (pHdl) && GOAL_RES_OK(res); pHdl = pHdl->pNext) {
            /* if module was found search for function */
            if (modId == pHdl->modId) {

                for (pFnElem = pHdl->pList; (pFnElem) && (pFnElem->id != fnctId); pFnElem = pFnElem->pNext);

                /* call function */
                if (NULL != pFnElem) {
                    if (cntParams == pFnElem->fncParams) {
                        res = pFnElem->fnPop();
                    }
                    else {
                        res = GOAL_ERR_PARAM;
                    }
                }
                /* function unknown */
                else {
                    GOAL_CSAP_SKIP(cntParams);
                }
            }
        }
    }

    return res;
}


/****************************************************************************/
/** Indicate start of configuration
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_csapCfgStart(
    void
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* return value */
    uint32_t modId;                             /* module Id */

    GOAL_CSAP_POP(modId, uint32_t);
    UNUSEDARG(modId);

    return res;
}


/****************************************************************************/
/** Indicate end of configuration
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_csapCfgEnd(
    void
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* return value */
    uint32_t modId;                             /* module Id */

    GOAL_CSAP_POP(modId, uint32_t);
    UNUSEDARG(modId);

    return res;
}


/****************************************************************************/
/** Lock configuration storage
 *
 */
void goal_csapLock(
    void
)
{
    goal_lockGet(csapMtx, GOAL_LOCK_INFINITE);
}


/****************************************************************************/
/** Unlock configuration storage
 *
 */
void goal_csapUnlock(
    void
)
{
    goal_lockPut(csapMtx);
}


/****************************************************************************/
/** Check for a csap configuration available
 *
 */
GOAL_STATUS_T goal_csapCfgValid(
    void
)
{
    if (0 == csapConfigPos) {
        return GOAL_ERR_EMPTY;
    }
    else {
        return GOAL_OK;
    }
}

#endif /* GOAL_CONFIG_CSAP */
