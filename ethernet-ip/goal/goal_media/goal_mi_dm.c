/** @file
 *
 * @brief GOAL MI DM - Data Mapper (Synchronized Partitioned Exchange of Data)
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

#define GOAL_ID GOAL_ID_MI_DM
#include <goal_includes.h>
#include <goal_media/goal_mi_dm.h>


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/
static void goal_miDmPartBufXchg(
    GOAL_MI_DM_PART_T *pPart,                   /**< partition */
    GOAL_BOOL_T flgRead                         /**< true = read, false = write */
);

static GOAL_STATUS_T goal_miDmPartRegInt(
    uint32_t idMiDm,                            /**< MI DM handle id */
    GOAL_ID_T idGroup,                          /**< group id */
    GOAL_MI_DM_PART_T *pPart,                   /**< partition data */
    uint32_t lenPart,                           /**< partition length */
    uint32_t pos,                               /**< initial/fixed position */
    GOAL_BOOL_T flgPosFixed                     /**< position, true = fixed */
);


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_INSTANCE_LIST_T *mpListMiDm;        /**< MI DM instance list */


/****************************************************************************/
/** GOAL MI DM - New Instance
 *
 * Creates a new GOAL MI DM instance. If ppMiDm isn't NULL the handle is
 * returned.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miDmNew(
    GOAL_MI_DM_T **ppMiDm,                      /**< [out] MI DM handle */
    uint32_t id,                                /**< handle id */
    unsigned int len,                           /**< buffer length */
    const char *strDesc                         /**< description */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MI_DM_T *pMiDm;                        /* MI DM */
    unsigned int cnt;                           /* counter */

    /* create or retrieve instance list */
    res = goal_instListNew(&mpListMiDm, GOAL_ID_MI_DM);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* create and get instance handle */
    res = goal_instNew((GOAL_INSTANCE_T **) &pMiDm, sizeof(GOAL_MI_DM_T), GOAL_ID_MI_DM, id, strDesc);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* allocate double buffer for stable read and volatile write partition buffers */
    for (cnt = 0; cnt < GOAL_MI_DM_CNT_BUF_PART; cnt++) {
        res = goal_memCalloc(&pMiDm->pBufPart[cnt], len);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
    }

    /* assign instance information */
    pMiDm->len = len;

    /* return entry if requested */
    if (NULL != ppMiDm) {
        *ppMiDm = pMiDm;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** GOAL MI DM - Create a Group
 *
 * Creates a new GOAL MI DM group. If ppGroup is NULL no handle is returned.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miDmGroupNew(
    GOAL_MI_DM_GROUP_T **ppGroup,               /**< [out] group handle */
    uint32_t idMiDm,                            /**< MI DM id */
    GOAL_ID_T idGroup                           /**< group id */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MI_DM_T *pMiDm;                        /* MI DM instance */
    GOAL_MI_DM_GROUP_T *pGroup;                 /* group */

    /* get instance */
    res = goal_instGetById((GOAL_INSTANCE_T **) &pMiDm, GOAL_ID_MI_DM, idMiDm);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("instance not found");
        return res;
    }

    /* check if group already exists */
    GOAL_LL_FOREACH(pMiDm->pGroups, pGroup) {
        if (idGroup == pGroup->id) {

            /* return group handle */
            if (NULL != ppGroup) {
                *ppGroup = pGroup;
            }

            return GOAL_OK;
        }
    }

    /* allocate group */
    res = goal_memCalloc(&pGroup, sizeof(GOAL_MI_DM_GROUP_T));
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to allocate group");
        return res;
    }

    /* allocate group lock */
    res = goal_lockCreate(GOAL_LOCK_BINARY, &pGroup->pLock, 0, 1, GOAL_ID_MI_DM);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create group lock");
        return res;
    }

    /* initialize group */
    pGroup->pMiDm = pMiDm;
    pGroup->id = idGroup;

    /* attach group to MI DM */
    GOAL_LL_APPEND(pMiDm->pGroups, pGroup);

    /* return group handle */
    if (NULL != ppGroup) {
        *ppGroup = pGroup;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** GOAL MI DM - Register Data Partition
 *
 * Note: Groups can only be created in the setup phase. If the memory
 * allocation is already closed the call to goal_miDmGroupNew will fail if
 * the group doesn't exist.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miDmPartReg(
    uint32_t idMiDm,                            /**< DM handle id */
    GOAL_ID_T idGroup,                          /**< group id */
    GOAL_MI_DM_PART_T *pPart,                   /**< partition data */
    uint32_t lenPart                            /**< partition length */
)
{
    return goal_miDmPartRegInt(idMiDm, idGroup, pPart, lenPart, 0, GOAL_FALSE);
}


/****************************************************************************/
/** GOAL MI DM - Register Data Partition with fixed Index
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miDmPartIdxReg(
    uint32_t idMiDm,                            /**< DM handle id */
    GOAL_ID_T idGroup,                          /**< group id */
    GOAL_MI_DM_PART_T *pPart,                   /**< partition data */
    uint32_t lenPart,                           /**< partition length */
    uint32_t idx                                /**< partition index */
)
{
    return goal_miDmPartRegInt(idMiDm, idGroup, pPart, lenPart, idx, GOAL_TRUE);
}


/****************************************************************************/
/** GOAL MI DM - Register Data Partition Internal Helper
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_miDmPartRegInt(
    uint32_t idMiDm,                            /**< DM handle id */
    GOAL_ID_T idGroup,                          /**< group id */
    GOAL_MI_DM_PART_T *pPart,                   /**< partition data */
    uint32_t lenPart,                           /**< partition length */
    uint32_t pos,                               /**< initial/fixed position */
    GOAL_BOOL_T flgPosFixed                     /**< position, true = fixed */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MI_DM_T *pMiDm = NULL;                 /* DM handle */
    GOAL_MI_DM_GROUP_T *pGroup = NULL;          /* group handle */
    GOAL_MI_DM_PART_T **ppPartIt;               /* partition iterator */
    GOAL_MI_DM_PART_T *pPartTmp;                /* temporary partition */

    /* check if partition is already configured (and active) */
    if (GOAL_TRUE == pPart->flgActive) {
        if ((idMiDm == pPart->pGroup->pMiDm->idInst) &&
            (idGroup == pPart->pGroup->id) &&
            (((GOAL_TRUE == flgPosFixed) && (pos != pPart->idxPart)) || (GOAL_FALSE == flgPosFixed)) &&
            (lenPart == pPart->len)) {
            /* if all elements matches, the partition is already configured */
            return GOAL_OK;
        } else {
            /* partition parameter missmatch */
            return GOAL_ERR_PARAM;
        }
    }

    /* get instance */
    res = goal_instGetById((GOAL_INSTANCE_T **) &pMiDm, GOAL_ID_MI_DM, idMiDm);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("instance not found");
        return res;
    }

    /* find free space for partition */
    for (ppPartIt = &pMiDm->pParts; *ppPartIt; ppPartIt = &(*ppPartIt)->pNextAll) {

        /* skip if current position is behind partition */
        if (pos > (*ppPartIt)->idxPart) {
            continue;
        }

        /* stop search if new partition fits here */
        if ((pos + lenPart) <= (*ppPartIt)->idxPart) {
            break;
        }

        /* fixed position requested but not possible, stop here */
        if (GOAL_TRUE == flgPosFixed) {
            goal_logErr("fixed pos: %"FMT_u32" already in use", pos);
            return GOAL_ERR_NOT_EMPTY;
        }

        /* calculate next position */
        pos = (*ppPartIt)->idxPart + (*ppPartIt)->len;
    }

    /* check if new partition fits into buffer */
    if ((pos + lenPart) > pMiDm->len) {
        goal_logErr("pos: %"FMT_u32", part len: %"FMT_u32", buf size: %u", pos, lenPart, pMiDm->len);
        return GOAL_ERR_OVERFLOW;
    }

    /* create or get group */
    res = goal_miDmGroupNew(&pGroup, idMiDm, idGroup);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* lock group metainfo
     *
     * since modification of the partition lists needs to be locked and iternation of the
     * partition lists can be executed from different threads, this is required
     *
    */
    res = goal_lockGet(pGroup->pLock, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* insert partition */
    pPartTmp = *ppPartIt;
    *ppPartIt = pPart;
    pPart->pNextAll = pPartTmp;

    /* attach partition to group */
    GOAL_LL_APPEND(pGroup->pParts, pPart);

    /* initialize and configure partition */
    pPart->pGroup = pGroup;
    pPart->idxPart = pos;
    pPart->len = lenPart;
    pPart->pBufRead = pMiDm->pBufPart[GOAL_MI_DM_ID_BUF_READ];
    pPart->pBufWrite = pMiDm->pBufPart[GOAL_MI_DM_ID_BUF_WRITE];
    pPart->pBufXchg = pMiDm->pBufPart[GOAL_MI_DM_ID_BUF_XCHG];
    pPart->flgActive = GOAL_TRUE;

    /* unlock group metadata */
    goal_lockPut(pGroup->pLock);

    goal_logInfo("part added to '%s', pos: %"FMT_u32", len: %"FMT_u32,
                 pMiDm->strInstDesc, pPart->idxPart, pPart->len);

    return GOAL_OK;
}


/****************************************************************************/
/** GOAL MI DM - Get Partition Index
 *
 * @returns index of partition
 */
uint32_t goal_miDmPartIdxGet(
    GOAL_MI_DM_PART_T *pPart                    /**< partition data */
)
{
    if ((NULL == pPart) || (GOAL_TRUE != pPart->flgActive)) {
        return 0;
    }

    return pPart->idxPart;
}


/****************************************************************************/
/** GOAL MI DM - Get Partition Size
 *
 * @returns size of partition
 */
uint32_t goal_miDmPartSizeGet(
    GOAL_MI_DM_PART_T *pPart                    /**< partition data */
)
{
    if ((NULL == pPart) || (GOAL_TRUE != pPart->flgActive)) {
        return 0;
    }

    return pPart->len;
}


/****************************************************************************/
/** GOAL MI DM - Get Partition Status
 *
 * This function returns the status of a registered partition. If the partition
 * handle is valid and active, GOAL\_TRUE is returned.
 *
 * @returns partition active flag
 */
GOAL_BOOL_T goal_miDmPartStatusGet(
    GOAL_MI_DM_PART_T *pPart                    /**< partition data */
)
{
    if (NULL == pPart) {
        return GOAL_FALSE;
    }
    return pPart->flgActive;
}


/****************************************************************************/
/** GOAL MI DM - Single Write
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miDmSingleWrite(
    GOAL_MI_DM_PART_T *pPart,                   /**< partition data */
    uint8_t *pBuf,                              /**< data source */
    unsigned int len                            /**< write length */
)
{
    GOAL_STATUS_T res;                          /* result */

    if (NULL == pPart) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (NULL == pBuf) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (GOAL_TRUE != pPart->flgActive) {
        return GOAL_ERR_WRONG_STATE;
    }

    /* write a single value */
    res = goal_miDmGroupWriteStart(pPart->pGroup);
    if (GOAL_RES_OK(res)) {
        res = goal_miDmGroupPartWrite(pPart, pBuf, len);
        if (GOAL_RES_OK(res)) {
            res = goal_miDmGroupWriteEnd(pPart->pGroup);
        }
    }

    return res;
}


/****************************************************************************/
/** GOAL MI DM - Partition Write
 *
 * Warning: Writing to a single partitions write buffer isn't thread safe.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miDmGroupPartWrite(
    GOAL_MI_DM_PART_T *pPart,                   /**< partition data */
    uint8_t *pBuf,                              /**< data source */
    unsigned int len                            /**< write length */
)
{

    /* check for NULL pointer */
    if ((NULL == pBuf)
        || (NULL == pPart)
        || (NULL == pPart->pGroup)) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (GOAL_TRUE != pPart->flgActive) {
        return GOAL_ERR_WRONG_STATE;
    }

    if (len > pPart->len) {
        return GOAL_ERR_OVERFLOW;
    }

    /* write partition value */
    GOAL_MEMCPY(&pPart->pBufWrite[pPart->idxPart], pBuf, len);

    /* mark partition as updated */
    pPart->flgWrite = GOAL_TRUE;

    /* mark group as updated */
    pPart->pGroup->flgSync = GOAL_TRUE;

    return GOAL_OK;
}


/****************************************************************************/
/** GOAL MI DM - Retrieve Direct Write Buffer
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miDmGroupWriteBufGet(
    uint8_t **ppBuf,                            /**< [out] partition pointer */
    uint32_t *pLen,                             /**< partition length */
    GOAL_MI_DM_PART_T *pPart                    /**< partition data */
)
{
    if (NULL == pPart) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (GOAL_TRUE != pPart->flgActive) {
        return GOAL_ERR_WRONG_STATE;
    }

    /* mark partition as updated */
    pPart->flgWrite = GOAL_TRUE;

    /* mark group as updated */
    pPart->pGroup->flgSync = GOAL_TRUE;

    /* return pointer and length */
    *ppBuf = &pPart->pBufWrite[pPart->idxPart];
    *pLen = pPart->len;

    return GOAL_OK;
}


/****************************************************************************/
/** GOAL MI DM - Group Write Start
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miDmGroupWriteStart(
    GOAL_MI_DM_GROUP_T *pGroup                  /**< partition group */
)
{
    GOAL_STATUS_T res;                          /* result */

    if (NULL == pGroup) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* lock group metainfo */
    res = goal_lockGet(pGroup->pLock, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* update active write counter */
    pGroup->cntActiveWrite++;

    /* release group lock */
    goal_lockPut(pGroup->pLock);

    return GOAL_OK;
}


/****************************************************************************/
/** GOAL MI DM - Group Write End
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miDmGroupWriteEnd(
    GOAL_MI_DM_GROUP_T *pGroup                  /**< partition group */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MI_DM_PART_T *pPart;                   /* partition */
    GOAL_MI_DM_CB_T *pCb;                       /* callback */
    GOAL_BOOL_T flgCb = GOAL_FALSE;             /* callback flag */

    if (NULL == pGroup) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* lock group metainfo */
    res = goal_lockGet(pGroup->pLock, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* update active write counter */
    pGroup->cntActiveWrite--;

    /* exchange partition buffers if no other write is active */
    if (0 == pGroup->cntActiveWrite) {
        GOAL_LL_FOREACH(pGroup->pParts, pPart) {

            /* skip unchanged partitions */
            if (GOAL_TRUE != pPart->flgWrite) {
                continue;
            }

            /* sync request for read buffer */
            pPart->flgRead = GOAL_TRUE;

            /* exchange write buffer */
            goal_miDmPartBufXchg(pPart, GOAL_MI_DM_XCHG_WRITE);
        }

        /* enable callbacks */
        flgCb = GOAL_TRUE;
    }

    /* release group lock */
    goal_lockPut(pGroup->pLock);

    /* inform group listeners about the update */
    if (GOAL_TRUE == flgCb) {
        GOAL_LL_FOREACH(pGroup->pCbsWrite, pCb) {
            res = pCb->funcCb(pGroup, pCb->pPriv);
            if (GOAL_RES_ERR(res)) {
                return res;
            }
        }

        /* inform global listeners about the update */
        GOAL_LL_FOREACH(pGroup->pMiDm->pCbsWrite, pCb) {
            res = pCb->funcCb(NULL, pCb->pPriv);
            if (GOAL_RES_ERR(res)) {
                return res;
            }
        }
    }

    return GOAL_OK;
}


/****************************************************************************/
/** GOAL MI DM - Write Sync
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miDmWriteSync(
    GOAL_MI_DM_T *pMiDm,                        /**< MI DM */
    uint8_t *pBuf                               /**< target buffer */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MI_DM_GROUP_T *pGroup;                 /* group iterator */
    GOAL_MI_DM_PART_T *pPart;                   /* part iterator */

    GOAL_LL_FOREACH(pMiDm->pGroups, pGroup) {

        /* lock group metainfo */
        res = goal_lockGet(pGroup->pLock, GOAL_LOCK_INFINITE);
        if (GOAL_RES_ERR(res)) {
            return res;
        }

        /* exchange read buffer */
        GOAL_LL_FOREACH(pGroup->pParts, pPart) {

            /* skip unchanged buffers */
            if (GOAL_FALSE == pPart->flgRead) {
                continue;
            }

            /* exchange buffers */
            goal_miDmPartBufXchg(pPart, GOAL_MI_DM_XCHG_READ);
        }

        /* release group lock */
        goal_lockPut(pGroup->pLock);

        /* copy partition data */
        GOAL_LL_FOREACH(pGroup->pParts, pPart) {
            GOAL_MEMCPY(&pBuf[pPart->idxPart], &pPart->pBufRead[pPart->idxPart], pPart->len);
        }
    }

    return GOAL_OK;
}


/****************************************************************************/
/** GOAL MI DM - Exchange Partition Buffers
 *
 * Exchange the read (true) or write (buffer) with the exchange buffer.
 */
static void goal_miDmPartBufXchg(
    GOAL_MI_DM_PART_T *pPart,                   /**< partition */
    GOAL_BOOL_T flgRead                         /**< true = read, false = write */
)
{
    uint8_t *pBufXchg;                          /* exchange buffer */

    /* exchange partition buffers */
    pBufXchg = pPart->pBufXchg;
    if (GOAL_TRUE == flgRead) {
        pPart->pBufXchg = pPart->pBufRead;
        pPart->pBufRead = pBufXchg;
        pPart->flgRead = GOAL_FALSE;
    } else {
        pPart->pBufXchg = pPart->pBufWrite;
        pPart->pBufWrite = pBufXchg;
        pPart->flgWrite = GOAL_FALSE;
    }
}


/****************************************************************************/
/** GOAL MI DM - Single Read
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miDmSingleRead(
    GOAL_MI_DM_PART_T *pPart,                   /**< partition data */
    uint8_t *pBuf                               /**< data source */
)
{
    GOAL_STATUS_T res;                          /* result */

    if ((NULL == pPart) || (NULL == pPart->pGroup)) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (GOAL_TRUE != pPart->flgActive) {
        return GOAL_ERR_WRONG_STATE;
    }

    /* read a single value */
    res = goal_miDmGroupReadStart(pPart->pGroup);
    if (GOAL_RES_OK(res)) {
        res = goal_miDmGroupPartRead(pPart, pBuf);
        if (GOAL_RES_OK(res)) {
            res = goal_miDmGroupReadEnd(pPart->pGroup);
        }
    }

    return res;
}


/****************************************************************************/
/** GOAL MI DM - Retrieve Direct Read Buffer
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miDmSingleReadBufGet(
    uint8_t **ppBuf,                            /**< [out] partition pointer */
    uint32_t *pLen,                             /**< partition length */
    GOAL_MI_DM_PART_T *pPart                    /**< partition data */
)
{
    GOAL_STATUS_T res;                          /* result */

    if ((NULL == pPart) || (NULL == ppBuf) || (NULL == pLen)) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (GOAL_TRUE != pPart->flgActive) {
        return GOAL_ERR_WRONG_STATE;
    }

    /* read a single value */
    res = goal_miDmGroupReadStart(pPart->pGroup);
    if (GOAL_RES_OK(res)) {

        /* return pointer and length */
        *ppBuf = &pPart->pBufRead[pPart->idxPart];
        *pLen = pPart->len;

        res = goal_miDmGroupReadEnd(pPart->pGroup);
    }

    return res;
}


/****************************************************************************/
/** GOAL MI DM - Partition Read
 *
 * Must only be called within a goal_miDmGroupReadStart and
 * goal_miDmGroupReadEnd block.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miDmGroupPartRead(
    GOAL_MI_DM_PART_T *pPart,                   /**< partition data */
    uint8_t *pBuf                               /**< data source */
)
{
    if (NULL == pPart) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (GOAL_TRUE != pPart->flgActive) {
        return GOAL_ERR_WRONG_STATE;
    }

    /* read partition value */
    GOAL_MEMCPY(pBuf, &pPart->pBufRead[pPart->idxPart], pPart->len);

    return GOAL_OK;
}


/****************************************************************************/
/** GOAL MI DM - Retrieve Direct Read Buffer
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miDmGroupReadBufGet(
    uint8_t **ppBuf,                            /**< [out] partition pointer */
    uint32_t *pLen,                             /**< partition length */
    GOAL_MI_DM_PART_T *pPart                    /**< partition data */
)
{
    if (NULL == pPart) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (GOAL_TRUE != pPart->flgActive) {
        return GOAL_ERR_WRONG_STATE;
    }

    /* return pointer and length */
    *ppBuf = &pPart->pBufRead[pPart->idxPart];
    *pLen = pPart->len;

    return GOAL_OK;
}


/****************************************************************************/
/** GOAL MI DM - Group Read Start
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miDmGroupReadStart(
    GOAL_MI_DM_GROUP_T *pGroup                  /**< partition group */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MI_DM_PART_T *pPart;                   /* partition iterator */

    if (NULL == pGroup) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* lock group metainfo */
    res = goal_lockGet(pGroup->pLock, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* if new data is available, exchange buffers if no read activity is detected */
    if ((0 == pGroup->cntActiveRead) && (GOAL_TRUE == pGroup->flgSync)) {

        /* clear group sync flag */
        pGroup->flgSync = GOAL_FALSE;

        /* exchange partition buffers */
        GOAL_LL_FOREACH(pGroup->pParts, pPart) {
            if (GOAL_TRUE == pPart->flgRead) {
                goal_miDmPartBufXchg(pPart, GOAL_MI_DM_XCHG_READ);
            }
        }
    }

    /* update active read counter */
    pGroup->cntActiveRead++;

    /* release group lock */
    goal_lockPut(pGroup->pLock);

    return GOAL_OK;
}


/****************************************************************************/
/** GOAL MI DM - Group Read End
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miDmGroupReadEnd(
    GOAL_MI_DM_GROUP_T *pGroup                  /**< partition group */
)
{
    GOAL_STATUS_T res;                          /* result */

    if (NULL == pGroup) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* lock group metainfo */
    res = goal_lockGet(pGroup->pLock, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* update active read counter */
    pGroup->cntActiveRead--;

    /* release group lock */
    goal_lockPut(pGroup->pLock);

    return GOAL_OK;
}


/****************************************************************************/
/** GOAL MI DM - Read Sync
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miDmReadSync(
    GOAL_MI_DM_T *pMiDm,                        /**< MI DM */
    uint8_t *pBuf                               /**< read buffer */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MI_DM_GROUP_T *pGroup;                 /* group iterator */
    GOAL_MI_DM_PART_T *pPart;                   /* partition iterator */
    GOAL_MI_DM_CB_T *pCb;                       /* callback iterator */

    if ((NULL == pMiDm) || (NULL == pBuf)) {
        return GOAL_ERR_NULL_POINTER;
    }

    GOAL_LL_FOREACH(pMiDm->pGroups, pGroup) {

        /* lock group metainfo
         *
         * iteration and modifications of the partition lists can be executed from
         * different contexts, thus need to be protected
         */
        res = goal_lockGet(pGroup->pLock, GOAL_LOCK_INFINITE);
        if (GOAL_RES_ERR(res)) {
            return res;
        }

        /* copy partition data */
        GOAL_LL_FOREACH(pGroup->pParts, pPart) {
            GOAL_MEMCPY(&pPart->pBufWrite[pPart->idxPart], &pBuf[pPart->idxPart], pPart->len);
        }

        /* indicate group update */
        pGroup->flgSync = GOAL_TRUE;

        /* exchange write buffer */
        GOAL_LL_FOREACH(pGroup->pParts, pPart) {
            goal_miDmPartBufXchg(pPart, GOAL_MI_DM_XCHG_WRITE);

            /* read buffer sync request */
            pPart->flgRead = GOAL_TRUE;
        }

        /* release group lock */
        goal_lockPut(pGroup->pLock);
    }

    /* inform listeners about the update */
    GOAL_LL_FOREACH(pMiDm->pGroups, pGroup) {
        GOAL_LL_FOREACH(pGroup->pCbsRead, pCb) {
            res = pCb->funcCb(pGroup, pCb->pPriv);
            if (GOAL_RES_ERR(res)) {
                return res;
            }
        }
    }

    /* inform global listeners about the update */
    GOAL_LL_FOREACH(pMiDm->pCbsRead, pCb) {
        res = pCb->funcCb(NULL, pCb->pPriv);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
    }

    return GOAL_OK;
}


/****************************************************************************/
/** GOAL MI DM - Get Group Handle by Index
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miDmGroupGetByIdx(
    GOAL_MI_DM_GROUP_T **ppGroup,               /**< [out] group handle */
    uint32_t idMiDm,                            /**< MI DM id */
    GOAL_ID_T idGroup                           /**< group id */
)
{
    GOAL_INSTANCE_T *pInst;                     /* instance iterator */
    GOAL_MI_DM_GROUP_T *pGroup;                 /* group iterator */

    GOAL_INSTANCE_FOREACH(mpListMiDm->pInst, pInst) {
        if (idMiDm == pInst->idInst) {
            GOAL_LL_FOREACH(((GOAL_MI_DM_T *) pInst)->pGroups, pGroup) {
                if (idGroup == pGroup->id) {
                    *ppGroup = pGroup;
                    return GOAL_OK;
                }
            }
        }
    }

    *ppGroup = NULL;
    return GOAL_ERR_NOT_FOUND;
}


/****************************************************************************/
/** GOAL MI DM - Get Instance By Part Handle
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miDmInstGetByPart(
    GOAL_MI_DM_T **ppDm,                        /**< [out] MI DM handle */
    GOAL_MI_DM_PART_T *pPart                    /**< part handle */
)
{
    if ((NULL == ppDm) || (NULL == pPart) || (NULL == pPart->pGroup) || (NULL == pPart->pGroup->pMiDm)) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (GOAL_TRUE != pPart->flgActive) {
        return GOAL_ERR_WRONG_STATE;
    }

    /* assign MI DM instance */
    *ppDm = pPart->pGroup->pMiDm;

    return GOAL_OK;
}


/****************************************************************************/
/** GOAL MI DM - Register Callback
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miDmCbReg(
    GOAL_MI_DM_T *pMiDm,                        /**< [in] MI DM handle */
    GOAL_MI_DM_GROUP_T *pGroup,                 /**< [in] MI DM group handle */
    uint32_t typeCb,                            /**< callback type */
    GOAL_MI_DM_CB_FUNC_T funcCb,                /**< callback function */
    void *pPriv                                 /**< callback private data */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MI_DM_CB_T *pCb = NULL;                /* callback data */

    /* allocate callback data */
    res = goal_memCalloc(&pCb, sizeof(GOAL_MI_DM_CB_T));
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* initialize callback data */
    pCb->funcCb = funcCb;
    pCb->pPriv = pPriv;

    /* attach callback to matching list */
    switch (typeCb) {
        case GOAL_MI_DM_CB_READ:
            if (NULL != pMiDm) {
                GOAL_LL_APPEND(pMiDm->pCbsRead, pCb);
            } else {
                GOAL_LL_APPEND(pGroup->pCbsRead, pCb);
            }
            break;

        case GOAL_MI_DM_CB_WRITE:
            if (NULL != pMiDm) {
                GOAL_LL_APPEND(pMiDm->pCbsWrite, pCb);
            } else {
                GOAL_LL_APPEND(pGroup->pCbsWrite, pCb);
            }
            break;

        default:
            return GOAL_ERR_PARAM;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** GOAL MI DM - Remove all mapped partitions
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miDmGroupPartsRemove(
    uint32_t idMiDm,                            /**< DM handle id */
    GOAL_ID_T idGroup                           /**< group id */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MI_DM_T *pMiDm = NULL;                 /* DM handle */
    GOAL_MI_DM_GROUP_T *pMiDmGroup = NULL;      /* DM group */
    GOAL_MI_DM_PART_T *pPart;                   /* dm partition */
    GOAL_MI_DM_PART_T **ppPartIt;               /* partition iterator */

    /* get instance */
    res = goal_instGetById((GOAL_INSTANCE_T **) &pMiDm, GOAL_ID_MI_DM, idMiDm);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("instance not found");
        return res;
    }

    /* create or get group */
    res = goal_miDmGroupNew(&pMiDmGroup, idMiDm, idGroup);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* lock group meta data */
    res = goal_lockGet(pMiDmGroup->pLock, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to lock meta data");
    }

    /* clear partition meta data */
    GOAL_LL_FOREACH(pMiDmGroup->pParts, pPart) {
        /* log notification of removed partition */
        goal_logInfo("part removed '%s', pos: %"FMT_u32", len: %"FMT_u32,
                 pMiDm->strInstDesc, pPart->idxPart, pPart->len);

        pPart->idxPart = 0;
        pPart->len = 0;
        pPart->pBufRead = NULL;
        pPart->pBufWrite = NULL;
        pPart->pBufXchg = NULL;
        pPart->flgRead = GOAL_FALSE;
        pPart->flgWrite = GOAL_FALSE;
        pPart->flgActive = GOAL_FALSE;

        /* remove partition from group partition list */
        GOAL_LL_DELETE(pMiDmGroup->pParts, pPart);

        /* remove partition from mi partition list */

        /* search entry in dm list */
        for (ppPartIt = &pMiDm->pParts; *ppPartIt; ppPartIt = &(*ppPartIt)->pNextAll) {
            if (pPart == *ppPartIt) {
                break;
            }
        }

        if (NULL != *ppPartIt) {
            /* if found entry in list, replacing with its successor */
            *ppPartIt = (*ppPartIt)->pNextAll;
        }
    }

    /* flush all parts from group */
    pMiDmGroup->pParts = NULL;
    pMiDmGroup->flgSync = GOAL_FALSE;
    pMiDmGroup->cntActiveRead = 0;
    pMiDmGroup->cntActiveWrite = 0;

    /* unlock group meta data */
    goal_lockPut(pMiDmGroup->pLock);

    return res;
}
