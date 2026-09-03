/** @file
 *
 * @brief
 * Driver for FreeRotos HEAP
 *
 * @details
 * This is a wrapper for allocating heap memory by FreeRTOS API.
 *
 * @copyright
 * Copyright 2010-2022 port GmbH Halle/Saale.
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

#define GOAL_ID GOAL_ID_MEM
#include <goal_includes.h>
#include <goal_media/goal_ma_heap.h>
#include <plat/drv/mem/heap/freertos/mem_heap_freertos.h>
#include <FreeRTOS.h>


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
static GOAL_STATUS_T goal_drvMemHeapStatusGet(
    GOAL_MA_HEAP_STATS_T *pStatus,              /**< [out] copy of status */
    void *pData                                 /**< driver data */
);

static GOAL_STATUS_T goal_drvMemHeapAllocAlign(
    void **ppRetBlock,                          /**< [out] ptr to store memory to */
    uint32_t wantedSize,                        /**< requested size to alloc */
    uint32_t align,                             /**< alignment */
    void *pData                                 /**< driver data */
);

static GOAL_STATUS_T goal_drvMemHeapCallocAlign(
    void **ppBlock,                             /**< [out] ptr to store memory to */
    uint32_t wantedSize,                        /**< requested size to alloc */
    uint32_t align,                             /**< alignment */
    void *pData                                 /**< driver data */
);

static GOAL_STATUS_T goal_drvMemHeapRealloc(
    void **ppRetBlock,                          /**< [out] ptr to store memory to */
    void *pOldBlock,                            /**< ptr to previously allocated memory */
    uint32_t wantedSize,                        /**< requested size to alloc */
    void *pData                                 /**< driver data */
);

static GOAL_STATUS_T goal_drvMemHeapFree(
    void *pBlock,                               /**< memory block to free */
    void *pData                                 /**< driver data */
);


/****************************************************************************/
/** Create Heap Region
 *
 * This function registers the heap wrapper to Media Adapter. It has to be
 * created in an early state of GOAL initialization, before the static memory
 * is available (GOAL_STAGE_MEM_PRE).
 *
 * @retval GOAL_OK successful
 * @retval other  fail
 */
GOAL_STATUS_T goal_drvMemHeapCreate(
    void
)
{
    return goal_maHeapDrvReg(GOAL_ID_DEFAULT,
                               goal_drvMemHeapAllocAlign,
                               goal_drvMemHeapCallocAlign,
                               goal_drvMemHeapRealloc,
                               goal_drvMemHeapFree,
                               goal_drvMemHeapStatusGet,
                               NULL);
}


/****************************************************************************/
/** Get Heap Status
 *
 * This function get some statistics about the heap size and occupied bytes.
 *
 * @retval GOAL_OK successful
 * @retval other  fail
 */
static GOAL_STATUS_T goal_drvMemHeapStatusGet(
    GOAL_MA_HEAP_STATS_T *pStatus,              /**< [out] copy of status */
    void *pData                                 /**< driver data */
)
{
    size_t freeHeapSize;                        /* free heap size in bytes */

    UNUSEDARG(pStatus);
    UNUSEDARG(pData);

    /* request free heap size for calculation of used bytes */
    freeHeapSize = xPortGetFreeHeapSize();

    pStatus->size = GOAL_CONFIG_FREERTOS_HEAP_SIZE;
    pStatus->used = GOAL_CONFIG_FREERTOS_HEAP_SIZE - freeHeapSize;

    return GOAL_OK;
}


/****************************************************************************/
/** Allocate aligned Block
 *
 * Wrapper for allocation of an aligned buffer from OS heap.
 *
 * @retval GOAL_OK successful
 * @retval other  fail
 */
static GOAL_STATUS_T goal_drvMemHeapAllocAlign(
    void **ppRetBlock,                          /**< [out] ptr to store memory to */
    uint32_t wantedSize,                        /**< requested size to alloc */
    uint32_t align,                             /**< alignment */
    void *pData                                 /**< driver data */
)
{
    UNUSEDARG(pData);

    /* prepare "error" case, the NULL pointer will be unset if allocation
     * was successful */
    *ppRetBlock = NULL;

    /* check alignment of buffer */
    if (0 == (portBYTE_ALIGNMENT % align)) {
        *ppRetBlock = pvPortMalloc(wantedSize);
    }

    return (NULL == *ppRetBlock) ? (GOAL_ERR_ALLOC) : (GOAL_OK);
}


/****************************************************************************/
/** Calloc Block
 *
 * This function allocated memory from heap and clears all data.
 *
 * @retval GOAL_OK successful
 * @retval other  fail
 */
static GOAL_STATUS_T goal_drvMemHeapCallocAlign(
    void **ppRetBlock,                          /**< [out] ptr to store memory to */
    uint32_t wantedSize,                        /**< requested size to alloc */
    uint32_t align,                             /**< alignment */
    void *pData                                 /**< driver data */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    res = goal_drvMemHeapAllocAlign(ppRetBlock, wantedSize, align, pData);
    if (GOAL_RES_OK(res)) {
        GOAL_MEMSET(*ppRetBlock, 0, wantedSize);
    }
    return res;
}


/****************************************************************************/
/** Free Block
 *
 * Wrapper for freeing a OS heap buffer.
 *
 * @retval GOAL_OK successful
 * @retval other  fail
 */
static GOAL_STATUS_T goal_drvMemHeapFree(
    void *pBlock,                               /**< memory block to free */
    void *pData                                 /**< driver data */
)
{
    UNUSEDARG(pData);

    vPortFree(pBlock);
    return GOAL_OK;
}


/****************************************************************************/
/** Realloc Block
 *
 * This function allocates a new block and copies the content of the old block
 * to the new one. If the new block is bigger, the remaining bytes are
 * uninitialized. The old block will be freed.
 * If @em pOldBlock is NULL the function call is equivalent to goal_drvMemHeapAlloc.
 * If @em wantedSize is 0 and @em pOldBlock is not NULL, the function call is
 * equivalent to goal_drvMemHeapFree.
 *
 * @retval GOAL_OK successful
 * @retval other  fail
 */
GOAL_STATUS_T goal_drvMemHeapRealloc(
    void **ppRetBlock,                          /**< [out] ptr to store memory to */
    void *pOldBlock,                            /**< ptr to previously allocated memory */
    uint32_t wantedSize,                        /**< requested size to alloc */
    void *pData                                 /**< driver data */
)
{
    void *pNewBlock;                            /* newly allocated memory */
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    UNUSEDARG(pData);

    if ((0 != wantedSize) && (NULL == ppRetBlock)) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (0 != wantedSize) {
        res = goal_drvMemHeapAllocAlign(&pNewBlock, wantedSize, GOAL_TARGET_MEM_ALIGN_CPU, pData);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to allocate new memory");
        } else {
            if (pOldBlock) {
                GOAL_MEMCPY(pNewBlock, pOldBlock, wantedSize);
            }
            *ppRetBlock = pNewBlock;
        }
    }

    if (pOldBlock) {
        res = goal_drvMemHeapFree(pOldBlock, pData);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to free old memory");
        }
    }

    return res;
}

