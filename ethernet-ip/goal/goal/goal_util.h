/** @file
 *
 * @brief GOAL Helper Functions
 *
 * @details
 * This module defines several utilities used by GOAL.
 *
 * @copyright
 * Copyright 2010-2020.
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

#ifndef GOAL_UTIL_H
#define GOAL_UTIL_H


/****************************************************************************/
/* Macros */
/****************************************************************************/
/**< iterate through linked list */
#define GOAL_LL_FOREACH(pList, pIt)     LL_FOREACH2(pList, pIt, pNext)

/**< put element on start of linked list */
#define GOAL_LL_PREPEND(pList, pElem)   LL_PREPEND2(pList, pElem, pNext)

/**< put element on end of linked list */
#define GOAL_LL_APPEND(pList, pElem)    LL_APPEND2(pList, pElem, pNext)

/**< remove element from linked list */
#define GOAL_LL_DELETE(pList, pElem)    LL_DELETE2(pList, pElem, pNext)


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_utilInitPre(
    void
);

uint32_t goal_utilCrcFletcher32(
    uint16_t const *pData,                      /**< data pointer */
    uint32_t words                              /**< size in 16-bit words */
);

uint16_t goal_utilCrcFletcher16(
    uint8_t const *pData,                       /**< data pointer */
    int count                                   /**< size in 8-bit bytes */
);

void goal_utilRandInit(
    uint32_t val                                /**< random start value */
);

uint32_t goal_utilRand(
    void
);

void goal_utilRandStream(
    uint32_t *pArea,                            /**< ptr to store stream */
    unsigned int len                            /**< length of stream */
);

void goal_utilHexdump(
    uint8_t *pArea,                             /**< area pointer */
    unsigned int len                            /**< area length */
);

GOAL_STATUS_T goal_utilGoalLoopCheck(
    void
);

GOAL_STATUS_T goal_utilFindFirstSetU32(
    uint32_t value,                             /**< value to be evaluated */
    uint8_t *pBitpos                            /**< [out] position of highest bit set */
);

int goal_utilStrnCaseCmp(
    const char *pStr1,                          /**< first string */
    const char *pStr2,                          /**< second string */
    size_t strLen                               /**< length of string */
);

#endif /* GOAL_UTIL_H */
