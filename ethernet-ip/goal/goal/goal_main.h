/** @file
 *
 * @brief Main Module of GOAL
 *
 * This module initialises the Generic Open Abstraction Layer (GOAL) and the
 * underlying low level drivers of the hardware. It can also shut down GOAL.
 *
 * @copyright
 * Copyright 2010-2020.
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

#ifndef GOAL_MAIN_H
#define GOAL_MAIN_H


/****************************************************************************/
/* List of public variables */
/****************************************************************************/
extern GOAL_BOOL_T goal_flgInit;                /**< GOAL init flag */
extern GOAL_BOOL_T GOAL_flagShutdown;           /**< GOAL shutdown flag */


/****************************************************************************/
/* Data structures */
/****************************************************************************/
/**< GOAL stage direction (init/shutdown) */
typedef enum {
    GOAL_STAGE_INIT,
    GOAL_STAGE_SHUTDOWN,
} GOAL_STAGE_DIR_T;


/**< GOAL init/shutdown stages */
typedef enum {
    GOAL_STAGE_MIN,                             /**< lowest stage marker */

    GOAL_STAGE_MEM_PRE,                         /**< memory management initialization */
    GOAL_STAGE_MEM,                             /**< memory management ready */
    GOAL_STAGE_TARGET_PRE,                      /**< target initialization */
    GOAL_STAGE_TARGET,                          /**< target ready */
    GOAL_STAGE_BOARD_PRE,                       /**< board initialization */
    GOAL_STAGE_BOARD,                           /**< board ready */
    GOAL_STAGE_LOG_EARLY,                       /**< early logging ready */
    GOAL_STAGE_LOCK_PRE,                        /**< locking initialization */
    GOAL_STAGE_LOCK,                            /**< locking ready */
    GOAL_STAGE_LOG_PRE,                         /**< logging initialization */
    GOAL_STAGE_LOG,                             /**< logging ready */
    GOAL_STAGE_TIMER_PRE,                       /**< timer initialization */
    GOAL_STAGE_TIMER,                           /**< timer ready */
    GOAL_STAGE_CM_MOD_REG,                      /**< config management module register stage */
    GOAL_STAGE_CM_INIT,                         /**< config management internal init (totalConfigSize calculated) */
    GOAL_STAGE_CM_NVS_PRE,                      /**< NVS initialization */
    GOAL_STAGE_CM_MOD_ADD,                      /**< config management module add stage */
    GOAL_STAGE_CM_NVS,                          /**< config management NVS access stage */
    GOAL_STAGE_CM,                              /**< config management ready */
    GOAL_STAGE_STAT_PRE,                        /**< statistics initialization */
    GOAL_STAGE_STAT,                            /**< statistics ready */
    GOAL_STAGE_QUEUE_PRE,                       /**< buffer queue management initialization */
    GOAL_STAGE_QUEUE,                           /**< buffer queue management ready */
    GOAL_STAGE_TASK_PRE,                        /**< multitasking initialization */
    GOAL_STAGE_TASK,                            /**< multitasking ready */
    GOAL_STAGE_ETH_PRE,                         /**< Ethernet initialization */
    GOAL_STAGE_ETH,                             /**< Ethernet ready */
    GOAL_STAGE_NET_PRE,                         /**< TCP/IP stack initialization */
    GOAL_STAGE_NET,                             /**< TCP/IP stack ready */
    GOAL_STAGE_UTIL_PRE,                        /**< utility functions (rng, checksum) initialization */
    GOAL_STAGE_UTIL,                            /**< utility functions (rng, checksum) ready */
    GOAL_STAGE_CLI_PRE,                         /**< CLI initialization */
    GOAL_STAGE_CLI,                             /**< CLI ready */
    GOAL_STAGE_CTC_PRE,                         /**< CTC initialization */
    GOAL_STAGE_CTC,                             /**< CTC read */
    GOAL_STAGE_MODULES_PRE,                     /**< modules initialization */
    GOAL_STAGE_MODULES,                         /**< modules ready */
    GOAL_STAGE_GOAL_PRE,                        /**< GOAL finalization */
    GOAL_STAGE_GOAL,                            /**< GOAL ready */

    GOAL_STAGE_MAX,                             /**< highest stage marker */
} GOAL_STAGES_T;


/**< init/shutdown stages function list */
typedef struct GOAL_STAGE_HANDLER_T {
    GOAL_STAGES_T id;                           /**< stage id */
    GOAL_STAGE_DIR_T dir;                       /**< stage direction (init/shutdown) */
    GOAL_BOOL_T flgSucceed;                     /**< successfully called */
    union {
        GOAL_FUNC_RET_NOARG_T func;             /**< stage callback function w/o arg */
        GOAL_FUNC_RET_T funcArg;                /**< stage callback function with arg */
    } func;
    void *pArgFunc;                             /**< function argument */
    struct GOAL_STAGE_HANDLER_T *pNext;         /**< next pointer */
} GOAL_STAGE_HANDLER_T;


/**< main loop item */
typedef struct GOAL_MAIN_LOOP_T {
    GOAL_FUNC_NORET_NOARG_T pFunc;              /**< function pointer */
    void *pParam;                               /**< parameter */
    GOAL_BOOL_T flgExec;                        /**< execution flag */
    GOAL_BOOL_T flgParam;                       /**< parameter flag */

    struct GOAL_MAIN_LOOP_T *pNext;             /**< next pointer */
} GOAL_MAIN_LOOP_T;


/**< generic callback */
typedef struct GOAL_MAIN_CB_T {
    struct GOAL_MAIN_CB_T *pNext;               /**< next pointer */
    uint32_t idCb;                              /**< callback id */
    GOAL_FUNC_RET_NOARG_T func;                 /**< callback function */
} GOAL_MAIN_CB_T;


/**< callback module ID mapping */
typedef struct GOAL_MAIN_CB_MOD_T {
    struct GOAL_MAIN_CB_MOD_T *pNext;           /**< next pointer */
    uint32_t idMod;                             /**< module ID */
    GOAL_MAIN_CB_T *pCbs;                       /**< callback list */
} GOAL_MAIN_CB_MOD_T;


/****************************************************************************/
/* Defines/Macros */
/****************************************************************************/
#define GOAL_MAIN_CB_FOREACH(_idMod, _idCb, _type, ...) \
    for (GOAL_MAIN_CB_MOD_T *_pCbMod = goal_mainCbListGet(); _pCbMod; _pCbMod = _pCbMod->pNext) { \
        if (_idMod == _pCbMod->idMod) { \
            for (GOAL_MAIN_CB_T *_pCb = _pCbMod->pCbs; _pCb; _pCb = _pCb->pNext) { \
                if (_idCb == _pCb->idCb) { \
                    ((_type) ((void *) _pCb->func))(__VA_ARGS__); \
                } \
            } \
        } \
    }


#define GOAL_MAIN_CB_FOREACH_RES(_idMod, _idCb, _type, ...) \
    for (GOAL_MAIN_CB_MOD_T *_pCbMod = goal_mainCbListGet(); _pCbMod && GOAL_RES_OK(res); _pCbMod = _pCbMod->pNext) { \
        if (_idMod == _pCbMod->idMod) { \
            for (GOAL_MAIN_CB_T *_pCb = _pCbMod->pCbs; _pCb && GOAL_RES_OK(res); _pCb = _pCb->pNext) { \
                if (_idCb == _pCb->idCb) { \
                    res = ((_type) ((void *) _pCb->func))(__VA_ARGS__); \
                } \
            } \
        } \
    }


/****************************************************************************/
/* List of public functions */
/****************************************************************************/
GOAL_STATUS_T goal_init(
    void
);

void goal_shutdown(
    void
);

GOAL_BOOL_T goal_isInitialized(
    void
);

void goal_loop(
    void
);

GOAL_STATUS_T goal_mainStageReg(
    GOAL_STAGES_T id,                           /**< stage id */
    GOAL_STAGE_HANDLER_T *pStage,               /**< stage structure */
    GOAL_STAGE_DIR_T dir,                       /**< init/shutdown flag */
    GOAL_FUNC_RET_NOARG_T func                  /**< callback function */
);

GOAL_STATUS_T goal_mainStageArgReg(
    GOAL_STAGES_T id,                           /**< stage id */
    GOAL_STAGE_HANDLER_T *pStage,               /**< stage structure */
    GOAL_STAGE_DIR_T dir,                       /**< init/shutdown flag */
    GOAL_FUNC_RET_T func,                       /**< callback function */
    void *pArgFunc                              /**< callback argument */
);


/****************************************************************************/
/* External prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_mainLoopReg(
    GOAL_FUNC_NORET_NOARG_T pFunc               /**< loop function */
);

GOAL_STATUS_T goal_mainLoopParamReg(
    GOAL_FUNC_NORET_T pFunc,                    /**< loop function */
    void *pParam                                /**< function parameter */
);

GOAL_STATUS_T appl_init(
    void
);

GOAL_STATUS_T appl_setup(
    void
);

void appl_loop(
    void
);

GOAL_STATUS_T goal_mainCbListReg(
    GOAL_MAIN_CB_MOD_T **ppCbMod,               /**< [out] callback module */
    uint32_t idMod                              /**< module id */
);

GOAL_STATUS_T goal_mainCbReg(
    uint32_t idMod,                             /**< module id */
    uint32_t idCb,                              /**< callback id */
    GOAL_FUNC_RET_NOARG_T func                  /**< callback function */
);

GOAL_MAIN_CB_MOD_T * goal_mainCbListGet(
    void
);


#endif /* GOAL_MAIN_H */
