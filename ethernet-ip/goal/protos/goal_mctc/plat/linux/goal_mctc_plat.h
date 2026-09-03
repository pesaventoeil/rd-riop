/** @file
 *
 * @brief GOAL Micro Core To Core - Linux Platform
 *
 * @copyright
 * Copyright 2010-2018.
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

#ifndef GOAL_MCTC_PLAT_LINUX_H
#define GOAL_MCTC_PLAT_LINUX_H

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


/****************************************************************************/
/* Platform support */
/****************************************************************************/
#define GOAL_OK 0
#define GOAL_ERR_ALLOC 1
#define GOAL_ERR_OVERFLOW 2
#define GOAL_ERR_UNDERFLOW 3
#define GOAL_ERR_NOT_FOUND 4
#define GOAL_ERR_DENY 5
#define GOAL_ERR_EXISTS 6
#define GOAL_ERR_NOT_EMPTY 7
#define GOAL_ERR_WRONG_STATE 8
#define GOAL_ERR_NULL_POINTER 9
#define GOAL_ERR_TIMEOUT 10

#define GOAL_STATUS_T uint32_t
#define GOAL_TIMESTAMP_T uint64_t
#define GOAL_TIMER_MSEC 1
#define GOAL_TIMER_SEC (GOAL_TIMER_MSEC * 1000)
#define GOAL_MEM_CALLOC(_ptr, _size) ((*_ptr = calloc(1, _size)), (!*_ptr) ? GOAL_ERR_ALLOC : GOAL_OK)
#define GOAL_UNUSED(_x) ((void)(_x))
#define GOAL_RES_ERR(_res) (0 != _res)
#define GOAL_RES_OK(_res) (0 == _res)
#define GOAL_LOG(...) printf("[INF] "); printf(__VA_ARGS__); fflush(stdout)
#define GOAL_LOG_ERR(...) printf("[ERR] "); printf(__VA_ARGS__); fflush(stdout)
#define GOAL_FLETCHER16(_data, _len) goal_mctcFletcher16(_data, _len)
#define GOAL_MEMCPY memcpy
#define GOAL_MEMCMP memcmp
#define GOAL_BOOL_T unsigned int
#define GOAL_TRUE 1
#define GOAL_FALSE 0
#define GOAL_LOCK_T void
#define GOAL_LOCK_CREATE(...)
#define GOAL_LOCK_GET(...)
#define GOAL_LOCK_GET_NO_RES(...)
#define GOAL_LOCK_PUT(...)
#define GOAL_le16toh(_val) le16toh(_val)
#define GOAL_le32toh(_val) le32toh(_val)
#define GOAL_htole16(_val) htole16(_val)
#define GOAL_htole32(_val) htole32(_val)
#define GOAL_le32toh_p(_ptr) le32toh(*_ptr)
#define GOAL_htole32_p(_ptr, _val) *_ptr = htole32(_val)
#define GOAL_LOG_EXCEPTION(_id, ...) printf("[EXC] "); printf(__VA_ARGS__); fflush(stdout); exit(1)
#define FMT_u32 PRIu32
#define GOAL_TARGET_PACKED_PRE
#define GOAL_TARGET_PACKED __attribute__((packed))
#define GOAL_TARGET_PACKED_STRUCT_PRE
#define GOAL_TARGET_PACKED_STRUCT_POST
#define GOAL_LOOP() pCtc->pInst->pTgtRx->funcRecv(pCtc->pInst, pCtc->pInst->pTgtRx)


/****************************************************************************/
/** Get Current Tick Count as 64 Bit value
 *
 * Returns the current tick counter in ms as GOAL_TIMESTAMP_T.
 * The 64 bit variant will overflow in 584942417 years.
 *
 * @return GOAL_TIMESTAMP_T 64 bit timestamp structure
 */
static inline GOAL_TIMESTAMP_T GOAL_TIMER_TS_GET(
    void
)
{
    struct timespec ts;                         /* time value */

    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (((uint64_t) ts.tv_sec * (uint64_t) 1000000000 + (uint64_t) ts.tv_nsec) / 1000000);
}


#endif /* GOAL_MCTC_PLAT_LINUX_H */
