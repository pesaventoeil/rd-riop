/** @file
 *
 * @brief GOAL MI DM - Data Mapper (Synchronized Partitioned Exchange of Data)
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
#ifndef GOAL_MI_DM_H
#define GOAL_MI_DM_H


/****************************************************************************/
/* Defines */
/****************************************************************************/
#define GOAL_MI_DM_CNT_BUF_PART 3               /**< count of partition buffers */

#define GOAL_MI_DM_ID_BUF_READ 0                /**< initial read buffer */
#define GOAL_MI_DM_ID_BUF_WRITE 1               /**< initial write buffer */
#define GOAL_MI_DM_ID_BUF_XCHG 2                /**< initial exchange buffer */

#define GOAL_MI_DM_XCHG_READ GOAL_TRUE          /**< exchange read buffer */
#define GOAL_MI_DM_XCHG_WRITE GOAL_FALSE        /**< exchange write buffer */

#define GOAL_MI_DM_CB_READ 0                    /**< read callback */
#define GOAL_MI_DM_CB_WRITE 1                   /**< write callback */


/****************************************************************************/
/* Datatypes */
/****************************************************************************/
struct GOAL_MI_DM_GROUP_T;


/**< MI DM Callback Function */
typedef GOAL_STATUS_T (* GOAL_MI_DM_CB_FUNC_T)(
    struct GOAL_MI_DM_GROUP_T *pGroup,          /**< [in] group handle */
    void *pPriv                                 /**< [in] private data */
);


/**< MI DM Callback Data */
typedef struct GOAL_MI_DM_CB_T {
    struct GOAL_MI_DM_CB_T *pNext;              /**< next element */

    GOAL_MI_DM_CB_FUNC_T funcCb;                /**< callback function */
    void *pPriv;                                /**< private callback data */
} GOAL_MI_DM_CB_T;


/**< MI DM Group Data */
typedef struct GOAL_MI_DM_GROUP_T {
    struct GOAL_MI_DM_GROUP_T *pNext;           /**< next element */

    uint32_t id;                                /**< group id */
    struct GOAL_MI_DM_T *pMiDm;                 /**< MI DM instance */
    struct GOAL_MI_DM_PART_T *pParts;           /**< partition list */
    struct GOAL_MI_DM_CB_T *pCbsRead;           /**< read callback list */
    struct GOAL_MI_DM_CB_T *pCbsWrite;          /**< write callback list */
    GOAL_LOCK_T *pLock;                         /**< group lock */
    GOAL_BOOL_T flgSync;                        /**< group was updated */
    unsigned int cntActiveRead;                 /**< active read counter */
    unsigned int cntActiveWrite;                /**< active write counter */
} GOAL_MI_DM_GROUP_T;


/**< MI DM Partition Data */
typedef struct GOAL_MI_DM_PART_T {
    struct GOAL_MI_DM_PART_T *pNext;            /**< next element - group list */
    struct GOAL_MI_DM_PART_T *pNextAll;         /**< next element - MI list */

    GOAL_MI_DM_GROUP_T *pGroup;                 /**< group info */
    uint32_t idxPart;                           /**< partition data index */
    uint32_t len;                               /**< partition length */
    GOAL_BOOL_T flgRead;                        /**< request read buffer exchange */
    GOAL_BOOL_T flgWrite;                       /**< request write buffer exchange */
    GOAL_BOOL_T flgActive;                      /**< active flag */
    uint8_t *pBufRead;                          /**< read buffer */
    uint8_t *pBufWrite;                         /**< write buffer */
    uint8_t *pBufXchg;                          /**< exchange buffer */
} GOAL_MI_DM_PART_T;


/**< MI DM Instance Data */
typedef struct GOAL_MI_DM_T {
    GOAL_INSTANCE_HEADER(GOAL_MI_DM_T);         /**< instance header */

    GOAL_MI_DM_GROUP_T *pGroups;                /**< group list */
    GOAL_MI_DM_PART_T *pParts;                  /**< partition list */
    uint8_t *pBufPart[GOAL_MI_DM_CNT_BUF_PART]; /**< partition buffers */
    unsigned int len;                           /**< buffer length */

    struct GOAL_MI_DM_CB_T *pCbsRead;           /**< read callback list */
    struct GOAL_MI_DM_CB_T *pCbsWrite;          /**< write callback list */
} GOAL_MI_DM_T;


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_miDmNew(
    GOAL_MI_DM_T **ppMiDm,                      /**< [out] MI DM handle */
    uint32_t id,                                /**< handle id */
    unsigned int len,                           /**< buffer length */
    const char *strDesc                         /**< description */
);

GOAL_STATUS_T goal_miDmGroupNew(
    GOAL_MI_DM_GROUP_T **ppGroup,               /**< [out] group handle */
    uint32_t idMiDm,                            /**< MI DM id */
    GOAL_ID_T idGroup                           /**< group id */
);

GOAL_STATUS_T goal_miDmPartReg(
    uint32_t idMiDm,                            /**< MI DM id */
    GOAL_ID_T idGroup,                          /**< group id */
    GOAL_MI_DM_PART_T *pPart,                   /**< partition data */
    uint32_t lenPart                            /**< partition length */
);

GOAL_STATUS_T goal_miDmPartIdxReg(
    uint32_t idMiDm,                            /**< DM handle id */
    GOAL_ID_T idGroup,                          /**< group id */
    GOAL_MI_DM_PART_T *pPart,                   /**< partition data */
    uint32_t lenPart,                           /**< partition length */
    uint32_t idx                                /**< partition index */
);

uint32_t goal_miDmPartIdxGet(
    GOAL_MI_DM_PART_T *pPart                    /**< partition data */
);

uint32_t goal_miDmPartSizeGet(
    GOAL_MI_DM_PART_T *pPart                    /**< partition data */
);

GOAL_BOOL_T goal_miDmPartStatusGet(
    GOAL_MI_DM_PART_T *pPart                    /**< partition data */
);

GOAL_STATUS_T goal_miDmSingleWrite(
    GOAL_MI_DM_PART_T *pPart,                   /**< partition data */
    uint8_t *pBuf,                              /**< data source */
    unsigned int len                            /**< write length */
);

GOAL_STATUS_T goal_miDmGroupPartWrite(
    GOAL_MI_DM_PART_T *pPart,                   /**< partition data */
    uint8_t *pBuf,                              /**< data source */
    unsigned int len                            /**< write length */
);

GOAL_STATUS_T goal_miDmGroupWriteStart(
    GOAL_MI_DM_GROUP_T *pGroup                  /**< partition group */
);

GOAL_STATUS_T goal_miDmGroupWriteBufGet(
    uint8_t **ppBuf,                            /**< [out] partition pointer */
    uint32_t *pLen,                             /**< partition length */
    GOAL_MI_DM_PART_T *pPart                    /**< partition data */
);

GOAL_STATUS_T goal_miDmGroupWriteEnd(
    GOAL_MI_DM_GROUP_T *pGroup                  /**< partition group */
);

GOAL_STATUS_T goal_miDmWriteSync(
    GOAL_MI_DM_T *pMiDm,                        /**< MI DM */
    uint8_t *pBuf                               /**< target buffer */
);

GOAL_STATUS_T goal_miDmSingleRead(
    GOAL_MI_DM_PART_T *pPart,                   /**< partition data */
    uint8_t *pBuf                               /**< data source */
);

GOAL_STATUS_T goal_miDmSingleReadBufGet(
    uint8_t **ppBuf,                            /**< [out] partition pointer */
    uint32_t *pLen,                             /**< partition length */
    GOAL_MI_DM_PART_T *pPart                    /**< partition data */
);

GOAL_STATUS_T goal_miDmGroupPartRead(
    GOAL_MI_DM_PART_T *pPart,                   /**< partition data */
    uint8_t *pBuf                               /**< data source */
);

GOAL_STATUS_T goal_miDmGroupReadStart(
    GOAL_MI_DM_GROUP_T *pGroup                  /**< partition group */
);

GOAL_STATUS_T goal_miDmGroupReadBufGet(
    uint8_t **ppBuf,                            /**< [out] partition pointer */
    uint32_t *pLen,                             /**< partition length */
    GOAL_MI_DM_PART_T *pPart                    /**< partition data */
);

GOAL_STATUS_T goal_miDmGroupReadEnd(
    GOAL_MI_DM_GROUP_T *pGroup                  /**< partition group */
);

GOAL_STATUS_T goal_miDmReadSync(
    GOAL_MI_DM_T *pMiDm,                        /**< MI DM */
    uint8_t *pBuf                               /**< read buffer */
);

GOAL_STATUS_T goal_miDmGroupGetByIdx(
    GOAL_MI_DM_GROUP_T **ppGroup,               /**< [out] group handle */
    uint32_t idMiDm,                            /**< MI DM id */
    GOAL_ID_T idGroup                           /**< group id */
);

GOAL_STATUS_T goal_miDmInstGetByPart(
    GOAL_MI_DM_T **ppDm,                        /**< [out] MI DM handle */
    GOAL_MI_DM_PART_T *pPart                    /**< part handle */
);

GOAL_STATUS_T goal_miDmCbReg(
    GOAL_MI_DM_T *pMiDm,                        /**< [in] MI DM handle */
    GOAL_MI_DM_GROUP_T *pGroup,                 /**< [in] MI DM group handle */
    uint32_t typeCb,                            /**< callback type */
    GOAL_MI_DM_CB_FUNC_T funcCb,                /**< callback function */
    void *pPriv                                 /**< callback private data */
);

GOAL_STATUS_T goal_miDmGroupPartsRemove(
    uint32_t idMiDm,                            /**< DM handle id */
    GOAL_ID_T idGroup                           /**< group id */
);

#endif /* GOAL_MI_DM_H */
