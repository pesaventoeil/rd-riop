/** @file
 *
 * @brief GOAL nvs media interface
 *
 * @details
 * Generic media interface for nvs.
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

#define GOAL_ID GOAL_ID_MI_NVS
#include <goal_includes.h>
#include <goal_media/goal_ma_nvs.h>
#include <goal_media/goal_mi_nvs.h>


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_MI_NVS_T *pListNvs;                 /**< NVS list */
static unsigned int cntMiNvs = 0;               /**< count of registered NVS MIs */
static GOAL_STAGE_HANDLER_T stageInit;          /**< init stage handler */
static GOAL_MI_NVS_BUFFERED_REGION_LIST_T *pListBufferedReg; /**< list of buffered regions */


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/
static void goal_miNvsLoop(
    void
);

static void goal_miNvsRegionProcess(
    GOAL_MI_NVS_REGION_T *pNvsRegion            /**< processed nvs region */
);

static GOAL_STATUS_T goal_miNvsBufferInitialize(
    void
);


/****************************************************************************/
/** Register nvs driver
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miNvsReg(
    GOAL_MI_NVS_T **ppMiNvs,                    /**< flash driver handle */
    GOAL_ID_T maId,                             /**< nvs MA id */
    GOAL_MI_NVS_REGION_LIST_T *pNvsRegionList,  /**< list of regions */
    int cntRegions                              /**< number of regions in list */
)
{
    GOAL_STATUS_T res;                          /* result */
    int idx;                                    /* index variable */
    GOAL_MI_NVS_REGION_T *ppRegion;             /* region handle */
    GOAL_MA_NVS_T *pMaNvs;                      /* flash MA to use */

    /* find MA */
    res = goal_maNvsGetById(&pMaNvs, maId);

    /* allocate nvs handle */
    if (GOAL_RES_OK(res)) {
        res = goal_memCalloc(ppMiNvs, sizeof(GOAL_MI_NVS_T));
    }

    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:316: */
        goal_lmLog(GOAL_ID_MI_NVS, 316, 266, 0, 0, GOAL_LOG_SEV_ERROR, "failed to allocate NVS MI handle");
        return res;
    }

     /* setup nvs mi handle */
    (*ppMiNvs)->pRegionList = NULL;
    (*ppMiNvs)->cntRegions = 0;
    (*ppMiNvs)->pMaNvs = pMaNvs;

    /* iterate to list end */
    GOAL_LL_APPEND(pListNvs, *ppMiNvs);

    /* install main loop function initially */
    if (0 == cntMiNvs) {
        res = goal_mainLoopReg(goal_miNvsLoop);

        /* register a stage to initialize buffered regions */
        if (GOAL_RES_OK(res)) {
            res = goal_mainStageReg(GOAL_STAGE_CM_MOD_ADD, &stageInit, GOAL_STAGE_INIT, goal_miNvsBufferInitialize);
        }
    }

    cntMiNvs++;

    if (NULL != pNvsRegionList) {
        for (idx = 0; idx < cntRegions; idx++) {

            /* register ranges of flash to the MI,
               while associating a MA with the region */
            res = goal_miNvsRegRegion(
                &ppRegion,
                (*ppMiNvs),
                pMaNvs,
                pNvsRegionList[idx].posStart,
                pNvsRegionList[idx].length,
                pNvsRegionList[idx].strName,
                pNvsRegionList[idx].id,
                pNvsRegionList[idx].mode,
                pNvsRegionList[idx].access);
            if (GOAL_RES_ERR(res)) {
                /* GG_LOG ID:317: */
                goal_lmLog(GOAL_ID_MI_NVS, 317, 267, 0, 0, GOAL_LOG_SEV_ERROR, "Could not register region");
                return res;
            }
        }
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Register region
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miNvsRegRegion(
    GOAL_MI_NVS_REGION_T **ppRegion,            /**< region handle */
    GOAL_MI_NVS_T *pMiNvs,                      /**< flash driver handle */
    GOAL_MA_NVS_T *pMaNvs,                      /**< flash MA to use */
    uint32_t offset,                            /**< offset of region */
    uint32_t len,                               /**< length of region */
    const char *strName,                        /**< name of region */
    unsigned int id,                            /**< id of region */
    GOAL_MI_NVS_REGION_MODE_T mode,             /**< region mode */
    GOAL_MI_NVS_REGION_ACCESS_T access          /**< access mode */
)
{
    GOAL_MI_NVS_REGION_T **ppElem;              /* nvs element */
    GOAL_MI_NVS_BUFFERED_REGION_LIST_T *pBufElem; /* buffered list element */
    GOAL_STATUS_T res;                          /* result */

    /* allocate nvs handle */
    res = goal_memCalloc(ppRegion, sizeof(GOAL_MI_NVS_REGION_T));
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:318: */
        goal_lmLog(GOAL_ID_MI_NVS, 318, 268, 0, 0, GOAL_LOG_SEV_ERROR, "failed to allocate NVS MI region handle");
        return res;
    }

    /* setup nvs mi handle */
    (*ppRegion)->strName = strName;
    (*ppRegion)->pMiNvs = pMiNvs;
    (*ppRegion)->offset = offset;
    (*ppRegion)->length = len;
    (*ppRegion)->id = id;

    if (0 != pMaNvs->infoFlash.sizeFlash) {
        if ((offset + len > pMaNvs->infoFlash.sizeFlash)) {
            /* GG_LOG ID:321: */
            goal_lmLog(GOAL_ID_MI_NVS, 321, 271, 4, 0, GOAL_LOG_SEV_WARNING, "Region at offset $1 exceeds flash size");
            goal_lmLogParamUINT32(offset);
            goal_lmLogFinish();
        }
    }

    if (0 != pMaNvs->infoFlash.sizeErase) {
        if ((offset % pMaNvs->infoFlash.sizeErase) != 0) {
            /* GG_LOG ID:319: */
            goal_lmLog(GOAL_ID_MI_NVS, 319, 269, 4, 4, GOAL_LOG_SEV_WARNING, "Region starting at $1 unaligned to sector size of $2");
            goal_lmLogParamUINT32(offset);
            goal_lmLogParamUINT32(pMaNvs->infoFlash.sizeErase);
            goal_lmLogFinish();

            return GOAL_ERR_UNALIGNED;
        }
    }

    /* allocate buffer for buffered access mode */
    if (GOAL_MI_NVS_REGION_MODE_BUFFERED == mode) {
        res = goal_memCalloc(&(*ppRegion)->wrDesc.pBuffer, len);
        if (GOAL_RES_ERR(res)) {
            /* GG_LOG ID:322: */
            goal_lmLog(GOAL_ID_MI_NVS, 322, 272, 0, 0, GOAL_LOG_SEV_ERROR, "failed to allocate buffer for nvs region");
            return GOAL_ERR_ALLOC_INIT;
        }
    }

    (*ppRegion)->access = access;
    (*ppRegion)->mode = mode;

    /* iterate to list end */
    for (ppElem = &pMiNvs->pRegionList; *ppElem; ppElem = &(*ppElem)->pNext);

    /* store region handle */
    *ppElem = *ppRegion;

    /* store it within the bufferered region list */
    if (mode == GOAL_MI_NVS_REGION_MODE_BUFFERED) {

        res = goal_memCalloc(&pBufElem, sizeof(GOAL_MI_NVS_BUFFERED_REGION_LIST_T));
        if (GOAL_RES_OK(res)) {
            pBufElem->pNvsRegion = (*ppElem);
            GOAL_LL_APPEND(pListBufferedReg, pBufElem);
        }
    }

    /* store nvs mi handle */
    pMiNvs->cntRegions++;

    return GOAL_OK;
}


/****************************************************************************/
/** Get region by id
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miNvsRegionGetById(
    GOAL_MI_NVS_REGION_T **ppRegion,            /**< ptr ref for region */
    unsigned int id                             /**< region id */
)
{
    GOAL_MI_NVS_T *pMiElem;                     /* MI element */
    GOAL_MI_NVS_REGION_T *pRegElem;             /* region element */

    GOAL_LL_FOREACH(pListNvs, pMiElem) {
        GOAL_LL_FOREACH(pMiElem->pRegionList, pRegElem) {
            if (pRegElem->id == id) {
                *ppRegion = pRegElem;
                return GOAL_OK;
            }
        }
    }

    return GOAL_ERR_NOT_FOUND;
}


/****************************************************************************/
/** Open a region
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miNvsRegionOpen(
    GOAL_MI_NVS_REGION_T *pRegion               /**< flash driver handle */
)
{
    GOAL_STATUS_T res;                          /* result */

    /* call ma open function */
    res = goal_maNvsOpen(pRegion->pMiNvs->pMaNvs);

    if (res == GOAL_ERR_ALREADY_USED) {
        res = GOAL_OK;
    }

    return res;
}


/****************************************************************************/
/** Write data to region
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miNvsWrite(
    GOAL_MI_NVS_REGION_T *pRegion,              /**< flash driver handle */
    uint8_t *pData,                             /**< data ptr */
    uint32_t offset,                            /**< write offset */
    uint32_t len                                /**< data length */
)
{
    if ((NULL == pRegion) || (NULL == pRegion->pMiNvs) || (NULL == pRegion->pMiNvs->pMaNvs)) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* check if region can be written */
    if (pRegion->access != GOAL_MI_NVS_REGION_ACCESS_WRITE) {
        return GOAL_ERR_NVS_WRITE;
    }

    /* check for overflow */
    if ((offset + len) > pRegion->length) {
        return GOAL_ERR_OVERFLOW;
    }

    /* only stream mode allows write access to arbitrary position within the region */
    if ((pRegion->mode != GOAL_MI_NVS_REGION_MODE_STREAM) && (0 != offset)) {
        goal_logErr("region isn't in stream mode but offset is set");
        return GOAL_ERR_NVS_WRITE;
    }

    /* buffered mode handling */
    if (GOAL_MI_NVS_REGION_MODE_BUFFERED == pRegion->mode) {

        /* check if content changed */
        if (GOAL_CMP_EQUAL == GOAL_MEMCMP(pRegion->wrDesc.pBuffer, pData, len)) {

            /* GG_LOG ID:324: */
            goal_lmLog(GOAL_ID_MI_NVS, 324, 274, GOAL_STRLEN((const char *) pRegion->strName), 0, GOAL_LOG_SEV_INFO, "flash region $1 is up to date");
            goal_lmLogParamSTRING0(pRegion->strName);
            goal_lmLogFinish();

            return GOAL_OK;
        }

        /* GG_LOG ID:323: */
        goal_lmLog(GOAL_ID_MI_NVS, 323, 273, GOAL_STRLEN((const char *) pRegion->strName), 0, GOAL_LOG_SEV_INFO, "update changed flash region $1");
        goal_lmLogParamSTRING0(pRegion->strName);
        goal_lmLogFinish();

        /* start write */
        GOAL_MEMCPY(pRegion->wrDesc.pBuffer, pData, len);
        pRegion->wrDesc.nvsState = GOAL_MI_FLASH_STATE_IDLE;
        pRegion->wrDesc.offset = pRegion->offset + offset;
        pRegion->wrDesc.len = len;
        pRegion->wrDesc.nvsTriggerWrite = GOAL_TRUE;

        return GOAL_OK;
    }

    /* start synchronized programming for non-buffered mode */
    pRegion->wrDesc.nvsState = GOAL_MI_FLASH_STATE_WRITE_START;
    pRegion->wrDesc.pBuffer = pData;
    pRegion->wrDesc.offset = pRegion->offset + offset;
    pRegion->wrDesc.len = len;
    pRegion->wrDesc.nvsTriggerWrite = GOAL_TRUE;

    /* process ended when state is idle */
    while (GOAL_MI_FLASH_STATE_IDLE != pRegion->wrDesc.nvsState) {

        /* run flash state machine */
        goal_miNvsRegionProcess(pRegion);

        /* run GOAL tasks */
        if (goal_isInitialized()) {
            goal_loop();
        }
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Read data from region
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miNvsRead(
    GOAL_MI_NVS_REGION_T *pRegion,              /**< flash driver handle */
    uint8_t *pData,                             /**< data ptr */
    uint32_t offset,                            /**< read offset */
    uint32_t len                                /**< data length */
)
{
    GOAL_STATUS_T res;                          /* result */
    uint32_t offsetParam;                       /* offset Parameter */

    if (NULL == pRegion || NULL == pRegion->pMiNvs ||
        NULL == pRegion->pMiNvs->pMaNvs ||
        NULL == pRegion->pMiNvs->pMaNvs->funcRead
       ) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* check for overflow */
    if ((offset + len) > pRegion->length) {
        return GOAL_ERR_OVERFLOW;
    }

    /* only stream and buffered mode allow read access to arbitrary position within the region */
    if ((pRegion->mode == GOAL_MI_NVS_REGION_MODE_COMPLETE) && (0 != offset)) {
        goal_logErr("region isn't in stream mode but offset is set");
        return GOAL_ERR_NVS_READ;
    }

    if (pRegion->mode == GOAL_MI_NVS_REGION_MODE_COMPLETE) {
        offsetParam = pRegion->offset;
    }
    else if (pRegion->mode == GOAL_MI_NVS_REGION_MODE_STREAM) {
        offsetParam = pRegion->offset + offset;
    }
    else {
        offsetParam = pRegion->offset;
    }

    /* buffer data for buffered mode and trigger write operation */
    if (GOAL_MI_NVS_REGION_MODE_BUFFERED == pRegion->mode) {
        GOAL_MEMCPY(pData, &pRegion->wrDesc.pBuffer[offset], len);
        res = GOAL_OK;
    }
    else {
        res = goal_maNvsRead(pRegion->pMiNvs->pMaNvs, offsetParam, pData, len);
    }

    return res;
}


/****************************************************************************/
/** Erase region
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miNvsErase(
    GOAL_MI_NVS_REGION_T *pRegion               /**< flash driver handle */
)
{
    if ((NULL == pRegion) || (NULL == pRegion->pMiNvs) || (NULL == pRegion->pMiNvs->pMaNvs)) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* check if erase is required by the underlying technology */
    if (pRegion->pMiNvs->pMaNvs->infoFlash.sizeErase == 0) {
        return GOAL_OK;
    }

    /* start flash erase */
    pRegion->wrDesc.nvsState = GOAL_MI_FLASH_STATE_ERASE_START;
    pRegion->wrDesc.offset = pRegion->offset;
    pRegion->wrDesc.len = pRegion->length;
    pRegion->wrDesc.nvsTriggerWrite = GOAL_TRUE;

    /* process ended when state is idle or write_start */
    while ((GOAL_MI_FLASH_STATE_IDLE != pRegion->wrDesc.nvsState) && (GOAL_MI_FLASH_STATE_WRITE_START != pRegion->wrDesc.nvsState)) {

        /* run flash state machine */
        goal_miNvsRegionProcess(pRegion);

        /* run GOAL tasks */
        if (goal_isInitialized()) {
            goal_loop();
        }
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Loop Processor for buffered Regions
 *
 */
static void goal_miNvsLoop(
    void
)
{
    GOAL_MI_NVS_BUFFERED_REGION_LIST_T *pElem;  /* list element */

    /* iterate over list of buffered regions */
    GOAL_LL_FOREACH(pListBufferedReg, pElem) {
        goal_miNvsRegionProcess(pElem->pNvsRegion);
    }
}


/****************************************************************************/
/** NVS MI state machine for buffered writing
 *
 */
static void goal_miNvsRegionProcess(
    GOAL_MI_NVS_REGION_T *pNvsRegion            /**< processed nvs region */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_BOOL_T busyStatus = GOAL_FALSE;        /* flash busy status */
    uint32_t sizeProgram;                       /* program sector size */

    /* switch through NVS state */
    switch (pNvsRegion->wrDesc.nvsState) {

        case GOAL_MI_FLASH_STATE_IDLE:

            /* check if NVS write is triggered */
            if (GOAL_TRUE == pNvsRegion->wrDesc.nvsTriggerWrite) {

                if (GOAL_MASK_IS_SET(pNvsRegion->pMiNvs->pMaNvs->infoFlash.flags, GOAL_MA_NVS_FLAG_ERASE_REQUIRED)) {
                    /* set NVS state to erase */
                    pNvsRegion->wrDesc.nvsStateNext = GOAL_MI_FLASH_STATE_ERASE_START;
                } else {
                    /* set NVS state to write if erase is not required */
                    pNvsRegion->wrDesc.nvsStateNext = GOAL_MI_FLASH_STATE_WRITE_START;
                }

                /* reset write trigger flag */
                pNvsRegion->wrDesc.nvsTriggerWrite = GOAL_FALSE;
            }

            break;

        case GOAL_MI_FLASH_STATE_ERASE_START:

            /* check if flash is ready */
            res = goal_maNvsBusy(pNvsRegion->pMiNvs->pMaNvs, &busyStatus);
            if (GOAL_RES_ERR(res)) {
                /* TODO: report error */
                break;
            }

            /* flash isn't ready, check again */
            if (GOAL_TRUE == busyStatus) {
                break;
            }

            /* initialize erase process */
            pNvsRegion->wrDesc.pos = 0;
            pNvsRegion->wrDesc.nvsStateNext = GOAL_MI_FLASH_STATE_ERASE;

            GOAL_TARGET_FALLTHROUGH;

        case GOAL_MI_FLASH_STATE_ERASE:

            /* erase flash area */
            res = goal_maNvsErase(pNvsRegion->pMiNvs->pMaNvs, pNvsRegion->wrDesc.offset + pNvsRegion->wrDesc.pos, pNvsRegion->pMiNvs->pMaNvs->infoFlash.sizeErase);
            if (GOAL_RES_ERR(res)) {
                /* TODO: report error */
                break;
            }

            /* next erase area */
            pNvsRegion->wrDesc.pos += pNvsRegion->pMiNvs->pMaNvs->infoFlash.sizeErase;

            /* wait until erase is done */
            pNvsRegion->wrDesc.nvsStateNext = GOAL_MI_FLASH_STATE_ERASE_WAIT;

            break;

        case GOAL_MI_FLASH_STATE_ERASE_WAIT:

            /* check if erasing finished */
            res = goal_maNvsBusy(pNvsRegion->pMiNvs->pMaNvs, &busyStatus);
            if (GOAL_RES_ERR(res)) {
                /* TODO: report error */
                break;
            }

            /* erase is still in progress, check again */
            if (GOAL_TRUE == busyStatus) {
                break;
            }

            /* if erase is done, start writing */
            if (pNvsRegion->wrDesc.len <= pNvsRegion->wrDesc.pos) {
                pNvsRegion->wrDesc.nvsStateNext = GOAL_MI_FLASH_STATE_WRITE_START;
                break;
            }

            /* otherwise erase next sector */
            pNvsRegion->wrDesc.nvsStateNext = GOAL_MI_FLASH_STATE_ERASE;
            break;

        case GOAL_MI_FLASH_STATE_WRITE_START:

            /* check if flash is ready */
            res = goal_maNvsBusy(pNvsRegion->pMiNvs->pMaNvs, &busyStatus);
            if (GOAL_RES_ERR(res)) {
                /* TODO: report error */
                break;
            }

            /* flash isn't ready, check again */
            if (GOAL_TRUE == busyStatus) {
                break;
            }

            /* initialize write process */
            pNvsRegion->wrDesc.pos = 0;
            pNvsRegion->wrDesc.nvsStateNext = GOAL_MI_FLASH_STATE_WRITE;

            break;

        case GOAL_MI_FLASH_STATE_WRITE:

            /* calculate program size */
            if ((pNvsRegion->wrDesc.len - pNvsRegion->wrDesc.pos) > pNvsRegion->pMiNvs->pMaNvs->infoFlash.sizeProgram) {
                sizeProgram = pNvsRegion->pMiNvs->pMaNvs->infoFlash.sizeProgram;
            } else {
                sizeProgram = pNvsRegion->wrDesc.len - pNvsRegion->wrDesc.pos;
            }

            /* write flash sector */
            res = goal_maNvsWrite(pNvsRegion->pMiNvs->pMaNvs, pNvsRegion->wrDesc.offset + pNvsRegion->wrDesc.pos, &pNvsRegion->wrDesc.pBuffer[pNvsRegion->wrDesc.pos], sizeProgram);
            if (GOAL_RES_ERR(res)) {
                /* TODO: report error */
                break;
            }

            /* next program sector */
            pNvsRegion->wrDesc.pos += sizeProgram;

            /* wait until programming is done */
            pNvsRegion->wrDesc.nvsStateNext = GOAL_MI_FLASH_STATE_WRITE_WAIT;

            break;

        case GOAL_MI_FLASH_STATE_WRITE_WAIT:

            /* check if writing finished */
            res = goal_maNvsBusy(pNvsRegion->pMiNvs->pMaNvs, &busyStatus);
            if (GOAL_RES_ERR(res)) {
                /* TODO: report error */
                break;
            }

            /* writing is still in progress, check again */
            if (GOAL_TRUE == busyStatus) {
                break;
            }

            /* if writing is done */
            if (pNvsRegion->wrDesc.len <= pNvsRegion->wrDesc.pos) {
                pNvsRegion->wrDesc.nvsStateNext = GOAL_MI_FLASH_STATE_IDLE;
                break;
            }

            /* otherwise program next sector */
            pNvsRegion->wrDesc.nvsStateNext = GOAL_MI_FLASH_STATE_WRITE;
            break;

        default:

            /* TODO: report error */
            pNvsRegion->wrDesc.nvsStateNext = GOAL_MI_FLASH_STATE_IDLE;

            break;
    }

    /* halt state machine on error */
    if (GOAL_RES_ERR(res)) {
        pNvsRegion->wrDesc.nvsStateNext = GOAL_MI_FLASH_STATE_IDLE;
        return;
    }

    /* go to next state */
    if (pNvsRegion->wrDesc.nvsState != pNvsRegion->wrDesc.nvsStateNext) {
        pNvsRegion->wrDesc.nvsState = pNvsRegion->wrDesc.nvsStateNext;
    }
}


/****************************************************************************/
/** Initialize buffered regions
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_miNvsBufferInitialize(
    void
)
{
    GOAL_MI_NVS_REGION_T *pNvsRegion;           /* processed nvs region */
    GOAL_MI_NVS_BUFFERED_REGION_LIST_T *pElem;  /* list element */
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    /* iterate over list of buffered regions */
    GOAL_LL_FOREACH(pListBufferedReg, pElem) {

        /* region shortcut */
        pNvsRegion = pElem->pNvsRegion;

        /* initialize buffer */
        res = goal_maNvsRead(
            pNvsRegion->pMiNvs->pMaNvs,
            pNvsRegion->offset,
            &pNvsRegion->wrDesc.pBuffer[0],
            pNvsRegion->length);
        if (GOAL_RES_ERR(res)) {
            break;
        }
    }

    return res;
}
