/** @file
 *
 * @brief Ethernet/IP system configuration
 *
 * This file lists the general Ethernet/IP system configuration.
 *
 * @copyright
 * Copyright 2020 port GmbH Halle/Saale.
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

#ifndef EIP_SYSTEM_CONF_H
#define EIP_SYSTEM_CONF_H


/****************************************************************************/
/* Defines */
/****************************************************************************/
/** Logging Options */
/**
 * To activate the logging option setup GOAL_CONFIG_LOGGING in goal_config.h.
 *
 * The following trace level are supported:
 *
 * EIP_LOGGING_TRACE_LEVEL_ERROR - log error messages
 * EIP_LOGGING_TRACE_LEVEL_WARNING - log warnings
 * EIP_LOGGING_TRACE_LEVEL_STATE - log state messages
 * EIP_LOGGING_TRACE_LEVEL_INFO - log informative messages
 */
#define EIP_LOGGING_TRACE_LEVEL (EIP_LOGGING_TRACE_LEVEL_ERROR | EIP_LOGGING_TRACE_LEVEL_INFO)

/**< IP changes requires reset */
/* #define EIP_CIP_IPCHANGE_RESET 1 */

/**< Number of supported implicit connections.
 * This number must be equal or greater than the number of CIP connections
 * supported by this device.
 */
#define EIP_CIP_NUM_IMPLICIT_CONNS 6

/**< Number of supported explicit connections. */
/**
 * This number should be greater than six (ODVA's PUB 70).
 */
#define EIP_CIP_NUM_EXPLICIT_CONNS 6

/**< Size of the reply data buffer (in bytes) */
#define EIP_MESSAGE_DATA_REPLY_BUFFER 1500

/**< Number of sessions that can be handled simultaneously */
#define EIP_NUMBER_OF_SUPPORTED_SESSIONS 20

/**< Time in ms of the timer in this implementation */
#define EIP_TIMER_TICK_MS 10

/**< RUN IDLE data is sent with consumed data */
/**
 * Default:
 * #define EIP_CONSUMED_DATA_HAS_RUN_IDLE_HEADER 1
 */
#define EIP_CONSUMED_DATA_HAS_RUN_IDLE_HEADER 1

/**< RUN IDLE data is to be sent with produced data */
/**
 * Default:
 * #define EIP_PRODUCED_DATA_HAS_RUN_IDLE_HEADER 0
 */
#define EIP_PRODUCED_DATA_HAS_RUN_IDLE_HEADER 0

/**< Ethernet link object options */
/**
 * EIP_ETH_LINK_INTERNAL - device has internal Interface (Embedded Switch)
 * EIP_ETH_LINK_CONTROL - Speed & Duplex mode is settable (Attribute 6)
 * EIP_ETH_LINK_RESET - change of Speed & Duplex mode requires reset
 * EIP_ETH_LINK_AUTONEG - Phy supports Auto Negotiation
 * EIP_ETH_LINK_AUTOMDIX - Phy supports Auto-MDIX
 * EIP_ETH_LINK_10_FULL - Phy upports 10 Mbit/s Full Duplex
 * EIP_ETH_LINK_10_HALF - Phy upports 10 Mbit/s Half Duplex
 * EIP_ETH_LINK_100_FULL - Phy upports 100 Mbit/s Full Duplex
 * EIP_ETH_LINK_100_HALF - Phy upports 100 Mbit/s Half Duplex
 * EIP_ETH_LINK_1000_FULL - Phy upports 1000 Mbit/s Full Duplex
 */
#define EIP_ETH_LINK_INTERNAL
#define EIP_ETH_LINK_CONTROL
/* #define EIP_ETH_LINK_RESET */
#define EIP_ETH_LINK_AUTONEG
#define EIP_ETH_LINK_AUTOMDIX
#define EIP_ETH_LINK_10_FULL
#define EIP_ETH_LINK_10_HALF
#define EIP_ETH_LINK_100_FULL
#define EIP_ETH_LINK_100_HALF
/* #define EIP_ETH_LINK_1000_FULL */

/**< Ethernet Link object supports media counter and interface counter */
#define EIP_ETH_LINK_COUNTERS 1

/**< QOS Object support */
#define EIP_CONFIG_QOS 1
/* #define EIP_CONFIG_QOS_VLAN_SUPPORT 1 */

#endif /* EIP_SYSTEM_CONF_H */
