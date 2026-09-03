/** @file
 *
 * @brief emitter for goal lm emitter over RAW
 *
 * @details
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
#ifndef GOAL_LM_EMIT_RAW_H
#define GOAL_LM_EMIT_RAW_H


/****************************************************************************/
/* Defines */
/****************************************************************************/

#define EMIT_BUF_SIZE GOAL_NETBUF_SIZE          /**< size the emitter can buffer */


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_lmEmitRawReg(
    void
);

#endif /* GOAL_LM_EMIT_RAW_H */
