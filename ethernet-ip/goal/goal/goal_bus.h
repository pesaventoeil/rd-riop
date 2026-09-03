/** @file
 *
 * @brief GOAL Bus Handling
 *
 * This module implements a generic bus interface.
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

#ifndef GOAL_BUS_H
#define GOAL_BUS_H

#include "goal_includes.h"


/****************************************************************************/
/* data types */
/****************************************************************************/
struct GOAL_BUS_T;

typedef GOAL_STATUS_T (* GOAL_BUS_FUNC_READ)(
    struct GOAL_BUS_T *pBus,                    /**< bus handle */
    uint32_t addr,                              /**< read address */
    uint32_t reg,                               /**< register */
    char *pVal,                                 /**< buffer address */
    uint32_t len                                /**< read length */
);

typedef GOAL_STATUS_T (* GOAL_BUS_FUNC_WRITE)(
    struct GOAL_BUS_T *pBus,                    /**< bus handle */
    uint32_t addr,                              /**< write address */
    uint32_t reg,                               /**< register */
    char *pVal,                                 /**< buffer address */
    uint32_t len                                /**< write length */
);

typedef struct GOAL_BUS_T {
    GOAL_BUS_FUNC_READ read;                    /**< data read */
    GOAL_BUS_FUNC_WRITE write;                  /**< data write */
    void *pData;                                /**< bus specific data */
} GOAL_BUS_T;


#endif /* GOAL_BUS_H */
