/** @file
 *
 * @brief GOAL Micro Core To Core - Ringbuffer Target
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

#ifndef GOAL_MCTC_RB_H
#define GOAL_MCTC_RB_H

#include "goal_mctc.h"


/****************************************************************************/
/* Defines */
/****************************************************************************/
/**< GOAL MCTC - start sequence */
#define GOAL_MCTC_SEQ_START { 0xaa, 0xee }

/**< GOAL MCTC - minimum data length (data hdr) */
#define GOAL_MCTC_LEN_DATA_MIN sizeof(GOAL_MCTC_HDR_RPC_T)

/**< GOAL MCTC - minimum valid frame length (hdr + data + CRC) */
#define GOAL_MCTC_LEN_FRAME_MIN (sizeof(GOAL_MCTC_HDR_T) + GOAL_MCTC_LEN_DATA_MIN + sizeof(GOAL_MCTC_CRC_T))

/**< GOAL MCTC - Fletcher16 offset to avoid false positives for 0x00 and 0xff */
#define GOAL_MCTC_FLETCHER_OFFSET 5

/**< GOAL MCTC - offsetof emulation from Wikipedia */
#define GOAL_MCTC_OFSOF(st, m) ((size_t) &(((st *) 0)->m))

/**< GOAL Log Ids (see goal_db.xls) */
#define GOAL_LOG_ID_RB_REMOVE_FAILED 54


/****************************************************************************/
/* Forward declarations */
/****************************************************************************/
struct GOAL_MCTC_RB_T;


/****************************************************************************/
/* Datatypes */
/****************************************************************************/
/**< GOAL MCTC - checksum */
typedef uint16_t GOAL_MCTC_CRC_T;

/**< GOAL MCTC - function overwrite */
typedef GOAL_STATUS_T (*GOAL_MCTC_RB_FUNC_OVERWRITE)(
    struct GOAL_MCTC_RB_T *pRb                  /**< ringbuffer */
);


/****************************************************************************/
/* Structures */
/****************************************************************************/
/**< ringbuffer data */
typedef struct GOAL_MCTC_RB_T {
    unsigned int idxRd;                         /**< read index */
    unsigned int idxWr;                         /**< write index */
    unsigned int sizeRb;                        /**< ringbuffer size */
    unsigned int sizeElem;                      /**< maximum element size */
    uint8_t *pData;                             /**< data pointer */
    uint8_t *pDataElem;                         /**< element data pointer */
    unsigned int lenData;                       /**< data length */
    GOAL_MCTC_RB_FUNC_OVERWRITE funcOverwrite;  /**< overwrite callback function */
    GOAL_LOCK_T *pLock;                         /**< lock */
} GOAL_MCTC_RB_T;


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_mctcRbNew(
    GOAL_MCTC_TGT_T **ppTgt,                    /**< [out] MCTC target */
    unsigned int sizeRb,                        /**< ringbuffer size */
    unsigned int sizeElem,                      /**< maximum element size */
    GOAL_STATUS_T (*funcOverwrite)(GOAL_MCTC_RB_T *pRb) /**< overwrite callback function */
);

GOAL_STATUS_T goal_mctcRbPut(
    GOAL_MCTC_RB_T *pRb,                        /**< ringbuffer ptr */
    uint8_t *pData,                             /**< data pointer */
    unsigned int lenData                        /**< data length */
);

GOAL_STATUS_T goal_mctcRbGet(
    GOAL_MCTC_RB_T *pRb,                        /**< ringbuffer ptr */
    uint8_t *pData,                             /**< data pointer */
    unsigned int lenData                        /**< data length */
);

unsigned int goal_mctcRbLenGet(
    GOAL_MCTC_RB_T *pRb                         /**< ringbuffer ptr */
);

unsigned int goal_mctcRbLenFreeGet(
    GOAL_MCTC_RB_T *pRb                         /**< ringbuffer ptr */
);

GOAL_STATUS_T goal_mctcRbPtnSeek(
    GOAL_MCTC_RB_T *pRb,                        /**< ringbuffer ptr */
    uint8_t *pPtn,                              /**< pattern */
    unsigned int lenPtn                         /**< pattern length */
);

GOAL_STATUS_T goal_mctcRbRemove(
    GOAL_MCTC_RB_T *pRb,                        /**< ringbuffer ptr */
    unsigned int len                            /**< data length */
);

GOAL_STATUS_T goal_mctcRbPeek(
    GOAL_MCTC_RB_T *pRb,                        /**< ringbuffer ptr */
    uint8_t *pData,                             /**< data pointer */
    unsigned int idxStart,                      /**< start index */
    unsigned int lenPeek                        /**< peek length */
);

uint16_t goal_mctcFletcher16(
    const uint8_t *pData,                       /**< data pointer */
    unsigned int lenData                        /**< data length */
);


#endif /* GOAL_MCTC_RB_H */
