/** @file
 *
 * @brief Config Manager CLI command
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

#define GOAL_ID GOAL_ID_CM
#include <goal_includes.h>

#if GOAL_CONFIG_CLI == 1


/****************************************************************************/
/* Local defines */
/****************************************************************************/
#define GOAL_CM_CLI_CMD       "cm"
#define GOAL_CM_CLI_HELP      "Config Manager interface"

#define CMD_SET_PARAMLEN      5
#define CMD_GET_PARAMLEN      4
#define CMD_SHOW_PARAMLEN     2


/****************************************************************************/
/* Local constants */
/****************************************************************************/
static const char strSet[] = "set";             /**< set */
static const char strShow[] = "show";           /**< show */


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/
static void goal_cmCliPrintHelp(
    void
);

static GOAL_STATUS_T goal_cmCliSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static GOAL_STATUS_T goal_cmCliGet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static GOAL_STATUS_T goal_cmCliPrintVal(
    GOAL_CM_VAR_T *pVar                         /**< Var pointer */
);

static void goal_cmCliPrintVarList(
    void
);

static void goal_cmCliFunc(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static GOAL_STATUS_T goal_cmCliSetVal(
    GOAL_CM_VAR_T *pVar,                        /**< Var pointer */
    GOAL_CLI_DATA_T *pData                      /**< Cmd line data */
);


/****************************************************************************/
/** Register 'cm' CLI command
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_cmCliReg(
    void
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_CLI_CMD_T *pHdlCm = NULL;              /* 'cm' handle */

    /* 'cm' main command */
    res = goal_cliCmdReg(GOAL_CM_CLI_CMD, GOAL_CM_CLI_HELP, goal_cmCliFunc, NULL, &pHdlCm);

    /* 'cm set' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(strSet, NULL, NULL, pHdlCm, NULL);
    }

    /* 'cm show' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(strShow, NULL, NULL, pHdlCm, NULL);
    }

    return res;
}


/****************************************************************************/
/** Prints the value of the given var entry to the CLI
 *
 * @retval GOAL_OK if successfull, otherwise error code
 */
static GOAL_STATUS_T goal_cmCliPrintVal(
    GOAL_CM_VAR_T *pVar                         /**< Var pointer */
)
{
    uint32_t idx;                               /* index */
    char *pData;                                /* data pointer */

    if (!pVar) {
      return GOAL_ERR_NULL_POINTER;
    }

    /* Print the value depending on the type */
    switch (pVar->type) {
        case GOAL_CM_UINT32:
            goal_cliPrintf("0x%x\n", GOAL_CM_VAR_UINT32(pVar));
            break;
        case GOAL_CM_INT32:
            goal_cliPrintf("%i\n", GOAL_CM_VAR_INT32(pVar));
            break;
        case GOAL_CM_UINT16:
            goal_cliPrintf("0x%x\n", GOAL_CM_VAR_UINT16(pVar));
            break;
        case GOAL_CM_INT16:
            goal_cliPrintf("%i\n", GOAL_CM_VAR_INT16(pVar));
            break;
        case GOAL_CM_UINT8:
            goal_cliPrintf("0x%x\n", GOAL_CM_VAR_UINT8(pVar));
            break;
        case GOAL_CM_INT8:
            goal_cliPrintf("%i\n", GOAL_CM_VAR_INT8(pVar));
            break;
        case GOAL_CM_STRING:
            goal_cliPrintf("%s\n", GOAL_CM_VAR_STRING(pVar));
            break;
        case GOAL_CM_GENERIC:
            goal_cliPrintf(" 0: ");
            pData = GOAL_CM_VAR_STRING(pVar);
            for (idx = 0; idx < pVar->curSize; idx++) {
                goal_cliPrintf("%x ", (uint8_t) *((uint8_t *) (pData + idx)));
                if (!(idx % 16) & (0 != idx)) {
                    goal_cliPrintf("\n");
                    goal_cliPrintf("%02u: ", idx);
                }
            }
            goal_cliPrintf("\n");
            break;
        case GOAL_CM_IPV4:
            pData = GOAL_CM_VAR_STRING(pVar);
            goal_cliPrintf("%hhd.%hhd.%hhd.%hhd\n", (pData)[3], (pData)[2], (pData)[1], (pData)[0]);
            break;
        default:
            goal_cliPrintf("Unknown type\n");
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Prints the value of the given var entry to the CLI
 *
 * @retval GOAL_OK if successfull, otherwise error code
 */
static GOAL_STATUS_T goal_cmCliSetVal(
    GOAL_CM_VAR_T *pVar,                        /**< Var pointer */
    GOAL_CLI_DATA_T *pData                      /**< Cmd line data */
)
{
    GOAL_STATUS_T res;                          /* Result */
    const char *pStr;                           /* Param string */
    unsigned int strLen;                        /* Param string length */
    uint32_t uval;                              /* Unsigned value */
    int32_t sval;                               /* Signed value */
    char *pEnd;                                 /* End pointer */

    /* Get value parameter */
    res = goal_cliParamGet(pData, CMD_SET_PARAMLEN - 1, &pStr, &strLen);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* Parse the value depending on the type */
    switch (pVar->type) {
        case GOAL_CM_UINT32:
            uval = (uint32_t) GOAL_STRTOUL(pStr, &pEnd, 10);
            if ((ERANGE == GOAL_ERRNO) || (pEnd != (pStr + strLen))) {
                goal_cliPrintf("Invalid parameter\n");
                return GOAL_ERR_PARAM;
            }
            GOAL_CM_SETVAR_UINT32(pVar, uval);
            break;
        case GOAL_CM_INT32:
            sval = (int32_t) GOAL_STRTOL(pStr, &pEnd, 10);
            if ((ERANGE == GOAL_ERRNO) || (pEnd != (pStr + strLen))) {
                goal_cliPrintf("Invalid parameter\n");
                return GOAL_ERR_PARAM;
            }
            GOAL_CM_SETVAR_INT32(pVar, sval);
            break;
        case GOAL_CM_UINT16:
            uval = (uint32_t) GOAL_STRTOUL(pStr, &pEnd, 10);
            if ((ERANGE == GOAL_ERRNO) || (pEnd != (pStr + strLen))) {
                goal_cliPrintf("Invalid parameter\n");
                return GOAL_ERR_PARAM;
            }
            GOAL_CM_SETVAR_UINT16(pVar, (uint16_t) uval);
            break;
        case GOAL_CM_INT16:
            sval = (int32_t) GOAL_STRTOL(pStr, &pEnd, 10);
            if ((ERANGE == GOAL_ERRNO) || (pEnd != (pStr + strLen))) {
                goal_cliPrintf("Invalid parameter\n");
                return GOAL_ERR_PARAM;
            }
            GOAL_CM_SETVAR_INT16(pVar, (int16_t) sval);
            break;
        case GOAL_CM_UINT8:
            uval = (uint32_t) GOAL_STRTOUL(pStr, &pEnd, 10);
            if ((ERANGE == GOAL_ERRNO) || (pEnd != (pStr + strLen))) {
                goal_cliPrintf("Invalid parameter\n");
                return GOAL_ERR_PARAM;
            }
            GOAL_CM_SETVAR_UINT8(pVar, (uint8_t) uval);
            break;
        case GOAL_CM_INT8:
            sval = (int32_t) GOAL_STRTOL(pStr, &pEnd, 10);
            if ((ERANGE == GOAL_ERRNO) || (pEnd != (pStr + strLen))) {
                goal_cliPrintf("Invalid parameter\n");
                return GOAL_ERR_PARAM;
            }
            GOAL_CM_SETVAR_INT8(pVar, (int8_t) sval);
            break;
        case GOAL_CM_STRING:
            goal_cliPrintf("%s\n", GOAL_CM_VAR_STRING(pVar));
            /* Strings must start and end with a " */
            if (('"' != *pStr) || ('"' != *(pStr + strLen - 1))) {
                goal_cliPrintf("Invalid parameter\n");
                return GOAL_ERR_PARAM;
            }
            /* Check string length */
            if ((strLen - 2) > (pVar->totalSize - 2)) {
                goal_cliPrintf("Param to long for this var. Max. length: %u\n", pVar->totalSize - 2);
                return GOAL_ERR_PARAM;
            }
            GOAL_CM_SETVAR_GENERIC(pVar, pStr + 1, strLen - 2);
            break;
        case GOAL_CM_GENERIC:
            goal_cliPrintf("Setting generic values not yet supported\n");
            break;
        case GOAL_CM_IPV4:
            res = goal_netChanIpAddrStrToU32Convert(pStr, strLen, &uval);
            if (GOAL_RES_OK(res)) {
                GOAL_CM_SETVAR_UINT32(pVar, uval);
            } else {
                goal_cliPrintf("Invalid IP address: %s\n", uval);
                return GOAL_ERR_PARAM;
            }
            break;
        default:
            goal_cliPrintf("Unknown type, setting not supported\n");
    }

    return res;
}


/****************************************************************************/
/** Print the list of available variables to the CLI
 *
 */
static void goal_cmCliPrintVarList(
    void
)
{
    uint32_t idx;                               /* Index */
    uint32_t modCnt;                            /* module counter */
    GOAL_CM_MODULE_T *pCmModulesPool;           /* modules pool */
    GOAL_CM_VARENTRY_T *pVar;                   /* Varentry pointer */
    GOAL_BOOL_T flgNone = GOAL_TRUE;            /* no-variables flag */

    /* retrieve raw module data */
    modCnt = goal_cmModGetCnt();
    pCmModulesPool = goal_cmModGetPool();

    /* iterate through all modules and show variables */
    for (idx = 0; idx < modCnt; idx++) {

        for (pVar = pCmModulesPool[idx].var; pVar->varId != GOAL_VM_VAR_EOL; pVar++) {

#if GOAL_CM_NAMES == 1
            goal_cliPrintf("%" FMT_u32 "-%" FMT_u32 ": type %" FMT_u32 " curSize %" FMT_u32 " max. size %" FMT_u32 " (%s:%s)\n",
                           pVar->var->modId, pVar->var->varId, pVar->var->type, pVar->var->curSize, pVar->var->totalSize,
                           pCmModulesPool[idx].modname, pVar->varname);
#else
            goal_cliPrintf("%" FMT_u32 "-%" FMT_u32 ": type %" FMT_u32 " curSize %" FMT_u32 " max. size %" FMT_u32 "\n",
                           pVar->var->modId, pVar->var->varId, pVar->var->type, pVar->var->curSize, pVar->var->totalSize);
#endif

            /* clear none-found flag */
            flgNone = GOAL_FALSE;
        }
    }

    /* notice for no registered variables */
    if (flgNone) {
        goal_cliPrintf("no variables are registered\n");
    }
}


/****************************************************************************/
/** Print help text for cmd
 *
 */
static void goal_cmCliPrintHelp(
    void
)
{
    goal_cliPrintf("cm - Config Manager Interface\n");
    goal_cliPrintf("        set  - set value\n");
    goal_cliPrintf("        show - print var (list)\n");
    goal_cliPrintf("Syntax: cm set <moduleid> <varid> <value>\n");
    goal_cliPrintf("        cm show [<moduleid> <varid>]\n");
    goal_cliPrintf("\n");
}


/****************************************************************************/
/** Execution handler for cm set command
 *
 * @retval GOAL_OK if successfull, otherwise error code
 */
static GOAL_STATUS_T goal_cmCliSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_CM_VAR_T *pVar;                        /* Var entry */
    uint32_t modid = 0;                         /* Module id */
    uint32_t varid = 0;                         /* Var id */
    char *pEnd;                                 /* End pointer */
    uint32_t idx;                               /* Index */
    GOAL_STATUS_T res = GOAL_OK;                /* Result */
    unsigned int strLen[CMD_SET_PARAMLEN];      /* Param string length */
    const char *pStr[CMD_SET_PARAMLEN];         /* Param string pointer */
    uint32_t argc = goal_cliParamCount(pData);  /* Argument count */

    if (CMD_SET_PARAMLEN != argc) {
        goal_cliPrintf("Invalid syntax\n");
        goal_cmCliPrintHelp();
        return GOAL_ERR_PARAM;
    }

    /* Retrieve all params */
    for (idx = 0; GOAL_RES_OK(res) && (idx < CMD_SET_PARAMLEN); idx++) {
        res = goal_cliParamGet(pData, idx, &pStr[idx], &strLen[idx]);
    }

    /* Need 5 arguments, otherwise syntax error */
    if (GOAL_RES_ERR(res) || (1 > strLen[2]) || (1 > strLen[3])) {
        goal_cliPrintf("Invalid syntax\n");
        goal_cmCliPrintHelp();
        return GOAL_ERR_PARAM;
    }

    /* Parse module id */
    modid = (uint32_t) GOAL_STRTOUL(pStr[2], &pEnd, 10);
    if (ERANGE == GOAL_ERRNO || pEnd != (pStr[2] + strLen[2])) {
        goal_cliPrintf("Invalid syntax\n");
        goal_cmCliPrintHelp();
        return GOAL_ERR_PARAM;
    }

    /* Parse variable id */
    varid = (uint32_t) GOAL_STRTOUL(pStr[3], &pEnd, 10);
    if (ERANGE == GOAL_ERRNO || pEnd != (pStr[3] + strLen[3])) {
        goal_cliPrintf("Invalid syntax\n");
        goal_cmCliPrintHelp();
        return GOAL_ERR_PARAM;
    }

    /* Get variable */
    res = goal_cmGetVarById(modid, varid, &pVar);

    if (GOAL_OK != res) {
        goal_cliPrintf("Unknown variable. Wrong module or var id?\n");
        return GOAL_ERR_PARAM;
    }

    res = goal_cmCliSetVal(pVar, pData);

    return res;
}


/****************************************************************************/
/** Execution handler for cm get command
 *
 * @retval 0 if successfull, otherwise error code
 */
static GOAL_STATUS_T goal_cmCliGet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    uint32_t modid = 0;                         /* Module id */
    uint32_t varid = 0;                         /* Var id */
    char *pEnd;                                 /* End pointer */
    GOAL_CM_VAR_T *pVar;                        /* Var entry */
    uint32_t idx;                               /* Index */
    GOAL_STATUS_T res = GOAL_OK;                /* Result */
    unsigned int strLen[CMD_GET_PARAMLEN];      /* Param string length */
    const char *pStr[CMD_GET_PARAMLEN];         /* Param pointer */
    uint32_t argc = goal_cliParamCount(pData);  /* argument count */

    if (CMD_SHOW_PARAMLEN == argc) {
        goal_cmCliPrintVarList();
        return GOAL_OK;
    }

    /* Check param count */
    if (CMD_GET_PARAMLEN != argc) {
        goal_cliPrintf("Invalid syntax\n");
        goal_cmCliPrintHelp();
        return GOAL_ERR_PARAM;
    }

    /* Retrieve all params */
    for (idx = 0; GOAL_RES_OK(res) && (idx < CMD_GET_PARAMLEN); idx++) {
        res = goal_cliParamGet(pData, idx, &pStr[idx], &strLen[idx]);
    }

    /* Need 4 arguments, otherwise syntax error */
    if (GOAL_RES_ERR(res) || (1 > strLen[2]) || (1 > strLen[3])) {
        goal_cliPrintf("Invalid syntax\n");
        goal_cmCliPrintHelp();
        return GOAL_ERR_PARAM;
    }

    /* Parse module id */
    modid = (uint32_t) GOAL_STRTOUL(pStr[2], &pEnd, 10);
    if (ERANGE == GOAL_ERRNO || pEnd != (pStr[2] + strLen[2])) {
        goal_cliPrintf("Invalid syntax\n");
        goal_cmCliPrintHelp();
        return GOAL_ERR_PARAM;
    }

    /* Parse variable id */
    varid = (uint32_t) GOAL_STRTOUL(pStr[3], &pEnd, 10);
    if (ERANGE == GOAL_ERRNO || pEnd != (pStr[3] + strLen[3])) {
        goal_cliPrintf("Invalid syntax\n");
        goal_cmCliPrintHelp();
        return GOAL_ERR_PARAM;
    }

    /* Get variable */
    res = goal_cmGetVarById(modid, varid, &pVar);

    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Unknown variable. Wrong module or var id?\n");
        return GOAL_ERR_PARAM;
    }

    return goal_cmCliPrintVal(pVar);
}


/****************************************************************************/
/** Command execution handler
 */
static void goal_cmCliFunc(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    unsigned int strLen;                        /* Param length */
    const char *pStr = NULL;                    /* Param string pointer */
    uint32_t argc = goal_cliParamCount(pData);  /* Argument count */

    /* Need at least 2 arguments */
    if (2 > argc) {
        goal_cliPrintf("Invalid syntax\n");
        goal_cmCliPrintHelp();
        return;
    }

    /* retrieve control command */
    res = goal_cliParamGet(pData, 1, &pStr, &strLen);
    if (GOAL_RES_OK(res)) {

        /* Set command */
        if (strSet == pStr) {
            goal_cmCliSet(pData);
            return;
        }

        /* Show var command */
        if (strShow == pStr) {
            goal_cmCliGet(pData);
            return;
        }
    }
}
#endif /* GOAL_CONFIG_CLI */
