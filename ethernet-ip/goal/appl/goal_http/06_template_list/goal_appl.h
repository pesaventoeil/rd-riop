/** @file
 *
 * @brief Http webserver Example
 *
 * This application runs a simple HTTP server that hosts a demo HTML page.
 * It can be accessed via web browser by browsing the IP address of this device.
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
#ifndef GOAL_APPL_H
#define GOAL_APPL_H

#include <goal_includes.h>


/****************************************************************************/
/* Webserver data as defines */
/****************************************************************************/

/** application template list defintion */
typedef struct {
    char name[32];                              /**< list name */
    char value[32];                             /**< value */
    void *sublist;                              /**< pionter to sublist */
} GOAL_APPL_LIST_T;


#endif /* GOAL_APPL_H */
