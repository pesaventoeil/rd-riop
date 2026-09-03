/** @file
 *
 * @brief Low Level Interface of the DLR stack
 *
 * This module connects the DLR stack to GOAL.
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

#ifndef GOAL_TARGET_DLR_H
#define GOAL_TARGET_DLR_H


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_targetDlrInit(
    GOAL_DLR_T *pHdlDlr                         /**< DLR handle */
);

GOAL_STATUS_T goal_targetDlrInitIsr(
    void
);

void goal_targetDlrIsr(
    void
);
#endif /* GOAL_TARGET_DLR_H */
