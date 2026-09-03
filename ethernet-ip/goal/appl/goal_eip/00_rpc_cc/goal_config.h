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

#define GOAL_CONFIG_NET_ADDR_IP_DEFAULT      0xC0A80103 /**< 192.168.1.3 */
#define GOAL_CONFIG_NET_ADDR_MASK_DEFAULT    0xFFFFFF00 /**< 255.255.255.0 */
#define GOAL_CONFIG_NET_ADDR_GATEWAY_DEFAULT 0x00000000 /**< 0.0.0.0 */

#endif /* GOAL_CONFIG_H */
