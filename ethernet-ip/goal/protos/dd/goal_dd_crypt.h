/** @file
 *
 * @brief Header for device detection
 *
 * @copyright
 * Copyright 2013-2021 port GmbH Halle/Saale.
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

#ifndef GOAL_DD_CRYPT_H
#define GOAL_DD_CRYPT_H


/****************************************************************************/
/* Dependencies */
/****************************************************************************/
#include "goal_dd.h"

#if 1 == GOAL_CONFIG_DD_CRYPT
#  include "goal_media/goal_mi_crypt.h"
#endif


/****************************************************************************/
/* Typedefs */
/****************************************************************************/
#if 1 == GOAL_CONFIG_DD_CRYPT
typedef struct {
    GOAL_BOOL_T flgActivated;                   /**< use encryption for packages */
    GOAL_MI_CRYPT_T *pMiCrypt;                  /**< MI crypt handle */
    uint8_t *pKey;                              /**< key for symmetric crypto algorithm */
    uint16_t sizeKey;                           /**< size of key in bytes */
} GOAL_DD_CRYPT_T;
#endif /* 1 == GOAL_CONFIG_DD_CRYPT */


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
#if 1 == GOAL_CONFIG_DD_CRYPT
GOAL_STATUS_T goal_ddCryptInit(
    GOAL_DD_T *pHdlDd                           /**< dd handle */
);

GOAL_STATUS_T goal_ddCryptInfoGet(
    GOAL_DD_CRYPT_T *pCrypt,                    /**< crypto instance of dd */
    uint8_t *pBuf,                              /**< message buffer */
    GOAL_BOOL_T *pFlgActivated,                 /**< encryption activated flag buffer */
    uint16_t *pSizeCrypt                        /**< number of bytes additional needed by encryption */
);

GOAL_STATUS_T goal_ddCryptEncrypt(
    GOAL_DD_CRYPT_T *pCrypt,                    /**< crypto instance of dd */
    uint8_t *pOutBuf,                           /**< buffer for encrypted data */
    uint16_t *pSizeOut,                         /**< size of encrypted data including authentication tag in bytes */
    uint8_t *pInBuf,                            /**< data to encrypt */
    uint16_t sizeIn                             /**< size of data to encrypt in bytes */
);

GOAL_STATUS_T goal_ddCryptDecrypt(
    GOAL_DD_CRYPT_T *pCrypt,                    /**< crypto instance of dd */
    uint8_t *pOutBuf,                           /**< buffer for denrypted data */
    uint16_t *pSizeOut,                         /**< size of decrypted data in  bytes */
    uint8_t *pInBuf,                            /**< data + authentication tag to encrypt */
    uint16_t sizeIn                             /**< size of data + authentication tag to encrypt in bytes */
);
#endif /* 1 == GOAL_CONFIG_DD_CRYPT */



GOAL_STATUS_T goal_ddCryptSettingsUpdate(
    GOAL_DD_T *pHdlDd                           /**< dd handle */
);

GOAL_STATUS_T goal_ddCryptActivatedValidateCb(
    uint32_t modId,                             /**< module ID */
    uint32_t varId,                             /**< variable Id */
    GOAL_CM_VAR_T *pVar,                        /**< variable pointer */
    void *pNewData,                             /**< pointer of data to check */
    uint32_t size                               /**< size of data */
);

#endif /* GOAL_DD_CRYPT_H */
