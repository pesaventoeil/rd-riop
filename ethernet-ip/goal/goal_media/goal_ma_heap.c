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

#define GOAL_ID GOAL_ID_MA_NET
#include "goal_includes.h"
#include "goal_media/goal_ma_heap.h"


/****************************************************************************/
/* Local Defines */
/****************************************************************************/
#ifndef GOAL_MA_HEAP_CNT
# define GOAL_MA_HEAP_CNT 1                     /**< number of available handles */
#endif


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/
typedef struct {
    uint32_t id;                                /**< ID */
    GOAL_MA_HEAP_ALLOC_ALIGN_T fAlloc;          /**< alloc function */
    GOAL_MA_HEAP_CALLOC_ALIGN_T fCalloc;        /**< calloc function */
    GOAL_MA_HEAP_REALLOC_T fRealloc;            /**< realloc function */
    GOAL_MA_HEAP_FREE_T fFree;                  /**< free function */
    GOAL_MA_HEAP_STATS_GET_T fStatsGet;         /**< get statistics function */
    void *pDrvData;                             /**< driver specific data */
} GOAL_MA_HEAP_T;


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_MA_HEAP_T maHeap[GOAL_MA_HEAP_CNT]; /**< heap memory */


/****************************************************************************/
/** Registers a HEAP MA compliant driver
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maHeapDrvReg(
    uint32_t id,                                /**< ID */
    GOAL_MA_HEAP_ALLOC_ALIGN_T fAlloc,          /**< alloc function */
    GOAL_MA_HEAP_CALLOC_ALIGN_T fCalloc,        /**< calloc function */
    GOAL_MA_HEAP_REALLOC_T fRealloc,            /**< realloc function */
    GOAL_MA_HEAP_FREE_T fFree,                  /**< free function */
    GOAL_MA_HEAP_STATS_GET_T fStatsGet,         /**< get statistics function */
    void *pDrvData                              /**< driver specific data */
)
{
    /* verify arguments */
    if ((GOAL_MA_HEAP_CNT <= id) ||
        (NULL == fAlloc) ||
        (NULL == fCalloc) ||
        (NULL == fRealloc) ||
        (NULL == fFree) ||
        (NULL == fStatsGet)) {
        return GOAL_ERR_PARAM;
    }

    /* initialize the structure items */
    maHeap[id].id = id;
    maHeap[id].fAlloc = fAlloc;
    maHeap[id].fCalloc = fCalloc;
    maHeap[id].fRealloc = fRealloc;
    maHeap[id].fFree = fFree;
    maHeap[id].fStatsGet = fStatsGet;
    maHeap[id].pDrvData = pDrvData;

    return GOAL_OK;
}


/****************************************************************************/
/** Wrapper for allocating aligned HEAP memory
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maHeapAllocAlign(
    uint32_t id,                                /**< ID */
    void **ppMem,                               /**< [out] ptr to store buf ptr to */
    uint32_t memLen,                            /**< size of buffer to alloc */
    uint32_t align                              /**< alignment */
)
{
    /* verify arguments */
    if (maHeap[id].id != id) {
        return GOAL_ERR_PARAM;
    }
    return maHeap[id].fAlloc(ppMem, memLen, align, maHeap[id].pDrvData);
}


/****************************************************************************/
/** Wrapper for callocating aligned HEAP memory
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maHeapCallocAlign(
    uint32_t id,                                /**< ID */
    void **ppMem,                               /**< [out] ptr to store buf ptr to */
    uint32_t memLen,                            /**< size of buffer to alloc */
    uint32_t align                              /**< alignment */
)
{
    /* verify arguments */
    if (maHeap[id].id != id) {
        return GOAL_ERR_PARAM;
    }
    return maHeap[id].fCalloc(ppMem, memLen, align, maHeap[id].pDrvData);
}


/****************************************************************************/
/** Wrapper for freeing HEAP memory
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maHeapFree(
    uint32_t id,                                /**< ID */
    void *pMem                                  /**< ptr of released buf */
)
{
    /* verify arguments */
    if (maHeap[id].id != id) {
        return GOAL_ERR_PARAM;
    }
    return maHeap[id].fFree(pMem, maHeap[id].pDrvData);
}


/****************************************************************************/
/** Wrapper for reallocating HEAP memory
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maHeapRealloc(
    GOAL_ID_T id,                               /**< caller id */
    void **ppMemNew,                            /**< [out] new ptr of buffer for reallocation */
    void *pMemOld,                              /**< old ptr of buffer for reallocation */
    uint32_t memLen                             /**< new size of buffer */
)
{
    /* verify arguments */
    if (maHeap[id].id != id) {
        return GOAL_ERR_PARAM;
    }
    return maHeap[id].fRealloc(ppMemNew, pMemOld, memLen, maHeap[id].pDrvData);
}


/****************************************************************************/
/** Wrapper for reading statistics of HEAP memory
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maHeapStatsGet(
    GOAL_ID_T id,                               /**< caller id */
    GOAL_MA_HEAP_STATS_T *pStats                /**< [out] memory statistics */
)
{
    /* verify arguments */
    if (maHeap[id].id != id) {
        return GOAL_ERR_PARAM;
    }
    return maHeap[id].fStatsGet(pStats, maHeap[id].pDrvData);
}
