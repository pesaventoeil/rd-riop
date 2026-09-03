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

#define GOAL_ID GOAL_ID_NET_CTC
#include <goal_includes.h>
#include <goal_media/goal_ma_net.h>
#include "ctc_net_cc.h"
#include "ctc_net_rpc.h"
#include "ctc_net_list.h"

#include <goal_media/goal_mi_mctc.h>


/****************************************************************************/
/* Structs */
/****************************************************************************/
typedef struct {
    GOAL_LOCK_T *pLockChan;                     /* Channel lock */
    uint32_t channel;                           /* Channel */
    GOAL_MA_NET_T *pHdlNet;                     /* assigned NET hdl */
} GOAL_DRV_NET_CTC_T;


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_STAGE_HANDLER_T stageInit;          /**< init stage handler */
static GOAL_RPC_HDL_CHN_T *pHdlRpcChn = GOAL_RPC_HDL_NONE; /**< acyclic transmission handle */
static GOAL_DRV_NET_CHANNEL_LIST_T *pListChanUsed; /**< list of free channels */
static GOAL_DRV_NET_CHANNEL_LIST_T *pListChanFree; /**< list of used channels */


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
static GOAL_STATUS_T goal_netRpcPastInit(
    void
);

static GOAL_STATUS_T goal_netRpcFreeEntryGet(
    GOAL_NET_TYPE_T type,                       /**< channel type */
    GOAL_NET_ADDR_T *pAddr,                     /**< network address */
    GOAL_DRV_NET_CHANNEL_LIST_T **ppEntry,      /**< [out] free channel list entry */
    GOAL_DRV_NET_CHANNEL_LIST_T **ppListenerEntry, /**< [out] already opened listener channel list entry */
    GOAL_BOOL_T *pChanNew                       /**< [out] channel is new */
);

static GOAL_STATUS_T goal_netRpcSetupChannels(
    void
);

static GOAL_STATUS_T goal_netRpcRegisterServices(
    void
);

static GOAL_STATUS_T goal_netIpSetServer(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< rpc handle */
);

static GOAL_STATUS_T goal_netChanOpenServer(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< rpc handle */
);

static GOAL_STATUS_T goal_netChanCloseServer(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< rpc handle */
);

static GOAL_STATUS_T goal_netSetOptionServer(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< rpc handle */
);

static GOAL_STATUS_T goal_netChanActivateServer(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< rpc handle */
);

static GOAL_STATUS_T goal_netChanSendServer(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< rpc handle */
);

static GOAL_STATUS_T goal_netChanGetRemoteAddrServer(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< rpc handle */
);

static GOAL_STATUS_T goal_netChanReopenServer(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< rpc handle */
);

static GOAL_STATUS_T goal_netCallback(
    void *pArg,                                 /**< callback argument */
    GOAL_NET_CB_TYPE_T cbType,                  /**< callback type */
    GOAL_NET_CHAN_T *pChan,                     /**< channel descriptor */
    GOAL_BUFFER_T *pBuf                         /**< GOAL buffer */
);


/****************************************************************************/
/** Registers the RPC server for goal net NET
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_netRpcInit(
    uint32_t id                                 /**< id for MA instance */
)
{
    GOAL_STATUS_T res;                          /* result */

    UNUSEDARG(id);

    /* set  the size of the RPC stack */
    res = goal_rpcSetStackMax(RPC_BUFF_SIZE_NET);

    /* register the NET via RPC init function */
    if (GOAL_RES_OK(res)) {
        res = goal_mainStageReg(GOAL_STAGE_CTC, &stageInit, GOAL_STAGE_INIT, goal_netRpcPastInit);
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
static GOAL_STATUS_T goal_netRpcPastInit(
    void
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_DRV_NET_CHANNEL_LIST_T *pEntry = NULL; /* channel list entry */
    uint32_t cnt;                               /* count */
    uint32_t numUdpChans;                       /* number of UDP channels */
    uint32_t numTcpChans;                       /* number of TCP data/client channels */
    uint32_t numTcpListeners;                   /* number of TCP listener channels */
    uint32_t numListEntries;                    /* number of list entries */
    GOAL_NET_TYPE_T type;                       /* channel type */

    /* calculate number of channel types */
    numTcpListeners = GOAL_CONFIG_NET_CHAN_LISTEN_MAX;
    numUdpChans = GOAL_CONFIG_NET_CHAN_MAX;
    numTcpChans = (numUdpChans / 2);
    if (numUdpChans & 0x00000001) {
        /* odd number of channels, use the remainder for TCP */
        numTcpChans++;
    }
    numUdpChans /= 2;

    /* for each listener at least one data channel is needed */
    if (numTcpChans <= numTcpListeners) {
        goal_logErr("to many TCP listeners defined for defined number of NET channels");
        res = GOAL_ERROR;
    }

    numListEntries = numTcpListeners + numUdpChans + numTcpChans;

    /* add net channels list item to channel pool */
    for (cnt = 0; (cnt < numListEntries) && GOAL_RES_OK(res); cnt++) {
        res = goal_memCalloc(&pEntry, sizeof(GOAL_DRV_NET_CHANNEL_LIST_T));
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to create a channel list entry");
            break;
        }

        if (numTcpListeners) {
            type = GOAL_NET_TCP_LISTENER;
            numTcpListeners--;
        }
        else if (numTcpChans) {
            type = GOAL_NET_TCP_CLIENT;
            numTcpChans--;
        }
        else {
            type = GOAL_NET_UDP_SERVER;
        }

        res = goal_netChanCreate(&pEntry->pChannel, type, 0);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to create a NET channel for list entry");
            break;
        }

        GOAL_LL_APPEND(pListChanFree, pEntry);
    }

    /* setup the service function */
    if (GOAL_RES_OK(res)) {
        res = goal_netRpcRegisterServices();
    }

    /* setup all necessary RPC channels */
    if (GOAL_RES_OK(res)) {
        res = goal_netRpcSetupChannels();
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
static GOAL_STATUS_T goal_netRpcRegisterServices(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* setup the service function */
    res = goal_rpcRegisterService(GOAL_ID_NET, GOAL_RPC_FUNC_NET_IP_SET, &goal_netIpSetServer);

    if (GOAL_RES_OK(res)) {
        res = goal_rpcRegisterService(GOAL_ID_NET, GOAL_RPC_FUNC_NET_CHAN_NEW, &goal_netChanOpenServer);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_rpcRegisterService(GOAL_ID_NET, GOAL_RPC_FUNC_NET_CHAN_CLOSE, &goal_netChanCloseServer);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_rpcRegisterService(GOAL_ID_NET, GOAL_RPC_FUNC_NET_SET_OPTION, &goal_netSetOptionServer);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_rpcRegisterService(GOAL_ID_NET, GOAL_RPC_FUNC_NET_CHAN_ACTIVATE, &goal_netChanActivateServer);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_rpcRegisterService(GOAL_ID_NET, GOAL_RPC_FUNC_NET_CHAN_SEND, &goal_netChanSendServer);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_rpcRegisterService(GOAL_ID_NET, GOAL_RPC_FUNC_NET_CHAN_GET_REMOTE_ADDR, &goal_netChanGetRemoteAddrServer);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_rpcRegisterService(GOAL_ID_NET, GOAL_RPC_FUNC_NET_CHAN_REOPEN, &goal_netChanReopenServer);
    }

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
static GOAL_STATUS_T goal_netRpcSetupChannels(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* setup a channel for RPC */
    res = goal_rpcSetupChannel(&pHdlRpcChn, GOAL_ID_MI_CTC_DEFAULT);

    return res;
}


/****************************************************************************/
/** Get a free list entry for a given NET channel type
 *
 * If the type is GOAL_NET_TCP_LISTENER, a potential data channel is returned.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_netRpcFreeEntryGet(
    GOAL_NET_TYPE_T type,                       /**< channel type */
    GOAL_NET_ADDR_T *pAddr,                     /**< network address */
    GOAL_DRV_NET_CHANNEL_LIST_T **ppEntry,      /**< [out] free channel list entry */
    GOAL_DRV_NET_CHANNEL_LIST_T **ppListenerEntry, /**< [out] already opened listener channel list entry */
    GOAL_BOOL_T *pChanNew                       /**< [out] channel is new */
)
{
    GOAL_DRV_NET_CHANNEL_LIST_T *pEntry;        /* current list entry */
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    if ((NULL == pAddr) || (NULL == ppEntry) || (NULL == pChanNew)) {
        return GOAL_ERR_NULL_POINTER;
    }

    if ((GOAL_NET_TCP_LISTENER == type) && (NULL == ppListenerEntry)) {
        return GOAL_ERR_NULL_POINTER;
    }

    if ((GOAL_NET_UDP_SERVER != type) && (GOAL_NET_UDP_CLIENT != type) &&
        (GOAL_NET_TCP_LISTENER != type) && (GOAL_NET_TCP_CLIENT != type)) {
        return GOAL_ERR_PARAM;
    }

    *ppListenerEntry = NULL;
    *pChanNew = GOAL_TRUE;

    if (GOAL_NET_TCP_LISTENER == type) {
        /* check if a listener for the given address was already opened */
        GOAL_LL_FOREACH(pListChanUsed, pEntry) {
            if ((GOAL_NET_TCP_LISTENER == pEntry->pChannel->type) &&
                (GOAL_CMP_EQUAL == GOAL_MEMCMP(&pEntry->pChannel->addr, pAddr, sizeof(GOAL_NET_ADDR_T)))) {
                *ppListenerEntry = pEntry;
                *pChanNew = GOAL_FALSE;
                break;
            }
        }

        if (NULL == pEntry) {
            /* get a free listener channel */
            GOAL_LL_FOREACH(pListChanFree, pEntry) {
                if (GOAL_NET_TCP_LISTENER == pEntry->pChannel->type) {
                    GOAL_LL_DELETE(pListChanFree, pEntry);
                    *ppListenerEntry = pEntry;
                    break;
                }
            }
        }

        if (NULL == pEntry) {
            goal_logErr("no free listener channel");
            res = GOAL_ERR_FULL;
        }

        /* search for free data channel next */
        type = GOAL_NET_TCP_CLIENT;
    }

    if (GOAL_NET_UDP_CLIENT == type) {
        /* UDP client and server channels have the same behavior */
        type = GOAL_NET_UDP_SERVER;
    }

    if (GOAL_RES_OK(res)) {
        /* get a free channel */
        GOAL_LL_FOREACH(pListChanFree, pEntry) {
            if (type == pEntry->pChannel->type) {
                GOAL_LL_DELETE(pListChanFree, pEntry);
                *ppEntry = pEntry;
                break;
            }
        }
        if (NULL == pEntry) {
            res = GOAL_ERR_FULL;
        }
    }

    return res;
}


/****************************************************************************/
/** Set IP address - server
 *
 * Pop the data from the RPC stack, call the selected function and send a
 * response to the client.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_netIpSetServer(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< rpc handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    uint32_t addrIp = 0;                        /* IP address */
    uint32_t addrMask = 0;                      /* subnet mask */
    uint32_t addrGw = 0;                        /* gateway */
    GOAL_BOOL_T flgTemp = GOAL_FALSE;           /* temporary IP config flag */

    /* GG_LOG ID:362: */
    goal_lmLog(GOAL_ID_NET_CTC, 362, 304, 0, 0, GOAL_LOG_SEV_INFO, "called");

    GOAL_RPC_POP(addrIp, uint32_t);
    GOAL_RPC_POP(addrMask, uint32_t);
    GOAL_RPC_POP(addrGw, uint32_t);
    GOAL_RPC_POP(flgTemp, GOAL_BOOL_T);

    if (GOAL_RES_OK(res)) {
        res = goal_netIpSet(
            addrIp, addrMask, addrGw, flgTemp);
    }

    return res;
}


/****************************************************************************/
/** Open network channel - server
 *
 * Pop the data from the RPC stack, call the selected function and send a
 * response to the client.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_netChanOpenServer(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< rpc handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_NET_ADDR_T addr;                       /* network address */
    GOAL_NET_TYPE_T type = GOAL_NET_INTERNAL;   /* channel type */
    uint32_t idChan = 0;                        /* channel id */
    GOAL_DRV_NET_CHANNEL_LIST_T *pEntry = NULL; /* channel list entry */
    GOAL_DRV_NET_CHANNEL_LIST_T *pListnerEntry = NULL; /* listener channel list entry */
    GOAL_BOOL_T isChanNew = GOAL_TRUE;          /* a new channel must be opened */
    uint32_t flgOption;                         /* flag for option */
    GOAL_NET_OPTION_T option = GOAL_NET_OPTION_NONBLOCK; /* socket option */
    uint32_t optVal;                            /* value of socket option */
    uint32_t flgImmediateActivation;            /* flag for immediate activation */
    GOAL_BOOL_T flgOldVersion = GOAL_FALSE;     /* old version flag */

#if GOAL_CONFIG_MEDIA_MI_ETH == 1
    /* it is required to start networking before stack initialization */

    /* open up Ethernet MI */
    if (GOAL_RES_OK(res)) {
        res = goal_miEthOpen(GOAL_MI_ID, GOAL_MA_ID_ETH);
    }

    /* open up NET */
    if (GOAL_RES_OK(res)) {
        res = goal_miNetOpen();
    }
#endif

    /* GG_LOG ID:363: */
    goal_lmLog(GOAL_ID_NET_CTC, 363, 304, 0, 0, GOAL_LOG_SEV_INFO, "called");

    GOAL_RPC_POP(idChan, uint32_t);
    GOAL_RPC_POP(addr.localIp, uint32_t);
    GOAL_RPC_POP(addr.localPort, uint16_t);
    GOAL_RPC_POP(addr.remoteIp, uint32_t);
    GOAL_RPC_POP(addr.remotePort, uint16_t);
    GOAL_RPC_POP(type, GOAL_NET_TYPE_T);
    GOAL_RPC_POP(flgOption, uint32_t);
    if (GOAL_RES_ERR(res)) {
        /* It means that AC app is using API version 2.0 or lower */
        flgOldVersion = GOAL_TRUE;
        res = GOAL_OK;
    }

    if (GOAL_TRUE != flgOldVersion) {
        if (GOAL_TRUE == (GOAL_BOOL_T) flgOption) {
             GOAL_RPC_POP(option, GOAL_NET_OPTION_T);
             GOAL_RPC_POP(optVal, uint32_t);
        }
        GOAL_RPC_POP(flgImmediateActivation, uint32_t);

        /* immediate activation flag cannot set if connection type is TCP Listener */
        if ((GOAL_NET_TCP_LISTENER == type) && (GOAL_TRUE == (GOAL_BOOL_T) flgImmediateActivation)) {
            res = GOAL_ERR_PARAM;
        }
    }

    if (GOAL_RES_OK(res)) {
        /* channel ID should be unique */
        GOAL_LL_FOREACH(pListChanUsed, pEntry) {
            if (idChan == pEntry->id) {
                res = GOAL_ERR_PARAM;
                pEntry = NULL;
                break;
            }
        }
    }

    if (GOAL_RES_OK(res)) {
        res = goal_netRpcFreeEntryGet(type, &addr, &pEntry, &pListnerEntry, &isChanNew);
    }

    if (GOAL_RES_OK(res) && (GOAL_NET_TCP_LISTENER == type)) {
        /* convert TCP client to data channel and connect it to listener */
        pEntry->pChannel->type = GOAL_NET_TCP;
        pEntry->pChannel->pChanListener = pListnerEntry->pChannel;
        pEntry->id = idChan;
        pListnerEntry->pChannel->numDataChannels++;
        GOAL_LL_APPEND(pListChanUsed, pEntry);
        /* we might need to open listener channel */
        pEntry = pListnerEntry;
    }

    if (GOAL_TRUE == isChanNew) {
        if (GOAL_RES_OK(res)) {
            res = goal_netChanOpen(pEntry->pChannel, &addr, goal_netCallback, NULL);
        }

        if (GOAL_TRUE != flgOldVersion) {
            if (GOAL_RES_OK(res) && (GOAL_TRUE == (GOAL_BOOL_T) flgOption)) {
                res = goal_netChanOptionSet(pEntry->pChannel, option, &optVal);
            }

            if (GOAL_RES_OK(res) && (GOAL_TRUE == (GOAL_BOOL_T) flgImmediateActivation)) {
                res = goal_netChanActivate(pEntry->pChannel);
            }
        }

        if (GOAL_RES_OK(res)) {
            GOAL_LL_APPEND(pListChanUsed, pEntry);
            if (GOAL_NET_TCP_LISTENER == pEntry->pChannel->type) {
                /* entry is internal only, use a dummy ID */
                pEntry->id = UINT32_MAX - idChan;
            }
            else {
                pEntry->id = idChan;
            }
        }
    }
    else {
        if (GOAL_RES_OK(res)) {
            /* TCP Listener got a new data channel */
            res = goal_netChanReopen(pEntry->pChannel);
        }
    }

    return res;
}


/****************************************************************************/
/** Close network channel - server
 *
 * Pop the data from the RPC stack, call the selected function and send a
 * response to the client.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_netChanCloseServer(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< rpc handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    uint32_t idChan = 0;                        /* channel id */
    GOAL_DRV_NET_CHANNEL_LIST_T *pEntry = NULL; /* channel list entry */

    /* GG_LOG ID:364: */
    goal_lmLog(GOAL_ID_NET_CTC, 364, 304, 0, 0, GOAL_LOG_SEV_INFO, "called");

    GOAL_RPC_POP(idChan, uint32_t);

    if (GOAL_RES_OK(res)) {
        res = goal_drvCtcChannelEntryById(pListChanUsed, idChan, &pEntry);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_netChanClose(pEntry->pChannel);
    }

    if (GOAL_RES_OK(res)) {
        if (GOAL_NET_TCP != pEntry->pChannel->type) {
            GOAL_LL_DELETE(pListChanUsed, pEntry);
            GOAL_LL_APPEND(pListChanFree, pEntry);
        }
        /* else: TCP data channel, must be opened by remote client connect event */
    }

    return res;
}


/****************************************************************************/
/** Open network channel - server
 *
 * Pop the data from the RPC stack, call the selected function and send a
 * response to the client.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_netSetOptionServer(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< rpc handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    uint32_t idChan = 0;                        /* channel id */
    uint32_t value = 0;                         /* option value */
    GOAL_NET_OPTION_T option = GOAL_NET_OPTION_NONBLOCK; /* socket option */
    GOAL_DRV_NET_CHANNEL_LIST_T *pEntry = NULL; /* channel list entry */

    /* GG_LOG ID:365: */
    goal_lmLog(GOAL_ID_NET_CTC, 365, 304, 0, 0, GOAL_LOG_SEV_INFO, "called");

    GOAL_RPC_POP(idChan, uint32_t);
    GOAL_RPC_POP(option, GOAL_NET_OPTION_T);
    GOAL_RPC_POP(value, uint32_t);

    if (GOAL_RES_OK(res)) {
        res = goal_drvCtcChannelEntryById(pListChanUsed, idChan, &pEntry);
    }

    if (GOAL_RES_OK(res)) {
        if ((GOAL_NET_TCP == pEntry->pChannel->type) && (0 == (pEntry->pChannel->flags & GOAL_NET_FLAGS_USED))) {
            /* not yet opened TCP data channel --> apply option to listener */
            res = goal_netChanOptionSet(pEntry->pChannel->pChanListener, option, &value);
        }
        else {
            res = goal_netChanOptionSet(pEntry->pChannel, option, &value);
        }
    }

    return res;
}


/****************************************************************************/
/** Activate network channel - server
 *
 * Pop the data from the RPC stack, call the selected function and send a
 * response to the client.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_netChanActivateServer(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< rpc handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    uint32_t idChan = 0;                        /* channel id */
    GOAL_DRV_NET_CHANNEL_LIST_T *pEntry = NULL; /* channel list entry */

    /* GG_LOG ID:366: */
    goal_lmLog(GOAL_ID_NET_CTC, 366, 304, 0, 0, GOAL_LOG_SEV_INFO, "called");

    GOAL_RPC_POP(idChan, uint32_t);

    if (GOAL_RES_OK(res)) {
        res = goal_drvCtcChannelEntryById(pListChanUsed, idChan, &pEntry);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_netChanActivate(pEntry->pChannel);
    }

    return res;
}


/****************************************************************************/
/** Reopen network channel - server
 *
 * Pop the data from the RPC stack, call the selected function and send a
 * response to the client.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_netChanReopenServer(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< rpc handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    uint32_t immediateActivation;               /* immediate Activation */
    uint32_t idChan;                            /* channel ID */
    GOAL_DRV_NET_CHANNEL_LIST_T *pEntry = NULL; /* channel list entry */

    GOAL_RPC_POP(immediateActivation, uint32_t);
    GOAL_RPC_POP(idChan, uint32_t);

    if (GOAL_RES_OK(res)) {
        res = goal_drvCtcChannelEntryById(pListChanUsed, idChan, &pEntry);
    }

    if (GOAL_RES_OK(res)) {
        /* set immediate activation value to non-zero if channel type is not TCP server and channel is active */
        if ((GOAL_NET_TCP != pEntry->pChannel->type) && (pEntry->pChannel->flags & GOAL_NET_FLAGS_ACTIVE)) {
            immediateActivation = 1;
        }
    }

    /* reopen channel */
    res = goal_netChanReopen(pEntry->pChannel);

    if (GOAL_RES_OK(res)) {
        if (GOAL_TRUE == (GOAL_BOOL_T) immediateActivation) {
            res = goal_netChanActivate(pEntry->pChannel);
        }
    }

    if (GOAL_RES_ERR(res)) {
        res = GOAL_ERR_NET_REOPEN;
    }

    return res;
}


/****************************************************************************/
/** Get channel remote address - server
 *
 * Pop the data from the RPC stack, call the selected function and send a
 * response to the client.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_netChanGetRemoteAddrServer(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< rpc handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    uint32_t idChan = 0;                        /* channel id */
    GOAL_NET_ADDR_T addr;                       /* remote address */
    GOAL_DRV_NET_CHANNEL_LIST_T *pEntry = NULL; /* channel list entry */

    /* GG_LOG ID:367: */
    goal_lmLog(GOAL_ID_NET_CTC, 367, 304, 0, 0, GOAL_LOG_SEV_INFO, "called");

    GOAL_RPC_POP(idChan, uint32_t);

    if (GOAL_RES_OK(res)) {
        res = goal_drvCtcChannelEntryById(pListChanUsed, idChan, &pEntry);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_netChanRemoteAddrGet(pEntry->pChannel, &addr);
    }

    GOAL_RPC_PUSH(addr.remotePort);
    GOAL_RPC_PUSH(addr.remoteIp);
    GOAL_RPC_PUSH(addr.localPort);
    GOAL_RPC_PUSH(addr.localIp);

    return res;
}


/****************************************************************************/
/** Net send - server
 *
 * Pop the data from the RPC stack, call the selected function and send a
 * response to the client.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_netChanSendServer(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< rpc handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    uint32_t idChan = 0;                        /* channel id */
    uint16_t dataLen = 0;                       /* data length */
    GOAL_BUFFER_T *pBuf = NULL;                 /* buffer */
    GOAL_DRV_NET_CHANNEL_LIST_T *pEntry = NULL; /* channel list entry */

    GOAL_RPC_POP(idChan, uint32_t);
    GOAL_RPC_POP(dataLen, uint16_t);

    if (GOAL_RES_OK(res)) {
       res = goal_queuePoolGetBuf(&pBuf, dataLen, GOAL_ID_NET);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_rpcArgPop(pHdlRpc, pBuf->ptrData, dataLen);
        pBuf->dataLen = dataLen;
    }

    if (GOAL_RES_OK(res)) {
        res = goal_drvCtcChannelEntryById(pListChanUsed, idChan, &pEntry);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_netChanSend(pEntry->pChannel, pBuf);
    }

    goal_queueReleaseBuf(&pBuf);

    return res;
}


/****************************************************************************/
/** Net Callback
 *
 */
static GOAL_STATUS_T goal_netCallback(
    void *pArg,                                 /**< callback argument */
    GOAL_NET_CB_TYPE_T cbType,                  /**< callback type */
    GOAL_NET_CHAN_T *pChan,                     /**< channel descriptor */
    GOAL_BUFFER_T *pBuf                         /**< GOAL buffer */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE;/* call handle */
    GOAL_DRV_NET_CHANNEL_LIST_T *pEntry;        /* channel list entry */
    uint32_t idChan = 0;                        /* channel id */
    uint32_t chanType = 0;                      /* channel type */

    UNUSEDARG(pArg);

    GOAL_RPC_NEW();

    if (NULL != pBuf) {
        /* push the data to the stack */
        if (GOAL_RES_OK(res)) {
            res = goal_rpcArgPush(pHdlRpc, pBuf->ptrData, (uint16_t) pBuf->dataLen);
        }

        /* push the data length to the stack */
        GOAL_RPC_PUSH(pBuf->dataLen);
    }
    else {
        GOAL_RPC_PUSH(0);
    }

    res = goal_drvCtcChannelEntry(pListChanUsed, pChan, &pEntry);
    if (GOAL_RES_OK(res)) {
        idChan = pEntry->id;
    }

    if ((pChan->type == GOAL_NET_UDP_SERVER) || (pChan->type == GOAL_NET_UDP_CLIENT)) {
        chanType = CHAN_UDP;
    }
    else {
        chanType = CHAN_TCP;
    }

    GOAL_RPC_PUSH(chanType);
    GOAL_RPC_PUSH(idChan);
    GOAL_RPC_PUSH(cbType);

    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_NET, GOAL_RPC_FUNC_NET_CHAN_CB);
    }

    GOAL_RPC_CLOSE();

    return GOAL_OK;
}
