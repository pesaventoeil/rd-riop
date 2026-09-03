/** @file
 *
 * @brief  goal lm Log Message Emitter (Exception Log)
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


/****************************************************************************/
/* Defines */
/****************************************************************************/
#define GOAL_ID GOAL_ID_LM


/****************************************************************************/
/* Includes */
/****************************************************************************/
#include <goal_includes.h>
#include <goal_media/goal_ma_nvs.h>
#include <goal_media/goal_mi_nvs.h>
#include <goal_target_board.h>
#include "goal_lm_emit.h"
#include "goal_lm_emit_exception_log.h"
#include <goal/cm/goal_lm_cm.h>


/****************************************************************************/
/* Structs */
/****************************************************************************/

/**< private data for ma_uart emitter */
typedef struct {
    GOAL_BUFFER_T *pBuffer;                     /**< goal buffer */
    uint32_t writePosition;                     /**< nvs write offset */
    uint32_t readPosition;                      /**< cm interface read position */
    uint32_t size;                              /**< nvs region size */
    GOAL_MI_NVS_REGION_T *pMiNvsActive;         /**< currently active region */
    GOAL_MI_NVS_REGION_T *pMiNvsInactive;       /**< currently inactive region */
    GOAL_MI_NVS_REGION_T *pMiNvs1;              /**< NVS region handle for region 1 */
    GOAL_MI_NVS_REGION_T *pMiNvs2;              /**< NVS region handle for region 2 */
    uint16_t nvs1State;                         /**< NVS region 1 state */
    uint16_t nvs2State;                         /**< NVS region 2 state */
    uint16_t *pNvsActiveState;                  /**< currently active region state */
    uint16_t *pNvsInactiveState;                /**< currently inactive region state */
    GOAL_MI_NVS_REGION_T *pMiNvsRead;           /**< pointer to region for reading */
    uint32_t thresholdErase;                    /**< erase threshold */
} GOAL_LM_EMIT_EXCEPTION_LOG_INST_T;

GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct {
    uint16_t header;
    uint16_t len;
    uint16_t crc;
} GOAL_TARGET_PACKED GOAL_LM_EMIT_EXCEPTION_LOG_ITEM_T;
GOAL_TARGET_PACKED_STRUCT_POST


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_LM_EMIT_EXCEPTION_LOG_INST_T *pInstExLog;


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
static GOAL_STATUS_T goal_lmEmitExceptionLogOpen(
    struct GOAL_LM_EMIT_EMITTER_T *pHdlEmit     /**< LM Emitter handle */
);

static GOAL_STATUS_T goal_lmEmitExceptionLogStart(
    struct GOAL_LM_EMIT_EMITTER_T *pHdlEmit     /**< LM Emitter handle */
);

static GOAL_STATUS_T goal_lmEmitExceptionLogData(
    struct GOAL_LM_EMIT_EMITTER_T *pHdlEmit,    /**< LM Emitter handle */
    const char *strData,                        /**< emitter data */
    int len                                     /**< emitter data length */
);

static GOAL_STATUS_T goal_lmEmitExceptionLogEnd(
    struct GOAL_LM_EMIT_EMITTER_T *pHdlEmit     /**< LM Emitter handle */
);

static GOAL_STATUS_T goal_lmEmitExceptionLogIdle(
    struct GOAL_LM_EMIT_EMITTER_T *pHdlEmit     /**< LM Emitter handle */
);

static GOAL_STATUS_T goal_lmEmitExceptionLogUpdate(
    GOAL_LM_EMIT_EXCEPTION_LOG_INST_T *pInst    /**< LM Emitter instance */
);


/****************************************************************************/
/** EXCEPTION_LOG Emitter registration
 *
 * This function registers the EXCEPTION_LOG emitter at the goal lm emit module
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_lmEmitExceptionLogReg(
    void
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_LM_EMIT_EXCEPTION_LOG_INST_T *pInst;            /* emitter instance */
    GOAL_LM_EMIT_EMITTER_T *pHdlEmit;           /* emitter handle */

    res = goal_memCalloc(&pInst, sizeof(GOAL_LM_EMIT_EXCEPTION_LOG_INST_T));

    if (GOAL_RES_OK(res)) {
        res = goal_queuePoolBufsReq(GOAL_ID, GOAL_NETBUF_SIZE, 1, 0);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_lmEmitEmitterReg(&pHdlEmit,
            goal_lmEmitExceptionLogOpen,
            goal_lmEmitExceptionLogStart,
            goal_lmEmitExceptionLogData,
            goal_lmEmitExceptionLogEnd,
            goal_lmEmitExceptionLogIdle,
            (void *) pInst);
    }

    return res;
}

/****************************************************************************/
/** EXCEPTION_LOG Emitter open function
 *
 * This function may initialize the output device if necessary
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_lmEmitExceptionLogOpen(
    struct GOAL_LM_EMIT_EMITTER_T *pHdlEmit     /**< LM Emitter handle */
)
{
    GOAL_LM_EMIT_EXCEPTION_LOG_INST_T *pInst;   /* emitter instance */
    GOAL_STATUS_T res;                          /* result */
    uint32_t readPosition = 0;                  /* read position */
    uint32_t writePosition = 0;                 /* end position for new data */
    GOAL_LM_EMIT_EXCEPTION_LOG_ITEM_T item;     /* header */
    GOAL_BOOL_T flgErase;                       /* erase region flag */

    pInst = (GOAL_LM_EMIT_EXCEPTION_LOG_INST_T *) pHdlEmit->pData;

    res = goal_queuePoolGetBuf(&(pInst->pBuffer), EMIT_BUF_SIZE, GOAL_ID);

    /* get region 1 */
    if (GOAL_RES_OK(res)) {
        res = goal_miNvsRegionGetById(&pInst->pMiNvs1, GOAL_ID_MI_NVS_REGION_LM1);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_miNvsRegionOpen(pInst->pMiNvs1);
    }

    /* get region 2 */
    if (GOAL_RES_OK(res)) {
        res = goal_miNvsRegionGetById(&pInst->pMiNvs2, GOAL_ID_MI_NVS_REGION_LM2);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_miNvsRegionOpen(pInst->pMiNvs2);
    }

    /* read current states */
    if (GOAL_RES_OK(res)) {
        res = goal_miNvsRead(pInst->pMiNvs1, (uint8_t *) &pInst->nvs1State, pInst->pMiNvs1->length - sizeof(pInst->nvs1State), sizeof(pInst->nvs1State));
    }

    if (GOAL_RES_OK(res)) {
        res = goal_miNvsRead(pInst->pMiNvs2, (uint8_t *) &pInst->nvs2State, pInst->pMiNvs2->length - sizeof(pInst->nvs1State), sizeof(pInst->nvs2State));
    }

#if GOAL_CONFIG_EXLOG_DEBUG == 1
    goal_logInfo("region %s state %4x", pInst->pMiNvs1->strName, pInst->nvs1State);
    goal_logInfo("region %s state %4x", pInst->pMiNvs2->strName, pInst->nvs2State);
#endif

    /* TODO: workaround - erase both and set to empty if both _OLD */
    if (GOAL_EXCEPTIONLOG_REGION_STATE_OLD == pInst->nvs1State && GOAL_EXCEPTIONLOG_REGION_STATE_OLD == pInst->nvs2State) {
        pInst->nvs1State = GOAL_EXCEPTIONLOG_REGION_STATE_INVALID;
        pInst->nvs2State = GOAL_EXCEPTIONLOG_REGION_STATE_INVALID;
    }

    /* check if state is unknown */
    if (pInst->nvs1State == GOAL_EXCEPTIONLOG_REGION_STATE_INVALID) {
        /* erase region and set valid state */
        res = goal_miNvsErase(pInst->pMiNvs1);
        if (GOAL_RES_OK(res)) {
            pInst->nvs1State = GOAL_EXCEPTIONLOG_REGION_STATE_EMPTY;
            res = goal_miNvsWrite(pInst->pMiNvs1, (uint8_t *) &pInst->nvs1State, pInst->pMiNvs1->length - sizeof(pInst->nvs1State), sizeof(pInst->nvs1State));
        }
    }

    /* check if state is unknown */
    if (pInst->nvs2State == GOAL_EXCEPTIONLOG_REGION_STATE_INVALID) {
        /* erase region and set valid state */
        res = goal_miNvsErase(pInst->pMiNvs2);
        if (GOAL_RES_OK(res)) {
            pInst->nvs2State = GOAL_EXCEPTIONLOG_REGION_STATE_EMPTY;
            res = goal_miNvsWrite(pInst->pMiNvs2, (uint8_t *) &pInst->nvs2State, pInst->pMiNvs2->length - sizeof(pInst->nvs2State), sizeof(pInst->nvs2State));
        }
    }

    /* set active region */
    if (pInst->nvs1State != GOAL_EXCEPTIONLOG_REGION_STATE_USED && pInst->nvs2State != GOAL_EXCEPTIONLOG_REGION_STATE_USED) {
        pInst->pMiNvsActive = pInst->pMiNvs1;
        pInst->nvs1State = GOAL_EXCEPTIONLOG_REGION_STATE_USED;
        pInst->pNvsActiveState = &pInst->nvs1State;

        res = goal_miNvsWrite(pInst->pMiNvs1, (uint8_t *) &pInst->nvs1State, pInst->pMiNvs1->length - sizeof(pInst->nvs1State), sizeof(pInst->nvs1State));
        pInst->pMiNvsInactive = pInst->pMiNvs2;
        pInst->pNvsInactiveState = &pInst->nvs2State;
    }
    else if (pInst->nvs1State == GOAL_EXCEPTIONLOG_REGION_STATE_USED) {
        pInst->pMiNvsActive = pInst->pMiNvs1;
        pInst->pMiNvsInactive = pInst->pMiNvs2;

        pInst->pNvsActiveState = &pInst->nvs1State;
        pInst->pNvsInactiveState = &pInst->nvs2State;
    }
    else if (pInst->nvs2State == GOAL_EXCEPTIONLOG_REGION_STATE_USED) {
        pInst->pMiNvsActive = pInst->pMiNvs2;
        pInst->pMiNvsInactive = pInst->pMiNvs1;

        pInst->pNvsActiveState = &pInst->nvs2State;
        pInst->pNvsInactiveState = &pInst->nvs1State;
    }
    else {
        pInst->pMiNvsActive = NULL;
    }

    /* determine write pointer for new data */
    if (GOAL_RES_OK(res)) {
        readPosition = 0;
        writePosition = 0;
        while (readPosition < pInst->pMiNvsActive->length - sizeof(item)) {
            res = goal_miNvsRead(
                pInst->pMiNvsActive,
                (uint8_t *) &item,
                readPosition,
                sizeof(item));

            if (item.header == GOAL_EXCEPTIONLOG_HEADER_ID) {
            /* valid header -> set position after entry */
                writePosition += sizeof(item) + item.len;
                readPosition = writePosition;
            }
            else {
                /* skip to next position to check for valid header */
                readPosition += sizeof(item.header);
            }

            /* dont propagate error at end of region */
            res = GOAL_OK;
        }
    }

    /* check for non empty data in remaining storage */
    if (pInst->pMiNvsActive->pMiNvs->pMaNvs->infoFlash.flags & GOAL_MA_NVS_FLAG_FLASH) {
        readPosition = writePosition;
        while (readPosition < pInst->pMiNvsActive->length - sizeof(item)) {
            res = goal_miNvsRead(
                pInst->pMiNvsActive,
                (uint8_t *) &item,
                readPosition,
                sizeof(item));

            if (item.header != GOAL_EXCEPTIONLOG_REGION_STATE_INVALID) {
                writePosition = readPosition + sizeof(item.header);
#if GOAL_CONFIG_EXLOG_DEBUG == 1
                goal_logInfo("garbage skip in active region");
#endif
            }
            if (item.len != GOAL_EXCEPTIONLOG_REGION_STATE_INVALID) {
                writePosition = readPosition + sizeof(item.header) + sizeof(item.len);
#if GOAL_CONFIG_EXLOG_DEBUG == 1
                goal_logInfo("garbage skip in active region");
#endif
            }
            if (item.crc != GOAL_EXCEPTIONLOG_REGION_STATE_INVALID) {
                writePosition = readPosition + sizeof(item.header) + sizeof(item.len) + sizeof(item.crc);
#if GOAL_CONFIG_EXLOG_DEBUG == 1
                goal_logInfo("garbage skip in active region");
#endif
            }

            readPosition += sizeof(item);
        }

        /* check non active region */
        if (*pInst->pNvsInactiveState == GOAL_EXCEPTIONLOG_REGION_STATE_EMPTY) {
            readPosition = 0;
            flgErase = GOAL_FALSE;
            while (readPosition < pInst->pMiNvsInactive->length - sizeof(item)) {
                res = goal_miNvsRead(
                    pInst->pMiNvsInactive,
                    (uint8_t *) &item,
                    readPosition,
                    sizeof(item));

                if (item.header != GOAL_EXCEPTIONLOG_REGION_STATE_INVALID) {
                    flgErase = GOAL_TRUE;
#if GOAL_CONFIG_EXLOG_DEBUG == 1
                    goal_logInfo("garbage skip in inactive region");
#endif
                }
                if (item.len != GOAL_EXCEPTIONLOG_REGION_STATE_INVALID) {
                    flgErase = GOAL_TRUE;
#if GOAL_CONFIG_EXLOG_DEBUG == 1
                    goal_logInfo("garbage skip in inactive region");
#endif
                }
                if (item.crc != GOAL_EXCEPTIONLOG_REGION_STATE_INVALID) {
                    flgErase = GOAL_TRUE;
#if GOAL_CONFIG_EXLOG_DEBUG == 1
                    goal_logInfo("garbage skip in inactive region");
#endif
                }

                readPosition += sizeof(item);
            }

            if (GOAL_TRUE == flgErase) {
                /* trigger erase inactive region */
                *pInst->pNvsInactiveState = GOAL_EXCEPTIONLOG_REGION_STATE_ERASING;
            }
        }
    }

    /* check if any section needs to be erased */
    if (pInst->nvs1State == GOAL_EXCEPTIONLOG_REGION_STATE_ERASING) {
        /* erase region and set valid state */
        res = goal_miNvsErase(pInst->pMiNvs1);
        if (GOAL_RES_OK(res)) {
            pInst->nvs1State = GOAL_EXCEPTIONLOG_REGION_STATE_EMPTY;
            res = goal_miNvsWrite(pInst->pMiNvs1, (uint8_t *) &pInst->nvs1State, pInst->pMiNvs1->length - sizeof(pInst->nvs1State), sizeof(pInst->nvs1State));
        }
    }

    if (pInst->nvs2State == GOAL_EXCEPTIONLOG_REGION_STATE_ERASING) {
        /* erase region and set valid state */
        res = goal_miNvsErase(pInst->pMiNvs2);
        if (GOAL_RES_OK(res)) {
            pInst->nvs2State = GOAL_EXCEPTIONLOG_REGION_STATE_EMPTY;
            res = goal_miNvsWrite(pInst->pMiNvs2, (uint8_t *) &pInst->nvs2State, pInst->pMiNvs2->length - sizeof(pInst->nvs2State), sizeof(pInst->nvs2State));
        }
    }

    pInst->size = pInst->pMiNvsActive->length;
    pInst->writePosition = writePosition;

    /* calculate erase threshold */
    pInst->thresholdErase = pInst->size / 2;

    pInstExLog = pInst;

#if GOAL_CONFIG_EXLOG_DEBUG == 1
    goal_logInfo("region %s state %4x", pInst->pMiNvs1->strName, pInst->nvs1State);
    goal_logInfo("region %s state %4x", pInst->pMiNvs2->strName, pInst->nvs2State);
#endif

    return res;
}


/****************************************************************************/
/** EXCEPTION_LOG Emitter start function
 *
 * This function may do initial steps if necessary for output of a log message
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_lmEmitExceptionLogStart(
    struct GOAL_LM_EMIT_EMITTER_T *pHdlEmit     /**< LM Emitter handle */
)
{
    GOAL_LM_EMIT_EXCEPTION_LOG_INST_T *pInst;   /* emitter instance */
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_LM_EMIT_EXCEPTION_LOG_ITEM_T item;     /* header */

    GOAL_MI_NVS_REGION_T *pMiNvs;               /* region pointer for region swap */
    uint16_t *pNvsState;                        /* region state for region swap */

    pInst = (GOAL_LM_EMIT_EXCEPTION_LOG_INST_T *) pHdlEmit->pData;

    if ((pHdlEmit->pDbEntry->severity == GOAL_LOG_SEV_EXCEPTION) ||
        (pHdlEmit->pDbEntry->severity == GOAL_LOG_SEV_ERROR)) {

        /* check if data fits into region */
        if ((pInst->writePosition + sizeof(item) + pHdlEmit->lenLogRaw + (pHdlEmit->lenLogRaw % 2) + sizeof(item)) >= pInst->pMiNvsActive->length) {

#if GOAL_CONFIG_EXLOG_DEBUG == 1
            goal_logInfo("switching to region %s", pInst->pMiNvsInactive->strName);
            goal_lmEmitExceptionLogSwitchCb(pInst->writePosition);
#endif

            /* doesnt fit -> switch to empty region */
            pMiNvs = pInst->pMiNvsInactive;
            pInst->pMiNvsInactive = pInst->pMiNvsActive;
            pInst->pMiNvsActive = pMiNvs;

            pNvsState = pInst->pNvsInactiveState;
            pInst->pNvsInactiveState = pInst->pNvsActiveState;
            pInst->pNvsActiveState = pNvsState;

            /* set state of inactive region to GOAL_EXCEPTIONLOG_REGION_STATE_OLD */
            *pInst->pNvsInactiveState = GOAL_EXCEPTIONLOG_REGION_STATE_OLD;
            goal_miNvsWrite(pInst->pMiNvsInactive, (uint8_t *) pInst->pNvsInactiveState, pInst->pMiNvsInactive->length - sizeof(uint16_t), sizeof(uint16_t));

            /* set state of active region to GOAL_EXCEPTIONLOG_REGION_STATE_USED */
            *pInst->pNvsActiveState = GOAL_EXCEPTIONLOG_REGION_STATE_USED;
            goal_miNvsWrite(pInst->pMiNvsActive, (uint8_t *) pInst->pNvsActiveState, pInst->pMiNvsActive->length - sizeof(uint16_t), sizeof(uint16_t));

            pInst->writePosition = 0;
        }

        /* calculate crc */
        item.header = GOAL_EXCEPTIONLOG_HEADER_ID;
        item.crc = goal_utilCrcFletcher16((uint8_t *) pHdlEmit->pLogRaw, (int) pHdlEmit->lenLogRaw);
        item.len = (uint16_t) pHdlEmit->lenLogRaw;

        res = goal_miNvsWrite(
            pInst->pMiNvsActive,
            (void *) &item,
            pInst->writePosition,
            sizeof(item));

        if (GOAL_RES_OK(res)) {
            pInst->writePosition += sizeof(item);

            res = goal_miNvsWrite(
                pInst->pMiNvsActive,
                (void *) pHdlEmit->pLogRaw,
                pInst->writePosition,
                pHdlEmit->lenLogRaw);
        }

        if (GOAL_RES_OK(res)) {
            pInst->writePosition += pHdlEmit->lenLogRaw + (pHdlEmit->lenLogRaw % 2);
        }
    }

    return res;
}


/****************************************************************************/
/** EXCEPTION_LOG Emitter data function
 *
 * This function outputs the given data
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_lmEmitExceptionLogData(
    struct GOAL_LM_EMIT_EMITTER_T *pHdlEmit,    /**< LM Emitter handle */
    const char *strData,                        /**< emitter data */
    int len                                     /**< emitter data length */
)
{
    UNUSEDARG(pHdlEmit);
    UNUSEDARG(strData);
    UNUSEDARG(len);
    return GOAL_OK;
}


/****************************************************************************/
/** EXCEPTION_LOG Emitter end function
 *
 * This function ends output of a log message
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_lmEmitExceptionLogEnd(
    struct GOAL_LM_EMIT_EMITTER_T *pHdlEmit     /**< LM Emitter handle */
)
{
    GOAL_LM_EMIT_EXCEPTION_LOG_INST_T *pInst;   /* emitter instance */
    GOAL_STATUS_T res;                          /* result */

    pInst = (GOAL_LM_EMIT_EXCEPTION_LOG_INST_T *) pHdlEmit->pData;
    goal_lmEmitExceptionLogUpdate(pInst);

    if (pInst->writePosition > pInst->thresholdErase) {
        if (*pInst->pNvsInactiveState != GOAL_EXCEPTIONLOG_REGION_STATE_EMPTY) {

#if GOAL_CONFIG_EXLOG_DEBUG == 1
            goal_logInfo("threshold reached, erasing region %s", pInst->pMiNvsInactive->strName);
            goal_lmEmitExceptionLogEraseCb(pInst->writePosition);
#endif

            res = goal_miNvsErase(pInst->pMiNvsInactive);
            if (GOAL_RES_OK(res)) {
                *pInst->pNvsInactiveState = GOAL_EXCEPTIONLOG_REGION_STATE_EMPTY;
                res = goal_miNvsWrite(pInst->pMiNvsInactive, (uint8_t *) pInst->pNvsInactiveState, pInst->pMiNvsInactive->length - sizeof(uint16_t), sizeof(uint16_t));
            }
        }
    }

    return GOAL_OK;
}


/****************************************************************************/
/** EXCEPTION_LOG Emitter idle check function
 *
 * This function checks weather the emitter is idle or busy after End call
 *
 * @retval GOAL_OK emitter is idle
 * @retval other emitter is busy
 */
static GOAL_STATUS_T goal_lmEmitExceptionLogIdle(
    struct GOAL_LM_EMIT_EMITTER_T *pHdlEmit     /**< LM Emitter handle */
)
{
    UNUSEDARG(pHdlEmit);
    return GOAL_OK;
}


/****************************************************************************/
/** Exception logging update function
 *
 * This function updates information about the exception log
 */
static GOAL_STATUS_T goal_lmEmitExceptionLogUpdate(
    GOAL_LM_EMIT_EXCEPTION_LOG_INST_T *pInst    /**< LM Emitter instance */
)
{
    GOAL_STATUS_T res;                          /* return value */
    GOAL_CM_VAR_T *pCmVar;                      /* CM variable */

    /* update size */
    res = goal_cmGetVarById(GOAL_CM_LM_MOD_ID, LM_CM_VAR_EXLOG_SIZE, &pCmVar);
    if (GOAL_RES_OK(res)) {
        GOAL_CM_SETVAR_UINT32(pCmVar, pInst->size);
    }

    if (GOAL_RES_OK(res)) {
        /* update usage */
        res = goal_cmGetVarById(GOAL_CM_LM_MOD_ID, LM_CM_VAR_EXLOG_USAGE, &pCmVar);
        if (GOAL_RES_OK(res)) {
            GOAL_CM_SETVAR_UINT8(pCmVar,
                (uint8_t) ((100 * pInst->writePosition)/pInst->size));
        }
    }

    return res;
}


/****************************************************************************/
/** Exception logging erase function
 *
 * This function erases the exception log, initiated by CM access
 */
GOAL_STATUS_T LM_exLogErase(
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

    res = goal_miNvsErase(pInstExLog->pMiNvsActive);

    if (GOAL_RES_OK(res)) {

        res = goal_miNvsErase(pInstExLog->pMiNvsInactive);

        pInstExLog->writePosition = 0;
        pInstExLog->readPosition = 0;
        res = goal_lmEmitExceptionLogUpdate(pInstExLog);
    }

    return res;
}


/****************************************************************************/
/** cm callback for variable change
 *
 * @returns status
 */
GOAL_STATUS_T LM_exLogAccess(
    uint32_t modId,                             /**< module ID */
    uint32_t varId,                             /**< variable Id */
    GOAL_CM_VAR_T *pVar                         /**< variable pointer */
)
{
    uint16_t value;                             /* variable value */
    size_t cntBytes;                            /* bytes counter */
    GOAL_CM_VAR_T *pCmBufVar = NULL;            /* logging buffer */
    GOAL_CM_VAR_T *pCmCntVar = NULL;            /* logging counter */
    GOAL_STATUS_T res;                          /* result */
    static uint16_t cntFragment;                /* counter for fragmented message */
    static uint16_t cntOffset;                  /* offset counter for fragmentation */
    uint16_t crc = 0;                           /* crc */
    GOAL_LM_EMIT_EXCEPTION_LOG_ITEM_T item;     /* header */
    uint32_t posLastMsg;                        /* store last start of message */

    UNUSEDARG(modId);
    UNUSEDARG(varId);

    cntBytes = 0;

    /* get logging buffer from CM */
    res = goal_cmGetVarById(GOAL_CM_LM_MOD_ID, LM_CM_VAR_EXLOG_READBUFFER, &pCmBufVar);

    if (GOAL_RES_OK(res)) {
        res = goal_cmGetVarById(GOAL_CM_LM_MOD_ID, LM_CM_VAR_EXLOG_CNT, &pCmCntVar);
    }

    if (GOAL_RES_OK(res)) {
        value = GOAL_CM_VAR_UINT16(pVar);

        if (0x0000 == value) {
            /* reset read pointer */
            pInstExLog->readPosition = 0;
            pInstExLog->pMiNvsRead = NULL;
            cntFragment = 0;
        }

        /* set start region */
        if (pInstExLog->pMiNvsRead == NULL) {
            if (*pInstExLog->pNvsInactiveState == GOAL_EXCEPTIONLOG_REGION_STATE_OLD) {
                pInstExLog->pMiNvsRead = pInstExLog->pMiNvsInactive;
            }
            else {
                pInstExLog->pMiNvsRead = pInstExLog->pMiNvsActive;
            }
        }

        if (0xFFFF == value) {
            /* get new bytes from buffer */

                cntBytes = 0;

                /* before reading new message, process last fragmented message */
                if (cntFragment > 0) {

                    cntBytes = cntFragment;

                    /* check if fragment needs to be fragmented again */
                    if (cntBytes > pCmBufVar->totalSize) {
                        /* calculate remaining length */
                        cntFragment = (uint16_t) (cntBytes - (uint16_t) pCmBufVar->totalSize);
                        cntBytes = (uint16_t) pCmBufVar->totalSize;
                    }
                    else {
                        cntFragment = 0;
                    }

                    GOAL_CM_CLEARVAR_GENERIC(pCmBufVar, 0, 1);
                    GOAL_CM_SETVAR_GENERIC(pCmBufVar, pInstExLog->pBuffer->ptrData + cntOffset, (uint16_t) cntBytes);
                    cntOffset += cntBytes;
                }
                else {

                    /* continue reading on active region */
                    if ((pInstExLog->readPosition + sizeof(item) + sizeof(uint16_t)) >= pInstExLog->pMiNvsRead->length) {
                        if (pInstExLog->pMiNvsRead != pInstExLog->pMiNvsActive) {
                            pInstExLog->pMiNvsRead = pInstExLog->pMiNvsActive;
                            pInstExLog->readPosition = 0;
                        }
                    }

                    /* read */
                    if ((pInstExLog->readPosition + sizeof(item) + sizeof(uint16_t)) < pInstExLog->pMiNvsRead->length) {

                        do {

                            /* set return value */
                            cntBytes = 0;

                            /* store current position */
                            posLastMsg = pInstExLog->readPosition;
                            GOAL_MEMSET(&item, 0, sizeof(item));

                            /* start reading */
                            res = goal_miNvsRead(pInstExLog->pMiNvsRead, (uint8_t *) &item, pInstExLog->readPosition, sizeof(item));
                            pInstExLog->readPosition += sizeof(item);

                            if (item.header == GOAL_EXCEPTIONLOG_HEADER_ID) {
                                res = goal_miNvsRead(pInstExLog->pMiNvsRead, pInstExLog->pBuffer->ptrData, pInstExLog->readPosition, item.len);
                                if (GOAL_RES_OK(res)) {
                                    crc = goal_utilCrcFletcher16((uint8_t *) pInstExLog->pBuffer->ptrData, item.len);
                                }

                                if (GOAL_RES_OK(res) && crc == item.crc) {
                                    pInstExLog->readPosition += item.len;
                                    pInstExLog->readPosition += (pInstExLog->readPosition % 2);
                                    cntBytes = item.len;

                                    /* found a message */
                                    break;
                                }
                            }

                            /* reset read position because of invalid data */
                            pInstExLog->readPosition = posLastMsg + sizeof(uint16_t);

                        } while (pInstExLog->readPosition + sizeof(item) < pInstExLog->pMiNvsRead->length);
                    }
                    else {
                        cntBytes = 0;
                        res = GOAL_ERR_EMPTY;
                    }

                    if (GOAL_RES_OK(res)) {
                        /* check if message needs to be fragmented */
                        if (cntBytes > pCmBufVar->totalSize) {
                            /* calculate remaining length */
                            cntFragment = (uint16_t) (cntBytes - (uint16_t) pCmBufVar->totalSize);
                            cntBytes = (uint16_t) pCmBufVar->totalSize;
                            cntOffset = (uint16_t) cntBytes;
                        }

                        GOAL_CM_CLEARVAR_GENERIC(pCmBufVar, 0, 1);
                        GOAL_CM_SETVAR_GENERIC(pCmBufVar, pInstExLog->pBuffer->ptrData, (uint16_t) cntBytes);
                    }
                }


                if (res == GOAL_ERR_EMPTY) {
                    /* if buffer is empty, don't propagate an error */
                    res = GOAL_OK;
                }

            GOAL_CM_SETVAR_UINT16(pCmCntVar, (uint16_t) cntBytes);
        }
    }

    return res;
}
