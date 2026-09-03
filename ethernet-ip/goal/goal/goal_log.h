/** @file
 *
 * @brief Message Logger
 *
 * This module implements logging functions that build different kinds of
 * messages. There are different logging levels. For each module logging can be
 * separately activated.
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

#ifndef GOAL_LOG_H
#define GOAL_LOG_H

#include "goal_includes.h"


/****************************************************************************/
/* Config Defaults */
/****************************************************************************/
#if GOAL_CONFIG_LOGGING_TARGET_SYSLOG == 1
#  ifndef GOAL_CONFIG_LOGGING_TARGET_SYSLOG_DST_IP
#    define GOAL_CONFIG_LOGGING_TARGET_SYSLOG_DST_IP 255, 255, 255, 255
#  endif
#  ifndef GOAL_CONFIG_LOGGING_TARGET_SYSLOG_DST_MAC
#    define GOAL_CONFIG_LOGGING_TARGET_SYSLOG_DST_MAC 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
#  endif
#  ifndef GOAL_CONFIG_LOGGING_TARGET_SYSLOG_BUF
#    define GOAL_CONFIG_LOGGING_TARGET_SYSLOG_BUF 2
#  endif
#  ifndef GOAL_CONFIG_LOGGING_TARGET_SYSLOG_SIZE
#    define GOAL_CONFIG_LOGGING_TARGET_SYSLOG_SIZE 256
#  endif
#endif


/****************************************************************************/
/* Defines */
/****************************************************************************/

/* macro to initialize module logging ID */
#if GOAL_CONFIG_LOGGING == 1
#  ifndef GOAL_LOG_LEVEL
#    define GOAL_LOG_LEVEL GOAL_DBG_DEFAULT
#  endif
#endif


/* check if logging is configured correctly */
#if GOAL_CONFIG_LOGGING == 1

#  if !defined(GOAL_CONFIG_LOGGING_BUFFER_SIZE)
#    define GOAL_CONFIG_LOGGING_BUFFER_SIZE 128
#  endif

#  if GOAL_CONFIG_GEN_CTC_AC == 1
#    define GOAL_LOG_MSG_PRE "AC_"
#  elif GOAL_CONFIG_GEN_CTC_CC == 1
#    define GOAL_LOG_MSG_PRE "CC_"
#  endif

#  if !defined(GOAL_LOG_MSG_PRE)
#    define GOAL_LOG_MSG_PRE
#  endif
#endif

/* defines for ModuleDbgFlag */
#define GOAL_DBG_NONE                           (0 << 0) /**< no logging */
#define GOAL_DBG_ERR                            (1 << 0) /**< log errors */
#define GOAL_DBG_WARN                           (1 << 1) /**< log warnings */
#define GOAL_DBG_INFO                           (1 << 2) /**< log information */
#define GOAL_DBG_USER                           (1 << 3) /**< log user */
#define GOAL_DBG_DBG                            (1 << 4) /**< log debug */
#define GOAL_DBG_ALL                            ((GOAL_DBG_DBG * 2) - 1) /**< log everything */
#define GOAL_DBG_DEFAULT                        (GOAL_DBG_INFO | GOAL_DBG_WARN | GOAL_DBG_ERR | GOAL_DBG_USER) /**< default logging */

/* defines for sublevel of Debug level */
#define GOAL_LOG_DBG_SUBLEVEL_HIGHEST           1 /**< highest priority of debug log level */
#define GOAL_LOG_DBG_SUBLEVEL_HIGHER            2 /**< higher priority of debug log level */
#define GOAL_LOG_DBG_SUBLEVEL_HIGH              3 /**< high priority of debug log level */
#define GOAL_LOG_DBG_SUBLEVEL_NORMAL            4 /**< normal priority of debug log level */
#define GOAL_LOG_DBG_SUBLEVEL_LOW               5 /**< low priority of debug log level */
#define GOAL_LOG_DBG_SUBLEVEL_LOWER             6 /**< lower priority of debug log level */
#define GOAL_LOG_DBG_SUBLEVEL_LOWEST            7 /**< lowest priority of debug log level */
#define GOAL_LOG_DBG_SUBLEVEL_DEFAULT           GOAL_LOG_DBG_SUBLEVEL_HIGHEST /**< default debug log sublevel */

/* logging functions */
#if GOAL_CONFIG_LOGGING == 0

#  define goal_logDbg(...) do { } while (0)
#  define goal_logDbgSublevel(...) do { } while (0)
#  define goal_logUser(...) do { } while (0)
#  define goal_logInfo(...) do { } while (0)
#  define goal_logWarn(...) do { } while (0)
#  define goal_logErr(...) do { } while (0)
#  define goal_logSetLevel(...) do { } while (0)
#  define goal_logEnableModule(...) do { } while (0)
#  define goal_logPrintNonBlock(...) do { } while (0)
#  define goal_logDbgSublevelSet(...) do { } while (0)
#  define goal_logModuleEnableAll(...) do { } while (0)
#  define goal_logModuleDisableAll(...) do { } while (0)
#  define goal_logModuleEnable(...) do { } while (0)
#  define goal_logModuleDisable(...) do { } while (0)
#  define goal_logModuleIsEnabled(...) do { } while (0)

#else /* GOAL_CONFIG_LOGGING == 0 */

extern GOAL_LOCK_T *goal_logMtx;
extern GOAL_BOOL_T goal_logInitDone;
#  define goal_logMsg(module, level, ...)  if ((GOAL_TRUE == goal_logInitDone) && \
                                       (level & goal_logLevel) && \
                                       (GOAL_TRUE == goal_logModuleIsEnabled(module))) { \
                                     goal_lockGet(goal_logMtx, GOAL_LOCK_INFINITE); \
                                     goal_logPrintFmt(level, __func__, __LINE__, __VA_ARGS__); \
                                     goal_lockPut(goal_logMtx); \
                                 }

#  define goal_logErr(...)                   do { goal_logMsg(GOAL_ID, GOAL_DBG_ERR, __VA_ARGS__) } while (0)
#  define goal_logWarn(...)                  do { goal_logMsg(GOAL_ID, GOAL_DBG_WARN, __VA_ARGS__) } while (0)
#  define goal_logInfo(...)                  do { goal_logMsg(GOAL_ID, GOAL_DBG_INFO, __VA_ARGS__) } while (0)
#  define goal_logUser(...)                  do { goal_logMsg(GOAL_ID, GOAL_DBG_USER, __VA_ARGS__) } while (0)
#  define goal_logDbgSublevel(sublevel, ...) do { if ((sublevel) <= goal_logDbgSublevel) { goal_logMsg(GOAL_ID, GOAL_DBG_DBG, __VA_ARGS__) } } while (0)
#  define goal_logDbg(...)                   goal_logDbgSublevel(GOAL_LOG_DBG_SUBLEVEL_DEFAULT,  __VA_ARGS__)

#if GOAL_CONFIG_LOGGING_TARGET_NONBLOCK == 0
# define goal_logPrintNonBlock(...) do { } while (0)
#endif


/****************************************************************************/
/* Exported variables */
/****************************************************************************/
extern uint8_t goal_logLevel;                   /**< logging level */
extern uint8_t goal_logDbgSublevel;             /**< debug logging sublevel */
extern uint32_t goal_logModules;                /**< modules with enabled logging */


/****************************************************************************/
/* Prototypes */
/****************************************************************************/

GOAL_STATUS_T goal_logInitPre(
    void
);

void goal_logPrintFmt(
    unsigned int level,                         /**< log level */
    const char *strFunc,                        /**< function name */
    unsigned int line,                          /**< line number */
    const char *strFormat,                      /**< printf-like format description */
    ...                                         /**< printf-like arguments */
) GOAL_TARGET_CC_FORMAT_PRINTF;

void goal_logSetLevel(
    uint8_t level                               /**< logging level */
);

void goal_logDbgSublevelSet(
    uint8_t sublevel                            /**< debug logging sublevel */
);

void goal_logModuleEnableAll(
    void
);

void goal_logModuleDisableAll(
    void
);

void goal_logModuleEnable(
    uint32_t module                             /**< module ID */
);

void goal_logModuleDisable(
    uint32_t module                             /**< module ID */
);

GOAL_BOOL_T goal_logModuleIsEnabled(
    uint32_t module                             /**< module ID */
);

#if GOAL_CONFIG_LOGGING_TARGET_NONBLOCK == 1
void goal_logPrintNonBlock(
    void
);
#endif

#endif /* GOAL_CONFIG_LOGGING == 0 */

#endif /* GOAL_LOG_H */
