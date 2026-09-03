
/** @file
 *
 *  Http webserver parsing module header
 *
 * This application runs a simple HTTP server that hosts a demo HTML page.
 * It can be accessed via web browser by browsing the IP address of this device.
 *
 * @copyright
 * Copyright 2010-2017 port GmbH Halle/Saale.
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


#ifndef GOAL_HTTP_PARSER_H
#define GOAL_HTTP_PARSER_H


/****************************************************************************/
/* Defines */
/****************************************************************************/

/** MIME types for POST request */
typedef enum {
    GOAL_HTTP_POST_M_NO_SUPP = 0,               /**< not supported */
    GOAL_HTTP_POST_M_MULTIPART,                 /**< mulipart/form-data */
    GOAL_HTTP_POST_M_URLENC,                    /**< application/x-www-form-urlencoded */
    GOAL_HTTP_POST_M_JSON                       /**< application/json */
} GOAL_HTTP_POST_MIME_T;


/****************************************************************************/
/* Prototypes */
/****************************************************************************/

GOAL_STATUS_T goal_httpParse(
    GOAL_HTTP_T         *pInst,                 /**< instance pointer */
    GOAL_HTTP_PARSE_T   *pInfoParse             /**< pointer to parser information struct */
);

#endif /* GOAL_HTTP_PARSER_H */
