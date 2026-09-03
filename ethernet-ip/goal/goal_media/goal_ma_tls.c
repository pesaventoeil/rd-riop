/** @file
 *
 * @brief GOAL media adapter for TLS
 *
 * @details
 * This GOAL component provides functionality for encryption and authentication
 * of TCP packets according to the Transport Layer Security (TLS) protocol.
 *
 * This media adapter represents a generic interface for different GOAL TLS drivers.
 * Different GOAL TLS drivers allow to integrate various TLS libraries in the GOAL system.
 *
 * The certificate information includes the private key and the X509-certificate.
 *
 * The TLS component requires an operating system. The algotithm for
 * encryption/decryption needs some time and is executed in an own task
 * with low priority.
 *
 *
 * @copyright
 * Copyright 2010-2021.
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

#define GOAL_ID GOAL_ID_MA_TLS

#include <goal_includes.h>
#include <goal_media/goal_ma_tls.h>


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_INSTANCE_LIST_T *mpListInst;        /**< ma tls instance list */


/****************************************************************************/
/** Register GOAL TLS driver
 *
 * This function creates a MA-TLS handle and registers the callback functions
 * of the GOAL TLS driver. These callbacks are called if the corresponding
 * MA function is called.
 *
 * The MA-ID is specified by the application.
 * This function must be called in state GOAL_FSA_INIT_APPL or GOAL_FSA_INIT_GOAL.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maTlsDrvReg(
    uint32_t id,                                /**< MA-ID */
    GOAL_MA_TLS_T **ppMaTls,                    /**< [out] MA handle for TLS */
    GOAL_MA_TLS_OPEN_T fOpen,                   /**< driver open function */
    GOAL_MA_TLS_CLOSE_T fClose,                 /**< driver close function */
    GOAL_MA_TLS_SESSION_OPEN_T fSessOpen,       /**< driver session open function */
    GOAL_MA_TLS_SESSION_CLOSE_T fSessClose,     /**< driver session close function */
    GOAL_MA_TLS_SESSION_HS_T fSessHs,           /**< driver handshake step function */
    GOAL_MA_TLS_SESSION_DECRYPT_T fSessDecrypt, /**< driver decrypt data function */
    GOAL_MA_TLS_SESSION_ENCRYPT_T fSessEncrypt, /**< driver sending function */
    void *pDrvData,                             /**< [in] reserved driver data */
    uint32_t sizeStackMax                       /**< maximum TLS stack size */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MA_TLS_T *pMaTls = NULL;               /* TLS MA */

    /* check pointers */
    if (NULL == ppMaTls) {
        goal_lmLog(GOAL_ID_MA_TLS, 494, 409, 0, 0, GOAL_LOG_SEV_ERROR, "NULL pointer error");
        return GOAL_ERR_NULL_POINTER;
    }

    /* id already in use? */
    res = goal_maTlsGetById(&pMaTls, id);
    if (GOAL_RES_OK(res)) {
        goal_lmLog(GOAL_ID_MA_TLS, 510, 425, 0, 0, GOAL_LOG_SEV_ERROR, "TLS MA with ID is already in use");
        return GOAL_ERR_ALREADY_USED;
    }

    /* create instance list */
    if (NULL == mpListInst) {
        res = goal_instListNew(&mpListInst, GOAL_ID);
    }

    /* create instance */
    if (GOAL_RES_OK(res)) {
        res = goal_instNew((GOAL_INSTANCE_T **) &pMaTls, sizeof(GOAL_MA_TLS_T), GOAL_ID, id, "goal_ma_TLS");
    }

    /* return handle */
    if (GOAL_RES_OK(res)) {
        pMaTls->id = id;
        pMaTls->fOpen = fOpen;
        pMaTls->fClose = fClose;
        pMaTls->fSessOpen = fSessOpen;
        pMaTls->fSessHs = fSessHs;
        pMaTls->fSessClose = fSessClose;
        pMaTls->fSessDecrypt = fSessDecrypt;
        pMaTls->fSessEncrypt = fSessEncrypt;
        pMaTls->pDrvData = pDrvData;
        pMaTls->sizeStackMax = sizeStackMax;

        *ppMaTls = pMaTls;
    }

    return res;
}


/****************************************************************************/
/** Open GOAL TLS driver
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maTlsOpen(
    GOAL_MA_TLS_T *pMaTls                       /**< [in] MA-TLS handle */
)
{
    GOAL_STATUS_T res;                          /* result */

    /* check handle */
    if ((NULL == pMaTls) || (NULL == pMaTls->fOpen)) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (GOAL_TRUE == pMaTls->flgOpened) {
        /* already opened */
        return GOAL_OK;
    }

    res = pMaTls->fOpen(pMaTls);

    if (GOAL_RES_OK(res)) {
        pMaTls->flgOpened = GOAL_TRUE;
    }

    return res;
}


/****************************************************************************/
/** Close GOAL TLS driver
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maTlsClose(
    GOAL_MA_TLS_T *pMaTls                       /**< [in] MA-TLS handle */
)
{
    GOAL_STATUS_T res;                          /* result */

    /* check handle */
    if ((NULL == pMaTls) || (NULL == pMaTls->fClose)) {
        return GOAL_ERR_NULL_POINTER;
    }

    if (GOAL_FALSE == pMaTls->flgOpened) {
        /* already closed */
        return GOAL_OK;
    }

    res = pMaTls->fClose(pMaTls);

    if (GOAL_RES_OK(res)) {
        pMaTls->flgOpened = GOAL_FALSE;
    }

    return res;
}


/****************************************************************************/
/** Creates empty TLS MA session
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maTlsSessionCreate(
    GOAL_MA_TLS_T *pMaTls,                      /**< [in] MA-TLS handle */
    GOAL_MA_TLS_SESSION_T **ppMaSess            /**< [out] MA TLS session */
)
{
    GOAL_MA_TLS_SESSION_T *pMaSess = NULL;      /* MA TLS session */
    GOAL_STATUS_T res;                          /* result */

    res = goal_memCalloc(&pMaSess, sizeof(GOAL_MA_TLS_SESSION_T));
    if (GOAL_RES_OK(res)) {
        pMaSess->pMaTls = pMaTls;
        *ppMaSess = pMaSess;
    }

    return res;
}


/****************************************************************************/
/** Deletes TLS MA session
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maTlsSessionFree(
    GOAL_MA_TLS_T *pMaTls,                      /**< [in] MA-TLS handle */
    GOAL_MA_TLS_SESSION_T **ppMaSess            /**< [out] MA TLS session */
)
{
    UNUSEDARG(pMaTls);

    if (NULL == ppMaSess) {
        return GOAL_ERR_NULL_POINTER;
    }

    return goal_memFree(ppMaSess);
}


/****************************************************************************/
/** Clear TLS session in driver
 *
 * Clears a TLS session startes of the driver.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maTlsSessionOpen(
    GOAL_MA_TLS_T *pMaTls,                      /**< [in] MA-TLS handle */
    GOAL_MA_TLS_SESSION_T *pMaSess              /**< [in] MA TLS session */
)
{
    if ((NULL == pMaTls) || (NULL == pMaTls->fSessOpen) || (NULL == pMaSess)) {
        return GOAL_ERR_NULL_POINTER;
    }

    return pMaTls->fSessOpen(pMaTls, pMaSess);
}


/****************************************************************************/
/** Close TLS session in driver
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maTlsSessionClose(
    GOAL_MA_TLS_T *pMaTls,                      /**< [in] MA-TLS handle */
    GOAL_MA_TLS_SESSION_T *pMaSess              /**< [in] MA TLS session */
)
{
    if ((NULL == pMaTls) || (NULL == pMaTls->fSessClose) || (NULL == pMaSess)) {
        return GOAL_ERR_NULL_POINTER;
    }

    return pMaTls->fSessClose(pMaTls, pMaSess);
}


/****************************************************************************/
/** Do a handshake step in the driver
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maTlsSessionHs(
    GOAL_MA_TLS_T *pMaTls,                      /**< [in] MA-TLS handle */
    GOAL_MA_TLS_SESSION_T *pMaSess              /**< [in] MA TLS session */
)
{
    UNUSEDARG(pMaTls);

    if ((NULL == pMaSess) || (NULL == pMaTls->fSessHs)) {
        return GOAL_ERR_NULL_POINTER;
    }

    return pMaTls->fSessHs(pMaTls, pMaSess);
}


/****************************************************************************/
/** Decrypt data
 *
 * This function forwards received data to the TLS driver for encryption.
 * Encrypted data are processed further by the TLS driver.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maTlsSessionDecrypt(
    GOAL_MA_TLS_T *pMaTls,                      /**< [in] MA-TLS handle */
    GOAL_MA_TLS_SESSION_T *pMaSess              /**< [in] MA TLS session */
)
{
    UNUSEDARG(pMaTls);

    if (NULL == pMaSess) {
        return GOAL_ERR_NULL_POINTER;
    }

    return pMaTls->fSessDecrypt(pMaTls, pMaSess);
}


/****************************************************************************/
/** Encrypt data
 *
 * This function forward data to encrypt for sending.
 * Encrypted data are processed further by the TLS driver.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maTlsSessionEncrypt(
    GOAL_MA_TLS_T *pMaTls,                      /**< [in] MA-TLS handle */
    GOAL_MA_TLS_SESSION_T *pMaSess,             /**< [in] MA TLS session */
    GOAL_BUFFER_T *pBuf                         /**< [in] buffer with data to send */
)
{
    UNUSEDARG(pMaTls);

    if ((NULL == pMaTls) || (NULL == pMaTls->fSessEncrypt) || (NULL == pMaSess) || (NULL == pBuf)) {
        return GOAL_ERR_NULL_POINTER;
    }

    return pMaTls->fSessEncrypt(pMaTls, pMaSess, pBuf);
}


/****************************************************************************/
/** Register MI Callback Function into MA
 *
 * The registered MI functions are called by the driver.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maTlsCbReg(
    GOAL_MA_TLS_T *pMaTls,                      /**< [in] MA handle */
    GOAL_MA_TLS_CB_DYN_MEM_ALLOC cbAlloc,       /**< cb func alloc */
    GOAL_MA_TLS_CB_DYN_MEM_FREE cbFree,         /**< cb func free */
    GOAL_MA_TLS_CB_HS_DONE_T cbHsDone,          /**< cb func handshake done */
    GOAL_MA_TLS_CB_RECV_T cbRecv,               /**< cb func receive */
    GOAL_MA_TLS_CB_SEND_T cbSend,               /**< cb func send */
    GOAL_MA_TLS_CB_CERT_GET_T cbCertGet,        /**< cb certificate get */
    GOAL_MA_TLS_CB_DECRYPTED_RX_T cbDecryptedDataRx /**< cb func decrypted data receive */
)
{
    /* check pointer */
    if (NULL == pMaTls) {
        return GOAL_ERR_NULL_POINTER;
    }

    pMaTls->cbAlloc = cbAlloc;
    pMaTls->cbFree = cbFree;
    pMaTls->cbHsDone = cbHsDone;
    pMaTls->cbRecv = cbRecv;
    pMaTls->cbSend = cbSend;
    pMaTls->cbCertGet = cbCertGet;
    pMaTls->cbDecryptedDataRx = cbDecryptedDataRx;

    return GOAL_OK;
}


/****************************************************************************/
/** Request dynamic memory from MI
 *
 * This forwards memory request from driver to MI.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maTlsCbAlloc(
    struct GOAL_MA_TLS_T *pMaTls,               /**< [in] MA-TLS handle */
    void **ppMem,                               /**< [out] ptr to store buf ptr to */
    uint32_t memLen                             /**< size of buffer to alloc */
)
{
    GOAL_STATUS_T res;                          /* result */

    /* check handle */
    if ((NULL == pMaTls) || (NULL == pMaTls->cbAlloc) || (NULL == ppMem)) {
        return GOAL_ERR_NULL_POINTER;
    }

    res = pMaTls->cbAlloc(pMaTls, ppMem, memLen);

    return res;
}


/****************************************************************************/
/** Request to free dynamic memory in MI
 *
 * This forwards free request from driver to MI.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maTlsCbFree(
    struct GOAL_MA_TLS_T *pMaTls,               /**< [in] MA-TLS handle */
    void **ppMem                                /**< [in,out] ptr to store buf ptr to */
)
{
    GOAL_STATUS_T res;                          /* result */

    /* check handle */
    if ((NULL == pMaTls) || (NULL == pMaTls->cbFree) || (NULL == ppMem)) {
        return GOAL_ERR_NULL_POINTER;
    }

    res = pMaTls->cbFree(pMaTls, ppMem);

    return res;
}


/****************************************************************************/
/** Inform MI of done handshake
 *
 * This function is called by the driver if the TLS handshake is done and the
 * connection is opened completly for communication.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maTlsSessCbHsDone(
    GOAL_MA_TLS_SESSION_T *pMaSess              /**< [in] MA TLS session */
)
{
    /* check handle */
    if ((NULL == pMaSess) || (NULL == pMaSess->pMaTls) || (NULL == pMaSess->pMaTls->cbHsDone)) {
        return GOAL_ERR_NULL_POINTER;
    }

    pMaSess->pMaTls->cbHsDone(pMaSess->pMaTls, pMaSess);

    return GOAL_OK;
}


/****************************************************************************/
/** Inform MI of new decrypted data
 *
 * This function is called by the driver if the received data are decrypted.
 * Decrypted data can be received by other GOAL layers.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maTlsSessCbRecv(
    struct GOAL_MA_TLS_T *pMaTls,               /**< [in] MA TLS handle */
    struct GOAL_MA_TLS_SESSION_T *pMaSess,      /**< [in] MA TLS session handle */
    uint8_t *pBuf,                              /**< [in] buffer */
    uint16_t len,                               /**< size of buffer in bytes */
    uint16_t *pLenRead                          /**< [out] read length in bytes */
)
{
    UNUSEDARG(pMaTls);

    /* check handle */
    if ((NULL == pMaSess) || (NULL == pMaSess->pMaTls) || (NULL == pMaSess->pMaTls->cbRecv) || (NULL == pBuf) || (NULL == pLenRead)) {
        return GOAL_ERR_NULL_POINTER;
    }

    return pMaSess->pMaTls->cbRecv(pMaSess->pMaTls, pMaSess, pBuf, len, pLenRead);
}


/****************************************************************************/
/** Inform MI of new encrypted data
 *
 * This function is called by the driver if data to send are encrypted.
 * Encrypted data can be sent to other GOAL layers.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maTlsSessCbSend(
    struct GOAL_MA_TLS_T *pMaTls,               /**< [in] MA TLS handle */
    struct GOAL_MA_TLS_SESSION_T *pMaSess,      /**< [in] MA TLS session handle */
    const uint8_t *pBuf,                        /**< [in] buffer */
    uint16_t len,                               /**< size of buffer in bytes */
    uint16_t *pLenSent                          /**< [out] sent length in bytes */
)
{
    UNUSEDARG(pMaTls);

    if ((NULL == pMaSess) || (NULL == pMaSess->pMaTls) ||
        (NULL == pMaSess->pMaTls->cbSend) || (NULL == pBuf) ||
        (NULL == pLenSent)) {
        /* check handle */
        return GOAL_ERR_NULL_POINTER;
    }

    return pMaSess->pMaTls->cbSend(pMaSess->pMaTls, pMaSess, pBuf, len, pLenSent);
}


/****************************************************************************/
/** Inform MI of new decrypted data
 *
 * This function is called by the driver if received data was decrypted.
 * Decrypted data can be sent to other GOAL layers.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maTlsSessCbDecryptedDataRx(
    struct GOAL_MA_TLS_SESSION_T *pMaSess,      /**< [in] MA TLS session handle */
    GOAL_BUFFER_T *pBuf                         /**< [in] buffer with decrypted data */
)
{
    if ((NULL == pMaSess) || (NULL == pMaSess->pMaTls) ||
        (NULL == pMaSess->pMaTls->cbDecryptedDataRx) || (NULL == pBuf)) {
        /* check handle */
        return GOAL_ERR_NULL_POINTER;
    }

    return pMaSess->pMaTls->cbDecryptedDataRx(pMaSess->pMaTls, pMaSess, pBuf);
}


/****************************************************************************/
/** Request certificae and key from MI
 *
 * This function is called by the driver if the TLs certificate and its private
 * key is needed.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_maTlsCertGet(
    struct GOAL_MA_TLS_T *pMaTls,               /**< [in] MA TLS handle */
    const uint8_t **ppCert,                     /**< [out] TLS certificate, including public key */
    const uint8_t **ppKeyPriv,                  /**< [out] private key */
    uint32_t *pSizeCert,                        /**< [out] TLS certificate size */
    uint32_t *pSizeKeyPriv                      /**< [out] private key size */
)
{

    /* check handle */
    if ((NULL == pMaTls) || (NULL == pMaTls->cbCertGet)) {
        return GOAL_ERR_NULL_POINTER;
    }

    return pMaTls->cbCertGet(pMaTls, ppCert, ppKeyPriv, pSizeCert, pSizeKeyPriv);
}


/****************************************************************************/
/** Get MA by ID
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_maTlsGetById(
    GOAL_MA_TLS_T **ppMaTls,                    /**< [out] MA handle for TLS */
    uint32_t id                                 /**< MA id */
)
{
    return goal_instGetById((GOAL_INSTANCE_T **) ppMaTls, GOAL_ID, id);
}
