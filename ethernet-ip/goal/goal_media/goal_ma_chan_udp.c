/** @file
 *
 * @brief Generic MA for IP
 *
 * @details
 * This module provides a MA for IP
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

#define GOAL_ID GOAL_ID_MA_CHAN_UDP
#include "goal_includes.h"
#include "goal_media/goal_ma_chan_udp.h"


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_MA_CHAN_UDP_T *pListChanUdp = NULL; /**< CHAN_UDP list */


/****************************************************************************/
/** Registers a CHAN_UDP MA compliant driver
 *
 * This function is called during board init to register available
 * CHAN_UDP drivers.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maChanUdpDrvReg(
    uint32_t id,                                /**< ID */
    GOAL_MA_CHAN_UDP_T **ppHdlMaChanUdp,        /**< CHAN_UDP handle ref ptr */
    GOAL_MA_CHAN_UDP_NEW fChanNew,              /**< open CHAN_UDP channel function */
    GOAL_MA_CHAN_UDP_CLOSE fChanClose,          /**< close CHAN_UDP channel function */
    GOAL_MA_CHAN_UDP_SET_NONBLOCKING fChanSetNonBlocking, /**< set CHAN_UDP channel options function */
    GOAL_MA_CHAN_UDP_SET_BROADCAST fChanSetBroadcast, /**< set CHAN_UDP channel options function */
    GOAL_MA_CHAN_UDP_GET_REMOTE_ADDR fChanGetRemoteAddr, /**< CHAN_UDP channel get remote address function */
    GOAL_MA_CHAN_UDP_ACTIVATE fChanActivate,    /**< CHAN_UDP channel activate function */
    GOAL_MA_CHAN_UDP_SEND fChanSend,            /**< CHAN_UDP channel send function */
    void *pDrvData                              /**< driver specific data */
)
{
    GOAL_MA_CHAN_UDP_T *pElem;                  /* spi element */
    GOAL_MA_CHAN_UDP_T *pMaValid = NULL;        /* MA validation element */
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    /* validate that the ID was not registered before */
    res = goal_maChanUdpGetById(&pMaValid, id);
    if (GOAL_RES_OK(res)) {
        goal_logErr("The CHAN_UDP MA with ID %"FMT_u32" is already in use", id);
        return GOAL_ERR_INIT;
    }

    res = goal_memCalloc(ppHdlMaChanUdp, sizeof(GOAL_MA_CHAN_UDP_T));
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Unable to allocate CHAN_UDP MA");
        return res;
    }

    /* initialize the structure items */
    (*ppHdlMaChanUdp)->id = id;
    (*ppHdlMaChanUdp)->pDrvData = pDrvData;
    (*ppHdlMaChanUdp)->fctChanNew = fChanNew;
    (*ppHdlMaChanUdp)->fctChanClose = fChanClose;
    (*ppHdlMaChanUdp)->fctChanSetNonBlocking = fChanSetNonBlocking;
    (*ppHdlMaChanUdp)->fctChanSetBroadcast = fChanSetBroadcast;
    (*ppHdlMaChanUdp)->fctChanGetRemoteAddr = fChanGetRemoteAddr;
    (*ppHdlMaChanUdp)->fctChanActivate = fChanActivate;
    (*ppHdlMaChanUdp)->fctChanSend = fChanSend;

    /* iterate to list end */
    if (NULL != pListChanUdp) {
        for (pElem = pListChanUdp; pElem->pNext; pElem = pElem->pNext);
        /* store CHAN_UDP handle */
        pElem->pNext = *ppHdlMaChanUdp;
    } else {
        /* store CHAN_UDP handle */
        pListChanUdp = *ppHdlMaChanUdp;
    }

    res = goal_queuePoolBufsReq(GOAL_ID, GOAL_NETBUF_SIZE, GOAL_CONFIG_BUF_NUM, 0);

    return res;
}


/****************************************************************************/
/** Get MA CHAN_UDP by ID
 *
 * This function returns the ID associated MA.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maChanUdpGetById(
    GOAL_MA_CHAN_UDP_T **ppHdlMaChanUdp,        /**< CHAN_UDP handle ref ptr */
    uint32_t id                                 /**< MA id */
)
{
    GOAL_MA_CHAN_UDP_T *pListChanUdpIdx = NULL; /* CHAN_UDP list idx */
    GOAL_STATUS_T res;                          /* result */

    res = GOAL_ERR_NOT_FOUND;

    /* validate the arguments */
    if (NULL == ppHdlMaChanUdp) {
        goal_logErr("MA CHAN_UDP reference is NULL");
        return GOAL_ERR_NULL_POINTER;
    }

    /* search the entry with the correct ID */
    for (pListChanUdpIdx = pListChanUdp; (pListChanUdpIdx); pListChanUdpIdx = pListChanUdpIdx->pNext) {
        if (pListChanUdpIdx->id == id) {
            *ppHdlMaChanUdp = pListChanUdpIdx;
            return GOAL_OK;
        }
    }

    /* no entry was found */
    return res;
}


/****************************************************************************/
/** Open MA CHAN_UDP by ID
 *
 * This function returns the ID associated MA.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maChanUdpOpen(
    uint32_t id,                                /**< MA id */
    GOAL_MA_CHAN_UDP_T **ppHdlMaChanUdp         /**< CHAN_UDP handle ref ptr */
)
{
    return goal_maChanUdpGetById(ppHdlMaChanUdp, id);
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
GOAL_STATUS_T goal_maChanUdpNewNonBlockingActivate(
    GOAL_MA_CHAN_UDP_T *pChanUdpHdl,            /**< pointer to store CHAN_UDP handler */
    GOAL_NET_CHAN_T **ppChanHandle,             /**< pointer to channel handle */
    GOAL_NET_CHAN_T *pChanOut,                  /**< pointer to channel handle for output */
    GOAL_NET_ADDR_T *pAddr,                     /**< remote address */
    GOAL_NET_TYPE_T type,                       /**< connection type */
    GOAL_MA_CHAN_UDP_CB_T callback,             /**< channel callback */
    GOAL_BOOL_T flgOption,                      /**< flag for socket option */
    GOAL_BOOL_T optVal,                         /**< value of socket option */
    GOAL_BOOL_T flgImmediateActivation          /**< flag for immediate activation */
)
{
    GOAL_STATUS_T res = GOAL_ERR_NULL_POINTER;  /* result */

    if (NULL != pChanUdpHdl && NULL != pChanUdpHdl->fctChanNew) {
        res = pChanUdpHdl->fctChanNew(pChanUdpHdl, ppChanHandle, pChanOut, pAddr, type, callback, flgOption, optVal, flgImmediateActivation);
    }

    return res;
}


/****************************************************************************/
/** Close a network channel
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_maChanUdpClose(
    GOAL_MA_CHAN_UDP_T *pChanUdpHdl,            /**< pointer to store CHAN_UDP handler */
    GOAL_NET_CHAN_T *pChanHandle                /**< pointer to channel handle */
)
{
    GOAL_STATUS_T res = GOAL_ERR_NULL_POINTER;  /* result */

    if (NULL != pChanUdpHdl && NULL != pChanUdpHdl->fctChanClose) {
        res = pChanUdpHdl->fctChanClose(pChanUdpHdl, pChanHandle);
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
GOAL_STATUS_T goal_maChanUdpSetNonBlocking(
    GOAL_MA_CHAN_UDP_T *pChanUdpHdl,            /**< pointer to store CHAN_UDP handler */
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_BOOL_T flgOption                       /**< option value */
)
{
    GOAL_STATUS_T res = GOAL_ERR_NULL_POINTER;  /* result */

    if (NULL != pChanUdpHdl && NULL != pChanUdpHdl->fctChanSetNonBlocking) {
        res = pChanUdpHdl->fctChanSetNonBlocking(pChanUdpHdl, pChanHandle, flgOption);
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
GOAL_STATUS_T goal_maChanUdpSetBroadcast(
    GOAL_MA_CHAN_UDP_T *pChanUdpHdl,            /**< pointer to store CHAN_UDP handler */
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_BOOL_T flgOption                       /**< option value */
)
{
    GOAL_STATUS_T res = GOAL_ERR_NULL_POINTER;  /* result */

    if (NULL != pChanUdpHdl && NULL != pChanUdpHdl->fctChanSetBroadcast) {
        res = pChanUdpHdl->fctChanSetBroadcast(pChanUdpHdl, pChanHandle, flgOption);
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
GOAL_STATUS_T goal_maChanUdpActivate(
    GOAL_MA_CHAN_UDP_T *pChanUdpHdl,            /**< pointer to store CHAN_UDP handler */
    GOAL_NET_CHAN_T *pChanHandle                /**< channel handle */
)
{
    GOAL_STATUS_T res = GOAL_ERR_NULL_POINTER;  /* result */

    if (NULL != pChanUdpHdl && NULL != pChanUdpHdl->fctChanActivate) {
        res = pChanUdpHdl->fctChanActivate(pChanUdpHdl, pChanHandle);
    }

    return res;
}


/****************************************************************************/
/** Get remote address of channel
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_maChanUdpGetRemoteAddr(
    GOAL_MA_CHAN_UDP_T *pChanUdpHdl,            /**< pointer to store CHAN_UDP handler */
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_NET_ADDR_T *pAddr                      /**< remote address */
)
{
    GOAL_STATUS_T res = GOAL_ERR_NULL_POINTER;  /* result */

    if (NULL != pChanUdpHdl && NULL != pChanUdpHdl->fctChanActivate) {
        res = pChanUdpHdl->fctChanGetRemoteAddr(pChanUdpHdl, pChanHandle, pAddr);
    }

    return res;
}


/****************************************************************************/
/** Send Data over ChanUdpwork Channel
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
GOAL_STATUS_T goal_maChanUdpSend(
    GOAL_MA_CHAN_UDP_T *pChanUdpHdl,            /**< pointer to store CHAN_UDP handler */
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_BUFFER_T *pBuf                         /**< buffer with data to send */
)
{
    GOAL_STATUS_T res = GOAL_ERR_NULL_POINTER;  /* result */

    if (NULL != pChanUdpHdl && NULL != pChanUdpHdl->fctChanSend) {
        res = pChanUdpHdl->fctChanSend(pChanUdpHdl, pChanHandle, pBuf);
    }

    return res;
}
