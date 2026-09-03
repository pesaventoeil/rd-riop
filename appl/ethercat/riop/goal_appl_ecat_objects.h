/** @file
 *
 * @brief EtherCAT Sample application
 *
 * This module contains functions to create the objects (device model) of
 * the EtherCAT slave
 *
 * @copyright
 * Copyright 2025-2026 NXP
 *
 * NXP Confidential and Proprietary. This software is owned or controlled by NXP
 * and may only be used strictly in accordance with the applicable license
 * terms. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you
 * have read, and that you agree to comply with and are bound by, such license
 * terms. If you do not agree to be bound by the applicable license terms, then
 * you may not retain, install, activate or otherwise use the software.
 *
 */


#ifndef GOAL_APPL_ECAT_OBJECTS_H
#define GOAL_APPL_ECAT_OBJECTS_H


#include <goal_includes.h>
#include <goal_ecat.h>

/*  additional typedefs to those in header files */
/** @struct DT10F1;
 * @brief manufacturer specific data type: DT10F1
 */
typedef struct {
    uint32_t LocalErrorReaction;
    uint16_t SyncErrorCounterLimit;
} DT10F1;

/** @struct DT6020;
 * @brief manufacturer specific data type: DT6020
 */
typedef struct {
    uint32_t adc1;
    uint32_t adc2;
    int32_t adc3;
    int32_t adc4;
    float adc5;
    float adc6;
    float adc7;
    float adc8;
    float adc9;
    float adc10;
    float adc11;
    float adc12;
    float adc13;
    float adc14;
    float adc15;
    float adc16;
} DT6020;

/** @struct DT6010;
 * @brief manufacturer specific data type: DT6010
 */
typedef struct {
    uint8_t di1;
    uint8_t di2;
    uint8_t di3;
    uint8_t di4;
    uint8_t di5;
    uint8_t di6;
    uint8_t di7;
    uint8_t di8;
    uint8_t di9;
    uint8_t di10;
    uint8_t di11;
    uint8_t di12;
    uint8_t di13;
    uint8_t di14;
    uint8_t di15;
    uint8_t di16;
    uint8_t di17;
    uint8_t di18;
    uint8_t di19;
    uint8_t di20;
    uint8_t di21;
    uint8_t di22;
    uint8_t di23;
    uint8_t di24;
    uint8_t di25;
    uint8_t di26;
    uint8_t di27;
    uint8_t di28;
    uint8_t di29;
    uint8_t di30;
    uint8_t di31;
    uint8_t di32;
    uint8_t led;
} DT6010;

/** @struct DT7000;
 * @brief manufacturer specific data type: DT7000
 */
typedef struct {
    uint8_t di1;
    uint8_t di2;
    uint8_t di3;
    uint8_t di4;
    uint8_t di5;
    uint8_t di6;
    uint8_t di7;
    uint8_t di8;
    uint8_t di9;
    uint8_t di10;
    uint8_t di11;
    uint8_t di12;
    uint8_t di13;
    uint8_t di14;
    uint8_t di15;
    uint8_t di16;
    uint8_t di17;
    uint8_t di18;
    uint8_t di19;
    uint8_t di20;
    uint8_t di21;
    uint8_t di22;
    uint8_t di23;
    uint8_t di24;
    uint8_t di25;
    uint8_t di26;
    uint8_t di27;
    uint8_t di28;
    uint8_t di29;
    uint8_t di30;
    uint8_t di31;
    uint8_t di32;
    uint8_t led;
} DT7000;

/** @struct DT7010;
 * @brief manufacturer specific data type: DT7010
 */
typedef struct {
    uint32_t adc1;
    uint32_t adc2;
    int32_t adc3;
    int32_t adc4;
    float adc5;
    float adc6;
    float adc7;
    float adc8;
    float adc9;
    float adc10;
    float adc11;
    float adc12;
    float adc13;
    float adc14;
    float adc15;
    float adc16;
} DT7010;

/** @struct DTF000;
 * @brief manufacturer specific data type: DTF000
 */
typedef struct {
    uint16_t Indexdistance;
    uint16_t Maximumnumberofmodules;
} DTF000;


/****************************************************************************/
/* Global variables declaration */
/****************************************************************************/
extern uint8_t di1_out;                         /**< 0x6010:0x01 di1 */
extern uint8_t di2_out;                         /**< 0x6010:0x02 di2 */
extern uint8_t di3_out;                         /**< 0x6010:0x03 di3 */
extern uint8_t di4_out;                         /**< 0x6010:0x04 di4 */
extern uint8_t di5_out;                         /**< 0x6010:0x05 di5 */
extern uint8_t di6_out;                         /**< 0x6010:0x06 di6 */
extern uint8_t di7_out;                         /**< 0x6010:0x07 di7 */
extern uint8_t di8_out;                         /**< 0x6010:0x08 di8 */
extern uint8_t di9_out;                         /**< 0x6010:0x09 di9 */
extern uint8_t di10_out;                        /**< 0x6010:0x0A di10 */
extern uint8_t di11_out;                        /**< 0x6010:0x0B di11 */
extern uint8_t di12_out;                        /**< 0x6010:0x0C di12 */
extern uint8_t di13_out;                        /**< 0x6010:0x0D di13 */
extern uint8_t di14_out;                        /**< 0x6010:0x0E di14 */
extern uint8_t di15_out;                        /**< 0x6010:0x0F di15 */
extern uint8_t di16_out;                        /**< 0x6010:0x10 di16 */
extern uint8_t di17_out;                        /**< 0x6010:0x11 di17 */
extern uint8_t di18_out;                        /**< 0x6010:0x12 di18 */
extern uint8_t di19_out;                        /**< 0x6010:0x13 di19 */
extern uint8_t di20_out;                        /**< 0x6010:0x14 di20 */
extern uint8_t di21_out;                        /**< 0x6010:0x15 di21 */
extern uint8_t di22_out;                        /**< 0x6010:0x16 di22 */
extern uint8_t di23_out;                        /**< 0x6010:0x17 di23 */
extern uint8_t di24_out;                        /**< 0x6010:0x18 di24 */
extern uint8_t di25_out;                        /**< 0x6010:0x19 di25 */
extern uint8_t di26_out;                        /**< 0x6010:0x1A di26 */
extern uint8_t di27_out;                        /**< 0x6010:0x1B di27 */
extern uint8_t di28_out;                        /**< 0x6010:0x1C di28 */
extern uint8_t di29_out;                        /**< 0x6010:0x1D di29 */
extern uint8_t di30_out;                        /**< 0x6010:0x1E di30 */
extern uint8_t di31_out;                        /**< 0x6010:0x1F di31 */
extern uint8_t di32_out;                        /**< 0x6010:0x20 di32 */
extern uint8_t led_out;                         /**< 0x6010:0x21 led */
extern uint32_t adc1_out;                       /**< 0x6020:0x01 adc1 */
extern uint32_t adc2_out;                       /**< 0x6020:0x02 adc2 */
extern int32_t adc3_out;                        /**< 0x6020:0x03 adc3 */
extern int32_t adc4_out;                        /**< 0x6020:0x04 adc4 */
extern float adc5_out;                          /**< 0x6020:0x05 adc5 */
extern float adc6_out;                          /**< 0x6020:0x06 adc6 */
extern float adc7_out;                          /**< 0x6020:0x07 adc7 */
extern float adc8_out;                          /**< 0x6020:0x08 adc8 */
extern float adc9_out;                          /**< 0x6020:0x09 adc9 */
extern float adc10_out;                         /**< 0x6020:0x0A adc10 */
extern float adc11_out;                         /**< 0x6020:0x0B adc11 */
extern float adc12_out;                         /**< 0x6020:0x0C adc12 */
extern float adc13_out;                         /**< 0x6020:0x0D adc13 */
extern float adc14_out;                         /**< 0x6020:0x0E adc14 */
extern float adc15_out;                         /**< 0x6020:0x0F adc15 */
extern float adc16_out;                         /**< 0x6020:0x10 adc16 */
extern uint8_t di1_in;                          /**< 0x7000:0x01 di1 */
extern uint8_t di2_in;                          /**< 0x7000:0x02 di2 */
extern uint8_t di3_in;                          /**< 0x7000:0x03 di3 */
extern uint8_t di4_in;                          /**< 0x7000:0x04 di4 */
extern uint8_t di5_in;                          /**< 0x7000:0x05 di5 */
extern uint8_t di6_in;                          /**< 0x7000:0x06 di6 */
extern uint8_t di7_in;                          /**< 0x7000:0x07 di7 */
extern uint8_t di8_in;                          /**< 0x7000:0x08 di8 */
extern uint8_t di9_in;                          /**< 0x7000:0x09 di9 */
extern uint8_t di10_in;                         /**< 0x7000:0x0A di10 */
extern uint8_t di11_in;                         /**< 0x7000:0x0B di11 */
extern uint8_t di12_in;                         /**< 0x7000:0x0C di12 */
extern uint8_t di13_in;                         /**< 0x7000:0x0D di13 */
extern uint8_t di14_in;                         /**< 0x7000:0x0E di14 */
extern uint8_t di15_in;                         /**< 0x7000:0x0F di15 */
extern uint8_t di16_in;                         /**< 0x7000:0x10 di16 */
extern uint8_t di17_in;                         /**< 0x7000:0x11 di17 */
extern uint8_t di18_in;                         /**< 0x7000:0x12 di18 */
extern uint8_t di19_in;                         /**< 0x7000:0x13 di19 */
extern uint8_t di20_in;                         /**< 0x7000:0x14 di20 */
extern uint8_t di21_in;                         /**< 0x7000:0x15 di21 */
extern uint8_t di22_in;                         /**< 0x7000:0x16 di22 */
extern uint8_t di23_in;                         /**< 0x7000:0x17 di23 */
extern uint8_t di24_in;                         /**< 0x7000:0x18 di24 */
extern uint8_t di25_in;                         /**< 0x7000:0x19 di25 */
extern uint8_t di26_in;                         /**< 0x7000:0x1A di26 */
extern uint8_t di27_in;                         /**< 0x7000:0x1B di27 */
extern uint8_t di28_in;                         /**< 0x7000:0x1C di28 */
extern uint8_t di29_in;                         /**< 0x7000:0x1D di29 */
extern uint8_t di30_in;                         /**< 0x7000:0x1E di30 */
extern uint8_t di31_in;                         /**< 0x7000:0x1F di31 */
extern uint8_t di32_in;                         /**< 0x7000:0x20 di32 */
extern uint8_t led_in;                          /**< 0x7000:0x21 led */
extern uint32_t adc1_in;                        /**< 0x7010:0x01 adc1 */
extern uint32_t adc2_in;                        /**< 0x7010:0x02 adc2 */
extern int32_t adc3_in;                         /**< 0x7010:0x03 adc3 */
extern int32_t adc4_in;                         /**< 0x7010:0x04 adc4 */
extern float adc5_in;                           /**< 0x7010:0x05 adc5 */
extern float adc6_in;                           /**< 0x7010:0x06 adc6 */
extern float adc7_in;                           /**< 0x7010:0x07 adc7 */
extern float adc8_in;                           /**< 0x7010:0x08 adc8 */
extern float adc9_in;                           /**< 0x7010:0x09 adc9 */
extern float adc10_in;                          /**< 0x7010:0x0A adc10 */
extern float adc11_in;                          /**< 0x7010:0x0B adc11 */
extern float adc12_in;                          /**< 0x7010:0x0C adc12 */
extern float adc13_in;                          /**< 0x7010:0x0D adc13 */
extern float adc14_in;                          /**< 0x7010:0x0E adc14 */
extern float adc15_in;                          /**< 0x7010:0x0F adc15 */
extern float adc16_in;                          /**< 0x7010:0x10 adc16 */


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T appl_ecatCreateObjects(
    GOAL_ECAT_T *pHdlEcat                       /**< GOAL EtherCAT handle */
);

#endif /* GOAL_APPL_ECAT_OBJECTS_H */
