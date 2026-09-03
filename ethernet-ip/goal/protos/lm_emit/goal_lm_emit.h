/** @file
 *
 * @brief emitter for goal lm logging messages
 *
 * @details
 * This module takes goal lm logging messages from the lm buffer. It converts
 * those messages to a readable form and outputs these to varous targets.
 *
 * @copyright
 * Copyright 2020 port GmbH Halle/Saale.
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
#ifndef GOAL_LM_EMIT_H
#define GOAL_LM_EMIT_H


/****************************************************************************/
/* Configuration */
/****************************************************************************/
#ifndef GOAL_CONFIG_LM_EMIT_MA_UART
# define GOAL_CONFIG_LM_EMIT_MA_UART 0
#endif

#ifndef GOAL_CONFIG_LOGGING_TARGET_RAW
# define GOAL_CONFIG_LOGGING_TARGET_RAW 0
#endif

#ifndef GOAL_CONFIG_LM_EMIT_LIMITER
# define GOAL_CONFIG_LM_EMIT_LIMITER 5
#endif


/****************************************************************************/
/* Forward declaration */
/****************************************************************************/
struct GOAL_LM_EMIT_EMITTER_T;                  /**< LM Emitter handle */


/****************************************************************************/
/* Type declaration */
/****************************************************************************/

typedef enum {
    LM_None,
    LM_INT8,
    LM_UINT8,
    LM_INT16,
    LM_UINT16,
    LM_INT32,
    LM_UINT32,
    LM_GENERIC,
    LM_MAC,
    LM_IPV4,
    LM_FLOAT,
    LM_STRING0,
    LM_PTR,
    LM_PTRDIFF,
    LM_INT64,
    LM_UINT64,
    LM_BOOLEAN
} GOAL_LM_DATATYPES_T;

typedef struct {
    uint32_t id;                                /**< log id */
    uint32_t modId;                             /**< module id */
    uint32_t textId;                            /**< text id */
    uint8_t severity;                           /**< severity */
    uint8_t p1Type;                             /**< param 1 type */
    int p1Pos;                                  /**< param 1 position */
    uint8_t p2Type;                             /**< param 2 type */
    int p2Pos;                                  /**< param 2 position */
    const char *text;                           /**< text message */
    int seg[3][2];                              /**< text segments */
} GOAL_LM_DB_ENTRY_T;

/**< open function prototype */
typedef GOAL_STATUS_T (*GOAL_LM_EMIT_EMITTER_OPEN) (
    struct GOAL_LM_EMIT_EMITTER_T *pHdlEmit     /**< LM Emitter handle */
);

/**< start function prototype */
typedef GOAL_STATUS_T (*GOAL_LM_EMIT_EMITTER_START) (
    struct GOAL_LM_EMIT_EMITTER_T *pHdlEmit     /**< LM Emitter handle */
);

/**< data function prototype */
typedef GOAL_STATUS_T (*GOAL_LM_EMIT_EMITTER_DATA) (
    struct GOAL_LM_EMIT_EMITTER_T *pHdlEmit,    /**< LM Emitter handle */
    const char *strData,                        /**< emitter data */
    int len                                     /**< emitter data length */
);

/**< end function prototype */
typedef GOAL_STATUS_T (*GOAL_LM_EMIT_EMITTER_END) (
    struct GOAL_LM_EMIT_EMITTER_T *pHdlEmit     /**< LM Emitter handle */
);

/**< idle check function prototype */
typedef GOAL_STATUS_T (*GOAL_LM_EMIT_EMITTER_IDLE) (
    struct GOAL_LM_EMIT_EMITTER_T *pHdlEmit     /**< LM Emitter handle */
);

typedef struct GOAL_LM_EMIT_EMITTER_T {
    struct GOAL_LM_EMIT_EMITTER_T *pNext;       /**< next entry */
    uint32_t id;                                /**< emitter id */
    GOAL_BOOL_T flgEnabled;                     /**< enable flag */
    GOAL_LM_EMIT_EMITTER_OPEN fnctOpen;         /**< open function */
    GOAL_LM_EMIT_EMITTER_START fnctStart;       /**< start emitting function */
    GOAL_LM_EMIT_EMITTER_DATA fnctData;         /**< data emitting function */
    GOAL_LM_EMIT_EMITTER_END fnctEnd;           /**< end emitting function */
    GOAL_LM_EMIT_EMITTER_IDLE fnctIdle;         /**< idle check emitting function */
    void *pData;                                /**< private data */
    GOAL_LM_LOG_ENTRY_T *pLogRaw;               /**< raw log message */
    uint32_t lenLogRaw;                         /**< raw message length */
    const GOAL_LM_DB_ENTRY_T *pDbEntry;         /**< log message description */
} GOAL_LM_EMIT_EMITTER_T;


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_lmEmitInitPre(
    void
);

GOAL_STATUS_T goal_lmEmitEmitterReg(
    GOAL_LM_EMIT_EMITTER_T **ppHdlEmitter,      /**< new emitter handle */
    GOAL_LM_EMIT_EMITTER_OPEN fnctOpen,         /**< open function */
    GOAL_LM_EMIT_EMITTER_START fnctStart,       /**< start emitting function */
    GOAL_LM_EMIT_EMITTER_DATA fnctData,         /**< data emitting function */
    GOAL_LM_EMIT_EMITTER_END fnctEnd,           /**< end emitting function */
    GOAL_LM_EMIT_EMITTER_IDLE fnctIdle,         /**< idle check emitting function */
    void *pData
);

void goal_lmEmitLogDump(
    void
);

void goal_lmEmitEmergencySet(
    GOAL_LM_EMIT_EMITTER_T *pEmit               /**< emergency emitter */
);

void goal_lmEmitEmergencyEnable(
    void
);

void goal_lmEmitEarly(
    void
);

#endif /* GOAL_LM_EMIT_H */
