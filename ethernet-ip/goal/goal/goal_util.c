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

#include <goal_includes.h>


/****************************************************************************/
/* Local defines */
/****************************************************************************/
#define STR_HEXDUMP_ELEM "0x00 "
#define STR_HEXDUMP "0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 "
#define GOAL_UTIL_CHAR_TOLO(c)     ((c) + 0x20) /**< convert upper case letter to lower case */


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static uint32_t randVal = 0x706f7274;           /**< random base value */
static char mBufHexdump[sizeof(STR_HEXDUMP)];   /**< hexdump buffer */
static GOAL_STAGE_HANDLER_T stageInit;          /**< init stage handler */


/****************************************************************************/
/* Local functions */
/****************************************************************************/
static GOAL_STATUS_T goal_utilInit(
    void
);


/****************************************************************************/
/** Register stage handler
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_utilInitPre(
    void
)
{
    /* register init stage */
    return goal_mainStageReg(GOAL_STAGE_UTIL_PRE, &stageInit, GOAL_STAGE_INIT, goal_utilInit);
}


/****************************************************************************/
/** Initialize Utility module
 *
 * Initialize the random number generator
 *
 * @retval GOAL_OK successful
 * @retval other fail
 */
static GOAL_STATUS_T goal_utilInit(
    void
)
{
#if GOAL_CONFIG_ETHERNET == 1
    GOAL_STATUS_T res;                          /* result */
    GOAL_ETH_MAC_ADDR_T mac;                    /* mac address */
    uint8_t *pMacArr;                           /* mac address array */
    uint32_t macSeed;                           /* seed value */

    /* get mac address */
    res = goal_ethMacAddrGet(GOAL_ETH_PORT_HOST, &mac);
    if (GOAL_RES_OK(res)) {
        /* use lower 32 bit of mac address as seed */
        pMacArr = (uint8_t *) &mac;
        macSeed = (uint32_t) (pMacArr[2] << 24 | pMacArr[3] << 18 | pMacArr[4] << 8 | pMacArr[5]);
        goal_utilRandInit(macSeed);
    }
#endif

    return GOAL_OK;
}


/****************************************************************************/
/** Calculate the Fletcher-32 checksum.
 *
 * Code from: http://en.wikipedia.org/wiki/Fletcher%27s_checksum#Optimizations
 */
uint32_t goal_utilCrcFletcher32(
    uint16_t const *pData,                      /**< data pointer */
    uint32_t words                              /**< size in 16-bit words */
)
{
    uint32_t sum1 = 0xffff;                     /* first sum */
    uint32_t sum2 = 0xffff;                     /* second sum */
    unsigned tlen;                              /* total length */

    while (words) {
        tlen = (words > 359) ? 359 : words;
        words -= tlen;
        do {
            sum2 += sum1 += *pData++;
        } while (--tlen);

        sum1 = (sum1 & 0xffff) + (sum1 >> 16);
        sum2 = (sum2 & 0xffff) + (sum2 >> 16);
    }

    /* Second reduction step to reduce sums to 16 bits */
    sum1 = (sum1 & 0xffff) + (sum1 >> 16);
    sum2 = (sum2 & 0xffff) + (sum2 >> 16);
    return sum2 << 16 | sum1;
}


/****************************************************************************/
/** Calculate the Fletcher-16 checksum.
 *
 * Code from: http://en.wikipedia.org/wiki/Fletcher%27s_checksum#Optimizations
 */
uint16_t goal_utilCrcFletcher16(
    uint8_t const *pData,                       /**< data pointer */
    int count                                   /**< size in 8-bit bytes */
)
{
    uint16_t sum1 = 0;                          /* first sum */
    uint16_t sum2 = 0;                          /* second sum */
    int index;                                  /* iteration variable */

    for (index = 0; index < count; index++) {
        sum1 = (sum1 + pData[index]) % 255;
        sum2 = (sum2 + sum1) % 255;
    }

    return (uint16_t) ((sum2 << 8) | sum1);
}


/****************************************************************************/
/** Initialize the Pseudo Random Number Generator
 *
 * Initializes the pseudo random number generator by setting the start value.
 * Multiple calls of this function alter the starting value, but do not
 * overwrite it.
 */
void goal_utilRandInit(
    uint32_t val                                /**< random start value */
)
{
    randVal ^= val;
}


/****************************************************************************/
/** Generate a Pseudo Random Number
 *
 * Generates a pseudo random number by using a hash function from
 * http://burtleburtle.net/bob/hash/integer.html
 */
uint32_t goal_utilRand(
    void
)
{
    uint32_t calc = randVal;                    /* calculation var */

    calc = (calc ^ 61) ^ (calc >> 16);
    calc = calc + (calc << 3);
    calc = calc ^ (calc >> 4);
    calc = calc * 0x27d4eb2d;
    calc = calc ^ (calc >> 15);

    return randVal = calc;
}


/****************************************************************************/
/** Generate a Pseudo Random Bytestream
 *
 * Fill a given pointer with a given length of random bytes. The count of bytes
 * must be a multiple of 4. This function uses the goal_utilRand function. This
 * is highly predictable and should not be used for security applications.
 */
void goal_utilRandStream(
    uint32_t *pArea,                            /**< ptr to store stream */
    unsigned int len                            /**< length of stream */
)
{
    uint32_t calc;                              /* calculation var */
    unsigned int cnt;                           /* counter */

    for (cnt = 0; cnt < (len / sizeof(uint32_t)); cnt++) {
        calc = goal_utilRand();
        GOAL_MEMCPY(pArea + cnt, &calc, sizeof(uint32_t));
    }
}


/****************************************************************************/
/** Generate a Hexdump
 *
 * Print a hexdump for a given area and its length. This function must not be
 * used in release builds and is not thread safe as it uses only one global
 * buffer.
 */
void goal_utilHexdump(
    uint8_t *pArea,                             /**< area pointer */
    unsigned int len                            /**< area length */
)
{
    unsigned int posArea;                       /* area position */
    unsigned int posBuf;                        /* buffer position */

    for (posArea = 0, posBuf = 0; posArea < len; posArea++) {
        GOAL_SNPRINTF(&mBufHexdump[posBuf], sizeof(STR_HEXDUMP) - posBuf, "0x%02x ", pArea[posArea]);
        posBuf += GOAL_STRLEN(STR_HEXDUMP_ELEM);
        if (sizeof(STR_HEXDUMP) <= (posBuf + 1)) {
            mBufHexdump[posBuf] = 0;
            posBuf = 0;
            /* GG_LOG ID:255: */
            goal_lmLog(GOAL_ID_UTIL, 255, 206, GOAL_STRLEN((const char *) mBufHexdump), 0, GOAL_LOG_SEV_INFO, "$1");
            goal_lmLogParamSTRING0(mBufHexdump);
            goal_lmLogFinish();
        }
    }

    if (sizeof(STR_HEXDUMP) > (posBuf + 1)) {
        mBufHexdump[posBuf] = 0;
        /* GG_LOG ID:256: */
        goal_lmLog(GOAL_ID_UTIL, 256, 207, GOAL_STRLEN((const char *) mBufHexdump), 0, GOAL_LOG_SEV_INFO, "$1");
        goal_lmLogParamSTRING0(mBufHexdump);
        goal_lmLogFinish();
    }
}


/****************************************************************************/
/** Test for GOAL loop
 *
 * Test if this function is called from the GOAL loop. On targets with task
 * support, the GOAL loop is performed in a separate task with lowest priority.
 * If the target doesn't support tasks, this function returns GOAL_OK always.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_utilGoalLoopCheck(
    void
)
{
#if GOAL_CONFIG_TASK == 1
    return goal_taskGoalMainCheck();
#else
    return GOAL_OK;
#endif
}


/****************************************************************************/
/** Find the first bit set in a uint32_t value
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_utilFindFirstSetU32(
    uint32_t value,                             /**< value to be evaluated */
    uint8_t *pBitpos                            /**< [out] position of highest bit set */
)
{
    uint8_t cnt;                                /* loop counter */
    uint32_t testVal = 0;                       /* value for comparison */

    if (0 == value) {
        return GOAL_ERROR;
    }

    for (cnt = 0; cnt < 32; cnt++) {
        testVal |= 1 << cnt;
        if (testVal >= value) {
            *pBitpos = cnt;
            return GOAL_OK;
        }
    }

    return GOAL_ERROR;
}


/****************************************************************************/
/** Case insensitive comparison of two strings
 *
 * This function compares two strings up to @em strLen bytes. IT ignores the
 * case of the letters.
 *
 * @retval 0 string is equal
 * @retval -1 first unequal character of pStr1 is less than character of pStr2
 * @retval 1 first unequal character of pStr1 is greater than character of pStr2
 * @retval other  fail
 */
int goal_utilStrnCaseCmp(
    const char *pStr1,                          /**< first string */
    const char *pStr2,                          /**< second string */
    size_t strLen                               /**< length of string */
)
{
    int res = 0;                                /* comparison result */
    char char1;                                 /* current character of first string */
    char char2;                                 /* current character of second string */

    for (; 0 != strLen; strLen--) {
        char1 = *pStr1;
        if (('A' <= char1) && ('Z' >= char1)) {
            /* convert to lower case */
            char1 = GOAL_UTIL_CHAR_TOLO(char1);
        }

        char2 = *pStr2;
        if (('A' <= char2) && ('Z' >= char2)) {
            /* convert to lower case */
            char2 = GOAL_UTIL_CHAR_TOLO(char2);
        }

        if (char1 != char2) {
            res = (char1 < char2) ? -1 : 1;
            break;
        }

        if ('\0' == char1) {
            /* both strings are equal but are shorter than strLen */
            break;
        }

        pStr1++;
        pStr2++;
    }

    return res;
}
