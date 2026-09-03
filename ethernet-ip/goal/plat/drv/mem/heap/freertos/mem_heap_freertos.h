/** @file
 *
 * @brief
 * Driver for FreeRotos HEAP
 *
 * @details
 * This is a wrapper for allocating heap memory by FreeRTOS API.
 *
 * @copyright
 * Copyright 2010-2022 port GmbH Halle/Saale.
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
#ifndef MEM_HEAP_FREERTOS_H
#define MEM_HEAP_FREERTOS_H

#ifndef GOAL_CONFIG_FREERTOS_HEAP_SIZE
  #define GOAL_CONFIG_FREERTOS_HEAP_SIZE configTOTAL_HEAP_SIZE
#endif


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_drvMemHeapCreate(
    void
);
#endif /* MEM_HEAP_FREERTOS_H */
