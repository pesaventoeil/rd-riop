/** @file
 *
 * @brief EtherNet/IP + ACD + DHCP + DLR + LED + Button
 *
 * This application provides a simple demo for the EtherNet/IP stack. It
 * supports Address Conflict Detection, DHCP and DLR.
 * Furthermore, the application shows how to read the button state
 * (including counter) and how to control an LED.
 *
 * @copyright
 * Copyright 2023 port GmbH Halle/Saale.
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


#include "goal_includes.h"
#include "goal_eip.h"
#include "goal_app_eip.h"
#include <goal_dd.h>

/**-- ICC_EIP_GEN_INCLUDES_START --**/
/* user code block */
/**-- ICC_EIP_GEN_INCLUDES_END --**/


/****************************************************************************/
/* Local Defines */
/****************************************************************************/
#define DEVICE_VENDOR_ID             1114       /**< vendor id */
#define DEVICE_PRODUCT_CODE          10         /**< product code */
/**-- ICC_EIP_GEN_DEFINE_START --**/
/* user code block */
/**-- ICC_EIP_GEN_DEFINE_END --**/


/****************************************************************************/
/* Local Types */
/****************************************************************************/
/**-- ICC_EIP_GEN_TYPES_START --**/
/* user code block */
/**-- ICC_EIP_GEN_TYPES_END --**/


/****************************************************************************/
/* Local Prototypes */
/****************************************************************************/
GOAL_STATUS_T main_eipCallback(
    GOAL_EIP_T *pHdlEip,                        /**< EIP handle */
    GOAL_EIP_CB_ID_T id,                        /**< callback id */
    GOAL_EIP_CB_DATA_T *pCb                     /**< callback parameters */
);

static GOAL_STATUS_T main_eipIoConnectionEvent(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t outputAssembly,                    /**< output assembly connection point */
    uint32_t inputAssembly,                     /**< input assembly connection point */
    uint32_t connectionEvent                    /**< event ID */
);

static GOAL_STATUS_T main_eipAfterAssemblyDataReceived(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t instanceNr                         /**< instance number of the assembly object */
);

static GOAL_STATUS_T main_eipBeforeAssemblyDataSend(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t instanceNr                         /**< instance number of the assembly object */
);

static GOAL_STATUS_T main_eipRunIdleChanged(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t outputAssembly,                    /**< output assembly connection point */
    uint32_t inputAssembly,                     /**< input assembly connection point */
    uint32_t runIdleValue                       /**< current value of the run/idle flag */
);

static GOAL_STATUS_T main_eipAcdConflict(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint8_t dhcpEnabled,                        /**< DHCP enabled */
    uint8_t acdActivity,                        /**< ACD status */
    uint32_t acdRemoteMacHigh,                  /**< conflicted device address */
    uint16_t acdRemoteMacLow                    /**< conflicted device address */
);

static GOAL_STATUS_T main_eipResetDevice(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t resetState                         /**< type of reset */
);

/**-- ICC_EIP_GEN_LOCALPROTO_START --**/
/* user code block */
/**-- ICC_EIP_GEN_LOCALPROTO_END --**/


/****************************************************************************/
/* Local Variables */
/****************************************************************************/
/**-- ICC_EIP_GEN_LOCALVARS_START --**/
static uint8_t outputData[GOAL_APP_ASM_SIZE_OUTPUT]; /**< output assembly data */
static uint8_t inputData[GOAL_APP_ASM_SIZE_INPUT];  /**< input assembly data */
static uint8_t buttonCnt = 0;                   /**< button counter */
/**-- ICC_EIP_GEN_LOCALVARS_END --**/


/****************************************************************************/
/** Application
 *
 * This function Register the EtherNet/IP stack in GOAL.
 *
 * @retval GOAL_STATUS_T result
 */
GOAL_STATUS_T appl_init(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* initialize device detection module */
    res = goal_ddInit();
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Initialization of GOAL dd failed");
        return res;
    }

    /**-- ICC_EIP_GEN_APPLINIT_BEFORE_START --**/
    /* user code block */
    /**-- ICC_EIP_GEN_APPLINIT_BEFORE_END --**/

    res = goal_eipInit();
    if (GOAL_RES_ERR(res)) {
       goal_logErr("Initialization of Ethernet/IP failed");
    }

    /**-- ICC_EIP_GEN_APPLINIT_AFTER_START --**/
    /* user code block */
    /**-- ICC_EIP_GEN_APPLINIT_AFTER_END --**/

    return res;
}


/****************************************************************************/
/** Application Setup
 *
 * Setup the application ressources, configure and start the protocol stack.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T appl_setup(
    void
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_EIP_T *pHdlEip = NULL;                 /* GOAL Ethernet/ IP handle */
    GOAL_DD_T *pHdlDd;                          /* DD handle */

    /* start goal dd with all features enabled */
    res = goal_ddNew(&pHdlDd, GOAL_DD_FEAT_ALL);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("error creating goal dd instance");
        return res;
    }

    /**-- ICC_EIP_GEN_APPLSETUP_BEFORE_START --**/
    /* user code block */
    /**-- ICC_EIP_GEN_APPLSETUP_BEFORE_END --**/

    res = goal_eipCfgVendorIdSet(DEVICE_VENDOR_ID);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to set Vendor ID");
        return res;
    }

    res = goal_eipCfgProductCodeSet(DEVICE_PRODUCT_CODE);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to set Product Code");
        return res;
    }

    /* activate DHCP */
    res = goal_eipCfgDhcpOn(GOAL_TRUE);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to activate DHCP support");
        return res;
    }

    /* activate DLR */
    res = goal_eipCfgDlrOn(GOAL_TRUE);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to activate DLR support");
        return res;
    }

    /* activate Address Conflict Detection */
    res = goal_eipCfgAcdOn(GOAL_TRUE);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to activate ACD support");
        return res;
    }

    goal_logInfo("create new instance");

    res = goal_eipNew(&pHdlEip, GOAL_ID_DEFAULT, main_eipCallback);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create a new EtherNet/IP instance");
        return res;
    }

    /* create assembly object instances */
    /* Input Assembly */
    res = goal_eipCreateAssemblyObject(pHdlEip, GOAL_APP_ASM_ID_INPUT, GOAL_APP_ASM_SIZE_INPUT);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create Input Assembly");
        return res;
    }

    /* Output Assembly */
    res = goal_eipCreateAssemblyObject(pHdlEip, GOAL_APP_ASM_ID_OUTPUT, GOAL_APP_ASM_SIZE_OUTPUT);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create Output Assembly");
        return res;
    }

    /* Config Assembly */
    res = goal_eipCreateAssemblyObject(pHdlEip, GOAL_APP_ASM_ID_CONFIG, GOAL_APP_ASM_SIZE_CONFIG);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create Config Assembly");
        return res;
    }

    /* Heartbeat Output Assembly for Input Only connection */
    res = goal_eipCreateAssemblyObject(pHdlEip, GOAL_APP_ASM_ID_HEARTBEAT_IO, 0);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create IO Heartbeat Assembly");
        return res;
    }

    /* Heartbeat Output Assembly for Listen Only connection */
    res = goal_eipCreateAssemblyObject(pHdlEip, GOAL_APP_ASM_ID_HEARTBEAT_LO, 0);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create LO Heartbeat Assembly");
        return res;
    }

    /* define allowed connections */
    res = goal_eipAddExclusiveOwnerConnection(pHdlEip,
                                        GOAL_APP_ASM_ID_OUTPUT,
                                        GOAL_APP_ASM_ID_INPUT,
                                        GOAL_APP_ASM_ID_CONFIG);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add exclusive owner connection");
        return res;
    }

    res = goal_eipAddInputOnlyConnection(pHdlEip,
                                        GOAL_APP_IOCON_NUM,
                                        GOAL_APP_ASM_ID_HEARTBEAT_IO,
                                        GOAL_APP_ASM_ID_INPUT,
                                        GOAL_APP_ASM_ID_CONFIG);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add input only connection");
        return res;
    }


    res = goal_eipAddListenOnlyConnection(pHdlEip,
                                        GOAL_APP_LOCON_NUM,
                                        GOAL_APP_ASM_ID_HEARTBEAT_LO,
                                        GOAL_APP_ASM_ID_INPUT,
                                        GOAL_APP_ASM_ID_CONFIG);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add listen only connection");
        return res;
    }

    /**-- ICC_EIP_GEN_APPLSETUP_AFTER_START --**/
    /* user code block */
    /**-- ICC_EIP_GEN_APPLSETUP_AFTER_END --**/

    return GOAL_OK;
}


/****************************************************************************/
/** Inform the application on changes occurred for a connection
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T main_eipIoConnectionEvent(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t pa_unOutputAssembly,               /**< output assembly connection point */
    uint32_t pa_unInputAssembly,                /**< input assembly connection point */
    uint32_t pa_eIOConnectionEvent              /**< event ID */
)
{
    UNUSEDARG(pHdlEip);
    UNUSEDARG(pa_unOutputAssembly);
    UNUSEDARG(pa_unInputAssembly);

    /**-- ICC_EIP_GEN_IOCONNECTIONEVENT_BEFORE_START --**/
    /* user code block */
    /**-- ICC_EIP_GEN_IOCONNECTIONEVENT_BEFORE_END --**/

    /* react to  connection events
     * - connection opened
     * - connection closed
     * - connection timed out
     */
    switch (pa_eIOConnectionEvent) {
        case GOAL_EIP_CONNECTION_EVENT_OPENED:
            /**-- ICC_EIP_GEN_EVENT_OPENED_START --**/
            /* user code block */
            /**-- ICC_EIP_GEN_EVNET_OPENED_END --**/

            break;
        case GOAL_EIP_CONNECTION_EVENT_CLOSED:
            /**-- ICC_EIP_GEN_EVENT_CLOSED_START --**/
            /* user code block */
            /**-- ICC_EIP_GEN_EVENT_CLOSED_END --**/

            break;
        case GOAL_EIP_CONNECTION_EVENT_TIMEOUT:
            /**-- ICC_EIP_GEN_EVENT_TIMEOUT_START --**/
            /* user code block */
            /**-- ICC_EIP_GEN_EVENT_TIMEOUT_END --**/

            break;
        default:
            break;
    }

    /**-- ICC_EIP_GEN_IOCONNECTIONEVENT_AFTER_START --**/
    /* user code block */
    /**-- ICC_EIP_GEN_IOCONNECTIONEVENT_BEFORE_END --**/

     return GOAL_OK;
}


/****************************************************************************/
/** Call back function to inform application on received data for an assembly object
 *
 * This function has to be implemented by the user of the CIP-stack.
 * Assembly Objects for Configuration Data:
 * The CIP-stack uses this function to inform on received configuration data. The length of the data
 * is already checked within the stack. Therefore the user only has to check if the data is valid.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T main_eipAfterAssemblyDataReceived(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t instanceNr                         /**< instance number of the assembly object */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* return value */

    UNUSEDARG(pHdlEip);

    /**-- ICC_EIP_GEN_AFTER_ASSEMBLYDATA_REC_BEFORE_START --**/
    /* user code block */
    /**-- ICC_EIP_GEN_AFTER_ASSEMBLYDATA_REC_BEFORE_END --**/

    /* check wich Assembly Object has new data */
    switch (instanceNr) {
        case GOAL_APP_ASM_ID_INPUT:
            /**-- ICC_EIP_GEN_ASM_ID_INPUT_START --**/
            /* user code block */
            /**-- ICC_EIP_GEN_ASM_ID_INPUT_END --**/

            break;

        case GOAL_APP_ASM_ID_OUTPUT:
            /**-- ICC_EIP_GEN_ASM_ID_OUTPUT_START --**/
            res = goal_eipAssemblyObjectRead(pHdlEip, GOAL_APP_ASM_ID_OUTPUT, &outputData[0], GOAL_APP_ASM_SIZE_OUTPUT);
            /* Data for the output assembly has been received.
             * Mirror first byte to the LEDs */
            if (GOAL_RES_OK(res)) {
                goal_targetSetLeds(outputData[0]);
            }
            /**-- ICC_EIP_GEN_ASM_ID_OUTPUT_END --**/

            break;

        case GOAL_APP_ASM_ID_CONFIG:
            /**-- ICC_EIP_GEN_ASM_ID_CONFIG_START --**/
            /* user code block */
            /**-- ICC_EIP_GEN_ASM_ID_CONFIG_END --**/

            break;

        case GOAL_APP_ASM_ID_HEARTBEAT_IO:
            /**-- ICC_EIP_GEN_ASM_ID_HB_IO_START --**/
            /* user code block */
            /**-- ICC_EIP_GEN_ASM_ID_HB_IO_END --**/

            break;

        case GOAL_APP_ASM_ID_HEARTBEAT_LO:
            /**-- ICC_EIP_GEN_ASM_ID_HB_LO_START --**/
            /* user code block */
            /**-- ICC_EIP_GEN_ASM_ID_HB_LO_END --**/

            break;

        default:
            goal_logErr("Unknown instance ID %"FMT_u32, instanceNr);
            res = GOAL_ERR_EIP;
            /**-- ICC_EIP_GEN_ASM_ID_DEFAULT_START --**/
            /* user code block */
            /**-- ICC_EIP_GEN_ASM_ID_DEFAULT_END --**/

            return res;
    }

    /**-- ICC_EIP_GEN_AFTER_ASSEMBLYDATA_REC_AFTER_START --**/
    /* user code block */
    /**-- ICC_EIP_GEN_AFTER_ASSEMBLYDATA_REC_BEFORE_END --**/

    return res;
}


/****************************************************************************/
/** Inform the application that the data of an assembly object will be sent.
 *
 * Within this function the user can update the data of the assembly object
 * before it gets sent. The application can inform the stack if data has changed.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T main_eipBeforeAssemblyDataSend(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t instanceNr                         /**< instance number of the assembly object */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* return value */

    UNUSEDARG(pHdlEip);

    /**-- ICC_EIP_GEN_BEFORE_ASSEMBLYDATA_SEND_BEFORE_START --**/
    uint32_t buttons;                           /* pressed buttons */
    /**-- ICC_EIP_GEN_BEFORE_ASSEMBLYDATA_SEND_BEFORE_END --**/

    switch (instanceNr) {
        /* example Code
         *
         * case GOAL_APP_ASM_ID_INPUT:
         * get output data
         * res = goal_eipAssemblyObjectRead(pHdlEip, GOAL_APP_ASM_ID_OUTPUT, &outputData[0], GOAL_APP_ASM_SIZE_OUTPUT);

         * mirror output data to input data
         *  if (GOAL_RES_OK(res)) {
         *      GOAL_MEMCPY(&inputData[0], &outputData[0], GOAL_APP_ASM_SIZE_INPUT);
         *  }

         * store input data
         *  if (GOAL_RES_OK(res)) {
         *      res = goal_eipAssemblyObjectWrite(pHdlEip, GOAL_APP_ASM_ID_INPUT, &inputData[0], GOAL_APP_ASM_SIZE_INPUT);
         *  }
         *  break;
         */
        /**-- ICC_EIP_GEN_BEFORE_ASSEMBLYDATA_SEND_SWITCH_START --**/
        case GOAL_APP_ASM_ID_INPUT:
            /* get output data */
            res = goal_eipAssemblyObjectRead(pHdlEip, GOAL_APP_ASM_ID_OUTPUT, &outputData[0], GOAL_APP_ASM_SIZE_OUTPUT);

            /* mirror output data to input data */
            if (GOAL_RES_OK(res)) {
                buttons = goal_targetGetButtons();
                if (1 == buttons) {
                    buttonCnt++;
                }
                inputData[0] = (uint8_t) (buttons & 0xFF);
                inputData[1] = buttonCnt;

                /* mirroring the output to input */
                inputData[2] = outputData[1];
            }

            /* store input data */
            if (GOAL_RES_OK(res)) {
                res = goal_eipAssemblyObjectWrite(pHdlEip, GOAL_APP_ASM_ID_INPUT, &inputData[0], GOAL_APP_ASM_SIZE_INPUT);
            }
            break;
        /**-- ICC_EIP_GEN_BEFORE_ASSEMBLYDATA_SEND_SWITCH_END --**/

        default:
            goal_logErr("Unknown instance ID %"FMT_u32, instanceNr);
            res = GOAL_ERR_EIP;

            /**-- ICC_EIP_GEN_BEFORE_ASSEMBLYDATA_SEND_SWITCHDEF_START --**/
            /* user code block */
            /**-- ICC_EIP_GEN_BEFORE_ASSEMBLYDATA_SEND_SWITCHDEF_END --**/

            break;
    }

    /**-- ICC_EIP_GEN_BEFORE_ASSEMBLYDATA_SEND_AFTER_START --**/
    /* user code block */
    /**-- ICC_EIP_GEN_BEFORE_ASSEMBLYDATA_SEND_AFTER_END --**/

    return res;
}


/****************************************************************************/
/** Inform the application that the Run/Idle State has been changed by the originator.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T main_eipRunIdleChanged(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t outputAssembly,                    /**< output assembly connection point */
    uint32_t inputAssembly,                     /**< input assembly connection point */
    uint32_t runIdleValue                       /**< current value of the run/idle flag */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* return value */

    UNUSEDARG(pHdlEip);
    UNUSEDARG(runIdleValue);
    UNUSEDARG(outputAssembly);
    UNUSEDARG(inputAssembly);

    /* example code
     *
     * if ((GOAL_APP_ASM_ID_OUTPUT == outputAssembly) &&
     *  (GOAL_APP_ASM_ID_INPUT == inputAssembly)) {
     *  goal_logInfo("runIdleHeader changed for exclusive owner connection: %"FMT_u32,
     *               runIdleValue);
     * }
     */
    /**-- ICC_EIP_GEN_RUN_IDLE_CHANGED_START --**/
    /* user code block */
    /**-- ICC_EIP_GEN_RUN_IDLE_CHANGED_END --**/

    return res;
}


/****************************************************************************/
/** Inform Application that device will be reset
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T main_eipResetDevice(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t resetState                         /**< type of reset */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* return value */

    UNUSEDARG(pHdlEip);
    UNUSEDARG(resetState);

    goal_logInfo("device is reset: %s", (GOAL_EIP_RESET_POWER == resetState) ? "Power Cycle"
                                       : (GOAL_EIP_RESET_FACTORY == resetState) ? "Factory Reset"
                                       : (GOAL_EIP_RESET_FACTORY_COMM == resetState) ? "Factory Reset (no Com)"
                                       : "unknown");
    /**-- ICC_EIP_GEN_RESET_DEVICE_START --**/
    /* user code block */
    /**-- ICC_EIP_GEN_RESET_DEVICE_END --**/

    return res;
}


/****************************************************************************/
/** EtherNet/IP Callback Handler
 *
 * This function collects all callbacks from the stack and decides if the
 * callback must be handled.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T main_eipCallback(
    GOAL_EIP_T *pHdlEip,                        /**< PROFINET handle */
    GOAL_EIP_CB_ID_T id,                        /**< callback id */
    GOAL_EIP_CB_DATA_T *pCb                     /**< callback parameters */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* return value */

    /**-- ICC_EIP_GEN_EIPCALLBACK_BEFORE_START --**/
    /* user code block */
    /**-- ICC_EIP_GEN_EIPCALLBACK_BEFORE_END --**/

    /* handle callback IDs */
    switch (id) {
        case GOAL_EIP_CB_ID_READY:
            /* inform the application about object creation done */
            break;

        case GOAL_EIP_CB_ID_CONNECT_EVENT:
            /* inform the application on changes occurred for a connection */
            res = main_eipIoConnectionEvent(
                pHdlEip,
                pCb->data[0].outputAssembly,
                pCb->data[1].inputAssembly,
                pCb->data[2].connectionEvent);
            break;

        case GOAL_EIP_CB_ID_RUN_IDLE_CHANGED:
            /* inform the application on Run/Idle State changed by the originator */
            res = main_eipRunIdleChanged(
                pHdlEip,
                pCb->data[0].outputAssembly,
                pCb->data[1].inputAssembly,
                pCb->data[2].runIdleValue);
            break;

        case GOAL_EIP_CB_ID_ASSEMBLY_DATA_SEND:
            /* Inform the application about assembly object being sent next */
            res = main_eipBeforeAssemblyDataSend(
                pHdlEip,
                pCb->data[0].instanceNr);
            break;

        case GOAL_EIP_CB_ID_ASSEMBLY_DATA_RECV:
            /* inform application on received data for an assembly object */
            res = main_eipAfterAssemblyDataReceived(
                pHdlEip,
                pCb->data[0].instanceNr);
            break;

        case GOAL_EIP_CB_ID_DEVICE_RESET:
            /* inform Application that device will be reset */
            res = main_eipResetDevice(
                pHdlEip,
                pCb->data[0].resetState);
            break;

        case GOAL_EIP_CB_ID_ACD_CONFLICT:
            /* Inform Application about a detected address conflict.
             * If a conflict behavior is configured, the device will
             * try to solve the conflict. Otherwise the device stays
             * in a mayor unrecoverable fault. If the return-value is
             * GOAL_ERROR, the device will also transit into a mayor
             * unrecoverable fault.
             */
            res = main_eipAcdConflict(
                pHdlEip,
                pCb->data[0].dhcpEnabled,
                pCb->data[1].acdActivity,
                pCb->data[2].acdRemoteMacHigh,
                pCb->data[3].acdRemoteMacLow);
            break;

        default:
            /**-- ICC_EIP_GEN_EIPCALLBACK_DEFAULT_START --**/
            /* user code block */
            /**-- ICC_EIP_GEN_EIPCALLBACK_DEFAULT_END --**/

            break;
    }

    /**-- ICC_EIP_GEN_EIPCALLBACK_AFTER_START --**/
    /* user code block */
    /**-- ICC_EIP_GEN_EIPCALLBACK_AFTER_END --**/

    return res;
}


/****************************************************************************/
/** Inform Application about detected address conflict
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T main_eipAcdConflict(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint8_t dhcpEnabled,                        /**< DHCP enabled */
    uint8_t acdActivity,                        /**< ACD status */
    uint32_t acdRemoteMacHigh,                  /**< conflicted device address */
    uint16_t acdRemoteMacLow                    /**< conflicted device address */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_ACD_LAST_CONFLICT_DETECTED_T attribute; /* ACD attribute */
    uint8_t acdRemoteMac[MAC_ADDR_LEN];         /* remote MAC */

    UNUSEDARG(dhcpEnabled);
    UNUSEDARG(acdActivity);

    /**-- ICC_EIP_GEN_ACDCONFLICT_BEFORE_START --**/
    /* user code block */
    /**-- ICC_EIP_GEN_ACDCONFLICT_BEFORE_END --**/

    /* copy received MAC into array */
    GOAL_MEMCPY(&acdRemoteMac, &acdRemoteMacHigh, sizeof(acdRemoteMacHigh));
    GOAL_MEMCPY(&acdRemoteMac[4], &acdRemoteMacLow, sizeof(acdRemoteMacLow));

    goal_logInfo("address conflict caused by device %02x:%02x:%02x:%02x:%02x:%02x",
            acdRemoteMac[0],
            acdRemoteMac[1],
            acdRemoteMac[2],
            acdRemoteMac[3],
            acdRemoteMac[4],
            acdRemoteMac[5]);

    /* get whole ACD attribute */
    res = goal_eipAcdLastConflictDetectedGet(pHdlEip, &attribute);
    if (GOAL_RES_OK(res)) {
        /* log information of attribute */
        goal_logInfo("stop using IP address %d.%d.%d.%d, get new IP after timeout",
                attribute.arpPru.targetProtodAdress[0],
                attribute.arpPru.targetProtodAdress[1],
                attribute.arpPru.targetProtodAdress[2],
                attribute.arpPru.targetProtodAdress[3]);
    }

    /* Transit into major unrecoverable fault */
    res = GOAL_ERROR;

    /**-- ICC_EIP_GEN_ACDCONFLICT_AFTER_START --**/
    /* user code block */
    /**-- ICC_EIP_GEN_ACDCONFLICT_AFTER_END --**/

    return res;

}


/****************************************************************************/
/** Application Loop
 *
 * This function must implement the application logic and must not block. It is
 * called in a loop (the GOAL loop) and if state tracking is necessary it
 * should use static or global variables.
 */
void appl_loop(
    void
)
{
    /**-- ICC_EIP_GEN_APPLLOOP_START --**/
    /* user code block */
    /**-- ICC_EIP_GEN_APPLLOOP_END --**/

}
