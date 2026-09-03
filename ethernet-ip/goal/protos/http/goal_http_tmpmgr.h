/** @file
 *
 * @brief Goal http template manager
 *
 * This is the goal webserver webpage template manager.
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


#ifndef GOAL_HTTP_TMPMGR_H
#define GOAL_HTTP_TMPMGR_H

#include <goal_includes.h>


/****************************************************************************/
/* Defines */
/****************************************************************************/

#define GOAL_HTTP_TMPMGR_TOKEN_OPEN    ((char) '[')
#define GOAL_HTTP_TMPMGR_TOKEN_CLOSE   ((char) ']')

#define GOAL_HTTP_TMPMGR_ERR_STRING    "#ERR#"

#define GOAL_HTTP_TMPMGR_MAX_CNT_LIST      10
#define GOAL_HTTP_TMPMGR_MAX_LEN_PATH      4


/****************************************************************************/
/* Data types */
/****************************************************************************/

/** list structure for template manager */
typedef struct GOAL_HTTP_TMPMGR_LL_T {
    struct GOAL_HTTP_TMPMGR_LL_T *pNext;        /**< pointer to next entry */
    uint32_t listId;                            /**< identifier of list */
    uint32_t cntMemb;                           /**< member count */
    char listName[32];                          /**< list name */
} GOAL_HTTP_TMPMGR_LL_T;


/****************************************************************************/
/* Prototypes */
/****************************************************************************/

GOAL_STATUS_T goal_httpTmpMgrParse(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    GOAL_BUFFER_T *pBuf,                        /**< goal buffer */
    GOAL_HTTP_HDL_T hdlRes,                     /**< resource handle */
    const uint8_t *pData,                       /**< data pointer */
    uint32_t lenData,                           /**< data length */
    uint32_t bufOffset                          /**< buffer offset */
);

#endif /* GOAL_HTTP_TMPMGR_H */


