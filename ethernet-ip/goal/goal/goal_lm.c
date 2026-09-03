/** @file
 *
 * @brief Message Logger
 *
 * This module implements logging functions that build different kinds of
 * messages. There are different logging levels. For each module logging can be
 * separately activated.
 *
 * @copyright
 * Copyright 2010-2017.
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

#define GOAL_ID GOAL_ID_LOG
#include <goal_includes.h>
#include "cm/goal_lm_cm.h"

#if GOAL_CONFIG_LOGGING == 1


/****************************************************************************/
/* Constant definitions */
/****************************************************************************/
#define GOAL_LM_LOOP_TIMEOUT (1 * GOAL_TIMER_SEC)


/****************************************************************************/
/* List of local defined functions */
/****************************************************************************/
static GOAL_STATUS_T goal_lmInit(
    void
);

static GOAL_STATUS_T goal_lmInitPost(
    void
);

static GOAL_STATUS_T goal_lmShutdown(
    void
);

static uint16_t lm_headMoveToNext
(
    uint16_t head
);

static GOAL_STATUS_T goal_lmReaderPosTrack(
    uint16_t headLast,                          /**< last head position */
    uint16_t headNext                           /**< next head position */
);

static GOAL_STATUS_T goal_lmReadersInform(
    void
);

static GOAL_STATUS_T goal_lmBufferMsgPeek(
    GOAL_LM_READER_T *pReader,                  /**< reader handle */
    uint16_t *pMsgSize                          /**< message size at position */
);

static GOAL_STATUS_T goal_lmBufferTailNotReached(
    GOAL_LM_READER_T *pReader                   /**< reader handle */
);

static GOAL_STATUS_T goal_lmBufferGet(
    GOAL_LM_READER_T *pHdlReader,               /**< buffer reader */
    uint8_t *pBuffer,                           /**< destination buffer */
    uint16_t bufferSize                         /**< destination buffer size */
);

static void goal_lmLoop(
    void
);


/****************************************************************************/
/* Local defined variables */
/****************************************************************************/
static GOAL_STAGE_HANDLER_T stageInit;          /**< init stage handler */
static GOAL_STAGE_HANDLER_T stagePostInit;      /**< post init stage handler */
static GOAL_STAGE_HANDLER_T stageShutdown;      /**< shutdown stage handler */
static GOAL_BOOL_T goal_lmInitDone = GOAL_FALSE;/**< logging init done */

static uint8_t cntLmParameter;                  /**< current parameter count */
static uint16_t lmParameterLength[2];           /**< parameter lengths */
static uint8_t cntPadding;                      /**< number of padding at end */

static GOAL_RB_T *pLmRb;                        /**< ring buffer */

static GOAL_BUFFER_T *pBufLmLog;                /**< buffer for CM logging interface */

static GOAL_LM_READER_T *pListReader = NULL;    /**< linked list */
static GOAL_LOCK_T *pLockReader;                /**< lock for readers */
static GOAL_LM_READER_T *pReaderCm;             /**< logging buffer reader CM interface */
static uint16_t rbHead;                         /**< rb position of oldest log message */
static uint32_t cntMsgOverwritten;              /**< counter for overwritten messages */
static GOAL_TIMESTAMP_T tsTout;                 /**< timeout value for cyclic check */


/****************************************************************************/
/** Register init and shutdown handlers.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_lmInitPre(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* normal logging */
    res = goal_mainStageReg(GOAL_STAGE_LOG_PRE, &stageInit, GOAL_STAGE_INIT, goal_lmInit);

    /* post initialization */
    if (GOAL_RES_OK(res)) {
        res = goal_mainStageReg(GOAL_STAGE_QUEUE, &stagePostInit, GOAL_STAGE_INIT, goal_lmInitPost);
    }
    /* shutdown logging */
    if (GOAL_RES_OK(res)) {
        res = goal_mainStageReg(GOAL_STAGE_LOG_PRE, &stageShutdown, GOAL_STAGE_SHUTDOWN, goal_lmShutdown);
    }

    tsTout = goal_timerTsGet() + GOAL_LM_LOOP_TIMEOUT;

    return res;
}


/****************************************************************************/
/** Initialize the logging module.
 *
 * This function initializes the logging module.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_lmInit(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    goal_lmRegCmVars();

    /* allocate logging buffer */
    res = goal_rbInit(&pLmRb, GOAL_LM_BUFFER_SIZE);
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:134: */
        goal_lmLog(GOAL_ID_LM, 134, 105, 4, 0, GOAL_LOG_SEV_ERROR, "failed to allocate logging buffer of size: $1 bytes");
        goal_lmLogParamUINT32(GOAL_LM_BUFFER_SIZE);
        goal_lmLogFinish();
        return res;
    }
    goal_logDbg("logging buffer size: %u bytes", GOAL_LM_BUFFER_SIZE);

    res = goal_lockCreate(GOAL_LOCK_BINARY, &pLockReader, 0, 1, GOAL_ID_LM);
    if (GOAL_RES_OK(res)) {
        res = goal_lmReaderReg(&pReaderCm, NULL);
    }

    /* enable logging */
    if (GOAL_RES_OK(res)) {
        goal_lmInitDone = GOAL_TRUE;
    }

    return res;
}


/****************************************************************************/
/** Post Initialization for the logging module.
 *
 * This function requests memory pools and a loop function for cyclic checking
 * of overwritten log messages.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_lmInitPost(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    res = goal_queuePoolBufsReq(GOAL_ID_LM, GOAL_NETBUF_SIZE, 1, 0);

    /* register the loop function */
    if (GOAL_RES_OK(res)) {
        res = goal_mainLoopReg(goal_lmLoop);
    }

    return res;
}


/****************************************************************************/
/** Cyclic function which checks overwritten log messages
 *
 */
void goal_lmLoop(
    void
)
{
    GOAL_TIMESTAMP_T tsCur;                     /* current timestamp */

    /* get current timestamp */
    tsCur = goal_timerTsGet();

    if (tsCur > tsTout) {

        if (cntMsgOverwritten) {
            goal_logWarn("Attention: %"FMT_u32" log messages dropped, consider increasing logging buffer (GOAL_LM_BUFFER_SIZE)", cntMsgOverwritten);
            cntMsgOverwritten = 0;
        }

        /* update timeout value */
        tsTout = goal_timerTsGet() + GOAL_LM_LOOP_TIMEOUT;
    }
}


/****************************************************************************/
/** Shutdown Logging handling
 *
 * This function flushes the logging buffer.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
static GOAL_STATUS_T goal_lmShutdown(
    void
)
{
    goal_rbShutdown(&pLmRb);

    goal_lmInitDone = GOAL_FALSE;

    return GOAL_OK;
}


/****************************************************************************/
/** ring buffer: get data from logging buffer
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_lmBufferGet(
    GOAL_LM_READER_T *pHdlReader,               /**< buffer reader */
    uint8_t *pBuffer,                           /**< destination buffer */
    uint16_t bufferSize                         /**< destination buffer size */
)
{
    uint16_t rdDataByteNum;                     /* dummy for number of read bytes, used for ring buffers
                                                * of type GOAL_RB_TYPE_SIZE_WRAP_OVERWRITE */
    GOAL_STATUS_T res;                          /* result */
    uint16_t readPointer = 0;                   /* read pointer */

    rdDataByteNum = 0;

    /* get read position of reader */
    res = goal_lmReaderPosGet(pHdlReader, &readPointer);

    /* access ring buffer */
    if (GOAL_RES_OK(res)) {
        res = goal_rbGet(pLmRb, pBuffer, bufferSize, readPointer, &rdDataByteNum);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_lmReaderPosSet(pHdlReader, rdDataByteNum);
    }

    return res;
}


/****************************************************************************/
/** ring buffer: get current head of logging buffer
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_lmBufferHead(
    uint16_t *pBufferHead                       /**< resulting buffer head */
)
{
    *pBufferHead = rbHead;
    return GOAL_OK;
}


/****************************************************************************/
/** ring buffer: get next message from logging buffer
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_lmBufferMsgGet(
    GOAL_LM_READER_T *pReader,                  /**< reader handle */
    uint8_t *pBuffer,                           /**< destination buffer */
    uint16_t bufferSize,                        /**< destination buffer size */
    uint16_t *pMsgLen                           /**< message length */
)
{
    static GOAL_BOOL_T flgOverflow = GOAL_FALSE; /* overflow once-detection flag */
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    res = goal_lockGet(pLockReader, GOAL_LOCK_INFINITE);

    if (NULL == pMsgLen || NULL == pBuffer) {
        res = GOAL_ERR_PARAM;
    }

    if (GOAL_RES_OK(res)) {
        res = goal_lmBufferTailNotReached(pReader);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_lmBufferMsgPeek(pReader, pMsgLen);
    }

    if (GOAL_RES_OK(res)) {
        if (*pMsgLen > bufferSize) {

            /* print message only once */
            if (GOAL_FALSE == flgOverflow) {
                goal_logWarn("message larger than log buffer");
                flgOverflow = GOAL_TRUE;
            }

            /* skip message */
            res = GOAL_ERR_OVERFLOW;
        }
        else {
            res = goal_lmBufferGet(pReader, pBuffer, *pMsgLen);
        }
    }

    goal_lockPut(pLockReader);

    return res;
}


/****************************************************************************/
/** ring buffer: check if a reader reached the tail of the buffer
 *
 * @retval GOAL_OK tail not reached
 * @retval other failed
 */
static GOAL_STATUS_T goal_lmBufferTailNotReached(
    GOAL_LM_READER_T *pReader                   /**< reader handle */
)
{
    uint16_t writePointer = 0;                  /* write pointer */
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    /* check if write Pointer of ring buffer exceeds the given read pointer */
    if (GOAL_RES_OK(res)) {
        res = goal_rbWpGet(pLmRb, &writePointer);
    }

    if (GOAL_RES_OK(res)) {
        if (pReader->readPointer == writePointer) {
            return GOAL_ERR_EMPTY;
        }
    }

    return GOAL_OK;
}


/****************************************************************************/
/** ring buffer: check if there is a new logging message at given position
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_lmBufferMsgPeek(
    GOAL_LM_READER_T *pReader,                  /**< reader handle */
    uint16_t *pMsgSize                          /**< message size at position */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_LM_LOG_ENTRY_T lmEntryHead;            /* log message header */
    uint16_t lmEntryParameter1Length;           /* parameter length */
    uint16_t msgLen;                            /* message length */
    uint16_t rdDataByteNum;                     /* read length */
    uint16_t readPointer;                       /* read pointer */

    if (GOAL_RES_OK(res)) {
        /* there is a new message, start calculation of length */
        rdDataByteNum = 0;
        msgLen = 0;

        readPointer = pReader->readPointer;

        /* evaluate message header */
        goal_rbGet(pLmRb, (uint8_t *) &lmEntryHead, sizeof(lmEntryHead), readPointer, &rdDataByteNum);

        /* skip header */
        readPointer += sizeof(lmEntryHead);
        /* skip function name */
        readPointer += lmEntryHead.nameLength;
        readPointer = readPointer % GOAL_LM_BUFFER_SIZE;
        msgLen += sizeof(lmEntryHead);
        msgLen += lmEntryHead.nameLength;

        /* evaluate message parameters */
        if (lmEntryHead.paramsLength != 0) {

            /* check number of parameters */
            goal_rbGet(pLmRb, (uint8_t *) &lmEntryParameter1Length, sizeof(uint16_t), readPointer, &rdDataByteNum);
            if (lmEntryParameter1Length != lmEntryHead.paramsLength) {
                /* two parameters */
                readPointer += lmEntryHead.paramsLength + 4;
                msgLen += lmEntryHead.paramsLength + 4;
            }
            else {
                /* one parameter */
                readPointer += lmEntryHead.paramsLength + 2;
                msgLen += lmEntryHead.paramsLength + 2;
            }
        }

        /* correct length according to padding */
        if (lmEntryHead.flags & GOAL_LM_FLAG_PAD) {
            readPointer++;
            msgLen++;
        }
        readPointer = readPointer % GOAL_LM_BUFFER_SIZE;

        /* message is found, starts ad readStart, ends at readPointer */
        *pMsgSize = msgLen;
    }

    return res;
}


/****************************************************************************/
/** ring buffer: put one message into logging buffer
 *
 * @returns status
 */
GOAL_STATUS_T goal_lmBufferMsgPut(
    uint8_t *pBuffer,                           /**< destination buffer */
    uint16_t msgLen,                            /**< legth of complete msg */
    uint16_t msgLenToWrite,                     /**< length to wirte */
    GOAL_BOOL_T flgLockKeep                     /**< flag if message is finished later */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    uint16_t writePointerStart;                 /* start of write */
    uint16_t writePointerEnd;                   /* end of write */
    uint16_t head;                              /* pointer to head */
    uint16_t headNext;                          /* pointer to head */

    if (NULL == pBuffer || 0 == msgLen || 0 == msgLenToWrite) {
        res = GOAL_ERR_PARAM;
    }

    /* lock for readers */
    if (GOAL_RES_OK(res)) {
        res = goal_lockGet(pLockReader, GOAL_LOCK_INFINITE);
    }

    if (GOAL_RES_OK(res)) {
        /* check if head will be overwritten */
        head = rbHead;
        goal_rbWpGet(pLmRb, &writePointerStart);
        writePointerEnd = writePointerStart + msgLen;

        writePointerEnd = (writePointerEnd % GOAL_LM_BUFFER_SIZE);

        /* only update head when buffer was overwritten once */

        if (GOAL_TRUE == goal_rbTouched(pLmRb)) {

            /* check if write operation overwrites existing entries */
            if (writePointerStart < writePointerEnd) {
                /* normal order */
                while (writePointerStart <= head && writePointerEnd >= head) {
                    /* increment head to next message in buffer */

                    headNext = lm_headMoveToNext(head);
                    /* track read position */
                    goal_lmReaderPosTrack(head, headNext);
                    head = headNext;
                }
            }
            else {
                /* inverse order */
                while (writePointerStart <= head || writePointerEnd >= head) {
                   /* increment head to next message in buffer */

                    headNext = lm_headMoveToNext(head);
                    /* track read position */
                    goal_lmReaderPosTrack(head, headNext);
                    head = headNext;
                }
            }
        }

        res = goal_rbPut(pLmRb, pBuffer, msgLenToWrite, flgLockKeep);

        /* if message was completed, inform readers */
        if (flgLockKeep == GOAL_FALSE && GOAL_RES_OK(res)) {
            res = goal_lmReadersInform();
        }

        /* if message was completed, unlock */
        if (flgLockKeep == GOAL_FALSE) {
            goal_lockPut(pLockReader);
        }

    }

    return res;
}


/****************************************************************************/
/** move head to next logging message
 *
 * @returns new head
 */
static uint16_t lm_headMoveToNext
(
    uint16_t head
)
{
    uint16_t headNext;                          /* next head position */
    GOAL_STATUS_T res;                          /* result */
    uint16_t msgLen;                            /* message length at head */
    GOAL_LM_READER_T reader;                    /* virtual reader for message peek */

    /* return old value in case there is no message */
    headNext = head;

    reader.readPointer = head;
    res = goal_lmBufferMsgPeek(&reader, &msgLen);
    if (GOAL_RES_OK(res)) {
        headNext = head + msgLen;
        headNext = headNext % GOAL_LM_BUFFER_SIZE;
        rbHead = headNext;
    }

    return headNext;
}


/****************************************************************************/
/** Logging function
 *
 * This function logs messages
 */
void goal_lmLogInternal(
    uint32_t logId,                             /**< log id */
    uint16_t param1Length,                      /**< parameter length */
    uint16_t param2Length,                      /**< parameter length */
    uint16_t lineNumber,                        /**< line number in code */
    const char *strNameFunc                     /**< name of calling function */
)
{
    GOAL_LM_LOG_ENTRY_T logEntry;               /* log entry */
    GOAL_BOOL_T flgLockKeep = GOAL_FALSE;       /* flag for log keeping */
    uint16_t msgLen;                            /* length of new log message */

    if (GOAL_TRUE == goal_lmInitDone) {

        /* parameters -> keep further locking */
        if (0 != (param1Length + param2Length)) {
            flgLockKeep = GOAL_TRUE;
        }

        /* prepare message data */
        logEntry.logId = logId;
        logEntry.timestamp = (uint32_t) (goal_timerTsGet() / 1000);
        logEntry.paramsLength = param1Length + param2Length;
        logEntry.lineNumber = lineNumber;
        logEntry.flags = 0;
        msgLen = sizeof(logEntry);

        /* fill name length if a valid string was given */
        if (NULL != strNameFunc) {
            logEntry.nameLength = (uint8_t) GOAL_STRLEN(strNameFunc);
            msgLen += logEntry.nameLength;
        }
        else {
            logEntry.nameLength = 0;
        }

        if (param1Length > 0) {
            msgLen += 2;
            msgLen += param1Length;
        }

        if (param2Length > 0) {
            msgLen += 2;
            msgLen += param2Length;
        }

        if (0 != (msgLen % 2)) {
            logEntry.flags |= GOAL_LM_FLAG_PAD;
            msgLen += 1;
            cntPadding = 1;
        }
        else {
            logEntry.flags = 0;
            cntPadding = 0;
        }

        /* add GOAL_LM_FLAG_EXT flag if code is running on application core */
#if GOAL_CONFIG_GEN_CTC_AC == 1
        logEntry.flags |= GOAL_LM_FLAG_EXT;
#endif

        goal_lmBufferMsgPut((uint8_t *) &logEntry, msgLen, sizeof(logEntry), GOAL_TRUE);
        if (logEntry.nameLength > 0) {
            goal_rbPutFast(pLmRb, (const uint8_t *) strNameFunc, (uint16_t) GOAL_STRLEN(strNameFunc));
        }
        if (GOAL_FALSE == flgLockKeep) {
            goal_lmLogPad();
            goal_lmLogFinishInternal();
        }

        /* set param length arrays here within protected context */
        lmParameterLength[0] = param1Length;
        lmParameterLength[1] = param2Length;

        /* initialize parameter propagation to parameter logging functions */
        cntLmParameter = 0;
    }
}


/****************************************************************************/
/** Logging finish function
 *
 * This function will free a locked logging buffer when additional parameters
 * have been written.
 *
 */
void goal_lmLogFinishInternal(
    void
)
{
    if (goal_lmInitDone) {
        goal_rbPutFastFinish(pLmRb);
        goal_lmReadersInform();

        /* access completed, unlock now */
        goal_lockPut(pLockReader);
    }
}


/****************************************************************************/
/** Logging parameter function
 *
 */
void goal_lmLogParamINT8Impl(
    int8_t parameter                            /**< parameter */
)
{
    if (goal_lmInitDone && cntLmParameter < 2) {
        goal_rbPutFast(pLmRb, (uint8_t *) &lmParameterLength[cntLmParameter], 2);
        goal_rbPutFast(pLmRb, (uint8_t *) &parameter, 1);
        cntLmParameter++;
    }
}


/****************************************************************************/
/** Logging parameter function
 *
 */
void goal_lmLogParamUINT8Impl(
    uint8_t parameter                           /**< parameter */
)
{
    if (goal_lmInitDone && cntLmParameter < 2) {
        goal_rbPutFast(pLmRb, (uint8_t *) &lmParameterLength[cntLmParameter], 2);
        goal_rbPutFast(pLmRb, (uint8_t *) &parameter, 1);
        cntLmParameter++;
    }
}


/****************************************************************************/
/** Logging parameter function
 *
 */
void goal_lmLogParamINT16Impl(
    int16_t parameter                           /**< parameter */
)
{
    parameter = GOAL_htole16(parameter);
    if (goal_lmInitDone && cntLmParameter < 2) {
        goal_rbPutFast(pLmRb, (uint8_t *) &lmParameterLength[cntLmParameter], 2);
        goal_rbPutFast(pLmRb, (uint8_t *) &parameter, 2);
        cntLmParameter++;
    }
}


/****************************************************************************/
/** Logging parameter function
 *
 */
void goal_lmLogParamUINT16Impl(
    uint16_t parameter                          /**< parameter */
)
{
    parameter = GOAL_htole16(parameter);
    if (goal_lmInitDone && cntLmParameter < 2) {
        goal_rbPutFast(pLmRb, (uint8_t *) &lmParameterLength[cntLmParameter], 2);
        goal_rbPutFast(pLmRb, (uint8_t *) &parameter, 2);
        cntLmParameter++;
    }
}


/****************************************************************************/
/** Logging parameter function
 *
 */
void goal_lmLogParamINT32Impl(
    int32_t parameter                           /**< parameter */
)
{
    parameter = GOAL_htole32(parameter);
    if (goal_lmInitDone && cntLmParameter < 2) {
        goal_rbPutFast(pLmRb, (uint8_t *) &lmParameterLength[cntLmParameter], 2);
        goal_rbPutFast(pLmRb, (uint8_t *) &parameter, 4);
        cntLmParameter++;
    }
}


/****************************************************************************/
/** Logging parameter function
 *
 */
void goal_lmLogParamUINT32Impl(
    uint32_t parameter                          /**< parameter */
)
{
    parameter = GOAL_htole32(parameter);
    if (goal_lmInitDone && cntLmParameter < 2) {
        goal_rbPutFast(pLmRb, (uint8_t *) &lmParameterLength[cntLmParameter], 2);
        goal_rbPutFast(pLmRb, (uint8_t *) &parameter, 4);
        cntLmParameter++;
    }
}


/****************************************************************************/
/** Logging parameter function
 *
 */
void goal_lmLogParamIPV4Impl(
    uint32_t parameter                          /**< parameter */
)
{
    parameter = GOAL_htole32(parameter);
    if (goal_lmInitDone && cntLmParameter < 2) {
        goal_rbPutFast(pLmRb, (uint8_t *) &lmParameterLength[cntLmParameter], 2);
        goal_rbPutFast(pLmRb, (uint8_t *) &parameter, 4);
        cntLmParameter++;
    }
}


/****************************************************************************/
/** Logging parameter function
 *
 */
void goal_lmLogParamMACImpl(
    uint8_t *pParameter                         /**< parameter */
)
{
    if (goal_lmInitDone && cntLmParameter < 2) {
        goal_rbPutFast(pLmRb, (uint8_t *) &lmParameterLength[cntLmParameter], 2);
        goal_rbPutFast(pLmRb, (uint8_t *) pParameter, 6);
        cntLmParameter++;
    }
}


/****************************************************************************/
/** Logging parameter function
 *
 */
void goal_lmLogParamPTRImpl(
    const char *pParameter                      /**< parameter */
)
{
    uint64_t val;                               /* ptr value */

    val = GOAL_htole64((PtrCast) pParameter);

    if (goal_lmInitDone && cntLmParameter < 2) {
        goal_rbPutFast(pLmRb, (uint8_t *) &lmParameterLength[cntLmParameter], 2);
        goal_rbPutFast(pLmRb, (uint8_t *) &val, 8);
        cntLmParameter++;
    }
}


/****************************************************************************/
/** Logging parameter function
 *
 */
void goal_lmLogParamPTRDIFFImpl(
    uint64_t parameter                          /**< parameter */
)
{
    parameter = GOAL_htole64(parameter);
    if (goal_lmInitDone && cntLmParameter < 2) {
        goal_rbPutFast(pLmRb, (uint8_t *) &lmParameterLength[cntLmParameter], 2);
        goal_rbPutFast(pLmRb, (uint8_t *) &parameter, 8);
        cntLmParameter++;
    }
}


/****************************************************************************/
/** Logging parameter function
 *
 */
void goal_lmLogParamBOOLEANImpl(
    GOAL_BOOL_T parameter                       /**< parameter */
)
{
    if (goal_lmInitDone && cntLmParameter < 2) {
        goal_rbPutFast(pLmRb, (uint8_t *) &lmParameterLength[cntLmParameter], 2);
        goal_rbPutFast(pLmRb, (uint8_t *) &parameter, 1);
        cntLmParameter++;
    }
}


/****************************************************************************/
/** Logging parameter function
 *
 */
void goal_lmLogParamSTRING0Impl(
    const char *pParameter                      /**< parameter */
)
{
    if (goal_lmInitDone && cntLmParameter < 2) {
        goal_rbPutFast(pLmRb, (uint8_t *) &lmParameterLength[cntLmParameter], 2);
        goal_rbPutFast(pLmRb, (const uint8_t *) pParameter, lmParameterLength[cntLmParameter]);
        cntLmParameter++;
    }
}


/****************************************************************************/
/** Logging parameter function
 *
 */
void goal_lmLogParamGENERICImpl(
    const char *pParameter                      /**< parameter */
)
{
    if (goal_lmInitDone && cntLmParameter < 2) {
        goal_rbPutFast(pLmRb, (uint8_t *) &lmParameterLength[cntLmParameter], 2);
        goal_rbPutFast(pLmRb, (const uint8_t *) pParameter, lmParameterLength[cntLmParameter]);
        cntLmParameter++;
    }
}


/****************************************************************************/
/** Logging parameter function
 *
 */
void goal_lmLogParamINT64Impl(
    int64_t parameter                           /**< parameter */
)
{
    parameter = GOAL_htole64(parameter);
    if (goal_lmInitDone && cntLmParameter < 2) {
        goal_rbPutFast(pLmRb, (uint8_t *) &lmParameterLength[cntLmParameter], 2);
        goal_rbPutFast(pLmRb, (uint8_t *) &parameter, 8);
        cntLmParameter++;
    }
}


/****************************************************************************/
/** Logging parameter function
 *
 */
void goal_lmLogParamUINT64Impl(
    uint64_t parameter                          /**< parameter */
)
{
    parameter = GOAL_htole64(parameter);
    if (goal_lmInitDone && cntLmParameter < 2) {
        goal_rbPutFast(pLmRb, (uint8_t *) &lmParameterLength[cntLmParameter], 2);
        goal_rbPutFast(pLmRb, (uint8_t *) &parameter, 8);
        cntLmParameter++;
    }
}


/****************************************************************************/
/** Logging parameter function
 *
 */
void goal_lmLogParamFLOATImpl(
    float parameter                             /**< parameter */
)
{
    if (goal_lmInitDone && cntLmParameter < 2) {
        goal_rbPutFast(pLmRb, (uint8_t *) &lmParameterLength[cntLmParameter], 2);
        goal_rbPutFast(pLmRb, (uint8_t *) &parameter, 4);
        cntLmParameter++;
    }
}


/****************************************************************************/
/** cm callback for variable change
 *
 * @returns status
 */
GOAL_STATUS_T LM_onLogAccess(
    uint32_t modId,                             /**< module ID */
    uint32_t varId,                             /**< variable Id */
    GOAL_CM_VAR_T *pVar                         /**< variable pointer */
)
{
    uint16_t value;                             /* variable value */
    uint16_t cntBytes;                          /* bytes counter */
    GOAL_CM_VAR_T *pCmBufVar = NULL;            /* logging buffer */
    GOAL_CM_VAR_T *pCmCntVar = NULL;            /* logging counter */
    GOAL_STATUS_T res;                          /* result */
    static uint16_t cntFragment;                /* counter for fragmented message */
    static uint16_t cntOffset;                  /* offset counter for fragmentation */

    UNUSEDARG(modId);
    UNUSEDARG(varId);

    cntBytes = 0;

    /* request initial goal buffer */
    if (pBufLmLog == NULL) {
        res = goal_queuePoolGetBuf(&pBufLmLog, GOAL_NETBUF_SIZE, GOAL_ID_LM);
    }
    else {
        res = GOAL_OK;
    }

    /* get logging buffer from CM */
    if (GOAL_RES_OK(res)) {
        res = goal_cmGetVarById(GOAL_CM_LM_MOD_ID, LM_CM_VAR_READBUFFER, &pCmBufVar);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_cmGetVarById(GOAL_CM_LM_MOD_ID, LM_CM_VAR_CNT, &pCmCntVar);
    }

    if (GOAL_RES_OK(res)) {
        value = GOAL_CM_VAR_UINT16(pVar);

        if (0x0000 == value) {
            /* reset read pointer */
            goal_lmReaderReset(pReaderCm);
            cntFragment = 0;
        }

        if (0xFFFF == value) {
            /* get new bytes from buffer */

                cntBytes = 0;

                /* before reading new message, process last fragmented message */
                if (cntFragment > 0) {

                    cntBytes = cntFragment;

                    /* check if fragment needs to be fragmented again */
                    if (cntBytes > pCmBufVar->totalSize) {
                        /* calculate remaining length */
                        cntFragment = (uint16_t) (cntBytes - (uint16_t) pCmBufVar->totalSize);
                        cntBytes = (uint16_t) pCmBufVar->totalSize;
                    }
                    else {
                        cntFragment = 0;
                    }

                    GOAL_CM_CLEARVAR_GENERIC(pCmBufVar, 0, 1);
                    GOAL_CM_SETVAR_GENERIC(pCmBufVar, pBufLmLog->ptrData + cntOffset, cntBytes);
                    cntOffset += cntBytes;
                }
                else {
                    res = goal_lmBufferMsgGet(pReaderCm, pBufLmLog->ptrData, pBufLmLog->bufSize, &cntBytes);
                    if (GOAL_RES_OK(res)) {

                        /* check if message needs to be fragmented */
                        if (cntBytes > pCmBufVar->totalSize) {
                            /* calculate remaining length */
                            cntFragment = (uint16_t) (cntBytes - (uint16_t) pCmBufVar->totalSize);
                            cntBytes = (uint16_t) pCmBufVar->totalSize;
                            cntOffset = cntBytes;
                        }

                        GOAL_CM_CLEARVAR_GENERIC(pCmBufVar, 0, 1);
                        GOAL_CM_SETVAR_GENERIC(pCmBufVar, pBufLmLog->ptrData, cntBytes);
                    }
                }


                if (res == GOAL_ERR_EMPTY) {
                    /* if buffer is empty, don't propagate an error */
                    res = GOAL_OK;
                }

            GOAL_CM_SETVAR_UINT16(pCmCntVar, cntBytes);
        }
    }

    return res;
}


#if GOAL_CONFIG_LM_EXCEPTION_LOG == 0
GOAL_STATUS_T LM_exLogAccess(
    uint32_t modId,                             /**< module ID */
    uint32_t varId,                             /**< variable Id */
    GOAL_CM_VAR_T *pVar                         /**< variable pointer */
)
{
    UNUSEDARG(modId);
    UNUSEDARG(varId);
    UNUSEDARG(pVar);

    return GOAL_ERR_UNSUPPORTED;
}
#endif


/****************************************************************************/
/** Legacy Logging api function
 *
 * This function logs messages from the legacy logging system
 */
void goal_lmLogLegacy(
    uint8_t level,                              /**< logging level */
    const char *strLog,                         /**< logging string */
    uint16_t len                                /**< logging string length */
)
{
    uint16_t lenstrLog;                         /* string length for logging call */

    lenstrLog = len;

    if (goal_lmInitDone == GOAL_TRUE) {
        switch (level) {
            case GOAL_DBG_ERR: /* Error */
                /* GG_LOG ID:20: */
                goal_lmLog(GOAL_ID_LM, 20, 12, (lenstrLog), 0, GOAL_LOG_SEV_ERROR, "$1");
                goal_lmLogParamGENERIC(strLog);
                goal_lmLogFinish();
                break;
            case GOAL_DBG_WARN: /* Warning */
                /* GG_LOG ID:19: */
                goal_lmLog(GOAL_ID_LM, 19, 12, (lenstrLog), 0, GOAL_LOG_SEV_WARNING, "$1");
                goal_lmLogParamGENERIC(strLog);
                goal_lmLogFinish();
                break;
            default:
            case GOAL_DBG_INFO: /* Info */
                /* GG_LOG ID:18: */
                goal_lmLog(GOAL_ID_LM, 18, 12, (lenstrLog), 0, GOAL_LOG_SEV_INFO, "$1");
                goal_lmLogParamGENERIC(strLog);
                goal_lmLogFinish();
                break;
            case GOAL_DBG_DBG: /* Debug */
                /* GG_LOG ID:17: */
                goal_lmLog(GOAL_ID_LM, 17, 12, (lenstrLog), 0, GOAL_LOG_SEV_DEBUG, "$1");
                goal_lmLogParamGENERIC(strLog);
                goal_lmLogFinish();
                break;
        }
    }
}


#if GOAL_CONFIG_LM_EXCEPTION_LOG == 0
/****************************************************************************/
/** Exception logging erase function
 *
 * This function erases the exception log, initiated by CM access
 */
GOAL_STATUS_T LM_exLogErase(
    uint32_t modId,                             /**< module ID */
    uint32_t varId,                             /**< variable Id */
    GOAL_CM_VAR_T *pVar                         /**< variable pointer */
)
{
    UNUSEDARG(modId);
    UNUSEDARG(varId);
    UNUSEDARG(pVar);

    return GOAL_ERR_UNSUPPORTED;
}
#endif


/****************************************************************************/
/** Insert padding if required
 *
 */
void goal_lmLogPad(
    void
)
{
    uint8_t bytePad = 0;                        /* padding byte */

    if (goal_lmInitDone && 0 != cntPadding) {
        goal_rbPutFast(pLmRb, (uint8_t *) &bytePad, 1);
    }
}


/****************************************************************************/
/** Register a reader of the logging buffer
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_lmReaderReg(
    GOAL_LM_READER_T **ppHdlReader,             /**< reader handle */
    GOAL_LM_READER_CB fnctCb                    /**< reader callback */
)
{
    GOAL_STATUS_T res;                          /* result */

    res = goal_memCalloc(ppHdlReader, sizeof(GOAL_LM_READER_T));

    if (GOAL_RES_OK(res)) {

        /* set read pointer to head */
        (*ppHdlReader)->readPointer = rbHead;

        /* set callback */
        (*ppHdlReader)->fnctCb = fnctCb;

        GOAL_LL_APPEND(pListReader, (*ppHdlReader));
        pListReader->cntReaders++;
    }

    return res;
}


/****************************************************************************/
/** Reset a reader of the logging buffer
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_lmReaderReset(
    GOAL_LM_READER_T *pHdlReader                /**< reader handle */
)
{
    GOAL_STATUS_T res;                          /* result */

    /* lock for readers */
    res = goal_lockGet(pLockReader, GOAL_LOCK_INFINITE);

    if (GOAL_RES_OK(res) && NULL != pHdlReader) {
        pHdlReader->readPointer = rbHead;
        res = GOAL_OK;
    }
    else {
        res = GOAL_ERR_NOT_INITIALIZED;
    }

    goal_lockPut(pLockReader);

    return res;
}


/****************************************************************************/
/** Get read position of a reader of the logging buffer
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_lmReaderPosGet(
    GOAL_LM_READER_T *pHdlReader,               /**< reader handle */
    uint16_t *pReadPointer                      /**< reader read position */
)
{
    GOAL_STATUS_T res;                          /* result */

    /* lock for readers */
    res = goal_lockGet(pLockReader, GOAL_LOCK_INFINITE);

    if (GOAL_RES_OK(res) && (NULL != pHdlReader) && (NULL != pReadPointer)) {
        *pReadPointer = pHdlReader->readPointer;
        res = GOAL_OK;
    }
    else {
        res = GOAL_ERR_NOT_INITIALIZED;
    }

    goal_lockPut(pLockReader);

    return res;
}


/****************************************************************************/
/** Set read position of a reader of the logging buffer
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_lmReaderPosSet(
    GOAL_LM_READER_T *pHdlReader,               /**< reader handle */
    uint16_t readPointer                        /**< reader read position */
)
{
    GOAL_STATUS_T res;                          /* result */

    /* lock for readers */
    res = goal_lockGet(pLockReader, GOAL_LOCK_INFINITE);

    if (GOAL_RES_OK(res) && NULL != pHdlReader) {

        readPointer = readPointer % GOAL_LM_BUFFER_SIZE;
        pHdlReader->readPointer = readPointer;
        res = GOAL_OK;
    }
    else {
        res = GOAL_ERR_NOT_INITIALIZED;
    }

    goal_lockPut(pLockReader);

    return res;
}


/****************************************************************************/
/** Track read positions of all readers of the logging buffer
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
static GOAL_STATUS_T goal_lmReaderPosTrack(
    uint16_t headLast,                          /**< last head position */
    uint16_t headNext                           /**< next head position */
)
{
    GOAL_LM_READER_T *pEntry;                   /* reader entry */
    GOAL_STATUS_T res;                          /* result */
    uint16_t cntReadersMissedMsg = 0;           /* counter of missed log messages */

    /* lock for readers */
    res = goal_lockGet(pLockReader, GOAL_LOCK_INFINITE);

    if (GOAL_RES_OK(res)) {
        /* check read position of all readers and move if head moving
           overlaps with the read position */
        GOAL_LL_FOREACH(pListReader, pEntry) {

            if (headNext > headLast) {
                if ((pEntry->readPointer >= headLast) &&
                    (pEntry->readPointer < headNext)) {
                    pEntry->readPointer = headNext;
                    cntReadersMissedMsg++;
                }
            }
            else {
                if ((pEntry->readPointer >= headLast) ||
                    (pEntry->readPointer < headNext)) {
                    pEntry->readPointer = headNext;
                    cntReadersMissedMsg++;
                }
            }
        }

        /* if no reader saw this message, note this */
        if (pListReader->cntReaders == cntReadersMissedMsg) {
            cntMsgOverwritten++;
        }

    }

    goal_lockPut(pLockReader);

    return res;
}


/****************************************************************************/
/** Determine length and position of parameters in lm logging message
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_lmMsgParamLengths(
    GOAL_LM_LOG_ENTRY_T *pLmEntry,              /**< lm log message */
    uint16_t **ppP1Len,                         /**< param 1 length */
    const char **ppP1Value,                     /**< param 1 value */
    uint16_t **ppP2Len,                         /**< param 1 length */
    const char **ppP2Value                      /**< param 1 value */
)
{
    GOAL_STATUS_T res;                          /* result */

    /* initiate return values */
    *ppP1Len = NULL;
    *ppP2Len = NULL;
    *ppP1Value = NULL;
    *ppP2Value = NULL;

    if (pLmEntry->paramsLength > 0) {

        /* determine length and value of p1 and p2 */
        *ppP1Len = (uint16_t *) ((char *) pLmEntry + sizeof(GOAL_LM_LOG_ENTRY_T) + pLmEntry->nameLength);
        *ppP1Value = (char *) *ppP1Len + 2;
        if (**ppP1Len != pLmEntry->paramsLength) {
            *ppP2Len = (uint16_t *) ((char *) *ppP1Len + 2 + **ppP1Len);
            *ppP2Value = (char *) *ppP2Len + 2;
        }

        res = GOAL_OK;
    }
    else {
        res = GOAL_ERR_NOT_FOUND;
    }

    return res;
}


/****************************************************************************/
/** Determine length of lm logging message
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_lmMsgLength(
    GOAL_LM_LOG_ENTRY_T *pLmEntry,              /**< lm log message */
    size_t *pLen                                /**< [out] length */
)
{
    GOAL_STATUS_T res;                          /* result */
    size_t len;                                 /* length */
    uint16_t *pLenP1;                           /* param 1 length */
    const char *pValP1;                         /* param 1 value */
    uint16_t *pLenP2;                           /* param 2 length */
    const char *pValP2;                         /* param  value */

    len = 0;
    res = goal_lmMsgParamLengths(pLmEntry, &pLenP1, &pValP1, &pLenP2, &pValP2);

    /* calculate message length */
    if (GOAL_RES_OK(res)) {
        len += sizeof(GOAL_LM_LOG_ENTRY_T);
        len += pLmEntry->nameLength;
        if (NULL != pLenP1) {
            len += *pLenP1;
            len += 2;
        }

        if (NULL != pLenP2) {
            len += *pLenP2;
            len += 2;
        }

        if (pLmEntry->flags & GOAL_LM_FLAG_PAD) {
            len += 1;
        }
    }

    *pLen = len;

    return res;
}


/****************************************************************************/
/** Inform readers which registered a callback about new data
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
static GOAL_STATUS_T goal_lmReadersInform(
    void
)
{
    GOAL_LM_READER_T *pEntry;                   /* reader entry */

    GOAL_LL_FOREACH(pListReader, pEntry) {
        if (NULL != pEntry->fnctCb) {
            pEntry->fnctCb((struct GOAL_LM_READER_T *) pEntry);
        }
    }

    return GOAL_OK;
}
#endif /* GOAL_CONFIG_LOGGING == 1 */
