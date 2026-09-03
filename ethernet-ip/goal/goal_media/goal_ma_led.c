/** @file
 *
 * @brief GOAL LED Media Adapter
 *
 * @details
 * Generic media adapter for LEDs.
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

#define GOAL_ID GOAL_ID_MA_LED
#include <goal_includes.h>
#include <goal_media/goal_ma_led.h>


/****************************************************************************/
/* Local variables */
/****************************************************************************/

/**< internal lists for LED instances */
static GOAL_MA_LED_T *pListMaLed = NULL;


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/
static GOAL_STATUS_T goal_maLedGetLed(
    GOAL_MA_LED_T *pMaLedHdl,                   /**< led ma */
    uint32_t ledId,                             /**< led id */
    GOAL_MA_LED_LEDS_T **ppLed                  /**< [out] led */
);


/****************************************************************************/
/** Register DRV-LED
 *
 * This function creates a MA-LED handle and registers the DRV-LED functions.
 * This function must be called in state GOAL_FSA_INIT.
 *
 * @return
 * GOAL return status
 */
GOAL_STATUS_T goal_maLedReg(
    uint32_t maId,                              /**< [in] MA-ID */
    GOAL_MA_LED_OPEN fOpen,                     /**< [in] open function of DRV_LED */
    GOAL_MA_LED_GET fLedSingleGet,              /**< [in] get function of DRV-LED for a single LED */
    GOAL_MA_LED_SET fLedSingleSet,              /**< [in] set function of DRV-LED for a single LED */
    GOAL_MA_LED_TOGGLE fLedSingleToggle,        /**< [in] toggle function of DRV-LED for a single LED */
    GOAL_MA_LED_GROUP_GET fLedGroupGet,         /**< [in] get function of DRV-LED for a group of LEDs */
    GOAL_MA_LED_GROUP_SET fLedGroupSet,         /**< [in] set function of DRV-LED for a group of LEDs */
    GOAL_MA_LED_CLOSE fClose,                   /**< [in] close function of DRV_LED */
    void *pDrvData                              /**< [in] driver-specific data */
)
{
    GOAL_STATUS_T res;                          /* GOAL return status */
    GOAL_MA_LED_T *pMaLedHdl = NULL;            /* MA-LED handle */

    res = GOAL_OK;

    /* --- check configuration of function pointers --- */
    /* mandatory callbacks */
    if ((NULL == fOpen) || (NULL == fClose)) {
        goal_logErr("Invalid callback (NULL) for open and/or close during DRV-LED registration");
        res = GOAL_ERR_INIT;
    }
    /* get/set callbacks are optional */

    /* --- allocate MA-LED handle --- */
    if (GOAL_RES_OK(res)) {
        res = goal_memCalloc(&pMaLedHdl, sizeof(GOAL_MA_LED_T));
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to allocate MA-LED handle");
        }
    }

    /* --- setup MA-LED handle --- */
    if (GOAL_RES_OK(res)) {
        pMaLedHdl->maId = maId;
        pMaLedHdl->fOpen = fOpen;
        pMaLedHdl->fLedSingleGet = fLedSingleGet;
        pMaLedHdl->fLedSingleSet = fLedSingleSet;
        pMaLedHdl->fLedSingleToggle = fLedSingleToggle;
        pMaLedHdl->fLedGroupGet = fLedGroupGet;
        pMaLedHdl->fLedGroupSet = fLedGroupSet;
        pMaLedHdl->fClose = fClose;
        pMaLedHdl->pDrvData = pDrvData;

        /* store MA-LED handle */
        GOAL_LL_APPEND(pListMaLed, pMaLedHdl);
    }

    return res;
}


/****************************************************************************/
/** Deregister DRV-LED
 *
 * This function deregisters the DRV-LED functions and destroys the MA-LED handle.
 *
 * @return
 * GOAL return status
 */
GOAL_STATUS_T goal_maLedDeReg(
    GOAL_MA_LED_T **ppMaLedHdl                  /**< [in] MA-LED handle */
)
{
    GOAL_STATUS_T res;                          /* GOAL return status */
    GOAL_MA_LED_T **ppElem;                     /* LED instance in the internal list */

    /* search the end of the MA-LED handle in internal list */
    for (ppElem = &pListMaLed; ((*ppElem) && (*ppElem != *ppMaLedHdl)); ppElem = &(*ppElem)->pNext);

    /* delete MA-LED handle from internal list */
    if ((*ppElem != NULL) && (*ppElem == *ppMaLedHdl)) {
        *ppElem = (*ppElem)->pNext;
    }

    /* destroy MA-LED handle */
    res = goal_memFree(ppMaLedHdl);

    return res;
}


/****************************************************************************/
/** Register a LED for the selected MA-ID
 *
 * This function registers a led id to the selected MA-ID.
 *
 * @return
 * GOAL return status
 */
GOAL_STATUS_T goal_maLedRegisterLed(
    GOAL_MA_LED_T *pMaLedHdl,                   /**< [in] MA-LED handle */
    uint32_t id,                                /**< led id */
    uint32_t position                           /**< led position */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MA_LED_LEDS_T *pLed;                   /* led entry */

    if (NULL == pMaLedHdl) {
        return GOAL_ERR_NULL_POINTER;
    }

    res = goal_memCalloc(&pLed, sizeof(GOAL_MA_LED_LEDS_T));

    if (GOAL_RES_OK(res)) {
        pLed->id = id;
        pLed->position = position;
        GOAL_LL_APPEND(pMaLedHdl->pLeds, pLed);
    }

    return res;
}


/****************************************************************************/
/** Open a LED instance for the selected MA-ID
 *
 * This function opens the DRV-LED for the selected MA-ID.
 *
 * @return
 * GOAL return status
 */
GOAL_STATUS_T goal_maLedOpen(
    uint32_t maId,                              /**< [in] MA-ID */
    GOAL_MA_LED_T **ppMaLedHdl                  /**< [out] MA-LED handle */
)
{
    GOAL_STATUS_T res;                          /* GOAL return status */

    /* --- get MA_LED hable by MA-ID --- */
    res = goal_maLedGetById(ppMaLedHdl, maId);

    /* --- check whether MA-LED handle is already in use --- */
    if (GOAL_RES_OK(res)) {
        if (GOAL_TRUE == (*ppMaLedHdl)->flgUsed) {
            res = GOAL_ERR_ACCESS;
        }
    }

    /* --- invoke DRV-LED open function --- */
    if (GOAL_RES_OK(res)) {
        res = (*ppMaLedHdl)->fOpen(*ppMaLedHdl);
    }

    /* --- mark MA-LED handle as used --- */
    if (GOAL_RES_OK(res)) {
        (*ppMaLedHdl)->flgUsed = GOAL_TRUE;
    }

    return res;
}


/****************************************************************************/
/** Close the LED instance
 *
 * This function closes the DRV-LED for the selected MA-LED handle.
 *
 * @return
 * GOAL return status
 */
GOAL_STATUS_T goal_maLedClose(
    GOAL_MA_LED_T *pMaLedHdl                    /**< [in] MA-LED handle */
)
{
    GOAL_STATUS_T res;                          /* GOAL return status */

    /* --- check existence of the MA-LED handle --- */
    if (NULL == pMaLedHdl) {
        res = GOAL_ERR_NULL_POINTER;
    }

    /* --- invoke DRV-LED close function --- */
    else {
        res = (pMaLedHdl)->fClose(pMaLedHdl);
        pMaLedHdl->flgUsed = GOAL_FALSE;
    }

    return res;
}


/****************************************************************************/
/** get state of a single LED
 *
 * This function returns the current state of the single LED assigned to the MA-LED handle.
 *
 * @return
 * GOAL return status
 */
GOAL_STATUS_T goal_maLedGet(
    GOAL_MA_LED_T *pMaLedHdl,                   /**< [in] MA-LED handle */
    uint32_t ledId,                             /**< [in] led id */
    GOAL_MA_LED_STATE_T *pState                 /**< [out] current LED state */
)
{
    GOAL_STATUS_T res;                          /* GOAL return status */
    uint32_t ledNumber = 0;                     /* led position */
    GOAL_MA_LED_LEDS_T *pLed = NULL;            /* led properties */

    res = GOAL_OK;

    if (NULL == pMaLedHdl) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* --- invoke get function --- */
    if (NULL == pMaLedHdl->fLedSingleGet) {
        goal_logErr("No get single function for MA-LED %"FMT_u32" registered", pMaLedHdl->maId);
        res = GOAL_ERR_NULL_POINTER;
    }

    if (GOAL_RES_OK(res)) {
        res = goal_maLedGetLed(pMaLedHdl, ledId, &pLed);
    }

    if (GOAL_RES_OK(res)) {
        ledNumber = pLed->position;

        res = pMaLedHdl->fLedSingleGet(pMaLedHdl, ledNumber, pState);
    }

    return res;
}


/****************************************************************************/
/** set state of a single LED
 *
 * This function sets the state of the LED assigned to the MA-LED handle.
 *
 * @return
 * GOAL return status
 */
GOAL_STATUS_T goal_maLedSet(
    GOAL_MA_LED_T *pMaLedHdl,                   /**< [in] MA-LED handle */
    uint32_t ledId,                             /**< [in] led number */
    GOAL_MA_LED_STATE_T state                   /**< [in] desired LED state */
)
{
    GOAL_STATUS_T res;                          /* GOAL return status */
    uint32_t ledNumber = 0;                     /* led position */
    GOAL_MA_LED_LEDS_T *pLed = NULL;            /* led properties */

    res = GOAL_OK;

    if (NULL == pMaLedHdl) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* --- invoke set function --- */
    if (NULL == pMaLedHdl->fLedSingleSet) {
        goal_logErr("No set single function for MA-LED %"FMT_u32" registered", pMaLedHdl->maId);
        res = GOAL_ERR_NULL_POINTER;
    }

    if (GOAL_RES_OK(res)) {
        res = goal_maLedGetLed(pMaLedHdl, ledId, &pLed);
    }

    if (pLed != NULL && GOAL_RES_OK(res)) {
        ledNumber = pLed->position;
        res = pMaLedHdl->fLedSingleSet(pMaLedHdl, ledNumber, state);
    }

    return res;
}


/****************************************************************************/
/** toggle state of a single LED
 *
 * This function toggles the state of the LED assigned to the MA-LED handle.
 *
 * @return
 * GOAL return status
 */
GOAL_STATUS_T goal_maLedToggle(
    GOAL_MA_LED_T *pMaLedHdl,                   /**< [in] MA-LED handle */
    uint32_t ledId                              /**< [in] led id */
)
{
    GOAL_STATUS_T res;                          /* GOAL return status */
    uint32_t ledNumber = 0;                     /* led position */
    GOAL_MA_LED_LEDS_T *pLed = NULL;            /* led properties */

    res = GOAL_OK;

    if (NULL == pMaLedHdl) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* --- invoke set function --- */
    if (NULL == pMaLedHdl->fLedSingleToggle) {
        goal_logErr("No toggle single function for MA-LED %"FMT_u32" registered", pMaLedHdl->maId);
        res = GOAL_ERR_NULL_POINTER;
    }

    if (GOAL_RES_OK(res)) {
        res = goal_maLedGetLed(pMaLedHdl, ledId, &pLed);
    }

    if (GOAL_RES_OK(res)) {
        ledNumber = pLed->position;

        res = pMaLedHdl->fLedSingleToggle(pMaLedHdl, ledNumber);
    }

    return res;
}


/****************************************************************************/
/** get state of a group of LEDs
 *
 * This function returns the current state of the group of LEDs assigned to the MA-LED handle.
 *
 * @return
 * GOAL return status
 */
GOAL_STATUS_T goal_maLedGroupGet(
    GOAL_MA_LED_T *pMaLedHdl,                   /**< [in] MA-LED handle */
    uint32_t *pState                            /**< [out] current state of all LEDs in the LED group */
)
{
    GOAL_STATUS_T res;                          /* GOAL return status */

    res = GOAL_OK;

    if (NULL == pMaLedHdl) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* --- invoke get function --- */
    if (NULL == pMaLedHdl->fLedGroupGet) {
        goal_logErr("No get group function for MA-LED %"FMT_u32" registered", pMaLedHdl->maId);
        res = GOAL_ERR_NULL_POINTER;
    }
    else {
        res = pMaLedHdl->fLedGroupGet(pMaLedHdl, pState);
    }

    return res;
}


/****************************************************************************/
/** set state of a group of LEDs
 *
 * This function sets the state of the group of LEDs assigned to the MA-LED handle.
 *
 * @return
 * GOAL return status
 */
GOAL_STATUS_T goal_maLedGroupSet(
    GOAL_MA_LED_T *pMaLedHdl,                   /**< [in] MA-LED handle */
    uint32_t mask,                              /**< [in] mask to set LEDs in the LED group individual */
    uint32_t state                              /**< [in] desired state of the masked LEDs in the LED group */
)
{
    GOAL_STATUS_T res;                          /* GOAL return status */

    res = GOAL_OK;

    if (NULL == pMaLedHdl) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* --- invoke set function --- */
    if (NULL == pMaLedHdl->fLedGroupSet) {
        goal_logErr("No set group function for MA-LED %"FMT_u32" registered", pMaLedHdl->maId);
        res = GOAL_ERR_NULL_POINTER;
    }
    else {
        res = pMaLedHdl->fLedGroupSet(pMaLedHdl, mask, state);
    }

    return res;
}


/****************************************************************************/
/** get MA-LED handle by MA-ID
 *
 * This function returns the MA-LED handle assigned to the selected MA-ID.
 *
 * @return
 * GOAL return status
 */
GOAL_STATUS_T goal_maLedGetById(
    GOAL_MA_LED_T **ppMaLedHdl,                 /**< [out] MA-LED handle */
    uint32_t maId                               /**< [in] MA-ID */
)
{
    GOAL_STATUS_T res;                          /* GOAL return status */
    GOAL_MA_LED_T *pListTmpLed;                 /* temporary pointer at MA-LED handle list */

    res = GOAL_ERR_NOT_FOUND;
    pListTmpLed = NULL;

    /* --- validate arguments --- */
    if (NULL == ppMaLedHdl) {
        goal_logErr("MA-LED reference is NULL");
        res = GOAL_ERR_NULL_POINTER;
    }

    /* --- search the entry with the correct MA-ID --- */
    else {
        for (pListTmpLed = pListMaLed; (pListTmpLed); pListTmpLed = pListTmpLed->pNext) {
            if (maId == pListTmpLed->maId) {
                *ppMaLedHdl = pListTmpLed;
                res = GOAL_OK;
            }
        }
    }

    return res;
}


/****************************************************************************/
/** get led information according to led id
 *
 * This function returns the led information structure for a given led id
 *
 * @return
 * GOAL return status
 */
static GOAL_STATUS_T goal_maLedGetLed(
    GOAL_MA_LED_T *pMaLedHdl,                   /**< led ma */
    uint32_t ledId,                             /**< led id */
    GOAL_MA_LED_LEDS_T **ppLed                  /**< [out] led */
)
{
    GOAL_MA_LED_LEDS_T *pLed = NULL;            /* led entry */

    GOAL_LL_FOREACH(pMaLedHdl->pLeds, pLed) {
        if (pLed->id == ledId) {
            break;
        }
    }

    *ppLed = pLed;

    if (pLed != NULL) {
        return GOAL_OK;
    }
    else {
        return GOAL_ERR_NOT_FOUND;
    }
}
