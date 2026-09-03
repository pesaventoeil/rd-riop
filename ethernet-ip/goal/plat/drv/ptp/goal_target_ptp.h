/** @file
 *
 * @brief Low Level Interface of PTP stack
 *
 * This provides the API to the target specific functions used by the PTP stack.
 *
 * @copyright
 * Copyright 2010-2016.
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

#ifndef GOAL_TARGET_PTP_H
#define GOAL_TARGET_PTP_H

#include <goal_includes.h>


/****************************************************************************/
/* Public Functions */
/****************************************************************************/
GOAL_STATUS_T goal_targetPtpInit(
    void
);

GOAL_STATUS_T goal_targetPtpUpdateTime(
    GOAL_BOOL_T add,                            /**< add offset to current time */
    int64_t secOffset,                          /**< offset of full seconds */
    int32_t nsecOffset                          /**< offset of sub-seconds */
);

GOAL_STATUS_T goal_targetPtpSetTime(
    uint64_t sec,                               /**< full seconds */
    uint32_t nsec                               /**< sub-seconds in ns */
);

GOAL_STATUS_T goal_targetPtpGetTime(
    uint64_t *pSec,                             /**< full seconds */
    uint32_t *pNsec                             /**< sub-seconds in ns */
);

void goal_targetPtpIsr(
    void
);

GOAL_STATUS_T goal_targetPtpUpdateFreq(
    double fac                                  /**< correction factor */
);

GOAL_BOOL_T goal_targetPtpIsStable(
    void
);

GOAL_STATUS_T goal_targetPtpCycleCallbackStart(
    uint64_t startTime,                         /**< absolute start time of first cycle in ns */
    uint64_t cycleTime,                         /**< cycle time in ns */
    GOAL_FUNC_NORET_T cbFunc,                   /**< callback handler */
    void *pCbArg                                /**< callback handler argument */
);

GOAL_STATUS_T goal_targetPtpCycleCallbackStop(
    void
);

#endif /* GOAL_TARGET_PTP_H */
