/**
 * @brief ACyCLIC - Auto Completition CommandLine InterfaCe
 *
 * A small commandline interface with autocompletition made for embedded
 * devices with low memory resources.
 *
 * Licensed under the MIT license, see LICENSE for details.
 */
#ifndef ACYCLIC_H
#define ACYCLIC_H


/*****************************************************************************/
/* Forward declarations */
/*****************************************************************************/
struct ACYCLIC_T;
struct ACYCLIC_CMD_DATA_T;


/*****************************************************************************/
/* Parameters */
/*****************************************************************************/
#define ACYCLIC_CMDLINE_LEN         50
#define ACYCLIC_CMDLINE_TERM        1
#define ACYCLIC_ARG_COUNT           8
#define ACYCLIC_PROMPT              "$ "


/*****************************************************************************/
/* Defines */
/*****************************************************************************/
#define ACYCLIC_CMP_MATCH           0
#define ACYCLIC_CMP_MISMATCH        1
#define ACYCLIC_CMP_END_FIRST       2
#define ACYCLIC_CMP_END_SECOND      3

#define ACYCLIC_SRCH_NONE           0
#define ACYCLIC_SRCH_FOUND          1
#define ACYCLIC_SRCH_AC             2

#define ACYCLIC_SUBSTR_OPEN         0
#define ACYCLIC_SUBSTR_CLOSED       1

#define ACYCLIC_KEY_TAB             9
#define ACYCLIC_KEY_ESCAPE          0x1b
#define ACYCLIC_KEY_LEFT_BRACKET    0x5b
#define ACYCLIC_KEY_UP              0x41
#define ACYCLIC_KEY_DOWN            0x42
#define ACYCLIC_KEY_RIGHT           0x43
#define ACYCLIC_KEY_LEFT            0x44
#define ACYCLIC_IS_KEY(key)         ((key >= 0x20) && (key <= 0x7e)) || ((key >= 0x80))
#define ACYCLIC_KEY_UTF8            0x80
#define ACYCLIC_KEY_UTF8_2B_BEG     0xc0
#define ACYCLIC_KEY_UTF8_2B_END     0xdf
#define ACYCLIC_KEY_UTF8_3B_BEG     0xe0
#define ACYCLIC_KEY_UTF8_3B_END     0xef
#define ACYCLIC_KEY_UTF8_4B_BEG     0xf0
#define ACYCLIC_KEY_UTF8_BYTES_2    1
#define ACYCLIC_KEY_UTF8_BYTES_3    2
#define ACYCLIC_KEY_UTF8_BYTES_4    3

#define ACYCLIC_FALSE               0
#define ACYCLIC_TRUE                1


/*****************************************************************************/
/* Typedefs */
/*****************************************************************************/
typedef void (* ACYCLIC_CMD_FUNC_T)(struct ACYCLIC_T *a);


/*****************************************************************************/
/* Platform includes */
/*****************************************************************************/
#include "acyclic_platform.h"


/*****************************************************************************/
/* Structures */
/*****************************************************************************/

/**< ACyCLIC command data */
typedef struct ACYCLIC_CMD_T {
    const char * name;                          /**< command name */
    unsigned int len;                           /**< command length */

    struct ACYCLIC_CMD_T *next;                 /**< next command */
    struct ACYCLIC_CMD_T *sub;                  /**< sub commands */

    struct ACYCLIC_CMD_T *next_ac;              /**< next found AC command */

    ACYCLIC_CMD_FUNC_T func;                    /**< command function */
} ACYCLIC_CMD_T;


/**< ACyCLIC arguments data */
typedef struct ACYCLIC_ARG_T {
    const char *name;                           /**< argument name start */
    unsigned int len;                           /**< argument name length */
    ACYCLIC_CMD_T *cmd;                         /**< command pointer */
} ACYCLIC_ARG_T;


/**< ACyCLIC cmd line history */
typedef struct {
    unsigned int cmdline_len;                   /**< commandline length */
    char cmdline_data[ACYCLIC_CMDLINE_LEN + ACYCLIC_CMDLINE_TERM]; /**< commandline content */
} ACYCLIC_HISTORY_T;


/**< ACyCLIC instance data */
typedef struct ACYCLIC_T {
    void *plat;                                 /**< platform specific data */

    ACYCLIC_CMD_T *cmds;                        /**< root command list */

    char *cmdline;                              /**< commandline start */
    unsigned int cmdline_len;                   /**< commandline length */
    char cmdline_data[ACYCLIC_CMDLINE_LEN + ACYCLIC_CMDLINE_TERM]; /**< commandline content */

#if ACYCLIC_HISTORY == 1
    /* Command history */
    ACYCLIC_HISTORY_T cmdHistory[ACYCLIC_HISTORY_SIZE]; /**< Command history */
    ACYCLIC_HISTORY_T *pCmdHistoryStart;        /**< Begin of history in ring buffer */
    ACYCLIC_HISTORY_T *pCmdHistoryEnd;          /**< End of history in ring buffer */
    ACYCLIC_HISTORY_T *pCmdHistoryCurEntry;     /**< Current history entry of ring buffer while scrolling */
#endif

    /* application arguments */
    uint8_t arg_cnt;                            /**< count of found arguments */
    ACYCLIC_ARG_T args[ACYCLIC_ARG_COUNT];      /**< found arguments */

    uint8_t key;
    uint8_t cnt_tab;
    uint8_t flg_prompt;                         /**< show prompt */
    uint8_t flg_cmd_show;                       /**< show commandline */
    uint8_t flg_match_exact;

    uint8_t flg_key_escape;
    uint8_t flg_key_cursor;

    uint8_t cnt_utf8;

    ACYCLIC_CMD_FUNC_T func;                    /**< latest command function */
} ACYCLIC_T;


/*****************************************************************************/
/* Global variables */
/*****************************************************************************/
extern uint8_t acyclic_flg_exit;                /**< exit flag */


/*****************************************************************************/
/* Prototypes */
/*****************************************************************************/
void acyclic_main(
    void
);

uint8_t acyclic_init(
    ACYCLIC_T **ppGlob
);

void acyclic_loop(
    ACYCLIC_T *a                                /**< instance handle */
);

uint8_t acyclic_cmd_not_found(
    ACYCLIC_T *a
);

int acyclic_cmd_add(
    ACYCLIC_T *a,
    ACYCLIC_CMD_T **cmd_root,                   /**< root */
    const char * const cmd_name,                /**< name */
    ACYCLIC_CMD_FUNC_T cmd_func,                /**< function */
    ACYCLIC_CMD_T **cmd_new                     /**< new command */
);

int acyclic_cmd_reg(
    struct ACYCLIC_T *a
);

ACYCLIC_CMD_T * acyclic_cmd_get(
    ACYCLIC_T *a,                               /**< instance handle */
    const char * const strCmd                   /**< commandline */
);


#endif /* ACYCLIC_H */
