/** @file
 *
 * @brief
 * Simple Dynamic Memory Allocator
 *
 * @details
 * This is a simple memory allocator which uses a preallocated area to assign
 * memory to various modules. It is possible to free memory while running.
 * Free neighboring blocks are merged automatically by the program.
 *
 * @copyright
 * Copyright 2010-2022
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
#include <plat/drv/mem/heap/goal/mem_heap_goal.h>


/****************************************************************************/
/* Defines */
/****************************************************************************/
#define GOAL_DRV_MEM_HEAP_IDENT 0x48454150      /**< memory identification mark "HEAP" */

#ifndef GOAL_CONFIG_HEAP_DYNAMIC_ADD_SIZE
# define GOAL_CONFIG_HEAP_DYNAMIC_ADD_SIZE 0    /**< addition size of dynamic heap memory */
#endif

#ifndef GOAL_CONFIG_HEAP_DYNAMIC_SIZE
/* default GOAL_CONFIG_HEAP_SIZE is able to keep the static memory exactly
 * Thus, GOAL_CONFIG_HEAP_SIZE needs to be increased by
 * GOAL_CONFIG_HEAP_DYNAMIC_HEADER_SIZE, which is 1x region header
 * and 3x block header (start block, last block, static memory block)
 *
 * additionally alignment needs to be considered, which might move the start
 * of the dynamic heap area
 * */
#  define GOAL_CONFIG_HEAP_DYNAMIC_SIZE (GOAL_CONFIG_HEAP_DYNAMIC_HEADER_SIZE \
                                + GOAL_CONFIG_HEAP_DYNAMIC_ADD_SIZE \
                                + GOAL_CONFIG_HEAP_SIZE \
                                + GOAL_TARGET_MEM_ALIGN_CPU)
#endif


/****************************************************************************/
/* Variables */
/****************************************************************************/
static uint8_t memHeap[GOAL_CONFIG_HEAP_DYNAMIC_SIZE]; /**< GOAL HEAP */
static GOAL_STAGE_HANDLER_T stageInitLock;      /**< lock init stage handler */
static GOAL_LOCK_T *mpHeapLock = NULL;          /**< heap access lock */
static GOAL_HEAP_BLOCK_T mStartBlock = {NULL, 0, 0, 0}; /**< start inidcator */
static GOAL_HEAP_BLOCK_T *mpFirstFreeBlock;     /**< first free block indicator */
static GOAL_HEAP_BLOCK_T *mpEndBlock;           /**< end block indicator */
static const uint32_t mBlockStructSize = sizeof(GOAL_HEAP_BLOCK_T); /**< size of heap struct */
static const uint32_t mHeapStructSize = sizeof(GOAL_HEAP_REGION_T); /**< size of block struct */
static uint32_t mFreeHeapSize;                  /**< total free size w/o overhead */
static uint32_t mAllocHeapSize;                 /**< total allocated mem w/o overhead */
static uint32_t mCntAllocBlocks;                /**< number of allocated blocks */


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
static GOAL_STATUS_T goal_drvMemHeapStatusGet(
    GOAL_MA_HEAP_STATS_T *pStatus,              /**< [out] copy of status */
    void *pData                                 /**< driver data */
);

static GOAL_STATUS_T goal_drvMemHeapLockInit(
    void
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
    void *pOldBlock,                            /**< [in] ptr to previously allocated memory */
    uint32_t wantedSize,                        /**< requested size to alloc */
    void *pData                                 /**< driver data */
);

static GOAL_STATUS_T goal_drvMemHeapFree(
    void *pBlock,                               /**< memory block to free */
    void *pData                                 /**< driver data */
);

static GOAL_STATUS_T goal_drvMemHeapRegionsDefine(
    GOAL_HEAP_REGION_T *pHeapRegion             /**< struct of heap region */
);

static GOAL_STATUS_T goal_drvMemHeapBlockInsert(
    GOAL_HEAP_BLOCK_T *pNewBlock                /**< memory block to insert */
);

static GOAL_STATUS_T goal_drvMemHeapBlockMerge(
    GOAL_HEAP_BLOCK_T *pBlockToMerge            /**< memory block to merge */
);


/****************************************************************************/
/** Create Heap Region
 *
 * This function creates a memory region for GOAL heap. It has to be done in
 * an early state of GOAL initialization, before the static memory is
 * available (GOAL_STAGE_MEM_PRE).
 *
 * @retval GOAL_OK successful
 * @retval other  fail
 */
GOAL_STATUS_T goal_drvMemHeapCreate(
    void
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_HEAP_REGION_T *pHeapRegion;            /* heap region reference */

    /* clear the complete heap */
    GOAL_MEMSET(memHeap, 0, GOAL_CONFIG_HEAP_DYNAMIC_SIZE);

    /* heap region information are placed at start of heap */
    pHeapRegion = (GOAL_HEAP_REGION_T *) memHeap;

    /* configure the heap size */
    pHeapRegion->heapRegionSize = GOAL_CONFIG_HEAP_DYNAMIC_SIZE;

    /* Start of the heap memory must be assigned as start address.
     * The pointer will be updated, when defining heap regions. */
    pHeapRegion->pStartAddress = memHeap;

    /* define region as heap */
    res = goal_drvMemHeapRegionsDefine(pHeapRegion);

    if (GOAL_RES_OK(res)) {
        res = goal_maHeapDrvReg(GOAL_ID_DEFAULT,
                                goal_drvMemHeapAllocAlign,
                                goal_drvMemHeapCallocAlign,
                                goal_drvMemHeapRealloc,
                                goal_drvMemHeapFree,
                                goal_drvMemHeapStatusGet,
                                (void *) memHeap);
    }

    /* register lock init stage */
    if (GOAL_RES_OK(res)) {
        res = goal_mainStageReg(GOAL_STAGE_LOCK, &stageInitLock, GOAL_STAGE_INIT, goal_drvMemHeapLockInit);
    }
    return res;
}


/****************************************************************************/
/** Define Heap Region
 *
 * This function define the entered heap region and aligned it.
 * The result is a heap block with a head structure.
 *
 * @retval GOAL_OK successful
 * @retval other  fail
 */
static GOAL_STATUS_T goal_drvMemHeapRegionsDefine(
    GOAL_HEAP_REGION_T *pHeapRegion             /**< struct of heap region */
)
{
    uint8_t *pAlignedHeap;                      /* heap ptr aligned */
    uint8_t *pAddress;                          /* tmp address */
    uint8_t *pAddressEnd;                       /* end address */

    if (NULL == pHeapRegion) {
        goal_logErr("heap regions not defined");
        return GOAL_ERR_NULL_POINTER;
    }

    /* adjust alignment */
    pAddress = pHeapRegion->pStartAddress + mHeapStructSize;
    pAlignedHeap = (uint8_t *) GOAL_alignPtr(GOAL_TARGET_MEM_ALIGN_CPU, pAddress);

    /* set total heap size without head
     * and with aligned ptr */
    mFreeHeapSize = pHeapRegion->heapRegionSize - mHeapStructSize;
    mFreeHeapSize -= (uint32_t) (pAlignedHeap - pAddress);

    /* set start block */
    mStartBlock.pNextBlock = (GOAL_HEAP_BLOCK_T *) pAlignedHeap;
    mStartBlock.heapBlockSize = (uint32_t) 0;

    /* calculate and set end block */
    pAddressEnd = pAlignedHeap + mFreeHeapSize - mBlockStructSize;
    mpEndBlock = (GOAL_HEAP_BLOCK_T *) (pAddressEnd);
    mpEndBlock->pNextBlock = NULL;
    mpEndBlock->heapBlockSize = 0;

    /* first free block contains size without start and end marker */
    mpFirstFreeBlock = (GOAL_HEAP_BLOCK_T *) pAlignedHeap;
    mpFirstFreeBlock->heapBlockSize = mFreeHeapSize - 2 * mBlockStructSize;
    mpFirstFreeBlock->pNextBlock = mpEndBlock;

    /* update total heap size */
    mFreeHeapSize = mpFirstFreeBlock->heapBlockSize;

    return GOAL_OK;
}


/****************************************************************************/
/** Initialize Memory Lock
 *
 * Initialize memory lock after locking subsystem started.
 *
 * @retval GOAL_OK successful
 * @retval other fail
 */
static GOAL_STATUS_T goal_drvMemHeapLockInit(
    void
)
{
    return goal_lockCreate(GOAL_LOCK_BINARY, &mpHeapLock, 0, 1, GOAL_ID);
}


/****************************************************************************/
/** Allocate aligned Block
 *
 * This function allocated memory from heap and set the ptr reference
 * to the start address of free memory. Additionally the chosen free
 * block will be splitted, if it's bigger than the requested size.
 * The head of the block is still intact because we need the size
 * to free the block later.
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
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_HEAP_BLOCK_T *pPrevBlock = NULL;       /* indicator ptr */
    GOAL_HEAP_BLOCK_T *pBlock = NULL;           /* indicator ptr */
    GOAL_HEAP_BLOCK_T *pNewBlock = NULL;        /* indicator ptr */
    uint32_t memSize;                           /* size of new memory block (header + data) */

    UNUSEDARG(align);
    UNUSEDARG(pData);

    if (NULL == mStartBlock.pNextBlock) {
        goal_logErr("GOAL heap is not initialized");
        return GOAL_ERROR;
    }

    /* verification of arguments */
    if (0 == wantedSize) {
        goal_logErr("allocatin %"FMT_u32" bytes is not supported", wantedSize);
        return GOAL_ERR_PARAM;
    }

    /* size alignment, so all buffers are aligned to the same alignment */
    if ((wantedSize + mBlockStructSize) % align) {
        wantedSize += align - ((wantedSize + mBlockStructSize) % align);
    }

    memSize = wantedSize + mBlockStructSize;

    if (NULL != mpHeapLock) {
        res = goal_lockGet(mpHeapLock, GOAL_LOCK_INFINITE);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to acquire lock");
            return res;
        }
    }

    /* find big enough free block */
    pPrevBlock = &mStartBlock;
    pBlock = mStartBlock.pNextBlock;

    while (pBlock->heapBlockSize < memSize) {
        if (NULL == pBlock->pNextBlock) {
            goal_logErr("can't allocate %"FMT_u64" bytes on heap memory (allocated: %"FMT_u64" Byte in %"FMT_u64" Blocks, free: %"FMT_u64" Byte",
                        (uint64_t) wantedSize, (uint64_t) mAllocHeapSize, (uint64_t) mCntAllocBlocks, (uint64_t) mFreeHeapSize);
            res = GOAL_ERROR;
            break;
        }
        pPrevBlock = pBlock;
        pBlock = pBlock->pNextBlock;
    }

    if (GOAL_RES_OK(res)) {
        /* split blocks */
        /* pNewBlock will be placed behind pBlock contianing remaining HEAP memory
         * the blocksize mBlockStructSize is not considered in wantedSize
         * pBlock is going to be removed from list of blocks starting by (mStartBlock) */
        pNewBlock = (GOAL_HEAP_BLOCK_T *) (((uint8_t *) pBlock) + memSize);
        pNewBlock->heapBlockSize = pBlock->heapBlockSize - memSize;
        pBlock->heapBlockSize = wantedSize;

        /* add new block to list of free blocks */
        res = goal_drvMemHeapBlockInsert(pNewBlock);
    }

    if (GOAL_RES_OK(res)) {
        /* update dbg info */
        mAllocHeapSize += memSize;
        mFreeHeapSize -= memSize;
        mCntAllocBlocks++;
        goal_logDbg("allocate %8"FMT_u64" Bytes | allocated Bytes: %8"FMT_u64" | Blocks: %8"FMT_u64" | free Bytes: %8"FMT_u64,
                    (uint64_t) (memSize), (uint64_t) mAllocHeapSize, (uint64_t) mCntAllocBlocks, (uint64_t) mFreeHeapSize);

        /* setting ptrs, remove block pBlock out of list */
        pPrevBlock->pNextBlock = pBlock->pNextBlock;
        mpFirstFreeBlock = pNewBlock;
        pBlock->pNextBlock = NULL;
        pBlock->ident = GOAL_DRV_MEM_HEAP_IDENT;

        /* set return address to begin of free space,
         * block head struct is still intact */
        *ppRetBlock = (void *) (((uint8_t *) pBlock) + mBlockStructSize);
    }

    if (NULL != mpHeapLock) {
        goal_lockPut(mpHeapLock);
    }

    return res;
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
 * This function insert the requested block into the list of free blocks
 * optionally neighboring free blocks are merged together.
 *
 * @retval GOAL_OK successful
 * @retval other  fail
 */
static GOAL_STATUS_T goal_drvMemHeapFree(
    void *pBlock,                               /**< memory block to free */
    void *pData                                 /**< driver data */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_HEAP_BLOCK_T *pFreeBlock = NULL;       /* block to be freed */
    uint8_t *pStartAddress = (uint8_t *) pBlock; /* starting address of head */

    UNUSEDARG(pData);

    if (NULL == pBlock) {
        goal_logDbg("block to free is null");
        return GOAL_OK;
    }

    /* calculating start address */
    pStartAddress -= (uint8_t) mBlockStructSize;

    /* get block */
    pFreeBlock = (GOAL_HEAP_BLOCK_T *) pStartAddress;

    /* verify block identification */
    if (GOAL_DRV_MEM_HEAP_IDENT != pFreeBlock->ident) {
        return GOAL_ERR_PARAM;
    }

    /* lock heap stats */
    if (NULL != mpHeapLock) {
        res = goal_lockGet(mpHeapLock, GOAL_LOCK_INFINITE);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to acquire lock");
            return res;
        }
    }

    /* update dbg info */
    mAllocHeapSize -= (pFreeBlock->heapBlockSize + mBlockStructSize);
    mFreeHeapSize += (pFreeBlock->heapBlockSize + mBlockStructSize);
    mCntAllocBlocks --;
    goal_logDbg(" free     %8"FMT_u64" Bytes | allocated Bytes: %8"FMT_u64" | Blocks: %8"FMT_u64" | free Bytes: %8"FMT_u64,
                 (uint64_t) (pFreeBlock->heapBlockSize + mBlockStructSize), (uint64_t) mAllocHeapSize, (uint64_t) mCntAllocBlocks, (uint64_t) mFreeHeapSize);

    /* insert block into list */
    res = goal_drvMemHeapBlockInsert(pFreeBlock);

    if (GOAL_RES_OK(res)) {
        /* check if blocks could be merged */
        res = goal_drvMemHeapBlockMerge(pFreeBlock);
    }

    if (NULL != mpHeapLock) {
        goal_lockPut(mpHeapLock);
    }
    return res;
}


/****************************************************************************/
/** Realloc Block
 *
 * This function allocates a new block and copies the content of the old block
 * to the new one (up to the smallest size). If the new block is bigger, the
 * remaining bytes are uninitialized. The old block will be freed.
 * If @em pOldBlock is NULL the function call is equivalent to goal_drvMemHeapAlloc.
 * If @em wantedSize is 0 and @em pOldBlock is not NULL, the function call is
 * equivalent to goal_drvMemHeapFree.
 *
 * @retval GOAL_OK successful
 * @retval other  fail
 */
GOAL_STATUS_T goal_drvMemHeapRealloc(
    void **ppRetBlock,                          /**< [out] ptr to store memory to */
    void *pOldBlock,                            /**< [in] ptr to previously allocated memory */
    uint32_t wantedSize,                        /**< requested size to alloc */
    void *pData                                 /**< driver data */
)
{
    GOAL_HEAP_BLOCK_T *pBlockHeader;            /* header of old block */
    uint32_t cpySize = 0;                       /* size of data to copy */
    void *pNewBlock;                            /* newly allocated memory */
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    UNUSEDARG(pData);

    if ((0 != wantedSize) && (NULL == ppRetBlock)) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (pOldBlock) {
        /* verify block identification of old memory block */
        pBlockHeader = (GOAL_HEAP_BLOCK_T *) (((uint8_t *) pOldBlock) - ((uint8_t) mBlockStructSize));
        if ((uint32_t) GOAL_DRV_MEM_HEAP_IDENT != pBlockHeader->ident) {
            return GOAL_ERR_PARAM;
        }
    }

    if (0 != wantedSize) {
        res = goal_drvMemHeapAllocAlign(&pNewBlock, wantedSize, GOAL_TARGET_MEM_ALIGN_CPU, pData);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to allocate new memory");
        } else {
            if (pOldBlock) {
                pBlockHeader = (GOAL_HEAP_BLOCK_T *) (((uint8_t *) pOldBlock) - ((uint8_t) mBlockStructSize));

                /* get smaller size out of old and new block and copy old bytes */
                cpySize = (wantedSize > pBlockHeader->heapBlockSize) ? pBlockHeader->heapBlockSize : wantedSize;
                GOAL_MEMCPY(pNewBlock, pOldBlock, cpySize);
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


/****************************************************************************/
/** Insert Block to Heap
 *
 * This function insert requested block into the right position on the linked
 * list of free blocks. Additionally all reference pointers will be set.
 *
 * @retval GOAL_OK successful
 * @retval other  fail
 */
static GOAL_STATUS_T goal_drvMemHeapBlockInsert(
    GOAL_HEAP_BLOCK_T *pNewBlock                /**< memory block to insert */
)
{
    GOAL_HEAP_BLOCK_T *pPrevBlock = NULL;       /* ptr helper */
    GOAL_HEAP_BLOCK_T *pIteraturBlock = &mStartBlock; /* ptr helper */
    UNUSEDARG(pPrevBlock);

    if (NULL == pNewBlock) {
        goal_logErr("block to insert is null");
        return GOAL_ERR_NULL_POINTER;
    }

    /* searching the right position for pBlock */
    while (pIteraturBlock->pNextBlock < pNewBlock) {
        pPrevBlock = pIteraturBlock;
        pIteraturBlock = pIteraturBlock->pNextBlock;
    }

    /* insert block to list */
    pNewBlock->pNextBlock = pIteraturBlock->pNextBlock;
    pIteraturBlock->pNextBlock = pNewBlock;

    if (mpFirstFreeBlock >= pNewBlock) {
        /* edit marker */
        mpFirstFreeBlock = pNewBlock;
        mpFirstFreeBlock->pNextBlock = pNewBlock->pNextBlock;
        mpFirstFreeBlock->heapBlockSize = pNewBlock->heapBlockSize;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Merge Blocks
 *
 * This function merge neighboring free blocks.
 *
 * @retval GOAL_OK successful
 * @retval other  fail
 */
static GOAL_STATUS_T goal_drvMemHeapBlockMerge(
    GOAL_HEAP_BLOCK_T *pBlockToMerge            /**< memory block to merge */
)
{
    GOAL_HEAP_BLOCK_T *pIteraturBlock = &mStartBlock; /* ptr helper */
    GOAL_HEAP_BLOCK_T *pPrevBlock = NULL;       /* ptr helper */
    uint8_t* addressPrev;                        /* ptr helper */
    uint8_t* address;                            /* ptr helper */
    uint8_t* addressNext;                        /* ptr helper */

    if (NULL == pBlockToMerge) {
        goal_logErr("block to merge is null");
        return GOAL_ERR_NULL_POINTER;
    }

    /* searching the right pos for pBlock */
    while (pIteraturBlock->pNextBlock <= pBlockToMerge) {
        pPrevBlock = pIteraturBlock;
        pIteraturBlock = pIteraturBlock->pNextBlock;
    }

    /* create markers */
    addressPrev = (uint8_t *) pPrevBlock;
    address = (uint8_t *) pBlockToMerge;
    addressNext = (uint8_t *) pIteraturBlock->pNextBlock;

    /* check if prev block are mergeable */
    if (addressPrev + mBlockStructSize + pPrevBlock->heapBlockSize == address) {
        pPrevBlock->heapBlockSize += pBlockToMerge->heapBlockSize + mBlockStructSize;
        pBlockToMerge = pPrevBlock;
        pBlockToMerge->pNextBlock = pBlockToMerge->pNextBlock->pNextBlock;
        /* update address for ensuing calculations */
        address = (uint8_t *) pBlockToMerge;
    }

    /* check if following block are mergeable */
    if (address + mBlockStructSize + pBlockToMerge->heapBlockSize == addressNext) {
        if (pIteraturBlock->pNextBlock != mpEndBlock) {
            pBlockToMerge->heapBlockSize += pIteraturBlock->pNextBlock->heapBlockSize + mBlockStructSize;
            pBlockToMerge->pNextBlock = pIteraturBlock->pNextBlock->pNextBlock;
        }
        else {
            pBlockToMerge->pNextBlock = mpEndBlock;
        }
    }

    return GOAL_OK;
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
    GOAL_HEAP_REGION_T *pHeapRegion;            /* heap region reference */

    UNUSEDARG(pStatus);

    pHeapRegion = (GOAL_HEAP_REGION_T *) pData;

    pStatus->size = pHeapRegion->heapRegionSize;
    pStatus->used = mAllocHeapSize;

    return GOAL_OK;
}
