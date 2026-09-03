/** @file
 *
 * @brief Bitmap Handling
 *
 * @details
 * This module implements bitmap handling.
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

#ifndef GOAL_BM_H
#define GOAL_BM_H

#include "goal_includes.h"


/****************************************************************************/
/* Defines */
/****************************************************************************/
#define GOAL_BM_BITS_PER_BYTE   8               /**< bits per byte */


/****************************************************************************/
/* Structures */
/****************************************************************************/
typedef struct {
    unsigned int numBits;                       /**< number of bits */
    unsigned int pos;                           /**< last search position */
    char *pBits;                                /**< bitfield */
} GOAL_BM_T;


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_bmAlloc(
    GOAL_BM_T **ppBm,                           /**< bitmap data ptr ref */
    unsigned int numBits                        /**< count of bits */
);

GOAL_STATUS_T goal_bmBitReq(
    GOAL_BM_T *pBm,                             /**< bitmap data ptr */
    unsigned int *pNumBit                       /**< bit num ptr */
);

GOAL_STATUS_T goal_bmBitRel(
    GOAL_BM_T *pBm,                             /**< bitmap data ptr */
    unsigned int numBit                         /**< bit num */
);


#endif /* GOAL_BM_H */
