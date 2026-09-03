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
#ifndef GOAL_MA_SPI_H
#define GOAL_MA_SPI_H


/****************************************************************************/
/* Defines */
/****************************************************************************/
#ifndef GOAL_MA_SPI_TX_SIZE
# define GOAL_MA_SPI_TX_SIZE 128                /**< MA transmission size */
#endif


/****************************************************************************/
/* Forward declaration */
/****************************************************************************/
struct GOAL_MA_SPI_T;                           /**< MA SPI handle */


/****************************************************************************/
/* Typedefs */
/****************************************************************************/
/**< SPI type (master or slave) */
typedef enum {
    GOAL_MA_SPI_TYPE_MASTER = 0,                /**< SPI interface works as master */
    GOAL_MA_SPI_TYPE_SLAVE                      /**< SPI interface works as slave */
} GOAL_MA_SPI_TYPE_T;

/**< SPI modes, CPOL = Clock Polarity, CPHA = Clock Phase */
typedef enum {
    GOAL_MA_SPI_MODE_0 = 0,                     /**< Mode 0: CPOL 0, CPHA 0 */
    GOAL_MA_SPI_MODE_1,                         /**< Mode 1: CPOL 0, CPHA 1 */
    GOAL_MA_SPI_MODE_2,                         /**< Mode 2: CPOL 1, CPHA 0 */
    GOAL_MA_SPI_MODE_3                          /**< Mode 3: CPOL 1, CPHA 1 */
} GOAL_MA_SPI_MODE_T;

/**< Transmission unit width of SPI controller */
typedef enum {
    GOAL_MA_SPI_UNITWIDTH_8BIT = 0,             /**< Unit width 8 bit */
    GOAL_MA_SPI_UNITWIDTH_16BIT,                /**< Unit width 16 bit */
    GOAL_MA_SPI_UNITWIDTH_32BIT                 /**< Unit width 32 bit */
} GOAL_MA_SPI_UNITWIDTH_T;

/**< Bit order */
typedef enum {
    GOAL_MA_SPI_BITORDER_MSB = 0,               /**< MSB first */
    GOAL_MA_SPI_BITORDER_LSB                    /**< LSB first */
} GOAL_MA_SPI_BITORDER_T;

/**< Driver transfer mode */
typedef enum {
    GOAL_MA_SPI_TRANSFER_MODE_SYNC = 0,         /**< synchronous mode */
    GOAL_MA_SPI_TRANSFER_MODE_ASYNC = 1         /**< asynchronous mode */
} GOAL_MA_SPI_TRANSFER_MODE;

/**< Events for successful and failed operations */
typedef enum {
    GOAL_MA_SPI_EVENT_TRANSFER_COMPLETE = 0,    /**< Transfer */
    GOAL_MA_SPI_EVENT_TRANSFER_ABORTED,         /**< The data transfer was aborted */
    GOAL_MA_SPI_ERR_MODE_FAULT,                 /**< Mode fault error */
    GOAL_MA_SPI_ERR_READ_OVERFLOW,              /**< Read overflow error */
    GOAL_MA_SPI_ERR_PARITY,                     /**< Parity error */
    GOAL_MA_SPI_ERR_DATA_CONSISTENCY,           /**< Data consistency check error */
    GOAL_MA_SPI_ERR_OVERFLOW,                   /**< Overflow error */
    GOAL_MA_SPI_ERR_OVERRUN,                    /**< Overrun error */
    GOAL_MA_SPI_ERR_BUF_OVERRUN,                /**< Buffer overrun error */
    GOAL_MA_SPI_ERR_FRAMING,                    /**< Framing error */
    GOAL_MA_SPI_ERR_MODE_UNDERRUN,              /**< Underrun error */
    GOAL_MA_SPI_ERR_MODE_UNKNOWN = 0xFF         /**< Unknown */
} GOAL_MA_SPI_EVENT_T;

/**< optional event data for notification */
typedef struct {
    uint16_t len;                               /**< length information */
} GOAL_MA_SPI_EVENT_DATA_T;

/**< configuration for an SPI interface */
typedef struct {
    GOAL_MA_SPI_TYPE_T type;                    /**< SPI type (master or slave) */
    GOAL_MA_SPI_MODE_T mode;                    /**< SPI mode (Mode 0 to 3) */
    uint32_t bitrate;                           /**< bit rate (only valid when type == master) */
    GOAL_MA_SPI_UNITWIDTH_T unitWidth;          /**< bits per unit */
    GOAL_MA_SPI_BITORDER_T bitorder;            /**< Bit order on SPI bus */
    GOAL_MA_SPI_TRANSFER_MODE txMode;           /**< Synchronous or asynchronous interface */
} GOAL_MA_SPI_CONF_T;

/**< write function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_SPI_WRITE)(
    struct GOAL_MA_SPI_T *pHdlMaSpi,            /**< MA handle for SPI  */
    uint8_t *pData,                             /**< message data */
    uint16_t len                                /**< message length in bytes */
);

/**< read function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_SPI_READ)(
    struct GOAL_MA_SPI_T *pHdlMaSpi,            /**< MA handle for SPI  */
    uint8_t *pData,                             /**< message data */
    uint16_t *pLen                              /**< message length in bytes */
);

/**< write/read function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_SPI_WRITEREAD)(
    struct GOAL_MA_SPI_T *pHdlMaSpi,            /**< MA handle for SPI  */
    uint8_t *pWriteData,                        /**< write message data */
    uint8_t *pReadData,                         /**< read message data */
    uint16_t *pLen                              /**< message length in bytes */
);

/**< write/read function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_SPI_ASYNC_START)(
    struct GOAL_MA_SPI_T *pMaSpi                /**< MA handle for SPI  */
);

/**< notify function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_SPI_NOTIFY)(
    struct GOAL_MA_SPI_T *pSpiHdl,              /**< handle where the event occurred */
    GOAL_MA_SPI_EVENT_T event,                  /**< event type */
    GOAL_MA_SPI_EVENT_DATA_T *pDataEvent,       /**< optional event data */
    void *pArg                                  /**< function argument */
);

/**< open function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_SPI_OPEN) (
    struct GOAL_MA_SPI_T *pHdlMaSpi             /**< MA handle for SPI  */
);

/**< reopen function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_SPI_REOPEN) (
    struct GOAL_MA_SPI_T *pHdlMaSpi             /**< MA handle for SPI  */
);

/**< set conf function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_SPI_CONF_SET) (
    struct GOAL_MA_SPI_T *pHdlMaSpi             /**< MA handle for SPI  */
);

/**< get conf function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_SPI_CONF_GET) (
    struct GOAL_MA_SPI_T *pHdlMaSpi             /**< MA handle for SPI  */
);

/**< close function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_SPI_CLOSE) (
    struct GOAL_MA_SPI_T *pHdlMaSpi             /**< MA handle for SPI  */
);

/**< reset function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_SPI_RESET) (
    struct GOAL_MA_SPI_T *pHdlMaSpi             /**< MA handle for SPI  */
);

/**< SPI handle instance struct */
typedef struct GOAL_MA_SPI_T {
    struct GOAL_MA_SPI_T *pNext;                /**< next entry */
    uint32_t id;                                /**< MA ID */
    GOAL_BOOL_T flgUsed;                        /**< used flag */
    GOAL_MA_SPI_OPEN fctOpen;                   /**< open SPI channel function */
    GOAL_MA_SPI_OPEN fctReopen;                 /**< reopen SPI channel function */
    GOAL_MA_SPI_CLOSE fctClose;                 /**< close SPI channel function */
    GOAL_MA_SPI_CONF_GET fctConfGet;            /**< get current config function */
    GOAL_MA_SPI_CONF_SET fctConfSet;            /**< set config function function */
    GOAL_MA_SPI_WRITE fctWrite;                 /**< write function */
    GOAL_MA_SPI_READ fctRead;                   /**< read function */
    GOAL_MA_SPI_WRITEREAD fctWriteRead;         /**< write/read function */
    GOAL_MA_SPI_NOTIFY fctNotify;               /**< notification function */
    GOAL_MA_SPI_ASYNC_START fctAsyncStart;      /**< async start function */
    GOAL_MA_SPI_RESET fctReset;                 /**< reset bus */
    void *pFctNotifyArg;                        /**< notification function argument */
    void *pDrvData;                             /**< driver specific data */
    uint8_t *pDataRx;                           /**< RX data */
    uint8_t *pDataTx;                           /**< TX data */
    unsigned int lenDataRx;                     /**< RX data length */
    unsigned int lenDataTx;                     /**< TX data length */
    GOAL_LOCK_T *pLockRxTx;                     /**< RX/TX lock */
    GOAL_MA_SPI_CONF_T cfg;                     /**< generic driver configuration */
    uint32_t cntErrorRx;                        /**< statistics: number of rx errors (receive data overrun) */
} GOAL_MA_SPI_T;


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_maSpiDrvReg(
    uint32_t id,                                /**< ID */
    GOAL_MA_SPI_T **ppHdlMaSpi,                 /**< SPI handle ref ptr */
    GOAL_MA_SPI_OPEN fOpen,                     /**< open function */
    GOAL_MA_SPI_OPEN fReopen,                   /**< reopen function */
    GOAL_MA_SPI_CONF_GET fConfGet,              /**< conf get function */
    GOAL_MA_SPI_CONF_SET fConfSet,              /**< conf set function */
    GOAL_MA_SPI_READ fRead,                     /**< read function */
    GOAL_MA_SPI_WRITE fWrite,                   /**< write function */
    GOAL_MA_SPI_WRITEREAD fWriteRead,           /**< write/read function */
    GOAL_MA_SPI_ASYNC_START fAsyncStart,        /**< start async function */
    GOAL_MA_SPI_CLOSE fClose,                   /**< close function */
    GOAL_MA_SPI_RESET fReset,                   /**< reset function */
    void *pDrvData                              /**< driver specific data */
);

GOAL_STATUS_T goal_maSpiOpen(
    uint32_t id,                                /**< id of SPI handler to use */
    GOAL_MA_SPI_T **ppSpiHdl,                   /**< pointer to store SPI handler */
    GOAL_MA_SPI_NOTIFY fNotify,                 /**< callback function ptr */
    void *pArgNotify                            /**< notification function argument */
);

GOAL_STATUS_T goal_maSpiReopen(
    GOAL_MA_SPI_T *pSpiHdl                      /**< MA handle for SPI */
);

GOAL_STATUS_T goal_maSpiClose(
    GOAL_MA_SPI_T *pSpiHdl                      /**< pointer to store SPI handler */
);

GOAL_STATUS_T goal_maSpiWrite(
    GOAL_MA_SPI_T *pHdlMaSpi,                   /**< MA handle for SPI */
    uint8_t *pData,                             /**< data */
    uint16_t len                                /**< data length in bytes */
);

GOAL_STATUS_T goal_maSpiRead(
    GOAL_MA_SPI_T *pHdlMaSpi,                   /**< MA handle for SPI */
    uint8_t *pData,                             /**< data */
    uint16_t *pLen                              /**< data length in bytes */
);

GOAL_STATUS_T goal_maSpiWriteRead(
    GOAL_MA_SPI_T *pHdlMaSpi,                   /**< MA handle for SPI */
    uint8_t *pWriteData,                        /**< data */
    uint8_t *pReadData,                         /**< data */
    uint16_t *pLen                              /**< data length in bytes */
);

GOAL_STATUS_T goal_maSpiGetById(
    GOAL_MA_SPI_T **ppHdlMaSpi,                 /**< SPI handle ref ptr */
    uint32_t id                                 /**< MA id */
);

GOAL_STATUS_T goal_maSpiDrvNotifyCb(
    GOAL_MA_SPI_T *pHdlMaSpi,                   /**< MA handle for SPI */
    GOAL_MA_SPI_EVENT_T event,                  /**< event type */
    GOAL_MA_SPI_EVENT_DATA_T *pDataEvent        /**< optional event data */
);

GOAL_STATUS_T goal_maSpiConfigSet(
    GOAL_MA_SPI_T *pHdlMaSpi                    /**< MA handle for SPI */
);

GOAL_STATUS_T goal_maSpiConfigGet(
    GOAL_MA_SPI_T *pHdlMaSpi                    /**< MA handle for SPI */
);

GOAL_STATUS_T goal_maSpiTxAsync(
    GOAL_MA_SPI_T *pHdlMaSpi,                   /**< MA handle for SPI */
    uint8_t *pDataTx,                           /**< tx data buffer */
    unsigned int lenDataTx                      /**< tx buffer size */
);

GOAL_STATUS_T goal_maSpiRxAsync(
    GOAL_MA_SPI_T *pHdlMaSpi,                   /**< MA handle for SPI */
    uint8_t *pDataRx,                           /**< tx data buffer */
    unsigned int *pLenDataRx                    /**< [in,out] rx buffer size */
);

unsigned int goal_maSpiBufSizeGet(
    GOAL_MA_SPI_T *pHdlMaSpi                    /**< MA handle for SPI */
);

GOAL_STATUS_T goal_maSpiAsyncStart(
    GOAL_MA_SPI_T *pMaSpi                       /**< MA handle for SPI */
);

GOAL_STATUS_T goal_maSpiCfgBitrateSet(
    GOAL_MA_SPI_T *pMaSpi,                      /**< MA handle for SPI */
    unsigned int valBitrate                     /**< bitrate */
);

GOAL_STATUS_T goal_maSpiCfgTypeSet(
    GOAL_MA_SPI_T *pMaSpi,                      /**< MA handle for SPI */
    GOAL_MA_SPI_TYPE_T type                     /**< SPI type master or slave */
);

GOAL_STATUS_T goal_maSpiCfgModeSet(
    GOAL_MA_SPI_T *pMaSpi,                      /**< MA handle for SPI */
    GOAL_MA_SPI_MODE_T mode                     /**< SPI mode */
);

GOAL_STATUS_T goal_maSpiCfgUnitWidthSet(
    GOAL_MA_SPI_T *pMaSpi,                      /**< MA handle for SPI */
    GOAL_MA_SPI_UNITWIDTH_T valUnitWidth        /**< SPI unit width */
);

GOAL_STATUS_T goal_maSpiCfgBitOrderSet(
    GOAL_MA_SPI_T *pMaSpi,                      /**< MA handle for SPI */
    GOAL_MA_SPI_BITORDER_T valBitOrder          /**< SPI bit order */
);

GOAL_STATUS_T goal_maSpiReset(
    GOAL_MA_SPI_T *pMaSpi                       /**< MA handle for SPI */
);


#endif /* GOAL_MA_SPI_H */
