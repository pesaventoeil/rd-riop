/** @file
 *
 * @brief Generic MA for SPI
 *
 * @details
 * This module provides a MA for SPI
 *
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

#define GOAL_ID GOAL_ID_MA_SPI
#include "goal_includes.h"
#include "goal_media/goal_ma_spi.h"


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_MA_SPI_T *pListSpi = NULL;          /**< SPI list */
static unsigned int cntSpi = 0;                 /**< count of registered SPIs */


/****************************************************************************/
/** Registers a SPI MA compliant driver
 *
 * This function is called during board init to register available
 * SPI drivers.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maSpiDrvReg(
    uint32_t id,                                /**< ID */
    GOAL_MA_SPI_T **ppHdlMaSpi,                 /**< SPI handle ref ptr */
    GOAL_MA_SPI_OPEN fOpen,                     /**< open function */
    GOAL_MA_SPI_OPEN fReopen,                   /**< open function */
    GOAL_MA_SPI_CONF_GET fConfGet,              /**< conf get function */
    GOAL_MA_SPI_CONF_SET fConfSet,              /**< conf set function */
    GOAL_MA_SPI_READ fRead,                     /**< read function */
    GOAL_MA_SPI_WRITE fWrite,                   /**< write function */
    GOAL_MA_SPI_WRITEREAD fWriteRead,           /**< write/read function */
    GOAL_MA_SPI_ASYNC_START fAsyncStart,        /**< start async function */
    GOAL_MA_SPI_CLOSE fClose,                   /**< close function */
    GOAL_MA_SPI_RESET fReset,                   /**< reset function */
    void *pDrvData                              /**< driver specific data */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MA_SPI_T **ppElem;                     /* spi element */
    GOAL_MA_SPI_T *pMaValid = NULL;             /* MA validation element */

    /* Check that all function ptr are not NULL */
    if (NULL == fOpen) {
        /* GG_LOG ID:263: */
        goal_lmLog(GOAL_ID_MA_SPI, 263, 214, 0, 0, GOAL_LOG_SEV_ERROR, "open function not defined");
        return GOAL_ERR_NULL_POINTER;
    }

    /* validate that the ID was not registered before */
    res = goal_maSpiGetById(&pMaValid, id);
    if (GOAL_RES_OK(res)) {
        /* GG_LOG ID:264: */
        goal_lmLog(GOAL_ID_MA_SPI, 264, 215, 4, 0, GOAL_LOG_SEV_ERROR, "The SPI MA with ID $1 is already in use");
        goal_lmLogParamUINT32(id);
        goal_lmLogFinish();
        return GOAL_ERR_INIT;
    }

    res = goal_memCalloc(ppHdlMaSpi, sizeof(GOAL_MA_SPI_T));
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:265: */
        goal_lmLog(GOAL_ID_MA_SPI, 265, 216, 0, 0, GOAL_LOG_SEV_ERROR, "Unable to allocate SPI MA");
        return res;
    }

    /* initialize the structure items */
    (*ppHdlMaSpi)->id = id;
    (*ppHdlMaSpi)->pDrvData = pDrvData;
    (*ppHdlMaSpi)->fctOpen = fOpen;
    (*ppHdlMaSpi)->fctReopen = fReopen;
    (*ppHdlMaSpi)->fctConfGet = fConfGet;
    (*ppHdlMaSpi)->fctConfSet = fConfSet;
    (*ppHdlMaSpi)->fctRead = fRead;
    (*ppHdlMaSpi)->fctWrite = fWrite;
    (*ppHdlMaSpi)->fctWriteRead = fWriteRead;
    (*ppHdlMaSpi)->fctAsyncStart = fAsyncStart;
    (*ppHdlMaSpi)->fctClose = fClose;
    (*ppHdlMaSpi)->fctReset = fReset;

    /* attach MA SPI handle to MA SPI list */
    for (ppElem = &pListSpi; *ppElem; ppElem = &(*ppElem)->pNext);
    *ppElem = *ppHdlMaSpi;

    /* increase registered SPI MA counter */
    cntSpi++;

    /* initialize default configuration */
    (*ppHdlMaSpi)->cfg.bitorder = GOAL_MA_SPI_BITORDER_MSB;
    (*ppHdlMaSpi)->cfg.bitrate = 2000000;
    (*ppHdlMaSpi)->cfg.mode = GOAL_MA_SPI_MODE_0;
    (*ppHdlMaSpi)->cfg.txMode = GOAL_MA_SPI_TRANSFER_MODE_SYNC;
    (*ppHdlMaSpi)->cfg.type = GOAL_MA_SPI_TYPE_MASTER;
    (*ppHdlMaSpi)->cfg.unitWidth = GOAL_MA_SPI_UNITWIDTH_8BIT;

    return GOAL_OK;
}


/****************************************************************************/
/** Open a SPI MA channel
 *
 * After successful execution, *ppSpiHdl contains a pointer to the opened
 * SPI channel instance, otherwise the pointer is invalid.
 *
 * @retval GOAL_OK successful
 * @retval GOAL_ERR_NULL_POINTER notify handler was null
 * @retval other failed
 */
GOAL_STATUS_T goal_maSpiOpen(
    uint32_t id,                                /**< id of SPI handler to use */
    GOAL_MA_SPI_T **ppSpiHdl,                   /**< pointer to store SPI handler */
    GOAL_MA_SPI_NOTIFY fNotify,                 /**< callback function ptr */
    void *pArgNotify                            /**< notification function argument */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* Return value */

    /* Get SPI handler for given ID */
    res = goal_maSpiGetById(ppSpiHdl, id);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* Check whether handle is already in use */
    if ((*ppSpiHdl)->flgUsed) {
        return GOAL_ERR_ACCESS;
    }

    /* TODO: only for async transfers needed */
    /* create RX databuffer */
    res = goal_memCalloc(&(*ppSpiHdl)->pDataRx, GOAL_MA_SPI_TX_SIZE);
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:266: */
        goal_lmLog(GOAL_ID_MA_SPI, 266, 217, 0, 0, GOAL_LOG_SEV_ERROR, "failed to allocate RX databuffer");
        return res;
    }

    /* create TX databuffer */
    res = goal_memCalloc(&(*ppSpiHdl)->pDataTx, GOAL_MA_SPI_TX_SIZE);
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:267: */
        goal_lmLog(GOAL_ID_MA_SPI, 267, 218, 0, 0, GOAL_LOG_SEV_ERROR, "failed to allocate TX databuffer");
        return res;
    }

    /* create RX/TX lock */
    res = goal_lockCreate(GOAL_LOCK_BINARY, &(*ppSpiHdl)->pLockRxTx, 0, 1, GOAL_ID_MA_SPI);
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:268: */
        goal_lmLog(GOAL_ID_MA_SPI, 268, 219, 0, 0, GOAL_LOG_SEV_ERROR, "failed to create RX/TX lock");
        return res;
    }

    /* assign notify callback */
    (*ppSpiHdl)->fctNotify = fNotify;
    (*ppSpiHdl)->pFctNotifyArg = pArgNotify;

    /* Invoke open function */
    res = (*ppSpiHdl)->fctOpen(*ppSpiHdl);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* Mark handle as used */
    (*ppSpiHdl)->flgUsed = GOAL_TRUE;

    return GOAL_OK;
}


/****************************************************************************/
/** Reopens an SPI channel instance
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maSpiReopen(
    GOAL_MA_SPI_T *pSpiHdl                      /**< pointer to store SPI handler */
)
{
    if (NULL == pSpiHdl) {
       return GOAL_ERR_NULL_POINTER;
    }

    if (NULL == pSpiHdl->fctReopen) {
        return GOAL_ERR_UNSUPPORTED;
    }

    /* Invoke close function */
    return pSpiHdl->fctReopen(pSpiHdl);
}


/****************************************************************************/
/** Closes an SPI channel instance
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maSpiClose(
    GOAL_MA_SPI_T *pSpiHdl                      /**< pointer to store SPI handler */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    if (NULL == pSpiHdl) {
       return GOAL_ERR_NULL_POINTER;
    }

    if (NULL == pSpiHdl->fctClose) {
        return GOAL_ERR_UNSUPPORTED;
    }

    /* Invoke close function */
    res = (pSpiHdl)->fctClose(pSpiHdl);
    pSpiHdl->flgUsed = GOAL_FALSE;

    return res;
}


/****************************************************************************/
/** Write to MA SPI
 *
 * This function writes data via the registered driver.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maSpiWrite(
    GOAL_MA_SPI_T *pHdlMaSpi,                   /**< MA handle for SPI */
    uint8_t *pData,                             /**< data */
    uint16_t len                                /**< data length in bytes */
)
{
    if (NULL == pHdlMaSpi) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (NULL == pHdlMaSpi->fctWrite) {
        return GOAL_ERR_UNSUPPORTED;
    }

    return pHdlMaSpi->fctWrite(pHdlMaSpi, pData, len);
}


/****************************************************************************/
/** Read from MA SPI
 *
 * This function reads data via the registered driver instance. In case there is currently
 * no data available (buffered), the callback will be invoked once the data is read.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maSpiRead(
    GOAL_MA_SPI_T *pHdlMaSpi,                   /**< MA handle for SPI */
    uint8_t *pData,                             /**< data */
    uint16_t *pLen                              /**< data length in bytes */
)
{
    if (NULL == pHdlMaSpi) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (NULL == pHdlMaSpi->fctRead) {
        return GOAL_ERR_UNSUPPORTED;
    }

    return pHdlMaSpi->fctRead(pHdlMaSpi, pData, pLen);
}


/****************************************************************************/
/** Reads/writes data to MA SPI
 *
 * This function writes data via the registered driver.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maSpiWriteRead(
    GOAL_MA_SPI_T *pHdlMaSpi,                   /**< MA handle for SPI */
    uint8_t *pWriteData,                        /**< data */
    uint8_t *pReadData,                         /**< data */
    uint16_t *pLen                              /**< data length in bytes */
)
{
    if (NULL == pHdlMaSpi) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (NULL == pHdlMaSpi->fctWriteRead) {
        return GOAL_ERR_UNSUPPORTED;
    }

    return pHdlMaSpi->fctWriteRead(pHdlMaSpi, pWriteData, pReadData, pLen);
}


/****************************************************************************/
/** Get MA SPI by ID
 *
 * This function returns the ID associated MA.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maSpiGetById(
    GOAL_MA_SPI_T **ppHdlMaSpi,                 /**< SPI handle ref ptr */
    uint32_t id                                 /**< MA id */
)
{
    GOAL_MA_SPI_T *pListSpiIdx;                 /* SPI list idx */

    /* validate the arguments */
    if (NULL == ppHdlMaSpi) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* search the entry with the correct ID */
    for (pListSpiIdx = pListSpi; (pListSpiIdx); pListSpiIdx = pListSpiIdx->pNext) {
        if (pListSpiIdx->id == id) {
            *ppHdlMaSpi = pListSpiIdx;
            return GOAL_OK;
        }
    }

    /* no entry was found */
    return GOAL_ERR_NOT_FOUND;
}


/****************************************************************************/
/** Notification callback for new SPI data
 *
 * Once the driver has data available, it will invoke this notify cb, which
 * itself will invoke the notify callback registered during opening the instance.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maSpiDrvNotifyCb(
    GOAL_MA_SPI_T *pHdlMaSpi,                   /**< MA handle for SPI */
    GOAL_MA_SPI_EVENT_T event,                  /**< event type */
    GOAL_MA_SPI_EVENT_DATA_T *pDataEvent        /**< optional event data */
)
{
    if (NULL == pHdlMaSpi) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (NULL == pHdlMaSpi->fctNotify) {
        return GOAL_ERR_UNSUPPORTED;
    }

    return pHdlMaSpi->fctNotify(pHdlMaSpi, event, pDataEvent, pHdlMaSpi->pFctNotifyArg);
}


/****************************************************************************/
/** Sets the configuration for the given SPI instance
 *
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maSpiConfigSet(
    GOAL_MA_SPI_T *pHdlMaSpi                    /**< MA handle for SPI */
)
{
    if (NULL == pHdlMaSpi) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (NULL == pHdlMaSpi->fctConfSet) {
        return GOAL_ERR_UNSUPPORTED;
    }

    return pHdlMaSpi->fctConfSet(pHdlMaSpi);
}


/****************************************************************************/
/** Gets the configuration for the given SPI instance
 *
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maSpiConfigGet(
    GOAL_MA_SPI_T *pHdlMaSpi                    /**< MA handle for SPI */
)
{
    if (NULL == pHdlMaSpi) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (NULL == pHdlMaSpi->fctConfGet) {
        return GOAL_ERR_UNSUPPORTED;
    }

    return pHdlMaSpi->fctConfGet(pHdlMaSpi);
}


/****************************************************************************/
/** Initiate a asynchronous transfer
 *
 * This interface function only applies if the TRANSFER_MODE is set to
 * GOAL_MA_SPI_TRANSFER_MODE_ASYNC.
 *
 * Copy data from a given buffer into the SPI TX buffer. If there is already
 * data in the TX buffer then GOAL_ERR_BUSY is returned.
 *
 * @retval GOAL_OK successful
 * @retval GOAL_ERR_BUSY TX buffer already filled
 * @retval other failed
 */
GOAL_STATUS_T goal_maSpiTxAsync(
    GOAL_MA_SPI_T *pHdlMaSpi,                   /**< MA handle for SPI */
    uint8_t *pDataTx,                           /**< tx data buffer */
    unsigned int lenDataTx                      /**< tx buffer size */
)
{
    GOAL_STATUS_T res;                          /* result */

    if ((NULL == pHdlMaSpi) || (NULL == pDataTx)) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* get lock */
    res = goal_lockGet(pHdlMaSpi->pLockRxTx, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* check if data fits into transmit buffer */
    if (lenDataTx > GOAL_MA_SPI_TX_SIZE) {
        return GOAL_ERR_OVERFLOW;
    }

    /* check if there is already data in TX buffer */
    if (0 != pHdlMaSpi->lenDataTx) {

        /* release lock */
        goal_lockPut(pHdlMaSpi->pLockRxTx);

        return GOAL_ERR_BUSY;
    }

    /* put data into ringbuffer */
    GOAL_MEMCPY(pHdlMaSpi->pDataTx, pDataTx, lenDataTx);
    pHdlMaSpi->lenDataTx = lenDataTx;

    /* release lock */
    goal_lockPut(pHdlMaSpi->pLockRxTx);

    return GOAL_OK;
}


/****************************************************************************/
/** Process a asynchronous reception
 *
 * This interface function only applies if the TRANSFER_MODE is set to
 * GOAL_MA_SPI_TRANSFER_MODE_ASYNC.
 *
 * Copy data from SPI RX to supplied RX buffer. If supplied buffer is smaller
 * than the SPI RX buffer than GOAL_OK_PARTIAL is returned.
 *
 * @retval GOAL_OK successful
 * @retval GOAL_OK_PARTIAL not all available data was read
 * @retval other failed
 */
GOAL_STATUS_T goal_maSpiRxAsync(
    GOAL_MA_SPI_T *pHdlMaSpi,                   /**< MA handle for SPI */
    uint8_t *pDataRx,                           /**< rx data buffer */
    unsigned int *pLenDataRx                    /**< [in,out] rx buffer size */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    if ((NULL == pHdlMaSpi) || (NULL == pDataRx)) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* get lock */
    res = goal_lockGet(pHdlMaSpi->pLockRxTx, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* skip if available length is zero */
    if (0 == pHdlMaSpi->lenDataRx) {

        *pLenDataRx = 0;

        /* release lock */
        goal_lockPut(pHdlMaSpi->pLockRxTx);

        return GOAL_OK;
    }

    /* check if requested data length is available */
    if (*pLenDataRx > pHdlMaSpi->lenDataRx) {
        *pLenDataRx = pHdlMaSpi->lenDataRx;
    }
    else if (*pLenDataRx < pHdlMaSpi->lenDataRx) {
        res = GOAL_OK_PARTIAL;
    }

    /* copy available data */
    GOAL_MEMCPY(pDataRx, pHdlMaSpi->pDataRx, *pLenDataRx);

    /* flush buffer */
    pHdlMaSpi->lenDataRx = 0;

    /* release lock */
    goal_lockPut(pHdlMaSpi->pLockRxTx);

    return res;
}


/****************************************************************************/
/** SPI Get maximum buffer size
 *
 * @returns buffer size in bytes
 */
unsigned int goal_maSpiBufSizeGet(
    GOAL_MA_SPI_T *pHdlMaSpi                    /**< MA handle for SPI */
)
{
    UNUSEDARG(pHdlMaSpi);

    return GOAL_MA_SPI_TX_SIZE;
}


/****************************************************************************/
/** SPI Start asynchronous transfer
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maSpiAsyncStart(
    GOAL_MA_SPI_T *pMaSpi                       /**< MA handle for SPI */
)
{
    if (NULL == pMaSpi) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (NULL == pMaSpi->fctAsyncStart) {
        return GOAL_ERR_UNSUPPORTED;
    }

    return pMaSpi->fctAsyncStart(pMaSpi);
}


/****************************************************************************/
/** SPI Configure bitrate
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maSpiCfgBitrateSet(
    GOAL_MA_SPI_T *pMaSpi,                      /**< MA handle for SPI */
    unsigned int valBitrate                     /**< bitrate */
)
{
    if (NULL == pMaSpi) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (NULL == pMaSpi->fctConfSet) {
        return GOAL_ERR_UNSUPPORTED;
    }

    pMaSpi->cfg.bitrate = valBitrate;
    return pMaSpi->fctConfSet(pMaSpi);
}


/****************************************************************************/
/** SPI Configure type
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maSpiCfgTypeSet(
    GOAL_MA_SPI_T *pMaSpi,                      /**< MA handle for SPI */
    GOAL_MA_SPI_TYPE_T type                     /**< SPI type master or slave */
)
{
    if (NULL == pMaSpi) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (NULL == pMaSpi->fctConfSet) {
        return GOAL_ERR_UNSUPPORTED;
    }

    pMaSpi->cfg.type = type;
    return pMaSpi->fctConfSet(pMaSpi);
}


/****************************************************************************/
/** SPI Configure mode
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maSpiCfgModeSet(
    GOAL_MA_SPI_T *pMaSpi,                      /**< MA handle for SPI */
    GOAL_MA_SPI_MODE_T mode                     /**< SPI mode */
)
{
    if (NULL == pMaSpi) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (NULL == pMaSpi->fctConfSet) {
        return GOAL_ERR_UNSUPPORTED;
    }

    pMaSpi->cfg.mode = mode;
    return pMaSpi->fctConfSet(pMaSpi);
}


/****************************************************************************/
/** SPI Configure unit width
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maSpiCfgUnitWidthSet(
    GOAL_MA_SPI_T *pMaSpi,                      /**< MA handle for SPI */
    GOAL_MA_SPI_UNITWIDTH_T valUnitWidth        /**< SPI unit width */
)
{
    if (NULL == pMaSpi) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (NULL == pMaSpi->fctConfSet) {
        return GOAL_ERR_UNSUPPORTED;
    }

    pMaSpi->cfg.unitWidth = valUnitWidth;
    return pMaSpi->fctConfSet(pMaSpi);
}


/****************************************************************************/
/** SPI Configure bit order
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maSpiCfgBitOrderSet(
    GOAL_MA_SPI_T *pMaSpi,                      /**< MA handle for SPI */
    GOAL_MA_SPI_BITORDER_T valBitOrder          /**< SPI bit order */
)
{
    if (NULL == pMaSpi) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (NULL == pMaSpi->fctConfSet) {
        return GOAL_ERR_UNSUPPORTED;
    }

    pMaSpi->cfg.bitorder = valBitOrder;
    return pMaSpi->fctConfSet(pMaSpi);
}


/****************************************************************************/
/** SPI Reset Chip
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maSpiReset(
    GOAL_MA_SPI_T *pMaSpi                       /**< MA handle for SPI */
)
{
    if (NULL == pMaSpi) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (NULL == pMaSpi->fctReset) {
        return GOAL_ERR_UNSUPPORTED;
    }

    return pMaSpi->fctReset(pMaSpi);
}
