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

#ifndef GOAL_ALLOC_H
#define GOAL_ALLOC_H


/****************************************************************************/
/* Defines */
/****************************************************************************/
#ifndef GOAL_CONFIG_HEAP_SIZE
#  define GOAL_CONFIG_HEAP_SIZE (32 * 1024)     /**< default static GOAL heap size */
#endif

#if GOAL_CONFIG_DEBUG_MEM_FENCES == 1
#  define GOAL_MEM_MGMT_WATCH_LEN     sizeof(uint32_t) /**< length of watch entry */
#  define GOAL_MEM_MGMT_WATCH_CONT    0x9B71DCAB /**< watch entry content */
#else
#  define GOAL_MEM_MGMT_WATCH_LEN     0         /**< length of watch entry */
#endif

/* only insert header if required */
#if GOAL_CONFIG_DEBUG_HEAP_USAGE == 1 || GOAL_CONFIG_DEBUG_MEM_FENCES == 1
# define GOAL_CONFIG_ALLOC_HEADER      1        /**< enable heap element header */
# define GOAL_MEM_MGMT_SIZE_LEN        (2 * sizeof(uint32_t)) /**< length of header */
# else
# define GOAL_CONFIG_ALLOC_HEADER      0        /**< disable heap element header */
# define GOAL_MEM_MGMT_SIZE_LEN        0        /**< length of header */
#endif


/****************************************************************************/
/* Structures */
/****************************************************************************/

/**< global static memory structure */
typedef struct {
#if GOAL_CONFIG_DEBUG_MEM_FENCES == 1
    uint32_t watchPre;                          /**< pre-fence */
#endif

    volatile uint32_t used;                     /**< global static memory usage index */
    volatile uint32_t cnt;                      /**< allocation count */
    GOAL_LOCK_T *pLock;                         /**< area lock */
    char *pArea;                                /**< heap area */

#if GOAL_CONFIG_DEBUG_MEM_FENCES == 1
    uint32_t watchPost;                         /**< post-fence */
#endif
} GOAL_MEM_AREA_T;


typedef uint32_t GOAL_MEM_DENY_HANDLE_T;        /**< deny delay handle */


/****************************************************************************/
/* Mapping */
/****************************************************************************/
#define goal_memAlloc(ppMem, menLen) goal_memAllocAlignImpl(GOAL_ID, (void **) ppMem, menLen, GOAL_TARGET_MEM_ALIGN_NET)
#define goal_memCalloc(ppMem, menLen) goal_memCallocAlignImpl(GOAL_ID, (void **) ppMem, menLen, GOAL_TARGET_MEM_ALIGN_NET)
#define goal_memRealloc(ppMemNew, pMemOld, menLen) goal_memReallocImpl(GOAL_ID, (void **) ppMemNew, pMemOld, menLen)
#define goal_memFree(pMem) goal_memFreeImpl(GOAL_ID, pMem)
#define goal_memAllocAlign(ppMem, menLen, align) goal_memAllocAlignImpl(GOAL_ID, (void **) ppMem, menLen, align)
#define goal_memCallocAlign(ppMem, menLen, align) goal_memCallocAlignImpl(GOAL_ID, (void **) ppMem, menLen, align)


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_memInitPre(
    void
);

void goal_memInitDone(
    void
);

GOAL_STATUS_T goal_memAllocAlignImpl(
    GOAL_ID_T id,                               /**< caller id */
    void **ppMem,                               /**< [out] ptr to store buf ptr to */
    uint32_t memLen,                            /**< size of buffer to alloc */
    uint32_t align                              /**< alignment */
);

GOAL_STATUS_T goal_memCallocAlignImpl(
    GOAL_ID_T id,                               /**< caller id */
    void **ppMem,                               /**< [out] ptr to store buf ptr to */
    uint32_t memLen,                            /**< size of buffer to alloc */
    uint32_t align                              /**< alignment */
);

GOAL_STATUS_T goal_memReallocImpl(
    void **ppMemNew,                            /**< [out] new ptr of buffer for reallocation */
    void *pMemOld,                              /**< old ptr of buffer for reallocation */
    uint32_t memLen                             /**< new size of buffer */
);

GOAL_STATUS_T goal_memFreeImpl(
    GOAL_ID_T id,                               /**< caller id */
    void *pMem                                  /**< ptr to store buf ptr to */
);

#if GOAL_CONFIG_DEBUG_MEM_FENCES == 1
void goal_memCheck(
    void
);

GOAL_BOOL_T goal_memStateErrGet(
    void
);
#endif

#if GOAL_CONFIG_DEBUG_HEAP_USAGE == 1
void goal_memDump(
    void
);
#endif

GOAL_STATUS_T goal_memDenyDelayHandleGet(
    GOAL_MEM_DENY_HANDLE_T *pHdl                /**< ptr to store handle */
);

GOAL_STATUS_T goal_memDenyDelayHandleRelease(
    GOAL_MEM_DENY_HANDLE_T hdl                  /**< handle */
);


#endif /* GOAL_ALLOC_H */
