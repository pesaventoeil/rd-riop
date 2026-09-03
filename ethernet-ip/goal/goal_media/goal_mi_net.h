/** @file
 *
 * @brief GOAL Network Interface Handling
 *
 * This module implements the interface for managing the TCP/IP stack.
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

#ifndef GOAL_MI_NET_H
#define GOAL_MI_NET_H

#include "goal_includes.h"


/****************************************************************************/
/* Configuration */
/****************************************************************************/
#ifndef GOAL_CONFIG_IP_STATS
#  define GOAL_CONFIG_IP_STATS 0
#endif

#ifndef GOAL_CONFIG_NET_ADDR_DEFAULT_USE
#  define GOAL_CONFIG_NET_ADDR_DEFAULT_USE 0
#endif

#ifndef GOAL_CONFIG_NET_ADDR_IP_DEFAULT
#  define GOAL_CONFIG_NET_ADDR_IP_DEFAULT      GOAL_NET_IPV4(192, 168, 0, 100)
#endif

#ifndef GOAL_CONFIG_NET_ADDR_MASK_DEFAULT
#  define GOAL_CONFIG_NET_ADDR_MASK_DEFAULT    GOAL_NET_IPV4(255, 255, 255, 0)
#endif

#ifndef GOAL_CONFIG_NET_ADDR_GATEWAY_DEFAULT
#  define GOAL_CONFIG_NET_ADDR_GATEWAY_DEFAULT GOAL_NET_IPV4(0, 0, 0, 0)
#endif


/****************************************************************************/
/* Public Defines */
/****************************************************************************/
#define GOAL_NET_DNAME_LEN    49                /**< Domain name length for DHCP data */
#define GOAL_NET_HNAME_LEN    65                /**< host name length for DHCP data */

#define GOAL_MA_ID_NET          6               /**< Net MA ID */

/* DHCP summary states */
#define GOAL_NET_DHCP_STATE_UNKNOWN     0       /**< DHCP state is unknown */
#define GOAL_NET_DHCP_STATE_DISABLED    1       /**< DHCP is disabled */
#define GOAL_NET_DHCP_STATE_PROGRESS    2       /**< DHCP is in progress */
#define GOAL_NET_DHCP_STATE_BOUND       3       /**< DHCP got IP configuration */

#define GOAL_NET_DHCP_CLIENTID_LEN 65           /**< client id length */


/****************************************************************************/
/* Forward Declaration */
/****************************************************************************/
struct GOAL_MA_NET_T;


/****************************************************************************/
/* Public Data types */
/****************************************************************************/
/**< network commands */
typedef enum {
    GOAL_NET_CMD_NONE = 0,                      /**< no command */
    GOAL_NET_CMD_DHCP_START,                    /**< start DHCP client */
    GOAL_NET_CMD_DHCP_STOP,                     /**< stop DHCP client */
    GOAL_NET_CMD_DHCP_RENEW,                    /**< renew DHCP lease */
    GOAL_NET_CMD_DHCP_RELEASE,                  /**< release DHCP lease */
    GOAL_NET_CMD_DHCP_INFORM,                   /**< inform DHCP about manual IP address */
    GOAL_NET_CMD_DHCP_ADDR,                     /**< retrieve DHCP address */
    GOAL_NET_CMD_DHCP_STATE,                    /**< get DHCP state */
    GOAL_NET_CMD_DHCP_SEND,                     /**< send DHCP frame */
    GOAL_NET_CMD_IP_STATS_GET,                  /**< get IP statistics */
    GOAL_NET_CMD_IP_STATS_RST,                  /**< reset IP statistics */
    GOAL_NET_CMD_IP_STATS_MASK_GET,             /**< get supported IP statistics */

    GOAL_NET_CMD_END,                           /**< command end marker */
} GOAL_NET_CMD_T;


#if (GOAL_CONFIG_IP_STATS == 1)
/**< statistics mask */
typedef uint64_t GOAL_NET_IP_STATS_MASK_T;


/**< IP statistics counter definition (identifier numbers are hardwired to drivers and must never be changed) */
#define GOAL_NET_IP_STATS_RV_LIST                                                                                                      \
                     /* nr,  identifier,                                   description */                                                  \
    GOAL_NET_IP_STATS_RV(0,  GOAL_NET_IP_STATS_IPINHDRERRORS,              "discarded datagrams due to hdr errors"),                       \
    GOAL_NET_IP_STATS_RV(1,  GOAL_NET_IP_STATS_IPINADDRERRORS,             "discarded due to invalid destination address"),                \
    GOAL_NET_IP_STATS_RV(2,  GOAL_NET_IP_STATS_IPINUNKOWNPROTOS,           "discarded due to unknown proto id"),                           \
    GOAL_NET_IP_STATS_RV(3,  GOAL_NET_IP_STATS_IPINDISCARDS,               "discarded datagrams without errors in packet"),                \
    GOAL_NET_IP_STATS_RV(4,  GOAL_NET_IP_STATS_IPINDELIVERS,               "numberd of packets delivered to application"),                 \
    GOAL_NET_IP_STATS_RV(5,  GOAL_NET_IP_STATS_IPOUTREQUESTS,              "number of packets requested to transmit"),                     \
    GOAL_NET_IP_STATS_RV(6,  GOAL_NET_IP_STATS_IPOUTDISCARDS,              "number of packets to send that were discard"),                 \
    GOAL_NET_IP_STATS_RV(7,  GOAL_NET_IP_STATS_IPOUTNOROUTES,              "number of packets discarded due to missing route"),            \
    GOAL_NET_IP_STATS_RV(8,  GOAL_NET_IP_STATS_IPREASMOKS,                 "max. number of seconds for reassembly"),                       \
    GOAL_NET_IP_STATS_RV(9,  GOAL_NET_IP_STATS_IPREASMFAILS,               "number of successfully reassembled packets"),                  \
    GOAL_NET_IP_STATS_RV(10, GOAL_NET_IP_STATS_IPFRAGOKS,                  "number fragmented packets"),                                   \
    GOAL_NET_IP_STATS_RV(11, GOAL_NET_IP_STATS_IPFRAGFAILS,                "number of failed framgentations"),                             \
    GOAL_NET_IP_STATS_RV(12, GOAL_NET_IP_STATS_IPFRAGCREATES,              "number of framgents created"),                                 \
    GOAL_NET_IP_STATS_RV(13, GOAL_NET_IP_STATS_IPREASMREQGDS,              "number of received IP fragnents"),                             \
    GOAL_NET_IP_STATS_RV(14, GOAL_NET_IP_STATS_IPFORWDATAGRAMS,            "number of input datagrams forwarded"),                         \
    GOAL_NET_IP_STATS_RV(15, GOAL_NET_IP_STATS_IPINRECEIVES,               "total number of received datagrams"),                          \
    GOAL_NET_IP_STATS_RV(16, GOAL_NET_IP_STATS_TCPACTIVEOPENS,             "direct transitions from SYN-SENT to CLOSED state"),            \
    GOAL_NET_IP_STATS_RV(17, GOAL_NET_IP_STATS_TCPPASSIVEOPENS,            "direct transitions from SYN-RCVD to LISTEN state"),            \
    GOAL_NET_IP_STATS_RV(18, GOAL_NET_IP_STATS_TCPATTEMPTFAILS,            "direct transitions to CLOSED from SYN-SNET or SYN-RCVD"),      \
    GOAL_NET_IP_STATS_RV(19, GOAL_NET_IP_STATS_TCPESTABRESETS,             "direct transitions to CLOSED from ESTABLISHED or CLOSE-WAIT"), \
    GOAL_NET_IP_STATS_RV(20, GOAL_NET_IP_STATS_TCPOUTSEGS,                 "total number of segments sent"),                               \
    GOAL_NET_IP_STATS_RV(21, GOAL_NET_IP_STATS_TCPRETRANSSEGS,             "total number of segments retransmitted"),                      \
    GOAL_NET_IP_STATS_RV(22, GOAL_NET_IP_STATS_TCPINSEGS,                  "total number of segments received"),                           \
    GOAL_NET_IP_STATS_RV(23, GOAL_NET_IP_STATS_TCPINERRS,                  "total number of segments received in error"),                  \
    GOAL_NET_IP_STATS_RV(24, GOAL_NET_IP_STATS_TCPOUTRSTS,                 "total number of segments sent with RST flag"),                 \
    GOAL_NET_IP_STATS_RV(25, GOAL_NET_IP_STATS_UDPINDATAGRAMS,             "total number of UDP datagrams deliverd to appl."),             \
    GOAL_NET_IP_STATS_RV(26, GOAL_NET_IP_STATS_UDPNOPORTS,                 "total number of UDP datagrams with unknown dst port"),         \
    GOAL_NET_IP_STATS_RV(27, GOAL_NET_IP_STATS_UDPINERRORS,                "total number of UDP datagrams received in error"),             \
    GOAL_NET_IP_STATS_RV(28, GOAL_NET_IP_STATS_UDPOUTDATAGRAMS,            "total number of UDP datagrams sent"),                          \
    GOAL_NET_IP_STATS_RV(29, GOAL_NET_IP_STATS_ICMPINMSGS,                 "total number of ICMP msgs received"),                          \
    GOAL_NET_IP_STATS_RV(30, GOAL_NET_IP_STATS_ICMPINERRORS,               "total number of ICMP msgs received in error"),                 \
    GOAL_NET_IP_STATS_RV(31, GOAL_NET_IP_STATS_ICMPINDESTUNREACHS,         "number of ICMP Destination Unreachable received"),             \
    GOAL_NET_IP_STATS_RV(32, GOAL_NET_IP_STATS_ICMPINTIMEEXCDS,            "number of ICMP Time Exceeded received"),                       \
    GOAL_NET_IP_STATS_RV(33, GOAL_NET_IP_STATS_ICMPINPARMPROBS,            "number of ICMP Parameter Problems received"),                  \
    GOAL_NET_IP_STATS_RV(34, GOAL_NET_IP_STATS_ICMPINSRCQUENCHS,           "number of ICMP Source Quench received"),                       \
    GOAL_NET_IP_STATS_RV(35, GOAL_NET_IP_STATS_ICMPINREDIRECTS,            "number of ICMP Redirect received"),                            \
    GOAL_NET_IP_STATS_RV(36, GOAL_NET_IP_STATS_ICMPINECHOS,                "number of ICMP Echo requests received"),                       \
    GOAL_NET_IP_STATS_RV(37, GOAL_NET_IP_STATS_ICMPINECHOREPS,             "number of ICMP Echo replies received"),                        \
    GOAL_NET_IP_STATS_RV(38, GOAL_NET_IP_STATS_ICMPINTIMESTAMPS,           "number of ICMP Timestamp requests received"),                  \
    GOAL_NET_IP_STATS_RV(39, GOAL_NET_IP_STATS_ICMPINTIMESTAMPREPS,        "number of ICMP Timestamp replies received"),                   \
    GOAL_NET_IP_STATS_RV(40, GOAL_NET_IP_STATS_ICMPINADDRMASKS,            "number of ICMP Address Mask requests received"),               \
    GOAL_NET_IP_STATS_RV(41, GOAL_NET_IP_STATS_ICMPINADDRMASKREPS,         "number of ICMP Address Mask replies received"),                \
    GOAL_NET_IP_STATS_RV(42, GOAL_NET_IP_STATS_ICMPOUTMSGS,                "total number of ICMP messages sent"),                          \
    GOAL_NET_IP_STATS_RV(43, GOAL_NET_IP_STATS_ICMPOUTERRORS,              "number of ICMP msgs discarded"),                               \
    GOAL_NET_IP_STATS_RV(44, GOAL_NET_IP_STATS_ICMPOUTDESTUNREACHS,        "number of ICMP Destination Unreachable sent "),                \
    GOAL_NET_IP_STATS_RV(45, GOAL_NET_IP_STATS_ICMPOUTTIMEEXCDS,           "number of ICMP Time Exceeded sent"),                           \
    GOAL_NET_IP_STATS_RV(46, GOAL_NET_IP_STATS_ICMPOUTECHOS,               "number of ICMP Echo requests sent"),                           \
    GOAL_NET_IP_STATS_RV(47, GOAL_NET_IP_STATS_ICMPOUTECHOREPS,            "number of ICMP Echo replies sent"),                            \
    GOAL_NET_IP_STATS_RV(48, GOAL_NET_IP_STATS_IFINOCTETS,                 "number of octets received"),                                   \
    GOAL_NET_IP_STATS_RV(49, GOAL_NET_IP_STATS_IFINUCASTPKTS,              "number of unicast packets received"),                          \
    GOAL_NET_IP_STATS_RV(50, GOAL_NET_IP_STATS_IFINNUCASTPKTS,             "number of non unicast packets received"),                      \
    GOAL_NET_IP_STATS_RV(51, GOAL_NET_IP_STATS_IFINDISCARDS,               "number of received packets disc. without errors"),             \
    GOAL_NET_IP_STATS_RV(52, GOAL_NET_IP_STATS_IFINERRORS,                 "number of received packets with errors"),                      \
    GOAL_NET_IP_STATS_RV(53, GOAL_NET_IP_STATS_IFINUNKOWNPROTOS,           "number of received packets with unknown proto"),               \
    GOAL_NET_IP_STATS_RV(54, GOAL_NET_IP_STATS_IFOUTOCTETS,                "number of octets received"),                                   \
    GOAL_NET_IP_STATS_RV(55, GOAL_NET_IP_STATS_IFOUTUCASTPKTS,             "number of unicast packets sent"),                              \
    GOAL_NET_IP_STATS_RV(56, GOAL_NET_IP_STATS_IFOUTNUCASTPKTS,            "number of non unicast packets sent"),                          \
    GOAL_NET_IP_STATS_RV(57, GOAL_NET_IP_STATS_IFOUTDISCARDS,              "number of packets to sent discarded without error"),           \
    GOAL_NET_IP_STATS_RV(58, GOAL_NET_IP_STATS_IFOUTERRORS,                "number of packets to be sent with errors")


/**< Ethernet statistics ids */
#undef GOAL_NET_IP_STATS_RV
#define GOAL_NET_IP_STATS_RV(nr, id, desc) id = nr
typedef enum {
    GOAL_NET_IP_STATS_RV_LIST,
    GOAL_NET_IP_STATS_END
} GOAL_NET_IP_STATS_ID_T;

/**< IP statistics request */
typedef struct {
    uint32_t val;                               /**< counter value */
} GOAL_NET_IP_STATS_COUNTER_T;

/**< IP statistics request */
typedef struct {
    GOAL_NET_IP_STATS_MASK_T maskStats;         /**< statistics mask */
    GOAL_NET_IP_STATS_COUNTER_T *pCounter;      /**< pointer to counter buffer */
} GOAL_NET_IP_STATS_GET_T;
#endif /* GOAL_CONFIG_IP_STATS */

/** network DHCP data */
typedef struct {
    uint32_t addrIp;                            /**< IP address */
    uint32_t addrNetmask;                       /**< subnet mask */
    uint32_t addrGateway;                       /**< gateway address */
    uint32_t addrDns1;                          /**< DNS server address 1 */
    uint32_t addrDns2;                          /**< DNS server address 2 */
    char domainName[GOAL_NET_DNAME_LEN];        /**< domain name */
    char hostName[GOAL_NET_HNAME_LEN];          /**< host name */
} GOAL_NET_DHCP_T;

/** DHCP Tx data */
typedef struct {
    GOAL_NET_CHAN_T *pChan;                     /**< DHCP NET Channel */
    GOAL_BUFFER_T *pBuf;                        /**< GOAL buffer with DHCP frame */
} GOAL_NET_DHCP_TX_DATA_T;

#if GOAL_CONFIG_DHCP == 1
/** DHCP state data */
typedef struct {
    uint8_t state;                              /**< state of DHCP client */
    GOAL_BOOL_T flgInit;                        /**< initialization flag */
    GOAL_BOOL_T flgActive;                      /**< initialization flag */
    GOAL_NET_CHAN_T *pChan;                     /**< DHCP socket */
    GOAL_TIMESTAMP_T timeout;                   /**< DHCP timeout timer */
    uint32_t xid;                               /**< transaction ID */
    uint32_t serverAddr;                        /**< DHCP server address */
    GOAL_ETH_MAC_ADDR_T macAddr;                /**< local MAC address */
    GOAL_NET_DHCP_T data;                       /**< DHCP data */
    GOAL_TIMESTAMP_T leaseTime;                 /**< lease time */
    GOAL_TIMESTAMP_T renewTime;                 /**< renewal time */
    GOAL_TIMESTAMP_T rebindTime;                /**< rebinding time */
    char clientId[GOAL_NET_DHCP_CLIENTID_LEN];  /**< client id */
    uint8_t prevDhcpState;                      /**< previous DHCP state */
    uint8_t retransCount;                       /**< retransmit counter */
} GOAL_NET_DHCP_DATA_T;
#endif /* GOAL_CONFIG_DHCP */

/** IP Set callback */
typedef GOAL_STATUS_T (* GOAL_NET_CB_IP_SET_T)(
    void *pMiNet,                               /**< network mi */
    uint32_t addrIp,                            /**< IP address */
    uint32_t addrMask,                          /**< subnet mask */
    uint32_t addrGw,                            /**< gateway */
    GOAL_BOOL_T flgTemp,                        /**< temporary config flag */
    void *pArg                                  /**< callback argument */
);

/** IP Verification callback */
typedef GOAL_STATUS_T (* GOAL_NET_CB_IP_SET_VERIFY_T)(
    uint32_t addrIp,                            /**< IP address */
    uint32_t addrMask,                          /**< subnet mask */
    uint32_t addrGw                             /**< gateway */
);

/** Network Up callback */
typedef GOAL_STATUS_T (* GOAL_NET_CB_NET_UP_T)(
    void *pMiNet,                               /**< network mi */
    uint32_t id                                 /**< id of network */
);

/** list entry of Network Up callback handlers */
typedef struct GOAL_NET_CB_LIST_NET_UP_T {
    struct GOAL_NET_CB_LIST_NET_UP_T *pNext;    /**< next callback entry */
    GOAL_NET_CB_NET_UP_T func;                  /**< registered callback function */
} GOAL_NET_CB_LIST_NET_UP_T;


/** list entry of IP Set callback handlers */
typedef struct GOAL_NET_CB_LIST_IP_SET_T {
    struct GOAL_NET_CB_LIST_IP_SET_T *pNext;    /**< next callback entry */
    GOAL_NET_CB_IP_SET_T func;                  /**< registered callback function */
    void *pArg;                                 /**< callback argument */
} GOAL_NET_CB_LIST_IP_SET_T;

/** list entry of IP verification callback handlers */
typedef struct GOAL_NET_CB_LIST_IP_VERIFY_T {
    struct GOAL_NET_CB_LIST_IP_VERIFY_T *pNext; /**< next callback entry */
    GOAL_NET_CB_IP_SET_VERIFY_T func;           /**< registered callback function */
} GOAL_NET_CB_LIST_IP_VERIFY_T;

/** NET MI handle */
typedef struct GOAL_MI_NET_T {
    GOAL_INSTANCE_HEADER(GOAL_MI_NET_T);        /**< instance header */
    struct GOAL_MA_NET_T *pMaNet;               /**< MA handle GOAL_MA_NET_T */
    GOAL_BOOL_T flgOpen;                        /**< Mi instance open */
    GOAL_NET_CB_LIST_NET_UP_T *pListCbNetUp;    /**< list of Network Up callback handlers */
    GOAL_NET_CB_LIST_IP_SET_T *pListCbIpSet;    /**< list of IP Set callback handlers */
    GOAL_NET_CB_LIST_IP_VERIFY_T *pListCbIpVerify; /**< list of IP verification callback handlers */
    uint32_t cmVarInstId;                       /**< instance ID for CM variables */
#if GOAL_CONFIG_DHCP == 1
    GOAL_NET_DHCP_DATA_T dhcpData;              /**< DHCP state data */
#endif /* GOAL_CONFIG_DHCP */
} GOAL_MI_NET_T;


/****************************************************************************/
/* Inline functions */
/****************************************************************************/


/****************************************************************************/
/** Get a Net Mi handle for a given ID
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static inline GOAL_STATUS_T goal_miNetHdlGet(
    uint32_t id,                                /**< NET MI id */
    GOAL_MI_NET_T **ppMiNetHdl                  /**< [out] MI handle */
)
{
    return goal_instGetById((GOAL_INSTANCE_T **) ppMiNetHdl, GOAL_ID_NET, id);
}


/****************************************************************************/
/* Public Functions */
/****************************************************************************/
GOAL_STATUS_T goal_netInitPre(
    void
);

GOAL_STATUS_T goal_miNetMaReg(
    uint32_t idMa,                              /**< ID MI */
    GOAL_MI_NET_T *pMiNetHdl                    /**< [in] MI handle */
);

GOAL_STATUS_T goal_miNetOpen(
    uint32_t idMi,                              /**< MI id */
    uint32_t idMa                               /**< MA id */
);

GOAL_STATUS_T goal_miNetCmd(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI handle */
    GOAL_NET_CMD_T cmd,                         /**< command */
    GOAL_BOOL_T wrFlag,                         /**< write flag */
    void *pArg                                  /**< argument */
);

GOAL_STATUS_T goal_miNetIpSet(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI handle */
    uint32_t addrIp,                            /**< IP address */
    uint32_t addrMask,                          /**< subnet mask */
    uint32_t addrGw,                            /**< gateway */
    GOAL_BOOL_T flgTemp                         /**< temporary IP config flag */
);

GOAL_STATUS_T goal_miNetIpGet(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI handle */
    uint32_t *pAddrIp,                          /**< IP address */
    uint32_t *pAddrMask,                        /**< subnet mask */
    uint32_t *pAddrGw,                          /**< gateway */
    GOAL_BOOL_T *pFlgTemp                       /**< temporary IP config flag */
);

GOAL_STATUS_T goal_miNetIpValidCheck(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI handle */
    uint32_t addrIp,                            /**< IP address */
    uint32_t addrMask,                          /**< subnet mask */
    uint32_t addrGw,                            /**< gateway */
    GOAL_BOOL_T flgTemp                         /**< temporary IP config flag */
);

#if GOAL_CONFIG_CLI == 1
GOAL_STATUS_T goal_netCliInit(
    void
);
#endif /* GOAL_CONFIG_CLI */

GOAL_STATUS_T goal_miNetCmIpSet(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI handle */
    uint32_t addrIp,                            /**< IP address */
    uint32_t addrMask,                          /**< subnet mask */
    uint32_t addrGw,                            /**< gateway */
    GOAL_BOOL_T flgValid                        /**< valid flag */
);

GOAL_STATUS_T goal_miNetCmIpCb(
    uint32_t modId,                             /**< module ID */
    uint32_t varId,                             /**< variable Id */
    GOAL_CM_VAR_T *pVar                         /**< variable pointer */
);

GOAL_STATUS_T goal_miNetFlgValidGet(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI handle */
    GOAL_BOOL_T *pFlgValid                      /**< valid flag ptr */
);

GOAL_STATUS_T goal_miNetFlgValidSet(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI handle */
    GOAL_BOOL_T flgValid                        /**< valid flag */
);

GOAL_STATUS_T goal_miNetCmIpCommit(
    GOAL_MI_NET_T *pMiNetHdl                    /**< MI handle */
);

GOAL_STATUS_T goal_miNetCmDhcpSet(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI handle */
    GOAL_BOOL_T enableDhcp                      /**< flag to enable DHCP */
);

GOAL_STATUS_T goal_miNetCmDhcpGet(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI handle */
    GOAL_BOOL_T *pEnableDhcp                    /**< DHCP enable status */
);

GOAL_STATUS_T goal_miNetCmDnsSet(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI handle */
    uint32_t dns1,                              /**< nameserver 1 */
    uint32_t dns2                               /**< nameserver 2 */
);

GOAL_STATUS_T goal_miNetCmDnsGet(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI handle */
    uint32_t *pDns1,                            /**< nameserver 1 */
    uint32_t *pDns2                             /**< nameserver 2 */
);

GOAL_STATUS_T goal_miNetCbNetworkUpReg(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI handle */
    GOAL_NET_CB_NET_UP_T func                   /**< callback function */
);

GOAL_STATUS_T goal_miNetCbIpSetReg(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI handle */
    GOAL_NET_CB_IP_SET_T func,                  /**< callback function */
    void *pArg                                  /**< callback argument */
);

GOAL_STATUS_T goal_miNetCbIpVerifyReg(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI handle */
    GOAL_NET_CB_IP_SET_VERIFY_T func            /**< callback function */
);


/****************************************************************************/
/* Compatibility Layer */
/****************************************************************************/
static inline GOAL_STATUS_T goal_netCmd(
    GOAL_NET_CMD_T cmd,                         /**< command */
    GOAL_BOOL_T wrFlag,                         /**< write flag */
    void *pArg                                  /**< argument */
)
{
    return goal_miNetCmd(NULL, cmd, wrFlag, pArg);
}

static inline GOAL_STATUS_T goal_netIpSet(
    uint32_t addrIp,                            /**< IP address */
    uint32_t addrMask,                          /**< subnet mask */
    uint32_t addrGw,                            /**< gateway */
    GOAL_BOOL_T flgTemp                         /**< temporary IP config flag */
)
{
    return goal_miNetIpSet(NULL, addrIp, addrMask, addrGw, flgTemp);
}

static inline GOAL_STATUS_T goal_netIpGet(
    uint32_t *pAddrIp,                          /**< IP address */
    uint32_t *pAddrMask,                        /**< subnet mask */
    uint32_t *pAddrGw,                          /**< gateway */
    GOAL_BOOL_T *pFlgTemp                       /**< temporary IP config flag */
)
{
    return goal_miNetIpGet(NULL, pAddrIp, pAddrMask, pAddrGw, pFlgTemp);
}

static inline GOAL_STATUS_T goal_netIpValidCheck(
    uint32_t addrIp,                            /**< IP address */
    uint32_t addrMask,                          /**< subnet mask */
    uint32_t addrGw,                            /**< gateway */
    GOAL_BOOL_T flgTemp                         /**< temporary IP config flag */
)
{
    return goal_miNetIpValidCheck(NULL, addrIp, addrMask, addrGw, flgTemp);
}

static inline GOAL_STATUS_T goal_netCmIpSet(
    uint32_t addrIp,                            /**< IP address */
    uint32_t addrMask,                          /**< subnet mask */
    uint32_t addrGw,                            /**< gateway */
    GOAL_BOOL_T flgValid                        /**< valid flag */
)
{
    return goal_miNetCmIpSet(NULL, addrIp, addrMask, addrGw, flgValid);
}

static inline GOAL_STATUS_T goal_netCmIpCb(
    uint32_t modId,                             /**< module ID */
    uint32_t varId,                             /**< variable Id */
    GOAL_CM_VAR_T *pVar                         /**< variable pointer */
)
{
    return goal_miNetCmIpCb(modId, varId, pVar);
}

static inline GOAL_STATUS_T goal_netFlgValidGet(
    GOAL_BOOL_T *pFlgValid                      /**< valid flag ptr */
)
{
    return goal_miNetFlgValidGet(NULL, pFlgValid);
}

static inline GOAL_STATUS_T goal_netFlgValidSet(
    GOAL_BOOL_T flgValid                        /**< valid flag */
)
{
    return goal_miNetFlgValidSet(NULL, flgValid);
}

static inline GOAL_STATUS_T goal_netCmIpCommit(
    void
)
{
    return goal_miNetCmIpCommit(NULL);
}

static inline GOAL_STATUS_T goal_netCmDhcpSet(
    GOAL_BOOL_T enableDhcp                      /**< flag to enable DHCP */
)
{
    return goal_miNetCmDhcpSet(NULL, enableDhcp);
}

static inline GOAL_STATUS_T goal_netCmDhcpGet(
    GOAL_BOOL_T *pEnableDhcp                    /**< DHCP enable status */
)
{
    return goal_miNetCmDhcpGet(NULL, pEnableDhcp);
}

static inline GOAL_STATUS_T goal_netCmDnsSet(
    uint32_t dns1,                              /**< nameserver 1 */
    uint32_t dns2                               /**< nameserver 2 */
)
{
    return goal_miNetCmDnsSet(NULL, dns1, dns2);
}

static inline GOAL_STATUS_T goal_netCmDnsGet(
    uint32_t *pDns1,                            /**< nameserver 1 */
    uint32_t *pDns2                             /**< nameserver 2 */
)
{
    return goal_miNetCmDnsGet(NULL, pDns1, pDns2);
}

static inline GOAL_STATUS_T goal_netCbRegIpSet(
    GOAL_NET_CB_IP_SET_T func                   /**< callback function */
)
{
    return goal_miNetCbIpSetReg(NULL, func, NULL);
}

static inline GOAL_STATUS_T goal_netCbRegIpArgSet(
    GOAL_NET_CB_IP_SET_T func,                  /**< callback function */
    void *pArg                                  /**< callback argument */
)
{
    return goal_miNetCbIpSetReg(NULL, func, pArg);
}

static inline GOAL_STATUS_T goal_netCbRegIpSetVerify(
    GOAL_NET_CB_IP_SET_VERIFY_T func            /**< callback function */
)
{
    return goal_miNetCbIpVerifyReg(NULL, func);
}

static inline GOAL_STATUS_T goal_miNetCbRegNetworkUp(
    GOAL_NET_CB_NET_UP_T func                   /**< callback function */
)
{
    return goal_miNetCbNetworkUpReg(NULL, func);
}
#endif /* GOAL_MI_NET_H */
