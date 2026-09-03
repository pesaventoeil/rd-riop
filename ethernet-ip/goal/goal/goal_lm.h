/** @file
 *
 * @brief Message Logger
 *
 * This module implements logging functions.
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

#ifndef GOAL_LM_H
#define GOAL_LM_H

#include "goal_includes.h"


/****************************************************************************/
/* Config Defaults */
/****************************************************************************/

#ifndef GOAL_LM_BUFFER_SIZE
# define GOAL_LM_BUFFER_SIZE (5 * 1024)         /**< default log buffer size */
#endif /* GOAL_LM_BUFFER_SIZE */

#ifndef GOAL_LM_BUFFER_CM_SIZE
# define GOAL_LM_BUFFER_CM_SIZE 128             /**< default CM logger buffer */
#endif /* GOAL_LM_BUFFER_CM_SIZE */


/****************************************************************************/
/* Defines */
/****************************************************************************/

#define GOAL_LOG_SEV_DEBUG      (1 << 0)
#define GOAL_LOG_SEV_INFO       (1 << 1)
#define GOAL_LOG_SEV_WARNING    (1 << 2)
#define GOAL_LOG_SEV_ERROR      (1 << 3)
#define GOAL_LOG_SEV_EXCEPTION  (1 << 4)

/* logging functions */
#if GOAL_CONFIG_LOGGING == 0

#  define goal_lmLog(...) do { } while (0)
#  define goal_lmLogFinish(...) do { } while (0)
#  define goal_lmLogParamINT8(...) do { } while (0)
#  define goal_lmLogParamUINT8(...) do { } while (0)
#  define goal_lmLogParamINT16(...) do { } while (0)
#  define goal_lmLogParamUINT16(...) do { } while (0)
#  define goal_lmLogParamINT32(...) do { } while (0)
#  define goal_lmLogParamUINT32(...) do { } while (0)
#  define goal_lmLogParamIPV4(...) do { } while (0)
#  define goal_lmLogParamMAC(...) do { } while (0)
#  define goal_lmLogParamPTR(...) do { } while (0)
#  define goal_lmLogParamPTRDIFF(...) do { } while (0)
#  define goal_lmLogParamBOOLEAN(...) do { } while (0)
#  define goal_lmLogParamSTRING0(...) do { } while (0)
#  define goal_lmLogParamGENERIC(...) do { } while (0)
#  define goal_lmLogParamINT64(...) do { } while (0)
#  define goal_lmLogParamUINT64(...) do { } while (0)
#  define goal_lmLogParamFLOAT(...) do { } while (0)

#else /* GOAL_CONFIG_LOGGING == 0 */

#  define goal_lmLog(mid, lid, tid, p1len, p2len, sev, text) \
   if (GOAL_TRUE == goal_logModuleIsEnabled(GOAL_ID)) \
    { \
        goal_lmLogInternal(lid, (uint16_t) p1len, (uint16_t) p2len, __LINE__, __func__); \
    }

#  define goal_lmLogFinish() \
    if (GOAL_TRUE == goal_logModuleIsEnabled(GOAL_ID)) \
    { \
        { \
            goal_lmLogPad(); \
            goal_lmLogFinishInternal(); \
        } \
    }

#  define goal_lmLogParamINT8(param) \
    if (GOAL_TRUE == goal_logModuleIsEnabled(GOAL_ID)) \
    { \
        goal_lmLogParamINT8Impl(param); \
    }

#  define goal_lmLogParamUINT8(param) \
    if (GOAL_TRUE == goal_logModuleIsEnabled(GOAL_ID)) \
    { \
        goal_lmLogParamUINT8Impl(param); \
    }

#  define goal_lmLogParamINT16(param) \
    if (GOAL_TRUE == goal_logModuleIsEnabled(GOAL_ID)) \
    { \
        goal_lmLogParamINT16Impl(param); \
    }

#  define goal_lmLogParamUINT16(param) \
    if (GOAL_TRUE == goal_logModuleIsEnabled(GOAL_ID)) \
    { \
        goal_lmLogParamUINT16Impl(param); \
    }

#  define goal_lmLogParamINT32(param) \
    if (GOAL_TRUE == goal_logModuleIsEnabled(GOAL_ID)) \
    { \
        goal_lmLogParamINT32Impl(param); \
    }

#  define goal_lmLogParamUINT32(param) \
    if (GOAL_TRUE == goal_logModuleIsEnabled(GOAL_ID)) \
    { \
        goal_lmLogParamUINT32Impl(param); \
    }

#  define goal_lmLogParamIPV4(param) \
    if (GOAL_TRUE == goal_logModuleIsEnabled(GOAL_ID)) \
    { \
        goal_lmLogParamIPV4Impl(param); \
    }

#  define goal_lmLogParamMAC(param) \
    if (GOAL_TRUE == goal_logModuleIsEnabled(GOAL_ID)) \
    { \
        goal_lmLogParamMACImpl(param); \
    }

#  define goal_lmLogParamPTR(param) \
    if (GOAL_TRUE == goal_logModuleIsEnabled(GOAL_ID)) \
    { \
        goal_lmLogParamPTRImpl(param); \
    }

#  define goal_lmLogParamPTRDIFF(param) \
    if (GOAL_TRUE == goal_logModuleIsEnabled(GOAL_ID)) \
    { \
        goal_lmLogParamPTRDIFFImpl(param); \
    }

#  define goal_lmLogParamBOOLEAN(param) \
    if (GOAL_TRUE == goal_logModuleIsEnabled(GOAL_ID)) \
    { \
        goal_lmLogParamBOOLEANImpl(param); \
    }

#  define goal_lmLogParamSTRING0(param) \
    if (GOAL_TRUE == goal_logModuleIsEnabled(GOAL_ID)) \
    { \
        goal_lmLogParamSTRING0Impl(param); \
    }

#  define goal_lmLogParamGENERIC(param) \
    if (GOAL_TRUE == goal_logModuleIsEnabled(GOAL_ID)) \
    { \
        goal_lmLogParamGENERICImpl(param); \
    }

#  define goal_lmLogParamINT64(param) \
    if (GOAL_TRUE == goal_logModuleIsEnabled(GOAL_ID)) \
    { \
        goal_lmLogParamINT64Impl(param); \
    }

#  define goal_lmLogParamUINT64(param) \
    if (GOAL_TRUE == goal_logModuleIsEnabled(GOAL_ID)) \
    { \
        goal_lmLogParamUINT64Impl(param); \
    }

#  define goal_lmLogParamFLOAT(param) \
    if (GOAL_TRUE == goal_logModuleIsEnabled(GOAL_ID)) \
    { \
        goal_lmLogParamFLOATImpl(param); \
    }

#endif /* GOAL_CONFIG_LOGGING == 0 */

#define GOAL_LM_FLAG_PAD (1)                    /**< message is padded with 1 byte at end */
#define GOAL_LM_FLAG_EXT (2)                    /**< message is from external source */


/****************************************************************************/
/* Type definitions */
/****************************************************************************/

struct GOAL_LM_READER_T;

GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct {
    uint32_t logId;                             /**< log id */
    uint32_t timestamp;                         /**< time stamp */
    uint16_t paramsLength;                      /**< sum of parameter  length */
    uint16_t lineNumber;                        /**< line number in code */
    uint8_t nameLength;                         /**< length of function name */
    uint8_t flags;                              /**< additional flags */
} GOAL_TARGET_PACKED GOAL_LM_LOG_ENTRY_T;
GOAL_TARGET_PACKED_STRUCT_POST

typedef GOAL_STATUS_T (*GOAL_LM_READER_CB) (
    struct GOAL_LM_READER_T *pHdlReader         /**< LM Reader handle */
);

/**< goal lm logging buffer reader */
typedef struct GOAL_LM_READER_T {
    struct GOAL_LM_READER_T *pNext;             /**< next reader */
    GOAL_LM_READER_CB fnctCb;                   /**< reader callback */
    uint16_t readPointer;                       /**< read pointer */
    uint16_t cntReaders;                        /**< number of readers */
} GOAL_LM_READER_T;


/****************************************************************************/
/* Prototypes */
/****************************************************************************/

GOAL_STATUS_T goal_lmInitPre(
    void
);

void goal_lmLogInternal(
    uint32_t logId,                             /**< log id */
    uint16_t param1Length,                      /**< parameter 1 length */
    uint16_t param2Length,                      /**< parameter 2 length */
    uint16_t lineNumber,                        /**< line number in code */
    const char *strNameFunc                     /**< name of calling function */
);

void goal_lmLogFinishInternal(
    void
);

void goal_lmLogParamINT8Impl(
    int8_t parameter                            /**< parameter */
);

void goal_lmLogParamUINT8Impl(
    uint8_t parameter                           /**< parameter */
);

void goal_lmLogParamINT16Impl(
    int16_t parameter                           /**< parameter */
);

void goal_lmLogParamUINT16Impl(
    uint16_t parameter                          /**< parameter */
);

void goal_lmLogParamINT32Impl(
    int32_t parameter                           /**< parameter */
);

void goal_lmLogParamUINT32Impl(
    uint32_t parameter                          /**< parameter */
);

void goal_lmLogParamIPV4Impl(
    uint32_t parameter                          /**< parameter */
);

void goal_lmLogParamMACImpl(
    uint8_t *pParameter                         /**< parameter */
);

void goal_lmLogParamSTRING0Impl(
    const char *pParameter                      /**< parameter */
);

void goal_lmLogParamGENERICImpl(
    const char *pParameter                      /**< parameter */
);

void goal_lmLogParamPTRImpl(
    const char *pParameter                      /**< parameter */
);

void goal_lmLogParamPTRDIFFImpl(
    uint64_t parameter                          /**< parameter */
);

void goal_lmLogParamBOOLEANImpl(
    GOAL_BOOL_T parameter                       /**< parameter */
);

void goal_lmLogParamINT64Impl(
    int64_t parameter                           /**< parameter */
);

void goal_lmLogParamUINT64Impl(
    uint64_t parameter                          /**< parameter */
);

void goal_lmLogParamFLOATImpl(
    float parameter                             /**< parameter */
);

GOAL_STATUS_T LM_onLogAccess(
    uint32_t modId,                             /**< module ID */
    uint32_t varId,                             /**< variable Id */
    GOAL_CM_VAR_T *pVar                         /**< variable pointer */
);

GOAL_STATUS_T LM_exLogAccess(
    uint32_t modId,                             /**< module ID */
    uint32_t varId,                             /**< variable Id */
    GOAL_CM_VAR_T *pVar                         /**< variable pointer */
);

void goal_lmLogLegacy(
    uint8_t level,                              /**< logging level */
    const char *strLog,                         /**< logging string */
    uint16_t len                                /**< logging string length */
);

void goal_lmLogExceptionStart(
    uint8_t level                               /**< logging level */
);

void goal_lmLogExceptionFinish(
    void
);

GOAL_STATUS_T LM_exLogErase(
    uint32_t modId,                             /**< module ID */
    uint32_t varId,                             /**< variable Id */
    GOAL_CM_VAR_T *pVar                         /**< variable pointer */
);

void goal_lmLogPad(
    void
);

GOAL_STATUS_T goal_lmBufferMsgPut(
    uint8_t *pBuffer,                           /**< destination buffer */
    uint16_t msgLen,                            /**< legth of complete msg */
    uint16_t msgLenToWrite,                     /**< length to wirte */
    GOAL_BOOL_T flgLockKeep                     /**< flag if message is finished later */
);

GOAL_STATUS_T goal_lmBufferHead(
    uint16_t *pBufferHead                       /**< resulting buffer head */
);

GOAL_STATUS_T goal_lmReaderReg(
    GOAL_LM_READER_T **pHdlReader,              /**< reader handle */
    GOAL_LM_READER_CB fnctCb                    /**< reader callback */
);

GOAL_STATUS_T goal_lmReaderReset(
    GOAL_LM_READER_T *pHdlReader                /**< reader handle */
);

GOAL_STATUS_T goal_lmReaderPosGet(
    GOAL_LM_READER_T *pHdlReader,               /**< reader handle */
    uint16_t *pReadPointer                      /**< reader read position */
);

GOAL_STATUS_T goal_lmReaderPosSet(
    GOAL_LM_READER_T *pHdlReader,               /**< reader handle */
    uint16_t readPointer                        /**< reader read position */
);

GOAL_STATUS_T goal_lmMsgParamLengths(
    GOAL_LM_LOG_ENTRY_T *pLmEntry,              /**< lm log message */
    uint16_t **ppP1Len,                         /**< param 1 length */
    const char **ppP1Value,                     /**< param 1 value */
    uint16_t **ppP2Len,                         /**< param 1 length */
    const char **ppP2Value                      /**< param 1 value */
);

GOAL_STATUS_T goal_lmBufferMsgGet(
    GOAL_LM_READER_T *pReader,                  /**< reader handle */
    uint8_t *pBuffer,                           /**< destination buffer */
    uint16_t bufferSize,                        /**< destination buffer size */
    uint16_t *pMsgLen                           /**< message length */
);

GOAL_STATUS_T goal_lmMsgLength(
    GOAL_LM_LOG_ENTRY_T *pLmEntry,              /**< lm log message */
    size_t *pLen                                /**< [out] length */
);

#endif /* GOAL_LM_H */
