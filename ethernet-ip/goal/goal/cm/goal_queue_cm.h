/** @file
 *
 * @brief queue configuration
 *
 * @copyright
 * Copyright 2021 port GmbH Halle/Saale.
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
#ifndef GOAL_QUEUE_CM_H
#define GOAL_QUEUE_CM_H

#include <goal_includes.h>


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_queueRegCmVars(
    void
);


/****************************************************************************/
/* Configuration Variables Management */
/****************************************************************************/

#define GOAL_CM_QUEUE_MOD_ID      42

#define GOAL_CM_QUEUE_VARS \
/*              Name,              Data type,       Max. size,        Validation Cb, Change Cb */   \
    GOAL_CM_VAR(QUEUE_CM_VAR_SMALLBUFSIZE, GOAL_CM_INT16, 2, NULL, NULL), \
    GOAL_CM_VAR(QUEUE_CM_VAR_SMALLBUFNUM, GOAL_CM_INT16, 2, NULL, NULL), \
    GOAL_CM_VAR(QUEUE_CM_VAR_MEDBUFSIZE, GOAL_CM_INT16, 2, NULL, NULL), \
    GOAL_CM_VAR(QUEUE_CM_VAR_MEDBUFNUM, GOAL_CM_INT16, 2, NULL, NULL), \
    GOAL_CM_VAR(QUEUE_CM_VAR_BIGBUFSIZE, GOAL_CM_INT16, 2, NULL, NULL), \
    GOAL_CM_VAR(QUEUE_CM_VAR_BIGBUFNUM, GOAL_CM_INT16, 2, NULL, NULL)

enum GOAL_CM_QUEUE_VAR_IDS {
    QUEUE_CM_VAR_SMALLBUFSIZE = 0,
    QUEUE_CM_VAR_SMALLBUFNUM = 1,
    QUEUE_CM_VAR_MEDBUFSIZE = 2,
    QUEUE_CM_VAR_MEDBUFNUM = 3,
    QUEUE_CM_VAR_BIGBUFSIZE = 4,
    QUEUE_CM_VAR_BIGBUFNUM = 5,
    GOAL_CM_QUEUE_VAR_LAST
};

enum GOAL_CM_QUEUE_VIRT_VAR_IDS {
    GOAL_CM_QUEUE_VIRT_VAR_LAST
};

#endif /* GOAL_QUEUE_CM_H */

