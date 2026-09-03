/** @file
 *
 * @brief GOAL Example for linked list
 *
 * This module provides a simple application of ring buffer usage
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


/****************************************************************************/
/* Definitions */
/****************************************************************************/
#define RB_SIZE 128


/****************************************************************************/
/* Local functions */
/****************************************************************************/
static GOAL_STATUS_T rbCb(
    struct GOAL_RB_READER_T *pHdlReader         /**< rb reader handle */
);


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_RB_T *pRb;                          /**< ring buffer */
static GOAL_RB_READER_T *pRbReader;             /**< ring buffer reader */
static uint8_t data[RB_SIZE];                   /**< test data */
static char str1[] = "AABBCCDDEEFFGGHHIIJJKKLLMMNNOOPPQQRRSSTTUUVVWWXXYYZZ";


/****************************************************************************/
/** Application Setup
 *
 * Configure GOAL and components.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T appl_setup(
    void
)
{
    GOAL_STATUS_T res;                          /* result */
    uint16_t size;                              /* data size */

    res = goal_rbInit(&pRb, RB_SIZE);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to initialize ring buffer");
    }

    res = goal_rbReaderReg(&pRbReader, pRb, rbCb);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to register ring buffer reader");
    }

    res = goal_rbPut(pRb, &data[0], RB_SIZE, GOAL_FALSE);
    if (GOAL_RES_OK(res)) {
        goal_logErr("ring buffer can be overflown");
        res = GOAL_ERR_OVERFLOW;
    }

    res = goal_rbPut(pRb, &data[0], RB_SIZE-1, GOAL_FALSE);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("unable to fill rb");
    }

    res = goal_rbPut(pRb, &data[0], 1, GOAL_FALSE);
    if (GOAL_RES_OK(res)) {
        goal_logErr("writing to rb possible although it is full");
    }
    else {
        /* expected behaviour */
        res = GOAL_OK;
    }

    /* try to read RB_SIZE + 1 bytes */
    if (GOAL_RES_OK(res)) {
        size = RB_SIZE;
        res = goal_rbReaderGet(pRbReader, data, &size);
        if (size != RB_SIZE -1) {
            res = GOAL_ERR_OVERFLOW;
        }
    }

    /* try to read again, buffer should be empty */
    if (GOAL_RES_OK(res)) {
        size = RB_SIZE;
        res = goal_rbReaderGet(pRbReader, data, &size);
        if (res != GOAL_ERR_EMPTY && size != 0) {
            res = GOAL_ERR_OVERFLOW;
        }
        else {
            res = GOAL_OK;
        }
    }

    /* initial write */
    do {
        res = goal_rbPut(pRb, (const uint8_t *) str1, (uint16_t) (GOAL_STRLEN(str1) + 1), GOAL_FALSE);
    } while (GOAL_RES_OK(res));

    do {
        size = (uint16_t) (GOAL_STRLEN(str1) + 1);
        res = goal_rbReaderGet(pRbReader, data, &size);
        goal_logInfo("%s", data);
    } while (GOAL_RES_OK(res));

    /* second round, where ring buffer should physically overlap */
    do {
        res = goal_rbPut(pRb, (const uint8_t *) str1, (uint16_t) (GOAL_STRLEN(str1) + 1), GOAL_FALSE);
    } while (GOAL_RES_OK(res));

    do {
        size = (uint16_t) (GOAL_STRLEN(str1) + 1);
        res = goal_rbReaderGet(pRbReader, data, &size);
        goal_logInfo("%s", data);
    } while (GOAL_RES_OK(res));

    if (res == GOAL_ERR_EMPTY) {
        res = GOAL_OK;
    }

    /* try writing with overlapping */
    if (GOAL_RES_OK(res)) {
        while (GOAL_RES_OK(res)) {
            size = 32;
            res = goal_rbPut(pRb, &data[0], size, GOAL_FALSE);
            if (GOAL_RES_OK(res)) {
                size = 16;
                goal_rbReaderGet(pRbReader, data, &size);
            }
        }

        res = GOAL_OK;
    }

    if (GOAL_RES_OK(res)) {
        goal_logInfo("test passed");
    }

    return res;
}


/****************************************************************************/
/** ring buffer reader callback
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T rbCb(
    struct GOAL_RB_READER_T *pHdlReader         /**< rb reader handle */
)
{
    UNUSEDARG(pHdlReader);

    return GOAL_OK;
}
