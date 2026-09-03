/** @file
 *
 * @brief GOAL media adapter for TLS
 *
 * @details
 * This GOAL component provides functionality for encryption and authentication
 * of TCP packets according to the Transport Layer Security (TLS) protocol.
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

#ifndef GOAL_MA_TLS_H
#define GOAL_MA_TLS_H


/****************************************************************************/
/* Global defines */
/****************************************************************************/
#ifndef GOAL_MA_TLS_ID_DEFAULT
#  define GOAL_MA_TLS_ID_DEFAULT 0              /**< default default TLS MA ID */
#endif


/****************************************************************************/
/* Forward declaration */
/****************************************************************************/
struct GOAL_MA_TLS_T;
struct GOAL_MA_TLS_SESSION_T;


/****************************************************************************/
/* Function types */
/****************************************************************************/
typedef GOAL_STATUS_T (*GOAL_MA_TLS_OPEN_T)(
    struct GOAL_MA_TLS_T *pMaTls                /**< [in] MA TLS handle */
);


typedef GOAL_STATUS_T (*GOAL_MA_TLS_CLOSE_T)(
    struct GOAL_MA_TLS_T *pMaTls                /**< [in] MA TLS handle */
);

typedef GOAL_STATUS_T (*GOAL_MA_TLS_SESSION_OPEN_T)(
    struct GOAL_MA_TLS_T *pMaTls,               /**< [in] MA TLS handle */
    struct GOAL_MA_TLS_SESSION_T *pMaSess       /**< [in] MA TLS session handle */
);

typedef GOAL_STATUS_T (*GOAL_MA_TLS_SESSION_CLOSE_T)(
    struct GOAL_MA_TLS_T *pMaTls,               /**< [in] MA TLS handle */
    struct GOAL_MA_TLS_SESSION_T *pMaSess       /**< [in] MA TLS session handle */
);

typedef GOAL_STATUS_T (*GOAL_MA_TLS_SESSION_HS_T)(
    struct GOAL_MA_TLS_T *pMaTls,               /**< [in] MA TLS handle */
    struct GOAL_MA_TLS_SESSION_T *pMaSess       /**< [in] MA TLS session handle */
);

typedef GOAL_STATUS_T (*GOAL_MA_TLS_SESSION_DECRYPT_T)(
    struct GOAL_MA_TLS_T *pMaTls,               /**< [in] MA TLS handle */
    struct GOAL_MA_TLS_SESSION_T *pMaSess       /**< [in] MA TLS session handle */
);

typedef GOAL_STATUS_T (*GOAL_MA_TLS_SESSION_ENCRYPT_T)(
    struct GOAL_MA_TLS_T *pMaTls,               /**< [in] MA TLS handle */
    struct GOAL_MA_TLS_SESSION_T *pMaSess,      /**< [in] MA TLS session handle */
    GOAL_BUFFER_T *pBuf                         /**< [in] buffer with data to send */
);

typedef GOAL_STATUS_T (*GOAL_MA_TLS_CB_DYN_MEM_ALLOC)(
    struct GOAL_MA_TLS_T *pMaTls,               /**< [in] MA TLS handle */
    void **ppMem,                               /**< [out] ptr to store buf ptr to */
    uint32_t memLen                             /**< size of buffer to alloc */
);

typedef GOAL_STATUS_T (*GOAL_MA_TLS_CB_DYN_MEM_FREE)(
    struct GOAL_MA_TLS_T *pMaTls,               /**< [in] MA TLS handle */
    void **ppMem                                /**< [in/out] ptr to store buf ptr to */
);

typedef GOAL_STATUS_T (*GOAL_MA_TLS_CB_HS_DONE_T)(
    struct GOAL_MA_TLS_T *pMaTls,               /**< [in] MA TLS handle */
    struct GOAL_MA_TLS_SESSION_T *pMaSess       /**< [in] MA TLS session handle */
);

typedef GOAL_STATUS_T (*GOAL_MA_TLS_CB_RECV_T)(
    struct GOAL_MA_TLS_T *pMaTls,               /**< [in] MA TLS handle */
    struct GOAL_MA_TLS_SESSION_T *pMaSess,      /**< [in] MA TLS session handle */
    uint8_t *pBuf,                              /**< [in] buffer */
    uint16_t len,                               /**< size of buffer in bytes */
    uint16_t *pLenRead                          /**< [out] read length in bytes */
);

typedef GOAL_STATUS_T (*GOAL_MA_TLS_CB_SEND_T)(
    struct GOAL_MA_TLS_T *pMaTls,               /**< [in] MA TLS handle */
    struct GOAL_MA_TLS_SESSION_T *pMaSess,      /**< [in] MA TLS session handle */
    const uint8_t *pBuf,                        /**< [in] buffer */
    uint16_t len,                               /**< size of buffer in bytes */
    uint16_t *pLenSent                          /**< [out] sent length in bytes */
);

typedef GOAL_STATUS_T (*GOAL_MA_TLS_CB_DECRYPTED_RX_T)(
    struct GOAL_MA_TLS_T *pMaTls,               /**< [in] MA TLS handle */
    struct GOAL_MA_TLS_SESSION_T *pMaSess,      /**< [in] MA TLS session handle */
    GOAL_BUFFER_T *pBuf                         /**< [in] GOAL buffer */
);

typedef GOAL_STATUS_T (*GOAL_MA_TLS_CB_CERT_GET_T)(
    struct GOAL_MA_TLS_T *pMaTls,               /**< [in] MA TLS handle */
    const uint8_t **ppCert,                     /**< [out] TLS certificate, including public key */
    const uint8_t **ppKeyPriv,                  /**< [out] private key */
    uint32_t *pSizeCert,                        /**< [out] TLS certificate size */
    uint32_t *pSizeKeyPriv                      /**< [out] private key size */
);


/****************************************************************************/
/* Structures */
/****************************************************************************/
typedef struct GOAL_MA_TLS_T {
    struct GOAL_MA_TLS_T *pNext;                /**< next element */
    unsigned int id;                            /**< MA ID */
    const char *strInstDesc;                    /**< instance description */
    GOAL_MA_TLS_CB_DYN_MEM_ALLOC cbAlloc;       /**< cb func alloc */
    GOAL_MA_TLS_CB_DYN_MEM_FREE cbFree;         /**< cb func free */
    GOAL_MA_TLS_CB_HS_DONE_T cbHsDone;          /**< cb func handshake done */
    GOAL_MA_TLS_CB_RECV_T cbRecv;               /**< cb func receive */
    GOAL_MA_TLS_CB_SEND_T cbSend;               /**< cb func receive */
    GOAL_MA_TLS_CB_DECRYPTED_RX_T cbDecryptedDataRx; /**< cb func decrypted data receive */
    GOAL_MA_TLS_CB_CERT_GET_T cbCertGet;        /**< cb func get certificate */
    GOAL_BOOL_T flgOpened;                      /**< opened flag */
    GOAL_MA_TLS_OPEN_T fOpen;                   /**< driver open function */
    GOAL_MA_TLS_CLOSE_T fClose;                 /**< driver close function */
    GOAL_MA_TLS_SESSION_OPEN_T fSessOpen;       /**< driver session open function */
    GOAL_MA_TLS_SESSION_CLOSE_T fSessClose;     /**< driver session close function */
    GOAL_MA_TLS_SESSION_HS_T fSessHs;           /**< driver handshake step function */
    GOAL_MA_TLS_SESSION_DECRYPT_T fSessDecrypt; /**< driver decrypt data function */
    GOAL_MA_TLS_SESSION_ENCRYPT_T fSessEncrypt; /**< driver sending function */
    uint32_t sizeStackMax;                      /**< maximum TLS stack size */
    void *pDrvData;                             /**< driver specific data */
    void *pMiTls;                               /**< MI instance */
} GOAL_MA_TLS_T;


typedef struct GOAL_MA_TLS_SESSION_T {
    struct GOAL_MA_TLS_T *pMaTls;               /**< MA TLS belonging to session */
    struct GOAL_MI_TLS_SESSION_T *pMiSess;      /**< MI session handle belonging to session */
    void *pDrvSess;                             /**< TLS driver session */
} GOAL_MA_TLS_SESSION_T;


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_maTlsDrvReg(
    uint32_t id,                                /**< [in] MA-ID */
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
);

GOAL_STATUS_T goal_maTlsOpen(
    GOAL_MA_TLS_T *pMaTls                       /**< [in] MA-TLS handle */
);

GOAL_STATUS_T goal_maTlsClose(
    GOAL_MA_TLS_T *pMaTls                       /**< [in] MA-TLS handle */
);

GOAL_STATUS_T goal_maTlsSessionCreate(
    GOAL_MA_TLS_T *pMaTls,                      /**< [in] MA-TLS handle */
    GOAL_MA_TLS_SESSION_T **ppMaSess            /**< [out] MA TLS session */
);

GOAL_STATUS_T goal_maTlsSessionFree(
    GOAL_MA_TLS_T *pMaTls,                      /**< [in] MA-TLS handle */
    GOAL_MA_TLS_SESSION_T **ppMaSess            /**< [out] MA TLS session */
);

GOAL_STATUS_T goal_maTlsSessionOpen(
    GOAL_MA_TLS_T *pMaTls,                      /**< [in] MA-TLS handle */
    GOAL_MA_TLS_SESSION_T *pMaSess              /**< [in] MA TLS session */
);

GOAL_STATUS_T goal_maTlsSessionClose(
    GOAL_MA_TLS_T *pMaTls,                      /**< [in] MA-TLS handle */
    GOAL_MA_TLS_SESSION_T *pMaSess              /**< [in] MA TLS session */
);

GOAL_STATUS_T goal_maTlsSessionHs(
    GOAL_MA_TLS_T *pMaTls,                      /**< [in] MA-TLS handle */
    GOAL_MA_TLS_SESSION_T *pMaSess              /**< [in] MA TLS session */
);

GOAL_STATUS_T goal_maTlsSessionDecrypt(
    GOAL_MA_TLS_T *pMaTls,                      /**< [in] MA-TLS handle */
    GOAL_MA_TLS_SESSION_T *pMaSess              /**< [in] MA TLS session */
);

GOAL_STATUS_T goal_maTlsSessionEncrypt(
    GOAL_MA_TLS_T *pMaTls,                      /**< [in] MA-TLS handle */
    GOAL_MA_TLS_SESSION_T *pMaSess,             /**< [in] MA TLS session */
    GOAL_BUFFER_T *pBuf                         /**< [in] buffer with data to send */
);

GOAL_STATUS_T goal_maTlsCbReg(
    GOAL_MA_TLS_T *pMaTls,                      /**< [in] MA handle */
    GOAL_MA_TLS_CB_DYN_MEM_ALLOC cbAlloc,       /**< cb func alloc */
    GOAL_MA_TLS_CB_DYN_MEM_FREE cbFree,         /**< cb func free */
    GOAL_MA_TLS_CB_HS_DONE_T cbHsDone,          /**< cb func handshake done */
    GOAL_MA_TLS_CB_RECV_T cbRecv,               /**< cb func receive */
    GOAL_MA_TLS_CB_SEND_T cbSend,               /**< cb func send */
    GOAL_MA_TLS_CB_CERT_GET_T cbCertGet,        /**< cb certificate get */
    GOAL_MA_TLS_CB_DECRYPTED_RX_T cbDecryptedDataRx /**< cb func decrypted data receive */
);

GOAL_STATUS_T goal_maTlsCbAlloc(
    struct GOAL_MA_TLS_T *pMaTls,               /**< [in] MA-TLS handle */
    void **ppMem,                               /**< [out] ptr to store buf ptr to */
    uint32_t memLen                             /**< size of buffer to alloc */
);

GOAL_STATUS_T goal_maTlsCbFree(
    struct GOAL_MA_TLS_T *pMaTls,               /**< [in] MA-TLS handle */
    void **ppMem                                /**< [in,out] ptr to store buf ptr to */
);

GOAL_STATUS_T goal_maTlsSessCbHsDone(
    GOAL_MA_TLS_SESSION_T *pMaSess              /**< [in] MA TLS session */
);

GOAL_STATUS_T goal_maTlsSessCbRecv(
    struct GOAL_MA_TLS_T *pMaTls,               /**< [in] MA TLS handle */
    struct GOAL_MA_TLS_SESSION_T *pMaSess,      /**< [in] MA TLS session handle */
    uint8_t *pBuf,                              /**< [in] buffer */
    uint16_t len,                               /**< size of buffer in bytes */
    uint16_t *pLenRead                          /**< [out] read length in bytes */
);

GOAL_STATUS_T goal_maTlsSessCbSend(
    struct GOAL_MA_TLS_T *pMaTls,               /**< [in] MA TLS handle */
    struct GOAL_MA_TLS_SESSION_T *pMaSess,      /**< [in] MA TLS session handle */
    const uint8_t *pBuf,                        /**< [in] buffer */
    uint16_t len,                               /**< size of buffer in bytes */
    uint16_t *pLenSent                          /**< [out] sent length in bytes */
);

GOAL_STATUS_T goal_maTlsSessCbDecryptedDataRx(
    struct GOAL_MA_TLS_SESSION_T *pMaSess,      /**< [in] MA TLS session handle */
    GOAL_BUFFER_T *pBuf                         /**< [in] buffer with decrypted data */
);

GOAL_STATUS_T goal_maTlsCertGet(
    struct GOAL_MA_TLS_T *pMaTls,               /**< [in] MA TLS handle */
    const uint8_t **ppCert,                     /**< [out] TLS certificate, including public key */
    const uint8_t **ppKeyPriv,                  /**< [out] private key */
    uint32_t *pSizeCert,                        /**< [out] TLS certificate size */
    uint32_t *pSizeKeyPriv                      /**< [out] private key size */
);

GOAL_STATUS_T goal_maTlsGetById(
    GOAL_MA_TLS_T **ppMaTls,                    /**< [out] MA handle for TLS */
    uint32_t id                                 /**< MA id */
);

#endif /* GOAL_MA_TLS_H */

