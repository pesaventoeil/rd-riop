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

#define GOAL_ID GOAL_ID_LOG
#include <goal_includes.h>


#if GOAL_CONFIG_LOGGING == 1


/****************************************************************************/
/* Constant definitions */
/****************************************************************************/

/* define where the message buffer resides */
#if GOAL_CONFIG_LOGGING_TARGET_SYSLOG == 1
#  define GOAL_LOGGING_BUF syslog_data->data
#else
#  define GOAL_LOGGING_BUF goal_logBuf
#endif

#if GOAL_CONFIG_LOGGING_TARGET_NONBLOCK == 0
# define GOAL_LOGGING_BUF_SIZE GOAL_CONFIG_LOGGING_BUFFER_SIZE
#else
# define GOAL_LOGGING_BUF_SIZE 400
#endif

#if GOAL_CONFIG_LOGGING_TARGET_SYSLOG == 1

/* define ethernet frame length */
#ifndef ETH_FRAME_LEN
#  define ETH_FRAME_LEN 1518
#endif

/* define size of UDP buffer (fragmentation is not supported) */
#define GOAL_LOG_UDP_MSG_LEN \
    ETH_FRAME_LEN - \
    sizeof(syslog_eth_t) + \
    sizeof(syslog_ip_t) + \
    sizeof(syslog_udp_t)

/* define UDP port */
#define GOAL_LOG_UDP_PORT_SYSLOG 514

#endif /* GOAL_CONFIG_LOGGING_TARGET_SYSLOG */


/****************************************************************************/
/* Local defined data types */
/****************************************************************************/

#if GOAL_CONFIG_LOGGING_TARGET_SYSLOG == 1
GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct {
    uint8_t  dst[MAC_ADDR_LEN];
    uint8_t  src[MAC_ADDR_LEN];
    uint16_t proto;
} GOAL_TARGET_PACKED syslog_eth_t;
GOAL_TARGET_PACKED_STRUCT_POST


GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct {
    uint8_t  hlen:4;
    uint8_t  ver:4;
    uint8_t  tos;
    uint16_t len;
    uint16_t id;
    uint16_t off;
    uint8_t  ttl;
    uint8_t  proto;
    uint16_t csum;
    uint32_t src;
    uint32_t dst;
} GOAL_TARGET_PACKED syslog_ip_t;
GOAL_TARGET_PACKED_STRUCT_POST


GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct {
    uint16_t src;
    uint16_t dst;
    uint16_t len;
    uint16_t csum;
} GOAL_TARGET_PACKED syslog_udp_t;
GOAL_TARGET_PACKED_STRUCT_POST


GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct {
    syslog_eth_t eth;
    syslog_ip_t  ip;
    syslog_udp_t udp;
    uint8_t data[GOAL_CONFIG_LOGGING_BUFFER_SIZE];
    uint8_t align[GOAL_ALIGN_PAD];              /**< alignment padding */
} GOAL_TARGET_PACKED syslog_data_t;
GOAL_TARGET_PACKED_STRUCT_POST
#endif /* GOAL_CONFIG_LOGGING_TARGET_SYSLOG */


/****************************************************************************/
/* List of local defined functions */
/****************************************************************************/

#if GOAL_CONFIG_LOGGING_TARGET_SYSLOG == 1
static void goal_logSendSyslog(
    int len                                     /**< length of data in buffer */
);

static GOAL_STATUS_T goal_logInitSyslog(
    void
);

static uint16_t csum(
    uint16_t *pBuf,                             /**< IP header buffer */
    int word_len                                /**< length in words */
);
#endif

#if GOAL_CONFIG_LOGGING_TARGET_NONBLOCK == 1
static void goal_logRingBufAdd(
    int len                                     /**< length of logging string */
);
#endif

static GOAL_STATUS_T goal_logInit(
    void
);

static GOAL_STATUS_T goal_logShutdown(
    void
);


/****************************************************************************/
/* Global variables */
/****************************************************************************/
GOAL_LOCK_T *goal_logMtx = NULL;                /**< logging buffer mutex */
GOAL_BOOL_T goal_logInitDone = GOAL_FALSE;      /**< logging init done */


/****************************************************************************/
/* Local defined variables */
/****************************************************************************/
static GOAL_STAGE_HANDLER_T stageInit;          /**< init stage handler */
static GOAL_STAGE_HANDLER_T stageShutdown;      /**< shutdown stage handler */
#if GOAL_CONFIG_LOGGING_TARGET_SYSLOG == 1
static GOAL_STAGE_HANDLER_T stageSyslog;        /**< syslog stage handler */
#endif

static const char GOAL_log_msg_levels[] = {
    "E"                                         /**< GOAL_DBG_ERR */
    "W"                                         /**< GOAL_DBG_WARN */
    "I"                                         /**< GOAL_DBG_INFO */
    "U"                                         /**< GOAL_DBG_USER */
    "D"                                         /**< GOAL_DBG_DBG */
};

uint8_t goal_logLevel = GOAL_LOG_LEVEL;         /**< logging level */
uint8_t goal_logDbgSublevel = GOAL_LOG_DBG_SUBLEVEL_HIGHEST; /**< debug logging sublevel */

#if GOAL_CONFIG_LOGGING_TARGET_SYSLOG == 1
static GOAL_BOOL_T goal_logInitSyslogDone = GOAL_FALSE; /**< syslog init done */
static syslog_data_t syslog_data_buf;           /**< syslog frame buffer */
static syslog_data_t *syslog_data;              /**< aligned syslog frame pointer */
#else
static uint8_t goal_logBuf[GOAL_LOGGING_BUF_SIZE + 2]; /**< + 2 = new line and zero termination */
#endif

#if GOAL_CONFIG_LOGGING_TARGET_NONBLOCK == 1
static char goal_logRingBuf[GOAL_CONFIG_LOGGING_BUFFER_SIZE]; /**< log ring buffer */
static unsigned int goal_logRdIdx = 0;          /**< read index */
static unsigned int goal_logWrIdx = 0;          /**< write index */
static int goal_logFree = GOAL_CONFIG_LOGGING_BUFFER_SIZE; /**< log buffer free */
#endif

static uint32_t goal_logIdArray[] = GOAL_MODULES_ARRAY; /**< Array containing all GOAL IDs */
static GOAL_BOOL_T *goal_logIdEnabledArray;     /**< Array containing all GOAL ID logging enabled flags */

static const uint32_t goal_logIdCnt = sizeof(goal_logIdArray); /**< total number of GOAL IDs */
static uint32_t goal_logIdEnabledCnt;           /**< number of current enabled log IDs */


/****************************************************************************/
/** Register init and shutdown handlers.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_logInitPre(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* normal logging */
    res = goal_mainStageReg(GOAL_STAGE_LOG_PRE, &stageInit, GOAL_STAGE_INIT, goal_logInit);

#if GOAL_CONFIG_LOGGING_TARGET_SYSLOG == 1
    /* syslog logging */
    if (GOAL_RES_OK(res)) {
        res = goal_mainStageReg(GOAL_STAGE_ETH, &stageSyslog, GOAL_STAGE_INIT, goal_logInitSyslog);
    }
#endif

    /* shutdown logging */
    if (GOAL_RES_OK(res)) {
        res = goal_mainStageReg(GOAL_STAGE_LOG_PRE, &stageShutdown, GOAL_STAGE_SHUTDOWN, goal_logShutdown);
    }

    return res;
}


/****************************************************************************/
/** Initialize the logging module.
 *
 * This function initializes the logging module.
 */
static GOAL_STATUS_T goal_logInit(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* init logging mutex */
    res = goal_lockCreate(GOAL_LOCK_BINARY, &goal_logMtx, 0, 1, GOAL_ID_LOG);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

#if GOAL_CONFIG_LOGGING_TARGET_NONBLOCK == 1
    /* register non-blocking log processing */
    res = goal_mainLoopReg(goal_logPrintNonBlock);
#endif

    /* alloc module enable list */
    if (GOAL_RES_OK(res)) {
        goal_memCalloc(&goal_logIdEnabledArray, goal_logIdCnt * sizeof(GOAL_BOOL_T));
    }

    /* enable logging */
    if (GOAL_RES_OK(res)) {
        goal_logInitDone = GOAL_TRUE;

        /* greeting */
        /* GG_LOG ID:140: */
        goal_lmLog(GOAL_ID_LOG, 140, 111, 4, 4, GOAL_LOG_SEV_INFO, "GOAL Version: $1.$2");
        goal_lmLogParamUINT32(GOAL_VER_MAJ);
        goal_lmLogParamUINT32(GOAL_VER_MIN);
        goal_lmLogFinish();
        /* GG_LOG ID:141: */
        goal_lmLog(GOAL_ID_LOG, 141, 112, 4, 0, GOAL_LOG_SEV_INFO, "GOAL Sub-Version: $1");
        goal_lmLogParamUINT32(GOAL_VER_SUB);
        goal_lmLogFinish();

        /* Output logging configuration */
        goal_logInfo("GOAL log system configuration:");
        goal_logInfo("- General logging %s", (GOAL_CONFIG_LOGGING == 1) ? "enabled" : "off");
        goal_logInfo("- Raw output      %s", (GOAL_CONFIG_LOGGING_TARGET_RAW == 1) ? "enabled" : "off");
        goal_logInfo("- - non-blocking  %s", (GOAL_CONFIG_LOGGING_TARGET_NONBLOCK == 1) ? "enabled" : "off");
        goal_logInfo("- - instant mode  %s", (GOAL_CONFIG_LOGGING_TARGET_RAW_INSTANT == 1) ? "enabled" : "off");
        goal_logInfo("- Syslog output   %s", (GOAL_CONFIG_LOGGING_TARGET_SYSLOG == 1) ? "enabled" : "off");
    }

    /* enable logging all modules */
    if (GOAL_RES_OK(res)) {
        goal_logModuleEnableAll();
    }

    return res;
}


/****************************************************************************/
/** Log message to available logging output.
 *
 * Before printing the module ID and the debug level are checked. If the message
 * doesn't have an active ID or debug level it is dropped.
 */
void goal_logPrintFmt(
    unsigned int level,                         /**< log level */
    const char *strFunc,                        /**< function name */
    unsigned int line,                          /**< line number */
    const char *strFormat,                      /**< printf-like format description */
    ...                                         /**< printf-like arguments */
)
{
    static volatile unsigned int active = 0;    /* print formatter active flag */
    int len = 0;                                /* length */
    int res = 0;                                /* result */
    va_list arglist;                            /* argument list */
    GOAL_BOOL_T errflag = GOAL_FALSE;           /* error flag */
    unsigned int levelIdx = 0;                  /* level index */
#if GOAL_CONFIG_LOGGING_TARGET_RAW_INSTANT == 0
    uint8_t lmLevel = (uint8_t) level;          /* save level info */
#endif

    /* check if logger is already initialized */
#if GOAL_CONFIG_LOGGING_TARGET_SYSLOG == 0
    if (GOAL_FALSE == goal_logInitDone) {
        return;
    }
#else
    if (GOAL_FALSE == goal_logInitSyslogDone) {
        return;
    }
#endif

    /* check if we are already active */
    if (0 == active) {

        /* set active flag to avoid recursive calling */
        active = 1;

        /* create log header */
        for (levelIdx = 0; level > 1; level >>= 1, levelIdx++);
        res = GOAL_SNPRINTF((char *) GOAL_LOGGING_BUF, GOAL_LOGGING_BUF_SIZE,
                        "[" GOAL_LOG_MSG_PRE "%c|%s:%u] ", GOAL_log_msg_levels[levelIdx], strFunc, line);
        if (!((0 < res) && (GOAL_LOGGING_BUF_SIZE > res))) {
            errflag = GOAL_TRUE;
        } else {
            len = res;
        }

        /* add formatted string */
        if (GOAL_FALSE == errflag) {
            GOAL_VA_START(arglist, strFormat);
            res = GOAL_VSNPRINTF((char *)&(GOAL_LOGGING_BUF[len]),
                               GOAL_LOGGING_BUF_SIZE - (unsigned int) len,
                               strFormat, arglist);
            GOAL_VA_END(arglist);
            if (!((0 < res) && (GOAL_LOGGING_BUF_SIZE > res))) {
                errflag = GOAL_TRUE;
            } else {
                len += res;
            }
        }

        /* remove CR and NL from message */
        if ((GOAL_FALSE == errflag) && (0 < len)) {

            if ('\n' == GOAL_LOGGING_BUF[len - 1]) {
                GOAL_LOGGING_BUF[len - 1] = 0x0;
                len--;
            }

            if ('\r' == GOAL_LOGGING_BUF[len - 1]) {
                GOAL_LOGGING_BUF[len - 1] = 0x0;
                len--;
            }

#if GOAL_CONFIG_LOGGING_TARGET_SYSLOG == 1
            goal_logSendSyslog(len);
#endif

#if GOAL_CONFIG_LOGGING_TARGET_RAW_INSTANT == 1
            /* output message instantly (for debug usage only) */
            goal_targetMsgRaw((char *) GOAL_LOGGING_BUF, (unsigned int) len);
#else
            /* output message unsing log management */
            goal_lmLogLegacy(lmLevel, (const char *) GOAL_LOGGING_BUF, (uint16_t) len);
#endif

#if GOAL_CONFIG_LOGGING_TARGET_NONBLOCK == 1
            goal_logRingBufAdd(len);
#endif
        }

        /* clear active flag */
        active = 0;
    }
}


#if GOAL_CONFIG_LOGGING_TARGET_NONBLOCK == 1
/****************************************************************************/
/** Add new logging string to ring buffer
 */
static void goal_logRingBufAdd(
    int len                                     /**< length of logging string */
)
{
    int left;                                   /* ring buffer upper space left */

    /* check if string fits into log buffer */
    if (goal_logFree > len + 1) {

        /* check if a buffer wrap is necessary */
        left = GOAL_CONFIG_LOGGING_BUFFER_SIZE - (int) goal_logWrIdx;
        if (left < len) {
            GOAL_MEMCPY(&goal_logRingBuf[goal_logWrIdx], GOAL_LOGGING_BUF, (size_t) left);
            GOAL_MEMCPY(goal_logRingBuf, &GOAL_LOGGING_BUF[left], (size_t) (len - left));
        } else {
            GOAL_MEMCPY(&goal_logRingBuf[goal_logWrIdx], GOAL_LOGGING_BUF, (size_t) len);
        }

        goal_logWrIdx = (goal_logWrIdx + (unsigned int) len) % GOAL_CONFIG_LOGGING_BUFFER_SIZE;

        /* add line ending */
        goal_logRingBuf[goal_logWrIdx] = '\n';
        goal_logWrIdx = (goal_logWrIdx + 1) % GOAL_CONFIG_LOGGING_BUFFER_SIZE;

        goal_logFree -= len + 1;
    }
}


/****************************************************************************/
/** Print from ring buffer
 *
 * Print log messages from ring buffer to stdout. This function must be called
 * cyclically.
 *
 * @retval GOAL_OK success
 * @retval GOAL_ERR_NODATA ring buffer is empty
 * @retval GOAL_ERR_LOCK_GET_TIMEOUT locking failed
 * @retval GOAL_ERR_WOULDBLOCK device not ready
 */
void goal_logPrintNonBlock(
    void
)
{
    char chr;                                   /* output char */

    if (goal_logRdIdx == goal_logWrIdx) {
        /* no data to send out */
        return;
    }

    if (GOAL_OK != goal_lockGet(goal_logMtx, GOAL_LOCK_INFINITE)) {
        return;
    }

    chr = goal_logRingBuf[goal_logRdIdx];

    if (GOAL_OK != goal_tgtCharPut(chr)) {
        /* device not ready */
        goal_lockPut(goal_logMtx);
        return;
    }
    goal_logFree++;
    goal_logRdIdx = (goal_logRdIdx + 1) % GOAL_CONFIG_LOGGING_BUFFER_SIZE;

    goal_lockPut(goal_logMtx);
}
#endif /* GOAL_CONFIG_LOGGING_TARGET_NONBLOCK */


#if GOAL_CONFIG_LOGGING_TARGET_SYSLOG == 1
/****************************************************************************/
/** Initialize the Syslog packet buffer
 */
static GOAL_STATUS_T goal_logInitSyslog(
    void
)
{
    static const unsigned char dst_ip[GOAL_LEN_IPV4_ADDR] = { GOAL_CONFIG_LOGGING_TARGET_SYSLOG_DST_IP }; /* dest IP addr */
    static const unsigned char dst_mac[MAC_ADDR_LEN] = { GOAL_CONFIG_LOGGING_TARGET_SYSLOG_DST_MAC }; /* dest MAC addr */
    GOAL_STATUS_T res;                          /* result */

    /* reset syslog_data structure */
    GOAL_MEMSET(&syslog_data_buf, 0, sizeof(syslog_data_t));

    /* align syslog buffer to architecture */
    syslog_data = GOAL_alignPtr(GOAL_TARGET_MEM_ALIGN_NET, &syslog_data_buf);

    /* ethernet header */
    GOAL_MEMCPY(&(syslog_data->eth.dst), dst_mac, MAC_ADDR_LEN);
    res = goal_ethMacAddrGet(GOAL_ETH_PORT_HOST, &syslog_data->eth.src);
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:142: */
        goal_lmLog(GOAL_ID_LOG, 142, 113, 0, 0, GOAL_LOG_SEV_ERROR, "failed to get MAC address");
        return res;
    }
    syslog_data->eth.proto = GOAL_htobe16(GOAL_ETH_ETHERTYPE_IPV4);

    /* IP header */
    syslog_data->ip.ver = 0x4; /* version 4 */
    syslog_data->ip.hlen = 0x5; /* header size: 20 byte */
    syslog_data->ip.tos = 0x00; /* type of service */
    syslog_data->ip.id = GOAL_htobe16(0x0001); /* identification */
    syslog_data->ip.off = GOAL_htobe16(0x0000); /* fragment offset */
    syslog_data->ip.ttl = 64; /* time to live */
    syslog_data->ip.proto = 17; /* protocol UDP */
    GOAL_MEMCPY((void *) &(syslog_data->ip.dst), dst_ip, GOAL_LEN_IPV4_ADDR); /* destination IP */

    /* UDP header */
    syslog_data->udp.dst = GOAL_htobe16(GOAL_LOG_UDP_PORT_SYSLOG); /* destination port */
    syslog_data->udp.src = GOAL_htobe16(GOAL_LOG_UDP_PORT_SYSLOG); /* source port */
    syslog_data->udp.csum = 0x00; /* disable UDP checksum */

    /* enable raw frame transmission */
    goal_ethCmd(GOAL_ETH_CMD_RAW_INIT, GOAL_FALSE, GOAL_ETH_PORT_HOST, NULL);

    /* enable syslog logging */
    goal_logInitSyslogDone = GOAL_TRUE;

    return GOAL_OK;
}


/****************************************************************************/
/** Prepare and send raw UDP Syslog packet.
 */
static void goal_logSendSyslog(
    int len                                     /**< length of data in buffer */
)
{
    int data_len = len;                         /* data length */
#if GOAL_CONFIG_TCPIP_STACK == 1
    uint32_t ipAddr;                            /* IPv4 address */
#endif

    /* check if Ethernet packet length is exceeded
       and cut message if necessary */
    if ((unsigned int) len > GOAL_LOG_UDP_MSG_LEN) {
        data_len = GOAL_LOG_UDP_MSG_LEN;
    }

#if GOAL_CONFIG_TCPIP_STACK == 1
    /* update source IP address */
    if (GOAL_OK != goal_netIpGet(&ipAddr, NULL, NULL, NULL)) {
        ipAddr = 0;
    }
    syslog_data->ip.src = GOAL_htobe32(ipAddr);
#else
    syslog_data->ip.src = UINT32_MAX;
#endif

    /* update packet length in IP and UDP header */
    syslog_data->ip.len = GOAL_htobe16(sizeof(syslog_ip_t) + sizeof(syslog_udp_t) + (unsigned int) data_len);
    syslog_data->udp.len = GOAL_htobe16(sizeof(syslog_udp_t) + (unsigned int) data_len);

    /* remove and recalculate IP checksum */
    syslog_data->ip.csum = 0x0000;
    syslog_data->ip.csum = csum((uint16_t *) &(syslog_data->ip), sizeof(syslog_ip_t) / 2);

    /* try to send RAW packet */
    goal_ethSendSyslog((char *) syslog_data, sizeof(syslog_eth_t) + sizeof(syslog_ip_t) + sizeof(syslog_udp_t) + (unsigned int) data_len);
}


/****************************************************************************/
/** Calculate checksum for IP header.
 *
 * @return checksum as 16 bit value
 */
static uint16_t csum(
    uint16_t *pBuf,                             /**< IP header buffer */
    int word_len                                /**< length in words */
)
{
    uint32_t sum = 0;                           /* sum */

    /* iterate through all words and sum them up */
    for (sum = 0; word_len > 0; word_len--) {
        sum += *pBuf++;
    }

    /* do the magic */
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    sum = ~sum;
    return ((uint16_t) (sum & 0xffff));
}

#endif /* GOAL_CONFIG_LOGGING_TARGET_SYSLOG */


/****************************************************************************/
/** Set the global logging level.
 *
 * This function activates the logging level. THe following values or an ORed
 * combination of these can be used:
 *
 * @li GOAL_DBG_NONE
 * @li GOAL_DBG_ERR
 * @li GOAL_DBG_WARN
 * @li GOAL_DBG_INFO
 * @li GOAL_DBG_USER
 * @li GOAL_DBG_DBG
 * @li GOAL_DBG_ALL
 */
void goal_logSetLevel(
    uint8_t level                               /**< logging level */
)
{
    goal_logLevel = level;
}


/****************************************************************************/
/** Enable Logging for all Modules
 *
 */
void goal_logModuleEnableAll(
    void
)
{
    uint32_t cnt;                               /* counter */

    if (GOAL_TRUE == goal_logInitDone) {
        /* enable modules */
        for (cnt = 0u; cnt < goal_logIdCnt; cnt++) {
            goal_logIdEnabledArray[cnt] = GOAL_TRUE;
        }

        /* indicate that all modules are enabled */
        goal_logIdEnabledCnt = goal_logIdCnt;
    }
}


/****************************************************************************/
/** Disable Logging for all Modules
 *
 */
void goal_logModuleDisableAll(
    void
)
{
    uint32_t cnt;                               /* counter */

    if (GOAL_TRUE == goal_logInitDone) {
        /* disable modules */
        for (cnt = 0u; cnt < goal_logIdCnt; cnt++) {
            goal_logIdEnabledArray[cnt] = GOAL_FALSE;
        }

        /* indicate that all modules are disabled */
        goal_logIdEnabledCnt = 0u;
    }
}


/****************************************************************************/
/** Enable Logging for a given Module
 *
 * The module IDs are given in goal_id.h.
 */
void goal_logModuleEnable(
    uint32_t module                             /**< module ID */
)
{
    uint32_t cnt;                               /* counter */

    if (GOAL_TRUE == goal_logInitDone) {
        for (cnt = 0u; cnt < goal_logIdCnt; cnt++) {
            if (goal_logIdArray[cnt] == module) {
                /* found module */
                if (GOAL_FALSE == goal_logIdEnabledArray[cnt])
                {
                    /* enable logging for module */
                    goal_logIdEnabledArray[cnt] = GOAL_TRUE;
                    goal_logIdEnabledCnt++;
                }
                break;
            }
        }
    }
}


/****************************************************************************/
/** Enable Logging for a given Module
 *
 * The module IDs are given in goal_id.h.
 */
void goal_logModuleDisable(
    uint32_t module                             /**< module ID */
)
{
    uint32_t cnt;                               /* counter */

    if (GOAL_TRUE == goal_logInitDone) {
        for (cnt = 0u; cnt < goal_logIdCnt; cnt++) {
            if (goal_logIdArray[cnt] == module) {
                /* found module */
                if (GOAL_TRUE == goal_logIdEnabledArray[cnt])
                {
                    /* disable logging for module */
                    goal_logIdEnabledArray[cnt] = GOAL_FALSE;
                    goal_logIdEnabledCnt--;
                }
                break;
            }
        }
    }
}


/****************************************************************************/
/** Check if Logging of a given Module is activated
 *
 * @return GOAL_BOOL_T value
 */
GOAL_BOOL_T goal_logModuleIsEnabled(
    uint32_t module                             /**< module ID */
)
{
    GOAL_BOOL_T flgEnabled = GOAL_FALSE;        /* module logging enabled Flag */
    uint32_t cnt;                               /* counter */

    if (GOAL_TRUE == goal_logInitDone) {
        /* check trivial cases first */
        if (0 == goal_logIdEnabledCnt) {
            /* no module is enabled */
            flgEnabled = GOAL_FALSE;
        }
        else if (goal_logIdCnt == goal_logIdEnabledCnt) {
            /* all modules are enabled */
            flgEnabled = GOAL_TRUE;
        }
        else {
            for (cnt = 0u; cnt < goal_logIdCnt; cnt++) {
                if (goal_logIdArray[cnt] == module) {
                    /* found module */
                    if (GOAL_TRUE == goal_logIdEnabledArray[cnt])
                    {
                        flgEnabled = GOAL_TRUE;
                    }
                    else {
                        flgEnabled = GOAL_FALSE;
                    }
                    break;
                }
            }
        }
    }

    return flgEnabled;
}


/****************************************************************************/
/** Set the global debug logging sublevel.
 *
 * This function activates the logging level. The following sublevels are
 * usable:
 *
 * @li GOAL_LOG_DBG_SUBLEVEL_HIGHEST
 * @li GOAL_LOG_DBG_SUBLEVEL_HIGHER
 * @li GOAL_LOG_DBG_SUBLEVEL_HIGH
 * @li GOAL_LOG_DBG_SUBLEVEL_NORMAL
 * @li GOAL_LOG_DBG_SUBLEVEL_LOW
 * @li GOAL_LOG_DBG_SUBLEVEL_LOWER
 * @li GOAL_LOG_DBG_SUBLEVEL_LOWEST
 */
void goal_logDbgSublevelSet(
    uint8_t sublevel                            /**< debug logging sublevel */
)
{
    goal_logDbgSublevel = sublevel;
}


/****************************************************************************/
/** Shutdown Logging handling
 *
 * This function flushes the logging buffer.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
static GOAL_STATUS_T goal_logShutdown(
    void
)
{
#if GOAL_CONFIG_LOGGING_TARGET_NONBLOCK == 1
    /* flush ring buffer */
    while (goal_logRdIdx != goal_logWrIdx) {
        goal_logPrintNonBlock();
    }
#endif

    goal_lockDelete(goal_logMtx);

    goal_logInitDone = GOAL_FALSE;

    return GOAL_OK;
}


#endif /* GOAL_CONFIG_LOGGING */
