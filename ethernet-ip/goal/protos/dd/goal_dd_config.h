/** @file
 *
 * @brief Header for device detection
 *
 * @copyright
 * Copyright 2013-2021 port GmbH Halle/Saale.
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

#ifndef GOAL_DD_CONFIG_H
#define GOAL_DD_CONFIG_H

uint32_t dd_cmGetCustomerID(
    void
);

GOAL_STATUS_T dd_cmSetCustomerID(
    uint32_t cid                                /**< customer ID */
);

uint8_t *dd_cmGetModuleName(
    void
);

GOAL_STATUS_T dd_cmSetModuleName(
    uint8_t *str                                /**< module name */
);

uint8_t dd_cmGetModuleState(
    void
);

GOAL_STATUS_T dd_cmSetModuleState(
    uint8_t moduleState                         /**< module state */
);

uint8_t *dd_cmGetHostName(
    void
);

GOAL_STATUS_T dd_cmGetDhcp(
    GOAL_BOOL_T *pFlag                          /**< dhcp state */
);

uint32_t dd_cmGetVariablesCnt(
    void
);

uint32_t dd_cmGetModuleId(
    uint32_t idx                                /**< module index */
);

GOAL_STATUS_T dd_cmFeaturesGet(
    uint32_t *pFeatures                         /**< result value */
);

GOAL_STATUS_T dd_cmFeaturesSet(
    uint32_t bitmaskFeatures                    /**< bitmask with feature disables */
);

GOAL_STATUS_T dd_cmCryptActivationSet(
    GOAL_BOOL_T flgActivate                     /**< crypto activation flag */
);

GOAL_STATUS_T dd_cmCryptPasswordSet(
    uint8_t *str                                /**< crypto password */
);

#endif /* GOAL_DD_CONFIG_H */

