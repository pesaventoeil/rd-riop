/** @file
 *
 * @brief Configuration Management
 *
 * @details
 * This module implements a generic management of config variables incl.
 * storage.
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

#define GOAL_ID GOAL_ID_CM
#include <goal_includes.h>
#include "cm/goal_cm_cm.h"

#if GOAL_CONFIG_MEDIA_MI_CMFS == 1
#include <goal_media/goal_mi_cmfs.h>
#endif


/****************************************************************************/
/* Local strings */
/****************************************************************************/
#define GOAL_CM_STR_NOT_FOUND "not found"


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_STAGE_HANDLER_T stageInit;          /**< init stage handler */
static GOAL_STAGE_HANDLER_T stageInitLock;      /**< lock init stage handler */
static GOAL_STAGE_HANDLER_T stageInitNvs;       /**< NVS init stage handler */
static char *pConfigDataBuffer = NULL;          /**< Variables data buffer */
static char *pConfigDataBufferMirror = NULL;    /**< write mirror for config vars */
static GOAL_CM_MODULE_T *cmModulesPool;         /**< Modules info buffer */
static GOAL_CM_VAR_T *pCurVar;                  /**< Pointer to next free space */
static uint32_t remSize = 0;                    /**< Remaining free space */
static uint32_t modCnt = 0;                     /**< Number of modules */
static uint32_t totalConfigSize = 0;            /**< Total size of config buffer */
static GOAL_LOCK_T *pLckVarTable;               /**< Lock for var table access */
static GOAL_BOOL_T flgValDefault = GOAL_FALSE;  /**< variables default state flag */

#if GOAL_CONFIG_CLI == 1
static GOAL_STAGE_HANDLER_T goalCmStageCli;     /**< CLI stage */
#endif

static GOAL_CM_VIRT_VAR_T *pCmVirtualVars;      /**< CM virtual variables list */


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
static GOAL_STATUS_T goal_cmRebuildVars(
    void
);

static GOAL_STATUS_T goal_cmInitLock(
    void
);

static GOAL_STATUS_T goal_cmInit(
    void
);

static GOAL_STATUS_T goal_cmInitNvs(
    void
);

static GOAL_STATUS_T goal_cmTmpSet(
    uint32_t idMod,                             /**< module id */
    uint32_t idVar,                             /**< variable id */
    GOAL_CM_VAR_T *pVar,                        /**< variable pointer */
    uint32_t *pSizeNew                          /**< new size ref */
);

static GOAL_STATUS_T goal_cmModByIdGet(
    uint32_t modId,                             /**< module id */
    GOAL_CM_MODULE_T **ppMod                    /**< points to module if found */
);

static GOAL_STATUS_T goal_cmFreeModEntryGet(
    GOAL_CM_MODULE_T **ppMod                    /**< points to module if found */
);


/****************************************************************************/
/** Register stage handler
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_cmInitPre(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* initialize lock for client register stage */
    res = goal_mainStageReg(GOAL_STAGE_LOCK, &stageInitLock, GOAL_STAGE_INIT, goal_cmInitLock);

    /* setup configuration management data structures */
    if (GOAL_RES_OK(res)) {
        res = goal_mainStageReg(GOAL_STAGE_CM_INIT, &stageInit, GOAL_STAGE_INIT, goal_cmInit);
    }

    /* read values from NVS */
    if (GOAL_RES_OK(res)) {
        res = goal_mainStageReg(GOAL_STAGE_CM_NVS, &stageInitNvs, GOAL_STAGE_INIT, goal_cmInitNvs);
    }

#if GOAL_CONFIG_CLI == 1
    /* Register cli cmd */
    if (GOAL_RES_OK(res)) {
        res = goal_mainStageReg(GOAL_STAGE_CLI, &goalCmStageCli, GOAL_STAGE_INIT, goal_cmCliReg);
    }
#endif

    goal_cmRegCmVars();

    return res;
}


/****************************************************************************/
/** Preinitializes the config module
 *
 * @retval GOAL_OK Module initialized
 * @retval other failed
 */
static GOAL_STATUS_T goal_cmInitLock(
    void
)
{
    /* allocate lock for table of variables */
    return goal_lockCreate(GOAL_LOCK_BINARY, &pLckVarTable, 0, 1, GOAL_ID_CM_VAR_TABLE);
}


/****************************************************************************/
/** Calculates the size of a var table for later memory allocation.
 *
 * All modules must register BEFORE goal_cmInit is called to ensure that
 * enough memory is allocated
 *
 * @retval GOAL_OK Module registered
 * @retval other failed
 */
GOAL_STATUS_T goal_cmRegModule(
    GOAL_CM_VARENTRY_T *pVarTable               /**< Pointer to variable table */
)
{
    goal_lockGet(pLckVarTable, GOAL_LOCK_INFINITE);
    while (GOAL_VM_VAR_EOL != pVarTable->varId) {
        totalConfigSize += sizeof(GOAL_CM_VAR_T) + pVarTable->maxSize;
        if (pVarTable->maxSize % GOAL_TARGET_MEM_ALIGN_CPU != 0) {
            totalConfigSize += (GOAL_TARGET_MEM_ALIGN_CPU - (pVarTable->maxSize % GOAL_TARGET_MEM_ALIGN_CPU));
        }
        pVarTable++;
    }
    modCnt++;
    goal_lockPut(pLckVarTable);
    return GOAL_OK;
}


/****************************************************************************/
/** Initializes the configuration management
 *
 * This function allocates the memory required for the data of all variables
 * so it is required that all modules register their table prior execution
 * of this function.
 *
 * @retval GOAL_OK Module initialized
 * @retval other failed
 */
static GOAL_STATUS_T goal_cmInit(
    void
)
{
    GOAL_STATUS_T res;                          /* Result */

    /* CRC calculates based on words so add pad byte if required */
    if (totalConfigSize % 2 > 0) {
        totalConfigSize++;
    }

    /* Allocate memory for config data and management structures pool */
    res = goal_memCalloc(&pConfigDataBuffer, totalConfigSize + GOAL_CM_HEADER_OFFSET);
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:87: */
        goal_lmLog(GOAL_ID_CM, 87, 58, 4, 0, GOAL_LOG_SEV_ERROR, "Could not allocate $1 bytes of config data buffer!");
        goal_lmLogParamUINT32(totalConfigSize + (uint32_t) GOAL_CM_HEADER_OFFSET);
        goal_lmLogFinish();
    }

    if (GOAL_RES_OK(res)) {
        res = goal_memCalloc(&cmModulesPool, sizeof(GOAL_CM_MODULE_T) * modCnt);
    }

    /* Allocate memory for locked write mirror */
    if (GOAL_RES_OK(res)) {
        res = goal_memCalloc(&pConfigDataBufferMirror, totalConfigSize + GOAL_CM_HEADER_OFFSET);
        if (GOAL_RES_ERR(res)) {
            /* GG_LOG ID:88: */
            goal_lmLog(GOAL_ID_CM, 88, 59, 4, 0, GOAL_LOG_SEV_ERROR, "Could not allocate $1 bytes of mirrored config data buffer!");
            goal_lmLogParamUINT32(totalConfigSize + (uint32_t) GOAL_CM_HEADER_OFFSET);
            goal_lmLogFinish();
        }
    }

    /* Adjust buffer pointer and data size */
    if (GOAL_RES_OK(res)) {
        pCurVar = (GOAL_CM_VAR_T *) (pConfigDataBuffer + GOAL_CM_HEADER_OFFSET);
        remSize = totalConfigSize;
        totalConfigSize += GOAL_CM_HEADER_OFFSET;
        /* GG_LOG ID:89: */
        goal_lmLog(GOAL_ID_CM, 89, 60, 4, 4, GOAL_LOG_SEV_INFO, "Calculated config size of $1 in $2 modules");
        goal_lmLogParamUINT32(totalConfigSize);
        goal_lmLogParamUINT32(modCnt);
        goal_lmLogFinish();
    }

    return res;
}


/****************************************************************************/
/** Load values from NVS
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_cmInitNvs(
    void
)
{
    GOAL_STATUS_T res;                          /* result */
#if GOAL_CONFIG_MEDIA_MI_CMFS == 0

    /* read values from NVS */
    res = goal_cmLoad();
    if (GOAL_RES_ERR(res)) {
        flgValDefault = GOAL_TRUE;
    }
#else
    GOAL_MI_CMFS_T *pMiCmfs;                    /* cmfs mi handle */
    res = goal_miCmfsOpen(&pMiCmfs, 0, cmModulesPool);

    if (GOAL_RES_OK(res)) {
        res = goal_miCmfsDefault(pMiCmfs, &flgValDefault);
    }
#endif /* GOAL_CONFIG_MEDIA_MI_CMFS == 0 */

    return GOAL_OK;
}


/****************************************************************************/
/** Returns the size of the config data when written to NVS
 *
 * @retval Size of config in bytes
 */
uint32_t goal_cmGetConfigSize(
    void
)
{
    return totalConfigSize;
}


/****************************************************************************/
/** Adds all variables for the given module to the config system
 *
 * This function allocates space in the central config buffer for all
 * variables contained in the given list and assigns a pointer to
 * each individual variable.
 * It also allows to specify pointer to callbacks for loading, saving and
 * requesting temp values after loading. A NULL callback signals, that the
 * according callback shall not be invoked.
 *
 * @retval GOAL_OK Module added
 * @retval other failed
 */
GOAL_STATUS_T goal_cmAddModule(
    GOAL_CM_MODDEF_T *pModdef,                  /**< The module to register */
    GOAL_CM_VARENTRY_T *pVarTable,              /**< Pointer to variable table */
    goal_cm_load load,                          /**< Pointer to load callback */
    goal_cm_save save,                          /**< Pointer to save callback */
    goal_cm_tmpset tmpset                       /**< Pointer to temp value callback */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* Result */
    GOAL_CM_VARENTRY_T *pVar;                   /* Varentry pointer */
    GOAL_CM_VAR_T *pMemVar;                     /* Var pointer */
    uint32_t padBytes;                          /* Number of pad bytes */
    GOAL_CM_MODULE_T *pMod = NULL;              /* Module pointer */


    goal_lockGet(pLckVarTable, GOAL_LOCK_INFINITE);

    /* Get pointer to a free module entry */
    res = goal_cmFreeModEntryGet(&pMod);

    /* No more free entries */
    if (GOAL_RES_ERR(res)) {
        goal_lockPut(pLckVarTable);
        return res;
    }

    pMod->var = pVarTable;
    pVar = pVarTable;
    pMod->modId = pModdef->modId;

#if GOAL_CM_NAMES == 1
    GOAL_MEMCPY(pMod->modname, pModdef->modName, strlen(pModdef->modName));
    goal_logDbg("Adding mod %s (%" FMT_u32 ")", pModdef->modName, pModdef->modId);
#endif
    /* Iterate through var list until end of list */
    while (GOAL_VM_VAR_EOL != pVar->varId) {
        if (remSize < sizeof(GOAL_CM_VAR_T) + pVar->maxSize) {
            res = GOAL_ERR_ALLOC;
            break;
        }
#if GOAL_CM_NAMES == 1
        goal_logDbg("Adding var %s of type %u", pVar->varname, pVar->type);
#else
        goal_logDbg("Adding var %"FMT_u32"-%"FMT_u32, pModdef->modId, pVar->varId);
        goal_logDbg("    : type %u", pVar->type);
#endif

        padBytes = 0;
        pMemVar = pCurVar;
        /* Set var type, max size and id */
        pMemVar->type = pVar->type;
        pMemVar->totalSize = pVar->maxSize;
        pMemVar->varId = pVar->varId;
        pMemVar->modId = pModdef->modId;
        pVar->var = pMemVar;

        if (pVar->maxSize % GOAL_TARGET_MEM_ALIGN_CPU != 0) {
            padBytes = (GOAL_TARGET_MEM_ALIGN_CPU - (pVar->maxSize % GOAL_TARGET_MEM_ALIGN_CPU));
        }
        remSize -= sizeof(GOAL_CM_VAR_T) + pVar->maxSize + padBytes;

        /* Move pointer to next free space */
        pCurVar = (GOAL_CM_VAR_T *) (((char *) pCurVar) + sizeof(GOAL_CM_VAR_T) + pVar->maxSize + padBytes);
        pVar++;
        pMod->varCnt++;
    }

    /* Set callbacks */
    pMod->load = load;
    pMod->save = save;

    /* select default tmpset callback */
    if (NULL == tmpset) {
        pMod->tmpset = goal_cmTmpSet;
    } else {
        pMod->tmpset = tmpset;
    }

    goal_lockPut(pLckVarTable);
    return res;
}


/****************************************************************************/
/** Returns a pointer to the var with the given module and variable id
 *
 * @retval GOAL_OK Variable found
 * @retval other failed
 */
GOAL_STATUS_T goal_cmGetVarById(
    uint32_t modId,                             /**< Module ID of the var to search for */
    uint32_t varId,                             /**< Variable ID of the var to search for */
    GOAL_CM_VAR_T **ppVar                       /**< Pointer to the var to search for */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* Result */
    GOAL_CM_VARENTRY_T *pVarEntry = NULL;       /* variable entry */

    res = goal_cmGetVarEntryById(modId, varId, &pVarEntry);
    if (GOAL_RES_OK(res)) {
        *ppVar = pVarEntry->var;
    }

    return res;
}


/****************************************************************************/
/** Returns a pointer to the var entry with the given module and variable id
 *
 * @retval GOAL_OK Variable found
 * @retval other failed
 */
GOAL_STATUS_T goal_cmGetVarEntryById(
    uint32_t modId,                             /**< Module ID of the var to search for */
    uint32_t varId,                             /**< Variable ID of the var to search for */
    GOAL_CM_VARENTRY_T **ppVar                  /**< Pointer to the var to search for */
)
{
    GOAL_CM_MODULE_T *pMod;                     /* module pointer */
    GOAL_STATUS_T res = GOAL_OK;                /* Result */
    GOAL_CM_VIRT_VAR_T *pVirtualEntry;          /* virtual variable */

    /* Get pointer to module */
    res = goal_cmModByIdGet(modId, &pMod);

    if (GOAL_RES_ERR(res) || (0 == pMod->varCnt) || (varId > (pMod->varCnt - 1))) {

        res = GOAL_ERR_PARAM;

        /* check if virtual variable exists */
        GOAL_LL_FOREACH(pCmVirtualVars, pVirtualEntry) {
            if (pVirtualEntry->pVarEntry->var->modId == modId && pVirtualEntry->pVarEntry->var->varId == varId) {
                *ppVar = pVirtualEntry->pVarEntry;
                res = GOAL_OK;
                continue;
            }
        }

        if (GOAL_RES_ERR(res)) {
            /* GG_LOG ID:90: */
            goal_lmLog(GOAL_ID_CM, 90, 61, 4, 4, GOAL_LOG_SEV_ERROR, "Tried to access UNKNOWN var with id $1-$2");
            goal_lmLogParamUINT32(modId);
            goal_lmLogParamUINT32(varId);
            goal_lmLogFinish();
        }
    } else {
        *ppVar = (pMod->var + varId);
    }

    return res;
}


/****************************************************************************/
/** Sets the value of the var identified by module and var id
 *
 * @retval GOAL_OK Value set
 * @retval other failed
 */
GOAL_STATUS_T goal_cmSetVarValue(
    uint32_t modId,                             /**< Module ID of the var */
    uint32_t varId,                             /**< Variable ID of the var */
    void *pData,                                /**< Pointer to the value to set */
    uint32_t size,                              /**< Size of the data to set */
    GOAL_BOOL_T temp,                           /**< If true, the value is marked as temporary */
    goal_cm_changed exclude                     /**< If not NULL, the given changed callback is not invoked */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* Result */
    GOAL_CM_VARENTRY_T *pEntry;                 /* Varentry pointer */
    GOAL_CM_CB_T *pCb;                          /* Callback pointer */
    GOAL_CM_MODULE_T *pMod;                     /* module pointer */
    GOAL_CM_VIRT_VAR_T *pVirtualEntry;          /* virtual variable */

    goal_lockGet(pLckVarTable, GOAL_LOCK_INFINITE);

    /* Get pointer to module */
    res = goal_cmModByIdGet(modId, &pMod);

    /* check if variable does not exist */
    if (GOAL_RES_ERR(res) || (varId > pMod->varCnt-1)) {

        res = GOAL_ERR_NOT_FOUND;
        pEntry = NULL;

        /* check if virtual variable was registered */
        GOAL_LL_FOREACH(pCmVirtualVars, pVirtualEntry) {
            if (pVirtualEntry->pVarEntry->var->modId == modId && pVirtualEntry->pVarEntry->var->varId == varId) {
                pEntry = pVirtualEntry->pVarEntry;
                res = GOAL_OK;
                continue;
            }
        }
    }
    else {
        pEntry = pMod->var + varId;
    }

    if (NULL == pEntry || pEntry->var->flags & GOAL_CM_FLAGS_LOCK) {
        res = GOAL_ERR_ACCESS;
    }
    if (GOAL_RES_OK(res)) {
        if (size>pEntry->var->totalSize) {
            res = GOAL_ERR_PARAM;
        }
    }
    if (GOAL_RES_OK(res)) {
        if (pEntry->validate != NULL) {
            res = pEntry->validate(modId, varId, pEntry->var, pData, size);
        }
    }

    /* check for modification in content */
    if (GOAL_RES_OK(res)) {
        if (0 != GOAL_MEMCMP((char *) pEntry->var + sizeof(GOAL_CM_VAR_T), pData, size)) {
            pEntry->var->flags |= GOAL_CM_FLAGS_MODIFIED;
        }
    }

    /* check for modification in size */
    if (GOAL_RES_OK(res)) {
        if (size != pEntry->var->curSize) {
            pEntry->var->flags |= GOAL_CM_FLAGS_MODIFIED;
        }
    }

    if (GOAL_RES_OK(res)) {
        GOAL_MEMSET((char *) pEntry->var + sizeof(GOAL_CM_VAR_T), 0, pEntry->var->totalSize);
        GOAL_MEMCPY((char *) pEntry->var + sizeof(GOAL_CM_VAR_T), pData, size);
        pEntry->var->curSize = size;
        if (GOAL_TRUE == temp) {
            pEntry->var->flags |= GOAL_CM_FLAGS_TEMP;
        } else {
            pEntry->var->flags &= (unsigned int) ~GOAL_CM_FLAGS_TEMP;
        }

        /* Inform callbacks about changed value */
        /* Owner callback first */
        if (pEntry->changed != NULL && exclude != pEntry->changed) {
            res = pEntry->changed(modId, varId, pEntry->var);
        }
        /* Other callbacks */
        if (NULL != pEntry->cb_first) {
            pCb = pEntry->cb_first;
            while (NULL != pCb) {
                if (exclude != pCb->changed) {
                    pCb->changed(modId, varId, pEntry->var);
                }
                pCb = pCb->next;
            }
        }
    }
    goal_lockPut(pLckVarTable);

    return res;
}


/****************************************************************************/
/** Triggers the change callbacks for the var identified by mod and var id
 *
 * @retval GOAL_OK Callbacks called
 * @retval other failed
 */
GOAL_STATUS_T goal_cmTriggerChange(
    uint32_t modId,                             /**< Module ID of the var */
    uint32_t varId,                             /**< Variable ID of the var */
    goal_cm_changed exclude                     /**< If not NULL, the given callback is not invoked */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* Result */
    GOAL_CM_VARENTRY_T *pEntry;                 /* Varentry pointer */
    GOAL_CM_CB_T *pCb;                          /* Callback pointer */
    GOAL_CM_MODULE_T *pMod;                     /* module pointer */
    GOAL_CM_VIRT_VAR_T *pVirtualEntry;          /* virtual variable */

    /* Get pointer to module */
    res = goal_cmModByIdGet(modId, &pMod);

    /* check if variable does not exist */
    if (GOAL_RES_ERR(res) || (varId > pMod->varCnt-1)) {

        res = GOAL_ERR_NOT_FOUND;
        pEntry = NULL;

        /* check if virtual variable was registered */
        GOAL_LL_FOREACH(pCmVirtualVars, pVirtualEntry) {
            if (pVirtualEntry->pVarEntry->var->modId == modId && pVirtualEntry->pVarEntry->var->varId == varId) {
                pEntry = pVirtualEntry->pVarEntry;
                res = GOAL_OK;
                continue;
            }
        }
    }
    else {
        pEntry = pMod->var + varId;
    }

    if (GOAL_RES_OK(res)) {
        if (pEntry->var->flags & GOAL_CM_FLAGS_LOCK) {
            res = GOAL_ERR_ACCESS;
        }
    }

    if (GOAL_RES_OK(res)) {
        /* Inform callbacks about changed value */
        /* Owner callback first */
        if (pEntry->changed != NULL && pEntry->changed != exclude) {
            res = pEntry->changed(modId, varId, pEntry->var);
        }
        /* Other callbacks */
        for (pCb = pEntry->cb_first; pCb; pCb = pCb->next) {
            if (pCb->changed != exclude) {
                pCb->changed(modId, varId, pEntry->var);
            }
        }
    }

    return res;
}


/****************************************************************************/
/** Sets the given flag for the var identified by module and var id
 *
 * @retval GOAL_OK Flag set
 * @retval other failed
 */
GOAL_STATUS_T goal_cmSetVarFlag(
    uint32_t modId,                             /**< Module ID of the var */
    uint32_t varId,                             /**< Variable ID of the var */
    uint32_t flag,                              /**< The flag(s) to set */
    GOAL_BOOL_T enable                          /**< If true, flag is set, otherwise flag is deleted */
)
{
    GOAL_STATUS_T res;                          /* Result */
    GOAL_CM_VAR_T *pEntry;                      /* Var pointer */

    res = goal_cmGetVarById(modId, varId, &pEntry);
    if (GOAL_RES_OK(res)) {
        if (GOAL_FALSE == enable) {
            pEntry->flags &= (unsigned int) ~flag;
        } else {
            pEntry->flags |= flag;
        }
    }
    return res;
}


/****************************************************************************/
/** Returns the given flag for the var identified by module and var id
 *
 * @retval GOAL_OK Flag set
 * @retval other failed
 */
GOAL_STATUS_T goal_cmGetVarFlag(
    uint32_t modId,                             /**< Module ID of the var */
    uint32_t varId,                             /**< Variable ID of the var */
    uint32_t flag,                              /**< The flag(s) to set */
    GOAL_BOOL_T *pEnabled                       /**< If true, flag is set, otherwise flag is not set */
)
{
    GOAL_STATUS_T res;                          /* Result */
    GOAL_CM_VAR_T *pEntry;                      /* Var pointer */

    goal_lockGet(pLckVarTable, GOAL_LOCK_INFINITE);
    res = goal_cmGetVarById(modId, varId, &pEntry);
    if (GOAL_RES_OK(res)) {
        *pEnabled = (pEntry->flags & flag) ? GOAL_TRUE : GOAL_FALSE;
    }
    goal_lockPut(pLckVarTable);
    return res;
}


/****************************************************************************/
/** Saves all variables to permanent storage using the appropriate target
 *  function
 *
 * @retval GOAL_OK Values saved
 * @retval other failed
 */
GOAL_STATUS_T goal_cmSaveImpl(
    void
)
{
    GOAL_STATUS_T res;                          /* Result */
    uint32_t idx;                               /* Index */
    GOAL_CM_VARENTRY_T *pVar;                   /* Var pointer */
    GOAL_CM_HEADER_T *pHeader;                  /* Config header pointer */

    res = goal_lockGet(pLckVarTable, GOAL_LOCK_INFINITE);

    if (GOAL_RES_OK(res)) {
        /* Delete modified flags for all vars */
        for (idx = 0; idx < modCnt; idx++) {
            pVar = cmModulesPool[idx].var;
            while (GOAL_VM_VAR_EOL != pVar->varId) {
                pVar->var->flags &= (unsigned int) ~GOAL_CM_FLAGS_MODIFIED;
                pVar++;
            }
        }

        /* only write to non volatile storage on change */
        if (0 != GOAL_MEMCMP(pConfigDataBuffer, pConfigDataBufferMirror, totalConfigSize)) {

            /* Fill header */
            pHeader = (GOAL_CM_HEADER_T *) pConfigDataBuffer;
            GOAL_MEMCPY(pHeader->magic, GOAL_CM_MAGIC, GOAL_CM_MAGIC_LEN);
            pHeader->version = GOAL_CM_VERSION;
            pHeader->size = (unsigned int) (((char *) pCurVar) - pConfigDataBuffer) - GOAL_CM_HEADER_OFFSET;

            /* CRC calculates based on words so add pad byte if required */
            if (pHeader->size % 2 != 0) {
                pHeader->size++;
            }
            pHeader->chksum = goal_utilCrcFletcher32((uint16_t *) (pConfigDataBuffer + GOAL_CM_HEADER_OFFSET), pHeader->size / 2);

            /* Write data to storage */
            res = goal_targetNvsWriteData(pConfigDataBuffer, totalConfigSize);
            if (GOAL_RES_OK(res)) {
                /* Copy buffer to config mirror */
                GOAL_MEMCPY(pConfigDataBufferMirror, pConfigDataBuffer, totalConfigSize);
            }
            else {
                /* GG_LOG ID:91: */
                goal_lmLog(GOAL_ID_CM, 91, 62, 0, 0, GOAL_LOG_SEV_ERROR, "failed to write to NVS");
            }
        }
    }

    goal_lockPut(pLckVarTable);

    return res;
}


/****************************************************************************/
/** Saves all variables to permanent storage using cmfs media interface
 *
 * @retval GOAL_OK Values saved
 * @retval other failed
 */
GOAL_STATUS_T goal_cmSaveCmfs(
    void
)
{
#if GOAL_CONFIG_MEDIA_MI_CMFS == 1
    return goal_miCmfsSave(pLckVarTable, modCnt, cmModulesPool);
#else
    return GOAL_ERR_UNSUPPORTED;
#endif
}


/****************************************************************************/
/** Loads the variables from permanent storage using the appropriate target
 *  function
 *
 * @retval GOAL_OK Values loaded
 * @retval other failed
 */
GOAL_STATUS_T goal_cmLoad(
    void
)
{
    GOAL_STATUS_T res;                          /* Result */
    GOAL_CM_VARENTRY_T *pVarEntry;              /* Varentry pointer */
    GOAL_CM_VAR_T *pVar = NULL;                 /* Var pointer */
    GOAL_CM_HEADER_T *pHeader = NULL;           /* Header pointer */
    uint32_t idx;                               /* Index */
    uint32_t size = 0;                          /* Size of data portion */
    uint32_t vSize = 0;                         /* Size incl. Var header */
    uint32_t chkSum;                            /* Fletcher-32 checksum */
    uint32_t padBytes;                          /* Number of pad bytes */

    goal_lockGet(pLckVarTable, GOAL_LOCK_INFINITE);

    /* Clear config buffer and load data from storage */
    GOAL_MEMSET(pConfigDataBuffer, 0, totalConfigSize);
    res = goal_targetNvsReadData(pConfigDataBuffer, totalConfigSize);

    /* Check for valid header magic */
    if (GOAL_RES_OK(res)) {
        pHeader = (GOAL_CM_HEADER_T *) pConfigDataBuffer;
        if (GOAL_MEMCMP(pHeader->magic, GOAL_CM_MAGIC, GOAL_CM_MAGIC_LEN)) {
            GOAL_MEMSET(pConfigDataBuffer, 0, totalConfigSize);
            res = GOAL_ERR_PARAM;
        }
    }
    if (GOAL_RES_OK(res)) {
        if (pHeader->version != GOAL_CM_VERSION) {
            /* GG_LOG ID:92: */
            goal_lmLog(GOAL_ID_CM, 92, 63, 4, 4, GOAL_LOG_SEV_ERROR, "Versions DO NOT match (found $1, expected $2)!");
            goal_lmLogParamUINT32(pHeader->version);
            goal_lmLogParamUINT32((uint32_t) GOAL_CM_VERSION);
            goal_lmLogFinish();
            GOAL_MEMSET(pConfigDataBuffer, 0, totalConfigSize);
            res = GOAL_ERR_PARAM;
        }
    }

    if (GOAL_RES_OK(res)) {
        pVar = (GOAL_CM_VAR_T *) ((char *) (pConfigDataBuffer + GOAL_CM_HEADER_OFFSET));

        vSize = sizeof(GOAL_CM_VAR_T);
        size = pHeader->size;
        if (size != (totalConfigSize - GOAL_CM_HEADER_OFFSET)) {
            /* GG_LOG ID:93: */
            goal_lmLog(GOAL_ID_CM, 93, 64, 4, 4, GOAL_LOG_SEV_ERROR, "Config sizes DO NOT match ($1 vs. $2)!");
            goal_lmLogParamUINT32(size);
            goal_lmLogParamUINT32(totalConfigSize - (uint32_t) GOAL_CM_HEADER_OFFSET);
            goal_lmLogFinish();
            GOAL_MEMSET(pConfigDataBuffer, 0, totalConfigSize);
            res = GOAL_ERR_PARAM;
        }
    }
    if (GOAL_RES_OK(res)) {
        chkSum = goal_utilCrcFletcher32((uint16_t *) (pConfigDataBuffer + GOAL_CM_HEADER_OFFSET), pHeader->size / 2);
        if (!(chkSum == pHeader->chksum)) {
            /* GG_LOG ID:94: */
            goal_lmLog(GOAL_ID_CM, 94, 65, 4, 4, GOAL_LOG_SEV_ERROR, "Checksums $1 and $2 DONT match!");
            goal_lmLogParamUINT32(chkSum);
            goal_lmLogParamUINT32(pHeader->chksum);
            goal_lmLogFinish();
            GOAL_MEMSET(pConfigDataBuffer, 0, totalConfigSize);
            res = GOAL_ERR_PARAM;
        } else {
            /* GG_LOG ID:95: */
            goal_lmLog(GOAL_ID_CM, 95, 66, 4, 4, GOAL_LOG_SEV_INFO, "Checksums $1 and $2 match!");
            goal_lmLogParamUINT32(chkSum);
            goal_lmLogParamUINT32(pHeader->chksum);
            goal_lmLogFinish();
        }
    }

    /* Assign var pointer for all variables */
    while ((size > vSize) && GOAL_RES_OK(res)) {
        padBytes = 0;
        res = goal_cmGetVarEntryById(pVar->modId, pVar->varId, &pVarEntry);

        if (GOAL_RES_OK(res)) {
            goal_logDbg("Loading %" FMT_u32 "-%" FMT_u32, pVar->modId, pVar->varId);
            goal_logDbg("    : at %" FMT_ptrdiff " (should be at %" FMT_ptrdiff ")", (char *) pVar - pConfigDataBuffer, (char *) pVarEntry->var - pConfigDataBuffer);

            if (pVarEntry->type != pVar->type) {
                /* GG_LOG ID:96: */
                goal_lmLog(GOAL_ID_CM, 96, 67, 4, 4, GOAL_LOG_SEV_ERROR, "Var type of var with mod ID $1 and var ID $2");
                goal_lmLogParamUINT32(pVar->modId);
                goal_lmLogParamUINT32(pVar->varId);
                goal_lmLogFinish();
                /* GG_LOG ID:97: */
                goal_lmLog(GOAL_ID_CM, 97, 68, 4, 4, GOAL_LOG_SEV_ERROR, "    : DOES NOT match ($1 vs. $2)!");
                goal_lmLogParamUINT32(pVarEntry->type);
                goal_lmLogParamUINT32(pVar->type);
                goal_lmLogFinish();
                res = GOAL_ERR_PARAM;
            } else {
                pVarEntry->var = pVar;
                pVarEntry->maxSize = pVar->totalSize;
                goal_logDbg("Added var with size %" FMT_u32, pVar->totalSize);
            }
        } else {
            /* GG_LOG ID:98: */
            goal_lmLog(GOAL_ID_CM, 98, 69, 4, 4, GOAL_LOG_SEV_ERROR, "Var with mod id $1 and var id $2");
            goal_lmLogParamUINT32(pVar->modId);
            goal_lmLogParamUINT32(pVar->varId);
            goal_lmLogFinish();
            /* GG_LOG ID:99: */
            goal_lmLog(GOAL_ID_CM, 99, 70, 8, 0, GOAL_LOG_SEV_ERROR, "    : NOT found (searched at $1)!");
            goal_lmLogParamPTRDIFF((uint64_t) ((char *) pVar - (char *) pConfigDataBuffer));
            goal_lmLogFinish();
            res = GOAL_ERR_PARAM;
        }
        if (pVar->totalSize % GOAL_TARGET_MEM_ALIGN_CPU != 0) {
            padBytes = (GOAL_TARGET_MEM_ALIGN_CPU - (pVar->totalSize % GOAL_TARGET_MEM_ALIGN_CPU));
        }
        size -= vSize + pVar->totalSize + padBytes;
        pVar = (GOAL_CM_VAR_T *) (((char *) pVar) + vSize + pVar->totalSize + padBytes);
        pCurVar = pVar;
    }

    /* Something went wrong restore default values */
    if (GOAL_RES_ERR(res)) {
        goal_cmRebuildVars();
    }
    else {
        /* non volatile storage is valid, update mirror buffer */
        GOAL_MEMCPY(pConfigDataBufferMirror, pConfigDataBuffer, totalConfigSize);
    }

    /* check if pVar is assigned at this point */
    if (GOAL_RES_OK(res) && !pVar) {
        res = GOAL_ERR_PARAM;
    }

    /* Call temporary value updater */
    if (GOAL_RES_OK(res)) {
        for (idx = 0; idx < modCnt; idx++) {
            pVarEntry = cmModulesPool[idx].var;
            if (cmModulesPool[idx].tmpset != NULL) {
                while (pVarEntry->varId != GOAL_VM_VAR_EOL) {
                    if (pVarEntry->var->flags & GOAL_CM_FLAGS_TEMP) {
                        pVar = pVarEntry->var;
                        size = pVar->curSize;
                        cmModulesPool[idx].tmpset(pVar->modId, pVar->varId, pVar, &size);
                        pVar->curSize = size;
                    }
                    pVarEntry++;
                }
            }
        }
    }
    goal_lockPut(pLckVarTable);
    return res;
}


/****************************************************************************/
/** This function rebuilds the var table in case an invalid storage file
 * was loaded from NVS
 *
 * @retval GOAL_OK Var table rebuild
 * @retval other failed
 */
static GOAL_STATUS_T goal_cmRebuildVars(
    void
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* Result */
    GOAL_CM_VARENTRY_T *pVarEntry;              /* Varentry pointer */
    uint32_t idx;                               /* Index */
    uint32_t padBytes;                          /* Number of pad bytes */

    GOAL_MEMSET(pConfigDataBuffer, 0, totalConfigSize);
    pCurVar = (GOAL_CM_VAR_T *) (pConfigDataBuffer + GOAL_CM_HEADER_OFFSET);

    /* Something went wrong restore default values */
    for (idx = 0; idx < modCnt; idx++) {
        pVarEntry = cmModulesPool[idx].var;
        while (pVarEntry->varId != GOAL_VM_VAR_EOL) {
            padBytes = 0;
            pCurVar->modId = cmModulesPool[idx].modId;
            pCurVar->varId = pVarEntry->varId;
            pCurVar->totalSize = pVarEntry->maxSize;
            pCurVar->type = pVarEntry->type;
            pCurVar->flags = 0;

            if (pCurVar->totalSize % GOAL_TARGET_MEM_ALIGN_CPU != 0) {
                padBytes = (GOAL_TARGET_MEM_ALIGN_CPU - (pCurVar->totalSize % GOAL_TARGET_MEM_ALIGN_CPU));
            }

            pVarEntry->var = pCurVar;
            pCurVar = (GOAL_CM_VAR_T *) (((char *) pCurVar) + sizeof(GOAL_CM_VAR_T) + pCurVar->totalSize + padBytes);

            pVarEntry++;
        }
    }

    if (GOAL_RES_OK(res)) {
        uint32_t version = GOAL_CM_VERSION;     /* cm version */
        res = goal_cmSetVarValue(GOAL_ID_CM, CM_CM_VAR_VERSION, &version, sizeof(version), GOAL_FALSE, NULL);
    }

    return res;
}


/****************************************************************************/
/** Adds the given change callback for the var identified by module and
 *  var id
 *
 * @retval GOAL_OK Callback added
 * @retval other failed
 */
GOAL_STATUS_T goal_cmAddCallback(
    uint32_t modId,                             /**< Module ID of the var */
    uint32_t varId,                             /**< Variable ID of the var */
    goal_cm_changed change                      /**< Function pointer to callback */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* Result */
    GOAL_CM_VARENTRY_T *pEntry;                 /* Varentry pointer */
    GOAL_CM_CB_T *pCb = NULL;                   /* Callback pointer */

    goal_lockGet(pLckVarTable, GOAL_LOCK_INFINITE);

    res = goal_cmGetVarEntryById(modId, varId, &pEntry);
    if (GOAL_RES_OK(res)) {
        res = goal_memCalloc(&pCb, sizeof(GOAL_CM_CB_T));
    }
    if (GOAL_RES_OK(res)) {
        pCb->next = NULL;
        pCb->changed = change;
        if (NULL == pEntry->cb_first) {
            pEntry->cb_first = pCb;
            pEntry->cb_last = pCb;
        } else {
            pEntry->cb_last->next = pCb;
            pEntry->cb_last = pCb;
        }
    }

    goal_lockPut(pLckVarTable);
    return res;
}


/****************************************************************************/
/** Locks the mutex for the given var
 *
 * @retval GOAL_OK Variable locked
 * @retval other failed
 */
GOAL_STATUS_T goal_cmLockVar(
    uint32_t modId,                             /**< Module ID of the var */
    uint32_t varId                              /**< Variable ID of the var */
)
{
    GOAL_STATUS_T res;                          /* Result */
    GOAL_CM_VARENTRY_T *pVar;                   /* Varentry pointer */

    res = goal_cmGetVarEntryById(modId, varId, &pVar);
    if (GOAL_RES_OK(res)) {
        res = goal_lockGet(pLckVarTable, GOAL_LOCK_INFINITE);
    }
    return res;
}


/****************************************************************************/
/** Locks the mutex for the given var
 *
 * @retval GOAL_OK Variable locked
 * @retval other failed
 */
GOAL_STATUS_T goal_cmLockVarEntry(
    GOAL_CM_VARENTRY_T *pVar                    /**< Pointer to var struct */
)
{
    GOAL_STATUS_T res;                          /* Result */

    UNUSEDARG(pVar);
    res = goal_lockGet(pLckVarTable, GOAL_LOCK_INFINITE);
    return res;
}


/****************************************************************************/
/** Unlocks the mutex for the given var
 *
 * @retval GOAL_OK Variable locked
 * @retval other failed
 */
GOAL_STATUS_T goal_cmUnlockVarEntry(
    GOAL_CM_VARENTRY_T *pVar                    /**< Pointer to var struct */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* Result */

    UNUSEDARG(pVar);
    goal_lockPut(pLckVarTable);
    return res;
}


/****************************************************************************/
/** Unlocks the mutex for the given var
 *
 * @param modId Module ID of the var
 * @param varId Variable ID of the var
 *
 * @retval GOAL_OK Variable locked
 * @retval other failed
 */
GOAL_STATUS_T goal_cmUnlockVar(
    uint32_t modId,                             /**< Module ID of the var */
    uint32_t varId                              /**< Variable ID of the var */
)
{
    GOAL_STATUS_T res;                          /* Result */
    GOAL_CM_VARENTRY_T *pVar;                   /* Varentry pointer */

    res = goal_cmGetVarEntryById(modId, varId, &pVar);
    if (GOAL_RES_OK(res)) {
        goal_lockPut(pLckVarTable);
    }
    return res;
}


/****************************************************************************/
/** Loads the variables from permanent storage using the appropriate customer
 *  specific callbacks for each module.
 *
 * @retval GOAL_OK Values loaded
 * @retval other failed
 */
GOAL_STATUS_T goal_cmLoadCustom(
    void
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* Result */
    uint32_t idx;                               /* Index */
    uint32_t size;                              /* Size of data portion */
    GOAL_CM_VARENTRY_T *pVarEntry;              /* Varentry pointer */

    for (idx = 0; idx < modCnt; idx++) {
        if (cmModulesPool[idx].load != NULL) {
            pVarEntry = cmModulesPool[idx].var;
            if (cmModulesPool[idx].tmpset != NULL) {
                while (pVarEntry->varId != GOAL_VM_VAR_EOL) {
                    res = cmModulesPool[idx].load(idx, pVarEntry->varId, pVarEntry->var, &size);
                    pVarEntry->var->curSize = size;
                    pVarEntry++;
                }
            }
        }
    }
    return res;
}


/****************************************************************************/
/** Saves the variables to permanent storage using the appropriate customer
 *  specific callbacks for each module.
 *
 * @retval GOAL_OK Values loaded
 * @retval other failed
 */
GOAL_STATUS_T goal_cmSaveCustom(
    void
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* Result */
    uint32_t idx;                               /* Index */
    GOAL_CM_VARENTRY_T *pVarEntry;              /* Varentry pointer */

    for (idx = 0; idx < modCnt; idx++) {
        if (cmModulesPool[idx].save != NULL) {
            pVarEntry = cmModulesPool[idx].var;
            if (cmModulesPool[idx].tmpset != NULL) {
                while (pVarEntry->varId != GOAL_VM_VAR_EOL) {
                    res = cmModulesPool[idx].save(idx, pVarEntry->varId, pVarEntry->var);
                    pVarEntry++;
                }
            }
        }
    }
    return res;
}


/****************************************************************************/
/** Compares the data of the given var with given data.
 *
 * @retval GOAL_OK Comparison successful
 * @retval other failed
 */
GOAL_STATUS_T goal_cmCompare(
    GOAL_CM_VAR_T *pVar,                        /**< Pointer to variable to compare with */
    const char *pData,                          /**< Pointer to data to compare with */
    unsigned int len,                           /**< Length of data to compare */
    GOAL_BOOL_T *pMatch                         /**< After execution, true if data matches, otherwise false */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* Result */

    *pMatch = GOAL_TRUE;
    if (NULL == pVar || NULL == pData) {
        res = GOAL_ERR_PARAM;
    }
    if (GOAL_RES_OK(res)) {
        if (pVar->curSize != len) {
            *pMatch = GOAL_FALSE;
        }
    }
    if (GOAL_RES_OK(res)) {
        if (GOAL_TRUE == *pMatch) {
            if (0 != GOAL_MEMCMP((char *) pVar + sizeof(GOAL_CM_VAR_T), pData, len)) {
                *pMatch = GOAL_FALSE;
            }
        }
    }
    return res;
}


/****************************************************************************/
/** Clears the value of the given variable
 *
 * @retval GOAL_OK Value cleared
 * @retval other failed
 */
GOAL_STATUS_T goal_cmClearVal(
    GOAL_CM_VAR_T *pVar                         /**< Pointer to variable to clear */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* Result */

    pVar->curSize = 0;
    res = goal_cmClearValPat(pVar, 0);
    return res;
}


/****************************************************************************/
/** Clears the value of the given variable with the given pattern
 *
 * @retval GOAL_OK Value cleared
 * @retval other failed
 */
GOAL_STATUS_T goal_cmClearValPat(
    GOAL_CM_VAR_T *pVar,                        /**< Pointer to variable to clear */
    char pattern                                /**< The pointer to use for clearing */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* Result */

    GOAL_MEMSET((char *) pVar + sizeof(GOAL_CM_VAR_T), pattern, pVar->totalSize);
    pVar->curSize = 0;
    return res;
}


/****************************************************************************/
/** Prints a list of currently registerd vars
 *
 * @retval GOAL_OK List printed
 * @retval other failed
 */
GOAL_STATUS_T goal_cmPrintVarList(
    void
)
{
    uint32_t idx;                               /* Index */
    GOAL_CM_VARENTRY_T *pVar;                   /* Varentry pointer */


    for (idx = 0; idx < modCnt; idx++) {

        for (pVar = cmModulesPool[idx].var; pVar->varId != GOAL_VM_VAR_EOL; pVar++) {

#if GOAL_CM_NAMES == 1
            /* GG_LOG ID:100: */
            goal_lmLog(GOAL_ID_CM, 100, 71, 4, 4, GOAL_LOG_SEV_INFO, "$1-$2: ");
            goal_lmLogParamUINT32(pVar->var->modId);
            goal_lmLogParamUINT32(pVar->var->varId);
            goal_lmLogFinish();
            /* GG_LOG ID:101: */
            goal_lmLog(GOAL_ID_CM, 101, 72, 4, 4, GOAL_LOG_SEV_INFO, "    : type $1 curSize $2");
            goal_lmLogParamUINT32(pVar->var->type);
            goal_lmLogParamUINT32(pVar->var->curSize);
            goal_lmLogFinish();
            /* GG_LOG ID:102: */
            goal_lmLog(GOAL_ID_CM, 102, 73, 4, 0, GOAL_LOG_SEV_INFO, "    : max. size $1 ");
            goal_lmLogParamUINT32(pVar->var->totalSize);
            goal_lmLogFinish();
            /* GG_LOG ID:103: */
            goal_lmLog(GOAL_ID_CM, 103, 74, GOAL_STRLEN((const char *) cmModulesPool[idx].modname), GOAL_STRLEN((const char *) pVar->varname), GOAL_LOG_SEV_INFO, "    : ($1:$2)");
            goal_lmLogParamSTRING0(cmModulesPool[idx].modname);
            goal_lmLogParamSTRING0(pVar->varname);
            goal_lmLogFinish();
#else
            /* GG_LOG ID:104: */
            goal_lmLog(GOAL_ID_CM, 104, 75, 4, 4, GOAL_LOG_SEV_INFO, "$1-$2: ");
            goal_lmLogParamUINT32(pVar->var->modId);
            goal_lmLogParamUINT32(pVar->var->varId);
            goal_lmLogFinish();
            /* GG_LOG ID:105: */
            goal_lmLog(GOAL_ID_CM, 105, 76, 4, 4, GOAL_LOG_SEV_INFO, "    : type $1 curSize $2");
            goal_lmLogParamUINT32(pVar->var->type);
            goal_lmLogParamUINT32(pVar->var->curSize);
            goal_lmLogFinish();
            /* GG_LOG ID:106: */
            goal_lmLog(GOAL_ID_CM, 106, 77, 4, 0, GOAL_LOG_SEV_INFO, "    : max. size $1 ");
            goal_lmLogParamUINT32(pVar->var->totalSize);
            goal_lmLogFinish();
#endif
        }
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Get default state of variables
 *
 * Returns if reading data from NVS succeeded, if not all variables are reset
 * to their default values.
 *
 * @retval GOAL_TRUE variables contain default values
 * @retval GOAL_FALSE variables contain stored values
 */
GOAL_BOOL_T goal_cmVarIsDefaultImpl(
    uint32_t modId                              /**< module id */
)
{
    UNUSEDARG(modId);

    return flgValDefault;
}


/****************************************************************************/
/** Default temporary variables handler (tmpset)
 *
 * This is the default handler for each variable that is marked as temporary.
 * It can be overridden by registering an own tmpset handler in the
 * goal_cmAddModule call.

 * @retval GOAL_TRUE variables contain default values
 * @retval GOAL_FALSE variables contain stored values
 */
static GOAL_STATUS_T goal_cmTmpSet(
    uint32_t idMod,                             /**< module id */
    uint32_t idVar,                             /**< variable id */
    GOAL_CM_VAR_T *pVar,                        /**< variable pointer */
    uint32_t *pSizeNew                          /**< new size ref */
)
{
#if GOAL_CONFIG_LOGGING == 1
    char *strName = " ";                        /* variable name */
#endif

    UNUSEDARG(idMod);
    UNUSEDARG(idVar);
    UNUSEDARG(pVar);

    /* set variable size to zero */
    *pSizeNew = 0;

    /* clear variable content (needed for IPv4 addresses and fixed-size numbers) */
    GOAL_MEMSET((char *) pVar + sizeof(GOAL_CM_VAR_T), 0, pVar->totalSize);

#if (GOAL_CONFIG_LOGGING == 1) && (GOAL_CM_NAMES == 1)
    if (GOAL_RES_ERR(goal_cmGetVarNameById(idMod, idVar, &strName))) {
        strName = GOAL_CM_STR_NOT_FOUND;
    }
#endif

    /* GG_LOG ID:107: */
    goal_lmLog(GOAL_ID_CM, 107, 78, 4, 4, GOAL_LOG_SEV_INFO, "tmp var cleared: $1, $2");
    goal_lmLogParamUINT32(idMod);
    goal_lmLogParamUINT32(idVar);
    goal_lmLogFinish();

#if GOAL_CONFIG_LOGGING == 1
    /* GG_LOG ID:108: */
    goal_lmLog(GOAL_ID_CM, 108, 79, GOAL_STRLEN((const char *) strName), 0, GOAL_LOG_SEV_INFO, "    : ($1)");
    goal_lmLogParamSTRING0(strName);
    goal_lmLogFinish();
#endif

    return GOAL_OK;
}


/****************************************************************************/
/** Gets the pointer to a module with the given module id
 *
 * @retval GOAL_OK Module found
 * @retval GOAL_ERR_NOT_FOUND Module not found
 */
static GOAL_STATUS_T goal_cmModByIdGet(
    uint32_t modId,                             /**< module id */
    GOAL_CM_MODULE_T **ppMod                    /**< points to module if found */
)
{
    uint32_t idx;                               /* index */

    for (idx = 0; idx < modCnt; idx++) {
        if (modId == cmModulesPool[idx].modId) {
            *ppMod = &cmModulesPool[idx];
            return GOAL_OK;
        }
    }

    return GOAL_ERR_NOT_FOUND;
}


/****************************************************************************/
/** Gets the pointer to a free module entry in the module pool
 *
 * @retval GOAL_OK Module found
 * @retval GOAL_ERR_NOT_FOUND Module not found
 */
static GOAL_STATUS_T goal_cmFreeModEntryGet(
    GOAL_CM_MODULE_T **ppMod                    /**< points to module if found */
)
{
    uint32_t idx;                               /* index */

    for (idx = 0; idx < modCnt; idx++) {
        if (0 == cmModulesPool[idx].varCnt) {
            *ppMod = &cmModulesPool[idx];
            return GOAL_OK;
        }
    }

    return GOAL_ERR_NOT_FOUND;
}


#if GOAL_CM_NAMES == 1
/****************************************************************************/
/** Gets a pointer to the variable name of the var identified by module and
 *  var id
 *
 * @retval GOAL_OK Var found
 * @retval other failed
 */
GOAL_STATUS_T goal_cmGetVarNameById(
    uint32_t modId,                             /**< Module id of the var to get */
    uint32_t varId,                             /**< Var id of the var to get */
    char **ppName                               /**< Points to var name after completion */
)
{
    GOAL_STATUS_T res;                          /* Result */
    GOAL_CM_VARENTRY_T *pVar;                   /* Varentry pointer */

    *ppName = NULL;
    res = goal_cmGetVarEntryById(modId, varId, &pVar);
    if (GOAL_RES_OK(res)) {
        *ppName = pVar->varname;
    }
    return res;
}


/****************************************************************************/
/** Gets a pointer to the variable identified by the given module Id and name
 *
 * @retval GOAL_OK Var found
 * @retval other failed
 */
GOAL_STATUS_T goal_cmGetVarByName(
    uint32_t modId,                             /**< Module id of the var to get */
    char *pName,                                /**< Name of the var to get */
    uint16_t len,                               /**< Length of var name */
    GOAL_CM_VAR_T **ppVar                       /**< Pointer to var entry after completion */
)
{
    GOAL_STATUS_T res = GOAL_ERR_PARAM;         /* Result */
    GOAL_CM_VARENTRY_T *pVarEntry;              /* Varentry pointer */
    GOAL_CM_MODULE_T *pMod;                     /* Module pointer */

    *ppVar = NULL;

    /* Get pointer to module */
    res = goal_cmModByIdGet(modId, &pMod);
    if (GOAL_RES_ERR(res)) {
        /* virtual CM Vars don't have a name */
        return res;
    }

    if (modId <= (modCnt - 1)) {
        pVarEntry = pMod->var;
        while (pVarEntry->varId != GOAL_VM_VAR_EOL) {
            if (GOAL_STRLEN(pVarEntry->varname) == len) {
                if (0 == GOAL_MEMCMP(pVarEntry->varname, pName, len)) {
                    *ppVar = pVarEntry->var;
                    res = GOAL_OK;
                    break;
                }
            }
            pVarEntry++;
        }
    }
    return res;
}


/****************************************************************************/
/** Gets a pointer to the module name identified by module id
 *
 * @retval GOAL_OK Module found
 * @retval other failed
 */
GOAL_STATUS_T goal_cmGetModNameById(
    uint32_t modId,                             /**< Module id */
    char **ppName                               /**< Points to module name after completion */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* Result */
    GOAL_CM_MODULE_T *pMod;                     /* Module pointer */

    /* Get pointer to module */
    res = goal_cmModByIdGet(modId, &pMod);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    if (modId > (modCnt - 1)) {
        *ppName = NULL;
        res = GOAL_ERR_PARAM;
    } else {
        *ppName = pMod->modname;
    }
    return res;
}


/****************************************************************************/
/** Gets module id identified by module name
 *
 * @retval GOAL_OK Module found
 * @retval other failed
 */
GOAL_STATUS_T goal_cmGetModIdByName(
    char *pName,                                /**< Name of module to get */
    uint16_t len,                               /**< Length of module name */
    uint32_t *pModId                            /**< Module id after completion */
)
{
    GOAL_STATUS_T res = GOAL_ERR_PARAM;         /* Result */
    uint32_t idx;                               /* Index */

    for (idx = 0; idx < modCnt; idx++) {
        if (GOAL_STRLEN(cmModulesPool[idx].modname) == len) {
            if (0 == GOAL_MEMCMP(cmModulesPool[idx].modname, pName, len)) {
                *pModId = cmModulesPool[idx].modId;
                res = GOAL_OK;
                break;
            }
        }
    }
    return res;
}
#endif /* GOAL_CM_NAMES */


/****************************************************************************/
/** get number of variables from CM
 *
 * @returns total number of variables
 */
uint32_t goal_cmGetVariablesCnt(
    void
)
{
    GOAL_CM_VARENTRY_T *pVar;                   /* variable entry */
    uint32_t idx;                               /* index */
    uint32_t nrOfVars;                          /* result counter */

    nrOfVars = 0;

    for (idx = 0; idx < modCnt; idx++) {
        for (pVar = cmModulesPool[idx].var; pVar->varId != GOAL_VM_VAR_EOL; pVar++) {
            nrOfVars++;
        }
    }

    return nrOfVars;
}


/****************************************************************************/
/** get number of virtual variables from CM
 *
 * @returns total number of virtual variables
 */
uint32_t goal_cmGetVirtualCnt(
    void
)
{
    uint32_t nrOfVars;                          /* result counter */
    GOAL_CM_VIRT_VAR_T *pVirtualEntry;          /* virtual variable */

    nrOfVars = 0;

    GOAL_LL_FOREACH(pCmVirtualVars, pVirtualEntry) {
        nrOfVars++;
    }

    return nrOfVars;
}


/****************************************************************************/
/** get module ID for given index
 *
 * @returns GOAL_STATUS_T result
 *
 */
GOAL_STATUS_T goal_cmGetModuleId(
    uint32_t idx,                               /**< module index */
    uint32_t *pModId                            /**< pointer to resulting module Id */
)
{
    GOAL_STATUS_T res;                          /* return value */

    if (NULL != pModId) {
        if (idx < modCnt) {
            *pModId = cmModulesPool[idx].modId;
            res = GOAL_OK;
        }
        else {
            res = GOAL_ERR_PARAM;
        }
    }
    else {
        res = GOAL_ERR_NULL_POINTER;
    }

    return res;
}


/****************************************************************************/
/** get first var entry for module
 *
 * @returns var entry or NULL pointer
 *
 */
GOAL_CM_VARENTRY_T *goal_cmModuleGetFirstVarEntry(
    uint32_t modId                              /**< module Id */
)
{
    GOAL_CM_VARENTRY_T *pVar;                   /* pointer to var entry */
    uint32_t idx;                               /* index */

    pVar = NULL;

    for (idx = 0; idx < modCnt; idx++) {
        if (cmModulesPool[idx].modId == modId) {
            pVar = cmModulesPool[idx].var;
            continue;
        }
    }

    return pVar;
}


/****************************************************************************/
/** get next variable entry for module
 *
 * @returns found var entry or NULL pointer
 *
 */
GOAL_CM_VARENTRY_T *goal_cmModuleGetNextVarEntry(
    GOAL_CM_VARENTRY_T *pEntry                  /**< pointer to var entry */
)
{
    GOAL_CM_VARENTRY_T *pVar;                   /* resulting pointer to var entry */

    pVar = NULL;

    /* if entry is valid, get next entry */
    if (pEntry != NULL) {
        if (pEntry->varId != GOAL_VM_VAR_EOL) {
            pVar = pEntry + 1;
        }
    }

    return pVar;
}


/****************************************************************************/
/** Retrieve the registered modules count
 *
 * @retval number modules count
 */
uint32_t goal_cmModGetCnt(
    void
)
{
    return modCnt;
}


/****************************************************************************/
/** Saves all variables to permanent storage using the appropriate target
 *  function
 *
 * This function is executed from CM variable access
 *
 * @retval GOAL_OK Values saved
 * @retval other failed
 */
GOAL_STATUS_T goal_cmCmSave(
    uint32_t modId,                             /**< module ID */
    uint32_t varId,                             /**< variable Id */
    GOAL_CM_VAR_T *pVar                         /**< variable pointer */
)
{
    GOAL_STATUS_T res;                          /* return value */

    UNUSEDARG(modId);
    UNUSEDARG(varId);

    /* clear trigger */
    GOAL_CM_SETVAR_UINT8(pVar, 0);

    res = goal_cmSave();

    return res;
}


#if (GOAL_CONFIG_CLI == 1)
/****************************************************************************/
/** Retrieve the module pool raw array
 *
 * @retval pointer modules pool pointer
 */
GOAL_CM_MODULE_T * goal_cmModGetPool(
    void
)
{
    return cmModulesPool;
}
#endif /* GOAL_CONFIG_CLI == 1 */


/****************************************************************************/
/** Register a callback to handle access to virtual variables of a specific
 * module id.
 *
 * @retval other failed
 */
GOAL_STATUS_T goal_cmRegVarVirtual(
    uint32_t modId,                             /**< module ID */
    uint32_t varId,                             /**< variable ID */
    GOAL_CM_DATATYPE_T type,                    /**< variable type */
    uint32_t sizeMax,                           /**< maximum size */
    goal_cm_validate validate,                  /**< validate callback */
    goal_cm_changed changed                     /**< change callback */
)
{
    GOAL_CM_VIRT_VAR_T *pEntry;                 /* list entry */
    GOAL_STATUS_T res;                          /* result */
    GOAL_CM_VAR_T *pVar = NULL;                 /* virtual var */
    GOAL_CM_VARENTRY_T *pVarEntry = NULL;       /* virtual var entry */

    /* allocate a list entry */
    res = goal_memCalloc(&pEntry, sizeof(GOAL_CM_VIRT_VAR_T));

    /* allocate cmVar */
    if (GOAL_RES_OK(res)) {
        res = goal_memCalloc(&pVar, sizeof(GOAL_CM_VAR_T) + sizeMax);
    }

    /* populate cmVar */
    if (GOAL_RES_OK(res)) {
        pVar->modId = modId;
        pVar->varId = varId;
        pVar->flags = 0;
        pVar->type = type;
        pVar->curSize = sizeMax;
        pVar->totalSize = sizeMax;
    }

    /* allocate cmVarEntry */
    if (GOAL_RES_OK(res)) {
        res = goal_memCalloc(&pVarEntry, sizeof(GOAL_CM_VARENTRY_T));
    }

    /* populate var entry */
    if (GOAL_RES_OK(res)) {
        pVarEntry->varId = varId;
        pVarEntry->type = type;
        pVarEntry->var = pVar;
        pVarEntry->maxSize = sizeMax;
        pVarEntry->validate = validate;
        pVarEntry->changed = changed;
        pVarEntry->cb_first = NULL;
        pVarEntry->cb_last = NULL;
    }

    /* link variable entry to virtual variable */
    if (GOAL_RES_OK(res)) {
        pEntry->pVarEntry = pVarEntry;
    }

    /* add to list */
    if (GOAL_RES_OK(res)) {
        GOAL_LL_APPEND(pCmVirtualVars, pEntry);
    }

    return res;
}


/****************************************************************************/
/** get first virtual var entry for module
 *
 * @returns var entry or NULL pointer
 *
 */
GOAL_CM_VARENTRY_T *goal_cmModuleGetFirstVirtualVarEntry(
    uint32_t modId                              /**< module Id */
)
{
    GOAL_CM_VARENTRY_T *pVar;                   /* pointer to var entry */
    GOAL_CM_VIRT_VAR_T *pVirtualEntry;          /* virtual variable */

    pVar = NULL;

    GOAL_LL_FOREACH(pCmVirtualVars, pVirtualEntry) {
        if (pVirtualEntry->pVarEntry->var->modId == modId) {
            pVar = pVirtualEntry->pVarEntry;
            break;
        }
    }

    return pVar;
}


/****************************************************************************/
/** get next virtual variable entry for module
 *
 * @returns found var entry or NULL pointer
 *
 */
GOAL_CM_VARENTRY_T *goal_cmModuleGetNextVirtualVarEntry(
    GOAL_CM_VARENTRY_T *pEntry                  /**< pointer to var entry */
)
{
    GOAL_CM_VARENTRY_T *pVar;                   /* variable entry */
    GOAL_CM_VIRT_VAR_T *pVirtualEntry;          /* virtual variable */
    GOAL_CM_VIRT_VAR_T *pCurrent;               /* virtual variable */

    pVar = NULL;
    pCurrent = NULL;

    /* search current virtual variable */
    GOAL_LL_FOREACH(pCmVirtualVars, pVirtualEntry) {
        if (pVirtualEntry->pVarEntry == pEntry) {
            /* set current element */
            pCurrent = pVirtualEntry;
        }
        else {
            /* search next */
            if (NULL != pCurrent) {
                /* if modId equals modId of current, next is found */
                if (pVirtualEntry->pVarEntry->var->modId == pCurrent->pVarEntry->var->modId) {
                    pVar = pVirtualEntry->pVarEntry;
                    break;
                }
            }
        }
    }

    return pVar;
}
