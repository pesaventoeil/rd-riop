/** @file
 *
 * @brief Header for device detection
 *
 * @copyright
 * Copyright 2013-2021 port GmbH Halle/Saale.
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

#ifndef GOAL_DD_GETLIST_H
#define GOAL_DD_GETLIST_H

struct Packet *dd_processGetList(
    struct Packet *request,                     /**< packet to process */
    struct Packet *reply                        /**< response packet */
);

#define DD_GETLIST_TYPE_DESCRIPTION 0           /**< GET LIST Request Type Description */
#define DD_GETLIST_TYPE_MODULES 1               /**< GET LIST Request Type Modulelist */
#define DD_GETLIST_TYPE_VARIABLES 2             /**< GET LIST Request Type Variables */

#define DD_GETLIST_EOL 0xffffffff               /**< end of variables */

#endif /* GOAL_DD_GETLIST_H */

