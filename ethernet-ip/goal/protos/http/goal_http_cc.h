/** @file
 *
 * @brief Goal HTTP RPC wrapper for the CC
 *
 * This is the Goal HTTP RPC wrapper for the CC.
 *
 * @copyright
 * Copyright 2010-2018 port GmbH Halle/Saale.
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


#ifndef GOAL_HTTP_CC_H
#define GOAL_HTTP_CC_H


/****************************************************************************/
/* Defines */
/****************************************************************************/


/****************************************************************************/
/* Data types */
/****************************************************************************/

typedef struct GOAL_HTTP_INSTANCE_CTC_T {
    GOAL_INSTANCE_HEADER(GOAL_HTTP_INSTANCE_CTC_T); /**< instance header */
    struct GOAL_RPC_HDL_CHN_T   *pHdlRpcChn;    /**< rpc channel handle */
    GOAL_HTTP_T                 *pHttpInst;     /**< pointer to http instance */
    uint32_t                    tempBufSize;    /**< temporary buffer size */
    char                        *pTempBuf;      /**< temporary data buffer */
} GOAL_HTTP_INSTANCE_CTC_T;


/****************************************************************************/
/* Prototypes */
/****************************************************************************/

GOAL_STATUS_T goal_httpInitCc(
    void
);

GOAL_STATUS_T goal_httpNewCc(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< RPC handle */
);

GOAL_STATUS_T goal_httpsNewCc(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< RPC handle */
);

GOAL_STATUS_T goal_httpResRegCc(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< RPC handle */
);

GOAL_STATUS_T goal_httpTmpMgrNewListCc(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< RPC handle */
);

GOAL_STATUS_T goal_httpAuthBasSetUserInfoCc(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< RPC handle */
);

GOAL_STATUS_T goal_httpDataContinueCc(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    const uint8_t **ppData,                     /**< address of data pointer */
    uint32_t *pLenData                          /**< length of data */
);

GOAL_STATUS_T goal_httpChnCloseCc(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan                  /**< channel pointer */
);

#endif /* GOAL_HTTP_CC_H */

