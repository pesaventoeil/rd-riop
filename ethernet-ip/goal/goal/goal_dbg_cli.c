/** @file
 *
 * @brief Debug CLI command
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


#include "goal_includes.h"

#if (GOAL_CONFIG_CLI == 1) && (GOAL_CONFIG_CLI_DBG == 1)
#include <goal_dbg_cli.h>


/****************************************************************************/
/* Local defines */
/****************************************************************************/
#define GOAL_DBG_CLI_CMD            "dbg"
#define GOAL_DBG_CLI_HELP           "Debug Interface"
#define CMD_DBG_PARAMLEN            4
#define CMD_DBG_WORDSIZE            2
#define CMD_DBG_DWORDSIZE           4


/****************************************************************************/
/* Local constants */
/****************************************************************************/
static const char dbgStrMemb[] = "memb";        /**< memb */
static const char dbgStrMemw[] = "memw";        /**< memw */
static const char dbgStrMemd[] = "memd";        /**< memd */
static const char dbgStrShow[] = "show";        /**< show */
static const char dbgStrHelp[] = "help";        /**< help */


/****************************************************************************/
/* Structs/enums */
/****************************************************************************/
typedef enum {
    MEMMODE_8BIT,
    MEMMODE_16BIT,
    MEMMODE_32BIT,
    MEMMODE_NONE = 0xFF
} GOAL_CLI_DBG_MEMMODE_T;


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
static void goal_dbgCliPrintHelp(
    void
);

static void goal_dbgCliMem(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_dbgCliFunc(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static uint32_t goal_dbgCliDwordGet(
    uint8_t *pAddr                              /**< address pointer */
);


/****************************************************************************/
/** Print help for Debug command
 *
 */
static void goal_dbgCliPrintHelp(
    void
)
{
    goal_cliPrintf("dbg - Debug Interface\n");
    goal_cliPrintf("    memb/memw/memd - set/show memory values\n");
    goal_cliPrintf("    dbg mem<b|w|d> show <address> [count]\n");
}


/****************************************************************************/
/** Returns the 32bit value at the given address
 *
 */
static uint32_t goal_dbgCliDwordGet(
    uint8_t *pAddr                              /**< address pointer */
)
{
    uint32_t lowVal;                            /* Lower value */
    uint32_t upperVal;                          /* Upper value */
    PtrCast diff;                               /* Diff until aligned ptr */
    uint32_t lMask = 0;                         /* Mask for lower value */
    uint32_t hMask = 0;                         /* Mask for upper value */
    uint8_t *pAligned;                          /* Aligned pointer */

    /* Get next 32 bit aligned pointer */
    pAligned = (uint8_t *) (((PtrCast) pAddr + 3) & ~0x03u);

    /* Get byte offset 'til next aligned pointer */
    diff = (PtrCast) pAligned - (PtrCast) pAddr;

    /* Already aligned? */
    if (pAligned == pAddr) {
        return *((uint32_t *) pAddr);
    }

    /* Get lower and higher value */
    lowVal = *(((uint32_t *) pAligned) - 1);
    upperVal = *((uint32_t *) pAligned);

    /* Calc the masks for lower and upper value */
    lMask = (1U << (diff * 8)) - 1;
    hMask = (1U << ((4 - diff) * 8)) - 1;

    /* Calc the value and return */
    return ((lowVal >> (8 * (4 - diff))) & lMask) | ((upperVal & hMask) << (diff * 8));
}


/****************************************************************************/
/** Prints the given type of memory
 */
static void goal_dbgCliMemPrint(
    GOAL_CLI_DBG_MEMMODE_T mode,                /**< Mode */
    uint32_t *pAddr,                            /**< Address */
    uint32_t count                              /**< Count */
)
{
    uint32_t val;                               /* Value */
    uint8_t *pBytePtr = (uint8_t *) pAddr;      /* Byte ptr */

    /* Print the value according to access type */
    switch (mode) {
        case MEMMODE_8BIT:
            while (0 < count) {
                val = goal_dbgCliDwordGet(pBytePtr);
                goal_cliPrintf("0x%04x : 0x%02x\n", pBytePtr, (uint8_t) (val & 0xFF));
                count--;
                pBytePtr++;
            }
            break;
        case MEMMODE_16BIT:
            while (0 < count) {
                val = goal_dbgCliDwordGet(pBytePtr);
                goal_cliPrintf("0x%04x : 0x%04x\n", pBytePtr, (uint16_t) (val & 0xFFFF));
                count--;
                pBytePtr += CMD_DBG_WORDSIZE;
            }
            break;
        case MEMMODE_32BIT:
            while (0 < count) {
                goal_cliPrintf("0x%04x : 0x%08x\n", pBytePtr, goal_dbgCliDwordGet(pBytePtr));
                count--;
                pBytePtr += CMD_DBG_DWORDSIZE;
            }
            break;
        default:
            break;
    }
}


/****************************************************************************/
/** Command execution handler
 */
static void goal_dbgCliMem(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    GOAL_CLI_DBG_MEMMODE_T mode;                /* Mode */
    unsigned int strLen;                        /* Param length */
    unsigned long count;                        /* Count */
    PtrCast addr;                               /* Address */
    const char *pStr = NULL;                    /* Param string pointer */
    char *pEndPtr = NULL;                       /* Param string end pointer */

    /* Init values */
    mode = MEMMODE_NONE;
    count = 1;
    addr = 0;

    /* Get show param */
    res = goal_cliParamGet(pData, 2, &pStr, &strLen);
    if (GOAL_RES_OK(res)) {
        if (pStr == dbgStrShow) {

            /* Get address param */
            res = goal_cliParamGet(pData, 3, &pStr, &strLen);
            if (GOAL_RES_OK(res)) {
                addr = strtoul(pStr, &pEndPtr, 0);
            }

            /* Get optional count parameter */
            res = goal_cliParamGet(pData, 4, &pStr, &strLen);
            if (GOAL_RES_OK(res)) {
                count = strtoul(pStr, &pEndPtr, 0);
            }

            /* When count and/or address is 0, the param given was invalid.
             * Otherwise get mode and invoke print method.
             */
            if (0 != count && 0 != addr) {
                res = goal_cliParamGet(pData, 1, &pStr, &strLen);
                if (GOAL_RES_OK(res)) {
                    if (pStr == dbgStrMemb) {
                        mode = MEMMODE_8BIT;
                    } else if (pStr == dbgStrMemw) {
                        mode = MEMMODE_16BIT;
                    } else if (pStr == dbgStrMemd) {
                        mode = MEMMODE_32BIT;
                    }

                    if (MEMMODE_NONE != mode) {
                        goal_dbgCliMemPrint(mode, (uint32_t *) addr, (uint32_t) count);
                        return;
                    }
                }
            }
        }
    }

    goal_cliPrintf("Syntax error\n");
}


/****************************************************************************/
/** Command execution handler
 */
static void goal_dbgCliFunc(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    unsigned int strLen;                        /* Param length */
    const char *pStr = NULL;                    /* Param string pointer */
    uint32_t argc = goal_cliParamCount(pData);  /* Argument count */

    /* Need at least 2 arguments */
    if (CMD_DBG_PARAMLEN > argc) {
        goal_cliPrintf("Invalid syntax\n");
        goal_dbgCliPrintHelp();
        return;
    }

    /* retrieve control command */
    res = goal_cliParamGet(pData, 1, &pStr, &strLen);
    if (GOAL_RES_OK(res)) {

        /* help command */
        if (dbgStrHelp == pStr) {
            goal_dbgCliPrintHelp();
            return;
        }

        /* mem<b|w|d> command */
        if (dbgStrMemb == pStr || dbgStrMemw == pStr || dbgStrMemd == pStr) {
            goal_dbgCliMem(pData);
            return;
        }

    }

    goal_cliPrintf("Syntax error\n");
}


/****************************************************************************/
/** Register 'dbg' CLI command
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_dbgInitCli(
    void
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_CLI_CMD_T *pHdlDbg = NULL;             /* 'dbg' handle */
    GOAL_CLI_CMD_T *pHdlMemb = NULL;            /* 'memb' handle */
    GOAL_CLI_CMD_T *pHdlMemw = NULL;            /* 'memw' handle */
    GOAL_CLI_CMD_T *pHdlMemd = NULL;            /* 'memd' handle */


    /* 'dbg' main command */
    res = goal_cliCmdReg(GOAL_DBG_CLI_CMD, GOAL_DBG_CLI_HELP, goal_dbgCliFunc, NULL, &pHdlDbg);

    /* 'dbg help' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(dbgStrHelp, NULL, NULL, pHdlDbg, NULL);
    }

    /* 'dbg memb' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(dbgStrMemb, NULL, NULL, pHdlDbg, &pHdlMemb);
    }

    /* 'dbg memb show' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(dbgStrShow, NULL, NULL, pHdlMemb, NULL);
    }

    /* 'dbg memw' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(dbgStrMemw, NULL, NULL, pHdlDbg, &pHdlMemw);
    }

    /* 'dbg memw show' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(dbgStrShow, NULL, NULL, pHdlMemw, NULL);
    }

    /* 'dbg memd' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(dbgStrMemd, NULL, NULL, pHdlDbg, &pHdlMemd);
    }

    /* 'dbg memd show' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(dbgStrShow, NULL, NULL, pHdlMemd, NULL);
    }

    return res;
}

#endif /* (GOAL_CONFIG_CLI == 1) && (GOAL_CONFIG_CLI_DBG == 1) */
