/** @file
 *
 * @brief Generic MA for ID
 *
 * @details
 * This module provides a MA for ID.
 * As a media adapter it provided a simple interface for getting the value of external
 * ID configuration hardware.
 *
 * @copyright
 * Copyright 2010-2020.
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

#define GOAL_ID GOAL_ID_MA_ID
#include "goal_includes.h"
#include "goal_media/goal_ma_id.h"


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_INSTANCE_LIST_T *mpListInst;        /**< ID list */


/****************************************************************************/
/** Registers a ID MA compliant driver
 *
 * This function is called during board init to register available
 * ID drivers.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maIdDrvReg(
    uint32_t id,                                /**< ID */
    GOAL_MA_ID_T **ppHdlMaId,                   /**< ID handle ref ptr */
    GOAL_MA_ID_OPEN fOpen,                      /**< open function */
    GOAL_MA_ID_GET fGet,                        /**< get function */
    GOAL_MA_ID_CLOSE fClose,                    /**< close function */
    void *pDrvData                              /**< driver specific data */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_INSTANCE_T *pElem = NULL;              /* wd element */

    /* Check that all function ptr are not NULL */
    if (NULL == fOpen || NULL == fGet || NULL == fClose) {
        /* GG_LOG ID:269: */
        goal_lmLog(GOAL_ID_MA_ID, 269, 214, 0, 0, GOAL_LOG_SEV_ERROR, "open function not defined");
        return GOAL_ERR_NULL_POINTER;
    }

    /* create instance list */
    if (mpListInst == NULL) {
        res = goal_instListNew(&mpListInst, GOAL_ID);
    }

    /* create instance */
    if (GOAL_RES_OK(res)) {
        res = goal_instNew(&pElem,
                sizeof(GOAL_MA_ID_T),
                GOAL_ID,
                id,
                "goal_ma_id");
    }

    /* return handle */
    if (GOAL_RES_OK(res)) {
        *ppHdlMaId = (GOAL_MA_ID_T *) pElem;
        (*ppHdlMaId)->pDrvData = pDrvData;
        (*ppHdlMaId)->fctOpen = fOpen;
        (*ppHdlMaId)->fctGet = fGet;
        (*ppHdlMaId)->fctClose = fClose;
    }

    return res;
}


/****************************************************************************/
/** Open a ID MA
 *
 * After successful execution, *ppIdHdl contains a pointer to the opened
 * ID instance, otherwise the pointer is invalid.
 *
 * @retval GOAL_OK successful
 * @retval GOAL_ERR_NULL_POINTER notify handler was null
 * @retval other failed
 */
GOAL_STATUS_T goal_maIdOpen(
    uint32_t id,                                /**< id of ID handler to use */
    GOAL_MA_ID_T **ppIdHdl                      /**< pointer to store ID handler */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* Return value */
    GOAL_INSTANCE_T *pElem = NULL;              /* wd element */

    /* Get ID handler for given ID */
    res = goal_instGetById(&pElem, GOAL_ID, id);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    ppIdHdl = (GOAL_MA_ID_T **) &pElem;

    /* Check whether handle is already in use */
    if ((*ppIdHdl)->flgUsed) {
        return GOAL_ERR_ACCESS;
    }

    /* Invoke open function */
    res = (*ppIdHdl)->fctOpen(*ppIdHdl);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* Mark handle as used */
    (*ppIdHdl)->flgUsed = GOAL_TRUE;

    return GOAL_OK;
}


/****************************************************************************/
/** Get an ID
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maIdGet(
    GOAL_MA_ID_T *pIdHdl,                       /**< pointer to store ID handler */
    uint32_t *pId                               /**< id value */
)
{
    if (NULL == pIdHdl) {
       return GOAL_ERR_NULL_POINTER;
    }

    /* Invoke start function */
    return pIdHdl->fctGet(pIdHdl, pId);
}


/****************************************************************************/
/** Close an ID instance
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maIdClose(
    GOAL_MA_ID_T *pIdHdl                        /**< pointer to store ID handler */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    if (NULL == pIdHdl) {
       return GOAL_ERR_NULL_POINTER;
    }

    /* Invoke clear function */
    res = (pIdHdl)->fctClose(pIdHdl);

    if (GOAL_RES_OK(res)) {
        pIdHdl->flgUsed = GOAL_FALSE;
    }

    return res;
}


/****************************************************************************/
/** Get MA ID by ID
 *
 * This function returns the ID associated MA.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maIdGetById(
    GOAL_MA_ID_T **ppHdlMaId,                   /**< ID handle ref ptr */
    uint32_t id                                 /**< MA id */
)
{
    return goal_instGetById((GOAL_INSTANCE_T **) ppHdlMaId, GOAL_ID, id);
}
