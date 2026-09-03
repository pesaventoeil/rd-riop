/** @file
 *
 * @brief
 * GOAL Connector for lwIP TCP/IP Stack
 *
 * @details
 * This module connects the lwIP TCP/IP stack to GOAL.
 *
 * @copyright
 * Copyright 2010-2017.
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
#include <goal_includes.h>
#include <plat/drv/net/lwip/goal_target_net.h>

/* lwIP Includes */
#include "lwip/opt.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "netif/etharp.h"
#include "lwip/sys.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include "lwip/timeouts.h"
#include "lwip/dns.h"
#include "lwip/igmp.h"

#if GOAL_CONFIG_WEBSERVER == 1
#  include "lwip/apps/httpd.h"
#endif

#if GOAL_CONFIG_DHCP_EXT == 1
#  include "lwip/dhcp.h"
#endif

#ifdef LWIP_DEBUG
#  include "lwip/debug.h"
#endif

#if GOAL_CONFIG_TCPIP_BSD_API == 1
#  include "lwip/tcpip.h"
#endif /* GOAL_CONFIG_TCPIP_BSD_API == 1 */


/****************************************************************************/
/* Compile-time checks */
/****************************************************************************/
/* check if sizeof(ip4_addr_t) is exactly 32-bit as lwIP won't run correctly on
 * systems that pad structures */
GOAL_CASSERT(sizeof(ip4_addr_t) == 4);


/****************************************************************************/
/* local defines */
/****************************************************************************/
/* net option defines */
#define GOAL_TARGET_NET_OPTION_MASK_T       uint32_t
#define GOAL_TARGET_NET_OPTION_BROADCAST    (1 << 0)
#define GOAL_TARGET_NET_OPTION_TTL          (1 << 1)
#define GOAL_TARGET_NET_OPTION_TOS          (1 << 2)
#define GOAL_TARGET_NET_OPTION_REUSEADDR    (1 << 3)

/* POWERLINK define */
#ifndef GOAL_CONFIG_EPL
#   define GOAL_CONFIG_EPL 0
#endif

#define GOAL_TARGET_NET_TCP_CLS_TMO_CNT       5 /**< number of closing tcp socket poll iterations before it is reset */


/****************************************************************************/
/* structures */
/****************************************************************************/
typedef struct {
    GOAL_TARGET_NET_OPTION_MASK_T optMask;      /**< bitmask of options to set */
    int optValNonBlock;                         /**< option value for nonblock option */
    int optValBroadcast;                        /**< option value for receiving breadcasts */
    uint8_t optValTtl;                          /**< option value for ttl */
    uint8_t optValTos;                          /**< option value for tos */
    int optValReuseaddr;                        /**< option value for reusing socket */
} GOAL_TARGET_NET_OPTION_T;

typedef struct {
    void *pPcbDesc;                             /**< pointer to TCP/ UDP PCB (must be first member) */
    GOAL_NET_CHAN_T *pGoalChan;                 /**< corresponding GOAL Net channel */
    GOAL_TARGET_NET_OPTION_T opts;              /**< socket options */
    GOAL_BOOL_T flgNotClose;                    /**< channel must not closed again */
} GOAL_TARGET_NET_HANDLE_T;

typedef struct {
    GOAL_MI_ETH_T *pMiEthHdl;                   /**< Ethernet MI handle */
#if GOAL_FEAT_LWIP_INIT == 1
    struct netif netIf;                         /**< lwIP network handle */
#endif /* GOAL_FEAT_LWIP_INIT */
    struct netif *pNetIf;                       /**< pointer to netif */
    uint32_t currentIp;                         /**< currently used IP address */
    GOAL_BOOL_T ipIsTemp;                       /**< temporary flag */
} GOAL_DRV_NET_LWIP_DATA_T;


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
static GOAL_STATUS_T goal_drvNetLwipStackInit(
    void
);

#if GOAL_FEAT_LWIP_INIT == 1
static err_t goal_targetLwipNetifInit(
    struct netif *pNetifPtr                     /**< pointer to lwip interface structure */
);

static err_t goal_targetLwipOutput(
    struct netif *pNetifPtr,                    /**< lwIP network interface handle */
    struct pbuf *pLwipBuffer                    /**< lwIP puffer to send via ethernet */
);

static err_t goal_tgtNetIgmpMacFilter(
    struct netif *pNetif,                       /**< lwIP network interface handle */
    const ip4_addr_t *pGroup,                   /**< lwIP multiast group */
    enum netif_mac_filter_action action         /**< lwIP mac filter action */
);

static void goal_targetNetTimerCbLwip(
    void                                        /**< timer argument */
);

static GOAL_STATUS_T goal_targetNetChanRecv(
    GOAL_BUFFER_T **ppGoalBuffer,               /**< GOAL Ethernet buffer */
    void *pArg                                  /**< callback argument */
);
#endif /* GOAL_FEAT_LWIP_INIT */

static void goal_targetNetUdpRecvCb(
    void *pArg,                                 /**< arg user supplied argument (udp_pcb.recv_arg) */
    struct udp_pcb *pUdpPcbDesc,                /**< pcb the udp_pcb which received data */
    struct pbuf *pLwipBuffer,                   /**< pointer the packet buffer that was received */
    const ip_addr_t *pAddrRemote,               /**< addr of the remote IP address from which the packet was received */
    u16_t remote_port                           /**< remote port which received the udp packet */
);

#if GOAL_CONFIG_DHCP_EXT == 1
static GOAL_STATUS_T goal_targetNetDhcpState(
    struct netif *pNetIf,                       /**< netif handle */
    uint32_t *pState                            /**< state pointer */
);

static GOAL_STATUS_T goal_targetNetDhcpAddr(
    struct netif *pNetIf,                       /**< netif handle */
    GOAL_NET_DHCP_T *pAddrDhcp                  /**< addresses received via DHCP */
);
#else /* GOAL_CONFIG_DHCP_EXT */
static GOAL_STATUS_T goal_targetNetDhcpSend(
    GOAL_DRV_NET_LWIP_DATA_T *pPrvData,         /**< private driver data */
    GOAL_NET_DHCP_TX_DATA_T *pTxData            /**< DHCP Tx data */
);
#endif /* GOAL_CONFIG_DHCP_EXT */

#if GOAL_CONFIG_IP_STATS == 1
static GOAL_STATUS_T goal_targetIpStatsGet(
    struct netif *pNetIf,                       /**< netif handle */
    GOAL_NET_IP_STATS_GET_T * const pStats      /**< statistics get request */
);

static GOAL_STATUS_T goal_targetIpStatsMaskGet(
    GOAL_NET_IP_STATS_MASK_T *pMaskPort         /**< mask data */
);

static GOAL_STATUS_T goal_targetIpStatsRst(
    struct netif *pNetIf                        /**< netif handle */
);
#endif /* GOAL_CONFIG_IP_STATS == 1 */


#if GOAL_CONFIG_TCPIP_TCP == 1
static err_t goal_tgtNetTcpPollCb(
    void *pArg,                                 /**< callback argument (not used) */
    struct tcp_pcb *pTcpPcb                     /**< polled TCP socket */
);

static err_t goal_tgtNetTcpAcceptCb(
    void *pArg,                                 /**< listener handle */
    struct tcp_pcb *pNewpcb,                    /**< new socket */
    err_t err                                   /**< status */
);

static err_t goal_tgtNetTcpRxCb(
    void *pArg,                                 /**< unused */
    struct tcp_pcb *pTcpPcb,                    /**< TCP socket handle */
    struct pbuf *pLwIpBuf,                      /**< data buffer */
    err_t err                                   /**< error code */
);

static err_t goal_tgtNetTcpConnectCb(
    void *pArg,                                 /**< unused */
    struct tcp_pcb *pTcpPcb,                    /**< TCP socket */
    err_t err                                   /**< error code */
);

static void goal_tgtNetTcpErrorHandlerCb(
    void *pArg,                                 /**< socket handle */
    err_t err                                   /**< error code */
);
#endif /* GOAL_CONFIG_TCPIP_TCP == 1 */

static GOAL_STATUS_T goal_targetNetOptSetSocket(
    void *pDesc,                                /**< descriptor ptr */
    GOAL_TARGET_NET_OPTION_T *pOpt              /**< pointer to options */
);

static GOAL_STATUS_T goal_drvNetLwipOpen(
    GOAL_MA_NET_T *pHdlMaNet                    /**< NET MA handle */
);

static GOAL_STATUS_T goal_drvNetLwipIpSet(
    GOAL_MA_NET_T *pNetHdl,                     /**< pointer to store NET handler */
    uint32_t addrIp,                            /**< IP address */
    uint32_t addrMask,                          /**< subnet mask */
    uint32_t addrGw,                            /**< gateway */
    GOAL_BOOL_T flgTemp                         /**< temporary IP config flag */
);

static GOAL_STATUS_T goal_drvNetLwipIpGet(
    GOAL_MA_NET_T *pHdlMaNet,                   /**< NET MA handle */
    uint32_t *pAddrIp,                          /**< IP address */
    uint32_t *pAddrMask,                        /**< subnet mask */
    uint32_t *pAddrGw,                          /**< gateway */
    GOAL_BOOL_T *pFlgTemp                       /**< temporary IP config flag */
);

static GOAL_STATUS_T goal_drvNetLwipCmd(
    GOAL_MA_NET_T *pHdlMaNet,                   /**< NET MA handle */
    GOAL_NET_CMD_T id,                          /**< command ID */
    GOAL_BOOL_T wrFlag,                         /**< write flag */
    void *pArg                                  /**< argument */
);

static GOAL_STATUS_T goal_drvNetLwipClose(
    GOAL_MA_NET_T *pHdlMaNet                    /**< NET MA handle */
);


/****************************************************************************/
/* External variables */
/****************************************************************************/
extern struct tcp_pcb *tcp_input_pcb;           /**< lwIP: receiving TCP socket */
#if GOAL_CONFIG_IP_STATS == 1
extern struct stats_ lwip_stats;                /**< LWIP stats */
#endif


/****************************************************************************/
/* Local variables */
/****************************************************************************/
#if GOAL_FEAT_LWIP_INIT == 1
static struct udp_pcb *m_pStaticUdpPcb = NULL;  /**< static UDP PCB */
#if GOAL_CONFIG_TCPIP_TCP == 1
static struct tcp_pcb *m_pStaticTcpPcb = NULL;  /**< static TCP PCB */
static struct tcp_pcb_listen *m_pStaticTcpListenerPcb = NULL;  /**< static TCP Listener PCB */
#endif /* GOAL_CONFIG_TCPIP_TCP */
#endif /* GOAL_FEAT_LWIP_INIT */
static GOAL_LOCK_T *pLwIpLock = NULL;           /**< lwIP access lock */
static GOAL_LOCK_T *pLwIpResLock = NULL;        /**< lwIP resource lock */
static GOAL_BOOL_T mLwIpInitilized = GOAL_FALSE; /**< lwIP stack is initialized  */

#if GOAL_CONFIG_IP_STATS == 1
static GOAL_NET_IP_STATS_MASK_T lwipStatsMask;  /**< lwIP statistics mask */

/* Mapping between generic ip stats and lwip stats */
/* Note: when adding entries make sure they are reflecting the GOAL_NET_IP_STATS_ID_T position */
static uint32_t *ipStatsRegs[GOAL_NET_IP_STATS_END] = {
    (uint32_t *) &(lwip_stats.mib2.ipinhdrerrors),
    (uint32_t *) &(lwip_stats.mib2.ipinaddrerrors),
    (uint32_t *) &(lwip_stats.mib2.ipinunknownprotos),
    (uint32_t *) &(lwip_stats.mib2.ipindiscards),
    (uint32_t *) &(lwip_stats.mib2.ipindelivers),
    (uint32_t *) &(lwip_stats.mib2.ipoutrequests),
    (uint32_t *) &(lwip_stats.mib2.ipoutdiscards),
    (uint32_t *) &(lwip_stats.mib2.ipoutnoroutes),
    (uint32_t *) &(lwip_stats.mib2.ipreasmoks),
    (uint32_t *) &(lwip_stats.mib2.ipreasmfails),
    (uint32_t *) &(lwip_stats.mib2.ipfragoks),
    (uint32_t *) &(lwip_stats.mib2.ipfragfails),
    (uint32_t *) &(lwip_stats.mib2.ipfragcreates),
    (uint32_t *) &(lwip_stats.mib2.ipreasmreqds),
    (uint32_t *) &(lwip_stats.mib2.ipforwdatagrams),
    (uint32_t *) &(lwip_stats.mib2.ipinreceives),

    (uint32_t *) &(lwip_stats.mib2.tcpactiveopens),
    (uint32_t *) &(lwip_stats.mib2.tcppassiveopens),
    (uint32_t *) &(lwip_stats.mib2.tcpattemptfails),
    (uint32_t *) &(lwip_stats.mib2.tcpestabresets),
    (uint32_t *) &(lwip_stats.mib2.tcpoutsegs),
    (uint32_t *) &(lwip_stats.mib2.tcpretranssegs),
    (uint32_t *) &(lwip_stats.mib2.tcpinsegs),
    (uint32_t *) &(lwip_stats.mib2.tcpinerrs),
    (uint32_t *) &(lwip_stats.mib2.tcpoutrsts),

    (uint32_t *) &(lwip_stats.mib2.udpindatagrams),
    (uint32_t *) &(lwip_stats.mib2.udpnoports),
    (uint32_t *) &(lwip_stats.mib2.udpinerrors),
    (uint32_t *) &(lwip_stats.mib2.udpoutdatagrams),

    (uint32_t *) &(lwip_stats.mib2.icmpinmsgs),
    (uint32_t *) &(lwip_stats.mib2.icmpinerrors),
    (uint32_t *) &(lwip_stats.mib2.icmpindestunreachs),
    (uint32_t *) &(lwip_stats.mib2.icmpintimeexcds),
    (uint32_t *) &(lwip_stats.mib2.icmpinparmprobs),
    (uint32_t *) &(lwip_stats.mib2.icmpinsrcquenchs),
    (uint32_t *) &(lwip_stats.mib2.icmpinredirects),
    (uint32_t *) &(lwip_stats.mib2.icmpinechos),
    (uint32_t *) &(lwip_stats.mib2.icmpinechoreps),
    (uint32_t *) &(lwip_stats.mib2.icmpintimestamps),
    (uint32_t *) &(lwip_stats.mib2.icmpintimestampreps),
    (uint32_t *) &(lwip_stats.mib2.icmpinaddrmasks),
    (uint32_t *) &(lwip_stats.mib2.icmpinaddrmaskreps),
    (uint32_t *) &(lwip_stats.mib2.icmpoutmsgs),
    (uint32_t *) &(lwip_stats.mib2.icmpouterrors),
    (uint32_t *) &(lwip_stats.mib2.icmpoutdestunreachs),
    (uint32_t *) &(lwip_stats.mib2.icmpouttimeexcds),
    (uint32_t *) &(lwip_stats.mib2.icmpoutechos),
    (uint32_t *) &(lwip_stats.mib2.icmpoutechoreps),
};
#endif /* GOAL_CONFIG_IP_STATS == 1 */


/****************************************************************************/
/** Register LWIP driver instance
 *
 * The parameter @em idNetMa defines the instance id of the driver.
 * The parameter @em idEthMi defines the corresponding Ethernet interface.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_drvNetLwipReg(
    uint32_t idNetMa,                           /**< Net MA ID */
    uint32_t idEthMi                            /**< Eth MI ID */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MA_NET_T *pMaLwipHdl;                  /* MA handle */
    GOAL_DRV_NET_LWIP_DATA_T *pPrvData = NULL;  /* private data */

    res = goal_memCalloc(&pPrvData, sizeof(GOAL_DRV_NET_LWIP_DATA_T));
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to allocate driver data");
    }

    if (GOAL_RES_OK(res)) {
        /* register driver */
        res = goal_maNetDrvReg(idNetMa, &pMaLwipHdl,
                               goal_drvNetLwipOpen,
                               goal_drvNetLwipIpSet,
                               goal_drvNetLwipIpGet,
                               goal_drvNetLwipCmd,
                               goal_drvNetLwipClose,
                               pPrvData);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to register Net MA");
        }
    }

    if (GOAL_RES_OK(res)) {
        pMaLwipHdl->idEthMi = idEthMi;
    }

    if (GOAL_RES_OK(res) && (GOAL_FALSE == mLwIpInitilized)) {
        res = goal_drvNetLwipStackInit();
        if (GOAL_RES_OK(res)) {
            mLwIpInitilized = GOAL_TRUE;
        }
    }

    return res;
}


/****************************************************************************/
/** Initialize the lwIP stack and common resources
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
static GOAL_STATUS_T goal_drvNetLwipStackInit(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* create access lock */
    res = goal_lockCreate(GOAL_LOCK_BINARY, &pLwIpLock, 0, 1, GOAL_ID_NET);
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:409: */
        goal_lmLog(GOAL_ID_DRV_NET, 409, 339, 0, 0, GOAL_LOG_SEV_ERROR, "failed to create access lock");
        return res;
    }

    /* create resource lock */
    res = goal_lockCreate(GOAL_LOCK_BINARY, &pLwIpResLock, 0, 1, GOAL_ID_NET);
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:410: */
        goal_lmLog(GOAL_ID_DRV_NET, 410, 340, 0, 0, GOAL_LOG_SEV_ERROR, "failed to create resource lock");
        return res;
    }

#if GOAL_FEAT_LWIP_INIT == 1
# if GOAL_CONFIG_TCPIP_BSD_API == 1
    tcpip_init(NULL, NULL);
# else /* GOAL_CONFIG_TCPIP_BSD_API */
    /* call stack internal init function setting internal buffer, etc. */
    lwip_init();
# endif /* GOAL_CONFIG_TCPIP_BSD_API */

    /* Configure and start timer for lwIP: check every second */
    res = goal_mainLoopReg(goal_targetNetTimerCbLwip);
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:412: */
        goal_lmLog(GOAL_ID_DRV_NET, 412, 342, 0, 0, GOAL_LOG_SEV_ERROR, "failed to setup GOAL timer for lwIP");
        return res;
    }

    /* output basic information on lwIP memory configuration */
    goal_logInfo("lwIP memory settings:");
    goal_logInfo("- MEM_SIZE         0x%x", MEM_SIZE);
    goal_logInfo("- MEMP_NUM_PBUF    %d", MEMP_NUM_PBUF);
    goal_logInfo("- MEMP_NUM_RAW_PCB %d", MEMP_NUM_RAW_PCB);
    goal_logInfo("- MEMP_NUM_UDP_PCB %d", MEMP_NUM_UDP_PCB);
    goal_logInfo("- MEMP_NUM_TCP_PCB %d", MEMP_NUM_TCP_PCB);
    goal_logInfo("- PBUF_POOL_SIZE   %d", PBUF_POOL_SIZE);

    /* output lwIP feature configuration */
    goal_logInfo("lwIP features/modules:");
    goal_logInfo("- LWIP_ARP     %s", (LWIP_ARP == 1) ? "enabled" : "off");
    goal_logInfo("- LWIP_ICMP    %s", (LWIP_ICMP == 1) ? "enabled" : "off");
    goal_logInfo("- LWIP_DHCP    %s", (LWIP_DHCP == 1) ? "enabled" : "off");
    goal_logInfo("- LWIP_AUTOIP  %s", (LWIP_AUTOIP == 1) ? "enabled" : "off");
    goal_logInfo("- LWIP_SNMP    %s", (LWIP_SNMP == 1) ? "enabled" : "off");
    goal_logInfo("- LWIP_IGMP    %s", (LWIP_IGMP == 1) ? "enabled" : "off");
    goal_logInfo("- LWIP_DNS     %s", (LWIP_DNS == 1) ? "enabled" : "off");
    goal_logInfo("- LWIP_UDP     %s", (LWIP_UDP == 1) ? "enabled" : "off");
    goal_logInfo("- LWIP_UDPLITE %s", (LWIP_UDPLITE == 1) ? "enabled" : "off");
    goal_logInfo("- LWIP_TCP     %s", (LWIP_TCP == 1) ? "enabled" : "off");
#endif /* GOAL_FEAT_LWIP_INIT == 1 */

#if GOAL_CONFIG_WEBSERVER == 1
    /* Initialize the httpd server. */
    httpd_init();
#endif

    return res;
}


/****************************************************************************/
/** Close the net driver
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
static GOAL_STATUS_T goal_drvNetLwipClose(
    GOAL_MA_NET_T *pHdlMaNet                    /**< NET MA handle */
)
{
    UNUSEDARG(pHdlMaNet);

    /* free handles */

    return GOAL_OK;

}


/****************************************************************************/
/** Open the net driver
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
static GOAL_STATUS_T goal_drvNetLwipOpen(
    GOAL_MA_NET_T *pHdlMaNet                    /**< NET MA handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_DRV_NET_LWIP_DATA_T *pPrvData;         /* private data */
#if GOAL_FEAT_LWIP_INIT == 1
    ip_addr_t ipZero;                           /* zero IP lwIP structure */
#endif
#if GOAL_CONFIG_IP_STATS == 1 && GOAL_FEAT_LWIP_INIT == 1
    uint32_t cnt;                               /* counter */
#endif /* GOAL_CONFIG_IP_STATS && GOAL_FEAT_LWIP_INIT */

    pPrvData = (GOAL_DRV_NET_LWIP_DATA_T *) pHdlMaNet->pDrvData;

    pPrvData->pMiEthHdl = NULL;
    res = goal_instGetById((GOAL_INSTANCE_T **) &pPrvData->pMiEthHdl, GOAL_ID_ETH, pHdlMaNet->idEthMi);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

#if GOAL_FEAT_LWIP_INIT == 1
    /* Clean IP configuration */
    IP4_ADDR(&ipZero, 0, 0, 0, 0);

    /* assign network interface pointer */
    GOAL_MEMSET(&pPrvData->netIf, 0, sizeof(struct netif));
    pPrvData->pNetIf = &pPrvData->netIf;
    pPrvData->pNetIf->state = pHdlMaNet;

#if GOAL_CONFIG_IP_STATS == 1
    lwipStatsMask = 0;

    /* Calculate statistics mask for common counters */
    for (cnt = 0; (cnt < GOAL_NET_IP_STATS_END) && (cnt < ARRAY_ELEMENTS(ipStatsRegs)); cnt++) {

        /* check if statistics entry is supported */
        if (NULL != ipStatsRegs[cnt]) {
            lwipStatsMask |= (1 << cnt);
        }
    }

    /* Calculate statistics mask for interface specific counters */
    lwipStatsMask |= ((GOAL_NET_IP_STATS_MASK_T) 1 << GOAL_NET_IP_STATS_IFINOCTETS);
    lwipStatsMask |= ((GOAL_NET_IP_STATS_MASK_T) 1 << GOAL_NET_IP_STATS_IFINUCASTPKTS);
    lwipStatsMask |= ((GOAL_NET_IP_STATS_MASK_T) 1 << GOAL_NET_IP_STATS_IFINNUCASTPKTS);
    lwipStatsMask |= ((GOAL_NET_IP_STATS_MASK_T) 1 << GOAL_NET_IP_STATS_IFINDISCARDS);
    lwipStatsMask |= ((GOAL_NET_IP_STATS_MASK_T) 1 << GOAL_NET_IP_STATS_IFINERRORS);
    lwipStatsMask |= ((GOAL_NET_IP_STATS_MASK_T) 1 << GOAL_NET_IP_STATS_IFINUNKOWNPROTOS);
    lwipStatsMask |= ((GOAL_NET_IP_STATS_MASK_T) 1 << GOAL_NET_IP_STATS_IFOUTOCTETS);
    lwipStatsMask |= ((GOAL_NET_IP_STATS_MASK_T) 1 << GOAL_NET_IP_STATS_IFOUTUCASTPKTS);
    lwipStatsMask |= ((GOAL_NET_IP_STATS_MASK_T) 1 << GOAL_NET_IP_STATS_IFOUTNUCASTPKTS);
    lwipStatsMask |= ((GOAL_NET_IP_STATS_MASK_T) 1 << GOAL_NET_IP_STATS_IFOUTDISCARDS);
    lwipStatsMask |= ((GOAL_NET_IP_STATS_MASK_T) 1 << GOAL_NET_IP_STATS_IFOUTERRORS);
#endif /* GOAL_CONFIG_IP_STATS == 1 */


    /* Create a new network interface instance for lwIP
     * NOTE: Adding a network interface via netif_add() cause a call of the
     * commited handle of the input and the initialization function immediately
     */

    if (NULL == netif_add(pPrvData->pNetIf, &ipZero, &ipZero, &ipZero, pHdlMaNet, &goal_targetLwipNetifInit, &ethernet_input)) {
        /* GG_LOG ID:411: */
        goal_lmLog(GOAL_ID_DRV_NET, 411, 341, 0, 0, GOAL_LOG_SEV_ERROR, "Couldn't add network interface for lwIP");
        return GOAL_ERR_NET_INIT;
    }

    if (GOAL_ID_DEFAULT == pHdlMaNet->id) {
        /* Registers "networkInterface" as the default network interface */
        netif_set_default(pPrvData->pNetIf);
    }

    /* set interface and link state to up */
    netif_set_up(pPrvData->pNetIf);
    netif_set_link_up(pPrvData->pNetIf);

    /* register IPv4 EtherType */
    if (GOAL_RES_OK(res)) {
        res = goal_miEthProtoAddPos(pPrvData->pMiEthHdl, GOAL_FALSE, GOAL_ETH_ETHERTYPE_IPV4, NULL, goal_targetNetChanRecv, pHdlMaNet, GOAL_FALSE);
        if (GOAL_RES_ERR(res)) {
            /* GG_LOG ID:413: */
            goal_lmLog(GOAL_ID_DRV_NET, 413, 343, 0, 0, GOAL_LOG_SEV_ERROR, "failed to register EtherType IPv4");
            return res;
        }
    }

    /* register ARP EtherTypes */
    if (GOAL_RES_OK(res)) {
        res = goal_miEthProtoAddPos(pPrvData->pMiEthHdl, GOAL_FALSE, GOAL_ETH_ETHERTYPE_ARP, NULL, goal_targetNetChanRecv, pHdlMaNet, GOAL_FALSE);
        if (GOAL_RES_ERR(res)) {
            /* GG_LOG ID:414: */
            goal_lmLog(GOAL_ID_DRV_NET, 414, 344, 0, 0, GOAL_LOG_SEV_ERROR, "failed to register EtherType ARP");
            return res;
        }
    }
#else /* GOAL_FEAT_LWIP_INIT */
    return goal_drvNetLwipIpGet(pHdlMaNet, &pPrvData->currentIp, NULL, NULL, NULL);
#endif /* GOAL_FEAT_LWIP_INIT */

    return res;
}


#if GOAL_FEAT_LWIP_INIT == 1
/****************************************************************************/
/** Initializes the current lwIP network interface
 *
 * This interface specific init function is called by the lwIP stack.
 *
 * It configures the following parameter of the interface:
 *  - output callback
 *  - link_output callback,
 *  - internal name of the interface
 *  - hostname
 *  - MAC address
 *  - maximum transfer unit (MTU)
 *  - capability flags
 *
 * This function should be passed as a parameter to netif_add().
 *
 * \return ERR_OK if initialization was successful
 */
static err_t goal_targetLwipNetifInit(
    struct netif *pNetifPtr                     /**< pointer to lwip interface structure */
)
{
    uint8_t mac[MAC_ADDR_LEN];                  /* MAC address */
    GOAL_MA_NET_T *pHdlMaNet;                   /* NET MA handle */
    GOAL_DRV_NET_LWIP_DATA_T *pPrvData;         /* private data */

    pHdlMaNet = pNetifPtr->state;
    pPrvData = (GOAL_DRV_NET_LWIP_DATA_T *) pHdlMaNet->pDrvData;

    #if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    pNetifPtr->hostname = "lwip";
    #endif /* LWIP_NETIF_HOSTNAME */

    pNetifPtr->name[0] = 'i';
    pNetifPtr->name[1] = 'f';

    /* We directly use etharp_output() here to save a function call.
    * You can instead declare your own function an call etharp_output()
    * from it if you have to do some checks before sending (e.g. if link
    * is available...) */
    pNetifPtr->output = etharp_output;
    pNetifPtr->linkoutput = goal_targetLwipOutput;

    /* Set netif link flag */
    pNetifPtr->flags |= NETIF_FLAG_LINK_UP;

    /* set MAC hardware address length */
    pNetifPtr->hwaddr_len = ETHARP_HWADDR_LEN;

    /* Read MAC address from target */
    if (GOAL_OK != goal_miEthMacAddrGet(pPrvData->pMiEthHdl, GOAL_ETH_PORT_HOST, (GOAL_ETH_MAC_ADDR_T *) mac)) {
        /* GG_LOG ID:415: */
        goal_lmLog(GOAL_ID_DRV_NET, 415, 345, 0, 0, GOAL_LOG_SEV_ERROR, "Couldn't read MAC address during lwIP initialization");
        return ERR_IF;
    }

    /* Write mac address to network interface structure */
    GOAL_MEMCPY(pNetifPtr->hwaddr, mac, MAC_ADDR_LEN);

    /* Set maximum transfer unit (MTU) */
#ifdef GOAL_TGT_MTU_LIMIT_FOR_LWIP
    /* use target specific MTU limit */
    pNetifPtr->mtu = GOAL_TGT_MTU_LIMIT_FOR_LWIP;
#else
    /* use default MTU */
    pNetifPtr->mtu = ETH_MTU_LEN;
#endif

    /* device capabilities */
    pNetifPtr->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_IGMP;

    /* set MAC filter function */
    pNetifPtr->igmp_mac_filter = &goal_tgtNetIgmpMacFilter;

    /* Return ERR_OK. Function halts on error. */
    return ERR_OK;
}


/****************************************************************************/
/** Map IP-Multicast-Address to Mac-Multicast-Address and Add/Delete filter
 *
 * @retval ERR_OK success
 * @retval other fail
 */
static err_t goal_tgtNetIgmpMacFilter(
    struct netif *pNetif,                       /**< lwIP network interface handle */
    const ip4_addr_t *pGroup,                   /**< lwIP multiast group */
    enum netif_mac_filter_action action         /**< lwIP mac filter action */
)
{
    uint32_t group23;                           /* ip address */
    GOAL_ETH_MAC_ADDR_T addr;                   /* mac address */
    GOAL_ETH_PORT_T portMask;                   /* port mask */
    GOAL_STATUS_T res;                          /* result */
    uint32_t numPorts;                          /* number of ports */
    GOAL_MA_NET_T *pHdlMaNet;                   /* NET MA handle */
    GOAL_DRV_NET_LWIP_DATA_T *pPrvData;         /* private data */

    pHdlMaNet = pNetif->state;
    pPrvData = (GOAL_DRV_NET_LWIP_DATA_T *) pHdlMaNet->pDrvData;

    /* get number of ethernet ports */
    res = goal_miEthCmd(pPrvData->pMiEthHdl, GOAL_ETH_CMD_PORT_COUNT, GOAL_FALSE, GOAL_ETH_PORT_HOST, &numPorts);
    if (GOAL_RES_ERR(res)) {
        goal_logInfo("failed to get number of ethernet ports");
        return res;
    }

    /* leave function on single port device */
    if (1 == numPorts) {
        return ERR_OK;
    }

    /* Map IP-Adress to MAC-Address */
    group23 = GOAL_htobe32(pGroup->addr) & 0x007FFFFF;
    addr[0] = LL_IP4_MULTICAST_ADDR_0;
    addr[1] = LL_IP4_MULTICAST_ADDR_1;
    addr[2] = LL_IP4_MULTICAST_ADDR_2;
    addr[3] = group23 >> 16;
    addr[4] = group23 >> 8;
    addr[5] = group23;
    portMask = UINT32_MAX;

    switch (action) {

        case NETIF_ADD_MAC_FILTER:
            /* Add multicast group */
            res = goal_miEthMcastAdd(pPrvData->pMiEthHdl, portMask,(GOAL_ETH_MAC_ADDR_T *) addr);
            if (GOAL_RES_ERR(res)) {
                return ERR_ARG;
            }
            return ERR_OK;

        case NETIF_DEL_MAC_FILTER:
            /* Delete multicast group */
            res = goal_miEthMcastDel(pPrvData->pMiEthHdl, portMask,(GOAL_ETH_MAC_ADDR_T *) addr);
            if (GOAL_RES_ERR(res)) {
                return ERR_ARG;
            }
            return ERR_OK;

        default:
            return ERR_ARG;
    }
}
#endif /* GOAL_FEAT_LWIP_INIT == 1 */


/****************************************************************************/
/** Acquire Resource lock
 *
 * This function is used by lwIP to lock a resource.
 *
 * @retval 0 success
 * @retval -1 error
 */
sys_prot_t sys_arch_protect(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    res = goal_lockGet(pLwIpResLock, GOAL_LOCK_INFINITE);

    return (GOAL_RES_OK(res)) ? 0 : -1;
}


/****************************************************************************/
/** Release Resource lock
 *
 * This function is used by lwIP to release resources.
 *
 * @retval 0 success
 * @retval -1 error
 */
void sys_arch_unprotect(
    sys_prot_t pval                             /**< unused */
)
{
    UNUSEDARG(pval);

    goal_lockPut(pLwIpResLock);
}


/****************************************************************************/
/** Check for new data
 *
 * Returns if the descriptors must be checked for new data.
 *
 * @retval GOAL_TRUE new data available
 * @retval GOAL_FALSE no data available
 */
GOAL_BOOL_T goal_targetNetChanDataPresentCheck(
    void
)
{
    /* not implemented */
    return GOAL_TRUE;
}


/****************************************************************************/
/** Poll descriptor for data
 *
 * Not implemented
 *
 */
void goal_targetNetChanPoll(
    GOAL_NET_CHAN_T *pChanList                  /**< Net channel list */
)
{
    /* not implemented */
    UNUSEDARG(pChanList);
}


/****************************************************************************/
/** Configure IP address settings
 *
 * Configures the IP address, the subnet mask and the gateway of the network
 * interface also setting the internal IP temp flag.
 *
 * @retval GOAL_OK (always)
 */
static GOAL_STATUS_T goal_drvNetLwipIpSet(
    GOAL_MA_NET_T *pNetHdl,                     /**< pointer to store NET handler */
    uint32_t addrIp,                            /**< IP address */
    uint32_t addrMask,                          /**< subnet mask */
    uint32_t addrGw,                            /**< gateway */
    GOAL_BOOL_T flgTemp                         /**< temporary IP config flag */
)
{
    ip_addr_t ipaddr;                           /* lwIP addr struct (host IP) */
    ip_addr_t netmask;                          /* lwIP addr struct (netmask) */
    ip_addr_t gateway;                          /* lwIP addr struct (default gateway) */
    GOAL_STATUS_T res;                          /* result */
    GOAL_DRV_NET_LWIP_DATA_T *pPrvData;         /* private data */

    res = goal_lockGet(pLwIpLock, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    pPrvData = (GOAL_DRV_NET_LWIP_DATA_T *) pNetHdl->pDrvData;

    ipaddr.addr = GOAL_htobe32(addrIp);
    netmask.addr = GOAL_htobe32(addrMask);
    gateway.addr = GOAL_htobe32(addrGw);

    /* shutdown the interface */
    netif_set_down(pPrvData->pNetIf);

    /* Update the network interface instance */
    netif_set_addr(pPrvData->pNetIf, &ipaddr, &netmask, &gateway);

    /* enable the interface */
    netif_set_up(pPrvData->pNetIf);

    /* set internal IP temp flag */
    pPrvData->ipIsTemp = flgTemp;

    /* remember current IP address */
    pPrvData->currentIp = addrIp;

    /* release lock */
    goal_lockPut(pLwIpLock);

    return GOAL_OK;
}


/****************************************************************************/
/** Read IP address settings
 *
 * The functions writes the current setting of the network interface to the
 * corresponding variables. Null-pointers are skipped.
 *
 * @retval GOAL_OK (always)
 */
static GOAL_STATUS_T goal_drvNetLwipIpGet(
    GOAL_MA_NET_T *pHdlMaNet,                   /**< NET MA handle */
    uint32_t *pAddrIp,                          /**< IP address */
    uint32_t *pAddrMask,                        /**< subnet mask */
    uint32_t *pAddrGw,                          /**< gateway */
    GOAL_BOOL_T *pFlgTemp                       /**< temporary IP config flag */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_DRV_NET_LWIP_DATA_T *pPrvData;         /* private data */

    res = goal_lockGet(pLwIpLock, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    pPrvData = (GOAL_DRV_NET_LWIP_DATA_T *) pHdlMaNet->pDrvData;

    /* Extract host IP from netif structure */
    if (pAddrIp) {
        *pAddrIp = GOAL_be32toh(pPrvData->pNetIf->ip_addr.addr);
    }

    /* Extract IP mask from netif structure */
    if (pAddrMask) {
        *pAddrMask = GOAL_be32toh(pPrvData->pNetIf->netmask.addr);
    }

    /* Extract default gateway address from netif structure */
    if (pAddrGw) {
        *pAddrGw = GOAL_be32toh(pPrvData->pNetIf->gw.addr);
    }

    /* get temp flag */
    if (pFlgTemp) {
        *pFlgTemp = pPrvData->ipIsTemp;
    }

    /* release lock */
    goal_lockPut(pLwIpLock);

    /* Return successfully */
    return GOAL_OK;
}


/****************************************************************************/
/** Create target network channel
 *
 * Allocate a target network channel for the specified type.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_targetNetChanCreate(
    GOAL_NET_CHAN_T *pChan,                     /**< GOAL Net channel */
    GOAL_NET_TYPE_T type                        /**< channel type */
)
{
    GOAL_TARGET_NET_HANDLE_T *pTgtHdl;          /* new target handle */
    GOAL_STATUS_T res;                          /* result */
#if GOAL_FEAT_LWIP_INIT == 1
    void *pPcb;                                 /* allocated PCB */
    uint32_t pcbSize;                           /* size of PCB */
    memp_t poolType;                            /* type of mem pool */

    /* store away static PCBs so they won't be accidentally freed later */
    if (NULL == m_pStaticUdpPcb) {
        m_pStaticUdpPcb = memp_malloc(MEMP_UDP_PCB);
    }
#if GOAL_CONFIG_TCPIP_TCP == 1
    if (NULL == m_pStaticTcpPcb) {
        m_pStaticTcpPcb = memp_malloc(MEMP_TCP_PCB);
    }

    if (NULL == m_pStaticTcpListenerPcb) {
        m_pStaticTcpListenerPcb = memp_malloc(MEMP_TCP_PCB_LISTEN);
    }
#endif /* GOAL_CONFIG_TCPIP_TCP */
#endif /* GOAL_FEAT_LWIP_INIT */

#if GOAL_CONFIG_TCPIP_TCP == 1
    if ((GOAL_NET_UDP_SERVER != type) && (GOAL_NET_UDP_CLIENT != type) &&
        (GOAL_NET_TCP_LISTENER != type) && (GOAL_NET_TCP_CLIENT != type) && (GOAL_NET_TCP != type)) {
#else /* GOAL_CONFIG_TCPIP_TCP */
    if ((GOAL_NET_UDP_SERVER != type) && (GOAL_NET_UDP_CLIENT != type)) {
#endif /* GOAL_CONFIG_TCPIP_TCP */
        return GOAL_ERR_PARAM;
    }

    if (NULL == pChan) {
        return GOAL_ERR_NULL_POINTER;
    }

    pTgtHdl = NULL;
    res = goal_memCalloc(&pTgtHdl, sizeof(GOAL_TARGET_NET_HANDLE_T));
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:514: */
        goal_lmLog(GOAL_ID_DRV_NET, 514, 428, 0, 0, GOAL_LOG_SEV_ERROR, "allocation of target handle failed");
    }
    else {
        pTgtHdl->pGoalChan = pChan;
        pChan->pData = (void *) pTgtHdl;
    }


#if GOAL_FEAT_LWIP_INIT == 1
    if (GOAL_RES_OK(res)) {
        switch (type) {
            case GOAL_NET_UDP_SERVER:
                GOAL_TARGET_FALLTHROUGH;

            case GOAL_NET_UDP_CLIENT:
                pcbSize = (uint32_t) sizeof(struct udp_pcb);
                poolType = MEMP_UDP_PCB;
                break;

#if GOAL_CONFIG_TCPIP_TCP == 1
            case GOAL_NET_TCP:
                GOAL_TARGET_FALLTHROUGH;

            case GOAL_NET_TCP_CLIENT:
                pcbSize = (uint32_t) sizeof(struct tcp_pcb);
                poolType = MEMP_TCP_PCB;
                break;

            case GOAL_NET_TCP_LISTENER:
                pcbSize = (uint32_t) sizeof(struct tcp_pcb_listen);
                poolType = MEMP_TCP_PCB_LISTEN;
                break;
#endif /* GOAL_CONFIG_TCPIP_TCP */

            default:
                res = GOAL_ERR_PARAM;
                break;
        }
    }

    if (GOAL_RES_OK(res)) {
        /* allocate matching PCB */
        pPcb = NULL;
        res = goal_memCalloc(&pPcb, pcbSize);
        if (GOAL_RES_ERR(res)) {
            if (GOAL_NET_TCP_LISTENER == type) {
                /* GG_LOG ID:516: */
                goal_lmLog(GOAL_ID_DRV_NET, 516, 430, 0, 0, GOAL_LOG_SEV_ERROR, "allocation of listener PCB failed");
            }
            else {
                /* GG_LOG ID:512: */
                goal_lmLog(GOAL_ID_DRV_NET, 517, 431, 0, 0, GOAL_LOG_SEV_ERROR, "allocation of data PCB failed");
            }
        }
        else {
            /* add it to memory pool */
            memp_free(poolType, pPcb);
        }
    }
#endif /* GOAL_FEAT_LWIP_INIT */

    return res;
}


/****************************************************************************/
/** Delete target channel handle of a NET channel
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_targetNetChanDelete(
    GOAL_NET_CHAN_T *pChan                      /**< GOAL Net channel */
)
{
#if GOAL_FEAT_LWIP_INIT == 1
    void *pPcb;                                 /* generic PCB pointer */
#endif /* GOAL_FEAT_LWIP_INIT */

    if (NULL == pChan) {
        return GOAL_ERR_NULL_POINTER;
    }

    goal_memFree(&pChan->pData);

#if GOAL_FEAT_LWIP_INIT == 1
    if ((GOAL_NET_UDP_CLIENT == pChan->type) || (GOAL_NET_UDP_SERVER == pChan->type)) {
        /* release one allocated UDP PCBs */
        pPcb = memp_malloc(MEMP_UDP_PCB);
        if (NULL != pPcb) {
            goal_memFree(&pPcb);
        }
    }
#if GOAL_CONFIG_TCPIP_TCP == 1
    else if ((GOAL_NET_TCP_CLIENT == pChan->type) || (GOAL_NET_TCP == pChan->type)) {
        /* release one allocated TCP PCBs */
        pPcb = memp_malloc(MEMP_TCP_PCB);
        if (NULL != pPcb) {
            goal_memFree(&pPcb);
        }
    }
    else if (GOAL_NET_TCP_LISTENER == pChan->type) {
        /* release one allocated TCP Listener PCBs */
        pPcb = memp_malloc(MEMP_TCP_PCB_LISTEN);
        if (NULL != pPcb) {
            goal_memFree(&pPcb);
        }
    }
#endif /* GOAL_CONFIG_TCPIP_TCP */
#endif /* GOAL_FEAT_LWIP_INIT */

    return GOAL_OK;
}


/****************************************************************************/
/** Open a network channel
 *
 * Open a network handle for the given type and address parameters of the GOAL
 * NET channel.
 *
 * @retval GOAL_OK - success
 * @retval GOAL_ERR_NET_OPEN - fail
 */
GOAL_STATUS_T goal_targetNetChanOpen(
    GOAL_NET_CHAN_T *pChan,                     /**< channel handle */
    GOAL_BOOL_T flgReopen                       /**< reopen GOAL channel */
)
{
    struct udp_pcb * pUdpPcbDesc;               /* pointer to UDP PCB */
    err_t retValLwip;                           /* error value for lwIP */
#if GOAL_CONFIG_TCPIP_TCP == 1
    struct tcp_pcb * pTcpPcbDesc = NULL;        /* pointer to TCP PCB */
    struct tcp_pcb *pTcpListener = NULL;        /* pointer to TCP Listener */
    ip_addr_t remoteIp;                         /* lwIP IP address */
#endif /* GOAL_CONFIG_TCPIP_TCP */
    GOAL_STATUS_T res;                          /* result */
    GOAL_TARGET_NET_HANDLE_T *pHandle = NULL;   /* target Handle */
    ip_addr_t localIp;                          /* local IP address */

    if ((NULL == pChan) || (NULL == pChan->pData)) {
        return GOAL_ERR_PARAM;
    }

    res = goal_lockGet(pLwIpLock, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    pHandle = (GOAL_TARGET_NET_HANDLE_T *) pChan->pData;
    if (GOAL_FALSE == flgReopen) {
        /* reset handle  */
        GOAL_MEMSET(&pHandle->opts, 0, sizeof(GOAL_TARGET_NET_OPTION_T));
    }

    if ((GOAL_NET_UDP_SERVER == pChan->type) || (GOAL_NET_UDP_CLIENT == pChan->type)) {
        /* UDP server or client */

        /* Create a new UDP lwIP control block */
        pUdpPcbDesc = udp_new();
        if (pUdpPcbDesc == NULL) {
            /* GG_LOG ID:418: */
            goal_lmLog(GOAL_ID_DRV_NET, 418, 348, 0, 0, GOAL_LOG_SEV_ERROR, "Couldn't create new UDP channel descriptor");
            res = GOAL_ERR_NET_OPEN;
        }

        if (GOAL_RES_OK(res)) {
            /* make addresses reusable */
            pUdpPcbDesc->so_options |= SOF_REUSEADDR;

            /* Open local port for local IP address */
            GOAL_MEMSET(&localIp, 0, sizeof(localIp));
            localIp.addr = GOAL_htobe32(pChan->addr.localIp);
            retValLwip = udp_bind(pUdpPcbDesc, &localIp, pChan->addr.localPort);

            /* Check if operation was successful */
            if (ERR_OK != retValLwip) {
                /* GG_LOG ID:419: */
                goal_lmLog(GOAL_ID_DRV_NET, 419, 349, 0, 0, GOAL_LOG_SEV_ERROR, "Couldn't connect UDP channel");
                res = GOAL_ERR_NET_OPEN;
            } else {
                /* store local port if dynamic port as local client port was used */
                if ((GOAL_NET_UDP_CLIENT == pChan->type) && (0 == pChan->addr.localPort)) {
                    pChan->addr.localPort = pUdpPcbDesc->local_port;
                }
            }
        }

        if (GOAL_RES_OK(res)) {
            /* Set UDP receive callback */
            udp_recv(pUdpPcbDesc, goal_targetNetUdpRecvCb, pHandle);

            /* Update channel handle */
            pHandle->pPcbDesc = (void *) pUdpPcbDesc;
        }
    }
#if GOAL_CONFIG_TCPIP_TCP == 1
    else if (GOAL_NET_TCP_LISTENER == pChan->type) {
        /* TCP listener setup */
        pTcpPcbDesc = tcp_new();
        if (NULL == pTcpPcbDesc) {
            /* GG_LOG ID:420: */
            goal_lmLog(GOAL_ID_DRV_NET, 420, 350, 0, 0, GOAL_LOG_SEV_ERROR, "Couldn't create new TCP channel descriptor");
            res = GOAL_ERR_NET_OPEN;
        }

        if (GOAL_RES_OK(res)) {
            /* make addresses reusable */
            pTcpPcbDesc->so_options |= SOF_REUSEADDR;

            /* Open local port for any local IP address */
            retValLwip = tcp_bind(pTcpPcbDesc, IP_ADDR_ANY, pChan->addr.localPort);

            /* Check if operation was successful */
            if (ERR_OK != retValLwip) {
                /* GG_LOG ID:421: */
                goal_lmLog(GOAL_ID_DRV_NET, 421, 351, 0, 0, GOAL_LOG_SEV_ERROR, "Couldn't bind TCP channel");
                res = GOAL_ERR_NET_OPEN;
            }
        }

        if (GOAL_RES_OK(res)) {
            /* set socket into listen mode */
            pTcpListener = tcp_listen_with_backlog(pTcpPcbDesc, pChan->numDataChannels);
            if (NULL == pTcpListener) {
                /* GG_LOG ID:422: */
                goal_lmLog(GOAL_ID_DRV_NET, 422, 352, 0, 0, GOAL_LOG_SEV_ERROR, "Couldn't set TCP Listener to listen mode");
                tcp_close(pTcpPcbDesc);
                res = GOAL_ERR_NET_OPEN;
            }
        }

        if (GOAL_RES_OK(res)) {
            /* use self-reference as callback argument */
            tcp_arg(pTcpListener, (void *) pHandle);

            /* register accept callback */
            tcp_accept(pTcpListener, goal_tgtNetTcpAcceptCb);

            /* Update channel handle */
            pHandle->pPcbDesc = (void *) pTcpListener;
        }
    }
    else if ((GOAL_NET_TCP_CLIENT == pChan->type)) {
        /* TCP client setup */
        pTcpPcbDesc = tcp_new();
        if (NULL == pTcpPcbDesc) {
            /* GG_LOG ID:424: */
            goal_lmLog(GOAL_ID_DRV_NET, 424, 350, 0, 0, GOAL_LOG_SEV_ERROR, "Couldn't create new TCP channel descriptor");
            res = GOAL_ERR_NET_OPEN;
        }

        if (GOAL_RES_OK(res)) {
            /* make addresses reusable */
            pTcpPcbDesc->so_options |= SOF_REUSEADDR;

            /* Open local port for local IP address */
            retValLwip = tcp_bind(pTcpPcbDesc, (ip_addr_t *) &pChan->addr.localIp, pChan->addr.localPort);

            /* Check if operation was successful */
            if (ERR_OK != retValLwip) {
                /* GG_LOG ID:425: */
                goal_lmLog(GOAL_ID_DRV_NET, 425, 351, 0, 0, GOAL_LOG_SEV_ERROR, "Couldn't bind TCP channel");
                res = GOAL_ERR_NET_OPEN;
            } else {
                /* store local port if dynamic port as local port was used */
                if (0 == pChan->addr.localPort) {
                    pChan->addr.localPort = pTcpPcbDesc->local_port;
                }
            }
        }

        if (GOAL_RES_OK(res)) {
            /* use self-reference as callback argument */
            tcp_arg(pTcpPcbDesc, (void *) pHandle);

            /* register error handler */
            tcp_err(pTcpPcbDesc, goal_tgtNetTcpErrorHandlerCb);

            tcp_poll(pTcpPcbDesc, goal_tgtNetTcpPollCb, 2);

            /* try to connect to server */
            remoteIp.addr = GOAL_htobe32(pChan->addr.remoteIp);
            retValLwip = tcp_connect(pTcpPcbDesc, &remoteIp, pChan->addr.remotePort,
                                     goal_tgtNetTcpConnectCb);
            if (ERR_OK != retValLwip) {
                /* GG_LOG ID:426: */
                goal_lmLog(GOAL_ID_DRV_NET, 426, 353, 0, 0, GOAL_LOG_SEV_ERROR, "failed to connect");
                tcp_close(pTcpPcbDesc);
                res = GOAL_ERR_NET_OPEN;
            }
        }

        if (GOAL_RES_OK(res)) {
            /* Update channel handle */
            pHandle->pPcbDesc = (void *) pTcpPcbDesc;

            /* set Rx callback */
            tcp_recv(pTcpPcbDesc, goal_tgtNetTcpRxCb);

            /* activate channel */
            goal_targetNetChanActivate(pHandle->pGoalChan);
        }
    }
#endif /* GOAL_CONFIG_TCPIP_TCP */
    else {
        res = GOAL_ERR_UNSUPPORTED;
    }

    /* release lock */
    goal_lockPut(pLwIpLock);

    return res;
}


/****************************************************************************/
/** Activate a network channel
 *
 * Apply pending socket options
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_targetNetChanActivate(
    GOAL_NET_CHAN_T *pChan                      /**< channel handle */
)
{
    GOAL_TARGET_NET_HANDLE_T *pHandle;          /* Target handle */

    if ((NULL == pChan) || (NULL == pChan->pData)) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* this works for normal and listener target handles */
    pHandle = (GOAL_TARGET_NET_HANDLE_T *) pChan->pData;

    return goal_targetNetOptSetSocket(pHandle->pPcbDesc, &pHandle->opts);
}


/****************************************************************************/
/** Deactivate a network channel
 *
 * Not implemented
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_targetNetChanDeactivate(
    GOAL_NET_CHAN_T *pChan                      /**< channel handle */
)
{
    if (NULL == pChan) {
        return GOAL_ERR_NULL_POINTER;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Close a network channel
 *
 * This function closes the UDP connection of the lwIP stack corresponding to
 * the committed handle removing the UDP PCB.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_targetNetChanClose(
    GOAL_NET_CHAN_T *pChan                      /**< channel handle */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_TARGET_NET_HANDLE_T *pHandle;          /* target net handle */
#if GOAL_CONFIG_TCPIP_TCP == 1
    struct tcp_pcb *pTcpPcb;                    /* TCP socket */
#endif /* GOAL_CONFIG_TCPIP_TCP == 1 */

    if ((NULL == pChan) || (NULL == pChan->pData)) {
        return GOAL_ERR_NULL_POINTER;
    }

    res = goal_lockGet(pLwIpLock, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* get channel handle */
    pHandle = (GOAL_TARGET_NET_HANDLE_T *) pChan->pData;

    /* close PCB */
    if (GOAL_FALSE == pHandle->flgNotClose) {

        /* close descriptor */
        if ((GOAL_NET_UDP_SERVER == pChan->type) || (GOAL_NET_UDP_CLIENT == pChan->type)) {
            udp_remove(pHandle->pPcbDesc);
        }
#if GOAL_CONFIG_TCPIP_TCP == 1
        else {
            /* mark all pending data as received for a clean shutdown of connection */
            pTcpPcb = (struct tcp_pcb *) pHandle->pPcbDesc;
            tcp_recved(pTcpPcb, (u16_t) (TCP_WND_MAX(pTcpPcb) - pTcpPcb->rcv_wnd));
            tcp_close(pTcpPcb);
        }
#endif /* GOAL_CONFIG_TCPIP_TCP == 1 */
    }

    /* deactivate channel */
    pHandle->pPcbDesc = NULL;
    pHandle->flgNotClose = GOAL_FALSE;

    /* release lock */

    goal_lockPut(pLwIpLock);

    return GOAL_OK;
}


/****************************************************************************/
/** Send data over a network channel
 *
 * The function sends a GOAL buffer using the PCB of the target channel handle.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_targetNetChanSend(
    GOAL_NET_CHAN_T *pChan,                     /**< channel handle */
    GOAL_BUFFER_T *pBuf                         /**< send buffer */
)
{
    struct udp_pcb *pUdpPcb;                    /* UDP socket descriptor */
#if GOAL_CONFIG_TCPIP_TCP == 1
    struct tcp_pcb *pTcpPcb;                    /* TCP socket descriptor */
    GOAL_BOOL_T restorePcb = GOAL_FALSE;        /* restore tcp_input_pcb */
#endif
    struct pbuf *pLwipBuffer;                   /* lwIP buffer for data */
    struct pbuf *pLwipSeg = NULL;               /* lwIP buffer segment ptr */
    err_t retValLwip;                           /* lwIP error code */
    unsigned int pos = 0;                       /* GOAL buffer position */
    GOAL_STATUS_T res;                          /* result */
    GOAL_TARGET_NET_HANDLE_T *pTgtHdl;          /* target channel handle */

    if ((NULL == pChan) || (NULL == pChan->pData) || (NULL == pBuf)) {
        return GOAL_ERR_NULL_POINTER;
    }

    res = goal_lockGet(pLwIpLock, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    pTgtHdl = (GOAL_TARGET_NET_HANDLE_T *) pChan->pData;

    if ((GOAL_NET_UDP_SERVER == pChan->type) || (GOAL_NET_UDP_CLIENT == pChan->type)) {
        /* UDP PCB */
        pUdpPcb = (struct udp_pcb *) pTgtHdl->pPcbDesc;

        /* allocate lwIP buffer */
        pLwipBuffer = pbuf_alloc(PBUF_TRANSPORT, pBuf->dataLen, PBUF_POOL);
        if (NULL == pLwipBuffer) {
            /* GG_LOG ID:429: */
            goal_lmLog(GOAL_ID_DRV_NET, 429, 346, 0, 0, GOAL_LOG_SEV_ERROR, "Allocation of lwIp buffer failed");
            res = GOAL_ERROR;
        }

        if (GOAL_RES_OK(res)) {
            /* Copy data from GOAL buffer to lwIP buffer */
            for (pLwipSeg = pLwipBuffer; NULL != pLwipSeg; pLwipSeg = pLwipSeg->next) {
                GOAL_MEMCPY(pLwipSeg->payload, &pBuf->ptrData[pos], pLwipSeg->len);
                pos += pLwipSeg->len;
            }

            /* Set remote end in local PCB copy */
            pUdpPcb->remote_ip.addr = GOAL_htobe32(pChan->addr.remoteIp);
            pUdpPcb->remote_port = pChan->addr.remotePort;

            /* Send data via UDP */
            retValLwip = udp_send(pUdpPcb, pLwipBuffer);

            /* release UDP buffer */
            pbuf_free(pLwipBuffer);

            if (ERR_OK != retValLwip) {
                goal_logInfo("LWIP send error: retValLwip %u", retValLwip);
                goal_logInfo("LWIP send error: ERR_MEM %u", ERR_MEM);
                res = GOAL_ERR_NET_SEND;
            }
        }
    }
#if GOAL_CONFIG_TCPIP_TCP == 1
    else {
        /* TCP PCB */
        pTcpPcb = (struct tcp_pcb *) pTgtHdl->pPcbDesc;

        /* copy data to lwIP buffers */
        retValLwip = tcp_write(pTcpPcb, pBuf->ptrData, pBuf->dataLen, TCP_WRITE_FLAG_COPY);
        if (ERR_OK == retValLwip) {
            /* send it out immediately */
            if (tcp_input_pcb == pTcpPcb) {
                tcp_input_pcb = NULL;
                restorePcb = GOAL_TRUE;
            }
            retValLwip = tcp_output(pTcpPcb);
            if (GOAL_TRUE == restorePcb) {
                tcp_input_pcb = pTcpPcb;
            }
        }
        else {
            res = GOAL_ERR_NET_SEND;
        }
    }
#endif /* GOAL_CONFIG_TCPIP_TCP == 1 */

    /* release lock */
    goal_lockPut(pLwIpLock);

    return res;
}


/****************************************************************************/
/** Set network channel properties
 *
 * Option value will be stored in target handle structure. If handle is
 * active already the option will be set to the matching socket.
 *
 * GOAL_NET_OPTION_MCAST option will be set immediately.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_targetNetChanOptSet(
    GOAL_NET_CHAN_T *pChan,                     /**< channel handle */
    GOAL_NET_OPTION_T option,                   /**< net option */
    void *pValue                                /**< net option value */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* return value */
    GOAL_NET_MCAST_GROUP_T *pGoalMcastGrp;      /* GOAL Multicast Group */
    ip4_addr_t ifAddr;                          /* interface address */
    ip4_addr_t groupAddr;                       /* multicast group address */
    GOAL_TARGET_NET_HANDLE_T *pHandle;          /* target handle */

    if ((NULL == pChan) || (NULL == pChan->pData)) {
        return GOAL_ERR_NULL_POINTER;
    }

    pHandle = (GOAL_TARGET_NET_HANDLE_T *) pChan->pData;

    switch (option) {
        case GOAL_NET_OPTION_REUSEADDR:
            pHandle->opts.optMask |= GOAL_TARGET_NET_OPTION_REUSEADDR;
            GOAL_MEMCPY(&pHandle->opts.optValReuseaddr, pValue, sizeof(pHandle->opts.optValReuseaddr));
            break;

        case GOAL_NET_OPTION_NONBLOCK:
            /* lwIP sockets are nonblocking */
            return GOAL_OK;

        case GOAL_NET_OPTION_BROADCAST:
            pHandle->opts.optMask |= GOAL_TARGET_NET_OPTION_BROADCAST;
            GOAL_MEMCPY(&pHandle->opts.optValBroadcast, pValue, sizeof(pHandle->opts.optValBroadcast));
            break;

        case GOAL_NET_OPTION_TTL:
            if ((GOAL_NET_UDP_SERVER == pChan->type) || (GOAL_NET_UDP_CLIENT == pChan->type)) {
                pHandle->opts.optMask |= GOAL_TARGET_NET_OPTION_TTL;
                GOAL_MEMCPY(&pHandle->opts.optValTtl, pValue, sizeof(pHandle->opts.optValTtl));
            }
            else {
                /* not supported for TCP */
                res = GOAL_ERR_NET_OPTION;
            }
            break;

        case GOAL_NET_OPTION_TOS:
            pHandle->opts.optMask |= GOAL_TARGET_NET_OPTION_TOS;
            GOAL_MEMCPY(&pHandle->opts.optValTos, pValue, sizeof(pHandle->opts.optValTos));
            break;

        case GOAL_NET_OPTION_MCAST_IF:
            /* not implemented */
            return GOAL_OK;

        case GOAL_NET_OPTION_MCAST_ADD:
            /* get lock */
            res = goal_lockGet(pLwIpLock, GOAL_LOCK_INFINITE);
            if (GOAL_RES_ERR(res)) {
                return res;
            }

            pGoalMcastGrp = (GOAL_NET_MCAST_GROUP_T *) pValue;
            groupAddr.addr = GOAL_htobe32(pGoalMcastGrp->mcastIp);
            ifAddr.addr = GOAL_htobe32(pGoalMcastGrp->localIp);
            if (ERR_OK != igmp_joingroup(&ifAddr, &groupAddr)) {
                res = GOAL_ERR_NET_OPTION;
            }

            /* release lock */
            goal_lockPut(pLwIpLock);

            break;

        case GOAL_NET_OPTION_MCAST_DROP:
            /* get lock */
            res = goal_lockGet(pLwIpLock, GOAL_LOCK_INFINITE);
            if (GOAL_RES_ERR(res)) {
                return res;
            }

            pGoalMcastGrp = (GOAL_NET_MCAST_GROUP_T *) pValue;
            groupAddr.addr = GOAL_htobe32(pGoalMcastGrp->mcastIp);
            ifAddr.addr = GOAL_htobe32(pGoalMcastGrp->localIp);
            if (ERR_OK != igmp_leavegroup(&ifAddr, &groupAddr)) {
                res = GOAL_ERR_NET_OPTION;
            }

            /* release lock */
            goal_lockPut(pLwIpLock);

            break;

        default:
            res = GOAL_ERR_NET_OPTION;
            /* GG_LOG ID:430: */
            goal_lmLog(GOAL_ID_DRV_NET, 430, 356, 4, 0, GOAL_LOG_SEV_ERROR, "invalid option $1");
            goal_lmLogParamINT32(option);
            goal_lmLogFinish();
            break;
    }


    /* set option if channel is active already */
    if (GOAL_RES_OK(res) && (pChan->flags & GOAL_NET_FLAGS_ACTIVE)) {
        res = goal_targetNetOptSetSocket(pHandle->pPcbDesc, &pHandle->opts);
    }

    return res;
}


/****************************************************************************/
/** Set all registered network channel properties
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
static GOAL_STATUS_T goal_targetNetOptSetSocket(
    void *pDesc,                                /**< descriptor ptr */
    GOAL_TARGET_NET_OPTION_T *pOpt              /**< pointer to options */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    struct udp_pcb *pUdpPcb;                    /* UDP socket descriptor */

    /* get lock */
    res = goal_lockGet(pLwIpLock, GOAL_LOCK_INFINITE);

    /* UDP PCB & TCP PCB have same IP fields, so this works for both types */
    pUdpPcb = (struct udp_pcb *) pDesc;

    /* set broadcast option */
    if (GOAL_RES_OK(res) && (pOpt->optMask & GOAL_TARGET_NET_OPTION_BROADCAST)) {
        if (0 != pOpt->optValBroadcast) {
            pUdpPcb->so_options |= SOF_BROADCAST;
        }
        else {
            pUdpPcb->so_options &= ~SOF_BROADCAST;
        }
    }

    /* set ttl option (UDP Multicast only) */
    if (GOAL_RES_OK(res) && (pOpt->optMask & GOAL_TARGET_NET_OPTION_TTL)) {
        pUdpPcb->mcast_ttl = pOpt->optValTtl;
    }

    /* set tos option */
    if (GOAL_RES_OK(res) && (pOpt->optMask & GOAL_TARGET_NET_OPTION_TOS)) {
        pUdpPcb->tos = pOpt->optValTos;
    }

    /* set reuseaddress option */
    if (GOAL_RES_OK(res) && (pOpt->optMask & GOAL_TARGET_NET_OPTION_REUSEADDR)) {
        if (0 != pOpt->optValReuseaddr) {
            pUdpPcb->so_options |= SOF_REUSEADDR;
        }
        else {
            pUdpPcb->so_options &= ~SOF_REUSEADDR;
        }
    }

    /* release lock */
    goal_lockPut(pLwIpLock);

    return res;
}


#if GOAL_FEAT_LWIP_INIT == 1
/****************************************************************************/
/** Output function to abstraction layer
 *
 * This function outputs the prepared frames from lwIP to the abstraction layer.
 * It is connected to the "link_output" of the network interface structure and
 * called automatically from the stack.
 *
 * The abstraction layer will forward the frames to the physical layer.
 *
 * @retval ERR_OK success
 * @retval other fail
 */
static err_t goal_targetLwipOutput(
    struct netif *pNetifPtr,                    /**< lwIP network interface handle */
    struct pbuf *pLwipBuffer                    /**< lwIP puffer to send via ethernet */
)
{
    GOAL_STATUS_T resGoal;                      /* GOAL result */
    err_t resLwip = ERR_OK;                     /* lwIP result */
#if (GOAL_CONFIG_EPL == 0)
    GOAL_BUFFER_T *pGoalBufferLwip = NULL;      /* internal GOAL buffer */
    GOAL_MA_NET_T *pHdlMaNet;                   /* NET MA handle */
    GOAL_DRV_NET_LWIP_DATA_T *pPrvData;         /* private data */
#else
    uint8_t *pBuf = NULL;                       /* pointer for buffer */
    uint16_t size = 0;                          /* size of unspec buffer */
#endif
    struct pbuf *pLwipSeg;                      /* lwIP buffer segment ptr */
    unsigned int pos;                           /* GOAL buffer position */

    resGoal = goal_lockGet(pLwIpLock, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(resGoal)) {
        return ERR_IF;
    }

    /* Check if the lwIP buffer pointer is emtpy */
    if (NULL == pLwipBuffer) {
        /* GG_LOG ID:431: */
        goal_lmLog(GOAL_ID_DRV_NET, 431, 346, 0, 0, GOAL_LOG_SEV_ERROR, "Allocation of lwIp buffer failed");
        resLwip = ERR_IF;
    }

#if GOAL_CONFIG_EPL == 0
    if (ERR_OK == resLwip) {
        pHdlMaNet = pNetifPtr->state;
        pPrvData = (GOAL_DRV_NET_LWIP_DATA_T *) pHdlMaNet->pDrvData;

        /* Get new GOAL buffer for output data */
        resGoal = goal_ethGetNetBuf(&pGoalBufferLwip);
        if (GOAL_RES_ERR(resGoal)) {
            /* GG_LOG ID:432: */
            goal_lmLog(GOAL_ID_DRV_NET, 432, 357, 0, 0, GOAL_LOG_SEV_ERROR, "Unable to allocate a free GOAL buffer");
            resLwip = ERR_IF;
        }
    }

    if (ERR_OK == resLwip) {
        /* Set data length in GOAL buffer */
        pGoalBufferLwip->dataLen = pLwipBuffer->tot_len;

        /* Copy data from (multiple) lwIP buffer to GOAL buffer */
        for (pLwipSeg = pLwipBuffer, pos = 0; NULL != pLwipSeg; pLwipSeg = pLwipSeg->next) {
            GOAL_MEMCPY(pGoalBufferLwip->ptrData + pos, pLwipSeg->payload, pLwipSeg->len);
            pos += pLwipSeg->len;
        }

        /* Hand over buffer to abstraction layer for sending */
        if (GOAL_OK != goal_miEthSend(pPrvData->pMiEthHdl, GOAL_NET_TX_LOW, (void **) &pGoalBufferLwip)) {
            /* GG_LOG ID:433: */
            goal_lmLog(GOAL_ID_DRV_NET, 433, 358, 0, 0, GOAL_LOG_SEV_ERROR, "Unable to send frame");
            resLwip = ERR_IF;
        }

    }
#else
    UNUSEDARG(pNetifPtr);

    if (ERR_OK == resLwip) {
        /* Get new buffer for output data */
        if (0 != (uint8_t) getUnspecBuf(&pBuf, &size)) {
            goal_logErr("Unable to allocate a free GOAL buffer");
            resLwip = ERR_IF;
        }
    }

    if (ERR_OK == resLwip) {
        /* Validate size on output data buffer */
        if (size < (uint16_t) pLwipBuffer->tot_len) {
            goal_logWarn("lwip frame dropped");
            resLwip = ERR_IF;
        }
    }

    if (ERR_OK == resLwip) {
        /* Copy data from (multiple) lwIP buffer to GOAL buffer */
        for (pLwipSeg = pLwipBuffer, pos = 0; NULL != pLwipSeg; pLwipSeg = pLwipSeg->next) {
            GOAL_MEMCPY(pBuf + pos, pLwipSeg->payload, pLwipSeg->len);
            pos += pLwipSeg->len;
        }

        /* Hand over buffer to abstraction layer for sending */
        if (0 != (uint8_t) sendUnspecBuf((uint8_t *) pBuf, (uint16_t) (pLwipBuffer->tot_len))) {
            goal_logErr("Unable to send frame");
            resLwip = ERR_IF;
        }
    }
#endif /* (GOAL_CONFIG_EPL == 0) */

    /* release lock */
    goal_lockPut(pLwIpLock);

    return resLwip;
}
#endif /* GOAL_FEAT_LWIP_INIT == 1 */


/****************************************************************************/
/** UDP Application Callback
 *
 * This function is called by the stack when an UDP datagram has been received.
 *
 * It writes the remote IP and port like the local IP and port number to a goal
 * address structure. The incoming frame is transfered to GOAL together with
 * this address structure.
 *
 * Frees the buffer of the incoming frame when finishes.
 */
static void goal_targetNetUdpRecvCb(
    void *pArg,                                 /**< arg user supplied argument (udp_pcb.recv_arg) */
    struct udp_pcb *pUdpPcbDesc,                /**< pcb the udp_pcb which received data */
    struct pbuf *pLwipBuffer,                   /**< pointer the packet buffer that was received */
    const ip_addr_t *pAddrRemote,               /**< addr of the remote IP address from which the packet was received */
    u16_t remote_port                           /**< remote port which received the udp packet */
)
{
    GOAL_NET_ADDR_T goalAddr;                   /* GOAL address structure */
    GOAL_BUFFER_T * pGoalBufferLwip = NULL;     /* internal GOAL buffer */
    struct pbuf *pLwipSeg = NULL;               /* lwIP buffer segment ptr */
    unsigned int pos = 0;                       /* GOAL buffer position */
    GOAL_TARGET_NET_HANDLE_T *pHandle;          /* target channel handle */
    GOAL_STATUS_T res;                          /* result */

    pHandle = (GOAL_TARGET_NET_HANDLE_T *) pArg;

    /* Get a free GOAL buffer */
    res = goal_ethGetNetBuf(&pGoalBufferLwip);
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:434: */
        goal_lmLog(GOAL_ID_DRV_NET, 434, 359, 0, 0, GOAL_LOG_SEV_ERROR, "Couldn't get a new GOAL buffer");
        pbuf_free(pLwipBuffer);
        return;
    }

    /* Update GOAL address structure */
    goalAddr.localIp = GOAL_be32toh(pUdpPcbDesc->local_ip.addr);
    goalAddr.remoteIp = GOAL_be32toh((uint32_t) pAddrRemote->addr);
    goalAddr.localPort = (uint16_t) pUdpPcbDesc->local_port;
    goalAddr.remotePort = (uint16_t) remote_port;

    /* Add data length information to GOAL buffer */
    pGoalBufferLwip->dataLen = pLwipBuffer->tot_len;

    /* Copy data from lwIP buffer to GOAL buffer */
    for (pLwipSeg = pLwipBuffer; NULL != pLwipSeg; pLwipSeg = pLwipSeg->next) {
        GOAL_MEMCPY(&pGoalBufferLwip->ptrData[pos], pLwipSeg->payload, pLwipSeg->len);
        pos += pLwipSeg->len;
    }

    /* Free received lwIP buffer */
    if (0 == pbuf_free(pLwipBuffer)) {
        /* GG_LOG ID:435: */
        goal_lmLog(GOAL_ID_DRV_NET, 435, 360, 0, 0, GOAL_LOG_SEV_ERROR, "Couldn't free any lwIp buffer");
        /* TODO: call exception handler instead of goal_targetHalt() */
    }

    /* goal_netChanRecv() will not release the committed GOAL buffer */
    goal_netChanRecv(pHandle->pGoalChan, &goalAddr, pGoalBufferLwip);

    /* Release allocated GOAL buffer */
    goal_queueReleaseBuf(&pGoalBufferLwip);
}


#if GOAL_CONFIG_TCPIP_TCP == 1
/****************************************************************************/
/** TCP polling callback handler
 *
 * This function is called by the lwIP stack for TCP sockets that are idle.
 * The handler checks if a closing TCP socket is closed within a time limit.
 * Otherwise the connection is reset.
 *
 * @retval ERR_OK success
 * @retval others fail
 */
static err_t goal_tgtNetTcpPollCb(
    void *pArg,                                 /**< callback argument (not used) */
    struct tcp_pcb *pTcpPcb                     /**< polled TCP socket */
)
{
    UNUSEDARG(pArg);

    if (TCP_STATE_IS_CLOSING(pTcpPcb->state)) {
        /* - socket is closing
         * - for GOAL it is already closed
         * - count poll intervals and reset connection
         */
        if (0 == (pTcpPcb->so_options & SOF_KEEPALIVE)) {
            /* SOF_KEEPALIVE is not used, use keep_cnt_sent as timeout count */
            pTcpPcb->keep_cnt_sent++;
            if (GOAL_TARGET_NET_TCP_CLS_TMO_CNT < pTcpPcb->keep_cnt_sent) {
                pTcpPcb->keep_cnt_sent = 0;
                tcp_abort(pTcpPcb);
                return ERR_ABRT;
            }
        }
        else {
            pTcpPcb->persist_cnt++;
            if (GOAL_TARGET_NET_TCP_CLS_TMO_CNT < pTcpPcb->persist_cnt) {
                pTcpPcb->persist_cnt = 0;
                tcp_abort(pTcpPcb);
                return ERR_ABRT;
            }
        }
    }

    return ERR_OK;
}


/****************************************************************************/
/** TCP Listener Acceptance callback
 *
 * This function is called by the lwIP stack if the TCP Listener connected to
 * a new TCP client. It tries to open a new GOAL channel.
 *
 * @retval ERR_OK success
 * @retval others fail
 */
static err_t goal_tgtNetTcpAcceptCb(
    void *pArg,                                 /**< listener handle */
    struct tcp_pcb *pNewpcb,                    /**< new socket */
    err_t err                                   /**< status */
)
{
    GOAL_NET_ADDR_T addr;                       /* address structure */
    GOAL_STATUS_T res;                          /* result */
    GOAL_TARGET_NET_HANDLE_T *pListen;          /* target listener channel */
    GOAL_NET_CHAN_T *pDataChan = NULL;          /* data channel belonging to listener */
    GOAL_TARGET_NET_HANDLE_T *pTgtHdl;          /* new target handle */

    if ((ERR_OK != err) || (NULL == pNewpcb)) {
        return ERR_VAL;
    }

    pListen = (GOAL_TARGET_NET_HANDLE_T *) pArg;

    /* set address structure */
    addr.remoteIp = GOAL_be32toh(pNewpcb->remote_ip.addr);
    addr.localIp = GOAL_be32toh(pNewpcb->local_ip.addr);
    addr.remotePort = pNewpcb->remote_port;
    addr.localPort = pNewpcb->local_port;

    /* get GOAL data channel associated with listener */
    res = goal_netChanFreeDataChanGet(pListen->pGoalChan, &pDataChan);
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:438: */
        goal_lmLog(GOAL_ID_DRV_NET, 438, 363, 0, 0, GOAL_LOG_SEV_INFO, "error getting channel from GOAL");
    }

    if (GOAL_RES_OK(res)) {
        /* update PCB in target channel handle */
        pTgtHdl = (GOAL_TARGET_NET_HANDLE_T *) pDataChan->pData;
        pTgtHdl->pPcbDesc = (void *) pNewpcb;

        /* set Rx callback */
        tcp_recv(pNewpcb, goal_tgtNetTcpRxCb);

        /* register error handler */
        tcp_err(pNewpcb, goal_tgtNetTcpErrorHandlerCb);

        tcp_poll(pNewpcb, goal_tgtNetTcpPollCb, 2);

        /* use self-reference as callback argument */
        tcp_arg(pNewpcb, (void *) pTgtHdl);

        res = goal_netChanOpenTgt(pDataChan, &addr);
    }

    return (GOAL_RES_OK(res)) ? ERR_OK : ERR_VAL;
}


/****************************************************************************/
/** TCP Receive Callback
 *
 * This function is called by the TCP/IP stack when TCP data has been received.
 *
 * Frees the buffer of the incoming frame when finishes.
 *
 * @retval ERR_OK success
 * @retval others fail
 */
static err_t goal_tgtNetTcpRxCb(
    void *pArg,                                 /**< target handle */
    struct tcp_pcb *pTcpPcb,                    /**< TCP socket handle */
    struct pbuf *pLwIpBuf,                      /**< data buffer */
    err_t err                                   /**< error code */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_BUFFER_T * pGoalBuf = NULL;            /* internal GOAL buffer */
    struct pbuf *pLwipSeg = NULL;               /* lwIP buffer segment ptr */
    uint16_t pos = 0;                           /* GOAL buffer position */
    uint32_t cpLen;                             /* already copied bytes */
    uint32_t maxLen;                            /* maximum buffer length */
    GOAL_TARGET_NET_HANDLE_T *pTgtHdl;          /* current target handle in list */

    /* convert to GOAL channel */
    pTgtHdl = (GOAL_TARGET_NET_HANDLE_T *) pArg;
    if (NULL == pTgtHdl) {
         /* GG_LOG ID:439: */
        goal_lmLog(GOAL_ID_DRV_NET, 439, 364, 0, 0, GOAL_LOG_SEV_ERROR, "Could not find target handle to given descriptor.");
        return ERR_VAL;
    }

    if ((ERR_OK != err) || (NULL == pLwIpBuf)) {
        if (NULL != pLwIpBuf) {
            pbuf_free(pLwIpBuf);
            goal_logErr("receive error for socket %p (target socket address): %d", pTgtHdl->pPcbDesc, err);
        }
        else {
            /* socket was closed by remote endpoint */
            goal_logInfo("socket %p closed by remote endpoint", (void *) pTgtHdl);

            /* close channel */
            res = goal_netChanCloseTgt(pTgtHdl->pGoalChan);
            if (GOAL_RES_ERR(res)) {
                tcp_close(pTcpPcb);
            }

            return ERR_OK;
        }
    }

    pLwipSeg = pLwIpBuf;
    cpLen = 0;

    while (cpLen < pLwIpBuf->tot_len) {
        /* Get a free GOAL buffer */
        res = goal_ethGetNetBuf(&pGoalBuf);
        if (GOAL_RES_ERR(res)) {
            /* GG_LOG ID:441: */
            goal_lmLog(GOAL_ID_DRV_NET, 441, 359, 0, 0, GOAL_LOG_SEV_ERROR, "Couldn't get a new GOAL buffer");
            return ERR_MEM;
        }

        pos = 0;
        maxLen = ((pLwIpBuf->tot_len - cpLen) > pGoalBuf->bufSize) ? pGoalBuf->bufSize : pLwIpBuf->tot_len - cpLen;

        /* Copy data from lwIP buffer to GOAL buffer */
        for (; NULL != pLwipSeg; pLwipSeg = pLwipSeg->next) {
            if ((pos + pLwipSeg->len) > maxLen) {
                /* segment does not fit into remaining GOAL buffer */
                break;
            }
            GOAL_MEMCPY(&pGoalBuf->ptrData[pos], pLwipSeg->payload, pLwipSeg->len);
            pos += pLwipSeg->len;
        }
        pGoalBuf->dataLen = pos;

        /* pass to upper layer */
        goal_netChanRecv(pTgtHdl->pGoalChan, NULL, pGoalBuf);

        /* release buffer */
        goal_queueReleaseBuf(&pGoalBuf);

        /* update copied length bytes */
        cpLen += pos;
    }

    /* Free received lwIP buffer */
    if (0 == pbuf_free(pLwIpBuf)) {
        /* GG_LOG ID:442: */
        goal_lmLog(GOAL_ID_DRV_NET, 442, 360, 0, 0, GOAL_LOG_SEV_ERROR, "Couldn't free any lwIp buffer");
        /* TODO: call exception handler instead of goal_targetHalt() */
    }

    /* update Rx window */
    tcp_recved(pTcpPcb, cpLen);

    return ERR_OK;
}


/****************************************************************************/
/** TCP Client connected to server
 *
 * This function is called by the TCP/IP stack when a TCP session was
 * successfully established with a server.
 *
 * @retval ERR_OK success
 * @retval others fail
 */
static err_t goal_tgtNetTcpConnectCb(
    void *pArg,                                 /**< unused */
    struct tcp_pcb *pTcpPcb,                    /**< TCP socket */
    err_t err                                   /**< error code */
)
{
    GOAL_TARGET_NET_HANDLE_T *pTgtHdl;          /* new target handle */

    pTgtHdl = (GOAL_TARGET_NET_HANDLE_T *) pArg;

    if (NULL == pTgtHdl) {
        /* GG_LOG ID:443: */
        goal_lmLog(GOAL_ID_DRV_NET, 443, 366, 8, 0, GOAL_LOG_SEV_ERROR, "no channel handle for socket $1 (target socket address)");
        goal_lmLogParamPTR((void *) pTcpPcb);
        goal_lmLogFinish();
        return ERR_VAL;
    }

    if (ERR_OK != err) {
        /* GG_LOG ID:444: */
        goal_lmLog(GOAL_ID_DRV_NET, 444, 367, 4, 8, GOAL_LOG_SEV_WARNING, "connect error $1 for socket $2 (target socket address)");
        goal_lmLogParamINT32(err);
        goal_lmLogParamPTR((void *) pTcpPcb);
        goal_lmLogFinish();
        goal_netChanCloseTgt(pTgtHdl->pGoalChan);
        tcp_close(pTcpPcb);

        return ERR_OK;
    }

    /* inform upper layer */
    goal_netChanConnectedTgt(pTgtHdl->pGoalChan);

    return ERR_OK;
}


/****************************************************************************/
/** TCP Error handler
 *
 * This function is called by the TCP/IP stack when a TCP session could not be
 * established with a server or if a server socket is aborted.
 *
 * @retval ERR_OK success
 * @retval others fail
 */
static void goal_tgtNetTcpErrorHandlerCb(
    void *pArg,                                 /**< target channel handle */
    err_t err                                   /**< error code */
)
{
    GOAL_TARGET_NET_HANDLE_T *pTgtHdl;          /* current target handle in list */

    pTgtHdl = (GOAL_TARGET_NET_HANDLE_T *) pArg;

    if (NULL == pTgtHdl) {
        return;
    }

    if ((GOAL_NET_TCP == pTgtHdl->pGoalChan->type) && (0 == (pTgtHdl->pGoalChan->flags & GOAL_NET_FLAGS_ACTIVE))) {
        /* received data for net channel that has been closed already */
        return;
    }

    if ((ERR_ABRT == err) || ((ERR_RST == err) && (GOAL_NET_TCP_CLIENT == pTgtHdl->pGoalChan->type))) {
        /* PCB is already released, avoid releasing it twice */
        pTgtHdl->flgNotClose = GOAL_TRUE;
    }

    /* GG_LOG ID:446: */
    goal_lmLog(GOAL_ID_DRV_NET, 446, 369, 4, 8, GOAL_LOG_SEV_WARNING, "connect error $1 for socket $2 (target socket address)");
    goal_lmLogParamINT32(err);
    goal_lmLogParamPTR((void *) pTgtHdl->pPcbDesc);
    goal_lmLogFinish();
    goal_netChanCloseTgt(pTgtHdl->pGoalChan);
}
#endif /* GOAL_CONFIG_TCPIP_TCP == 1 */


#if GOAL_FEAT_LWIP_INIT == 0
/****************************************************************************/
/** Set the Network interface for a driver instance
 *
 * This function is only applicable if lwIP is not initialized by GOAL.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_drvLwipNetIfHdlSet(
    uint32_t idMa,                              /**< MA ID */
    struct netif *pNetIfNew                     /**< lwIP iface handle */
)
{
#if GOAL_CONFIG_IP_STATS == 1
    uint32_t cnt;                               /* counter */
#endif /* GOAL_CONFIG_IP_STATS */
    GOAL_MA_NET_T *pMaHdl;                      /* MA handle */
    GOAL_STATUS_T res;                          /* result */
    GOAL_DRV_NET_LWIP_DATA_T *pPrvData;         /* private data */

    /* get ma handle and bind together */
    res = goal_maNetGetById(&pMaHdl, idMa);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    pPrvData = (GOAL_DRV_NET_LWIP_DATA_T *) pMaHdl->pDrvData;

    pPrvData->pNetIf = pNetIfNew;

#if GOAL_CONFIG_IP_STATS == 1
    lwipStatsMask = 0;

    /* Calculate statistics mask for common counters */
    for (cnt = 0; (cnt < GOAL_NET_IP_STATS_END) && (cnt < ARRAY_ELEMENTS(ipStatsRegs)); cnt++) {

        /* check if statistics entry is supported */
        if (NULL != ipStatsRegs[cnt]) {
            lwipStatsMask |= (1 << cnt);
        }
    }

    /* Calculate statistics mask for interface specific counters */
    lwipStatsMask |= (1 << GOAL_NET_IP_STATS_IFINOCTETS);
    lwipStatsMask |= (1 << GOAL_NET_IP_STATS_IFINUCASTPKTS);
    lwipStatsMask |= (1 << GOAL_NET_IP_STATS_IFINNUCASTPKTS);
    lwipStatsMask |= (1 << GOAL_NET_IP_STATS_IFINDISCARDS);
    lwipStatsMask |= (1 << GOAL_NET_IP_STATS_IFINERRORS);
    lwipStatsMask |= (1 << GOAL_NET_IP_STATS_IFINUNKOWNPROTOS);
    lwipStatsMask |= (1 << GOAL_NET_IP_STATS_IFOUTOCTETS);
    lwipStatsMask |= (1 << GOAL_NET_IP_STATS_IFOUTUCASTPKTS);
    lwipStatsMask |= (1 << GOAL_NET_IP_STATS_IFOUTNUCASTPKTS);
    lwipStatsMask |= (1 << GOAL_NET_IP_STATS_IFOUTDISCARDS);
    lwipStatsMask |= (1 << GOAL_NET_IP_STATS_IFOUTERRORS);
#endif /* GOAL_CONFIG_IP_STATS == 1 */

    return GOAL_OK;
}
#endif /* GOAL_FEAT_LWIP_INIT == 0 */


/****************************************************************************/
/** LwIP time function
 *
 * This function is demanded by the lwIP stack and returns the current time
 * in milliseconds.
 *
 * @retval u32_t Current Time value (ms)
 */
u32_t sys_now(
    void
)
{
    return (u32_t) goal_targetGetTimestamp();
}


#if GOAL_FEAT_LWIP_INIT == 1
/****************************************************************************/
/** Callback for received Ethernet frames
 *
 * This function is called every time an Ethernet frame for the TCP/IP stack is
 * received. It represents the input function from the abstraction layer to the
 * lwIp stack and processes the committed GOAL buffer.
 *
 * Steps:
 *  - extract data length from GOAL buffer
 *  - allocate lwIP buffer
 *  - copy complete frame from GOAL buffer into lwIP buffer
 *  - release GOAL buffer
 *  - call lwip netif input callback for lwIP buffer
 *  - in case of an error: release lwIP buffer
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
static GOAL_STATUS_T goal_targetNetChanRecv(
    GOAL_BUFFER_T **ppGoalBuffer,               /**< GOAL Ethernet buffer */
    void *pArg                                  /**< callback argument */
)
{
    struct pbuf *pLwipBuf = NULL;               /* lwIP buffer pointer */
    struct pbuf *pLwipSeg = NULL;               /* lwIP buffer segment ptr */
    unsigned int pos = 0;                       /* GOAL buffer position */
    err_t err;                                  /* error code */
    GOAL_STATUS_T res;                          /* result */
    GOAL_MA_NET_T *pHdlMaNet;                   /* NET MA handle */
    GOAL_DRV_NET_LWIP_DATA_T *pPrvData;         /* private data */

    pHdlMaNet = (GOAL_MA_NET_T *) pArg;
    pPrvData = (GOAL_DRV_NET_LWIP_DATA_T *) pHdlMaNet->pDrvData;

    res = goal_lockGet(pLwIpLock, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* allocate new lwIP buffer to transfer frame into the stack */
    pLwipBuf = pbuf_alloc(PBUF_RAW, (*ppGoalBuffer)->dataLen, PBUF_POOL);

    /* Check if the lwIP buffer was allocated successfully */
    if (NULL == pLwipBuf) {
        /* GG_LOG ID:416: */
        goal_lmLog(GOAL_ID_DRV_NET, 416, 346, 0, 0, GOAL_LOG_SEV_ERROR, "Allocation of lwIp buffer failed");
        res = GOAL_ERROR;
    }

    if (GOAL_RES_OK(res)) {
        /* copy received frame from GOAL buffer to lwIP buffer */
        for (pLwipSeg = pLwipBuf; NULL != pLwipSeg; pLwipSeg = pLwipSeg->next) {
            GOAL_MEMCPY(pLwipSeg->payload, &(*ppGoalBuffer)->ptrData[pos], pLwipSeg->len);
            pos += pLwipSeg->len;
        }

        /* Transfer lwIP buffer into lwIP stack for further processing */
        err = pPrvData->pNetIf->input(pLwipBuf, pPrvData->pNetIf);
        if (ERR_OK != err) {
            res = GOAL_ERROR;
        }
    }

    if (GOAL_RES_OK(res)) {
        /* Release GOAL buffer */
        goal_queueReleaseBuf(ppGoalBuffer);
    }

    /* release lock */
    goal_lockPut(pLwIpLock);

    return res;
}


/****************************************************************************/
/** Time-based callback to trigger lwIP timer check function
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static void goal_targetNetTimerCbLwip(
    void                                        /**< timer argument */
)
{
    GOAL_STATUS_T res;                          /* result */

    res = goal_lockGet(pLwIpLock, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(res)) {
        return;
    }

    /* Check lwIP DCHP timeouts */
    sys_check_timeouts();

    goal_lockPut(pLwIpLock);
}
#endif /* GOAL_FEAT_LWIP_INIT */


#if GOAL_CONFIG_IP_STATS == 1
/****************************************************************************/
/** Retrieves IP statistic counter(s)
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_targetIpStatsGet(
    struct netif *pNetIf,                       /**< netif handle */
    GOAL_NET_IP_STATS_GET_T * const pStats      /**< statistics get request */
)
{
    GOAL_NET_IP_STATS_COUNTER_T *pCounter;      /* switch counter storage */
    unsigned int cnt;                           /* counter */
    uint64_t mask;                              /* switch counter mask */

    /* assign initial counter storage */
    pCounter = pStats->pCounter;

    /* iterate through statistics mask */
    for (cnt = 0; (cnt < GOAL_NET_IP_STATS_END) && (cnt < ARRAY_ELEMENTS(ipStatsRegs)); cnt++) {

        /* calculate statistics entry mask */
        mask = (1 << cnt);

        /* check if statistics entry is requested */
        if (!(pStats->maskStats & mask)) {
            continue;
        }

        if (cnt < GOAL_NET_IP_STATS_IFINOCTETS) {
            /* common counters */
            if (NULL == ipStatsRegs[cnt]) {
                return GOAL_ERR_UNSUPPORTED;
            }

            pCounter->val = *(ipStatsRegs[cnt]);
            pCounter++;
        }
        else {
            /* interface specific counters */
            switch (cnt) {
                case GOAL_NET_IP_STATS_IFINOCTETS:
                    pCounter->val = pNetIf->mib2_counters.ifinoctets;
                    break;

                case GOAL_NET_IP_STATS_IFINUCASTPKTS:
                    pCounter->val = pNetIf->mib2_counters.ifinucastpkts;
                    break;

                case GOAL_NET_IP_STATS_IFINNUCASTPKTS:
                    pCounter->val = pNetIf->mib2_counters.ifinnucastpkts;
                    break;

                case GOAL_NET_IP_STATS_IFINDISCARDS:
                    pCounter->val = pNetIf->mib2_counters.ifindiscards;
                    break;

                case GOAL_NET_IP_STATS_IFINERRORS:
                    pCounter->val = pNetIf->mib2_counters.ifinerrors;
                    break;

                case GOAL_NET_IP_STATS_IFINUNKOWNPROTOS:
                    pCounter->val = pNetIf->mib2_counters.ifinunknownprotos;
                    break;

                case GOAL_NET_IP_STATS_IFOUTOCTETS:
                    pCounter->val = pNetIf->mib2_counters.ifoutoctets;
                    break;

                case GOAL_NET_IP_STATS_IFOUTUCASTPKTS:
                    pCounter->val = pNetIf->mib2_counters.ifoutucastpkts;
                    break;

                case GOAL_NET_IP_STATS_IFOUTNUCASTPKTS:
                    pCounter->val = pNetIf->mib2_counters.ifoutnucastpkts;
                    break;

                case GOAL_NET_IP_STATS_IFOUTDISCARDS:
                    pCounter->val = pNetIf->mib2_counters.ifoutdiscards;
                    break;

                case GOAL_NET_IP_STATS_IFOUTERRORS:
                    pCounter->val = pNetIf->mib2_counters.ifouterrors;
                    break;

                default:
                    break;
            }
        }
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Retrieves mask with supported stats
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_targetIpStatsMaskGet(
    GOAL_NET_IP_STATS_MASK_T *pMaskPort         /**< mask data */
)
{
    *pMaskPort = lwipStatsMask;
    return GOAL_OK;
}


/****************************************************************************/
/** Resets IP stats
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_targetIpStatsRst(
    struct netif *pNetIf                        /**< netif handle */
)
{
    uint32_t cnt;                               /* counter */

    /* iterate through statistics mask */
    for (cnt = 0; (cnt < GOAL_NET_IP_STATS_END) && (cnt < ARRAY_ELEMENTS(ipStatsRegs)); cnt++) {

        /* check whether statType is supported and reset it */
        if (NULL != ipStatsRegs[cnt]) {
            *(ipStatsRegs[cnt]) = 0;
        }
    }

    GOAL_MEMSET(&pNetIf->mib2_counters, 0, sizeof(struct stats_mib2_netif_ctrs));

    return GOAL_OK;
}
#endif /* GOAL_CONFIG_IP_STATS */


/****************************************************************************/
/** Network command handler
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_drvNetLwipCmd(
    GOAL_MA_NET_T *pHdlMaNet,                   /**< NET MA handle */
    GOAL_NET_CMD_T id,                          /**< command ID */
    GOAL_BOOL_T wrFlag,                         /**< write flag */
    void *pArg                                  /**< argument */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_DRV_NET_LWIP_DATA_T *pPrvData;         /* private data */

    UNUSEDARG(wrFlag);
    UNUSEDARG(pArg);

    res = goal_lockGet(pLwIpLock, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    pPrvData = (GOAL_DRV_NET_LWIP_DATA_T *) pHdlMaNet->pDrvData;

    switch (id) {

#if GOAL_CONFIG_DHCP_EXT == 1
        case GOAL_NET_CMD_DHCP_START:
            res = (ERR_OK == dhcp_start(pPrvData->pNetIf)) ? GOAL_OK : GOAL_ERR_NET_DHCP_START;
            break;

        case GOAL_NET_CMD_DHCP_STOP:
            dhcp_stop(pPrvData->pNetIf);
            res = GOAL_OK;
            break;

        case GOAL_NET_CMD_DHCP_RENEW:
            res = (ERR_OK == dhcp_renew(pPrvData->pNetIf)) ? GOAL_OK : GOAL_ERR_NET_DHCP_RENEW;
            break;

        case GOAL_NET_CMD_DHCP_RELEASE:
            res = (ERR_OK == dhcp_release(pPrvData->pNetIf)) ? GOAL_OK : GOAL_ERR_NET_DHCP_RELEASE;
            break;

        case GOAL_NET_CMD_DHCP_INFORM:
            dhcp_inform(pPrvData->pNetIf);
            res = GOAL_OK;
            break;

        case GOAL_NET_CMD_DHCP_STATE:
            res = goal_targetNetDhcpState(pPrvData->pNetIf, (uint32_t *) pArg);
            break;

        case GOAL_NET_CMD_DHCP_ADDR:
            res = goal_targetNetDhcpAddr(pPrvData->pNetIf, (GOAL_NET_DHCP_T *) pArg);
            break;

#else /* GOAL_CONFIG_DHCP_EXT */
        case GOAL_NET_CMD_DHCP_SEND:
            res = goal_targetNetDhcpSend(pPrvData, (GOAL_NET_DHCP_TX_DATA_T *) pArg);
            break;
#endif /* GOAL_CONFIG_DHCP_EXT == 1 */

#if GOAL_CONFIG_IP_STATS == 1
        case GOAL_NET_CMD_IP_STATS_GET:
            res = goal_targetIpStatsGet(pPrvData->pNetIf, (GOAL_NET_IP_STATS_GET_T *) pArg);
            break;

        case GOAL_NET_CMD_IP_STATS_RST:
            res = goal_targetIpStatsRst(pPrvData->pNetIf);
            break;

        case GOAL_NET_CMD_IP_STATS_MASK_GET:
            res = goal_targetIpStatsMaskGet((GOAL_NET_IP_STATS_MASK_T *) pArg);
            break;

#endif
        default:
            res = GOAL_ERR_UNSUPPORTED;
            break;
    }

    /* release lock */
    goal_lockPut(pLwIpLock);

    return res;
}


#if GOAL_CONFIG_DHCP_EXT == 1
/****************************************************************************/
/** Get DHCP State
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_targetNetDhcpState(
    struct netif *pNetIf,                       /**< netif handle */
    uint32_t *pState                            /**< state pointer */
)
{
    if (DHCP_STATE_BOUND == pNetIf->dhcp->state) {
        *pState = GOAL_NET_DHCP_STATE_BOUND;
    }
    else if (DHCP_STATE_OFF == pNetIf->dhcp->state) {
        *pState = GOAL_NET_DHCP_STATE_DISABLED;
    }
    else {
        *pState = GOAL_NET_DHCP_STATE_PROGRESS;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Return DHCP IP address
 *
 * @retval GOAL_OK address configuration valid
 * @retval other invalid or no address configuration available
 */
static GOAL_STATUS_T goal_targetNetDhcpAddr(
    struct netif *pNetIf,                       /**< netif handle */
    GOAL_NET_DHCP_T *pAddrDhcp                  /**< addresses received via DHCP */
)
{
    ip_addr_t dnsServer;                        /* lwip DNS server address */

    if (DHCP_STATE_BOUND != pNetIf->dhcp->state) {
        return GOAL_ERR_NET_DHCP_ADDR;
    }

    pAddrDhcp->addrIp = GOAL_be32toh((uint32_t) pNetIf->ip_addr.addr);
    pAddrDhcp->addrNetmask = GOAL_be32toh((uint32_t) pNetIf->netmask.addr);
    pAddrDhcp->addrGateway = GOAL_be32toh((uint32_t) pNetIf->gw.addr);
    dnsServer = dns_getserver(0);
    pAddrDhcp->addrDns1 = GOAL_be32toh((uint32_t) dnsServer.addr);
    dnsServer = dns_getserver(1);
    pAddrDhcp->addrDns2 = GOAL_be32toh((uint32_t) dnsServer.addr);;

    return GOAL_OK;
}
#else /* GOAL_CONFIG_DHCP_EXT */


/****************************************************************************/
/** Send a DHCP message
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
static GOAL_STATUS_T goal_targetNetDhcpSend(
    GOAL_DRV_NET_LWIP_DATA_T *pPrvData,         /**< private driver data */
    GOAL_NET_DHCP_TX_DATA_T *pTxData            /**< DHCP Tx data */
)
{
    GOAL_NET_CHAN_T *pChan;                     /* GOAL NET channel */
    GOAL_BUFFER_T *pBuf;                        /* GOAL buffer with DHCP frame */
    struct udp_pcb *pUdpPcb;                    /* UDP socket descriptor */
    struct pbuf *pLwipBuffer;                   /* lwIP buffer for data */
    struct pbuf *pLwipSeg = NULL;               /* lwIP buffer segment ptr */
    err_t retValLwip;                           /* lwIP error code */
    unsigned int pos = 0;                       /* GOAL buffer position */
    GOAL_STATUS_T res;                          /* result */
    GOAL_TARGET_NET_HANDLE_T *pTgtHdl;          /* target channel handle */

    if (NULL == pTxData) {
        return GOAL_ERR_NULL_POINTER;
    }

    pChan = pTxData->pChan;
    pBuf = pTxData->pBuf;

    if ((NULL == pChan) || (NULL == pChan->pData) || (NULL == pBuf)) {
        return GOAL_ERR_NULL_POINTER;
    }

    if ((GOAL_NET_UDP_SERVER != pChan->type) && (GOAL_NET_UDP_CLIENT != pChan->type)) {
        return GOAL_ERR_PARAM;
    }

    res = goal_lockGet(pLwIpLock, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    pTgtHdl = (GOAL_TARGET_NET_HANDLE_T *) pChan->pData;

    /* UDP PCB */
    pUdpPcb = (struct udp_pcb *) pTgtHdl->pPcbDesc;

    /* allocate lwIP buffer */
    pLwipBuffer = pbuf_alloc(PBUF_TRANSPORT, pBuf->dataLen, PBUF_POOL);
    if (NULL == pLwipBuffer) {
        /* GG_LOG ID:429: */
        goal_lmLog(GOAL_ID_DRV_NET, 429, 346, 0, 0, GOAL_LOG_SEV_ERROR, "Allocation of lwIp buffer failed");
        res = GOAL_ERROR;
    }

    if (GOAL_RES_OK(res)) {
        /* Copy data from GOAL buffer to lwIP buffer */
        for (pLwipSeg = pLwipBuffer; NULL != pLwipSeg; pLwipSeg = pLwipSeg->next) {
            GOAL_MEMCPY(pLwipSeg->payload, &pBuf->ptrData[pos], pLwipSeg->len);
            pos += pLwipSeg->len;
        }

        if (pPrvData->currentIp == 0) {
            /* no IP set, allow sending broadcasts for DHCP */
            retValLwip = udp_sendto_if_src(pUdpPcb, pLwipBuffer, IP_ADDR_BROADCAST,
                                           pChan->addr.remotePort, pPrvData->pNetIf, IP_ADDR_ANY);
        }
        else {
            /* Set remote end in local PCB copy */
            pUdpPcb->remote_ip.addr = GOAL_htobe32(pChan->addr.remoteIp);
            pUdpPcb->remote_port = pChan->addr.remotePort;

            /* Send data via UDP */
            retValLwip = udp_send(pUdpPcb, pLwipBuffer);
        }

        /* release UDP buffer */
        pbuf_free(pLwipBuffer);

        if (ERR_OK != retValLwip) {
            goal_logInfo("LWIP send error: retValLwip %u", retValLwip);
            goal_logInfo("LWIP send error: ERR_MEM %u", ERR_MEM);
            res = GOAL_ERR_NET_SEND;
        }
    }

    /* release lock */
    goal_lockPut(pLwIpLock);

    return res;
}
#endif /* GOAL_CONFIG_DHCP_EXT == 1 */
