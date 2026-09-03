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

#ifndef GOAL_CM_H
#define GOAL_CM_H


/****************************************************************************/
/* Constants */
/****************************************************************************/
#define GOAL_CM_HEADER_OFFSET sizeof(GOAL_CM_HEADER_T) /**< Size of buffer offset */
#define GOAL_CM_MAGIC "port"                    /**< Magic of file header */
#define GOAL_CM_MAGIC_LEN 4                     /**< Length of magic */
#define GOAL_CM_VERSION 0x1001                  /**< Version of storage */
#define GOAL_CM_MAX_NAME_LEN 128                /**< Max name length */


/****************************************************************************/
/* Macros */
/****************************************************************************/
#define GOAL_CM_VAR_UINT8(x)    (*((uint8_t *) ((char *) x + sizeof(GOAL_CM_VAR_T))))
#define GOAL_CM_VAR_UINT16(x)   (*((uint16_t *) ((char *) x + sizeof(GOAL_CM_VAR_T))))
#define GOAL_CM_VAR_UINT32(x)   (*((uint32_t *) ((char *) x + sizeof(GOAL_CM_VAR_T))))
#define GOAL_CM_VAR_INT8(x)     (*((int8_t *) ((char *) x + sizeof(GOAL_CM_VAR_T))))
#define GOAL_CM_VAR_INT16(x)    (*((int16_t *) ((char *) x + sizeof(GOAL_CM_VAR_T))))
#define GOAL_CM_VAR_INT32(x)    (*((int32_t *) ((char *) x + sizeof(GOAL_CM_VAR_T))))

#define GOAL_CM_VAR_STRING(x)   (((char *) x + sizeof(GOAL_CM_VAR_T)))

#define GOAL_CM_GETVAR_UINT8(x, y) \
    if (NULL == x) { \
        goal_targetHalt(); \
    } else { \
        goal_cmLockVar(x->modId, x->varId); \
        y = GOAL_CM_VAR_UINT8(x); \
        goal_cmUnlockVar(x->modId, x->varId); \
    }

#define GOAL_CM_GETVAR_UINT16(x, y) \
    if (NULL == x) { \
        goal_targetHalt(); \
    } else { \
        goal_cmLockVar(x->modId, x->varId); \
        y = GOAL_CM_VAR_UINT16(x); \
        goal_cmUnlockVar(x->modId, x->varId); \
    }

#define GOAL_CM_GETVAR_UINT32(x, y) \
    if (NULL == x) { \
        goal_targetHalt(); \
    } else { \
        goal_cmLockVar(x->modId, x->varId); \
        y = GOAL_CM_VAR_UINT32(x); \
        goal_cmUnlockVar(x->modId, x->varId); \
    }

#define GOAL_CM_GETVAR_GENERIC(x, y) \
    if (NULL == x) { \
        goal_targetHalt(); \
    } else { \
        goal_cmLockVar(x->modId, x->varId); \
        GOAL_MEMCPY(y, ((char *) x + sizeof(GOAL_CM_VAR_T)), x->curSize); \
        goal_cmUnlockVar(x->modId, x->varId); \
    }

#define GOAL_CM_GETVARLEN_GENERIC(x, y, z) \
    if (NULL == x) { \
        goal_targetHalt(); \
    } else { \
        goal_cmLockVar(x->modId, x->varId); \
        GOAL_MEMCPY(y, ((char *) x + sizeof(GOAL_CM_VAR_T)), x->curSize); \
        z = x->curSize; \
        goal_cmUnlockVar(x->modId, x->varId); \
    }

#define GOAL_CM_GETVARLEN_CURR(x, y) \
    if (NULL == x) { \
        goal_targetHalt(); \
    } else { \
        goal_cmLockVar(x->modId, x->varId); \
        y = x->curSize; \
        goal_cmUnlockVar(x->modId, x->varId); \
    }

#define GOAL_CM_GETVARLEN_MAX(x, y) \
    if (NULL == x) { \
        goal_targetHalt(); \
    } else { \
        goal_cmLockVar(x->modId, x->varId); \
        y = x->totalSize; \
        goal_cmUnlockVar(x->modId, x->varId); \
    }

#define GOAL_CM_SETVAR_UINT8(x, y) \
    if (NULL == x) { \
        goal_targetHalt(); \
    } else { \
        goal_cmLockVar(x->modId, x->varId); \
        if (GOAL_CM_VAR_UINT8(x) != y) { \
            x->flags |= GOAL_CM_FLAGS_MODIFIED; \
            GOAL_CM_VAR_UINT8(x) = y; \
        } \
        x->curSize = sizeof(uint8_t); \
        goal_cmUnlockVar(x->modId, x->varId); \
    }

#define GOAL_CM_SETVAR_INT8(x, y) \
    if (NULL == x) { \
        goal_targetHalt(); \
    } else { \
        goal_cmLockVar(x->modId, x->varId); \
        if (GOAL_CM_VAR_INT8(x) != y) { \
            x->flags |= GOAL_CM_FLAGS_MODIFIED; \
            GOAL_CM_VAR_INT8(x) = y; \
        } \
        x->curSize = sizeof(int8_t); \
        goal_cmUnlockVar(x->modId, x->varId); \
    }

#define GOAL_CM_SETVAR_UINT16(x, y) \
    if (NULL == x) { \
        goal_targetHalt(); \
    } else { \
        goal_cmLockVar(x->modId, x->varId); \
        GOAL_ASSERT(x->totalSize >= sizeof(uint16_t)); \
        if (GOAL_CM_VAR_UINT16(x) != y) { \
            x->flags |= GOAL_CM_FLAGS_MODIFIED; \
            GOAL_CM_VAR_UINT16(x)= y; \
        } \
        x->curSize = sizeof(uint16_t); \
        goal_cmUnlockVar(x->modId, x->varId); \
    }

#define GOAL_CM_SETVAR_INT16(x, y) \
    if (NULL == x) { \
        goal_targetHalt(); \
    } else { \
        goal_cmLockVar(x->modId, x->varId); \
        GOAL_ASSERT(x->totalSize >= sizeof(int16_t)); \
        if (GOAL_CM_VAR_INT16(x) != y) { \
            x->flags |= GOAL_CM_FLAGS_MODIFIED; \
            GOAL_CM_VAR_INT16(x)= y; \
        } \
        x->curSize = sizeof(int16_t); \
        goal_cmUnlockVar(x->modId, x->varId); \
    }

#define GOAL_CM_SETVAR_UINT32(x, y) \
    if (NULL == x) { \
        goal_targetHalt(); \
    } else { \
        goal_cmLockVar(x->modId, x->varId); \
        GOAL_ASSERT(x->totalSize >= sizeof(uint32_t)); \
        if (GOAL_CM_VAR_UINT32(x) != y) { \
            x->flags |= GOAL_CM_FLAGS_MODIFIED; \
            GOAL_CM_VAR_UINT32(x) = y; \
        } \
        x->curSize = sizeof(uint32_t); \
        goal_cmUnlockVar(x->modId, x->varId); \
    }

#define GOAL_CM_SETVAR_INT32(x, y) \
    if (NULL == x) { \
        goal_targetHalt(); \
    } else { \
        goal_cmLockVar(x->modId, x->varId); \
        GOAL_ASSERT(x->totalSize >= sizeof(int32_t)); \
        if (GOAL_CM_VAR_INT32(x) != y) { \
            x->flags |= GOAL_CM_FLAGS_MODIFIED; \
            GOAL_CM_VAR_INT32(x) = y; \
        } \
        x->curSize = sizeof(int32_t); \
        goal_cmUnlockVar(x->modId, x->varId); \
    }

#define GOAL_CM_SETVAR_GENERIC(x, y, z) \
    if (NULL == x) { \
        goal_targetHalt(); \
    } else { \
        goal_cmLockVar(x->modId, x->varId); \
        GOAL_ASSERT(x->totalSize >= z); \
        if ((x->curSize != z) || (0 != GOAL_MEMCMP(((char *) x + sizeof(GOAL_CM_VAR_T)), y, z))) { \
            x->flags |= GOAL_CM_FLAGS_MODIFIED; \
            GOAL_MEMCPY(((char *) x + sizeof(GOAL_CM_VAR_T)), y, z); \
            x->curSize = z; \
        } \
        goal_cmUnlockVar(x->modId, x->varId); \
    }

#define GOAL_CM_CLEARVAR_GENERIC(x, y, z) \
    if (NULL == x) { \
        goal_targetHalt(); \
    } else { \
        goal_cmLockVar(x->modId, x->varId); \
        goal_cmClearValPat(x, y); \
        if (z) { \
            x->curSize = x->totalSize; \
        } \
        x->flags |= GOAL_CM_FLAGS_MODIFIED; \
        goal_cmUnlockVar(x->modId, x->varId); \
    }

#define GOAL_CM_SETFLAG(modId, varId, flag, val) \
    goal_cmLockVar(modId, varId); \
    goal_cmSetVarFlag(modId, varId, flag, val); \
    goal_cmUnlockVar(modId, varId);

/* Variables list */
#define GOAL_CM_VARLIST(x, ...) \
static GOAL_CM_VARENTRY_T x[] = { __VA_ARGS__, GOAL_CM_VAR_END }

/* Public variables list */
#define GOAL_CM_VARLIST_EXT(x, ...) \
GOAL_CM_VARENTRY_T x[] = { __VA_ARGS__, GOAL_CM_VAR_END }

/* End of list marker */
#define GOAL_VM_VAR_EOL 0xFFFFFFFF

/* VAR IDs list */
#define GOAL_CM_VAR_IDS(x, y) \
enum x { \
    y \
}

/* Config Manager Id */
#define GOAL_CM_ID(x, y, z, a)  ((uint32_t) ((x << 24) | (y << 16) | (z << 8) | a))

#if GOAL_CM_NAMES == 1
#  define GOAL_CM_VARNAME       char *varname;
#  define GOAL_CM_MODNAME       char modname[GOAL_CM_MAX_NAME_LEN];
#  define GOAL_CM_VAR_END       { GOAL_VM_VAR_EOL, GOAL_CM_NONE, 0, 0, 0, 0, 0, 0, 0 }
#  define GOAL_CM_MOD(x)        x, #x
#  define GOAL_CM_PARAM         uint32_t modId; char *modName;
#  define GOAL_CM_MODDEF(x, y)  { x, y }
#else
#  define GOAL_CM_VARNAME
#  define GOAL_CM_VAR_END       { GOAL_VM_VAR_EOL, GOAL_CM_NONE, 0, 0, 0, 0, 0, 0 }
#  define GOAL_CM_MOD(x)        x
#  define GOAL_CM_PARAM         uint32_t modId;
#  define GOAL_CM_MODNAME
#  define GOAL_CM_MODDEF(x, y)  { x }
#endif

#define GOAL_CM_FLAGS_MODIFIED  (1 << 0)
#define GOAL_CM_FLAGS_TEMP      (1 << 1)
#define GOAL_CM_FLAGS_LOCK      (1 << 2)
#define GOAL_CM_FLAGS_ARCHIVE   (1 << 3)


/****************************************************************************/
/* Data types */
/****************************************************************************/
typedef enum goal_cm_datatypes {
    GOAL_CM_NONE = 0,
    GOAL_CM_UINT8 = 1,
    GOAL_CM_UINT16,
    GOAL_CM_UINT32,
    GOAL_CM_INT8,
    GOAL_CM_INT16,
    GOAL_CM_INT32,
    GOAL_CM_IPV4,
    GOAL_CM_STRING,
    GOAL_CM_GENERIC
} GOAL_CM_DATATYPE_T;


/****************************************************************************/
/* Structures */
/****************************************************************************/
GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct {
    uint32_t modId;                             /**< Module ID */
    uint32_t varId;                             /**< Variable ID */
    uint32_t flags;                             /**< Flags */
    uint32_t type;                              /**< Data type */
    uint32_t curSize;                           /**< Current data size */
    uint32_t totalSize;                         /**< Max. size */
} GOAL_TARGET_PACKED GOAL_CM_VAR_T;
GOAL_TARGET_PACKED_STRUCT_POST

typedef GOAL_STATUS_T (* goal_cm_validate)(uint32_t modId, uint32_t varId, GOAL_CM_VAR_T *var, void *newData, uint32_t size);
typedef GOAL_STATUS_T (* goal_cm_changed)(uint32_t modId, uint32_t varId, GOAL_CM_VAR_T *var);
typedef GOAL_STATUS_T (* goal_cm_save)(uint32_t modId, uint32_t varId, GOAL_CM_VAR_T *var);
typedef GOAL_STATUS_T (* goal_cm_load)(uint32_t modId, uint32_t varId, GOAL_CM_VAR_T *var, uint32_t *size);
typedef GOAL_STATUS_T (* goal_cm_tmpset)(uint32_t modId, uint32_t varId, GOAL_CM_VAR_T *var, uint32_t *newSize);

typedef struct goal_cm_cb {
    goal_cm_changed changed;
    struct goal_cm_cb *next;
} GOAL_CM_CB_T;

typedef struct {
    uint32_t varId;
    GOAL_CM_DATATYPE_T type;
    uint32_t maxSize;
    GOAL_CM_VAR_T *var;
    goal_cm_validate validate;
    goal_cm_changed changed;
    GOAL_CM_CB_T *cb_first;
    GOAL_CM_CB_T *cb_last;
    GOAL_CM_VARNAME
} GOAL_CM_VARENTRY_T;

typedef struct {
    uint32_t modId;
    uint32_t varCnt;
    goal_cm_save save;
    goal_cm_load load;
    goal_cm_tmpset tmpset;
    GOAL_CM_MODNAME
    GOAL_CM_VARENTRY_T *var;
} GOAL_CM_MODULE_T;

GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct {
    char magic[GOAL_CM_MAGIC_LEN];
    uint32_t version;
    uint32_t size;
    uint32_t chksum;
} GOAL_TARGET_PACKED GOAL_CM_HEADER_T;
GOAL_TARGET_PACKED_STRUCT_POST

typedef struct {
    GOAL_CM_PARAM
} GOAL_CM_MODDEF_T;

typedef struct GOAL_CM_VIRT_VAR_T {
    struct GOAL_CM_VIRT_VAR_T *pNext;           /**< next pointer */
    GOAL_CM_VARENTRY_T *pVarEntry;              /**< variable entry */
} GOAL_CM_VIRT_VAR_T;


/****************************************************************************/
/* Function mapping */
/****************************************************************************/

#if GOAL_CONFIG_MEDIA_MI_CMFS
# define goal_cmSave goal_cmSaveCmfs
# define goal_cmVarIsDefault(modId) goal_miCmfsIsDefault(modId)
#else
# define goal_cmSave goal_cmSaveImpl
# define goal_cmVarIsDefault(modId) goal_cmVarIsDefaultImpl(modId)
#endif


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_cmInitPre(
    void
);

GOAL_STATUS_T goal_cmRegModule(
    GOAL_CM_VARENTRY_T *pVarTable               /**< Pointer to variable table */
);

uint32_t goal_cmGetConfigSize(
    void
);

GOAL_STATUS_T goal_cmAddModule(
    GOAL_CM_MODDEF_T *pModdef,                  /**< The module to register */
    GOAL_CM_VARENTRY_T *pVarTable,              /**< Pointer to variable table */
    goal_cm_load load,                          /**< Pointer to load callback */
    goal_cm_save save,                          /**< Pointer to save callback */
    goal_cm_tmpset tmpset                       /**< Pointer to temp value callback */
);

GOAL_STATUS_T goal_cmGetVarById(
    uint32_t modId,                             /**< Module ID of the var to search for */
    uint32_t varId,                             /**< Variable ID of the var to search for */
    GOAL_CM_VAR_T **ppVar                       /**< Pointer to the var to search for */
);

GOAL_STATUS_T goal_cmGetVarEntryById(
    uint32_t modId,                             /**< Module ID of the var to search for */
    uint32_t varId,                             /**< Variable ID of the var to search for */
    GOAL_CM_VARENTRY_T **ppVar                  /**< Pointer to the var to search for */
);

GOAL_STATUS_T goal_cmSetVarValue(
    uint32_t modId,                             /**< Module ID of the var */
    uint32_t varId,                             /**< Variable ID of the var */
    void *pData,                                /**< Pointer to the value to set */
    uint32_t size,                              /**< Size of the data to set */
    GOAL_BOOL_T temp,                           /**< If true, the value is marked as temporary */
    goal_cm_changed exclude                     /**< If not NULL, the given changed callback is not invoked */
);

GOAL_STATUS_T goal_cmTriggerChange(
    uint32_t modId,                             /**< Module ID of the var */
    uint32_t varId,                             /**< Variable ID of the var */
    goal_cm_changed exclude                     /**< If not NULL, the given callback is not invoked */
);

GOAL_STATUS_T goal_cmSetVarFlag(
    uint32_t modId,                             /**< Module ID of the var */
    uint32_t varId,                             /**< Variable ID of the var */
    uint32_t flag,                              /**< The flag(s) to set */
    GOAL_BOOL_T enable                          /**< If true, flag is set, otherwise flag is deleted */
);

GOAL_STATUS_T goal_cmGetVarFlag(
    uint32_t modId,                             /**< Module ID of the var */
    uint32_t varId,                             /**< Variable ID of the var */
    uint32_t flag,                              /**< The flag(s) to set */
    GOAL_BOOL_T *pEnabled                       /**< If true, flag is set, otherwise flag is not set */
);

GOAL_STATUS_T goal_cmSaveImpl(
    void
);

GOAL_STATUS_T goal_cmSaveCmfs(
    void
);

GOAL_STATUS_T goal_cmLoad(
    void
);

GOAL_STATUS_T goal_cmAddCallback(
    uint32_t modId,                             /**< Module ID of the var */
    uint32_t varId,                             /**< Variable ID of the var */
    goal_cm_changed change                      /**< Function pointer to callback */
);

GOAL_STATUS_T goal_cmLockVar(
    uint32_t modId,                             /**< Module ID of the var */
    uint32_t varId                              /**< Variable ID of the var */
);

GOAL_STATUS_T goal_cmLockVarEntry(
    GOAL_CM_VARENTRY_T *pVar                    /**< Pointer to var struct */
);

GOAL_STATUS_T goal_cmUnlockVarEntry(
    GOAL_CM_VARENTRY_T *pVar                    /**< Pointer to var struct */
);

GOAL_STATUS_T goal_cmUnlockVar(
    uint32_t modId,                             /**< Module ID of the var */
    uint32_t varId                              /**< Variable ID of the var */
);

GOAL_STATUS_T goal_cmLoadCustom(
    void
);

GOAL_STATUS_T goal_cmSaveCustom(
    void
);

GOAL_STATUS_T goal_cmCompare(
    GOAL_CM_VAR_T *pVar,                        /**< Pointer to variable to compare with */
    const char *pData,                          /**< Pointer to data to compare with */
    unsigned int len,                           /**< Length of data to compare */
    GOAL_BOOL_T *pMatch                         /**< After execution, true if data matches, otherwise false */
);

GOAL_STATUS_T goal_cmClearVal(
    GOAL_CM_VAR_T *pVar                         /**< Pointer to variable to clear */
);

GOAL_STATUS_T goal_cmClearValPat(
    GOAL_CM_VAR_T *pVar,                        /**< Pointer to variable to clear */
    char pattern                                /**< The pointer to use for clearing */
);

GOAL_STATUS_T goal_cmPrintVarList(
    void
);

GOAL_BOOL_T goal_miCmfsIsDefault(
    uint32_t modId                              /**< module id */
);

GOAL_BOOL_T goal_cmVarIsDefaultImpl(
    uint32_t modId                              /**< module id */
);

#if GOAL_CM_NAMES == 1
GOAL_STATUS_T goal_cmGetVarNameById(
    uint32_t modId,                             /**< Module id of the var to get */
    uint32_t varId,                             /**< Var id of the var to get */
    char **ppName                               /**< Points to var name after completion */
);

GOAL_STATUS_T goal_cmGetVarByName(
    uint32_t modId,                             /**< Module id of the var to get */
    char *pName,                                /**< Name of the var to get */
    uint16_t len,                               /**< Length of var name */
    GOAL_CM_VAR_T **ppVar                       /**< Pointer to var entry after completion */
);

GOAL_STATUS_T goal_cmGetModNameById(
    uint32_t modId,                             /**< Module id */
    char **ppName                               /**< Points to module name after completion */
);

GOAL_STATUS_T goal_cmGetModIdByName(
    char *pName,                                /**< Name of module to get */
    uint16_t len,                               /**< Lenght of module name */
    uint32_t *pModId                            /**< Module id after completion */
);
#endif /* GOAL_CM_NAMES */

uint32_t goal_cmGetVariablesCnt(
    void
);

uint32_t goal_cmGetVirtualCnt(
    void
);

GOAL_STATUS_T goal_cmCliReg(
    void
);

GOAL_STATUS_T goal_cmGetModuleId(
    uint32_t idx,                               /**< module index */
    uint32_t *pModId                            /**< pointer to resulting module Id */
);

GOAL_CM_VARENTRY_T *goal_cmModuleGetFirstVarEntry(
    uint32_t modId                              /**< module Id */
);

GOAL_CM_VARENTRY_T *goal_cmModuleGetNextVarEntry(
    GOAL_CM_VARENTRY_T *pEntry                  /**< pointer to var entry */
);

GOAL_CM_VARENTRY_T *goal_cmModuleGetFirstVirtualVarEntry(
    uint32_t modId                              /**< module Id */
);

GOAL_CM_VARENTRY_T *goal_cmModuleGetNextVirtualVarEntry(
    GOAL_CM_VARENTRY_T *pEntry                  /**< pointer to var entry */
);

uint32_t goal_cmModGetCnt(
    void
);

GOAL_CM_MODULE_T * goal_cmModGetPool(
    void
);

GOAL_STATUS_T goal_cmCmSave(
    uint32_t modId,                             /**< module ID */
    uint32_t varId,                             /**< variable Id */
    GOAL_CM_VAR_T *pVar                         /**< variable pointer */
);

GOAL_STATUS_T goal_cmRegVarVirtual(
    uint32_t modId,                             /**< module ID */
    uint32_t varId,                             /**< variable ID */
    GOAL_CM_DATATYPE_T type,                    /**< variable type */
    uint32_t sizeMax,                           /**< maximum size */
    goal_cm_validate validate,                  /**< validate callback */
    goal_cm_changed changed                     /**< change callback */
);

#endif /* GOAL_CM_H */
