/** @file
 *
 * @brief GOAL Statistics
 *
 * This module manages generic GOAL statistics and is mainly used by the
 * Ethernet subsystem.
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
#define GOAL_ID GOAL_ID_STAT
#include "goal_includes.h"

#if GOAL_CONFIG_STATS == 1


/****************************************************************************/
/* Statistics Name Mapping Table */
/****************************************************************************/
#if GOAL_CONFIG_STATS_NAMES == 1
#  undef GOAL_STAT_ENTRY_RV
#  define GOAL_STAT_ENTRY_RV(nameList, id, name, desc) { nameList, name, desc },

/**< statistics naming table */
static GOAL_STAT_NAME_T mStatNames[] = {
    GOAL_STAT_ENTRY_RV_LIST
};
#endif /* GOAL_CONFIG_STATS_NAMES == 1 */


/****************************************************************************/
/* Local variables */
/****************************************************************************/
GOAL_STAT_LIST_T *gpGoalStatList;               /**< statistics list */


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/
static GOAL_STATUS_T goal_statListNew(
    GOAL_STAT_LIST_T **ppList,                  /**< [out] statistics list */
    GOAL_ID_T id                                /**< statistics list id */
);


static GOAL_STATUS_T goal_statEntryNew(
    GOAL_STAT_ENTRY_T **ppEntry,                /**< [out] statistics entry */
    GOAL_ID_T idList,                           /**< statistics list id */
    uint32_t idEntry                            /**< statistics entry id */
);


/****************************************************************************/
/** Create Statistics List Entry
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_statListNew(
    GOAL_STAT_LIST_T **ppList,                  /**< [out] statistics list */
    GOAL_ID_T id                                /**< statistics list id */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_STAT_LIST_T **ppListIt;                /* list iterator */

    /* find statistics list */
    for (ppListIt = &gpGoalStatList; *ppListIt; ppListIt = &(*ppListIt)->pNext) {
        if (id == (*ppListIt)->id) {
            *ppList = *ppListIt;
            return GOAL_OK;
        }
    }

    /* create entry */
    res = goal_memCalloc(ppListIt, sizeof(GOAL_STAT_LIST_T));
    GOAL_RES_ERR_RET(res, "goal_memCalloc");

    /* assign id */
    (*ppListIt)->id = id;

    /* return statistics list */
    *ppList = *ppListIt;

    return GOAL_OK;
}


/****************************************************************************/
/** Create Statistics Entry
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_statEntryNew(
    GOAL_STAT_ENTRY_T **ppEntry,                /**< [out] statistics entry */
    GOAL_ID_T idList,                           /**< statistics list id */
    uint32_t idEntry                            /**< statistics entry id */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_STAT_ENTRY_T **ppEntryIt;              /* entry iterator */
    GOAL_STAT_LIST_T *pList = NULL;             /* statistics list */

    /* find or create statistics list entry */
    res = goal_statListNew(&pList, idList);
    GOAL_RES_ERR_RET(res, "goal_statListNew");

    /* find statistics entry */
    for (ppEntryIt = &pList->pListEntry; *ppEntryIt; ppEntryIt = &(*ppEntryIt)->pNext) {
        if (idEntry == (*ppEntryIt)->id) {
            *ppEntry = *ppEntryIt;
            return GOAL_OK;
        }
    }

    /* create entry */
    res = goal_memCalloc(ppEntryIt, sizeof(GOAL_STAT_ENTRY_T));
    GOAL_RES_ERR_RET(res, "goal_memCalloc");

    /* assign id */
    (*ppEntryIt)->id = idEntry;

    /* return statistics list */
    *ppEntry = *ppEntryIt;

    return GOAL_OK;
}


/****************************************************************************/
/** Create Statistics Entry
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_statNew(
    GOAL_STAT_T **ppStat,                       /**< [out] statistics ptr storage */
    GOAL_ID_T idList,                           /**< statistics list id */
    uint32_t idEntry,                           /**< statistics entry id */
    uint32_t idStat,                            /**< statistics id */
    GOAL_BOOL_T flgWeak,                        /**< weak flag */
    GOAL_STAT_FUNC_T funcGet,                   /**< get function */
    GOAL_STAT_FUNC_T funcReset,                 /**< reset function */
    const void *pPriv                           /**< private function data */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_STAT_ENTRY_T *pEntry;                  /* statistics entry */
    GOAL_STAT_T **ppStatIt;                     /* statistics index iterator */

    /* verify callback function */
    if (NULL == funcGet) {
        /* GG_LOG ID:235: */
        goal_lmLog(GOAL_ID_STAT, 235, 190, 0, 0, GOAL_LOG_SEV_ERROR, "no get function set");
        return GOAL_ERR_PARAM;
    }

    /* find or create statistics entry */
    res = goal_statEntryNew(&pEntry, idList, idEntry);
    GOAL_RES_ERR_RET(res, "goal_statEntryNew");

    /* find statistics entry */
    for (ppStatIt = &pEntry->pListStat; *ppStatIt; ppStatIt = &(*ppStatIt)->pNext) {
        if (idStat == (*ppStatIt)->id) {

            /* check if entry is weak so it can be overwritten */
            if (GOAL_FALSE == (*ppStatIt)->flgWeak) {
                return GOAL_ERR_EXISTS;
            }

            break;
        }
    }

    /* create entry if didn't exists */
    if (NULL == *ppStatIt) {

        /* add statistics entry */
        res = goal_memCalloc(ppStatIt, sizeof(GOAL_STAT_T));
        GOAL_RES_ERR_RET(res, "goal_memCalloc");

        /* assign id */
        (*ppStatIt)->id = idStat;
    }

    /* assign or update values */
    (*ppStatIt)->flgWeak = flgWeak;
    (*ppStatIt)->funcGet = funcGet;
    (*ppStatIt)->funcReset = funcReset;
    (*ppStatIt)->pPriv = pPriv;

    /* return statistics entry if requested */
    if (NULL != ppStat) {
        *ppStat = *ppStatIt;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Get Statistics Entry by Id
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_statGetById(
    GOAL_STAT_T **ppStat,                       /**< [out] statistics ptr storage */
    GOAL_ID_T idList,                           /**< statistics list id */
    uint32_t idEntry,                           /**< statistics entry id */
    uint32_t id                                 /**< statistics id */
)
{
    GOAL_STAT_LIST_T *pList;                    /* statistics list iterator */
    GOAL_STAT_ENTRY_T *pEntry;                  /* statistics list iterator */
    GOAL_STAT_T *pStat;                         /* statistics iterator */

    /* find list entry */
    GOAL_LL_FOREACH(gpGoalStatList, pList) {
        if (idList == pList->id) {
            GOAL_LL_FOREACH(pList->pListEntry, pEntry) {
                if (idEntry == pEntry->id) {
                    GOAL_LL_FOREACH(pEntry->pListStat, pStat) {
                        if (id == pStat->id) {
                            *ppStat = pStat;
                            return GOAL_OK;
                        }
                    }
                }
            }
        }
    }

    return GOAL_ERR_NOT_FOUND;
}


/****************************************************************************/
/** Get Statistics Value
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_statValGet(
    uint64_t *pVal,                             /**< [out] statistics value storage */
    GOAL_STAT_T *pStat                          /**< statistics entry */
)
{
    /* return function result */
    return pStat->funcGet(pVal, pStat);
}


/****************************************************************************/
/** Get Statistics Value by Id
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_statValGetById(
    uint64_t *pVal,                             /**< [out] statistics value storage */
    GOAL_ID_T idList,                           /**< statistics list id */
    uint32_t idEntry,                           /**< statistics entry id */
    uint32_t id                                 /**< statistics id */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_STAT_T *pStat;                         /* statistics entry */

    /* find entry */
    res = goal_statGetById(&pStat, idList, idEntry, id);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* return function result */
    return pStat->funcGet(pVal, pStat);
}


/****************************************************************************/
/** Reset Statistics Value
 *
 * Returning the last statistics value isn't supported on all drivers.
 * Supplying NULL as pVal is supported.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_statReset(
    uint64_t *pVal,                             /**< [out] statistics last value */
    GOAL_STAT_T *pStat                          /**< statistics entry */
)
{
    GOAL_STATUS_T res;                          /* result */
    uint64_t val;                               /* value before reset */

    /* check if reset is supported */
    if (NULL == pStat->funcReset) {
        return GOAL_OK;
    }

    /* return function result */
    res = pStat->funcReset(&val, pStat);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* return pre-reset value */
    if (NULL != pVal) {
        *pVal = val;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Reset Statistics By Statistics Id
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_statResetById(
    GOAL_ID_T idList,                           /**< statistics list id */
    uint32_t idEntry,                           /**< statistics entry id */
    uint32_t id                                 /**< statistics id */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_STAT_T *pStat;                         /* statistics entry */

    /* find entry */
    res = goal_statGetById(&pStat, idList, idEntry, id);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* reset entry */
    return goal_statReset(NULL, pStat);
}


/****************************************************************************/
/** Reset Statistics By List Id
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_statResetByListId(
    GOAL_ID_T idList                            /**< statistics list id */
)
{
    GOAL_STAT_LIST_T *pList;                    /* list iterator */
    GOAL_STAT_ENTRY_T *pEntry;                  /* statistics list iterator */
    GOAL_STAT_T *pStat;                         /* statistics iterator */

    /* find list entry */
    GOAL_LL_FOREACH(gpGoalStatList, pList) {
        if (idList == pList->id) {
            GOAL_LL_FOREACH(pList->pListEntry, pEntry) {
                GOAL_LL_FOREACH(pEntry->pListStat, pStat) {
                    goal_statReset(NULL, pStat);
                }
            }
        }
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Return if a Statistics Entry is unused
 *
 * @retval GOAL_OK_AVAILABLE available
 * @retval GOAL_ERR_EXISTS not available
 */
GOAL_STATUS_T goal_statIsUnused(
    GOAL_ID_T idList,                           /**< statistics list id */
    uint32_t idEntry,                           /**< statistics entry id */
    uint32_t id                                 /**< statistics id */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_STAT_T *pStat;                         /* statistics entry */

    /* find entry */
    res = goal_statGetById(&pStat, idList, idEntry, id);
    if (GOAL_ERR_NOT_FOUND == res) {
        return GOAL_OK_AVAILABLE;
    }

    return GOAL_ERR_EXISTS;
}


#if GOAL_CONFIG_STATS_NAMES == 1
/****************************************************************************/
/** Get Statistics Name by Id
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_statNameGetById(
    const char **pStrName,                      /**< [out] statistics name */
    GOAL_ID_T idList,                           /**< statistics list id */
    uint32_t id                                 /**< statistics id */
)
{
    unsigned int cnt;                           /* counter */

    /* find entry */
    for (cnt = 0; cnt < ARRAY_ELEMENTS(mStatNames); cnt++) {
        if ((mStatNames[cnt].idList == idList) && (mStatNames[cnt].id == id)) {
            *pStrName = mStatNames[cnt].strName;
            return GOAL_OK;
        }
    }

    return GOAL_ERR_NOT_FOUND;
}
#endif /* GOAL_CONFIG_STATS_NAMES == 1 */


#endif /* GOAL_CONFIG_STATS */
