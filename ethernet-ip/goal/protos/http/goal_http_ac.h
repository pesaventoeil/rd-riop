/** @file
 *
 * @brief Goal HTTP RPC wrapper for AC
 *
 * This is the goal HTTP RPC wrapper for the application controller.
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


#ifndef GOAL_HTTP_AC_H
#define GOAL_HTTP_AC_H


/****************************************************************************/
/* Defines */
/****************************************************************************/


/****************************************************************************/
/* Data types */
/****************************************************************************/

/** list of data callback functions for AC */
typedef struct GOAL_HTTP_CB_DATA_LL_T {
    struct GOAL_HTTP_CB_DATA_LL_T *pNext;       /* pointer to next element */
    GOAL_HTTP_REQCB_T pfnCbData;                /* pointer to callback function */
    uint32_t id;                                /* identifier */
} GOAL_HTTP_CB_DATA_LL_T;

/** list of template callback functions for AC */
typedef struct GOAL_HTTP_CB_TEMPL_LL_T {
    struct GOAL_HTTP_CB_TEMPL_LL_T *pNext;      /* pointer to next element */
    GOAL_HTTP_TMPCB_T pfnCbTempl;               /* pointer to callback function */
    uint32_t id;                                /* identifier */
} GOAL_HTTP_CB_TEMPL_LL_T;

/** list of resources for AC */
typedef struct GOAL_HTTP_AC_RES_LL_T {
    struct GOAL_HTTP_AC_RES_LL_T *pNext;        /* pointer to next element */
    GOAL_HTTP_HDL_T hdlRes;                     /* resource handle */
    GOAL_HTTP_REQCB_T pfnCbData;                /* data callback pointer */
    GOAL_HTTP_TMPCB_T pfnCbTempl;               /* template callback pointer */
} GOAL_HTTP_AC_RES_LL_T;

/** list of channels for AC */
typedef struct GOAL_HTTP_CHN_AC_LL_T {
    struct GOAL_HTTP_CHN_AC_LL_T *pNext;        /* pointer to next element */
    GOAL_HTTP_HDL_T hdlTransm;                  /* transmission handle */
    GOAL_HTTP_CTC_FRAG_T ctcFrag;               /* CTC fragmentation */
    const uint8_t *pData;                       /* data pointer */
    uint32_t offset;                            /* offset for data pointer */
    uint32_t lenData;                           /* total data length */
} GOAL_HTTP_CHN_AC_LL_T;

/** HTTP instance type for CTC usage */
typedef struct GOAL_HTTP_INSTANCE_CTC_T {
    GOAL_INSTANCE_HEADER(GOAL_HTTP_INSTANCE_CTC_T); /**< instance header */
    struct GOAL_RPC_HDL_CHN_T *pHdlRpcChn;      /**< rpc channel handle */
    GOAL_HTTP_CB_DATA_LL_T *pCbDataList;        /**< pointer to list of data cb functions */
    GOAL_HTTP_CB_TEMPL_LL_T *pCbTemplList;      /**< pointer to list of template cb functions */
    GOAL_HTTP_AC_RES_LL_T *pResList;            /**< pointer to resources list */
    GOAL_HTTP_CHN_AC_LL_T *pChn;                /**< pointer to channel list */
    uint8_t               *pCtcTemp;            /**< temporary CTC buffer */
    char strUrl[GOAL_HTTP_URL_MAX_LENGTH];      /**< url handling string buffer */
    char strRet[GOAL_HTTP_URL_MAX_LENGTH];      /**< url handling string buffer */
} GOAL_HTTP_INSTANCE_CTC_T;


/****************************************************************************/
/* Prototypes */
/****************************************************************************/

GOAL_STATUS_T goal_httpInitAc(
    void
);

GOAL_STATUS_T goal_httpNewAc(
    GOAL_HTTP_T **ppInst,                       /**< pointer to instance pointer */
    uint16_t port,                              /**< port to link instance to */
    uint16_t cntChn                             /**< channel count */
);

#if GOAL_CONFIG_HTTPS == 1
GOAL_STATUS_T goal_httpsNewAc(
    GOAL_HTTP_T **ppInst,                       /**< pointer to instance pointer */
    uint16_t port,                              /**< port to link instance to */
    uint16_t cntChn                             /**< channel count */
);
#endif /* GOAL_CONFIG_HTTPS == 1 */

GOAL_STATUS_T goal_httpResRegAc(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    uint8_t *strUrl,                            /**< URL string */
    uint32_t allowMeth,                         /**< allowed methods for resource */
    GOAL_HTTP_REQCB_T pfnCbData,                /**< application callback */
    GOAL_HTTP_TMPCB_T pfnCbTemplate,            /**< template handler callback */
    GOAL_HTTP_HDL_T *pHdl                       /**< resource handle return */
);

GOAL_STATUS_T goal_httpTmpMgrNewListAc(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_TEMPLATE_LIST_INIT_T *pInit       /**< pointer to list init data */
);

GOAL_STATUS_T goal_httpAuthBasSetUserInfoAc(
    GOAL_HTTP_T         *pInst,                 /**< instance pointer */
    uint32_t            userLevel,              /**< user level */
    char                *strUser,               /**< user name string */
    char                *strPw                  /**< password string */
);

GOAL_STATUS_T goal_httpCbDataAcPost(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< RPC handle */
);

GOAL_STATUS_T goal_httpCbDataAcGet(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< RPC handle */
);

GOAL_STATUS_T goal_httpCbTransDoneAc(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< RPC handle */
);

GOAL_STATUS_T goal_httpDataCloseChnAc(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< RPC handle */
);

GOAL_STATUS_T goal_httpDataContAc(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< RPC handle */
);

GOAL_STATUS_T goal_httpCbTemplSmplAc(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< RPC handle */
);

GOAL_STATUS_T goal_httpCbTemplListAc(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< RPC handle */
);

#endif /* GOAL_HTTP_AC_H */

