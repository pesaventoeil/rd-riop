/** @file
 *
 * @brief NXP SDK Package for SFlash Driver
 *
 * This module provides read and write access to the serial flash via xSPI.
 *
 * @copyright
 * Copyright 2022.
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
#ifndef GOAL_DRV_NVS_SFLASH_FSL_H
#define GOAL_DRV_NVS_SFLASH_FSL_H
#include <goal_includes.h>
#include <goal_media/goal_ma_nvs.h>
#include "plat/drv/nvs/goal_drv_nvs.h"

#if RIOP
#include "flexspi_ops.h"
#else
#include "flexspi_defaults.h"
#endif
#include "fsl_flexspi.h"


/****************************************************************************/
/* Defines */
/****************************************************************************/
#define CUSTOM_LUT_LENGTH        60

typedef struct {
    FLEXSPI_Type *pBase;
    flexspi_device_config_t *deviceconfig;
#if RIOP
    const uint32_t *customLUT;
#else
    uint32_t *customLUT;
#endif
} spi_flash_instance_t;


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_drvNvsSflashFslReg(
    GOAL_ID_T id,                               /**< MA id */
    GOAL_MA_NVS_INFO_T *pInfoNvs,               /**< NVS information */
    uint8_t *pAddrNvs,                          /**< address offset to NVS section */
    FLEXSPI_Type *pfsl                          /**< sflash base */
);

/* prototypes for ops */
status_t flexspi_nor_check_bus_busy(
    FLEXSPI_Type *pBase,                        /**< flexspi instance */
    bool *pBusy                                 /**< currently busy */
);

status_t flexspi_nor_enable_quad_mode(
    FLEXSPI_Type *pBase                         /**< flexspi instance */
);

status_t flexspi_nor_erase_chip(
    FLEXSPI_Type *pBase                         /**< flexspi instance */
);

status_t flexspi_nor_flash_erase_sector(
    FLEXSPI_Type *pBase,                        /**< flexspi instance */
    uint32_t address                            /**< flash address to erase */
);

void flexspi_nor_flash_init(
    FLEXSPI_Type *pBase                         /**< flexspi instance */
);

status_t flexspi_nor_flash_page_program(
    FLEXSPI_Type *pBase,                        /**< flexspi instance */
    uint32_t dstAddr,                           /**< flash address to program */
    const uint32_t *pSrc                        /**< source data */
);

status_t flexspi_nor_flash_program(
    FLEXSPI_Type *pBase,                        /**< flexspi instance */
    uint32_t dstAddr,                           /**< flash address to program */
    const uint32_t *pSrc,                       /**< source data */
    uint32_t length                             /**< length of data */
);

status_t flexspi_nor_get_vendor_id(
    FLEXSPI_Type *pBase,                        /**< flexspi instance */
    uint8_t *pVendorId                          /**< [out] vendor ID */
);

status_t flexspi_nor_read_data(
    FLEXSPI_Type *pBase,                        /**< flexspi instance */
    uint32_t startAddress,                      /**< start address to read */
    uint32_t *pBuffer,                          /**< [out] data */
    uint32_t length                             /**< length of data */
);

status_t flexspi_nor_wait_bus_busy(
    FLEXSPI_Type *pBase                         /**< flexspi instance */
);

status_t flexspi_nor_write_enable(
    FLEXSPI_Type *pBase,                        /**< flexspi instance */
    uint32_t baseAddr                           /**< base address */
);

#endif /* GOAL_DRV_NVS_SFLASH_FSL_H */
