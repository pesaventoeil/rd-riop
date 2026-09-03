/** @file
 *
 * @brief emitter for goal lm emitter (exception log)
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
#ifndef GOAL_LM_EMIT_EXCEPTION_LOG_H
#define GOAL_LM_EMIT_EXCEPTION_LOG_H


/****************************************************************************/
/* Defines */
/****************************************************************************/

#ifndef GOAL_CONFIG_EXLOG_DEBUG
# define GOAL_CONFIG_EXLOG_DEBUG 0
#endif

#define EMIT_BUF_SIZE GOAL_NETBUF_SIZE          /**< size the emitter can buffer */
#define GOAL_EXCEPTIONLOG_REGION_STATE_INVALID (0xFFFF) /**< region state is unknown */
#define GOAL_EXCEPTIONLOG_REGION_STATE_EMPTY   (0xFFF0) /**< region is empty */
#define GOAL_EXCEPTIONLOG_REGION_STATE_USED    (0xFF00) /**< region is currently used for storage */
#define GOAL_EXCEPTIONLOG_REGION_STATE_OLD     (0xF000) /**< region is kept but not used */
#define GOAL_EXCEPTIONLOG_REGION_STATE_ERASING (0x0000) /**< region is ready for erasing */

#define GOAL_EXCEPTIONLOG_HEADER_ID            (0xcad9) /**< signature for header */


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_lmEmitExceptionLogReg(
    void
);

#if GOAL_CONFIG_EXLOG_DEBUG == 1
void goal_lmEmitExceptionLogSwitchCb(
    uint32_t writePosition                      /**< write position */
);

void goal_lmEmitExceptionLogEraseCb(
    uint32_t writePosition                      /**< write position */
);
#endif

#endif /* GOAL_LM_EMIT_EXCEPTION_LOG_H */
