/** @file
 *
 * @brief Low Level Interface of GOAL
 *
 * This module defines prototypes of FreeRTOS which must be implemented
 * by GOAL.
 *
 * @copyright
 * Copyright 2010-2017 port GmbH Halle/Saale.
 * This software is protected Intellectual Property and may only be used
 * according to the license agreement.
 */



#ifndef GOAL_TARGET_EXT_H
#define GOAL_TARGET_EXT_H


#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "portmacro.h"


/****************************************************************************/
/* External Prototypes */
/****************************************************************************/
#define GOAL_TARGET_TASK_MAX_DELAY portMAX_DELAY /**< task delay */


/****************************************************************************/
/* External Prototypes */
/****************************************************************************/
void vApplicationGetTimerTaskMemory(
    StaticTask_t **ppxTimerTaskTCBBuffer,       /**< pointer to Task struct reference */
    StackType_t **ppxTimerTaskStackBuffer,      /**< pointer to stack buffer */
    uint32_t *pulTimerTaskStackSize             /**< size of stack buffer */
);


void vApplicationGetIdleTaskMemory(
    StaticTask_t **ppxIdleTaskTCBBuffer,        /**< pointer to Task struct reference */
    StackType_t **ppxIdleTaskStackBuffer,       /**< pointer to stack buffer */
    uint32_t *pulIdleTaskStackSize              /**< size of stack buffer */
);

void xPortSysTickHandler(
    void
);

void vApplicationTickHook(
    void
);

#endif /* GOAL_TARGET_EXT_H */
