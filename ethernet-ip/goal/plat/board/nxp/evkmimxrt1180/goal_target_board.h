/** @file
 *
 * @brief Board File for NXP RT1180 evkmimxrt1180
 *
 * @details
 *
 *
 * @copyright
 * Copyright 2022.
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

#ifndef GOAL_TARGET_BOARD_NXP_RT1180_H
#define GOAL_TARGET_BOARD_NXP_RT1180_H

#if GOAL_CONFIG_ETHERNET == 1
# include <plat/drv/eth/nxp/netc/goal_target_eth.h>
#endif /* GOAL_CONFIG_ETHERNET */


/****************************************************************************/
/* definitions */
/****************************************************************************/
/* NVS definitions */
#define GOAL_DRV_NVS_SFLASH_FSL_FLEXSPI 1       /**< select driver version */
#define GOAL_DRV_NVS_SFLASH_FSL_FLEXSPI_DEDICATED_DATA_FLASH 1 /**< use second flash port as only data flash */
#define GOAL_ID_MI_NVS_REGION_CMCONFIG 0        /**< region ID of CM variables */
#define GOAL_TGT_NVS_REGION_CMCONFIG_LENGTH 0x2000 /**< size of CM config (8kB) */
#define GOAL_TGT_BOARD_SFLASH_ADDR 0x28000000   /**< address offset to memory mapped SFLASH */

/* FreeRTOS settings */
//#define GOAL_TARGET_FREERTOS_FAST_BASETICK 1    /**< use base tick of less than 1ms */

#if GOAL_CONFIG_TASK == 0

# ifndef GOAL_CONFIG_HEAP_SIZE
#   define GOAL_CONFIG_HEAP_SIZE ((256 * 1024) - 128) /**< target HEAP size */
# endif

# ifndef GOAL_LM_BUFFER_SIZE
#   define GOAL_LM_BUFFER_SIZE (15 * 1024)       /**< target log buffer size */
# endif

#endif

#if GOAL_CONFIG_ETHERNET == 1
/* Ethernet settings */
# ifndef GOAL_TARGET_ETH_PORT_COUNT
#  define GOAL_TARGET_ETH_PORT_COUNT 1
# endif

/* Ethernet queue sizes */
# ifndef GOAL_ETH_RX_LOW_QUEUE_NUM
#  define GOAL_ETH_RX_LOW_QUEUE_NUM    4
# endif

# ifndef GOAL_ETH_TX_LOW_QUEUE_NUM
#  define GOAL_ETH_TX_LOW_QUEUE_NUM    4
# endif

# ifndef GOAL_ETH_TX_HIGH_QUEUE_NUM
#  define GOAL_ETH_TX_HIGH_QUEUE_NUM   2
# endif
#endif /* GOAL_CONFIG_ETHERNET */

/* PTP settings */
#define GOAL_TGT_BOARD_LATENCY_EGRESS 0         /**< PTP: egress timestamping latency */
#define GOAL_TGT_BOARD_LATENCY_INGRESS 0        /**< PTP: ingress timestamping latency */

/* Ethernet POWERLINK */
#if GOAL_CONFIG_EPL == 1

#  ifndef GOAL_CONFIG_EPL_EXTERN_BUF
#    define GOAL_CONFIG_EPL_EXTERN_BUF 1
#  endif

#  if GOAL_CONFIG_EPL_EXTERN_BUF == 1
#    define PL_CONFIG_EXTERN_IDENT_BUF      1       /**< external ident buffer */
#    define PL_CONFIG_EXTERN_PRES_BUF       1       /**< external pres buffer */
#    define PL_CONFIG_EXTERN_STATUS_BUF     1       /**< external status buffer */
#    define PL_CONFIG_EXTERN_UNSPEC_BUF     1       /**< external unspec buffer */
#    define PL_CONFIG_IDENT_AUTOREPLY       1       /**< auto reply to ident */
#    define PL_CONFIG_PREQ_AUTOREPLY        1       /**< auto reply to preq */
#    define PL_CONFIG_STATUS_AUTOREPLY      1       /**< auto reply to status */
#    define PL_CONFIG_UNSPEC_AUTOREPLY      1       /**< auto reply to unspec */
#  endif /* GOAL_CONFIG_EPL_EXTERN_BUF == 1 */

# if GOAL_CONFIG_EPL == 1
#  if GOAL_CONFIG_GEN_CTC_AC == 0
#    include <pl_inc.h>
#  endif
# endif
#endif /* GOAL_CONFIG_EPL == 1 */

#ifndef GOAL_CONFIG_TGT_RT1180_TRACE
# define GOAL_CONFIG_TGT_RT1180_TRACE 0
#endif

/* CC-Link IE TSN settings */
#define CYC_NUM_CORRECTION_FACTOR_ENABLED     0 /**< CCLIETSN: disable correction factor of cycle number */

/* DLR settings */
#define DLR_CONFIG_NEW_STATE_DELAYED          1 /**< NEW_STATE event might be delayed due to hardware emulation */


/****************************************************************************/
/* Forward Declarations */
/****************************************************************************/
#if GOAL_CONFIG_ETHERNET == 1
struct GOAL_DRV_ETH_NETC_PORT_INFO_T;
#endif /* GOAL_CONFIG_ETHERNET */


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_targetBoardInit(
    void
);

#if GOAL_CONFIG_ETHERNET == 1
GOAL_STATUS_T goal_targetBoardEthInit(
    GOAL_DRV_ETH_NETC_PORT_INFO_T *pPortInfo,   /**< port info data table */
    uint32_t numEntries                         /**< entries in port info data table */
);

GOAL_STATUS_T goal_targetBoardEthPostInit(
    GOAL_MA_ETH_T *pMaEth,                      /**< Ethernet MA handle */
    GOAL_DRV_ETH_NETC_PORT_INFO_T *pPortInfo,   /**< port info data table */
    uint32_t numEntries,                        /**< entries in port info data table */
    GOAL_ETH_IFACE_T *pIfaceSwitch,             /**< Switch interface handle */
    GOAL_ETH_IFACE_T *pIfacePhys,               /**< PHY interface handles */
    void *pSwitchHandle                         /**< target switch handle */
);
#endif /* GOAL_CONFIG_ETHERNET */

#if GOAL_CONFIG_TGT_RT1180_TRACE == 1
void goal_tgtTraceInit(
    void
);

void goal_tgtTaskTrace(
    unsigned int enterOrLeave,                  /**< 1: switched on, 0: switched off */
    unsigned int taskId                         /**< task id */
);
#endif

#endif /* GOAL_TARGET_BOARD_NXP_RT1180_H */
