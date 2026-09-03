/** @file
 *
 * @brief
 * GOAL HTTP Wrapper Common RPC Header
 *
 * @details
 * Common RPC header for shared functionality between RPC AC and RPC CC
 * implementation.
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
#ifndef GOAL_HTTP_RPC_H
#define GOAL_HTTP_RPC_H

#if (GOAL_CONFIG_GEN_CTC == 1)

#include <goal_media/goal_mi_mctc.h>


/****************************************************************************/
/* Defines */
/****************************************************************************/
#define GOAL_HTTP_RPC_STACK_SIZE 1024           /**< RPC stack size */
#define GOAL_HTTP_RPC_STACK_SIZE_USABLE 800
#define GOAL_HTTP_RPC_SNMP_ID_DEFAULT 1         /**< SNMP default ID */

#define GOAL_HTTP_RPC_NEW(pInst) \
    if (GOAL_RES_OK(res)) { \
        res = goal_rpcNew(&pHdlRpc, pInst->pHdlRpcChn); \
        if (GOAL_RES_ERR(res)) { \
            goal_logErr("RPC new failed: %"FMT_x32, res); \
        } \
    }

#define GOAL_HTTP_RPC_CALL(pInst, func) \
    GOAL_RPC_PUSH(pInst->idInst); \
    if (GOAL_RES_OK(res)) { \
        res = goal_rpcCall(pHdlRpc, GOAL_ID_HTTP, func); \
        if (GOAL_RES_ERR(res)) { \
            goal_logErr("RPC call failed: %"FMT_x32, res); \
        } \
    }

#define GOAL_HTTP_RPC_CLOSE() \
    goal_rpcClose(pHdlRpc)

#define GOAL_HTTP_RPC_PUSH_INST(pInst) \
    if (GOAL_RES_OK(res)) { \
        GOAL_RPC_PUSH(pInst->idInst); \
    }

#define GOAL_HTTP_RPC_POP_INST(_tmp) \
    if (GOAL_RES_OK(res)) { \
        GOAL_INSTANCE_T *_pInst; \
        uint32_t idInst = 0; \
        GOAL_RPC_POP(idInst, uint32_t); \
        if (GOAL_RES_OK(res)) { \
            GOAL_INSTANCE_FOREACH(pListCtcInst->pInst, _pInst) { \
                if (idInst == _pInst->idInst) { \
                    _tmp = (GOAL_HTTP_INSTANCE_CTC_T *) _pInst; \
                    break; \
                } \
            } \
            if (NULL == _tmp) { \
                res = GOAL_ERR_NOT_FOUND; \
            } \
        } \
    }


/****************************************************************************/
/* Data types */
/****************************************************************************/

/** HTTP RPC request types */
typedef enum {
    GOAL_HTTP_RPC_REQTYPE_GET_START = 1,
    GOAL_HTTP_RPC_REQTYPE_GET_FRAG
} GOAL_HTTP_RPC_REQTYPE_T;


/****************************************************************************/
/* Data type mapping */
/****************************************************************************/

#if (GOAL_CONFIG_GEN_CTC_AC == 1)
struct GOAL_HTTP_INSTANCE_CTC_T;
#  define GOAL_HTTP_T       GOAL_HTTP_INSTANCE_CTC_T
#endif


/****************************************************************************/
/* HTTP interface mapping */
/****************************************************************************/

#if GOAL_CONFIG_GEN_CTC_AC == 1
#  include <goal_http_ac.h>
#  define goal_httpInit             goal_httpInitAc
#  define goal_httpNew              goal_httpNewAc
#  if GOAL_CONFIG_HTTPS == 1
#    define goal_httpsNew           goal_httpsNewAc
#  endif
#  define goal_httpResReg           goal_httpResRegAc
#  define goal_httpTmpMgrNewList    goal_httpTmpMgrNewListAc
#  define goal_httpAuthBasSetUserInfo goal_httpAuthBasSetUserInfoAc
#endif /* GOAL_CONFIG_GEN_CTC_AC */

#if GOAL_CONFIG_GEN_CTC_CC == 1
#  include <goal_http_cc.h>
#  define goal_httpInit             goal_httpInitCc
#  define goal_httpNew              goal_httpNewCc
#  define goal_httpsNew             goal_httpsNewCc
#  define goal_httpResReg           goal_httpResRegCc
#  define goal_httpTmpMgrNewList    goal_httpTmpMgrNewListCc
#  define goal_httpAuthBasSetUserInfo goal_httpAuthBasSetUserInfoCc
#endif /* GOAL_CONFIG_GEN_CTC_CC */

#endif /* (GOAL_CONFIG_GEN_CTC == 1) */


/****************************************************************************/
/* Standalone mappings */
/****************************************************************************/

GOAL_STATUS_T goal_httpInitImpl(
    void
);

GOAL_STATUS_T goal_httpNewImpl(
    GOAL_HTTP_T **ppInst,                       /**< pointer to instance pointer */
    uint16_t port,                              /**< port to link instance to */
    uint16_t cntChn                             /**< channel count */
);

GOAL_STATUS_T goal_httpsNewImpl(
    GOAL_HTTP_T **ppInst,                       /**< HTTPS instance */
    uint16_t port,                              /**< port to link instance to */
    uint16_t cntChn                             /**< channel count */
);

GOAL_STATUS_T goal_httpResRegImpl(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    uint8_t *strUrl,                            /**< URL string */
    uint32_t allowMeth,                         /**< allowed methods for resource */
    GOAL_HTTP_REQCB_T pfnCbData,                /**< application callback */
    GOAL_HTTP_TMPCB_T pfnCbTemplate,            /**< template handler callback */
    GOAL_HTTP_HDL_T *pHdl                       /**< resource handle return */
);

GOAL_STATUS_T goal_httpTmpMgrNewListImpl(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_TEMPLATE_LIST_INIT_T *pInit       /**< pointer to list init data */
);

GOAL_STATUS_T goal_httpAuthBasSetUserInfoImpl(
    GOAL_HTTP_T         *pInst,                 /**< instance pointer */
    uint32_t            userLevel,              /**< user level */
    char                *strUser,               /**< user name string */
    char                *strPw                  /**< password string */
);

#ifndef goal_httpInit
#  define goal_httpInit             goal_httpInitImpl
#endif

#ifndef goal_httpNew
#  define goal_httpNew              goal_httpNewImpl
#endif

#ifndef goal_httpsNew
#  define goal_httpsNew             goal_httpsNewImpl
#endif

#ifndef goal_httpResReg
#  define goal_httpResReg           goal_httpResRegImpl
#endif

#ifndef goal_httpTmpMgrNewList
#  define goal_httpTmpMgrNewList    goal_httpTmpMgrNewListImpl
#endif

#ifndef goal_httpAuthBasSetUserInfo
#  define goal_httpAuthBasSetUserInfo goal_httpAuthBasSetUserInfoImpl
#endif


/****************************************************************************/
/* Function and Parameter Mapping */
/****************************************************************************/

#if (GOAL_CONFIG_GEN_CTC == 1)

#define GOAL_HTTP_FUNC_CALLBACK 0               /**< callback function id */

/* callback function list */
#define GOAL_HTTP_FUNC_CB_LIST \
    /*                      id, name,                               function */             \
    GOAL_HTTP_FUNC_CB_ENTRY( 0, GOAL_HTTP_CB_DATA_GET,              goal_httpCbDataAcGet)   \
    GOAL_HTTP_FUNC_CB_ENTRY( 1, GOAL_HTTP_CB_DATA_POST,             goal_httpCbDataAcPost)  \
    GOAL_HTTP_FUNC_CB_ENTRY( 2, GOAL_HTTP_CB_TRANS_DONE,            goal_httpCbTransDoneAc) \
    GOAL_HTTP_FUNC_CB_ENTRY( 3, GOAL_HTTP_CB_CONT,                  goal_httpDataContAc)    \
    GOAL_HTTP_FUNC_CB_ENTRY( 4, GOAL_HTTP_CB_CLOSECHN,              goal_httpDataCloseChnAc)\
    GOAL_HTTP_FUNC_CB_ENTRY( 5, GOAL_HTTP_CB_TEMPL_SMPL,            goal_httpCbTemplSmplAc) \
    GOAL_HTTP_FUNC_CB_ENTRY( 6, GOAL_HTTP_CB_TEMPL_LIST,            goal_httpCbTemplListAc)

/* create callback function id enum */
#undef GOAL_HTTP_FUNC_CB_ENTRY
#define GOAL_HTTP_FUNC_CB_ENTRY(id, name, ...) name = id,
typedef enum {
    GOAL_HTTP_FUNC_CB_LIST
} GOAL_HTTP_FUNC_CB_ID_T;

/* function list */
#define GOAL_HTTP_FUNC_LIST \
    /*                   id, name,                                  function */             \
    GOAL_HTTP_FUNC_ENTRY( 7, GOAL_HTTP_FUNC_NEW,                    goal_httpNew)           \
    GOAL_HTTP_FUNC_ENTRY( 8, GOAL_HTTP_FUNC_RES_REG,                goal_httpResReg)        \
    GOAL_HTTP_FUNC_ENTRY( 9, GOAL_HTTP_FUNC_TEMPL_NEW_LIST,         goal_httpTmpMgrNewList) \
    GOAL_HTTP_FUNC_ENTRY( 10, GOAL_HTTP_FUNC_AUTHBAS_SET_INFO,      goal_httpAuthBasSetUserInfo) \
    GOAL_HTTP_FUNC_ENTRY( 11, GOAL_HTTP_FUNC_NEW_HTTPS,             goal_httpsNew)


/* create function id enum */
#undef GOAL_HTTP_FUNC_ENTRY
#define GOAL_HTTP_FUNC_ENTRY(id, name, ...) name = id,
typedef enum {
    GOAL_HTTP_FUNC_LIST
} GOAL_HTTP_FUNC_ID_T;

#endif /* (GOAL_CONFIG_GEN_CTC == 1) */

#endif /* GOAL_HTTP_RPC_H */
