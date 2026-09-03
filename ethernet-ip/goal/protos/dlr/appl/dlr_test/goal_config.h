/** @file
 *
 * @brief GOAL Configuration File
 *
 * This file controls various components and settings of the Generic Open
 * Abstraction Layer (GOAL).
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

#ifndef GOAL_CONFIG_H
#define GOAL_CONFIG_H


#define GOAL_CONFIG_LOGGING 1
#define GOAL_CONFIG_ETHERNET 1
#define GOAL_CONFIG_ETH_STATS 1
#define GOAL_CONFIG_NVM 1
#define GOAL_CONFIG_LOCKING 1
#define GOAL_CONFIG_TIMER 1
#define GOAL_CONFIG_TCPIP_STACK 1
#define GOAL_CONFIG_PHY_DETECTION 1
#define GOAL_CONFIG_TASK 1

#define GOAL_DEVICE_MAC_ADDR { 0x00, 0x45, 0x56, 0x78, 0x9a, 0xbc }


#define GOAL_CONFIG_LOGGING_BUFFER_SIZE 4096
/* #define GOAL_CONFIG_LOGGING_TARGET_RAW 1 */
#define GOAL_CONFIG_LOGGING_TARGET_NONBLOCK 1

#define GOAL_CONFIG_MAX_TIMERS_LOW 24
#define GOAL_CONFIG_MAX_TIMERS_HIGH 8

#endif /* GOAL_CONFIG_H */
