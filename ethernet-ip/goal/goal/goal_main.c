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

#define GOAL_ID GOAL_ID_MAIN
#include <goal_includes.h>

#if GOAL_CONFIG_LM_EMIT == 1
#include "protos/lm_emit/goal_lm_emit.h"
#endif

#if GOAL_CONFIG_MCTC == 1
#  include <goal_media/goal_mi_mctc.h>
#endif


/****************************************************************************/
/* Global variables */
/****************************************************************************/
GOAL_BOOL_T goal_flgInit = GOAL_FALSE;          /**< GOAL init flag */
GOAL_BOOL_T GOAL_flagShutdown = GOAL_FALSE;     /**< GOAL shutdown flag */


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/
static GOAL_STATUS_T goal_mainStageProcess(
    GOAL_STAGE_DIR_T dir,                       /**< init or shutdown */
    GOAL_STAGES_T id                            /**< stage id */
);

static GOAL_STATUS_T goal_mainLoopInit(
    void
);


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_STAGE_HANDLER_T *pStageHandler = NULL; /**< init/shutdown stage list */
static GOAL_MAIN_LOOP_T *pMainLoop = NULL;      /**< main loop function list */
static GOAL_LOCK_T *mpLockLoop;                 /**< loop lock */
static GOAL_STAGE_HANDLER_T mStageLoopInit;     /**< loop init stage handle */

static GOAL_FUNC_RET_NOARG_T funcInit[] = {
    goal_targetInitPre,

    goal_lockInitPre,

#if GOAL_CONFIG_LOGGING == 1
    goal_logInitPre,
    goal_lmInitPre,
#endif

#if (GOAL_CONFIG_LOGGING == 1) && (GOAL_CONFIG_LM_EMIT == 1)
    goal_lmEmitInitPre,
#endif

#if GOAL_CONFIG_CSAP == 1
    goal_csapInitPre,
#endif

    goal_memInitPre,

    goal_timerInitPre,
    goal_cmInitPre,
    goal_queuePoolInitPre,

#if GOAL_CONFIG_TASK == 1
    goal_taskInitPre,
#endif

#if GOAL_CONFIG_ETHERNET == 1
    goal_ethInitPre,
#endif

#if GOAL_CONFIG_TCPIP_STACK == 1
    goal_netInitPre,
    goal_netChanInitPre,
#endif

#if GOAL_CONFIG_MCTC == 1
    goal_mctcInitPre,
#endif

#if (GOAL_CONFIG_LIBRARY == 1) && (GOAL_CONFIG_MCTC_AC != 1)
    goal_libUtilInitPre,
#endif

    goal_utilInitPre,
};

static GOAL_MAIN_CB_MOD_T *mpCbMod;             /**< callback list */
static GOAL_STAGES_T mStage;                    /**< stage iterator */


/****************************************************************************/
/** GOAL Initialization
 *
 * This function initializes the Generic Open Abstraction Layer (GOAL) and the
 * underlying low level drivers.
 * It must be called before any other function.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_init(
    void
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    unsigned int cnt;                           /* counter */

    /* register stage for main loop initialization */
    res = goal_mainStageReg(GOAL_STAGE_LOCK, &mStageLoopInit, GOAL_STAGE_INIT, goal_mainLoopInit);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* initialize the application */
    res = appl_init();
    if (GOAL_RES_ERR(res)) {
        goal_shutdown();
    }

    /* process handler register calls */
    for (cnt = 0; ARRAY_ELEMENTS(funcInit) > cnt; cnt++) {
        res = funcInit[cnt]();
        if (GOAL_RES_ERR(res)) {
            return res;
        }
    }

    /* process stage handlers */
    for (mStage = (GOAL_STAGES_T) (GOAL_STAGE_MIN + 1); GOAL_RES_OK(res) && (GOAL_STAGE_MAX > mStage); mStage++) {
        res = goal_mainStageProcess(GOAL_STAGE_INIT, mStage);
    }

    /* cleanup if something went wrong */
    if (GOAL_RES_ERR(res)) {
        goal_shutdown();
    }

    /* init done */
    if (GOAL_RES_OK(res)) {

        /* GG_LOG ID:143: */
        goal_lmLog(GOAL_ID_MAIN, 143, 114, 0, 0, GOAL_LOG_SEV_INFO, "GOAL initialized");

#if GOAL_CONFIG_CSAP == 1
        res = goal_csapCfgValid();
        if (GOAL_RES_OK(res)) {
            /* GG_LOG ID:144: */
            goal_lmLog(GOAL_ID_MAIN, 144, 115, 0, 0, GOAL_LOG_SEV_INFO, "running csap setup");
            res = goal_csapSetup();
            if (GOAL_RES_ERR(res)) {
                /* GG_LOG ID:145: */
                goal_lmLog(GOAL_ID_MAIN, 145, 116, 0, 0, GOAL_LOG_SEV_ERROR, "application setup failed");
            }
        }
        else {
            /* skip an empty configuration, and process with initialization */
            res = GOAL_OK;
        }
#endif

#if (GOAL_CONFIG_MCTC == 0)
        /* call application setup if this device doesn't use MCTC */
        res = appl_setup();
        if (GOAL_RES_ERR(res)) {
            /* GG_LOG ID:146: */
            goal_lmLog(GOAL_ID_MAIN, 146, 116, 0, 0, GOAL_LOG_SEV_ERROR, "application setup failed");
            return res;
        }

        /* register application loop */
        res = goal_mainLoopReg(appl_loop);
        if (GOAL_RES_ERR(res)) {
            /* GG_LOG ID:147: */
            goal_lmLog(GOAL_ID_MAIN, 147, 117, 0, 0, GOAL_LOG_SEV_ERROR, "registering application loop failed");
            return res;
        }
#endif

#if GOAL_CONFIG_ETHERNET == 1
        /* signalize finish of application setup, only required by special platforms */
        goal_ethCmd(GOAL_ETH_CMD_SETUP_DONE, GOAL_FALSE, 0, NULL);
#endif

#if (GOAL_CONFIG_GEN_CTC_AC != 1) && (GOAL_CONFIG_GEN_CTC_CC != 1)
        /* lock memory allocation */
        goal_memInitDone();
#endif
    }

    /* if goal and application are initialized and set up, set flag */
    if (GOAL_RES_OK(res)) {
        goal_flgInit = GOAL_TRUE;
    }

    return res;
}


/****************************************************************************/
/** GOAL Stage Processor
 *
 * Process registered stage handlers.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_mainStageProcess(
    GOAL_STAGE_DIR_T dir,                       /**< init or shutdown */
    GOAL_STAGES_T id                            /**< stage id */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_STAGE_HANDLER_T *pStage;               /* stage pointer */

    /* iterate through all registered stage callbacks */
    GOAL_LL_FOREACH(pStageHandler, pStage) {

        if ((id == pStage->id) && (dir == pStage->dir)) {

            /* process all functions on init */
            if (GOAL_STAGE_INIT == dir) {

                /* call stage notification function and leave on first error */
                if (NULL == pStage->pArgFunc) {
                    res = pStage->func.func();
                } else {
                    res = pStage->func.funcArg(pStage->pArgFunc);
                }
                if (GOAL_RES_ERR(res)) {
                    return res;
                }

                /* set successful flag to true */
                pStage->flgSucceed = GOAL_TRUE;

            } else {

                /* on shutdown only process stages that were successful initialized */
                if (GOAL_TRUE == pStage->flgSucceed) {
                    if (NULL == pStage->pArgFunc) {
                        res = pStage->func.func();
                    } else {
                        res = pStage->func.funcArg(pStage->pArgFunc);
                    }
                    if (GOAL_RES_ERR(res)) {
                        return res;
                    }
                }
            }
        }
    }

    return GOAL_OK;
}


/****************************************************************************/
/** GOAL Stage registration without function argument
 *
 * Register a stage handler.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_mainStageReg(
    GOAL_STAGES_T id,                           /**< stage id */
    GOAL_STAGE_HANDLER_T *pStage,               /**< stage struct pointer */
    GOAL_STAGE_DIR_T dir,                       /**< init/shutdown flag */
    GOAL_FUNC_RET_NOARG_T func                  /**< callback function */
)
{
    return goal_mainStageArgReg(id, pStage, dir, (GOAL_FUNC_RET_T) ((void *) func), NULL);
}


/****************************************************************************/
/** GOAL Stage registration with function argument
 *
 * Register a stage handler.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_mainStageArgReg(
    GOAL_STAGES_T id,                           /**< stage id */
    GOAL_STAGE_HANDLER_T *pStage,               /**< stage structure */
    GOAL_STAGE_DIR_T dir,                       /**< init/shutdown flag */
    GOAL_FUNC_RET_T func,                       /**< callback function */
    void *pArgFunc                              /**< callback argument */
)
{
    GOAL_STAGE_HANDLER_T *pStageIdx;            /* stage iterator */

    /* check if stage is beyond current stage */
    if ((GOAL_STAGE_INIT == dir) && (id <= mStage)) {
        goal_logErr("trying to register stage handler for stage before current stage");
        return GOAL_ERR_WRONG_STATE;
    }

    /* return, if entry already in linked list */
    GOAL_LL_FOREACH(pStageHandler, pStageIdx) {
        if (NULL != pStage && pStageIdx == pStage) {
            return GOAL_OK;
        }
    }

    /* clear stage handler structure */
    GOAL_MEMSET(pStage, 0, sizeof(GOAL_STAGE_HANDLER_T));

    /* setup handler */
    pStage->id = id;
    pStage->dir = dir;
    pStage->func.funcArg = func;
    pStage->pArgFunc = pArgFunc;

    /* add handler to end of list */
    GOAL_LL_APPEND(pStageHandler, pStage);

    return GOAL_OK;
}


/****************************************************************************/
/** GOAL Shutdown
 *
 * This function deinitialises the Generic Open Abstraction Layer (GOAL) and
 * the underlying low level drivers.
 */
void goal_shutdown(
    void
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_STAGES_T stage;                        /* stage */

#if ((GOAL_CONFIG_LOGGING == 1) && (GOAL_CONFIG_LM_EMIT == 1))
    /* show log messages */
    goal_lmEmitLogDump();
#endif

    /* set the shutdown flag */
    GOAL_flagShutdown = GOAL_TRUE;

    /* process stage handlers in reverse order */
    for (stage = (GOAL_STAGES_T) (GOAL_STAGE_MAX - 1); GOAL_STAGE_MIN < stage; stage--) {
        res = goal_mainStageProcess(GOAL_STAGE_SHUTDOWN, stage);
        if (GOAL_RES_ERR(res)) {
            break;
        }
    }
}


/****************************************************************************/
/** Check the initialization status of the Abstraction Layer
 *
 * @retval GOAL_TRUE  GOAL has been started
 * @retval GOAL_FALSE GOAL was not started yet
 */
GOAL_BOOL_T goal_isInitialized(
    void
)
{
    return goal_flgInit;
}


/****************************************************************************/
/** Initialize the GOAL loop handling
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_mainLoopInit(
    void
)
{
    return goal_lockCreate(GOAL_LOCK_BINARY, &mpLockLoop, 0, 1, GOAL_ID_MAIN);
}


/****************************************************************************/
/** Execute cyclic GOAL tasks
 *
 * This function must be called cyclically. It performs cyclic tasks needed by
 * the Abstraction Layer. It also performs cyclic low level tasks.
 */
void goal_loop(
    void
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MAIN_LOOP_T *pLoop;                    /* main loop iterator */

    /* process registered handlers */
    for (pLoop = pMainLoop; pLoop; pLoop = pLoop->pNext) {

        /* lock meta flags */
        res = goal_lockGet(mpLockLoop, GOAL_LOCK_INFINITE);
        if (GOAL_RES_ERR(res)) {
            break;
        }

        /* don't call function recursively */
        /* TODO: secure with mutex, could be called from different threads */
        if (GOAL_TRUE == pLoop->flgExec) {

            /* unlock meta flags */
            goal_lockPut(mpLockLoop);

            continue;
        }

        /* run function */
        pLoop->flgExec = GOAL_TRUE;

        /* unlock meta flags */
        goal_lockPut(mpLockLoop);

        if (GOAL_TRUE == pLoop->flgParam) {
            ((GOAL_FUNC_NORET_T) (void *) pLoop->pFunc)(pLoop->pParam);
        } else {
            pLoop->pFunc();
        }

        /* lock meta flags */
        res = goal_lockGet(mpLockLoop, GOAL_LOCK_INFINITE);
        if (GOAL_RES_ERR(res)) {
            break;
        }

        /* mark function as free to call */
        pLoop->flgExec = GOAL_FALSE;

        /* unlock meta flags */
        goal_lockPut(mpLockLoop);
    }
}


/****************************************************************************/
/** Register main loop handler
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_mainLoopReg(
    GOAL_FUNC_NORET_NOARG_T pFunc               /**< loop function */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MAIN_LOOP_T **ppLoop;                  /* main loop iterator */
    GOAL_MAIN_LOOP_T *pLoop = NULL;             /* temp variable for allocation */

    /* find free loop list entry */
    for (ppLoop = &pMainLoop; *ppLoop; ppLoop = &(*ppLoop)->pNext);

    /* allocate temp loop element */
    res = goal_memCalloc(&pLoop, sizeof(GOAL_MAIN_LOOP_T));
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:148: */
        goal_lmLog(GOAL_ID_MAIN, 148, 118, 0, 0, GOAL_LOG_SEV_ERROR, "failed to allocate main loop element");
        return res;
    }

    /* assign loop element data */
    pLoop->pFunc = pFunc;

    /* assign new element */
    *ppLoop = pLoop;

    return GOAL_OK;
}


/****************************************************************************/
/** Register main loop handler with parameter
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_mainLoopParamReg(
    GOAL_FUNC_NORET_T pFunc,                    /**< loop function */
    void *pParam                                /**< function parameter */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MAIN_LOOP_T **ppLoop;                  /* main loop iterator */
    GOAL_MAIN_LOOP_T *pLoop = NULL;             /* temp variable for allocation */

    /* find free loop list entry */
    for (ppLoop = &pMainLoop; *ppLoop; ppLoop = &(*ppLoop)->pNext);

    /* allocate temp loop element */
    res = goal_memCalloc(&pLoop, sizeof(GOAL_MAIN_LOOP_T));;
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:149: */
        goal_lmLog(GOAL_ID_MAIN, 149, 118, 0, 0, GOAL_LOG_SEV_ERROR, "failed to allocate main loop element");
        return res;
    }

    /* assign loop element data */
    pLoop->pFunc = (GOAL_FUNC_NORET_NOARG_T) (void *) pFunc;
    pLoop->pParam = pParam;
    pLoop->flgParam = GOAL_TRUE;

    /* assign new element */
    *ppLoop = pLoop;

    return GOAL_OK;
}


/****************************************************************************/
/** Register Callback Module
 *
 * Before a callback can be registered a module that handles the callback must
 * register its ID and callback list. For GOAL core modules this is already
 * done by the compiler and makes them available at the earliest stage.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_mainCbListReg(
    GOAL_MAIN_CB_MOD_T **ppCbMod,               /**< [out] callback module */
    uint32_t idMod                              /**< module id */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MAIN_CB_MOD_T **ppCbModIt;             /* module iterator */

    /* search element in list or skip to list end */
    for (ppCbModIt = &mpCbMod; *ppCbModIt; ppCbModIt = &(*ppCbModIt)->pNext) {
        if (idMod == (*ppCbModIt)->idMod) {
            if (NULL != ppCbMod) {
                *ppCbMod = *ppCbModIt;
                return GOAL_OK_EXISTS;
            }
        }
    }

    /* add module entry */
    res = goal_memCalloc(ppCbModIt, sizeof(GOAL_MAIN_CB_MOD_T));
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:155: */
        goal_lmLog(GOAL_ID_MAIN, 155, 124, 0, 0, GOAL_LOG_SEV_ERROR, "failed to allocate callback module data");
        return res;
    }

    /* assign module id */
    (*ppCbModIt)->idMod = idMod;

    /* return callback module */
    if (NULL != ppCbMod) {
        *ppCbMod = *ppCbModIt;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Register Callback
 *
 * Callbacks can be registered as early as the matching module is registered in
 * the mCbMod list. GOAL core module callbacks are available at the earliest
 * stage while other modules must be registered through goal_mainCbListReg.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_mainCbReg(
    uint32_t idMod,                             /**< module id */
    uint32_t idCb,                              /**< callback id */
    GOAL_FUNC_RET_NOARG_T func                  /**< callback function */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MAIN_CB_MOD_T *pCbMod;                 /* callback module */
    GOAL_MAIN_CB_T **ppCbIt = NULL;             /* callback iterator */

    /* find or create module specific callback list */
    res = goal_mainCbListReg(&pCbMod, idMod);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create or get callback list");
        return res;
    }

    /* jump to list end */
    for (ppCbIt = &pCbMod->pCbs; *ppCbIt; ppCbIt = &(*ppCbIt)->pNext);

    /* attach new callback */
    res = goal_memCalloc(ppCbIt, sizeof(GOAL_MAIN_CB_T));
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:157: */
        goal_lmLog(GOAL_ID_MAIN, 157, 126, 0, 0, GOAL_LOG_SEV_ERROR, "failed to allocate callback data");
        return res;
    }

    (*ppCbIt)->idCb = idCb;
    (*ppCbIt)->func = func;

    return GOAL_OK;
}


/****************************************************************************/
/** Application Init Stub
 *
 * Stub for application init if application uses obsolete main_appl.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_TARGET_WEAK GOAL_STATUS_T appl_init(
    void
)
{
    return GOAL_OK;
}


/****************************************************************************/
/** Application Setup Stub
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_TARGET_WEAK GOAL_STATUS_T appl_setup(
    void
)
{
    return GOAL_OK;
}


/****************************************************************************/
/** Application Loop Stub
 */
GOAL_TARGET_WEAK void appl_loop(
    void
)
{
}


/****************************************************************************/
/** GOAL Main - Get Callback List
 */
GOAL_MAIN_CB_MOD_T * goal_mainCbListGet(
    void
)
{
    return mpCbMod;
}
