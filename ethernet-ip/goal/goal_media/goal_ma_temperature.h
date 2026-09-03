/** @file
 *
 * @brief Generic MA for Temperature
 *
 * @details
 * This module provides a MA for Temperature
 *
 *
 * @copyright
 * Copyright 2024.
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
#ifndef GOAL_MA_TEMPERATURE_H
#define GOAL_MA_TEMPERATURE_H


/****************************************************************************/
/* Forward declaration */
/****************************************************************************/
struct GOAL_MA_TEMPERATURE_T;                   /**< MA temperature handle */


/****************************************************************************/
/* Typedefs */
/****************************************************************************/
/**< get Value function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_TEMPERATURE_GET_VALUE)(
    struct GOAL_MA_TEMPERATURE_T *pHdlMaTemperature, /**< [in] MA handle for temperature  */
    int32_t *pTemperature                       /**< [out] temperature value */
);

/**< Open function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_TEMPERATURE_OPEN) (
    struct GOAL_MA_TEMPERATURE_T *pHdlMaTemperature /**< [in] MA handle for temperature  */
);

/**< Close function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_TEMPERATURE_CLOSE) (
    struct GOAL_MA_TEMPERATURE_T *pHdlMaTemperature /**< [in] MA handle for temperature  */
);

/**< Configuration set function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_TEMPERATURE_CONFIG_SET)(
    struct GOAL_MA_TEMPERATURE_T *pHdlMaTemperature, /**< [in] MA handle for temperature  */
    void *pDrvConfig                            /**< [in] configuration to be set */
);

/**< Configuration get function prototype */
typedef GOAL_STATUS_T (*GOAL_MA_TEMPERATURE_CONFIG_GET)(
    struct GOAL_MA_TEMPERATURE_T *pHdlMaTemperature, /**< [in] MA handle for temperature  */
    void *pDrvConfig                            /**< [out] configuration which is set */
);

/** Temperature handle instance struct */
typedef struct GOAL_MA_TEMPERATURE_T {
    struct GOAL_MA_TEMPERATURE_T *pNext;        /**< next entry */
    uint32_t id;                                /**< MA ID */
    GOAL_BOOL_T flgUsed;                        /**< used flag */
    GOAL_MA_TEMPERATURE_CONFIG_SET fctConfSet;  /**< set config */
    GOAL_MA_TEMPERATURE_CONFIG_GET fctConfGet;  /**< get config */
    GOAL_MA_TEMPERATURE_OPEN fctOpen;           /**< opening driver */
    GOAL_MA_TEMPERATURE_CLOSE fctClose;         /**< closing driver */
    GOAL_MA_TEMPERATURE_GET_VALUE fctGetVal;    /**< gets the current temperature */
    void *pDrvData;                             /**< driver specific data */
} GOAL_MA_TEMPERATURE_T;


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_maTemperatureDrvReg(
    uint32_t id,                                /**< ID */
    GOAL_MA_TEMPERATURE_T **ppHdlMaTemperature, /**< [out] Temperature handle reference pointer */
    GOAL_MA_TEMPERATURE_CONFIG_GET fConfGet,    /**< config get function */
    GOAL_MA_TEMPERATURE_CONFIG_SET fConfSet,    /**< config set function */
    GOAL_MA_TEMPERATURE_OPEN fOpen,             /**< open driver function */
    GOAL_MA_TEMPERATURE_CLOSE fClose,           /**< close driver function */
    GOAL_MA_TEMPERATURE_GET_VALUE fValueGet,    /**< get temperature value function */
    void *pDrvData                              /**< [in] driver specific data */
);

GOAL_STATUS_T goal_maTemperatureDrvConfigurationSet(
    GOAL_MA_TEMPERATURE_T *pHdlMaTemperature,   /**< [in] pointer to store Temperature handler */
    void *pDrvConfig                            /**< [in] driver specific configuration */
);

GOAL_STATUS_T goal_maTemperatureDrvConfigurationGet(
    GOAL_MA_TEMPERATURE_T *pHdlMaTemperature,   /**< [in] pointer to store Temperature handler */
    void *pDrvConfig                            /**< [out] driver specific configuration */
);

GOAL_STATUS_T goal_maTemperatureGetById(
    GOAL_MA_TEMPERATURE_T **ppHdlMaTemperature, /**< [out] Temperature handle reference pointer */
    uint32_t id                                 /**< ID of the Instance */
);

GOAL_STATUS_T goal_maTemperatureValueGet(
    GOAL_MA_TEMPERATURE_T *pHdlMaTemperature,   /**< [in] Temperature handle ref ptr */
    int32_t *pTemperature                       /**< [out] Temperature Value Pointer */
);

GOAL_STATUS_T goal_maTemperatureOpen(
    GOAL_MA_TEMPERATURE_T *pHdlMaTemperature    /**< [in] Temperature handle ref ptr */
);

GOAL_STATUS_T goal_maTemperatureClose(
    GOAL_MA_TEMPERATURE_T *pHdlMaTemperature    /**< [in] Temperature handle ref ptr */
);

#endif /* GOAL_MA_TEMPERATURE_H */