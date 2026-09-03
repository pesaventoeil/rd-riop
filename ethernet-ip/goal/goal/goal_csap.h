/** @file
 *
 * @brief GOAL Serialization and Execution of configuration
 *
 * @copyright
 * Copyright 2010-2017.
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
#ifndef GOAL_CSAP_H
#define GOAL_CSAP_H

#include "goal_includes.h"

#if GOAL_CONFIG_CSAP == 1


/****************************************************************************/
/* Defines */
/****************************************************************************/
#define GOAL_CSAP_CFG_START   1
#define GOAL_CSAP_CFG_END     2

#define GOAL_CSAP_START(hdl) { \
    goal_csapLock(); \
    GOAL_CSAP_CALL(GOAL_ID_CSAP, GOAL_CSAP_CFG_START) \
    GOAL_CSAP_PUSH(1); \
    GOAL_CSAP_PUSH(hdl->modId); \
    GOAL_CSAP_CLOSE(); \
}

#define GOAL_CSAP_END(hdl) { \
    GOAL_CSAP_CALL(GOAL_ID_CSAP, GOAL_CSAP_CFG_END) \
    GOAL_CSAP_PUSH(1); \
    GOAL_CSAP_PUSH(hdl->modId); \
    GOAL_CSAP_CLOSE(); \
    goal_csapUnlock(); \
}

#define GOAL_CSAP_CALL(modId, func) { \
    goal_csapPush((uint32_t) modId); \
    goal_csapPush((uint32_t) func); \
}

#define GOAL_CSAP_CLOSE()  { \
    goal_csapConfigSizeSet(); \
}

#define GOAL_CSAP_PUSH(value) { \
    goal_csapPush((uint32_t) value); \
}

#define GOAL_CSAP_PUSH_STR(value) { \
    goal_csapPushString(value); \
}

#define GOAL_CSAP_POP(_value, _type) { \
    _value = (_type) (PtrCast) goal_csapPop(); \
}

#define GOAL_CSAP_POP_STR(value, type) { \
    goal_csapPopString((char **) &value); \
}

#define GOAL_CSAP_POP_NOTYPE(value) { \
    value = goal_csapPop(); \
}

#define GOAL_CSAP_POP_PTR(_value, _type) { \
    _value = (_type) (PtrCast) goal_csapPop(); \
}

#define GOAL_CSAP_POP_PPTR(_value, _type) { \
    _value = (_type) (PtrCast) goal_csapPop(); \
}

#define GOAL_CSAP_SKIP(cntParams) { \
    goal_csapSkip(cntParams); \
}


/****************************************************************************/
/* Public data types */
/****************************************************************************/
typedef uint32_t GOAL_CSAP_TYPE;
typedef uint32_t GOAL_CSAP_FUNC_ID_TYPE;
typedef uint32_t GOAL_CSAP_PARAMS_TYPE;

typedef GOAL_STATUS_T (*GOAL_CSAP_FUNC_POP)(
    void
);

typedef GOAL_STATUS_T (*GOAL_CSAP_FUNC_PUSH)(
    void
);

typedef struct GOAL_CSAP_FUNCTION_LIST_T {
    GOAL_CSAP_FUNC_ID_TYPE id;
    GOAL_CSAP_FUNC_POP fnPop;
    GOAL_CSAP_PARAMS_TYPE fncParams;
    GOAL_BOOL_T severity;
    struct GOAL_CSAP_FUNCTION_LIST_T *pNext;
} GOAL_CSAP_FUNCTION_LIST_T;

typedef struct GOAL_CSAP_HANDLE_T {
    uint32_t modId;
    GOAL_CSAP_FUNCTION_LIST_T *pList;
    struct GOAL_CSAP_HANDLE_T *pNext;
} GOAL_CSAP_HANDLE_T;


/****************************************************************************/
/* List of public functions */
/****************************************************************************/
GOAL_STATUS_T goal_csapInitPre(
    void
);

void goal_csapConfigSizeSet(
    void
);

GOAL_STATUS_T goal_csapReg(
    GOAL_CSAP_HANDLE_T **ppHdlCsap,             /**< handle */
    uint32_t modId,                             /**< goal module id */
    GOAL_CSAP_FUNCTION_LIST_T *pFunctionList    /**< list of functions */
);

void goal_csapPush(
    GOAL_CSAP_TYPE value                        /**< configurations value */
);

GOAL_CSAP_TYPE goal_csapPop(
    void
);

void goal_csapPushString(
    const char *strValue                        /**< string value */
);

void goal_csapPopString(
    char **pStrValue                            /**< return string */
);

void goal_csapSkip(
    GOAL_CSAP_TYPE cnt                          /**< number of values to skip */
);

void goal_csapLock(
    void
);

void goal_csapUnlock(
    void
);

GOAL_STATUS_T goal_csapSetup(
    void
);

GOAL_STATUS_T goal_csapCfgValid(
    void
);

#endif /* GOAL_CONFIG_CSAP */

#endif /* GOAL_CSAP_H */
