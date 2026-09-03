/** @file
 *
 * @brief GOAL MCTC - Common Media Interface
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

#ifndef GOAL_MI_MCTC_H
#define GOAL_MI_MCTC_H

#include <protos/goal_mctc/src/goal_mctc.h>
#include <goal_media/goal_mi_dm.h>


/****************************************************************************/
/* Defines */
/****************************************************************************/
/**< Fletcher16 offset to avoid false positives for 0x00 and 0xff */
#define GOAL_MI_MCTC_FLETCHER16_OFFSET 0x0007

/**< default RPC transfer size */
#define GOAL_MI_MCTC_XFER_SIZE 32

/**< default RPC transmission handle count */
#define GOAL_MI_MCTC_HDL_CNT 4

/* direction ids */
#define GOAL_MI_MCTC_DIR_PEER_TO 0              /**< 'to peer' id */
#define GOAL_MI_MCTC_DIR_PEER_FROM 1            /**< 'from peer' id */

/* MCTC communication channel */
#define GOAL_MI_MCTC_CHAN_SPI 0                 /**< MCTC via SPI */
#define GOAL_MI_MCTC_CHAN_UART 1                /**< MCTC via UART */

/* RPC function ids */
#define GOAL_MCTC_RPC_FUNC_SETUP_DONE 1         /**< setup done request */
#define GOAL_MCTC_RPC_FUNC_SETUP_STATE_GET 2    /**< setup state get request */
#define GOAL_MCTC_RPC_FUNC_SETUP_CFG_TOUT 3     /**< setup cfg timout */
#define GOAL_MCTC_RPC_FUNC_CFG_ETH_MAC_ADDRESS_SET 4 /**< set ETH MAC address */

/* data provider status */
#define GOAL_MCTC_DP_STATUS_FLG_CONN    (1 << 0) /**< connection signal */
#define GOAL_MCTC_DP_STATUS_FLG_ERR     (1 << 1) /**< error signal */
#define GOAL_MCTC_DP_STATUS_FLG_VALID   (1 << 2) /**< data valid */
#define GOAL_MCTC_DP_STATUS_FLG_SYNC_1  (1 << 3) /**< sync signal 1 */
#define GOAL_MCTC_DP_STATUS_FLG_SYNC_2  (1 << 4) /**< sync signal 2 */

/* data provider LED state */
#define GOAL_MCTC_DP_LED_WINK           (1 << 0) /**< wink LED */
#define GOAL_MCTC_DP_LED_RED_1          (1 << 1) /**< red LED 1 */
#define GOAL_MCTC_DP_LED_RED_2          (1 << 2) /**< red LED 2 */
#define GOAL_MCTC_DP_LED_GREEN_1        (1 << 3) /**< green LED 1 */
#define GOAL_MCTC_DP_LED_GREEN_2        (1 << 4) /**< green LED 2 */

/* RPC configuration */
#define GOAL_MI_MCTC_RPC_MAX_SEQ 10             /**< maximum sequence */
#define GOAL_MI_MCTC_RPC_CNT_RESEND 1           /**< resend buffer count */
#define GOAL_MI_MCTC_RPC_RECV_TOUT (10 * GOAL_TIMER_MSEC) /**< receive timeout */
#define GOAL_MI_MCTC_RPC_FLG_SYNC_REQ (1 << 0)  /**< sync request flag */
#define GOAL_MI_MCTC_RPC_FLG_SYNC_ACK (1 << 1)  /**< sync acknowledge flag */
#define GOAL_MI_MCTC_RPC_FLG_RESEND (1 << 2)    /**< resend flag */
#define GOAL_MI_MCTC_RPC_FLG_REQ_ACK (1 << 3)   /**< request acknowledge */

#define GOAL_MI_MCTC_RPC_STATE_SYNC_INIT 0      /**< initialize sync */
#define GOAL_MI_MCTC_RPC_STATE_SYNC_REQ 1       /**< request sync */
#define GOAL_MI_MCTC_RPC_STATE_SYNC_ACK_LOCAL 2 /**< local sync acknowledged */
#define GOAL_MI_MCTC_RPC_STATE_SYNC_ACK_REMOTE 3 /**< remote sync acknowledged */
#define GOAL_MI_MCTC_RPC_STATE_RUN 4            /**< run */
#define GOAL_MI_MCTC_RPC_STATE_RUN_ONCE 5       /**< run once */
#define GOAL_MI_MCTC_RPC_STATE_STOP 6           /**< stop */

/* define the default number of data length bytes */
#ifndef GOAL_CONFIG_MCTC_RPC_DATA_LENGTH
# define GOAL_CONFIG_MCTC_RPC_DATA_LENGTH 1
#endif

/* define the data length type */
#if GOAL_CONFIG_MCTC_RPC_DATA_LENGTH == 1
# define GOAL_MI_MCTC_RPC_DATA_LENGTH_T uint8_t
# define GOAL_MI_MCTC_RPC_DATA_LENGTH_htole(x) ((GOAL_MI_MCTC_RPC_DATA_LENGTH_T) (x))
# define GOAL_MI_MCTC_RPC_DATA_LENGTH_letoh
#elif GOAL_CONFIG_MCTC_RPC_DATA_LENGTH == 2
# define GOAL_MI_MCTC_RPC_DATA_LENGTH_T uint16_t
# define GOAL_MI_MCTC_RPC_DATA_LENGTH_htole(x) ((GOAL_MI_MCTC_RPC_DATA_LENGTH_T) GOAL_htole16(x))
# define GOAL_MI_MCTC_RPC_DATA_LENGTH_letoh GOAL_le16toh
#endif


/****************************************************************************/
/* Forward declarations */
/****************************************************************************/
struct GOAL_MI_MCTC_INST_T;


/****************************************************************************/
/* Datatypes */
/****************************************************************************/
typedef GOAL_MCTC_T GOAL_MI_MCTC_T;


/**< data provider meta container */
GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct {
    uint8_t status;                             /**< status */
    uint8_t leds;                               /**< LED state */
    uint16_t cntUpdate;                         /**< update counter */
} GOAL_TARGET_PACKED GOAL_MI_MCTC_DP_T;
GOAL_TARGET_PACKED_STRUCT_POST


/**< RPC RX data header */
GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct {
    uint16_t crc_le16;                          /**< checksum */
    uint8_t seqRemote;                          /**< local sequence number */
    uint8_t seqLocalAck;                        /**< remote sequence number */
    GOAL_MI_MCTC_RPC_DATA_LENGTH_T lenData_le;  /**< data length */
    uint8_t flags;                              /**< flags */
} GOAL_TARGET_PACKED GOAL_MI_MCTC_RPC_RX_T;
GOAL_TARGET_PACKED_STRUCT_POST


/**< RPC TX data header */
GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct {
    uint16_t crc_le16;                          /**< checksum */
    uint8_t seqLocal;                           /**< local sequence number */
    uint8_t seqRemoteAck;                       /**< remote sequence number */
    GOAL_MI_MCTC_RPC_DATA_LENGTH_T lenData_le;  /**< data length */
    uint8_t flags;                              /**< flags */
} GOAL_TARGET_PACKED GOAL_MI_MCTC_RPC_TX_T;
GOAL_TARGET_PACKED_STRUCT_POST


/****************************************************************************/
/* CTC compatibility layer */
/****************************************************************************/
/**< empty RPC handle */
#define GOAL_RPC_HDL_NONE 0

/**< RPC instance handle */
#define GOAL_RPC_HDL_CHN_T GOAL_MI_MCTC_INST_T

/**< RPC handle */
#define GOAL_RPC_HDL_T GOAL_MI_MCTC_T

/**< default RPC instance ID */
#define GOAL_ID_MI_CTC_DEFAULT GOAL_ID_DEFAULT

/**< open RPC instance */
#define goal_rpcSetupChannel(_pHdl, _id) goal_miMctcOpen(_pHdl, _id)

/**< configure RPC stack size */
#define goal_rpcSetStackMax goal_miMctcCfgXferSize

/**< configure RPC handle count */
#define goal_rpcHdlMaxSet goal_miMctcCfgHdlCnt

/**< RPC function id type */
#define GOAL_RPC_FUNC_ID uint32_t

/**< RPC not initialized error */
#define GOAL_ERR_RPC_NOT_INITIALIZED (GOAL_RES_RPC | GOAL_ERROR | 7)

/**< RPC status */
#define goal_rpcStatus(_id) goal_miMctcStatusGet(_id)

/**< get default RPC channel */
#define goal_rpcHdlChnDefaultGet(x) (UNUSEDARG(x), GOAL_OK)

/**< get RPC handle */
#define goal_rpcNew goal_miMctcNew

/**< call RPC function */
#define goal_rpcCall goal_miMctcSendReq

/**< release RPC handle */
#define goal_rpcClose goal_miMctcRelease

/**< register RPC function */
#define goal_rpcRegisterService(idRpc, idFunc, func) goal_miMctcRpcReg(idRpc, idFunc, func)

/**< pop from RPC stack */
#define goal_rpcArgPop(_pHdl, _pData, _lenData) goal_miMctcPop(_pHdl, _pData, _lenData)

/**< push to RPC stack */
#define goal_rpcArgPush(_pHdl, _pData, _lenData) goal_miMctcPush(_pHdl, _pData, _lenData)

/**< create a new RPC handle */
#define GOAL_RPC_NEW() { \
    if (GOAL_RES_OK(res)) { \
        res = goal_miMctcNew(&pHdlRpc, pHdlRpcChn); \
        if (GOAL_RES_ERR(res)) { \
            goal_logDbg("failed to create MCTC handle"); \
        } \
    } \
}

/**< close a RPC handle */
#define GOAL_RPC_CLOSE() \
    if (NULL != pHdlRpc) { \
        goal_miMctcRelease(pHdlRpc); \
    }

/**< convert value to 32-bit little endian and push it */
#define GOAL_RPC_PUSH(goal_rpcVal) { \
            GOAL_CASSERT(sizeof(uint32_t) >= sizeof(goal_rpcVal)); \
            if (GOAL_RES_OK(res)) { \
                uint32_t val32_le = GOAL_htole32((uint32_t) goal_rpcVal); \
                res = goal_miMctcPush(pHdlRpc, (uint8_t *) &val32_le, sizeof(uint32_t)); \
                if (GOAL_RES_ERR(res)) { \
                    goal_logDbg("RPC push result: 0x%"FMT_x32, res); \
                } \
            } \
        }

/**< pop a 32-bit little endian value and convert it back */
#define GOAL_RPC_POP(goal_rpcVal, goal_rpcDdstType) { \
            GOAL_CASSERT(sizeof(uint32_t) >= sizeof(goal_rpcDdstType)); \
            if (GOAL_RES_OK(res)) { \
                uint32_t val32_le; \
                res = goal_miMctcPop(pHdlRpc, (uint8_t *) &val32_le, sizeof(uint32_t)); \
                if (GOAL_RES_OK(res)) { \
                    goal_rpcVal = (goal_rpcDdstType) GOAL_le32toh(val32_le); \
                } else { \
                    goal_logDbg("RPC pop result: 0x%"FMT_x32, res); \
                } \
            } \
        }

/**< push data from pointer to stack */
#define GOAL_RPC_PUSH_PTR(ptr, len) \
    if (GOAL_RES_OK(res)) { \
        res = goal_miMctcPush(pHdlRpc, (const uint8_t *) ptr, len); \
        if (GOAL_RES_ERR(res)) { \
            goal_logErr("RPC push ptr failed: %"FMT_x32, res); \
        } \
    }

/**< pop data from stack to pointer */
#define GOAL_RPC_POP_PTR(ptr, len) \
    if (GOAL_RES_OK(res)) { \
        res = goal_miMctcPop(pHdlRpc, (uint8_t *) ptr, len); \
        if (GOAL_RES_ERR(res)) { \
            goal_logErr("RPC pop ptr failed: %"FMT_x32, res); \
        } \
    } \

    /**< register a RPC function to APPL ID */
#define GOAL_RPC_USER_REGISTER_SERVICE(idFunc, func) { \
    if (GOAL_RES_OK(res)) { \
        res = goal_miMctcRpcReg(GOAL_ID_APPL, idFunc, func); \
    } \
}

/**< call RPC function on APPL ID */
#define GOAL_RPC_USER_CALL(func) { \
    if (GOAL_RES_OK(res)) { \
        res = goal_rpcCall(pHdlRpc, GOAL_ID_APPL, func); \
        if (GOAL_RES_ERR(res)) { \
            goal_logErr("RPC call failed: %"FMT_x32, res); \
        } \
    } \
}

/**< RPC function type */
typedef GOAL_STATUS_T (* GOAL_RPC_FUNC_T)(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< RPC handle */
);


/****************************************************************************/
/* Convenience macros */
/****************************************************************************/
/**< allocate a MCTC handle */
#define GOAL_MCTC_NEW(_pInst) \
    if (GOAL_RES_OK(res)) { \
        res = goal_miMctcNew(&pHdlMctcTx, _pInst); \
        if (GOAL_RES_ERR(res)) { \
            pHdlMctcTx = NULL; \
        } \
    }

/**< free a MCTC handle */
#define GOAL_MCTC_RELEASE() \
    if (NULL != pHdlMctcTx) { \
        if (GOAL_RES_OK(res)) { \
            res = goal_miMctcRelease(pHdlMctcTx); \
        } else { \
            goal_miMctcRelease(pHdlMctcTx); \
        } \
    }

/**< push to request handle */
#define GOAL_MCTC_PUSH_REQ(_val) GOAL_MCTC_PUSH(Tx, _val)

/**< push to response handle */
#define GOAL_MCTC_PUSH_CB(_val) GOAL_MCTC_PUSH(Rx, _val)

/**< convert value to little endian and push it to an allocated MCTC handle */
#define GOAL_MCTC_PUSH(_hdl, _val) \
    GOAL_CASSERT(sizeof(uint32_t) >= sizeof(_val)); \
    if (GOAL_RES_OK(res)) { \
        uint32_t _val_le32 = GOAL_htole32((uint32_t) _val); \
        res = goal_miMctcPush(pHdlMctc ## _hdl, (const uint8_t *) &_val_le32, sizeof(uint32_t)); \
        if (GOAL_RES_ERR(res)) { \
            goal_logErr("failed to push value"); \
        } \
    }

/**< pop from request handle */
#define GOAL_MCTC_POP_REQ(_val, _type) GOAL_MCTC_POP(Tx, _val, _type)

/**< pop from response handle */
#define GOAL_MCTC_POP_CB(_val, _type) GOAL_MCTC_POP(Rx, _val, _type)

/**< pop little endian value from an allocated MCTC handle and convert it to given type */
#define GOAL_MCTC_POP(_hdl, _val, _type) \
    GOAL_CASSERT(sizeof(uint32_t) >= sizeof(_type)); \
    if (GOAL_RES_OK(res)) { \
        uint32_t _val_le32; \
        res = goal_miMctcPop(pHdlMctc ## _hdl, (uint8_t *) &_val_le32, sizeof(uint32_t)); \
        if (GOAL_RES_OK(res)) { \
            _val = (_type) GOAL_le32toh(_val_le32); \
        } else { \
            goal_logErr("failed to pop value"); \
        } \
    }

/**< push pointer data to request handle */
#define GOAL_MCTC_PUSH_PTR_REQ(_pData, _lenData) GOAL_MCTC_POP_PTR(Tx, _pData, _lenData)

/**< push pointer data to response handle */
#define GOAL_MCTC_PUSH_PTR_CB(_pData, _lenData) GOAL_MCTC_POP_PTR(Rx, _pData, _lenData)

/**< push data to an allocated MCTC handle */
#define GOAL_MCTC_PUSH_PTR(_hdl, _pData, _lenData) \
    if (GOAL_RES_OK(res)) { \
        res = goal_miMctcPush(pHdlMctc ## _hdl, (const uint8_t *) _pData, _lenData); \
    }

/**< pop pointer data from request handle */
#define GOAL_MCTC_POP_PTR_REQ(_pData, _lenData) GOAL_MCTC_POP_PTR(Tx, _pData, _lenData)

/**< pop pointer data from response handle */
#define GOAL_MCTC_POP_PTR_CB(_pData, _lenData) GOAL_MCTC_POP_PTR(Rx, _pData, _lenData)

/**< pop data from an allocated MCTC handle */
#define GOAL_MCTC_POP_PTR(_pData, _lenData) \
    if (GOAL_RES_OK(res)) { \
        res = goal_miMctcPop(pHdlMctc, (uint8_t *) _pData, _lenData); \
    }

/**< send data without waiting for a response */
#define GOAL_MCTC_SEND_INFO(_idRpc, _idFunc) \
    if (GOAL_RES_OK(res)) { \
        res = goal_miMctcSendInfo(pHdlMctcTx, _idRpc, _idFunc); \
    }

/**< send data and wait for a response */
#define GOAL_MCTC_SEND_REQ(_idRpc, _idFunc) \
    if (GOAL_RES_OK(res)) { \
        res = goal_miMctcSendReq(pHdlMctcTx, _idRpc, _idFunc); \
    }


/****************************************************************************/
/* Callback Helpers */
/****************************************************************************/
#define GOAL_MI_MCTC_CB_ID_RESET 0              /**< device reset callback */
#define GOAL_MI_MCTC_CB_ID_TIMEOUT_RX 1         /**< receive timeout callback */
#define GOAL_MI_MCTC_CB_ID_ONLINE 2             /**< data channel online */

typedef GOAL_STATUS_T (* GOAL_MI_MCTC_CB_RESET_T)(
    struct GOAL_MI_MCTC_INST_T *pMiMctc         /**< MI MCTC instance */
);

typedef GOAL_STATUS_T (* GOAL_MI_MCTC_CB_TIMEOUT_RX_T)(
    struct GOAL_MI_MCTC_INST_T *pMiMctc         /**< MI MCTC instance */
);

typedef GOAL_STATUS_T (* GOAL_MI_MCTC_CB_ONLINE_T)(
    struct GOAL_MI_MCTC_INST_T *pMiMctc         /**< MI MCTC instance */
);

#define GOAL_MI_MCTC_CB_RESET(...) \
    GOAL_MAIN_CB_FOREACH(GOAL_ID_MI_MCTC, GOAL_MI_MCTC_CB_ID_RESET, GOAL_MI_MCTC_CB_RESET_T, __VA_ARGS__)

#define GOAL_MI_MCTC_CB_TIMEOUT_RX(...) \
    GOAL_MAIN_CB_FOREACH(GOAL_ID_MI_MCTC, GOAL_MI_MCTC_CB_ID_TIMEOUT_RX, GOAL_MI_MCTC_CB_TIMEOUT_RX_T, __VA_ARGS__)

#define GOAL_MI_MCTC_CB_ONLINE(...) \
    GOAL_MAIN_CB_FOREACH(GOAL_ID_MI_MCTC, GOAL_MI_MCTC_CB_ID_ONLINE, GOAL_MI_MCTC_CB_ONLINE_T, __VA_ARGS__)


/****************************************************************************/
/** GOAL MI MCTC - Register Reset Callback
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_TARGET_INLINE GOAL_STATUS_T goal_miMctcCbRegReset(
    GOAL_MI_MCTC_CB_RESET_T func                /**< callback function */
)
{
    return goal_mainCbReg(GOAL_ID_MI_MCTC, GOAL_MI_MCTC_CB_ID_RESET, (GOAL_FUNC_RET_NOARG_T) ((void *) func));
}


/****************************************************************************/
/** GOAL MI MCTC - Register RX Timeout Callback
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_TARGET_INLINE GOAL_STATUS_T goal_miMctcCbRegToutRx(
    GOAL_MI_MCTC_CB_TIMEOUT_RX_T func           /**< callback function */
)
{
    return goal_mainCbReg(GOAL_ID_MI_MCTC, GOAL_MI_MCTC_CB_ID_TIMEOUT_RX, (GOAL_FUNC_RET_NOARG_T) ((void *) func));
}


/****************************************************************************/
/** GOAL MI MCTC - Register Data Channel Online Callback
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_TARGET_INLINE GOAL_STATUS_T goal_miMctcCbRegOnline(
    GOAL_MI_MCTC_CB_ONLINE_T func               /**< callback function */
)
{
    return goal_mainCbReg(GOAL_ID_MI_MCTC, GOAL_MI_MCTC_CB_ID_ONLINE, (GOAL_FUNC_RET_NOARG_T) ((void *) func));
}


/****************************************************************************/
/* Datatypes */
/****************************************************************************/
/**< MI MCTC RPC function type */
typedef GOAL_STATUS_T (* GOAL_MI_MCTC_FUNC_RPC_T)(
    GOAL_MI_MCTC_T *pHdl                        /**< MCTC handle */
);


/**< MI MCTC open function type */
typedef GOAL_STATUS_T (* GOAL_MI_MCTC_FUNC_OPEN_T)(
    struct GOAL_MI_MCTC_INST_T *pInst,          /**< MCTC instance */
    unsigned int sizeXfer                       /**< transfer size */
);


/**< MI MCTC send function type */
typedef GOAL_STATUS_T (* GOAL_MI_MCTC_FUNC_SEND_T)(
    GOAL_MI_MCTC_T *pHdl                        /**< MCTC handle */
);

typedef GOAL_STATUS_T (*GOAL_MI_MCTC_FUNC_NOTIFY_T)(
    struct GOAL_MI_MCTC_INST_T *pInst,          /**< MCTC instance */
    uint8_t *pData,                             /**< notify data */
    uint16_t len                                /**< notify data length */
);

/**< MI MCTC instance type */
typedef struct GOAL_MI_MCTC_INST_T {
    GOAL_INSTANCE_HEADER(GOAL_MI_MCTC_INST_T);  /**< instance header */

    GOAL_BOOL_T flgInit;                        /**< initialization flag */

    GOAL_MI_MCTC_FUNC_OPEN_T funcOpen;          /**< function open */
    GOAL_MI_MCTC_FUNC_NOTIFY_T funcNotifyCb;    /**< function notify callback */
    void *pPriv;                                /**< private data for sub-MI */

    GOAL_LOCK_T *pLock;                         /**< instance lock */
    GOAL_BOOL_T flgStateSync;                   /**< state sync flag */
    GOAL_BOOL_T flgSetupLocal;                  /**< local setup done flag */
    GOAL_BOOL_T flgSetupRemote;                 /**< remote setup done flag */
    GOAL_BOOL_T flgSetupRemoteRx;               /**< remote setup done flag received */
    GOAL_BOOL_T flgInfoToutRx;                  /**< remote timeout info tracker */

    uint32_t sizeXferRemote;                    /**< maximum remote transfer size */

    /* RPC sequence counters and trackers */
    GOAL_MCTC_INST_T *pInstMctc;                /**< MCTC instance */
    uint8_t flgRpc;                             /**< RPC flags */
    uint8_t flgRpcRemote;                       /**< RPC remote flags */
    uint8_t seqRpcLocal;                        /**< local seq nr */
    uint8_t seqRpcLocalAck;                     /**< remotely acked local seq nr */
    uint8_t seqRpcRemoteAck;                    /**< acked remote seq nr */
    unsigned int cntResend;                     /**< resend index */
    unsigned int sizeRpc;                       /**< maximum RPC data size */
    uint8_t *pBufRpcTx[GOAL_MI_MCTC_RPC_CNT_RESEND]; /**< RPC buffer cache */
    unsigned int lenBufRpcTx[GOAL_MI_MCTC_RPC_CNT_RESEND]; /**< RPC buffer length */
    unsigned int stateRpc;                      /**< RPC state */
    GOAL_BOOL_T flgRpcPrevEmpty;                /**< previously frame was empty */
    GOAL_BOOL_T flgRpcAck;                      /**< RPC send acknowledge */
    GOAL_TIMESTAMP_T tsRpcTout;                 /**< RPC receive timeout */
    unsigned int cntRpcActive;                  /**< active RPC send items */
    GOAL_MI_DM_PART_T *pDmPartRead;             /**< MI DM read partition */
    GOAL_MI_DM_PART_T *pDmPartWrite;            /**< MI DM write partition */

    /* CTC compatibility variables */
    uint32_t channelId;                         /**< channel id */

    /* configuration variables */
    uint32_t cfgToutValue;                      /**< configured timeout value */

    GOAL_CM_VAR_T *pCmVarRpcCount;              /**< statistics */
    GOAL_CM_VAR_T *pCmVarRpcTimeouts;           /**< statistics */
    GOAL_CM_VAR_T *pCmVarRpcDelayMin;           /**< statistics */
    GOAL_CM_VAR_T *pCmVarRpcDelayMean;          /**< statistics */
    GOAL_CM_VAR_T *pCmVarRpcDelayMax;           /**< statistics */
    GOAL_CM_VAR_T *pCmVarFrameValidCount;       /**< statistics */
    GOAL_CM_VAR_T *pCmVarFrameInvalidCount;     /**< statistics */

} GOAL_MI_MCTC_INST_T;


/**< MI MCTC RPC function list type */
typedef struct GOAL_MI_MCTC_FUNC_RPC_LIST_T {
    struct GOAL_MI_MCTC_FUNC_RPC_LIST_T *pNext; /**< next entry */
    uint32_t idRpc;                             /**< RPC id */
    uint32_t idFunc;                            /**< function id */
    GOAL_MI_MCTC_FUNC_RPC_T func;               /**< RPC function */
} GOAL_MI_MCTC_FUNC_RPC_LIST_T;


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_mctcInitPre(
    void
);

GOAL_STATUS_T goal_miMctcReg(
    GOAL_MI_MCTC_INST_T **ppInst,               /**< [out] MCTC instance */
    unsigned int id,                            /**< instance id */
    const char *strDesc,                        /**< description */
    GOAL_MI_MCTC_FUNC_OPEN_T funcOpen,          /**< function open */
    GOAL_MI_MCTC_FUNC_NOTIFY_T funcNotifyCb     /**< function notify callback */
);

GOAL_STATUS_T goal_miMctcRpcReg(
    uint32_t idRpc,                             /**< RPC id */
    uint32_t idFunc,                            /**< function id */
    GOAL_MI_MCTC_FUNC_RPC_T funcRpc             /**< RPC function */
);

GOAL_STATUS_T goal_miMctcOpen(
    GOAL_MI_MCTC_INST_T **ppInst,               /**< MCTC instance */
    unsigned int id                             /**< instance id */
);

GOAL_STATUS_T goal_miMctcNew(
    GOAL_MI_MCTC_T **ppHdlMctc,                 /**< [out] MCTC handle */
    GOAL_MI_MCTC_INST_T *pInst                  /**< MCTC instance */
);

GOAL_STATUS_T goal_miMctcRelease(
    GOAL_MI_MCTC_T *pHdlMctc                    /**< MCTC handle */
);

GOAL_STATUS_T goal_miMctcPush(
    GOAL_MI_MCTC_T *pHdl,                       /**< MCTC handle */
    const uint8_t *pData,                       /**< data */
    unsigned int len                            /**< data length */
);

GOAL_STATUS_T goal_miMctcPop(
    GOAL_MI_MCTC_T *pHdl,                       /**< MCTC handle */
    uint8_t *pData,                             /**< [out] data */
    unsigned int len                            /**< data length */
);

GOAL_STATUS_T goal_miMctcSendInfo(
    GOAL_MI_MCTC_T *pHdl,                       /**< MCTC handle */
    uint32_t idRpc,                             /**< RPC id */
    uint32_t idFunc                             /**< function id */
);

GOAL_STATUS_T goal_miMctcSendReq(
    GOAL_MI_MCTC_T *pHdl,                       /**< MCTC handle */
    uint32_t idRpc,                             /**< RPC id */
    uint32_t idFunc                             /**< function id */
);

GOAL_STATUS_T goal_miMctcCfgXferSize(
    unsigned int sizeXfer                       /**< maximum transfer size */
);

GOAL_STATUS_T goal_miMctcXferSizeGet(
    unsigned int *pSizeXfer,                    /**< transfer size */
    GOAL_MI_MCTC_INST_T *pInst                  /**< MCTC instance */
);

GOAL_STATUS_T goal_miMctcCfgHdlCnt(
    unsigned int cntHdl                         /**< maximum handle count */
);

GOAL_STATUS_T goal_miMctcStatusGet(
    unsigned int id                             /**< instance id */
);

GOAL_STATUS_T goal_miMctcInstGetById(
    GOAL_MI_MCTC_INST_T **ppMiMctcInst,         /**< [out] MCTC MI instance */
    uint32_t id                                 /**< MCTC MI instance id */
);

GOAL_STATUS_T goal_miMctcRpcDmMap(
    GOAL_MI_MCTC_INST_T *pMiMctc,               /**< [in] MI MCTC instance */
    GOAL_MI_DM_PART_T *pPartRead,               /**< [in] read partition */
    GOAL_MI_DM_PART_T *pPartWrite               /**< [in] write partition */
);


GOAL_STATUS_T goal_miMctcCrc(
    uint8_t *pData,                             /**< data */
    unsigned int len,                           /**< data length */
    uint16_t valCrc                             /**< expected CRC */
);

GOAL_STATUS_T goal_miMctcRpcTxProcess(
    GOAL_MI_MCTC_INST_T *pMiMctc                /**< MI MCTC handle */
);

void goal_miMctcRpcProcessLoop(
    void *pParam                                /**< loop parameter */
);

void goal_miMctcMonitorRx(
    GOAL_MI_MCTC_INST_T *pMiMctc,               /**< MI MCTC handle */
    GOAL_BOOL_T flgConn                         /**< connection flag */
);

GOAL_STATUS_T goal_miMctcNotify(
    void *pArg,                                 /**< argument */
    uint8_t *pData,                             /**< notify data */
    uint16_t len                                /**< notify data length */
);

GOAL_STATUS_T goal_miMctcCfgTout(
    uint32_t toutRpc                            /**< initial rpc timeout */
);

#if (1 == GOAL_CONFIG_GEN_CTC_AC)
GOAL_STATUS_T goal_miMctcEthCfgMacAddr(
    uint32_t port,                              /**< port ID */
    uint32_t idMa,                              /**< associated MA */
    GOAL_ETH_MAC_ADDR_T *pMacAddress            /**< new device MAC address */
);
#endif
#endif /* GOAL_MI_MCTC_H */
