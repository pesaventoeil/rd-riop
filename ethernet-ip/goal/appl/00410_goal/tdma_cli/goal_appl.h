/** @file
 *
  * @brief header for goal DD usage example
 *
 * This application demonstrates the integration of DD.
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
#ifndef GOAL_APPL_H
#define GOAL_APPL_H

#include <goal_includes.h>


/****************************************************************************/
/* Defines */
/****************************************************************************/

#define APPL_VERSION "0.0.0.1"


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T appl_cmChg(
    uint32_t modId,                             /**< module ID */
    uint32_t varId,                             /**< variable Id */
    GOAL_CM_VAR_T *pVar                         /**< variable pointer */
);


/****************************************************************************/
/* Configuration Variables Management */
/****************************************************************************/
#define APPL_CM_MOD_ID      0                   /**< CM module ID */
#define APPL_CM_STRING_LEN  20                  /**< CM string length */

#define APPL_CM_VARS \
/*              Name,                Data type,       Max. size,          Validation Cb, Change Cb */   \
    GOAL_CM_VAR(APPL_CM_VAR_UINT8,   GOAL_CM_UINT8,   sizeof(uint8_t),    NULL,          appl_cmChg),   \
    GOAL_CM_VAR(APPL_CM_VAR_UINT16,  GOAL_CM_UINT16,  sizeof(uint16_t),   NULL,          appl_cmChg),   \
    GOAL_CM_VAR(APPL_CM_VAR_UINT32,  GOAL_CM_UINT32,  sizeof(uint32_t),   NULL,          appl_cmChg),   \
    GOAL_CM_VAR(APPL_CM_VAR_GENERIC, GOAL_CM_GENERIC, APPL_CM_STRING_LEN, NULL,          appl_cmChg),   \
    GOAL_CM_VAR(APPL_CM_VAR_VERSION, GOAL_CM_STRING,  APPL_CM_STRING_LEN, NULL,          NULL)

/* generate 'enum APPL_CM_VARS_ID_T' that contains all variable names */
#include <goal_cm_id.h>
GOAL_CM_VAR_IDS(APPL_CM_VARS_ID_T, APPL_CM_VARS);


#endif /* GOAL_APPL_H */

