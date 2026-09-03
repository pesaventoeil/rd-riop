/** @file
 *
 * @brief HEAP application
 *
 * This module is an application, showing the HEAP access by running some tests.
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

#include "goal_includes.h"
#include "goal_media/goal_ma_heap.h"


/****************************************************************************/
/* Local Defines */
/****************************************************************************/
#define APPL_TEST_PATTERN_SIZE  62
#define APPL_BUFFERS 32
#define APPL_TIMEOUT GOAL_TIMER_SEC


/****************************************************************************/
/* test pattern */
/****************************************************************************/
static const uint8_t testPattern [APPL_TEST_PATTERN_SIZE] = {
    0x01, 0x00, 0x5e, 0x00, 0x00, 0x16, 0xb4, 0x0e, 0xde, 0x75, 0x41, 0xbc, 0x08, 0x00, 0x46, 0xc0,
    0x00, 0x30, 0x00, 0x00, 0x40, 0x00, 0x01, 0x02, 0x91, 0x2a, 0xc0, 0xa8, 0xb2, 0x1e, 0xe0, 0x00,
    0x00, 0x16, 0x94, 0x04, 0x00, 0x00, 0x22, 0x00, 0x09, 0x07, 0x00, 0x00, 0x00, 0x02, 0x02, 0x00,
    0x00, 0x00, 0xef, 0xff, 0xff, 0xfa, 0x02, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0xfb
};
static GOAL_TIMESTAMP_T tsTout;


/****************************************************************************/
/** Application Init
 *
 * Initialize required GOAL components like protocols.
 *
 * Note: If the function body is empty the whole function can be removed.
 */
GOAL_STATUS_T appl_init(
    void
)
{
    return GOAL_OK;
}


/****************************************************************************/
/** Application Setup
 *
 * Configure GOAL and components.
 *
 * Note: If the function body is empty the whole function can be removed.
 */
GOAL_STATUS_T appl_setup(
    void
)
{
    tsTout = goal_timerTsGet() + APPL_TIMEOUT;
    return GOAL_OK;
}


/****************************************************************************/
/** Application Loop
 *
 * Perform cyclic application tasks.
 *
 * Note: If the function body is empty the whole function can be removed.
 */
void appl_loop(
    void
)
{
    GOAL_STATUS_T res;                          /* result */
    void *pBlock[APPL_BUFFERS] = { NULL };      /* memory block array */
    GOAL_MA_HEAP_STATS_T stats;                 /* memory statistics */
    GOAL_MA_HEAP_STATS_T statsCmp;              /* memory statistics for compare */
    uint32_t ratio;                             /* ratio used/unused memory */
    uint32_t cntBlock;                          /* block iterator */
    uint32_t *pBuffer;                          /* buffer address */

    if (goal_timerTsGet() < tsTout) {
        return;
    }
    tsTout = goal_timerTsGet() + APPL_TIMEOUT;

    /****************************************************************************/
    /* get heap stats for compare */
    /****************************************************************************/
    res = goal_heapStatsGet(&statsCmp);
    if (GOAL_RES_OK(res)) {
        ratio = 100 - ((statsCmp.size - statsCmp.used) * 100 / statsCmp.size);
        goal_logInfo("HEAP usage: %"FMT_u32"/ %"FMT_u32" (%"FMT_u32"%%)", statsCmp.used, statsCmp.size, ratio);
    } else {
        goal_logErr("failed to get heap stats");
    }


    /****************************************************************************/
    /* Test 1: */
    /****************************************************************************/
    goal_logInfo("test 1: allocation and free");

    /* allocate blocks */
    if (GOAL_RES_OK(res)) {
        res = goal_heapAlloc(&pBlock[0], 48);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_heapAlloc(&pBlock[1], 512);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_heapAlloc(&pBlock[2], 64);
    }

    /* get heap stats */
    if (GOAL_RES_OK(res)) {
        res = goal_heapStatsGet(&stats);
    }
    if (GOAL_RES_OK(res)) {
        ratio = 100 - ((stats.size - stats.used) * 100 / stats.size);
        goal_logInfo("HEAP usage: %"FMT_u32"/ %"FMT_u32" (%"FMT_u32"%%)", stats.used, stats.size, ratio);
    } else {
        goal_logErr("failed to get heap stats");
    }


    if (GOAL_RES_OK(res)) {
        res = goal_heapFree(pBlock[1]);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_heapFree(pBlock[0]);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_heapFree(pBlock[2]);
    }

    /* get heap stats */
    if (GOAL_RES_OK(res)) {
        res = goal_heapStatsGet(&stats);
    }
    if (GOAL_RES_OK(res)) {
        ratio = 100 - ((stats.size - stats.used) * 100 / stats.size);
        goal_logInfo("HEAP usage: %"FMT_u32"/ %"FMT_u32" (%"FMT_u32"%%)", stats.used, stats.size, ratio);
    } else {
        goal_logErr("failed to get heap stats");
    }

    /* log results */
    if (GOAL_RES_ERR(res) || (stats.used != statsCmp.used)) {
        goal_logErr("test 1 failed");
    } else {
        goal_logInfo("test 1 passed");
    }


    /****************************************************************************/
    /* Test 2: */
    /****************************************************************************/
    goal_logInfo("test 2: realloc");

    /* only alloc new memory */
    if (GOAL_RES_OK(res)) {
        res = goal_heapRealloc(&pBlock[0], NULL, 34);
    }

    /* free memory */
    if (GOAL_RES_OK(res)) {
        res = goal_heapRealloc(NULL, pBlock[0], 0);
    }

    /* get heap stats */
    if (GOAL_RES_OK(res)) {
        res = goal_heapStatsGet(&stats);
    }
    if (GOAL_RES_OK(res)) {
        ratio = 100 - ((stats.size - stats.used) * 100 / stats.size);
        goal_logInfo("HEAP usage: %"FMT_u32"/ %"FMT_u32" (%"FMT_u32"%%)", stats.used, stats.size, ratio);
    } else {
        goal_logErr("failed to get heap stats");
    }

    /* log results */
    if (GOAL_RES_ERR(res) || (stats.used != statsCmp.used)) {
        goal_logErr("test 2 failed");
    } else {
        goal_logInfo("test 2 passed");
    }


    /****************************************************************************/
    /* Test 3: */
    /****************************************************************************/
    goal_logInfo("test 3: realloc new block");

    /* allocate a heap block */
    res = goal_heapAlloc(&pBlock[0], APPL_TEST_PATTERN_SIZE);

    if (GOAL_RES_OK(res)) {
        /* prepare test pattern */
        GOAL_MEMCPY(pBlock[0], testPattern, APPL_TEST_PATTERN_SIZE);

        /* realloc the test pattern into new block */
        res = goal_heapRealloc(&pBlock[1], pBlock[0], APPL_TEST_PATTERN_SIZE);
    }

    /* compare test pattern */
    if (GOAL_RES_OK(res)) {
        res = (GOAL_CMP_EQUAL == GOAL_MEMCMP(pBlock[1], testPattern, APPL_TEST_PATTERN_SIZE)) ? (GOAL_OK) : (GOAL_ERROR);
    }

    /* free block */
    if (GOAL_RES_OK(res)) {
        res = goal_heapFree(pBlock[1]);
    }

    /* get heap stats */
    if (GOAL_RES_OK(res)) {
        res = goal_heapStatsGet(&stats);
    }
    if (GOAL_RES_OK(res)) {
        ratio = 100 - ((stats.size - stats.used) * 100 / stats.size);
        goal_logInfo("HEAP usage: %"FMT_u32"/ %"FMT_u32" (%"FMT_u32"%%)", stats.used, stats.size, ratio);
    } else {
        goal_logErr("failed to get heap stats");
    }

    /* log results */
    if (GOAL_RES_ERR(res) || (stats.used != statsCmp.used)) {
        goal_logErr("test 3 failed");
    } else {
        goal_logInfo("test 3 passed");
    }

    /****************************************************************************/
    /* Test 4: */
    /****************************************************************************/
    goal_logInfo("test 4: allocation, alignment and free");

    /* allocate blocks */
    for (cntBlock = 0; cntBlock < APPL_BUFFERS; cntBlock++) {
        if (GOAL_RES_OK(res)) {
            res = goal_heapAlloc(&pBlock[cntBlock], sizeof(uint32_t) + cntBlock);
        }
    }

    /* get heap stats */
    if (GOAL_RES_OK(res)) {
        res = goal_heapStatsGet(&stats);
    }
    if (GOAL_RES_OK(res)) {
        ratio = 100 - ((stats.size - stats.used) * 100 / stats.size);
        goal_logInfo("HEAP usage: %"FMT_u32"/ %"FMT_u32" (%"FMT_u32"%%)", stats.used, stats.size, ratio);
    } else {
        goal_logErr("failed to get heap stats");
    }

#if 1
    /* write blocks */
    if (GOAL_RES_OK(res)) {
        for (cntBlock = 0; cntBlock < APPL_BUFFERS; cntBlock++) {
            pBuffer = (uint32_t *) pBlock[cntBlock];
            if (((PtrCast) pBuffer % GOAL_TARGET_MEM_ALIGN_CPU) != 0) {
                goal_logErr("alignment error : %p", (void *) pBuffer);
                res = GOAL_ERROR;
            }
            else {
                *pBuffer = 0xbeaf;
            }
        }
    }
#else
    UNUSEDARG(pBuffer);
#endif

    /* free blocks */
    for (cntBlock = 0; cntBlock < APPL_BUFFERS; cntBlock++) {
        if (GOAL_RES_OK(res)) {
            res = goal_heapFree(pBlock[cntBlock]);
        }
    }

    /* get heap stats */
    if (GOAL_RES_OK(res)) {
        res = goal_heapStatsGet(&stats);
    }
    if (GOAL_RES_OK(res)) {
        ratio = 100 - ((stats.size - stats.used) * 100 / stats.size);
        goal_logInfo("HEAP usage: %"FMT_u32"/ %"FMT_u32" (%"FMT_u32"%%)", stats.used, stats.size, ratio);
    } else {
        goal_logErr("failed to get heap stats");
    }

    /* log results */
    if (GOAL_RES_ERR(res) || (stats.used != statsCmp.used)) {
        goal_logErr("test 4 failed");
    } else {
        goal_logInfo("test 4 passed");
    }
}
