/** @file
 *
 * @brief Mailbox example
 *
 * This module demonstrate the handling of GOAL its mailbox MA.
 *
 * Note:    This demo application is meant to demonstrate the basic
 *          handling of this feature.
 *          It is not meant for performance benchmarking and it is
 *          not optimized for performance evaluation purposes.
 *
 * @copyright
 * Copyright 2010-2018.
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

#include "goal_includes.h"
#include <goal_media/goal_ma_mbox.h>


/****************************************************************************/
/* Defines */
/****************************************************************************/
#define APPL_TX_TIME (GOAL_TIMER_MSEC * 1)      /**< cycle time for sending messages */
#define APPL_LOG_TIME (GOAL_TIMER_SEC * 5)      /**< cycle time for logging stats */
#define APPL_FIRST_ID 0                         /**< start message */


/****************************************************************************/
/* local variables */
/****************************************************************************/
static GOAL_MA_MBOX_T *pMboxTx = NULL;          /**< transmission mailbox handle */
static GOAL_MA_MBOX_T *pMboxRx = NULL;          /**< receive mailbox handle */
static GOAL_TIMESTAMP_T tsNextTx;               /**< timestamp for next message */
static GOAL_TIMESTAMP_T tsNextLog;              /**< timestamp for logging stats */
static uint32_t dataTx = APPL_FIRST_ID;         /**< data for transmission */
static uint32_t cntRx = 0;                      /**< number of received frames */
static uint32_t cntFailTx = 0;                  /**< number of invalid transmissions */
static uint32_t cntInvData = 0;                 /**< number of invalid message data */


/****************************************************************************/
/** Mailbox message callback
 *
 * This function is the callback of the goal media mailbox.
 *
 * @retval GOAL_OK success
 * @retval other failure
 */
static GOAL_STATUS_T goal_tgtMboxCb(
    struct GOAL_MA_MBOX_T *pHdlMbox,            /**< mailbox handle */
    uint8_t *pData,                             /**< data ptr */
    unsigned int len                            /**< data length */
)
{
    static uint32_t dataRx = APPL_FIRST_ID;     /* data for verification */

    UNUSEDARG(pHdlMbox);
    UNUSEDARG(pData);
    UNUSEDARG(len);

    /* count the number of received messages */
    cntRx++;

    /* validate the message data */
    if (dataRx != *((uint32_t *) pData)) {
      cntInvData++;
      dataRx = *((uint32_t *) pData);
    }

    /* increase for next callback */
    dataRx++;

    return GOAL_OK;
}


/****************************************************************************/
/** Application Setup
 *
 * Configure GOAL and components.
 *
 * Note: If the function body is empty the whole function can be removed.
 */
GOAL_STATUS_T appl_setup(
    void
)
{
    GOAL_STATUS_T res;                          /* result */
    //uint32_t tmpMboxIdRegVal = GOAL_MBOX_DST_ID; /* mailbox ID assistance register value */

    res = goal_maMboxGetByIdx(&pMboxRx, GOAL_ID_MA_MBOX_DST_0_RX);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Unable to get mbox ID %u", GOAL_ID_MA_MBOX_DST_0_RX);
        return res;
    }

    res = goal_maMboxAlloc(pMboxRx, goal_tgtMboxCb);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Unable to alloc mbox ID %u", GOAL_ID_MA_MBOX_DST_0_RX);
        return res;
    }

    res = goal_maMboxGetByIdx(&pMboxTx, GOAL_ID_MA_MBOX_DST_0_TX);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Unable to get mbox ID %u", GOAL_ID_MA_MBOX_DST_0_TX);
        return res;
    }

    res = goal_maMboxAlloc(pMboxTx, NULL);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Unable to alloc mbox ID %u", GOAL_ID_MA_MBOX_DST_0_TX);
        return res;
    }

    tsNextTx = goal_timerTsGet() + APPL_TX_TIME;
    tsNextLog = goal_timerTsGet() + APPL_LOG_TIME;

    return res;
}


/****************************************************************************/
/** Application Loop
 *
 * Perform cyclic application tasks.
 *
 * Note: If the function body is empty the whole function can be removed.
 */
void appl_loop(
    void
)
{
    static GOAL_TIMESTAMP_T ts;                 /* timestamp */
    GOAL_STATUS_T res;                          /* result */

    ts = goal_timerTsGet();

    /* send message */
    if (tsNextTx < ts) {
        res = goal_maMboxSend(pMboxTx, (uint8_t *) &dataTx, sizeof(dataTx));
        dataTx++;
        if (GOAL_RES_ERR(res)) {
            cntFailTx++;
        }
        /* Update next timestamp */
        tsNextTx = goal_timerTsGet() + APPL_TX_TIME;
    }

    /* log statistics */
    if (tsNextLog < ts) {
        if (cntRx) {
            goal_logInfo("%"FMT_u32" messages received", cntRx);
        }
        if (cntFailTx) {
            goal_logInfo("%"FMT_u32" invalid write attempt", cntFailTx);
        }
        if (cntInvData) {
            goal_logInfo("%"FMT_u32" invalid message data", cntInvData);
        }
        /* Update next timestamp */
        tsNextLog = goal_timerTsGet() + APPL_LOG_TIME;
    }
}
