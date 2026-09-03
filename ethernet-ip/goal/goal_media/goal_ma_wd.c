/** @file
 *
 * @brief Generic MA for WD
 *
 * @details
 * This module provides a MA for WD.
 * As a media adapter it provided a simple interface for utilizing a hardware
 * based watchdog timer. If the watchdog timer is activated, it will require
 * a cyclic update before if ommited it consequently reset the hardware.
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

#define GOAL_ID GOAL_ID_MA_WD
#include "goal_includes.h"
#include "goal_media/goal_ma_wd.h"


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_INSTANCE_LIST_T *mpListInst;        /**< WD list */


/****************************************************************************/
/** Registers a WD MA compliant driver
 *
 * This function is called during board init to register available
 * WD drivers.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maWdDrvReg(
    uint32_t id,                                /**< ID */
    GOAL_MA_WD_T **ppHdlMaWd,                   /**< WD handle ref ptr */
    GOAL_MA_WD_OPEN fOpen,                      /**< open function */
    GOAL_MA_WD_START fStart,                    /**< start function */
    GOAL_MA_WD_CLEAR fClear,                    /**< clear function */
    void *pDrvData                              /**< driver specific data */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_INSTANCE_T *pElem = NULL;              /* wd element */

    /* Check that all function ptr are not NULL */
    if (NULL == fOpen) {
        /* GG_LOG ID:269: */
        goal_lmLog(GOAL_ID_MA_WD, 269, 214, 0, 0, GOAL_LOG_SEV_ERROR, "open function not defined");
        return GOAL_ERR_NULL_POINTER;
    }

    /* create instance list */
    if (mpListInst == NULL) {
        res = goal_instListNew(&mpListInst, GOAL_ID);
    }

    /* create instance */
    if (GOAL_RES_OK(res)) {
        res = goal_instNew(&pElem,
                sizeof(GOAL_MA_WD_T),
                GOAL_ID,
                id,
                "goal_ma_wd");
    }

    /* return handle */
    if (GOAL_RES_OK(res)) {
        *ppHdlMaWd = (GOAL_MA_WD_T *) pElem;
        (*ppHdlMaWd)->pDrvData = pDrvData;
        (*ppHdlMaWd)->fctOpen = fOpen;
        (*ppHdlMaWd)->fctStart = fStart;
        (*ppHdlMaWd)->fctClear = fClear;
    }

    return res;
}


/****************************************************************************/
/** Open a WD MA
 *
 * After successful execution, *ppWdHdl contains a pointer to the opened
 * WD instance, otherwise the pointer is invalid.
 *
 * @retval GOAL_OK successful
 * @retval GOAL_ERR_NULL_POINTER notify handler was null
 * @retval other failed
 */
GOAL_STATUS_T goal_maWdOpen(
    uint32_t id,                                /**< id of WD handler to use */
    GOAL_MA_WD_T **ppWdHdl                      /**< pointer to store WD handler */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* Return value */
    GOAL_INSTANCE_T *pElem = NULL;              /* wd element */

    /* Get WD handler for given ID */
    res = goal_instGetById(&pElem, GOAL_ID, id);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    ppWdHdl = (GOAL_MA_WD_T **) &pElem;

    /* Check whether handle is already in use */
    if ((*ppWdHdl)->flgUsed) {
        return GOAL_ERR_ACCESS;
    }

    /* Invoke open function */
    res = (*ppWdHdl)->fctOpen(*ppWdHdl);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* Mark handle as used */
    (*ppWdHdl)->flgUsed = GOAL_TRUE;

    return GOAL_OK;
}


/****************************************************************************/
/** Start an WD
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maWdStart(
    GOAL_MA_WD_T *pWdHdl                        /**< pointer to store WD handler */
)
{
    if (NULL == pWdHdl) {
       return GOAL_ERR_NULL_POINTER;
    }

    if (NULL == pWdHdl->fctStart) {
        return GOAL_ERR_UNSUPPORTED;
    }

    /* Invoke start function */
    return pWdHdl->fctStart(pWdHdl);
}


/****************************************************************************/
/** Clears an WD instance
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maWdClear(
    GOAL_MA_WD_T *pWdHdl                        /**< pointer to store WD handler */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    if (NULL == pWdHdl) {
       return GOAL_ERR_NULL_POINTER;
    }

    if (NULL == pWdHdl->fctClear) {
        return GOAL_ERR_UNSUPPORTED;
    }

    /* Invoke clear function */
    res = (pWdHdl)->fctClear(pWdHdl);

    return res;
}


/****************************************************************************/
/** Get MA WD by ID
 *
 * This function returns the ID associated MA.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maWdGetById(
    GOAL_MA_WD_T **ppHdlMaWd,                   /**< WD handle ref ptr */
    uint32_t id                                 /**< MA id */
)
{
    return goal_instGetById((GOAL_INSTANCE_T **) ppHdlMaWd, GOAL_ID, id);
}
