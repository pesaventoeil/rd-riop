/** @file
 *
 * @brief GOAL section Media Adapter header
 *
 * @details
 * Generic media adapter for section.
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

#ifndef GOAL_MA_SECTION_H
#define GOAL_MA_SECTION_H


/****************************************************************************/
/* Forward declaration */
/****************************************************************************/
struct GOAL_MA_SECTION_T;


/****************************************************************************/
/* Typedefs */
/****************************************************************************/
typedef GOAL_STATUS_T (*GOAL_MA_SECTION_READ_T)(
    struct GOAL_MA_SECTION_T *pSection,         /**< Section handle */
    uint32_t offset,                            /**< read offset */
    uint8_t *pData,                             /**< data ptr */
    uint32_t len                                /**< data length */
);

typedef GOAL_STATUS_T (*GOAL_MA_SECTION_WRITE_T)(
    struct GOAL_MA_SECTION_T *pSection,         /**< Section handle */
    uint32_t offset,                            /**< write offset */
    uint8_t  *pData,                            /**< data ptr */
    uint32_t len                                /**< data length */
);


/****************************************************************************/
/* Structures */
/****************************************************************************/
typedef struct GOAL_MA_SECTION_T {
    struct GOAL_MA_SECTION_T *pNext;            /**< next element */
    unsigned int sectionId;                     /**< section id */
    unsigned int sectionSize;                   /**< section size */
    GOAL_MA_SECTION_READ_T funcRead;            /**< driver read function */
    GOAL_MA_SECTION_WRITE_T funcWrite;          /**< driver write function */
    GOAL_BOOL_T flgUsed;                        /**< usage flag */
    char *pSectionRaw;                          /**< raw section data */
    void *pData;                                /**< driver specific data */
} GOAL_MA_SECTION_T;


/****************************************************************************/
/* SECTION MA interface */
/****************************************************************************/
GOAL_STATUS_T goal_maSectionReg(
    GOAL_MA_SECTION_T **ppSection,              /**< section driver handle */
    unsigned int sectionId,                     /**< section id */
    unsigned int sectionSize,                   /**< section size */
    char *pSectionRaw,                          /**< section ptr */
    GOAL_MA_SECTION_READ_T funcRead,            /**< section read function */
    GOAL_MA_SECTION_WRITE_T funcWrite,          /**< section write function */
    void *pData                                 /**< driver specific data */
);

GOAL_STATUS_T goal_maSectionAlloc(
    GOAL_MA_SECTION_T *pSection                 /**< section driver handle */
);

GOAL_STATUS_T goal_maSectionFree(
    GOAL_MA_SECTION_T *pSection                 /**< section driver handle */
);

GOAL_STATUS_T goal_maSectionWrite(
    GOAL_MA_SECTION_T *pSection,                /**< section driver handle */
    uint32_t offset,                            /**< write offset */
    uint8_t *pData,                             /**< data ptr */
    uint32_t len                                /**< data length */
);

GOAL_STATUS_T goal_maSectionRead(
    GOAL_MA_SECTION_T *pSection,                /**< section driver handle */
    uint32_t offset,                            /**< write offset */
    uint8_t *pData,                             /**< data ptr */
    uint32_t len                                /**< data length */
);

GOAL_STATUS_T goal_maSectionGetById(
    GOAL_MA_SECTION_T **ppSection,              /**< ptr ref for section */
    unsigned int id                             /**< region id */
);


#endif /* GOAL_MA_SECTION_H */

