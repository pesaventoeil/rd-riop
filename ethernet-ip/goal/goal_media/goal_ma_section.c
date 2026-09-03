/** @file
 *
 * @brief GOAL section Media Adapter
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

#define GOAL_ID GOAL_ID_MA_SECTION
#include <goal_includes.h>
#include <goal_media/goal_ma_section.h>


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_MA_SECTION_T *pListSection = NULL;  /**< section list */
static unsigned int cntSection = 0;             /**< count of registered sections */


/****************************************************************************/
/** Register section driver
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maSectionReg(
    GOAL_MA_SECTION_T **ppSection,              /**< section driver handle */
    unsigned int sectionId,                     /**< section id */
    unsigned int sectionSize,                   /**< section id */
    char *pSectionRaw,                          /**< section ptr */
    GOAL_MA_SECTION_READ_T funcRead,            /**< section read function */
    GOAL_MA_SECTION_WRITE_T funcWrite,          /**< section write function */
    void *pData                                 /**< driver specific data */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MA_SECTION_T **ppElem;                 /* section element */

    /* allocate section handle */
    res = goal_memCalloc(ppSection, sizeof(GOAL_MA_SECTION_T));

    if (GOAL_RES_OK(res)) {
        /* setup section handle */
        (*ppSection)->funcRead = funcRead;
        (*ppSection)->funcWrite = funcWrite;
        (*ppSection)->pData = pData;
        (*ppSection)->sectionId = sectionId;
        (*ppSection)->sectionSize = sectionSize;
        (*ppSection)->pSectionRaw = pSectionRaw;

        /* iterate to list end */
        for (ppElem = &pListSection; *ppElem; ppElem = &(*ppElem)->pNext);

        /* store section handle */
        *ppElem = *ppSection;
        cntSection++;
    }

    return res;
}


/****************************************************************************/
/** section allocate function
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maSectionAlloc(
    GOAL_MA_SECTION_T *pSection                 /**< section driver handle */
)
{
    if (pSection == NULL) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (GOAL_FALSE != pSection->flgUsed) {
        return GOAL_ERR_ALREADY_USED;
    }

    pSection->flgUsed = GOAL_TRUE;

    return GOAL_OK;
}


/****************************************************************************/
/** section free function
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maSectionFree(
    GOAL_MA_SECTION_T *pSection                 /**< section driver handle */
)
{
    if (pSection == NULL) {
        return GOAL_ERR_NULL_POINTER;
    }

     if (GOAL_FALSE == pSection->flgUsed) {
        return GOAL_ERR_WRONG_STATE;
    }

    pSection->flgUsed = GOAL_FALSE;

    return GOAL_OK;
}


/****************************************************************************/
/** get section by id
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maSectionGetById(
    GOAL_MA_SECTION_T **ppSection,              /**< ptr ref for section */
    unsigned int id                             /**< region id */
)
{
    GOAL_MA_SECTION_T *pMaElem;                 /* MI element */
    GOAL_STATUS_T res = GOAL_OK;                /* return value */

    for (pMaElem = pListSection ; (pMaElem) && pMaElem->sectionId != id; pMaElem = pMaElem->pNext);

    if (NULL == pMaElem) {
        res = GOAL_ERR_NULL_POINTER;
    }

    if (GOAL_RES_OK(res)) {
        *ppSection = pMaElem;
    }

    return res;
}


/****************************************************************************/
/** section read function
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maSectionRead(
    GOAL_MA_SECTION_T *pSection,                /**< section driver handle */
    uint32_t offset,                            /**< write offset */
    uint8_t *pData,                             /**< data ptr */
    uint32_t len                                /**< data length */
)
{
    if (pSection == NULL) {
        return GOAL_ERR_NULL_POINTER;
    }

     if (NULL != pSection->funcRead) {
        pSection->funcRead(pSection, offset, pData, len);
    }

     return GOAL_OK;
}


/****************************************************************************/
/** section write function
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maSectionWrite(
    GOAL_MA_SECTION_T *pSection,                /**< section driver handle */
    uint32_t offset,                            /**< write offset */
    uint8_t *pData,                             /**< data ptr */
    uint32_t len                                /**< data length */
)
{
    if (pSection == NULL) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (NULL != pSection->funcWrite) {
        pSection->funcWrite(pSection, offset, pData, len);
    }

    return GOAL_OK;
}

