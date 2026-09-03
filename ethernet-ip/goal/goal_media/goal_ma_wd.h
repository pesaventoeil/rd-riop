/** @file
 *
 * @brief Generic MA for WD
 *
 * @details
 * This module provides a MA for WD
 *
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
#ifndef GOAL_MA_WD_H
#define GOAL_MA_WD_H


/****************************************************************************/
/* Defines */
/****************************************************************************/


/****************************************************************************/
/* Forward declaration */
/****************************************************************************/
struct GOAL_MA_WD_T;                            /**< MA WD handle */


/****************************************************************************/
/* Typedefs */
/****************************************************************************/

/**< open function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_WD_OPEN)(
    struct GOAL_MA_WD_T *pHdlMaWd               /**< MA handle for WD  */
);

/**< start function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_WD_START) (
    struct GOAL_MA_WD_T *pHdlMaWd               /**< MA handle for WD  */
);

/**< clear function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_WD_CLEAR)(
    struct GOAL_MA_WD_T *pHdlMaWd               /**< MA handle for WD  */
);

/**< WD handle instance struct */
typedef struct GOAL_MA_WD_T {
    GOAL_INSTANCE_HEADER(GOAL_MA_WD_T);         /**< instance header */
    GOAL_BOOL_T flgUsed;                        /**< used flag */
    GOAL_MA_WD_OPEN fctOpen;                    /**< open WD function */
    GOAL_MA_WD_START fctStart;                  /**< start WD function */
    GOAL_MA_WD_CLEAR fctClear;                  /**< clear WD function */
    uint32_t windowSize;                        /**< window size in percent */
    uint32_t overflowThreshold;                 /**< overflow time in ms */
    void *pDrvData;                             /**< driver specific data */
} GOAL_MA_WD_T;


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_maWdDrvReg(
    uint32_t id,                                /**< ID */
    GOAL_MA_WD_T **ppHdlMaWd,                   /**< WD handle ref ptr */
    GOAL_MA_WD_OPEN fOpen,                      /**< open function */
    GOAL_MA_WD_START fStart,                    /**< start function */
    GOAL_MA_WD_CLEAR fClear,                    /**< clear function */
    void *pDrvData                              /**< driver specific data */
);

GOAL_STATUS_T goal_maWdOpen(
    uint32_t id,                                /**< id of WD handler to use */
    GOAL_MA_WD_T **ppWdHdl                      /**< pointer to store WD handler */
);

GOAL_STATUS_T goal_maWdStart(
    GOAL_MA_WD_T *pWdHdl                        /**< MA handle for WD */
);

GOAL_STATUS_T goal_maWdClear(
    GOAL_MA_WD_T *pWdHdl                        /**< pointer to store WD handler */
);

GOAL_STATUS_T goal_maWdGetById(
    GOAL_MA_WD_T **ppHdlMaWd,                   /**< WD handle ref ptr */
    uint32_t id                                 /**< MA id */
);

#endif /* GOAL_MA_WD_H */
