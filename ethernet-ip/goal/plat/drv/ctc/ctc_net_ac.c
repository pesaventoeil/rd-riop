/** @file
 *
 * @brief  CTC wrapper for networking
 *
 * @details
 * This module provides a CTC wrapper for networking
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


/****************************************************************************/
/* Defines */
/****************************************************************************/
#define GOAL_ID GOAL_ID_NET_CTC

#ifndef GOAL_CONFIG_MEDIA_MA_CHAN_TCP
# define GOAL_CONFIG_MEDIA_MA_CHAN_TCP 0
#endif

#ifndef GOAL_CONFIG_MEDIA_MA_CHAN_UDP
# define GOAL_CONFIG_MEDIA_MA_CHAN_UDP 0
#endif


/****************************************************************************/
/* Includes */
/****************************************************************************/
#include <goal_includes.h>
#include <goal_net_chan.h>
#include <goal_media/goal_ma_net.h>

#if GOAL_CONFIG_MEDIA_MA_CHAN_TCP == 1
# include <goal_media/goal_ma_chan_tcp.h>
#endif

#if GOAL_CONFIG_MEDIA_MA_CHAN_UDP == 1
# include <goal_media/goal_ma_chan_udp.h>
#endif

#include "ctc_net_ac.h"
#include "ctc_net_rpc.h"
#include "ctc_net_list.h"

#include <goal_media/goal_mi_mctc.h>


/****************************************************************************/
/* Structs */
/****************************************************************************/
typedef struct {
    uint32_t channel;                           /* Channel */
    GOAL_MA_NET_T *pHdlNet;                     /* assigned NET hdl */
#if GOAL_CONFIG_MEDIA_MA_CHAN_TCP == 1
    GOAL_MA_CHAN_TCP_T *pHdlTcp;                /* TCP channel MA */
#endif

#if GOAL_CONFIG_MEDIA_MA_CHAN_UDP == 1
    GOAL_MA_CHAN_UDP_T *pHdlUdp;                /* UDP channel MA */
#endif
} GOAL_DRV_NET_CTC_T;


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_DRV_NET_CTC_T *pChannel;            /* Chip select instances */
static GOAL_BOOL_T chanOpen = GOAL_FALSE;       /* Channel opened */
static GOAL_STAGE_HANDLER_T stageInit;          /**< init stage handler */

static GOAL_RPC_HDL_CHN_T *pHdlRpcChn = GOAL_RPC_HDL_NONE; /**< acyclic transmission handle */
#if GOAL_CONFIG_MEDIA_MA_CHAN_TCP == 1 || GOAL_CONFIG_MEDIA_MA_CHAN_UDP == 1
static uint32_t cntChannels;                    /**< channel counter */
#endif
static GOAL_DRV_NET_CHANNEL_LIST_T *pListChan;  /**< channel list */
static uint32_t idNew;                          /**< id */


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
static GOAL_STATUS_T goal_drvCtcNetIpSet(
    GOAL_MA_NET_T *pHdlMaNet,                   /**< MA handle for Net  */
    uint32_t addrIp,                            /**< IP address */
    uint32_t addrMask,                          /**< subnet mask */
    uint32_t addrGw,                            /**< gateway */
    GOAL_BOOL_T flgTemp                         /**< temporary IP config flag */
);

#if GOAL_CONFIG_MEDIA_MA_CHAN_UDP == 1
static GOAL_STATUS_T goal_drvCtcUdpChanNew(
    GOAL_MA_CHAN_UDP_T *pHdlMaUdp,              /**< pointer to store NET handler */
    GOAL_NET_CHAN_T **ppChanHandle,             /**< pointer to channel handle */
    GOAL_NET_CHAN_T *pChanOut,                  /**< pointer to channel handle for output */
    GOAL_NET_ADDR_T *pAddr,                     /**< remote address */
    GOAL_NET_TYPE_T type,                       /**< connection type */
    GOAL_MA_CHAN_UDP_CB_T callback,             /**< channel callback */
    GOAL_BOOL_T flgOption,                      /**< flag for socket option */
    GOAL_BOOL_T optVal,                         /**< value of socket option */
    GOAL_BOOL_T flgImmediateActivation          /**< flag for immediate activation */
);

static GOAL_STATUS_T goal_drvCtcUdpChanClose(
    GOAL_MA_CHAN_UDP_T *pHdlMaUdp,              /**< pointer to store NET handler */
    GOAL_NET_CHAN_T *pChanHandle                /**< pointer to channel handle */
);

static GOAL_STATUS_T goal_drvCtcUdpChanSetNonBlocking(
    GOAL_MA_CHAN_UDP_T *pHdlMaUdp,              /**< pointer to store NET handler */
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_BOOL_T flgOption                       /**< option value */
);

static GOAL_STATUS_T goal_drvCtcUdpChanSetBroadcast(
    GOAL_MA_CHAN_UDP_T *pHdlMaUdp,              /**< pointer to store NET handler */
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_BOOL_T flgOption                       /**< option value */
);

static GOAL_STATUS_T goal_drvCtcUdpChanActivate(
    GOAL_MA_CHAN_UDP_T *pHdlMaUdp,              /**< pointer to store NET handler */
    GOAL_NET_CHAN_T *pChanHandle                /**< channel handle */
);

static GOAL_STATUS_T goal_drvCtcUdpChanGetRemoteAddr(
    GOAL_MA_CHAN_UDP_T *pHdlMaUdp,              /**< pointer to store NET handler */
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_NET_ADDR_T *pAddr                      /**< remote address */
);

static GOAL_STATUS_T goal_drvCtcUdpChanSend(
    GOAL_MA_CHAN_UDP_T *pHdlMaUdp,              /**< pointer to store NET handler */
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_BUFFER_T *pBuf                         /**< buffer with data to send */
);
#endif /* GOAL_CONFIG_MEDIA_MA_CHAN_UDP */

#if GOAL_CONFIG_MEDIA_MA_CHAN_TCP == 1
static GOAL_STATUS_T goal_drvCtcTcpChanNew(
    GOAL_MA_CHAN_TCP_T *pHdlMaTcp,              /**< pointer to store NET handler */
    GOAL_NET_CHAN_T **ppChanHandle,             /**< pointer to channel handle */
    GOAL_NET_CHAN_T *pChanOut,                  /**< pointer to channel handle for output */
    GOAL_NET_ADDR_T *pAddr,                     /**< remote address */
    GOAL_NET_TYPE_T type,                       /**< connection type */
    GOAL_MA_CHAN_TCP_CB_T callback,             /**< channel callback */
    GOAL_BOOL_T flgOption,                      /**< flag for socket option */
    GOAL_BOOL_T optVal,                         /**< value of socket option */
    GOAL_BOOL_T flgImmediateActivation          /**< flag for immediate activation */
);

static GOAL_STATUS_T goal_drvCtcTcpChanClose(
    GOAL_MA_CHAN_TCP_T *pHdlMaTcp,              /**< pointer to store NET handler */
    GOAL_NET_CHAN_T *pChanHandle                /**< pointer to channel handle */
);

static GOAL_STATUS_T goal_drvCtcTcpChanSetNonBlocking(
    GOAL_MA_CHAN_TCP_T *pHdlMaUdp,              /**< pointer to store NET handler */
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_BOOL_T flgOption                       /**< non blocking option */
);

static GOAL_STATUS_T goal_drvCtcTcpChanActivate(
    GOAL_MA_CHAN_TCP_T *pHdlMaUdp,              /**< pointer to store NET handler */
    GOAL_NET_CHAN_T *pChanHandle                /**< channel handle */
);

static GOAL_STATUS_T goal_drvCtcTcpChanReopen(
    GOAL_MA_CHAN_TCP_T *pHdlMaTcp,              /**< pointer to store NET handler */
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_BOOL_T immediateActivation             /**< flag for immediate activation */
);

static GOAL_STATUS_T goal_drvCtcTcpChanGetRemoteAddr(
    GOAL_MA_CHAN_TCP_T *pHdlMaUdp,              /**< pointer to store NET handler */
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_NET_ADDR_T *pAddr                      /**< remote address */
);

static GOAL_STATUS_T goal_drvCtcTcpChanSend(
    GOAL_MA_CHAN_TCP_T *pHdlMaUdp,              /**< pointer to store NET handler */
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_BUFFER_T *pBuf                         /**< buffer with data to send */
);
#endif /* GOAL_CONFIG_MEDIA_MA_CHAN_TCP */

static GOAL_STATUS_T goal_drvCtcNetOpen(
    GOAL_MA_NET_T *pHdlMaNet                    /**< MA handle for Net */
);

static GOAL_STATUS_T goal_drvCtcNetClose(
    GOAL_MA_NET_T *pHdlMaNet                    /**< MA handle for Net  */
);

static GOAL_STATUS_T goal_drvCtcNetRpcInit(
    void
);

static GOAL_STATUS_T goal_drvCtcNetRpcPastInit(
    void
);

static GOAL_STATUS_T goal_drvCtcNetRpcSetupChannels(
    void
);

static GOAL_STATUS_T goal_drvCtcNetRpcRegisterServices(
    void
);

static GOAL_STATUS_T goal_drvCtcNetChanCbServer(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< rpc handle */
);

static GOAL_STATUS_T goal_drvCtcNetPastInit(
    void
);


/****************************************************************************/
/** Registers the instances for a given channel at the NET MA
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_netRpcInit(
    uint32_t id                                 /**< ID for MA instance */
)
{
    GOAL_STATUS_T res;                          /* result */
    idNew = id;

    /* set  the size of the RPC stack */
    res = goal_rpcSetStackMax(RPC_BUFF_SIZE_NET);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to set RPC stack size");
        return res;
    }

    return goal_mainStageReg(GOAL_STAGE_CTC, &stageInit, GOAL_STAGE_INIT, goal_drvCtcNetPastInit);
}


/****************************************************************************/
/** Execute initialization at proper goal stage
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_drvCtcNetPastInit(
    void
)
{
    GOAL_DRV_NET_CTC_T *pCsInstance = NULL;     /* specific instance data */
    GOAL_STATUS_T res = GOAL_OK;                /* Status */

    /* Alloc memory for MA instance */
    res = goal_memCalloc(&pChannel, sizeof(GOAL_DRV_NET_CTC_T));

    if (GOAL_RES_OK(res)) {
        pCsInstance = pChannel;

        /* Register driver instance at MA */
        res = goal_maNetDrvReg(
                idNew,
                &(pCsInstance->pHdlNet),
                goal_drvCtcNetOpen,
                goal_drvCtcNetIpSet,
                NULL,
                NULL,
                goal_drvCtcNetClose,
                pCsInstance
        );
    }

#if GOAL_CONFIG_MEDIA_MA_CHAN_TCP == 1
    if (GOAL_RES_OK(res)) {
        res = goal_maChanTcpDrvReg(
                idNew,
                &(pCsInstance->pHdlTcp),
                goal_drvCtcTcpChanNew,
                goal_drvCtcTcpChanClose,
                goal_drvCtcTcpChanSetNonBlocking,
                goal_drvCtcTcpChanGetRemoteAddr,
                goal_drvCtcTcpChanActivate,
                goal_drvCtcTcpChanSend,
                pCsInstance
        );
    }
#endif

#if GOAL_CONFIG_MEDIA_MA_CHAN_UDP == 1
    if (GOAL_RES_OK(res)) {
        res = goal_maChanUdpDrvReg(
                idNew,
                &(pCsInstance->pHdlUdp),
                goal_drvCtcUdpChanNew,
                goal_drvCtcUdpChanClose,
                goal_drvCtcUdpChanSetNonBlocking,
                goal_drvCtcUdpChanSetBroadcast,
                goal_drvCtcUdpChanGetRemoteAddr,
                goal_drvCtcUdpChanActivate,
                goal_drvCtcUdpChanSend,
                pCsInstance
        );
    }
#endif

    if (GOAL_RES_OK(res)) {
        res = goal_drvCtcNetRpcInit();
    }

    return res;

}


/****************************************************************************/
/** Opens an NET channel
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_drvCtcNetOpen(
    GOAL_MA_NET_T *pHdlMaNet                    /**< MA handle for NET  */
)
{
    UNUSEDARG(pHdlMaNet);

    return GOAL_OK;
}


/****************************************************************************/
/** Closes an NET channel
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_drvCtcNetClose(
    GOAL_MA_NET_T *pHdlMaNet                    /**< MA handle for NET  */
)
{
    GOAL_DRV_NET_CTC_T *pCsInstance;            /* specific instance data */
    GOAL_BOOL_T *pChannelOpen;                  /* channel status */
    GOAL_STATUS_T res = GOAL_OK;                /* Status */

    pCsInstance = (GOAL_DRV_NET_CTC_T *) (pHdlMaNet->pDrvData);
    pChannelOpen = &chanOpen;

    UNUSEDARG(pCsInstance);

    /* Close channel */
    if (*pChannelOpen) {
        res = GOAL_OK;
    }

    return res;
}


/****************************************************************************/
/** Set IP address
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_drvCtcNetIpSet(
    GOAL_MA_NET_T *pHdlMaNet,                   /**< MA handle for Net  */
    uint32_t addrIp,                            /**< IP address */
    uint32_t addrMask,                          /**< subnet mask */
    uint32_t addrGw,                            /**< gateway */
    GOAL_BOOL_T flgTemp                         /**< temporary IP config flag */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE;/* call handle */

    UNUSEDARG(pHdlMaNet);

    GOAL_RPC_NEW();
    GOAL_RPC_PUSH(flgTemp);
    GOAL_RPC_PUSH(addrGw);
    GOAL_RPC_PUSH(addrMask);
    GOAL_RPC_PUSH(addrIp);

    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_NET, GOAL_RPC_FUNC_NET_IP_SET);
    }

    GOAL_RPC_CLOSE();

    return res;
}


#if GOAL_CONFIG_MEDIA_MA_CHAN_TCP == 1
/****************************************************************************/
/** Writes data to the NET
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_drvCtcTcpChanSend(
    GOAL_MA_CHAN_TCP_T *pHdlMaTcp,              /**< pointer to store NET handler */
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_BUFFER_T *pBuf                         /**< buffer with data to send */
)
{
    GOAL_DRV_NET_CTC_T *pCsInstance;            /* specific instance data */
    GOAL_STATUS_T res = GOAL_OK;                /* Status */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE;/* call handle */
    uint32_t idChan;                            /* channel id */

    pCsInstance = (GOAL_DRV_NET_CTC_T *) (pHdlMaTcp->pDrvData);
    UNUSEDARG(pCsInstance);

    GOAL_RPC_NEW();

    /* push the data to the stack */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcArgPush(pHdlRpc, pBuf->ptrData, (uint16_t) pBuf->dataLen);
    }

    /* push the data length to the stack */
    GOAL_RPC_PUSH(pBuf->dataLen);

    res = goal_drvCtcChannelId(pListChan, pChanHandle, &idChan);
    GOAL_RPC_PUSH(idChan);

    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_NET, GOAL_RPC_FUNC_NET_CHAN_SEND);
    }

    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** RPC wrapper for network channel open function
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
static GOAL_STATUS_T goal_drvCtcTcpChanNew(
    GOAL_MA_CHAN_TCP_T *pHdlMaTcp,              /**< pointer to store NET handler */
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
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE;/* call handle */
    uint32_t idChan;                            /* local channel id */
    GOAL_DRV_NET_CHANNEL_LIST_T *pEntry = NULL; /* channel list entry */
    uint32_t flgVal;                            /* flag value */
    uint32_t option;                            /* option */

    UNUSEDARG(pHdlMaTcp);
    UNUSEDARG(pChanOut);

    /* check channel type */
    if ((GOAL_NET_TCP_LISTENER != type) && (GOAL_NET_TCP != type) && (GOAL_NET_TCP_CLIENT != type)) {
        return GOAL_ERR_PARAM;
    }

    /* immediate activation flag cannot set if connection type is TCP Listener/Server */
    if (((GOAL_NET_TCP_LISTENER == type) || (GOAL_NET_TCP == type)) && (GOAL_TRUE == flgImmediateActivation)) {
        return GOAL_ERR_PARAM;
    }

    /* It is considered that it is wanted to call the channel reopen function,
     * if the channel handle is already assigned and used and the address pointer
     * and the type are same and the corresponding channel list entry exists.
     */
    if (NULL != *ppChanHandle) {
        GOAL_LL_FOREACH(pListChan, pEntry) {
            if ((pEntry->pChannel == *ppChanHandle) && (&(*ppChanHandle)->addr == pAddr) && ((*ppChanHandle)->type == type) && (NULL == callback)) {
                break;
            }
        }
        if (NULL != pEntry) {
            return goal_drvCtcTcpChanReopen(pHdlMaTcp, *ppChanHandle, flgImmediateActivation);
        }
    }

    /* return parameter error if connection type is TCP server and reopen handler was not called */
    if (GOAL_NET_TCP == type) {
        return GOAL_ERR_PARAM;
    }

    GOAL_RPC_NEW();

    /* create local channel */
    idChan = cntChannels;

    res = goal_memCalloc(ppChanHandle, sizeof(GOAL_NET_CHAN_T));

    if (GOAL_RES_OK(res)) {
        res = goal_memCalloc(&pEntry, sizeof(GOAL_DRV_NET_CHANNEL_LIST_T));
    }

    /* populate entry and add to list */
    if (GOAL_RES_OK(res)) {
        pEntry->id = idChan;
        pEntry->pChannel = *ppChanHandle;
    }

    /* initiate creation of remote channel */
    flgVal = (GOAL_TRUE == flgImmediateActivation) ? 1 : 0;
    GOAL_RPC_PUSH(flgVal);
    if (GOAL_TRUE == flgOption) {
        GOAL_RPC_PUSH(optVal);
        option = GOAL_NET_OPTION_NONBLOCK;
        GOAL_RPC_PUSH(option);
    }
    flgVal = (GOAL_TRUE == flgOption) ? 1 : 0;
    GOAL_RPC_PUSH(flgVal);
    GOAL_RPC_PUSH(type);
    GOAL_RPC_PUSH(pAddr->remotePort);
    GOAL_RPC_PUSH(pAddr->remoteIp);
    GOAL_RPC_PUSH(pAddr->localPort);
    GOAL_RPC_PUSH(pAddr->localIp);

    GOAL_RPC_PUSH(idChan);

    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_NET, GOAL_RPC_FUNC_NET_CHAN_NEW);
    }

    GOAL_RPC_CLOSE();

    if (GOAL_RES_OK(res)) {
        pEntry->pChannel->flags |= GOAL_NET_FLAGS_USED;
        if (GOAL_TRUE == flgImmediateActivation) {
            pEntry->pChannel->flags |= GOAL_NET_FLAGS_ACTIVE;
        }
        else {
            pEntry->pChannel->flags &= ~GOAL_NET_FLAGS_ACTIVE;
        }
        pEntry->fnCallback = (void *) callback;
        pEntry->pChannel->type = (GOAL_NET_TCP_LISTENER == type) ? GOAL_NET_TCP : type;
        GOAL_MEMCPY(&pEntry->pChannel->addr, pAddr, sizeof(GOAL_NET_ADDR_T));
        GOAL_LL_APPEND(pListChan, pEntry);
    }

    /* increase global channel counter */
    cntChannels++;

    return res;
}


/****************************************************************************/
/** RPC wrapper for network channel close function
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
static GOAL_STATUS_T goal_drvCtcTcpChanClose(
    GOAL_MA_CHAN_TCP_T *pHdlMaTcp,              /**< pointer to store NET handler */
    GOAL_NET_CHAN_T *pChanHandle                /**< pointer to channel handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE;/* call handle */
    GOAL_DRV_NET_CHANNEL_LIST_T *pEntry = NULL; /* list entry */
    uint32_t idChan;                            /* local channel id */

    UNUSEDARG(pHdlMaTcp);

    /* check if channel is in use */
    if (0 == (pChanHandle->flags & GOAL_NET_FLAGS_USED)) {
        return GOAL_ERR_NET_CLOSE;
    }

    /* check if channel is active */
    if (0 == (pChanHandle->flags & GOAL_NET_FLAGS_ACTIVE)) {
        return GOAL_ERR_NET_CLOSE;
    }

    GOAL_RPC_NEW();

    /* associate local channel */
    res = goal_drvCtcChannelId(pListChan, pChanHandle, &idChan);

    /* initiate creation of remote channel */
    GOAL_RPC_PUSH(idChan);

    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_NET, GOAL_RPC_FUNC_NET_CHAN_CLOSE);
    }

    GOAL_RPC_CLOSE();

    if (GOAL_RES_OK(res)) {
        res = goal_drvCtcChannelEntry(pListChan, pChanHandle, &pEntry);
    }

    if (GOAL_RES_OK(res)) {
        pEntry->pChannel->flags &= ~GOAL_NET_FLAGS_ACTIVE;
    }

    return res;
}


/****************************************************************************/
/** RPC wrapper for network channel set option function
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
static GOAL_STATUS_T goal_drvCtcTcpChanSetNonBlocking(
    GOAL_MA_CHAN_TCP_T *pHdlMaTcp,              /**< pointer to store NET handler */
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_BOOL_T flgOption                       /**< non blocking option */
)

{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE;/* call handle */
    uint32_t idChan;                            /* local channel id */
    uint32_t value;                             /* option value */
    uint32_t option;                            /* opion */

    UNUSEDARG(pHdlMaTcp);

    /* check if channel is in use */
    if (0 == (pChanHandle->flags & GOAL_NET_FLAGS_USED)) {
        return GOAL_ERR_NET_OPTION;
    }

    GOAL_RPC_NEW();

    /* get local channel */
    res = goal_drvCtcChannelId(pListChan, pChanHandle, &idChan);

    if (flgOption == GOAL_TRUE) {
        value = 1;
    }
    else {
        value = 0;
    }

    /* initiate creation of remote channel */
    GOAL_RPC_PUSH(value);

    option = GOAL_NET_OPTION_NONBLOCK;
    GOAL_RPC_PUSH(option);
    GOAL_RPC_PUSH(idChan);

    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_NET, GOAL_RPC_FUNC_NET_SET_OPTION);
    }

    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** RPC wrapper for network channel activation function
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
static GOAL_STATUS_T goal_drvCtcTcpChanActivate(
    GOAL_MA_CHAN_TCP_T *pHdlMaTcp,              /**< pointer to store NET handler */
    GOAL_NET_CHAN_T *pChanHandle                /**< channel handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE;/* call handle */
    uint32_t idChan;                            /* channel id */

    UNUSEDARG(pHdlMaTcp);

    /* check if channel is in use */
    if (0 == (pChanHandle->flags & GOAL_NET_FLAGS_USED)) {
        return GOAL_ERR_NET_ACTIVATE;
    }

    GOAL_RPC_NEW();

    /* get local channel */
    res = goal_drvCtcChannelId(pListChan, pChanHandle, &idChan);

    GOAL_RPC_PUSH(idChan);

    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_NET, GOAL_RPC_FUNC_NET_CHAN_ACTIVATE);
    }

    if (GOAL_RES_OK(res)) {
        pChanHandle->flags |= GOAL_NET_FLAGS_ACTIVE;
    }

    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** RPC wrapper for network channel reopen function
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
static GOAL_STATUS_T goal_drvCtcTcpChanReopen(
    GOAL_MA_CHAN_TCP_T *pHdlMaTcp,              /**< pointer to store NET handler */
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_BOOL_T immediateActivation             /**< flag for immediate activation */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE;/* call handle */
    uint32_t idChan = 0;                        /* channel ID */

    UNUSEDARG(pHdlMaTcp);

    GOAL_RPC_NEW();

    if (GOAL_RES_OK(res)) {
        /* get local channel */
        res = goal_drvCtcChannelId(pListChan, pChanHandle, &idChan);
    }

    GOAL_RPC_PUSH(idChan);
    GOAL_RPC_PUSH((uint32_t) immediateActivation);

    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_NET, GOAL_RPC_FUNC_NET_CHAN_REOPEN);
    }

    GOAL_RPC_CLOSE();

    if (GOAL_RES_OK(res)) {
        if ((GOAL_NET_TCP_CLIENT == pChanHandle->type) && (GOAL_TRUE == immediateActivation)) {
            pChanHandle->flags |= GOAL_NET_FLAGS_ACTIVE;
        }
    }

    return res;
}


/****************************************************************************/
/** RPC wrapper for network channel remote address getter function
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
static GOAL_STATUS_T goal_drvCtcTcpChanGetRemoteAddr(
    GOAL_MA_CHAN_TCP_T *pHdlMaTcp,              /**< pointer to store NET handler */
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_NET_ADDR_T *pAddr                      /**< remote address */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE;/* call handle */
    uint32_t idChan;                            /* channel id */

    UNUSEDARG(pHdlMaTcp);

    /* check if channel is active */
    if (0 == (pChanHandle->flags & GOAL_NET_FLAGS_ACTIVE)) {
        return GOAL_ERR_NET_ADDRESS;
    }

    GOAL_RPC_NEW();

    /* get local channel */
    res = goal_drvCtcChannelId(pListChan, pChanHandle, &idChan);

    GOAL_RPC_PUSH(idChan);

    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_NET, GOAL_RPC_FUNC_NET_CHAN_GET_REMOTE_ADDR);
    }

    GOAL_RPC_POP(pAddr->localIp, uint32_t);
    GOAL_RPC_POP(pAddr->localPort, uint16_t);
    GOAL_RPC_POP(pAddr->remoteIp, uint32_t);
    GOAL_RPC_POP(pAddr->remotePort, uint16_t);

    GOAL_RPC_CLOSE();

    return res;
}
#endif /* GOAL_CONFIG_MEDIA_MA_CHAN_TCP */

#if GOAL_CONFIG_MEDIA_MA_CHAN_UDP == 1
/****************************************************************************/
/** Writes data to the NET
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_drvCtcUdpChanSend(
    GOAL_MA_CHAN_UDP_T *pHdlMaUdp,              /**< pointer to store NET handler */
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_BUFFER_T *pBuf                         /**< buffer with data to send */
)
{
    GOAL_DRV_NET_CTC_T *pCsInstance;            /* specific instance data */
    GOAL_STATUS_T res = GOAL_OK;                /* Status */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE;/* call handle */
    uint32_t idChan;                            /* channel id */

    pCsInstance = (GOAL_DRV_NET_CTC_T *) (pHdlMaUdp->pDrvData);
    UNUSEDARG(pCsInstance);

    GOAL_RPC_NEW();

    /* push the data to the stack */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcArgPush(pHdlRpc, pBuf->ptrData, (uint16_t) pBuf->dataLen);
    }

    /* push the data length to the stack */
    GOAL_RPC_PUSH(pBuf->dataLen);

    res = goal_drvCtcChannelId(pListChan, pChanHandle, &idChan);
    GOAL_RPC_PUSH(idChan);

    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_NET, GOAL_RPC_FUNC_NET_CHAN_SEND);
    }

    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** RPC wrapper for network channel open function
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
static GOAL_STATUS_T goal_drvCtcUdpChanNew(
    GOAL_MA_CHAN_UDP_T *pHdlMaUdp,              /**< pointer to store NET handler */
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
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE;/* call handle */
    uint32_t idChan;                            /* local channel id */
    GOAL_DRV_NET_CHANNEL_LIST_T *pEntry = NULL; /* channel list entry */
    uint32_t flgVal;                            /* flag value */
    uint32_t option;                            /* option */
    GOAL_BOOL_T flgReuse = GOAL_FALSE;          /* reusing channel handle flag */

    UNUSEDARG(pHdlMaUdp);
    UNUSEDARG(pChanOut);

    /* check channel type */
    if ((GOAL_NET_UDP_SERVER != type) && (GOAL_NET_UDP_CLIENT != type)) {
        return GOAL_ERR_PARAM;
    }

    /* check channel handle */
    if (NULL != *ppChanHandle) {
        /* search corresponding channel list entry */
        GOAL_LL_FOREACH(pListChan, pEntry) {
            if ((pEntry->pChannel == *ppChanHandle) && (&(*ppChanHandle)->addr == pAddr) && ((*ppChanHandle)->type == type) && (0 == ((*ppChanHandle)->flags & GOAL_NET_FLAGS_USED)) && (0 == ((*ppChanHandle)->flags & GOAL_NET_FLAGS_ACTIVE))) {
                break;
            }
        }

        /* set reusing channel handle flag if corresponding channel list entry was found and callback is identical */
        if ((NULL != pEntry) && (pEntry->fnCallback == (void *) callback)) {
            flgReuse = GOAL_TRUE;
        }
    }

    GOAL_RPC_NEW();

    /* create local channel */
    idChan = cntChannels;

    if (GOAL_FALSE == flgReuse) {
        res = goal_memCalloc(ppChanHandle, sizeof(GOAL_NET_CHAN_T));
        if (GOAL_RES_OK(res)) {
            res = goal_memCalloc(&pEntry, sizeof(GOAL_DRV_NET_CHANNEL_LIST_T));
        }
    }

    /* populate entry and add to list */
    if (GOAL_RES_OK(res)) {
        pEntry->id = idChan;
        pEntry->pChannel = *ppChanHandle;
    }

    /* initiate creation of remote channel */
    flgVal = (GOAL_TRUE == flgImmediateActivation) ? 1 : 0;
    GOAL_RPC_PUSH(flgVal);
    if (GOAL_TRUE == flgOption) {
        GOAL_RPC_PUSH(optVal);
        option = GOAL_NET_OPTION_NONBLOCK;
        GOAL_RPC_PUSH(option);
    }
    flgVal = (GOAL_TRUE == flgOption) ? 1 : 0;
    GOAL_RPC_PUSH(flgVal);
    GOAL_RPC_PUSH(type);
    GOAL_RPC_PUSH(pAddr->remotePort);
    GOAL_RPC_PUSH(pAddr->remoteIp);
    GOAL_RPC_PUSH(pAddr->localPort);
    GOAL_RPC_PUSH(pAddr->localIp);

    GOAL_RPC_PUSH(idChan);

    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_NET, GOAL_RPC_FUNC_NET_CHAN_NEW);
    }

    GOAL_RPC_CLOSE();

    if (GOAL_RES_OK(res)) {
        pEntry->pChannel->flags |= GOAL_NET_FLAGS_USED;
        if (GOAL_TRUE == flgImmediateActivation) {
            pEntry->pChannel->flags |= GOAL_NET_FLAGS_ACTIVE;
        }
        else {
            pEntry->pChannel->flags &= ~GOAL_NET_FLAGS_ACTIVE;
        }

        if (GOAL_FALSE == flgReuse) {
            pEntry->fnCallback = (void *) callback;
            pEntry->pChannel->type = type;
            GOAL_MEMCPY(&pEntry->pChannel->addr, pAddr, sizeof(GOAL_NET_ADDR_T));
            GOAL_LL_APPEND(pListChan, pEntry);
        }
    }

    /* increase global channel counter */
    cntChannels++;

    return res;
}


/****************************************************************************/
/** RPC wrapper for network channel close function
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
static GOAL_STATUS_T goal_drvCtcUdpChanClose(
    GOAL_MA_CHAN_UDP_T *pHdlMaUdp,              /**< pointer to store NET handler */
    GOAL_NET_CHAN_T *pChanHandle                /**< pointer to channel handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE;/* call handle */
    GOAL_DRV_NET_CHANNEL_LIST_T *pEntry = NULL; /* list entry */
    uint32_t idChan;                            /* local channel id */

    UNUSEDARG(pHdlMaUdp);

    /* check if channel is in use */
    if (0 == (pChanHandle->flags & GOAL_NET_FLAGS_USED)) {
        return GOAL_ERR_NET_CLOSE;
    }

    /* check if channel is active */
    if (0 == (pChanHandle->flags & GOAL_NET_FLAGS_ACTIVE)) {
        return GOAL_ERR_NET_CLOSE;
    }

    GOAL_RPC_NEW();

    /* associate local channel */
    res = goal_drvCtcChannelId(pListChan, pChanHandle, &idChan);

    /* initiate creation of remote channel */
    GOAL_RPC_PUSH(idChan);

    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_NET, GOAL_RPC_FUNC_NET_CHAN_CLOSE);
    }

    GOAL_RPC_CLOSE();

    if (GOAL_RES_OK(res)) {
        res = goal_drvCtcChannelEntry(pListChan, pChanHandle, &pEntry);
    }

    if (GOAL_RES_OK(res)) {
        pEntry->pChannel->flags &= ~(GOAL_NET_FLAGS_ACTIVE | GOAL_NET_FLAGS_USED);
    }

    return res;
}


/****************************************************************************/
/** RPC wrapper for network channel set option function
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
static GOAL_STATUS_T goal_drvCtcUdpChanSetNonBlocking(
    GOAL_MA_CHAN_UDP_T *pHdlMaUdp,              /**< pointer to store NET handler */
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_BOOL_T flgOption                       /**< option value */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE;/* call handle */
    uint32_t idChan;                            /* local channel id */
    uint32_t option;                            /* option */
    uint32_t value;                             /* option value */

    UNUSEDARG(pHdlMaUdp);

    /* check if channel is in use */
    if (0 == (pChanHandle->flags & GOAL_NET_FLAGS_USED)) {
        return GOAL_ERR_NET_OPTION;
    }

    GOAL_RPC_NEW();

    /* get local channel */
    res = goal_drvCtcChannelId(pListChan, pChanHandle, &idChan);

    if (GOAL_TRUE == flgOption) {
        value = 1;
    }
    else {
        value = 0;
    }

    /* initiate creation of remote channel */
    GOAL_RPC_PUSH(value);

    option = GOAL_NET_OPTION_NONBLOCK;
    GOAL_RPC_PUSH(option);
    GOAL_RPC_PUSH(idChan);

    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_NET, GOAL_RPC_FUNC_NET_SET_OPTION);
    }

    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** RPC wrapper for network channel set option function
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
static GOAL_STATUS_T goal_drvCtcUdpChanSetBroadcast(
    GOAL_MA_CHAN_UDP_T *pHdlMaUdp,              /**< pointer to store NET handler */
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_BOOL_T flgOption                       /**< option value */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE;/* call handle */
    uint32_t idChan;                            /* local channel id */
    uint32_t option;                            /* option */
    uint32_t value;                             /* option value */

    UNUSEDARG(pHdlMaUdp);

    /* check if channel is in use */
    if (0 == (pChanHandle->flags & GOAL_NET_FLAGS_USED)) {
        return GOAL_ERR_NET_OPTION;
    }

    GOAL_RPC_NEW();

    /* get local channel */
    res = goal_drvCtcChannelId(pListChan, pChanHandle, &idChan);

    if (GOAL_TRUE == flgOption) {
        value = 1;
    }
    else {
        value = 0;
    }

    /* initiate creation of remote channel */
    GOAL_RPC_PUSH(value);

    option = GOAL_NET_OPTION_BROADCAST;
    GOAL_RPC_PUSH(option);
    GOAL_RPC_PUSH(idChan);

    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_NET, GOAL_RPC_FUNC_NET_SET_OPTION);
    }

    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** RPC wrapper for network channel activation function
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
static GOAL_STATUS_T goal_drvCtcUdpChanActivate(
    GOAL_MA_CHAN_UDP_T *pHdlMaUdp,              /**< pointer to store NET handler */
    GOAL_NET_CHAN_T *pChanHandle                /**< channel handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE;/* call handle */
    uint32_t idChan;                            /* channel id */

    UNUSEDARG(pHdlMaUdp);

    /* check if channel is in use */
    if (0 == (pChanHandle->flags & GOAL_NET_FLAGS_USED)) {
        return GOAL_ERR_NET_ACTIVATE;
    }

    GOAL_RPC_NEW();

    /* get local channel */
    res = goal_drvCtcChannelId(pListChan, pChanHandle, &idChan);

    GOAL_RPC_PUSH(idChan);

    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_NET, GOAL_RPC_FUNC_NET_CHAN_ACTIVATE);
    }

    if (GOAL_RES_OK(res)) {
        pChanHandle->flags |= GOAL_NET_FLAGS_ACTIVE;
    }

    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** RPC wrapper for network channel remote address getter function
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
static GOAL_STATUS_T goal_drvCtcUdpChanGetRemoteAddr(
    GOAL_MA_CHAN_UDP_T *pHdlMaUdp,              /**< pointer to store NET handler */
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_NET_ADDR_T *pAddr                      /**< remote address */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE;/* call handle */
    uint32_t idChan;                            /* channel id */

    UNUSEDARG(pHdlMaUdp);

    /* check if channel is active */
    if (0 == (pChanHandle->flags & GOAL_NET_FLAGS_ACTIVE)) {
        return GOAL_ERR_NET_ADDRESS;
    }

    GOAL_RPC_NEW();

    /* get local channel */
    res = goal_drvCtcChannelId(pListChan, pChanHandle, &idChan);

    GOAL_RPC_PUSH(idChan);

    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_NET, GOAL_RPC_FUNC_NET_CHAN_GET_REMOTE_ADDR);
    }

    GOAL_RPC_POP(pAddr->localIp, uint32_t);
    GOAL_RPC_POP(pAddr->localPort, uint16_t);
    GOAL_RPC_POP(pAddr->remoteIp, uint32_t);
    GOAL_RPC_POP(pAddr->remotePort, uint16_t);

    GOAL_RPC_CLOSE();

    return res;
}
#endif /* GOAL_CONFIG_MEDIA_MA_CHAN_UDP */


/****************************************************************************/
/** RPC initialization
 *
 * This function initialize all necessary function to use NET via RPC.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_drvCtcNetRpcInit(
    void
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    /* register the NET via RPC init function */
    if (GOAL_RES_OK(res)) {
        res = goal_drvCtcNetRpcPastInit();
    }

    return res;
}


/****************************************************************************/
/** NET via RPC past initialization
 *
 * This function is called during the init stage of the GOAL_STAGE_CTC_PRE.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_drvCtcNetRpcPastInit(
    void
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    /* setup the service function */
    if (GOAL_RES_OK(res)) {
        res = goal_drvCtcNetRpcRegisterServices();
    }

    /* setup all necessary RPC channels */
    if (GOAL_RES_OK(res)) {
        res = goal_drvCtcNetRpcSetupChannels();
    }

    return res;
}


/****************************************************************************/
/** Register server function
 *
 * This function registers all server functions.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_drvCtcNetRpcRegisterServices(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* setup the service function */
    res = goal_rpcRegisterService(GOAL_ID_NET, GOAL_RPC_FUNC_NET_CHAN_CB, &goal_drvCtcNetChanCbServer);

    return res;
}


/****************************************************************************/
/** Setup channels for NET via RPC
 *
 * This function initialize the ctc channels for RPC.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_drvCtcNetRpcSetupChannels(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* setup a channel for RPC */
    res = goal_rpcSetupChannel(&pHdlRpcChn, GOAL_ID_MI_CTC_DEFAULT);

    return res;
}


/****************************************************************************/
/** Channel callback - server
 *
 * Pop the data from the RPC stack, call the selected function and send a
 * response to the client.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_drvCtcNetChanCbServer(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< rpc handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    uint32_t idChan = 0;                        /* channel id */
    uint16_t dataLen = 0;                       /* data length */
    GOAL_BUFFER_T *pBuf = NULL;                 /* buffer */
    GOAL_NET_CB_TYPE_T cbType = GOAL_NET_CB_NEW_DATA; /* callback type */
    uint32_t chanType = 0;                      /* channel type */
    GOAL_DRV_NET_CHANNEL_LIST_T *pEntry;        /* channel list entry */
#if GOAL_CONFIG_MEDIA_MA_CHAN_UDP == 1
    GOAL_MA_CHAN_UDP_CB_T fnUdpCallback;        /* udp callback */
#endif
#if GOAL_CONFIG_MEDIA_MA_CHAN_TCP == 1
    GOAL_MA_CHAN_TCP_CB_T fnTcpCallback;        /* udp callback */
#endif

    GOAL_RPC_POP(cbType, GOAL_NET_CB_TYPE_T);
    GOAL_RPC_POP(idChan, uint32_t);
    GOAL_RPC_POP(chanType, uint32_t);
    GOAL_RPC_POP(dataLen, uint16_t);

    if (GOAL_RES_OK(res)) {
       res = goal_queuePoolGetBuf(&pBuf, dataLen, GOAL_ID_NET);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_rpcArgPop(pHdlRpc, pBuf->ptrData, dataLen);
        pBuf->dataLen = dataLen;
    }

    if (GOAL_RES_OK(res)) {

        res = goal_drvCtcChannelEntryById(pListChan, idChan, &pEntry);

        /* set channel active state if necessary */
        if (GOAL_RES_OK(res)) {
            if ((GOAL_NET_TCP == pEntry->pChannel->type) && (GOAL_NET_CB_NEW_SOCKET == cbType)) {
                pEntry->pChannel->flags |= GOAL_NET_FLAGS_ACTIVE;
            } else if (((GOAL_NET_TCP == pEntry->pChannel->type) || (GOAL_NET_TCP_CLIENT == pEntry->pChannel->type)) && (GOAL_NET_CB_CLOSING == cbType)) {
                pEntry->pChannel->flags &= GOAL_NET_FLAGS_ACTIVE;
            }
        }

        /* call callback */

        /* check channel type */
#if GOAL_CONFIG_MEDIA_MA_CHAN_UDP == 1
        if (chanType == CHAN_UDP) {
            fnUdpCallback = (GOAL_MA_CHAN_UDP_CB_T) pEntry->fnCallback;
            fnUdpCallback(pChannel->pHdlUdp, cbType, pEntry->pChannel, pBuf);
        }
#endif

#if GOAL_CONFIG_MEDIA_MA_CHAN_TCP == 1
        if (chanType == CHAN_TCP) {
            fnTcpCallback = (GOAL_MA_CHAN_TCP_CB_T) pEntry->fnCallback;
            fnTcpCallback(pChannel->pHdlTcp, cbType, pEntry->pChannel, pBuf);
        }
#endif
    }

#if GOAL_CONFIG_MEDIA_MA_CHAN_TCP == 0 && GOAL_CONFIG_MEDIA_MA_CHAN_UDP == 0
    UNUSEDARG(chanType);
    UNUSEDARG(cbType);
#endif

    goal_queueReleaseBuf(&pBuf);

    return res;
}

