/** @file
 *
 * @brief Goal http resource manager
 *
 * This is the goal webserver resource (html, img, etc,) manager.
 *
 * @copyright
 * Copyright 2010-2017 port GmbH Halle/Saale.
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


#ifndef GOAL_HTTP_RESMGR_H
#define GOAL_HTTP_RESMGR_H

#include <goal_includes.h>


/****************************************************************************/
/* Defines */
/****************************************************************************/


/****************************************************************************/
/* Data types */
/****************************************************************************/

/** linked list, resource struct */
typedef struct HTTP_RESMGR_LL_RES_T {
    struct HTTP_RESMGR_LL_RES_T *pNext;         /**< pointer to next element */
    struct HTTP_RESMGR_LL_RES_T *pChild;        /**< pointer to child element */
    uint8_t *strUrl;                            /**< resource name */
    uint32_t lenUrl;                            /**< resource name length */
    GOAL_HTTP_HDL_T hdlRes;                     /**< resource handle */
    uint16_t id;                                /**< resource id */
    uint32_t accMeth;                           /**< allowed access methods */
    GOAL_HTTP_REQCB_T pfnCbData;                /**< application callback */
    GOAL_HTTP_TMPCB_T pfnCbTemplate;            /**< template handler */
} HTTP_RESMGR_LL_RES_T;

/** return values of string sort function */
typedef enum {
    GOAL_HTTP_RESMGR_STR_EQ = 0,                /**< both strings equal */
    GOAL_HTTP_RESMGR_STR_1,                     /**< string one is higher */
    GOAL_HTTP_RESMGR_STR_2,                     /**< string two is higher */
} GOAL_HTTP_RESMGR_STR_RET_T;


/****************************************************************************/
/* Prototypes */
/****************************************************************************/

GOAL_STATUS_T goal_httpResGetDataCb(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_HDL_T hdlRes,                     /**< resource handle */
    GOAL_HTTP_REQCB_T *pFcnRet                  /**< return for function pointer */
);

GOAL_STATUS_T goal_httpResGetTemplCb(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_HDL_T hdlRes,                     /**< resource handle */
    GOAL_HTTP_TMPCB_T *pFcnRet                  /**< return for function pointer */
);

GOAL_HTTP_HDL_T goal_httpResrcPathHdlGet(
    uint8_t  *strPath,                          /**< path string */
    uint8_t  strLen                             /**< string length */
);

GOAL_STATUS_T goal_httpResMgrParse(
    GOAL_HTTP_PARSE_T *pInfoParse               /**< pointer to parser information struct */
);

GOAL_STATUS_T goal_httpResMgrSearch(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    uint8_t *strUrl,                            /**< URL string */
    uint32_t lenUrl,                            /**< URL string length */
    HTTP_RESMGR_LL_RES_T **ppResRet,            /**< return pointer for resource */
    uint32_t *pLenMatched                       /**< pointer to matching length var */
);

GOAL_STATUS_T goal_httpGetResAcc(
    GOAL_HTTP_T         *pInst,                 /**< instance pointer */
    GOAL_HTTP_HDL_T     hdlRes,                 /**< resource handle */
    uint32_t            *pAccess                /**< pointer to access rights */
);

GOAL_STATUS_T goal_httpIdByHdl(
    uint16_t *pId,                              /**< id of handle */
    GOAL_HTTP_HDL_T hdl                         /**< http handle */
);

GOAL_STATUS_T goal_httpHdlById(
    GOAL_HTTP_HDL_T *pHdl,                      /**< http handle */
    uint16_t id                                 /**< id of handle */
);

#endif /* GOAL_HTTP_RESMGR_H */


