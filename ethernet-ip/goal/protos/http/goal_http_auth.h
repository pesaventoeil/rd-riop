/** @file
 *
 * @brief Goal HTTP authentication
 *
 * This is the goal HTTP authentication handler.
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


#ifndef GOAL_HTTP_AUTH_H
#define GOAL_HTTP_AUTH_H


/****************************************************************************/
/* Defines */
/****************************************************************************/

#define GOAL_HTTP_AUTH_BAS_STR "Authorization: Basic"


/****************************************************************************/
/* Data types */
/****************************************************************************/


/****************************************************************************/
/* Prototypes */
/****************************************************************************/

GOAL_STATUS_T goal_httpAuthBasCheck(
    GOAL_HTTP_T         *pInst,                 /**< instance pointer */
    GOAL_HTTP_HDL_T     hdlRes,                 /**< resource handle */
    char                *strAuth,               /**< authentication string */
    uint32_t            lenAuth                 /**< length of string */
);

#endif /* GOAL_HTTP_AUTH_H */

