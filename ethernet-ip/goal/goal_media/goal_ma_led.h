/** @file
 *
 * @brief GOAL media adapter for LEDs
 *
 * @details
 * This header file contains public definitions of the MA-LED.
 *
 * @copyright
 * Copyright 2010-2018.
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

#ifndef GOAL_MA_LED_H
#define GOAL_MA_LED_H


/****************************************************************************/
/* Forward declaration */
/****************************************************************************/
struct GOAL_MA_LED_T;                           /**< LED handle */
struct GOAL_MA_LED_LEDS_T;                      /**< LED properties */


/****************************************************************************/
/* Enums */
/****************************************************************************/
typedef enum GOAL_MA_LED_STATE_T {
    GOAL_MA_LED_STATE_OFF = 0,                  /**< LED off */
    GOAL_MA_LED_STATE_ON = 1                    /**< LED on */
} GOAL_MA_LED_STATE_T;


/****************************************************************************/
/* Typedefs */
/****************************************************************************/
/**< prototype for the DRV-LED function to open */
typedef GOAL_STATUS_T (*GOAL_MA_LED_OPEN) (
    struct GOAL_MA_LED_T *pMaLedHdl             /**< [in] MA-LED handle */
);

/**< prototype for the DRV-LED function to close */
typedef GOAL_STATUS_T (*GOAL_MA_LED_CLOSE) (
    struct GOAL_MA_LED_T *pMaLedHdl             /**< [in] MA-LED handle */
);

/**< DRV-LED function prototype to get the state of a single LED */
typedef GOAL_STATUS_T (*GOAL_MA_LED_GET)(
    struct GOAL_MA_LED_T *pMaLedHdl,            /**< [in] MA-LED handle */
    uint32_t ledNumber,                         /**< [in] led number */
    GOAL_MA_LED_STATE_T *pState                 /**< [out] current LED state */
);

/**< DRV-LED function prototype to set the state of a single LED */
typedef GOAL_STATUS_T (*GOAL_MA_LED_SET)(
    struct GOAL_MA_LED_T *pMaLedHdl,            /**< [in] MA-LED handle */
    uint32_t ledNumber,                         /**< [in] led number */
    GOAL_MA_LED_STATE_T state                   /**< [in] desired LED state */
);

/**< DRV-LED function prototype to toggle the state of a single LED */
typedef GOAL_STATUS_T (*GOAL_MA_LED_TOGGLE)(
    struct GOAL_MA_LED_T *pMaLedHdl,            /**< [in] MA-LED handle */
    uint32_t ledNumber                          /**< [in] led number */
);

/**< DRV-LED function prototype to get the state of a LED group */
typedef GOAL_STATUS_T (*GOAL_MA_LED_GROUP_GET)(
    struct GOAL_MA_LED_T *pMaLedHdl,            /**< [in] LED handle */
    uint32_t *pState                            /**< [out] state of all LEDs in the LED group */
);

/**< DRV-LED function prototype to set the state of a LED group */
typedef GOAL_STATUS_T (*GOAL_MA_LED_GROUP_SET)(
    struct GOAL_MA_LED_T *pMaLedHdl,            /**< [in] LED handle */
    uint32_t mask,                              /**< [in] mask to set LEDs in the LED group individual */
    uint32_t state                              /**< [in] state of the LEDs in the LED group */
);


/****************************************************************************/
/* Structures */
/****************************************************************************/
/** LED properties structure */
typedef struct GOAL_MA_LED_LEDS_T {
    struct GOAL_MA_LED_LEDS_T *pNext;           /**< list element */
    uint32_t id;                                /**< led id */
    uint32_t position;                          /**< led position information */
} GOAL_MA_LED_LEDS_T;

/**< LED handle instance struct */
typedef struct GOAL_MA_LED_T {
    struct GOAL_MA_LED_T *pNext;                /**< next element */
    uint32_t maId;                              /**< MA-ID */
    GOAL_MA_LED_OPEN fOpen;                     /**< open function of DRV-LED */
    GOAL_MA_LED_GET fLedSingleGet;              /**< get function of DRV-LED for a single LED */
    GOAL_MA_LED_SET fLedSingleSet;              /**< set function of DRV_LED for a single LED */
    GOAL_MA_LED_TOGGLE fLedSingleToggle;        /**< toggle function of DRV_LED for a single LED */
    GOAL_MA_LED_GROUP_GET fLedGroupGet;         /**< get function of DRV-LED for a group of LEDs */
    GOAL_MA_LED_GROUP_SET fLedGroupSet;         /**< set function of DRV_LED for a group of LEDs */
    GOAL_MA_LED_CLOSE fClose;                   /**< close function of DRV-LED */
    GOAL_BOOL_T flgUsed;                        /**< usage flag */
    GOAL_MA_LED_LEDS_T *pLeds;                  /**< registered leds */
    void *pDrvData;                             /**< driver-specific data */
} GOAL_MA_LED_T;


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_maLedReg(
    uint32_t id,                                /**< [in] MA-ID */
    GOAL_MA_LED_OPEN fOpen,                     /**< [in] open function of DRV-LED */
    GOAL_MA_LED_GET fLedSingleGet,              /**< [in] get function of DRV-LED for a single LED */
    GOAL_MA_LED_SET fLedSingleSet,              /**< [in] set function of DRV-LED for a single LED */
    GOAL_MA_LED_TOGGLE fLedSingleToggle,        /**< [in] toggle function of DRV-LED for a single LED */
    GOAL_MA_LED_GROUP_GET fLedGroupGet,         /**< [in] get function of DRV-LED for a group of LEDs */
    GOAL_MA_LED_GROUP_SET fLedGroupSet,         /**< [in] set function of DRV-LED for a group of LEDs */
    GOAL_MA_LED_CLOSE fClose,                   /**< [in] close function of DRV-LED */
    void *pDrvData                              /**< [in] driver-specific data */
);

GOAL_STATUS_T goal_maLedDeReg(
    GOAL_MA_LED_T **ppMaLedHdl                  /**< [in] MA-LED handle */
);

GOAL_STATUS_T goal_maLedRegisterLed(
    GOAL_MA_LED_T *pMaLedHdl,                   /**< [in] MA-LED handle */
    uint32_t id,                                /**< led id */
    uint32_t position                           /**< led position */
);

GOAL_STATUS_T goal_maLedOpen(
    uint32_t maId,                              /**< [in] MA-ID */
    GOAL_MA_LED_T **ppMaLedHdl                  /**< [out] MA-LED handle */
);

GOAL_STATUS_T goal_maLedClose(
    GOAL_MA_LED_T *pMaLedHdl                    /**< [in] MA-LED handle */
);

GOAL_STATUS_T goal_maLedGet(
    GOAL_MA_LED_T *pMaLedHdl,                   /**< [in] MA-LED handle */
    uint32_t ledId,                             /**< [in] led id */
    GOAL_MA_LED_STATE_T *pState                 /**< [out] current LED state */
);

GOAL_STATUS_T goal_maLedSet(
    GOAL_MA_LED_T *pMaLedHdl,                   /**< [in] MA-LED handle */
    uint32_t ledId,                             /**< [in] led id */
    GOAL_MA_LED_STATE_T state                   /**< [in] desired LED state */
);

GOAL_STATUS_T goal_maLedToggle(
    GOAL_MA_LED_T *pMaLedHdl,                   /**< [in] MA-LED handle */
    uint32_t ledNumber                          /**< [in] led number */
);

GOAL_STATUS_T goal_maLedGroupGet(
    GOAL_MA_LED_T *pMaLedHdl,                   /**< [in] MA-LED handle */
    uint32_t *pState                            /**< [out] current state of all LEDs in the LED group */
);

GOAL_STATUS_T goal_maLedGroupSet(
    struct GOAL_MA_LED_T *pMaLedHdl,            /**< [in] MA-LED handle */
    uint32_t mask,                              /**< [in] mask to set LEDs in the LED group individual */
    uint32_t state                              /**< [in] desired state of the masked LEDs in the LED group */
);

GOAL_STATUS_T goal_maLedGetById(
    GOAL_MA_LED_T **ppMaLedHdl,                 /**< [out] MA-LED handle */
    uint32_t maId                               /**< [in] MA-ID */
);

#endif /* GOAL_MA_LED_H */
