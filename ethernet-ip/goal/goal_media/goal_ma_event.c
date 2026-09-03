/** @file
 *
 * @brief Generic MA for EVENT
 *
 * @details
 * This module provides a MA for EVENT access.
 * As a media adapter it provided a simple interface for utilizing a hardware
 * based EVENT controller.
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

#define GOAL_ID GOAL_ID_MA_EVENT
#include "goal_includes.h"
#include "goal_media/goal_ma_event.h"


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_INSTANCE_LIST_T *mpListInst;        /**< EVENT list */


/****************************************************************************/
/** Registers a EVENT MA compliant driver
 *
 * This function is called during board init to register available
 * EVENT drivers.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maEventDrvReg(
    uint32_t id,                                /**< ID */
    GOAL_MA_EVENT_T **ppHdlMaEvent,             /**< EVENT handle ref ptr */
    GOAL_MA_EVENT_OPEN fOpen,                   /**< open function */
    GOAL_MA_EVENT_ENABLE fEnable,               /**< enable function */
    GOAL_MA_EVENT_DISABLE fDisable,             /**< disable function */
    GOAL_MA_EVENT_SET_PENDING fSetPending,      /**< set pending function */
    GOAL_MA_EVENT_POLL fPoll,                   /**< poll function */
    void *pDrvData                              /**< driver specific data */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_INSTANCE_T *pElem = NULL;              /* event element */

    /* Check that all function ptr are not NULL */
    if (NULL == fOpen) {
        /* GG_LOG ID:269: */
        goal_lmLog(GOAL_ID_MA_EVENT, 269, 214, 0, 0, GOAL_LOG_SEV_ERROR, "open function not defined");
        return GOAL_ERR_NULL_POINTER;
    }

    /* create instance list */
    if (mpListInst == NULL) {
        res = goal_instListNew(&mpListInst, GOAL_ID);
    }

    /* create instance */
    if (GOAL_RES_OK(res)) {
        res = goal_instNew(&pElem,
                sizeof(GOAL_MA_EVENT_T),
                GOAL_ID,
                id,
                "goal_ma_event");
    }

    /* return handle */
    if (GOAL_RES_OK(res)) {
        *ppHdlMaEvent = (GOAL_MA_EVENT_T *) pElem;
        (*ppHdlMaEvent)->pDrvData = pDrvData;
        (*ppHdlMaEvent)->fctOpen = fOpen;
        (*ppHdlMaEvent)->fctEnable = fEnable;
        (*ppHdlMaEvent)->fctDisable = fDisable;
        (*ppHdlMaEvent)->fctPoll = fPoll;
        (*ppHdlMaEvent)->fctSetPending = fSetPending;
    }

    return res;
}


/****************************************************************************/
/** Open a EVENT MA
 *
 * After successful execution, *ppEventHdl contains a pointer to the opened
 * EVENT instance, otherwise the pointer is invalid.
 *
 * @retval GOAL_OK successful
 * @retval GOAL_ERR_NULL_POINTER notify handler was null
 * @retval other failed
 */
GOAL_STATUS_T goal_maEventOpen(
    uint32_t id,                                /**< id of EVENT handler to use */
    GOAL_MA_EVENT_T **ppEventHdl,               /**< pointer to store EVENT handler */
    GOAL_BOOL_T flgAutoStart,                   /**< auto start of event generation */
    GOAL_MA_EVENT_NOTIFY fNotify                /**< notify callback */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* Return value */
    GOAL_INSTANCE_T *pElem = NULL;              /* event element */

    /* Get EVENT handler for given ID */
    res = goal_instGetById(&pElem, GOAL_ID, id);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    ppEventHdl = (GOAL_MA_EVENT_T **) &pElem;

    /* Check whether handle is already in use */
    if ((*ppEventHdl)->flgUsed) {
        return GOAL_ERR_ACCESS;
    }

    /* Invoke open function */
    res = (*ppEventHdl)->fctOpen(*ppEventHdl, flgAutoStart);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* store cb */
    (*ppEventHdl)->fctNotify = fNotify;

    /* Mark handle as used */
    (*ppEventHdl)->flgUsed = GOAL_TRUE;

    if (flgAutoStart) {
        (*ppEventHdl)->flgEnable = GOAL_TRUE;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Enable event generation
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maEventEnable(
    GOAL_MA_EVENT_T *pEventHdl                  /**< MA handle for EVENT */
)
{
    if (NULL == pEventHdl) {
       return GOAL_ERR_NULL_POINTER;
    }

    if (NULL == pEventHdl->fctEnable) {
        return GOAL_ERR_UNSUPPORTED;
    }

    pEventHdl->flgEnable = GOAL_TRUE;

    /* Invoke start function */
    return pEventHdl->fctEnable(pEventHdl);
}


/****************************************************************************/
/** Disable event generation
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maEventDisable(
    GOAL_MA_EVENT_T *pEventHdl                  /**< MA handle for EVENT */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    if (NULL == pEventHdl) {
       return GOAL_ERR_NULL_POINTER;
    }

    if (NULL == pEventHdl->fctDisable) {
        return GOAL_ERR_UNSUPPORTED;
    }

    pEventHdl->flgEnable = GOAL_FALSE;

    /* Invoke clear function */
    res = (pEventHdl)->fctDisable(pEventHdl);

    return res;
}


/****************************************************************************/
/** Poll the current state of the event (e.g. ISR source)
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maEventPoll(
    GOAL_MA_EVENT_T *pEventHdl,                 /**< MA handle for EVENT */
    GOAL_BOOL_T *pState                         /**< event state */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    if (NULL == pEventHdl) {
       return GOAL_ERR_NULL_POINTER;
    }

    if (NULL == pEventHdl->fctPoll) {
        return GOAL_ERR_UNSUPPORTED;
    }

    /* Invoke clear function */
    res = (pEventHdl)->fctPoll(pEventHdl, pState);

    return res;
}


/****************************************************************************/
/** Set event generation as pending
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maEventSetPending(
    GOAL_MA_EVENT_T *pEventHdl                  /**< MA handle for EVENT */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    if (NULL == pEventHdl) {
       return GOAL_ERR_NULL_POINTER;
    }

    if (NULL == pEventHdl->fctSetPending) {
        return GOAL_ERR_UNSUPPORTED;
    }

    /* Invoke clear function */
    res = (pEventHdl)->fctSetPending(pEventHdl);

    return res;
}


/****************************************************************************/
/** Notification callback for EVENT event
 *
 * Once the driver detects an event, it will invoke this notify cb
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maEventDrvNotifyCb(
    GOAL_MA_EVENT_T *pHdlMaEvent,               /**< MA handle for EVENT */
    uint32_t id,                                /**< id of event */
    GOAL_MA_EVENT_EVENT_T event                 /**< event type */
)
{
    GOAL_STATUS_T res;                          /* return value */

    res = GOAL_OK;

    if (NULL == pHdlMaEvent) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (NULL != pHdlMaEvent->fctNotify) {
        res = (pHdlMaEvent)->fctNotify(pHdlMaEvent, id, event);
    }

    return res;
}


/****************************************************************************/
/** Get MA EVENT by ID
 *
 * This function returns the ID associated MA.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maEventGetById(
    GOAL_MA_EVENT_T **ppHdlMaEvent,             /**< EVENT handle ref ptr */
    uint32_t id                                 /**< MA id */
)
{
    return goal_instGetById((GOAL_INSTANCE_T **) ppHdlMaEvent, GOAL_ID, id);
}
