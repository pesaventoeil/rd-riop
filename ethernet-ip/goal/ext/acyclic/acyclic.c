/**
 * @brief ACyCLIC - Auto Completion CommandLine InterfaCe
 *
 * A small commandline interface with auto completion made for embedded
 * devices with low memory resources.
 *
 * Licensed under the MIT license, see LICENSE for details.
 */
#include "acyclic.h"


/*****************************************************************************/
/* Local defines */
/*****************************************************************************/
#if ACYCLIC_HISTORY  == 1
/**< Move history pointer macro */
#define ACYCLIC_HISTORY_MOVE_P(x) \
    if ((ACYCLIC_HISTORY_SIZE - 1) == (x - &(a->cmdHistory[0]))) { \
        x = &(a->cmdHistory[0]); \
    } else { \
        x++; \
    }

/**< Get predecessor of history entry x */
#define ACYCLIC_HISTORY_PRED_P(x,y) \
    if (x == &(a->cmdHistory[0])) { \
        y = &(a->cmdHistory[0]) + (ACYCLIC_HISTORY_SIZE - 1); \
    } else { \
        y = x - 1; \
    }
#endif


/*****************************************************************************/
/* Local prototypes */
/*****************************************************************************/
static void acyclic_space_skip(
    char **str,                                 /**< parser string */
    unsigned int *len                           /**< string length */
);

static void acyclic_cmd_show(
    ACYCLIC_T *a,                               /**< instance handle */
    ACYCLIC_CMD_T *cmd,                         /**< commands */
    uint8_t flg_show_all                        /**< show all flag */
);

static void acyclic_ac(
    ACYCLIC_T *a                                /**< instance handle */
);

static void acyclic_bs(
    ACYCLIC_T *a                                /**< instance handle */
);

static uint8_t acyclic_substr(
    char *sub,                                  /**< substring */
    unsigned int *sub_len,                      /**< substring length */
    unsigned int *len                           /**< overall length */
);

static void acyclic_tab_count(
    ACYCLIC_T *a                                /**< instance handle */
);

static void acyclic_key(
    ACYCLIC_T *a                                /**< instance handle */
);

static void acyclic_enter(
    ACYCLIC_T *a                                /**< instance handle */
);

static void acyclic_cmdline(
    ACYCLIC_T *a                                /**< instance handle */
);

static uint8_t acyclic_cmp(
    const char *first,                          /**< first string */
    const char *second,                         /**< second string */
    unsigned int len_first,                     /**< first length */
    unsigned int len_second                     /**< second length */
);

static uint8_t acyclic_srch(
    char *sub,                                  /**< substring */
    unsigned int len_sub,                       /**< substring length */
    ACYCLIC_CMD_T *cmds,                        /**< command list */
    ACYCLIC_CMD_T **cmds_found                  /**< list of found commands */
);

static void acyclic_char_add(
    ACYCLIC_T *a,                               /**< instance handle */
    char chr                                    /**< character */
);

static int acyclic_key_pre(
    ACYCLIC_T *a                                /**< instance handle */
);

#if ACYCLIC_HISTORY == 1
static void acyclic_history(
    ACYCLIC_T *a                                /**< instance handle */
);

static void acyclic_up(
    ACYCLIC_T *a                                /**< instance handle */
);

static void acyclic_print_history(
        ACYCLIC_T *a                            /**< instance handle */
);

static void acyclic_down(
    ACYCLIC_T *a                                /**< instance handle */
);
#endif


#ifdef ACYCLIC_CALLOC_INTERNAL
static void * acyclic_calloc(
    unsigned int size                           /**< allocation size */
);
#endif


/*****************************************************************************/
/* Local variables */
/*****************************************************************************/
DBG_VARS

/* exit flag */
uint8_t acyclic_flg_exit = 0;                   /**< exit flag */

/* global acyclic data structure */
static ACYCLIC_T glob_a;                        /**< global instance */

#ifdef ACYCLIC_CALLOC_INTERNAL
/* local memory pool */
static uint8_t acyclic_mem[ACYCLIC_MEM_POOL];   /**< memory pool */
static unsigned int acyclic_mem_pos = 0;        /**< memory pool position */
#endif


#ifndef ACYCLIC_CONF_INTEGRATED
/*****************************************************************************/
/** Main entrance
 *
 * @retval shell return value
 */
int main(
    void
)
{
    acyclic_main();

    return 0;
}


/*****************************************************************************/
/** Command not-found handler
 *
 * @retval 0 handled
 * @retval other unhandled
 */
uint8_t acyclic_cmd_not_found(
    ACYCLIC_T *a                                /**< instance handle */
)
{
    ACYCLIC_UNUSED(a);

    /* leave command unhandled */
    return 1;
}


/*****************************************************************************/
/** ACyCLIC entrance
 *
 * @retval shell return value
 */
void acyclic_main(
    void
)
{
    /* initialize acyclic data */
    memset(&glob_a, 0, sizeof(ACYCLIC_T));
    glob_a.flg_prompt = 1;

    /* set initial commandline entry */
    glob_a.cmdline = glob_a.cmdline_data;

    /* register commands */
    if (acyclic_cmd_reg(&glob_a)) {
        return;
    }

    DBG_INIT();
    DBG_PRINTF("\n\nacyclic started\n");

    ACYCLIC_TERM_INIT(&glob_a);

    PUTS_LINE_INF("acyclic started");
    while (!acyclic_flg_exit) {
        acyclic_loop(&glob_a);
    }

    ACYCLIC_TERM_EXIT(&glob_a);
    DBG_CLOSE();
}
#endif /* ACYCLIC_CONF_INTEGRATED */

uint8_t acyclic_init(
    ACYCLIC_T **ppGlob
)
{
   /* initialize acyclic data */
    memset(&glob_a, 0, sizeof(ACYCLIC_T));
    glob_a.flg_prompt = 1;

    /* set initial commandline entry */
    glob_a.cmdline = glob_a.cmdline_data;

#if ACYCLIC_HISTORY == 1
    glob_a.pCmdHistoryStart = &(glob_a.cmdHistory[0]);
    glob_a.pCmdHistoryEnd = NULL;
#endif

    DBG_INIT();
    DBG_PRINTF("\n\nacyclic started\n");

    ACYCLIC_TERM_INIT();

    *ppGlob = &glob_a;

    return 0;
}


/*****************************************************************************/
/** Main loop
 */
void acyclic_loop(
    ACYCLIC_T *a                         /**< instance handle */
)
{
    /* show current commandline */
    acyclic_cmdline(a);

    /* read character */
    GETC(a->key);
    if (!a->key) {
        return;
    }
    DBG_PRINTF("[getc]   key: 0x%02x, %d", a->key, a->key);

    /* handle meta sequences */
    if (acyclic_key_pre(a)) {
        return;
    }

    /* handle tab */
    acyclic_tab_count(a);

    /* check control characters */
    switch (a->key) {

        case ACYCLIC_KEY_BS_1:
        case ACYCLIC_KEY_BS_2:
            acyclic_bs(a);
            break;

        case ACYCLIC_KEY_TAB:
            DBG_PRINTF("[tab]    a->cmdline_len: %3u", a->cmdline_len);

            /* reset application arguments */
            a->arg_cnt = 0;

            acyclic_ac(a);
            break;

        case ACYCLIC_KEY_ENTER_1:
        case ACYCLIC_KEY_ENTER_2:
            DBG_PRINTF("[enter]  a->cmdline_len: %3u | a->cmdline: %.*s", a->cmdline_len, a->cmdline_len, a->cmdline);
            acyclic_enter(a);
            break;

#if ACYCLIC_HISTORY == 1
        case ACYCLIC_KEY_UP:
            acyclic_up(a);
            break;

        case ACYCLIC_KEY_DOWN:
            acyclic_down(a);
            break;
#endif

        default:
            acyclic_key(a);
    }
}


/*****************************************************************************/
/** Get command match or list of AC targets
 *
 * @retval ACYCLIC_SRCH_FOUND
 * @retval ACYCLIC_SRCH_AC
 * @retval ACYCLIC_SRCH_NONE
 */
static uint8_t acyclic_srch(
    char *sub,                                  /**< substring */
    unsigned int len_sub,                       /**< substring length */
    ACYCLIC_CMD_T *cmds,                        /**< command list */
    ACYCLIC_CMD_T **cmds_found                  /**< list of found commands */
)
{
    uint8_t res_cmp;
    ACYCLIC_CMD_T *cmd;
    ACYCLIC_CMD_T *cmd_last = NULL;
    uint8_t flg_match_full = 0;

    *cmds_found = NULL;

    /* if no commands are available, return full match */
    if (!cmds) {
        return ACYCLIC_SRCH_NONE;
    }

    for (cmd = cmds; cmd; cmd = cmd->next) {

        res_cmp = acyclic_cmp(sub, cmd->name, len_sub, cmd->len);
        switch (res_cmp) {

            case ACYCLIC_CMP_MATCH:
                flg_match_full = 1;

            case ACYCLIC_CMP_END_FIRST:
                /* found AC match */
                if (!*cmds_found) {
                    *cmds_found = cmd;
                    cmd_last = cmd;
                } else {
                    cmd_last->next_ac = cmd;
                    cmd_last = cmd;
                }
                cmd->next_ac = NULL;
                break;
        }
    }

    /* check if a single full match was found */
    if (flg_match_full) {
        if (!(*cmds_found)->next_ac) {
            return ACYCLIC_SRCH_FOUND;
        }
    }

    return (*cmds_found) ? ACYCLIC_SRCH_AC : ACYCLIC_SRCH_NONE;
}


/*****************************************************************************/
/** Autocomplete command
 */
static void acyclic_ac(
    ACYCLIC_T *a                                /**< instance handle */
)
{
    unsigned int cmdline_len = a->cmdline_len;
    uint8_t res_sub = 0;
    uint8_t res_srch = 0;
    unsigned int cnt;
    ACYCLIC_CMD_T *cmd = a->cmds;
    ACYCLIC_CMD_T *cmds_found = NULL;
    char *arg = a->cmdline;
    unsigned int arg_len;

    /* stop at string end */
    while (cmdline_len) {

        /* remove leading spaces */
        acyclic_space_skip(&arg, &cmdline_len);
        if (!cmdline_len) {
            break;
        }

        /* find and skip substring */
        res_sub = acyclic_substr(arg, &arg_len, &cmdline_len);

        /* find substring in commands */
        res_srch = acyclic_srch(arg, arg_len, cmd, &cmds_found);

        /* tab */
        if (a->cnt_tab && (ACYCLIC_SUBSTR_OPEN == res_sub)) {
            if (ACYCLIC_SRCH_AC == res_srch) {

                /* more than one result found, need double tab to show */
                if (cmds_found->next_ac) {
                    acyclic_cmd_show(a, cmds_found, 0);
                }
                /* only one result found, do autocomplete */
                else {
                    for (cnt = arg_len; cnt < cmds_found->len; cnt++) {
                        acyclic_char_add(a, cmds_found->name[cnt]);
                    }

                    /* update search result */
                    res_srch = ACYCLIC_SRCH_FOUND;
                }
            }

            /* word complete, add space */
            if (ACYCLIC_SRCH_FOUND == res_srch) {
                acyclic_char_add(a, ' ');
            }

            /* no further action needed here */
            return;
        }

        /* no tab requested, need full match on first argument */
        if (0 == a->arg_cnt) {
            if (ACYCLIC_SRCH_FOUND != res_srch) {
                if (0 == a->cnt_tab) {
                    if (acyclic_cmd_not_found(a)) {
                        PUTS_LINE_INF("command not found");
                    }
                    a->flg_prompt = 1;
                    a->flg_cmd_show = 1;
                }

                /* end parsing */
                return;
            }
        }

        /* update found arguments */
        a->args[a->arg_cnt].cmd = cmds_found;
        if (!cmds_found) {
            a->args[a->arg_cnt].name = arg;
        } else {
            a->args[a->arg_cnt].name = cmds_found->name;
            if (cmds_found->func) {
                a->func = cmds_found->func;
            }
        }
        a->args[a->arg_cnt].len = arg_len;
        a->arg_cnt++;

        /* check if next argument can be stored */
        if (ACYCLIC_ARG_COUNT <= a->arg_cnt) {

            /* enhance last found argument to complete rest of the input string */
            if (cmdline_len) {
                a->args[a->arg_cnt - 1].len += cmdline_len;
                a->args[a->arg_cnt - 1].cmd = NULL;

                /* end parsing */
                return;
            }
        }

        /* next argument */
        arg += arg_len;

        /* argument was found, update command list for following parameters */
        if (cmds_found) {
            DBG_PRINTF("[ac]     changing to sub command list");
            cmd = cmds_found->sub;
        }
    }

    /* show available commands on double tab */
    acyclic_cmd_show(a, cmd, 1);
}


/*****************************************************************************/
/** Backspace key handler
 */
static void acyclic_bs(
    ACYCLIC_T *a                                /**< instance handle */
)
{
    if (a->cmdline_len) {
        a->cmdline_len--;
        ACYCLIC_MACRO_BACKSPACE();
    }
    DBG_PRINTF("[back]   a->cmdline_len: %3u | a->cmdline: '%.*s'", a->cmdline_len, a->cmdline_len, a->cmdline);
}


#if ACYCLIC_HISTORY == 1
/*****************************************************************************/
/** Up key handler
 */
static void acyclic_up(
    ACYCLIC_T *a                                /**< instance handle */
)
{
    if (NULL == a->pCmdHistoryEnd) {
        return;
    }

    /* If end of history reached, wrap around */
    if (a->pCmdHistoryCurEntry == a->pCmdHistoryStart) {
        a->pCmdHistoryCurEntry = a->pCmdHistoryEnd;
    }

    /* Get next entry */
    if (NULL == a->pCmdHistoryCurEntry) {
        ACYCLIC_HISTORY_PRED_P(a->pCmdHistoryEnd, a->pCmdHistoryCurEntry);
    } else {
        ACYCLIC_HISTORY_PRED_P(a->pCmdHistoryCurEntry, a->pCmdHistoryCurEntry);
    }

    /* Print the history entry */
    acyclic_print_history(a);

    /* Make history entry the current entry */
    a->cmdline_len = a->pCmdHistoryCurEntry->cmdline_len;
    memcpy(a->cmdline, a->pCmdHistoryCurEntry->cmdline_data, a->pCmdHistoryCurEntry->cmdline_len);
}


/*****************************************************************************/
/** Down key handler
 */
static void acyclic_down(
    ACYCLIC_T *a                                /**< instance handle */
)
{
    ACYCLIC_HISTORY_T *pEntry;                  /**< History entry */


    if (NULL == a->pCmdHistoryEnd) {
        return;
    }

    ACYCLIC_HISTORY_PRED_P(a->pCmdHistoryEnd, pEntry);
    /* If end of history reached, wrap around */
    if (a->pCmdHistoryCurEntry == pEntry) {
        ACYCLIC_HISTORY_PRED_P(a->pCmdHistoryStart, a->pCmdHistoryCurEntry);
    }

    /* Get next entry */
    if (NULL == a->pCmdHistoryCurEntry) {
        ACYCLIC_HISTORY_PRED_P(a->pCmdHistoryEnd, a->pCmdHistoryCurEntry);
    } else {
        /* Get next entry */
        ACYCLIC_HISTORY_MOVE_P(a->pCmdHistoryCurEntry);
    }

    /* Print the history entry */
    acyclic_print_history(a);

    /* Make history entry the current entry */
    a->cmdline_len = a->pCmdHistoryCurEntry->cmdline_len;
    memcpy(a->cmdline, a->pCmdHistoryCurEntry->cmdline_data, a->pCmdHistoryCurEntry->cmdline_len);

}


/*****************************************************************************/
/** Print current history entry
 */
static void acyclic_print_history(
        ACYCLIC_T *a                            /**< ACyCLI data */
)
{
    uint32_t lenDiff;
    uint32_t idx;

    /* Print the history entry */
    PUTC('\r');
    PRINTF(ACYCLIC_PROMPT);
    PRINTF("%.*s", a->pCmdHistoryCurEntry->cmdline_len, a->pCmdHistoryCurEntry->cmdline_data);

    if (a->pCmdHistoryCurEntry->cmdline_len < a->cmdline_len) {
        /* Fill up with spaces if length differs */
        lenDiff = a->cmdline_len - a->pCmdHistoryCurEntry->cmdline_len;
        for (idx = 0; idx < lenDiff; idx++) {
            PUTC(' ');
        }

        /* Bring cursor back */
        for (idx = 0; idx < lenDiff; idx++) {
            ACYCLIC_MACRO_BACKSPACE();
        }
    }
}
#endif /* ACYCLIC_HISTORY  */


/*****************************************************************************/
/** Show AC targets
 */
static void acyclic_cmd_show(
    ACYCLIC_T *a,                               /**< instance handle */
    ACYCLIC_CMD_T *cmd,                         /**< commands */
    uint8_t flg_show_all                        /**< show all flag */
)
{
    if (2 > a->cnt_tab) {
        return;
    }

    if (cmd) {
        ACYCLIC_MACRO_NEWLINE();

        for (; cmd; cmd = (flg_show_all) ? cmd->next : cmd->next_ac) {
            PUTS_INF(cmd->name);
            ACYCLIC_MACRO_SPACE();
        }
    }

    ACYCLIC_MACRO_NEWLINE();
    a->flg_prompt = 1;
    a->flg_cmd_show = 1;
}


/*****************************************************************************/
/** Skip spaces
 *
 * Skip spaces until string end or next non-space character.
 *
 * @retval len
 */
static void acyclic_space_skip(
    char **str,                                 /**< parser string */
    unsigned int *len                           /**< string length */
)
{
    /* eat preceding spaces */
    for (; *len && (' ' == **str); (*str)++, (*len)--) {
        DBG_PRINTF("[ac]     len: %u | pre-space skipped: '%.*s'", *len, *len, *str);
    }
    DBG_PRINTF("[ac]     len: %u | pre-space check finished: '%.*s'", *len, *len, *str);
}


/*****************************************************************************/
/** Return substring and type
 *
 * @retval ACYCLIC_SUBSTR_OPEN
 * @retval ACYCLIC_SUBSTR_CLOSED
 */
static uint8_t acyclic_substr(
    char *sub,                                  /**< substring */
    unsigned int *sub_len,                      /**< substring length */
    unsigned int *len                           /**< overall length */
)
{
    uint8_t res = ACYCLIC_SUBSTR_OPEN;

    for (*sub_len = 0; *sub_len < *len; (*sub_len)++) {
        if (sub[*sub_len] == ' ') {
            res = ACYCLIC_SUBSTR_CLOSED;
            break;
        }
    }

    *len -= *sub_len;

    DBG_PRINTF("[substr] str: '%.*s', len: %d", *sub_len, sub, *sub_len);
    return res;
}


/*****************************************************************************/
/** Tab key handler
 *
 * Count successive tab keys or reset tab count for any other key pressed.
 */
static void acyclic_tab_count(
    ACYCLIC_T *a                                /**< instance handle */
)
{
    if (ACYCLIC_KEY_TAB == a->key) {
        if (a->cnt_tab < 2) {
            a->cnt_tab++;
        }
    } else {
        a->cnt_tab = 0;
    }
}


/*****************************************************************************/
/** Add character to input string
 */
static void acyclic_char_add(
    ACYCLIC_T *a,                               /**< instance handle */
    char chr                                    /**< character */
)
{
    if (a->cmdline_len < ACYCLIC_CMDLINE_LEN) {
        a->cmdline[a->cmdline_len++] = chr;
        PUTC(chr);
        DBG_PRINTF("[add]    a->cmdline_len: %3u | a->cmdline[%u]: %c | a->cmdline: '%.*s'", a->cmdline_len, a->cmdline_len - 1, a->key, a->cmdline_len, a->cmdline);
    }
}


/*****************************************************************************/
/** Handle non-control keys
 */
static void acyclic_key(
    ACYCLIC_T *a                                /**< instance handle */
)
{
    if (ACYCLIC_IS_KEY(a->key)) {
        acyclic_char_add(a, (char) a->key);
    }
    else {
        DBG_PRINTF("[undef]  a->key: %u", a->key);
    }
}


/*****************************************************************************/
/** Handle enter key
 */
static void acyclic_enter(
    ACYCLIC_T *a                                /**< instance handle */
)
{
    ACYCLIC_MACRO_NEWLINE();

    /* make sure commandline is null-terminated */
    a->cmdline[a->cmdline_len] = 0;

#if ACYCLIC_HISTORY == 1
    /* store commandline in history */
    acyclic_history(a);
#endif

    /* reset application arguments */
    a->arg_cnt = 0;
    a->args[0].cmd = NULL;
    a->func = NULL;

    acyclic_ac(a);
    if (acyclic_flg_exit) {
        return;
    }
    a->flg_prompt = 1;
    DBG_PRINTF("[enter]  a->cmdline_len: %3u", a->cmdline_len);

#if ACYCLIC_DEBUG == 1
    if (a->arg_cnt) {
        PUTS_INF("args: ");
        for (cnt = 0; cnt < a->arg_cnt; cnt++) {
            PUTS_INF("[");
            if (a->args[cnt].cmd) {
                PUTC('*');
            }
            for (cnt_arg = 0; cnt_arg < a->args[cnt].len; cnt_arg++) {
                PUTC(a->args[cnt].name[cnt_arg]);
            }
            PUTS_INF("] ");
        }
        ACYCLIC_MACRO_NEWLINE();
    }
#endif

    /* call function callback if available */
    if (a->func) {
        a->func(a);
    } else {
        PUTS_LINE_INF("no function assigned");
    }

    /* clear commandline */
    a->cmdline_len = 0;
}


/*****************************************************************************/
/** Print prompt and commandline
 */
static void acyclic_cmdline(
    ACYCLIC_T *a                                /**< instance handle */
)
{
    unsigned int cnt;

    /* show prompt */
    if (a->flg_prompt) {
        PUTS_INF(ACYCLIC_PROMPT);
        a->flg_prompt = 0;
    }

    if (a->flg_cmd_show) {
        for (cnt = 0; cnt < a->cmdline_len; cnt++) {
            PUTC(a->cmdline[cnt]);
        }
        a->flg_cmd_show = 0;
    }
}


/*****************************************************************************/
/** Compare two strings
 *
 * @retval ACYCLIC_CMP_MATCH strings match
 * @retval ACYCLIC_CMP_MISMATCH strings mismatch
 * @retval ACYCLIC_CMP_END_FIRST first string ended first
 * @retval ACYCLIC_CMP_END_SECOND second string ended first
 */
static uint8_t acyclic_cmp(
    const char *first,                          /**< first string */
    const char *second,                         /**< second string */
    unsigned int len_first,                     /**< first length */
    unsigned int len_second                     /**< second length */
)
{
    uint8_t flg_end_first = 0;
    uint8_t flg_end_second = 0;
    unsigned int cnt;

    for (cnt = 0; 1; cnt++) {

        if (cnt >= len_first) {
            flg_end_first = 1;
        }

        if (cnt >= len_second) {
            flg_end_second = 1;
        }

        if (flg_end_first || flg_end_second) {
            break;
        }

        if (first[cnt] != second[cnt]) {
            return ACYCLIC_CMP_MISMATCH;
        }
    }

    if (flg_end_first && flg_end_second) {
        return ACYCLIC_CMP_MATCH;
    }

    if (flg_end_first) {
        return ACYCLIC_CMP_END_FIRST;
    }

    /* end of destination string */
    return ACYCLIC_CMP_END_SECOND;
}


/*****************************************************************************/
/** Handle meta keys
 *
 * @retval 0 normal key received
 * @retval 1 meta key processed
 */
static int acyclic_key_pre(
    ACYCLIC_T *a                                /**< instance handle */
)
{
    /* detect and skip UTF-8 multibyte sequences */
    if (a->cnt_utf8) {
        a->cnt_utf8--;
        return 1;
    }

    /* detect UTF-8 intro sequence */
    while (ACYCLIC_KEY_UTF8 <= a->key) {

        if ((ACYCLIC_KEY_UTF8_2B_BEG <= a->key) && (ACYCLIC_KEY_UTF8_2B_END >= a->key)) {
            a->cnt_utf8 = ACYCLIC_KEY_UTF8_BYTES_2;
        }
        else if ((ACYCLIC_KEY_UTF8_3B_BEG <= a->key) && (ACYCLIC_KEY_UTF8_3B_END >= a->key)) {
            a->cnt_utf8 = ACYCLIC_KEY_UTF8_BYTES_3;
        }
        else if (ACYCLIC_KEY_UTF8_4B_BEG <= a->key) {
            a->cnt_utf8 = ACYCLIC_KEY_UTF8_BYTES_4;
        }
        else {
            break;
        }

        return 1;
    }

    /* detect escape sequence start */
    if (!a->flg_key_escape) {
        if (ACYCLIC_KEY_ESCAPE == a->key) {
            a->flg_key_escape = 1;
            return 1;
        }

        return 0;
    }

    /* detect left bracket as second escape sequence char */
    if (!a->flg_key_cursor) {
        if (ACYCLIC_KEY_LEFT_BRACKET == a->key) {
            a->flg_key_cursor = 1;
            return 1;
        }

        return 0;
    }

    /* clear escape sequence state */
    a->flg_key_cursor = 0;
    a->flg_key_escape = 0;

    /* Handle escaped keys e.g. for history scrolling */
    switch (a->key) {
#if ACYCLIC_HISTORY == 1
        case ACYCLIC_KEY_UP:
        case ACYCLIC_KEY_DOWN:
            return 0;
#endif
        default:
            return 1;
    }
}


#if ACYCLIC_HISTORY == 1
/*****************************************************************************/
/** Store current command in history
 */
static void acyclic_history(
    ACYCLIC_T *a                                /**< instance handle */
)
{
    /* Do not add entry lines to history */
    if (0 == a->cmdline_len) {
        return;
    }

    if (NULL == a->pCmdHistoryEnd) {
        a->pCmdHistoryEnd = a->pCmdHistoryStart;
    }

    memcpy(a->pCmdHistoryEnd->cmdline_data, a->cmdline, a->cmdline_len);
    a->pCmdHistoryEnd->cmdline_len = a->cmdline_len;

    ACYCLIC_HISTORY_MOVE_P(a->pCmdHistoryEnd);

    if (a->pCmdHistoryStart == a->pCmdHistoryEnd) {
        ACYCLIC_HISTORY_MOVE_P(a->pCmdHistoryStart);
    }

    /* Reset history scrolling */;
    a->pCmdHistoryCurEntry = NULL;
}
#endif


#ifdef ACYCLIC_CALLOC_INTERNAL
/*****************************************************************************/
/** Local mem allocator
 *
 * @retval ptr successful
 * @retval null failed
 */
static void * acyclic_calloc(
    unsigned int size                           /**< allocation size */
)
{
    uint8_t *ptr;
    uint8_t *ptr_end;

    if ((ACYCLIC_MEM_POOL - acyclic_mem_pos) < size) {
        return NULL;
    }

    ptr = &acyclic_mem[acyclic_mem_pos];
    acyclic_mem_pos += size;
    ptr_end = &acyclic_mem[acyclic_mem_pos];

    for (; ptr_end >= ptr; ptr_end--) {
        *ptr_end = 0;
    }

    return ptr;
}
#endif


/*****************************************************************************/
/** Add command to command list
 */
int acyclic_cmd_add(
    ACYCLIC_T *a,
    ACYCLIC_CMD_T **cmd_root,                   /**< root */
    const char * const cmd_name,                /**< name */
    ACYCLIC_CMD_FUNC_T cmd_func,                /**< function */
    ACYCLIC_CMD_T **cmd_new                     /**< new command */
)
{
    struct ACYCLIC_CMD_T **cmd;

    ACYCLIC_UNUSED(a);

    /* find end of command list */
    for (cmd = cmd_root; *cmd; cmd = &(*cmd)->next);

    /* allocate element for new command */
    *cmd = ACYCLIC_CALLOC(sizeof(ACYCLIC_CMD_T));
    if (!*cmd) {
        return -1;
    }

    /* assign command name */
    (*cmd)->name = cmd_name;
    (*cmd)->len = (unsigned int) strlen(cmd_name);

    /* assign function pointer on origin list */
    (*cmd)->func = cmd_func;

    /* assign new command to callers variable */
    if (cmd_new) {
        *cmd_new = *cmd;
    }

    return 0;
}


/*****************************************************************************/
/** Get command pointer of last commandline element
 *
 * @returns command pointer or NULL
 */
ACYCLIC_CMD_T * acyclic_cmd_get(
    ACYCLIC_T *a,                               /**< ACyCLIC handle */
    const char * const strCmd                   /**< commandline */
)
{
    unsigned int len;                           /* command len */
    ACYCLIC_CMD_T *cmd;                         /* command buffer */

    /* copy search string to current commandline */
    len = strlen(strCmd);
    if (ACYCLIC_CMDLINE_LEN < len) {
        return NULL;
    }
    strncpy(a->cmdline, strCmd, len + ACYCLIC_CMDLINE_TERM);
    a->cmdline_len = len;

    acyclic_ac(a);
    if (!a->arg_cnt) {
        return NULL;
    }

    /* reset state */
    cmd = a->args[a->arg_cnt - 1].cmd;
    a->arg_cnt = 0;
    a->args[0].cmd = NULL;
    a->cmdline_len = 0;

    return cmd;
}
