/** @file
 *
 * @brief ring buffer
 *
 * This module implements functions for ring buffers.
 * The ring buffer consists of a header to manage the ring buffer
 * and a buffer for the data. The access to the ring buffer is protected
 * by the GOAL locking mechanism.
 *
 * @copyright
 * Copyright 2017-2018.
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


/****************************************************************************/
/** Initialize the ring buffer
 *
 * This function allocates the memory for the ring buffer and initializes
 * the header of the ring buffer.
 *
 * @return
 * GOAL return status
 */
GOAL_STATUS_T goal_rbInit(
    GOAL_RB_T **ppRb,                           /**< return reference to ring buffer */
    uint16_t size                               /**< storage size of ring buffer */
)
{
    GOAL_STATUS_T res;                          /* result */
    uint8_t *pBuf;                              /* allocation buffer */

    res = goal_memCalloc(ppRb, sizeof(GOAL_RB_T));
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:231: */
        goal_lmLog(GOAL_ID_RB, 231, 186, 0, 0, GOAL_LOG_SEV_ERROR, "failed to allocate rb handle");
        return res;
    }

    /* init logging mutex */
    res = goal_lockCreate(GOAL_LOCK_BINARY, &(*ppRb)->pMtx, 0, 1, GOAL_ID_LOG);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* allocate logging buffer */
    res = goal_memCalloc(&pBuf, size);
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:232: */
        goal_lmLog(GOAL_ID_RB, 232, 187, 0, 0, GOAL_LOG_SEV_ERROR, "failed to allocate logging buffer");
        return res;
    }

    (*ppRb)->pBuffer = pBuf;
    (*ppRb)->size = size;
    (*ppRb)->wp = 0;
    (*ppRb)->flgTouched = GOAL_FALSE;

    return res;
}


/****************************************************************************/
/** Shutdown of ring buffer
 *
 * This funtion deinitializes the ring buffer and frees the memory.
 */
void goal_rbShutdown(
    GOAL_RB_T **ppRb                            /**< return reference to ring buffer */
)
{
    if (NULL != ppRb) {
        /* delete mutex */
        goal_lockDelete((*ppRb)->pMtx);

        /* free buffer */
        goal_memFree(&(*ppRb)->pBuffer);
        goal_memFree(ppRb);
    }
}


/****************************************************************************/
/** Ringbuffer: Put data
 *
 * If flgLockKeep is set to GOAL_TRUE, the ring buffer keeps locked
 * to continue writing data into the ring buffer.
 * In this case call function goal_rbPutFastFinish() to finish
 * the continous writing and to unlock the ring buffer.
 *
 * @return
 * GOAL return status
 */
GOAL_STATUS_T goal_rbPut(
    GOAL_RB_T *pRb,                             /**< ring buffer */
    const uint8_t *pData,                       /**< message buffer */
    uint16_t size,                              /**< message size */
    GOAL_BOOL_T flgLockKeep                     /**< keep log flag */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    if (NULL != pRb->pMtx) {
        res = goal_lockGet(pRb->pMtx, GOAL_LOCK_INFINITE);
    }

    if (GOAL_RES_OK(res)) {
         res = goal_rbPutFast(pRb, pData, size);
    }

    if (GOAL_FALSE == flgLockKeep) {
        if (NULL != pRb->pMtx) {
            goal_lockPut(pRb->pMtx);
        }
    }

    return res;
}


/****************************************************************************/
/** Get unread data
 *
 * This function gets all unread data from the ring buffer.
 *
 * @return
 * GOAL return status
 */
GOAL_STATUS_T goal_rbGet(
    GOAL_RB_T *pRb,                             /**< ring buffer */
    uint8_t *pDestBuffer,                       /**< destination buffer */
    uint16_t destBufferSize,                    /**< size of destination buffer */
    uint16_t readPointer,                       /**< read pointer of reader */
    uint16_t *pReturn                           /**< return value */
)
{
    uint16_t cntBytes = 0;                      /* counter for bytes to copy */
    uint16_t cntRemain;                         /* remaining bytes after overlap */
    GOAL_STATUS_T res = GOAL_OK;                /* status */

    goal_rbCntGet(pRb, readPointer, &cntBytes);

    if (cntBytes > destBufferSize) {
        cntBytes = destBufferSize;
    }

    /* will write operation wrap ? */
    if ((readPointer + cntBytes) > pRb->size) {
        /* split copy */

        cntRemain = (readPointer + cntBytes) % pRb->size;

        GOAL_MEMCPY(pDestBuffer, &pRb->pBuffer[readPointer], cntBytes - cntRemain);
        GOAL_MEMCPY(&pDestBuffer[cntBytes - cntRemain], &pRb->pBuffer[0], cntRemain);
    }
    else {
        /* single copy */
        GOAL_MEMCPY(pDestBuffer, &pRb->pBuffer[readPointer], cntBytes);

        /* remember new read pointer */
        cntRemain = (readPointer + cntBytes) % pRb->size;
    }

    if (NULL != pReturn) {
        *pReturn = cntRemain;
    }

    return res;
}


/****************************************************************************/
/** Put data into buffer without locking
 *
 * This function writes data to the ring buffer without access locking.
 *
 * @return
 * GOAL return status
 */
GOAL_STATUS_T goal_rbPutFast(
    GOAL_RB_T *pRb,                             /**< ring buffer */
    const uint8_t *pMessageData,                /**< message buffer */
    uint16_t messageSize                        /**< message size */
)
{
    uint16_t writePointer;                      /* local write pointer */
    uint16_t writePointerEnd;                   /* expected write pointer after operation */
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_RB_READER_T *pEntry;                   /* reader entry */
    uint16_t left;                              /* space at end of buffer */

    if (messageSize >= pRb->size) {
        res = GOAL_ERR_OVERFLOW;
    }

    /* set local write pointer */
    writePointer = pRb->wp;
    writePointerEnd = (pRb->wp + messageSize) % pRb->size;
    left = pRb->size - writePointer;

    /* check if readers got all data */
    if (goal_rbTouched(pRb)) {
        GOAL_LL_FOREACH(pRb->pListReader, pEntry) {
            if (writePointerEnd > writePointer) {
                if (pEntry->readPointer > writePointer && pEntry->readPointer <= writePointerEnd) {
                    res = GOAL_ERR_OVERFLOW;
                }
            }
            else {
                if (pEntry->readPointer > writePointer || pEntry->readPointer <= writePointerEnd) {
                    res = GOAL_ERR_OVERFLOW;
                }
            }
        }
    }

    if (GOAL_RES_OK(res)) {

        /* will write operation wrap ? */
        if (messageSize > left) {
            /* split copy */
            GOAL_MEMCPY(&pRb->pBuffer[writePointer], pMessageData, left);
            GOAL_MEMCPY(&pRb->pBuffer[0], pMessageData + left, (messageSize - left));
        }
        else {
            /* single copy */
            GOAL_MEMCPY(&pRb->pBuffer[writePointer], pMessageData, messageSize);
        }

        pRb->flgTouched = GOAL_TRUE;

        /* set global write pointer */
        pRb->wp = writePointerEnd;

    }

    return res;
}


/****************************************************************************/
/** Unlock ring buffer after writing
 *
 * This function unlocks the ring buffer after writing.
 * This function must be called if goal_rbPut() is called with parameter
 * flgLockKeep = GOAL_TRUE - also in error case.
 *
 */
void goal_rbPutFastFinish(
    GOAL_RB_T *pRb                              /**< ring buffer */
)
{
    if (NULL != pRb && NULL != pRb->pMtx) {
        goal_lockPut(pRb->pMtx);
    }
}


/****************************************************************************/
/** ring buffer: get write pointer of buffer
 *
 * @returns write pointer
 */
GOAL_STATUS_T goal_rbWpGet(
    GOAL_RB_T *pRb,                             /**< ring buffer */
    uint16_t *pWp                               /**< result */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* return status */

    if (NULL != pRb) {
        res = goal_lockGet(pRb->pMtx, GOAL_LOCK_INFINITE);
        if (GOAL_RES_ERR(res)) {
            return res;
        }

        *pWp = pRb->wp;

        goal_lockPut(pRb->pMtx);
    }

    return res;
}


/****************************************************************************/
/** Get amount of unread data
 *
 * This function returns the number of unread data bytes.
 *
 * @return
 * GOAL return status
 */
GOAL_STATUS_T goal_rbCntGet(
    GOAL_RB_T *pRb,                             /**< ring buffer */
    uint16_t readPointer,                       /**< readers read pointer */
    uint16_t *pResult                           /**< result */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* return value */

    if (NULL != pResult && NULL != pRb) {

        if (readPointer < pRb->wp) {
            *pResult = pRb->wp - readPointer;
        }
        else if (readPointer == pRb->wp) {
                    *pResult = 0;
        }
        else {
            *pResult = pRb->wp + (pRb->size - readPointer);
        }
    }
    else {
        res = GOAL_ERR_NULL_POINTER;
    }

    return res;
}


/****************************************************************************/
/** Check if buffer was written at least once
 *
 * This function returns the current state of the 1st writing.
 *
 * @retval GOAL_TRUE
 * new data was written for the first time into the ring buffer
 * @retval GOAL_FALSE
 * no new data was written into the ring buffer
 */
GOAL_BOOL_T goal_rbTouched(
    GOAL_RB_T *pRb                              /**< ring buffer */
)
{
    GOAL_BOOL_T res = GOAL_FALSE;               /* return value */

    if (NULL != pRb) {
        res = pRb->flgTouched;
    }

    return res;
}


/****************************************************************************/
/** Register a reader of the ring buffer
 *
 * Multiple readers can be registered per read buffer. The reader holds track
 * of the read position of each reader. Beside that, readers can be informed
 * about events regarding the ring buffer.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_rbReaderReg(
    GOAL_RB_READER_T **ppRbReader,              /**< RB reader handle */
    GOAL_RB_T *pRb,                             /**< RB handle */
    GOAL_RB_READER_CB fnctCb                    /**< RB reader callback */
)
{
    GOAL_STATUS_T res;                          /* result */

    res = goal_memCalloc(ppRbReader, sizeof(GOAL_RB_READER_T));

    if (GOAL_RES_OK(res)) {
        res = goal_lockCreate(GOAL_LOCK_BINARY, &(*ppRbReader)->pMtx, 0, 1, GOAL_ID_LM);
    }

    if (GOAL_RES_OK(res)) {

        /* set read pointer to head */
        (*ppRbReader)->readPointer = 0;

        /* set rb */
        (*ppRbReader)->pRb = pRb;

        /* set callback */
        (*ppRbReader)->fnctCb = fnctCb;

        GOAL_LL_APPEND(pRb->pListReader, (*ppRbReader));
    }

    return res;
}


/****************************************************************************/
/** ring buffer: get data from buffer
 *
 * The parameter pBufferSize indicates the available space in the destination
 * buffer. After get operation, it contains the actual length of data read.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_rbReaderGet(
    GOAL_RB_READER_T *pHdlReader,               /**< buffer reader */
    uint8_t *pBuffer,                           /**< destination buffer */
    uint16_t *pBufferSize                       /**< destination buffer size, actual read length */
)
{
    uint16_t readPointerNew;                    /* read position after get */
    uint16_t readAmount;                        /* length to read */
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    readPointerNew = 0;
    readAmount = 0;

    if (NULL == pHdlReader || NULL == pHdlReader->pRb) {
        res = GOAL_ERR_NULL_POINTER;
    }

    if (GOAL_RES_OK(res)) {
        res = goal_lockGet(pHdlReader->pMtx, GOAL_LOCK_INFINITE);
    }

    if (GOAL_RES_OK(res)) {
        /* is data available */
        if (pHdlReader->pRb->wp == pHdlReader->readPointer) {
            res = GOAL_ERR_EMPTY;
        }
    }

    /* get available data count */
    if (GOAL_RES_OK(res)) {
        res = goal_rbCntGet(pHdlReader->pRb, pHdlReader->readPointer, &readAmount);
    }

    /* limit amount of data to buffer size */
    if (GOAL_RES_OK(res)) {
        if (*pBufferSize > readAmount) {
            *pBufferSize = readAmount;
        }
    }

    /* access ring buffer */
    if (GOAL_RES_OK(res)) {
        res = goal_rbGet(pHdlReader->pRb, pBuffer, *pBufferSize, pHdlReader->readPointer, &readPointerNew);
        pHdlReader->readPointer = readPointerNew;
    }

    goal_lockPut(pHdlReader->pMtx);

    return res;
}


/****************************************************************************/
/** flush ring buffer
 *
 * The read pointer of the GOAL_RB_READER_T is set to the write pointer
 * of the  GOAL_RB_T. Due this the ring buffer is empty.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_rbFlush(
    GOAL_RB_READER_T *pHdlReader                /**< ring buffer reader */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    if (NULL != pHdlReader) {
        /* set read pointer at write ponter */
        pHdlReader->pRb->wp = pHdlReader->readPointer;
    }
    else {
        res = GOAL_ERR_NULL_POINTER;
    }

    return res;
}
