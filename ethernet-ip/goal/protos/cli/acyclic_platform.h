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
#ifndef ACYCLIC_PLATFORM_H
#define ACYCLIC_PLATFORM_H

#include <goal_includes.h>
#include <errno.h>


/****************************************************************************/
/* Forward declarations */
/****************************************************************************/
struct ACYCLIC_T;


/****************************************************************************/
/* Defines */
/****************************************************************************/
#define ACYCLIC_DEBUG               0
#define ACYCLIC_UNUSED(x)           (void)(x)
#define ACYCLIC_CALLOC(x)           goal_cliCalloc(x)
#define ACYCLIC_KEY_ENTER_1         10
#define ACYCLIC_KEY_ENTER_2         13
#define ACYCLIC_KEY_BS_1            8
#define ACYCLIC_KEY_BS_2            127

#if GOAL_CONFIG_CLI_UDP == 1
#   define GETC(key)                goal_cliUdpCmdCharGet((char *) &key);
#   define PUTC(x)                  goal_cliUdpCharPut(x);
#else
#   define GETC(key)                goal_tgtCharGet((char *)&key);
#   define PUTC(x)                  goal_tgtCharPut(x);
#endif /* GOAL_CONFIG_CLI_UDP */

#define ACYCLIC_EOL                 '\n'
#define ACYCLIC_MACRO_NEWLINE()     PUTC('\r');                             \
                                    PUTC('\n')

#define PUTS_INF(x)                 {                                       \
                                        const char *s = x;                  \
                                        while (*s) {                        \
                                            if (ACYCLIC_EOL == *s) {        \
                                                ACYCLIC_MACRO_NEWLINE();    \
                                            } else {                        \
                                                PUTC(*s);                   \
                                            }                               \
                                            s++;                            \
                                        }                                   \
                                    }
#define PRINTF(...)                 goal_cliPrintf(__VA_ARGS__)
#define ACYCLIC_MACRO_BACKSPACE()   PUTC('\b'); PUTC(' '); PUTC('\b')
#define DBG_VARS
#define DBG_INIT()
#define DBG_PRINTF(...)
#define DBG_CLOSE()

#define PUTS_LINE_INF(x)            goal_cliPrintf("%s\r\n", x);

#if defined(__LP64__) || defined(_LP64)
#   define ACYCLIC_TERM_INIT(x)     acyclic_term_init(x)
#   define ACYCLIC_TERM_EXIT(x)     acyclic_term_exit(x)
#else
#   define ACYCLIC_TERM_INIT(x)
#   define ACYCLIC_TERM_EXIT(x)
#endif
#define ACYCLIC_MACRO_SPACE()       PUTC(' ')
#define ACYCLIC_CONF_INTEGRATED
#define ACYCLIC_HISTORY             GOAL_CONFIG_CLI_HISTORY
#define ACYCLIC_HISTORY_SIZE        GOAL_CONFIG_CLI_HISTORY_SIZE


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
void *goal_cliCalloc(
    unsigned int size                           /**< calloc size */
);

void goal_cliPrintf(
    const char *strFormat,                      /**< printf-like format description */
    ...                                         /**< printf-like arguments */
);


#endif /* ACYCLIC_PLATFORM_H */
