/** @file
 *
 * @brief Ethernet Statistics Example
 *
 * This application polls all Ethernet statistics and shows changed statistics
 * every second.
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
/* Local defines */
/****************************************************************************/
#define GOAL_APPL_ETH_STATS_TIMER_PERIOD        GOAL_TIMER_SEC


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static unsigned int mCntVal;                    /**< value counter */
static uint64_t *mpValShadow;                   /**< shadow value storage */


/****************************************************************************/
/* Local Prototypes */
/****************************************************************************/
static void appl_timerEthStatsCb(
    void *pArg                                  /**< timer argument */
);


/****************************************************************************/
/** Application Setup
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T appl_setup(
    void
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_TIMER_T *pTimerEthStats = NULL;        /* eth stats timer */
    GOAL_STAT_LIST_T *pListStat;                /* statistics list */
    GOAL_STAT_ENTRY_T *pEntry;                  /* statistics entry */
    GOAL_STAT_T *pStat;                         /* statistics data */

    /* create eth stats timer */
    res = goal_timerCreate(&pTimerEthStats, GOAL_TIMER_LOW);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("error during eth stats timer creation");
        return res;
    }

    /* setup and start eth stats timer */
    res = goal_timerSetup(pTimerEthStats, GOAL_TIMER_PERIODIC, GOAL_APPL_ETH_STATS_TIMER_PERIOD, appl_timerEthStatsCb, NULL, GOAL_TRUE);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("error while starting eth stats timer");
        return res;
    }

    /* assume the statistics counters are fixed at this point and create a shadow */
    GOAL_LL_FOREACH(gpGoalStatList, pListStat) {

        /* skip non-Ethernet statistics */
        if (GOAL_ID_ETH != pListStat->id) {
            continue;
        }

        /* iterate through Ethernet statistics entries */
        GOAL_LL_FOREACH(pListStat->pListEntry, pEntry) {

            /* count Ethernet statistics */
            GOAL_LL_FOREACH(pEntry->pListStat, pStat) {

                /* count entry */
                mCntVal++;

                /* reset statistics */
                goal_statReset(NULL, pStat);
            }
        }
    }

    goal_logInfo("found %u statistics counters", mCntVal);

    /* allocate shadow counters */
    return goal_memCalloc(&mpValShadow, sizeof(uint64_t) * mCntVal);
}


/****************************************************************************/
/** Application Loop
 */
static void appl_timerEthStatsCb(
    void *pArg                                  /**< timer argument */
)
{
    GOAL_STATUS_T res;                          /* result */
    uint64_t val;                               /* value */
    GOAL_STAT_LIST_T *pListStat;                /* statistics list */
    GOAL_STAT_ENTRY_T *pEntry;                  /* statistics entry */
    GOAL_STAT_T *pStat;                         /* statistics counter */
#if GOAL_CONFIG_STATS_NAMES == 1
    const char *strName;                        /* statistics name */
#endif
    int cntVal = -1;                            /* value position */
    GOAL_BOOL_T flgUpdate = GOAL_FALSE;         /* update flag */

    UNUSEDARG(pArg);

    /* iterate through statistics */
    GOAL_LL_FOREACH(gpGoalStatList, pListStat) {

        /* skip non-Ethernet statistics */
        if (GOAL_ID_ETH != pListStat->id) {
            continue;
        }

        /* iterate through Ethernet statistics entries */
        GOAL_LL_FOREACH(pListStat->pListEntry, pEntry) {

            /* iterate through statistics counters */
            GOAL_LL_FOREACH(pEntry->pListStat, pStat) {

                /* increase value counter */
                cntVal++;

                /* retrieve current value */
                res = goal_statValGet(&val, pStat);
                if (GOAL_RES_ERR(res)) {
                    goal_logErr("failed to retrieve statistics counter");
                    return;
                }

                /* compare value to shadow */
                if (mpValShadow[cntVal] == val) {
                    continue;
                }

                /* set update flag */
                flgUpdate = GOAL_TRUE;

                /* update shadow */
                mpValShadow[cntVal] = val;

                /* clear name string */
                strName = NULL;
#if GOAL_CONFIG_STATS_NAMES == 1
                goal_statNameGetById(&strName, pListStat->id, pEntry->id);
#endif
                if (NULL == strName) {
                    if (GOAL_ETH_PORT_HOST == pStat->id) {
                        goal_logInfo("port host, stat %"FMT_u32": %"FMT_u64, pEntry->id, val);
                    } else {
                        goal_logInfo("port %"FMT_u32", stat %"FMT_u32": %"FMT_u64, pStat->id, pEntry->id, val);
                    }
                } else {
                    if (GOAL_ETH_PORT_HOST == pStat->id) {
                        goal_logInfo("port host, %s: %"FMT_u64, strName, val);
                    } else {
                        goal_logInfo("port %"FMT_u32", %s: %"FMT_u64, pStat->id, strName, val);
                    }
                }
            }
        }
    }

    /* divider line */
    if (GOAL_TRUE == flgUpdate) {
        goal_logInfo("-------------------------------------------------------");
    }
}
