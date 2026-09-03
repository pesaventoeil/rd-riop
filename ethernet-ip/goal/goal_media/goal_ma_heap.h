/** @file
 *
 * @brief Generic MA for HEAP
 *
 * @details
 * This module provides a Media Adapter for HEAP drivers
 *
 * @copyright
 * Copyright 2010-2022.
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
#ifndef GOAL_MA_HEAP_H
#define GOAL_MA_HEAP_H


/****************************************************************************/
/* typedefs */
/****************************************************************************/
typedef struct {
    uint32_t size;                              /**< total HEAP size in bytes */
    uint32_t used;                              /**< used HEAP memory in bytes */
} GOAL_MA_HEAP_STATS_T;

typedef GOAL_STATUS_T (* GOAL_MA_HEAP_ALLOC_ALIGN_T)(
    void **ppMem,                               /**< [out] ptr to store buf ptr to */
    uint32_t memLen,                            /**< size of buffer to alloc */
    uint32_t align,                             /**< alignment */
    void *pData                                 /**< driver data */
);

typedef GOAL_STATUS_T (* GOAL_MA_HEAP_CALLOC_ALIGN_T)(
    void **ppMem,                               /**< [out] ptr to store buf ptr to */
    uint32_t memLen,                            /**< size of buffer to alloc */
    uint32_t align,                             /**< alignment */
    void *pData                                 /**< driver data */
);

typedef GOAL_STATUS_T (* GOAL_MA_HEAP_FREE_T)(
    void *pMem,                                 /**< ptr of released buf */
    void *pData                                 /**< driver data */
);

typedef GOAL_STATUS_T (* GOAL_MA_HEAP_REALLOC_T)(
    void **ppMemNew,                            /**< [out] new ptr of buffer for reallocation */
    void *pMemOld,                              /**< old ptr of buffer for reallocation */
    uint32_t memLen,                            /**< new size of buffer */
    void *pData                                 /**< driver data */
);

typedef GOAL_STATUS_T (* GOAL_MA_HEAP_STATS_GET_T)(
    GOAL_MA_HEAP_STATS_T *pStats,               /**< [out] memory statistics */
    void *pData                                 /**< driver data */
);


/****************************************************************************/
/* Function Casts */
/****************************************************************************/
#define goal_maHeapAlloc(id, ppMem, memLen) goal_maHeapAllocAlign(id, ppMem, memLen, GOAL_TARGET_MEM_ALIGN_CPU)
#define goal_maHeapCalloc(id, ppMem, memLen) goal_maHeapCallocAlign(id, ppMem, memLen, GOAL_TARGET_MEM_ALIGN_CPU)

/* old API */
#define goal_heapAlloc(ppMem, len) goal_maHeapAlloc(GOAL_ID_DEFAULT, (void **) ppMem, len)
#define goal_heapCalloc(ppMem, len) goal_maHeapCalloc(GOAL_ID_DEFAULT, (void **) ppMem, len)
#define goal_heapRealloc(pptrNew, ptrOld, len) goal_maHeapRealloc(GOAL_ID_DEFAULT, (void **) pptrNew, ptrOld, len)
#define goal_heapFree(ptr) goal_maHeapFree(GOAL_ID_DEFAULT, ptr)
#define goal_heapStatsGet(pStats) goal_maHeapStatsGet(GOAL_ID_DEFAULT, pStats)


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_maHeapDrvReg(
    uint32_t id,                                /**< ID */
    GOAL_MA_HEAP_ALLOC_ALIGN_T fAlloc,          /**< alloc function */
    GOAL_MA_HEAP_CALLOC_ALIGN_T fCalloc,        /**< calloc function */
    GOAL_MA_HEAP_REALLOC_T fRealloc,            /**< realloc function */
    GOAL_MA_HEAP_FREE_T fFree,                  /**< free function */
    GOAL_MA_HEAP_STATS_GET_T fStatsGet,         /**< get statistics function */
    void *pDrvData                              /**< driver specific data */
);

GOAL_STATUS_T goal_maHeapAllocAlign(
    uint32_t id,                                /**< ID */
    void **ppMem,                               /**< [out] ptr to store buf ptr to */
    uint32_t memLen,                            /**< size of buffer to alloc */
    uint32_t align                              /**< alignment */
);

GOAL_STATUS_T goal_maHeapCallocAlign(
    uint32_t id,                                /**< ID */
    void **ppMem,                               /**< [out] ptr to store buf ptr to */
    uint32_t memLen,                            /**< size of buffer to alloc */
    uint32_t align                              /**< alignment */
);

GOAL_STATUS_T goal_maHeapFree(
    uint32_t id,                                /**< ID */
    void *pMem                                  /**< ptr of released buf */
);

GOAL_STATUS_T goal_maHeapRealloc(
    GOAL_ID_T id,                               /**< caller id */
    void **ppMemNew,                            /**< [out] new ptr of buffer for reallocation */
    void *pMemOld,                              /**< old ptr of buffer for reallocation */
    uint32_t memLen                             /**< new size of buffer */
);

GOAL_STATUS_T goal_maHeapStatsGet(
    GOAL_ID_T id,                               /**< caller id */
    GOAL_MA_HEAP_STATS_T *pStats                /**< [out] memory statistics */
);

#endif /* GOAL_MA_NET_H */
