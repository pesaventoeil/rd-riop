/** @file
 *
 * @brief GOAL nvs Media Interface header
 *
 * @details
 * Generic media interface for nvs.
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

#ifndef GOAL_MI_NVS_H
#define GOAL_MI_NVS_H

#include "goal_ma_nvs.h"


/****************************************************************************/
/* Configuration */
/****************************************************************************/


/****************************************************************************/
/* Forward declaration */
/****************************************************************************/
struct GOAL_MI_NVS_T;
struct GOAL_MI_NVS_REGION_T;


/****************************************************************************/
/* Typedefs */
/****************************************************************************/
typedef enum {
    GOAL_MI_FLASH_STATE_IDLE,                   /**< flash is ready for action */
    GOAL_MI_FLASH_STATE_ERASE_START,            /**< prepare NVS sector erase */
    GOAL_MI_FLASH_STATE_ERASE,                  /**< erase NVS sector */
    GOAL_MI_FLASH_STATE_ERASE_WAIT,             /**< wait for erase to finish */
    GOAL_MI_FLASH_STATE_WRITE_START,            /**< pepare NVS sector write */
    GOAL_MI_FLASH_STATE_WRITE,                  /**< write NVS sector */
    GOAL_MI_FLASH_STATE_WRITE_WAIT,             /**< wait to finish write process */
} GOAL_MI_FLASH_STATE_T;

typedef enum {
    GOAL_MI_NVS_REGION_MODE_COMPLETE = 0,       /**< access region as one block */
    GOAL_MI_NVS_REGION_MODE_STREAM = 1,         /**< access region by offset */
    GOAL_MI_NVS_REGION_MODE_BUFFERED = 2        /**< access buffered and decoupled */
} GOAL_MI_NVS_REGION_MODE_T;

typedef enum {
    GOAL_MI_NVS_REGION_ACCESS_READ = 0,         /**< access region read only */
    GOAL_MI_NVS_REGION_ACCESS_WRITE = 1         /**< access region read and write */
} GOAL_MI_NVS_REGION_ACCESS_T;

typedef struct {
    uint32_t posStart;
    uint32_t length;
    const char *strName;
    unsigned int id;
    GOAL_MI_NVS_REGION_MODE_T mode;
    GOAL_MI_NVS_REGION_ACCESS_T access;
} GOAL_MI_NVS_REGION_LIST_T;


/****************************************************************************/
/* Structures */
/****************************************************************************/
typedef struct GOAL_MI_NVS_T {
    struct GOAL_MI_NVS_T *pNext;                /**< next element */
    GOAL_BOOL_T flgUsed;                        /**< usage flag */
    struct GOAL_MI_NVS_REGION_T *pRegionList;   /**< region list */
    struct GOAL_MA_NVS_T *pMaNvs;               /**< associated MA nvs */
    unsigned int cntRegions;                    /**< number of regions */
} GOAL_MI_NVS_T;

typedef struct GOAL_MI_NVS_REGION_WRDESC_T {
    uint8_t *pBuffer;                           /**< buffer in buffered mode */
    GOAL_MI_FLASH_STATE_T nvsState;             /**< nvs state */
    GOAL_MI_FLASH_STATE_T nvsStateNext;         /**< nvs state */
    GOAL_BOOL_T nvsTriggerWrite;                /**< write trigger */
    uint32_t offset;                            /**< erase/write offset */
    uint32_t pos;                               /**< erase/write position */
    uint32_t len;                               /**< erase/write length */
} GOAL_MI_NVS_REGION_WRDESCT_T;

typedef struct GOAL_MI_NVS_REGION_T {
    struct GOAL_MI_NVS_REGION_T *pNext;         /**< next element */
    struct GOAL_MI_NVS_T *pMiNvs;               /**< associated MI */
    uint32_t offset;                            /**< region offset */
    uint32_t length;                            /**< region length */
    const char *strName;                        /**< region name */
    unsigned int id;                            /**< region id */
    GOAL_MI_NVS_REGION_MODE_T mode;             /**< region mode */
    GOAL_MI_NVS_REGION_ACCESS_T access;         /**< region access */
    GOAL_MI_NVS_REGION_WRDESCT_T wrDesc;        /**< write descriptor */
} GOAL_MI_NVS_REGION_T;

typedef struct GOAL_MI_NVS_BUFFERED_REGION_LIST_T {
    struct GOAL_MI_NVS_BUFFERED_REGION_LIST_T *pNext; /**< next pointer */
    GOAL_MI_NVS_REGION_T *pNvsRegion;
} GOAL_MI_NVS_BUFFERED_REGION_LIST_T;


/****************************************************************************/
/* FLASH MI interface */
/****************************************************************************/

GOAL_STATUS_T goal_miNvsReg(
    GOAL_MI_NVS_T **ppMiNvs,                    /**< flash driver handle */
    GOAL_ID_T maId,                             /**< nvs ma id */
    GOAL_MI_NVS_REGION_LIST_T *pNvsRegionList,  /**< list of regions */
    int cntRegions                              /**< number of regions in list */
);

GOAL_STATUS_T goal_miNvsRegRegion(
    GOAL_MI_NVS_REGION_T **ppRegion,            /**< region handle */
    GOAL_MI_NVS_T *pMiNvs,                      /**< flash driver handle */
    GOAL_MA_NVS_T *pMaNvs,                      /**< flash MA to use */
    uint32_t offset,                            /**< offset of region */
    uint32_t len,                               /**< length of region */
    const char *strName,                        /**< name of region */
    unsigned int id,                            /**< region id */
    GOAL_MI_NVS_REGION_MODE_T mode,             /**< region mode */
    GOAL_MI_NVS_REGION_ACCESS_T access          /**< access mode */
);

GOAL_STATUS_T goal_miNvsRegionGetById(
    GOAL_MI_NVS_REGION_T **ppRegion,            /**< ptr ref for region */
    unsigned int id                             /**< region id */
);

GOAL_STATUS_T goal_miNvsRegionOpen(
    GOAL_MI_NVS_REGION_T *pRegion               /**< flash driver handle */
);

GOAL_STATUS_T goal_miNvsWrite(
    GOAL_MI_NVS_REGION_T *pRegion,              /**< flash driver handle */
    uint8_t *pData,                             /**< data ptr */
    uint32_t offset,                            /**< write offset */
    uint32_t len                                /**< data length */
);

GOAL_STATUS_T goal_miNvsRead(
    GOAL_MI_NVS_REGION_T *pRegion,              /**< flash driver handle */
    uint8_t *pData,                             /**< data ptr */
    uint32_t offset,                            /**< read offset */
    uint32_t len                                /**< data length */
);

GOAL_STATUS_T goal_miNvsErase(
    GOAL_MI_NVS_REGION_T *pRegion               /**< flash driver handle */
);

#endif /* GOAL_MI_NVS_H */

