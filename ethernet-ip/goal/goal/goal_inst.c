/** @file
 *
 * @brief GOAL Instance Module
 *
 * This module manages the creation of generic instances.
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

#define GOAL_ID GOAL_ID_INSTANCE
#include <goal_includes.h>


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_INSTANCE_LIST_T *mpListInstAll = NULL; /**< list of instances */


/****************************************************************************/
/** Create a new instance list
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_instListNew(
    GOAL_INSTANCE_LIST_T **ppList,              /**< [out] instance ptr ref */
    uint32_t idType                             /**< instance type */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_INSTANCE_LIST_T **ppIt;                /* instance list iterator */

    /* find instance list entry */
    for (ppIt = &mpListInstAll; *ppIt; ppIt = &(*ppIt)->pNext) {
        if (idType == (*ppIt)->idType) {
            *ppList = *ppIt;
            return GOAL_OK_EXISTS;
        }
    }

    /* if not found then allocate instance list entry */
    res = goal_memCalloc(ppIt, sizeof(GOAL_INSTANCE_LIST_T));
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:131: */
        goal_lmLog(GOAL_ID_INSTANCE, 131, 102, 0, 0, GOAL_LOG_SEV_ERROR, "failed to create instance list entry");
    } else {
        /* assign data */
        (*ppIt)->idType = idType;

        /* return list */
        *ppList = *ppIt;
    }
    return res;
}


/****************************************************************************/
/** Create a new instance
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_instNew(
    GOAL_INSTANCE_T **ppInst,                   /**< [out] instance ptr ref */
    const unsigned int sizeInst,                /**< instance size in bytes */
    uint32_t idType,                            /**< instance type */
    unsigned int id,                            /**< instance id */
    const char *strDesc                         /**< instance description */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_INSTANCE_LIST_T *pList = NULL;         /* instance list entry */
    GOAL_INSTANCE_T **ppIt;                     /* instance iterator */

    /* create or find list entry */
    res = goal_instListNew(&pList, idType);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* find instance entry */
    for (ppIt = &pList->pInst; *ppIt; ppIt = &(*ppIt)->pNext) {
        if (id == (*ppIt)->idInst) {
            /* GG_LOG ID:132: */
            goal_lmLog(GOAL_ID_INSTANCE, 132, 103, 4, 4, GOAL_LOG_SEV_ERROR, "instance $1 of type $2 already created");
            goal_lmLogParamUINT32(id);
            goal_lmLogParamUINT32(idType);
            goal_lmLogFinish();
            return GOAL_ERR_ALREADY_USED;
        }
    }

    /* allocate instance entry */
    res = goal_memCalloc(ppIt, sizeInst);
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:133: */
        goal_lmLog(GOAL_ID_INSTANCE, 133, 104, 0, 0, GOAL_LOG_SEV_ERROR, "failed to create instance entry");
        return res;
    }

    /* initialize instance entry */
    (*ppIt)->idInst = id;
    (*ppIt)->strInstDesc = strDesc;

    /* return instance */
    *ppInst = *ppIt;

    return GOAL_OK;
}


/****************************************************************************/
/** Free a instance
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_instFree(
    GOAL_INSTANCE_T **ppInst                    /**< [in/out] instance ptr ref */
)
{
    /* release instance */
    goal_memFree(ppInst);

    return GOAL_OK;
}


/****************************************************************************/
/** Get instance by id
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_instGetById(
    GOAL_INSTANCE_T **ppInst,                   /**< [out] instance ptr ref */
    uint32_t idType,                            /**< instance type */
    unsigned int id                             /**< instance id */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_INSTANCE_LIST_T *pList = NULL;         /* instance list entry */
    GOAL_INSTANCE_T *pInst;                     /* instance entry */

    /* find instance list entry */
    res = goal_instListGetByType(&pList, idType);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* find instance entry */
    GOAL_LL_FOREACH(pList->pInst, pInst) {
        if (id == pInst->idInst) {
            *ppInst = pInst;
            return GOAL_OK;
        }
    }

    return GOAL_ERR_NOT_FOUND;
}


/****************************************************************************/
/** Get instance list by type
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_instListGetByType(
    GOAL_INSTANCE_LIST_T **ppList,              /**< [out] instance list ref */
    uint32_t idType                             /**< instance type */
)
{
    GOAL_INSTANCE_LIST_T *pIt;                  /* instance list iterator */

    /* find instance list entry */
    GOAL_LL_FOREACH(mpListInstAll, pIt) {
        if (idType == pIt->idType) {
            *ppList = pIt;
            return GOAL_OK;
        }
    }

    return GOAL_ERR_NOT_FOUND;
}
