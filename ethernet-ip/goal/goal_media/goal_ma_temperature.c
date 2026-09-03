/** @file
 *
 * @brief Generic MA for temperature
 *
 * @details
 * This module provides a MA for temperature
 *
 *
 * @copyright
 * Copyright 2024.
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

#define GOAL_ID GOAL_ID_MA_TEMPERATURE
#include "goal_includes.h"
#include "goal_media/goal_ma_temperature.h"
#include "goal_util.h"


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_MA_TEMPERATURE_T *mpListTemperature = NULL; /**< Temperature Handler list */


/****************************************************************************/
/** Registers a temperature MA compliant driver
 *
 * This function is called during board init to register available
 * Temperature drivers.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maTemperatureDrvReg(
    uint32_t id,                                /**< ID */
    GOAL_MA_TEMPERATURE_T **ppTempHdl,          /**< [out] temperature handle ref ptr */
    GOAL_MA_TEMPERATURE_CONFIG_GET fConfGet,    /**< config get function */
    GOAL_MA_TEMPERATURE_CONFIG_SET fConfSet,    /**< config set function */
    GOAL_MA_TEMPERATURE_OPEN fOpen,             /**< Open function */
    GOAL_MA_TEMPERATURE_CLOSE fClose,           /**< Close function */
    GOAL_MA_TEMPERATURE_GET_VALUE fGetVal,      /**< get temperature function */
    void *pDrvData                              /**< [in] driver specific data */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MA_TEMPERATURE_T *pMaValid = NULL;     /* MA validation element */

    /* NULL pointer check */
    if (NULL == ppTempHdl) {
        goal_logErr("MA Temperature drv handle pointer is null ");
        return GOAL_ERR_NULL_POINTER;
    }

    /* check that all function ptr are not NULL */
    if ((NULL == fOpen) || (NULL == fClose) || (NULL == fGetVal) || (NULL == fConfGet) || (NULL == fConfSet) || (NULL == pDrvData)) {
        goal_logErr("MA Temperature drv has one or more function not defined");
        return GOAL_ERR_NULL_POINTER;
    }

    /* validate that the ID was not registered before */
    res = goal_maTemperatureGetById(&pMaValid, id);
    if (GOAL_RES_OK(res)) {
        goal_logErr("TEMPERATURE MA with ID %"FMT_u32" is already in use", id);
        return res;
    }

    /* allocate memory for the handle */
    res = goal_memCalloc(ppTempHdl, sizeof(GOAL_MA_TEMPERATURE_T));
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Unable to allocate TEMPERATURE MA");
        return res;
    }

    /* initialize the structure items */
    (*ppTempHdl)->id = id;
    (*ppTempHdl)->pDrvData = pDrvData;
    (*ppTempHdl)->fctConfGet = fConfGet;
    (*ppTempHdl)->fctConfSet = fConfSet;
    (*ppTempHdl)->fctOpen = fOpen;
    (*ppTempHdl)->fctClose = fClose;
    (*ppTempHdl)->fctGetVal = fGetVal;

    /* attach MA temperature handle to MA TEMPERATURE list */
    GOAL_LL_APPEND(mpListTemperature, *ppTempHdl);

    /* Mark handle as used */
    (*ppTempHdl)->flgUsed = GOAL_TRUE;

    return GOAL_OK;
}


/****************************************************************************/
/** Set the MA Configuration
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maTemperatureDrvConfigurationSet(
    GOAL_MA_TEMPERATURE_T *pTemperatureHdl,     /**< [in] pointer to store temperature handler */
    void *pDrvConfig                            /**< [in] driver specific configuration */
)
{
    /* Check for Null Pointer */
    if (NULL == pTemperatureHdl || NULL == pDrvConfig) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* Check if there is a function */
    if (NULL == pTemperatureHdl->fctConfSet) {
        return GOAL_ERR_UNSUPPORTED;
    }

    /* Invoke close function */
    return pTemperatureHdl->fctConfSet(pTemperatureHdl, pDrvConfig);

}


/****************************************************************************/
/** Get the MA Configuration
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maTemperatureDrvConfigurationGet(
    GOAL_MA_TEMPERATURE_T *pTemperatureHdl,     /**< [in] pointer to store temperature handler */
    void *pDrvConfig                            /**< [out] driver specific configuration */
)
{
    /* Check for Null Pointer */
    if (NULL == pTemperatureHdl || NULL == pDrvConfig) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* Check if there is a function */
    if (NULL == pTemperatureHdl->fctConfGet) {
        return GOAL_ERR_UNSUPPORTED;
    }

    /* Invoke close function */
    return pTemperatureHdl->fctConfGet(pTemperatureHdl, pDrvConfig);

}


/****************************************************************************/
/** Get MA temperature by ID
 *
 * This function returns the ID associated MA.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maTemperatureGetById(
    GOAL_MA_TEMPERATURE_T **ppHdlMaTemperature, /**< [in/out] temperature handle ref ptr */
    uint32_t id                                 /**< MA id */
)
{
    GOAL_MA_TEMPERATURE_T *pListTemperatureIdx; /* Temperature list idx */

    /* validate the arguments */
    if (NULL == ppHdlMaTemperature) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* search the entry with the correct ID */
    GOAL_LL_FOREACH(mpListTemperature, pListTemperatureIdx) {
        if (pListTemperatureIdx->id == id) {
            *ppHdlMaTemperature = pListTemperatureIdx;
            return GOAL_OK;
        }
    }

    /* no entry was found */
    return GOAL_ERR_NOT_FOUND;
}


/****************************************************************************/
/** Opens an temperature channel instance
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maTemperatureOpen(
    GOAL_MA_TEMPERATURE_T *pTemperatureHdl      /**< [in] pointer to store temperature handler */
)
{
    /* Check if there is a Handle */
    if (NULL == pTemperatureHdl) {
       return GOAL_ERR_NULL_POINTER;
    }

    /* Check if there is a function */
    if (NULL == pTemperatureHdl->fctOpen) {
        return GOAL_ERR_UNSUPPORTED;
    }

    /* Invoke close function */
    return pTemperatureHdl->fctOpen(pTemperatureHdl);
}


/****************************************************************************/
/** Closes an temperature channel instance
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maTemperatureClose(
    GOAL_MA_TEMPERATURE_T *pTemperatureHdl      /**< [in] pointer to store temperature handler */
)
{
    /* Check if there is a Handle */
    if (NULL == pTemperatureHdl) {
       return GOAL_ERR_NULL_POINTER;
    }

    /* Check if there is a function */
    if (NULL == pTemperatureHdl->fctClose) {
        return GOAL_ERR_UNSUPPORTED;
    }

    /* Invoke close function */
    return pTemperatureHdl->fctClose(pTemperatureHdl);
}


/****************************************************************************/
/** Gets the value of a temperature channel instance
 *
 * @retval int32_t Temperature value
 */
GOAL_STATUS_T goal_maTemperatureValueGet(
    GOAL_MA_TEMPERATURE_T *pTemperatureHdl,     /**< [in] pointer to store temperature handler */
    int32_t *pTemperature                       /**< [out] Temperature value pointer */
)
{
    /* Check if there is a Handle */
    if (NULL == pTemperatureHdl) {
       return GOAL_ERR_NULL_POINTER;
    }

    /* Check if there is a function */
    if (NULL == pTemperatureHdl->fctGetVal) {
        return GOAL_ERR_UNSUPPORTED;
    }

    /* Invoke close function */
    return pTemperatureHdl->fctGetVal(pTemperatureHdl, pTemperature);
}
