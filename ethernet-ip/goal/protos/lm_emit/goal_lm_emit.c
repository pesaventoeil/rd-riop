/** @file
 *
 * @brief  CTC wrapper for iRJ45 RPC functions
 *
 * @details
 * This module takes goal lm logging messages from the lm buffer. It converts
 * those messages to a readable form and outputs these to varous targets.
 *
 * @copyright
 * Copyright 2020 port GmbH Halle/Saale.
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

#define GOAL_ID GOAL_ID_LM
#include <goal_includes.h>

#if (GOAL_CONFIG_LOGGING == 1) && (GOAL_CONFIG_LM_EMIT == 1)


/****************************************************************************/
/* Defines */
/****************************************************************************/

#define GOAL_LOG_DEBUG      (1 << 0)
#define GOAL_LOG_INFO       (1 << 1)
#define GOAL_LOG_WARNING    (1 << 2)
#define GOAL_LOG_ERROR      (1 << 3)
#define GOAL_LOG_EXCEPTION  (1 << 4)

static const char strLevel[] = {
    '-',
    'D',
    'I',
    '-',
    'W',
    '-',
    '-',
    '-',
    'E',
    '-',
    '-',
    '-',
    '-',
    '-',
    '-',
    '-',
    'X'
};

#define EMIT_DATA() { \
    GOAL_LL_FOREACH(pListEmitter, pEmitEntry) { \
       pEmitEntry->fnctData(pEmitEntry, (const char *) pEmitterBuf, strLen); \
    } \
}

#ifndef GOAL_CONFIG_LM_EMIT_TIMESTAMP
# define GOAL_CONFIG_LM_EMIT_TIMESTAMP 0
#endif

#define GOAL_LM_EMIT_MB_SIZE 256                /**< log message buffer size */
#define GOAL_LM_EMIT_EB_SIZE 512                /**< log emit buffer size */

#define GOAL_LM_CORE_EXT "AC_"                  /**< message prefix of external source */
#if GOAL_CONFIG_MCTC_CC == 1
#  define GOAL_LM_CORE_INT "CC_"                /**< message prefix of internal source in multi core mode */
#else
#  define GOAL_LM_CORE_INT ""                   /**< message prefix of internal source in single core mode */
#endif


/****************************************************************************/
/* Includes */
/****************************************************************************/
#include <goal_includes.h>
#include "goal_lm_emit.h"
#include "goal_lm_db.h"
#include "goal_lm_emit_raw.h"
#include "goal_lm_emit_ma_uart.h"
#include "goal_lm_emit_exception_log.h"


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_LM_READER_T *pReaderEmit;           /**< logging reader */
static GOAL_LM_EMIT_EMITTER_T *pListEmitter;    /**< list of registered emitters */
static GOAL_LM_EMIT_EMITTER_T *mpEmitEmergency; /**< emergency emitter */
static GOAL_STAGE_HANDLER_T stageInit;          /**< init stage */
static GOAL_BOOL_T flgUpdate = GOAL_FALSE;      /**< update flag to signal new data */
static GOAL_BOOL_T flgInit = GOAL_FALSE;        /**< initialization flag */

static uint8_t *pMessageBuf;                    /**< message buffer */
static uint8_t *pEmitterBuf;                    /**< emitter buffer */


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
static void goal_lmEmitLoop(
    void
);

static GOAL_STATUS_T goal_lmEmitInitPost(
    void
);

static GOAL_STATUS_T goal_lmEmitParameter(
   const GOAL_LM_DB_ENTRY_T *dbEntry,           /**< log message description */
   int pCnt,                                    /**< param counter */
   GOAL_LM_LOG_ENTRY_T *pLmEntry                /**< log message */
);

static GOAL_STATUS_T goal_lmEmitReaderCb(
    struct GOAL_LM_READER_T *pHdlReader
);

static GOAL_STATUS_T goal_lmEmitAllIdle(
    void
);


/****************************************************************************/
/** initialization
 *
 * This function initialize all necessary function for the lm emitter
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_lmEmitInitPre(
    void
)
{
    /* register init stage */
    return goal_mainStageReg(GOAL_STAGE_QUEUE, &stageInit, GOAL_STAGE_INIT, goal_lmEmitInitPost);
}


/****************************************************************************/
/** post initialization
 *
 * This function initialize memory buffers for the lm emitter
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_lmEmitInitPost(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* allocate message buffer */
    res = goal_memCalloc(&pMessageBuf, GOAL_LM_EMIT_MB_SIZE);

    /* allocate emitter buffer */
    if (GOAL_RES_OK(res)) {
        res = goal_memCalloc(&pEmitterBuf, GOAL_LM_EMIT_EB_SIZE);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_lmReaderReg(&pReaderEmit, goal_lmEmitReaderCb);
    }

    /* request pool buffer */
    if (GOAL_RES_OK(res)) {
        res = goal_queuePoolBufsReq(GOAL_ID, GOAL_NETBUF_SIZE, 2, 0);
    }

#if GOAL_CONFIG_LM_EMIT_MA_UART == 1
    if (GOAL_RES_OK(res)) {
        res = goal_lmEmitMaUartReg();
    }
#elif GOAL_CONFIG_LOGGING_TARGET_RAW == 1
    if (GOAL_RES_OK(res)) {
        res = goal_lmEmitRawReg();
    }
#endif

#if GOAL_CONFIG_LM_EXCEPTION_LOG == 1
    if (GOAL_RES_OK(res)) {
        res = goal_lmEmitExceptionLogReg();
    }
#endif

    /* only if emitters are registered, register loop */
    if (pListEmitter != NULL) {
        if (GOAL_RES_OK(res)) {
            res = goal_mainLoopReg(goal_lmEmitLoop);
        }
    }

    flgInit = GOAL_TRUE;

    return res;
}


/****************************************************************************/
/** Loop function to reading logging buffer
 *
 * This function is called cyclic.
 */
static void goal_lmEmitLoop(
    void
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_LM_LOG_ENTRY_T *logEntry;              /* log message header */
    const GOAL_LM_DB_ENTRY_T *dbEntry;          /* log message description */
    int strLen = 0;                             /* snprintf length */
    int idxSegment;                             /* segment index */
    GOAL_LM_EMIT_EMITTER_T *pEmitEntry;         /* emitter list element */
#if GOAL_CONFIG_LM_EMIT_TIMESTAMP == 1
    time_t ts;                                  /* timestamp */
    struct tm *pTv;                             /* time */
#endif
    const char *strFuncName;                    /* function name */
    uint16_t len;                               /* message len */
    uint16_t cntCycle;                          /* limiter for continueous logging */

    cntCycle = 0;

    do {

        /* stop if contineouly operating */
        if (cntCycle > GOAL_CONFIG_LM_EMIT_LIMITER) {
            break;
        }

        /* skip if no update is available */
        if (GOAL_FALSE == flgUpdate) {
            break;
        }

        /* skip if message or emit buffer isn't initialized yet */
        if ((NULL == pMessageBuf) || (NULL == pEmitterBuf)) {
            flgUpdate = GOAL_FALSE;
            break;
        }

        /* skip if any emitter is not idle */
        res = goal_lmEmitAllIdle();
        if (GOAL_RES_ERR(res)) {
            break;
        }

        /* all emitters are idle, continue to the next message */
        res = goal_lmBufferMsgGet(pReaderEmit, pMessageBuf, GOAL_LM_EMIT_MB_SIZE, &len);
        if (GOAL_RES_ERR(res)) {
            flgUpdate = GOAL_FALSE;
        }

        if (GOAL_RES_OK(res)) {

            /* parse message */
            if (GOAL_RES_OK(res)) {
                logEntry = (GOAL_LM_LOG_ENTRY_T *) pMessageBuf;

                /* find description for log id */
                dbEntry = &goal_lm_db[logEntry->logId - 1];

                /* initiate emitters */
                GOAL_LL_FOREACH(pListEmitter, pEmitEntry) {
                    /* update raw log message */
                    pEmitEntry->pLogRaw = logEntry;
                    pEmitEntry->lenLogRaw = len;
                    pEmitEntry->pDbEntry = dbEntry;

                    /* call start functions */
                    if (NULL != pEmitEntry->fnctStart) {
                        pEmitEntry->fnctStart(pEmitEntry);
                    }
                }

#if GOAL_CONFIG_LM_EMIT_TIMESTAMP == 1
                /* create timestamp */
                ts = logEntry->timestamp;
                pTv = localtime(&ts);
                strLen = GOAL_SNPRINTF((char *) pEmitterBuf, GOAL_LM_EMIT_EB_SIZE, "[%02d:%02d:%02d] ", pTv->tm_hour, pTv->tm_min, pTv->tm_sec);
                EMIT_DATA();
#endif

                /* supress goal_lmLogLegacy prefix */
                if ((logEntry->logId != 17) &&
                    (logEntry->logId != 18) &&
                    (logEntry->logId != 19) &&
                    (logEntry->logId != 20)
                    ) {
                    /* create message header */
                    strFuncName = (const char *) logEntry + sizeof(GOAL_LM_LOG_ENTRY_T);
                    strLen = GOAL_SNPRINTF((char *) pEmitterBuf, GOAL_LM_EMIT_EB_SIZE, "[%s%c|%.*s:%u] ", (logEntry->flags&GOAL_LM_FLAG_EXT) ? GOAL_LM_CORE_EXT:GOAL_LM_CORE_INT, strLevel[dbEntry->severity], logEntry->nameLength, strFuncName, logEntry->lineNumber);
                    EMIT_DATA();
                }

                /* create message text */
                for (idxSegment = 0; idxSegment < 3; idxSegment ++) {

                    strLen = GOAL_SNPRINTF((char *) pEmitterBuf, GOAL_LM_EMIT_EB_SIZE, "%.*s", dbEntry->seg[idxSegment][1], &(dbEntry->text[dbEntry->seg[idxSegment][0]]));
                    EMIT_DATA();

                    goal_lmEmitParameter(dbEntry, idxSegment, logEntry);

                }

                EMIT_DATA();

                /* stop emitters */
                GOAL_LL_FOREACH(pListEmitter, pEmitEntry) {
                    pEmitEntry->fnctEnd(pEmitEntry);
                }
            }
        }

        cntCycle++;

    } while (1);
}


/****************************************************************************/
/** Register an emitter module
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_lmEmitEmitterReg(
    GOAL_LM_EMIT_EMITTER_T **ppHdlEmitter,      /**< new emitter handle */
    GOAL_LM_EMIT_EMITTER_OPEN fnctOpen,         /**< open function */
    GOAL_LM_EMIT_EMITTER_START fnctStart,       /**< start emitting function */
    GOAL_LM_EMIT_EMITTER_DATA fnctData,         /**< data emitting function */
    GOAL_LM_EMIT_EMITTER_END fnctEnd,           /**< end emitting function */
    GOAL_LM_EMIT_EMITTER_IDLE fnctIdle,         /**< idle check emitting function */
    void *pData
)
{
    GOAL_STATUS_T res;                          /* result */

    /* disable the emergency emitter when a real emitter is registered */
    if (NULL != mpEmitEmergency) {
        pListEmitter = NULL;

        /* clear emergency emitter */
        mpEmitEmergency = NULL;
    }

    res = goal_memCalloc(ppHdlEmitter, sizeof(GOAL_LM_EMIT_EMITTER_T));

    if (GOAL_RES_OK(res)) {
        (*ppHdlEmitter)->fnctOpen = fnctOpen;
        (*ppHdlEmitter)->fnctStart = fnctStart;
        (*ppHdlEmitter)->fnctData = fnctData;
        (*ppHdlEmitter)->fnctEnd = fnctEnd;
        (*ppHdlEmitter)->fnctIdle = fnctIdle;
        (*ppHdlEmitter)->pData = pData;
        (*ppHdlEmitter)->flgEnabled = GOAL_FALSE;

        /* open this emitter */
        if (NULL != (*ppHdlEmitter)->fnctOpen) {
            res = (*ppHdlEmitter)->fnctOpen(*ppHdlEmitter);
        }

        /* if OK, add to list */
        if (GOAL_RES_OK(res)) {
            GOAL_LL_APPEND(pListEmitter, (*ppHdlEmitter));
        }
    }

    return res;
}


/****************************************************************************/
/** Output parameter according to data type
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_lmEmitParameter(
   const GOAL_LM_DB_ENTRY_T *dbEntry,           /**< log message description */
   int pCnt,                                    /**< param counter */
   GOAL_LM_LOG_ENTRY_T *pLmEntry                /**< log message */
)
{
    GOAL_STATUS_T res;                          /* result */
    uint16_t *pP1Len = NULL;                    /* param 1 length */
    const char *pP1Value = NULL;                /* param 1 value */
    uint16_t *pP2Len = NULL;                    /* param 2 length */
    const char *pP2Value = NULL;                /* param 1 value */

    uint16_t *pEmParamLen = NULL;               /* emit param len */
    const char *pEmParamVal = NULL;             /* emit param val */
    GOAL_LM_EMIT_EMITTER_T *pEmitEntry;         /* emitter iterator */
    int strLen = 0;                             /* string length */
    uint8_t pType = LM_None;                    /* parameter type */

    res = goal_lmMsgParamLengths(pLmEntry, &pP1Len, &pP1Value, &pP2Len, &pP2Value);

    if (GOAL_RES_OK(res)) {

        /* select parameter to emit */
        switch(pCnt) {
            case 0:
                pEmParamLen = pP1Len;
                pEmParamVal = pP1Value;
                pType = dbEntry->p1Type;
                break;
            case 1:
                pEmParamLen = pP2Len;
                pEmParamVal  =pP2Value;
                pType = dbEntry->p2Type;
                break;
            default:
                break;
        }
    }

    /* emit value if parameter was found */
    if (NULL != pEmParamLen && NULL != pEmParamVal) {
        switch(pType) {
            case LM_GENERIC:
            case LM_STRING0:
                strLen = GOAL_SNPRINTF((char *) pEmitterBuf, GOAL_LM_EMIT_EB_SIZE, "%.*s", *pEmParamLen, pEmParamVal);
                break;
            case LM_PTR:
            case LM_PTRDIFF:
                {
                    uint64_t val;

                    GOAL_MEMCPY(&val, pEmParamVal, sizeof(uint64_t));
                    strLen = GOAL_SNPRINTF((char *) pEmitterBuf, GOAL_LM_EMIT_EB_SIZE, "%"FMT_x64, GOAL_le64toh(val));
                    break;
                }
            case LM_UINT64:
                {
                    uint64_t val;

                    GOAL_MEMCPY(&val, pEmParamVal, sizeof(uint64_t));
                    strLen = GOAL_SNPRINTF((char *) pEmitterBuf, GOAL_LM_EMIT_EB_SIZE, "%"FMT_u64, GOAL_le64toh(val));
                    break;
                }
            case LM_INT64:
                {
                    uint64_t val;

                    GOAL_MEMCPY(&val, pEmParamVal, sizeof(int64_t));
                    strLen = GOAL_SNPRINTF((char *) pEmitterBuf, GOAL_LM_EMIT_EB_SIZE, "%"FMT_i64, GOAL_le64toh(val));
                    break;
                }
             case LM_UINT32:
                {
                    uint32_t val;

                    GOAL_MEMCPY(&val, pEmParamVal, sizeof(uint32_t));
                    strLen = GOAL_SNPRINTF((char *) pEmitterBuf, GOAL_LM_EMIT_EB_SIZE, "%"FMT_u32, GOAL_le32toh(val));
                    break;
                }
            case LM_INT32:
                {
                    int32_t val;

                    GOAL_MEMCPY(&val, pEmParamVal, sizeof(int32_t));
                    strLen = GOAL_SNPRINTF((char *) pEmitterBuf, GOAL_LM_EMIT_EB_SIZE, "%"FMT_i32, GOAL_le32toh(val));
                    break;
                }
            case LM_UINT16:
                {
                    uint16_t val;

                    GOAL_MEMCPY(&val, pEmParamVal, sizeof(uint16_t));
                    strLen = GOAL_SNPRINTF((char *) pEmitterBuf, GOAL_LM_EMIT_EB_SIZE, "%u", GOAL_le16toh(val));
                    break;
                }
            case LM_INT16:
                {
                    int16_t val;

                    GOAL_MEMCPY(&val, pEmParamVal, sizeof(int16_t));
                    strLen = GOAL_SNPRINTF((char *) pEmitterBuf, GOAL_LM_EMIT_EB_SIZE, "%d", GOAL_le16toh(val));
                    break;
                }
            case LM_UINT8:
                {
                    uint8_t val;

                    GOAL_MEMCPY(&val, pEmParamVal, sizeof(uint8_t));
                    strLen = GOAL_SNPRINTF((char *) pEmitterBuf, GOAL_LM_EMIT_EB_SIZE, "%u", val);
                    break;
                }
            case LM_INT8:
                {
                    int8_t val;

                    GOAL_MEMCPY(&val, pEmParamVal, sizeof(int8_t));
                    strLen = GOAL_SNPRINTF((char *) pEmitterBuf, GOAL_LM_EMIT_EB_SIZE, "%d", val);
                    break;
                }
            case LM_FLOAT:
                {
                    double dVal;
                    float val;

                    GOAL_MEMCPY(&val, pEmParamVal, sizeof(float));

                    /* c considers double the default data type, therefore a implicit cast happens with printf */
                    dVal = (double) val;
                    val = (float) GOAL_le32toh(val);

                    strLen = GOAL_SNPRINTF((char *) pEmitterBuf, GOAL_LM_EMIT_EB_SIZE, "%f", dVal);
                    break;
                }
             case LM_MAC:
                strLen = GOAL_SNPRINTF((char *) pEmitterBuf, GOAL_LM_EMIT_EB_SIZE, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx",
                    pEmParamVal[0],pEmParamVal[1], pEmParamVal[2],
                    pEmParamVal[3],pEmParamVal[4], pEmParamVal[5]);
                break;
            case LM_BOOLEAN:
                strLen = GOAL_SNPRINTF((char *) pEmitterBuf, GOAL_LM_EMIT_EB_SIZE, "%s", *pEmParamVal ? "true" : "false");
                break;
            case LM_IPV4:
                {
                    uint32_t leVal;

                    GOAL_MEMCPY(&leVal, pEmParamVal, sizeof(uint32_t));
                    leVal = GOAL_le32toh(leVal);
                    strLen = GOAL_SNPRINTF((char *) pEmitterBuf, GOAL_LM_EMIT_EB_SIZE, "%"FMT_u32".%"FMT_u32".%"FMT_u32".%"FMT_u32,
                    (leVal >> 24) & 0xff, (leVal >> 16) & 0xff, (leVal >> 8) & 0xff, leVal & 0xff);
                    break;
                }
            case LM_None:
            default:
                strLen = 0;
                break;
        }

        EMIT_DATA();

        res = GOAL_OK;
    }
    else {
        res = GOAL_ERR_NOT_FOUND;
    }

    return res;
}


/****************************************************************************/
/** LM reader callback function
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
 static GOAL_STATUS_T goal_lmEmitReaderCb(
    struct GOAL_LM_READER_T *pHdlReader         /**< reader handle */
)
{
    UNUSEDARG(pHdlReader);

    flgUpdate = GOAL_TRUE;

    return GOAL_OK;
}


/****************************************************************************/
/** Checks if all emitters are idle
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_lmEmitAllIdle(
    void
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_LM_EMIT_EMITTER_T *pEmitEntry;         /* emitter list element */

    GOAL_LL_FOREACH(pListEmitter, pEmitEntry) {
        if (NULL != pEmitEntry->fnctIdle) {
            res = pEmitEntry->fnctIdle(pEmitEntry);
            if (GOAL_RES_ERR(res)) {
                return res;
            }
        }
    }

    return res;
}


/****************************************************************************/
/** Show content of logging buffer
 *
 * This function is meant to be used if an exception occurs and execution is
 * stopped, e.g. when entering goal_targetHalt
 *
 */
void goal_lmEmitLogDump(
    void
)
{
    /* enable message processing */
    flgUpdate = GOAL_TRUE;

    /* try to initialize resources */
    if (NULL == pReaderEmit) {
        goal_lmEmitInitPost();
    }

    /* process messages until buffer is empty */
    while (GOAL_TRUE == flgUpdate) {
        goal_lmEmitLoop();
    }
}


/****************************************************************************/
/** Set the emergency emitter
 */
void goal_lmEmitEmergencySet(
    GOAL_LM_EMIT_EMITTER_T *pEmit               /**< emitter emitter */
)
{
    mpEmitEmergency = pEmit;
    pEmit->flgEnabled = GOAL_TRUE;
}


/****************************************************************************/
/** Enable Emergency Mode
 *
 * This function disables all emitters and enables the emergency emitter to
 * allow logging output in emergency situations.
 */
void goal_lmEmitEmergencyEnable(
    void
)
{
    GOAL_LM_EMIT_EMITTER_T *pEmit;              /**< emitter iterator */

    pEmit = pListEmitter;
    pListEmitter = mpEmitEmergency;
    mpEmitEmergency = pEmit;
}


/****************************************************************************/
/** Early logging
 *
 * This function is called pre goal_loop to support early log messages
 */
void goal_lmEmitEarly(
    void
)
{
    if (GOAL_TRUE == flgInit) {
        flgUpdate = GOAL_TRUE;
        while (GOAL_TRUE == flgUpdate) {
            goal_lmEmitLoop();
        }
    }
}
#endif /* (GOAL_CONFIG_LOGGING == 1) && (GOAL_CONFIG_LM_EMIT == 1) */
