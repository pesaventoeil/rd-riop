/** @file
 *
 * @brief OpENer LED handler
 *
 * This module implements the LED handler.
 *
 * @copyright
 * Copyright 2010-2019 port GmbH Halle/Saale.
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


#ifndef OPENER_LED_H
#define OPENER_LED_H


/****************************************************************************/
/* Public functions */
/****************************************************************************/
GOAL_STATUS_T opener_ledInit(
    EIP_INSTANCE_T *pEip                        /**< EtherNet/IP instance */
);

GOAL_STATUS_T opener_ledReset(
    EIP_INSTANCE_T *pEip                        /**< EtherNet/IP instance */
);

GOAL_STATUS_T opener_ledLoop(
    EIP_INSTANCE_T *pEip                        /**< EtherNet/IP instance */
);

GOAL_STATUS_T opener_ledSet(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_LED_ID_T ledId,                         /**< LED ID */
    EIP_LED_STATE_T newState                    /**< new LED state */
);

GOAL_STATUS_T opener_ledBiColorSet(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_LED_BC_ID_T ledId,                      /**< Bi-Color LED ID */
    EIP_LED_BC_STATE_T newState                 /**< new Bi-Color LED state */
);


#endif /* OPENER_LED_H */
