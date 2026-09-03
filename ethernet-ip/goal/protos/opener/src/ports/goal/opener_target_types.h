/** @file
 *
 * @brief OpENer target specific types
 *
 * @copyright
 * Copyright 2010-2019 port GmbH Halle/Saale.
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

#ifndef OPENER_TARGET_TYPES_H
#define OPENER_TARGET_TYPES_H

#include <goal_includes.h>

#if GOAL_CONFIG_MCTC == 1
# include <goal_media/goal_mi_mctc.h>
#endif


/****************************************************************************/
/* Configuration */
/****************************************************************************/
#ifndef GOAL_CONFIG_MAX_PORT_COUNT
# define GOAL_CONFIG_MAX_PORT_COUNT 4           /**< maximum number of ethernet ports */
#endif


/****************************************************************************/
/* Public Defines */
/****************************************************************************/
#define OPENER_TIMESTAMP_T GOAL_TIMESTAMP_T     /**< timestamp type */
#define opener_timestampGet goal_targetGetTimestamp /**< timestamp get function */
#define EIP_SOCKT_T GOAL_NET_CHAN_T *           /**< socket handle */
#define EIP_INVALID_SOCKET NULL                 /**< invalid socket handle */
#define EIP_FMT_SOCK  "%p"                      /**< socket format specifier for printf */
#define OPENER_ASSERT GOAL_ASSERT               /**< assert a condition */


/****************************************************************************/
/* Public Data types */
/****************************************************************************/
/** main loop states */
typedef enum {
    OPENER_STATE_INIT,                          /**< initialization */
    OPENER_STATE_DHCP,                          /**< issue DHCP requests */
    OPENER_STATE_RUN,                           /**< process CIP messages */
    OPENER_STATE_WAIT,                          /**< wait for link up */
    OPENER_STATE_IP_SETUP,                      /**< set up IP address */
    OPENER_STATE_IP_PROBED,                     /**< IP address probed */
    OPENER_STATE_IP_WAIT,                       /**< wait till IP address is probed */
    OPENER_STATE_ACD_CONFLICT,                  /**< ACD detected ip address conflict */
    OPENER_STATE_ERROR,                         /**< ERROR state */
} OPENER_STATE_T;

/** driver instance data */
typedef struct {
    EIP_SOCKT_T tcpListener;                    /**< TCP server socket */
    EIP_SOCKT_T udpListenerUnicast;             /**< UDP server socket for unicast (OpENer) */
    EIP_SOCKT_T udpListenerBcast;               /**< UDP server socket for Global broadcast (OpENer) */
    EIP_SOCKT_T udpListenerBcastSn;             /**< UDP server socket for Subnet broadcast (OpENer) */
    EIP_SOCKT_T ioSockPrioLow;                  /**< UDP I/O socket for priority LOW */
    EIP_SOCKT_T ioSockPrioHigh;                 /**< UDP I/O socket for priority HIGH */
    EIP_SOCKT_T ioSockPrioScheduled;            /**< UDP I/O socket for priority SCHEDULED */
    EIP_SOCKT_T ioSockPrioUrgent;               /**< UDP I/O socket for priority URGENT */
    GOAL_BOOL_T writeToFlash;                   /**< save configuration to flash */
    OPENER_STATE_T loopState;                   /**< execution state */
    GOAL_BOOL_T nvmInitialized;                 /**< empty NVM was initialized */
    GOAL_BOOL_T dhcpFlag;                       /**< dhcp is active */
    int numPorts;                               /**< number of external ports */
    GOAL_BOOL_T acdEnable;                      /**< enable ACD */
} EIP_DRV_DATA_T ;

/** LED Test states */
typedef enum {
    EIP_LED_TEST_INIT,                          /**< initial state */
    EIP_LED_TEST_MS_G,                          /**< test green Module Status LED */
    EIP_LED_TEST_MS_R,                          /**< test red Module Status LED */
    EIP_LED_TEST_NS_G,                          /**< test green Network Status LED */
    EIP_LED_TEST_NS_R,                          /**< test red Network Status LED */
    EIP_LED_TEST_DONE,                          /**< tests done */
} EIP_LED_TEST_STATE_T;

/** LED states */
typedef enum {
    EIP_LED_OFF,                                /**< LED is off */
    EIP_LED_ON,                                 /**< LED is on */
    EIP_LED_FLASH,                              /**< LED is flashing */
} EIP_LED_STATE_T;

/** LED IDs */
typedef enum {
    EIP_LED_ID_MS_G,                            /**< Modul Status Green */
    EIP_LED_ID_MS_R,                            /**< Modul Status Red */
    EIP_LED_ID_NS_G,                            /**< Network Status Green */
    EIP_LED_ID_NS_R,                            /**< Network Status Red */
} EIP_LED_ID_T;

/** Bi-Color LED IDs */
typedef enum {
    EIP_LED_BC_ID_MS,                           /**< Module Status Bi-Color LED */
    EIP_LED_BC_ID_NS,                           /**< Network Status Bi-Color LED */
} EIP_LED_BC_ID_T;

/** Bi-Color LED states */
typedef enum {
    EIP_LED_BC_OFF,                             /**< Bi-Color LED is off */
    EIP_LED_BC_GREEN_STEADY,                    /**< Bi-Color LED is steady green */
    EIP_LED_BC_GREEN_FLASHING,                  /**< Bi-Color LED is flashing green */
    EIP_LED_BC_RED_STEADY,                      /**< Bi-Color LED is steady red */
    EIP_LED_BC_RED_FLASHING,                    /**< Bi-Color LED is flashing red */
} EIP_LED_BC_STATE_T;

/** LED instance data */
typedef struct {
    GOAL_LOCK_T *ledLock;                       /**< LED handler lock */
    EIP_LED_TEST_STATE_T testState;             /**< test state */
    GOAL_TIMESTAMP_T testNextTs;                /**< timestamp of next test state */
    uint32_t leds;                              /**< active LEDs */
    EIP_LED_STATE_T msgState;                   /**< state of MS Green */
    GOAL_TIMESTAMP_T msgNext;                   /**< flash toggle time of MS Green */
    EIP_LED_STATE_T msrState;                   /**< state of MS Red */
    GOAL_TIMESTAMP_T msrNext;                   /**< flash toggle time of MS Red */
    EIP_LED_STATE_T nsgState;                   /**< state of NS Green */
    GOAL_TIMESTAMP_T nsgNext;                   /**< flash toggle time of NS Green */
    EIP_LED_STATE_T nsrState;                   /**< state of NS Red */
    GOAL_TIMESTAMP_T nsrNext;                   /**< flash toggle time of NS Red */
} EIP_LED_DATA_T;


#endif /* OPENER_TARGET_TYPES_H */
