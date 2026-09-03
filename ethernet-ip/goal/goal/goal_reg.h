/** @file
 *
 * @brief GOAL Register Handling
 *
 * This module implements a generic register handling interface.
 *
 * @copyright
 * Copyright 2010-2017.
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

#ifndef GOAL_REG_H
#define GOAL_REG_H

#include "goal_includes.h"


/****************************************************************************/
/* Defines */
/****************************************************************************/
#define GOAL_REG_FLG_RW          (1 << 1)       /**< flag: RW access */
#define GOAL_REG_FLG_RO          (1 << 2)       /**< flag: read-only access */
#define GOAL_REG_FLG_WO          (1 << 3)       /**< flag: write-only access */


/****************************************************************************/
/* Structures */
/****************************************************************************/
/** register table entry */
typedef struct {
    uint32_t reg;                               /**< register address */
    uint8_t shift;                              /**< shift count */
    uint32_t mask;                              /**< bit mask */
    uint16_t flags;                             /**< flags */
} GOAL_REG_T;

/** command to register mapping table entry */
typedef struct {
    uint32_t cmd;                               /**< command */
    uint32_t reg;                               /**< register */
} GOAL_REG_CMD_T;


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_regGet(
    GOAL_REG_T *pTable,                         /**< register table */
    GOAL_BUS_T *pBus,                           /**< bus handle */
    uint32_t addr,                              /**< address */
    uint32_t reg,                               /**< register/offset */
    uint32_t *pVal                              /**< value ptr */
);

GOAL_STATUS_T goal_regSet(
    GOAL_REG_T *pTable,                         /**< register table */
    GOAL_BUS_T *pBus,                           /**< bus handle */
    uint32_t addr,                              /**< address */
    uint32_t reg,                               /**< register/offset */
    uint32_t val                                /**< value */
);

GOAL_STATUS_T goal_regCmd(
    GOAL_REG_T *pTable,                         /**< register table */
    GOAL_BUS_T *pBus,                           /**< bus handle */
    GOAL_REG_CMD_T *pTableCmd,                  /**< command map table */
    uint32_t addr,                              /**< address */
    uint32_t cmd,                               /**< command ID */
    GOAL_BOOL_T wrFlag,                         /**< write flag */
    uint32_t *pVal                              /**< value ptr */
);


#endif /* GOAL_REG_H */
