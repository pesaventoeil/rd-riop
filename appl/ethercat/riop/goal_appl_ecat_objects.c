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

#include "goal_includes.h"
#include <goal_ecat.h>
#include "goal_appl_ecat_objects.h"


/****************************************************************************/
/* Global variables */
/****************************************************************************/
uint8_t di1_out;                                /**< 0x6010:0x01 di1 */
uint8_t di2_out;                                /**< 0x6010:0x02 di2 */
uint8_t di3_out;                                /**< 0x6010:0x03 di3 */
uint8_t di4_out;                                /**< 0x6010:0x04 di4 */
uint8_t di5_out;                                /**< 0x6010:0x05 di5 */
uint8_t di6_out;                                /**< 0x6010:0x06 di6 */
uint8_t di7_out;                                /**< 0x6010:0x07 di7 */
uint8_t di8_out;                                /**< 0x6010:0x08 di8 */
uint8_t di9_out;                                /**< 0x6010:0x09 di9 */
uint8_t di10_out;                               /**< 0x6010:0x0A di10 */
uint8_t di11_out;                               /**< 0x6010:0x0B di11 */
uint8_t di12_out;                               /**< 0x6010:0x0C di12 */
uint8_t di13_out;                               /**< 0x6010:0x0D di13 */
uint8_t di14_out;                               /**< 0x6010:0x0E di14 */
uint8_t di15_out;                               /**< 0x6010:0x0F di15 */
uint8_t di16_out;                               /**< 0x6010:0x10 di16 */
uint8_t di17_out;                               /**< 0x6010:0x11 di17 */
uint8_t di18_out;                               /**< 0x6010:0x12 di18 */
uint8_t di19_out;                               /**< 0x6010:0x13 di19 */
uint8_t di20_out;                               /**< 0x6010:0x14 di20 */
uint8_t di21_out;                               /**< 0x6010:0x15 di21 */
uint8_t di22_out;                               /**< 0x6010:0x16 di22 */
uint8_t di23_out;                               /**< 0x6010:0x17 di23 */
uint8_t di24_out;                               /**< 0x6010:0x18 di24 */
uint8_t di25_out;                               /**< 0x6010:0x19 di25 */
uint8_t di26_out;                               /**< 0x6010:0x1A di26 */
uint8_t di27_out;                               /**< 0x6010:0x1B di27 */
uint8_t di28_out;                               /**< 0x6010:0x1C di28 */
uint8_t di29_out;                               /**< 0x6010:0x1D di29 */
uint8_t di30_out;                               /**< 0x6010:0x1E di30 */
uint8_t di31_out;                               /**< 0x6010:0x1F di31 */
uint8_t di32_out;                               /**< 0x6010:0x20 di32 */
uint8_t led_out;                                /**< 0x6010:0x21 led */
uint32_t adc1_out;                              /**< 0x6020:0x01 adc1 */
uint32_t adc2_out;                              /**< 0x6020:0x02 adc2 */
int32_t adc3_out;                               /**< 0x6020:0x03 adc3 */
int32_t adc4_out;                               /**< 0x6020:0x04 adc4 */
float adc5_out;                                 /**< 0x6020:0x05 adc5 */
float adc6_out;                                 /**< 0x6020:0x06 adc6 */
float adc7_out;                                 /**< 0x6020:0x07 adc7 */
float adc8_out;                                 /**< 0x6020:0x08 adc8 */
float adc9_out;                                 /**< 0x6020:0x09 adc9 */
float adc10_out;                                /**< 0x6020:0x0A adc10 */
float adc11_out;                                /**< 0x6020:0x0B adc11 */
float adc12_out;                                /**< 0x6020:0x0C adc12 */
float adc13_out;                                /**< 0x6020:0x0D adc13 */
float adc14_out;                                /**< 0x6020:0x0E adc14 */
float adc15_out;                                /**< 0x6020:0x0F adc15 */
float adc16_out;                                /**< 0x6020:0x10 adc16 */
uint8_t di1_in;                                 /**< 0x7000:0x01 di1 */
uint8_t di2_in;                                 /**< 0x7000:0x02 di2 */
uint8_t di3_in;                                 /**< 0x7000:0x03 di3 */
uint8_t di4_in;                                 /**< 0x7000:0x04 di4 */
uint8_t di5_in;                                 /**< 0x7000:0x05 di5 */
uint8_t di6_in;                                 /**< 0x7000:0x06 di6 */
uint8_t di7_in;                                 /**< 0x7000:0x07 di7 */
uint8_t di8_in;                                 /**< 0x7000:0x08 di8 */
uint8_t di9_in;                                 /**< 0x7000:0x09 di9 */
uint8_t di10_in;                                /**< 0x7000:0x0A di10 */
uint8_t di11_in;                                /**< 0x7000:0x0B di11 */
uint8_t di12_in;                                /**< 0x7000:0x0C di12 */
uint8_t di13_in;                                /**< 0x7000:0x0D di13 */
uint8_t di14_in;                                /**< 0x7000:0x0E di14 */
uint8_t di15_in;                                /**< 0x7000:0x0F di15 */
uint8_t di16_in;                                /**< 0x7000:0x10 di16 */
uint8_t di17_in;                                /**< 0x7000:0x11 di17 */
uint8_t di18_in;                                /**< 0x7000:0x12 di18 */
uint8_t di19_in;                                /**< 0x7000:0x13 di19 */
uint8_t di20_in;                                /**< 0x7000:0x14 di20 */
uint8_t di21_in;                                /**< 0x7000:0x15 di21 */
uint8_t di22_in;                                /**< 0x7000:0x16 di22 */
uint8_t di23_in;                                /**< 0x7000:0x17 di23 */
uint8_t di24_in;                                /**< 0x7000:0x18 di24 */
uint8_t di25_in;                                /**< 0x7000:0x19 di25 */
uint8_t di26_in;                                /**< 0x7000:0x1A di26 */
uint8_t di27_in;                                /**< 0x7000:0x1B di27 */
uint8_t di28_in;                                /**< 0x7000:0x1C di28 */
uint8_t di29_in;                                /**< 0x7000:0x1D di29 */
uint8_t di30_in;                                /**< 0x7000:0x1E di30 */
uint8_t di31_in;                                /**< 0x7000:0x1F di31 */
uint8_t di32_in;                                /**< 0x7000:0x20 di32 */
uint8_t led_in;                                 /**< 0x7000:0x21 led */
uint32_t adc1_in;                               /**< 0x7010:0x01 adc1 */
uint32_t adc2_in;                               /**< 0x7010:0x02 adc2 */
int32_t adc3_in;                                /**< 0x7010:0x03 adc3 */
int32_t adc4_in;                                /**< 0x7010:0x04 adc4 */
float adc5_in;                                  /**< 0x7010:0x05 adc5 */
float adc6_in;                                  /**< 0x7010:0x06 adc6 */
float adc7_in;                                  /**< 0x7010:0x07 adc7 */
float adc8_in;                                  /**< 0x7010:0x08 adc8 */
float adc9_in;                                  /**< 0x7010:0x09 adc9 */
float adc10_in;                                 /**< 0x7010:0x0A adc10 */
float adc11_in;                                 /**< 0x7010:0x0B adc11 */
float adc12_in;                                 /**< 0x7010:0x0C adc12 */
float adc13_in;                                 /**< 0x7010:0x0D adc13 */
float adc14_in;                                 /**< 0x7010:0x0E adc14 */
float adc15_in;                                 /**< 0x7010:0x0F adc15 */
float adc16_in;                                 /**< 0x7010:0x10 adc16 */


/****************************************************************************/
/* Local variables */
/****************************************************************************/


/****************************************************************************/
/** Application initialization
 *
 * This function is used to initialize application specific resources.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T appl_ecatCreateObjects(
    GOAL_ECAT_T *pHdlEcat                       /**< GOAL EtherCAT handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    uint32_t uint32ValueDef = 0;                /* value of type uint32_t */
    uint32_t uint32ValueMin = 0;                /* value of type uint32_t */
    uint32_t uint32ValueMax = 0;                /* value of type uint32_t */

    uint8_t uint8ValueDef = 0;                  /* value of type uint8_t */
    uint8_t uint8ValueMin = 0;                  /* value of type uint8_t */
    uint8_t uint8ValueMax = 0;                  /* value of type uint8_t */

    uint16_t uint16ValueDef = 0;                /* value of type uint16_t */
    uint16_t uint16ValueMin = 0;                /* value of type uint16_t */
    uint16_t uint16ValueMax = 0;                /* value of type uint16_t */

    uint64_t uint64ValueDef = 0;                /* value of type uint64_t */
    uint64_t uint64ValueMin = 0;                /* value of type uint64_t */
    uint64_t uint64ValueMax = 0;                /* value of type uint64_t */

    GOAL_BOOL_T boolValueDef = GOAL_FALSE;      /* value of type bool */

    int32_t int32ValueDef = 0;                  /* value of type int32_t */
    int32_t int32ValueMin = 0;                  /* value of type int32_t */
    int32_t int32ValueMax = 0;                  /* value of type int32_t */

    float floatValueDef = 0;                    /* value of type float */
    float floatValueMin = 0;                    /* value of type float */
    float floatValueMax = 0;                    /* value of type float */

    /* create object dictionary */

    /************************************************/
    /* 0x1000 - Device Type                         */
    /************************************************/
    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdObjAdd(pHdlEcat, 0x1000, GOAL_ECAT_OBJCODE_VAR, GOAL_ECAT_DATATYPE_UNSIGNED32);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdObjNameAdd(pHdlEcat, 0x1000, "Device Type");
    }

    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x00001389;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1000,
            0x00,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create object 0x1000");
    }

    /************************************************/
    /* 0x1001 - Error Register                      */
    /************************************************/
    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdObjAdd(pHdlEcat, 0x1001, GOAL_ECAT_OBJCODE_VAR, GOAL_ECAT_DATATYPE_UNSIGNED8);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdObjNameAdd(pHdlEcat, 0x1001, "Error Register");
    }

    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1001,
            0x00,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            NULL);
    }

    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create object 0x1001");
    }

    /************************************************/
    /* 0x1018 - Identity Object                     */
    /************************************************/
    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdObjAdd(pHdlEcat, 0x1018, GOAL_ECAT_OBJCODE_RECORD, 0x23);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdObjNameAdd(pHdlEcat, 0x1018, "Identity Object");
    }

    /************************************************/
    /* 0x1018:0x00 number of entries               */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x01;
        uint8ValueDef = 0x04;
        uint8ValueMax = 0x04;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1018,
            0x00,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1018, 0x00, "number of entries");
    }

    /************************************************/
    /* 0x1018:0x01 Vendor Id                       */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x00000CC2;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1018,
            0x01,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1018, 0x01, "Vendor Id");
    }

    /************************************************/
    /* 0x1018:0x02 Product Code                    */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x00000001;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1018,
            0x02,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1018, 0x02, "Product Code");
    }

    /************************************************/
    /* 0x1018:0x03 Revision number                 */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x00000011;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1018,
            0x03,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1018, 0x03, "Revision number");
    }

    /************************************************/
    /* 0x1018:0x04 Serial number                   */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x00000000;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1018,
            0x04,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1018, 0x04, "Serial number");
    }

    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create object 0x1018");
    }

    /************************************************/
    /* 0x10F1 - Error Settings                      */
    /************************************************/
    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdObjAdd(pHdlEcat, 0x10F1, GOAL_ECAT_OBJCODE_RECORD, 0x40);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdObjNameAdd(pHdlEcat, 0x10F1, "Error Settings");
    }

    /************************************************/
    /* 0x10F1:0x00 SubIndex 000                    */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x02;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x10F1,
            0x00,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x10F1, 0x00, "SubIndex 000");
    }

    /************************************************/
    /* 0x10F1:0x01 Local Error Reaction            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x00000001;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x10F1,
            0x01,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x10F1, 0x01, "Local Error Reaction");
    }

    /************************************************/
    /* 0x10F1:0x02 Sync Error Counter Limit        */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint16ValueMin = 0x0000;
        uint16ValueDef = 0x0004;
        uint16ValueMax = 0xFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x10F1,
            0x02,
            GOAL_ECAT_DATATYPE_UNSIGNED16,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint16ValueDef,
            (uint8_t *) &uint16ValueMin,
            (uint8_t *) &uint16ValueMax,
            2,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x10F1, 0x02, "Sync Error Counter Limit");
    }

    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create object 0x10F1");
    }

    /************************************************/
    /* 0x10F8 - Timestamp Object                    */
    /************************************************/
    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdObjAdd(pHdlEcat, 0x10F8, GOAL_ECAT_OBJCODE_VAR, GOAL_ECAT_DATATYPE_UNSIGNED64);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdObjNameAdd(pHdlEcat, 0x10F8, "Timestamp Object");
    }

    if (GOAL_RES_OK(res)) {

        uint64ValueMin = 0x0000000000000000;
        uint64ValueDef = 0x0000000000000000;
        uint64ValueMax = 0xFFFFFFFFFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x10F8,
            0x00,
            GOAL_ECAT_DATATYPE_UNSIGNED64,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint64ValueDef,
            (uint8_t *) &uint64ValueMin,
            (uint8_t *) &uint64ValueMax,
            8,
            NULL);
    }

    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create object 0x10F8");
    }

    /************************************************/
    /* 0x1600 - Receive PDO Mapping Parameter 1     */
    /************************************************/
    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdObjAdd(pHdlEcat, 0x1600, GOAL_ECAT_OBJCODE_RECORD, 0x21);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdObjNameAdd(pHdlEcat, 0x1600, "Receive PDO Mapping Parameter 1");
    }

    /************************************************/
    /* 0x1600:0x00 Number of Entries               */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x31;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x00,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x00, "Number of Entries");
    }

    /************************************************/
    /* 0x1600:0x01 Mapping Entry 1                 */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70000108;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x01,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x01, "Mapping Entry 1");
    }

    /************************************************/
    /* 0x1600:0x02 Mapping Entry 2                 */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70000208;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x02,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x02, "Mapping Entry 2");
    }

    /************************************************/
    /* 0x1600:0x03 Mapping Entry 3                 */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70000308;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x03,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x03, "Mapping Entry 3");
    }

    /************************************************/
    /* 0x1600:0x04 Mapping Entry 4                 */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70000408;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x04,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x04, "Mapping Entry 4");
    }

    /************************************************/
    /* 0x1600:0x05 Mapping Entry 5                 */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70000508;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x05,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x05, "Mapping Entry 5");
    }

    /************************************************/
    /* 0x1600:0x06 Mapping Entry 6                 */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70000608;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x06,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x06, "Mapping Entry 6");
    }

    /************************************************/
    /* 0x1600:0x07 Mapping Entry 7                 */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70000708;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x07,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x07, "Mapping Entry 7");
    }

    /************************************************/
    /* 0x1600:0x08 Mapping Entry 8                 */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70000808;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x08,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x08, "Mapping Entry 8");
    }

    /************************************************/
    /* 0x1600:0x09 Mapping Entry 9                 */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70000908;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x09,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x09, "Mapping Entry 9");
    }

    /************************************************/
    /* 0x1600:0x0A Mapping Entry 10                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70000A08;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x0A,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x0A, "Mapping Entry 10");
    }

    /************************************************/
    /* 0x1600:0x0B Mapping Entry 11                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70000B08;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x0B,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x0B, "Mapping Entry 11");
    }

    /************************************************/
    /* 0x1600:0x0C Mapping Entry 12                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70000C08;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x0C,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x0C, "Mapping Entry 12");
    }

    /************************************************/
    /* 0x1600:0x0D Mapping Entry 13                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70000D08;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x0D,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x0D, "Mapping Entry 13");
    }

    /************************************************/
    /* 0x1600:0x0E Mapping Entry 14                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70000E08;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x0E,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x0E, "Mapping Entry 14");
    }

    /************************************************/
    /* 0x1600:0x0F Mapping Entry 15                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70000F08;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x0F,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x0F, "Mapping Entry 15");
    }

    /************************************************/
    /* 0x1600:0x10 Mapping Entry 16                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70001008;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x10,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x10, "Mapping Entry 16");
    }

    /************************************************/
    /* 0x1600:0x11 Mapping Entry 17                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70001108;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x11,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x11, "Mapping Entry 17");
    }

    /************************************************/
    /* 0x1600:0x12 Mapping Entry 18                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70001208;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x12,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x12, "Mapping Entry 18");
    }

    /************************************************/
    /* 0x1600:0x13 Mapping Entry 19                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70001308;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x13,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x13, "Mapping Entry 19");
    }

    /************************************************/
    /* 0x1600:0x14 Mapping Entry 20                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70001408;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x14,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x14, "Mapping Entry 20");
    }

    /************************************************/
    /* 0x1600:0x15 Mapping Entry 21                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70001508;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x15,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x15, "Mapping Entry 21");
    }

    /************************************************/
    /* 0x1600:0x16 Mapping Entry 22                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70001608;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x16,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x16, "Mapping Entry 22");
    }

    /************************************************/
    /* 0x1600:0x17 Mapping Entry 23                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70001708;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x17,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x17, "Mapping Entry 23");
    }

    /************************************************/
    /* 0x1600:0x18 Mapping Entry 24                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70001808;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x18,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x18, "Mapping Entry 24");
    }

    /************************************************/
    /* 0x1600:0x19 Mapping Entry 25                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70001908;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x19,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x19, "Mapping Entry 25");
    }

    /************************************************/
    /* 0x1600:0x1A Mapping Entry 26                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70001A08;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x1A,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x1A, "Mapping Entry 26");
    }

    /************************************************/
    /* 0x1600:0x1B Mapping Entry 27                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70001B08;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x1B,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x1B, "Mapping Entry 27");
    }

    /************************************************/
    /* 0x1600:0x1C Mapping Entry 28                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70001C08;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x1C,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x1C, "Mapping Entry 28");
    }

    /************************************************/
    /* 0x1600:0x1D Mapping Entry 29                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70001D08;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x1D,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x1D, "Mapping Entry 29");
    }

    /************************************************/
    /* 0x1600:0x1E Mapping Entry 30                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70001E08;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x1E,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x1E, "Mapping Entry 30");
    }

    /************************************************/
    /* 0x1600:0x1F Mapping Entry 31                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70001F08;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x1F,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x1F, "Mapping Entry 31");
    }

    /************************************************/
    /* 0x1600:0x20 Mapping Entry 32                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70002008;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x20,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x20, "Mapping Entry 32");
    }

    /************************************************/
    /* 0x1600:0x21 Mapping Entry 33                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70002108;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x21,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x21, "Mapping Entry 33");
    }

    /************************************************/
    /* 0x1600:0x22 Mapping Entry 34                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70100120;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x22,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x22, "Mapping Entry 34");
    }

    /************************************************/
    /* 0x1600:0x23 Mapping Entry 35                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70100220;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x23,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x23, "Mapping Entry 35");
    }

    /************************************************/
    /* 0x1600:0x24 Mapping Entry 36                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70100320;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x24,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x24, "Mapping Entry 36");
    }

    /************************************************/
    /* 0x1600:0x25 Mapping Entry 37                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70100420;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x25,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x25, "Mapping Entry 37");
    }

    /************************************************/
    /* 0x1600:0x26 Mapping Entry 38                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70100520;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x26,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x26, "Mapping Entry 38");
    }

    /************************************************/
    /* 0x1600:0x27 Mapping Entry 39                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70100620;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x27,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x27, "Mapping Entry 39");
    }

    /************************************************/
    /* 0x1600:0x28 Mapping Entry 40                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70100720;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x28,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x28, "Mapping Entry 40");
    }

    /************************************************/
    /* 0x1600:0x29 Mapping Entry 41                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70100820;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x29,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x29, "Mapping Entry 41");
    }

    /************************************************/
    /* 0x1600:0x2A Mapping Entry 42                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70100920;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x2A,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x2A, "Mapping Entry 42");
    }

    /************************************************/
    /* 0x1600:0x2B Mapping Entry 43                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70100A20;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x2B,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x2B, "Mapping Entry 43");
    }

    /************************************************/
    /* 0x1600:0x2C Mapping Entry 44                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70100B20;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x2C,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x2C, "Mapping Entry 44");
    }

    /************************************************/
    /* 0x1600:0x2D Mapping Entry 45                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70100C20;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x2D,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x2D, "Mapping Entry 45");
    }

    /************************************************/
    /* 0x1600:0x2E Mapping Entry 46                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70100D20;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x2E,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x2E, "Mapping Entry 46");
    }

    /************************************************/
    /* 0x1600:0x2F Mapping Entry 47                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70100E20;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x2F,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x2F, "Mapping Entry 47");
    }

    /************************************************/
    /* 0x1600:0x30 Mapping Entry 48                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70100F20;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x30,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x30, "Mapping Entry 48");
    }

    /************************************************/
    /* 0x1600:0x31 Mapping Entry 49                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x70101020;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1600,
            0x31,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1600, 0x31, "Mapping Entry 49");
    }

    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create object 0x1600");
    }

    /************************************************/
    /* 0x1A00 - Obj0x1A00                           */
    /************************************************/
    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdObjAdd(pHdlEcat, 0x1A00, GOAL_ECAT_OBJCODE_RECORD, 0x21);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdObjNameAdd(pHdlEcat, 0x1A00, "Obj0x1A00");
    }

    /************************************************/
    /* 0x1A00:0x00 Number of Entries               */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x31;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x00,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x00, "Number of Entries");
    }

    /************************************************/
    /* 0x1A00:0x01 Mapping Entry 1                 */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60100108;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x01,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x01, "Mapping Entry 1");
    }

    /************************************************/
    /* 0x1A00:0x02 Mapping Entry 2                 */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60100208;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x02,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x02, "Mapping Entry 2");
    }

    /************************************************/
    /* 0x1A00:0x03 Mapping Entry 3                 */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60100308;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x03,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x03, "Mapping Entry 3");
    }

    /************************************************/
    /* 0x1A00:0x04 Mapping Entry 4                 */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60100408;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x04,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x04, "Mapping Entry 4");
    }

    /************************************************/
    /* 0x1A00:0x05 Mapping Entry 5                 */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60100508;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x05,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x05, "Mapping Entry 5");
    }

    /************************************************/
    /* 0x1A00:0x06 Mapping Entry 6                 */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60100608;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x06,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x06, "Mapping Entry 6");
    }

    /************************************************/
    /* 0x1A00:0x07 Mapping Entry 7                 */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60100708;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x07,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x07, "Mapping Entry 7");
    }

    /************************************************/
    /* 0x1A00:0x08 Mapping Entry 8                 */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60100808;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x08,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x08, "Mapping Entry 8");
    }

    /************************************************/
    /* 0x1A00:0x09 Mapping Entry 9                 */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60100908;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x09,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x09, "Mapping Entry 9");
    }

    /************************************************/
    /* 0x1A00:0x0A Mapping Entry 10                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60100A08;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x0A,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x0A, "Mapping Entry 10");
    }

    /************************************************/
    /* 0x1A00:0x0B Mapping Entry 11                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60100B08;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x0B,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x0B, "Mapping Entry 11");
    }

    /************************************************/
    /* 0x1A00:0x0C Mapping Entry 12                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60100C08;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x0C,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x0C, "Mapping Entry 12");
    }

    /************************************************/
    /* 0x1A00:0x0D Mapping Entry 13                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60100D08;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x0D,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x0D, "Mapping Entry 13");
    }

    /************************************************/
    /* 0x1A00:0x0E Mapping Entry 14                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60100E08;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x0E,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x0E, "Mapping Entry 14");
    }

    /************************************************/
    /* 0x1A00:0x0F Mapping Entry 15                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60100F08;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x0F,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x0F, "Mapping Entry 15");
    }

    /************************************************/
    /* 0x1A00:0x10 Mapping Entry 16                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60101008;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x10,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x10, "Mapping Entry 16");
    }

    /************************************************/
    /* 0x1A00:0x11 Mapping Entry 17                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60101108;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x11,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x11, "Mapping Entry 17");
    }

    /************************************************/
    /* 0x1A00:0x12 Mapping Entry 18                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60101208;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x12,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x12, "Mapping Entry 18");
    }

    /************************************************/
    /* 0x1A00:0x13 Mapping Entry 19                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60101308;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x13,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x13, "Mapping Entry 19");
    }

    /************************************************/
    /* 0x1A00:0x14 Mapping Entry 20                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60101408;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x14,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x14, "Mapping Entry 20");
    }

    /************************************************/
    /* 0x1A00:0x15 Mapping Entry 21                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60101508;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x15,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x15, "Mapping Entry 21");
    }

    /************************************************/
    /* 0x1A00:0x16 Mapping Entry 22                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60101608;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x16,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x16, "Mapping Entry 22");
    }

    /************************************************/
    /* 0x1A00:0x17 Mapping Entry 23                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60101708;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x17,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x17, "Mapping Entry 23");
    }

    /************************************************/
    /* 0x1A00:0x18 Mapping Entry 24                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60101808;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x18,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x18, "Mapping Entry 24");
    }

    /************************************************/
    /* 0x1A00:0x19 Mapping Entry 25                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60101908;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x19,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x19, "Mapping Entry 25");
    }

    /************************************************/
    /* 0x1A00:0x1A Mapping Entry 26                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60101A08;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x1A,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x1A, "Mapping Entry 26");
    }

    /************************************************/
    /* 0x1A00:0x1B Mapping Entry 27                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60101B08;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x1B,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x1B, "Mapping Entry 27");
    }

    /************************************************/
    /* 0x1A00:0x1C Mapping Entry 28                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60101C08;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x1C,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x1C, "Mapping Entry 28");
    }

    /************************************************/
    /* 0x1A00:0x1D Mapping Entry 29                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60101D08;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x1D,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x1D, "Mapping Entry 29");
    }

    /************************************************/
    /* 0x1A00:0x1E Mapping Entry 30                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60101E08;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x1E,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x1E, "Mapping Entry 30");
    }

    /************************************************/
    /* 0x1A00:0x1F Mapping Entry 31                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60101F08;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x1F,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x1F, "Mapping Entry 31");
    }

    /************************************************/
    /* 0x1A00:0x20 Mapping Entry 32                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60102008;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x20,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x20, "Mapping Entry 32");
    }

    /************************************************/
    /* 0x1A00:0x21 Mapping Entry 33                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60102108;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x21,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x21, "Mapping Entry 33");
    }

    /************************************************/
    /* 0x1A00:0x22 Mapping Entry 34                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60200120;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x22,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x22, "Mapping Entry 34");
    }

    /************************************************/
    /* 0x1A00:0x23 Mapping Entry 35                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60200220;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x23,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x23, "Mapping Entry 35");
    }

    /************************************************/
    /* 0x1A00:0x24 Mapping Entry 36                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60200320;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x24,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x24, "Mapping Entry 36");
    }

    /************************************************/
    /* 0x1A00:0x25 Mapping Entry 37                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60200420;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x25,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x25, "Mapping Entry 37");
    }

    /************************************************/
    /* 0x1A00:0x26 Mapping Entry 38                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60200520;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x26,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x26, "Mapping Entry 38");
    }

    /************************************************/
    /* 0x1A00:0x27 Mapping Entry 39                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60200620;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x27,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x27, "Mapping Entry 39");
    }

    /************************************************/
    /* 0x1A00:0x28 Mapping Entry 40                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60200720;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x28,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x28, "Mapping Entry 40");
    }

    /************************************************/
    /* 0x1A00:0x29 Mapping Entry 41                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60200820;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x29,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x29, "Mapping Entry 41");
    }

    /************************************************/
    /* 0x1A00:0x2A Mapping Entry 42                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60200920;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x2A,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x2A, "Mapping Entry 42");
    }

    /************************************************/
    /* 0x1A00:0x2B Mapping Entry 43                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60200A20;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x2B,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x2B, "Mapping Entry 43");
    }

    /************************************************/
    /* 0x1A00:0x2C Mapping Entry 44                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60200B20;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x2C,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x2C, "Mapping Entry 44");
    }

    /************************************************/
    /* 0x1A00:0x2D Mapping Entry 45                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60200C20;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x2D,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x2D, "Mapping Entry 45");
    }

    /************************************************/
    /* 0x1A00:0x2E Mapping Entry 46                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60200D20;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x2E,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x2E, "Mapping Entry 46");
    }

    /************************************************/
    /* 0x1A00:0x2F Mapping Entry 47                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60200E20;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x2F,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x2F, "Mapping Entry 47");
    }

    /************************************************/
    /* 0x1A00:0x30 Mapping Entry 48                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60200F20;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x30,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x30, "Mapping Entry 48");
    }

    /************************************************/
    /* 0x1A00:0x31 Mapping Entry 49                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x60201020;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1A00,
            0x31,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1A00, 0x31, "Mapping Entry 49");
    }

    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create object 0x1A00");
    }

    /************************************************/
    /* 0x1C00 - Sync Manager Communication Type     */
    /************************************************/
    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdObjAdd(pHdlEcat, 0x1C00, GOAL_ECAT_OBJCODE_ARRAY, GOAL_ECAT_DATATYPE_UNSIGNED8);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdObjNameAdd(pHdlEcat, 0x1C00, "Sync Manager Communication Type");
    }

    /************************************************/
    /* 0x1C00:0x00 Number of used Sync Managers    */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x04;
        uint8ValueDef = 0x04;
        uint8ValueMax = 0x05;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C00,
            0x00,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C00, 0x00, "Number of used Sync Managers");
    }

    /************************************************/
    /* 0x1C00:0x01 Sync Manager  Communication Type */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x01;
        uint8ValueMax = 0x04;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C00,
            0x01,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C00, 0x01, "Sync Manager  Communication Type");
    }

    /************************************************/
    /* 0x1C00:0x02 Sync Manager  Communication Type */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x02;
        uint8ValueMax = 0x04;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C00,
            0x02,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C00, 0x02, "Sync Manager  Communication Type");
    }

    /************************************************/
    /* 0x1C00:0x03 Sync Manager  Communication Type */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x03;
        uint8ValueMax = 0x04;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C00,
            0x03,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C00, 0x03, "Sync Manager  Communication Type");
    }

    /************************************************/
    /* 0x1C00:0x04 Sync Manager  Communication Type */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x04;
        uint8ValueMax = 0x04;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C00,
            0x04,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C00, 0x04, "Sync Manager  Communication Type");
    }

    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create object 0x1C00");
    }

    /************************************************/
    /* 0x1C12 - Sync Manager 2 PDO Assignment       */
    /************************************************/
    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdObjAdd(pHdlEcat, 0x1C12, GOAL_ECAT_OBJCODE_ARRAY, GOAL_ECAT_DATATYPE_UNSIGNED16);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdObjNameAdd(pHdlEcat, 0x1C12, "Sync Manager 2 PDO Assignment");
    }

    /************************************************/
    /* 0x1C12:0x00 Number of assigned PDOs         */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x01;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C12,
            0x00,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C12, 0x00, "Number of assigned PDOs");
    }

    /************************************************/
    /* 0x1C12:0x01 PDO Mapping index of assigned PDOs */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint16ValueMin = 0x1600;
        uint16ValueDef = 0x1600;
        uint16ValueMax = 0x1BFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C12,
            0x01,
            GOAL_ECAT_DATATYPE_UNSIGNED16,
            EC_OBJATTR_RD | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint16ValueDef,
            (uint8_t *) &uint16ValueMin,
            (uint8_t *) &uint16ValueMax,
            2,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C12, 0x01, "PDO Mapping index of assigned PDOs");
    }

    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create object 0x1C12");
    }

    /************************************************/
    /* 0x1C13 - Sync Manager 3 PDO Assignment       */
    /************************************************/
    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdObjAdd(pHdlEcat, 0x1C13, GOAL_ECAT_OBJCODE_ARRAY, GOAL_ECAT_DATATYPE_UNSIGNED16);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdObjNameAdd(pHdlEcat, 0x1C13, "Sync Manager 3 PDO Assignment");
    }

    /************************************************/
    /* 0x1C13:0x00 Number of assigned PDOs         */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x01;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C13,
            0x00,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C13, 0x00, "Number of assigned PDOs");
    }

    /************************************************/
    /* 0x1C13:0x01 PDO Mapping index of assigned PDOs */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint16ValueMin = 0x1600;
        uint16ValueDef = 0x1A00;
        uint16ValueMax = 0x1BFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C13,
            0x01,
            GOAL_ECAT_DATATYPE_UNSIGNED16,
            EC_OBJATTR_RD | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint16ValueDef,
            (uint8_t *) &uint16ValueMin,
            (uint8_t *) &uint16ValueMax,
            2,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C13, 0x01, "PDO Mapping index of assigned PDOs");
    }

    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create object 0x1C13");
    }

    /************************************************/
    /* 0x1C32 - Sync Manager 2 Synchronization      */
    /************************************************/
    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdObjAdd(pHdlEcat, 0x1C32, GOAL_ECAT_OBJCODE_RECORD, 0x29);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdObjNameAdd(pHdlEcat, 0x1C32, "Sync Manager 2 Synchronization");
    }

    /************************************************/
    /* 0x1C32:0x00 Number of Synchronization Parameters */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x01;
        uint8ValueDef = 0x20;
        uint8ValueMax = 0x20;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C32,
            0x00,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C32, 0x00, "Number of Synchronization Parameters");
    }

    /************************************************/
    /* 0x1C32:0x01 Synchronization Type            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint16ValueMin = 0x0000;
        uint16ValueDef = 0x0001;
        uint16ValueMax = 0x003F;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C32,
            0x01,
            GOAL_ECAT_DATATYPE_UNSIGNED16,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC,
            (uint8_t *) &uint16ValueDef,
            (uint8_t *) &uint16ValueMin,
            (uint8_t *) &uint16ValueMax,
            2,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C32, 0x01, "Synchronization Type");
    }

    /************************************************/
    /* 0x1C32:0x02 Cycle Time                      */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x00000000;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C32,
            0x02,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C32, 0x02, "Cycle Time");
    }

    /************************************************/
    /* 0x1C32:0x03 Shift Time                      */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x00000000;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C32,
            0x03,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C32, 0x03, "Shift Time");
    }

    /************************************************/
    /* 0x1C32:0x04 Synchronization Types Supported */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint16ValueMin = 0x0000;
        uint16ValueDef = 0x0007;
        uint16ValueMax = 0x7FFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C32,
            0x04,
            GOAL_ECAT_DATATYPE_UNSIGNED16,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint16ValueDef,
            (uint8_t *) &uint16ValueMin,
            (uint8_t *) &uint16ValueMax,
            2,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C32, 0x04, "Synchronization Types Supported");
    }

    /************************************************/
    /* 0x1C32:0x05 Minimum Cycle Time              */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x000079E0;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C32,
            0x05,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C32, 0x05, "Minimum Cycle Time");
    }

    /************************************************/
    /* 0x1C32:0x06 Calc and Copy Time              */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x00000000;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C32,
            0x06,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C32, 0x06, "Calc and Copy Time");
    }

    /************************************************/
    /* 0x1C32:0x07 Minimum Delay Time              */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x00000000;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C32,
            0x07,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C32, 0x07, "Minimum Delay Time");
    }

    /************************************************/
    /* 0x1C32:0x08 Get Cycle Time                  */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint16ValueMin = 0x0000;
        uint16ValueDef = 0x0000;
        uint16ValueMax = 0x0003;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C32,
            0x08,
            GOAL_ECAT_DATATYPE_UNSIGNED16,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint16ValueDef,
            (uint8_t *) &uint16ValueMin,
            (uint8_t *) &uint16ValueMax,
            2,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C32, 0x08, "Get Cycle Time");
    }

    /************************************************/
    /* 0x1C32:0x09 Delay Time                      */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x00000000;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C32,
            0x09,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C32, 0x09, "Delay Time");
    }

    /************************************************/
    /* 0x1C32:0x0A Sync0 Cycle Time                */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x00000000;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C32,
            0x0A,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C32, 0x0A, "Sync0 Cycle Time");
    }

    /************************************************/
    /* 0x1C32:0x0B SM-Event Missed                 */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint16ValueMin = 0x0000;
        uint16ValueDef = 0x0000;
        uint16ValueMax = 0xFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C32,
            0x0B,
            GOAL_ECAT_DATATYPE_UNSIGNED16,
            EC_OBJATTR_RD | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint16ValueDef,
            (uint8_t *) &uint16ValueMin,
            (uint8_t *) &uint16ValueMax,
            2,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C32, 0x0B, "SM-Event Missed");
    }

    /************************************************/
    /* 0x1C32:0x0C Cycle Time Too Small            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint16ValueMin = 0x0000;
        uint16ValueDef = 0x0000;
        uint16ValueMax = 0xFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C32,
            0x0C,
            GOAL_ECAT_DATATYPE_UNSIGNED16,
            EC_OBJATTR_RD | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint16ValueDef,
            (uint8_t *) &uint16ValueMin,
            (uint8_t *) &uint16ValueMax,
            2,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C32, 0x0C, "Cycle Time Too Small");
    }

    /************************************************/
    /* 0x1C32:0x0D Shift Time Too Short            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint16ValueMin = 0x0000;
        uint16ValueDef = 0x0000;
        uint16ValueMax = 0xFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C32,
            0x0D,
            GOAL_ECAT_DATATYPE_UNSIGNED16,
            EC_OBJATTR_RD | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint16ValueDef,
            (uint8_t *) &uint16ValueMin,
            (uint8_t *) &uint16ValueMax,
            2,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C32, 0x0D, "Shift Time Too Short");
    }

    /************************************************/
    /* 0x1C32:0x0E RxPDO Toggle Failed             */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint16ValueMin = 0x0000;
        uint16ValueDef = 0x0000;
        uint16ValueMax = 0xFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C32,
            0x0E,
            GOAL_ECAT_DATATYPE_UNSIGNED16,
            EC_OBJATTR_RD | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint16ValueDef,
            (uint8_t *) &uint16ValueMin,
            (uint8_t *) &uint16ValueMax,
            2,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C32, 0x0E, "RxPDO Toggle Failed");
    }

    /************************************************/
    /* 0x1C32:0x0F Minimum Cycle Distance          */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x00000000;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C32,
            0x0F,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C32, 0x0F, "Minimum Cycle Distance");
    }

    /************************************************/
    /* 0x1C32:0x10 Maximum Cycle Distance          */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x00000000;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C32,
            0x10,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C32, 0x10, "Maximum Cycle Distance");
    }

    /************************************************/
    /* 0x1C32:0x11 Minimum SM Sync Distance        */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x00000000;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C32,
            0x11,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C32, 0x11, "Minimum SM Sync Distance");
    }

    /************************************************/
    /* 0x1C32:0x12 Maximum SM Sync Distance        */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x00000000;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C32,
            0x12,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C32, 0x12, "Maximum SM Sync Distance");
    }

    /************************************************/
    /* 0x1C32:0x20 Sync Error                      */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        boolValueDef = GOAL_FALSE;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C32,
            0x20,
            GOAL_ECAT_DATATYPE_BOOL,
            EC_OBJATTR_RD | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &boolValueDef,
            NULL,
            NULL,
            sizeof(GOAL_BOOL_T),
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C32, 0x20, "Sync Error");
    }

    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create object 0x1C32");
    }

    /************************************************/
    /* 0x1C33 - Sync Manager 3 Synchronization      */
    /************************************************/
    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdObjAdd(pHdlEcat, 0x1C33, GOAL_ECAT_OBJCODE_RECORD, 0x29);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdObjNameAdd(pHdlEcat, 0x1C33, "Sync Manager 3 Synchronization");
    }

    /************************************************/
    /* 0x1C33:0x00 Number of Synchronization Parameters */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x01;
        uint8ValueDef = 0x20;
        uint8ValueMax = 0x20;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C33,
            0x00,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C33, 0x00, "Number of Synchronization Parameters");
    }

    /************************************************/
    /* 0x1C33:0x01 Synchronization Type            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint16ValueMin = 0x0000;
        uint16ValueDef = 0x0022;
        uint16ValueMax = 0x003F;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C33,
            0x01,
            GOAL_ECAT_DATATYPE_UNSIGNED16,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC,
            (uint8_t *) &uint16ValueDef,
            (uint8_t *) &uint16ValueMin,
            (uint8_t *) &uint16ValueMax,
            2,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C33, 0x01, "Synchronization Type");
    }

    /************************************************/
    /* 0x1C33:0x02 Cycle Time                      */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x00000000;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C33,
            0x02,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C33, 0x02, "Cycle Time");
    }

    /************************************************/
    /* 0x1C33:0x03 Shift Time                      */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x00000000;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C33,
            0x03,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C33, 0x03, "Shift Time");
    }

    /************************************************/
    /* 0x1C33:0x04 Synchronization Types Supported */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint16ValueMin = 0x0000;
        uint16ValueDef = 0x0007;
        uint16ValueMax = 0x7FFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C33,
            0x04,
            GOAL_ECAT_DATATYPE_UNSIGNED16,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint16ValueDef,
            (uint8_t *) &uint16ValueMin,
            (uint8_t *) &uint16ValueMax,
            2,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C33, 0x04, "Synchronization Types Supported");
    }

    /************************************************/
    /* 0x1C33:0x05 Minimum Cycle Time              */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x000079E0;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C33,
            0x05,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C33, 0x05, "Minimum Cycle Time");
    }

    /************************************************/
    /* 0x1C33:0x06 Calc and Copy Time              */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x00000000;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C33,
            0x06,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C33, 0x06, "Calc and Copy Time");
    }

    /************************************************/
    /* 0x1C33:0x07 Minimum Delay Time              */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x00000000;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C33,
            0x07,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C33, 0x07, "Minimum Delay Time");
    }

    /************************************************/
    /* 0x1C33:0x08 Get Cycle Time                  */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint16ValueMin = 0x0000;
        uint16ValueDef = 0x0000;
        uint16ValueMax = 0xFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C33,
            0x08,
            GOAL_ECAT_DATATYPE_UNSIGNED16,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint16ValueDef,
            (uint8_t *) &uint16ValueMin,
            (uint8_t *) &uint16ValueMax,
            2,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C33, 0x08, "Get Cycle Time");
    }

    /************************************************/
    /* 0x1C33:0x09 Delay Time                      */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x00000000;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C33,
            0x09,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C33, 0x09, "Delay Time");
    }

    /************************************************/
    /* 0x1C33:0x0A Sync0 Cyle Time                 */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x00000000;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C33,
            0x0A,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C33, 0x0A, "Sync0 Cyle Time");
    }

    /************************************************/
    /* 0x1C33:0x0B SM-Event Missed                 */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint16ValueMin = 0x0000;
        uint16ValueDef = 0x0000;
        uint16ValueMax = 0xFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C33,
            0x0B,
            GOAL_ECAT_DATATYPE_UNSIGNED16,
            EC_OBJATTR_RD | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint16ValueDef,
            (uint8_t *) &uint16ValueMin,
            (uint8_t *) &uint16ValueMax,
            2,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C33, 0x0B, "SM-Event Missed");
    }

    /************************************************/
    /* 0x1C33:0x0C Cycle Time Too Small            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint16ValueMin = 0x0000;
        uint16ValueDef = 0x0000;
        uint16ValueMax = 0xFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C33,
            0x0C,
            GOAL_ECAT_DATATYPE_UNSIGNED16,
            EC_OBJATTR_RD | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint16ValueDef,
            (uint8_t *) &uint16ValueMin,
            (uint8_t *) &uint16ValueMax,
            2,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C33, 0x0C, "Cycle Time Too Small");
    }

    /************************************************/
    /* 0x1C33:0x0D Shift Time Too Short            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint16ValueMin = 0x0000;
        uint16ValueDef = 0x0000;
        uint16ValueMax = 0xFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C33,
            0x0D,
            GOAL_ECAT_DATATYPE_UNSIGNED16,
            EC_OBJATTR_RD | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint16ValueDef,
            (uint8_t *) &uint16ValueMin,
            (uint8_t *) &uint16ValueMax,
            2,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C33, 0x0D, "Shift Time Too Short");
    }

    /************************************************/
    /* 0x1C33:0x0E RxPDO Toggle Failed             */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint16ValueMin = 0x0000;
        uint16ValueDef = 0x0000;
        uint16ValueMax = 0xFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C33,
            0x0E,
            GOAL_ECAT_DATATYPE_UNSIGNED16,
            EC_OBJATTR_RD | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint16ValueDef,
            (uint8_t *) &uint16ValueMin,
            (uint8_t *) &uint16ValueMax,
            2,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C33, 0x0E, "RxPDO Toggle Failed");
    }

    /************************************************/
    /* 0x1C33:0x0F Minimum Cycle Distance          */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x00000000;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C33,
            0x0F,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C33, 0x0F, "Minimum Cycle Distance");
    }

    /************************************************/
    /* 0x1C33:0x10 Maximum Cycle Distance          */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x00000000;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C33,
            0x10,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C33, 0x10, "Maximum Cycle Distance");
    }

    /************************************************/
    /* 0x1C33:0x11 Minimum SM Sync Distance        */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x00000000;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C33,
            0x11,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C33, 0x11, "Minimum SM Sync Distance");
    }

    /************************************************/
    /* 0x1C33:0x12 Maximum SM Sync Distance        */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x00000000;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C33,
            0x12,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C33, 0x12, "Maximum SM Sync Distance");
    }

    /************************************************/
    /* 0x1C33:0x20 Sync Error                      */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        boolValueDef = GOAL_FALSE;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x1C33,
            0x20,
            GOAL_ECAT_DATATYPE_BOOL,
            EC_OBJATTR_RD | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &boolValueDef,
            NULL,
            NULL,
            sizeof(GOAL_BOOL_T),
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x1C33, 0x20, "Sync Error");
    }

    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create object 0x1C33");
    }

    /************************************************/
    /* 0x6010 - Obj0x6010                           */
    /************************************************/
    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdObjAdd(pHdlEcat, 0x6010, GOAL_ECAT_OBJCODE_RECORD, 0x42);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdObjNameAdd(pHdlEcat, 0x6010, "Obj0x6010");
    }

    /************************************************/
    /* 0x6010:0x00 numOfEntries                    */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x21;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6010,
            0x00,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6010, 0x00, "numOfEntries");
    }

    /************************************************/
    /* 0x6010:0x01 di1                             */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6010,
            0x01,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD_PREOP | EC_OBJATTR_RD_SAFEOP | EC_OBJATTR_RD_OP | EC_OBJATTR_WR_OP | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di1_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6010, 0x01, "di1");
    }

    /************************************************/
    /* 0x6010:0x02 di2                             */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6010,
            0x02,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di2_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6010, 0x02, "di2");
    }

    /************************************************/
    /* 0x6010:0x03 di3                             */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6010,
            0x03,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di3_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6010, 0x03, "di3");
    }

    /************************************************/
    /* 0x6010:0x04 di4                             */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6010,
            0x04,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di4_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6010, 0x04, "di4");
    }

    /************************************************/
    /* 0x6010:0x05 di5                             */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6010,
            0x05,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di5_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6010, 0x05, "di5");
    }

    /************************************************/
    /* 0x6010:0x06 di6                             */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6010,
            0x06,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di6_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6010, 0x06, "di6");
    }

    /************************************************/
    /* 0x6010:0x07 di7                             */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6010,
            0x07,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di7_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6010, 0x07, "di7");
    }

    /************************************************/
    /* 0x6010:0x08 di8                             */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6010,
            0x08,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di8_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6010, 0x08, "di8");
    }

    /************************************************/
    /* 0x6010:0x09 di9                             */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6010,
            0x09,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di9_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6010, 0x09, "di9");
    }

    /************************************************/
    /* 0x6010:0x0A di10                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6010,
            0x0A,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di10_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6010, 0x0A, "di10");
    }

    /************************************************/
    /* 0x6010:0x0B di11                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6010,
            0x0B,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di11_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6010, 0x0B, "di11");
    }

    /************************************************/
    /* 0x6010:0x0C di12                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6010,
            0x0C,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di12_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6010, 0x0C, "di12");
    }

    /************************************************/
    /* 0x6010:0x0D di13                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6010,
            0x0D,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di13_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6010, 0x0D, "di13");
    }

    /************************************************/
    /* 0x6010:0x0E di14                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6010,
            0x0E,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di14_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6010, 0x0E, "di14");
    }

    /************************************************/
    /* 0x6010:0x0F di15                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6010,
            0x0F,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di15_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6010, 0x0F, "di15");
    }

    /************************************************/
    /* 0x6010:0x10 di16                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6010,
            0x10,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di16_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6010, 0x10, "di16");
    }

    /************************************************/
    /* 0x6010:0x11 di17                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6010,
            0x11,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di17_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6010, 0x11, "di17");
    }

    /************************************************/
    /* 0x6010:0x12 di18                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6010,
            0x12,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di18_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6010, 0x12, "di18");
    }

    /************************************************/
    /* 0x6010:0x13 di19                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6010,
            0x13,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di19_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6010, 0x13, "di19");
    }

    /************************************************/
    /* 0x6010:0x14 di20                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6010,
            0x14,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di20_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6010, 0x14, "di20");
    }

    /************************************************/
    /* 0x6010:0x15 di21                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6010,
            0x15,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di21_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6010, 0x15, "di21");
    }

    /************************************************/
    /* 0x6010:0x16 di22                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6010,
            0x16,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di22_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6010, 0x16, "di22");
    }

    /************************************************/
    /* 0x6010:0x17 di23                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6010,
            0x17,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di23_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6010, 0x17, "di23");
    }

    /************************************************/
    /* 0x6010:0x18 di24                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6010,
            0x18,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di24_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6010, 0x18, "di24");
    }

    /************************************************/
    /* 0x6010:0x19 di25                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6010,
            0x19,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di25_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6010, 0x19, "di25");
    }

    /************************************************/
    /* 0x6010:0x1A di26                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6010,
            0x1A,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di26_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6010, 0x1A, "di26");
    }

    /************************************************/
    /* 0x6010:0x1B di27                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6010,
            0x1B,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di27_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6010, 0x1B, "di27");
    }

    /************************************************/
    /* 0x6010:0x1C di28                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6010,
            0x1C,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di28_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6010, 0x1C, "di28");
    }

    /************************************************/
    /* 0x6010:0x1D di29                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6010,
            0x1D,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di29_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6010, 0x1D, "di29");
    }

    /************************************************/
    /* 0x6010:0x1E di30                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6010,
            0x1E,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di30_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6010, 0x1E, "di30");
    }

    /************************************************/
    /* 0x6010:0x1F di31                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6010,
            0x1F,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di31_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6010, 0x1F, "di31");
    }

    /************************************************/
    /* 0x6010:0x20 di32                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6010,
            0x20,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di32_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6010, 0x20, "di32");
    }

    /************************************************/
    /* 0x6010:0x21 led                             */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x01;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6010,
            0x21,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &led_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6010, 0x21, "led");
    }

    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create object 0x6010");
    }

    /************************************************/
    /* 0x6020 - Obj0x6020                           */
    /************************************************/
    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdObjAdd(pHdlEcat, 0x6020, GOAL_ECAT_OBJCODE_RECORD, 0x41);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdObjNameAdd(pHdlEcat, 0x6020, "Obj0x6020");
    }

    /************************************************/
    /* 0x6020:0x00 numOfEntries                    */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x10;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6020,
            0x00,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6020, 0x00, "numOfEntries");
    }

    /************************************************/
    /* 0x6020:0x01 adc1                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x00000000;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6020,
            0x01,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            (uint8_t *) &adc1_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6020, 0x01, "adc1");
    }

    /************************************************/
    /* 0x6020:0x02 adc2                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x00000000;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6020,
            0x02,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            (uint8_t *) &adc2_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6020, 0x02, "adc2");
    }

    /************************************************/
    /* 0x6020:0x03 adc3                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        int32ValueMin = -2147483648;
        int32ValueDef = 0;
        int32ValueMax = 2147483647;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6020,
            0x03,
            GOAL_ECAT_DATATYPE_INTEGER32,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &int32ValueDef,
            (uint8_t *) &int32ValueMin,
            (uint8_t *) &int32ValueMax,
            4,
            (uint8_t *) &adc3_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6020, 0x03, "adc3");
    }

    /************************************************/
    /* 0x6020:0x04 adc4                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        int32ValueMin = -2147483648;
        int32ValueDef = 0;
        int32ValueMax = 2147483647;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6020,
            0x04,
            GOAL_ECAT_DATATYPE_INTEGER32,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &int32ValueDef,
            (uint8_t *) &int32ValueMin,
            (uint8_t *) &int32ValueMax,
            4,
            (uint8_t *) &adc4_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6020, 0x04, "adc4");
    }

    /************************************************/
    /* 0x6020:0x05 adc5                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        floatValueMin = -3.40282346639e+38f;
        floatValueDef = 0.0;
        floatValueMax = 3.40282346639e+38f;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6020,
            0x05,
            GOAL_ECAT_DATATYPE_REAL32,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &floatValueDef,
            (uint8_t *) &floatValueMin,
            (uint8_t *) &floatValueMax,
            sizeof(float),
            (uint8_t *) &adc5_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6020, 0x05, "adc5");
    }

    /************************************************/
    /* 0x6020:0x06 adc6                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        floatValueMin = -3.40282346639e+38f;
        floatValueDef = 0.0;
        floatValueMax = 3.40282346639e+38f;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6020,
            0x06,
            GOAL_ECAT_DATATYPE_REAL32,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &floatValueDef,
            (uint8_t *) &floatValueMin,
            (uint8_t *) &floatValueMax,
            sizeof(float),
            (uint8_t *) &adc6_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6020, 0x06, "adc6");
    }

    /************************************************/
    /* 0x6020:0x07 adc7                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        floatValueMin = -3.40282346639e+38f;
        floatValueDef = 0.0;
        floatValueMax = 3.40282346639e+38f;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6020,
            0x07,
            GOAL_ECAT_DATATYPE_REAL32,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &floatValueDef,
            (uint8_t *) &floatValueMin,
            (uint8_t *) &floatValueMax,
            sizeof(float),
            (uint8_t *) &adc7_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6020, 0x07, "adc7");
    }

    /************************************************/
    /* 0x6020:0x08 adc8                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        floatValueMin = -3.40282346639e+38f;
        floatValueDef = 0.0;
        floatValueMax = 3.40282346639e+38f;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6020,
            0x08,
            GOAL_ECAT_DATATYPE_REAL32,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &floatValueDef,
            (uint8_t *) &floatValueMin,
            (uint8_t *) &floatValueMax,
            sizeof(float),
            (uint8_t *) &adc8_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6020, 0x08, "adc8");
    }

    /************************************************/
    /* 0x6020:0x09 adc9                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        floatValueMin = -3.40282346639e+38f;
        floatValueDef = 0.0;
        floatValueMax = 3.40282346639e+38f;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6020,
            0x09,
            GOAL_ECAT_DATATYPE_REAL32,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &floatValueDef,
            (uint8_t *) &floatValueMin,
            (uint8_t *) &floatValueMax,
            sizeof(float),
            (uint8_t *) &adc9_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6020, 0x09, "adc9");
    }

    /************************************************/
    /* 0x6020:0x0A adc10                           */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        floatValueMin = -3.40282346639e+38f;
        floatValueDef = 0.0;
        floatValueMax = 3.40282346639e+38f;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6020,
            0x0A,
            GOAL_ECAT_DATATYPE_REAL32,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &floatValueDef,
            (uint8_t *) &floatValueMin,
            (uint8_t *) &floatValueMax,
            sizeof(float),
            (uint8_t *) &adc10_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6020, 0x0A, "adc10");
    }

    /************************************************/
    /* 0x6020:0x0B adc11                           */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        floatValueMin = -3.40282346639e+38f;
        floatValueDef = 0.0;
        floatValueMax = 3.40282346639e+38f;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6020,
            0x0B,
            GOAL_ECAT_DATATYPE_REAL32,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &floatValueDef,
            (uint8_t *) &floatValueMin,
            (uint8_t *) &floatValueMax,
            sizeof(float),
            (uint8_t *) &adc11_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6020, 0x0B, "adc11");
    }

    /************************************************/
    /* 0x6020:0x0C adc12                           */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        floatValueMin = -3.40282346639e+38f;
        floatValueDef = 0.0;
        floatValueMax = 3.40282346639e+38f;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6020,
            0x0C,
            GOAL_ECAT_DATATYPE_REAL32,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &floatValueDef,
            (uint8_t *) &floatValueMin,
            (uint8_t *) &floatValueMax,
            sizeof(float),
            (uint8_t *) &adc12_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6020, 0x0C, "adc12");
    }

    /************************************************/
    /* 0x6020:0x0D adc13                           */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        floatValueMin = -3.40282346639e+38f;
        floatValueDef = 0.0;
        floatValueMax = 3.40282346639e+38f;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6020,
            0x0D,
            GOAL_ECAT_DATATYPE_REAL32,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &floatValueDef,
            (uint8_t *) &floatValueMin,
            (uint8_t *) &floatValueMax,
            sizeof(float),
            (uint8_t *) &adc13_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6020, 0x0D, "adc13");
    }

    /************************************************/
    /* 0x6020:0x0E adc14                           */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        floatValueMin = -3.40282346639e+38f;
        floatValueDef = 0.0;
        floatValueMax = 3.40282346639e+38f;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6020,
            0x0E,
            GOAL_ECAT_DATATYPE_REAL32,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &floatValueDef,
            (uint8_t *) &floatValueMin,
            (uint8_t *) &floatValueMax,
            sizeof(float),
            (uint8_t *) &adc14_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6020, 0x0E, "adc14");
    }

    /************************************************/
    /* 0x6020:0x0F adc15                           */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        floatValueMin = -3.40282346639e+38f;
        floatValueDef = 0.0;
        floatValueMax = 3.40282346639e+38f;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6020,
            0x0F,
            GOAL_ECAT_DATATYPE_REAL32,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &floatValueDef,
            (uint8_t *) &floatValueMin,
            (uint8_t *) &floatValueMax,
            sizeof(float),
            (uint8_t *) &adc15_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6020, 0x0F, "adc15");
    }

    /************************************************/
    /* 0x6020:0x10 adc16                           */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        floatValueMin = -3.40282346639e+38f;
        floatValueDef = 0.0;
        floatValueMax = 3.40282346639e+38f;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x6020,
            0x10,
            GOAL_ECAT_DATATYPE_REAL32,
            EC_OBJATTR_RD | EC_OBJATTR_TXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &floatValueDef,
            (uint8_t *) &floatValueMin,
            (uint8_t *) &floatValueMax,
            sizeof(float),
            (uint8_t *) &adc16_out);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x6020, 0x10, "adc16");
    }

    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create object 0x6020");
    }

    /************************************************/
    /* 0x7000 - Obj0x7000                           */
    /************************************************/
    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdObjAdd(pHdlEcat, 0x7000, GOAL_ECAT_OBJCODE_RECORD, 0x43);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdObjNameAdd(pHdlEcat, 0x7000, "Obj0x7000");
    }

    /************************************************/
    /* 0x7000:0x00 numOfEntries                    */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x21;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7000,
            0x00,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7000, 0x00, "numOfEntries");
    }

    /************************************************/
    /* 0x7000:0x01 di1                             */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7000,
            0x01,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di1_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7000, 0x01, "di1");
    }

    /************************************************/
    /* 0x7000:0x02 di2                             */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7000,
            0x02,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di2_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7000, 0x02, "di2");
    }

    /************************************************/
    /* 0x7000:0x03 di3                             */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7000,
            0x03,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di3_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7000, 0x03, "di3");
    }

    /************************************************/
    /* 0x7000:0x04 di4                             */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7000,
            0x04,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di4_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7000, 0x04, "di4");
    }

    /************************************************/
    /* 0x7000:0x05 di5                             */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7000,
            0x05,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di5_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7000, 0x05, "di5");
    }

    /************************************************/
    /* 0x7000:0x06 di6                             */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7000,
            0x06,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di6_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7000, 0x06, "di6");
    }

    /************************************************/
    /* 0x7000:0x07 di7                             */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7000,
            0x07,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di7_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7000, 0x07, "di7");
    }

    /************************************************/
    /* 0x7000:0x08 di8                             */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7000,
            0x08,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di8_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7000, 0x08, "di8");
    }

    /************************************************/
    /* 0x7000:0x09 di9                             */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7000,
            0x09,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di9_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7000, 0x09, "di9");
    }

    /************************************************/
    /* 0x7000:0x0A di10                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7000,
            0x0A,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di10_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7000, 0x0A, "di10");
    }

    /************************************************/
    /* 0x7000:0x0B di11                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7000,
            0x0B,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di11_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7000, 0x0B, "di11");
    }

    /************************************************/
    /* 0x7000:0x0C di12                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7000,
            0x0C,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di12_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7000, 0x0C, "di12");
    }

    /************************************************/
    /* 0x7000:0x0D di13                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7000,
            0x0D,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di13_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7000, 0x0D, "di13");
    }

    /************************************************/
    /* 0x7000:0x0E di14                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7000,
            0x0E,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di14_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7000, 0x0E, "di14");
    }

    /************************************************/
    /* 0x7000:0x0F di15                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7000,
            0x0F,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di15_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7000, 0x0F, "di15");
    }

    /************************************************/
    /* 0x7000:0x10 di16                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7000,
            0x10,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di16_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7000, 0x10, "di16");
    }

    /************************************************/
    /* 0x7000:0x11 di17                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7000,
            0x11,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di17_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7000, 0x11, "di17");
    }

    /************************************************/
    /* 0x7000:0x12 di18                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7000,
            0x12,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di18_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7000, 0x12, "di18");
    }

    /************************************************/
    /* 0x7000:0x13 di19                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7000,
            0x13,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di19_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7000, 0x13, "di19");
    }

    /************************************************/
    /* 0x7000:0x14 di20                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7000,
            0x14,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di20_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7000, 0x14, "di20");
    }

    /************************************************/
    /* 0x7000:0x15 di21                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7000,
            0x15,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di21_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7000, 0x15, "di21");
    }

    /************************************************/
    /* 0x7000:0x16 di22                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7000,
            0x16,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di22_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7000, 0x16, "di22");
    }

    /************************************************/
    /* 0x7000:0x17 di23                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7000,
            0x17,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di23_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7000, 0x17, "di23");
    }

    /************************************************/
    /* 0x7000:0x18 di24                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7000,
            0x18,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di24_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7000, 0x18, "di24");
    }

    /************************************************/
    /* 0x7000:0x19 di25                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7000,
            0x19,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di25_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7000, 0x19, "di25");
    }

    /************************************************/
    /* 0x7000:0x1A di26                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7000,
            0x1A,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di26_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7000, 0x1A, "di26");
    }

    /************************************************/
    /* 0x7000:0x1B di27                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7000,
            0x1B,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di27_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7000, 0x1B, "di27");
    }

    /************************************************/
    /* 0x7000:0x1C di28                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7000,
            0x1C,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di28_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7000, 0x1C, "di28");
    }

    /************************************************/
    /* 0x7000:0x1D di29                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7000,
            0x1D,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di29_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7000, 0x1D, "di29");
    }

    /************************************************/
    /* 0x7000:0x1E di30                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7000,
            0x1E,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di30_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7000, 0x1E, "di30");
    }

    /************************************************/
    /* 0x7000:0x1F di31                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7000,
            0x1F,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di31_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7000, 0x1F, "di31");
    }

    /************************************************/
    /* 0x7000:0x20 di32                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x00;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7000,
            0x20,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &di32_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7000, 0x20, "di32");
    }

    /************************************************/
    /* 0x7000:0x21 led                             */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x01;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7000,
            0x21,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            (uint8_t *) &led_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7000, 0x21, "led");
    }

    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create object 0x7000");
    }

    /************************************************/
    /* 0x7010 - Obj0x7010                           */
    /************************************************/
    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdObjAdd(pHdlEcat, 0x7010, GOAL_ECAT_OBJCODE_RECORD, 0x44);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdObjNameAdd(pHdlEcat, 0x7010, "Obj0x7010");
    }

    /************************************************/
    /* 0x7010:0x00 numOfEntries                    */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x10;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7010,
            0x00,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7010, 0x00, "numOfEntries");
    }

    /************************************************/
    /* 0x7010:0x01 adc1                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x00000000;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7010,
            0x01,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            (uint8_t *) &adc1_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7010, 0x01, "adc1");
    }

    /************************************************/
    /* 0x7010:0x02 adc2                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint32ValueMin = 0x00000000;
        uint32ValueDef = 0x00000000;
        uint32ValueMax = 0xFFFFFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7010,
            0x02,
            GOAL_ECAT_DATATYPE_UNSIGNED32,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &uint32ValueDef,
            (uint8_t *) &uint32ValueMin,
            (uint8_t *) &uint32ValueMax,
            4,
            (uint8_t *) &adc2_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7010, 0x02, "adc2");
    }

    /************************************************/
    /* 0x7010:0x03 adc3                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        int32ValueMin = -2147483648;
        int32ValueDef = 0;
        int32ValueMax = 2147483647;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7010,
            0x03,
            GOAL_ECAT_DATATYPE_INTEGER32,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &int32ValueDef,
            (uint8_t *) &int32ValueMin,
            (uint8_t *) &int32ValueMax,
            4,
            (uint8_t *) &adc3_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7010, 0x03, "adc3");
    }

    /************************************************/
    /* 0x7010:0x04 adc4                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        int32ValueMin = -2147483648;
        int32ValueDef = 0;
        int32ValueMax = 2147483647;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7010,
            0x04,
            GOAL_ECAT_DATATYPE_INTEGER32,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &int32ValueDef,
            (uint8_t *) &int32ValueMin,
            (uint8_t *) &int32ValueMax,
            4,
            (uint8_t *) &adc4_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7010, 0x04, "adc4");
    }

    /************************************************/
    /* 0x7010:0x05 adc5                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        floatValueMin = -3.40282346639e+38f;
        floatValueDef = 0.0;
        floatValueMax = 3.40282346639e+38f;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7010,
            0x05,
            GOAL_ECAT_DATATYPE_REAL32,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &floatValueDef,
            (uint8_t *) &floatValueMin,
            (uint8_t *) &floatValueMax,
            sizeof(float),
            (uint8_t *) &adc5_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7010, 0x05, "adc5");
    }

    /************************************************/
    /* 0x7010:0x06 adc6                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        floatValueMin = -3.40282346639e+38f;
        floatValueDef = 0.0;
        floatValueMax = 3.40282346639e+38f;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7010,
            0x06,
            GOAL_ECAT_DATATYPE_REAL32,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &floatValueDef,
            (uint8_t *) &floatValueMin,
            (uint8_t *) &floatValueMax,
            sizeof(float),
            (uint8_t *) &adc6_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7010, 0x06, "adc6");
    }

    /************************************************/
    /* 0x7010:0x07 adc7                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        floatValueMin = -3.40282346639e+38f;
        floatValueDef = 0.0;
        floatValueMax = 3.40282346639e+38f;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7010,
            0x07,
            GOAL_ECAT_DATATYPE_REAL32,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &floatValueDef,
            (uint8_t *) &floatValueMin,
            (uint8_t *) &floatValueMax,
            sizeof(float),
            (uint8_t *) &adc7_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7010, 0x07, "adc7");
    }

    /************************************************/
    /* 0x7010:0x08 adc8                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        floatValueMin = -3.40282346639e+38f;
        floatValueDef = 0.0;
        floatValueMax = 3.40282346639e+38f;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7010,
            0x08,
            GOAL_ECAT_DATATYPE_REAL32,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &floatValueDef,
            (uint8_t *) &floatValueMin,
            (uint8_t *) &floatValueMax,
            sizeof(float),
            (uint8_t *) &adc8_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7010, 0x08, "adc8");
    }

    /************************************************/
    /* 0x7010:0x09 adc9                            */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        floatValueMin = -3.40282346639e+38f;
        floatValueDef = 0.0;
        floatValueMax = 3.40282346639e+38f;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7010,
            0x09,
            GOAL_ECAT_DATATYPE_REAL32,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &floatValueDef,
            (uint8_t *) &floatValueMin,
            (uint8_t *) &floatValueMax,
            sizeof(float),
            (uint8_t *) &adc9_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7010, 0x09, "adc9");
    }

    /************************************************/
    /* 0x7010:0x0A adc10                           */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        floatValueMin = -3.40282346639e+38f;
        floatValueDef = 0.0;
        floatValueMax = 3.40282346639e+38f;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7010,
            0x0A,
            GOAL_ECAT_DATATYPE_REAL32,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &floatValueDef,
            (uint8_t *) &floatValueMin,
            (uint8_t *) &floatValueMax,
            sizeof(float),
            (uint8_t *) &adc10_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7010, 0x0A, "adc10");
    }

    /************************************************/
    /* 0x7010:0x0B adc11                           */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        floatValueMin = -3.40282346639e+38f;
        floatValueDef = 0.0;
        floatValueMax = 3.40282346639e+38f;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7010,
            0x0B,
            GOAL_ECAT_DATATYPE_REAL32,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &floatValueDef,
            (uint8_t *) &floatValueMin,
            (uint8_t *) &floatValueMax,
            sizeof(float),
            (uint8_t *) &adc11_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7010, 0x0B, "adc11");
    }

    /************************************************/
    /* 0x7010:0x0C adc12                           */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        floatValueMin = -3.40282346639e+38f;
        floatValueDef = 0.0;
        floatValueMax = 3.40282346639e+38f;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7010,
            0x0C,
            GOAL_ECAT_DATATYPE_REAL32,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &floatValueDef,
            (uint8_t *) &floatValueMin,
            (uint8_t *) &floatValueMax,
            sizeof(float),
            (uint8_t *) &adc12_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7010, 0x0C, "adc12");
    }

    /************************************************/
    /* 0x7010:0x0D adc13                           */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        floatValueMin = -3.40282346639e+38f;
        floatValueDef = 0.0;
        floatValueMax = 3.40282346639e+38f;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7010,
            0x0D,
            GOAL_ECAT_DATATYPE_REAL32,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &floatValueDef,
            (uint8_t *) &floatValueMin,
            (uint8_t *) &floatValueMax,
            sizeof(float),
            (uint8_t *) &adc13_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7010, 0x0D, "adc13");
    }

    /************************************************/
    /* 0x7010:0x0E adc14                           */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        floatValueMin = -3.40282346639e+38f;
        floatValueDef = 0.0;
        floatValueMax = 3.40282346639e+38f;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7010,
            0x0E,
            GOAL_ECAT_DATATYPE_REAL32,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &floatValueDef,
            (uint8_t *) &floatValueMin,
            (uint8_t *) &floatValueMax,
            sizeof(float),
            (uint8_t *) &adc14_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7010, 0x0E, "adc14");
    }

    /************************************************/
    /* 0x7010:0x0F adc15                           */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        floatValueMin = -3.40282346639e+38f;
        floatValueDef = 0.0;
        floatValueMax = 3.40282346639e+38f;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7010,
            0x0F,
            GOAL_ECAT_DATATYPE_REAL32,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &floatValueDef,
            (uint8_t *) &floatValueMin,
            (uint8_t *) &floatValueMax,
            sizeof(float),
            (uint8_t *) &adc15_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7010, 0x0F, "adc15");
    }

    /************************************************/
    /* 0x7010:0x10 adc16                           */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        floatValueMin = -3.40282346639e+38f;
        floatValueDef = 0.0;
        floatValueMax = 3.40282346639e+38f;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0x7010,
            0x10,
            GOAL_ECAT_DATATYPE_REAL32,
            EC_OBJATTR_RD | EC_OBJATTR_WR | EC_OBJATTR_RXPDOMAPPING | EC_OBJATTR_OPT | EC_OBJATTR_NUMERIC | EC_OBJATTR_NO_LIMITS,
            (uint8_t *) &floatValueDef,
            (uint8_t *) &floatValueMin,
            (uint8_t *) &floatValueMax,
            sizeof(float),
            (uint8_t *) &adc16_in);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0x7010, 0x10, "adc16");
    }

    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create object 0x7010");
    }

    /************************************************/
    /* 0xF000 - Obj0xF000                           */
    /************************************************/
    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdObjAdd(pHdlEcat, 0xF000, GOAL_ECAT_OBJCODE_RECORD, 0x45);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdObjNameAdd(pHdlEcat, 0xF000, "Obj0xF000");
    }

    /************************************************/
    /* 0xF000:0x00 numOfEntries                    */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint8ValueMin = 0x00;
        uint8ValueDef = 0x02;
        uint8ValueMax = 0xFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0xF000,
            0x00,
            GOAL_ECAT_DATATYPE_UNSIGNED8,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC,
            (uint8_t *) &uint8ValueDef,
            (uint8_t *) &uint8ValueMin,
            (uint8_t *) &uint8ValueMax,
            1,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0xF000, 0x00, "numOfEntries");
    }

    /************************************************/
    /* 0xF000:0x01 Index distance                  */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint16ValueMin = 0x0000;
        uint16ValueDef = 0x0010;
        uint16ValueMax = 0xFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0xF000,
            0x01,
            GOAL_ECAT_DATATYPE_UNSIGNED16,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC,
            (uint8_t *) &uint16ValueDef,
            (uint8_t *) &uint16ValueMin,
            (uint8_t *) &uint16ValueMax,
            2,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0xF000, 0x01, "Index distance");
    }

    /************************************************/
    /* 0xF000:0x02 Maximum number of modules       */
    /************************************************/
    if (GOAL_RES_OK(res)) {

        uint16ValueMin = 0x0000;
        uint16ValueDef = 0x0000;
        uint16ValueMax = 0xFFFF;

        res = goal_ecatdynOdSubIndexAdd(
            pHdlEcat,
            0xF000,
            0x02,
            GOAL_ECAT_DATATYPE_UNSIGNED16,
            EC_OBJATTR_RD | EC_OBJATTR_MAN | EC_OBJATTR_NUMERIC,
            (uint8_t *) &uint16ValueDef,
            (uint8_t *) &uint16ValueMin,
            (uint8_t *) &uint16ValueMax,
            2,
            NULL);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdSubIndexNameAdd(pHdlEcat, 0xF000, 0x02, "Maximum number of modules");
    }

    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create object 0xF000");
    }

    /* finish object dictionary creation */
    if (GOAL_RES_OK(res)) {
        res = goal_ecatdynOdFinish(pHdlEcat);
    }

    return res;
}
