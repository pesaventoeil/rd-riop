/** @file
 *
 * @brief GOAL Adaption Layer for ACyCLIC
 *
 * @copyright
 * Copyright 2020 port GmbH Halle/Saale.
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
#ifndef GOAL_CLI_H
#define GOAL_CLI_H

#include <acyclic.h>
#include <goal_cli_udp.h>


/****************************************************************************/
/* Forward defines */
/****************************************************************************/
struct ACYCLIC_T;
struct ACYCLIC_CMD_T;


/****************************************************************************/
/* Defines */
/****************************************************************************/
#define GOAL_CLI_PARAM(x) x
#define GOAL_CLI_DATA_T struct ACYCLIC_T
#define GOAL_CLI_CMD_T struct ACYCLIC_CMD_T
#define GOAL_CLI_CMD_FUNC_T ACYCLIC_CMD_FUNC_T
#define GOAL_CLI_CMD_HELP "help"
#define GOAL_CLI_CMD_HELP_TEXT "this help"


/****************************************************************************/
/* Typedefs */
/****************************************************************************/
typedef GOAL_STATUS_T (* GOAL_CLI_CMD_UNKNOWN_T)(GOAL_CLI_DATA_T *);


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_cliInit(
    GOAL_CLI_CMD_UNKNOWN_T pFunc                /**< unknown cmd handler */
);

GOAL_STATUS_T goal_cliCmdReg(
    const char *strCmd,                         /**< command name */
    const char *strCmdHelp,                     /**< command help text */
    GOAL_CLI_CMD_FUNC_T pFunc,                  /**< command function */
    GOAL_CLI_CMD_T *pHdlCmd,                    /**< parent command handle */
    GOAL_CLI_CMD_T **ppHdlCmdChild              /**< child command handle */
);

GOAL_STATUS_T goal_cliCmdGet(
    GOAL_CLI_CMD_T **ppHdlCmd,                  /**< command handle */
    const char *strCmd                          /**< command name */
);

unsigned int goal_cliParamCount(
    GOAL_CLI_DATA_T *pAcyclic                   /**< ACyCLIC handle */
);

GOAL_STATUS_T goal_cliParamGet(
    GOAL_CLI_DATA_T *pAcyclic,                  /**< ACyCLIC handle */
    unsigned int idx,                           /**< parameter index */
    const char **ppStr,                         /**< parameter string start */
    unsigned int *pStrLen                       /**< parameter string length */
);

GOAL_STATUS_T goal_cliParamAsPortGet(
    GOAL_CLI_DATA_T *pData,                     /**< Parameter data */
    uint32_t idx,                               /**< Parameter index */
    uint32_t *pPort,                            /**< Pointer to port number */
    uint32_t *pPortCount                        /**< Pointer to number of ports */
);

GOAL_STATUS_T goal_cliParamAsUint32Get(
    GOAL_CLI_DATA_T *pData,                     /**< Parameter data */
    uint32_t idx,                               /**< Parameter index */
    uint32_t *pVal                              /**< Pointer to parsed value */
);

GOAL_STATUS_T goal_cliParamAsIpGet(
    GOAL_CLI_DATA_T *pData,                     /**< Parameter data */
    uint32_t idx,                               /**< Parameter index */
    uint32_t *pVal,                             /**< Pointer to parsed IP */
    GOAL_BOOL_T isNetmask                       /**< If true, checks whether IP is valid netmask */
);

GOAL_STATUS_T goal_cliParamAsMaskGet(
    GOAL_CLI_DATA_T *pData,                     /**< Parameter data */
    uint32_t idx,                               /**< Parameter index */
    uint32_t *pVal,                             /**< Pointer to parsed value */
    uint32_t len                                /**< The required len */
);

void goal_cliPrintMaskParam(
    uint32_t val,                               /**< Value to print */
    uint32_t len                                /**< Bit count of mask */
);

#endif /* GOAL_CLI_H */
