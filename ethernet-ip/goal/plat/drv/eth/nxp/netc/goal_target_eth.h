/** @file
 *
 * @brief NETC Ethernet driver
 *
 * @copyright
 * Copyright 2010-2022 port GmbH.
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

#ifndef GOAL_TARGET_ETH_NETC_H
#define GOAL_TARGET_ETH_NETC_H

#include "fsl_netc_endpoint.h"
#include "fsl_netc_switch.h"
#include <fsl_netc.h>


/****************************************************************************/
/* Configuration */
/****************************************************************************/
#ifndef GOAL_CONFIG_DRV_ETH_NETC_IPFILTER
# define GOAL_CONFIG_DRV_ETH_NETC_IPFILTER 1    /**< Enable filtering using IPF feature of NETC */
#endif

#ifndef GOAL_CONFIG_DRV_ETH_NETC_IPFILTER_ENTRIES
# define GOAL_CONFIG_DRV_ETH_NETC_IPFILTER_ENTRIES 10 /**< number of managable IPF entries */
#endif

#define GOAL_CONFIG_DRV_ETH_NETC_IPFILTER_PRECEDENSE 100 /**< default precedence for IPF entries */


/****************************************************************************/
/* Public defines */
/****************************************************************************/
#ifndef GOAL_DRV_ETH_TC_MAX
# define GOAL_DRV_ETH_TC_MAX                  4 /**< number of traffic classes and queues */
#endif

#define GOAL_DRV_ETH_DESC_NUM                16 /**< number of descriptors per ring */
#define GOAL_DRV_ETH_ALIGN_DESC             128 /**< alignment for descriptors: 128 Bytes */
#define GOAL_DRV_ETH_ALIGN_BUF               64 /**< alignment for buffers: 64 Bytes */
#define GOAL_DRV_ETH_BUF_SIZE              1536 /**< buffer size that is a multiple of 64 Bytes */
#define GOAL_DRV_ETH_BUF_NUM                  8 /**< number of buffers */
#define GOAL_DRV_ETH_TSLT_NUM                 8 /**< number of timeslot configs */
#define GOAL_DRV_ETH_TXTS_NUM                 5 /**< number of Tx Timestamp store entries */

#define GOAL_DRV_ETH_ETYPE_PRIO_NUM_MAX       4 /**< number of ethertype priority mappings */

/* Beacon Timer is handled by internal state machine */
#define goal_targetDLRBeaconTimerReset(...)
#define goal_targetDLRBeaconTimerStop(...)
#define goal_targetDLRBeaconTimerStart(...)

#define GOAL_DRV_ETH_TASK_NAME_MAX          12  /* Task name buffer size */


/****************************************************************************/
/* Public data types */
/****************************************************************************/
typedef uint8_t GOAL_DRV_ETH_RX_BUF_T[GOAL_DRV_ETH_BUF_SIZE]; /**< Rx buffer type */

/** port information for NETC Ethernet driver */
typedef struct GOAL_DRV_ETH_NETC_PORT_INFO_T {
    netc_hw_eth_port_idx_t portId;              /**< port ID */
    netc_hw_mii_mode_t miiType;                 /**< MII Type */
    uint8_t phyAddr;                            /**< Phy Address */
    uint32_t maxSpeed;                          /**< maximum link speed */
    void *pPhyResetGpioBase;                    /**< Phy Reset Pin GPIO bank */
    uint32_t phyResetPin;                       /**< Phy Reset Pin ID */
} GOAL_DRV_ETH_NETC_PORT_INFO_T;

/** buffer and descriptor info */
typedef struct {
    uint32_t numTc;                             /**< number of traffic classes */
    netc_rx_bd_t *rxBuffDesc[GOAL_DRV_ETH_TC_MAX]; /**< rx buffer descriptors */
    netc_tx_bd_t *txBuffDesc[GOAL_DRV_ETH_TC_MAX]; /**< tx buffer descriptors */
    GOAL_DRV_ETH_RX_BUF_T *rxBuffers[GOAL_DRV_ETH_TC_MAX]; /**< rx buffers */
    netc_tx_bd_t *swTxBuffDesc;                 /**< Switch tx buffer descriptors */
    netc_cmd_bd_t *switchCmdDesc;               /**< Switch command buffer descriptors */
    netc_cmd_bd_t *epCmdDesc;                   /**< Endpoint command buffer descriptors */
} GOAL_DRV_ETH_NETC_BUFDESC_INFO_T;

#if GOAL_PTP == 1
/** Tx timestamp store data */
typedef struct {
    uint32_t txPort;                            /**< ID of egress port (UINT32_MAX means unused) */
    uint16_t txTsId;                            /**< internal Timestamp ID */
    uint32_t tsVal;                             /**< timestamp value */
} GOAL_DRV_ETH_NETC_TX_TS_DATAT_T;
#endif /* GOAL_PTP */

/** private managing structure for IPF entries */
typedef struct {
    uint32_t ipfEntryId;
    GOAL_ETH_MAC_ADDR_T addr;
} GOAL_DRV_ETH_NETC_IPF_ENTRY_T;

#if GOAL_DLR == 1
/** forward declaration of DLR driver data */
struct GOAL_DRV_DLR_DATA_T;
#endif /* GOAL_DLR */

/** private data of NETC Ethernet driver */
typedef struct {
    uint32_t numPorts;                          /**< number of active external ports */
    uint32_t numTc;                             /**< number of traffic classes */
    GOAL_DRV_ETH_NETC_PORT_INFO_T *pPortInfo;   /**< port info data table */
    GOAL_ETH_IFACE_T ifaceSwitch;               /**< Switch interface handle */
    GOAL_ETH_IFACE_T *pIfacePhys;               /**< PHY interface handles */
    GOAL_BOOL_T hasSwitch;                      /**< controlled interface has a Switch */
    netc_rx_bd_t *rxBuffDesc[GOAL_DRV_ETH_TC_MAX]; /**< rx buffer descriptors (will be allocated) */
    netc_tx_bd_t *txBuffDesc[GOAL_DRV_ETH_TC_MAX]; /**< tx buffer descriptors (will be allocated) */
    GOAL_DRV_ETH_RX_BUF_T *rxBuffers[GOAL_DRV_ETH_TC_MAX]; /**< rx buffers (will be allocated) */
    netc_tx_frame_info_t **txFrameInfo;         /**< tx frame meta data (will be allocated) */
    uint64_t **rxBuffAddrArray;                 /**< rx buffer address array (will be allocated) */
    netc_rx_bdr_config_t *rxBdrConfig;          /**< rx buffer descriptor config (will be allocated) */
    netc_tx_bdr_config_t *txBdrConfig;          /**< tx buffer descriptor config (will be allocated) */
    netc_tx_bd_t *swTxBuffDesc;                 /**< Switch tx buffer descriptors (will be allocated) */
    netc_tx_frame_info_t *swTxFrameInfo;        /**< Switch tx frame meta data (will be allocated) */
    netc_cmd_bd_t *switchCmdDesc;               /**< Switch command buffer descriptors */
    netc_cmd_bd_t *epCmdDesc;                   /**< Endpoint command buffer descriptors */
    ep_handle_t epHandle;                       /**< endpoint driver handle */
    swt_config_t swConfig;                      /**< Switch configuration */
    swt_handle_t swHandle;                      /**< Switch driver handle */
    swt_transfer_config_t swRxTxConfig;         /**< Switch Transfer config */
    uint8_t rxRing;                             /**< ID of Ring that contains a new frame */
#if GOAL_CONFIG_TASK == 1
    GOAL_LOCK_T *pTxStatusLock;                 /**< synchronization lock for tx status */
#endif /* GOAL_CONFIG_TASK */
    GOAL_BUFFER_T *pRxBuf;                      /**< receive buffer */
    uint32_t ipfEntryBroadcast;                 /**< entry id in IPF table for broadcast filter */
    uint32_t ipfEntryUnicast;                   /**< entry id in IPF table for unicast filter */
#if GOAL_CONFIG_DRV_ETH_NETC_IPFILTER == 1
    GOAL_DRV_ETH_NETC_IPF_ENTRY_T ipfEntries[GOAL_CONFIG_DRV_ETH_NETC_IPFILTER_ENTRIES]; /**< manage IPF entries */
#endif
#if GOAL_CONFIG_TASK == 1
    GOAL_TASK_T *pTskEthRecv;                   /**< Rx Task handle */
    GOAL_TASK_T *pTskEthSend;                   /**< Tx Task handle */
    GOAL_MBOX_T *pMbxRxIrq;                     /**< Rx IRQ Mailbox */
    char rxTaskName[GOAL_DRV_ETH_TASK_NAME_MAX]; /**< Rx task name buffer */
    char txTaskName[GOAL_DRV_ETH_TASK_NAME_MAX]; /**< Tx task name buffer */
#endif /* GOAL_CONFIG_TASK */
#if GOAL_PTP == 1
    GOAL_DRV_ETH_NETC_TX_TS_DATAT_T txTsStore[GOAL_DRV_ETH_TXTS_NUM]; /**< Tx timestamp store */
#endif /* GOAL_PTP */
    GOAL_ETH_QOS_MODE_T qosMode;                /**< QoS settings */
    uint8_t **vlanPrioMap;                      /**< VLAN priority mapping */
    GOAL_ETH_QOS_PRIO_TYPE_T etypePrioMap[GOAL_DRV_ETH_ETYPE_PRIO_NUM_MAX]; /**< EtherType priority mapping */
#if GOAL_CONFIG_TDMA == 1
    GOAL_ETH_TDMA_CONFIG_T tdmaConfig;          /**< TDMA schedule config */
    GOAL_ETH_TDMA_TSLT_CFG_T tdmaTsltCfg[GOAL_DRV_ETH_TSLT_NUM]; /**< TDMA timeslot config */
    uint32_t maxValidTslt;                      /**< maximum valid timeslot config entry */
    GOAL_BOOL_T tdmaEnabled;                    /**< TDMA scheduler is enabled */
    netc_tgs_gate_entry_t tsltCfg[GOAL_DRV_ETH_TSLT_NUM]; /**< SDK timeslot configuration */
#endif /* GOAL_CONFIG_TDMA */
#if GOAL_DLR == 1
    struct GOAL_DRV_DLR_DATA_T *pDlr;           /**< DLR instance data */
#endif /* GOAL_DLR */
} GOAL_DRV_ETH_NETC_DATA_T;


/****************************************************************************/
/* Public functions */
/****************************************************************************/
GOAL_STATUS_T goal_drvEthNetcReg(
    uint32_t idMa,                              /**< id ma */
    GOAL_DRV_ETH_NETC_PORT_INFO_T *pPortInfo,   /**< port info data table */
    uint32_t numEntries,                        /**< entries in port info data table */
    GOAL_DRV_ETH_NETC_BUFDESC_INFO_T *pBufDescInfo /**< buffers and descriptors */
);

GOAL_STATUS_T goal_drvEthNetcShutdown(
    GOAL_MA_ETH_T *pMaHdl                       /**< MA Hdl */
);

#endif /* GOAL_TARGET_ETH_NETC_H */
