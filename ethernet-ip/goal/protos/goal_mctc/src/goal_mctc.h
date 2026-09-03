/** @file
 *
 * @brief GOAL Micro Core To Core
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

#ifndef GOAL_MCTC_H
#define GOAL_MCTC_H

#include <goal_mctc_plat.h>


/****************************************************************************/
/* Defines */
/****************************************************************************/
/**< GOAL MCTC - resquest flag */
#define GOAL_MCTC_FLG_TYPE          (3 << 0)    /**< type bitmask */
#define GOAL_MCTC_FLG_RESPONSE      (0 << 0)    /**< response type */
#define GOAL_MCTC_FLG_REQUEST       (1 << 0)    /**< request type */
#define GOAL_MCTC_FLG_INFO          (2 << 0)    /**< info type */


/**< default retry count to wait for a response */
#define GOAL_MCTC_CNT_RETRY         3

/**< default timeout per retry to wait for a response */
#define GOAL_MCTC_TIMEOUT_RX        (3 * GOAL_TIMER_SEC)

/**< default RPC value push size */
#define GOAL_MCTC_RPC_VAL_SIZE      sizeof(uint32_t)


/****************************************************************************/
/* Forward declarations */
/****************************************************************************/
struct GOAL_MCTC_INST_T;
struct GOAL_MCTC_T;
struct GOAL_MCTC_TGT_T;


/****************************************************************************/
/* Datatypes */
/****************************************************************************/
/**< GOAL MCTC - RPC function */
typedef GOAL_STATUS_T (* GOAL_MCTC_RPC_FUNC_T)(
    struct GOAL_MCTC_T *pCtc                    /**< MCTC handle */
);


/**< GOAL MCTC - target receive function */
typedef void (* GOAL_MCTC_TGT_RECV_T)(
    struct GOAL_MCTC_INST_T *pInst,             /**< MCTC instance */
    struct GOAL_MCTC_TGT_T *pTgt                /**< target handle */
);


/**< GOAL MCTC - target send function */
typedef GOAL_STATUS_T (* GOAL_MCTC_TGT_SEND_T)(
    struct GOAL_MCTC_T *pCtc,                   /**< MCTC handle */
    struct GOAL_MCTC_TGT_T *pTgt                /**< target handle */
);


/****************************************************************************/
/* Structures */
/****************************************************************************/
/**< frame header */
GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct GOAL_MCTC_HDR_T {
    uint16_t seq;                               /**< start sequence */
    uint32_t lenData;                           /**< data length */
} GOAL_TARGET_PACKED GOAL_MCTC_HDR_T;
GOAL_TARGET_PACKED_STRUCT_POST


/**< MCTC - RPC common header */
GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct GOAL_MCTC_HDR_RPC_T {
    uint8_t idCtc;                              /**< CTC message id */
    uint8_t flags;                              /**< flags */
} GOAL_TARGET_PACKED GOAL_MCTC_HDR_RPC_T;
GOAL_TARGET_PACKED_STRUCT_POST


/**< MCTC - RPC request header */
GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct GOAL_MCTC_HDR_RPC_REQ_T {
    uint32_t idFunc_le32;                       /**< function id */
    uint32_t idRpc_le32;                        /**< RPC id */
    GOAL_MCTC_HDR_RPC_T common;                 /**< RPC common data */
} GOAL_TARGET_PACKED GOAL_MCTC_HDR_RPC_REQ_T;
GOAL_TARGET_PACKED_STRUCT_POST


/**< MCTC - instance */
typedef struct GOAL_MCTC_INST_T {
    struct GOAL_MCTC_T *pHdls;                  /**< MCTC handles */
    struct GOAL_MCTC_TGT_T *pTgtRx;             /**< RX target handle */
    struct GOAL_MCTC_TGT_T *pTgtTx;             /**< TX target handle */
    void *pPriv;                                /**< private data */
    unsigned int cntHdl;                        /**< handle count */
    unsigned int sizeData;                      /**< data size per handle */

    uint32_t cntRpc;                            /**< stat: rpc count */
    uint32_t cntRpcTimeout;                     /**< stat: rpc timeouts */
    uint32_t tsRpcMin;                          /**< stat: min. RPC time */
    uint32_t tsRpcMean;                         /**< stat: median RPC time */
    uint32_t tsRpcMax;                          /**< stat: max. RPC time */
    uint32_t cntFrameValid;                     /**< stat: frame CRC valid count */
    uint32_t cntFrameInvalid;                   /**< stat: frame CRC invalid count */

    uint32_t cfgToutValue;                      /**< cfg: timeout value */
} GOAL_MCTC_INST_T;


/**< MCTC - handle */
typedef struct GOAL_MCTC_T {
    GOAL_BOOL_T flgUsed;                        /**< usage flag */
    GOAL_BOOL_T flgRes;                         /**< result-avail flag */
    GOAL_BOOL_T flgRemote;                      /**< remote handle */
    uint8_t flags;                              /**< RPC flags */
    uint8_t idCtc;                              /**< CTC message id */
    uint8_t *pData;                             /**< CTC data */
    unsigned int idxData;                       /**< CTC data index */
    unsigned int sizeData;                      /**< CTC maximum data size */
    uint32_t idRpc;                             /**< RPC id */
    uint32_t idFunc;                            /**< function id */
    struct GOAL_MCTC_INST_T *pInst;             /**< instance */
    GOAL_LOCK_T *pLock;                         /**< lock */
    GOAL_TIMESTAMP_T tsTout;                    /**< timeout in ms */
} GOAL_MCTC_T;


/**< MCTC - RPC handle */
typedef struct GOAL_MCTC_RPC_T {
    struct GOAL_MCTC_RPC_T *pNext;              /**< next element */

    uint32_t idRpc;                             /**< GOAL id */
    uint32_t idFunc;                            /**< function id */
    GOAL_MCTC_RPC_FUNC_T func;                  /**< RPC function */
} GOAL_MCTC_RPC_T;


/**< MCTC - target handle */
typedef struct GOAL_MCTC_TGT_T {
    GOAL_MCTC_TGT_RECV_T funcRecv;              /**< receive function */
    GOAL_MCTC_TGT_SEND_T funcSend;              /**< send function */
    void *pPriv;                                /**< private data */
} GOAL_MCTC_TGT_T;


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_mctcInstNew(
    GOAL_MCTC_INST_T **ppInst,                  /**< [out] MCTC instance */
    GOAL_MCTC_TGT_T *pTgtRx,                    /**< RX target handle */
    GOAL_MCTC_TGT_T *pTgtTx,                    /**< TX target handle */
    unsigned int cntHdl,                        /**< handle count */
    unsigned int sizeData                       /**< data size per handle */
);

GOAL_STATUS_T goal_mctcRelease(
    GOAL_MCTC_T *pCtc                           /**< CTC handle */
);

GOAL_STATUS_T goal_mctcRpcReg(
    uint32_t idRpc,                             /**< RPC id */
    uint32_t idFunc,                            /**< function id */
    GOAL_MCTC_RPC_FUNC_T func                   /**< RPC function */
);

GOAL_STATUS_T goal_mctcNew(
    GOAL_MCTC_T **ppHdl,                        /**< [out] MCTC handle */
    GOAL_MCTC_INST_T *pInst                     /**< instance */
);

GOAL_STATUS_T goal_mctcPush(
    GOAL_MCTC_T *pCtc,                          /**< CTC handle */
    const uint8_t *pData,                       /**< data pointer */
    unsigned int lenData                        /**< data length */
);

GOAL_STATUS_T goal_mctcPop(
    GOAL_MCTC_T *pCtc,                          /**< CTC handle */
    uint8_t *pData,                             /**< data pointer */
    unsigned int lenData                        /**< data length */
);

GOAL_STATUS_T goal_mctcLenGet(
    GOAL_MCTC_T *pCtc,                          /**< CTC handle */
    unsigned int *pLen                          /**< [out] stack length */
);

GOAL_STATUS_T goal_mctcLenFreeGet(
    GOAL_MCTC_T *pCtc,                          /**< CTC handle */
    unsigned int *pLenFree                      /**< [out] free data size */
);

GOAL_STATUS_T goal_mctcSendInfo(
    GOAL_MCTC_T *pCtc,                          /**< CTC handle */
    uint32_t idRpc,                             /**< RPC id */
    uint32_t idFunc                             /**< function id */
);

GOAL_STATUS_T goal_mctcSendReq(
    GOAL_MCTC_T *pCtc,                          /**< CTC handle */
    uint32_t idRpc,                             /**< RPC id */
    uint32_t idFunc                             /**< function id */
);

void goal_mctcMsgParse(
    GOAL_MCTC_INST_T *pInst,                    /**< instance */
    uint8_t *pData,                             /**< data pointer */
    unsigned int lenData                        /**< data length */
);

GOAL_STATUS_T goal_mctcToutSet(
    GOAL_MCTC_INST_T *pInst,                    /**< instance */
    uint32_t tOut                               /**< timeout value */
);

GOAL_STATUS_T goal_mctcCfgTout(
    uint32_t tOut                               /**< timeout value */
);

#endif /* GOAL_MCTC_H */
