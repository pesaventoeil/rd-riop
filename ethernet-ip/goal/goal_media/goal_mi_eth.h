/** @file
 *
 * @brief Generic Media Adapter for Ethernet Frame Handling
 *
 * @details
 * This module implements generic handling functions for Ethernet frames. Its
 * functions manage Ethernet Instances with assigned Media Adapter and
 * registered drivers for e. g. Ethernet or EoE. The Instances contains
 * Ethernet Queues that can be used to prioritize specific Frame types.
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

#ifndef GOAL_ETH_H
#define GOAL_ETH_H

#include "goal_includes.h"


/****************************************************************************/
/* Media Interface and Adapter IDs */
/****************************************************************************/
#define GOAL_MI_ID              0               /**< default MI ID */

#define GOAL_MA_ID_ETH          0               /**< Ethernet MA ID */
#define GOAL_MA_ID_EOE          1               /**< EoE MA ID */
#define GOAL_MA_ID_DUMMY        2               /**< Dummy driver ID */
#define GOAL_MA_ID_ETH_0        3               /**< Virtual Ethernet [CC] MA ID */
#define GOAL_MA_ID_ETH_1        4               /**< Virtual Ethernet [AC] MA ID */
#define GOAL_MA_ID_QBV          5               /**< Qbv driver ID */


/****************************************************************************/
/* Constants */
/****************************************************************************/
/**< Ethernet port types */
typedef uint32_t GOAL_ETH_PORT_T;
typedef uint32_t GOAL_ETH_PORT_MASK_T;

/**< Ethernet port macros */
#define GOAL_ETH_PORT_HOST_CNT      1
#define GOAL_ETH_PORT_HOST          ((GOAL_ETH_PORT_T) (1 << 30))
#define GOAL_ETH_PORT_MASK          ((GOAL_ETH_PORT_T) ((uint64_t) 1 << 31))
#define GOAL_ETH_PORT(x)            ((GOAL_ETH_PORT_T) (x) & ~GOAL_ETH_PORT_MASK)
#define GOAL_ETH_PORT_BIT(x)        ((GOAL_ETH_PORT_T) (1 << (x)) | GOAL_ETH_PORT_MASK)
#define GOAL_ETH_PORT_BITS          (~(GOAL_ETH_PORT_HOST | GOAL_ETH_PORT_MASK))

/**< default MAC address */
#define GOAL_ETH_MAC_ADDR { 0x02, 0x00, 0x00, 0x00, 0x00, 0x01 }

/**< any MAC address */
#define GOAL_ETH_MAC_ADDR_ANY { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

/* MAC Multicast Bit for First Octet */
#define GOAL_ETH_MAC_MULTICAST      0x01

/**< MAC address macro */
#if (1 == GOAL_CONFIG_GEN_CTC_AC)
#define goal_miEthCfgMacAddr goal_miMctcEthCfgMacAddr
#else
#define goal_miEthCfgMacAddr goal_miEthCfgMacAddrImpl
#endif

/* Ethernet EtherTypes */
#define GOAL_ETH_ETHERTYPE_ANY      0x0000
#define GOAL_ETH_ETHERTYPE_IPV4     0x0800
#define GOAL_ETH_ETHERTYPE_ARP      0x0806
#define GOAL_ETH_ETHERTYPE_VLAN     0x8100
#define GOAL_ETH_ETHERTYPE_MRP      0x88E3

/* Ethernet defines */
#define GOAL_ETH_MTU_SIZE_DFL       1536
#define ETH_MTU_LEN                 goal_miEthMtuGet()
#define MAC_ADDR_LEN                6
#define VLAN_TAG_LEN                4
#define GOAL_LEN_IPV4_ADDR          4
#define GOAL_LEN_ETH_TYPE           2

/**< Default number of output queues */
#define GOAL_ETH_DEFAULT_QUEUES_PER_PORT 1

/**< Offset for Destination MAC Address */
#define GOAL_ETH_DST_MAC_OFFSET (0)

/**< Offset for Source MAC Address */
#define GOAL_ETH_SRC_MAC_OFFSET (6)

/** Offset for EtherType if no VLAN tag is present */
#define GOAL_ETH_ETHTYPE_OFFSET (12)

/**< Offset for data payload when no vlan is present */
#define GOAL_ETH_NOVLAN_DATA_OFFSET (14)

/**< Offset for data payload when vlan is present */
#define GOAL_ETH_VLAN_DATA_OFFSET (GOAL_ETH_NOVLAN_DATA_OFFSET + 4)

/**< Pointer to beginning of frame payload */
#define GOAL_ETH_DATA_PTR(pFrame) (((pFrame)->flags & GOAL_QUEUE_FLG_VLAN) ? (pFrame)->ptrData + (uint8_t) GOAL_ETH_VLAN_DATA_OFFSET : (pFrame)->ptrData + (uint8_t) GOAL_ETH_NOVLAN_DATA_OFFSET)

/* Ethernet command macros */
#define GOAL_ETH_COMMAND(x) x

#define GOAL_ETH_FUNC(cmd, func, flags) { cmd, func, flags }

#define GOAL_ETH_FUNCS_END_FWD(fwd)     GOAL_ETH_FUNC(GOAL_ETH_CMD_END, fwd, 0)
#define GOAL_ETH_FUNCS_END              GOAL_ETH_FUNCS_END_FWD(NULL)

#define GOAL_ETH_FUNC_LIST(x)           {x, GOAL_ETH_FUNCS_END}
#define GOAL_ETH_FUNC_LIST_FWD(x, fwd)  {x, GOAL_ETH_FUNCS_END_FWD(fwd)}

/* Predefine config vars */
#ifndef GOAL_CONFIG_TDMA
#  define GOAL_CONFIG_TDMA 0
#endif

/**< network buffer size */
#if GOAL_CONFIG_ETHERNET == 1
#  define GOAL_NETBUF_SIZE (uint16_t) (ETH_MTU_LEN)
#else
#  define GOAL_NETBUF_SIZE (uint16_t) (GOAL_ETH_MTU_SIZE_DFL)
#endif

/* network buffer calculation (default to 10) */
#ifndef GOAL_CONFIG_BUF_NUM
#  define GOAL_CONFIG_BUF_NUM   10
#endif


/****************************************************************************/
/* Commands (never change order, only add new commands at the end) */
/****************************************************************************/
/**< Ethernet commands */

#define GOAL_ETH_COMMANDS                                                                              \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_NONE),        /**< no command */                                     \
                                                                                                       \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_AUTONEG_PROGRESS), /**< autoneg progress */                          \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_AUTONEG),     /**< autoneg state */                                  \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_AUTONEG_RESTART), /**< restart autonegotiation */                    \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_DUPLEX),      /**< link duplex */                                    \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_HW_FAULT),    /**< hardware fault */                                 \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_SPEED),       /**< link speed */                                     \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_SPEED_MAX),   /**< max possible link speed */                        \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_LINK_STATE),  /**< link state */                                     \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_PORT_STATE),  /**< port state */                                     \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_LINK_CAPABILITIES), /**< link capabilities */                        \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_AUTONEG_ADVERTISEMENT), /**< autonegation advertisement */           \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_PORT_ADMIN_STATE), /**< enabled/disables the port */                 \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_LED_LINK),    /**< Activity/Link LED control */                      \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_PORT_COUNT),  /**< count of available external Ethernet ports */     \
                                                                                                       \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_DLR_ENABLE),  /**< DLR activation */                                 \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_DLR_INIT),    /**< DLR initialization */                             \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_DLR_LAST_BEACON), /**< DLR last beacon */                            \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_DLR_NODE_STATE), /**< DLR node state */                              \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_DLR_RING_STATE), /**< DLR ring state */                              \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_DLR_SHUTDOWN), /**< DLR shutdown */                                  \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_DLR_SV_IP),   /**< DLR supervisor IP */                              \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_DLR_SV_MAC),  /**< DLR supervisor MAC */                             \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_DLR_SV_PREC), /**< DLR supervisor precedence */                      \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_DLR_VLAN),    /**< DLR VLAN */                                       \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_DLR_FORWARD), /**< DLR forwarding for ring support, required of DLR should not be supported, however ring compatibility is required */ \
                                                                                                       \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_MAC_ADDR),    /**< get/set host or port MAC address */               \
                                                                                                       \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_MAC_MULTI_FLUSH), /**< flush multicast MAC table */                  \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_MAC_UNI_FLUSH), /**< flush unicast MAC table */                      \
                                                                                                       \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_PORT_FWD_ADD), /**< add port to forward table */                     \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_PORT_FWD_DEL), /**< delete port from forward table */                \
                                                                                                       \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_MACTAB_CONF), /**< set MAC table conf params */                      \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_MACTAB_SET),  /**< set portmask for MAC */                           \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_MACTAB_GET),  /**< get portmask for MAC */                           \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_MACTAB_CLR),  /**< clear (parts of) the MAC table */                 \
                                                                                                       \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_VLAN_MODE_IN), /**< set/get VLAN input mode */                       \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_VLAN_MODE_OUT), /**< set/get VLAN output mode */                     \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_VLAN_DEF),    /**< set/get default VLAN */                           \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_VLAN_PORT_ADD), /**< set VLAN membership */                          \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_VLAN_PORT_REM), /**< remove VLAN membership */                       \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_VLAN_TABLE_CNT), /**< get max. number of VLAN entries */             \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_VLAN_TABLE_GET), /**< get VLAN table entry */                        \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_VLAN_VERIFY), /**< set/get VLAN verifcation */                       \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_VLAN_DISCUNK), /**< set/get VLAN discartion of unknown frames */     \
                                                                                                       \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_PORT_MIRROR), /**< set/get port mirror config */                     \
                                                                                                       \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_QOS_MODE),    /**< set/get QoS mapping type */                       \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_QOS_PRIO_VLAN), /**< set/get QoS VLAN priority */                    \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_QOS_PRIO_IP), /**< set/get QoS IP priority */                        \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_QOS_PRIO_TYPE), /**< set/get QoS Ethertype priority */               \
                                                                                                       \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_TXRATE),      /**< set/get TX rate for a port */                     \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_STORMTIME),   /**< set/get time frame for rate limit */              \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_BCAST_LIMIT), /**< set/get max. bcast frames allowed */              \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_MCAST_LIMIT), /**< set/get max. bcast frames allowed */              \
                                                                                                       \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_EEE),         /**< set/get EEE config */                             \
                                                                                                       \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_SNOOP_ADD),   /**< add snooping config */                            \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_SNOOP_REM),   /**< remove snooping config */                         \
                                                                                                       \
                                                                                                       \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_LEARNING),    /**< address learning */                               \
                                                                                                       \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_BPDU),        /**< BPDU management control */                        \
                                                                                                       \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_PORT_AUTH),   /**< set/get port authorization */                     \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_PORT_CTRL_DIR), /**< set/get port controlled directions */           \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_PORT_EAPOL_ENABLE), /**< set/get port EAPOL frame reception mode */  \
                                                                                                       \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_TIMESTAMP),   /**< enable timestamping for a port */                 \
                                                                                                       \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_EPL),         /**< initialize Ethernet POWERLINK */                  \
                                                                                                       \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_RAW_INIT),    /**< initialize raw Ethernet frame support */          \
                                                                                                       \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_TDMA_QUEUES_START), /**< allow the queue transmit when the scheduler was enabled until the system timer has reached the first cycle time */ \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_TDMA_CONFIG), /**< configures the TDMA scheduler */                  \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_TDMA_TSLT_CFG), /**< configures a TDMA timeslot */                   \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_TDMA_ENABLE), /**< enables/disables the TDMA scheduler */            \
                                                                                                       \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_QUEUES_PER_PORT), /**< gets the hw output queues per port */         \
                                                                                                       \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_RX_PORT_RES_ADD), /**< add MAC address of frames that must resolve their Rx port */ \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_RX_PORT_RES_CLR), /**< remove MAC address of frames that must resolve their Rx port */ \
                                                                                                       \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_DISC_UNK),    /**< set/get discartion of unknown frames */           \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_FLOOD_UNK),   /**< set/get flood domain of unknown frames */         \
                                                                                                       \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_LATENCY_EGRESS), /**< latency between leaving PHY and timestamping of outgoing frames */ \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_LATENCY_INGRESS), /**< latency between receiving in PHY and timestamping of incoming frames */ \
                                                                                                       \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_PHY_REG_GET), /**< get register value from PHY */                    \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_PHY_REG_SET), /**< set register value in PHY */                      \
                                                                                                       \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_FRAMESIZE_MAX), /**< get/set maximum frame size (of a port) */       \
                                                                                                       \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_MDI_MODE),    /**< get/set MDI mode */                               \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_MDI_STATE),   /**< get/set MDI state */                              \
                                                                                                       \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_TIMEUPDATE),  /**< update Qbv base time */                           \
                                                                                                       \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_SETUP_DONE),  /**< indication for application/stack setup is done */ \
                                                                                                       \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_PHYSICS),     /**< get port physics */                               \
                                                                                                       \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_FP_SUPPORTED), /**< get Frame preemption support status */           \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_FP_ENABLE),    /**< get/set Frame preemption enable status */        \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_FP_VERIFICATION_STATUS), /**< get Frame preemption verification status */ \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_FP_LOCAL_ADD_FRAG_SIZE), /**< Frame preemption: get local additional fragment size */ \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_FP_REMOTE_ADD_FRAG_SIZE), /**< Frame preemption: set remote additional fragment size */ \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_FP_LOCAL_STATUS), /**< get local Frame preemption activation status */ \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_FP_TC_ASSIGN), /**< Frame preemption: get/set preemptable traffic classes */ \
                                                                                                       \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_DLR_RESET), /**< DLR: reset driver's state machine */ \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_DLR_BEACON_TX), /**< DLR Supervisor: start/stop Beacon frame transmission */ \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_DLR_PORT_BLOCK), /**< DLR Supervisor: block/unblock port */ \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_DLR_RAPID_FAULT), /**< DLR Supervisor: clear Rapid fault */ \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_DLR_SUVI_CFG), /**< DLR Supervisor: get/set Supervisor configuration */ \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_DLR_SUVI_CFG_SUPPORTED), /**< DLR Supervisor: is Supervisor configuration supported */ \
                                                                                                       \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_IFACE_NAME),  /**< get interface name */                             \
                                                                                                       \
    GOAL_ETH_COMMAND(GOAL_ETH_CMD_END),         /**< command end marker */

typedef enum {GOAL_ETH_COMMANDS} GOAL_ETH_CMD_T;

/**< VLAN input processing modes */
typedef enum {
    GOAL_ETH_VLANIN_PASSTHROUGH_OVERRIDE,       /**< Single Tagging with Passthrough/VID Overwrite. */
    GOAL_ETH_VLANIN_REPLACE,                    /**< Single Tagging with Replace */
    GOAL_ETH_VLANIN_TAG_ALWAYS,                 /**< Tag always */
    GOAL_ETH_VLANIN_DISABLE = 0xFF              /**< Disable VLAN */
} GOAL_ETH_VLANIN_MODE_T;

/**< VLAN output processing mode */
typedef enum {
    GOAL_ETH_VLANOUT_DISABLE,                   /**< No manipulation */
    GOAL_ETH_VLANOUT_STRIP,                     /**< Strip mode */
    GOAL_ETH_VLANOUT_TAGTHROUGH,                /**< Tag Thru mode */
    GOAL_ETH_VLANOUT_DOMAINTRANSP               /**< Domain / Transparent mode */
} GOAL_ETH_VLANOUT_MODE_T;

/**< port mirroring modes */
typedef enum {
    GOAL_ETH_MIRR_MODE_EGRESS_DA,               /**< Egress DA match */
    GOAL_ETH_MIRR_MODE_EGRESS_SA,               /**< Egress SA match */
    GOAL_ETH_MIRR_MODE_INGRESS_DA,              /**< Ingress DA match */
    GOAL_ETH_MIRR_MODE_INGRESS_SA,              /**< Ingress SA match */
    GOAL_ETH_MIRR_MODE_INGRESS_PORT,            /**< Ingress port match */
    GOAL_ETH_MIRR_MODE_EGRESS_PORT              /**< Ingress port match */
} GOAL_ETH_MIRR_MODE_T;

/**< snooping modes */
typedef enum {
    GOAL_ETH_SNOOP_MODE_IPDATA,                 /**< Match data within IP pkt */
    GOAL_ETH_SNOOP_MODE_IPPROT,                 /**< Match IP protocol */
    GOAL_ETH_SNOOP_MODE_ETYPE,                  /**< Match ether type */
    GOAL_ETH_SNOOP_MODE_DATA                    /**< Match any data */
} GOAL_ETH_SNOOP_MATCHMODE_T;

/**< snooping actions */
typedef enum {
    GOAL_ETH_SNOOP_ACTION_CAPTURE = 1,          /**< Frames are only forwarded to mgt port */
    GOAL_ETH_SNOOP_ACTION_CPY_FW,               /**< Frames are forwarded to mgt port and dest port */
    GOAL_ETH_SNOOP_ACTION_DISCARD               /**< Discard frames */
} GOAL_ETH_SNOOP_ACTION_T;

/* Ethernet port state forward declaration */
struct GOAL_ETH_PORT_STATE_T;

/**< Ethernet port state callback function */
typedef void (* GOAL_ETH_PORT_STATE_CB_FUNC_T)(
    GOAL_ETH_PORT_T id,                         /**< GOAL port id */
    uint32_t maskChg,                           /**< change mask */
    struct GOAL_ETH_PORT_STATE_T *pState        /**< port state structure ptr */
);

typedef void (* GOAL_ETH_PORT_STATE_ARG_CB_FUNC_T)(
    GOAL_ETH_PORT_T id,                         /**< GOAL port id */
    uint32_t maskChg,                           /**< change mask */
    struct GOAL_ETH_PORT_STATE_T *pState,       /**< port state structure ptr */
    void *pArg                                  /**< [in] registered callback argument */
);

typedef union GOAL_ETH_PORT_STATE_CB_UNION_T {
    GOAL_ETH_PORT_STATE_CB_FUNC_T pFuncNoArg;   /**< callback withput additional argument */
    GOAL_ETH_PORT_STATE_ARG_CB_FUNC_T pFuncArg; /**< callback withput additional argument */
} GOAL_ETH_PORT_STATE_CB_UNION_T;

/**< Ethernet port state callback */
typedef struct GOAL_ETH_PORT_STATE_CB_T {
    GOAL_ETH_PORT_STATE_CB_UNION_T cb;          /**< callback function union */
    GOAL_BOOL_T hasArg;                         /**< callback function uses registered callback argument */
    void *pArg;                                 /**< registered callback argument (NULL if unused) */
    struct GOAL_ETH_PORT_STATE_CB_T *pNext;     /**< next callback */
} GOAL_ETH_PORT_STATE_CB_T;

/**< Ethernet port state */
typedef struct GOAL_ETH_PORT_STATE_T {
    uint32_t stateLink;                         /**< link state */
    uint32_t modeDuplex;                        /**< duplex mode */
    uint32_t speed;                             /**< speed */
    uint32_t stateAutoNeg;                      /**< autonegotiation state */
    uint32_t stateAutoNegProgress;              /**< autonegotiation progress state */

    GOAL_ETH_PORT_STATE_CB_T *pCb;              /**< state change callback list */
} GOAL_ETH_PORT_STATE_T;

/**< BPDU management control states */
typedef enum {
    GOAL_ETH_BPDU_CTRL_UNKNOWN,                 /**< unknown */
    GOAL_ETH_BPDU_CTRL_DEFAULT,                 /**< default */
    GOAL_ETH_BPDU_CTRL_DISCARD,                 /**< discarding */
    GOAL_ETH_BPDU_CTRL_MGMT_FWD,                /**< mgmt port forward only */
} GOAL_ETH_BPDU_CTRL_T;


#if GOAL_CONFIG_ETH_STATS_INTERNAL == 1

/**< Internal Statistics Mapping */
#define GOAL_ETH_STAT_LIST_RV \
    GOAL_ETH_STAT_RV(GOAL_STAT_ID_ETH_IFINOCTETS, IfInOctets, "interface incoming octets") \
    GOAL_ETH_STAT_RV(GOAL_STAT_ID_ETH_IFOUTOCTETS, IfOutOctets, "interface outgoing octets") \
    GOAL_ETH_STAT_RV(GOAL_STAT_ID_ETH_IFINDISCARDS, IfInDiscards, "interface incoming discards") \
    GOAL_ETH_STAT_RV(GOAL_STAT_ID_ETH_IFOUTDISCARDS, IfOutDiscards, "interface outgoing discards") \
    GOAL_ETH_STAT_RV(GOAL_STAT_ID_ETH_IFINERRORS, IfInErrors, "interface incoming errors") \
    GOAL_ETH_STAT_RV(GOAL_STAT_ID_ETH_IFOUTERRORS, IfOutErrors, "interface outgoing errors")


/**< Ethernet Statistics */
typedef struct {
    uint32_t id;                                /**< statistics id */
    GOAL_BOOL_T flgUse;                         /**< use-flag */
    GOAL_STAT_T *pStat;                         /**< statistics entry */
    uint64_t *pVal;                             /**< values */
} GOAL_ETH_STAT_T;


#undef GOAL_ETH_STAT_RV
#define GOAL_ETH_STAT_RV(id, name, desc) ENUM_ ## id,


/**< Ethernet id assignment */
typedef enum {
    GOAL_ETH_STAT_LIST_RV

    GOAL_ETH_STAT_ID_END,
} GOAL_ETH_STAT_ENUM_T;


#define GOAL_ETH_STAT_INC(id, idx) goal_miEthStatInternalInc(NULL, ENUM_ ## id, idx, 1)
#define GOAL_ETH_STAT_ADD(id, idx, val) goal_miEthStatInternalInc(NULL, ENUM_ ## id, idx, val)

#else /* GOAL_CONFIG_ETH_STATS_INTERNAL */

#define GOAL_ETH_STAT_INC(...)
#define GOAL_ETH_STAT_ADD(...)
#define goal_miEthStatInternalInc(...)

#endif /* GOAL_CONFIG_ETH_STATS_INTERNAL */


/****************************************************************************/
/* Port state change bits */
/****************************************************************************/
#define GOAL_ETH_PORT_STATE_LINK            (1 << 0) /**< link change */
#define GOAL_ETH_PORT_STATE_DUPLEX          (1 << 1) /**< duplex mode change */
#define GOAL_ETH_PORT_STATE_SPEED           (1 << 2) /**< speed change */
#define GOAL_ETH_PORT_STATE_AUTONEG         (1 << 3) /**< autoneg change */
#define GOAL_ETH_PORT_STATE_AUTONEG_PROGRESS (1 << 4) /**< autoneg progress change */


/****************************************************************************/
/* Link states */
/****************************************************************************/
#define GOAL_ETH_STATE_DEFAULT              0   /**< default state */
#define GOAL_ETH_STATE_UNKNOWN              1   /**< unknown state */
#define GOAL_ETH_STATE_DOWN                 2   /**< state down */
#define GOAL_ETH_STATE_UP                   3   /**< state up */


/****************************************************************************/
/* Duplex modes */
/****************************************************************************/
#define GOAL_ETH_DUPLEX_DEFAULT             0   /**< default duplex setting */
#define GOAL_ETH_DUPLEX_UNKNOWN             1   /**< unknown duplex setting */
#define GOAL_ETH_DUPLEX_HALF                2   /**< half duplex */
#define GOAL_ETH_DUPLEX_FULL                3   /**< full duplex */


/****************************************************************************/
/* Speed */
/****************************************************************************/
#define GOAL_ETH_SPEED_DEFAULT              0   /**< default speed */
#define GOAL_ETH_SPEED_UNKNOWN              1   /**< unknown speed */
#define GOAL_ETH_SPEED_10                   10  /**< 10 Mbit/s */
#define GOAL_ETH_SPEED_100                  100 /**< 100 Mbit/s */
#define GOAL_ETH_SPEED_1000                 1000 /**< 1000 Mbit/s */


/****************************************************************************/
/* Physics */
/****************************************************************************/
#define GOAL_ETH_PHYSICS_DEFAULT            0   /**< default physics */
#define GOAL_ETH_PHYSICS_APL                1   /**< APL 10Base-T1L */


/****************************************************************************/
/* Medium Dependent Interface Mode */
/****************************************************************************/
#define GOAL_ETH_MDI_MODE_DEFAULT           0   /**< default */
#define GOAL_ETH_MDI_MODE_UNKNOWN           1   /**< unknown */
#define GOAL_ETH_MDI_MODE_AUTO              2   /**< auto MDI/MDI-X mode */
#define GOAL_ETH_MDI_MODE_MANUAL            3   /**< manual */


/****************************************************************************/
/* Medium Dependent Interface State */
/****************************************************************************/
#define GOAL_ETH_MDI_STATE_DEFAULT          0   /**< default */
#define GOAL_ETH_MDI_STATE_UNKNOWN          1   /**< unknown */
#define GOAL_ETH_MDI_STATE_UNCROSSED        2   /**< uncrossed */
#define GOAL_ETH_MDI_STATE_CROSSED          3   /**< crossed */


/****************************************************************************/
/* Autonegotiation */
/****************************************************************************/
#define GOAL_ETH_AUTONEG_DEFAULT            0   /**< default autonegotiation */
#define GOAL_ETH_AUTONEG_UNKNOWN            1   /**< unknown autonegotiation */
#define GOAL_ETH_AUTONEG_OFF                2   /**< autonegotiation disabled */
#define GOAL_ETH_AUTONEG_ON                 3   /**< autonegotiation enabled */


/****************************************************************************/
/* Autonegotiation Progress */
/****************************************************************************/
#define GOAL_ETH_AUTONEG_INPROGRESS         0   /**< autonegotiation in progress */
#define GOAL_ETH_AUTONEG_FAIL_ALL           1   /**< autonegotiation completely failed */
#define GOAL_ETH_AUTONEG_FAIL_DUPLEX        2   /**< autonegotiation failed at duplex */
#define GOAL_ETH_AUTONEG_DONE               3   /**< autonegotiation done */
#define GOAL_ETH_AUTONEG_SKIPPED            4   /**< autonegotiation skipped */


/****************************************************************************/
/* Autonegotiation Advertisement */
/****************************************************************************/
#define GOAL_ETH_AUTONEG_AD_NONE    0x00000000  /**< advertise none */
#define GOAL_ETH_AUTONEG_AD_10_HD   (1<<0)      /**< advertise 10 Mbit/s, HD */
#define GOAL_ETH_AUTONEG_AD_10_FD   (1<<1)      /**< advertise 10 Mbit/s, FD */
#define GOAL_ETH_AUTONEG_AD_100_HD  (1<<2)      /**< advertise 100 Mbit/s, HD */
#define GOAL_ETH_AUTONEG_AD_100_FD  (1<<3)      /**< advertise 100 Mbit/s, FD */
#define GOAL_ETH_AUTONEG_AD_1000_HD (1<<4)      /**< advertise 1000 Mbit/s, HD */
#define GOAL_ETH_AUTONEG_AD_1000_FD (1<<5)      /**< advertise 1000 Mbit/s, FD */
#define GOAL_ETH_AUTONEG_AD_ALL     0xffffffff  /**< advertise all */


/****************************************************************************/
/* Link Capabilities */
/****************************************************************************/
#define GOAL_ETH_CAPABL_10HD            (1u<<0) /**< 10 Mbit/s Half Duplex */
#define GOAL_ETH_CAPABL_10FD            (1u<<1) /**< 10 Mbit/s Full Duplex */
#define GOAL_ETH_CAPABL_100HD           (1u<<2) /**< 100 Mbit/s Half Duplex */
#define GOAL_ETH_CAPABL_100FD           (1u<<3) /**< 100 Mbit/s Full Duplex */
#define GOAL_ETH_CAPABL_1000HD          (1u<<4) /**< 1000 Mbit/s Half Duplex */
#define GOAL_ETH_CAPABL_1000FD          (1u<<5) /**< 1000 Mbit/s Full Duplex */


/****************************************************************************/
/* QoS modes */
/****************************************************************************/
#define GOAL_ETH_QOS_MODE_VLAN         (1)
#define GOAL_ETH_QOS_MODE_IP           (1<<1)
#define GOAL_ETH_QOS_MODE_MAC          (1<<2)
#define GOAL_ETH_QOS_MODE_TYPE         (1<<3)


/****************************************************************************/
/* Snooping mode flags */
/****************************************************************************/
#define GOAL_ETH_SNOOP_FLAGS_COMPARE16    (1<<28)
#define GOAL_ETH_SNOOP_FLAGS_OFFSETPLUS2  (1<<29)
#define GOAL_ETH_SNOOP_FLAGS_CMP_OR       (1<<30)


/****************************************************************************/
/* Port authorization states */
/****************************************************************************/
#define GOAL_ETH_AUTH_DEFAULT           0       /**< default state */
#define GOAL_ETH_AUTH_UNKNOWN           1       /**< unknown state */
#define GOAL_ETH_AUTH_UNAUTHORIZED      2       /**< port is unauthorized */
#define GOAL_ETH_AUTH_AUTHORIZED        3       /**< port is authorized */


/****************************************************************************/
/* Port control directions */
/****************************************************************************/
#define GOAL_ETH_CTRL_DIR_DEFAULT       0       /**< default EAPOL reception state */
#define GOAL_ETH_CTRL_DIR_UNKNOWN       1       /**< unknown EAPOL reception state */
#define GOAL_ETH_CTRL_DIR_INPUT         2       /**< EAPOL reception disabled */
#define GOAL_ETH_CTRL_DIR_BOTH          3       /**< EAPOL reception enabled */


/****************************************************************************/
/* Port EAPOL reception states */
/****************************************************************************/
#define GOAL_ETH_EAPOL_DEFAULT          0       /**< default EAPOL reception state */
#define GOAL_ETH_EAPOL_UNKNOWN          1       /**< unknown EAPOL reception state */
#define GOAL_ETH_EAPOL_DISABLE          2       /**< EAPOL reception disabled */
#define GOAL_ETH_EAPOL_ENABLE           3       /**< EAPOL reception enabled */


/****************************************************************************/
/* Ethernet command parameters */
/****************************************************************************/
#define GOAL_ETH_CMD_PORT_HOST          (1 << 0) /**< restrict to host port */
#define GOAL_ETH_CMD_PORT_EXT           (1 << 1) /**< restrict to external ports */


/****************************************************************************/
/* TDMA */
/****************************************************************************/
#define GOAL_ETH_TDMA_EVENT_EOC          (1 << 0) /**< TDMA interrupt flag: cycle start */
#define GOAL_ETH_TDMA_EVENT_T1           (1 << 1) /**< TDMA interrupt flag: T1 offset reached */
#define GOAL_ETH_TDMA_EVENT_T2           (1 << 2) /**< TDMA interrupt flag: T2 offset reached */
#define GOAL_ETH_TDMA_EVENT_T3           (1 << 3) /**< TDMA interrupt flag: T3 offset reached */


/****************************************************************************/
/* Frame Preemption Verification Results */
/****************************************************************************/
#define GOAL_ETH_FP_VERIFICATION_DISABLED     0 /**< Frame Preemption Verification result: not attempted */
#define GOAL_ETH_FP_VERIFICATION_SUCCESS      1 /**< Frame Preemption Verification result: success */
#define GOAL_ETH_FP_VERIFICATION_FAIL         2 /**< Frame Preemption Verification result: failed */
#define GOAL_ETH_FP_VERIFICATION_IN_PROGRESS  3 /**< Frame Preemption Verification result: in progress */
#define GOAL_ETH_FP_VERIFICATION_UNKNOWN      4 /**< Frame Preemption Verification result: unknown */


/****************************************************************************/
/* Forward declarations */
/****************************************************************************/
struct GOAL_ETH_IFACE_T;
struct GOAL_ETH_CMD_DATA_T;


/****************************************************************************/
/* Data types */
/****************************************************************************/
typedef uint8_t GOAL_ETH_MAC_ADDR_T[MAC_ADDR_LEN];


/****************************************************************************/
/* Function types */
/****************************************************************************/
typedef GOAL_STATUS_T (* GOAL_ETH_BUF_FUNC_T)(GOAL_BUFFER_T **, void *);

#if (GOAL_CONFIG_TDMA == 1)
typedef GOAL_STATUS_T (* GOAL_ETH_TDMA_CB_T)(void *pArg, uint32_t eventMask);
#endif

#if GOAL_PTP == 1
/** Egress Timestamp capture callback */
typedef void (* GOAL_ETH_TIMESTAMP_CB_T)(
    void *pArg,                                 /**< callback data */
    uint64_t sec,                               /**< full seconds */
    uint32_t nsec,                              /**< sub-seconds in ns */
    uint32_t port                               /**< port id */
);
#endif


/****************************************************************************/
/* Structures */
/****************************************************************************/
/**< Ethernet buffer handler */
typedef struct GOAL_ETH_BUF_HANDLER_T {
    union {
        uint16_t etherType;                     /**< EtherType */
        GOAL_ETH_MAC_ADDR_T *pAddrMac;          /**< MAC address pointer */
    } type;

    GOAL_ETH_BUF_FUNC_T pFunc;                  /**< buffer handler function */
    void *pArg;                                 /**< callback argument */

    struct GOAL_ETH_BUF_HANDLER_T *pNext;       /**< next handler */
} GOAL_ETH_BUF_HANDLER_T;


/**< command function */
typedef GOAL_STATUS_T (* GOAL_ETH_CMD_FUNC_T)(struct GOAL_ETH_CMD_DATA_T *);


/**< Interface data */
typedef struct GOAL_ETH_IFACE_T {
    GOAL_BUS_T bus;                             /**< bus */
    uint32_t addr;                              /**< bus address */
    GOAL_ETH_CMD_FUNC_T pCmd;                   /**< command func */
    void *pData;                                /**< specific data */
} GOAL_ETH_IFACE_T;


/**< mirror port config */
typedef struct {
    uint32_t port;                              /**< The port id that will received all mirrored frames. A value of 0 disables the port mirroring. */
    GOAL_ETH_MIRR_MODE_T mirrortype;            /**< The type of mirroring to configure. */
    uint8_t portmap;                            /**< The port map to which the mirror configuration will be applied to. */
    GOAL_ETH_MAC_ADDR_T *macaddr;               /**< MAC address pointer */
} GOAL_ETH_MIRROR_CONF_T;


/**< MAC table entry */
typedef struct {
    GOAL_ETH_MAC_ADDR_T *addr;                  /**< MAC address pointer */
    uint32_t portmask;                          /**< port mask */
    uint32_t priority;                          /**< switching priority */
} GOAL_ETH_MACTAB_ENTRY_T;


/**< MAC table config */
typedef struct {
    GOAL_BOOL_T learn;                          /**< Enable/disable MAC learning */
    GOAL_BOOL_T ageing;                         /**< Enable/disable address aging */
    GOAL_BOOL_T migration;                      /**< Enable/disable the migration of devices from one port to another */
    GOAL_BOOL_T pervlan;                        /**< Enable/disable per VLAN MAC learning */
    GOAL_BOOL_T discunknown;                    /**< Enable/disable discarding of unknown destination frames */
} GOAL_ETH_MACTAB_CONF_T;


/**< MAC table clear/flush modes */
typedef enum {
    GOAL_ETH_MACTAB_CLR_STATIC,                 /**< Static MAC table entries */
    GOAL_ETH_MACTAB_CLR_DYNAMIC,                /**< Learned MAC table entries */
    GOAL_ETH_MACTAB_CLR_ALL                     /**< Static and learn entries */
} GOAL_ETH_MACTAB_CLR_MODES_T;


/**< QoS mode */
typedef struct {
    uint16_t mode;                              /**< A bitfield describing which modes shall be used. */
    uint8_t defprio;                            /**< Default priority */
} GOAL_ETH_QOS_MODE_T;


/**< QoS IP priority */
typedef struct {
    uint8_t diffserv;                           /**< The DiffServ field of the IP packet */
    uint8_t priority;                           /**< The priority to assign. */
} GOAL_ETH_QOS_PRIO_IP_T;


/**< QoS Ethertype priority */
typedef struct {
    uint16_t ethType;                           /**< EtherType to match against. A value of 0 disables that match */
    uint8_t prio;                               /**< priority to be assigned for the match */
} GOAL_ETH_QOS_PRIO_TYPE_T;


/**< QoS VLAN priority */
typedef struct {
    uint8_t pcp;                                /**< VLAN priority code point */
    uint8_t prio;                               /**< priority to be assigned for the match */
} GOAL_ETH_QOS_PRIO_VLAN_T;


/**< EEE */
typedef struct {
   GOAL_BOOL_T state;                           /**< State of EEE auto mode (enabled/disabled) */
   uint32_t idletime;                           /**< Idle time of EEE */
} GOAL_ETH_EEE_T;


/**< Snoop config */
typedef struct {
    GOAL_ETH_SNOOP_MATCHMODE_T mode;            /**< Snooping mode */
    GOAL_ETH_SNOOP_ACTION_T action;             /**< How to handle matched frames */
    uint16_t match;                             /**< Data to match */
    uint8_t mask;                               /**< Match mask */
    uint32_t flags;                             /**< Flags */
    uint32_t offset;                            /**< Data offset */
    uint32_t id;                                /**< Id of snoop config */
} GOAL_ETH_SNOOP_CONFIG_T;


/**< VLAN table entry */
typedef struct {
   uint32_t index;                              /**< Index of entry */
   GOAL_BOOL_T valid;                           /**< Entry valid? */
   uint32_t vlan;                               /**< VLAN ID */
   uint32_t portMask;                           /**< Port mask */
} GOAL_ETH_VLAN_ENTRY_T;


#if (GOAL_CONFIG_TDMA == 1)
/** TDMA general configuration */
typedef struct {
    GOAL_ETH_PORT_MASK_T tdmaPorts;             /**< ports where TDMA is to be activated */
    uint32_t tdmaCycleTime;                     /**< cycle time */
    uint32_t eventMask;                         /**< specifies which events shall be triggered */
    GOAL_ETH_TDMA_CB_T cb;                      /**< callback for TDMA events */
    void *pArg;                                 /**< generic callback argument */
} GOAL_ETH_TDMA_CONFIG_T;

/** TDMA timeslot configuration */
typedef struct {
    uint8_t timeslotNum;                        /**< timeslot number */
    uint32_t startOffset;                       /**< start of timeslot in us */
    uint32_t queueMsk;                          /**< queues belonging to timeslot */
    uint32_t guardBand;                         /**< size of Guard Band in us */
} GOAL_ETH_TDMA_TSLT_CFG_T;

/** TDMA enable settings */
typedef struct {
    GOAL_BOOL_T enable;                         /**< enable/disable TDMA scheduler */
    uint64_t tdmaStartTime;                     /**< the start time when the first frame is to be sent in ns */
    int64_t tdmaStartOffset;                    /**< offset between calculated start time and actual start time set by driver in ns */
} GOAL_ETH_TDMA_ENABLE_T;
#endif

/**< flood domain configuration for unknown frames */
typedef struct {
    GOAL_ETH_PORT_MASK_T portMaskBcast;         /**< flood domain port mask for broadcasts with unkown destination */
    GOAL_ETH_PORT_MASK_T portMaskMcast;         /**< flood domain port mask for multicasts with unkown destination */
    GOAL_ETH_PORT_MASK_T portMaskUcast;         /**< flood domain port mask for unicasts with unkown destination */
} GOAL_ETH_FLOOD_UNK_CONF_T;

/**< Rx Port Filter Type */
typedef enum {
    GOAL_ETH_RX_PORT_FILTER_MAC,                /**< filter frame by Destination MAC */
    GOAL_ETH_RX_PORT_FILTER_ETHTYPE,            /**< filter frame by EtherType */
} GOAL_ETH_RX_PORT_FILTER_TYPE_T;

/**< Rx Port Filter data */
typedef struct {
    GOAL_ETH_RX_PORT_FILTER_TYPE_T type;        /**< filter type */
    union {
        GOAL_ETH_MAC_ADDR_T *pMacAddr;          /**< MAC address filter value */
        uint16_t ethType;                       /**< EtherType filter value */
    } data;
} GOAL_ETH_RX_PORT_FILTER_T;

/** DLR Supervisor Configuration data */
typedef struct {
    GOAL_BOOL_T enable;                         /**< activate Supervisor */
    uint8_t precedence;                         /**< precedence value */
    uint32_t beaconInterval;                    /**< beacon interval in us */
    uint32_t beaconTimeout;                     /**< beacon time out in us */
    uint16_t vlanId;                            /**< VLAN ID */
} GOAL_ETH_DLR_SUVI_CFG_T;

/**< Ethernet command data */
typedef struct GOAL_ETH_CMD_DATA_T {
    GOAL_MA_ETH_T *pMaEth;                      /**< MA handle GOAL_MA_ETH_T */
    GOAL_ETH_IFACE_T *pIface;                   /**< interface data */
    GOAL_ETH_CMD_T cmd;                         /**< command */
    GOAL_BOOL_T flgSet;                         /**< write flag */
    GOAL_ETH_PORT_T port;                       /**< port or port mask */
    union {
        void *pRaw;                             /**< raw data pointer */
        uint32_t *pSpeed;                       /**< speed */
        uint32_t *pStateLink;                   /**< link state (up/down) */
        uint32_t *pStatePort;                   /**< port state (on/off) */
        uint32_t *pStateAutoNeg;                /**< auto neg state (on/off) */
        uint32_t *pStateAutoNegProgress;        /**< auto neg progress state */
        uint32_t *pStateHwFault;                /**< hardware fault state */
        uint32_t *pStateLedLink;                /**< link led state */
        uint32_t *pStateAuth;                   /**< port authorization state */
        uint32_t *pStateMdi;                    /**< MDI state */
        uint32_t *pModeDuplex;                  /**< duplex mode */
        uint32_t *pModeAdv;                     /**< advertisement mode */
        uint32_t *pModeMdi;                     /**< MDI mode */
        uint32_t *pCntPorts;                    /**< port count */
        uint32_t *pCapa;                        /**< capabilities */
        uint32_t *pFlgLearnAddr;                /**< address learning flag */
        uint16_t *pIdVlan;                      /**< VLAN id */
        uint32_t *pCntTabEntries;               /**< table entries */
        float *pRate;                           /**< rate */
        uint16_t *pNumStormFrames;              /**< number of frames for storm protection */
        uint16_t *pTimeStorm;                   /**< timeframe for storm protection */
        uint32_t *pIdCnfSnoop;                  /**< IGMP snooping configuration id */
        uint32_t *pStateDlr;                    /**< DLR status */
        uint32_t *pStateDirPortCtrl;            /**< port control direction state */
        uint32_t *pModeEapolRecv;               /**< port EAPOL receive mode */
        GOAL_BOOL_T *pFlgEnable;                /**< enable flag */
        GOAL_BOOL_T *pFlgDlrLearn;              /**< DLR learning flag */
        GOAL_ETH_MAC_ADDR_T *ppAddrMac;         /**< MAC address ptr ref */
        GOAL_ETH_MACTAB_ENTRY_T *pMacTab;       /**< MAC table entry ptr */
        GOAL_ETH_MACTAB_CLR_MODES_T *pMacTabClr; /**< MAC table clear modes ptr */
        GOAL_ETH_MACTAB_CONF_T *pMacTabCnf;     /**< MAC table config ptr */
        GOAL_ETH_BPDU_CTRL_T *pStateBpduCtrl;   /**< BPDU management control state */
        GOAL_ETH_VLAN_ENTRY_T *pVlan;           /**< VLAN entry */
        GOAL_ETH_VLANIN_MODE_T *pModeVlanIn;    /**< VLAN in mode */
        GOAL_ETH_VLANOUT_MODE_T *pModeVlanOut;  /**< VLAN out mode */
        GOAL_ETH_QOS_MODE_T *pModeQos;          /**< QoS mode */
        GOAL_ETH_QOS_PRIO_IP_T *pPrioQosIp;     /**< QoS IP priority */
        GOAL_ETH_QOS_PRIO_TYPE_T *pPrioQosEtype; /**< QoS Ethertype priority */
        GOAL_ETH_QOS_PRIO_VLAN_T *pPrioQosVlan; /**< VLAN priority mapping */
        GOAL_ETH_MIRROR_CONF_T *pCnfMirror;     /**< mirror port configuration */
        GOAL_ETH_EEE_T *pCnfEee;                /**< EEE configuration */
        GOAL_ETH_SNOOP_CONFIG_T *pCnfSnoop;     /**< IGMP snooping configuration */
#if (GOAL_CONFIG_TDMA == 1)
        GOAL_ETH_TDMA_CONFIG_T *pTdmaConfig;    /**< TDMA schedule config */
        GOAL_ETH_TDMA_TSLT_CFG_T *pTdmaTsltCfg; /**< TDMA timeslot configuration */
        GOAL_ETH_TDMA_ENABLE_T *pTdmaEnable;    /**< TDMA enable setting */
        uint32_t *pState;                       /**< new state for TDMA scheduler */
        uint32_t *pQueuesStart;                 /**< TDMA First Cycle Transmit Enable state */
#endif
        uint32_t *pOutQueues;                   /**< Number of output queues */
        GOAL_ETH_FLOOD_UNK_CONF_T *pFloodUnkConf; /**< configuration of flood domain for frames with unknown destination */
        uint32_t *pLatencyEgress;               /**< egress latency */
        uint32_t *pLatencyIngress;              /**< ingress latency */
        uint32_t *pFrameSizeMax;                /**< maximum frame size */
        GOAL_ETH_RX_PORT_FILTER_T *pRxPortFilter; /**< filter of frames that must resolve their Rx port */
        uint32_t *pPhysics;                     /**< interface physics */
        GOAL_BOOL_T *pFramePreemptSupport;      /**< is frame preemption supported */
        GOAL_BOOL_T *pFramePreemptEnable;       /**< enable/disable frame preemption */
        uint32_t *pFramePreemptVerificationStatus; /**< frame preemption verification status */
        uint32_t *pFramePreemptAddFragSize;     /**< frame preemption additional fragmentation size (in 64 byte units) */
        GOAL_BOOL_T *pFramePreemptActive;       /**< is frame preemption active */
        uint32_t *pFramePreemptTrafficClassAssign; /**< frame preemption: bitmap of preemptable traffic classes */
        GOAL_ETH_DLR_SUVI_CFG_T *pSuViCfg;      /**< DLR Supervisor Configuration */
        GOAL_BOOL_T *pSuViCfgSupported;         /**< current Supervisor Configuration supported */
        char **ppIfaceName;                     /**< interface name */
    } data;
} GOAL_ETH_CMD_DATA_T;


/** Ethernet command function */
typedef struct {
    GOAL_ETH_CMD_T cmd;                         /**< command */
    GOAL_ETH_CMD_FUNC_T func;                   /**< command function */
    uint32_t param;                             /**< parameters needed */
} GOAL_ETH_FUNCTION_ENTRY_T;


/**< ETH handle instance struct */
typedef struct GOAL_MI_ETH_T {
    struct GOAL_MI_ETH_T *pNext;                /**< next */
    uint32_t id;                                /**< instance id */
    uint32_t numPorts;                          /**< number of ports */
    uint32_t portMaskValidAll;                  /**< port mask containing all valid port bits */
    GOAL_MA_ETH_T *pMaEth;                      /**< MA handle GOAL_MA_ETH_T */
    GOAL_QUEUE_T *pEthRxLowQueue;               /**< received frames */
    GOAL_QUEUE_T *pEthTxLowQueue;               /**< frames to transmit */
    GOAL_QUEUE_T *pEthTxHighQueue;              /**< received frames */
    GOAL_LOCK_T *pCmdLock;                      /**< command table access lock */
    GOAL_ETH_BUF_HANDLER_T *pHandlerLow;        /**< low priority EtherType RX handler */
    GOAL_ETH_BUF_HANDLER_T *pHandlerHigh;       /**< high priority EtherType RX handler */
    GOAL_ETH_BUF_HANDLER_T *pHandlerMacLow;     /**< low priority MAC address RX handler */
    GOAL_ETH_BUF_HANDLER_T *pHandlerMacHigh;    /**< high priority MAC address RX handler */
    GOAL_ETH_FUNCTION_ENTRY_T *pEthTableCmd;    /**< table of featured ethernet commands */
    uint32_t cmVarInstId;                       /**< instance ID for CM variables */
#if GOAL_CONFIG_DD == 1
    GOAL_CM_VAR_T *pVarSpeed;                   /**< speed cm var reference */
    GOAL_CM_VAR_T *pVarLink;                    /**< link cm var reference */
    GOAL_CM_VAR_T *pVarDuplex;                  /**< duplex cm var reference */
    GOAL_CM_VAR_T *pVarPortCnt;                 /**< port count cm var reference */
#endif /* GOAL_CONFIG_DD */
    GOAL_BOOL_T initFlag;                       /**< init flag */
    GOAL_ETH_PORT_STATE_T *pStatePorts;         /**< Ethernet port states */
#if GOAL_PTP == 1
    GOAL_ETH_TIMESTAMP_CB_T pPortTsCbFunc;      /**< Egress Timestamp capture callback handler */
    void *pPortTsCbArg;                         /**< Egress Timestamp capture callback argument */
#endif
#if GOAL_CONFIG_ETH_STATS_INTERNAL == 1
    uint64_t *statBuf[GOAL_ETH_STAT_ID_END];
#endif /* GOAL_CONFIG_ETH_STATS_INTERNAL */

} GOAL_MI_ETH_T;


/****************************************************************************/
/* Inline functions */
/****************************************************************************/


/****************************************************************************/
/** Get an Eth Mi handle for a given ID
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static inline GOAL_STATUS_T goal_miEthHdlGet(
    uint32_t id,                                /**< NET MI id */
    GOAL_MI_ETH_T **ppMiEthHdl                  /**< [out] MI handle */
)
{
    return goal_instGetById((GOAL_INSTANCE_T **) ppMiEthHdl, GOAL_ID_ETH, id);
}


/****************************************************************************/
/** Create a VLAN tag control information
 *
 * @return tag control information
 */
static inline uint16_t goal_miEthVlanTciCreate(
    uint16_t vid,                               /**< VLAN ID */
    uint8_t pcp                                 /**< priority code point */
)
{
    uint16_t tci;                               /* tag control information */

    tci = vid & 0x0FFF;
    tci |= (uint16_t) ((pcp & 0x0007) << 13);

    return tci;
}


/****************************************************************************/
/** Check for valid VLAN tag control information
 *
 * @returns GOAL_BOOL_T value
 */
static inline GOAL_BOOL_T goal_miEthVlanTciValid(
    uint16_t vid,                               /**< VLAN ID */
    uint8_t pcp                                 /**< priority code point */
)
{
    if ((0x0FFF >= vid) && (0x07 >= pcp)) {
        return GOAL_TRUE;
    }

    return GOAL_FALSE;
}


/** Extract PCP value from a frame with a VLAN tag
 *
 * @return PCP value
 */
static inline uint8_t goal_miEthVlanPcpExtract(
    GOAL_BUFFER_T *pBuf                         /**< GOAL network buffer */
)
{
    return ((pBuf->ptrData[14] >> 5) & 0x07);
}


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_miEthReg(
    void
);

GOAL_STATUS_T goal_miEthOpen(
    uint32_t idMi,                              /**< MI id */
    uint32_t idMa                               /**< MA id */
);

void goal_miEthRecv(
    GOAL_MA_ETH_T *pMaEthHdl,                   /**< [in] MA handle */
    uint32_t portIdx,                           /**< Ethernet port index */
    GOAL_BUFFER_T **ppBuf                       /**< [out] OS Ethernet buffer */
);

GOAL_STATUS_T goal_miEthSend(
    GOAL_MI_ETH_T *pMiEthHdl,                   /**< [in] MI handle */
    GOAL_NET_TX_TYPE_T type,                    /**< frame channel type */
    void **ppBuf                                /**< [in,out] pointer to PROFINET buffer */
);

GOAL_STATUS_T goal_miEthSendFrameGet(
    GOAL_MA_ETH_T *pMaEthHdl,                   /**< [in] MA handle */
    GOAL_BUFFER_T **ppBuf                       /**< [out] GOAL buffer ptr ref */
);

void goal_miEthSendDone(
    GOAL_MA_ETH_T *pMaEthHdl,                   /**< [in] MA handle */
    GOAL_BUFFER_T **ppBuf,                      /**< [out]frame buffer */
    GOAL_BOOL_T doneFlag                        /**< done flag */
);

GOAL_STATUS_T goal_miEthProtoAdd(
    GOAL_MI_ETH_T *pMiEthHdl,                   /**< MI handle */
    GOAL_BOOL_T highPrio,                       /**< high priority flag */
    uint16_t etherType,                         /**< EtherType */
    GOAL_ETH_MAC_ADDR_T *pAddrMac,              /**< MAC address ptr */
    GOAL_ETH_BUF_FUNC_T pFunc                   /**< protocol handler function */
);

GOAL_STATUS_T goal_miEthProtoAddPos(
    GOAL_MI_ETH_T *pMiEthHdl,                   /**< MI handle */
    GOAL_BOOL_T highPrio,                       /**< high priority flag */
    uint16_t etherType,                         /**< EtherType */
    GOAL_ETH_MAC_ADDR_T *pAddrMac,              /**< MAC address ptr */
    GOAL_ETH_BUF_FUNC_T pFunc,                  /**< protocol handler function */
    void *pArg,                                 /**< callback argument */
    GOAL_BOOL_T beforeSame                      /**< add handler before handlers using same ether type */
);

#if GOAL_CONFIG_LOGGING_TARGET_SYSLOG == 1
void goal_miEthSendSyslog(
    GOAL_MI_ETH_T *pMiEthHdl,                   /**< MI handle */
    const char *str,                            /**< log string */
    unsigned int len                            /**< string length */
);
#endif /* GOAL_CONFIG_LOGGING_TARGET_SYSLOG */

GOAL_STATUS_T goal_miEthPortsGet(
    GOAL_MI_ETH_T *pMiEthHdl,                   /**< [in] MI handle */
    uint32_t *pPorts                            /**< port count ref */
);

GOAL_STATUS_T goal_miEthCmd(
    GOAL_MI_ETH_T *pMiEthHdl,                   /**< [in] MI handle */
    GOAL_ETH_CMD_T cmd,                         /**< command */
    GOAL_BOOL_T wrFlag,                         /**< write flag */
    uint32_t port,                              /**< port ID */
    void *pArg                                  /**< argument */
);

GOAL_STATUS_T goal_miEthCmdTable(
    GOAL_MI_ETH_T *pMiEthHdl,                   /**< [in] MI handle */
    GOAL_ETH_CMD_T cmd,                         /**< command */
    GOAL_ETH_FUNCTION_ENTRY_T *pTableFunc,      /**< table of Ethernet functions */
    GOAL_BOOL_T wrFlag,                         /**< write flag */
    uint32_t port,                              /**< port ID */
    void *pArg                                  /**< argument */
);

void goal_miEthCmdData(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    GOAL_ETH_CMD_DATA_T *pDataCmd,              /**< data structure */
    GOAL_ETH_CMD_T cmd,                         /**< command */
    GOAL_BOOL_T flgSet,                         /**< set flag */
    uint32_t port,                              /**< port or port mask */
    void *pArg                                  /**< argument ptr */
);

GOAL_STATUS_T goal_miEthCmdExist(
    GOAL_MI_ETH_T *pMiEthHdl,                   /**< [in] MI handle */
    GOAL_ETH_CMD_T cmd                          /**< command */
);

GOAL_STATUS_T goal_miEthCmdExistTable(
    GOAL_MI_ETH_T *pMiEthHdl,                   /**< [in] MI handle */
    GOAL_ETH_CMD_T cmd,                         /**< command */
    GOAL_ETH_FUNCTION_ENTRY_T *pTableFunc       /**< eth function table */
);

GOAL_STATUS_T goal_miEthTableReg(
    GOAL_MI_ETH_T *pMiEthHdl,                   /**< [in] MI handle */
    GOAL_ETH_FUNCTION_ENTRY_T *pTable           /**< table of Ethernet functions to register */
);

#if GOAL_ETH_NAMES == 1
const char * goal_miEthCmdNameGet(
    uint32_t cmd                                /**< command id */
);
#endif /* GOAL_ETH_NAMES */

GOAL_STATUS_T goal_miEthLinkStateGet(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t id,                                /**< interface ID */
    uint32_t *pState                            /**< link state ref */
);

GOAL_STATUS_T goal_miEthPortStateGet(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t port,                              /**< port ID */
    uint32_t *pState                            /**< port state ref */
);

GOAL_STATUS_T goal_miEthPortStateSet(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t id,                                /**< interface ID */
    uint32_t state                              /**< port state */
);

GOAL_STATUS_T goal_miEthPhysicsGet(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t port,                              /**< port ID */
    uint32_t *pPhysics                          /**< physics ref */
);

GOAL_STATUS_T goal_miEthMacAddrGet(
    GOAL_MI_ETH_T *pMiEthHdl,                   /**< [in] MI handle */
    uint32_t id,                                /**< interface ID */
    GOAL_ETH_MAC_ADDR_T *pMac                   /**< MAC address pointer */
);

GOAL_STATUS_T goal_miEthCfgMacAddrImpl(
    uint32_t port,                              /**< port ID */
    uint32_t idMa,                              /**< associated MA */
    GOAL_ETH_MAC_ADDR_T *pMac                   /**< MAC address ptr */
);

GOAL_STATUS_T goal_miEthMacAddrSet(
    GOAL_MI_ETH_T *pMiEthHdl,                   /**< [in] MI handle */
    uint32_t id,                                /**< interface ID */
    GOAL_ETH_MAC_ADDR_T *pMac                   /**< MAC address pointer */
);

GOAL_STATUS_T goal_miEthMacUniFlush(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t id                                 /**< interface ID */
);

GOAL_STATUS_T goal_miEthMacMultiFlush(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t id                                 /**< interface ID */
);

GOAL_STATUS_T goal_miEthMcastAdd(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    GOAL_ETH_PORT_T port,                       /**< port or mask */
    GOAL_ETH_MAC_ADDR_T *pMac                   /**< MAC address */
);

GOAL_STATUS_T goal_miEthMcastDel(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    GOAL_ETH_PORT_T port,                       /**< port or mask */
    GOAL_ETH_MAC_ADDR_T *pMac                   /**< MAC address */
);

GOAL_STATUS_T goal_miEthLinkSpeedGet(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t id,                                /**< interface ID */
    uint32_t *pSpeed                            /**< speed ref */
);

GOAL_STATUS_T goal_miEthLinkSpeedSet(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t id,                                /**< interface ID */
    uint32_t speed                              /**< speed */
);

GOAL_STATUS_T goal_miEthLinkDuplexGet(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t id,                                /**< interface ID */
    uint32_t *pDuplex                           /**< duplex ref */
);

GOAL_STATUS_T goal_miEthLinkDuplexSet(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t id,                                /**< interface ID */
    uint32_t duplex                             /**< duplex */
);

GOAL_STATUS_T goal_miEthMdiModeGet(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t port,                              /**< port id */
    uint32_t *pModeMdi                          /**< MDI mode */
);

GOAL_STATUS_T goal_miEthMdiModeSet(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t port,                              /**< port id */
    uint32_t modeMdi                            /**< MDI mode */
);

GOAL_STATUS_T goal_miEthMdiStateGet(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t port,                              /**< port id */
    uint32_t *pModeMdi                          /**< MDI mode */
);

GOAL_STATUS_T goal_miEthMdiStateSet(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t port,                              /**< port id */
    uint32_t modeMdi                            /**< MDI mode */
);

GOAL_STATUS_T goal_miEthAutonegGet(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t id,                                /**< interface ID */
    uint32_t *pAutoneg                          /**< autoneg ref */
);

GOAL_STATUS_T goal_miEthAutonegSet(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t id,                                /**< interface ID */
    uint32_t autoneg                            /**< autoneg */
);

GOAL_STATUS_T goal_miEthAutonegProgressGet(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t port,                              /**< port ID */
    uint32_t *pAutoneg                          /**< autoneg ref */
);

GOAL_STATUS_T goal_miEthPortStateCbReg(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t id,                                /**< port id */
    GOAL_ETH_PORT_STATE_CB_FUNC_T pFunc         /**< callback function */
);

GOAL_STATUS_T goal_miEthPortStateArgCbReg(
    GOAL_MI_ETH_T *pMiInst,                     /**< MI instance */
    uint32_t id,                                /**< port id */
    GOAL_ETH_PORT_STATE_ARG_CB_FUNC_T pFunc,    /**< callback function */
    void *pArg                                  /**< additional callback argument */
);

uint32_t goal_miEthMtuGet(
    void
);

#if GOAL_PTP == 1
GOAL_STATUS_T goal_miEthTimestampCaptureCbReg(
    GOAL_MI_ETH_T *pMiInst,                     /**< Eth MI instance */
    GOAL_ETH_TIMESTAMP_CB_T pCb,                /**< callback function */
    void *pArg                                  /**< callback argument */
);

void goal_miEthTimestampCaptureCbExec(
    GOAL_MA_ETH_T *pMaEthHdl,                   /**< [in] MA handle */
    uint64_t sec,                               /**< full seconds */
    uint32_t nsec,                              /**< sub-seconds in ns */
    uint32_t port                               /**< sending port */
);
#endif /* GOAL_PTP == 1 */

#if GOAL_CONFIG_ETH_STATS_INTERNAL == 1
void goal_miEthStatInternalInc(
    GOAL_MI_ETH_T *pMiEthHdl,                   /**< [in] MI handle */
    GOAL_ETH_STAT_ENUM_T statId,                /**< statistic type */
    uint32_t portId,                            /**< port Index */
    uint64_t incVal                             /**< increment value */
);
#endif /* GOAL_CONFIG_ETH_STATS_INTERNAL */


/****************************************************************************/
/* Layer of Compatibility */
/****************************************************************************/
static inline GOAL_STATUS_T goal_ethGetNetBuf(
    GOAL_BUFFER_T **ppBuf                       /**< ptr to store new buffer */
)
{
    return goal_queuePoolGetBuf(ppBuf, GOAL_NETBUF_SIZE, GOAL_ID_ETH_RECV);
}

static inline GOAL_STATUS_T goal_ethInitPre(
    void
)
{
    return goal_miEthReg();
}

static inline void goal_ethReceive(
    uint32_t id,                                /**< Ethernet port index */
    GOAL_BUFFER_T **ppBuf                       /**< OS Ethernet buffer */
)
{
    goal_miEthRecv(NULL, id, ppBuf);
}

static inline GOAL_STATUS_T goal_ethSend(
    void **ppBuf,                               /**< pointer to PROFINET buffer */
    GOAL_NET_TX_TYPE_T type                     /**< frame channel type */
)
{
    return goal_miEthSend(NULL, type, ppBuf);
}

#if GOAL_CONFIG_LOGGING_TARGET_SYSLOG == 1
static inline void goal_ethSendSyslog(
    const char *str,                            /**< log string */
    unsigned int len                            /**< string length */
)
{
    goal_miEthSendSyslog(NULL, str, len);
}
#endif /* GOAL_CONFIG_LOGGING_TARGET_SYSLOG */

static inline GOAL_STATUS_T goal_ethSendFrameGet(
    GOAL_BUFFER_T **ppBuf                       /**< GOAL buffer ptr ref */
)
{
    return goal_miEthSendFrameGet(NULL, ppBuf);
}

static inline void goal_ethSendDone(
    GOAL_BUFFER_T **ppBuf,                      /**< frame buffer */
    GOAL_BOOL_T doneFlag                        /**< done flag */
)
{
    goal_miEthSendDone(NULL, ppBuf, doneFlag);
}

static inline GOAL_STATUS_T goal_ethProtoAdd(
    GOAL_BOOL_T highPrio,                       /**< high priority flag */
    uint16_t etherType,                         /**< EtherType */
    GOAL_ETH_MAC_ADDR_T *pAddrMac,              /**< MAC address ptr */
    GOAL_ETH_BUF_FUNC_T pFunc,                  /**< protocol handler function */
    void *pArg                                  /**< callback argument */
)
{
    return goal_miEthProtoAddPos(NULL, highPrio, etherType, pAddrMac, pFunc, pArg, GOAL_FALSE);
}

static inline GOAL_STATUS_T goal_ethProtoAddPos(
    GOAL_BOOL_T highPrio,                       /**< high priority flag */
    uint16_t etherType,                         /**< EtherType */
    GOAL_ETH_MAC_ADDR_T *pAddrMac,              /**< MAC address ptr */
    GOAL_ETH_BUF_FUNC_T pFunc,                  /**< protocol handler function */
    void *pArg,                                 /**< callback argument */
    GOAL_BOOL_T beforeSame                      /**< add handler before handlers using same ether type */
)
{
    return goal_miEthProtoAddPos(NULL, highPrio, etherType, pAddrMac, pFunc, pArg, beforeSame);
}

static inline GOAL_STATUS_T goal_ethMacAddrGet(
    uint32_t port,                              /**< port ID */
    GOAL_ETH_MAC_ADDR_T *pMac                   /**< MAC address ptr */
)
{
    return goal_miEthMacAddrGet(NULL, port, pMac);
}

static inline GOAL_STATUS_T goal_ethMacAddrSet(
    uint32_t port,                              /**< port ID */
    GOAL_ETH_MAC_ADDR_T *pMac                   /**< MAC address ptr */
)
{
    return goal_miEthMacAddrSet(NULL, port, pMac);
}

static inline GOAL_STATUS_T goal_ethMacUniFlush(
    uint32_t port                               /**< port ID */
)
{
    return goal_miEthMacUniFlush(NULL, port);
}

static inline GOAL_STATUS_T goal_ethMacMultiFlush(
    uint32_t port                               /**< port ID */
)
{
    return goal_miEthMacMultiFlush(NULL, port);
}

static inline GOAL_STATUS_T goal_ethMcastAdd(
    GOAL_ETH_PORT_T port,                       /**< port or mask */
    GOAL_ETH_MAC_ADDR_T *pMac                   /**< MAC address */
)
{
    return goal_miEthMcastAdd(NULL, port, pMac);
}

static inline GOAL_STATUS_T goal_ethMcastDel(
    GOAL_ETH_PORT_T port,                       /**< port or mask */
    GOAL_ETH_MAC_ADDR_T *pMac                   /**< MAC address */
)
{
    return goal_miEthMcastDel(NULL, port, pMac);
}

static inline GOAL_STATUS_T goal_ethTableReg(
    GOAL_ETH_FUNCTION_ENTRY_T *pTable           /**< table of Ethernet functions to register */
)
{
    return goal_miEthTableReg(NULL, pTable);
}

static inline GOAL_STATUS_T goal_ethPortsGet(
    uint32_t *pPorts                            /**< port count ref */
)
{
    return goal_miEthPortsGet(NULL, pPorts);
}

static inline GOAL_STATUS_T goal_ethPortStateSet(
    uint32_t port,                              /**< port ID */
    uint32_t state                              /**< port state */
)
{
    return goal_miEthPortStateSet(NULL, port, state);
}

static inline GOAL_STATUS_T goal_ethPortStateCbReg(
    uint32_t id,                                /**< port id */
    GOAL_ETH_PORT_STATE_CB_FUNC_T pFunc         /**< callback function */
)
{
    return goal_miEthPortStateCbReg(NULL, id, pFunc);
}

static inline GOAL_STATUS_T goal_ethPortStateArgCbReg(
    uint32_t id,                                /**< port id */
    GOAL_ETH_PORT_STATE_ARG_CB_FUNC_T pFunc,    /**< callback function */
    void *pArg                                  /**< [in] callback argument to register */
)
{
    return goal_miEthPortStateArgCbReg(NULL, id, pFunc, pArg);
}

static inline GOAL_STATUS_T goal_ethCmd(
    GOAL_ETH_CMD_T cmd,                         /**< command */
    GOAL_BOOL_T wrFlag,                         /**< write flag */
    uint32_t port,                              /**< port ID */
    void *pArg                                  /**< argument */
)
{
    return goal_miEthCmd(NULL, cmd, wrFlag, port, pArg);
}

static inline void goal_ethCmdData(
    GOAL_ETH_CMD_DATA_T *pDataCmd,              /**< data structure */
    GOAL_ETH_CMD_T cmd,                         /**< command */
    GOAL_BOOL_T flgSet,                         /**< set flag */
    uint32_t port,                              /**< port or port mask */
    void *pArg                                  /**< argument ptr */
)
{
    goal_miEthCmdData(NULL, pDataCmd, cmd, flgSet, port, pArg);
}

static inline GOAL_STATUS_T goal_ethCmdExist(
    GOAL_ETH_CMD_T cmd                          /**< command */
)
{
    return goal_miEthCmdExist(NULL, cmd);
}

#if GOAL_ETH_NAMES == 1
static inline const char * goal_ethCmdNameGet(
    uint32_t cmd                                /**< command id */
)
{
    return goal_miEthCmdNameGet(cmd);
}
#endif /* GOAL_ETH_NAMES */

static inline GOAL_STATUS_T goal_ethLinkSpeedGet(
    uint32_t port,                              /**< port ID */
    uint32_t *pSpeed                            /**< speed ref */
)
{
    return goal_miEthLinkSpeedGet(NULL, port, pSpeed);
}

static inline GOAL_STATUS_T goal_ethLinkSpeedSet(
    uint32_t port,                              /**< port ID */
    uint32_t speed                              /**< speed */
)
{
    return goal_miEthLinkSpeedSet(NULL, port, speed);
}

static inline GOAL_STATUS_T goal_ethLinkDuplexGet(
    uint32_t port,                              /**< port ID */
    uint32_t *pDuplex                           /**< duplex ref */
)
{
    return goal_miEthLinkDuplexGet(NULL, port, pDuplex);
}

static inline GOAL_STATUS_T goal_ethLinkDuplexSet(
    uint32_t port,                              /**< port ID */
    uint32_t duplex                             /**< duplex */
)
{
    return goal_miEthLinkDuplexSet(NULL, port, duplex);
}

static inline GOAL_STATUS_T goal_ethMdiModeGet(
    uint32_t port,                              /**< port id */
    uint32_t *pModeMdi                          /**< MDI mode */
)
{
    return goal_miEthMdiModeGet(NULL, port, pModeMdi);
}

static inline GOAL_STATUS_T goal_ethMdiModeSet(
    uint32_t port,                              /**< port id */
    uint32_t modeMdi                            /**< MDI mode */
)
{
    return goal_miEthMdiModeSet(NULL, port, modeMdi);
}

static inline GOAL_STATUS_T goal_ethMdiStateGet(
    uint32_t port,                              /**< port id */
    uint32_t *pModeMdi                          /**< MDI mode */
)
{
    return goal_miEthMdiStateGet(NULL, port, pModeMdi);
}

static inline GOAL_STATUS_T goal_ethMdiStateSet(
    uint32_t port,                              /**< port id */
    uint32_t stateMdi                           /**< MDI state */
)
{
    return goal_miEthMdiStateSet(NULL, port, stateMdi);
}

static inline GOAL_STATUS_T goal_ethAutonegGet(
    uint32_t id,                                /**< interface ID */
    uint32_t *pAutoneg                          /**< autoneg ref */
)
{
    return goal_miEthAutonegGet(NULL, id, pAutoneg);
}

static inline GOAL_STATUS_T goal_ethAutonegSet(
    uint32_t id,                                /**< interface ID */
    uint32_t autoneg                            /**< autoneg */
)
{
    return goal_miEthAutonegSet(NULL, id, autoneg);
}

static inline GOAL_STATUS_T goal_ethAutonegProgressGet(
    uint32_t port,                              /**< port ID */
    uint32_t *pAutoneg                          /**< autoneg ref */
)
{
    return goal_miEthAutonegProgressGet(NULL, port, pAutoneg);
}

static inline GOAL_STATUS_T goal_ethPortStateGet(
    uint32_t port,                              /**< port ID */
    uint32_t *pState                            /**< port state ref */
)
{
    return goal_miEthPortStateGet(NULL, port, pState);
}

static inline GOAL_STATUS_T goal_ethLinkStateGet(
    uint32_t port,                              /**< port ID */
    uint32_t *pState                            /**< link state ref */
)
{
    return goal_miEthLinkStateGet(NULL, port, pState);
}

static inline GOAL_STATUS_T goal_ethPhysicsGet(
    uint32_t port,                              /**< port ID */
    uint32_t *pPhysics                          /**< physics ref */
)
{
    return goal_miEthPhysicsGet(NULL, port, pPhysics);
}

#if GOAL_PTP == 1
static inline GOAL_STATUS_T goal_ethTimestampCaptureCbReg(
    GOAL_ETH_TIMESTAMP_CB_T pCb,                /**< callback function */
    void *pArg                                  /**< callback argument */
)
{
    return goal_miEthTimestampCaptureCbReg(NULL, pCb, pArg);
}

static inline void goal_ethTimestampCaptureCbExec(
    uint64_t sec,                               /**< full seconds */
    uint32_t nsec,                              /**< sub-seconds in ns */
    uint32_t port                               /**< sending port */
)
{
    goal_miEthTimestampCaptureCbExec(NULL, sec, nsec, port);
}
#endif /* GOAL_PTP == 1 */

static inline uint32_t goal_ethMtuGet(
    void
)
{
    return goal_miEthMtuGet();
}


#endif /* GOAL_ETH_H */
