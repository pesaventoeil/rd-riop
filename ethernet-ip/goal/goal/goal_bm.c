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

#define GOAL_ID GOAL_ID_BM
#include <goal_includes.h>


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/
static GOAL_STATUS_T goal_bmBitReqSpec(
    GOAL_BM_T *pBm,                             /**< bitmap data ptr */
    unsigned int numBit                         /**< bit num ptr */
);


/****************************************************************************/
/** Allocate bitmap
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_bmAlloc(
    GOAL_BM_T **ppBm,                           /**< bitmap data ptr ref */
    unsigned int numBits                        /**< count of bits */
)
{
    GOAL_STATUS_T res;                          /* result */
    unsigned int numBytes;                      /* number of bytes */

    /* convert bits to bytes */
    numBytes = numBits / GOAL_BM_BITS_PER_BYTE;
    if (numBits % GOAL_BM_BITS_PER_BYTE) {
        numBytes++;
    }

    /* allocate bitmap data */
    res = goal_memCalloc(ppBm, sizeof(GOAL_BM_T));
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:84: */
        goal_lmLog(GOAL_ID_BM, 84, 55, 0, 0, GOAL_LOG_SEV_ERROR, "failed to allocate bitmap structure");
        return res;
    }

    /* allocate bitmap */
    res = goal_memCalloc(&(*ppBm)->pBits, numBytes);
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:85: */
        goal_lmLog(GOAL_ID_BM, 85, 56, 4, 0, GOAL_LOG_SEV_ERROR, "failed to allocate $1 bytes");
        goal_lmLogParamUINT32(numBytes);
        goal_lmLogFinish();
        return res;
    }

    /* initialize bitmap */
    (*ppBm)->numBits = numBits;

    return GOAL_OK;
}


/****************************************************************************/
/** Request bit from bitmap
 *
 * Note: If locking is needed it needs to be implemented in the caller.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_bmBitReq(
    GOAL_BM_T *pBm,                             /**< bitmap data ptr */
    unsigned int *pNumBit                       /**< bit num ptr */
)
{
    unsigned int lastPos;                       /* current position */

    /* iterate from current position to bitmap end */
    for (lastPos = pBm->pos; (pBm->pos < pBm->numBits) && GOAL_RES_ERR(goal_bmBitReqSpec(pBm, pBm->pos)); pBm->pos++);
    if (pBm->pos >= pBm->numBits) {

        /* iterate from bitmap start to current position */
        for (pBm->pos = 0; (pBm->pos < lastPos) && GOAL_RES_ERR(goal_bmBitReqSpec(pBm, pBm->pos)); pBm->pos++);
        if (pBm->pos >= lastPos) {
            return GOAL_ERR_ALLOC;
        }
    }

    /* store bit number */
    *pNumBit = pBm->pos;

    /* increase search position */
    pBm->pos = (pBm->pos + 1) % pBm->numBits;

    return GOAL_OK;
}


/****************************************************************************/
/** Try to request a specific bit
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_bmBitReqSpec(
    GOAL_BM_T *pBm,                             /**< bitmap data ptr */
    unsigned int numBit                         /**< bit num ptr */
)
{
    unsigned int posBit;                        /* bit position */
    unsigned int posByte;                       /* byte position */

    /* calculate byte position */
    posByte = numBit / GOAL_BM_BITS_PER_BYTE;

    /* calculate bit position */
    posBit = numBit % GOAL_BM_BITS_PER_BYTE;

    /* check if bit is available */
    if (!(pBm->pBits[posByte] & (1 << posBit))) {

        /* reserve bit */
        pBm->pBits[posByte] |= (1 << posBit);

        return GOAL_OK;
    }

    return GOAL_ERR_ALLOC;
}


/****************************************************************************/
/** Release bit from bitmap
 *
 * Note: If locking is needed it needs to be implemented in the caller.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_bmBitRel(
    GOAL_BM_T *pBm,                             /**< bitmap data ptr */
    unsigned int numBit                         /**< bit num */
)
{
    unsigned int posBit;                        /* bit position */
    unsigned int posByte;                       /* byte position */

    /* calculate byte position */
    posByte = numBit / GOAL_BM_BITS_PER_BYTE;

    /* calculate bit position */
    posBit = numBit % GOAL_BM_BITS_PER_BYTE;

    /* check if bit is used */
    if (!(pBm->pBits[posByte] & (1 << posBit))) {
        /* GG_LOG ID:86: */
        goal_lmLog(GOAL_ID_BM, 86, 57, 0, 0, GOAL_LOG_SEV_ERROR, "bit already released");
        return GOAL_ERR_PARAM;
    }

    /* release bit */
    pBm->pBits[posByte] &= ~(1 << posBit);

    /* update first-search position */
    pBm->pos = numBit;

    return GOAL_OK;
}
