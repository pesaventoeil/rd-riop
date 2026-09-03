/** @file
 *
 * @brief GOAL nvs Media Adapter
 *
 * @details
 * Generic media adapter for nvs.
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

#define GOAL_ID GOAL_ID_MA_NVS
#include <goal_includes.h>
#include <goal_media/goal_ma_nvs.h>


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_MA_NVS_T *pListNvs = NULL;          /**< NVS list */
static unsigned int cntNvs = 0;                 /**< count of registered NVSs */


/****************************************************************************/
/** Register nvs driver
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maNvsReg(
    GOAL_ID_T id,                               /**< MA id */
    GOAL_MA_NVS_T **ppNvs,                      /**< flash driver handle */
    GOAL_MA_NVS_OPEN_T funcOpen,                /**< flash open function */
    GOAL_MA_NVS_READ_T funcRead,                /**< flash read function */
    GOAL_MA_NVS_WRITE_T funcWrite,              /**< flash write function */
    GOAL_MA_NVS_ERASE_T funcErase,              /**< flash erase function */
    GOAL_MA_NVS_BUSY_T funcBusy,                /**< flash busy function */
    void *pData                                 /**< driver specific data */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MA_NVS_T **ppElem;                     /* nvs element */

    /* allocate nvs handle */
    res = goal_memCalloc(ppNvs, sizeof(GOAL_MA_NVS_T));
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:261: */
        goal_lmLog(GOAL_ID_MA_NVS, 261, 212, 0, 0, GOAL_LOG_SEV_ERROR, "failed to allocate NVS handle");
        return res;
    }

    /* setup nvs handle */
    (*ppNvs)->funcOpen = funcOpen;
    (*ppNvs)->funcRead = funcRead;
    (*ppNvs)->funcWrite = funcWrite;
    (*ppNvs)->funcErase = funcErase;
    (*ppNvs)->funcBusy = funcBusy;
    (*ppNvs)->id = id;
    (*ppNvs)->pData = pData;

    /* iterate to list end */
    for (ppElem = &pListNvs; *ppElem; ppElem = &(*ppElem)->pNext);

    /* store nvs handle */
    *ppElem = *ppNvs;
    cntNvs++;

    return GOAL_OK;
}


/****************************************************************************/
/** nvs allocate function
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maNvsOpen(
    GOAL_MA_NVS_T *pNvs                         /**< flash driver handle */
)
{
    if (pNvs == NULL) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (GOAL_FALSE != pNvs->flgUsed) {
        return GOAL_ERR_ALREADY_USED;
    }

    pNvs->flgUsed = GOAL_TRUE;

    /* call drivers open function */
    return pNvs->funcOpen(pNvs);
}


/****************************************************************************/
/** nvs free function
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maNvsClose(
    GOAL_MA_NVS_T *pNvs                         /**< flash driver handle */
)
{
    if (pNvs == NULL) {
        return GOAL_ERR_NULL_POINTER;
    }

     if (GOAL_FALSE == pNvs->flgUsed) {
        return GOAL_ERR_WRONG_STATE;
    }

    pNvs->flgUsed = GOAL_FALSE;

    return GOAL_OK;
}


/****************************************************************************/
/** nvs read function
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maNvsRead(
    GOAL_MA_NVS_T *pNvs,                        /**< flash driver handle */
    uint32_t offset,                            /**< write offset */
    uint8_t *pData,                             /**< data ptr */
    uint32_t len                                /**< data length */
)
{
    if (pNvs == NULL) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (NULL == pNvs->funcRead) {
        return GOAL_ERR_UNSUPPORTED;
    }

    /* check flash size */
    if ((offset + len) > pNvs->infoFlash.sizeFlash) {
        goal_logErr("flash read beyond flash size");
        return GOAL_ERR_OVERFLOW;
    }

    return pNvs->funcRead(pNvs, offset, pData, len);
}


/****************************************************************************/
/** nvs write function
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maNvsWrite(
    GOAL_MA_NVS_T *pNvs,                        /**< flash driver handle */
    uint32_t offset,                            /**< write offset */
    uint8_t *pData,                             /**< data ptr */
    uint32_t len                                /**< data length */
)
{
    if (pNvs == NULL) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (NULL == pNvs->funcWrite) {
        return GOAL_ERR_UNSUPPORTED;
    }

    /* check flash size */
    if ((offset + len) > pNvs->infoFlash.sizeFlash) {
        goal_logErr("flash write beyond flash size");
        return GOAL_ERR_OVERFLOW;
    }

    return pNvs->funcWrite(pNvs, offset, pData, len);
}


/****************************************************************************/
/** nvs erase function
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maNvsErase(
    GOAL_MA_NVS_T *pNvs,                        /**< flash driver handle */
    uint32_t offset,                            /**< erase offset */
    uint32_t len                                /**< data length */
)
{
    if (pNvs == NULL) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* erase function is optinal because driver based technology
     * may require erase before write */
    if (NULL == pNvs->funcErase) {
        return GOAL_OK;
    }

    /* check flash size */
    if ((offset + len) > pNvs->infoFlash.sizeFlash) {
        goal_logErr("flash erase beyond flash size");
        return GOAL_ERR_OVERFLOW;
    }

    return pNvs->funcErase(pNvs, offset, len);
}


/****************************************************************************/
/** nvs status function
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maNvsBusy(
    GOAL_MA_NVS_T *pNvs,                        /**< flash driver handle */
    GOAL_BOOL_T *pStatus                        /**< flash busy status */
)
{
    if (pNvs == NULL) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (NULL != pNvs->funcBusy) {
        pNvs->funcBusy(pNvs, pStatus);
    }
    /* if no function is registered, we assume the write function
     * of the driver is synchronous, thus the driver is never busy
     * outside of this function
     */
    else {
        *pStatus = GOAL_FALSE;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Get MA by ID
 *
 * This function returns the ID associated MA.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maNvsGetById(
    GOAL_MA_NVS_T **ppHdlMa,                    /**< handle */
    GOAL_ID_T id                                /**< MA id */
)
{
    GOAL_MA_NVS_T *pListIdx = NULL;             /* list idx */
    GOAL_STATUS_T res;                          /* result */

    res = GOAL_ERR_NOT_FOUND;

    /* validate the arguments */
    if (NULL == ppHdlMa) {
        /* GG_LOG ID:262: */
        goal_lmLog(GOAL_ID_MA_NVS, 262, 213, 0, 0, GOAL_LOG_SEV_ERROR, "MA reference is NULL");
        return GOAL_ERR_NULL_POINTER;
    }

    /* search the entry with the correct ID */
    GOAL_LL_FOREACH(pListNvs, pListIdx) {
        if (pListIdx->id == id) {
            *ppHdlMa = pListIdx;
            res = GOAL_OK;
        }
    }

    /* no entry was found */
    return res;
}

