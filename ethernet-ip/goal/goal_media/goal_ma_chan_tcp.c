/** @file
 *
 * @brief Generic MA for TCP cannel
 *
 * @details
 * This module provides a MA for a TCP channel
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

#define GOAL_ID GOAL_ID_MA_CHAN_TCP
#include "goal_includes.h"
#include "goal_media/goal_ma_chan_tcp.h"


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_MA_CHAN_TCP_T *pListChanTcp = NULL; /**< CHAN_TCP list */


/****************************************************************************/
/** Registers a CHAN_TCP MA compliant driver
 *
 * This function is called during board init to register available
 * CHAN_TCP drivers.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maChanTcpDrvReg(
    uint32_t id,                                /**< ID */
    GOAL_MA_CHAN_TCP_T **ppHdlMaChanTcp,        /**< CHAN_TCP handle ref ptr */
    GOAL_MA_CHAN_TCP_NEW fChanNew,              /**< open CHAN_TCP channel function */
    GOAL_MA_CHAN_TCP_CLOSE fChanClose,          /**< close CHAN_TCP channel function */
    GOAL_MA_CHAN_TCP_SET_NONBLOCKING fChanSetNonBlocking, /**< set CHAN_TCP channel options function */
    GOAL_MA_CHAN_TCP_GET_REMOTE_ADDR fChanGetRemoteAddr, /**< CHAN_TCP channel get remote address function */
    GOAL_MA_CHAN_TCP_ACTIVATE fChanActivate,    /**< CHAN_TCP channel activate function */
    GOAL_MA_CHAN_TCP_SEND fChanSend,            /**< CHAN_TCP channel send function */
    void *pDrvData                              /**< driver specific data */
)
{
    GOAL_MA_CHAN_TCP_T *pElem;                  /* spi element */
    GOAL_MA_CHAN_TCP_T *pMaValid = NULL;        /* MA validation element */
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    /* validate that the ID was not registered before */
    res = goal_maChanTcpGetById(&pMaValid, id);
    if (GOAL_RES_OK(res)) {
        goal_logErr("The CHAN_TCP MA with ID %"FMT_u32" is already in use", id);
        return GOAL_ERR_INIT;
    }

    res = goal_memCalloc(ppHdlMaChanTcp, sizeof(GOAL_MA_CHAN_TCP_T));
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Unable to allocate CHAN_TCP MA");
        return res;
    }

    /* initialize the structure items */
    (*ppHdlMaChanTcp)->id = id;
    (*ppHdlMaChanTcp)->pDrvData = pDrvData;
    (*ppHdlMaChanTcp)->fctChanNew = fChanNew;
    (*ppHdlMaChanTcp)->fctChanClose = fChanClose;
    (*ppHdlMaChanTcp)->fctChanSetNonBlocking = fChanSetNonBlocking;
    (*ppHdlMaChanTcp)->fctChanGetRemoteAddr = fChanGetRemoteAddr;
    (*ppHdlMaChanTcp)->fctChanActivate = fChanActivate;
    (*ppHdlMaChanTcp)->fctChanSend = fChanSend;

    /* iterate to list end */
    if (NULL != pListChanTcp) {
        for (pElem = pListChanTcp; pElem->pNext; pElem = pElem->pNext);
        /* store CHAN_TCP handle */
        pElem->pNext = *ppHdlMaChanTcp;
    } else {
        /* store CHAN_TCP handle */
        pListChanTcp = *ppHdlMaChanTcp;
    }

    res = goal_queuePoolBufsReq(GOAL_ID, GOAL_NETBUF_SIZE, GOAL_CONFIG_BUF_NUM, 0);

    return res;
}


/****************************************************************************/
/** Get MA CHAN_TCP by ID
 *
 * This function returns the ID associated MA.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maChanTcpGetById(
    GOAL_MA_CHAN_TCP_T **ppHdlMaChanTcp,        /**< CHAN_TCP handle ref ptr */
    uint32_t id                                 /**< MA id */
)
{
    GOAL_MA_CHAN_TCP_T *pListChanTcpIdx = NULL; /* CHAN_TCP list idx */
    GOAL_STATUS_T res;                          /* result */

    res = GOAL_ERR_NOT_FOUND;

    /* validate the arguments */
    if (NULL == ppHdlMaChanTcp) {
        goal_logErr("MA CHAN_TCP reference is NULL");
        return GOAL_ERR_NULL_POINTER;
    }

    /* search the entry with the correct ID */
    for (pListChanTcpIdx = pListChanTcp; (pListChanTcpIdx); pListChanTcpIdx = pListChanTcpIdx->pNext) {
        if (pListChanTcpIdx->id == id) {
            *ppHdlMaChanTcp = pListChanTcpIdx;
            return GOAL_OK;
        }
    }

    /* no entry was found */
    return res;
}


/****************************************************************************/
/** Open MA CHAN_TCP by ID
 *
 * This function returns the ID associated MA.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maChanTcpOpen(
    uint32_t id,                                /**< MA id */
    GOAL_MA_CHAN_TCP_T **ppHdlMaChanTcp         /**< CHAN_TCP handle ref ptr */
)
{
    return goal_maChanTcpGetById(ppHdlMaChanTcp, id);
}


/****************************************************************************/
/** Open a network channel
 *
 * Open an network channel on the specified port.
 * The new channel is bound to pAddr->localIp and pAddr->localPort.
 * If a TCP client connection is opened the channel will also be connected to
 * pAddr->remoteIp and pAddr->remotePort.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_maChanTcpNewNonBlockingActivate(
    GOAL_MA_CHAN_TCP_T *pChanTcpHdl,            /**< pointer to store CHAN_TCP handler */
    GOAL_NET_CHAN_T **ppChanHandle,             /**< pointer to channel handle */
    GOAL_NET_CHAN_T *pChanOut,                  /**< pointer to channel handle for output */
    GOAL_NET_ADDR_T *pAddr,                     /**< remote address */
    GOAL_NET_TYPE_T type,                       /**< connection type */
    GOAL_MA_CHAN_TCP_CB_T callback,             /**< channel callback */
    GOAL_BOOL_T flgOption,                      /**< flag for socket option */
    GOAL_BOOL_T optVal,                         /**< value of socket option */
    GOAL_BOOL_T flgImmediateActivation          /**< flag for immediate activation */
)
{
    GOAL_STATUS_T res = GOAL_ERR_NULL_POINTER;  /* result */

    if (NULL != pChanTcpHdl && NULL != pChanTcpHdl->fctChanNew) {
        res = pChanTcpHdl->fctChanNew(pChanTcpHdl, ppChanHandle, pChanOut, pAddr, type, callback, flgOption, optVal, flgImmediateActivation);
    }

    return res;
}


/****************************************************************************/
/** Close a network channel
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_maChanTcpClose(
    GOAL_MA_CHAN_TCP_T *pChanTcpHdl,            /**< pointer to store CHAN_TCP handler */
    GOAL_NET_CHAN_T *pChanHandle                /**< pointer to channel handle */
)
{
    GOAL_STATUS_T res = GOAL_ERR_NULL_POINTER;  /* result */

    if (NULL != pChanTcpHdl && NULL != pChanTcpHdl->fctChanClose) {
        res = pChanTcpHdl->fctChanClose(pChanTcpHdl, pChanHandle);
    }

    return res;
}


/****************************************************************************/
/** Change the property of a network channel
 *
 * This function will change the specified property of a net socket by calling
 * a target specific function.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_maChanTcpSetNonBlocking(
    GOAL_MA_CHAN_TCP_T *pChanTcpHdl,            /**< pointer to store CHAN_TCP handler */
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_BOOL_T flgOption                       /**< non blocking state */
)
{
    GOAL_STATUS_T res = GOAL_ERR_NULL_POINTER;  /* result */

    if (NULL != pChanTcpHdl && NULL != pChanTcpHdl->fctChanSetNonBlocking) {
        res = pChanTcpHdl->fctChanSetNonBlocking(pChanTcpHdl, pChanHandle, flgOption);
    }

    return res;
}


/****************************************************************************/
/** Activate channel
 *
 * Only activated channels can send and receive data.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_maChanTcpActivate(
    GOAL_MA_CHAN_TCP_T *pChanTcpHdl,            /**< pointer to store CHAN_TCP handler */
    GOAL_NET_CHAN_T *pChanHandle                /**< channel handle */
)
{
    GOAL_STATUS_T res = GOAL_ERR_NULL_POINTER;  /* result */

    if (NULL != pChanTcpHdl && NULL != pChanTcpHdl->fctChanActivate) {
        res = pChanTcpHdl->fctChanActivate(pChanTcpHdl, pChanHandle);
    }

    return res;
}


/****************************************************************************/
/** Reopen network channel
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_maChanTcpReopen(
    struct GOAL_MA_CHAN_TCP_T *pChanTcpHdl,     /**< pointer to store CHAN_TCP handler */
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_BOOL_T immediateActivation             /**< flag for immediate activation */
)
{
    GOAL_STATUS_T res = GOAL_ERR_NULL_POINTER;  /* result */

    if (NULL != pChanTcpHdl && NULL != pChanTcpHdl->fctChanNew) {
        res = pChanTcpHdl->fctChanNew(pChanTcpHdl, &pChanHandle, NULL, &pChanHandle->addr, pChanHandle->type, NULL, GOAL_FALSE, GOAL_FALSE, immediateActivation);
    }

    return res;
}


/****************************************************************************/
/** Get remote address of channel
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_maChanTcpGetRemoteAddr(
    GOAL_MA_CHAN_TCP_T *pChanTcpHdl,            /**< pointer to store CHAN_TCP handler */
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_NET_ADDR_T *pAddr                      /**< remote address */
)
{
    GOAL_STATUS_T res = GOAL_ERR_NULL_POINTER;  /* result */

    if (NULL != pChanTcpHdl && NULL != pChanTcpHdl->fctChanActivate) {
        res = pChanTcpHdl->fctChanGetRemoteAddr(pChanTcpHdl, pChanHandle, pAddr);
    }

    return res;
}


/****************************************************************************/
/** Send Data over ChanTcpwork Channel
 *
 * Send data in pBuf with bufLen size over the network channel.
 *
 * If destination again is a GOAL channel, its sending function is used or
 * - if no one is defined - its directly forwarded to next channel.
 *
 * Otherwise pBuf is forwarded to target.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_maChanTcpSend(
    GOAL_MA_CHAN_TCP_T *pChanTcpHdl,            /**< pointer to store CHAN_TCP handler */
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_BUFFER_T *pBuf                         /**< buffer with data to send */
)
{
    GOAL_STATUS_T res = GOAL_ERR_NULL_POINTER;  /* result */

    if (NULL != pChanTcpHdl && NULL != pChanTcpHdl->fctChanSend) {
        res = pChanTcpHdl->fctChanSend(pChanTcpHdl, pChanHandle, pBuf);
    }

    return res;
}
