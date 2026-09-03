/** @file
 *
 * @brief GOAL configuration defaults
 *
 * This header defines default values for undefined GOAL configuration defines.
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

#ifndef GOAL_CONFIG_DEF_H
#define GOAL_CONFIG_DEF_H

#include "goal_config.h"


/****************************************************************************/
/* Commandline Interface */
/****************************************************************************/
#ifndef GOAL_CONFIG_CLI_DBG
#  define GOAL_CONFIG_CLI_DBG 0
#endif

#ifndef GOAL_CONFIG_CLI_HISTORY
#  define GOAL_CONFIG_CLI_HISTORY 0
#endif

#ifndef GOAL_CONFIG_CLI_UART
#  define GOAL_CONFIG_CLI_UART 0
#endif

#ifndef GOAL_CONFIG_CLI_UDP
#  define GOAL_CONFIG_CLI_UDP 0
#endif

#ifndef GOAL_CONFIG_CLI
#  define GOAL_CONFIG_CLI 0
#endif

#ifndef GOAL_CONFIG_CLI_UART
#  define GOAL_CONFIG_CLI_UART 0
#endif

#ifndef GOAL_CONFIG_CLI_UDP
#  define GOAL_CONFIG_CLI_UDP 0
#endif


/****************************************************************************/
/* Config Manager */
/****************************************************************************/
#ifndef GOAL_CM_NAMES
#  define GOAL_CM_NAMES 0
#endif


/****************************************************************************/
/* Demo Mode */
/****************************************************************************/
#ifndef GOAL_CONFIG_DEMO
#  define GOAL_CONFIG_DEMO 0
#endif


/****************************************************************************/
/* Debug */
/****************************************************************************/
#ifndef GOAL_CONFIG_DEBUG_LINUX_BACKTRACE
#  define GOAL_CONFIG_DEBUG_LINUX_BACKTRACE 0
#endif

#ifndef GOAL_CONFIG_DEBUG_MEM_FENCES
#  define GOAL_CONFIG_DEBUG_MEM_FENCES 0
#endif

#ifndef GOAL_CONFIG_DEBUG_SOFTSCOPE
#  define GOAL_CONFIG_DEBUG_SOFTSCOPE 0
#endif

#ifndef GOAL_CONFIG_DEBUG_CONTEXT_LOCKING
#  define GOAL_CONFIG_DEBUG_CONTEXT_LOCKING 0
#endif

#ifndef GOAL_CONFIG_DEBUG_HEAP_USAGE
#  define GOAL_CONFIG_DEBUG_HEAP_USAGE 0
#endif

#ifndef GOAL_CONFIG_DEBUG_HEAP_USAGE_ENTRIES
#  define GOAL_CONFIG_DEBUG_HEAP_USAGE_ENTRIES 0
#endif

#ifndef GOAL_CONFIG_DEBUG_QUEUE_USAGE
#  define GOAL_CONFIG_DEBUG_QUEUE_USAGE 0
#endif


/****************************************************************************/
/* Ethernet */
/****************************************************************************/
#ifndef GOAL_ETH_NAMES
#  define GOAL_ETH_NAMES 0
#endif

#ifndef GOAL_CONFIG_ETH_STATS
#  define GOAL_CONFIG_ETH_STATS 0
#  define GOAL_CONFIG_ETH_STATS_INTERNAL 0
#elif GOAL_CONFIG_ETH_STATS == 1
#  define GOAL_CONFIG_ETH_STATS_INTERNAL 1
#endif

#ifndef GOAL_CONFIG_ETH_STATS_INTERNAL
#  define GOAL_CONFIG_ETH_STATS_INTERNAL 0
#endif

#ifndef GOAL_CONFIG_ETH_STATS_NAMES
#  define GOAL_CONFIG_ETH_STATS_NAMES 0
#endif

#ifndef GOAL_CONFIG_MAC_ADDR_FILTER
#  define GOAL_CONFIG_MAC_ADDR_FILTER 0
#endif

#ifndef GOAL_CONFIG_ETHERNET_DUMP_MAC
#  define GOAL_CONFIG_ETHERNET_DUMP_MAC 0
#endif

#ifndef GOAL_CONFIG_ETHERNET
#  define GOAL_CONFIG_ETHERNET 0
#endif

#ifndef GOAL_CONFIG_SW_RN_ADVSW
#  define GOAL_CONFIG_SW_RN_ADVSW 0
#endif

#ifndef GOAL_CONFIG_ECAT
#  define GOAL_CONFIG_ECAT 0
#endif

#ifndef GOAL_CONFIG_FW
#  define GOAL_CONFIG_FW 0
#endif

#ifndef GOAL_CONFIG_JUMBO_FRAMES
#  define GOAL_CONFIG_JUMBO_FRAMES 0
#endif

#ifndef GOAL_CONFIG_VETH_RPC_AC
#  define GOAL_CONFIG_VETH_RPC_AC 0
#endif


/****************************************************************************/
/* Library Mode */
/****************************************************************************/
#ifndef GOAL_CONFIG_LIBRARY_APPL
#  define GOAL_CONFIG_LIBRARY_APPL 0
#endif

#ifndef GOAL_CONFIG_LIBRARY
#  define GOAL_CONFIG_LIBRARY 0
#endif


/****************************************************************************/
/* Logging */
/****************************************************************************/
#ifndef GOAL_CONFIG_LOGGING
#  define GOAL_CONFIG_LOGGING 0
#endif

#ifndef GOAL_CONFIG_LOGGING_TARGET_NONBLOCK
#  define GOAL_CONFIG_LOGGING_TARGET_NONBLOCK 0
#endif

#ifndef GOAL_CONFIG_LOGGING_TARGET_RAW
#  define GOAL_CONFIG_LOGGING_TARGET_RAW 0
#endif

#ifndef GOAL_CONFIG_LOGGING_TARGET_SYSLOG
#  define GOAL_CONFIG_LOGGING_TARGET_SYSLOG 0
#endif

#ifndef GOAL_CONFIG_LOGGING_TARGET_RAW_INSTANT
#  define GOAL_CONFIG_LOGGING_TARGET_RAW_INSTANT 0
#endif

#ifndef GOAL_CONFIG_LOGGING_TIMESTAMP
#  define GOAL_CONFIG_LOGGING_TIMESTAMP 0
#endif

#ifndef GOAL_CONFIG_LM_EXCEPTION_LOG
#  define GOAL_CONFIG_LM_EXCEPTION_LOG 0
#endif

#ifndef GOAL_CONFIG_LM_EMIT
#  define GOAL_CONFIG_LM_EMIT 0
#endif


/****************************************************************************/
/* Memory Management */
/****************************************************************************/
#ifndef GOAL_CONFIG_MM_EXT
#  define GOAL_CONFIG_MM_EXT 0
#endif


/****************************************************************************/
/* SNMP */
/****************************************************************************/
#ifndef GOAL_CONFIG_SNMP
#  define GOAL_CONFIG_SNMP 0
#endif


/****************************************************************************/
/* Statistics */
/****************************************************************************/
#ifndef GOAL_CONFIG_STATS
#  define GOAL_CONFIG_STATS 0
#endif

#ifndef GOAL_CONFIG_STATS_NAMES
#  define GOAL_CONFIG_STATS_NAMES 0
#endif


/****************************************************************************/
/* Task Management */
/****************************************************************************/
#ifndef GOAL_CONFIG_TASK
#  define GOAL_CONFIG_TASK 0
#endif

#ifndef GOAL_CONFIG_TASK_STACK_MONITOR
#  define GOAL_CONFIG_TASK_STACK_MONITOR 0
#endif

#ifndef GOAL_CONFIG_TASK_STACK_PAINT
#  define GOAL_CONFIG_TASK_STACK_PAINT 0x55
#endif

#ifndef GOAL_CONFIG_TASK_STACK_MONITOR_SLEEP_MS
#  define GOAL_CONFIG_TASK_STACK_MONITOR_SLEEP_MS (5 * GOAL_TIMER_SEC)
#endif


/****************************************************************************/
/* Timer */
/****************************************************************************/
#ifndef GOAL_CONFIG_TIMER_TS_NO_STUCK
#  define GOAL_CONFIG_TIMER_TS_NO_STUCK 0
#endif


/****************************************************************************/
/* CSAP Configuration */
/****************************************************************************/
#ifndef GOAL_CONFIG_CSAP
#  define GOAL_CONFIG_CSAP 0
#endif


/****************************************************************************/
/* TCP/IP Networking */
/****************************************************************************/
#ifndef GOAL_CONFIG_DHCP
#  define GOAL_CONFIG_DHCP 0
#endif

#ifndef GOAL_CONFIG_DHCP_EXT
#  define GOAL_CONFIG_DHCP_EXT 0
#endif

#ifndef GOAL_CONFIG_TCPIP_STACK
#  define GOAL_CONFIG_TCPIP_STACK 0
#endif

#ifndef GOAL_CONFIG_TCPIP_TCP
#  define GOAL_CONFIG_TCPIP_TCP 0
#endif

#ifndef GOAL_CONFIG_IP_STATS_NAMES
#  define GOAL_CONFIG_IP_STATS_NAMES 0
#endif

#ifndef GOAL_CONFIG_TCPIP_BSD_API
#  define GOAL_CONFIG_TCPIP_BSD_API 0
#endif

#ifndef GOAL_CONFIG_TCPIP_BSD_API_USE_LWIP_ERRNO
#  define GOAL_CONFIG_TCPIP_BSD_API_USE_LWIP_ERRNO 1
#endif

#ifndef GOAL_CONFIG_TCPIP_BSD_API_IGNORE_TS_STRUCT
#  define GOAL_CONFIG_TCPIP_BSD_API_IGNORE_TS_STRUCT 0
#endif

#ifndef GOAL_CONFIG_CTC_NET_CC
#  define GOAL_CONFIG_CTC_NET_CC 0
#endif

#ifndef GOAL_CONFIG_CTC_NET_AC
# define GOAL_CONFIG_CTC_NET_AC 0
#endif

#ifndef GOAL_CONFIG_NUM_INTERFACES
# define GOAL_CONFIG_NUM_INTERFACES 1
#endif


/****************************************************************************/
/* uthash Library */
/****************************************************************************/
#ifndef GOAL_CONFIG_FEAT_UTHASH
#  define GOAL_CONFIG_FEAT_UTHASH 0
#endif


/****************************************************************************/
/* Webserver */
/****************************************************************************/
#ifndef GOAL_CONFIG_WEBSERVER
#  define GOAL_CONFIG_WEBSERVER 0
#endif


/****************************************************************************/
/* Micro Core 2 Core */
/****************************************************************************/
#ifndef GOAL_CONFIG_RPC
#  define GOAL_CONFIG_RPC 0
#endif

#ifndef GOAL_CONFIG_GEN_RPC
#  define GOAL_CONFIG_GEN_RPC 0
#endif

#ifndef GOAL_CONFIG_MCTC_AC
#  define GOAL_CONFIG_MCTC_AC 0
#endif

#ifndef GOAL_CONFIG_MCTC_CC
#  define GOAL_CONFIG_MCTC_CC 0
#endif

#ifndef GOAL_CONFIG_GEN_CTC
#  define GOAL_CONFIG_GEN_CTC 0
#endif

#ifndef GOAL_CONFIG_GEN_CTC_AC
#  define GOAL_CONFIG_GEN_CTC_AC 0
#endif

#ifndef GOAL_CONFIG_GEN_CTC_CC
#  define GOAL_CONFIG_GEN_CTC_CC 0
#endif

#ifndef GOAL_CONFIG_MCTC
#  define GOAL_CONFIG_MCTC 0
#endif

#ifndef GOAL_CONFIG_MCTC_RAM
#  define GOAL_CONFIG_MCTC_RAM 0
#endif

#ifndef GOAL_CONFIG_MCTC_UART
#  define GOAL_CONFIG_MCTC_UART 0
#endif


/****************************************************************************/
/* Device Detection */
/****************************************************************************/
#ifndef GOAL_CONFIG_DD
#  define GOAL_CONFIG_DD 0
#endif


/****************************************************************************/
/* Media Interfaces */
/****************************************************************************/
#ifndef GOAL_CONFIG_MEDIA_MI_CMFS
#  define GOAL_CONFIG_MEDIA_MI_CMFS 0
#endif

#ifndef GOAL_CONFIG_MEDIA_MI_DM
#  define GOAL_CONFIG_MEDIA_MI_DM 0
#endif

#ifndef GOAL_CONFIG_MEDIA_MI_ETH
#  define GOAL_CONFIG_MEDIA_MI_ETH 0
#endif

#ifndef GOAL_CONFIG_MEDIA_MA_EVENT
#  define GOAL_CONFIG_MEDIA_MA_EVENT 0
#endif

#ifndef GOAL_CONFIG_MEDIA_MA_ID
#  define GOAL_CONFIG_MEDIA_MA_ID 0
#endif

#ifndef GOAL_CONFIG_MEDIA_MA_SPI
#  define GOAL_CONFIG_MEDIA_MA_SPI 0
#endif

#ifndef GOAL_CONFIG_MEDIA_MA_LED
#  define GOAL_CONFIG_MEDIA_MA_LED 0
#endif

#ifndef GOAL_CONFIG_MEDIA_MA_UART
#  define GOAL_CONFIG_MEDIA_MA_UART 0
#endif

#ifndef GOAL_CONFIG_MEDIA_MA_TEMPERATURE
#  define GOAL_CONFIG_MEDIA_MA_TEMPERATURE 0
#endif


/****************************************************************************/
/* Crypto Media Interface */
/****************************************************************************/
#ifndef GOAL_CONFIG_MI_CRYPT_AES128
#  define GOAL_CONFIG_MI_CRYPT_AES128 0
#endif

#ifndef GOAL_CONFIG_MI_CRYPT_GCM
#  define GOAL_CONFIG_MI_CRYPT_GCM 0
#endif

#ifndef GOAL_CONFIG_MI_CRYPT_SHA1
#  define GOAL_CONFIG_MI_CRYPT_SHA1 0
#endif


/****************************************************************************/
/* Generic Target Configuration */
/****************************************************************************/
#ifndef GOAL_CONFIG_TGT_TRACE
#  define GOAL_CONFIG_TGT_TRACE 0
#endif

#ifndef GOAL_CONFIG_TGT_TRACE_PIN
#  define GOAL_CONFIG_TGT_TRACE_PIN 0
#endif

#ifndef GOAL_CONFIG_TGT_TRACE_ITM
#  define GOAL_CONFIG_TGT_TRACE_ITM 0
#endif

#ifndef GOAL_CONFIG_TGT_TRACE_ITM_WITHOUT_PC
#  define GOAL_CONFIG_TGT_TRACE_ITM_WITHOUT_PC 0
#endif

#ifndef GOAL_CONFIG_TGT_64BIT
#  define GOAL_CONFIG_TGT_64BIT 0
#endif

#ifndef GOAL_CONFIG_SRAM_EXT_IRJ45
#  define GOAL_CONFIG_SRAM_EXT_IRJ45 0
#endif


/****************************************************************************/
/* Time Synchronization */
/****************************************************************************/
#ifndef GOAL_PTP
#  define GOAL_PTP 0
#endif


/****************************************************************************/
/* Address Conflict Detection */
/****************************************************************************/
#ifndef GOAL_CONFIG_ACD
#  define GOAL_CONFIG_ACD 0
#endif


/****************************************************************************/
/* Transport Layer Security */
/****************************************************************************/
#ifndef GOAL_CONFIG_TLS
#   define GOAL_CONFIG_TLS 0
#endif


/****************************************************************************/
/* GOAL OPC UA Wrapper for open62541 */
/****************************************************************************/
#ifndef GOAL_CONFIG_OPCUA_OPEN62541
#   define GOAL_CONFIG_OPCUA_OPEN62541 0
#endif


/****************************************************************************/
/* Industrial Ethernet */
/****************************************************************************/
/**< GOAL OPC-UA stack */
#ifndef GOAL_CONFIG_OPCUA
#   define GOAL_CONFIG_OPCUA 0
#endif

/**< GOAL Modbus stack */
#ifndef GOAL_CONFIG_MODBUS
  #define GOAL_CONFIG_MODBUS 0
#endif

/**< GOAL EtherNet/IP stack */
#ifndef GOAL_CONFIG_EIP
  #define GOAL_CONFIG_EIP 0
#endif

/**< GOAL Profinet stack */
#ifndef GOAL_CONFIG_PNIO
  #define GOAL_CONFIG_PNIO 0
#endif

/**< Device Level Ring Protocol */
#ifndef GOAL_DLR
  #define GOAL_DLR 0
#endif

#ifndef GOAL_DLR_SUPERVISOR
  #define GOAL_DLR_SUPERVISOR 0
#endif

#endif /* GOAL_CONFIG_DEF_H */
