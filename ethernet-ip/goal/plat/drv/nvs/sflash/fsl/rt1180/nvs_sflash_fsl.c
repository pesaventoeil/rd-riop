/** @file
 *
 * @brief Serial FLash Access via FLexSPI
 *
 * This module provides read and write access to the serial flash via xSPI.
 *
 * @copyright
 * Copyright 2025.
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
#define GOAL_ID GOAL_ID_DRV_NVS
#include <goal_includes.h>
#include <goal_target_common.h>
#include <nvs_sflash_fsl.h>
#include "fsl_cache.h"


/****************************************************************************/
/* Local Defines */
/****************************************************************************/
#define GOAL_DRV_NVS_SFLASH_FSL_BUSY_EXCLUDE 1  /**< excluding FSL busy function */


/****************************************************************************/
/* Local typedefs */
/****************************************************************************/
typedef struct {
    uint8_t *pAddrNvs;                          /**< sflash address */
    spi_flash_instance_t *pSflash;              /**< sflash instance */
} GOAL_NVS_SFLASH_FSL_T;


/****************************************************************************/
/* Variables */
/****************************************************************************/
spi_flash_instance_t fsl_instance;

#if RIOP /* External configuration */
#define FLASH_OFFSET 0
#else
/* 1 config, 2 chips of same type on board */
flexspi_device_config_t deviceconfig = {
    .flexspiRootClk = 12000000,
    .flashSize = FLASH_SIZE,
    .CSIntervalUnit = kFLEXSPI_CsIntervalUnit1SckCycle,
    .CSInterval = 2,
    .CSHoldTime = 3,
    .CSSetupTime = 3,
    .dataValidTime = 0,
    .columnspace = 0,
    .enableWordAddress = 0,
    .AWRSeqIndex = 0,
    .AWRSeqNumber = 0,
    .ARDSeqIndex = NOR_CMD_LUT_SEQ_IDX_READ_FAST_QUAD,
    .ARDSeqNumber = 1,
    .AHBWriteWaitUnit = kFLEXSPI_AhbWriteWaitUnit2AhbCycle,
    .AHBWriteWaitInterval = 0,
};

uint32_t customLUT[CUSTOM_LUT_LENGTH] = {
    /* Normal read mode -SDR */
    [4 * NOR_CMD_LUT_SEQ_IDX_READ_NORMAL] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x03, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 0x18),
    [4 * NOR_CMD_LUT_SEQ_IDX_READ_NORMAL + 1] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

    /* Fast read mode - SDR */
    [4 * NOR_CMD_LUT_SEQ_IDX_READ_FAST] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x0B, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 0x18),
    [4 * NOR_CMD_LUT_SEQ_IDX_READ_FAST + 1] = FLEXSPI_LUT_SEQ(
        kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_1PAD, 0x08, kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04),

    /* Fast read quad mode - SDR */
    [4 * NOR_CMD_LUT_SEQ_IDX_READ_FAST_QUAD] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0xEB, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_4PAD, 0x18),
    [4 * NOR_CMD_LUT_SEQ_IDX_READ_FAST_QUAD + 1] = FLEXSPI_LUT_SEQ(
        kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_4PAD, 0x06, kFLEXSPI_Command_READ_SDR, kFLEXSPI_4PAD, 0x04),

    /* Write Enable */
    [4 * NOR_CMD_LUT_SEQ_IDX_WRITEENABLE] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x06, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

    /* Erase Sector  */
    [4 * NOR_CMD_LUT_SEQ_IDX_ERASESECTOR] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x20, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 0x18),

    /* Page Program - single mode */
    [4 * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_SINGLE] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x02, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 0x18),
    [4 * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_SINGLE + 1] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_1PAD, 0x04, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

    /* Page Program - quad mode */
    [4 * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_QUAD] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x32, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 0x18),
    [4 * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_QUAD + 1] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_4PAD, 0x04, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

    /* Read ID */
    [4 * NOR_CMD_LUT_SEQ_IDX_READID] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x9F, kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04),

    /* Enable Quad mode */
    [4 * NOR_CMD_LUT_SEQ_IDX_WRITESTATUSREG] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x01, kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_1PAD, 0x04),

    /* Read status register */
    [4 * NOR_CMD_LUT_SEQ_IDX_READSTATUSREG] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x05, kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04),

    /* Erase whole chip */
    [4 * NOR_CMD_LUT_SEQ_IDX_ERASECHIP] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0xC7, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),
};
#endif

/****************************************************************************/
/* Local prototypes */
/****************************************************************************/
static GOAL_STATUS_T goal_drvNvsSflashFslOpen(
    GOAL_MA_NVS_T *pMaNvs                       /**< nvs handle */
);

static GOAL_STATUS_T goal_drvNvsSflashFslRead(
    struct GOAL_MA_NVS_T *pMaNvs,               /**< nvs handle */
    uint32_t offset,                            /**< read offset */
    uint8_t *pData,                             /**< data ptr */
    uint32_t len                                /**< data length */
);

static GOAL_STATUS_T goal_drvNvsSflashFslWrite(
    struct GOAL_MA_NVS_T *pMaNvs,               /**< nvs handle */
    uint32_t offset,                            /**< write offset */
    uint8_t  *pData,                            /**< data ptr */
    uint32_t len                                /**< data length */
);

static GOAL_STATUS_T goal_drvNvsSflashFslErase(
    struct GOAL_MA_NVS_T *pMaNvs,               /**< nvs handle */
    uint32_t offset,                            /**< erase offset */
    uint32_t len                                /**< data length */
);

#if GOAL_DRV_NVS_SFLASH_FSL_BUSY_EXCLUDE == 0
static GOAL_STATUS_T goal_drvNvsSflashFslBusy(
    struct GOAL_MA_NVS_T *pMaNvs,               /**< nvs handle */
    GOAL_BOOL_T *pState                         /**< state ref */
);
#endif


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/


/****************************************************************************/
/** Initialize FSL SFlash driver
 *
 * This function registers the driver on NVS MA and assigns required variables
 * to the handle.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_drvNvsSflashFslReg(
    GOAL_ID_T id,                               /**< MA id */
    GOAL_MA_NVS_INFO_T *pInfoNvs,               /**< NVS information */
    uint8_t *pAddrNvs,                          /**< address offset to NVS section */
    FLEXSPI_Type *pfsl                          /**< sflash base */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_NVS_SFLASH_FSL_T *pHdlDrv;             /* driver handle */
    GOAL_MA_NVS_T *pMaNvs = NULL;               /* nvs handle */

    /* allocating a driver handle */
    res = goal_memCalloc(&pHdlDrv, sizeof(GOAL_NVS_SFLASH_FSL_T));
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to allocate memory for NVS data");
        return res;
    }

    /* register NVS MA */
    res = goal_maNvsReg(
        id,
        &pMaNvs,
        goal_drvNvsSflashFslOpen,
        goal_drvNvsSflashFslRead,
        goal_drvNvsSflashFslWrite,
        goal_drvNvsSflashFslErase,
#if GOAL_DRV_NVS_SFLASH_FSL_BUSY_EXCLUDE == 1
        NULL,
#else
        goal_drvNvsSflashFslBusy,
#endif
        pHdlDrv);

    if (GOAL_RES_OK(res)) {
        /* store offset to mapped memory */
        pHdlDrv->pAddrNvs = pAddrNvs;
        pHdlDrv->pSflash = &fsl_instance;
        pHdlDrv->pSflash->pBase = pfsl;

        /* link configuration */
        pHdlDrv->pSflash->deviceconfig = &deviceconfig;
        pHdlDrv->pSflash->customLUT = &customLUT[0];

        /* set MA flash values */
        pMaNvs->infoFlash.sizeFlash = pInfoNvs->sizeFlash;
        pMaNvs->infoFlash.sizeErase = pInfoNvs->sizeErase;

        /* compare the available memory according to GOAL and FSL NVS */
#if RIOP
        if (pInfoNvs->sizeFlash != (pHdlDrv->pSflash->deviceconfig->flashSize*1024)) {
            goal_logWarn("configuration of GOAL and FSL sflash differs");
        }
#else
        if (pInfoNvs->sizeProgram != pHdlDrv->pSflash->deviceconfig->flashSize) {
            goal_logWarn("configuration of GOAL and FSL sflash differs");
        }
#endif
        pMaNvs->infoFlash.sizeProgram = pInfoNvs->sizeProgram;
        pMaNvs->infoFlash.flags = pInfoNvs->flags | GOAL_MA_NVS_FLAG_ERASE_REQUIRED | GOAL_MA_NVS_FLAG_MEM_READ;
    }

    if (GOAL_RES_OK(res)) {
        goal_logInfo("FSL flexspi version %"FMT_u32, FSL_FLEXSPI_DRIVER_VERSION);
    }

    return res;
}


/****************************************************************************/
/** FSL SFlash open function
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_drvNvsSflashFslOpen(
    GOAL_MA_NVS_T *pMaNvs                       /**< nvs handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

#if !RIOP /* External flash initialization is done as part of HW_Init() */
    GOAL_NVS_SFLASH_FSL_T *pHdlDrv;             /* driver handle */
    uint8_t vendorID = 0;                       /* Vendor ID */
    status_t status;                            /* NXP return value */
    uint32_t prim;                              /* previous PRIMASK value */

    /* assign driver handle */
    pHdlDrv = (GOAL_NVS_SFLASH_FSL_T *) pMaNvs->pData;

    prim = __get_PRIMASK();

    /* Disable interrupts */
    __disable_irq();

    flexspi_nor_flash_init(pHdlDrv->pSflash->pBase);

    /* Enable interrupts back */
    if (!prim) {
        __enable_irq();
    }

    /* Get vendor ID. */
    status = flexspi_nor_get_vendor_id(pHdlDrv->pSflash->pBase, &vendorID);
    if (status != kStatus_Success)
    {
        return GOAL_ERROR;
    }
    goal_logInfo("Flash initialized. Vendor ID: 0x%x\r\n", vendorID);

    /* Enter quad mode. */
    status = flexspi_nor_enable_quad_mode(EXAMPLE_FLEXSPI);
    if (status != kStatus_Success)
    {
        return GOAL_ERROR;
    }
    goal_logInfo("flash entered quad mode");
#endif

    return res;
}


#if GOAL_DRV_NVS_SFLASH_FSL_BUSY_EXCLUDE == 0
/****************************************************************************/
/** Check if Flash Driver is busy
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_drvNvsSflashFslBusy(
    struct GOAL_MA_NVS_T *pMaNvs,               /**< nvs handle */
    GOAL_BOOL_T *pState                         /**< state ref */
)
{
    GOAL_NVS_SFLASH_FSL_T *pHdlDrv;             /* driver handle */
    status_t status = kStatus_Success;          /* NXP return value */

    bool busy;                                  /* busy state */

    /* assign driver handle */
    pHdlDrv = (GOAL_NVS_SFLASH_FSL_T *) pMaNvs->pData;

    status = flexspi_nor_check_bus_busy(pHdlDrv->pSflash->pBase, &busy);
    if (status != kStatus_Success)
    {
        return GOAL_ERROR;
    }

    if (busy == 1) {
        *pState = GOAL_TRUE;
    }
    else {
        *pState = GOAL_FALSE;
    }

    return GOAL_OK;
}
#endif /* GOAL_DRV_NVS_SFLASH_FSL_BUSY_EXCLUDE == 1 */


/****************************************************************************/
/** Read Data From Flash
 *
 * Returns the flash stored data.
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_drvNvsSflashFslRead(
    struct GOAL_MA_NVS_T *pMaNvs,               /**< nvs handle */
    uint32_t offset,                            /**< read offset */
    uint8_t *pData,                             /**< data ptr */
    uint32_t len                                /**< data length */
)
{
    GOAL_NVS_SFLASH_FSL_T *pHdlDrv;             /* driver handle */

    /* assign driver handle */
    pHdlDrv = (GOAL_NVS_SFLASH_FSL_T *) pMaNvs->pData;

    DCACHE_InvalidateByRange(*(pHdlDrv->pAddrNvs) + FLASH_OFFSET + offset, FLASH_PAGE_SIZE);

    GOAL_MEMCPY(pData, pHdlDrv->pAddrNvs + FLASH_OFFSET + offset, len);

    return GOAL_OK;
}


/****************************************************************************/
/** Write Data To Flash
 *
 * This function writes data to the flash. The memory section needs to be
 * erased previously manually.
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_drvNvsSflashFslWrite(
    struct GOAL_MA_NVS_T *pMaNvs,               /**< nvs handle */
    uint32_t offset,                            /**< write offset */
    uint8_t  *pData,                            /**< data ptr */
    uint32_t len                                /**< data length */
)
{
    GOAL_STATUS_T res;                          /* result */
    status_t err = kStatus_Success;             /* FSL return value */
    GOAL_NVS_SFLASH_FSL_T *pHdlDrv;             /* driver handle */
    uint32_t prim;                              /* previous PRIMASK value */

    /* assign driver handle */
    pHdlDrv = (GOAL_NVS_SFLASH_FSL_T *) pMaNvs->pData;

    /* Read PRIMASK register, check interrupt status before you disable them */
    /* Returns 0 if they are enabled, or non-zero if disabled */
    prim = __get_PRIMASK();

    /* Disable interrupts */
    __disable_irq();

    err = flexspi_nor_flash_program(pHdlDrv->pSflash->pBase, FLASH_OFFSET + offset, (uint32_t *) pData, len);
    res = goal_targetResToGoal(err);

    /* Enable interrupts back */
    if (!prim) {
        __enable_irq();
    }

    return res;
}


/****************************************************************************/
/** Erase Data on Flash
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_drvNvsSflashFslErase(
    struct GOAL_MA_NVS_T *pMaNvs,               /**< nvs handle */
    uint32_t offset,                            /**< erase offset */
    uint32_t len                                /**< data length */
)
{
    status_t err = kStatus_Success;             /* FSL return value */
    GOAL_NVS_SFLASH_FSL_T *pHdlDrv;             /* driver handle */
    uint32_t prim;                              /* previous PRIMASK value */

    UNUSEDARG(len);

    /* assign driver handle */
    pHdlDrv = (GOAL_NVS_SFLASH_FSL_T *) pMaNvs->pData;

    /* Read PRIMASK register, check interrupt status before you disable them */
    /* Returns 0 if they are enabled, or non-zero if disabled */
    prim = __get_PRIMASK();

    /* Disable interrupts */
    __disable_irq();

#if RIOP
    err = flexspi_erase_sector(pHdlDrv->pSflash->pBase, FLASH_OFFSET + offset);
#else
    err = flexspi_nor_flash_erase_sector(pHdlDrv->pSflash->pBase, FLASH_OFFSET + offset);
#endif

    /* Enable interrupts back */
    if (!prim) {
        __enable_irq();
    }

    return goal_targetResToGoal(err);
}
