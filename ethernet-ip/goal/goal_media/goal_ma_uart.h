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

#ifndef GOAL_MA_UART_H
#define GOAL_MA_UART_H


/****************************************************************************/
/* Public defines */
/****************************************************************************/
/**< Use 115200 baud/s if not defined */
#ifndef GOAL_MA_UART_BAUD
#  define GOAL_MA_UART_BAUD GOAL_MA_UART_BAUD_115200
#endif


/****************************************************************************/
/* Forward declaration */
/****************************************************************************/
struct GOAL_MA_UART_T;


/****************************************************************************/
/* Typedefs */
/****************************************************************************/
/**< Events for successful and failed operations */
typedef enum {
    GOAL_MA_UART_EVENT_TRANSFER_COMPLETED = 0,  /**< transfer completed */
    GOAL_MA_UART_EVENT_TRANSFER_ABORTED,        /**< The data transfer was aborted */
    GOAL_MA_UART_EVENT_RECEIVED,                /**< data received */
} GOAL_MA_UART_EVENT_T;

/**< Driver transfer mode */
typedef enum {
    GOAL_MA_UART_TRANSFER_MODE_SYNC = 0,         /**< synchronous mode */
    GOAL_MA_UART_TRANSFER_MODE_ASYNC = 1         /**< asynchronous mode */
} GOAL_MA_UART_TRANSFER_MODE_T;

/**< Baud rate */
typedef enum {
    GOAL_MA_UART_BAUD_115200 = 0,               /**< 115200 baud/s */
    GOAL_MA_UART_BAUD_230400,                   /**< 230400 baud/s */
    GOAL_MA_UART_BAUD_576000,                   /**< 576000 baud/s */
    GOAL_MA_UART_BAUD_921600                    /**< 921600 baud/s */
} GOAL_MA_UART_BAUDRATE_T;

typedef GOAL_STATUS_T (*GOAL_MA_UART_OPEN_T)(
    struct GOAL_MA_UART_T *pUart                /**< UART handle */
);

typedef GOAL_STATUS_T (*GOAL_MA_UART_GET_T)(
    struct GOAL_MA_UART_T *pUart,               /**< UART handle */
    char *pChr                                  /**< character */
);

typedef GOAL_STATUS_T (*GOAL_MA_UART_GETSTR_T)(
    struct GOAL_MA_UART_T *pUart,               /**< UART driver handle */
    char *pStr,                                 /**< string */
    uint16_t len                                /**< len */
);

typedef GOAL_STATUS_T (*GOAL_MA_UART_PUT_T)(
    struct GOAL_MA_UART_T *pUart,               /**< UART handle */
    const char chr
);

typedef GOAL_STATUS_T (*GOAL_MA_UART_PUTSTR_T)(
    struct GOAL_MA_UART_T *pUart,               /**< UART handle */
    const char *pStr,
    uint32_t len
);

typedef GOAL_STATUS_T (*GOAL_MA_UART_NOTIFY_T)(
    struct GOAL_MA_UART_T *pUart,               /**< UART handle */
    GOAL_MA_UART_EVENT_T event,                 /**< UART event */
    void *pArgNotify                            /**< notify arguments */
);


/****************************************************************************/
/* Structures */
/****************************************************************************/
typedef struct {
    GOAL_MA_UART_TRANSFER_MODE_T txMode;        /**< transfer mode */
    uint16_t rxSize;                            /**< notify if rxSize is reached */
    GOAL_MA_UART_BAUDRATE_T baudrate;           /**< configured baudrate */
} GOAL_MA_UART_CFG_T;

typedef struct GOAL_MA_UART_T {
    struct GOAL_MA_UART_T *pNext;               /**< next element */
    GOAL_ID_T id;                               /**< MA id */
    GOAL_MA_UART_OPEN_T funcOpen;               /**< driver open function */
    GOAL_MA_UART_GET_T funcGet;                 /**< driver get function */
    GOAL_MA_UART_GETSTR_T funcGetStr;          /**< driver get string function */
    GOAL_MA_UART_PUT_T funcPut;                 /**< driver put function */
    GOAL_MA_UART_PUTSTR_T funcPutStr;           /**< driver put string function */
    GOAL_BOOL_T flgUsed;                        /**< usage flag */
    void *pData;                                /**< driver specific data */
    GOAL_MA_UART_NOTIFY_T funcNotify;           /**< event notification callback */
    GOAL_MA_UART_CFG_T cfg;                     /**< uart configuration */
    struct GOAL_MI_MCTC_UART_T *pMiMctcUart;    /**< assigned MCTC MI */
    GOAL_LOCK_T *pLockRxTx;                     /**< lock */
    void *pArgNotify;                           /**< notification function argument */
} GOAL_MA_UART_T;


/****************************************************************************/
/* UART MA interface */
/****************************************************************************/
GOAL_STATUS_T goal_maUartReg(
    GOAL_MA_UART_T **ppUart,                    /**< UART driver handle */
    GOAL_ID_T id,                               /**< UART MA id */
    GOAL_MA_UART_OPEN_T funcOpen,               /**< UART open function */
    GOAL_MA_UART_PUT_T funcPut,                 /**< UART put function */
    GOAL_MA_UART_PUTSTR_T funcPutStr,           /**< UART put string function */
    void *pData                                 /**< driver specific data */
);

GOAL_STATUS_T goal_maUartOpen(
    GOAL_MA_UART_T **ppUart,                    /**< UART driver handle */
    unsigned int idx,                           /**< UART index */
    GOAL_MA_UART_GET_T funcGet,                 /**< get char callback */
    GOAL_MA_UART_GETSTR_T funcGetStr,           /**< get str callback */
    GOAL_MA_UART_NOTIFY_T funcNotify,           /**< event notification callback */
    void *pArgNotify                            /**< notification function argument */
);

GOAL_STATUS_T goal_maUartClose(
    GOAL_MA_UART_T *pUart                       /**< UART driver handle */
);

GOAL_STATUS_T goal_maUartPut(
    GOAL_MA_UART_T *pUart,                      /**< UART driver handle */
    const char chr                              /**< character */
);

GOAL_STATUS_T goal_maUartPutStr(
    GOAL_MA_UART_T *pUart,                      /**< UART driver handle */
    const char *pStr,                           /**< string ptr */
    uint32_t len                                /**< string len */
);

GOAL_STATUS_T goal_maUartGet(
    GOAL_MA_UART_T *pUart,                      /**< UART driver handle */
    char *pChr                                  /**< character */
);

GOAL_STATUS_T goal_maUartGetStr(
    GOAL_MA_UART_T *pUart,                      /**< UART driver handle */
    char *pStr,                                 /**< string */
    uint16_t len                                /**< len */
);

GOAL_STATUS_T goal_maUartDrvNotifyCb(
    GOAL_MA_UART_T *pHdlMaUart,                 /**< MA handle for UART */
    GOAL_MA_UART_EVENT_T event                  /**< event type */
);

GOAL_STATUS_T goal_maUartCfgTransferModeSet(
    GOAL_MA_UART_T *pMaUart,                    /**< MA handle for UART */
    GOAL_MA_UART_TRANSFER_MODE_T txMode         /**< transfer mode */
);

GOAL_STATUS_T goal_maUartCfgReceiveBlockSize(
    GOAL_MA_UART_T *pMaUart,                    /**< MA handle for UART */
    uint16_t rxSize                             /**< rx size */
);

GOAL_STATUS_T goal_maUartCfgBaudrateSet(
    GOAL_MA_UART_T *pMaUart,                    /**< MA handle for UART */
    GOAL_MA_UART_BAUDRATE_T baudrate            /**< baudrate */
);

GOAL_STATUS_T goal_maUartGetById(
    GOAL_MA_UART_T **ppHdlMaUart,               /**< handle ref ptr */
    uint32_t id                                 /**< MA id */
);

#endif /* GOAL_MA_UART_H */
