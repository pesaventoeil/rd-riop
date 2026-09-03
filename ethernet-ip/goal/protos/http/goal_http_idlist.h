
/** @file
 *
 *  Http webserver channel list  module header
 *
 * This application runs a simple HTTP server that hosts a demo HTML page.
 * It can be accessed via web browser by browsing the IP address of this device.
 *
 * @copyright
 * Copyright 2010-2019 port GmbH Halle/Saale.
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


#ifndef GOAL_HTTP_IDLIST_H
#define GOAL_HTTP_IDLIST_H

GOAL_STATUS_T goal_httpChannelId(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    uint32_t *pId                               /**< [out] channel id */
);

GOAL_STATUS_T goal_httpChannelEntryById(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    uint32_t id,                                /**< channel id */
    GOAL_HTTP_CHN_T **ppEntry                   /**< [out] channel list entry */
);

#endif

