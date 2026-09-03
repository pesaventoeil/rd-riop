/** @file
 *
 * @brief goal LM usage example
 *
 * This application demonstrates the integration of LM and example log messages
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

#include <goal_includes.h>
#include <goal_dd.h>
#include "goal_appl.h"


/****************************************************************************/
/* Local defines */
/****************************************************************************/
#define APPL_DELAY_MS (100 * GOAL_TIMER_MSEC)
#define APPL_STR_LEN 32


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/

static GOAL_STATUS_T ddCallback(
    GOAL_DD_T *pHdlDd,                          /**< dd handle */
    GOAL_DD_CB_ID_T cbId,                       /**< callback ID */
    GOAL_DD_CB_DATA_T *pCbData                  /**< callback data */
);

static GOAL_STATUS_T appl_logMessages(
    void
);

void appl_logMessage(
    int id                                      /**< id for log source */
);

static GOAL_TARGET_NORETURN GOAL_STATUS_T goal_applTask(
    void *pArg                                  /**< task argument */
);


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_TIMESTAMP_T tsNext;                 /**< next iteration time */
static GOAL_TASK_T *pTask;                      /**< additional task */


/****************************************************************************/
/** Application Init
 *
 * This function must initialize GOAL and all used protocol stacks.
 * Furthermore application specific resources must be initialized.
 */
GOAL_STATUS_T appl_init(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* initialize dd which supports logging to Management Tool */
    res = goal_ddInit();
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Initialization of GOAL dd failed");
        return res;
    }

    return res;
}


/****************************************************************************/
/** Application Setup
 *
 * This function must setup all used protocol stacks.
 *
 */
GOAL_STATUS_T appl_setup(
    void
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_DD_T *pHdlDd;                          /* DD handle */

    /* start goal dd with all features enabled */
    res = goal_ddNew(&pHdlDd, GOAL_DD_FEAT_ALL);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("error creating goal dd instance");
    }

    res = goal_ddCustomerIdSet(pHdlDd, 0x00000001);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("error updating 32-bit variable");
    }

    res = goal_ddModuleNameSet(pHdlDd, (uint8_t *)"device");
    if (GOAL_RES_ERR(res)) {
        goal_logErr("error updating 32-bit variable");
    }

    goal_ddCallbackReg(pHdlDd, (GOAL_DD_FUNC_CB_T) ddCallback);

    appl_logMessages();

    return res;
}


/****************************************************************************/
/** goal dd callback
 *
 */
 static GOAL_STATUS_T ddCallback(
    GOAL_DD_T *pHdlDd,                          /**< dd handle */
    GOAL_DD_CB_ID_T cbId,                       /**< callback ID */
    GOAL_DD_CB_DATA_T *pCbData                  /**< callback data */
)
{
    UNUSEDARG(pHdlDd);
    UNUSEDARG(pCbData);

    switch (cbId) {
        case GOAL_DD_CB_ID_WINK:
            goal_logInfo("Blink command received");
            break;
        default:
            break;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** example logging function
 *
 * This function generates log messages with all supported types.
 *
 *
 */
static GOAL_STATUS_T appl_logMessages(
    void
)
{
    GOAL_STATUS_T res;                          /* result */
    int8_t valInt8;                             /* logging parameter value */
    uint8_t valUint8;                           /* logging parameter value */
    int16_t valInt16;                           /* logging parameter value */
    uint16_t valUint16;                         /* logging parameter value */
    int32_t valInt32;                           /* logging parameter value */
    uint32_t valUint32;                         /* logging parameter value */
    int64_t valInt64;                           /* logging parameter value */
    uint64_t valUint64;                         /* logging parameter value */
    float valFloat;                             /* logging parameter value */
    void *pValPtr;                              /* logging parameter value */
    uint64_t valPtrDiff;                        /* logging parameter value */
    uint32_t valIpV4;                           /* logging parameter value */
    uint8_t valMac[6] = { 0x02, 0x01, 0x00, 0x00, 0x5c, 0x01}; /* logging parameter value */
    GOAL_BOOL_T valBoolean;                     /* logging parameter value */
    const char strValGeneric[]="Generic";       /* logging parameter value */
    uint16_t lenstrValGeneric = (uint16_t) GOAL_STRLEN(strValGeneric); /* logging parameter length */
    const char strValString[]="String";         /* logging parameter value */

    valInt8 = -23;
    /* GG_LOG ID:38: */
    goal_lmLog(GOAL_ID_ZERO, 38, 24, GOAL_STRLEN((const char *) "INT8"), 1, GOAL_LOG_INFO, "type: $1, value: $2");
    goal_lmLogParamSTRING0("INT8");
    goal_lmLogParamINT8(valInt8);
    goal_lmLogFinish();

    valUint8 = 0xe6;
    /* GG_LOG ID:39: */
    goal_lmLog(GOAL_ID_ZERO, 39, 24, GOAL_STRLEN((const char *) "UINT8"), 1, GOAL_LOG_INFO, "type: $1, value: $2");
    goal_lmLogParamSTRING0("UINT8");
    goal_lmLogParamUINT8(valUint8);
    goal_lmLogFinish();

    valInt16 = -258;
    /* GG_LOG ID:40: */
    goal_lmLog(GOAL_ID_ZERO, 40, 24, GOAL_STRLEN((const char *) "INT16"), 2, GOAL_LOG_INFO, "type: $1, value: $2");
    goal_lmLogParamSTRING0("INT16");
    goal_lmLogParamINT16(valInt16);
    goal_lmLogFinish();

    valUint16 = 23504;
    /* GG_LOG ID:41: */
    goal_lmLog(GOAL_ID_ZERO, 41, 24, GOAL_STRLEN((const char *) "UINT16"), 2, GOAL_LOG_INFO, "type: $1, value: $2");
    goal_lmLogParamSTRING0("UINT16");
    goal_lmLogParamUINT16(valUint16);
    goal_lmLogFinish();

    valInt32 = -76043;
    /* GG_LOG ID:42: */
    goal_lmLog(GOAL_ID_ZERO, 42, 24, GOAL_STRLEN((const char *) "INT32"), 4, GOAL_LOG_INFO, "type: $1, value: $2");
    goal_lmLogParamSTRING0("INT32");
    goal_lmLogParamINT32(valInt32);
    goal_lmLogFinish();

    valUint32 = 22021;
    /* GG_LOG ID:43: */
    goal_lmLog(GOAL_ID_ZERO, 43, 24, GOAL_STRLEN((const char *) "UINT32"), 4, GOAL_LOG_INFO, "type: $1, value: $2");
    goal_lmLogParamSTRING0("UINT32");
    goal_lmLogParamUINT32(valUint32);
    goal_lmLogFinish();

    valInt64 = -320356076;
    /* GG_LOG ID:44: */
    goal_lmLog(GOAL_ID_ZERO, 44, 24, GOAL_STRLEN((const char *) "INT64"), 8, GOAL_LOG_INFO, "type: $1, value: $2");
    goal_lmLogParamSTRING0("INT64");
    goal_lmLogParamINT64(valInt64);
    goal_lmLogFinish();

    valUint64 = 630323030;
    /* GG_LOG ID:45: */
    goal_lmLog(GOAL_ID_ZERO, 45, 24, GOAL_STRLEN((const char *) "UINT64"), 8, GOAL_LOG_INFO, "type: $1, value: $2");
    goal_lmLogParamSTRING0("UINT64");
    goal_lmLogParamUINT64(valUint64);
    goal_lmLogFinish();

    valFloat = (float) -2.76;
    /* GG_LOG ID:46: */
    goal_lmLog(GOAL_ID_ZERO, 46, 24, GOAL_STRLEN((const char *) "FLOAT"), 4, GOAL_LOG_INFO, "type: $1, value: $2");
    goal_lmLogParamSTRING0("FLOAT");
    goal_lmLogParamFLOAT(valFloat);
    goal_lmLogFinish();

    pValPtr = (void *) &valInt8;
    /* GG_LOG ID:47: */
    goal_lmLog(GOAL_ID_ZERO, 47, 24, GOAL_STRLEN((const char *) "PTR"), 8, GOAL_LOG_INFO, "type: $1, value: $2");
    goal_lmLogParamSTRING0("PTR");
    goal_lmLogParamPTR(pValPtr);
    goal_lmLogFinish();

    valPtrDiff = 0x00020000;
    /* GG_LOG ID:48: */
    goal_lmLog(GOAL_ID_ZERO, 48, 24, GOAL_STRLEN((const char *) "PTRDIFF"), 8, GOAL_LOG_INFO, "type: $1, value: $2");
    goal_lmLogParamSTRING0("PTRDIFF");
    goal_lmLogParamPTRDIFF(valPtrDiff);
    goal_lmLogFinish();

    valIpV4 = 0xc0a80001;
    /* GG_LOG ID:49: */
    goal_lmLog(GOAL_ID_ZERO, 49, 24, GOAL_STRLEN((const char *) "IPV4"), 4, GOAL_LOG_INFO, "type: $1, value: $2");
    goal_lmLogParamSTRING0("IPV4");
    goal_lmLogParamIPV4(valIpV4);
    goal_lmLogFinish();

    /* GG_LOG ID:50: */
    goal_lmLog(GOAL_ID_ZERO, 50, 24, GOAL_STRLEN((const char *) "MAC"), 6, GOAL_LOG_INFO, "type: $1, value: $2");
    goal_lmLogParamSTRING0("MAC");
    goal_lmLogParamMAC(valMac);
    goal_lmLogFinish();

    valBoolean = GOAL_TRUE;
    /* GG_LOG ID:51: */
    goal_lmLog(GOAL_ID_ZERO, 51, 24, GOAL_STRLEN((const char *) "BOOLEAN"), 1, GOAL_LOG_INFO, "type: $1, value: $2");
    goal_lmLogParamSTRING0("BOOLEAN");
    goal_lmLogParamBOOLEAN(valBoolean);
    goal_lmLogFinish();

    /* GG_LOG ID:52: */
    goal_lmLog(GOAL_ID_ZERO, 52, 24, GOAL_STRLEN((const char *) "GENERIC"), (lenstrValGeneric), GOAL_LOG_INFO, "type: $1, value: $2");
    goal_lmLogParamSTRING0("GENERIC");
    goal_lmLogParamGENERIC(strValGeneric);
    goal_lmLogFinish();

    /* GG_LOG ID:53: */
    goal_lmLog(GOAL_ID_ZERO, 53, 24, GOAL_STRLEN((const char *) "STRING0"), GOAL_STRLEN((const char *) strValString), GOAL_LOG_INFO, "type: $1, value: $2");
    goal_lmLogParamSTRING0("STRING0");
    goal_lmLogParamSTRING0(strValString);
    goal_lmLogFinish();

    /* initialize timestamp */
    tsNext = goal_timerTsGet() + APPL_DELAY_MS;

    res = goal_taskCreate(&pTask, "Task", goal_applTask, NULL, 15, GOAL_TASK_STACK_DEFAULT, GOAL_TRUE);

    return res;
}


/****************************************************************************/
/** Application Loop
 */
void appl_loop(
    void
)
{
    GOAL_TIMESTAMP_T tsCur;                     /* current timestamp */

    /* get current timestamp */
    tsCur = goal_timerTsGet();

    /* cyclicly log a message */
    if (tsNext <= tsCur) {
        appl_logMessage(0);

        tsNext = tsCur + APPL_DELAY_MS;
    }
}


/****************************************************************************/
/** Log task, cyclicly logs a message
 *
 */
static GOAL_TARGET_NORETURN GOAL_STATUS_T goal_applTask(
    void *pArg                                  /**< task argument */
)
{
    UNUSEDARG(pArg);

    while (1) {
        goal_taskMsSleep(APPL_DELAY_MS, NULL);
        appl_logMessage(1);
    }
}


/****************************************************************************/
/** Log a single message
 *
 */
void appl_logMessage(
    int id                                      /**< id for log source */
)
{
    const char strValStringTmplA[] = "01234567890123456789012345678901"; /* log parameter template */
    const char strValStringTmplB[] = "abcdefghijklmnopqrstuvwxyzABCDEF"; /* log parameter template */

    char strValString1[APPL_STR_LEN + 1];       /* log parameter string */
    char strValString2[APPL_STR_LEN + 1];       /* log parameter string */
    int lenId0, lenId1;                         /* log parameter length */

    /* randomize parameter lengths */
    lenId0 = 1 + (rand() % (APPL_STR_LEN - 1));
    lenId1 = 1 + (rand() % (APPL_STR_LEN - 1));

    /* create parameter depending on id given */
    if (0 == id) {
        memcpy(strValString1, strValStringTmplA, sizeof(strValStringTmplA));
        memcpy(strValString2, strValStringTmplA, sizeof(strValStringTmplA));
    }
    else {
        memcpy(strValString1, strValStringTmplB, sizeof(strValStringTmplB));
        memcpy(strValString2, strValStringTmplB, sizeof(strValStringTmplB));
    }

    /* set actual parameter lengths */
    strValString1[lenId0] = '\0';
    strValString2[lenId1] = '\0';

    /* usage of existing log message */
    goal_lmLog(GOAL_ID_ZERO, 53, 24, GOAL_STRLEN((const char *) strValString1), GOAL_STRLEN((const char *) strValString2), GOAL_LOG_INFO, "type: $1, value: $2");
    goal_lmLogParamSTRING0(strValString1);
    goal_lmLogParamSTRING0(strValString2);
    goal_lmLogFinish();
}
