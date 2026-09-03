/** @file
 *
 * @brief DHCP Client
 *
 * @details
 * This module implements a DHCPv4 client.
 *
 * @copyright
 * Copyright 2016-2020.
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

#define GOAL_ID GOAL_ID_DHCP
#include <goal_includes.h>
#include "cm/goal_net_cm.h"


/****************************************************************************/
/* Local defines */
/****************************************************************************/
/** DHCP frame field offsets */
#define DHCP_FIELD_OP       0                   /**< DHCP frame field "op" */
#define DHCP_FIELD_HTYPE    1                   /**< DHCP frame field "htype" */
#define DHCP_FIELD_HLEN     2                   /**< DHCP frame field "hlen" */
#define DHCP_FIELD_HOPS     3                   /**< DHCP frame field "hops" */
#define DHCP_FIELD_XID      4                   /**< DHCP frame field "xid" */
#define DHCP_FIELD_SECS     8                   /**< DHCP frame field "secs" */
#define DHCP_FIELD_FLAGS    10                  /**< DHCP frame field "flags" */
#define DHCP_FIELD_CIADDR   12                  /**< DHCP frame field "ciaddr" */
#define DHCP_FIELD_YIADDR   16                  /**< DHCP frame field "yiaddr" */
#define DHCP_FIELD_SIADDR   20                  /**< DHCP frame field "siaddr" */
#define DHCP_FIELD_GIADDR   24                  /**< DHCP frame field "giaddr" */
#define DHCP_FIELD_CHADDR   28                  /**< DHCP frame field "chaddr" */
#define DHCP_FIELD_PAD      34                  /**< rest of chaddr + sname + file */
#define DHCP_FIELD_SNAME    44                  /**< DHCP frame field "sname" */
#define DHCP_FIELD_FILE     108                 /**< DHCP frame field "file" */
#define DHCP_FIELD_COOKIE   236                 /**< cookie position in "options" */
#define DHCP_FIELD_OPTIONS  240                 /**< DHCP frame field "options" */

/** DHCP frame field values */
#define DHCP_OP_BOOTREQ     1                   /**< BOOTP Boot Request */
#define DHCP_OP_BOOTRES     2                   /**< BOOTP Boot Reply */
#define DHCP_COOKIE         0x63825363          /**< magic cookie */
#define DHCP_HTYPE_OTHER    0                   /**< hardware type: Other */
#define DHCP_HTYPE_ETH      1                   /**< hardware type: Ethernet */
#define DHCP_HLEN_ETH       6                   /**< hardware address lenght: Ethernet */
#define DHCP_FLAG_BCAST     (1<<15)             /**< Broadcast flag */
#define DHCP_XID_INIT       0xabcdef00          /**< initial xid value */

/** DHCP frame lengths */
#define DHCP_LEN_PAD        202                 /**< lenght of pad bytes */
#define DHCP_LEN_HEADER     240                 /**< lenght of DHCP header */

/** DHCP option fields */
#define DHCP_OPT_ID         0                   /**< option Id */
#define DHCP_OPT_LEN        1                   /**< option length */
#define DHCP_OPT_DATA       2                   /**< option data */

/** DHCP Option codes */
#define DHCP_OPT_ID_PAD         0               /**< padding */
#define DHCP_OPT_ID_NETMASK     1               /**< Subnet Mask */
#define DHCP_OPT_ID_GATEWAY     3               /**< Gateway address */
#define DHCP_OPT_ID_DNS_SERVER  6               /**< DNS Server address */
#define DHCP_OPT_ID_HOSTNAME   12               /**< Host Name */
#define DHCP_OPT_ID_DOMAINNAME 15               /**< Domain Name */
#define DHCP_OPT_ID_REQ_IPADDR 50               /**< requested IP address */
#define DHCP_OPT_ID_LEASE_TIME 51               /**< IP Lease time */
#define DHCP_OPT_ID_MSG_TYPE   53               /**< DHCP Message type */
#define DHCP_OPT_ID_SERVER_ID  54               /**< Server Identifier */
#define DHCP_OPT_ID_PARAM_LIST 55               /**< Parameter Request list */
#define DHCP_OPT_ID_T1_VAL     58               /**< Renewal Time (T1) */
#define DHCP_OPT_ID_T2_VAL     59               /**< Rebinding Time (T2) */
#define DHCP_OPT_ID_CLIENT_ID  61               /**< Client Identifier */
#define DHCP_OPT_ID_END       255               /**< End marker */

/** DHCP Message Types */
#define DHCP_MSG_TYPE_DISCOVER 1                /**< DHCPDISCOVER */
#define DHCP_MSG_TYPE_OFFER    2                /**< DHCPOFFER */
#define DHCP_MSG_TYPE_REQUEST  3                /**< DHCPREQUEST */
#define DHCP_MSG_TYPE_DECLINE  4                /**< DHCPDECLINE */
#define DHCP_MSG_TYPE_ACK      5                /**< DHCPACK */
#define DHCP_MSG_TYPE_NAK      6                /**< DHCPNAK */
#define DHCP_MSG_TYPE_RELEASE  7                /**< DHCPRELEASE */
#define DHCP_MSG_TYPE_INFORM   8                /**< DHCPINFORM */

/** port numbers */
#define DHCP_CLIENT_PORT      68                /**< DHCP client port */
#define DHCP_SERVER_PORT      67                /**< DHCP server port */

/** Timeouts in ms */
#define DHCP_TIMEOUT_OFF 0xffffffffffffffff     /**< timeout timer is off */
#define DHCP_TIMEOUT_DISCOVER    2000           /**< rx timeout after sending DISCOVER */
#define DHCP_TIMEOUT_REQUEST     10000          /**< rx timeout after sending REQUEST */
#define DHCP_TIMEOUT_RETRANS_MAX 64000          /**< maximum timeout for retransmission */

#define DHCP_TIME_INFINITE 0xffffffff           /**< infinite timeout value */

/** Retransmission multiplier for timeouts */
#define DHCP_RETRANS_MULTIPL_MAX 5              /**< maximum retransm. multiplier */


/****************************************************************************/
/* Local data types */
/****************************************************************************/
/** DHCP client states */
typedef uint8_t DHCP_STATE_T;
#define DHCP_STATE_INIT       0                 /**< initial state */
#define DHCP_STATE_SELECTING  1                 /**< selecting DHCP server */
#define DHCP_STATE_REQUESTING 2                 /**< requesting configuration */
#define DHCP_STATE_BOUND      3                 /**< iface bound to ip address */
#define DHCP_STATE_RENEWING   4                 /**< renewing configuration */
#define DHCP_STATE_REBINDING  5                 /**< rebinding ip address to iface */

/** DHCP client events */
typedef enum {
    DHCP_EVENT_START,                           /**< start DHCP Discovery */
    DHCP_EVENT_STOP,                            /**< abort DHCP process */
    DHCP_EVENT_TIMEOUT,                         /**< receive timeout */
    DHCP_EVENT_OFFER_RX,                        /**< DHCP OFFER received */
    DHCP_EVENT_ACK_RX,                          /**< DHCP ACK received */
    DHCP_EVENT_NACK_RX,                         /**< DHCP NACK received */
    DHCP_EVENT_T1_EXPIRED,                      /**< T1 timer expired */
    DHCP_EVENT_T2_EXPIRED,                      /**< T2 timer expired */
    DHCP_EVENT_LEASE_EXPIRED,                   /**< IP address leasing expired */
} DHCP_EVENT_T;


/****************************************************************************/
/* Local functions */
/****************************************************************************/
static void goal_netDhcpLoop(
    void *pArg                                  /**< MI NET handle */
);

static GOAL_STATUS_T goal_netDhcpReceiveCb(
    void *pArg,                                 /**< callback argument */
    GOAL_NET_CB_TYPE_T cbType,                  /**< callback type */
    GOAL_NET_CHAN_T *pChan,                     /**< channel descriptor */
    GOAL_BUFFER_T *pBuf                         /**< GOAL buffer */
);

static GOAL_STATUS_T goal_netDhcpEventThrown(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI handle */
    DHCP_EVENT_T event                          /**< event Id */
);

static GOAL_STATUS_T goal_netDhcpTxDiscover(
    GOAL_MI_NET_T *pMiNetHdl                    /**< MI handle */
);

static GOAL_STATUS_T goal_netDhcpTxRequest(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI handle */
    GOAL_BOOL_T renewing                        /**< renew active lease */
);

static void goal_netDhcpTxTimeGet(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI handle */
    uint64_t timeout                            /**< original timeout in ms */
);

static void goal_netDhcpCreateHeader(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI handle */
    uint8_t *pData,                             /**< frame data */
    uint16_t *pLen                              /**< current frame length */
);

static void goal_netDhcpTimeCalc(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI NET handle */
    uint32_t leaseTime,                         /**< DHCP lease time in s */
    uint32_t renewTime,                         /**< DHCP renewing time in s */
    uint32_t rebindTime                         /**< DHCP rebinding time in s */
);

static void goal_netDhcpIpLost(
    GOAL_MI_NET_T *pMiNetHdl                    /**< MI NET handle */
);

static GOAL_STATUS_T goal_netDhcpIpSet(
    GOAL_MI_NET_T *pMiNetHdl                    /**< MI NET handle */
);

static GOAL_STATUS_T goal_netDhcpStateSet(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI NET handle */
    DHCP_STATE_T state                          /**< DHCP enable */
);


/****************************************************************************/
/** Initialize the DHCP client
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_netDhcpInit(
    GOAL_MI_NET_T *pMiNetHdl                    /**< MI NET handle */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_NET_ADDR_T netAddr;                    /* local net address */
    int optVal;                                 /* socket option value */

    if (NULL == pMiNetHdl) {
        /* get handle of default interface */
        res = goal_instGetById((GOAL_INSTANCE_T **) &pMiNetHdl, GOAL_ID_NET, GOAL_ID_DEFAULT);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
    }

    pMiNetHdl->dhcpData.state = DHCP_STATE_INIT;
    pMiNetHdl->dhcpData.flgInit = GOAL_FALSE;
    pMiNetHdl->dhcpData.flgActive = GOAL_FALSE;
    pMiNetHdl->dhcpData.pChan = NULL;
    pMiNetHdl->dhcpData.timeout = DHCP_TIMEOUT_OFF;
    pMiNetHdl->dhcpData.xid = DHCP_XID_INIT;
    pMiNetHdl->dhcpData.serverAddr = 0xffffffff;
    pMiNetHdl->dhcpData.leaseTime = DHCP_TIMEOUT_OFF;
    pMiNetHdl->dhcpData.renewTime = DHCP_TIMEOUT_OFF;
    pMiNetHdl->dhcpData.rebindTime = DHCP_TIMEOUT_OFF;
    pMiNetHdl->dhcpData.prevDhcpState = DHCP_STATE_INIT;
    pMiNetHdl->dhcpData.retransCount = 0;

    GOAL_MEMSET(&netAddr, 0, sizeof(GOAL_NET_ADDR_T));
    netAddr.localPort = DHCP_CLIENT_PORT;

    /* DHCP requires one buffer temporarily for protocol execution */
    res = goal_queuePoolBufsReq(GOAL_ID_DHCP, GOAL_NETBUF_SIZE, 0, 1);
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:199: */
        goal_lmLog(GOAL_ID_DHCP, 199, 159, 0, 0, GOAL_LOG_SEV_ERROR, "failed to request network buffers");
        return res;
    }

    /* create NET channel */
    res = goal_netChanCreate(&pMiNetHdl->dhcpData.pChan, GOAL_NET_UDP_CLIENT, 0);
    if (GOAL_RES_OK(res)) {
        /* open socket */
        res = goal_netChanOpen(pMiNetHdl->dhcpData.pChan, &netAddr, goal_netDhcpReceiveCb, pMiNetHdl);
    }
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:200: */
        goal_lmLog(GOAL_ID_DHCP, 200, 160, 0, 0, GOAL_LOG_SEV_ERROR, "failed to allocate socket");
        return res;
    }

    /* make socket non-blocking */
    optVal = 1;
    res = goal_netChanOptionSet(pMiNetHdl->dhcpData.pChan, GOAL_NET_OPTION_NONBLOCK, &optVal);
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:201: */
        goal_lmLog(GOAL_ID_DHCP, 201, 161, 0, 0, GOAL_LOG_SEV_ERROR, "failed to set socket to non-blocking");
        goal_netChanClose(pMiNetHdl->dhcpData.pChan);
        return res;
    }

    /* make socket reusable */
    optVal = 1;
    res = goal_netChanOptionSet(pMiNetHdl->dhcpData.pChan, GOAL_NET_OPTION_REUSEADDR, &optVal);
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:202: */
        goal_lmLog(GOAL_ID_DHCP, 202, 162, 0, 0, GOAL_LOG_SEV_ERROR, "failed to make socket reusable");
        goal_netChanClose(pMiNetHdl->dhcpData.pChan);
        return res;
    }

    /* allow socket to receive broadcast messages */
    optVal = 1;
    res = goal_netChanOptionSet(pMiNetHdl->dhcpData.pChan, GOAL_NET_OPTION_BROADCAST, &optVal);
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:203: */
        goal_lmLog(GOAL_ID_DHCP, 203, 163, 0, 0, GOAL_LOG_SEV_ERROR, "failed to enable Broadcast for socket");
        goal_netChanClose(pMiNetHdl->dhcpData.pChan);
        return res;
    }

    goal_netChanActivate(pMiNetHdl->dhcpData.pChan);

    res = goal_ethCmd(GOAL_ETH_CMD_MAC_ADDR, GOAL_FALSE, 0, &pMiNetHdl->dhcpData.macAddr);
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:204: */
        goal_lmLog(GOAL_ID_DHCP, 204, 113, 0, 0, GOAL_LOG_SEV_ERROR, "failed to get MAC address");
        goal_netChanClose(pMiNetHdl->dhcpData.pChan);
        return res;
    }

    res = goal_mainLoopParamReg(goal_netDhcpLoop, pMiNetHdl);
    if (GOAL_RES_OK(res)) {
        res = goal_netDhcpStateSet(pMiNetHdl, DHCP_STATE_INIT);
    }

    pMiNetHdl->dhcpData.flgInit = GOAL_TRUE;

    return GOAL_OK;
}


/****************************************************************************/
/** Shut down the DHCP client
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_netDhcpShutdown(
    GOAL_MI_NET_T *pMiNetHdl                    /**< MI NET handle */
)
{
    GOAL_STATUS_T res;                          /* result */

    if (NULL == pMiNetHdl) {
        /* get handle of default interface */
        res = goal_instGetById((GOAL_INSTANCE_T **) &pMiNetHdl, GOAL_ID_NET, GOAL_ID_DEFAULT);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
    }

    if (GOAL_TRUE == pMiNetHdl->dhcpData.flgInit) {
        goal_netChanClose(pMiNetHdl->dhcpData.pChan);
        goal_netChanDelete(pMiNetHdl->dhcpData.pChan);
        pMiNetHdl->dhcpData.flgInit = GOAL_FALSE;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Start the DHCP client
 *
 * This function triggers the DHCP client to send DHCPDISCOVER messages until a
 * response was received.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_netDhcpStart(
    GOAL_MI_NET_T *pMiNetHdl                    /**< MI NET handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    if (NULL == pMiNetHdl) {
        /* get handle of default interface */
        res = goal_instGetById((GOAL_INSTANCE_T **) &pMiNetHdl, GOAL_ID_NET, GOAL_ID_DEFAULT);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
    }

    if (GOAL_FALSE == pMiNetHdl->dhcpData.flgInit) {
        res = goal_netDhcpInit(pMiNetHdl);
    }

    if (GOAL_RES_ERR(res)) {
        return res;
    }

    return goal_netDhcpEventThrown(pMiNetHdl, DHCP_EVENT_START);
}


/****************************************************************************/
/** Stop the DHCP client
 *
 * This function stops the DHCP client from obtaining an IP address. If the the
 * client is in the state DHCP_STATE_BOUND or higher this function has no effect.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_netDhcpStop(
    GOAL_MI_NET_T *pMiNetHdl                    /**< MI NET handle */
)
{
    GOAL_STATUS_T res;                          /* result */

    if (NULL == pMiNetHdl) {
        /* get handle of default interface */
        res = goal_instGetById((GOAL_INSTANCE_T **) &pMiNetHdl, GOAL_ID_NET, GOAL_ID_DEFAULT);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
    }

    if (GOAL_FALSE == pMiNetHdl->dhcpData.flgInit) {
        return GOAL_ERROR;
    }

    return goal_netDhcpEventThrown(pMiNetHdl, DHCP_EVENT_STOP);
}


/****************************************************************************/
/** Release the current IP Address
 *
 * This function also removes the IP address from the TCP/IP stack.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_netDhcpRelease(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI NET handle */
    GOAL_BOOL_T *pReleaseTx                     /**< send RELEASE message */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_BUFFER_T *pTxBuf = NULL;               /* send buffer */
    uint8_t *pData;                             /* current byte in frame */
    GOAL_NET_DHCP_TX_DATA_T txData;             /* DHCP Tx Data */

    if (NULL == pMiNetHdl) {
        /* get handle of default interface */
        res = goal_instGetById((GOAL_INSTANCE_T **) &pMiNetHdl, GOAL_ID_NET, GOAL_ID_DEFAULT);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
    }

    if (GOAL_FALSE == pMiNetHdl->dhcpData.flgInit) {
        return GOAL_ERROR;
    }

    if (DHCP_STATE_INIT == pMiNetHdl->dhcpData.state) {
        /* client has not started */
        return GOAL_OK;
    }

    if ((DHCP_STATE_BOUND <= pMiNetHdl->dhcpData.state) && (GOAL_TRUE == *pReleaseTx)) {
        /* get send buffer */
        res = goal_ethGetNetBuf(&pTxBuf);
        if (GOAL_RES_ERR(res)) {
            /* GG_LOG ID:205: */
            goal_lmLog(GOAL_ID_DHCP, 205, 164, 0, 0, GOAL_LOG_SEV_ERROR, "no free buffer");
            return res;
        }

        goal_netDhcpCreateHeader(pMiNetHdl, pTxBuf->ptrData, &pTxBuf->dataLen);

        GOAL_htobe32_p(&pTxBuf->ptrData[DHCP_FIELD_CIADDR], pMiNetHdl->dhcpData.data.addrIp);

        pData = &pTxBuf->ptrData[DHCP_FIELD_OPTIONS];

        /* add option DHCP Message Type: RELEASE */
        pData[DHCP_OPT_ID] = DHCP_OPT_ID_MSG_TYPE;
        pData[DHCP_OPT_LEN] = 1;
        pData[DHCP_OPT_DATA] = DHCP_MSG_TYPE_RELEASE;
        pData += 3;
        pTxBuf->dataLen += 3;

        /* add end marker */
        *pData = DHCP_OPT_ID_END;
        pTxBuf->dataLen++;

        /* send frame */
        pMiNetHdl->dhcpData.pChan->addr.remotePort = DHCP_SERVER_PORT;
        pMiNetHdl->dhcpData.pChan->addr.remoteIp = pMiNetHdl->dhcpData.serverAddr;

        txData.pChan = pMiNetHdl->dhcpData.pChan;
        txData.pBuf = pTxBuf;
        res = goal_miNetCmd(pMiNetHdl, GOAL_NET_CMD_DHCP_SEND, GOAL_TRUE, &txData);

        goal_queueReleaseBuf(&pTxBuf);
    }

    goal_netDhcpStateSet(pMiNetHdl, DHCP_STATE_INIT);
    pMiNetHdl->dhcpData.timeout = DHCP_TIMEOUT_OFF;
    pMiNetHdl->dhcpData.serverAddr = 0xffffffff;

    return goal_miNetIpSet(pMiNetHdl, 0, 0, 0, GOAL_FALSE);
}


/****************************************************************************/
/** Get the address data obtained via DHCP
 *
 * This function only succeeds if the client is in the state DHCP_STATE_BOUND
 * or above.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_netDhcpGetAddr(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI NET handle */
    GOAL_NET_DHCP_T *pAddrData                  /**< DHCP address data */
)
{
    GOAL_STATUS_T res;                          /* result */

    if (NULL == pMiNetHdl) {
        /* get handle of default interface */
        res = goal_instGetById((GOAL_INSTANCE_T **) &pMiNetHdl, GOAL_ID_NET, GOAL_ID_DEFAULT);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
    }

    if (GOAL_FALSE == pMiNetHdl->dhcpData.flgInit) {
        return GOAL_ERROR;
    }

    if (DHCP_STATE_BOUND > pMiNetHdl->dhcpData.state) {
        return GOAL_ERR_NET_DHCP_ADDR;
    }

    GOAL_MEMCPY(pAddrData, &pMiNetHdl->dhcpData.data, sizeof(GOAL_NET_DHCP_T));

    return GOAL_OK;
}


/****************************************************************************/
/** Get the current state of the DHCP client
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_netDhcpGetState(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI NET handle */
    uint32_t *pState                            /**< current state */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    if (NULL == pMiNetHdl) {
        /* get handle of default interface */
        res = goal_instGetById((GOAL_INSTANCE_T **) &pMiNetHdl, GOAL_ID_NET, GOAL_ID_DEFAULT);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
    }

    switch (pMiNetHdl->dhcpData.state) {
        case DHCP_STATE_INIT:
            *pState = GOAL_NET_DHCP_STATE_DISABLED;
            break;

        case DHCP_STATE_SELECTING:
        case DHCP_STATE_REQUESTING:
            *pState = GOAL_NET_DHCP_STATE_PROGRESS;
            break;

        case DHCP_STATE_BOUND:
        case DHCP_STATE_RENEWING:
        case DHCP_STATE_REBINDING:
            *pState = GOAL_NET_DHCP_STATE_BOUND;
            break;

        default:
            goal_logErr("invalid state: %u", pMiNetHdl->dhcpData.state);
            res = GOAL_ERROR;
    }

    return res;
}


/****************************************************************************/
/** Set the current client ID
 *
 * This function sets the DHCP option 61, client ID
 * with a maximum length of GOAL_NET_DHCP_CLIENTID_LEN - 1.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_netDhcpClientIdSet(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI NET handle */
    char *pStrName,                             /**< client id */
    uint8_t lenName,                            /**< client id length */
    GOAL_BOOL_T flgPerm                         /**< permanent flag */
)
{
    GOAL_STATUS_T res;                          /* result */

    UNUSEDARG(flgPerm);

    if (NULL == pMiNetHdl) {
        /* get handle of default interface */
        res = goal_instGetById((GOAL_INSTANCE_T **) &pMiNetHdl, GOAL_ID_NET, GOAL_ID_DEFAULT);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
    }

    /* sanity check */
    if (GOAL_NET_DHCP_CLIENTID_LEN < lenName + 1) {
        return GOAL_ERR_PARAM;
    }

    /* setting not allowed if already in progress */
    if (DHCP_STATE_SELECTING < pMiNetHdl->dhcpData.state) {
        goal_logErr("setting client id is not allowed if DHCP in progress");
        return GOAL_ERR_WRONG_STATE;
    }

    /* NULL pointer only allowed for clearing */
    if ((NULL == pStrName) && (0 < lenName)) {
        return GOAL_ERR_PARAM;
    }

    /* also add null terminator to string */
    GOAL_MEMCPY(pMiNetHdl->dhcpData.clientId, pStrName, lenName);
    pMiNetHdl->dhcpData.clientId[lenName] = '\0';

    return GOAL_OK;
}


/****************************************************************************/
/** DHCP cyclic loop
 *
 * This function must be called cyclically to check for timeouts.
 */
static void goal_netDhcpLoop(
    void *pArg                                  /**< MI NET handle */
)
{
    GOAL_MI_NET_T *pMiNetHdl;                   /* MI NET handle */

    pMiNetHdl = (GOAL_MI_NET_T *) pArg;

    /* check for timeout */
    if ((DHCP_TIMEOUT_OFF != pMiNetHdl->dhcpData.timeout) &&
        (pMiNetHdl->dhcpData.timeout <= goal_timerTsGet())) {
        goal_netDhcpEventThrown(pMiNetHdl, DHCP_EVENT_TIMEOUT);
    }

    if (DHCP_STATE_BOUND == pMiNetHdl->dhcpData.state) {
        if ((pMiNetHdl->dhcpData.renewTime != DHCP_TIMEOUT_OFF) && (pMiNetHdl->dhcpData.renewTime <= goal_timerTsGet())) {
            goal_netDhcpEventThrown(pMiNetHdl, DHCP_EVENT_T1_EXPIRED);
        }
    }
    else if (DHCP_STATE_RENEWING == pMiNetHdl->dhcpData.state) {
        if ((pMiNetHdl->dhcpData.rebindTime != DHCP_TIMEOUT_OFF) && (pMiNetHdl->dhcpData.rebindTime <= goal_timerTsGet())) {
            goal_netDhcpEventThrown(pMiNetHdl, DHCP_EVENT_T2_EXPIRED);
        }
    }
    else if (DHCP_STATE_REBINDING == pMiNetHdl->dhcpData.state) {
        if ((pMiNetHdl->dhcpData.leaseTime != DHCP_TIMEOUT_OFF) && (pMiNetHdl->dhcpData.leaseTime <= goal_timerTsGet())) {
            goal_netDhcpEventThrown(pMiNetHdl, DHCP_EVENT_LEASE_EXPIRED);
        }
    }
}


/****************************************************************************/
/** Process a DHCP event
 *
 * This function is called to process a DHCP event. The event might change the
 * DHCP state and thus the behavior of the DHCP client.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_netDhcpEventThrown(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI handle */
    DHCP_EVENT_T event                          /**< event Id */
)
{
    GOAL_STATUS_T res = GOAL_ERROR;             /* result */

    switch (pMiNetHdl->dhcpData.state) {
        case DHCP_STATE_INIT:
            if (DHCP_EVENT_START == event) {
                res = goal_netDhcpTxDiscover(pMiNetHdl);
                if (GOAL_RES_ERR(res)) {
                    return res;
                }
                goal_netDhcpStateSet(pMiNetHdl, DHCP_STATE_SELECTING);
                goal_netDhcpTxTimeGet(pMiNetHdl, DHCP_TIMEOUT_DISCOVER);
                pMiNetHdl->dhcpData.flgActive = GOAL_TRUE;
                res = GOAL_OK;
            }
            break;

        case DHCP_STATE_SELECTING:
            if (DHCP_EVENT_TIMEOUT == event) {
                res = goal_netDhcpTxDiscover(pMiNetHdl);
                if (GOAL_RES_ERR(res)) {
                    return res;
                }
                goal_netDhcpTxTimeGet(pMiNetHdl, DHCP_TIMEOUT_DISCOVER);
                res = GOAL_OK;
            }
            else if (DHCP_EVENT_STOP == event) {
                goal_netDhcpStateSet(pMiNetHdl, DHCP_STATE_INIT);
                pMiNetHdl->dhcpData.timeout = DHCP_TIMEOUT_OFF;
                pMiNetHdl->dhcpData.flgActive = GOAL_FALSE;
                res = GOAL_OK;
            }
            else if (DHCP_EVENT_OFFER_RX == event) {
                /* check offer for validity, ignore if not */
                res = goal_miNetIpValidCheck(pMiNetHdl,
                                             pMiNetHdl->dhcpData.data.addrIp,
                                             pMiNetHdl->dhcpData.data.addrNetmask,
                                             pMiNetHdl->dhcpData.data.addrGateway,
                                             GOAL_TRUE);
                if (GOAL_RES_OK(res)) {
                    res = goal_netDhcpTxRequest(pMiNetHdl, GOAL_FALSE);
                    if (GOAL_RES_ERR(res)) {
                        return res;
                    }
                    goal_netDhcpStateSet(pMiNetHdl, DHCP_STATE_REQUESTING);
                    goal_netDhcpTxTimeGet(pMiNetHdl, DHCP_TIMEOUT_REQUEST);
                }
                res = GOAL_OK;
            }
            break;

        case DHCP_STATE_REQUESTING:
            if (DHCP_EVENT_TIMEOUT == event) {
                res = goal_netDhcpTxDiscover(pMiNetHdl);
                if (GOAL_RES_ERR(res)) {
                    return res;
                }
                goal_netDhcpStateSet(pMiNetHdl, DHCP_STATE_SELECTING);
                goal_netDhcpTxTimeGet(pMiNetHdl, DHCP_TIMEOUT_DISCOVER);
                res = GOAL_OK;
            }
            else if (DHCP_EVENT_STOP == event) {
                goal_netDhcpStateSet(pMiNetHdl, DHCP_STATE_INIT);
                pMiNetHdl->dhcpData.timeout = DHCP_TIMEOUT_OFF;
                res = GOAL_OK;
            }
            else if (DHCP_EVENT_ACK_RX == event) {

                /* apply IP settings */
                res = goal_netDhcpIpSet(pMiNetHdl);

                goal_netDhcpStateSet(pMiNetHdl, DHCP_STATE_BOUND);
                pMiNetHdl->dhcpData.timeout = DHCP_TIMEOUT_OFF;
            }
            else if (DHCP_EVENT_NACK_RX == event) {
                /* GG_LOG ID:206: */
                goal_lmLog(GOAL_ID_DHCP, 206, 165, 0, 0, GOAL_LOG_SEV_ERROR, "DHCP Request rejected by server");
                goal_netDhcpStateSet(pMiNetHdl, DHCP_STATE_INIT);
                pMiNetHdl->dhcpData.timeout = DHCP_TIMEOUT_OFF;
                res = GOAL_OK;
            }
            break;

        case DHCP_STATE_BOUND:
            if (DHCP_EVENT_T1_EXPIRED == event) {
                res = goal_netDhcpTxRequest(pMiNetHdl, GOAL_TRUE);
                if (GOAL_RES_ERR(res)) {
                    return res;
                }

                goal_netDhcpStateSet(pMiNetHdl, DHCP_STATE_RENEWING);
                goal_netDhcpTxTimeGet(pMiNetHdl, DHCP_TIMEOUT_REQUEST);
                res = GOAL_OK;
            }
            break;

        case DHCP_STATE_RENEWING:
            if (DHCP_EVENT_TIMEOUT == event) {
                res = goal_netDhcpTxRequest(pMiNetHdl, GOAL_TRUE);
                if (GOAL_RES_ERR(res)) {
                    return res;
                }
                goal_netDhcpTxTimeGet(pMiNetHdl, DHCP_TIMEOUT_REQUEST);
                res = GOAL_OK;
            }
            else if (DHCP_EVENT_ACK_RX == event) {
                goal_netDhcpStateSet(pMiNetHdl, DHCP_STATE_BOUND);
                pMiNetHdl->dhcpData.timeout = DHCP_TIMEOUT_OFF;
                res = GOAL_OK;
            }
            else if (DHCP_EVENT_NACK_RX == event) {
                /* GG_LOG ID:207: */
                goal_lmLog(GOAL_ID_DHCP, 207, 165, 0, 0, GOAL_LOG_SEV_ERROR, "DHCP Request rejected by server");
                goal_netDhcpIpLost(pMiNetHdl);
                goal_netDhcpStateSet(pMiNetHdl, DHCP_STATE_INIT);
                pMiNetHdl->dhcpData.timeout = DHCP_TIMEOUT_OFF;
                res = GOAL_OK;
            }
            else if (DHCP_EVENT_T2_EXPIRED == event) {
                pMiNetHdl->dhcpData.serverAddr = 0xffffffff;
                res = goal_netDhcpTxRequest(pMiNetHdl, GOAL_TRUE);
                if (GOAL_RES_ERR(res)) {
                    return res;
                }
                goal_netDhcpStateSet(pMiNetHdl, DHCP_STATE_REBINDING);
                goal_netDhcpTxTimeGet(pMiNetHdl, DHCP_TIMEOUT_REQUEST);
                res = GOAL_OK;
            }
            break;

        case DHCP_STATE_REBINDING:
            if (DHCP_EVENT_TIMEOUT == event) {
                res = goal_netDhcpTxRequest(pMiNetHdl, GOAL_TRUE);
                if (GOAL_RES_ERR(res)) {
                    return res;
                }
                goal_netDhcpTxTimeGet(pMiNetHdl, DHCP_TIMEOUT_REQUEST);
                res = GOAL_OK;
            }
            else if (DHCP_EVENT_ACK_RX == event) {
                goal_netDhcpStateSet(pMiNetHdl, DHCP_STATE_BOUND);
                pMiNetHdl->dhcpData.timeout = DHCP_TIMEOUT_OFF;
                res = GOAL_OK;
            }
            else if (DHCP_EVENT_NACK_RX == event) {
                /* GG_LOG ID:208: */
                goal_lmLog(GOAL_ID_DHCP, 208, 165, 0, 0, GOAL_LOG_SEV_ERROR, "DHCP Request rejected by server");
                goal_netDhcpIpLost(pMiNetHdl);
                goal_netDhcpStateSet(pMiNetHdl, DHCP_STATE_INIT);
                pMiNetHdl->dhcpData.timeout = DHCP_TIMEOUT_OFF;
                res = GOAL_OK;
            }
            else if (DHCP_EVENT_LEASE_EXPIRED == event) {
                /* GG_LOG ID:209: */
                goal_lmLog(GOAL_ID_DHCP, 209, 166, 0, 0, GOAL_LOG_SEV_ERROR, "Lost IP Lease");
                goal_netDhcpIpLost(pMiNetHdl);
                goal_netDhcpStateSet(pMiNetHdl, DHCP_STATE_INIT);
                pMiNetHdl->dhcpData.timeout = DHCP_TIMEOUT_OFF;
                /* restart state machine */
                res = goal_netDhcpEventThrown(pMiNetHdl, DHCP_EVENT_START);
            }
            break;

        default:
            goal_logErr("invalid state: %u", pMiNetHdl->dhcpData.state);
            res = GOAL_ERROR;
            break;
    }

    return res;
}


/****************************************************************************/
/** Build the DHCP header
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static void goal_netDhcpCreateHeader(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI handle */
    uint8_t *pData,                             /**< frame data */
    uint16_t *pLen                              /**< current frame length */
)
{
    pData[DHCP_FIELD_OP] = DHCP_OP_BOOTREQ;
    pData[DHCP_FIELD_HTYPE] = DHCP_HTYPE_ETH;
    pData[DHCP_FIELD_HLEN] = DHCP_HLEN_ETH;
    pData[DHCP_FIELD_HOPS] = 0;
    GOAL_htobe32_p(&pData[DHCP_FIELD_XID], pMiNetHdl->dhcpData.xid);
    GOAL_htobe16_p(&pData[DHCP_FIELD_SECS], 0);
    GOAL_htobe16_p(&pData[DHCP_FIELD_FLAGS], DHCP_FLAG_BCAST);
    /*  set ciaddr, yiaddr, siaddr & giaddr to 0 */
    GOAL_MEMSET(&pData[DHCP_FIELD_CIADDR], 0, 4 * sizeof(uint32_t));
    GOAL_MEMCPY(&pData[DHCP_FIELD_CHADDR], (uint8_t *) pMiNetHdl->dhcpData.macAddr, 6);
    /* pad rest of chaddr field + sname + file */
    GOAL_MEMSET(&pData[DHCP_FIELD_PAD], 0, DHCP_LEN_PAD);
    GOAL_htobe32_p(&pData[DHCP_FIELD_COOKIE], DHCP_COOKIE);

    *pLen = DHCP_LEN_HEADER;
}


/****************************************************************************/
/** Create and send a DHCP DISCOVER frame
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_netDhcpTxDiscover(
    GOAL_MI_NET_T *pMiNetHdl                    /**< MI handle */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_BUFFER_T *pTxBuf = NULL;               /* send buffer */
    uint8_t *pData;                             /* current byte in frame */
    uint8_t len;                                /* string length */
    GOAL_NET_DHCP_TX_DATA_T txData;             /* DHCP Tx Data */

    /* get send buffer */
    res = goal_ethGetNetBuf(&pTxBuf);
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:210: */
        goal_lmLog(GOAL_ID_DHCP, 210, 164, 0, 0, GOAL_LOG_SEV_ERROR, "no free buffer");
        return res;
    }

    /* set transaction id with each new frame */
    pMiNetHdl->dhcpData.xid = goal_utilRand();

    goal_netDhcpCreateHeader(pMiNetHdl, pTxBuf->ptrData, &pTxBuf->dataLen);

    pData = &pTxBuf->ptrData[DHCP_FIELD_OPTIONS];

    /* add option DHCP Message Type: DISCOVER */
    pData[DHCP_OPT_ID] = DHCP_OPT_ID_MSG_TYPE;
    pData[DHCP_OPT_LEN] = 1;
    pData[DHCP_OPT_DATA] = DHCP_MSG_TYPE_DISCOVER;
    pData += 3;
    pTxBuf->dataLen += 3;

    /* add option Client Identifier */
    pData[DHCP_OPT_ID] = DHCP_OPT_ID_CLIENT_ID;

    len = (uint8_t) GOAL_STRLEN(pMiNetHdl->dhcpData.clientId);
    if (0 < len) {
        /* use set client ID */
        pData[DHCP_OPT_LEN] = (uint8_t) (1 + len);
        pData[DHCP_OPT_DATA] = DHCP_HTYPE_OTHER;
        GOAL_MEMCPY(&pData[DHCP_OPT_DATA + 1], pMiNetHdl->dhcpData.clientId, len);
        pData += 3 + len;
        pTxBuf->dataLen += 3 + len;
    }
    else {
        /* no client id, use MAC */
        pData[DHCP_OPT_LEN] = (uint8_t) (1 + DHCP_HLEN_ETH);
        pData[DHCP_OPT_DATA] = DHCP_HTYPE_ETH;
        GOAL_MEMCPY(&pData[DHCP_OPT_DATA + 1], pMiNetHdl->dhcpData.macAddr, DHCP_HLEN_ETH);
        pData += 3 + DHCP_HLEN_ETH;
        pTxBuf->dataLen += 3 + DHCP_HLEN_ETH;
    }

    /* add end marker */
    *pData = DHCP_OPT_ID_END;
    pTxBuf->dataLen++;


    /* send frame */
    pMiNetHdl->dhcpData.pChan->addr.remotePort = DHCP_SERVER_PORT;
    pMiNetHdl->dhcpData.pChan->addr.remoteIp = pMiNetHdl->dhcpData.serverAddr;

    txData.pChan = pMiNetHdl->dhcpData.pChan;
    txData.pBuf = pTxBuf;
    res = goal_miNetCmd(pMiNetHdl, GOAL_NET_CMD_DHCP_SEND, GOAL_TRUE, &txData);

    goal_queueReleaseBuf(&pTxBuf);

    return res;
}


/****************************************************************************/
/** Create and send a DHCP REQUEST frame
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_netDhcpTxRequest(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI handle */
    GOAL_BOOL_T renewing                        /**< renew active lease */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_BUFFER_T *pTxBuf = NULL;               /* send buffer */
    uint8_t *pData;                             /* current byte in frame */
    uint8_t len;                                /* string length */
    GOAL_NET_DHCP_TX_DATA_T txData;             /* DHCP Tx Data */

    /* get send buffer */
    res = goal_ethGetNetBuf(&pTxBuf);
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:211: */
        goal_lmLog(GOAL_ID_DHCP, 211, 164, 0, 0, GOAL_LOG_SEV_ERROR, "no free buffer");
        return res;
    }

    goal_netDhcpCreateHeader(pMiNetHdl, pTxBuf->ptrData, &pTxBuf->dataLen);

    if (GOAL_TRUE == renewing) {
        GOAL_htobe32_p(&pTxBuf->ptrData[DHCP_FIELD_CIADDR], pMiNetHdl->dhcpData.data.addrIp);
    }

    pData = &pTxBuf->ptrData[DHCP_FIELD_OPTIONS];

    /* add option DHCP Message Type: REQUEST */
    pData[DHCP_OPT_ID] = DHCP_OPT_ID_MSG_TYPE;
    pData[DHCP_OPT_LEN] = 1;
    pData[DHCP_OPT_DATA] = DHCP_MSG_TYPE_REQUEST;
    pData += 3;
    pTxBuf->dataLen += 3;

    if (GOAL_FALSE == renewing) {
        /* add server Id */
        pData[DHCP_OPT_ID] = DHCP_OPT_ID_SERVER_ID;
        pData[DHCP_OPT_LEN] = 4;
        GOAL_htobe32_p(&pData[DHCP_OPT_DATA], pMiNetHdl->dhcpData.serverAddr);
        pData += 6;
        pTxBuf->dataLen += 6;

        /* add requested IP */
        pData[DHCP_OPT_ID] = DHCP_OPT_ID_REQ_IPADDR;
        pData[DHCP_OPT_LEN] = 4;
        GOAL_htobe32_p(&pData[DHCP_OPT_DATA], pMiNetHdl->dhcpData.data.addrIp);
        pData += 6;
        pTxBuf->dataLen += 6;
    }

    /* add requested parameter list */
    pData[DHCP_OPT_ID] = DHCP_OPT_ID_PARAM_LIST;
    pData[DHCP_OPT_LEN] = 8;
    pData[DHCP_OPT_DATA + 0] = DHCP_OPT_ID_NETMASK;
    pData[DHCP_OPT_DATA + 1] = DHCP_OPT_ID_GATEWAY;
    pData[DHCP_OPT_DATA + 2] = DHCP_OPT_ID_DNS_SERVER;
    pData[DHCP_OPT_DATA + 3] = DHCP_OPT_ID_HOSTNAME;
    pData[DHCP_OPT_DATA + 4] = DHCP_OPT_ID_DOMAINNAME;
    pData[DHCP_OPT_DATA + 5] = DHCP_OPT_ID_LEASE_TIME;
    pData[DHCP_OPT_DATA + 6] = DHCP_OPT_ID_T1_VAL;
    pData[DHCP_OPT_DATA + 7] = DHCP_OPT_ID_T2_VAL;
    pData += 10;
    pTxBuf->dataLen += 10;

    /* add option Client Identifier */
    pData[DHCP_OPT_ID] = DHCP_OPT_ID_CLIENT_ID;

    len = (uint8_t) GOAL_STRLEN(pMiNetHdl->dhcpData.clientId);
    if (0 < len) {
        /* use set client ID */
        pData[DHCP_OPT_LEN] = (uint8_t) (1 + len);
        pData[DHCP_OPT_DATA] = DHCP_HTYPE_OTHER;
        GOAL_MEMCPY(&pData[DHCP_OPT_DATA + 1], pMiNetHdl->dhcpData.clientId, len);
        pData += 3 + len;
        pTxBuf->dataLen += 3 + len;
    }
    else {
        /* no client id, use MAC */
        pData[DHCP_OPT_LEN] = (uint8_t) (1 + DHCP_HLEN_ETH);
        pData[DHCP_OPT_DATA] = DHCP_HTYPE_ETH;
        GOAL_MEMCPY(&pData[DHCP_OPT_DATA + 1], pMiNetHdl->dhcpData.macAddr, DHCP_HLEN_ETH);
        pData += 3 + DHCP_HLEN_ETH;
        pTxBuf->dataLen += 3 + DHCP_HLEN_ETH;
    }

    /* add end marker */
    *pData = DHCP_OPT_ID_END;
    pTxBuf->dataLen++;

    /* send frame */
    pMiNetHdl->dhcpData.pChan->addr.remotePort = DHCP_SERVER_PORT;
    pMiNetHdl->dhcpData.pChan->addr.remoteIp = pMiNetHdl->dhcpData.serverAddr;

    txData.pChan = pMiNetHdl->dhcpData.pChan;
    txData.pBuf = pTxBuf;
    res = goal_miNetCmd(pMiNetHdl, GOAL_NET_CMD_DHCP_SEND, GOAL_TRUE, &txData);

    goal_queueReleaseBuf(&pTxBuf);

    return res;
}


/****************************************************************************/
/** Calculate and Get Time stamp for Retransmission
 *
 * This function calculates and returns the time stamp for the next retrans-
 * mission according to RFC2131, section 4.4, with successively doubled values.
 */
static void goal_netDhcpTxTimeGet(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI handle */
    uint64_t timeout                            /**< original timeout in ms */
)
{
    DHCP_STATE_T dhcpStateAct;                  /* actual DHCP state */
    GOAL_TIMESTAMP_T retransTimeout;            /* calculated timeout */

    dhcpStateAct = pMiNetHdl->dhcpData.state;
    if (pMiNetHdl->dhcpData.prevDhcpState != dhcpStateAct) {
        pMiNetHdl->dhcpData.prevDhcpState = dhcpStateAct;
        pMiNetHdl->dhcpData.retransCount = 0;
    }
    else if (DHCP_RETRANS_MULTIPL_MAX > pMiNetHdl->dhcpData.retransCount) {
        pMiNetHdl->dhcpData.retransCount += 1;
    }

    /* calculate successively doubled timeout by multiplying */
    retransTimeout = (GOAL_TIMESTAMP_T) (timeout * (1U << pMiNetHdl->dhcpData.retransCount));

    /* randomize timeout according to RFC, subtract max. 100 ms */
    retransTimeout -= goal_utilRand() % 100;

    if (DHCP_TIMEOUT_RETRANS_MAX < retransTimeout) {
        retransTimeout = DHCP_TIMEOUT_RETRANS_MAX;
    }

    pMiNetHdl->dhcpData.timeout = goal_timerTsGet() + retransTimeout;
}


/****************************************************************************/
/** Callback for Implicit Message data
 *
 * This function is called by GOAL, if new data was received for our DHCP
 * socket.
 */
static GOAL_STATUS_T goal_netDhcpReceiveCb(
    void *pArg,                                 /**< callback argument */
    GOAL_NET_CB_TYPE_T cbType,                  /**< callback type */
    GOAL_NET_CHAN_T *pChan,                     /**< channel descriptor */
    GOAL_BUFFER_T *pBuf                         /**< GOAL buffer */
)
{
    GOAL_MI_NET_T *pMiNetHdl;                   /* MI NET handle */
    uint16_t optLen;                            /* length of option block */
    uint8_t *pOptData;                          /* option block */
    uint32_t tmpU32;                            /* temporary U32 value */
    uint8_t len;                                /* option data length */
    uint32_t leaseTime = 0;                     /* DHCP lease time */
    uint8_t msgType = 0;                        /* DHCP message type */
    uint32_t serverId = 0;                      /* DHCP Server Id */
    uint32_t t1Val = 0;                         /* T1 (Renewal Time) */
    uint32_t t2Val = 0;                         /* T2 (Rebinding Time) */
    uint8_t strLen;                             /* string length */

    UNUSEDARG(cbType);

    pMiNetHdl = (GOAL_MI_NET_T *) pArg;

    if (GOAL_FALSE == pMiNetHdl->dhcpData.flgActive) {
        return GOAL_OK;
    }

    if ((pChan != pMiNetHdl->dhcpData.pChan) || (NULL == pBuf)) {
        /* GG_LOG ID:212: */
        goal_lmLog(GOAL_ID_DHCP, 212, 167, 0, 0, GOAL_LOG_SEV_ERROR, "invalid callback");
        return GOAL_OK;
    }

    if (DHCP_LEN_HEADER > pBuf->dataLen) {
        /* GG_LOG ID:213: */
        goal_lmLog(GOAL_ID_DHCP, 213, 168, 0, 0, GOAL_LOG_SEV_ERROR, "invalid length");
        return GOAL_OK;
    }

    if (DHCP_OP_BOOTRES != pBuf->ptrData[DHCP_FIELD_OP]) {
        /* GG_LOG ID:214: */
        goal_lmLog(GOAL_ID_DHCP, 214, 169, 0, 0, GOAL_LOG_SEV_ERROR, "no BOOTRES");
        return GOAL_OK;
    }

    tmpU32 = GOAL_be32toh_p(&pBuf->ptrData[DHCP_FIELD_XID]);
    if (tmpU32 != pMiNetHdl->dhcpData.xid) {
        /* silently ignore responses for other hosts */
        return GOAL_OK;
    }

    if (0 != GOAL_MEMCMP(&pBuf->ptrData[DHCP_FIELD_CHADDR], pMiNetHdl->dhcpData.macAddr, DHCP_HLEN_ETH)) {
        /* GG_LOG ID:216: */
        goal_lmLog(GOAL_ID_DHCP, 216, 171, 0, 0, GOAL_LOG_SEV_ERROR, "invalid HW address");
        return GOAL_OK;
    }

    tmpU32 = GOAL_be32toh_p(&pBuf->ptrData[DHCP_FIELD_COOKIE]);
    if (DHCP_COOKIE != tmpU32) {
        /* GG_LOG ID:217: */
        goal_lmLog(GOAL_ID_DHCP, 217, 172, 4, 0, GOAL_LOG_SEV_ERROR, "invalid cookie $1");
        goal_lmLogParamUINT32(tmpU32);
        goal_lmLogFinish();
        return GOAL_OK;
    }

    /* read all options */
    optLen = pBuf->dataLen - DHCP_LEN_HEADER;
    pOptData = &pBuf->ptrData[DHCP_FIELD_OPTIONS];
    GOAL_MEMSET(&pMiNetHdl->dhcpData.data, 0, sizeof(GOAL_NET_DHCP_T));

    while (optLen) {
        switch (pOptData[DHCP_OPT_ID]) {
            case DHCP_OPT_ID_PAD:
                /* padding byte, skip it */
                pOptData++;
                optLen--;
                break;

            case DHCP_OPT_ID_NETMASK:
                /* subnet mask, 4 bytes */
                pMiNetHdl->dhcpData.data.addrNetmask = GOAL_be32toh_p(&pOptData[DHCP_OPT_DATA]);
                pOptData += 6;
                optLen -= 6;
                break;

            case DHCP_OPT_ID_GATEWAY:
                /* Gateway address, multiple of 4 bytes */
                pMiNetHdl->dhcpData.data.addrGateway = GOAL_be32toh_p(&pOptData[DHCP_OPT_DATA]);
                len = pOptData[DHCP_OPT_LEN];
                pOptData += len + 2;
                optLen -= len + 2;
                break;

            case DHCP_OPT_ID_DNS_SERVER:
                /* DNS Server address, multiple of 4 bytes */
                pMiNetHdl->dhcpData.data.addrDns1 = GOAL_be32toh_p(&pOptData[DHCP_OPT_DATA]);
                len = pOptData[DHCP_OPT_LEN];
                if (8 <= len) {
                    pMiNetHdl->dhcpData.data.addrDns2 = GOAL_be32toh_p(&pOptData[DHCP_OPT_DATA + 4]);
                }
                else {
                    pMiNetHdl->dhcpData.data.addrDns2 = 0;
                }
                pOptData += len + 2;
                optLen -= len + 2;
                break;

            case DHCP_OPT_ID_DOMAINNAME:
                /* arbitrary length */
                len = pOptData[DHCP_OPT_LEN];
                strLen = len;
                if (GOAL_NET_DNAME_LEN - 1 < strLen) {
                    /* string must fit into buffer, last byte reserved for \0 */
                    strLen = GOAL_NET_DNAME_LEN - 1;
                }
                GOAL_MEMCPY(pMiNetHdl->dhcpData.data.domainName, &pOptData[DHCP_OPT_DATA], strLen);
                pMiNetHdl->dhcpData.data.domainName[strLen] = '\0';
                pOptData += len + 2;
                optLen -= len + 2;
                break;

            case DHCP_OPT_ID_HOSTNAME:
                /* arbitrary length */
                len = pOptData[DHCP_OPT_LEN];
                strLen = len;
                if (GOAL_NET_HNAME_LEN - 1 < strLen) {
                    /* string must fit into buffer, last byte reserved for \0 */
                    strLen = GOAL_NET_DNAME_LEN - 1;
                }
                GOAL_MEMCPY(pMiNetHdl->dhcpData.data.hostName, &pOptData[DHCP_OPT_DATA], strLen);
                pMiNetHdl->dhcpData.data.hostName[strLen] = '\0';
                pOptData += len + 2;
                optLen -= len + 2;
                break;

            case DHCP_OPT_ID_LEASE_TIME:
                leaseTime = GOAL_be32toh_p(&pOptData[DHCP_OPT_DATA]);
                pOptData += 6;
                optLen -= 6;
                break;

            case DHCP_OPT_ID_MSG_TYPE:
                msgType = pOptData[DHCP_OPT_DATA];
                pOptData += 3;
                optLen -= 3;
                break;

            case DHCP_OPT_ID_SERVER_ID:
                serverId = GOAL_be32toh_p(&pOptData[DHCP_OPT_DATA]);
                pOptData += 6;
                optLen -= 6;
                break;

            case DHCP_OPT_ID_T1_VAL:
                t1Val = GOAL_be32toh_p(&pOptData[DHCP_OPT_DATA]);
                pOptData += 6;
                optLen -= 6;
                break;

            case DHCP_OPT_ID_T2_VAL:
                t2Val = GOAL_be32toh_p(&pOptData[DHCP_OPT_DATA]);
                pOptData += 6;
                optLen -= 6;
                break;

            case DHCP_OPT_ID_END:
                /* reached end of options */
                optLen = 0;
                break;

            default:
                /* unknown option, skip it */
                len = pOptData[DHCP_OPT_LEN];
                pOptData += len + 2;
                optLen -= len + 2;
                break;
        }
    }

    /* depending on the message type, do some specific checking */
    switch (msgType) {
        case DHCP_MSG_TYPE_OFFER:
            if (0 == serverId) {
                /* GG_LOG ID:218: */
                goal_lmLog(GOAL_ID_DHCP, 218, 173, 0, 0, GOAL_LOG_SEV_ERROR, "OFFER had no server ID");
                return GOAL_OK;
            }
            pMiNetHdl->dhcpData.data.addrIp = GOAL_be32toh_p(&pBuf->ptrData[DHCP_FIELD_YIADDR]);
            pMiNetHdl->dhcpData.serverAddr = serverId;
            goal_netDhcpEventThrown(pMiNetHdl, DHCP_EVENT_OFFER_RX);
            break;

        case DHCP_MSG_TYPE_ACK:
            if (((serverId == pMiNetHdl->dhcpData.serverAddr) || (DHCP_STATE_REBINDING == pMiNetHdl->dhcpData.state)) &&
                (0 != leaseTime)) {
                goal_netDhcpTimeCalc(pMiNetHdl, leaseTime, t1Val, t2Val);
                pMiNetHdl->dhcpData.data.addrIp = GOAL_be32toh_p(&pBuf->ptrData[DHCP_FIELD_YIADDR]);
                goal_netDhcpEventThrown(pMiNetHdl, DHCP_EVENT_ACK_RX);
            }
            break;

        case DHCP_MSG_TYPE_NAK:
            goal_netDhcpEventThrown(pMiNetHdl, DHCP_EVENT_NACK_RX);
            break;

        default:
            /* GG_LOG ID:219: */
            goal_lmLog(GOAL_ID_DHCP, 219, 174, 4, 0, GOAL_LOG_SEV_ERROR, "unknown or no message type $1");
            goal_lmLogParamUINT32(msgType);
            goal_lmLogFinish();
            break;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Calculate renewing and rebinding times
 */
static void goal_netDhcpTimeCalc(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI NET handle */
    uint32_t leaseTime,                         /**< DHCP lease time in s */
    uint32_t renewTime,                         /**< DHCP renewing time in s */
    uint32_t rebindTime                         /**< DHCP rebinding time in s */
)
{
    GOAL_TIMESTAMP_T now;                       /* current time in ms */

    now = goal_timerTsGet();

    if (0 == renewTime) {
        if (DHCP_TIME_INFINITE != leaseTime) {
            renewTime = (uint32_t) (0.5 * leaseTime);
        }
        else {
            renewTime = DHCP_TIME_INFINITE;
        }
    }

    if (0 == rebindTime) {
        if (DHCP_TIME_INFINITE != leaseTime) {
            rebindTime = (uint32_t) (0.875 * leaseTime);
        }
        else {
            rebindTime = DHCP_TIME_INFINITE;
        }
    }

    if (DHCP_TIME_INFINITE != leaseTime) {
        pMiNetHdl->dhcpData.leaseTime = now + (leaseTime * 1000);
    }
    else {
        pMiNetHdl->dhcpData.leaseTime = DHCP_TIMEOUT_OFF;
    }

    if (DHCP_TIME_INFINITE != renewTime) {
        pMiNetHdl->dhcpData.renewTime = now + (renewTime * 1000);
    }
    else {
        pMiNetHdl->dhcpData.renewTime = DHCP_TIMEOUT_OFF;
    }

    if (DHCP_TIME_INFINITE != rebindTime) {
        pMiNetHdl->dhcpData.rebindTime = now + (rebindTime * 1000);
    }
    else {
        pMiNetHdl->dhcpData.rebindTime = DHCP_TIMEOUT_OFF;
    }
}


/****************************************************************************/
/** IP address lease time expired
 *
 * This function removes the current IP address from the device.
 */
static void goal_netDhcpIpLost(
    GOAL_MI_NET_T *pMiNetHdl                    /**< MI NET handle */
)
{
    GOAL_STATUS_T res;                          /* return value */
    GOAL_CM_VAR_T *pCmVar = NULL;               /* CM variable entry */

    /* GG_LOG ID:220: */
    goal_lmLog(GOAL_ID_DHCP, 220, 175, 0, 0, GOAL_LOG_SEV_INFO, "IP address lease time expired !!!");

    pMiNetHdl->dhcpData.serverAddr = 0xffffffff;

    res = goal_miNetIpSet(pMiNetHdl, 0, 0, 0, GOAL_FALSE);

    if (GOAL_RES_OK(res)) {
        res = goal_cmGetVarById(GOAL_CM_NET_MOD_ID, NET_CM_VAR_DHCP_STATE + (pMiNetHdl->cmVarInstId * GOAL_CM_NET_VAR_LAST), &pCmVar);
    }

    if (GOAL_RES_OK(res)) {
        GOAL_CM_SETVAR_UINT8(pCmVar, pMiNetHdl->dhcpData.state);
    }

}


/****************************************************************************/
/** IP address valid
 *
 * This function sets the current IP address of the device.
 */
static GOAL_STATUS_T goal_netDhcpIpSet(
    GOAL_MI_NET_T *pMiNetHdl                    /**< MI NET handle */
)
{
    GOAL_STATUS_T res;                          /* return value */
    GOAL_BOOL_T flgValid = GOAL_FALSE;          /* IP valid flag */
    GOAL_CM_VAR_T *pCmVar = NULL;               /* CM variable entry */

    res = goal_miNetIpSet(pMiNetHdl, pMiNetHdl->dhcpData.data.addrIp, pMiNetHdl->dhcpData.data.addrNetmask, pMiNetHdl->dhcpData.data.addrGateway, GOAL_FALSE);
    if (GOAL_RES_OK(res)) {
        res = goal_miNetFlgValidGet(pMiNetHdl, &flgValid);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_miNetCmIpSet(pMiNetHdl, pMiNetHdl->dhcpData.data.addrIp, pMiNetHdl->dhcpData.data.addrNetmask, pMiNetHdl->dhcpData.data.addrGateway, flgValid);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_cmGetVarById(GOAL_CM_NET_MOD_ID, NET_CM_VAR_DHCP_STATE + (pMiNetHdl->cmVarInstId * GOAL_CM_NET_VAR_LAST), &pCmVar);
    }

    if (GOAL_RES_OK(res)) {
        GOAL_CM_SETVAR_UINT8(pCmVar, pMiNetHdl->dhcpData.state);
    }

    return res;
}


/****************************************************************************/
/** set DHCP state in CM
 *
 */
static GOAL_STATUS_T goal_netDhcpStateSet(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI NET handle */
    DHCP_STATE_T state                          /**< DHCP enable */
)
{
    GOAL_STATUS_T res;                          /* return value */
    GOAL_CM_VAR_T *pCmVar;                      /* CM variable entry */

    res = goal_cmGetVarById(GOAL_CM_NET_MOD_ID, NET_CM_VAR_DHCP_STATE + (pMiNetHdl->cmVarInstId * GOAL_CM_NET_VAR_LAST), &pCmVar);

    if (GOAL_RES_OK(res)) {
        GOAL_CM_SETVAR_UINT8(pCmVar, (uint8_t) state);
    }

    pMiNetHdl->dhcpData.state = state;

    return res;
}
