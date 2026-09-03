/** @file
 *
 * @brief GOAL Micro Core To Core - GOAL Platform Support
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
#ifndef GOAL_MCTC_PLAT_H
#define GOAL_MCTC_PLAT_H

#include <goal_includes.h>


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
void goal_miMctcRpcProcessLoop(
    void *pParam                                /**< loop parameter */
);


/****************************************************************************/
/* Defines */
/****************************************************************************/
#define GOAL_MEM_CALLOC goal_memCalloc
#define GOAL_LOG(...)
#define GOAL_LOG_ERR(...)
#define GOAL_FLETCHER16(x, y) goal_utilCrcFletcher16(x, (int) y)
#define GOAL_LOCK_CREATE(_pLock) { GOAL_STATUS_T _res = goal_lockCreate(GOAL_LOCK_BINARY, &_pLock, 0, 1, GOAL_ID_CTC); if (GOAL_RES_ERR(_res)) { return _res; } }
#define GOAL_LOCK_GET(_pLock) { GOAL_STATUS_T _res = goal_lockGet(_pLock, GOAL_LOCK_INFINITE); if (GOAL_RES_ERR(_res)) { return _res; } }
#define GOAL_LOCK_GET_NO_RES(_pLock) { GOAL_STATUS_T _res = goal_lockGet(_pLock, GOAL_LOCK_INFINITE); if (GOAL_RES_ERR(_res)) { return; } }
#define GOAL_LOCK_PUT(_pLock) goal_lockPut(_pLock)
#define GOAL_LOG_EXCEPTION(_id, ...) goal_lmLog(0, _id, 0, 0, 0, GOAL_LOG_SEV_EXCEPTION, 0); goal_lmLogFinish()
#define GOAL_TIMER_TS_GET goal_timerTsGet
#define GOAL_LOOP() goal_loop(); goal_miMctcRpcProcessLoop(pCtc->pInst->pPriv)


#endif /* GOAL_MCTC_PLAT_H */
