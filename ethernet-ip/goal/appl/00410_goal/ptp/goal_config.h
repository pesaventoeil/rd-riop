/** @file
 *
 * @brief GOAL Configuration File
 *
 * This file controls various components and settings of the Generic Open
 * Abstraction Layer (GOAL).
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

#ifndef GOAL_CONFIG_H
#define GOAL_CONFIG_H

#define GOAL_CONFIG_LOGGING 1
#define GOAL_CONFIG_ETHERNET 1
#define GOAL_CONFIG_ETH_STATS 1
#define GOAL_CONFIG_TCPIP_STACK 1

#define GOAL_CONFIG_LOGGING_BUFFER_SIZE 4096
/* #define GOAL_CONFIG_LOGGING_TARGET_RAW 1 */
#define GOAL_CONFIG_LOGGING_TARGET_NONBLOCK 1

#define GOAL_LOG_ID_PTP (1<<0)

#endif /* GOAL_CONFIG_H */
