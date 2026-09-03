/** @file
 *
 * @brief Simple Static Memory Allocator
 *
 * This is a simple memory allocator, which allocates memory from dynamic heap
 * and assigns it as static heap to various modules. It is @b not possible to
 * free the memory once it has been taken. As a debug feature you can enable
 * memory fences by define.
 * With this feature enabled, the allocated memory buffers don't follow each
 * other in memory but have some kind of check variable between them. If a bug
 * causes the buffer to overflow there is a good chance that the check variable
 * is overwritten and the allocator detects it.
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

#define GOAL_ID GOAL_ID_MEM
#include <goal_includes.h>
#include <goal_media/goal_ma_heap.h>
#if GOAL_CONFIG_MM_EXT == 0
#include <plat/drv/mem/heap/goal/mem_heap_goal.h>
#endif


/****************************************************************************/
/* Type definitions */
/****************************************************************************/
#if GOAL_CONFIG_DEBUG_HEAP_USAGE == 1
typedef struct GOAL_ALLOC_LIST_ENTRY_T {
    struct GOAL_ALLOC_LIST_ENTRY_T *pNext;      /**< next list element */
    uint16_t id;                                /**< list entry id */
    uint32_t cntAlloc;                          /**< list entry allocation */
} GOAL_ALLOC_LIST_ENTRY_T;
#endif


/****************************************************************************/
/* Local Defines */
/****************************************************************************/
#define GOAL_MEM_DENY_HANDLE_MAX 32             /**< max nr of deny handles */
#define GOAL_MEM_OFS_BIT_ALIGN 28               /**< alignment bit offset */


/****************************************************************************/
/* Heap Area */
/****************************************************************************/
static GOAL_MEM_AREA_T memArea;                 /**< global static memory area */


/****************************************************************************/
/* Local Variables */
/****************************************************************************/
static GOAL_STAGE_HANDLER_T stageInitMemPre;    /**< mem pre init stage handler */
static GOAL_STAGE_HANDLER_T stageInitLock;      /**< lock init stage handler */
static GOAL_BOOL_T memFlgLock = GOAL_FALSE;     /**< mem lock available */
static GOAL_BOOL_T memFlgDeny = GOAL_FALSE;     /**< mem allocation deny flag */
static uint32_t memBmDenyDelay = 0;             /**< memory deny delay bitmap */
static GOAL_MEM_DENY_HANDLE_T memBmDenyDelayCnt = 0; /**< handle counter */
#if GOAL_CONFIG_DEBUG_HEAP_USAGE == 1
static GOAL_ALLOC_LIST_ENTRY_T *pListAlloc;     /**< linked list */
#endif
#if GOAL_CONFIG_DEBUG_MEM_FENCES == 1
static GOAL_BOOL_T mFlgMemErr;                  /**< memory error detection flag */
#endif


/****************************************************************************/
/* Local Prototypes */
/****************************************************************************/
static GOAL_STATUS_T goal_memPreInit(
    void
);

static GOAL_STATUS_T goal_memLockInit(
    void
);


/****************************************************************************/
/** Register stage handler
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_memInitPre(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

#if GOAL_CONFIG_DEBUG_MEM_FENCES == 1
    /* initialize memory fences */
    memArea.watchPre = GOAL_MEM_MGMT_WATCH_CONT;
    memArea.watchPost = GOAL_MEM_MGMT_WATCH_CONT;
#endif

    /* register lock init stage */
    res = goal_mainStageReg(GOAL_STAGE_LOCK, &stageInitLock, GOAL_STAGE_INIT, goal_memLockInit);

    /* register mem init stage */
    if (GOAL_RES_OK(res)) {
        res = goal_mainStageReg(GOAL_STAGE_MEM_PRE, &stageInitMemPre, GOAL_STAGE_INIT, goal_memPreInit);
    }

    return res;
}


/****************************************************************************/
/** Initialize Memory Lock
 *
 * Initialize memory lock after locking subsystem started.
 *
 * @retval GOAL_OK successful
 * @retval other fail
 */
static GOAL_STATUS_T goal_memPreInit(
    void
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

#if GOAL_CONFIG_MM_EXT == 0
    /* create GOAL HEAP if no external HEAP is available */
    res = goal_drvMemHeapCreate();
#endif

    /* allocate a heap segment for static memory allocation */
    if (GOAL_RES_OK(res)) {
        res = goal_heapCalloc(&memArea.pArea, GOAL_CONFIG_HEAP_SIZE);
    }

    return res;
}


/****************************************************************************/
/** Initialize Memory Lock
 *
 * Initialize memory lock after locking subsystem started.
 *
 * @retval GOAL_OK successful
 * @retval other fail
 */
static GOAL_STATUS_T goal_memLockInit(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    res = goal_lockCreate(GOAL_LOCK_BINARY, &memArea.pLock, 0, 1, GOAL_ID_MEM_POOL);
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:70: */
        goal_lmLog(GOAL_ID_MEM, 70, 41, 0, 0, GOAL_LOG_SEV_ERROR, "failed to create memory area lock");
        return res;
    }

    memFlgLock = GOAL_TRUE;

#if GOAL_CONFIG_DEBUG_MEM_FENCES == 1
    /* register memory fence check in main loop */
    goal_mainLoopReg(goal_memCheck);
#endif

    return GOAL_OK;
}


/****************************************************************************/
/** Disable Memory Allocation
 *
 * GOAL doesn't support dynamic memory management. Every allocation attempt
 * will be denied after the GOAL setup is done.
 */
void goal_memInitDone(
    void
)
{
#if GOAL_CONFIG_LOGGING == 1
    uint32_t ratio;                             /* ratio used/unused mem */
    GOAL_MA_HEAP_STATS_T stats;                 /* dynamic memory statistics */
    GOAL_STATUS_T res;                          /* result */
#endif

#if GOAL_CONFIG_DEBUG_HEAP_USAGE == 1
    goal_memDump();
#endif

    /* set allocation deny flag */
    memFlgDeny = GOAL_TRUE;

#if GOAL_CONFIG_LOGGING == 1
    if (0 == memBmDenyDelay) {
        /* print memory statistics */
        ratio = 100 - ((GOAL_CONFIG_HEAP_SIZE - memArea.used) * 100 / GOAL_CONFIG_HEAP_SIZE);

        /* GG_LOG ID:71: */
        goal_lmLog(GOAL_ID_MEM, 71, 42, 4, 4, GOAL_LOG_SEV_INFO, "fixed memory usage: $1/$2 bytes");
        goal_lmLogParamUINT32(memArea.used);
        goal_lmLogParamUINT32(GOAL_CONFIG_HEAP_SIZE);
        goal_lmLogFinish();
        /* GG_LOG ID:72: */
        goal_lmLog(GOAL_ID_MEM, 72, 43, 4, 0, GOAL_LOG_SEV_INFO, "fixed memory usage: ($1%)");
        goal_lmLogParamUINT32(ratio);
        goal_lmLogFinish();

        /* print dynamic memory stats */
        res = goal_heapStatsGet(&stats);
        if (GOAL_RES_OK(res)) {
            /* print memory statistics */
            ratio = 100 - ((stats.size - stats.used) * 100 / stats.size);

            goal_logInfo("dynamic memory usage: %"FMT_u32"/%"FMT_u32" bytes", stats.used, stats.size);
            goal_logInfo("dynamic memory usage: (%"FMT_u32"%%)", ratio);
        }
    }
#endif
}


/****************************************************************************/
/** Allocate Aligned Buffer
 *
 * This function tries to allocate a buffer of the given size from the memory
 * array and aligns it at the given value.
 *
 * @retval GOAL_OK successful
 * @retval other  fail
 */
GOAL_STATUS_T goal_memAllocAlignImpl(
    GOAL_ID_T id,                               /**< caller id */
    void **ppMem,                               /**< [out] ptr to store buf ptr to */
    uint32_t memLen,                            /**< size of buffer to alloc */
    uint32_t align                              /**< alignment */
)
{
    uint8_t *pBeg;                              /* begin pointer */
    uint8_t *pHdr;                              /* header pointer */
    uint8_t *pData;                             /* data pointer */
    uint8_t *pEnd;                              /* end pointer */
#if GOAL_CONFIG_DEBUG_MEM_FENCES == 1
    uint8_t *pFencePre;                         /* pre-fence pointer */
    uint8_t *pFencePost;                        /* post-fence pointer */
#endif
    uint32_t len;                               /* real area length */

    /* unused if logging is disabled */
    UNUSEDARG(id);

#if GOAL_CONFIG_DEBUG_MEM_FENCES == 1
    uint32_t watch = GOAL_MEM_MGMT_WATCH_CONT;  /* fence data */
#endif

    /* deny allocation after GOAL and its componentens are fully initialized */
    if ((memFlgDeny) && (!memBmDenyDelay)) {
        /* GG_LOG ID:73: */
        goal_lmLog(GOAL_ID_MEM, 73, 44, 0, 0, GOAL_LOG_SEV_ERROR, "GOAL fully initialized -> alloc denied");
        return GOAL_ERR_ACCESS;
    }

    /* catch empty length */
    if (!memLen) {
        *(ppMem) = NULL;
        return GOAL_OK;
    }

    /* check if static memory region was allocated */
    if (NULL == memArea.pArea) {
        return GOAL_ERR_NOT_INITIALIZED;
    }

    /* lock memory area */
    if (GOAL_TRUE == memFlgLock) {
        goal_lockGet(memArea.pLock, GOAL_LOCK_INFINITE);
    }

    /* calculate pointers */
    pBeg = (uint8_t *) &memArea.pArea[memArea.used];
    pHdr = GOAL_alignPtr(GOAL_TARGET_MEM_ALIGN_CPU, pBeg);
    pData = pHdr + GOAL_MEM_MGMT_SIZE_LEN;

#if GOAL_CONFIG_DEBUG_MEM_FENCES == 1
    /* add fence to aligned position */
    pFencePre = pData;
    pData = pFencePre + GOAL_MEM_MGMT_WATCH_LEN;
#endif

    pData = GOAL_alignPtr(align, pData);
    pEnd = pData + memLen;

#if GOAL_CONFIG_DEBUG_MEM_FENCES == 1
    /* adjust pre-fence address */
    pFencePre = pData - GOAL_MEM_MGMT_WATCH_LEN;

    /* calc post-fence address */
    pFencePost = pEnd;
    pEnd = pFencePost + GOAL_MEM_MGMT_WATCH_LEN;
#endif

    /* calculate memory area usage */
    len = memArea.used + (uint32_t) (pEnd - pBeg);

    /* check if memory area is big enough */
    if (GOAL_CONFIG_HEAP_SIZE < len) {
        /* GG_LOG ID:74: */
        goal_lmLog(GOAL_ID_MEM, 74, 45, 4, 4, GOAL_LOG_SEV_ERROR, "could not allocate $1 bytes (gross:$2bytes), out of memory");
        goal_lmLogParamUINT32(memLen);
        goal_lmLogParamUINT32((uint32_t) (pEnd - pBeg));
        goal_lmLogFinish();
        if (GOAL_TRUE == memFlgLock) {
            goal_lockPut(memArea.pLock);
        }
        return GOAL_ERR_ALLOC;
    }

#if GOAL_CONFIG_ALLOC_HEADER == 1
    /* write header (length and alignment) */
    *(uint32_t *) pHdr = (uint32_t) (memLen | (align << GOAL_MEM_OFS_BIT_ALIGN));

    /* write header (id) */
    (*(((uint32_t *) pHdr) + 1)) = (uint32_t) id;
#endif

#if GOAL_CONFIG_DEBUG_MEM_FENCES == 1
    /* write pre-fence */
    GOAL_MEMCPY(pFencePre, &watch, GOAL_MEM_MGMT_WATCH_LEN);

    /* write post-fence */
    GOAL_MEMCPY(pFencePost, &watch, GOAL_MEM_MGMT_WATCH_LEN);
#endif

    /* assign data pointer */
    *ppMem = pData;

    /* set new memory area end */
    memArea.used += (uint32_t) (pEnd - pBeg);
    memArea.cnt++;

    /* unlock memory area */
    if (GOAL_TRUE == memFlgLock) {
        goal_lockPut(memArea.pLock);
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Allocate and initialize an aligned buffer with zeros
 *
 * This implementation simulates the calloc behaviour without the split into
 * members and size.
 *
 * @retval GOAL_OK successful
 * @retval other  fail
 */
GOAL_STATUS_T goal_memCallocAlignImpl(
    GOAL_ID_T id,                               /**< caller id */
    void **ppMem,                               /**< [out] ptr to store buf ptr to */
    uint32_t memLen,                            /**< size of buffer to alloc */
    uint32_t align                              /**< alignment */
)
{
    GOAL_STATUS_T res;                          /* result */

    /* allocate memory */
    res = goal_memAllocAlignImpl(id, ppMem, memLen, align);
    if (GOAL_RES_OK(res)) {
        /* fill memory with given pattern */
        GOAL_MEMSET(*(ppMem), 0, memLen);
    }
    return res;
}


/****************************************************************************/
/** Realloc a buffer
 *
 * This implementation simulates the relocate behaviour by allocating a new
 * memory segment and copying the data into it.
 * If the new section is greater than the old one, the memory will be filled
 * invalid content.
 *
 * @retval GOAL_OK successful
 * @retval other  fail
 */
GOAL_STATUS_T goal_memReallocImpl(
    void **ppMemNew,                            /**< [out] new ptr of buffer for reallocation */
    void *pMemOld,                              /**< old ptr of buffer for reallocation */
    uint32_t memLen                             /**< new size of buffer */
)
{
    GOAL_STATUS_T res;                          /* result */

    goal_logDbg("memory realloc not implemented");

    /* alloc the new region */
    res = goal_memCalloc(ppMemNew, memLen);
    if (GOAL_RES_OK(res)) {
        GOAL_MEMCPY((*ppMemNew), pMemOld, memLen);
        res = goal_memFree(pMemOld);
    }

    return res;
}


/****************************************************************************/
/** Free a buffer
 *
 * This implementation simulates the free behaviour and sets the given memory
 * pointer to zero.
 *
 * @retval GOAL_OK successful
 * @retval other  fail
 */
GOAL_STATUS_T goal_memFreeImpl(
    GOAL_ID_T id,                               /**< caller id */
    void *pMem                                  /**< ptr to store buf ptr to */
)
{
    UNUSEDARG(id);

    /* GG_LOG ID:75: */
    goal_logDbg("memory free not implemented");
    *((void **) pMem) = NULL;
    return GOAL_OK;
}


#if GOAL_CONFIG_DEBUG_MEM_FENCES == 1
/****************************************************************************/
/** Check the Memory Stream for Consistency
 *
 * If memory checking is enabled, this function checks the fences around the
 * allocated buffers.
 *
 * @retval GOAL_OK successful
 * @retval other  fail
 */
void goal_memCheck(
    void
)
{
    uint8_t *pMax;                              /* highest pointer value */
    uint8_t *pHdr;                              /* header pointer */
    uint8_t *pData;                             /* data pointer */
    uint8_t *pEnd;                              /* end pointer */
    uint8_t *pFencePre;                         /* pre-fence pointer */
    uint8_t *pFencePost;                        /* post-fence pointer */
    uint32_t entryLen;                          /* entry length */
    uint32_t entryAlign;                        /* entry alignment */
    uint32_t watch;                             /* fence data */

    /* only run if permanent error flag isn't set */
    if (GOAL_TRUE == mFlgMemErr) {
        return;
    }

    /* lock memory area */
    goal_lockGet(memArea.pLock, GOAL_LOCK_INFINITE);

    /* check fences of memory area */
    if ((GOAL_MEM_MGMT_WATCH_CONT != memArea.watchPre) ||
        (GOAL_MEM_MGMT_WATCH_CONT != memArea.watchPost)) {

        goal_lockPut(memArea.pLock);
        /* GG_LOG ID:76: */
        goal_lmLog(GOAL_ID_MEM, 76, 47, 0, 0, GOAL_LOG_SEV_ERROR, "main pre and post fence not detected");
        return;
    }

    pHdr = (uint8_t *) memArea.pArea;
    pMax = (uint8_t *) ((PtrCast) memArea.pArea + (PtrCast) memArea.used);
    watch = GOAL_MEM_MGMT_WATCH_CONT;

    while (pMax > pHdr) {

        /* align header pointer */
        pHdr = GOAL_alignPtr(GOAL_TARGET_MEM_ALIGN_CPU, pHdr);

        /* get length and alignment */
        entryLen = (*(uint32_t *) pHdr) & 0x0fffffff;
        entryAlign = ((*(uint32_t *) pHdr) >> GOAL_MEM_OFS_BIT_ALIGN) & 0xf;

        /* calculate pointer positions */
        pData = pHdr + GOAL_MEM_MGMT_SIZE_LEN;

        /* add fence to aligned position */
        pFencePre = pData;
        pData = pFencePre + GOAL_MEM_MGMT_WATCH_LEN;

        pData = GOAL_alignPtr(entryAlign, pData);
        pEnd = pData + entryLen;

        /* adjust pre-fence address */
        pFencePre = pData - GOAL_MEM_MGMT_WATCH_LEN;

        /* calc post-fence address */
        pFencePost = pEnd;
        pEnd = pFencePost + GOAL_MEM_MGMT_WATCH_LEN;

        /* check pre-fence */
        if (GOAL_CMP_EQUAL != GOAL_MEMCMP(pFencePre, &watch, GOAL_MEM_MGMT_WATCH_LEN)) {
            /* GG_LOG ID:77: */
            goal_lmLog(GOAL_ID_MEM, 77, 48, 8, 0, GOAL_LOG_SEV_ERROR, "pre-fence error at mem: $1");
            goal_lmLogParamPTR((void *) pData);
            goal_lmLogFinish();
            goal_lockPut(memArea.pLock);

            /* set permanent memory error flag */
            mFlgMemErr = GOAL_TRUE;

            return;
        }

        /* check post-fence */
        if (GOAL_CMP_EQUAL != GOAL_MEMCMP(pFencePost, &watch, GOAL_MEM_MGMT_WATCH_LEN)) {
            /* GG_LOG ID:78: */
            goal_lmLog(GOAL_ID_MEM, 78, 49, 8, 0, GOAL_LOG_SEV_ERROR, "post-fence error at mem: $1");
            goal_lmLogParamPTR((void *) pData);
            goal_lmLogFinish();
            goal_lockPut(memArea.pLock);

            /* set permanent memory error flag */
            mFlgMemErr = GOAL_TRUE;

            return;
        }

        /* next entry starts on CPU boundary */
        pHdr = pEnd;
    }

    /* unlock memory area */
    goal_lockPut(memArea.pLock);
}


/****************************************************************************/
/** Memory Error Detection State
 *
 * Returns the state of the permanent memory error detection flag.
 *
 * @retval GOAL_TRUE memory error detected
 * @retval GOAL_FALSE memory ok
 */
GOAL_BOOL_T goal_memStateErrGet(
    void
)
{
    return mFlgMemErr;
}
#endif /* GOAL_CONFIG_DEBUG_MEM_FENCES == 1 */


#if GOAL_CONFIG_DEBUG_HEAP_USAGE == 1
/****************************************************************************/
/** Dump Memory Usage
 *
 * Dump the current memory usage.
 */
void goal_memDump(
    void
)
{
    uint8_t *pMax;                              /* highest pointer value */
    uint8_t *pHdr;                              /* header pointer */
    uint8_t *pData;                             /* data pointer */
    uint8_t *pDataAligned;                      /* aligned data pointer */
    uint8_t *pEnd;                              /* end pointer */
#if GOAL_CONFIG_DEBUG_MEM_FENCES == 1
    uint8_t *pFencePre;                         /* pre-fence pointer */
    uint8_t *pFencePost;                        /* post-fence pointer */
#endif
    uint32_t entryLen;                          /* entry length */
    uint32_t entryAlign;                        /* entry alignment */
    uint32_t cnt;                               /* counter */
    uint32_t ratio;                             /* ratio used/unused mem */
    uint32_t cntAlign = 0;                      /* alignment overhead */
    GOAL_ID_T id = 0;                           /* allocation id */
    GOAL_ALLOC_LIST_ENTRY_T *pEntry;            /* list iterator */
    GOAL_ALLOC_LIST_ENTRY_T *pEntryFound;       /* list entry */
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    /* lock memory area */
    goal_lockGet(memArea.pLock, GOAL_LOCK_INFINITE);

    cnt = 0;
    pHdr = (uint8_t *) memArea.area;
    pMax = (uint8_t *) ((PtrCast) memArea.area + (PtrCast) memArea.used);

    while (pMax > pHdr) {

        /* align header pointer */
        pHdr = GOAL_alignPtr(GOAL_TARGET_MEM_ALIGN_CPU, pHdr);

        /* get length and alignment */
        entryLen = (*(uint32_t *) pHdr) & 0x0fffffff;
        entryAlign = ((*(uint32_t *) pHdr) >> GOAL_MEM_OFS_BIT_ALIGN) & 0xf;

        /* calculate pointer positions */
        pData = pHdr + GOAL_MEM_MGMT_SIZE_LEN;

        /* get id from header */
        id = (uint16_t) (*(((uint32_t *) pHdr) + 1));

#if GOAL_CONFIG_DEBUG_MEM_FENCES == 1
        /* add fence to aligned position */
        pFencePre = pData;
        pData = pFencePre + GOAL_MEM_MGMT_WATCH_LEN;
#endif

        /* align data address */
        pDataAligned = GOAL_alignPtr(entryAlign, pData);

        /* build sum of alignment overhead */
        cntAlign += pDataAligned - pData;

        pData = pDataAligned;
        pEnd = pData + entryLen;

#if GOAL_CONFIG_DEBUG_MEM_FENCES == 1
        /* adjust pre-fence address */
        pFencePre = pData - GOAL_MEM_MGMT_WATCH_LEN;

        /* calc post-fence address */
        pFencePost = pEnd;
        pEnd = pFencePost + GOAL_MEM_MGMT_WATCH_LEN;
#endif

        cnt++;

#if GOAL_CONFIG_DEBUG_HEAP_USAGE_ENTRIES == 1
        /* GG_LOG ID:79: */
        goal_lmLog(GOAL_ID_MEM, 79, 50, 4, 0, GOAL_LOG_SEV_INFO, "$1");
        goal_lmLogParamUINT32(cnt);
        goal_lmLogFinish();

        /* GG_LOG ID:80: */
        goal_lmLog(GOAL_ID_MEM, 80, 51, 8, 8, GOAL_LOG_SEV_INFO, "    : raw: $1, ptr: $2");
        goal_lmLogParamPTR((void *) pHdr);
        goal_lmLogParamPTR((void *) pData);
        goal_lmLogFinish();

        /* GG_LOG ID:81: */
        goal_lmLog(GOAL_ID_MEM, 81, 52, 4, 4, GOAL_LOG_SEV_INFO, "    : len: $1, align: $2");
        goal_lmLogParamUINT32(entryLen);
        goal_lmLogParamUINT32(entryAlign);
        goal_lmLogFinish();
#endif

        /* search for existing entry */
        pEntryFound = NULL;
        GOAL_LL_FOREACH(pListAlloc, pEntry) {
            if (pEntry->id == id) {
                pEntryFound = pEntry;
                break;
            }
        }

        if (NULL == pEntryFound) {
            res = goal_memCalloc(&pEntryFound, sizeof(GOAL_ALLOC_LIST_ENTRY_T));

            if (GOAL_RES_OK(res)) {
                pEntryFound->id = id;
                pEntryFound->cntAlloc = 0;
                GOAL_LL_APPEND(pListAlloc, pEntryFound);
            }
        }

        /* sum up allocation */
        pEntryFound->cntAlloc += entryLen;

        /* go to next header */
        pHdr = pEnd;
    }

    /* show allocation of all modules */
    GOAL_LL_FOREACH(pListAlloc, pEntry) {
        id = pEntry->id;
        entryLen = pEntry->cntAlloc;

        /* GG_LOG ID:482: */
        goal_lmLog(GOAL_ID_MEM, 482, 399, 2, 4, GOAL_LOG_SEV_INFO, "heap usage id($1): $2 Bytes");
        goal_lmLogParamUINT16(id);
        goal_lmLogParamUINT32(entryLen);
        goal_lmLogFinish();
    }

    /* unlock memory area */
    goal_lockPut(memArea.pLock);

    ratio = 100 - ((GOAL_CONFIG_HEAP_SIZE - memArea.used) * 100 / GOAL_CONFIG_HEAP_SIZE);

    /* GG_LOG ID:483: */
    goal_lmLog(GOAL_ID_MEM, 483, 400, 4, 0, GOAL_LOG_SEV_INFO, "memory allocation number: $1");
    goal_lmLogParamUINT32(cnt);
    goal_lmLogFinish();
    /* GG_LOG ID:484: */
    goal_lmLog(GOAL_ID_MEM, 484, 401, 4, 0, GOAL_LOG_SEV_INFO, "memory allocation alignment overhead: $1");
    goal_lmLogParamUINT32(cntAlign);
    goal_lmLogFinish();
    /* GG_LOG ID:82: */
    goal_lmLog(GOAL_ID_MEM, 82, 53, 4, 4, GOAL_LOG_SEV_INFO, "memory usage: $1/$2 bytes");
    goal_lmLogParamUINT32(memArea.used);
    goal_lmLogParamUINT32(GOAL_CONFIG_HEAP_SIZE);
    goal_lmLogFinish();
    /* GG_LOG ID:83: */
    goal_lmLog(GOAL_ID_MEM, 83, 54, 4, 0, GOAL_LOG_SEV_INFO, "memory usage: ($1%)");
    goal_lmLogParamUINT32(ratio);
    goal_lmLogFinish();
}
#endif /* GOAL_CONFIG_LOGGING == 1 */


/****************************************************************************/
/** Get handle to delay memory alloc deny after GOAL_STAGE_GOAL
 *
 * In GOAL_STAGE_GOAL every memory allocation attempt is normally blocked to
 * get a fixed memory usage. If the application requires that even after
 * GOAL_STAGE_GOAL allocation is available the block can be delayed by
 * requesting a delay handle. As long as the handle is active memory allocation
 * is possible. To close the handle call goal_memDenayDelayHandleClose.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_memDenyDelayHandleGet(
    GOAL_MEM_DENY_HANDLE_T *pHdl                /**< ptr to store handle */
)
{
    /* deny handle request in GOAL_STAGE_GOAL */
    if (memFlgDeny) {
        return GOAL_ERR_WRONG_STATE;
    }

    if (GOAL_MEM_DENY_HANDLE_MAX < memBmDenyDelayCnt) {
        return GOAL_ERR_FULL;
    }

    /* enable handle */
    memBmDenyDelay |= (1 << memBmDenyDelayCnt);

    /* return handle to caller */
    *pHdl = memBmDenyDelayCnt;

    /* increase current handle count */
    memBmDenyDelayCnt++;

    return GOAL_OK;
}


/****************************************************************************/
/** Release handle to delay memory alloc deny after GOAL_STAGE_GOAL
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_memDenyDelayHandleRelease(
    GOAL_MEM_DENY_HANDLE_T hdl                  /**< handle */
)
{
    /* release handle */
    memBmDenyDelay &= ~(1 << hdl);

    /* if there are no more handles available, call goal_memInitDone to log
     * memory stats */
    if (0 == memBmDenyDelay) {
        goal_memInitDone();
    }
    return GOAL_OK;
}
