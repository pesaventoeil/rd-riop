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
#ifndef MEM_HEAP_GOAL_H
#define MEM_HEAP_GOAL_H


/****************************************************************************/
/* Structures */
/****************************************************************************/
typedef struct GOAL_HEAP_REGION_T {
    uint8_t *pStartAddress;                     /**< start address of heap */
    uint32_t heapRegionSize;                    /**< size of heap in bytes */
} GOAL_HEAP_REGION_T;

typedef struct GOAL_HEAP_BLOCK_T {
    struct GOAL_HEAP_BLOCK_T *pNextBlock;       /**< next block */
    GOAL_ID_T id;                               /**< owner */
    uint32_t ident;                             /**< heap identification */
    uint32_t heapBlockSize;                     /**< size of heap block (excl. size of GOAL_HEAP_BLOCK_T) in bytes */
} GOAL_HEAP_BLOCK_T;

typedef struct GOAL_HEAP_STATUS_T {
    uint32_t mTotalHeapSize;                    /**< size of heap */
    uint32_t mHeapSizeUsed;                     /**< size of alloc blocks */
    uint8_t mCntBlocksUsed;                     /**< number of used blocks */
} GOAL_HEAP_STATUS_T;


/****************************************************************************/
/* Makros */
/****************************************************************************/
#define GOAL_CONFIG_HEAP_DYNAMIC_HEADER_SIZE (sizeof(GOAL_HEAP_REGION_T) \
                                + (3 * sizeof(GOAL_HEAP_BLOCK_T))) /**< size of dynamic HEAP header */


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_drvMemHeapCreate(
    void
);

#endif /* MEM_HEAP_GOAL_H */
