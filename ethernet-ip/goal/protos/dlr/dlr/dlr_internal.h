/** @file
 *
 * @brief Device Level Ring Protocol (DLR)
 *
 * This module contains internal functions and macros for the DLR protocol
 * stack.
 *
 * @copyright
 * Copyright 2023 port GmbH Halle/Saale.
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

#ifndef DLR_INTERNAL_H
#define DLR_INTERNAL_H


/****************************************************************************/
/* Internal defines */
/****************************************************************************/
#define DLR_ETH_PORT_CNT                      2 /**< number of DLR ports */
#define DLR_ETHERTYPE                    0x80E1 /**< DLR EtherType */

/* offsets of DLR frame fields from the DLR EthertType */
#define DLR_FRAME_RINGSUBTYPE              0x02 /**< Ring Sub-type */
#define DLR_FRAME_PROTOCOLVERSION          0x03 /**< Ring Protocol Version */
#define DLR_FRAME_FRAMETYPE                0x04 /**< Frame Type */
#define DLR_FRAME_SOURCEPORT               0x05 /**< Source Port */
#define DLR_FRAME_SOURCEIP                 0x06 /**< Source IP Address */
#define DLR_FRAME_SEQID                    0x0A /**< Sequence Id */
#define DLR_FRAME_DATA                     0x0E /**< frame tyte specific data */

/* DLR frame types */
#define DLR_FRAMETYPE_BEACON               0x01 /**< Beacon */
#define DLR_FRAMETYPE_NEIGHBORCHECKREQ     0x02 /**< Neighbor_Check_Request */
#define DLR_FRAMETYPE_NEIGHBORCHECKRES     0x03 /**< Neighbor_Check_Response */
#define DLR_FRAMETYPE_LINKSTATUS           0x04 /**< Link_Status / Neighbor_Status */
#define DLR_FRAMETYPE_LOCATEFAULT          0x05 /**< Locate_Fault */
#define DLR_FRAMETYPE_ANNOUNCE             0x06 /**< Announce */
#define DLR_FRAMETYPE_SIGNON               0x07 /**< Sign_On */
#define DLR_FRAMETYPE_ADVERTISE            0x08 /**< Advertise */
#define DLR_FRAMETYPE_FLUSHTABLES          0x09 /**< Flush_Tables */
#define DLR_FRAMETYPE_LEARNINGUPDATE       0x0A /**< Learning_Update */

/* constant DLR frame field values */
#define DLR_RINGSUBTYPE                    0x02 /**< Ring Sub-type value */
#define DLR_PROTOCOLVERSION                0x01 /**< Ring Protocol Version value */

/* DLR Ring State in DLR frames */
#define DLR_RINGSTATE_FRAME_NORMAL         0x01 /**< normal ring state */
#define DLR_RINGSTATE_FRAME_FAULT          0x02 /**< fault ring state */

#define DLR_VLAN_PCP                     0xE000 /**< VLAN PCP value for DLR frames */

/* Neighbor Checking */
#define DLR_NEIGHBOR_CHECK_TRIES 3              /**< no. of check requests */
#define DLR_NEIGHBOR_CHECK_TIME_MS 100          /**< check request timeout */


/****************************************************************************/
/* Internal Data types */
/****************************************************************************/
/** internal state of a DLR node */
typedef enum {
    DLR_NODESTATE_IDLE = 0,                     /**< IDLE */
    DLR_NODESTATE_NORMAL = 1,                   /**< NORMAL */
    DLR_NODESTATE_FAULT = 2,                    /**< FAULT */
} DLR_NODESTATE_T;

/** network state */
typedef enum {
    DLR_RINGSTATE_NORMAL,                       /**< NORMAL */
    DLR_RINGSTATE_FAULT,                        /**< FAULT */
    DLR_RINGSTATE_LOOP                          /**< local loop */
} DLR_RINGSTATE_T;

/** Last Beacon indicator */
typedef enum {
    DLR_LASTBEACON_NONE,                        /**< no frame received */
    DLR_LASTBEACON_PORT1,                       /**< frame received on port 1 */
    DLR_LASTBEACON_PORT2,                       /**< frame received on port 2  */
    DLR_LASTBEACON_ALL                          /**< frame received on both ports */
} DLR_LASTBEACON_T;

/** Neighbor_Check data set */
typedef struct {
    GOAL_ETH_PORT_T port;                       /**< Rx port */
    uint32_t seqId;                             /**< received sequence ID */
} DLR_NEIGHBORCHECKDATA_T;

#if DLR_CONFIG_ANNOUNCE_RINGNODE == 1
/** data of a received Announce frame */
typedef struct {
    GOAL_ETH_MAC_ADDR_T suViMac;                /**< Supervisor MAC Address */
    uint16_t vlanId;                            /**< Supervisor VLAN Id */
    uint32_t suViIp;                            /**< Supervisor IP Address */
    uint32_t seqId;                             /**< sequence Id */
    uint8_t ringState;                          /**< Ring State propagated by Supervisor */
} DLR_ANNOUNCE_DATA_T;
#endif /* DLR_CONFIG_ANNOUNCE_RINGNODE */

/** timer argument */
typedef struct {
    struct DLR_INSTANCE_T *pDlr;                /**< instance data */
    GOAL_ETH_PORT_T portId;                     /**< port ID */
} DLR_TIMER_ARG_T;

/** DLR instance data */
typedef struct DLR_INSTANCE_T {
    GOAL_INSTANCE_HEADER(DLR_INSTANCE_T);       /**< instance header */
    GOAL_MI_ETH_T *pMiEthHdl;                   /**< Eth Mi handle of interface */
    GOAL_MI_NET_T *pMiNetHdl;                   /**< Net MI handle of interface */
    GOAL_DLR_FUNC_CB_T goal_dlrCb;              /**< application callback */
    void *pCbArg;                               /**< application callback argument */
    DLR_NODESTATE_T nodeState;                  /**< local Node state */
#if DLR_CONFIG_BEACON_RINGNODE == 1
    DLR_LASTBEACON_T lastBeacon;                /**< last Beacon port */
#endif /* DLR_CONFIG_BEACON_RINGNODE == 1 */
    DLR_RINGSTATE_T ringState;                  /**< global Ring state */
    GOAL_TIMER_T *pTmrNCheck1;                  /**< NeighborCheck Timer 1 */
    GOAL_TIMER_T *pTmrNCheck2;                  /**< NeighborCheck Timer 2 */
    uint8_t nCheckTry[2];                       /**< NeighborCheck execution counts */
    uint32_t nCheckTimedOut;                    /**< mark which port's neighbor check failed */
    GOAL_BOOL_T sendLerningUpdate;              /**< LerningUpdate Tx flag */
    GOAL_LOCK_T *mtx;                           /**< mutex for state machine */
    uint8_t suViMac[6];                         /**< active Supervisor MAC */
    uint32_t suViIp;                            /**< active Supervisor IP */
    uint8_t suViPrecedence;                     /**< active Supervisor Precedence */
    uint16_t vlan;                              /**< active Supervisor VLAN Id */
    DLR_TIMER_ARG_T timerArg[2];                /**< timer argument */
    DLR_NEIGHBORCHECKDATA_T nCheckReqData[2];   /**< NeighborCheck data set */
    uint32_t nCheckSeqID[2];                    /**< NeighborCheck Tx Sequence Id */
    GOAL_BUFFER_T *pSignOnFrame;                /**< received SignOn frame */
    GOAL_QUEUE_T *rxQueue;                      /**< DLR frame Rx queue */
#if DLR_CONFIG_ANNOUNCE_RINGNODE == 1
    DLR_ANNOUNCE_DATA_T announceData;           /**< Announce frame data */
    uint32_t anSeqId;                           /**< last received sequence ID of Announce frame */
    GOAL_TIMER_T *pTmrAnnounce;                 /**< Announce Rx Timeout timer */
    uint32_t portLinkUp;                        /**< link up mask */
#endif /* DLR_CONFIG_ANNOUNCE_RINGNODE */
    uint32_t linkStatusSeqId;                   /**< sequence ID of LinkStatus frames */
    uint32_t learningUpdateSeqId;               /**< sequence ID of LerningUpdate frames */
} DLR_INSTANCE_T;


/****************************************************************************/
/* Internal Functions */
/****************************************************************************/
#if DLR_CONFIG_BEACON_RINGNODE == 1
void DLR_storeSupervisor(
    DLR_INSTANCE_T *pDlr                        /**< DLR instance */
);

GOAL_BOOL_T DLR_isPrecedenceHigher(
    DLR_INSTANCE_T *pDlr                        /**< DLR instance */
);
#endif /* DLR_CONFIG_BEACON_RINGNODE == 1 */

GOAL_STATUS_T DLR_NeighborCheckReqTx(
    DLR_INSTANCE_T *pDlr,                       /**< DLR instance */
    GOAL_ETH_PORT_T port                        /**< Tx port */
);

GOAL_STATUS_T DLR_NeighborCheckResTx(
    DLR_INSTANCE_T *pDlr,                       /**< DLR instance */
    GOAL_ETH_PORT_T port                        /**< Tx port */
);

GOAL_STATUS_T DLR_LerningUpdateTx(
    DLR_INSTANCE_T *pDlr                        /**< DLR instance */
);

#endif /* DLR_INTERNAL_H */
