/** @file
 *
 * @brief GOAL Example for instance handling
 *
 * This module provides a simple application of goal instance handling
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
/* Configuration */
/****************************************************************************/
#define APPL_ID (0xffff)                        /**< instances type id */


/****************************************************************************/
/* Type definitions */
/****************************************************************************/
typedef struct INST_ENTRY_T {
    GOAL_INSTANCE_HEADER(INST_ENTRY_T);         /**< instance header */
} INST_ENTRY_T;


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_INSTANCE_LIST_T *mpListInst;        /**< instance list */
static char *strIdNames[] = {
    "Fichtelberg",
    "Pöhlberg",
    "Bärenstein",
    "Hirtstein",
    "Rabenberg",
    "Ochsenkopf",
    "Steinhübel",
    "Schwarzberg",
    "Greifensteine",
    "Fuchshöhe"
};


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
    GOAL_STATUS_T res;                          /* result */
    GOAL_INSTANCE_T *pEntry;                    /* instance entry */
    unsigned int id;                            /* instance ids */

    /* create an instance list */
    res = goal_instListNew(&mpListInst, APPL_ID);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create instance list");
    }

    /* create some instances */
    for (id = 0; GOAL_RES_OK(res) && id < 10; id++) {
        res = goal_instNew(&pEntry,
                sizeof(INST_ENTRY_T),
                APPL_ID,
                id,
                strIdNames[id]);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to create instance");
        }
    }

    /* list all instances */
    GOAL_INSTANCE_FOREACH(mpListInst->pInst, pEntry) {
        goal_logInfo("Instance %"FMT_u32": name %s", (uint32_t) pEntry->idInst, pEntry->strInstDesc);
    }

    /* get a specific ID */
    if (GOAL_RES_OK(res)) {
        res = goal_instGetById(&pEntry, APPL_ID, 5);
    }

    /* show result */
    if (GOAL_RES_OK(res)) {
        goal_logInfo("found instance with id %"FMT_u32": %s", (uint32_t) pEntry->idInst, pEntry->strInstDesc);
    }

    return res;
}
