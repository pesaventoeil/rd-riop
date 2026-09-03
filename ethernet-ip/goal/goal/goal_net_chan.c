/** @file
 *
 * @brief GOAL Network Channel Handling
 *
 * This module implements the interface for managing TCP and UDP connections.
 *
 * @copyright
 * Copyright 2010-2025.
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

#define GOAL_ID GOAL_ID_NET
#include <goal_includes.h>

#if GOAL_CONFIG_TCPIP_STACK == 1


/****************************************************************************/
/* local variables */
/****************************************************************************/
static GOAL_NET_CHAN_T *m_pNetChannelList = NULL; /**< List of allocated net channels */
static GOAL_STAGE_HANDLER_T stageInit;          /**< init stage handler */


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/
static GOAL_STATUS_T goal_netChanInit(
    void
);

static GOAL_STATUS_T goal_netChanSendInternal(
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_BUFFER_T *pBuf,                        /**< buffer with data to send */
    GOAL_BOOL_T fromTunnel                      /**< buffer was sent from tunnel */
);

static void goal_netChanLoop(
    void
);


/****************************************************************************/
/** Register stage handler
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_netChanInitPre(
    void
)
{
    return goal_mainStageReg(GOAL_STAGE_NET, &stageInit, GOAL_STAGE_INIT, goal_netChanInit);
}


/****************************************************************************/
/** Initialize Network Channel Handling
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
static GOAL_STATUS_T goal_netChanInit(
    void
)
{
    return goal_mainLoopReg(goal_netChanLoop);
}


/****************************************************************************/
/** Create a Network channel
 *
 * If @em type is @em GOAL_NET_TCP_LISTENER, the function will additionally
 * allocate @em numDataChannels data channels.
 *
 * For other channel types, the parameter @em numDataChannels is ignored.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_netChanCreate(
    GOAL_NET_CHAN_T **ppChan,                   /**< [out] net channel reference */
    GOAL_NET_TYPE_T type,                       /**< channel type */
    uint32_t numDataChannels                    /**< number of data channels for TCP Listener */
)
{
    uint32_t cnt;                               /* loop counter */
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_NET_CHAN_T *pNetChan;                  /* current net channel */

    if (ppChan == NULL) {
        return GOAL_ERR_PARAM;
    }

    switch (type) {
        case GOAL_NET_UDP_SERVER:
            break;

        case GOAL_NET_UDP_CLIENT:
            break;

#if GOAL_CONFIG_TCPIP_TCP == 1
        case GOAL_NET_TCP_LISTENER:
            break;

        case GOAL_NET_TCP:
            break;

        case GOAL_NET_TCP_CLIENT:
            break;
#endif /* GOAL_CONFIG_TCPIP_TCP */

        default:
            return GOAL_ERR_PARAM;
    }

#if GOAL_CONFIG_CTC_NET_CC == 0
    if ((GOAL_NET_TCP_LISTENER == type) && (0 == numDataChannels)) {
        return GOAL_ERR_PARAM;
    }
#endif /* GOAL_CONFIG_CTC_NET_CC */

    /* allocate specified number of GOAL channels and NET target data */
    pNetChan = NULL;
    res = goal_memCalloc(&pNetChan, sizeof(GOAL_NET_CHAN_T));
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:512: */
        goal_lmLog(GOAL_ID_NET, 512, 426, 0, 0, GOAL_LOG_SEV_ERROR, "failed to allocate net channel");
    }

    if (GOAL_RES_OK(res)) {
        res = goal_targetNetChanCreate(pNetChan, type);
        if (GOAL_RES_ERR(res)) {
            /* GG_LOG ID:513: */
            goal_lmLog(GOAL_ID_NET, 513, 427, 0, 0, GOAL_LOG_SEV_ERROR, "allocation of target net channels failed");
        }
    }

    if (GOAL_RES_OK(res)) {
        pNetChan->type = type;
        pNetChan->pNext = m_pNetChannelList;
        m_pNetChannelList = pNetChan;
        *ppChan = pNetChan;
    }

    if (GOAL_RES_OK(res) && (GOAL_NET_TCP_LISTENER == type)) {
        /* TCP listener: allocate specified number of data channels */
        pNetChan->numDataChannels = numDataChannels;
        for (cnt = 0; (cnt < numDataChannels) && GOAL_RES_OK(res); cnt++) {
            pNetChan = NULL;
            res = goal_netChanCreate(&pNetChan, GOAL_NET_TCP, 0);
            /* data channels will be opened by the TCP/IP stack, so the caller does not need to know them */
            pNetChan->pChanListener = *ppChan;
        }
    }

    return res;
}


/****************************************************************************/
/** Delete Network channel
 *
 * If the channel is a TCP listener, its associated data channels will be
 * deleted, too.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_netChanDelete(
    GOAL_NET_CHAN_T *pChan                      /**< channel handle */
)
{
    GOAL_NET_CHAN_T *pChanIter;                 /* data channel iterator */
    GOAL_NET_CHAN_T *pChanNext;                 /* next data channel */
    GOAL_NET_CHAN_T *pChanPrev = NULL;          /* previous data channel */

    if (NULL == pChan) {
        return GOAL_ERR_PARAM;
    }

    if (GOAL_NET_TCP_LISTENER == pChan->type) {
        pChanIter = m_pNetChannelList;

        while (pChanIter) {
            if (pChanIter->pChanListener == pChan) {
                /* this channel belongs to listener, delete it */
                pChanNext = pChanIter->pNext;
                goal_netChanDelete(pChanIter);
                pChanIter = pChanNext;
            }
            else {
                /* try next */
                pChanIter = pChanIter->pNext;
            }
        }
    }

    /* make sure channel is closed */
    (void) goal_netChanClose(pChan);

    /* remove channel from list */
    pChanIter = m_pNetChannelList;

    while (pChanIter) {
        if (pChanIter == pChan) {
            /* this channel shall be removed */
            if (NULL == pChanPrev) {
                m_pNetChannelList = pChanIter->pNext;
            }
            else {
                pChanPrev->pNext = pChanIter->pNext;
            }
            break;
        }
        else {
            /* try next */
            pChanPrev = pChanIter;
            pChanIter = pChanIter->pNext;
        }
    }

    (void) goal_targetNetChanDelete(pChan);

    goal_memFree(&pChan);

    return GOAL_OK;
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
GOAL_STATUS_T goal_netChanOpen(
    GOAL_NET_CHAN_T *pChan,                     /**< channel handle */
    GOAL_NET_ADDR_T *pAddr,                     /**< remote address */
    GOAL_NET_CB_T callback,                     /**< channel callback */
    void *pArg                                  /**< callback argument */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_BOOL_T flgDynPort = GOAL_FALSE;        /* dynamic port flag */

    /* check channel handle reference */
    if ((NULL == pChan) || (NULL == pAddr)) {
        return GOAL_ERR_PARAM;
    }

    if (pChan->flags & GOAL_NET_FLAGS_USED) {
        return GOAL_ERR_WRONG_STATE;
    }

    switch (pChan->type) {
        case GOAL_NET_UDP_SERVER:
            break;

        case GOAL_NET_UDP_CLIENT:
            break;

#if GOAL_CONFIG_TCPIP_TCP == 1
        case GOAL_NET_TCP_LISTENER:
            break;

        case GOAL_NET_TCP:
            break;

        case GOAL_NET_TCP_CLIENT:
            break;
#endif /* GOAL_CONFIG_TCPIP_TCP */

        default:
            return GOAL_ERR_PARAM;
    }

    /* set dynamic port flag if client local port is 0 */
    if (((GOAL_NET_TCP_CLIENT == pChan->type) || (GOAL_NET_UDP_CLIENT == pChan->type)) && (0 == pAddr->localPort)) {
        flgDynPort = GOAL_TRUE;
    }

    GOAL_MEMCPY(&pChan->addr, pAddr, sizeof(GOAL_NET_ADDR_T));
    pChan->callback = callback;
    pChan->pArg = pArg;

    /* open channel */
    res = goal_targetNetChanOpen(pChan, GOAL_FALSE);
    if (GOAL_RES_ERR(res)) {

        /* free channel */
        pChan->flags = 0;

        /* GG_LOG ID:163: */
        goal_lmLog(GOAL_ID_NET, 163, 132, 0, 0, GOAL_LOG_SEV_ERROR, "opening channel failed");
        return res;
    }

    pChan->flags |= GOAL_NET_FLAGS_USED;

    if (GOAL_TRUE == flgDynPort) {
        /* mark local port as dynamically assigned */
        pChan->flags |= GOAL_NET_FLAGS_DYN_PORT;
    }
    else {
        pChan->flags &= ~GOAL_NET_FLAGS_DYN_PORT;
    }

    if ((GOAL_NET_TCP_CLIENT == pChan->type) || (GOAL_NET_TCP_LISTENER == pChan->type)) {
        pChan->flags |= GOAL_NET_FLAGS_ACTIVE;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Enable tunneling of a GOAL NET channel
 *
 * This function will register a tunnel callback. The caller must take the
 * application callback and is responsible for relaying appropriate events to
 * application.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_netChanTunnelingEnable(
    GOAL_NET_CHAN_T *pChanHandle,               /**< tunnel channel handle */
    GOAL_NET_CB_T tunnelCallback,               /**< [in] tunnel callback */
    void *pTunnelCbArg,                         /**< [in] tunnel callback argument */
    GOAL_NET_CB_T *pApplCb,                     /**< [out] application callback */
    void **ppApplCbArg                          /**< [out] application callback argument */
)
{
    if ((NULL == pChanHandle) || (NULL == pApplCb) || (NULL == ppApplCbArg)) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* check for valid callback */
    if (tunnelCallback == NULL) {
        /* GG_LOG ID:164: */
        goal_lmLog(GOAL_ID_NET, 164, 133, 0, 0, GOAL_LOG_SEV_ERROR, "No callback is given");
        return GOAL_ERR_NET_OPEN;
    }

    *pApplCb = pChanHandle->callback;
    *ppApplCbArg = pChanHandle->pArg;
    pChanHandle->callback = tunnelCallback;
    pChanHandle->pArg = pTunnelCbArg;
    pChanHandle->flags |= GOAL_NET_FLAGS_TUNNELING;

    return GOAL_OK;
}


/****************************************************************************/
/** GOAL net Option - manual closure after peer close
 *
 * This function sets a flag indicating if GOAL shall close a net channel
 * automatically if the it is closed by the peer or if it shall be closed
 * manually. This allows to delay the channel close process
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_netChanCloseManSet(
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle structure */
    GOAL_BOOL_T flgCloseMan                     /**< manually close flag if */
)
{
    /* check NULL pointer */
    if (NULL == pChanHandle) {
        return GOAL_ERROR;
    }

    if (GOAL_TRUE == flgCloseMan) {
        pChanHandle->flags |= GOAL_NET_FLAGS_CLOSE_MAN;
    }
    else {
        pChanHandle->flags &= ~GOAL_NET_FLAGS_CLOSE_MAN;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Find a free Data Channel belonging to a Listener
 *
 * @retval GOAL_OK success
 * @retval other fail
 */

GOAL_STATUS_T goal_netChanFreeDataChanGet(
    GOAL_NET_CHAN_T *pListener,                 /**< listener handle */
    GOAL_NET_CHAN_T **ppDataChan                /**< [out] data channel */
)
{
    GOAL_STATUS_T res = GOAL_ERROR;             /* result */
    GOAL_NET_CHAN_T *pChan = NULL;              /* new data channel */

    /* check if channel exists */
    if (pListener == NULL) {
        /* GG_LOG ID:166: */
        goal_lmLog(GOAL_ID_NET, 166, 134, 0, 0, GOAL_LOG_SEV_ERROR, "Chanel handle is not valid");
        return GOAL_ERR_NULL_POINTER;
    }

    if (NULL == ppDataChan) {
        return GOAL_ERR_NULL_POINTER;
    }

    GOAL_LL_FOREACH(m_pNetChannelList, pChan) {
        if (pChan->pChanListener != pListener) {
            continue;
        }

        if (0 == (pChan->flags & (GOAL_NET_FLAGS_USED | GOAL_NET_FLAGS_ACTIVE))) {
            /* channel is free, use it */
            pChan->flags |= GOAL_NET_FLAGS_USED;
            pChan->callback = pListener->callback;
            pChan->pArg = pListener->pArg;
            if (pListener->flags & GOAL_NET_FLAGS_TUNNELING) {
                pChan->flags |= GOAL_NET_FLAGS_TUNNELING;
            }
            *ppDataChan = pChan;
            res = GOAL_OK;
            break;
        }
    }

    return res;
}


/****************************************************************************/
/** Open a network channel from target
 *
 * Open a data channel.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_netChanOpenTgt(
    GOAL_NET_CHAN_T *pChan,                     /**< data channel handle */
    GOAL_NET_ADDR_T *pAddr                      /**< remote address */
)
{
    GOAL_STATUS_T res = GOAL_ERROR;             /* result */

    if ((NULL == pChan) || (NULL == pAddr)) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* update channel data */
    GOAL_MEMCPY(&pChan->addr, pAddr, sizeof(GOAL_NET_ADDR_T));
    pChan->flags |= GOAL_NET_FLAGS_ACTIVE;

    if (pChan->callback) {
        res = pChan->callback(pChan->pArg, GOAL_NET_CB_NEW_SOCKET, pChan, NULL);
    }

    if (GOAL_RES_ERR(res)) {
        pChan->flags &= ~(GOAL_NET_FLAGS_USED | GOAL_NET_FLAGS_ACTIVE | GOAL_NET_FLAGS_TUNNELING);
    }

    return res;
}


/****************************************************************************/
/** TCP client channel connected to server
 *
 * This function is called by the net driver if a TCP client socket connected to
 * its remote endpoint.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_netChanConnectedTgt(
    GOAL_NET_CHAN_T *pChanHandle                /**< channel handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    if (NULL == pChanHandle) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* check if channel is in use */
    if (0 == (pChanHandle->flags & GOAL_NET_FLAGS_USED)) {
        return GOAL_ERR_NET_CLOSE;
    }

    /* check if channel is active */
    if (0 == (pChanHandle->flags & GOAL_NET_FLAGS_ACTIVE)) {
        return GOAL_ERR_NET_CLOSE;
    }

    /* check if this is a TCP client socket */
    if (GOAL_NET_TCP_CLIENT != pChanHandle->type) {
        return GOAL_ERR_NET_CLOSE;
    }

    if (pChanHandle->callback) {
        (void) pChanHandle->callback(pChanHandle->pArg, GOAL_NET_CB_CONNECTED, pChanHandle, NULL);
    }

    return res;
}


/****************************************************************************/
/** Close a network channel
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_netChanClose(
    GOAL_NET_CHAN_T *pChanHandle                /**< channel handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    if (NULL == pChanHandle) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* check if channel is in use */
    if (0 == (pChanHandle->flags & GOAL_NET_FLAGS_USED)) {
        return GOAL_ERR_NET_CLOSE;
    }

    if (pChanHandle->flags & GOAL_NET_FLAGS_TUNNELING) {
        /* the tunnel handler must close the channel */
        pChanHandle->flags &= ~(GOAL_NET_FLAGS_TUNNELING | GOAL_NET_FLAGS_ACTIVE);
        (void) pChanHandle->callback(pChanHandle->pArg, GOAL_NET_CB_CLOSING_BY_APP, pChanHandle, NULL);
    }
    else {
        /* deactivate channel before closing it */
        goal_targetNetChanDeactivate(pChanHandle);

        /* close channel in target */
        res = goal_targetNetChanClose(pChanHandle);

        /* deactivate GOAL channel */
        pChanHandle->flags &= ~(GOAL_NET_FLAGS_USED | GOAL_NET_FLAGS_ACTIVE | GOAL_NET_FLAGS_DYN_PORT | GOAL_NET_FLAGS_CLOSE_MAN);
    }

    return res;
}


/****************************************************************************/
/** Close a network channel from target
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_netChanCloseTgt(
    GOAL_NET_CHAN_T *pChanHandle                /**< channel handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    if (NULL == pChanHandle) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* check if channel is in use */
    if (0 == (pChanHandle->flags & GOAL_NET_FLAGS_USED)) {
        return GOAL_ERR_NET_CLOSE;
    }

    if (0 == (pChanHandle->flags & GOAL_NET_FLAGS_CLOSE_MAN)) {
        /* deactivate channel before closing it */
        res = goal_targetNetChanDeactivate(pChanHandle);
        if (GOAL_RES_ERR(res)) {
            /* GG_LOG ID:170: */
            goal_lmLog(GOAL_ID_NET, 170, 138, 0, 0, GOAL_LOG_SEV_ERROR, "Could not deactivate target channel handle.");
        }

        /* close target channel from our side */
        res = goal_targetNetChanClose(pChanHandle);
        if (GOAL_RES_ERR(res)) {
            /* GG_LOG ID:169: */
            goal_lmLog(GOAL_ID_NET, 169, 137, 0, 0, GOAL_LOG_SEV_ERROR, "Could not close target channel handle");
        }
    }
    else {
        /* closure is called manually */
        res = GOAL_OK_DELAYED;
    }

    /* inform owner about channel close */
    if (pChanHandle->callback) {
        (void) pChanHandle->callback(pChanHandle->pArg, GOAL_NET_CB_CLOSING, pChanHandle, NULL);
    }

    /* deactivate and close GOAL channel */
    if (0 == (pChanHandle->flags & GOAL_NET_FLAGS_CLOSE_MAN)) {
        pChanHandle->flags &= ~(GOAL_NET_FLAGS_TUNNELING | GOAL_NET_FLAGS_ACTIVE | GOAL_NET_FLAGS_USED | GOAL_NET_FLAGS_DYN_PORT);
    }

    return res;
}


/****************************************************************************/
/** Reopen open network channel
 *
 * This function closes and opens a used NET channels.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_netChanReopen(
    GOAL_NET_CHAN_T *pChanHandle                /**< channel handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_BOOL_T isActive = GOAL_FALSE;          /* current channel is active */

    if (NULL == pChanHandle) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (GOAL_NET_TCP == pChanHandle->type) {
        res = GOAL_ERROR;
    }

    if (GOAL_RES_OK(res)) {
        if (pChanHandle->flags & GOAL_NET_FLAGS_DYN_PORT) {
            /* if local port was dynamically assigned, trigger new assignment */
            pChanHandle->addr.localPort = 0;
        }
        else if (((GOAL_NET_TCP_CLIENT == pChanHandle->type) || (GOAL_NET_UDP_CLIENT == pChanHandle->type)) && (0 == pChanHandle->addr.localPort)) {
            /* port was not dynamically assigned before, but is now */
            pChanHandle->flags |= GOAL_NET_FLAGS_DYN_PORT;
        }

        isActive = (pChanHandle->flags & GOAL_NET_FLAGS_ACTIVE) ? GOAL_TRUE : GOAL_FALSE;

        if (pChanHandle->flags & GOAL_NET_FLAGS_USED) {
            res = goal_targetNetChanClose(pChanHandle);
        }
    }

    if (GOAL_RES_OK(res)) {
        res = goal_targetNetChanOpen(pChanHandle, GOAL_TRUE);
    }

    if (GOAL_RES_OK(res)) {
        pChanHandle->flags |= GOAL_NET_FLAGS_USED;

        if (GOAL_TRUE == isActive) {
            /* reactivate reopened channel if it was active before */
            res = goal_netChanActivate(pChanHandle);
            if (GOAL_RES_ERR(res)) {
                /* GG_LOG ID:173: */
                goal_lmLog(GOAL_ID_NET, 173, 141, 0, 0, GOAL_LOG_SEV_ERROR, "Could not reactivate reopened channel");
            }
        }
    }

    if (GOAL_RES_ERR(res)) {

        /* disable channel */
        pChanHandle->flags &= ~GOAL_NET_FLAGS_ACTIVE;

        /* inform application about channel close */
        if (NULL != pChanHandle->callback) {
            res = pChanHandle->callback(pChanHandle->pArg, GOAL_NET_CB_CLOSING, pChanHandle, NULL);
            if (GOAL_RES_ERR(res)) {
                /* GG_LOG ID:171: */
                goal_lmLog(GOAL_ID_NET, 171, 139, 0, 0, GOAL_LOG_SEV_WARNING, "Could not close channel after failed reopen call");
            }
        }

        /* release channel */
        pChanHandle->flags &= ~GOAL_NET_FLAGS_USED;

        /* GG_LOG ID:172: */
        goal_lmLog(GOAL_ID_NET, 172, 140, 4, 0, GOAL_LOG_SEV_ERROR, "failed to reopen channel: $1");
        goal_lmLogParamUINT32((uint32_t) (uintptr_t) pChanHandle);
        goal_lmLogFinish();
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
GOAL_STATUS_T goal_netChanActivate(
    GOAL_NET_CHAN_T *pChanHandle                /**< channel handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    if (NULL == pChanHandle) {
        /* GG_LOG ID:175: */
        goal_lmLog(GOAL_ID_NET, 175, 143, 0, 0, GOAL_LOG_SEV_ERROR, "Invalid Channel given, no target channel found");
        return GOAL_ERR_PARAM;
    }

    /* check if channel is reserved */
    if (0 == (pChanHandle->flags & GOAL_NET_FLAGS_USED)) {
        /* GG_LOG ID:174: */
        goal_lmLog(GOAL_ID_NET, 174, 142, 0, 0, GOAL_LOG_SEV_ERROR, "Given channel isn't reserved.");
        return GOAL_ERR_NET_ACTIVATE;
    }

    /* enable target */
    res = goal_targetNetChanActivate(pChanHandle);
    if (GOAL_RES_OK(res)) {
        pChanHandle->flags |= GOAL_NET_FLAGS_ACTIVE;
    }

    return res;
}


/****************************************************************************/
/** Get the remote address for a open channel
 *
 * This function provides the remote address of an open channel.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_netChanRemoteAddrGet(
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_NET_ADDR_T *pAddr                      /**< pointer to store address */
)
{
    if (NULL == pAddr) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (NULL == pChanHandle) {
        /* GG_LOG ID:176: */
        goal_lmLog(GOAL_ID_NET, 176, 144, 0, 0, GOAL_LOG_SEV_ERROR, "Could not find target handle to given channel");
        return GOAL_ERR_NET_ADDRESS;
    }

    /* check if channel is active */
    if (0 == (pChanHandle->flags & GOAL_NET_FLAGS_ACTIVE)) {
        /* GG_LOG ID:177: */
        goal_lmLog(GOAL_ID_NET, 177, 145, 0, 0, GOAL_LOG_SEV_ERROR, "given channel isn't ready");
        return GOAL_ERR_NET_ADDRESS;
    }

    /* return remote address */
    GOAL_MEMCPY(pAddr, &pChanHandle->addr, sizeof(GOAL_NET_ADDR_T));

    return GOAL_OK;
}


/****************************************************************************/
/** Send Data from Network Tunnel
 *
 * Send data in pBuf with bufLen size over the network channel.
 *
 * This function is used by the channel tunnel to send data to the network.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_netChanTunnelSend(
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_BUFFER_T *pBuf                         /**< buffer with data to send */
)
{
    return goal_netChanSendInternal(pChanHandle, pBuf, GOAL_TRUE);
}


/****************************************************************************/
/** Send Data over Network Channel
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
GOAL_STATUS_T goal_netChanSend(
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_BUFFER_T *pBuf                         /**< buffer with data to send */
)
{
    return goal_netChanSendInternal(pChanHandle, pBuf, GOAL_FALSE);
}


/****************************************************************************/
/** Send Data over Network Channel or over tunnel
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
static GOAL_STATUS_T goal_netChanSendInternal(
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_BUFFER_T *pBuf,                        /**< buffer with data to send */
    GOAL_BOOL_T fromTunnel                      /**< buffer was sent from tunnel */
)
{
    GOAL_STATUS_T res;                          /* result */

    if (NULL == pBuf) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* check if channel is valid */
    if (NULL == pChanHandle) {
        /* GG_LOG ID:178: */
        goal_lmLog(GOAL_ID_NET, 178, 146, 0, 0, GOAL_LOG_SEV_ERROR, "Could not send. Channel is disabled");
        return GOAL_ERR_NET_SEND_CHAN_DISABLED;
    }

    /* check if channel is active */
    if (0 == (pChanHandle->flags & GOAL_NET_FLAGS_ACTIVE)) {
        /* GG_LOG ID:179: */
        goal_lmLog(GOAL_ID_NET, 179, 146, 0, 0, GOAL_LOG_SEV_ERROR, "Could not send. Channel is disabled");
        return GOAL_ERR_NET_SEND_CHAN_DISABLED;
    }

    /* send buffer */
    if ((GOAL_FALSE == fromTunnel) && (pChanHandle->flags & GOAL_NET_FLAGS_TUNNELING)) {
        res = pChanHandle->callback(pChanHandle->pArg, GOAL_NET_CB_SEND_DATA, pChanHandle, pBuf);
    }
    else {
        res = goal_targetNetChanSend(pChanHandle, pBuf);
    }

    return  res;
}


/****************************************************************************/
/** Change the property of a network socket
 *
 * This function will change the specified property of a net socket by calling
 * a target specific function.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_netChanOptionSet(
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_NET_OPTION_T option,                   /**< GOAL option ID */
    void *pValue                                /**< option value */
)
{
    GOAL_STATUS_T res;                          /* result */

    if (NULL == pChanHandle) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* check if channel is active */
    if (0 == (pChanHandle->flags & GOAL_NET_FLAGS_USED)) {
        /* GG_LOG ID:184: */
        goal_lmLog(GOAL_ID_NET, 184, 149, 0, 0, GOAL_LOG_SEV_ERROR, "setting option on inactive channel isn't possible");
        return GOAL_ERR_NET_SEND;
    }

    /* set option in target */
    res = goal_targetNetChanOptSet(pChanHandle, option, pValue);
    return res;
}


/****************************************************************************/
/** Network data receive handler
 *
 * This function hands over the received data to the registered callback
 * function.
 */
void goal_netChanRecv(
    GOAL_NET_CHAN_T *pChan,                     /**< channel handle */
    GOAL_NET_ADDR_T *pAddr,                     /**< remote address */
    GOAL_BUFFER_T *pBuf                         /**< buffer with rx data */
)
{
    if ((NULL == pChan) || (0 == (pChan->flags & GOAL_NET_FLAGS_USED)) || (0 == (pChan->flags & GOAL_NET_FLAGS_ACTIVE))) {
        /* GG_LOG ID:190: */
        goal_lmLog(GOAL_ID_NET, 190, 153, 0, 0, GOAL_LOG_SEV_ERROR, "channel isn't ready yet");
        return;
    }

    if (NULL != pAddr) {
        /* store remote data */
        pChan->addr.remoteIp = pAddr->remoteIp;
        pChan->addr.remotePort = pAddr->remotePort;
    }

    if (pChan->callback) {
        (void) pChan->callback(pChan->pArg, GOAL_NET_CB_NEW_DATA, pChan, pBuf);
    }
    else {
        /* GG_LOG ID:191: */
        goal_lmLog(GOAL_ID_NET, 191, 154, 0, 0, GOAL_LOG_SEV_ERROR, "no callback function registered");
    }
}


/****************************************************************************/
/** Network cyclic loop
 *
 * Check if new data is available and if yes, process each data channel.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
static void goal_netChanLoop(
    void
)
{
    if (GOAL_TRUE == goal_targetNetChanDataPresentCheck()) {
        goal_targetNetChanPoll(m_pNetChannelList);
    }
}


/****************************************************************************/
/** Convert an ASCII string (aaa.bbb.ccc.ddd) to an IP address
 *
 * The generated address is in host byte order.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_netChanIpAddrStrToU32Convert(
    const char *pStr,                           /**< ASCII string */
    uint32_t strLen,                            /**< string buffer length */
    uint32_t *pNetAddr                          /**< net address */
)
{
    uint32_t tmp = 0;                           /* temporary value */
    uint32_t dots = 0;                          /* number of dots read */
    uint32_t charCnt = 0;                       /* number of read chars */

    do {
        switch (*pStr) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                /* char2int
                 * assume decimal power of one for the current char
                 * if last char was also a number, previous tmp has a higher
                 * decimal power
                 */
                tmp = tmp * 10 + (uint32_t) (*pStr - '0');
                break;

            case '.':
                dots++;
                if (dots > 3) {
                    return GOAL_ERROR;
                }
                /* no break, fall trough, last byte has no dot */
                GOAL_TARGET_FALLTHROUGH;

            case '\0':
                if (tmp > 255) {
                    return GOAL_ERROR;
                }
                /* add converted byte to address, assume segment size of 8 bit */
                *pNetAddr = (*pNetAddr << 8) | tmp;
                tmp = 0;
                break;

            default:
                if (charCnt == strLen) {
                    *pNetAddr = (*pNetAddr << 8) | tmp;
                }
                else {
                    return GOAL_ERROR;
                }
                break;
        }
    } while ((*pStr++ != '\0') && (charCnt++ < strLen));

    /* get address format by number of dots
     * - a.b.c.d (3 dots): last segment is 8 bit
     * - a.b.c (2 dots): last segment is 16 bit
     * - a.b (1 dots): last segment is 24 bit
     * - a (0 dots): last segment is 32bit
     * --> size of last segment might need correction
     */
    *pNetAddr <<= 8 * (3 - dots);

    return GOAL_OK;
}


/****************************************************************************/
/** Convert a network address to an ASCII string (x.x.x.x)
 *
 * @returns string length
 */
int goal_netChanIpAddrU32ToStrConvert(
    const uint32_t netAddr,                     /**< net address */
    char *pBuf,                                 /**< ASCII buffer */
    uint32_t bufLen                             /**< buffer length */
)
{
    return GOAL_SNPRINTF(pBuf, bufLen, "%"FMT_u32".%"FMT_u32".%"FMT_u32".%"FMT_u32,
                         (netAddr >> 24) & 0xff, (netAddr >> 16) & 0xff,
                         (netAddr >> 8) & 0xff, netAddr & 0xff);
}


/****************************************************************************/
/** Get Channel List
 *
 * Using this API is not recommended.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_netChanDataGet(
    GOAL_NET_CHAN_T **ppChanData                /**< [out] channel data */
)
{
    *ppChanData = m_pNetChannelList;

    return GOAL_OK;
}
#endif /* GOAL_CONFIG_TCPIP_STACK */
