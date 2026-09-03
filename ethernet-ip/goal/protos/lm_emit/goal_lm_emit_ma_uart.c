/** @file
 *
 * @brief  goal lm Log Message Emitter
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
#include <goal_media/goal_ma_uart.h>
#include "goal_lm_emit.h"
#include "goal_lm_emit_ma_uart.h"


/****************************************************************************/
/* Structs */
/****************************************************************************/

/**< private data for ma_uart emitter */
typedef struct {
    GOAL_BUFFER_T *pBuffer;                     /**< goal buffer */
    GOAL_BOOL_T flgStart;                       /**< flag for start detection */
    GOAL_BOOL_T flagStop;                       /**< flag for stop detection */
    GOAL_BOOL_T flgBusy;                       /**< flag for busy detection */
    int posBuf;                                 /**< buffer write position */
    GOAL_MA_UART_T *pMaUart;             /* UART MA handle */
} GOAL_LM_EMIT_MA_UART_INST_T;


/****************************************************************************/
/* Local variables */
/****************************************************************************/


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
static GOAL_STATUS_T goal_lmEmitMaUartOpen(
    struct GOAL_LM_EMIT_EMITTER_T *pHdlEmit     /**< LM Emitter handle */
);

static GOAL_STATUS_T goal_lmEmitMaUartStart(
    struct GOAL_LM_EMIT_EMITTER_T *pHdlEmit     /**< LM Emitter handle */
);

static GOAL_STATUS_T goal_lmEmitMaUartData(
    struct GOAL_LM_EMIT_EMITTER_T *pHdlEmit,    /**< LM Emitter handle */
    const char *strData,                        /**< emitter data */
    int len                                     /**< emitter data length */
);

static GOAL_STATUS_T goal_lmEmitMaUartEnd(
    struct GOAL_LM_EMIT_EMITTER_T *pHdlEmit     /**< LM Emitter handle */
);

static GOAL_STATUS_T goal_lmEmitMaUartIdle(
    struct GOAL_LM_EMIT_EMITTER_T *pHdlEmit     /**< LM Emitter handle */
);

static GOAL_STATUS_T goal_maEmitMaUartNotify(
    struct GOAL_MA_UART_T *pUart,               /**< UART handle */
    GOAL_MA_UART_EVENT_T event,                 /**< UART event */
    void *pArgNotify                            /**< notify arguments */
);


/****************************************************************************/
/** MA_UART Emitter registration
 *
 * This function registers the MA_UART emitter at the goal lm emit module
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_lmEmitMaUartReg(
    void
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_LM_EMIT_MA_UART_INST_T *pInst;            /* emitter instance */
    GOAL_LM_EMIT_EMITTER_T *pHdlEmit;           /* emitter handle */

    res = goal_memCalloc(&pInst, sizeof(GOAL_LM_EMIT_MA_UART_INST_T));

    if (GOAL_RES_OK(res)) {
        res = goal_queuePoolBufsReq(GOAL_ID, GOAL_NETBUF_SIZE, 1, 0);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_lmEmitEmitterReg(&pHdlEmit,
            goal_lmEmitMaUartOpen,
            goal_lmEmitMaUartStart,
            goal_lmEmitMaUartData,
            goal_lmEmitMaUartEnd,
            goal_lmEmitMaUartIdle,
            (void *) pInst);
    }

    return res;
}

/****************************************************************************/
/** MA_UART Emitter open function
 *
 * This function may initialize the output device if necessary
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_lmEmitMaUartOpen(
    struct GOAL_LM_EMIT_EMITTER_T *pHdlEmit     /**< LM Emitter handle */
)
{
    GOAL_LM_EMIT_MA_UART_INST_T *pInst;            /* emitter instance */
    GOAL_STATUS_T res;                          /* result */

    pInst = (GOAL_LM_EMIT_MA_UART_INST_T *) pHdlEmit->pData;

    res = goal_queuePoolGetBuf(&(pInst->pBuffer), EMIT_BUF_SIZE, GOAL_ID);
    if (GOAL_RES_OK(res)) {
        res = goal_maUartOpen(&pInst->pMaUart, GOAL_ID_MA_UART_DEFAULT, NULL, NULL, goal_maEmitMaUartNotify, pInst);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_maUartCfgTransferModeSet(pInst->pMaUart, GOAL_MA_UART_TRANSFER_MODE_ASYNC);
    }

    return res;
}


/****************************************************************************/
/** MA_UART Emitter start function
 *
 * This function may do initial steps if necessary for output of a log message
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_lmEmitMaUartStart(
    struct GOAL_LM_EMIT_EMITTER_T *pHdlEmit     /**< LM Emitter handle */
)
{
    GOAL_LM_EMIT_MA_UART_INST_T *pInst;            /* emitter instance */
    GOAL_STATUS_T res;                          /* result */

    pInst = (GOAL_LM_EMIT_MA_UART_INST_T *) pHdlEmit->pData;

    pInst->flagStop = GOAL_FALSE;
    pInst->flgStart = GOAL_TRUE;
    pInst->posBuf = 0;

    res = GOAL_OK;

    return res;
}


/****************************************************************************/
/** MA_UART Emitter data function
 *
 * This function outputs the given data
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_lmEmitMaUartData(
    struct GOAL_LM_EMIT_EMITTER_T *pHdlEmit,    /**< LM Emitter handle */
    const char *strData,                        /**< emitter data */
    int len                                     /**< emitter data length */
)
{
    GOAL_LM_EMIT_MA_UART_INST_T *pInst;            /* emitter instance */
    GOAL_STATUS_T res;                          /* result */

    pInst = (GOAL_LM_EMIT_MA_UART_INST_T *) pHdlEmit->pData;

    if (pInst->posBuf + len < pInst->pBuffer->bufSize) {
        GOAL_MEMCPY(&pInst->pBuffer->ptrData[pInst->posBuf], strData, (unsigned long) len);
        pInst->posBuf += len;

        res = GOAL_OK;
    }
    else {
        res = GOAL_ERR_OVERFLOW;
    }

    return res;
}


/****************************************************************************/
/** MA_UART Emitter end function
 *
 * This function ends output of a log message
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_lmEmitMaUartEnd(
    struct GOAL_LM_EMIT_EMITTER_T *pHdlEmit     /**< LM Emitter handle */
)
{
    GOAL_LM_EMIT_MA_UART_INST_T *pInst;            /* emitter instance */
    GOAL_STATUS_T res;                          /* result */

    pInst = (GOAL_LM_EMIT_MA_UART_INST_T *) pHdlEmit->pData;

    pInst->flagStop = GOAL_TRUE;
    pInst->flgStart = GOAL_FALSE;

    /* append carriage return */
    pInst->pBuffer->ptrData[pInst->posBuf] = '\n';
    pInst->pBuffer->ptrData[pInst->posBuf + 1] = '\0';
    pInst->posBuf += 1;

    /* output buffer */
    pInst->flgBusy = GOAL_TRUE;
    res = goal_maUartPutStr(pInst->pMaUart, (const char *) pInst->pBuffer->ptrData, (unsigned int) pInst->posBuf);

    return res;
}


/****************************************************************************/
/** MA_UART Emitter idle check function
 *
 * This function checks weather the emitter is idle or busy after End call
 *
 * @retval GOAL_OK emitter is idle
 * @retval other emitter is busy
 */
static GOAL_STATUS_T goal_lmEmitMaUartIdle(
    struct GOAL_LM_EMIT_EMITTER_T *pHdlEmit     /**< LM Emitter handle */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_LM_EMIT_MA_UART_INST_T *pInst;         /* emitter instance */

    pInst = (GOAL_LM_EMIT_MA_UART_INST_T *) pHdlEmit->pData;

    if (pInst->flgBusy == GOAL_TRUE) {
        res = GOAL_ERR_BUSY;
    }
    else {
        res = GOAL_OK;
    }

    return res;
}


/****************************************************************************/
/** MA_UART MA notify function, which signals a completed transfer
 *
 * @retval GOAL_OK success
 * @retval other failure
 */
static GOAL_STATUS_T goal_maEmitMaUartNotify(
    struct GOAL_MA_UART_T *pUart,               /**< UART handle */
    GOAL_MA_UART_EVENT_T event,                 /**< UART event */
    void *pArgNotify                            /**< notify arguments */
)
{
    GOAL_LM_EMIT_MA_UART_INST_T *pInst;            /* emitter instance */

    UNUSEDARG(pUart);

    pInst = (GOAL_LM_EMIT_MA_UART_INST_T *) pArgNotify;

    if (GOAL_MA_UART_EVENT_TRANSFER_COMPLETED == event) {
        pInst->flgBusy = GOAL_FALSE;
    }

    return GOAL_OK;
}
