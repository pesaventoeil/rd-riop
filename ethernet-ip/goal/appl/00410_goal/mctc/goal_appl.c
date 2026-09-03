/** @file
 *
 * @brief application template
 *
 * This module provides a simple micro core to core application using GOAL. This
 * application is used for AC and CC. The application handles cyclic data for
 * the MCTC communication. They contain a local time stamp and an incremental
 * transmission counter, which are transfer to the peer core. Meanwhile, the
 * other core do the same.
 *
 * The content of received data is compared to the content of the next cyclic
 * data, for determining the delay of communication.
 *
 * Note:    This demo application is meant to demonstrate the basic
 *          handling of this feature.
 *          It is not meant for performance benchmarking and it is
 *          not optimized for performance evaluation purposes.
 *
 * @copyright
 * Copyright 2010-2019.
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
#include <goal_media/goal_mi_mctc.h>


/****************************************************************************/
/* Defines */
/****************************************************************************/
#define APPL_LOOP_TIMEOUT 1                     /**< application timeout in ms */
#define APPL_STATUS_TIMEOUT 5000                /**< application timeout of status log in ms */


/****************************************************************************/
/* Typedefs */
/****************************************************************************/
GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct {
    uint64_t cnt;                               /**< message counter */
    GOAL_TIMESTAMP_T ts;                        /**< message timestamp */
} GOAL_TARGET_PACKED APPL_MSG_T;
GOAL_TARGET_PACKED_STRUCT_POST


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static APPL_MSG_T msgData_be;                   /**< message data in big endian format */
static APPL_MSG_T msgData;                      /**< message data */
static APPL_MSG_T msgDataPrev;                  /**< message data of last cycle */
static uint64_t cntTx = 1;                      /**< transmission counter */
static uint64_t cntErr = 0;                     /**< number of errors */
static GOAL_TIMESTAMP_T tsTout;                 /**< timeout timestamp */
static GOAL_TIMESTAMP_T tsStatus;               /**< log timestamp */

static GOAL_MI_DM_PART_T mMiDmRead;             /**< data read partition */
static GOAL_MI_DM_PART_T mMiDmWrite;            /**< data write partition */


/****************************************************************************/
/** Application Setup
 *
 * Setup the application.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T appl_setup(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* register MI DM partitions */
    res = goal_miDmPartReg(GOAL_MI_MCTC_DIR_PEER_FROM, GOAL_ID_APPL, &mMiDmRead, sizeof(APPL_MSG_T));
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to register cyclic read part");
        return res;
    }

    res = goal_miDmPartReg(GOAL_MI_MCTC_DIR_PEER_TO, GOAL_ID_APPL, &mMiDmWrite, sizeof(APPL_MSG_T));
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to register cyclic write part");
        return res;
    }

    return res;
}


/****************************************************************************/
/** Main Loop
 *
 * This function must implement the application logic and must not block. It is
 * called in a loop (the GOAL loop) and if state tracking is necessary it
 * should use static or global variables.
 *
 * The application handles cyclic data for the MCTC communication. They contain
 * a local time stamp and an incremental transmission counter, which are
 * transfer to the peer core. Meanwhile, the other core do the same.
 *
 * The content of received data is compared to the content of the next cyclic
 * data, for determining the delay of communication.
 */
void appl_loop(
    void
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_TIMESTAMP_T ts;                        /* latest timestamp */

    /* check if MCTC is ready */
    res = goal_miMctcStatusGet(GOAL_ID_DEFAULT);
    if (GOAL_RES_ERR(res)) {
        return;
    }

    ts = goal_timerTsGet();

    /* run every APPL_LOOP_TIMEOUT */
    if (tsTout <= ts) {
        /* read output partition data */
        res = goal_miDmSingleRead(&mMiDmRead, (uint8_t *) &msgData_be);
        if (GOAL_RES_OK(res)) {
            msgData.cnt = GOAL_be64toh(msgData_be.cnt);
            msgData.ts = GOAL_be64toh(msgData_be.ts);
            if (msgData.cnt >= msgDataPrev.cnt && (msgData.ts >= msgDataPrev.ts)) {
                /* update previous data */
                GOAL_MEMCPY(&msgDataPrev, &msgData, sizeof(APPL_MSG_T));
            } else {
                goal_logErr("invalid data: counter: %"FMT_u64" previous: %"FMT_u64" | time stamp: %"FMT_u64" previous: %"FMT_u64, msgData.cnt, msgDataPrev.cnt, msgData.ts, msgDataPrev.ts);

                /* reset previous data */
                GOAL_MEMSET(&msgDataPrev, 0, sizeof(APPL_MSG_T));
            }

            /* update the next message */
            msgData_be.ts = GOAL_htobe64(ts);
            msgData_be.cnt = GOAL_htobe64(cntTx);

            /* transmit the messages until the maximum number of transmissions is reached */
            if (0 != cntTx) {
                /* write input partition data */
                res = goal_miDmSingleWrite(&mMiDmWrite, (uint8_t *) &msgData_be, sizeof(APPL_MSG_T));

                /* increase the transmission counter */
                cntTx++;
            } else {
                goal_logWarn("Maximal number of messages transmitted.");
            }
        }

        if (GOAL_ERR_BUSY == res) {
            goal_logDbg("MCTC is busy.");
        } else if (GOAL_RES_ERR(res)) {
            goal_logErr("communication failed err: %"FMT_x32, res);
            cntErr++;
        }

        /* update timeout value */
        tsTout = ts + APPL_LOOP_TIMEOUT;
    }

    /* log every APPL_STATUS_TIMEOUT a status */
    if (tsStatus <= ts) {
        goal_logInfo("counter: %"FMT_u64" | time stamp: %"FMT_u64" | errors: %"FMT_u64, msgData.cnt, msgData.ts, cntErr);
        /* update timeout value */
        tsStatus = ts + APPL_STATUS_TIMEOUT;
    }
}
