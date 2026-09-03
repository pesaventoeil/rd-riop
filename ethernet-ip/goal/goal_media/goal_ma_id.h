/** @file
 *
 * @brief Generic MA for external ID configuration (e.g. rotary switches)
 *
 * @details
 * This module provides a MA for a configuration ID
 *
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
#ifndef GOAL_MA_ID_H
#define GOAL_MA_ID_H


/****************************************************************************/
/* Defines */
/****************************************************************************/


/****************************************************************************/
/* Forward declaration */
/****************************************************************************/
struct GOAL_MA_ID_T;                            /**< MA ID handle */


/****************************************************************************/
/* Typedefs */
/****************************************************************************/

/**< open function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_ID_OPEN)(
    struct GOAL_MA_ID_T *pHdlMaId               /**< MA handle for ID  */
);

/**< close function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_ID_CLOSE)(
    struct GOAL_MA_ID_T *pHdlMaId               /**< MA handle for ID  */
);

/**< get function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_ID_GET)(
    struct GOAL_MA_ID_T *pHdlMaId,              /**< MA handle for ID  */
    uint32_t *pId                               /**< ID return value */
);

/**< ID handle instance struct */
typedef struct GOAL_MA_ID_T {
    GOAL_INSTANCE_HEADER(GOAL_MA_ID_T);         /**< instance header */
    GOAL_MA_ID_OPEN fctOpen;                    /**< open ID function */
    GOAL_MA_ID_GET fctGet;                      /**< get ID function */
    GOAL_MA_ID_CLOSE fctClose;                  /**< close ID function */
    GOAL_BOOL_T flgUsed;                        /**< used flag */
    void *pDrvData;                             /**< driver specific data */
} GOAL_MA_ID_T;


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_maIdDrvReg(
    uint32_t id,                                /**< ID */
    GOAL_MA_ID_T **ppHdlMaId,                   /**< ID handle ref ptr */
    GOAL_MA_ID_OPEN fOpen,                      /**< open function */
    GOAL_MA_ID_GET fGet,                        /**< get function */
    GOAL_MA_ID_CLOSE fClose,                    /**< close function */
    void *pDrvData                              /**< driver specific data */
);

GOAL_STATUS_T goal_maIdOpen(
    uint32_t id,                                /**< id of ID handler to use */
    GOAL_MA_ID_T **ppIdHdl                      /**< pointer to store ID handler */
);

GOAL_STATUS_T goal_maIdGet(
    GOAL_MA_ID_T *pIdHdl,                       /**< MA handle for ID */
    uint32_t *pId                               /**< [out] id value */
);

GOAL_STATUS_T goal_maIdClose(
    GOAL_MA_ID_T *pIdHdl                        /**< MA handle for ID */
);

GOAL_STATUS_T goal_maIdGetById(
    GOAL_MA_ID_T **ppHdlMaId,                   /**< ID handle ref ptr */
    uint32_t id                                 /**< MA id */
);

#endif /* GOAL_MA_ID_H */
