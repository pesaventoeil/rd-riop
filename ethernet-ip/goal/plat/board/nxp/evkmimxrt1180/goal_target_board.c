/** @file
 *
 * @brief GOAL Board specific implementation
 *
 * @copyright
 * Copyright 2010-2021.
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

#define GOAL_ID GOAL_ID_TGT

#include <goal_config.h>
#include <goal_includes.h>
#if GOAL_CONFIG_MEDIA_MI_NVS == 1
#  include <nvs_sflash_fsl.h>
#  include <goal_media/goal_mi_nvs.h>
#endif

#include <board.h>
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_silicon_id.h"
#include "pin_mux.h"
#include "fsl_iomuxc.h"
#include "clock_config.h"

#if GOAL_CONFIG_TARGET_FREERTOS_TARGET_MAIN == 1
# include "goal_target_freertos.h"
#endif /* GOAL_CONFIG_TARGET_FREERTOS_TARGET_MAIN */

#if GOAL_CONFIG_LOGGING_TARGET_RAW == 1
# include "fsl_debug_console.h"
#endif /* GOAL_CONFIG_LOGGING_TARGET_RAW */

#if GOAL_CONFIG_ETHERNET == 1
# include <plat/drv/eth/nxp/netc/goal_target_eth.h>
#if GOAL_CONFIG_TCPIP_STACK == 1
# include <plat/drv/net/lwip/goal_target_net.h>
#endif /* GOAL_CONFIG_TCPIP_STACK */
# include <plat/drv/bus/mdio/netc/mdio.h>
#if RIOP && GOAL_CONFIG_MCTC_CC == 1
GOAL_STATUS_T goal_targetPhyVsc8541Open(
    GOAL_ETH_IFACE_T *pIface                    /**< interface data */
);
#ifndef PHY_PAGE_SELECT_REG
#define PHY_PAGE_SELECT_REG   0x1FU /*!< The PHY page select register. */
#endif
# include "fsl_netc_ierb.h"
#else
# include <plat/drv/phy/realtek_rtl8201/phy_rtl8201.h>
# include <plat/drv/phy/realtek_rtl8211/phy_rtl8211.h>
#endif
# include <plat/drv/switch/nxp_netc/nxp_netc.h>
# if GOAL_CONFIG_ECAT == 1
#  include <plat/drv/eoe/goal_drv_eoe.h>
# endif
#endif

#if GOAL_CONFIG_TASK == 1
# include "FreeRTOSConfig.h"
# include "task.h"
# include "timers.h"
#endif /* GOAL_CONFIG_TASK  */

#if GOAL_CONFIG_MEDIA_MA_UART == 1
# include <plat/drv/uart/rt1180/uart_rt1180.h>
#endif /* GOAL_CONFIG_MEDIA_MA_UART */

#if GOAL_CONFIG_TLS == 1
# include <goal_media/goal_ma_tls.h>
# if (1 == GOAL_CONFIG_TLS_WOLFSSL)
#  include <plat/drv/tls/wolfssl/goal_drv_tls_wolfssl.h>
# endif /* GOAL_MA_TLS_WOLFSSL */
# if (1 == GOAL_CONFIG_TLS_MBEDTLS)
#  include <plat/drv/tls/mbedtls/goal_drv_tls_mbedtls.h>
# endif /* GOAL_CONFIG_TLS_MBEDTLS */
#endif /* GOAL_CONFIG_TLS == 1 */

#if GOAL_CONFIG_MCTC == 1
# include <goal_media/goal_mi_mctc_ram.h>
# include <mcmgr.h>
# include <fsl_sema42.h>
# include <fsl_mu.h>
#endif


/****************************************************************************/
/* Local defines */
/****************************************************************************/
#define GOAL_TARGET_LED1    (1 << 0)            /**< user LED bit */
#define GOAL_TARGET_LED2    (1 << 1)            /**< user LED bit */

#define GOAL_TARGET_BUTTON  (1 << 0)            /**< user button bit */

/* BSP supports only one LED, define another one here */
#define BOARD_USER_LED_GPIO_PINR (26U)          /**< red user LED */

/* supported Ethernet ports (enabled: 1, disabled: 0) */
#if RIOP
#define GOAL_BOARD_ETH_PORT_0                 0 /**< Ethernet port ENET0: Switch Port 0 (J28) */
#define GOAL_BOARD_ETH_PORT_1                 0 /**< Ethernet port ENET1: Switch Port 1 (J29) */
#define GOAL_BOARD_ETH_PORT_2                 0 /**< Ethernet port ENET2: Switch Port 2 (J30, not connected by default) */
#define GOAL_BOARD_ETH_PORT_3                 0 /**< Ethernet port ENET3: Switch Port 3 (J31, not connected by default) */
#define GOAL_BOARD_ETH_PORT_4                 1 /**< Ethernet port ENET4: MAC Controller (J32) */
#else
#define GOAL_BOARD_ETH_PORT_0                 0 /**< Ethernet port ENET0: Switch Port 0 (J28) */
#define GOAL_BOARD_ETH_PORT_1                 0 /**< Ethernet port ENET1: Switch Port 1 (J29) */
#define GOAL_BOARD_ETH_PORT_2                 1 /**< Ethernet port ENET2: Switch Port 2 (J30, not connected by default) */
#define GOAL_BOARD_ETH_PORT_3                 1 /**< Ethernet port ENET3: Switch Port 3 (J31, not connected by default) */
#define GOAL_BOARD_ETH_PORT_4                 0 /**< Ethernet port ENET4: MAC Controller (J32) */
#endif

#if GOAL_CONFIG_TLS == 1
# if (1 == GOAL_CONFIG_TLS_WOLFSSL)
#  define GOAL_TGT_MA_TLS_ID_WOLFSSL GOAL_MA_TLS_ID_DEFAULT /**< TLS MA ID for WolfSSL */
#  if (1 == GOAL_CONFIG_TLS_MBEDTLS)
#   define GOAL_TGT_MA_TLS_ID_MBEDTLS 1         /**< TLS MA ID for Mbed TLS */
#  endif /* GOAL_CONFIG_TLS_MBEDTLS */
# elif (1 == GOAL_CONFIG_TLS_MBEDTLS)
#  define GOAL_TGT_MA_TLS_ID_MBEDTLS GOAL_MA_TLS_ID_DEFAULT /**< TLS MA ID for Mbed TLS */
# endif /* GOAL_CONFIG_TLS_MBEDTLS */
#endif /* GOAL_CONFIG_TLS == 1 */

#define GOAL_BOARD_ETH_IF1_ID                 0 /**< ID of Ethernet interface 1 (should always be 0 for compatibility) */
#define GOAL_BOARD_ETH_IF2_ID                 1 /**< ID of Ethernet interface 2 */
#define GOAL_BOARD_ETH_IF1_NUM_TC             4 /**< number of traffic classes for Ethernet interface 1 */
#define GOAL_BOARD_ETH_IF2_NUM_TC             1 /**< number of traffic classes for Ethernet interface 2 */

#if GOAL_CONFIG_MCTC == 1
/* synchronization modes for multicore */
# define GOAL_BOARD_MCMGR_SYNC (0)              /**< sync. start of both cores */

/* shared memory definitions */
# define GOAL_BOARD_SHM_SIZE (8192U)            /**< size of shmem */

unsigned char rpmsg_sh_mem[GOAL_BOARD_SHM_SIZE] __attribute__((section(".noinit.$rpmsg_sh_mem"))); /* linker region */
# define APP_SH_MEM_BASE (uint32_t) &rpmsg_sh_mem /**< linker region for shmem */

# define GOAL_BOARD_SHM_ADDR ((uint8_t *) APP_SH_MEM_BASE) /**< RAM address */
# define GOAL_BOARD_RAM_SIZE ((((GOAL_BOARD_SHM_SIZE / 2) - \
                            sizeof(GOAL_MI_MCTC_RAM_INFO_T)) / 3) - \
                            sizeof(GOAL_MI_MCTC_DPRAM_CYCLIC_T)) /**< RAM transfer size */

# define GOAL_BOARD_RPC_SIZE (1024U)            /**< RPC transfer size */
# define GOAL_BOARD_RPC_POS  (GOAL_BOARD_RAM_SIZE - GOAL_BOARD_RPC_SIZE - 1) /**< RPC partition position */
# define GOAL_BOARD_SHM_SEMAP_0 0               /**< hw semaphore for shmem */
# define GOAL_BOARD_SHM_SEMAP_1 1               /**< hw semaphore for shmem */

/* core-depending definitions */
# if GOAL_CONFIG_MCTC_AC == 1
#  define GOAL_BOARD_CORE_ID 9                  /**< core id for CM7 */
#  define GOAL_BOARD_DM_READ_DESC "Read from CC" /**< read description: from CC */
#  define GOAL_BOARD_DM_WRITE_DESC "Write to CC" /**< write description: from CC */
# else
#  define GOAL_BOARD_CORE_ID 8                  /**< core id for CM33 */
#  define GOAL_BOARD_DM_READ_DESC "Read from AC" /**< read description: from AC */
#  define GOAL_BOARD_DM_WRITE_DESC "Write to AC" /**< write description: from AC */
# endif

#endif /* GOAL_CONFIG_MCTC == 1 */


/****************************************************************************/
/* Plausibility check */
/****************************************************************************/
#if GOAL_CONFIG_EPL == 1

# if GOAL_CONFIG_ETHERNET == 0
#  error "POWERLINK requires Ethernet enabled (GOAL_CONFIG_ETHERNET)"
# endif

# if (GOAL_BOARD_ETH_PORT_4 == 1)
 # error "POWERLINK does not work work on the endpoint port"
# endif

# if (GOAL_BOARD_ETH_PORT_0 == 0) && (GOAL_BOARD_ETH_PORT_1 == 0) && (GOAL_BOARD_ETH_PORT_2 == 0) && (GOAL_BOARD_ETH_PORT_3 == 0)
#  error "POWERLINK requires at least one switch port enabled"
# endif

#endif

#if GOAL_CONFIG_NUM_INTERFACES > 2
# error "maximum supported value of GOAL_CONFIG_NUM_INTERFACES is 2"
#endif /* GOAL_CONFIG_NUM_INTERFACES */

#if GOAL_CONFIG_NUM_INTERFACES == 2
# if (GOAL_BOARD_ETH_PORT_0 == 0) && (GOAL_BOARD_ETH_PORT_1 == 0) && (GOAL_BOARD_ETH_PORT_2 == 0) && (GOAL_BOARD_ETH_PORT_3 == 0)
#  error "At least one switch port must be enabled"
# endif
#endif /* GOAL_CONFIG_NUM_INTERFACES */


/****************************************************************************/
/* Local variables */
/****************************************************************************/
#if GOAL_CONFIG_MEDIA_MI_NVS == 1
static GOAL_MA_NVS_INFO_T mInfoNvs = {          /**< NVS chip description */
#if RIOP
    GOAL_DRV_NVS_CHIP_MACRONIX_MX25UW6345GXDI00
#else
    GOAL_DRV_NVS_CHIP_WINBOND_W25Q128JWSIQ
#endif
};
static GOAL_MI_NVS_REGION_T *pNvsRegionCm = NULL; /**< NVS region handle */
static GOAL_MI_NVS_REGION_LIST_T region_list[] = { /**< NVS region list */
    {
        /* The elements .posStart and .mode are set in goal_targetNvsInit() */
        .length = GOAL_TGT_NVS_REGION_CMCONFIG_LENGTH, /* length of region */
        .strName = "goal_cc_cm_nvs",            /* name of region */
        .id = GOAL_ID_MI_NVS_REGION_CMCONFIG,   /* region type ID */
        .access = GOAL_MI_NVS_REGION_ACCESS_WRITE /* region access rights */
    }
};
#endif /* GOAL_CONFIG_MEDIA_MI_NVS == 1 */

#if GOAL_CONFIG_MCTC == 1
static GOAL_MI_MCTC_INST_T *pInstMctc = NULL;   /**< MI Mctc RAM instance */
static GOAL_MI_DM_T *mpMiDmRead;                /**< MI DM Read handle */
static GOAL_MI_DM_T *mpMiDmWrite;               /**< MI DM Write handle */
static GOAL_MI_DM_PART_T mMiDmRpcRead;          /**< MI DM RPC Read partition */
static GOAL_MI_DM_PART_T mMiDmRpcWrite;         /**< MI DM RPC Write partition */
static GOAL_STAGE_HANDLER_T mStageCtcPre;       /**< stage CTC pre handle */
static volatile uint16_t mMulticoreEventData;   /**< multicore event data */
#endif

#define EXAMPLE_NETC_LINK_PORT 4
#define NETC_MII_SPEED kNETC_MiiSpeed1000M
#define NETC_MII_MODE kNETC_RgmiiMode
#define EXAMPLE_MII_MODE       NETC_MII_MODE
#define EXAMPLE_PHY_ADDRESS    BOARD_EP0_PHY_ADDR

/****************************************************************************/
/* Ethernet driver data */
/****************************************************************************/
#if GOAL_CONFIG_ETHERNET == 1
static GOAL_BOOL_T mEthInitFlag = GOAL_FALSE;   /**< Ethernet initialization flag */

/** Ethernet Port Configuration */
static GOAL_DRV_ETH_NETC_PORT_INFO_T ethPortInfo[] = {

#if GOAL_BOARD_ETH_PORT_0 == 1
    {
        .portId = kNETC_SWITCH0EthPort0,
        .miiType = kNETC_RmiiMode,
        .phyAddr = 2,
        .maxSpeed = GOAL_ETH_SPEED_100,
        .pPhyResetGpioBase = RGPIO4,
        .phyResetPin = 25,
    },
#endif /* GOAL_BOARD_ETH_PORT_0 */
#if GOAL_BOARD_ETH_PORT_1 == 1
    {
        .portId = kNETC_SWITCH0EthPort1,
        .miiType = kNETC_RgmiiMode,
        .phyAddr = 5,
        .maxSpeed = GOAL_ETH_SPEED_1000,
        .pPhyResetGpioBase = RGPIO6,
        .phyResetPin = 13,
    },
#endif /* GOAL_BOARD_ETH_PORT_1 */
#if GOAL_BOARD_ETH_PORT_2 == 1
    {
        .portId = kNETC_SWITCH0EthPort2,
        .miiType = kNETC_RgmiiMode,
        .phyAddr = 4,
        .maxSpeed = GOAL_ETH_SPEED_1000,
        .pPhyResetGpioBase = RGPIO4,
        .phyResetPin = 28,
    },
#endif /* GOAL_BOARD_ETH_PORT_2 */
#if GOAL_BOARD_ETH_PORT_3 == 1
    {
        .portId = kNETC_SWITCH0EthPort3,
        .miiType = kNETC_RgmiiMode,
        .phyAddr = 7,
        .maxSpeed = GOAL_ETH_SPEED_1000,
        .pPhyResetGpioBase = RGPIO6,
        .phyResetPin = 15,
    },
#endif /* GOAL_BOARD_ETH_PORT_3 */
#if GOAL_BOARD_ETH_PORT_4 == 1 && GOAL_CONFIG_NUM_INTERFACES == 1
    {
#if RIOP
        .portId = kNETC_ENETC0EthPort,
        .miiType = NETC_MII_MODE,
        .phyAddr = 3,
        .maxSpeed = GOAL_ETH_SPEED_1000,
        .pPhyResetGpioBase = RGPIO3,
        .phyResetPin = 13,
#else
        .portId = kNETC_ENETC0EthPort,
        .miiType = kNETC_RmiiMode,
        .phyAddr = 3,
        .maxSpeed = GOAL_ETH_SPEED_100,
        .pPhyResetGpioBase = RGPIO4,
        .phyResetPin = 13,
#endif	
    },
#endif /* GOAL_BOARD_ETH_PORT_4  && GOAL_CONFIG_NUM_INTERFACES == 1 */
};

#if GOAL_CONFIG_NUM_INTERFACES == 2
static GOAL_DRV_ETH_NETC_PORT_INFO_T ethPortInfo2 = {
    .portId = kNETC_ENETC0EthPort,
    .miiType = kNETC_RmiiMode,
    .phyAddr = 3,
    .maxSpeed = GOAL_ETH_SPEED_100,
    .pPhyResetGpioBase = RGPIO4,
    .phyResetPin = 13,
};
#endif /* GOAL_CONFIG_NUM_INTERFACES == 2 */


AT_NONCACHEABLE_SECTION_ALIGN(static netc_rx_bd_t rxBuffDescrip[GOAL_BOARD_ETH_IF1_NUM_TC][GOAL_DRV_ETH_DESC_NUM], GOAL_DRV_ETH_ALIGN_DESC);
AT_NONCACHEABLE_SECTION_ALIGN(static netc_tx_bd_t txBuffDescrip[GOAL_BOARD_ETH_IF1_NUM_TC][GOAL_DRV_ETH_DESC_NUM], GOAL_DRV_ETH_ALIGN_DESC);
AT_NONCACHEABLE_SECTION_ALIGN(static GOAL_DRV_ETH_RX_BUF_T rxDataBuff[GOAL_BOARD_ETH_IF1_NUM_TC][GOAL_DRV_ETH_BUF_NUM], GOAL_DRV_ETH_ALIGN_BUF);
AT_NONCACHEABLE_SECTION_ALIGN(static netc_cmd_bd_t epCmdDescrip[GOAL_DRV_ETH_DESC_NUM], GOAL_DRV_ETH_ALIGN_DESC);
#if (GOAL_BOARD_ETH_PORT_0 == 1) || (GOAL_BOARD_ETH_PORT_1 == 1) || (GOAL_BOARD_ETH_PORT_2 == 1) || (GOAL_BOARD_ETH_PORT_3 == 1)
AT_NONCACHEABLE_SECTION_ALIGN(static netc_tx_bd_t swTxBuffDescrip[GOAL_DRV_ETH_DESC_NUM], GOAL_DRV_ETH_ALIGN_DESC);
AT_NONCACHEABLE_SECTION_ALIGN(static netc_cmd_bd_t swCmdDescrip[GOAL_DRV_ETH_BUF_NUM], GOAL_DRV_ETH_ALIGN_DESC);
#endif /* (GOAL_BOARD_ETH_PORT_0 == 1) || (GOAL_BOARD_ETH_PORT_1 == 1) || (GOAL_BOARD_ETH_PORT_2 == 1) || (GOAL_BOARD_ETH_PORT_3 == 1) */
#if GOAL_CONFIG_NUM_INTERFACES == 2
AT_NONCACHEABLE_SECTION_ALIGN(static netc_rx_bd_t rxBuffDescrip2[GOAL_BOARD_ETH_IF2_NUM_TC][GOAL_DRV_ETH_DESC_NUM], GOAL_DRV_ETH_ALIGN_DESC);
AT_NONCACHEABLE_SECTION_ALIGN(static netc_tx_bd_t txBuffDescrip2[GOAL_BOARD_ETH_IF2_NUM_TC][GOAL_DRV_ETH_DESC_NUM], GOAL_DRV_ETH_ALIGN_DESC);
AT_NONCACHEABLE_SECTION_ALIGN(static GOAL_DRV_ETH_RX_BUF_T rxDataBuff2[GOAL_BOARD_ETH_IF2_NUM_TC][GOAL_DRV_ETH_BUF_NUM], GOAL_DRV_ETH_ALIGN_BUF);
AT_NONCACHEABLE_SECTION_ALIGN(static netc_cmd_bd_t epCmdDescrip2[GOAL_DRV_ETH_DESC_NUM], GOAL_DRV_ETH_ALIGN_DESC);
#endif /* GOAL_CONFIG_NUM_INTERFACES == 2 */
static GOAL_DRV_ETH_NETC_BUFDESC_INFO_T bufDescInfo; /**< buffers and descriptors */
#endif /* GOAL_CONFIG_ETHERNET */


/****************************************************************************/
/* FreeRTOS tasks mapping table */
/****************************************************************************/
#if GOAL_CONFIG_TASK == 1
#define GOAL_TGT_TASK_SIZE_MAIN            9192 /**< size of static FreeRTOS task: Main Tasks */
#define GOAL_TGT_TASK_SIZE_ETH_RX          1024 /**< size of static FreeRTOS task: ETH Rx */
#define GOAL_TGT_TASK_SIZE_ETH_TX          1024 /**< size of static FreeRTOS task: ETH Tx */
#define GOAL_TGT_TASK_SIZE_PTP             2048 /**< size of static FreeRTOS task: PTP */
#define GOAL_TGT_TASK_SIZE_CANOPEN         2048 /**< size of static FreeRTOS task: CANopen */
#define GOAL_TGT_TASK_SIZE_ECAT            2048 /**< size of static FreeRTOS task: EtherCAT */
#define GOAL_TGT_TASK_SIZE_TLS       (8 * 1024) /**< size of static FreeRTOS task: TLS */

AT_QUICKACCESS_SECTION_DATA(static uint8_t stackMain[GOAL_TGT_TASK_SIZE_MAIN]); /**< static stack for FreeRTOS task: Main */
#if GOAL_CONFIG_ETHERNET == 1
AT_QUICKACCESS_SECTION_DATA(static uint8_t stackEthRx[GOAL_TGT_TASK_SIZE_ETH_RX]); /**< static stack for FreeRTOS task: ETH Rx 0 */
AT_QUICKACCESS_SECTION_DATA(static uint8_t stackEthTx[GOAL_TGT_TASK_SIZE_ETH_TX]); /**< static stack for FreeRTOS task: ETH Tx 0 */
# if GOAL_CONFIG_NUM_INTERFACES == 2
AT_QUICKACCESS_SECTION_DATA(static uint8_t stackEthRx2[GOAL_TGT_TASK_SIZE_ETH_RX]); /**< static stack for FreeRTOS task: ETH Rx 1 */
AT_QUICKACCESS_SECTION_DATA(static uint8_t stackEthTx2[GOAL_TGT_TASK_SIZE_ETH_TX]); /**< static stack for FreeRTOS task: ETH Tx 1 */
# endif /* GOAL_CONFIG_NUM_INTERFACES == 2 */
#endif /* GOAL_CONFIG_ETHERNET */
#if GOAL_PTP == 1
AT_QUICKACCESS_SECTION_DATA(static uint8_t stackPtp[GOAL_TGT_TASK_SIZE_PTP]);   /**< static stack for FreeRTOS task: PTP */
#endif /* GOAL_PTP */
#if GOAL_CANOPEN == 1
AT_QUICKACCESS_SECTION_DATA(static uint8_t stackCANopen[GOAL_TGT_TASK_SIZE_CANOPEN]);   /**< static stack for FreeRTOS task: CANopen */
#endif /* GOAL_CANOPEN */
#if GOAL_CONFIG_ECAT == 1 && defined(EC_CONFIG_PDI_INTERRUPT)
AT_QUICKACCESS_SECTION_DATA(static uint8_t stackEcatPdi[GOAL_TGT_TASK_SIZE_ECAT]); /**< static stack for FreeRTOS task: Ecat PDI */
# if defined(EC_CONFIG_DC)
AT_QUICKACCESS_SECTION_DATA(static uint8_t stackEcatDc[GOAL_TGT_TASK_SIZE_ECAT]); /**< static stack for FreeRTOS task: Ecat DC */
# endif
#endif
#if GOAL_CONFIG_TLS == 1
AT_QUICKACCESS_SECTION_DATA(static uint8_t stackTls[GOAL_TGT_TASK_SIZE_TLS]); /**< static stack for FreeRTOS task: TLS */
#endif /* GOAL_CONFIG_TLS */

/** static stack mapping table */
static GOAL_TGT_TASK_MAP_T taskMap[] = {
    {"GOAL_LOOP_TASK", {0}, GOAL_TGT_TASK_SIZE_MAIN, stackMain},
#if GOAL_CONFIG_ETHERNET == 1
    {"Eth RX 0", {0}, GOAL_TGT_TASK_SIZE_ETH_RX, stackEthRx},
    {"Eth TX 0", {0}, GOAL_TGT_TASK_SIZE_ETH_TX, stackEthTx},
# if GOAL_CONFIG_NUM_INTERFACES == 2
    {"Eth RX 1", {0}, GOAL_TGT_TASK_SIZE_ETH_RX, stackEthRx2},
    {"Eth TX 1", {0}, GOAL_TGT_TASK_SIZE_ETH_TX, stackEthTx2},
# endif /* GOAL_CONFIG_NUM_INTERFACES == 2 */
#endif /* GOAL_CONFIG_ETHERNET */
#if GOAL_PTP == 1
    {"PTP Task", {0}, GOAL_TGT_TASK_SIZE_PTP, stackPtp},
#endif /* GOAL_PTP */
#if GOAL_CANOPEN == 1
    {"CANopen Task", {0}, GOAL_TGT_TASK_SIZE_CANOPEN, stackCANopen},
#endif /* GOAL_CANOPEN */
#if GOAL_CONFIG_ECAT == 1 && defined(EC_CONFIG_PDI_INTERRUPT)
    {"EtherCAT PDI Task", {0}, GOAL_TGT_TASK_SIZE_ECAT, stackEcatPdi},
# if defined(EC_CONFIG_DC)
    {"EtherCAT DC Task", {0}, GOAL_TGT_TASK_SIZE_ECAT, stackEcatDc},
# endif
#endif
#if GOAL_CONFIG_TLS == 1
    {"TLS Task", {0}, GOAL_TGT_TASK_SIZE_TLS, stackTls},
#endif /* GOAL_CONFIG_TLS */
};

static StaticTask_t taskMemIdle;                /**< FreeRTOS idle task memory */
AT_QUICKACCESS_SECTION_DATA(static StackType_t taskStackIdle[configMINIMAL_STACK_SIZE]); /**< FreeRTOS idle task stack */
#if (GOAL_CONFIG_TARGET_FREERTOS_TIMER_SUPPORT == 1)
static StaticTask_t taskMemTimer;               /**< FreeRTOS idle task memory */
AT_QUICKACCESS_SECTION_DATA(static StackType_t taskStackTimer[configMINIMAL_STACK_SIZE]); /**< FreeRTOS idle task stack */
#endif /* (GOAL_CONFIG_TARGET_FREERTOS_TIMER_SUPPORT == 1) */
#endif /* GOAL_CONFIG_TASK */


/****************************************************************************/
/* local prototypes */
/****************************************************************************/
#if GOAL_CONFIG_MEDIA_MI_NVS == 1
static GOAL_STATUS_T goal_targetNvsInit(
    void
);
#endif

#if GOAL_CONFIG_MCTC == 1
static GOAL_STATUS_T goal_targetBoardMctcRamInit(
    void
);

static GOAL_STATUS_T goal_tgtDpramSemLock(
    GOAL_BOOL_T flgWr                           /**< lock write flag */
);

static GOAL_STATUS_T goal_tgtDpramSemUnlock(
    GOAL_BOOL_T flgWr                           /**< unlock write flag */
);

static GOAL_STATUS_T goal_targetBoardMcInit(
    void
);

# if GOAL_CONFIG_MCTC_CC == 1
static void goal_targetBoardMcEventHdl(
    mcmgr_core_t coreNum,                       /**< source core */
    uint16_t eventData,                         /**< event */
    void *pContext                              /**< user data */
);
# endif
#endif /* GOAL_CONFIG_MCTC == 1 */


/****************************************************************************/
/** Board init
 *
 * Low level board initialization.
 *
 * @retval GOAL_OK - success
 * @retval GOAL_ERR_BOARD_INIT - error initializing board
 */
GOAL_STATUS_T goal_targetBoardInit(
    void
)
{
    GOAL_STATUS_T res;                          /* result */
#if GOAL_CONFIG_ETHERNET == 1
    uint32_t cnt;                               /* loop counter */
    status_t ret;                               /* return value */
    GOAL_ETH_MAC_ADDR_T mac = { 0 };            /* MAC address */
#endif /* GOAL_CONFIG_ETHERNET */
#if GOAL_CONFIG_TLS == 1
    GOAL_MA_TLS_T *pTls = NULL;                 /* tls ma handle */
#endif /* GOAL_CONFIG_TLS */

#if GOAL_CONFIG_TASK == 1
    /* disable interrupts so systick does not run yet */
    portDISABLE_INTERRUPTS();
#endif

#if GOAL_CONFIG_TASK == 1
    res = goal_tgtTaskMapSet(taskMap, sizeof(taskMap)/sizeof(taskMap[0]));
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to register task mapping table");
        return res;
    }
#endif /* GOAL_CONFIG_TASK */

#if GOAL_CONFIG_TARGET_FREERTOS_TARGET_MAIN == 1
#else
#if RIOP /* Already done in External Main Function. */
    BOARD_InitHardware();
#else
    /* Hardware Initialization. */
    BOARD_ConfigMPU();
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();
# if GOAL_CONFIG_LOGGING == 1 && GOAL_CONFIG_LOGGING_TARGET_RAW == 1 && GOAL_CONFIG_MCTC_CC == 1
    BOARD_InitDEBUG_UARTPins();
# endif
#endif
#endif /* GOAL_CONFIG_TARGET_FREERTOS_TARGET_MAIN */

#if GOAL_CONFIG_TGT_RT1180_TRACE == 1
    goal_tgtTraceInit();
#endif

#if GOAL_CONFIG_MEDIA_MI_NVS == 1
    /* NVS initialization */
    res = goal_targetNvsInit();
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to initialize NVS");
        return res;
    }
#endif

#if GOAL_CONFIG_ETHERNET == 1
    GOAL_MEMSET(&bufDescInfo, 0, sizeof(GOAL_DRV_ETH_NETC_BUFDESC_INFO_T));
    bufDescInfo.numTc = GOAL_BOARD_ETH_IF1_NUM_TC;
    for (cnt = 0; cnt < GOAL_BOARD_ETH_IF1_NUM_TC; cnt++) {
        bufDescInfo.rxBuffDesc[cnt] = &rxBuffDescrip[cnt][0];
        bufDescInfo.txBuffDesc[cnt] = &txBuffDescrip[cnt][0];
        bufDescInfo.rxBuffers[cnt] = &rxDataBuff[cnt][0];
    }
    bufDescInfo.epCmdDesc = &epCmdDescrip[0];
#if (GOAL_BOARD_ETH_PORT_0 == 1) || (GOAL_BOARD_ETH_PORT_1 == 1) || (GOAL_BOARD_ETH_PORT_2 == 1) || (GOAL_BOARD_ETH_PORT_3 == 1)
    bufDescInfo.swTxBuffDesc = swTxBuffDescrip;
    bufDescInfo.switchCmdDesc = swCmdDescrip;
#endif /* (GOAL_BOARD_ETH_PORT_0 == 1) || (GOAL_BOARD_ETH_PORT_1 == 1) || (GOAL_BOARD_ETH_PORT_2 == 1) || (GOAL_BOARD_ETH_PORT_3 == 1) */

    /* set actual MAC address */
    ret = SILICONID_ConvertToMacAddr((uint8_t (*)[6]) &mac);
    if (kStatus_Success == ret) {
        goal_logInfo("setting MAC address: %02X:%02X:%02X:%02X:%02X:%02X",
                     mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

        goal_miEthCfgMacAddr(GOAL_ETH_PORT_HOST, GOAL_MA_ID_ETH, &mac);
    }

    res = goal_drvEthNetcReg(GOAL_BOARD_ETH_IF1_ID, ethPortInfo, (uint32_t) ARRAY_ELEMENTS(ethPortInfo), &bufDescInfo);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to initialize NETC Ethernet driver");
        return res;
    }

    if (GOAL_RES_OK(res)) {
        res = goal_targetSwitchNXPNetcReg(ethPortInfo, (uint32_t) ARRAY_ELEMENTS(ethPortInfo));
    }

#if GOAL_CONFIG_TCPIP_STACK == 1
    if (GOAL_RES_OK(res)) {
        res = goal_drvNetLwipReg(GOAL_BOARD_ETH_IF1_ID, GOAL_BOARD_ETH_IF1_ID);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to register lwIP Net driver");
        }
    }
#endif /* GOAL_CONFIG_TCPIP_STACK */

#if GOAL_CONFIG_NUM_INTERFACES == 2
    GOAL_MEMSET(&bufDescInfo, 0, sizeof(GOAL_DRV_ETH_NETC_BUFDESC_INFO_T));
    bufDescInfo.numTc = GOAL_BOARD_ETH_IF2_NUM_TC;
    for (cnt = 0; cnt < GOAL_BOARD_ETH_IF2_NUM_TC; cnt++) {
        bufDescInfo.rxBuffDesc[cnt] = &rxBuffDescrip2[cnt][0];
        bufDescInfo.txBuffDesc[cnt] = &txBuffDescrip2[cnt][0];
        bufDescInfo.rxBuffers[cnt] = &rxDataBuff2[cnt][0];
    }
    bufDescInfo.epCmdDesc = &epCmdDescrip2[0];

    res = goal_drvEthNetcReg(GOAL_BOARD_ETH_IF2_ID, &ethPortInfo2, 1, &bufDescInfo);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to initialize NETC Ethernet driver");
        return res;
    }

#if GOAL_CONFIG_TCPIP_STACK == 1
    if (GOAL_RES_OK(res)) {
        res = goal_drvNetLwipReg(GOAL_BOARD_ETH_IF2_ID, GOAL_BOARD_ETH_IF2_ID);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to register lwIP Net driver");
        }
    }
#endif /* GOAL_CONFIG_TCPIP_STACK */
#endif /* GOAL_CONFIG_NUM_INTERFACES == 2 */

# if GOAL_CONFIG_ECAT == 1
    if (GOAL_RES_OK(res)) {
        res = goal_drvEoeReg(GOAL_MA_ID_EOE);
    }
# endif
#endif /* GOAL_CONFIG_ETHERNET */

#if GOAL_CONFIG_MEDIA_MA_UART == 1
    if (GOAL_RES_OK(res)) {
        /* UART initialization for modbus stack */
        res = goal_drvUartRt1180Reg(GOAL_ID_DEFAULT, LPUART12);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
    }
#endif /* GOAL_CONFIG_MEDIA_MA_UART */

#if GOAL_CONFIG_TLS == 1
    /* register TLS driver */
    #if (1 == GOAL_CONFIG_TLS_WOLFSSL)
        res = goal_drvTlsWolfsslInit(&pTls, GOAL_TGT_MA_TLS_ID_WOLFSSL);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to register TLS driver for WolfSSL");
            return res;
        }
    #endif
    #if (1 == GOAL_CONFIG_TLS_MBEDTLS)
        res = goal_drvTlsMbedtlsInit(&pTls, GOAL_TGT_MA_TLS_ID_MBEDTLS);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to register TLS driver for Mbed TLS");
            return res;
        }
    #endif
    #if (0 == GOAL_CONFIG_TLS_WOLFSSL) && (0 == GOAL_CONFIG_TLS_MBEDTLS)
        UNUSEDARG(pTls);
        res = GOAL_ERROR;
        if (GOAL_RES_ERR(res)) {
            goal_logErr("TLS activated but no supported TLS stack selected");
            return GOAL_ERROR;
        }
    #endif
#endif /* GOAL_CONFIG_TLS == 1 */

#if GOAL_CONFIG_MCTC == 1
    /* initialize MCTC over RAM */
    res = goal_mainStageReg(GOAL_STAGE_CTC_PRE, &mStageCtcPre, GOAL_STAGE_INIT, goal_targetBoardMctcRamInit);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to register MCTC over RAM init handler");
        return res;
    }
#endif

    return res;
}

#if GOAL_CONFIG_TASK == 1
/****************************************************************************/
/** FreeRtos Idle Task memory get
 *
 * This function provides memory for the task structure and the stack of
 * the Idle Task which is created in vTaskStartScheduler().
 *
 */
void vApplicationGetIdleTaskMemory(
    StaticTask_t **ppxIdleTaskTCBBuffer,        /**< pointer to Task struct reference */
    StackType_t **ppxIdleTaskStackBuffer,       /**< pointer to stack buffer */
    uint32_t *pulIdleTaskStackSize              /**< size of stack buffer */
)
{
    *ppxIdleTaskTCBBuffer = &taskMemIdle;
    *ppxIdleTaskStackBuffer = &taskStackIdle[0];
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}


#if (GOAL_CONFIG_TARGET_FREERTOS_TIMER_SUPPORT == 1)
/****************************************************************************/
/** FreeRtos Timer Task memory get
 *
 * This function provides memory for the task structure and the stack of
 * the Timer Task which is created in vTaskStartScheduler().
 *
 */
void vApplicationGetTimerTaskMemory(
    StaticTask_t **ppxTimerTaskTCBBuffer,       /**< pointer to Task struct reference */
    StackType_t **ppxTimerTaskStackBuffer,      /**< pointer to stack buffer */
    uint32_t *pulTimerTaskStackSize             /**< size of stack buffer */
)
{
    *ppxTimerTaskTCBBuffer = &taskMemTimer;
    *ppxTimerTaskStackBuffer = &taskStackTimer[0];
    *pulTimerTaskStackSize = configMINIMAL_STACK_SIZE;
}
#endif /* (GOAL_CONFIG_TARGET_FREERTOS_TIMER_SUPPORT == 1) */
#endif /* GOAL_CONFIG_TASK */


#if GOAL_CONFIG_MEDIA_MI_NVS == 1
/****************************************************************************/
/** NVS Initialization
 *
 * Initializes the target for NVS usage. By default, SFLASH is used but setting
 * compiler define GOAL_CONFIG_NVS_EEPROM_I2C gains access to EEPROM to store
 * CM variables.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
static GOAL_STATUS_T goal_targetNvsInit(
    void
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MI_NVS_T *ppMiNvs;                     /* NVS MI handle */

    res = goal_drvNvsSflashFslReg(GOAL_ID_DEFAULT, &mInfoNvs, (uint8_t *) GOAL_TGT_BOARD_SFLASH_ADDR, FLEXSPI1);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to reg SFLASH driver");
        return res;
    }

    /* individual modification of CM region */
    /* SFLASH is accessed in bufferd mode */
    region_list->mode = GOAL_MI_NVS_REGION_MODE_BUFFERED;

    /* place CM variables at the end of SFLASH */
    region_list->posStart = mInfoNvs.sizeFlash - (region_list->length / mInfoNvs.sizeErase) * mInfoNvs.sizeErase;

    /* registration of NVS region within NVS */
    res = goal_miNvsReg(&ppMiNvs, GOAL_ID_DEFAULT, region_list, ARRAY_ELEMENTS(region_list));
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Could not register nvs MI");
        return res;
    }

    /* get the region of CM variables */
    res = goal_miNvsRegionGetById(&pNvsRegionCm, GOAL_ID_MI_NVS_REGION_CMCONFIG);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Could not get handle of CM region");
        return res;
    }

    /* open the region of CM variables */
    res = goal_miNvsRegionOpen(pNvsRegionCm);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Could not open the CM region");
        return res;
    }

    return res;
}
#endif /* GOAL_CONFIG_MEDIA_MI_NVS == 1 */


#if GOAL_CONFIG_LOGGING_TARGET_RAW == 1
/****************************************************************************/
/** Print Message
 *
 * If logging is enabled, this function is called by the logging module to
 * print the message over serial, stdout or other ways like logfiles.
 */
void goal_targetMsgRaw(
    const char *str,                            /**< log string */
    unsigned int len                            /**< string length */
)
{
    UNUSEDARG(str);
    UNUSEDARG(len);

    /* print string via UART */
    PRINTF("%.*s\n", len, str);
}
#endif /* GOAL_CONFIG_LOGGING_TARGET_RAW */


#if (GOAL_CONFIG_LOGGING_TARGET_NONBLOCK == 1) || (GOAL_CONFIG_CLI_UART == 1)
/****************************************************************************/
/** Print Message in non-blocking mode
 *
 * If non-blocking logging is enabled, this function is called if the logging
 * ring buffer is not empty. This function must determine if  the actual
 * printing function is ready. If not the function must return immediately.
 *
 * @retval GOAL_OK character printed
 * @retval GOAL_ERROR device not ready
 */
GOAL_STATUS_T goal_tgtCharPut(
    char c                                      /**< character */
)
{
    UNUSEDARG(c);

    printf("%c", c);

    return GOAL_OK;
}
#endif /* (GOAL_CONFIG_LOGGING_TARGET_NONBLOCK == 1) || (GOAL_CONFIG_CLI_UART == 1) */


#if GOAL_CONFIG_CLI_UART == 1
/****************************************************************************/
/** Receives a char from the UART
 *
 *  @retval GOAL_OK successful
 *  @retval GOAL_ERR_NODATA no char available
 */
GOAL_STATUS_T goal_tgtCharGet(
    char *pChar                                 /**< Pointer to character */
)
{
    gets(pChar);
    return GOAL_OK;
}
#endif /* GOAL_CONFIG_CLI_UART */


/****************************************************************************/
/** Get Flash Size
 *
 * Get the flash size that can be used to store values for this target.
 *
 * @retval size NVS size
 */
unsigned int goal_targetNvsGetSize(
    void
)
{
#if GOAL_CONFIG_MEDIA_MI_NVS == 1
    return GOAL_TGT_NVS_REGION_CMCONFIG_LENGTH;
#else
    return 0;
#endif
}


/****************************************************************************/
/** Get the data stored in NVS
 *
 * The size of the NVS should be verified with @see goal_targetNvsGetSize.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_targetNvsReadData(
    void *pData,                                /**< pointer to store data */
    unsigned int size                           /**< data size */
)
{
#if GOAL_CONFIG_MEDIA_MI_NVS == 1
    return goal_miNvsRead(pNvsRegionCm, pData, 0, size);
#else
    UNUSEDARG(pData);
    UNUSEDARG(size);

    return GOAL_ERR_UNSUPPORTED;
#endif
}


/****************************************************************************/
/** Store data in NVS
 *
 * The size of the NVS should be verified with @see goal_targetNvsGetSize.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_targetNvsWriteData(
    void *pData,                                /**< pointer to read data from */
    unsigned int size                           /**< data size */
)
{
#if GOAL_CONFIG_MEDIA_MI_NVS == 1
    return goal_miNvsWrite(pNvsRegionCm, pData, 0, size);
#else
    UNUSEDARG(pData);
    UNUSEDARG(size);

    return GOAL_ERR_UNSUPPORTED;
#endif
}


/****************************************************************************/
/** Sets the state of the available LEDs
 *
 */
void goal_targetSetLeds(
    uint32_t leds                               /**< LED status bit field */
)
{
    UNUSEDARG(leds);

    /* Debug output */
    goal_logDbg("Requested to set LED mask: 0x%x.", (unsigned int) leds);

#ifdef BOARD_INITPINS_USER_LED_1_PERIPHERAL
    /* set / reset LED1 */
    if (leds & GOAL_TARGET_LED1) {
        RGPIO_PinWrite(BOARD_INITPINS_USER_LED_1_PERIPHERAL, BOARD_INITPINS_USER_LED_1_GPIO_PIN, 1U);
    } else {
        RGPIO_PinWrite(BOARD_INITPINS_USER_LED_1_PERIPHERAL, BOARD_INITPINS_USER_LED_1_GPIO_PIN, 0U);
    }
#endif
#ifdef BOARD_INITPINS_USER_LED_2_PERIPHERAL
    /* set / reset LED2 */
    if (leds & GOAL_TARGET_LED2) {
        RGPIO_PinWrite(BOARD_INITPINS_USER_LED_2_PERIPHERAL, BOARD_INITPINS_USER_LED_2_GPIO_PIN, 1U);
    } else {
        RGPIO_PinWrite(BOARD_INITPINS_USER_LED_2_PERIPHERAL, BOARD_INITPINS_USER_LED_2_GPIO_PIN, 0U);
    }
#endif
}


/****************************************************************************/
/** Read the state of the available LEDs
 *
 * Get the state of the available LEDs. If a LED is active, the corresponding
 * bit is set to 1.
 *
 * Example: If LED 4 on GPIO Pin 12 is active, the 3rd bit in the bit field is
 *          set to 1 (starting numbering with 0 for the first bit)
 *
 *
 * @retval uint32_T - LED status bit field
 */
uint32_t goal_targetGetLeds(
    void
)
{
    uint32_t led = 0;                           /* enabled leds */

#ifdef BOARD_INITPINS_USER_LED_1_PERIPHERAL
    /* Check if LED 1 is active */
    if (1 == RGPIO_PinRead(BOARD_INITPINS_USER_LED_1_PERIPHERAL, BOARD_INITPINS_USER_LED_1_CHANNEL)) {
        led |= GOAL_TARGET_LED1;
    }
#endif
#ifdef BOARD_INITPINS_USER_LED_2_PERIPHERAL
    /* Check if LED 2 is active */
    if (1 == RGPIO_PinRead(BOARD_INITPINS_USER_LED_2_PERIPHERAL, BOARD_INITPINS_USER_LED_2_CHANNEL)) {
        led |= GOAL_TARGET_LED2;
    }
#endif

    /* Return LED bit field */
    return led;
}


#if GOAL_CONFIG_MCTC == 1
/****************************************************************************/
/** Setup MCTC Over Shared Memory
 *
 * This function creates needed partitions for the multicore communication.
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_targetBoardMctcRamInit(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    if (GOAL_BOARD_RAM_SIZE <= (GOAL_BOARD_RPC_SIZE - 1)) {
        goal_logErr("%"FMT_u32" bytes for SHM are too less for %"FMT_u32" byte RPC transfer",
                    (uint32_t) GOAL_BOARD_RAM_SIZE,
                    (uint32_t) GOAL_BOARD_RPC_SIZE);
        return GOAL_ERR_INIT;
    }

    /* create read instance */
    res = goal_miDmNew(&mpMiDmRead, GOAL_MI_MCTC_DIR_PEER_FROM, GOAL_BOARD_RAM_SIZE, GOAL_BOARD_DM_READ_DESC);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create MI DM Output instance");
        return res;
    }

    /* create write instance */
    res = goal_miDmNew(&mpMiDmWrite, GOAL_MI_MCTC_DIR_PEER_TO, GOAL_BOARD_RAM_SIZE, GOAL_BOARD_DM_WRITE_DESC);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create MI DM Input instance");
        return res;
    }

    /* initialize the mailbox for MCTC usage */
# if GOAL_CONFIG_MEDIA_MA_MBOX == 1
    res = goal_tgtMboxInit();
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Unable to initialize the mailbox");
        return res;
    }
# endif

    /* register a new MCTC MI */
# if GOAL_CONFIG_MEDIA_MA_MBOX == 1
    res = goal_miMctcRamReg(GOAL_ID_DEFAULT,
                            GOAL_BOARD_SHM_ADDR,
                            GOAL_TGT_BOARD_SHM_SIZE,
                            GOAL_FALSE,
                            goal_tgtCtcNotifyTx,
                            goal_tgtDpramSemLock,
                            goal_tgtDpramSemUnlock,
                            mpMiDmRead,
                            mpMiDmWrite);
# else /* GOAL_CONFIG_MEDIA_MA_MBOX == 1 */
    res = goal_miMctcRamReg(GOAL_ID_DEFAULT,
                            GOAL_BOARD_SHM_ADDR,
                            GOAL_BOARD_SHM_SIZE,
                            GOAL_FALSE,
                            NULL,
                            goal_tgtDpramSemLock,
                            goal_tgtDpramSemUnlock,
                            mpMiDmRead,
                            mpMiDmWrite);
# endif /* GOAL_CONFIG_MEDIA_MA_MBOX == 1 */
    if (GOAL_RES_ERR(res)) {
        goal_logInfo("Unable to reg MI ram");
        return res;
    }

    /* we need to initialize MCTC here to enable send and receive */
    res = goal_miMctcOpen(&pInstMctc, GOAL_ID_DEFAULT);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to open MCTC ram");
        return res;
    }

    /* register RPC MI DM partitions */
    res = goal_miDmPartIdxReg(GOAL_MI_MCTC_DIR_PEER_FROM, GOAL_ID_RPC, &mMiDmRpcRead, GOAL_BOARD_RPC_SIZE, GOAL_BOARD_RPC_POS);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    res = goal_miDmPartIdxReg(GOAL_MI_MCTC_DIR_PEER_TO, GOAL_ID_RPC, &mMiDmRpcWrite, GOAL_BOARD_RPC_SIZE, GOAL_BOARD_RPC_POS);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* map MI DM partitions to RPC */
    res = goal_miMctcRpcDmMap(pInstMctc, &mMiDmRpcRead, &mMiDmRpcWrite);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Lock a semaphore
 *
 * This function locks a semaphore. If flgWr is GOAL_TRUE, the write semaphore
 * is locked, otherwise the read semaphore.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_tgtDpramSemLock(
    GOAL_BOOL_T flgWr                           /**< lock write flag */
)
{
    status_t res;                               /* status */
    uint8_t idSemaphore;                        /* semaphore id */

# if GOAL_CONFIG_MCTC_AC == 1
    idSemaphore = (GOAL_TRUE == flgWr) ? (GOAL_BOARD_SHM_SEMAP_0) : (GOAL_BOARD_SHM_SEMAP_1);
# else
    idSemaphore = (GOAL_TRUE == flgWr) ? (GOAL_BOARD_SHM_SEMAP_1) : (GOAL_BOARD_SHM_SEMAP_0);
# endif

    res = SEMA42_Lock(SEMA1, idSemaphore, GOAL_BOARD_CORE_ID);

    return (kStatus_Success != res) ? GOAL_ERR_BUSY : GOAL_OK;
}


/****************************************************************************/
/** Unlock a semaphore
 *
 * This function unlocks a semaphore. If flgWr is GOAL_TRUE, the write
 * semaphore is unlocked, otherwise the read semaphore.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_tgtDpramSemUnlock(
    GOAL_BOOL_T flgWr                           /**< unlock write flag */
)
{
    uint8_t idSemaphore;                        /* semaphore id */

# if GOAL_CONFIG_MCTC_AC == 1
    idSemaphore = (GOAL_TRUE == flgWr) ? (GOAL_BOARD_SHM_SEMAP_0) : (GOAL_BOARD_SHM_SEMAP_1);
# else
    idSemaphore = (GOAL_TRUE == flgWr) ? (GOAL_BOARD_SHM_SEMAP_1) : (GOAL_BOARD_SHM_SEMAP_0);
# endif

    SEMA42_Unlock(SEMA1, idSemaphore);

    return GOAL_OK;
}
#endif /* GOAL_CONFIG_MCTC == 1 */


/****************************************************************************/
/** Get Buttons
 *
 * @retval uint32_t buttons
 */
uint32_t goal_targetGetButtons(
    void
)
{
    uint32_t button = 0;                        /* enabled buttons */
#if !RIOP
    /* Check if button is active */
    if (0 == RGPIO_PinRead(BOARD_INITPINS_USER_BUTTON_1_PERIPHERAL, BOARD_INITPINS_USER_BUTTON_1_CHANNEL)) {
        button = GOAL_TARGET_BUTTON;
    }
#endif
    /* Return button bit field */
    return button;
}

#if GOAL_CONFIG_ETHERNET == 1
/****************************************************************************/
/** Initialize board specific components for Ethernet communication
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_targetBoardEthInit(
    GOAL_DRV_ETH_NETC_PORT_INFO_T *pPortInfo,   /**< port info data table */
    uint32_t numEntries                         /**< entries in port info data table */
)
{
#if !RIOP
    rgpio_pin_config_t pinConfig;               /* pin configuration data */
    uint32_t cnt;                               /* loop counter */
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    pinConfig.pinDirection = kRGPIO_DigitalOutput;
    pinConfig.outputLogic = 0;

    for (cnt = 0; (cnt < numEntries) && GOAL_RES_OK(res); cnt++) {
        switch (pPortInfo[cnt].portId) {
            case kNETC_ENETC0EthPort:
                BLK_CTRL_WAKEUPMIX->NETC_LINK_CFG[4] = BLK_CTRL_WAKEUPMIX_NETC_LINK_CFG_MII_PROT(1);
                break;

            case kNETC_SWITCH0EthPort0:
                BOARD_InitSwtPort0Pins();
                BLK_CTRL_WAKEUPMIX->NETC_LINK_CFG[0] = BLK_CTRL_WAKEUPMIX_NETC_LINK_CFG_MII_PROT(1);
                break;

            case kNETC_SWITCH0EthPort1:
                BOARD_InitSwtPort1Pins();
                BLK_CTRL_WAKEUPMIX->NETC_LINK_CFG[1] = BLK_CTRL_WAKEUPMIX_NETC_LINK_CFG_MII_PROT(2);
                break;

            case kNETC_SWITCH0EthPort2:
                BOARD_InitSwtPort2Pins();
                BLK_CTRL_WAKEUPMIX->NETC_LINK_CFG[2] = BLK_CTRL_WAKEUPMIX_NETC_LINK_CFG_MII_PROT(2);
                break;

            case kNETC_SWITCH0EthPort3:
                BOARD_InitSwtPort3Pins();
                BLK_CTRL_WAKEUPMIX->NETC_LINK_CFG[3] = BLK_CTRL_WAKEUPMIX_NETC_LINK_CFG_MII_PROT(2);
                break;

            default:
                goal_logErr("unknown port ID: %d", pPortInfo[cnt].portId);
                res = GOAL_ERROR;
                break;
        }

        if (GOAL_RES_OK(res)) {
            RGPIO_PinInit(pPortInfo[cnt].pPhyResetGpioBase, pPortInfo[cnt].phyResetPin, &pinConfig);
        }
    }

    SDK_DelayAtLeastUs(10000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);

    for (cnt = 0; (cnt < numEntries) && GOAL_RES_OK(res); cnt++) {
        RGPIO_PinWrite(pPortInfo[cnt].pPhyResetGpioBase, pPortInfo[cnt].phyResetPin, 1);
    }

    SDK_DelayAtLeastUs(150000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);

    if (GOAL_RES_OK(res) && (GOAL_FALSE == mEthInitFlag)) {
        /* Output reference clock for RMII */
        BLK_CTRL_WAKEUPMIX->NETC_PORT_MISC_CFG |= BLK_CTRL_WAKEUPMIX_NETC_PORT_MISC_CFG_PORT0_RMII_REF_CLK_DIR_MASK |
                                                  BLK_CTRL_WAKEUPMIX_NETC_PORT_MISC_CFG_PORT4_RMII_REF_CLK_DIR_MASK;

        /* Unlock the IERB. It will warm reset whole NETC. */
        NETC_PRIV->NETCRR &= ~NETC_PRIV_NETCRR_LOCK_MASK;
        while ((NETC_PRIV->NETCRR & NETC_PRIV_NETCRR_LOCK_MASK) != 0U);

        /* Set the access attribute, otherwise MSIX access will be blocked. */
        NETC_IERB->ARRAY_NUM_RC[0].RCMSIAMQR &= ~(7U << 27);
        NETC_IERB->ARRAY_NUM_RC[0].RCMSIAMQR |= (1U << 27);

        /* Lock the IERB. */
        NETC_PRIV->NETCRR |= NETC_PRIV_NETCRR_LOCK_MASK;
        while ((NETC_PRIV->NETCSR & NETC_PRIV_NETCSR_STATE_MASK) != 0U);

        mEthInitFlag = GOAL_TRUE;
    }

    return res;

#else
   if (EXAMPLE_NETC_LINK_PORT < BLK_CTRL_WAKEUPMIX_NETC_LINK_CFG_COUNT)
    {
        /* Set RMII or RGMII mode*/
        status_t status = NETC_SocSetMiiMode(EXAMPLE_NETC_LINK_PORT, EXAMPLE_MII_MODE);
        if (status != kStatus_Success)
        {
            PRINTF("ERROR: Cannot set the MII mode, returned %d\n", status);
        }

        /* Output reference clock for RMII */
        status = NETC_SocSetRmiiRefClk((netc_soc_link_t)EXAMPLE_NETC_LINK_PORT, true);
        if (status != kStatus_Success)
        {
            PRINTF("ERROR: Cannot set RMII clock reference, returned %d\n", status);
        }

        /* Unlock the IERB. It will warm reset whole NETC. */
        if (!NETC_IERBIsLockOver())
        {
            status = NETC_IERBUnlock();
            if (status != kStatus_Success)
            {
                PRINTF("ERROR: Cannot unlock the IERB, returned %d\n", status);
            }
        }

        /* Set link port PHY address */
        status = NETC_SocSetLinkAddr((netc_soc_link_t)EXAMPLE_NETC_LINK_PORT, EXAMPLE_PHY_ADDRESS);
        if (status != kStatus_Success)
        {
            PRINTF("ERROR: Cannot set link address, returned %d\n", status);
        }

        /* Set the access attribute, otherwise MSIX access will be blocked. */
        NETC_IERB->ARRAY_NUM_RC[0].RCMSIAMQR &= ~(7U << 27);
        NETC_IERB->ARRAY_NUM_RC[0].RCMSIAMQR |= (1U << 27);

        /* Lock the IERB. */
        status = NETC_IERBLock();
        if (status != kStatus_Success)
        {
            PRINTF("ERROR: Cannot lock the IERB, returned %d\n", status);
        }
        /* The RGPIO_PinInit and PinWrite is taken care in BOARD_InitBootPins() */

		SDK_DelayAtLeastUs(150000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);

		if (status == kStatus_Success) 
		{
			mEthInitFlag = GOAL_TRUE;
		}
    }
    return GOAL_OK;
#endif
}


/****************************************************************************/
/** Initialize the Switch driver and Phy drivers
 *
 * This function is called by the Ethernet driver after it has been initialized.
 * If the Ethernet driver expects a switch to be connected to the interface,
 * the handle @em pIfaceSwitch is not NULL. In that case the board must load the
 * appropriate Switch driver.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_targetBoardEthPostInit(
    GOAL_MA_ETH_T *pMaEth,                      /**< Ethernet MA handle */
    GOAL_DRV_ETH_NETC_PORT_INFO_T *pPortInfo,   /**< port info data table */
    uint32_t numEntries,                        /**< entries in port info data table */
    GOAL_ETH_IFACE_T *pIfaceSwitch,             /**< Switch interface handle */
    GOAL_ETH_IFACE_T *pIfacePhys,               /**< PHY interface handles */
    void *pSwitchHandle                         /**< target switch handle */
)
{
    uint32_t cnt;                               /* loop counter */
    GOAL_BUS_T mdioBus;                         /* MDIO bus handle */
    GOAL_STATUS_T res;                          /* result */
    uint16_t regData;                           /* Phy register data */

    res = goal_drvBusMdioNetcOpen(&mdioBus);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to open MDIO bus");
    }

    /* register PHY ports */
    cnt = 0;
    while (GOAL_RES_OK(res) && (cnt < numEntries)) {

        /* assign bus handle and Phy address to interface */
        GOAL_MEMCPY(&pIfacePhys[cnt].bus, &mdioBus, sizeof(GOAL_BUS_T));
        pIfacePhys[cnt].addr = pPortInfo[cnt].phyAddr;

        /* register PHY driver */
        if (GOAL_ETH_SPEED_1000 == pPortInfo[cnt].maxSpeed) {
#if RIOP
            res = goal_targetPhyVsc8541Open(&pIfacePhys[cnt]);
#else
            res = goal_targetPhyRtl8211Open(&pIfacePhys[cnt]);
#endif
        }
        else {
#if RIOP
            res = goal_targetPhyVsc8541Open(&pIfacePhys[cnt]);
#else
            res = goal_targetPhyRtl8201Open(&pIfacePhys[cnt]);
#endif
        }
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to open Phy driver for port %"FMT_u32, cnt);
            SDK_DelayAtLeastUs(100000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
            RGPIO_PinWrite(pPortInfo[cnt].pPhyResetGpioBase, pPortInfo[cnt].phyResetPin, 0);
            SDK_DelayAtLeastUs(10000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
            RGPIO_PinWrite(pPortInfo[cnt].pPhyResetGpioBase, pPortInfo[cnt].phyResetPin, 1);
            SDK_DelayAtLeastUs(150000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
            res = GOAL_OK;
            continue;
        }

        if (GOAL_RES_OK(res) && (GOAL_ETH_SPEED_100 == pPortInfo[cnt].maxSpeed)) {
            /* on this board the CRS/CRS_DV pin of the Rtl8201 is used as RXDV signal */

            /* set page 7 */
            regData = 7;
#if RIOP
            res = pIfacePhys[cnt].bus.write(&pIfacePhys[cnt].bus, pIfacePhys[cnt].addr, PHY_PAGE_SELECT_REG, (char *) &regData, 2);
#else
            res = pIfacePhys[cnt].bus.write(&pIfacePhys[cnt].bus, pIfacePhys[cnt].addr, GOAL_PHY_RTL8201_REG_PAGE_SELECT, (char *) &regData, 2);
#endif
            if (GOAL_RES_ERR(res)) {
                goal_logErr("failed to set Phy page to 7, port %"FMT_u32, cnt);
            }

#if !RIOP
            if (GOAL_RES_OK(res)) {
                /* get RMII Mode Setting Register value */
                res = pIfacePhys[cnt].bus.read(&pIfacePhys[cnt].bus, pIfacePhys[cnt].addr, GOAL_PHY_RTL8201_REG_RMSR, (char *) &regData, 2);
                if (GOAL_RES_ERR(res)) {
                    goal_logErr("failed to get Phy RMSR register, port %"FMT_u32, cnt);
                }
            }

            if (GOAL_RES_OK(res)) {
                /* set RMII Mode Setting Register value */
                regData |= GOAL_PHY_RTL8201_RMSR_RXDV_SEL;
                res = pIfacePhys[cnt].bus.write(&pIfacePhys[cnt].bus, pIfacePhys[cnt].addr, GOAL_PHY_RTL8201_REG_RMSR, (char *) &regData, 2);
                if (GOAL_RES_ERR(res)) {
                    goal_logErr("failed to set Phy RMSR register, port %"FMT_u32, cnt);
                }
            }

            if (GOAL_RES_OK(res)) {
                /* enable Half Duplex connections via RMII */
                regData = GOAL_PHY_RTL8201_MIITXISO_HD_FIX;
                res = pIfacePhys[cnt].bus.write(&pIfacePhys[cnt].bus, pIfacePhys[cnt].addr, GOAL_PHY_RTL8201_REG_MIITXISO, (char *) &regData, 2);
                if (GOAL_RES_ERR(res)) {
                    goal_logErr("failed to set Phy RMSR register, port %"FMT_u32, cnt);
                }
            }
#endif /* !RIOP */

            if (GOAL_RES_OK(res)) {
                /* set page 0 */
                regData = 0;
#if RIOP
                res = pIfacePhys[cnt].bus.write(&pIfacePhys[cnt].bus, pIfacePhys[cnt].addr, PHY_PAGE_SELECT_REG, (char *) &regData, 2);
#else
                res = pIfacePhys[cnt].bus.write(&pIfacePhys[cnt].bus, pIfacePhys[cnt].addr, GOAL_PHY_RTL8201_REG_PAGE_SELECT, (char *) &regData, 2);
#endif
                if (GOAL_RES_ERR(res)) {
                    goal_logErr("failed to set Phy page to 0, port %"FMT_u32, cnt);
                }
            }
        }
        cnt++;
    }

#ifdef SDK_OS_FREE_RTOS
    NVIC_SetPriority(MSGINTR1_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1);
    NVIC_SetPriority(MSGINTR2_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1);
#else /* SDK_OS_FREE_RTOS */
    /* bare metal, MSGINTR1_IRQn (Ethernet) must be higher than MSGINTR2_IRQn (Qbv cycle) */
    NVIC_SetPriority(MSGINTR1_IRQn, 0);
    NVIC_SetPriority(MSGINTR2_IRQn, 1);
#endif /* SDK_OS_FREE_RTOS */

    if (GOAL_RES_OK(res) && (NULL != pIfaceSwitch)) {
        /* pSwitchHandle needs to be passed to the switch driver to operate on the handle */
        /* pIfaceSwitch needs to be filled by the switch driver so commands can be executed */
        res = goal_targetSwitchNXPNetcOpen(pMaEth, pSwitchHandle, pIfaceSwitch);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to open switch driver");
        }
    }

    return res;
}
#endif /* GOAL_CONFIG_ETHERNET */


#if GOAL_CONFIG_TARGET_FREERTOS_TARGET_MAIN == 1
/****************************************************************************/
/** Implementation of goal_mainPreOsCb
 *
 * This optional Cb is called from goal_main(). It can be used to initialize
 * needed HW before goal_main.
 *
 * @retval GOAL_OK success
 */
GOAL_STATUS_T goal_targetMainPreOsCb(
    void
)
{
# if GOAL_CONFIG_MCTC == 1
    GOAL_STATUS_T res;                          /* result */
# endif

#if RIOP
    BOARD_InitHardware();
#else
    /* Hardware Initialization. */
    BOARD_ConfigMPU();
    BOARD_InitBootPins();
# if GOAL_CONFIG_MCTC_AC == 1
    /* CM7 shall not touch clocks */
    SystemCoreClock = CLOCK_GetRootClockFreq(kCLOCK_Root_M7);
# else
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();
#  if GOAL_CONFIG_LOGGING == 1 && GOAL_CONFIG_LOGGING_TARGET_RAW == 1
    BOARD_InitDEBUG_UARTPins();
#  endif /* GOAL_CONFIG_LOGGING == 1 && GOAL_CONFIG_LOGGING_TARGET_RAW == 1 */
# endif
#endif

    goal_logInfo("executed HardwareInit");

# if GOAL_CONFIG_MCTC == 1
    res = goal_targetBoardMcInit();
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    goal_logInfo("executed multicore initialization");
# endif

    return GOAL_OK;
}
#endif /* GOAL_CONFIG_TARGET_FREERTOS_TARGET_MAIN */


#if GOAL_CONFIG_MCTC == 1
/****************************************************************************/
/** System Initialization Hook
 *
 * This function is needed for multicore projects and prepares
 * the Multicore Manager as well as the CM7 core.
 */
void SystemInitHook(
    void
)
{
    mcmgr_status_t res;                         /* result */

    /* must be called as close as possible to the reset entry
     * to allow core up event triggering
     */
    res = MCMGR_EarlyInit();
    if (kStatus_MCMGR_Success != res) {
        return;
    }

# if GOAL_CONFIG_MCTC_CC == 1
    /* other core must be prepared */
    Prepare_CM7(0U);
# endif
}


/****************************************************************************/
/** Multicore Initialization
 *
 * This function initializes multicore related peripherals and takes care
 * of the synchronized startup of both cores by copying the image
 * for the CM7 to a specified memory. The CM3 then releases the CM7.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
static GOAL_STATUS_T goal_targetBoardMcInit(
    void
)
{
    mcmgr_status_t res;                         /* result */
# if GOAL_CONFIG_MCTC_AC == 1
    uint32_t startupData = 0;                   /* data from other core */
# endif
# if GOAL_CONFIG_MCTC_CC == 1
    extern const char core1_image_start[];      /* CM33 linker symbols */
    extern int core1_image_size;                /* CM33 linker symbols */
    uint32_t cnt;                               /* counter */
    uint8_t *pSrc;                              /* copy source */
    uint8_t *pDst;                              /* copy destination */
#endif

# if GOAL_CONFIG_MCTC_CC == 1
    /* clear SHMEM for data exchange */
    GOAL_MEMSET((void *) GOAL_BOARD_SHM_ADDR, 0, GOAL_BOARD_SHM_SIZE);

    /* load secondary image byte-wise to RAM, memcpy
     * leads to fault for (imagesizes % 4) != 0
     */
    pSrc = (uint8_t *) ((void *) core1_image_start);
    pDst = (uint8_t *) ((void *) 0x303C0000);
    for (cnt = 0; cnt < ((uint32_t) core1_image_size); cnt++) {
        pDst[cnt] = pSrc[cnt];
    }
# endif /* GOAL_CONFIG_MCTC_CC == 1 */

    res = MCMGR_Init();
    if (kStatus_MCMGR_Success != res) {
        goal_logErr("error during multicore initialization");
        return GOAL_ERR_INIT;
    }

# if GOAL_CONFIG_MCTC_CC == 1
    res = MCMGR_RegisterEvent(kMCMGR_RemoteApplicationEvent, goal_targetBoardMcEventHdl,
                              (void *) &mMulticoreEventData);
    if (kStatus_MCMGR_Success != res) {
        return GOAL_ERR_INIT;
    }

    /* start core from core1 boot address - actually unused by SDK */
#  if GOAL_BOARD_MCMGR_SYNC == 1
    res = MCMGR_StartCore(kMCMGR_Core1, (void *) 0x303C0000,
                          0, kMCMGR_Start_Synchronous);
#  else /* GOAL_BOARD_MCMGR_SYNC == 1 */
    res = MCMGR_StartCore(kMCMGR_Core1, (void *) 0x303C0000,
                          0, kMCMGR_Start_Asynchronous);
#  endif /* GOAL_BOARD_MCMGR_SYNC == 1 */

    if (kStatus_MCMGR_Success != res) {
        return GOAL_ERR_INIT;
    }

#  if GOAL_BOARD_MCMGR_SYNC == 1
    /* wait until the secondary core application signals it is ready to communicate. */
    while (0x01U != mMulticoreEventData) {
        ;
    }
#  endif

#if RIOP
    PRINTF("Secondary Core Started...\r\n");
#endif

# elif GOAL_CONFIG_MCTC_AC == 1
    UNUSEDARG(startupData);
#  if GOAL_BOARD_MCMGR_SYNC == 1
    /* data is provided by successful started communication core */
    do {
        res = MCMGR_GetStartupData(kMCMGR_Core0, &startupData);
    } while (res != kStatus_MCMGR_Success);

    /* signal the other core we are ready */
    res = MCMGR_TriggerEvent(kMCMGR_Core0, kMCMGR_RemoteApplicationEvent, 0x01U);
    if (kStatus_MCMGR_Success != res) {
        return GOAL_ERR_INIT;
    }
#  endif  /* GOAL_BOARD_MCMGR_SYNC == 1 */
# endif /* GOAL_CONFIG_MCTC_CC == 1 */

# if GOAL_CONFIG_MCTC_CC == 1
    /* use hw semaphore for synchronisation of shmem */
    SEMA42_Init(SEMA1);

    res = SEMA42_ResetAllGates(SEMA1);
    if (kStatus_MCMGR_Success != res) {
        return GOAL_ERR_INIT;
    }
# endif /* GOAL_CONFIG_MCTC_CC == 1 */

    /* TODO: add trigger for SHMEM access, so this workaround
     * can be removed to avoid timeout due to blocking goal_loop */
    goal_miMctcRamCfgTout(3000);

    return GOAL_OK;
}


# if GOAL_CONFIG_MCTC_CC == 1
/****************************************************************************/
/** Multicore Event Handler
 *
 * This function is registered and called by the Multicore Manager
 * to provide user specific data and inform about events.
 */
static void goal_targetBoardMcEventHdl(
    mcmgr_core_t coreNum,                       /**< source core */
    uint16_t eventData,                         /**< event */
    void *pContext                              /**< user data */
)
{
    uint16_t *pData;                            /* user data */

    UNUSEDARG(coreNum);

    /* set received event */
    pData = (uint16_t *) pContext;
    *pData = eventData;
}
# endif /* GOAL_CONFIG_MCTC_CC == 1 */
#endif /* GOAL_CONFIG_MCTC == 1 */


#if GOAL_CONFIG_TGT_RT1180_TRACE == 1
/****************************************************************************/
/** Trace init function
 *
 */
void goal_tgtTraceInit(
    void
)
{
    rgpio_pin_config_t pinConfig;               /* pin configuration */

    pinConfig.outputLogic = 0;
    pinConfig.pinDirection = kRGPIO_DigitalOutput;

    RGPIO_PinInit(RGPIO4, 1, &pinConfig);
    RGPIO_PinInit(RGPIO4, 3, &pinConfig);
    RGPIO_PinInit(RGPIO4, 5, &pinConfig);
    RGPIO_PinInit(RGPIO4, 7, &pinConfig);
    RGPIO_PinInit(RGPIO4, 9, &pinConfig);
    RGPIO_PinInit(RGPIO4, 11, &pinConfig);

    RGPIO_PinWrite(RGPIO4, 1, 1);
    RGPIO_PinWrite(RGPIO4, 3, 1);
    RGPIO_PinWrite(RGPIO4, 5, 1);
    RGPIO_PinWrite(RGPIO4, 7, 1);
    RGPIO_PinWrite(RGPIO4, 9, 1);
    RGPIO_PinWrite(RGPIO4, 11, 1);
}


/****************************************************************************/
/** Trace helper function
 *
 */
void goal_tgtTaskTrace(
    unsigned int enterOrLeave,                  /**< 1: switched on, 0: switched off */
    unsigned int taskId                         /**< task id */
)
{
    uint8_t pin = 0;                            /* GPIO pin number */

    switch (taskId) {
        case 1:
            pin = 7;
            break;
        case 2:
            pin = 3;
            break;
        case 3:
            pin = 5;
            break;
        case 4:
            pin = 11;
            break;
        case 5:
            pin = 9;
            break;
        default:
            /* all tasks without a tag (IDLE task, timer task) */
            pin = 1;
            break;
    }

    if (pin == 0) {
        return;
    }

    if (enterOrLeave) {
        RGPIO_PinWrite(RGPIO4, pin, 1);
    }
    else {
        RGPIO_PinWrite(RGPIO4, pin, 0);
    }
}
#endif
