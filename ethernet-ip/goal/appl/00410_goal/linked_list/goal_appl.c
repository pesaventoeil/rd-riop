/** @file
 *
 * @brief GOAL Example for linked list
 *
 * This module provides a simple application of linked lists
 *
 * Note: Make sure the initial list pointer is set to NULL before using it with
 *       the GOAL_LL_* API.
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

#include "goal_includes.h"


/****************************************************************************/
/* Type definitions */
/****************************************************************************/
typedef struct LIST_ENTRY_T {
    struct LIST_ENTRY_T *pNext;                 /**< next list element */
    uint32_t id;                                /**< list entry id */
    const char *strName;                        /**< list entry name */
} LIST_ENTRY_T;


/****************************************************************************/
/* Local functions */
/****************************************************************************/
static GOAL_STATUS_T appl_IdEntryNew(
    LIST_ENTRY_T **ppEntry,                     /**< return pointer of entry */
    uint32_t id,                                /**< entry id */
    const char *strName                         /**< entry name */
);


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static LIST_ENTRY_T *pList = NULL;              /**< linked list */


/****************************************************************************/
/** create new entry and append it to a list
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T appl_IdEntryNew(
    LIST_ENTRY_T **ppEntry,                     /**< return pointer of entry */
    uint32_t id,                                /**< entry id */
    const char *strName                         /**< entry name */
)
{
    GOAL_STATUS_T res;                          /* result */

    /* allocate */
    res = goal_memCalloc(ppEntry, sizeof(LIST_ENTRY_T));

    if (GOAL_RES_OK(res)) {

        /* populate */
        (*ppEntry)->id = id;
        (*ppEntry)->strName = strName;

        /* append */
        GOAL_LL_APPEND(pList, *(ppEntry));

    }

    return res;
}


/****************************************************************************/
/** Application Setup
 *
 * Configure GOAL and components.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T appl_setup(
    void
)
{
    LIST_ENTRY_T *pEntry;                       /* list entry */
    GOAL_STATUS_T res;                          /* result */

    /* iterate over empty list */
    goal_logInfo("iterating over empty list");
    GOAL_LL_FOREACH(pList, pEntry) {
        goal_logInfo("> name for id %"FMT_u32" is %s", pEntry->id, pEntry->strName);
    }

    /* create entry PNIO */
    res = appl_IdEntryNew(&pEntry, GOAL_ID_PNIO, "PNIO");

    /* create entry EIP */
    if (GOAL_RES_OK(res)) {
        res = appl_IdEntryNew(&pEntry, GOAL_ID_EIP, "EIP");
    }

    /* create entry CAN */
    if (GOAL_RES_OK(res)) {
        res = appl_IdEntryNew(&pEntry, GOAL_ID_CAN, "CAN");
    }

    /* iterate over filled list */
    goal_logInfo("iterating over filled list");
    GOAL_LL_FOREACH(pList, pEntry) {
        goal_logInfo("> name for id %"FMT_u32" is %s", pEntry->id, pEntry->strName);
    }

    return res;
}
