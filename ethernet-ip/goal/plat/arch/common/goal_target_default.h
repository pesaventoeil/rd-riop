/** @file
 *
 * @brief GOAL target default values
 *
 * This header defines default values for target parameters which are not
 * defined by the target but ay needed in some device or build configurations
 * for compliance.
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

#ifndef GOAL_TARGET_DEFAULT_H
#define GOAL_TARGET_DEFAULT_H

#include "goal_config.h"


/****************************************************************************/
/* Ethernet */
/****************************************************************************/
#ifndef GOAL_TGT_MTU_SIZE
#  define GOAL_TGT_MTU_SIZE     GOAL_ETH_MTU_SIZE_DFL
#endif

#ifndef GOAL_TARGET_SWITCH_PORT_COUNT
#  define GOAL_TARGET_SWITCH_PORT_COUNT GOAL_TARGET_ETH_PORT_COUNT
#endif

#endif /* GOAL_TARGET_DEFAULT_H */
