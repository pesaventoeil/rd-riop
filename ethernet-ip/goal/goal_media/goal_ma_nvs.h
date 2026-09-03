/** @file
 *
 * @brief GOAL nvs Media Adapter header
 *
 * @details
 * Generic media adapter for nvs.
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

#ifndef GOAL_MA_NVS_H
#define GOAL_MA_NVS_H


/****************************************************************************/
/* Definitions */
/****************************************************************************/
#define GOAL_MA_NVS_FLAG_MEM_READ        (1<<0) /**< memory mapped read access */
#define GOAL_MA_NVS_FLAG_ERASE_REQUIRED  (1<<1) /**< technology requires erase before writing */
#define GOAL_MA_NVS_FLAG_FLASH           (1<<2) /**< flash technology available */
#define GOAL_MA_NVS_FLAG_ERASE_STATE_ON  (1<<3) /**< erase state is all ones */
#define GOAL_MA_NVS_FLAG_ERASE_STATE_OFF (1<<4) /**< erase state os all zeros */


/****************************************************************************/
/* Forward declaration */
/****************************************************************************/
struct GOAL_MA_NVS_T;
struct GOAL_MA_NVS_DESC_T;


/****************************************************************************/
/* Typedefs */
/****************************************************************************/
typedef GOAL_STATUS_T (*GOAL_MA_NVS_OPEN_T)(
    struct GOAL_MA_NVS_T *pNvs                  /**< nvs handle */
);

typedef GOAL_STATUS_T (*GOAL_MA_NVS_READ_T)(
    struct GOAL_MA_NVS_T *pNvs,                 /**< nvs handle */
    uint32_t offset,                            /**< read offset */
    uint8_t *pData,                             /**< data ptr */
    uint32_t len                                /**< data length */
);

typedef GOAL_STATUS_T (*GOAL_MA_NVS_WRITE_T)(
    struct GOAL_MA_NVS_T *pNvs,                 /**< nvs handle */
    uint32_t offset,                            /**< write offset */
    uint8_t  *pData,                            /**< data ptr */
    uint32_t len                                /**< data length */
);

typedef GOAL_STATUS_T (*GOAL_MA_NVS_ERASE_T)(
    struct GOAL_MA_NVS_T *pNvs,                 /**< nvs handle */
    uint32_t offset,                            /**< erase offset */
    uint32_t len                                /**< data length */
);

typedef GOAL_STATUS_T (*GOAL_MA_NVS_BUSY_T)(
    struct GOAL_MA_NVS_T *pNvs,                 /**< nvs handle */
    GOAL_BOOL_T *pStatus                        /**< return status */
);


/****************************************************************************/
/* Structures */
/****************************************************************************/
/** flash chip description */
typedef struct {
    const char *strName;                        /**< flash name */
    uint32_t sizeFlash;                         /**< total flash size */
    uint32_t sizeErase;                         /**< size of erasable sectors */
    uint32_t sizeProgram;                       /**< size of a programmable page */
    uint32_t flags;                             /**< flash properties */
} GOAL_MA_NVS_INFO_T;

typedef struct GOAL_MA_NVS_T {
    struct GOAL_MA_NVS_T *pNext;                /**< next element */
    GOAL_ID_T id;                               /**< MA id */
    GOAL_MA_NVS_OPEN_T funcOpen;                /**< driver open function */
    GOAL_MA_NVS_READ_T funcRead;                /**< driver read function */
    GOAL_MA_NVS_WRITE_T funcWrite;              /**< driver write function */
    GOAL_MA_NVS_ERASE_T funcErase;              /**< driver erase function */
    GOAL_MA_NVS_BUSY_T funcBusy;                /**< driver busy function */
    GOAL_BOOL_T flgUsed;                        /**< usage flag */
    GOAL_MA_NVS_INFO_T infoFlash;               /**< flash description */
    void *pData;                                /**< driver specific data */
} GOAL_MA_NVS_T;


/****************************************************************************/
/* FLASH MA interface */
/****************************************************************************/
GOAL_STATUS_T goal_maNvsReg(
    GOAL_ID_T id,                               /**< MA id */
    GOAL_MA_NVS_T **ppNvs,                      /**< flash driver handle */
    GOAL_MA_NVS_OPEN_T funcOpen,                /**< flash open function */
    GOAL_MA_NVS_READ_T funcRead,                /**< flash read function */
    GOAL_MA_NVS_WRITE_T funcWrite,              /**< flash write function */
    GOAL_MA_NVS_ERASE_T funcErase,              /**< flash erase function */
    GOAL_MA_NVS_BUSY_T funcBusy,                /**< flash busy function */
    void *pData                                 /**< driver specific data */
);

GOAL_STATUS_T goal_maNvsOpen(
    GOAL_MA_NVS_T *pNvs                         /**< flash driver handle */
);

GOAL_STATUS_T goal_maNvsClose(
    GOAL_MA_NVS_T *pNvs                         /**< flash driver handle */
);

GOAL_STATUS_T goal_maNvsWrite(
    GOAL_MA_NVS_T *pNvs,                        /**< flash driver handle */
    uint32_t offset,                            /**< write offset */
    uint8_t *pData,                             /**< data ptr */
    uint32_t len                                /**< data length */
);

GOAL_STATUS_T goal_maNvsRead(
    GOAL_MA_NVS_T *pNvs,                        /**< flash driver handle */
    uint32_t offset,                            /**< write offset */
    uint8_t *pData,                             /**< data ptr */
    uint32_t len                                /**< data length */
);

GOAL_STATUS_T goal_maNvsErase(
    GOAL_MA_NVS_T *pNvs,                        /**< flash driver handle */
    uint32_t offset,                            /**< erase offset */
    uint32_t len                                /**< data length */
);

GOAL_STATUS_T goal_maNvsBusy(
    GOAL_MA_NVS_T *pNvs,                        /**< flash driver handle */
    GOAL_BOOL_T *pStatus                        /**< return status */
);

GOAL_STATUS_T goal_maNvsGetById(
    GOAL_MA_NVS_T **ppHdlMa,                    /**< handle */
    GOAL_ID_T id                                /**< MA id */
);

#endif /* GOAL_MA_NVS_H */

