/** @file
 *
 * @brief GOAL UART Media Adapter
 *
 * @details
 * Generic media adapter for UART.
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

#define GOAL_ID GOAL_ID_MA_UART
#include <goal_includes.h>
#include <goal_media/goal_ma_uart.h>


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_MA_UART_T *pListUart = NULL;        /**< UART list */
static unsigned int cntUart = 0;                /**< count of registered UARTs */


/****************************************************************************/
/** Register UART driver
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maUartReg(
    GOAL_MA_UART_T **ppUart,                    /**< UART driver handle */
    GOAL_ID_T id,                               /**< UART MA id */
    GOAL_MA_UART_OPEN_T funcOpen,               /**< UART open function */
    GOAL_MA_UART_PUT_T funcPut,                 /**< UART put function */
    GOAL_MA_UART_PUTSTR_T funcPutStr,           /**< UART put string function */
    void *pData                                 /**< driver specific data */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MA_UART_T **ppElem;                    /* UART element */

    /* allocate UART handle */
    res = goal_memCalloc(ppUart, sizeof(GOAL_MA_UART_T));
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to allocate UART handle");
        return res;
    }

    /* setup UART handle */
    (*ppUart)->funcOpen = funcOpen;
    (*ppUart)->funcPut = funcPut;
    (*ppUart)->funcPutStr = funcPutStr;
    (*ppUart)->pData = pData;
    (*ppUart)->id = id;

    /* set default config:
     * synchronus transmitting,
     * notify at every received char and
     * baudrate at 115200 baud/s
     */
    (*ppUart)->cfg.txMode = GOAL_MA_UART_TRANSFER_MODE_SYNC;
    (*ppUart)->cfg.rxSize = 1;
    (*ppUart)->cfg.baudrate = GOAL_MA_UART_BAUD;

    /* iterate to list end */
    for (ppElem = &pListUart; *ppElem; ppElem = &(*ppElem)->pNext);

    /* store UART handle */
    *ppElem = *ppUart;
    cntUart++;

    return GOAL_OK;
}


/****************************************************************************/
/** UART open function
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maUartOpen(
    GOAL_MA_UART_T **ppUart,                    /**< UART driver handle */
    unsigned int idx,                           /**< UART index */
    GOAL_MA_UART_GET_T funcGet,                 /**< get char callback */
    GOAL_MA_UART_GETSTR_T funcGetStr,           /**< get str callback */
    GOAL_MA_UART_NOTIFY_T funcNotify,           /**< event notification callback */
    void *pArgNotify                            /**< notification function argument */
)
{
    GOAL_STATUS_T res;                          /* result */
    unsigned int cnt;                           /* UART list counter */
    GOAL_MA_UART_T *pUart;                      /* UART element */

    if (NULL == ppUart) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* find UART index in list */
    for (pUart = pListUart, cnt = 0; pUart && (cnt < idx); pUart = pUart->pNext, cnt++);
    if (NULL == pUart) {
        return GOAL_ERR_NOT_FOUND;
    }

    if (GOAL_FALSE != pUart->flgUsed) {
        return GOAL_ERR_ALREADY_USED;
    }

    res = goal_lockCreate(GOAL_LOCK_BINARY, &(pUart->pLockRxTx), 0, 1, GOAL_ID_MA_UART);
    if (GOAL_RES_OK(res)) {
        pUart->funcGet = funcGet;
        pUart->funcGetStr = funcGetStr;
        pUart->funcNotify = funcNotify;
        pUart->pArgNotify = pArgNotify;
        pUart->flgUsed = GOAL_TRUE;
        *ppUart = pUart;
    }

    if ((GOAL_RES_OK(res)) && NULL != pUart->funcOpen) {
        pUart->funcOpen(pUart);
    }

    return res;
}


/****************************************************************************/
/** UART close function
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maUartClose(
    GOAL_MA_UART_T *pUart                       /**< UART driver handle */
)
{
    if (NULL == pUart) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (GOAL_FALSE == pUart->flgUsed) {
        return GOAL_ERR_WRONG_STATE;
    }

    pUart->flgUsed = GOAL_FALSE;

    return GOAL_OK;
}


/****************************************************************************/
/** UART put function
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maUartPut(
    GOAL_MA_UART_T *pUart,                      /**< UART driver handle */
    const char chr                              /**< character */
)
{
    if (NULL == pUart) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (GOAL_FALSE == pUart->flgUsed) {
        return GOAL_ERR_WRONG_STATE;
    }

    if (NULL != pUart->funcPut) {
        return pUart->funcPut(pUart, chr);
    }

    return GOAL_ERR_UNSUPPORTED;
}


/****************************************************************************/
/** UART put function
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maUartPutStr(
    GOAL_MA_UART_T *pUart,                      /**< UART driver handle */
    const char *pStr,                           /**< string ptr */
    uint32_t len                                /**< string len */
)
{
    GOAL_STATUS_T res;                          /* return value */

    if (NULL == pUart) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (GOAL_FALSE == pUart->flgUsed) {
        return GOAL_ERR_WRONG_STATE;
    }

    if (NULL != pUart->funcPutStr) {
        res = pUart->funcPutStr(pUart, pStr, len);
    }
    else {
        res = GOAL_ERR_UNSUPPORTED;
    }

    return res;
}


/****************************************************************************/
/** UART get function
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maUartGet(
    GOAL_MA_UART_T *pUart,                      /**< UART driver handle */
    char *pChr                                  /**< character */
)
{
    if (GOAL_FALSE == pUart->flgUsed) {
        return GOAL_OK;
    }

    if (NULL != pUart->funcGet) {
        return pUart->funcGet(pUart, pChr);
    }

    return GOAL_OK;
}


/****************************************************************************/
/** UART get function
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maUartGetStr(
    GOAL_MA_UART_T *pUart,                      /**< UART driver handle */
    char *pStr,                                 /**< string */
    uint16_t len                                /**< string */
)
{
    if (GOAL_FALSE == pUart->flgUsed) {
        return GOAL_OK;
    }

    if (NULL != pUart->funcGetStr) {
        return pUart->funcGetStr(pUart, pStr, len);
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Notification callback for UART
 *
 * Once the driver has completed a transfer, it will invoke this notify cb, which
 * itself will invoke the notify callback registered during opening the instance.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maUartDrvNotifyCb(
    GOAL_MA_UART_T *pHdlMaUart,                 /**< MA handle for SPI */
    GOAL_MA_UART_EVENT_T event                  /**< event type */
)
{
    if (NULL == pHdlMaUart) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (NULL == pHdlMaUart->funcNotify) {
        return GOAL_ERR_UNSUPPORTED;
    }

    return pHdlMaUart->funcNotify(pHdlMaUart, event, pHdlMaUart->pArgNotify);
}


/****************************************************************************/
/** UART configuration function for transfer mode
 *
 * This function either configures the usage of the send string function
 * as synchronous (GOAL_MA_UART_TRANSFER_MODE_SYNC) or asynchronous
 * (GOAL_MA_UART_TRANSFER_MODE_ASYNC).
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maUartCfgTransferModeSet(
    GOAL_MA_UART_T *pMaUart,                    /**< MA handle for UART */
    GOAL_MA_UART_TRANSFER_MODE_T txMode         /**< transfer mode */
)
{
    if (NULL == pMaUart) {
        return GOAL_ERR_NULL_POINTER;
    }

    pMaUart->cfg.txMode = txMode;

    return GOAL_OK;
}


/****************************************************************************/
/** UART configuration function for receive block size
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maUartCfgReceiveBlockSize(
    GOAL_MA_UART_T *pMaUart,                    /**< MA handle for UART */
    uint16_t rxSize                             /**< rx size */
)
{
    if (NULL == pMaUart) {
        return GOAL_ERR_NULL_POINTER;
    }

    pMaUart->cfg.rxSize = rxSize;

    return GOAL_OK;
}


/****************************************************************************/
/** UART configuration function for baudrate
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maUartCfgBaudrateSet(
    GOAL_MA_UART_T *pMaUart,                    /**< MA handle for UART */
    GOAL_MA_UART_BAUDRATE_T baudrate            /**< baudrate */
)
{
    if (NULL == pMaUart) {
        return GOAL_ERR_NULL_POINTER;
    }

    pMaUart->cfg.baudrate = baudrate;

    return GOAL_OK;
}


/****************************************************************************/
/** Get UART MA handle by ID
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maUartGetById(
    GOAL_MA_UART_T **ppHdlMaUart,               /**< handle ref ptr */
    uint32_t id                                 /**< MA id */
)
{
    GOAL_MA_UART_T *pUart;                      /* UART element */
    unsigned int cnt;                           /* UART list counter */

    /* validate the arguments */
    if (NULL == ppHdlMaUart) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* find UART index in list */
    for (pUart = pListUart, cnt = 0; pUart && pUart->id != id; pUart = pUart->pNext, cnt++);
    if (NULL != pUart) {
        *ppHdlMaUart = pUart;
        return GOAL_OK;
    }

    /* no entry was found */
    return GOAL_ERR_NOT_FOUND;
}
