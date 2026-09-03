/** @file
 *
 * @brief Ethernet/IP application header
 *
 * This application defines some Assembly objects and connections. The first
 * byte of the Output Assembly is mapped to the LEDs. The first byte of the
 * Input Assembly contains a bitmap of the pressed buttons. Additionally, a
 * application specific CIP class is registered with parameters, listed in
 * the Instance Table.
 *
 * @copyright
 * Copyright 2025-2026 NXP
 *
 * NXP Confidential and Proprietary. This software is owned or controlled by NXP
 * and may only be used strictly in accordance with the applicable license
 * terms. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you
 * have read, and that you agree to comply with and are bound by, such license
 * terms. If you do not agree to be bound by the applicable license terms, then
 * you may not retain, install, activate or otherwise use the software.
 *
 */

#ifndef GOAL_APP_EIP_H
#define GOAL_APP_EIP_H


/****************************************************************************/
/* Local defines */
/****************************************************************************/
#define GOAL_APP_ASM_ID_INPUT        100        /**< Input Assembly ID */
#define GOAL_APP_ASM_ID_OUTPUT       150        /**< Output Assembly ID */
#define GOAL_APP_ASM_ID_CONFIG       151        /**< Config Assembly ID */
#define GOAL_APP_ASM_ID_HEARTBEAT_IO 152        /**< Heartbeat Assembly ID  (Input Only) */
#define GOAL_APP_ASM_ID_HEARTBEAT_LO 153        /**< Heartbeat Assembly ID  (Listen Only) */
#define GOAL_APP_IOCON_NUM           6          /**< number of simultaneous connections for Input Only connections */
#define GOAL_APP_LOCON_NUM           6          /**< number of simultaneous connections for Listen Only connections */
#define GOAL_APP_ASM_SIZE_INPUT      97         /**< Input Assembly Data */
#define GOAL_APP_ASM_SIZE_OUTPUT     97         /**< Output Assembly Data */
#define GOAL_APP_ASM_SIZE_CONFIG     10         /**< Config Assembly Data */

#endif /* GOAL_APP_EIP_H */
