/** @file
 *
 * @brief EtherCAT Sample application
 *
 * This application demonstrates the usage of the application callback function.
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
#include "goal_ecat.h"
#include "goal_appl_ecat.h"
#include "goal_appl_ecat_objects.h"

#if RIOP
#include "gl.h"

#define MEMCPY memcpy
#define SIZEOF sizeof
#endif

/****************************************************************************/
/* Local defines */
/****************************************************************************/
#define APPL_ECAT_MIN_CYCLE_TIME 200000         /**< minimum cycle time in ns */
#define APPL_ECAT_EXPL_DEV_ID 5                 /**< demo value for Explicit Device ID */


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/
static GOAL_STATUS_T appl_ecatSdoDownload(
    GOAL_ECAT_T *pHdlEcat,                      /**< GOAL EtherCAT handle */
    uint16_t index,                             /**< index of object */
    uint8_t subIndex,                           /**< sub-index of object */
    uint8_t *pData,                             /**< pointer to new data */
    uint32_t size                               /**< data size */
);

static GOAL_STATUS_T appl_ecatSdoUpload(
    GOAL_ECAT_T *pHdlEcat,                      /**< GOAL EtherCAT handle */
    uint16_t index,                             /**< index of object */
    uint8_t subIndex,                           /**< sub-index of object */
    uint8_t **ppData,                           /**< [out] pointer reference for object value */
    uint32_t *pSize                             /**< [out] object's value size */
);

static void appl_ecatPdoReceived(
    GOAL_ECAT_T *pHdlEcat                       /**< GOAL EtherCAT handle */
);

static void appl_ecatPdoTxPrepare(
    GOAL_ECAT_T *pHdlEcat                       /**< GOAL EtherCAT handle */
);

static GOAL_STATUS_T appl_ecatDcSettingsCheck(
    GOAL_ECAT_T *pHdlEcat,                      /**< GOAL EtherCAT handle */
    uint32_t dcCycleTime,                       /**< current DC cycle time in ns */
    uint32_t *pMinCycleTime                     /**< minimum cylce time supported by application in ns */
);

static void appl_ecatExplDevIdGet(
    GOAL_ECAT_T *pHdlEcat,                      /**< GOAL EtherCAT handle */
    uint16_t *pDevId                            /**< [out] explicit device ID */
);

static void appl_ecatEsmTransitionDone(
    GOAL_ECAT_T *pHdlEcat,                      /**< GOAL EtherCAT handle */
    uint16_t newState,                          /**< new ESM state entered */
    GOAL_BOOL_T errFlag,                        /**< error flag */
    uint16_t statusCode                         /**< AL status code */
);

static GOAL_STATUS_T appl_ecatEsmTransitionRequest(
    GOAL_ECAT_T *pHdlEcat,                      /**< GOAL EtherCAT handle */
    uint16_t esmState,                          /**< requested ESM state */
    uint16_t statusCode,                        /**< current AL status code */
    uint16_t *pApplStatusCode                   /**< application specific status code */
);


/****************************************************************************/
/** Application initialization
 *
 * This function is used to initialize application specific resources.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T appl_ecatApplInit(
    GOAL_ECAT_T *pHdlEcat                       /**< GOAL EtherCAT handle */
)
{
    UNUSEDARG(pHdlEcat);

    return GOAL_OK;
}


/****************************************************************************/
/** EtherCAT Callback Handler
 *
 * This function collects all callbacks from the stack and decides if the
 * callback must be handled.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T appl_ecatCallback(
    GOAL_ECAT_T *pHdlEcat,                      /**< GOAL EtherCAT handle */
    GOAL_ECAT_CB_ID_T id,                       /**< callback id */
    GOAL_ECAT_CB_DATA_T *pCb                    /**< callback parameters */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    switch (id) {
        case GOAL_ECAT_CB_ID_SDO_DOWNLOAD:
            res = appl_ecatSdoDownload(pHdlEcat, pCb->data[0].index, pCb->data[1].subIndex, pCb->data[2].pData, pCb->data[3].size);
            break;

        case GOAL_ECAT_CB_ID_SDO_UPLOAD:
            res = appl_ecatSdoUpload(pHdlEcat, pCb->data[0].index, pCb->data[1].subIndex, &pCb->data[2].pData, &pCb->data[3].size);
            break;

        case GOAL_ECAT_CB_ID_RxPDO_RECEIVED:
            appl_ecatPdoReceived(pHdlEcat);
            break;

        case GOAL_ECAT_CB_ID_TxPDO_PREPARE:
            appl_ecatPdoTxPrepare(pHdlEcat);
            break;

        case GOAL_ECAT_CB_ID_SYNC_FAIL:
            goal_logErr("EtherCAT slave lost synchronization");
            break;

        case GOAL_ECAT_CB_ID_NEW_DL_STATE:
            goal_logInfo("EtherCAT Data Link layer changed: 0x%04x", pCb->data[0].dlState);
            break;

        case GOAL_ECAT_CB_ID_NEW_DC_CONFIG:
            res = appl_ecatDcSettingsCheck(pHdlEcat, pCb->data[0].dcCycleTime, &pCb->data[1].dcCycleTimeMin);
            break;

        case GOAL_ECAT_CB_ID_DC_FAIL:
            goal_logErr("too many DC events were missed");
            break;

        case GOAL_ECAT_CB_ID_SM_WATCHDOG_EXPIRED:
            goal_logErr("Process Data Watchdog expired");
            break;

        case GOAL_ECAT_CB_ID_EXPLICIT_DEV_ID:
            appl_ecatExplDevIdGet(pHdlEcat, &pCb->data[0].explDevId);
            break;

        case GOAL_ECAT_CB_ID_NEW_ESM_STATE_ENTERED:
            appl_ecatEsmTransitionDone(pHdlEcat, pCb->data[0].esmState, pCb->data[1].esmError, pCb->data[2].statusCode);
            break;

        case GOAL_ECAT_CB_ID_NEW_ESM_STATE_REQUESTED:
            appl_ecatEsmTransitionRequest(pHdlEcat, pCb->data[0].esmState, pCb->data[1].statusCode, &pCb->data[2].appStatusCode);
            break;
            
        case GOAL_ECAT_CB_ID_FOE_READ_REQ:
        	GOAL_TARGET_FALLTHROUGH;
        case GOAL_ECAT_CB_ID_FOE_READ_DATA:
        	GOAL_TARGET_FALLTHROUGH;
        case GOAL_ECAT_CB_ID_FOE_WRITE_REQ:
        	GOAL_TARGET_FALLTHROUGH;
        case GOAL_ECAT_CB_ID_FOE_ERROR:
        	GOAL_TARGET_FALLTHROUGH;
        case GOAL_ECAT_CB_ID_FOE_WRITE_DATA:
        	GOAL_TARGET_FALLTHROUGH;
        case GOAL_ECAT_CB_ID_RUN_LED_STATE:
        	GOAL_TARGET_FALLTHROUGH;
        case GOAL_ECAT_CB_ID_ERROR_LED_STATE:
            break;

        default:
            goal_logErr("unknown callback Id: %d", (int) id);
            res = GOAL_ERROR;
            break;
    }

    return res;
}


/****************************************************************************/
/** Handle a SDO Download Request
 *
 * This function is called if the EtherCAT master wants to write a new value to
 * an object. The new value can be read from @em pData.
 * The application can reject the write access by returning an error code.
 *
 * @retval GOAL_OK success, allow write access
 * @retval GOAL_ERR_SDO_HARDWARE hardware error
 * @retval GOAL_ERR_SDO_TOO_LOW value to low
 * @retval GOAL_ERR_SDO_TOO_HIGH value to high
 * @retval GOAL_ERR_SDO_APPL application specific reason
 * @retval GOAL_ERR_SDO_DEV_STATE current device state insufficient
 * @retval GOAL_ERR_SDO_GENERAL unspecified SDO error
 */
static GOAL_STATUS_T appl_ecatSdoDownload(
    GOAL_ECAT_T *pHdlEcat,                      /**< GOAL EtherCAT handle */
    uint16_t index,                             /**< index of object */
    uint8_t subIndex,                           /**< sub-index of object */
    uint8_t *pData,                             /**< pointer to new data */
    uint32_t size                               /**< data size */
)
{
    UNUSEDARG(pHdlEcat);
    UNUSEDARG(index);
    UNUSEDARG(subIndex);
    UNUSEDARG(pData);
    UNUSEDARG(size);

    goal_logInfo("index: 0x%04x, sub: %d", index, subIndex);

    return GOAL_OK;
}


/****************************************************************************/
/** Handle a SDO Upload Request
 *
 * This function is called if the EtherCAT master wants to read a value from
 * an object. The application can update a value before it is sent out. The
 * pointer reference must be set to the new value and its size must be written
 * to @em pSize.
 * The application can reject the read access by returning an error code.
 *
 * @retval GOAL_OK success, allow write access
 * @retval GOAL_ERR_SDO_HARDWARE hardware error
 * @retval GOAL_ERR_SDO_APPL application specific reason
 * @retval GOAL_ERR_SDO_DEV_STATE current device state insufficient
 * @retval GOAL_ERR_SDO_GENERAL unspecified SDO error
 */
static GOAL_STATUS_T appl_ecatSdoUpload(
    GOAL_ECAT_T *pHdlEcat,                      /**< GOAL EtherCAT handle */
    uint16_t index,                             /**< index of object */
    uint8_t subIndex,                           /**< sub-index of object */
    uint8_t **ppData,                           /**< [out] pointer reference for object value */
    uint32_t *pSize                             /**< [out] object's value size */
)
{
    UNUSEDARG(pHdlEcat);
    UNUSEDARG(index);
    UNUSEDARG(subIndex);
    UNUSEDARG(ppData);
    UNUSEDARG(pSize);

    goal_logInfo("index: 0x%04x, sub: %d", index, subIndex);

    return GOAL_OK;
}


/****************************************************************************/
/** New Process Data has been received (Outputs)
 *
 * All objects that are mapped to RxPDOs have been updated.
 */
static void appl_ecatPdoReceived(
    GOAL_ECAT_T *pHdlEcat                       /**< GOAL EtherCAT handle */
)
{
    UNUSEDARG(pHdlEcat);

#if RIOP

    /* Copy data from master to slave*/
        uint16_t * pi;
        uint8_t * pii;
        pi=&riop_command.afe_command.afe_gpio_command.gpio_output_value;
#define ATOMIC_OP(var, XXX) if(var) *pi|=(uint16_t)XXX; else *pi&=~(uint16_t)XXX
        ATOMIC_OP(di1_in, 0x0004);
        ATOMIC_OP(di2_in, 0x0008);
        ATOMIC_OP(di3_in, 0x0010);
        ATOMIC_OP(di4_in, 0x0020);

        ATOMIC_OP(di5_in, 0x0040);
        ATOMIC_OP(di6_in, 0x0080);
        ATOMIC_OP(di7_in, 0x0100);
        ATOMIC_OP(di8_in, 0x0200);
#undef ATOMIC_OP

#define ATOMIC_OP(var, XXX) if(var) *pii|=(uint8_t)XXX; else *pii&=~(uint8_t)XXX
        pii=&riop_command.mcu_digital_output_command.mcu_digital_output;
        ATOMIC_OP(di17_in, 0x01);
        ATOMIC_OP(di18_in, 0x02);
        ATOMIC_OP(di19_in, 0x04);
        ATOMIC_OP(di20_in, 0x08);

        ATOMIC_OP(di21_in, 0x10);
        ATOMIC_OP(di22_in, 0x20);
        ATOMIC_OP(di23_in, 0x40);
        ATOMIC_OP(di24_in, 0x80);
#undef ATOMIC_OP

#define ATOMIC_OP(var, XXX) if(var) riop_command.afe_command.afe_gpio_command.gpio_pin_direction |=XXX; \
                else riop_command.afe_command.afe_gpio_command.gpio_pin_direction &=~(uint16_t)XXX;

        ATOMIC_OP(di25_in, 0x0004);
        ATOMIC_OP(di26_in, 0x0008);
        ATOMIC_OP(di27_in, 0x0010);
        ATOMIC_OP(di28_in, 0x0020);
        ATOMIC_OP(di29_in, 0x0040);
        ATOMIC_OP(di30_in, 0x0080);
        ATOMIC_OP(di31_in, 0x0100);
        ATOMIC_OP(di32_in, 0x0200);
#undef ATOMIC_OP

        //Mostly unused analog outputs.
        MEMCPY(&LED_status,&led_in,SIZEOF(LED_status));
        {
                uint32_t tt;
                MEMCPY( &fsm_mode, &adc1_in, SIZEOF(tt));
                MEMCPY( &tt, &adc2_in, SIZEOF(tt));
        }
        led_green = LED_status;

        {
                int32_t tt;
                MEMCPY( &tt, &adc3_in, SIZEOF(tt));
                MEMCPY( &tt, &adc4_in, SIZEOF(tt));
        }
        {
                float tt;
                tt=0.0;
                MEMCPY( &tt, &adc5_in, SIZEOF(tt));
                MEMCPY( &tt, &adc6_in, SIZEOF(tt));
                MEMCPY( &tt, &adc7_in, SIZEOF(tt));
                MEMCPY( &tt, &adc8_in, SIZEOF(tt));
                MEMCPY( &tt, &adc9_in, SIZEOF(tt));
                MEMCPY( &tt, &adc10_in, SIZEOF(tt));

                MEMCPY( &tt, &adc11_in, SIZEOF(tt));
                MEMCPY( &tt, &adc12_in, SIZEOF(tt));
                MEMCPY( &tt, &adc13_in, SIZEOF(tt));
                MEMCPY( &tt, &adc14_in, SIZEOF(tt));
                MEMCPY( &tt, &adc15_in, SIZEOF(tt));
                MEMCPY( &tt, &adc16_in, SIZEOF(tt));
        }

#endif
}


/****************************************************************************/
/** Prepare Process Data that will be sent (Inputs)
 *
 * This function is called before the Inputs are sent to the master. The
 * application has the chance to update the objects that will be mapped to the
 * TxPDOs.
 */
static void appl_ecatPdoTxPrepare(
    GOAL_ECAT_T *pHdlEcat                       /**< GOAL EtherCAT handle */
)
{
    UNUSEDARG(pHdlEcat);

#if RIOP

        uint8_t di;
        di=0;
        uint16_t * pi;
        uint8_t * pii;
        pi=&g_boardStatus.afe_status.afe_gpio_input_pins_status;

#define ATOMIC_OP(var, XXX) if(*pi&(uint16_t)XXX) var=1; else var=0
        ATOMIC_OP(di1_out, 0x0004);
        ATOMIC_OP(di2_out, 0x0008);
        ATOMIC_OP(di3_out, 0x0010);
        ATOMIC_OP(di4_out, 0x0020);

        ATOMIC_OP(di5_out, 0x0040);
        ATOMIC_OP(di6_out, 0x0080);
        ATOMIC_OP(di7_out, 0x0100);
        ATOMIC_OP(di8_out, 0x0200);
#undef ATOMIC_OP

#define ATOMIC_OP(var, XXX) if(*pii&(uint8_t)XXX) var=1; else var=0
        pii=&g_boardStatus.mcu_digital_input_pins_status;
        ATOMIC_OP(di9_out, 0x01);
        ATOMIC_OP(di10_out, 0x02);
        ATOMIC_OP(di11_out, 0x04);
        ATOMIC_OP(di12_out, 0x08);

        ATOMIC_OP(di13_out, 0x10);
        ATOMIC_OP(di14_out, 0x20);
        ATOMIC_OP(di15_out, 0x40);
        MEMCPY(&di16_out, &di, SIZEOF(di));

        pii=&g_boardStatus.mcu_digital_output_pins_status;
        ATOMIC_OP(di17_out, 0x01);
        ATOMIC_OP(di18_out, 0x02);
        ATOMIC_OP(di19_out, 0x04);
        ATOMIC_OP(di20_out, 0x08);

        ATOMIC_OP(di21_out, 0x10);
        ATOMIC_OP(di22_out, 0x20);
        ATOMIC_OP(di23_out, 0x40);
        ATOMIC_OP(di24_out, 0x80);

#undef ATOMIC_OP
	//NA di25-32

	//LED
        MEMCPY(&led_out,&LED_status,SIZEOF(LED_status));
        {
                uint32_t tt;
                tt=(adc_1);
                MEMCPY(&adc1_out, &tt, SIZEOF(tt));
                MEMCPY(&adc2_out, &adc_1, SIZEOF(adc_1));
        }
        {
                int32_t tt;
                tt=(adc_1);
                tt=-tt;
                MEMCPY(&adc3_out, &tt, SIZEOF(tt));
                MEMCPY(&adc4_out, &tt, SIZEOF(tt));
        }
        {
                float * pf;
                float tt;
                tt=(adc_1);
                pf=&(g_boardStatus.afe_status.hvsig_status.channel_value[0]);
                tt=*pf++;
                MEMCPY(&adc5_out, &tt, SIZEOF(tt));
                tt=*pf++;
                MEMCPY(&adc6_out, &tt, SIZEOF(tt));
                tt=*pf++;
                MEMCPY(&adc7_out, &tt, SIZEOF(tt));
                pf=&(g_boardStatus.afe_status.current_status.channel_value);
                tt=*pf++;
                MEMCPY(&adc8_out, &tt, SIZEOF(tt));
                pf=&(g_boardStatus.afe_status.lvsig_status.channel_mean[0]);
                tt=*pf++;
                MEMCPY(&adc9_out, &tt, SIZEOF(tt));
                tt=*pf++;
                MEMCPY(&adc10_out, &tt, SIZEOF(tt));
                tt=*pf++;

                MEMCPY(&adc11_out, &tt, SIZEOF(tt));
                tt=*pf++;
                MEMCPY(&adc12_out, &tt, SIZEOF(tt));
                tt=*pf++;
                MEMCPY(&adc13_out, &tt, SIZEOF(tt)); //#13

                pf=&(g_boardStatus.afe_status.temperature_status.channel_value);
                tt=*pf++;
                MEMCPY(&adc14_out, &tt, SIZEOF(tt)); //#14
                MEMCPY(&adc15_out, &tt, SIZEOF(tt)); //#15
                MEMCPY(&adc16_out, &tt, SIZEOF(tt)); //#16
        }
#endif
}


/****************************************************************************/
/** New DC configuration received
 *
 * This function is called if the master has written a new DC configuration.
 * The application must provide its smallest supported cycle time.
 * If the current cycle time exceeds the limit the settings can be rejected.
 *
 * @retval GOAL_OK accept settings
 * @retval GOAL_ERROR reject settings
 */
static GOAL_STATUS_T appl_ecatDcSettingsCheck(
    GOAL_ECAT_T *pHdlEcat,                      /**< GOAL EtherCAT handle */
    uint32_t dcCycleTime,                       /**< current DC cycle time in ns */
    uint32_t *pMinCycleTime                     /**< [out] minimum cycle time supported by application in ns */
)
{
    UNUSEDARG(pHdlEcat);

    *pMinCycleTime = APPL_ECAT_MIN_CYCLE_TIME;

    return (APPL_ECAT_MIN_CYCLE_TIME <= dcCycleTime) ? GOAL_OK : GOAL_ERROR;
}


/****************************************************************************/
/** Get Explicit Device ID
 *
 * This function must provide the Explicit Device ID. It is used by the master
 * to explicitly identify a slave. The device ID must be configurable by
 * non-EtherCAT means (e.g. hardware switches, terminal, display, etc.).
 * The Device ID must not equal 0.
 */
static void appl_ecatExplDevIdGet(
    GOAL_ECAT_T *pHdlEcat,                      /**< GOAL EtherCAT handle */
    uint16_t *pDevId                            /**< [out] explicit device ID */
)
{
    UNUSEDARG(pHdlEcat);

    /* for this demo a fixed value is used, this value is expected by the CTT */
    *pDevId = APPL_ECAT_EXPL_DEV_ID;
}


/****************************************************************************/
/** New ESM state entered
 *
 * This function is called if the EtherCAT State Machine has entered a new
 * state. If an error occurred @em errFlag is set and @em statusCode indicates
 * the reason.
 */
static void appl_ecatEsmTransitionDone(
    GOAL_ECAT_T *pHdlEcat,                      /**< GOAL EtherCAT handle */
    uint16_t newState,                          /**< new ESM state entered */
    GOAL_BOOL_T errFlag,                        /**< error flag */
    uint16_t statusCode                         /**< AL status code */
)
{
    UNUSEDARG(pHdlEcat);
    UNUSEDARG(newState);
    UNUSEDARG(errFlag);
    UNUSEDARG(statusCode);

    if (GOAL_TRUE == errFlag) {
        goal_logErr("error in ESM state: %s, AL status code: 0x%04x",
                     (GOAL_ECAT_ESM_STATE_INIT == newState) ? "INIT"
                     : (GOAL_ECAT_ESM_STATE_PREOP == newState) ? "PreOp"
                     : (GOAL_ECAT_ESM_STATE_BOOTSTRAP == newState) ? "BOOTSTRAP"
                     : (GOAL_ECAT_ESM_STATE_SAFEOP == newState) ? "SafeOp"
                     : (GOAL_ECAT_ESM_STATE_OP == newState) ? "OP"
                     : "unknown", statusCode);
    }
    else {
        goal_logInfo("new ESM state: %s",
                     (GOAL_ECAT_ESM_STATE_INIT == newState) ? "INIT"
                     : (GOAL_ECAT_ESM_STATE_PREOP == newState) ? "PreOp"
                     : (GOAL_ECAT_ESM_STATE_BOOTSTRAP == newState) ? "BOOTSTRAP"
                     : (GOAL_ECAT_ESM_STATE_SAFEOP == newState) ? "SafeOp"
                     : (GOAL_ECAT_ESM_STATE_OP == newState) ? "OP"
                     : "unknown");
    }
}


/****************************************************************************/
/** New ESM state transition requested
 *
 * This function is called if the master requested a transition of the EtherCAT
 * State Machine. The new state is in @em esmState If an error occurred the
 * status code is shown in @em statusCode.
 *
 * The application can reject the transition via return value. In that case @em
 * pApplStatusCode must contain the appropriate AL status code. One of the
 * GOAL_ECAT_STATE_CODE_ macros shall be used.
 *
 * @retval GOAL_OK accept transition
 * @retval GOAL_ERROR reject transition
 */
static GOAL_STATUS_T appl_ecatEsmTransitionRequest(
    GOAL_ECAT_T *pHdlEcat,                      /**< GOAL EtherCAT handle */
    uint16_t esmState,                          /**< requested ESM state */
    uint16_t statusCode,                        /**< current AL status code */
    uint16_t *pApplStatusCode                   /**< application specific status code */
)
{
    UNUSEDARG(pHdlEcat);
    UNUSEDARG(esmState);
    UNUSEDARG(statusCode);
    UNUSEDARG(pApplStatusCode);

    goal_logInfo("requested state: %s",
                 (GOAL_ECAT_ESM_STATE_INIT == esmState) ? "INIT"
                 : (GOAL_ECAT_ESM_STATE_PREOP == esmState) ? "PreOp"
                 : (GOAL_ECAT_ESM_STATE_BOOTSTRAP == esmState) ? "BOOTSTRAP"
                 : (GOAL_ECAT_ESM_STATE_SAFEOP == esmState) ? "SafeOp"
                 : (GOAL_ECAT_ESM_STATE_OP == esmState) ? "OP"
                 : "unknown");

    return GOAL_OK;
}
