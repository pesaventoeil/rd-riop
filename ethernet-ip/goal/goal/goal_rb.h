/** @file
 *
 * @brief ring buffer
 *
 * This module implements a ring buffer
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

#ifndef GOAL_RB_H
#define GOAL_RB_H

#include "goal_includes.h"


/****************************************************************************/
/* Config Defaults */
/****************************************************************************/


/****************************************************************************/
/* Defines */
/****************************************************************************/


/****************************************************************************/
/* Type definitions */
/****************************************************************************/

struct GOAL_RB_READER_T;

typedef GOAL_STATUS_T (*GOAL_RB_READER_CB) (
    struct GOAL_RB_READER_T *pHdlReader         /**< RB reader handle */
);

/**< goal lm logging buffer reader */
typedef struct GOAL_RB_READER_T {
    struct GOAL_RB_READER_T *pNext;             /**< next reader */
    struct GOAL_RB_T *pRb;                      /**< rb handle */
    GOAL_RB_READER_CB fnctCb;                   /**< reader callback */
    uint16_t readPointer;                       /**< read pointer */
    GOAL_LOCK_T *pMtx;                          /**< reader lock */
} GOAL_RB_READER_T;

/* structure for the header of the ring buffer */
typedef struct GOAL_RB_T {
    uint8_t *pBuffer;                           /**< buffer for data */
    uint16_t size;                              /**< size of the data buffer within the ring buffer in byte */
    uint16_t wp;                                /**< write pointer */
    GOAL_BOOL_T flgTouched;                     /**< state of the 1st writing */
    GOAL_LOCK_T *pMtx;                          /**< mutex for buffer access */
    struct GOAL_RB_READER_T *pListReader;       /**< registered readers */
} GOAL_RB_T;


/****************************************************************************/
/* Prototypes */
/****************************************************************************/

GOAL_STATUS_T goal_rbInit(
    GOAL_RB_T **ppRb,                           /**< RB-handle */
    uint16_t size                               /**< size of ring buffer in bytes */
);

GOAL_STATUS_T goal_rbReaderReg(
    GOAL_RB_READER_T **ppRbReader,              /**< RB reader handle */
    GOAL_RB_T *pRb,                             /**< RB handle */
    GOAL_RB_READER_CB fnctCb                    /**< RB reader callback */
);

GOAL_STATUS_T goal_rbReaderGet(
    GOAL_RB_READER_T *pHdlReader,               /**< buffer reader */
    uint8_t *pBuffer,                           /**< destination buffer */
    uint16_t *pBufferSize                       /**< destination buffer size, actual read length */
);

void goal_rbShutdown(
    GOAL_RB_T **ppRb                            /**< RB-handle ref */
);

GOAL_STATUS_T goal_rbPut(
    GOAL_RB_T *pRb,                             /**< PB-handle */
    const uint8_t *pData,                       /**< pointer to data for writing */
    uint16_t size,                              /**< size of data for writing */
    GOAL_BOOL_T flgLockKeep                     /**< keep locking flag */
);

GOAL_STATUS_T goal_rbGet(
    GOAL_RB_T *pRb,                             /**< RB-handle */
    uint8_t *pDestBuffer,                       /**< buffer for read data */
    uint16_t destBufferSize,                    /**< size of destination buffer or desired number of bytes to read in byte */
    uint16_t readPointer,                       /**< read pointer before reading from reader */
    uint16_t *pReturn                           /**< information about reading depending on ring buffer type */
);

GOAL_STATUS_T goal_rbPutFast(
    GOAL_RB_T *pRb,                             /**< RB-handle */
    const uint8_t *pData,                       /**< pointer to data for writing */
    uint16_t size                               /**< size of data for writing in byte */
);

void goal_rbPutFastFinish(
    GOAL_RB_T *pRb                              /**< RB-handle */
);

GOAL_STATUS_T goal_rbWpGet(
    GOAL_RB_T *pRb,                             /**< RB-handle */
    uint16_t *pWp                               /**< write pointer */
);

GOAL_STATUS_T goal_rbCntGet(
    GOAL_RB_T *pRb,                             /**< RB-handle */
    uint16_t readPointer,                       /**< read pointer (only for GOAL_RB_TYPE_SIZE_WRAP_OVERWRITE, else 0) */
    uint16_t *pResult                           /**< number of unread data bytes */
);

GOAL_BOOL_T goal_rbTouched(
    GOAL_RB_T *pRb                              /**< RB-handle */
);

GOAL_STATUS_T goal_rbFlush(
    GOAL_RB_READER_T *pHdlReader                /**< ring buffer reader */
);

#endif /* GOAL_RB_H */
