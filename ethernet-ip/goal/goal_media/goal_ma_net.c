/** @file
 *
 * @brief Generic MA for NET
 *
 * @details
 * This module provides a MA for NET
 *
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

#define GOAL_ID GOAL_ID_MA_NET
#include "goal_includes.h"


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_MA_NET_T *pListNet = NULL;          /**< NET list */


/****************************************************************************/
/** Registers a NET MA compliant driver
 *
 * This function is called during board init to register available
 * NET drivers.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maNetDrvReg(
    uint32_t id,                                /**< ID */
    GOAL_MA_NET_T **ppHdlMaNet,                 /**< NET handle ref ptr */
    GOAL_MA_NET_OPEN fOpen,                     /**< open function */
    GOAL_MA_NET_IP_SET fSetIp,                  /**< ip set function */
    GOAL_MA_NET_IP_GET fGetIp,                  /**< ip Get function */
    GOAL_MA_NET_CMD fCmd,                       /**< net command function */
    GOAL_MA_NET_CLOSE fClose,                   /**< close function */
    void *pDrvData                              /**< driver specific data */
)
{
    GOAL_MA_NET_T *pElem;                       /* spi element */
    GOAL_MA_NET_T *pMaValid = NULL;             /* MA validation element */
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    /* validate that the ID was not registered before */
    res = goal_maNetGetById(&pMaValid, id);
    if (GOAL_RES_OK(res)) {
        goal_logErr("The NET MA with ID %"FMT_u32" is already in use", id);
        return GOAL_ERR_INIT;
    }

    res = goal_memCalloc(ppHdlMaNet, sizeof(GOAL_MA_NET_T));
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Unable to allocate NET MA");
        return res;
    }

    /* initialize the structure items */
    (*ppHdlMaNet)->id = id;
    (*ppHdlMaNet)->pDrvData = pDrvData;
    (*ppHdlMaNet)->fctOpen = fOpen;
    (*ppHdlMaNet)->fctSetIp = fSetIp;
    (*ppHdlMaNet)->fctGetIp = fGetIp;
    (*ppHdlMaNet)->fctCmd = fCmd;
    (*ppHdlMaNet)->fctClose = fClose;

    /* iterate to list end */
    if (NULL != pListNet) {
        for (pElem = pListNet; pElem->pNext; pElem = pElem->pNext);
        /* store NET handle */
        pElem->pNext = *ppHdlMaNet;
    } else {
        /* store NET handle */
        pListNet = *ppHdlMaNet;
    }

    return res;
}


/****************************************************************************/
/** Open a NET MA channel
 *
 * THis function invokes the open function of the registered driver.
 *
 * @retval GOAL_OK successful
 * @retval GOAL_ERR_NULL_POINTER driver handler was null
 * @retval other failed
 */
GOAL_STATUS_T goal_maNetOpen(
    GOAL_MA_NET_T *pNetHdl                      /**< NET MA Handle */
)
{
    GOAL_STATUS_T res;                          /* result */

    /* check pointer */
    if ((NULL == pNetHdl) || (NULL == pNetHdl->fctOpen)) {
        goal_logErr("no registered open function");
        return GOAL_ERR_NULL_POINTER;
    }

    /* check if already opened */
    if (GOAL_TRUE == pNetHdl->flgUsed) {
        return GOAL_OK;
    }

    /* open registered function */
    res = pNetHdl->fctOpen(pNetHdl);
    if (GOAL_RES_OK(res)) {
        pNetHdl->flgUsed = GOAL_TRUE;
    }

    return res;
}


/****************************************************************************/
/** Closes an NET channel instance
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maNetClose(
    GOAL_MA_NET_T *pNetHdl                      /**< pointer to store NET handler */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    if (NULL == pNetHdl) {
       res = GOAL_ERR_NULL_POINTER;
    }

    /* Invoke close function */
    if (GOAL_RES_OK(res)) {
        res = (pNetHdl)->fctClose(pNetHdl);
        pNetHdl->flgUsed = GOAL_FALSE;
    }

    return res;
}


/****************************************************************************/
/** Get MA NET by ID
 *
 * This function returns the ID associated MA.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maNetGetById(
    GOAL_MA_NET_T **ppHdlMaNet,                 /**< NET handle ref ptr */
    uint32_t id                                 /**< MA id */
)
{
    GOAL_MA_NET_T *pListNetIdx = NULL;          /* NET list idx */
    GOAL_STATUS_T res;                          /* result */

    res = GOAL_ERR_NOT_FOUND;

    /* validate the arguments */
    if (NULL == ppHdlMaNet) {
        goal_logErr("MA NET reference is NULL");
        return GOAL_ERR_NULL_POINTER;
    }

    /* search the entry with the correct ID */
    for (pListNetIdx = pListNet; (pListNetIdx); pListNetIdx = pListNetIdx->pNext) {
        if (pListNetIdx->id == id) {
            *ppHdlMaNet = pListNetIdx;
            return GOAL_OK;
        }
    }

    /* no entry was found */
    return res;
}


/****************************************************************************/
/** Set IP Address
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maNetIpSet(
    GOAL_MA_NET_T *pNetHdl,                     /**< pointer to store NET handler */
    uint32_t addrIp,                            /**< IP address */
    uint32_t addrMask,                          /**< subnet mask */
    uint32_t addrGw,                            /**< gateway */
    GOAL_BOOL_T flgTemp                         /**< temporary IP config flag */
)
{
    GOAL_STATUS_T res = GOAL_ERR_NULL_POINTER;  /* result */

    if ((NULL != pNetHdl) && (NULL != pNetHdl->fctSetIp)) {
        res = pNetHdl->fctSetIp(pNetHdl, addrIp, addrMask, addrGw, flgTemp);
    }

    return res;
}


/****************************************************************************/
/** Get IP Address
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maNetIpGet(
    GOAL_MA_NET_T *pNetHdl,                     /**< pointer to store NET handler */
    uint32_t *pAddrIp,                          /**< IP address */
    uint32_t *pAddrMask,                        /**< subnet mask */
    uint32_t *pAddrGw,                          /**< gateway */
    GOAL_BOOL_T *pFlgTemp                       /**< temporary IP config flag */
)
{
    GOAL_STATUS_T res = GOAL_ERR_NULL_POINTER;  /* result */

    if ((NULL != pNetHdl) && (NULL != pNetHdl->fctGetIp)) {
        res = pNetHdl->fctGetIp(pNetHdl, pAddrIp, pAddrMask, pAddrGw, pFlgTemp);
    }

    return res;
}


/****************************************************************************/
/** Execute a NET command
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maNetCmd(
    GOAL_MA_NET_T *pNetHdl,                     /**< pointer to store NET handler */
    GOAL_NET_CMD_T id,                          /**< command ID */
    GOAL_BOOL_T wrFlag,                         /**< write flag */
    void *pArg                                  /**< argument */
)
{
    GOAL_STATUS_T res = GOAL_ERR_NULL_POINTER;  /* result */

    if ((NULL != pNetHdl) && (NULL != pNetHdl->fctCmd)) {
        res = pNetHdl->fctCmd(pNetHdl, id, wrFlag, pArg);
    }

    return res;
}
