/** @file
 *
 * @brief GOAL Micro Core To Core - Ringbuffer Target
 *
 * @details
 * This module implements serial data handling for GOAL Micro Core To Core.
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
#define GOAL_ID GOAL_ID_CTC
#include "goal_mctc.h"
#include "goal_mctc_rb.h"

/* TODO: use GOAL_ERR_CTC_* instead of common error codes */


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
static void goal_mctcRbRecv(
    GOAL_MCTC_INST_T *pInst,                    /**< MCTC instance */
    GOAL_MCTC_TGT_T *pTgt                       /**< target handle */
);

static GOAL_STATUS_T goal_mctcRbSend(
    GOAL_MCTC_T *pCtc,                          /**< CTC handle */
    GOAL_MCTC_TGT_T *pTgt                       /**< target handle */
);


/****************************************************************************/
/* Local variables */
/****************************************************************************/
/**< start sequence */
static uint8_t mpSeqStart[] = GOAL_MCTC_SEQ_START;


/****************************************************************************/
/** GOAL MCTC - Parse Receive Buffer
 */
static void goal_mctcRbRecv(
    GOAL_MCTC_INST_T *pInst,                    /**< MCTC instance */
    GOAL_MCTC_TGT_T *pTgt                       /**< target handle */
)
{
    GOAL_STATUS_T res;                          /* result */
    unsigned int lenDataRb;                     /* ringbuffer data length */
    uint32_t lenDataCtc_le32;                   /* CTC frame data length */
    GOAL_MCTC_CRC_T valCrc_le16;                /* CTC frame CRC value */
    GOAL_MCTC_CRC_T valCrcCalc;                 /* calculated CRC value */
    GOAL_MCTC_RB_T *pRb;                        /* ringbuffer handle */

    /* assign ringbuffer */
    pRb = pTgt->pPriv;

    /* search ringbuffer for start sequence */
    while (1) {

        /* lock ringbuffer */
        GOAL_LOCK_GET_NO_RES(pRb->pLock);

        /* make sure ringbuffer contains at least the header + crc */
        lenDataRb = goal_mctcRbLenGet(pRb);
        if (GOAL_MCTC_LEN_FRAME_MIN > lenDataRb) {

            /* unlock ringbuffer */
            GOAL_LOCK_PUT(pRb->pLock);

            break;
        }

        /* if no sequence was found then leave */
        res = goal_mctcRbPtnSeek(pRb, mpSeqStart, sizeof(mpSeqStart));
        if (GOAL_RES_ERR(res)) {

            /* leave only "pattern size - 1" bytes in ringbuffer */
            goal_mctcRbRemove(pRb, lenDataRb - sizeof(mpSeqStart) + 1);

            /* unlock ringbuffer */
            GOAL_LOCK_PUT(pRb->pLock);

            break;
        }

        /* read length from ringbuffer without changing its state */
        res = goal_mctcRbPeek(pRb, (uint8_t *) &lenDataCtc_le32, GOAL_MCTC_OFSOF(GOAL_MCTC_HDR_T, lenData), sizeof(lenDataCtc_le32));
        if (GOAL_RES_ERR(res)) {

            /* unlock ringbuffer */
            GOAL_LOCK_PUT(pRb->pLock);

            break;
        }

        /* convert data length endianness */
        lenDataCtc_le32 = GOAL_le32toh(lenDataCtc_le32);

        /* data length exceeds maximum data length */
        if (pRb->sizeElem < lenDataCtc_le32) {

            /* remove first sequence byte to continue search at next iteration */
            goal_mctcRbRemove(pRb, 1);

            /* unlock ringbuffer */
            GOAL_LOCK_PUT(pRb->pLock);

            continue;
        }

        /* data length too short to contain CTC info */
        if (sizeof(GOAL_MCTC_HDR_RPC_T) > lenDataCtc_le32) {

            /* remove first sequence byte to continue search at next iteration */
            goal_mctcRbRemove(pRb, 1);

            /* unlock ringbuffer */
            GOAL_LOCK_PUT(pRb->pLock);

            continue;
        }

        /* read data without changing ringbuffer state */
        res = goal_mctcRbPeek(pRb, pRb->pDataElem, sizeof(GOAL_MCTC_HDR_T), lenDataCtc_le32);
        if (GOAL_RES_ERR(res)) {

            /* unlock ringbuffer */
            GOAL_LOCK_PUT(pRb->pLock);

            break;
        }

        /* read CRC without changing ringbuffer state */
        res = goal_mctcRbPeek(pRb, (uint8_t *) &valCrc_le16, sizeof(GOAL_MCTC_HDR_T) + lenDataCtc_le32, sizeof(GOAL_MCTC_CRC_T));
        if (GOAL_RES_ERR(res)) {

            /* unlock ringbuffer */
            GOAL_LOCK_PUT(pRb->pLock);

            break;
        }

        /* convert CRC endianness */
        valCrc_le16 = GOAL_le16toh(valCrc_le16);

        /* calculate data CRC */
        valCrcCalc = GOAL_FLETCHER16(pRb->pDataElem, lenDataCtc_le32) + GOAL_MCTC_FLETCHER_OFFSET;

        /* compare CRC */
        if (valCrc_le16 != valCrcCalc) {
            GOAL_LOG("    * CRC mismatch: %04x vs %04x\n", valCrcCalc, valCrc_le16);

            /* remove first sequence byte to continue search at next iteration */
            goal_mctcRbRemove(pRb, 1);

            /* unlock ringbuffer */
            GOAL_LOCK_PUT(pRb->pLock);

            continue;
        }

        /* remove valid data chunk from ringbuffer */
        res = goal_mctcRbRemove(pRb, sizeof(GOAL_MCTC_HDR_T) + lenDataCtc_le32 + sizeof(GOAL_MCTC_CRC_T));
        if (GOAL_RES_ERR(res)) {
            /* fatal error */
            GOAL_LOG_EXCEPTION(GOAL_LOG_ID_RB_REMOVE_FAILED, "failed to remove element from ringbuffer");
        }

        /* unlock ringbuffer */
        GOAL_LOCK_PUT(pRb->pLock);

        /* TODO: remove debug */
        GOAL_LOG("  * goal_mctcRecv: found valid data block of %"FMT_u32" bytes size\n", lenDataCtc_le32);

        /* handle CTC message */
        goal_mctcMsgParse(pInst, pRb->pDataElem, lenDataCtc_le32);
    }
}


/****************************************************************************/
/** GOAL MCTC - Send Data into CTC TX
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_mctcRbSend(
    GOAL_MCTC_T *pCtc,                          /**< CTC handle */
    GOAL_MCTC_TGT_T *pTgt                       /**< target handle */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MCTC_CRC_T valCrc_le16;                /* checksum */
    uint32_t lenData_le32;                      /* data length */
    GOAL_MCTC_RB_T *pRb;                        /* ringbuffer handle */

    /* assign ringbuffer */
    pRb = pTgt->pPriv;

    /* lock ringbuffer */
    GOAL_LOCK_GET(pRb->pLock);

    /* check if data size fits into ringbuffers element size */
    if (pRb->sizeElem < pCtc->idxData) {
        GOAL_LOG("  * goal_mctcRbSend: data length exceeds allowed CTC frame length\n");

        /* unlock ringbuffer */
        GOAL_LOCK_PUT(pRb->pLock);

        return GOAL_ERR_OVERFLOW;
    }

    /* start sequence */
    GOAL_LOG("    * put: start sequence\n");
    res = goal_mctcRbPut(pRb, mpSeqStart, sizeof(mpSeqStart));
    if (GOAL_RES_ERR(res)) {

        /* unlock ringbuffer */
        GOAL_LOCK_PUT(pRb->pLock);

        return res;
    }

    /* convert data length endianness */
    GOAL_LOG("    * put: data length\n");
    lenData_le32 = GOAL_htole32(pCtc->idxData);
    res = goal_mctcRbPut(pRb, (uint8_t *) &lenData_le32, sizeof(lenData_le32));
    if (GOAL_RES_ERR(res)) {

        /* unlock ringbuffer */
        GOAL_LOCK_PUT(pRb->pLock);

        return res;
    }

    /* data */
    GOAL_LOG("    * put: data, %u bytes\n", pCtc->idxData);
    res = goal_mctcRbPut(pRb, pCtc->pData, pCtc->idxData);
    if (GOAL_RES_ERR(res)) {

        /* unlock ringbuffer */
        GOAL_LOCK_PUT(pRb->pLock);

        return res;
    }

    /* convert CRC endianness */
    valCrc_le16 = GOAL_htole16(GOAL_FLETCHER16(pCtc->pData, pCtc->idxData) + GOAL_MCTC_FLETCHER_OFFSET);

    /* send frame */
    GOAL_LOG("    * put: CRC\n");

    /* unlock ringbuffer */
    GOAL_LOCK_PUT(pRb->pLock);

    /* goal_mctcRbPut locks the ringbuffer itself so keeping the lock is no problem here */
    return goal_mctcRbPut(pRb, (uint8_t *) &valCrc_le16, sizeof(valCrc_le16));
}


/****************************************************************************/
/** GOAL MCTC - Allocate Ringbuffer
 *
 * @returns GOAL_STATUS_T
 */
GOAL_STATUS_T goal_mctcRbNew(
    GOAL_MCTC_TGT_T **ppTgt,                    /**< [out] MCTC target */
    unsigned int sizeRb,                        /**< ringbuffer size */
    unsigned int sizeElem,                      /**< maximum element size */
    GOAL_STATUS_T (*funcOverwrite)(GOAL_MCTC_RB_T *pRb) /**< overwrite callback function */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MCTC_RB_T *pRb = NULL;                 /* ringbuffer handle */

    /* allocate MCTC target data */
    res = GOAL_MEM_CALLOC(ppTgt, sizeof(GOAL_MCTC_TGT_T));
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* allocate ringbuffer metadata */
    res = GOAL_MEM_CALLOC(&(*ppTgt)->pPriv, sizeof(GOAL_MCTC_RB_T));
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* assign ringbuffer data */
    pRb = (GOAL_MCTC_RB_T *) (*ppTgt)->pPriv;

    /* allocate ringbuffer data */
    res = GOAL_MEM_CALLOC(&pRb->pData, sizeRb);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* allocate element data */
    res = GOAL_MEM_CALLOC(&pRb->pDataElem, sizeElem);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* allocate ringbuffer lock */
    GOAL_LOCK_CREATE(pRb->pLock);

    /* assign metadata */
    pRb->sizeRb = sizeRb;
    pRb->sizeElem = sizeElem;
    pRb->funcOverwrite = funcOverwrite;

    /* assign MCTC target data */
    (*ppTgt)->funcRecv = goal_mctcRbRecv;
    (*ppTgt)->funcSend = goal_mctcRbSend;

    return GOAL_OK;
}


/****************************************************************************/
/** GOAL MCTC - Put Data Into Ringbuffer
 *
 * @returns GOAL_STATUS_T
 */
GOAL_STATUS_T goal_mctcRbPut(
    GOAL_MCTC_RB_T *pRb,                        /**< ringbuffer ptr */
    uint8_t *pData,                             /**< data pointer */
    unsigned int lenData                        /**< data length */
)
{
    GOAL_STATUS_T res;                          /* result */
    unsigned int idxWr;                         /* write index */
    unsigned int lenRight;                      /* right length */
    unsigned int lenLeft;                       /* left length */

    /* verify size */
    if (pRb->sizeRb < lenData) {
        GOAL_LOG("      * goal_mctcRbPut: error, data size (%u) larger than ringbuffer (%u)\n", lenData, pRb->sizeRb);
        return GOAL_ERR_OVERFLOW;
    }

    /* lock ringbuffer */
    GOAL_LOCK_GET(pRb->pLock);

    /* calculate write index */
    idxWr = (pRb->idxWr + lenData) % pRb->sizeRb;

    /* check if data would be overwritten */
    if ((pRb->sizeRb - pRb->lenData) < lenData) {
        if (pRb->funcOverwrite) {
            res = pRb->funcOverwrite(pRb);
            if (res) {

                /* unlock ringbuffer */
                GOAL_LOCK_PUT(pRb->pLock);

                return res;
            }
        }

        /* adjust read index */
        pRb->idxRd = idxWr % pRb->sizeRb;
    }

    /* write data to ringbuffer */
    lenRight = pRb->sizeRb - pRb->idxWr;
    if (lenRight > lenData) {
        lenLeft = 0;
        lenRight = lenData;
    } else {
        lenLeft = lenData - lenRight;
    }

    GOAL_MEMCPY(&pRb->pData[pRb->idxWr], pData, lenRight);
    if (lenLeft) {
        GOAL_MEMCPY(pRb->pData, &pData[lenRight], lenLeft);
    }

    /* update write index */
    pRb->idxWr = idxWr;

    /* update data length */
    pRb->lenData = pRb->lenData + lenData;
    if (pRb->lenData > pRb->sizeRb) {
        pRb->lenData = pRb->sizeRb;
    }
    GOAL_LOG("      * goal_mctcRbPut: bytes = %u, new ringbuffer data len: %u\n", lenData, pRb->lenData);

    /* unlock ringbuffer */
    GOAL_LOCK_PUT(pRb->pLock);

    return GOAL_OK;
}


/****************************************************************************/
/** GOAL MCTC - Get Data From Ringbuffer
 *
 * @returns GOAL_STATUS_T
 */
GOAL_STATUS_T goal_mctcRbGet(
    GOAL_MCTC_RB_T *pRb,                        /**< ringbuffer ptr */
    uint8_t *pData,                             /**< data pointer */
    unsigned int lenData                        /**< data length */
)
{
    unsigned int lenRight;                      /* right length */
    unsigned int lenLeft;                       /* left length */

    /* verify length */
    if (pRb->lenData < lenData) {
        GOAL_LOG("  * goal_mctcRbGet: error, requested length (%u) larger than ringbuffer data length (%u)\n", lenData, pRb->lenData);
        return GOAL_ERR_UNDERFLOW;
    }

    /* lock ringbuffer */
    GOAL_LOCK_GET(pRb->pLock);

    /* copy ringbuffer data to buffer */
    lenRight = pRb->sizeRb - pRb->idxRd;
    if (lenRight > lenData) {
        lenLeft = 0;
        lenRight = lenData;
    } else {
        lenLeft = lenData - lenRight;
    }

    GOAL_MEMCPY(pData, &pRb->pData[pRb->idxRd], lenRight);
    if (lenLeft) {
        GOAL_MEMCPY(&pData[lenRight], pRb->pData, lenLeft);
    }

    /* calculate read index */
    pRb->idxRd = (pRb->idxRd + lenData) % pRb->sizeRb;

    /* update data length */
    pRb->lenData -= lenData;

    /* unlock ringbuffer */
    GOAL_LOCK_PUT(pRb->pLock);

    return GOAL_OK;
}


/****************************************************************************/
/** GOAL MCTC - Get Ringbuffer Data Length
 *
 * @returns ringbuffer data length
 */
unsigned int goal_mctcRbLenGet(
    GOAL_MCTC_RB_T *pRb                         /**< ringbuffer ptr */
)
{
    return pRb->lenData;
}


/****************************************************************************/
/** GOAL MCTC - Get Ringbuffer Free Space
 *
 * @returns ringbuffer free space
 */
unsigned int goal_mctcRbLenFreeGet(
    GOAL_MCTC_RB_T *pRb                         /**< ringbuffer ptr */
)
{
    return pRb->sizeRb - pRb->lenData;
}


/****************************************************************************/
/** GOAL MCTC - Destructive Ringbuffer Pattern Search
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_mctcRbPtnSeek(
    GOAL_MCTC_RB_T *pRb,                        /**< ringbuffer ptr */
    uint8_t *pPtn,                              /**< pattern */
    unsigned int lenPtn                         /**< pattern length */
)
{
    unsigned int idxEnd;                        /* end index */
    unsigned int idxEndLeft;                    /* end index left */
    unsigned int idxEndRight;                   /* end index right */
    unsigned int cntRb;                         /* ringbuffer counter */
    unsigned int cntPtn;                        /* pattern counter */
    unsigned int idxPtn;                        /* pattern index */
    unsigned int cntLoop;                       /* loop counter */
    unsigned int cntRemove;                     /* removal counter */

    /* check pattern length */
    if (pRb->lenData < lenPtn) {
        GOAL_LOG("  * goal_mctcRbPtnSeek: error, pattern larger than data length\n");
        return GOAL_ERR_UNDERFLOW;
    }

    /* lock ringbuffer */
    GOAL_LOCK_GET(pRb->pLock);

    /* calculate right and left search length */
    idxEndRight = pRb->idxRd + pRb->lenData;
    if (idxEndRight >= pRb->sizeRb) {
        idxEndLeft = idxEndRight - pRb->sizeRb;
        idxEndRight = pRb->sizeRb;
    } else {
        idxEndLeft = 0;
    }

    /* first search right and than search left ringbuffer side */
    cntRb = idxPtn = pRb->idxRd;
    cntPtn = 0;
    idxEnd = idxEndRight;
    cntRemove = 0;
    for (cntLoop = 2; cntLoop; cntLoop--) {

        /* iterate through right part of ringbuffer */
        for (; (cntPtn < lenPtn) && (cntRb < idxEnd); cntRb++) {

            /* if pattern doesn't match then reset pattern search */
            if (pRb->pData[cntRb] != pPtn[cntPtn]) {

                /* reset pattern position and pattern ringbuffer index */
                cntPtn = 0;
                idxPtn = cntRb;

                /* if the resetted pattern doesn't start here then make sure to skip it */
                if (pRb->pData[cntRb] != pPtn[0]) {
                    idxPtn++;
                    cntRemove++;
                    continue;
                }
            }

            /* iterate to next pattern part */
            cntPtn++;
        }

        /* proceed search on left ringbuffer side */
        cntRb = 0;
        idxEnd = idxEndLeft;
    }

    /* check if pattern was found */
    if (cntPtn >= lenPtn) {

        /* remove part before pattern from ringbuffer */
        pRb->idxRd = idxPtn;
        pRb->lenData -= cntRemove;

        /* unlock ringbuffer */
        GOAL_LOCK_PUT(pRb->pLock);

        return GOAL_OK;
    }

    /* unlock ringbuffer */
    GOAL_LOCK_PUT(pRb->pLock);

    return GOAL_ERR_NOT_FOUND;
}


/****************************************************************************/
/** GOAL MCTC - Drop Data From Ringbuffer
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_mctcRbRemove(
    GOAL_MCTC_RB_T *pRb,                        /**< ringbuffer ptr */
    unsigned int len                            /**< data length */
)
{
    /* verify length */
    if (pRb->lenData < len) {
        GOAL_LOG("  * goal_mctcRbRemove: error, removal length (%u) larger than ringbuffer data length (%u)\n", len, pRb->lenData);
        return GOAL_ERR_UNDERFLOW;
    }

    /* lock ringbuffer */
    GOAL_LOCK_GET(pRb->pLock);

    /* update read index and length */
    pRb->idxRd = (pRb->idxRd + len) % pRb->sizeRb;
    pRb->lenData -= len;

    /* unlock ringbuffer */
    GOAL_LOCK_PUT(pRb->pLock);

    return GOAL_OK;
}


/****************************************************************************/
/** GOAL MCTC - Get Data From Ringbuffer At Specific Offset Without Removal
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_mctcRbPeek(
    GOAL_MCTC_RB_T *pRb,                        /**< ringbuffer ptr */
    uint8_t *pData,                             /**< data pointer */
    unsigned int idxStart,                      /**< start index */
    unsigned int lenPeek                        /**< peek length */
)
{
    unsigned int lenRight;                      /* right length */
    unsigned int lenLeft;                       /* left length */
    unsigned int idxRdStart;                    /* read index start */

    /* verify length */
    if (pRb->lenData < (idxStart + lenPeek)) {
        GOAL_LOG("  * goal_mctcRbPeek: error, peek length (%u + %u) larger than ringbuffer data length (%u)\n", idxStart, lenPeek, pRb->lenData);
        return GOAL_ERR_UNDERFLOW;
    }

    /* lock ringbuffer */
    GOAL_LOCK_GET(pRb->pLock);

    /* copy ringbuffer data to buffer */
    idxRdStart = (pRb->idxRd + idxStart) % pRb->sizeRb;
    lenRight = pRb->sizeRb - idxRdStart;
    if (lenRight > lenPeek) {
        lenLeft = 0;
        lenRight = lenPeek;
    } else {
        lenLeft = lenPeek - lenRight;
    }

    GOAL_MEMCPY(pData, &pRb->pData[idxRdStart], lenRight);
    if (lenLeft) {
        GOAL_MEMCPY(&pData[lenRight], pRb->pData, lenLeft);
    }

    /* unlock ringbuffer */
    GOAL_LOCK_PUT(pRb->pLock);

    return GOAL_OK;
}


/****************************************************************************/
/** GOAL MCTC - Fletcher-16 Checksum
 *
 * Source: https://en.wikipedia.org/wiki/Fletcher%27s_checksum#Straightforward
 *
 * @returns Fletcher-16 Checksum
 */
uint16_t goal_mctcFletcher16(
    const uint8_t *pData,                       /**< data pointer */
    unsigned int lenData                        /**< data length */
)
{
    uint16_t sum1 = 0;                          /* sum 1 */
    uint16_t sum2 = 0;                          /* sum 2 */
    unsigned int idx;                           /* index */

    for (idx = 0; idx < lenData; idx++) {
        sum1 = (sum1 + pData[idx]) % 255;
        sum2 = (sum2 + sum1) % 255;
    }

    return (uint16_t) ((sum2 << 8) | sum1);
}
