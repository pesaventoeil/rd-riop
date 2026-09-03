/** @file
 *
 * @brief Delay deny of memory allocation after stage GOAL_STAGE_GOAL
 *
 * This application shows how delay the memory allocation block after
 * GOAL_STAGE_GOAL by getting a delay handle which must be later released.
 *
 * @copyright
 * Copyright 2010-2017.
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


/****************************************************************************/
/* Defines */
/****************************************************************************/
#define GOAL_APPL_MEM_SIZE 100                  /**< allocation size */


/****************************************************************************/
/* Variables */
/****************************************************************************/
static GOAL_MEM_DENY_HANDLE_T mHdlMemDeny;      /* memory deny delay handle */


/****************************************************************************/
/** Application Init
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T appl_init(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* request memory deny delay handle */
    res = goal_memDenyDelayHandleGet(&mHdlMemDeny);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to get deny delay handle");
    }

    return res;
}


/****************************************************************************/
/** Application Loop
 */
void appl_loop(
    void
)
{
    static GOAL_BOOL_T flgFirstRun = GOAL_TRUE; /* first run */
    GOAL_STATUS_T res;                          /* result */
    void *pBuf = NULL;                          /* buffer pointer */

    /* only show allocation on first run */
    if (GOAL_TRUE != flgFirstRun) {
        return;
    }
    flgFirstRun = GOAL_FALSE;

    /* allocate memory after stage GOAL_STAGE_GOAL */
    goal_logInfo("allocating memory after GOAL_STAGE_GOAL with deny delay handle");
    res = goal_memCalloc(&pBuf, GOAL_APPL_MEM_SIZE);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to allocate %u bytes", GOAL_APPL_MEM_SIZE);
        return;

    }
    goal_logInfo("successfully allocated %u bytes", GOAL_APPL_MEM_SIZE);

    /* release memory deny delay handle */
    res = goal_memDenyDelayHandleRelease(mHdlMemDeny);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to release memory deny delay handle");
        return;
    }
    goal_logInfo("successfully released memory deny delay handle");

    /* allocate memory after release of deny delay handle */
    goal_logInfo("allocating memory after GOAL_STAGE_GOAL without deny delay handle");
    pBuf = NULL;
    res = goal_memCalloc(&pBuf, GOAL_APPL_MEM_SIZE);
    if (GOAL_RES_ERR(res)) {
        goal_logInfo("test successful: failed to allocate %u bytes", GOAL_APPL_MEM_SIZE);
    } else {
        goal_logErr("test failed: successfully allocated %u bytes", GOAL_APPL_MEM_SIZE);
    }
}
