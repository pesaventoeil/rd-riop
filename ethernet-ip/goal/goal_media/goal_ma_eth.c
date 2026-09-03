/** @file
 *
 * @brief Generic Media Adapter for Ethernet
 *
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

#define GOAL_ID GOAL_ID_MA_ETH
#include "goal_includes.h"


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_INSTANCE_LIST_T *mpListInst;        /**< Eth list */


/****************************************************************************/
/** Registers a driver into Ethernet MA
 *
 * Description
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maEthDrvReg(
    uint32_t id,                                /**< ID */
    GOAL_MA_ETH_T **ppMaEthHdl,                 /**< [out] MA handle ref ptr */
    GOAL_MA_ETH_INIT_T fInit,                   /**< init function */
    GOAL_MA_ETH_OPEN_T fOpen,                   /**< open function */
    GOAL_MA_ETH_SEND_T fSend,                   /**< send function */
    GOAL_MA_ETH_CMD_T fCmd,                     /**< ethernet cmd function */
    void *pData                                 /**< [in] reserved data */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_INSTANCE_T *pElem = NULL;              /* inst */
    GOAL_MA_ETH_T *pMaValid = NULL;             /* MA validation element */

    /* id already in use? */
    res = goal_maEthGetById(&pMaValid, id);
    if (GOAL_RES_OK(res)) {
        goal_logErr("Ethernet MA with ID is already in use");
        return GOAL_ERR_INIT;
    }
    else {
        res = GOAL_OK;
    }

    /* create instance list */
    if (mpListInst == NULL) {
        res = goal_instListNew(&mpListInst, GOAL_ID);
    }

    /* create instance */
    if (GOAL_RES_OK(res)) {
        res = goal_instNew(&pElem,
                sizeof(GOAL_MA_ETH_T),
                GOAL_ID,
                id,
                "goal_ma_eth");
    }

    /* return handle */
    if (GOAL_RES_OK(res)) {
        *ppMaEthHdl = (GOAL_MA_ETH_T *) pElem;
        (*ppMaEthHdl)->fInit = fInit;
        (*ppMaEthHdl)->fOpen = fOpen;
        (*ppMaEthHdl)->fSend = fSend;
        (*ppMaEthHdl)->fCmd = fCmd;
        (*ppMaEthHdl)->pData = pData;
    }

    return res;
}


/****************************************************************************/
/** Initialize Driver
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maEthInit(
    GOAL_MA_ETH_T *pMaEthHdl                    /**< [in] pointer handler */
)
{
    /* check pointer */
    if ((NULL == pMaEthHdl) || (NULL == pMaEthHdl->fInit)) {
        goal_logErr("no registered init function");
        return GOAL_ERR_NULL_POINTER;
    }

    /* open registered function */
    pMaEthHdl->fInit(pMaEthHdl);
    return GOAL_OK;
}


/****************************************************************************/
/** Open Driver
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maEthOpen(
    GOAL_MA_ETH_T *pMaEthHdl                    /**< [in] pointer handler */
)
{
    GOAL_STATUS_T res;                          /* result */

    /* check pointer */
    if ((NULL == pMaEthHdl) || (NULL == pMaEthHdl->fOpen)) {
        goal_logErr("no registered open function");
        return GOAL_ERR_NULL_POINTER;
    }

    /* check if already opened */
    if (GOAL_TRUE == pMaEthHdl->flgOpen) {
        return GOAL_OK;
    }

    /* open registered function */
    res = pMaEthHdl->fOpen(pMaEthHdl);

    if (GOAL_RES_OK(res)) {
        pMaEthHdl->flgOpen = GOAL_TRUE;
    }

    return res;
}


/****************************************************************************/
/** Send through MA
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maEthSend(
    GOAL_MA_ETH_T *pMaEthHdl                    /**< [in] pointer handler */
)
{
    /* check pointer */
    if ((NULL == pMaEthHdl) || (NULL == pMaEthHdl->fSend)) {
        goal_logErr("no registered send function");
        return GOAL_ERR_NULL_POINTER;
    }

    /* send through registered function */
    pMaEthHdl->fSend(pMaEthHdl);
    return GOAL_OK;
}


/****************************************************************************/
/** Get Ethernet Features through MA
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maEthCmd(
    GOAL_MA_ETH_T *pMaEthHdl,                   /**< [in] pointer handler */
    uint32_t cmd,                               /**< command */
    GOAL_BOOL_T wrFlag,                         /**< write flag */
    uint32_t port,                              /**< port ID */
    void *pArg                                  /**< argument */
)
{
    /* check pointer */
    if ((NULL == pMaEthHdl) || (NULL == pMaEthHdl->fCmd)) {
        goal_logErr("no registered command function %"FMT_u32, cmd);
        return GOAL_ERR_NULL_POINTER;
    }

    /* get features through registered function */
    return pMaEthHdl->fCmd(pMaEthHdl, cmd, wrFlag, port, pArg);
}


/****************************************************************************/
/** Get MY by ID
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maEthGetById(
    GOAL_MA_ETH_T **ppMaEthHdl,                 /**< [in,out] handle ref ptr */
    uint32_t id                                 /**< MA id */
)
{
    return goal_instGetById((GOAL_INSTANCE_T **) ppMaEthHdl, GOAL_ID, id);
}


/****************************************************************************/
/** Get MAC out of MA
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maEthCfgMacAddrGet(
    GOAL_MA_ETH_T *pMaHdl,                      /**< [in,out] MAC ptr */
    uint8_t *pMacAddr                           /**< MA id */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    /* check MAC address pointer */
    if (NULL == pMacAddr) {
        res = GOAL_ERR_PARAM;
    }

    GOAL_MEMCPY(pMacAddr, pMaHdl->cfg.macAddr, MAC_ADDR_LEN);

    return res;
}


/****************************************************************************/
/** Register MI Callback Function into MA
 *
 * The registered MI functions are called by the driver.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maEthCbReg(
    GOAL_MA_ETH_T *pMaEthHdl,                   /**< [in] MA handle */
    GOAL_MA_ETH_CB_TYPE_T typeCb,               /**< cb type eg send, sendDone etc. */
    GOAL_MA_ETH_CB_T fCb                        /**< cb takes void pointer */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MA_ETH_CB_LIST_T *pCb;                 /* callback list */

    /* check handle */
    if (NULL == pMaEthHdl) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* allocate callback data */
    res = goal_memCalloc(&pCb, sizeof(GOAL_MA_ETH_CB_LIST_T));
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* register cb data */
    pCb->fCb = fCb;
    pCb->typeCb = typeCb;

    /* append cb on list in handle */
    GOAL_LL_APPEND(pMaEthHdl->pCbList, pCb);

    return GOAL_OK;
}


/****************************************************************************/
/** Inform MI about received Frames
 *
 */
void goal_maEthRecvCb(
    GOAL_MA_ETH_T *pMaEthHdl,                   /**< [in] MA handle */
    uint32_t portIdx,                           /**< port index */
    GOAL_BUFFER_T **ppBuf                       /**< [out] frame containing buffer */
)
{
    GOAL_MA_ETH_CB_LIST_T *pList;               /* cb list */
    GOAL_MA_ETH_CB_LIST_T *pEntry;              /* cb list entry */

    /* check handle */
    if (NULL == pMaEthHdl) {
        return;
    }

    /* get List */
    pList = pMaEthHdl->pCbList;

    /* searching for cb */
    GOAL_LL_FOREACH(pList, pEntry) {
        if (pEntry->typeCb == GOAL_MA_ETH_RECV) {
            pEntry->fCb.fCbRecv(pMaEthHdl, portIdx, ppBuf);
        }
    }
}


/****************************************************************************/
/** Inform MI about needed Send Frame
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maEthSendFrameGetCb(
    GOAL_MA_ETH_T *pMaEthHdl,                   /**< [in] MA handle */
    GOAL_BUFFER_T **ppBuf                       /**< [out] buffer ref */
)
{
    GOAL_MA_ETH_CB_LIST_T *pList;               /* cb list */
    GOAL_MA_ETH_CB_LIST_T *pEntry;              /* cb list entry */

    /* check handle */
    if (NULL == pMaEthHdl) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* get List */
    pList = pMaEthHdl->pCbList;

    /* searching for cb */
    GOAL_LL_FOREACH(pList, pEntry) {
        if (pEntry->typeCb == GOAL_MA_ETH_SEND_FRAME_GET) {
            pEntry->fCb.fCbSendFrameGet(pMaEthHdl, ppBuf);
        }
    }

    if (NULL == *ppBuf) {
        return GOAL_ERR_NULL_POINTER;
    }
    else {
        return GOAL_OK;
    }
}


/****************************************************************************/
/** Inform MI about sended Frame
 *
 */
void goal_maEthSendDoneCb(
    GOAL_MA_ETH_T *pMaEthHdl,                   /**< [in] MA handle */
    GOAL_BUFFER_T **ppBuf,                      /**< [out] buffer ref */
    GOAL_BOOL_T doneFlag                        /**< succesfully send? */
)
{
    GOAL_MA_ETH_CB_LIST_T *pList;               /* cb list */
    GOAL_MA_ETH_CB_LIST_T *pEntry;              /* cb list entry */

    /* check handle */
    if (NULL == pMaEthHdl) {
        return;
    }

    /* get List */
    pList = pMaEthHdl->pCbList;

    /* searching for cb */
    GOAL_LL_FOREACH(pList, pEntry) {
        if (pEntry->typeCb == GOAL_MA_ETH_SEND_DONE) {
            pEntry->fCb.fCbSendDone(pMaEthHdl, ppBuf, doneFlag);
        }
    }
}
