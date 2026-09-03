/** @file
 *
 * @brief
 * CAN Simple I/O Example
 *
 * @details
 * This application provides a simple IO example for CAN.
 *
 * @copyright
 * Copyright 2010-2024 port GmbH Halle/Saale.
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

#include <goal_includes.h>
#include <goal_can.h>


/****************************************************************************/
/* Local Defines */
/****************************************************************************/
#define APPL_CAN_ID 0x20                        /**< CAN ID */
#define APPL_CAN_LINE 0                         /**< CAN line (interface) */
#define APPL_CAN_MESSAGE_LEN 8                  /**< application message length */


/****************************************************************************/
/* Local Prototypes */
/****************************************************************************/
static void appl_canCallback(
    GOAL_CANEVENT_T event,                      /**< CAN event ID */
    GOAL_CAN_MSG_T *pData,                      /**< CAN message data */
    uint32_t canLine                            /**< CAN line */
);


/****************************************************************************/
/* Local Variables */
/****************************************************************************/
static GOAL_TIMESTAMP_T tsNext;                 /**< next iteration time */
static GOAL_CAN_MSG_T canMsgTx;                 /**< CAN TX message */


/****************************************************************************/
/** Application Setup
 *
 * Setup the application.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T appl_setup(
    void
)
{
    GOAL_STATUS_T res;                          /* result */
    uint32_t value;                             /* temp value */

    /* CAN pre-initialization */
    res = goal_canPreInit();
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to pre-init CAN");
        return res;
    }

    /* initialize CAN instance */
    res = goal_canInit();
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to initialize CAN");
        return res;
    }

    /* set baudrate */
    value = GOAL_CAN_BAUDRATE_500;
    res = goal_canCmd(GOAL_CAN_CMD_BAUDRATE, GOAL_TRUE, APPL_CAN_LINE, &value);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("error setting baud rate");
        return res;
    }

    /* register CAN callback */
    goal_canRegisterCallback(appl_canCallback, APPL_CAN_LINE);

    /* initialize timestamp */
    tsNext = goal_timerTsGet() + GOAL_TIMER_SEC;

    GOAL_MEMSET(&canMsgTx, 0, sizeof(GOAL_CAN_MSG_T));
    canMsgTx.messageID = APPL_CAN_ID;
    canMsgTx.dataLen = APPL_CAN_MESSAGE_LEN;

    /* activate the CAN controller */
    value = GOAL_CAN_CMD_STATE_ON;
    res = goal_canCmd(GOAL_CAN_CMD_STATE, GOAL_TRUE, APPL_CAN_LINE, &value);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to start CAN");
        return res;
    }

    /* CAN configuration and startup successful */
    goal_logInfo("CAN initialized and started");

    return res;
}


/****************************************************************************/
/** CAN Callback
 *
 * This callback reports events from CAN interface(s).
 */
static void appl_canCallback(
    GOAL_CANEVENT_T event,                      /**< CAN event ID */
    GOAL_CAN_MSG_T *pData,                      /**< CAN message data */
    uint32_t canLine                            /**< CAN line */
)
{
    UNUSEDARG(pData);
    UNUSEDARG(canLine);

    switch (event) {
        case GOAL_CAN_EVENT_RX:
            goal_logInfo("received %u bytes at line %"FMT_u32, pData->dataLen, canLine);
            goal_logInfo("first byte: %x, last byte: %x", pData->data[0], pData->data[pData->dataLen - 1]);
            break;
        default:
            break;
    }
}


/****************************************************************************/
/** Application Loop
 *
 * This function must implement the application logic and must not block. It is
 * called in a loop (the GOAL loop) and if state tracking is necessary it
 * should use static or global variables.
 */
void appl_loop(
    void
)
{
    if (goal_timerTsGet() > tsNext) {
        /* set  */
        GOAL_MEMSET(canMsgTx.data, canMsgTx.data[0] + 1, APPL_CAN_MESSAGE_LEN);
        goal_canSend(&canMsgTx);
        tsNext = goal_timerTsGet() + GOAL_TIMER_SEC;
    }
}
