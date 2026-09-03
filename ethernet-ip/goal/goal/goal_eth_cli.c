/** @file
 *
 * @brief Ethernet CLI command
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

#if GOAL_CONFIG_CLI == 1
#include <goal_eth_cli.h>

#if GOAL_CONFIG_TDMA == 1
#include <goal_ts.h>
#endif /* GOAL_CONFIG_TDMA */


/****************************************************************************/
/* Local defines */
/****************************************************************************/
#define GOAL_ETH_CLI_CMD            "eth"
#define GOAL_ETH_CLI_HELP           "Ethernet Interface"
#define GOAL_ETH_CLI_VLAN_MAX       4096
#define GOAL_ETH_CLI_PRIO_MAX       7

#define GOAL_ETH_CLI_VLAN_MASK      0xFFF
#define GOAL_ETH_CLI_VLAN_PRIO_OFFSET 13

#define CMD_MODESET_PARAMLEN        7
#define CMD_MODESHOW_PARAMLEN       6
#define CMD_MEMADD_PARAMLEN         6
#define CMD_DEFAULTSET_PARAMLEN     7
#define CMD_DEFAULTSHOW_PARAMLEN    5
#define CMD_VERIFYSET_PARAMLEN      6
#define CMD_VERIFYSHOW_PARAMLEN     5
#define CMD_DISCUNKNOWN_PARAMLEN    6
#define CMD_VLAN_PARAMLEN           3
#define CMD_ETH_PARAMLEN            2
#define CMD_MIRRORSET_PARAMLEN      7
#define CMD_MIRRORSHOW_PARAMLEN     5

#define CMD_PARAM_ON                "on"
#define CMD_PARAM_ON_LEN            2
#define CMD_PARAM_OFF               "off"
#define CMD_PARAM_OFF_LEN           3
#define CMD_PORTSTR_LEN             8
#define CMD_PORTSTR_OFFSET          6

#define CMD_MACTAB_CONF_LEARNING         "learning"
#define CMD_MACTAB_CONF_LEARNING_LEN     8
#define CMD_MACTAB_CONF_AGEING           "ageing"
#define CMD_MACTAB_CONF_AGEING_LEN       6
#define CMD_MACTAB_CONF_MIGRATION        "migration"
#define CMD_MACTAB_CONF_MIGRATION_LEN    9
#define CMD_MACTAB_CONF_DISCUNKNOWN      "discunknown"
#define CMD_MACTAB_CONF_DISCUNKNOWN_LEN  11
#define CMD_MACTAB_CONF_PERVLAN          "pervlan"
#define CMD_MACTAB_CONF_PERVLAN_LEN      7

#define CMD_QOS_DEFPRIO_MAX     7
#define CMD_QOS_VLANPRIO_OFFSET 3
#define CMD_QOS_VLANPRIO_MASK   7

#define CMD_QOS_IPPRIO_MAX     63

#define CMD_TXRATE_LIMIT_MAX   80

#define CMD_PORT_MIRROR_PORT    2

#define CMD_TDMA_CYCLE_OFFSET  10


#define GOAL_ETH_CLI_STR_MATCH(x, y) (0 == GOAL_MEMCMP(x, y, GOAL_STRLEN(y)))


/****************************************************************************/
/* Local constants */
/****************************************************************************/
static const char ethStrVlan[] = "vlan";        /**< vlan */
static const char ethStrMode[] = "mode";        /**< mode */
static const char ethStrIn[] = "in";            /**< in */
static const char ethStrOut[] = "out";          /**< out */
static const char ethStrShow[] = "show";        /**< show */
static const char ethStrSet[] = "set";          /**< set */
static const char ethStrPort[] = "port";        /**< port */
static const char ethStrAdd[] = "add";          /**< add */
static const char ethStrRem[] = "rem";          /**< rem */
static const char ethStrDef[] = "default";      /**< default */
static const char ethStrHelp[] = "help";        /**< help */
static const char ethStrTable[] = "table";      /**< table */
static const char ethStrVerify[] = "verify";    /**< table */
static const char ethStrDisc[] = "discunknown"; /**< table */
static const char ethStrMactab[] = "mactab";    /**< mactab */
static const char ethStrConf[] = "conf";        /**< conf */
static const char ethStrMac[] = "mac";          /**< mac */
static const char ethStrDos[] = "dos";          /**< dos */
static const char ethStrMlimit[] = "mlimit";    /**< mlimit */
static const char ethStrBlimit[] = "blimit";    /**< blimit */
static const char ethStrTimebase[] = "timebase"; /**< mlimit */
static const char ethStrTxrate[] = "txrate";    /**< txrate */
static const char ethStrAdstate[] = "adstate";  /**< adstate */
static const char ethStrSpeed[] = "speed";      /**< speed */
static const char ethStrLink[] = "link";        /**< link */
static const char ethStrDuplex[] = "duplex";    /**< duplex */
static const char ethStrQos[] = "qos";          /**< qos */
static const char ethStrDefprio[] = "defprio";  /**< defprio */
static const char ethStrVlanprio[] = "vlanprio"; /**< vlanprio */
static const char ethStrClear[] = "clear";      /**< clear */
static const char ethStrAll[] = "all";          /**< all */
static const char ethStrDynamic[] = "dynamic";  /**< dynamic */
static const char ethStrStatic[] = "static";    /**< static */
static const char ethStrMirror[] = "mirror";    /**< mirror */
static const char ethStrIpprio[] = "ipprio";    /**< ipprio */
static const char ethStrRxRslv[] = "rxrslv";    /**< rxrslv */
static const char ethStrState[] = "state";      /**< state */
static const char ethStrAutoneg[] = "autoneg";  /**< autoneg */
static const char ethStrMdi[] = "mdi";          /**< Media Dependent Interface */

#if GOAL_CONFIG_TDMA == 1
static const char ethStrTdma[] = "tdma";        /**< tdma */
static const char ethStrConfig[] = "config";    /**< config */
static const char ethStrTdmaCt[] = "ct";        /**< ct */
static const char ethStrTdmaPorts[] = "ports"; /**< ports */
static const char ethStrTdmaTslt[] = "timeslot"; /**< timeslot */
#endif


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_ETH_CLI_VLANINMODE_T inModes[] = {
    { "ptrover", GOAL_ETH_VLANIN_PASSTHROUGH_OVERRIDE },
    { "replace", GOAL_ETH_VLANIN_REPLACE },
    { "tag",     GOAL_ETH_VLANIN_TAG_ALWAYS },
    { "disable", GOAL_ETH_VLANIN_DISABLE },
    { NULL,      GOAL_ETH_VLANIN_DISABLE }
};

static GOAL_ETH_CLI_VLANOUTMODE_T outModes[] = {
    { "tagthr",  GOAL_ETH_VLANOUT_TAGTHROUGH },
    { "domain",  GOAL_ETH_VLANOUT_DOMAINTRANSP },
    { "strip",   GOAL_ETH_VLANOUT_STRIP },
    { "disable", GOAL_ETH_VLANOUT_DISABLE },
    { NULL,      GOAL_ETH_VLANOUT_DISABLE }
};

static GOAL_ETH_CLI_SPEEDMODE_T speedModes[] = {
    { "default", GOAL_ETH_SPEED_DEFAULT },
    { "unknown", GOAL_ETH_SPEED_UNKNOWN },
    { "10Mbit", GOAL_ETH_SPEED_10 },
    { "100Mbit", GOAL_ETH_SPEED_100 },
    { "1000Mbit", GOAL_ETH_SPEED_1000 },
    { NULL, 0 }
};

static GOAL_ETH_CLI_LINKSTATE_T linkModes[] = {
    { "Default", GOAL_ETH_STATE_DEFAULT },
    { "Unknown", GOAL_ETH_STATE_UNKNOWN },
    { "Down", GOAL_ETH_STATE_DOWN },
    { "Up", GOAL_ETH_STATE_UP },
    { NULL, 0 }
};

static GOAL_ETH_CLI_DUPLEXMODE_T duplexModes[] = {
    { "default" , GOAL_ETH_DUPLEX_DEFAULT },
    { "unknown", GOAL_ETH_DUPLEX_UNKNOWN },
    { "half", GOAL_ETH_DUPLEX_HALF },
    { "full", GOAL_ETH_DUPLEX_FULL },
    { NULL, 0 }
};

static GOAL_ETH_CLI_AUTONEG_T autonegModes[] = {
    {"default", GOAL_ETH_AUTONEG_DEFAULT},
    {"unknown", GOAL_ETH_AUTONEG_UNKNOWN},
    {"off", GOAL_ETH_AUTONEG_OFF},
    {"on", GOAL_ETH_AUTONEG_ON},
    { NULL, 0 }
};

static GOAL_ETH_CLI_QOSMODE_T qosModes[] = {
    { "vlan",  GOAL_ETH_QOS_MODE_VLAN },
    { "ip",    GOAL_ETH_QOS_MODE_IP },
    { "mac",   GOAL_ETH_QOS_MODE_MAC },
    { "etype", GOAL_ETH_QOS_MODE_TYPE },
    { NULL,    0 }
};

static GOAL_ETH_CLI_MIRRORMODE_T mirrorModes[] = {
    { "eport",  GOAL_ETH_MIRR_MODE_EGRESS_PORT },
    { "inport", GOAL_ETH_MIRR_MODE_INGRESS_PORT },
    { "eda",    GOAL_ETH_MIRR_MODE_EGRESS_DA },
    { "esa",    GOAL_ETH_MIRR_MODE_EGRESS_SA },
    { "inda",   GOAL_ETH_MIRR_MODE_INGRESS_DA },
    { "insa",   GOAL_ETH_MIRR_MODE_INGRESS_SA },
    { NULL,     (GOAL_ETH_MIRR_MODE_T) 0}
};

static GOAL_ETH_CLI_MDISTATE_T mdiState[] = {
    { "default", GOAL_ETH_MDI_STATE_DEFAULT},
    { "unknown", GOAL_ETH_MDI_STATE_UNKNOWN },
    { "uncrossed", GOAL_ETH_MDI_STATE_UNCROSSED},
    { "crossed",  GOAL_ETH_MDI_STATE_CROSSED},
    { NULL, 0 }
};

static GOAL_ETH_CLI_MDIMODE_T mdiMode[] = {
    { "default", GOAL_ETH_MDI_MODE_DEFAULT},
    { "unknown",  GOAL_ETH_MDI_MODE_UNKNOWN},
    { "auto", GOAL_ETH_MDI_MODE_AUTO},
    { "manual", GOAL_ETH_MDI_MODE_MANUAL},
    { NULL, 0 }
};


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/
static void goal_ethCliVlan(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliPrintHelp(
    void
);

static void goal_ethCliVlanInModeShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliVlanOutModeShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliVlanOutModeSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliVlanInModeSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliVlanPortAdd(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliVlanPortRem(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliVlanDefaultSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliVlanDefaultShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliFunc(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliVlanVerifyShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliVlanVerifySet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliVlanDiscunkShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliMactab(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliMactabConfSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliMactabConfShow(
    void
);

static void goal_ethCliMactabMacShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliMactabMacSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliMactabMacRem(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliMactabMacClr(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliDos(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliDosTimeframeShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliDosTimeframeSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliDosMlimitSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliDosMlimitShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliDosBlimitSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliDosBlimitShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliPortAdminStateSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliPortAdminStateShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliPort(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliPortSpeedShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliPortSpeedSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliPortLinkShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliPortDuplexShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliPortDuplexSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliPortAutonegShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliPortAutonegSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliPortMirrorSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliQosModeSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliQos(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliQosModeShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliQosDefprioSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliQosDefprioShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliQosVlanPrioSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliQosVlanPrioShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static GOAL_STATUS_T goal_ethCliParamAsMac(
    const char *pParam,                         /**< parameter */
    const unsigned int lenParam,                /**< parameter length */
    GOAL_ETH_MAC_ADDR_T *pMac                   /**< MAC address buffer */
);

static void goal_ethCliQosIpPrioShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliQosIpPrioSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

#if GOAL_CONFIG_TDMA == 1
static void goal_ethCliTdma(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliTdmaStateShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliTdmaStateSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliTdmaConfigFullShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliTdmaConfigSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliTdmaTimeslotShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliTdmaTimeslotSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);
#endif /* GOAL_CONFIG_TDMA */

static void goal_ethCliRxRslv(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliRxRslvAdd(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliRxRslvRem(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliPortStateSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliPortStateShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliPortMdiStateShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliPortMdiStateSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliPortMdiModeShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);

static void goal_ethCliPortMdiModeSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
);


/****************************************************************************/
/** Print help for Ethernet command
 *
 */
static void goal_ethCliPrintHelp(
    void
)
{
    goal_cliPrintf("eth - Ethernet Interface\n");
    goal_cliPrintf("    vlan - set/show VLAN settings\n");
    goal_cliPrintf("    eth vlan mode in set <port> <ptrover|replace|tag|disable>\n");
    goal_cliPrintf("    eth vlan mode in show [port]\n");
    goal_cliPrintf("    eth vlan mode out set <port> <tagthr|doamin|strip|disable>\n");
    goal_cliPrintf("    eth vlan mode out show [port]\n");
    goal_cliPrintf("    eth vlan port add <port> <vlanid>\n");
    goal_cliPrintf("    eth vlan port rem <port> <vlanid>\n");
    goal_cliPrintf("    eth vlan default set <port> <vlanid> <priority>\n");
    goal_cliPrintf("    eth vlan default show [port]\n");
    goal_cliPrintf("    eth vlan verify set <port> <on|off>\n");
    goal_cliPrintf("    eth vlan verify show [port]\n");
    goal_cliPrintf("    eth vlan discunknown set <port> <on|off>\n");
    goal_cliPrintf("    eth vlan discunknown show [port]\n");
#if GOAL_CONFIG_TDMA == 1
    goal_cliPrintf("    eth tdma state show\n");
    goal_cliPrintf("    eth tdma state set <on|off>\n");
    goal_cliPrintf("    eth tdma config show\n");
    goal_cliPrintf("    eth tdma config set ct <cycle time>\n");
    goal_cliPrintf("    eth tdma config set ports <ports>\n");
    goal_cliPrintf("    eth tdma timeslot show <tslotNum>\n");
    goal_cliPrintf("    eth tdma timeslot set <tslotNum> <startOffset> <queues> <guardBand>\n");
#endif
}


/****************************************************************************/
/** Set the VLAN input processing mode
 *
 */
static void goal_ethCliVlanInModeSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    unsigned int strLen;                        /* Param length */
    unsigned int portStrLen;                    /* Param length */
    const char *pStr = NULL;                    /* Param string pointer */
    const char *pPortStr = NULL;                /* Param string pointer */
    GOAL_ETH_CLI_VLANINMODE_T *pMode;           /* VLAN in mode */
    uint32_t port;                              /* Port number */
    uint32_t portCount;                         /* Port count */
    char *pEndPtr;                              /* End ptr */

    /* Retrieve parameters */
    res = goal_cliParamGet(pData, CMD_MODESET_PARAMLEN - 1, &pStr, &strLen);

    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    res = goal_cliParamGet(pData, CMD_MODESET_PARAMLEN - 2, &pPortStr, &portStrLen);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    port = (uint32_t) GOAL_STRTOUL(pPortStr, &pEndPtr, 10);

    /* Check whether port is valid */
    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count.\n");
        return;
    }

    if ((portCount + 1) < port) {
        goal_cliPrintf("Invalid port number %u. Max. port number is %u.\n", port, portCount);
        return;
    }

    /* Set VLAN input mode */
    if (GOAL_RES_OK(res)) {
        pMode = inModes;
        while (NULL != pMode->strMode) {
            if ((GOAL_STRLEN(pMode->strMode) == strLen) && !strncmp(pMode->strMode, pStr, strLen)) {
                res = goal_ethCmd(GOAL_ETH_CMD_VLAN_MODE_IN, GOAL_TRUE, (portCount == port) ? GOAL_ETH_PORT_HOST : GOAL_ETH_PORT(port), &(pMode->inmode));
                if (GOAL_RES_ERR(res)) {
                    goal_cliPrintf("Setting input failed with code: 0x08%"FMT_x32"\n", res);
                } else {
                    goal_cliPrintf("Input mode of port %u is now %s.\n", port, pStr);
                }
                return;
            }
            pMode++;
        }
    }
    goal_cliPrintf("Unknown input mode: %s\n", pStr);
}


/****************************************************************************/
/** Set the VLAN output processing mode
 *
 */
static void goal_ethCliVlanOutModeSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    unsigned int strLen;                        /* Param length */
    unsigned int portStrLen;                    /* Param length */
    const char *pStr = NULL;                    /* Param string pointer */
    const char *pPortStr = NULL;                /* Param string pointer */
    GOAL_ETH_CLI_VLANOUTMODE_T *pMode;          /* VLAN in mode */
    uint32_t port;                              /* Port number */
    uint32_t portCount;                         /* Port count */
    char *pEndPtr;                              /* End ptr */

    /* Retrieve parameters */
    res = goal_cliParamGet(pData, CMD_MODESET_PARAMLEN - 1, &pStr, &strLen);

    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    res = goal_cliParamGet(pData, CMD_MODESET_PARAMLEN - 2, &pPortStr, &portStrLen);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    port = (uint32_t) GOAL_STRTOUL(pPortStr, &pEndPtr, 10);

    /* Check whether port is valid */
    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count.\n");
        return;
    }

    if ((portCount + 1) < port) {
        goal_cliPrintf("Invalid port number %u. Max. port number is %u.\n", port, portCount);
        return;
    }

    /* Set VLAN input mode */
    if (GOAL_RES_OK(res)) {
        pMode = outModes;
        while (NULL != pMode->strMode) {
            if ((GOAL_STRLEN(pMode->strMode) == strLen) && !strncmp(pMode->strMode, pStr, strLen)) {
                res = goal_ethCmd(GOAL_ETH_CMD_VLAN_MODE_OUT, GOAL_TRUE, (portCount == port) ? GOAL_ETH_PORT_HOST : GOAL_ETH_PORT(port), &(pMode->outmode));
                if (GOAL_RES_ERR(res)) {
                    goal_cliPrintf("Setting output mode failed with code: 0x08%"FMT_x32"\n", res);
                } else {
                    goal_cliPrintf("Output mode of port %u is now %s.\n", port, pStr);
                }
                return;
            }
            pMode++;
        }
    }
    goal_cliPrintf("Unknown Output mode: %s\n", pStr);
}


/****************************************************************************/
/** Show current VLAN input processing mode
 *
 */
static void goal_ethCliVlanInModeShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    GOAL_ETH_CLI_VLANINMODE_T *pMode;           /* VLAN in mode */
    uint32_t port = 0;                          /* Port */
    uint32_t portCount;                         /* Port count */
    const char *pPortStr;                       /* Port string */
    unsigned int portStrLen;                    /* Port string length */
    char *pEndPtr;                              /* End pointer */
    uint32_t inmode;                            /* Input mode */
    uint32_t idx;                               /* Index */
    uint32_t argc = goal_cliParamCount(pData);  /* Argument count */

    if (CMD_MODESHOW_PARAMLEN == argc) {
        res = goal_cliParamGet(pData, CMD_MODESHOW_PARAMLEN - 1, &pPortStr, &portStrLen);
        port = (uint32_t) GOAL_STRTOUL(pPortStr, &pEndPtr, 10);
    }

    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count.\n");
        return;
    }

    if (CMD_MODESHOW_PARAMLEN == argc) {
        if ((portCount + 1) < port) {
            goal_cliPrintf("Invalid port number %u. Max. port number is %u.\n", port, portCount);
            return;
        }
    }

    /* Output single input mode */
    if (CMD_MODESHOW_PARAMLEN == argc) {
        res = goal_ethCmd(GOAL_ETH_CMD_VLAN_MODE_IN, GOAL_FALSE, (portCount == port) ? GOAL_ETH_PORT_HOST : GOAL_ETH_PORT(port), &inmode);
        if (GOAL_RES_ERR(res)) {
            goal_cliPrintf("Getting input mode failed with code: 0x08%"FMT_x32"\n", res);
            return;
        } else {
            pMode = inModes;
            while (NULL != pMode->strMode) {
                if ((GOAL_ETH_VLANIN_MODE_T) inmode == pMode->inmode) {
                    goal_cliPrintf("Input mode of port %u is %s.\n", port, pMode->strMode);
                    return;
                }
                pMode++;
            }
        }
    } else {
        /* Output modes of all ports */
        goal_cliPrintf("  Port       Input Mode\n");
        goal_cliPrintf("===========================\n");
        for (idx = 0; idx < portCount + 1; idx++) {
            res = goal_ethCmd(GOAL_ETH_CMD_VLAN_MODE_IN, GOAL_FALSE, (portCount == idx) ? GOAL_ETH_PORT_HOST : GOAL_ETH_PORT(idx), &inmode);
            if (GOAL_RES_ERR(res)) {
                goal_cliPrintf("Getting input mode failed with code: 0x08%"FMT_x32"\n", res);
                return;
            } else {
                pMode = inModes;
                while (NULL != pMode->strMode) {
                    if ((GOAL_ETH_VLANIN_MODE_T) inmode == pMode->inmode) {
                        goal_cliPrintf("%4u         %s\n", GOAL_ETH_PORT(idx), pMode->strMode);
                        break;
                    }
                    pMode++;
                }
            }
        }
        return;
    }
}


/****************************************************************************/
/** Show current VLAN output processing mode
 *
 */
static void goal_ethCliVlanOutModeShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    GOAL_ETH_CLI_VLANOUTMODE_T *pMode;          /* VLAN in mode */
    uint32_t port = 0;                          /* Port */
    uint32_t portCount;                         /* Port count */
    const char *pPortStr;                       /* Port string */
    unsigned int portStrLen;                    /* Port string length */
    char *pEndPtr;                              /* End pointer */
    uint32_t outmode;                           /* Input mode */
    uint32_t idx;                               /* Index */
    uint32_t argc = goal_cliParamCount(pData);  /* Argument count */

    if (CMD_MODESHOW_PARAMLEN == argc) {
        res = goal_cliParamGet(pData, CMD_MODESHOW_PARAMLEN - 1, &pPortStr, &portStrLen);
        port = (uint32_t) GOAL_STRTOUL(pPortStr, &pEndPtr, 10);
    }

    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count.\n");
        return;
    }

    if (CMD_MODESHOW_PARAMLEN == argc) {
        if ((portCount + 1) < port) {
            goal_cliPrintf("Invalid port number %u. Max. port number is %u.\n", port, portCount);
            return;
        }
    }

    /* Output single input mode */
    if (CMD_MODESHOW_PARAMLEN == argc) {
        res = goal_ethCmd(GOAL_ETH_CMD_VLAN_MODE_OUT, GOAL_FALSE, (portCount == port) ? GOAL_ETH_PORT_HOST : GOAL_ETH_PORT(port), &outmode);
        if (GOAL_RES_ERR(res)) {
            goal_cliPrintf("Getting input mode failed with code: 0x08%"FMT_x32"\n", res);
            return;
        } else {
            pMode = outModes;
            while (NULL != pMode->strMode) {
                if ((GOAL_ETH_VLANOUT_MODE_T) outmode == pMode->outmode) {
                    goal_cliPrintf("Output mode of port %u is %s.\n", port, pMode->strMode);
                    return;
                }
                pMode++;
            }
        }
    } else {
        /* Output modes of all ports */
        goal_cliPrintf("  Port       Output Mode\n");
        goal_cliPrintf("===========================\n");
        for (idx = 0; idx < portCount + 1; idx++) {
            res = goal_ethCmd(GOAL_ETH_CMD_VLAN_MODE_OUT, GOAL_FALSE, (portCount == idx) ? GOAL_ETH_PORT_HOST : GOAL_ETH_PORT(idx), &outmode);
            if (GOAL_RES_ERR(res)) {
                goal_cliPrintf("Getting input mode failed with code: 0x08%"FMT_x32"\n", res);
                return;
            } else {
                pMode = outModes;
                while (NULL != pMode->strMode) {
                    if ((GOAL_ETH_VLANOUT_MODE_T) outmode == pMode->outmode) {
                        goal_cliPrintf("%4u         %s\n", GOAL_ETH_PORT(idx), pMode->strMode);
                        break;
                    }
                    pMode++;
                }
            }
        }
        return;
    }
}


/****************************************************************************/
/** Set VLAN membership of a port
 *
 */
static void goal_ethCliVlanPortAdd(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    uint32_t port;                              /* Port */
    uint32_t portCount;                         /* Port count */
    const char *pPortStr;                       /* Port string */
    unsigned int portStrLen;                    /* Port string length */
    const char *pVlanStr;                       /* Port string */
    unsigned int vlanStrLen;                    /* Port string length */
    char *pEndPtr;                              /* End pointer */
    uint32_t vlan;                              /* VLAN ID */
    uint32_t argc = goal_cliParamCount(pData);  /* Argument count */

    if (CMD_MEMADD_PARAMLEN != argc) {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    /* Get port param and check whether a valid port */
    res = goal_cliParamGet(pData, CMD_MEMADD_PARAMLEN - 2, &pPortStr, &portStrLen);
    port = (uint32_t) GOAL_STRTOUL(pPortStr, &pEndPtr, 10);

    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count.\n");
        return;
    }

    if ((portCount + 1) < port) {
        goal_cliPrintf("Invalid port number %u. Max. port number is %u.\n", port, portCount);
        return;
    }

    /* Get VLAN ID parameter */
    res = goal_cliParamGet(pData, CMD_MEMADD_PARAMLEN - 1, &pVlanStr, &vlanStrLen);
    vlan = (uint32_t) GOAL_STRTOUL(pVlanStr, &pEndPtr, 10);

    if (GOAL_ETH_CLI_VLAN_MAX < vlan) {
        goal_cliPrintf("Not a valid vlan id: %u\n", vlan);
        return;
    }

    /* Set VLAN membership */
    res = goal_ethCmd(GOAL_ETH_CMD_VLAN_PORT_ADD, GOAL_TRUE, (portCount == port) ? GOAL_ETH_PORT_HOST : GOAL_ETH_PORT(port), &vlan);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Setting membership of port %u to vlan id %u failed\n", port, vlan);
    } else {
        goal_cliPrintf("Port %u is now member of vlan id %u\n", port, vlan);
    }
}


/****************************************************************************/
/** Remove VLAN membership of a port
 *
 */
static void goal_ethCliVlanPortRem(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    uint32_t port;                              /* Port */
    uint32_t portCount;                         /* Port count */
    const char *pPortStr;                       /* Port string */
    unsigned int portStrLen;                    /* Port string length */
    const char *pVlanStr;                       /* Port string */
    unsigned int vlanStrLen;                    /* Port string length */
    char *pEndPtr;                              /* End pointer */
    uint32_t vlan;                              /* VLAN ID */
    uint32_t argc = goal_cliParamCount(pData);  /* Argument count */

    if (CMD_MEMADD_PARAMLEN != argc) {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    /* Get port param and check whether a valid port */
    res = goal_cliParamGet(pData, CMD_MEMADD_PARAMLEN - 2, &pPortStr, &portStrLen);
    port = (uint32_t) GOAL_STRTOUL(pPortStr, &pEndPtr, 10);

    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count.\n");
        return;
    }

    if ((portCount + 1) < port) {
        goal_cliPrintf("Invalid port number %u. Max. port number is %u.\n", port, portCount);
        return;
    }

    /* Get VLAN ID parameter */
    res = goal_cliParamGet(pData, CMD_MEMADD_PARAMLEN - 1, &pVlanStr, &vlanStrLen);
    vlan = (uint32_t) GOAL_STRTOUL(pVlanStr, &pEndPtr, 10);

    if (GOAL_ETH_CLI_VLAN_MAX < vlan) {
        goal_cliPrintf("Not a valid vlan id: %u\n", vlan);
        return;
    }

    /* Set VLAN membership */
    res = goal_ethCmd(GOAL_ETH_CMD_VLAN_PORT_REM, GOAL_TRUE, (portCount == port) ? GOAL_ETH_PORT_HOST : GOAL_ETH_PORT(port), &vlan);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Removing membership of port %u from vlan id %u failed\n", port, vlan);
    } else {
        goal_cliPrintf("Port %u is no longer member of vlan id %u\n", port, vlan);
    }
}


/****************************************************************************/
/** Set default VLAN of a port
 *
 */
static void goal_ethCliVlanDefaultSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    uint32_t port;                              /* Port */
    uint32_t portCount;                         /* Port count */
    const char *pPortStr;                       /* Port string */
    unsigned int portStrLen;                    /* Port string length */
    const char *pVlanStr;                       /* Port string */
    unsigned int vlanStrLen;                    /* Port string length */
    const char *pPrioStr;                       /* Port string */
    unsigned int prioStrLen;                    /* Port string length */
    char *pEndPtr;                              /* End pointer */
    uint32_t vlan;                              /* VLAN ID */
    uint32_t prio;                              /* Priority */
    uint32_t tag;                               /* VLAN tag */
    uint32_t argc = goal_cliParamCount(pData);  /* Argument count */

    if (CMD_DEFAULTSET_PARAMLEN != argc) {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    /* Get port param and check whether a valid port */
    res = goal_cliParamGet(pData, CMD_DEFAULTSET_PARAMLEN - 3, &pPortStr, &portStrLen);
    port = (uint32_t) GOAL_STRTOUL(pPortStr, &pEndPtr, 10);

    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count.\n");
        return;
    }

    if ((portCount + 1) < port) {
        goal_cliPrintf("Invalid port number %u. Max. port number is %u.\n", port, portCount);
        return;
    }

    /* Get VLAN ID parameter */
    res = goal_cliParamGet(pData, CMD_DEFAULTSET_PARAMLEN - 2, &pVlanStr, &vlanStrLen);
    vlan = (uint32_t) GOAL_STRTOUL(pVlanStr, &pEndPtr, 10);

    if (GOAL_ETH_CLI_VLAN_MAX < vlan) {
        goal_cliPrintf("Not a valid vlan id: %u\n", vlan);
        return;
    }

    /* Get Priority parameter */
    res = goal_cliParamGet(pData, CMD_DEFAULTSET_PARAMLEN - 1, &pPrioStr, &prioStrLen);
    prio = (uint32_t) GOAL_STRTOUL(pPrioStr, &pEndPtr, 10);

    if (GOAL_ETH_CLI_PRIO_MAX < prio) {
        goal_cliPrintf("Invalid prio. Prio can be 0 - %u.\n", GOAL_ETH_CLI_PRIO_MAX);
        return;
    }

    tag = (prio << GOAL_ETH_CLI_VLAN_PRIO_OFFSET) | (vlan & GOAL_ETH_CLI_VLAN_MASK);

    /* Set default VLAN */
    res = goal_ethCmd(GOAL_ETH_CMD_VLAN_DEF, GOAL_TRUE, (portCount == port) ? GOAL_ETH_PORT_HOST : GOAL_ETH_PORT(port), &tag);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Setting default VLAN of port %u to vlan id %u with prio %u failed\n", port, vlan, prio);
    } else {
        goal_cliPrintf("Setting default VLAN of port %u to vlan id %u with prio %u successful.\n", port, vlan, prio);
    }
}


/****************************************************************************/
/** Show default VLAN of a port
 *
 */
static void goal_ethCliVlanDefaultShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    uint32_t port = 0;                          /* Port */
    uint32_t portCount;                         /* Port count */
    const char *pPortStr;                       /* Port string */
    unsigned int portStrLen;                    /* Port string length */
    char *pEndPtr;                              /* End pointer */
    uint32_t tag = 0;                           /* VLAN tag */
    uint32_t idx;                               /* Index */
    uint32_t argc = goal_cliParamCount(pData);  /* Argument count */

    if (CMD_DEFAULTSHOW_PARAMLEN - 1 > argc) {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count.\n");
        return;
    }

    if (CMD_DEFAULTSHOW_PARAMLEN == argc) {
        /* Get port param and check whether a valid port */
        res = goal_cliParamGet(pData, CMD_DEFAULTSET_PARAMLEN - 3, &pPortStr, &portStrLen);
        port = (uint32_t) GOAL_STRTOUL(pPortStr, &pEndPtr, 10);

        if ((portCount + 1) < port) {
            goal_cliPrintf("Invalid port number %u. Max. port number is %u.\n", port, portCount);
            return;
        }
    }

    /* Show VLAN for a single port */
    if (CMD_DEFAULTSHOW_PARAMLEN == argc) {
        res = goal_ethCmd(GOAL_ETH_CMD_VLAN_DEF, GOAL_FALSE, (portCount == port) ? GOAL_ETH_PORT_HOST : GOAL_ETH_PORT(port), &tag);
        if (GOAL_RES_ERR(res)) {
            goal_cliPrintf("Getting default VLAN of port %u failed.\n", port);
        } else {
            goal_cliPrintf("Default VLAN of port %u is vlan id %u with prio %u\n", port, tag & GOAL_ETH_CLI_VLAN_MASK, tag >> GOAL_ETH_CLI_VLAN_PRIO_OFFSET);
        }
    } else {
        /* Show VLAN for all ports */
        goal_cliPrintf("  Port    VLAN     Prio\n");
        goal_cliPrintf("===========================\n");
        for (idx = 0; idx < (portCount + 1); idx++) {
            tag = 0;
            res = goal_ethCmd(GOAL_ETH_CMD_VLAN_DEF, GOAL_FALSE, (portCount == idx) ? GOAL_ETH_PORT_HOST : GOAL_ETH_PORT(idx), &tag);
            if (GOAL_RES_ERR(res)) {
                goal_cliPrintf("Setting default VLAN of port %u failed.\n", port);
            } else {
                goal_cliPrintf("%4u      %4u     %2u\n", idx, tag & GOAL_ETH_CLI_VLAN_MASK, tag >> GOAL_ETH_CLI_VLAN_PRIO_OFFSET);
            }
        }
    }

}


/****************************************************************************/
/** Show VLAN table
 *
 */
static void goal_ethCliVlanTableShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    uint32_t count;                             /* Table entry count */
    uint32_t portCount;                         /* Port count */
    uint32_t idx;                               /* Index */
    uint32_t cntPort;                           /* port count */
    GOAL_ETH_VLAN_ENTRY_T vlanEntry;            /* VLAN entry */

    UNUSEDARG(pData);

    /* Get number of table entries */
    res = goal_ethCmd(GOAL_ETH_CMD_VLAN_TABLE_CNT, GOAL_FALSE, 0, &count);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve number of VLAN table entries\n");
        return;
    }

    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count.\n");
        return;
    }

    /* Print table header */
    goal_cliPrintf("  VLAN");
    for (idx = 0; idx < portCount; idx++) {
        goal_cliPrintf("  Port %u", idx);
    }
    goal_cliPrintf("  Port %u\n", portCount);

    for (idx = 0; idx < (portCount + 1) * CMD_PORTSTR_LEN + CMD_PORTSTR_OFFSET; idx++) {
        goal_cliPrintf("=");
    }
    goal_cliPrintf("\n");

    /* Retrieve all entries */
    for (idx = 0; idx < count; idx++) {
        vlanEntry.index = idx;
        res = goal_ethCmd(GOAL_ETH_CMD_VLAN_TABLE_GET, GOAL_FALSE, 0, &vlanEntry);
        if (GOAL_RES_ERR(res)) {
            goal_cliPrintf("Could not retrieve VLAN table entry #%u\n", idx);
            return;
        }
        if (GOAL_TRUE == vlanEntry.valid) {
            goal_cliPrintf("  %4u", vlanEntry.vlan);
            for (cntPort = 0; cntPort < portCount; cntPort++) {
                if ((1 << cntPort) & vlanEntry.portMask) {
                    goal_cliPrintf("     X  ");
                } else {
                    goal_cliPrintf("        ");
                }
            }
            if (GOAL_ETH_PORT_BITS & vlanEntry.portMask) {
                goal_cliPrintf("     X");
            }
            goal_cliPrintf("\n");
        }
    }

}


/****************************************************************************/
/** Set VLAN verification of a port
 *
 */
static void goal_ethCliVlanVerifySet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    uint32_t port = 0;                          /* Port */
    uint32_t portCount;                         /* Port count */
    const char *pPortStr;                       /* Port string */
    unsigned int portStrLen;                    /* Port string length */
    const char *pModeStr;                       /* Port string */
    unsigned int modeStrLen;                    /* Port string length */
    char *pEndPtr;                              /* End pointer */
    GOAL_BOOL_T mode;                           /* Mode */
    uint32_t argc = goal_cliParamCount(pData);  /* Argument count */

    if (CMD_VERIFYSET_PARAMLEN != argc) {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count.\n");
        return;
    }

    /* Get port param and check whether a valid port */
    res = goal_cliParamGet(pData, CMD_VERIFYSET_PARAMLEN - 2, &pPortStr, &portStrLen);
    port = (uint32_t) GOAL_STRTOUL(pPortStr, &pEndPtr, 10);

    if ((portCount + 1) < port) {
        goal_cliPrintf("Invalid port number %u. Max. port number is %u.\n", port, portCount);
        return;
    }

    /* Get mode param */
    res = goal_cliParamGet(pData, CMD_VERIFYSET_PARAMLEN - 1, &pModeStr, &modeStrLen);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    /* Set mode */
    if (CMD_PARAM_ON_LEN == modeStrLen && !strcmp(pModeStr, CMD_PARAM_ON)) {
        mode = GOAL_TRUE;
    } else if (CMD_PARAM_OFF_LEN == modeStrLen && !strcmp(pModeStr, CMD_PARAM_OFF)) {
        mode = GOAL_FALSE;
    } else {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    res = goal_ethCmd(GOAL_ETH_CMD_VLAN_VERIFY, GOAL_TRUE, port, &mode);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Setting VLAN verification for port %s to %s failed.\n", pPortStr, pModeStr);
    } else {
        goal_cliPrintf("VLAN verification for port %s is now %s.\n", pPortStr, pModeStr);
    }
}


/****************************************************************************/
/** Show VLAN verification of a port
 *
 */
static void goal_ethCliVlanVerifyShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    uint32_t port = 0;                          /* Port */
    uint32_t portCount;                         /* Port count */
    const char *pPortStr = NULL;                /* Port string */
    unsigned int portStrLen;                    /* Port string length */
    char *pEndPtr;                              /* End pointer */
    GOAL_BOOL_T mode;                           /* Mode */
    uint32_t idx;                               /* Index */
    uint32_t argc = goal_cliParamCount(pData);  /* Argument count */

    if ((CMD_VERIFYSHOW_PARAMLEN != argc) && ((CMD_VERIFYSHOW_PARAMLEN - 1) != argc)) {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count.\n");
        return;
    }

    /* dump mode for all ports if port parameter is missing */
    if (CMD_VERIFYSHOW_PARAMLEN != argc) {
        goal_cliPrintf(" Port    Mode \n");
        goal_cliPrintf("===============\n");

        for (idx = 0; idx < (portCount + 1); idx++) {
            res = goal_ethCmd(GOAL_ETH_CMD_VLAN_VERIFY, GOAL_FALSE, (portCount == idx) ? GOAL_ETH_PORT_HOST : GOAL_ETH_PORT(idx), &mode);
            if (GOAL_RES_ERR(res)) {
                goal_cliPrintf("Getting VLAN verification for port %u failed.\n", idx);
            } else {
                goal_cliPrintf("%3u       %s\n", idx, (GOAL_TRUE == mode) ? "on" : "off");
            }
        }

        return;
    }

    /* Get port param and check whether a valid port */
    res = goal_cliParamGet(pData, CMD_VERIFYSHOW_PARAMLEN - 1, &pPortStr, &portStrLen);
    port = (uint32_t) GOAL_STRTOUL(pPortStr, &pEndPtr, 10);

    if ((portCount + 1) < port) {
        goal_cliPrintf("Invalid port number %u. Max. port number is %u.\n", port, portCount);
        return;
    }

    res = goal_ethCmd(GOAL_ETH_CMD_VLAN_VERIFY, GOAL_FALSE, port, &mode);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Getting VLAN verification for port %u failed.\n", pPortStr);
    } else {
        goal_cliPrintf("VLAN verification for port %s is %s.\n", pPortStr, (GOAL_TRUE == mode) ? "on" : "off");
    }
}


/****************************************************************************/
/** Set VLAN discartion of unknown frames of a port
 *
 */
static void goal_ethCliVlanDiscunkSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    uint32_t port;                              /* Port */
    uint32_t portCount;                         /* Port count */
    const char *pPortStr;                       /* Port string */
    unsigned int portStrLen;                    /* Port string length */
    const char *pModeStr;                       /* Port string */
    unsigned int modeStrLen;                    /* Port string length */
    char *pEndPtr;                              /* End pointer */
    GOAL_BOOL_T mode;                           /* Mode */
    uint32_t argc = goal_cliParamCount(pData);  /* Argument count */

    if (CMD_VERIFYSET_PARAMLEN != argc) {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count.\n");
        return;
    }

    /* Get port param and check whether a valid port */
    res = goal_cliParamGet(pData, CMD_VERIFYSET_PARAMLEN - 2, &pPortStr, &portStrLen);
    port = (uint32_t) GOAL_STRTOUL(pPortStr, &pEndPtr, 10);

    if ((portCount + 1) < port) {
        goal_cliPrintf("Invalid port number %u. Max. port number is %u.\n", port, portCount);
        return;
    }

    /* Get mode param */
    res = goal_cliParamGet(pData, CMD_VERIFYSET_PARAMLEN - 1, &pModeStr, &modeStrLen);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    /* Set mode */
    if (CMD_PARAM_ON_LEN == modeStrLen && !strcmp(pModeStr, CMD_PARAM_ON)) {
        mode = GOAL_TRUE;
    } else if (CMD_PARAM_OFF_LEN == modeStrLen && !strcmp(pModeStr, CMD_PARAM_OFF)) {
        mode = GOAL_FALSE;
    } else {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    res = goal_ethCmd(GOAL_ETH_CMD_VLAN_DISCUNK, GOAL_TRUE, port, &mode);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Setting VLAN discartion for port %s to %s failed.\n", pPortStr, pModeStr);
    } else {
        goal_cliPrintf("VLAN discartion for port %s is now %s.\n", pPortStr, pModeStr);
    }
}


/****************************************************************************/
/** Show VLAN discartion of a port
 *
 */
static void goal_ethCliVlanDiscunkShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    uint32_t port = 0;                          /* Port */
    uint32_t portCount;                         /* Port count */
    const char *pPortStr = NULL;                /* Port string */
    unsigned int portStrLen;                    /* Port string length */
    char *pEndPtr;                              /* End pointer */
    GOAL_BOOL_T mode;                           /* Mode */
    uint32_t idx;                               /* Index */
    uint32_t argc = goal_cliParamCount(pData);  /* Argument count */

    if ((CMD_DISCUNKNOWN_PARAMLEN != argc) && ((CMD_DISCUNKNOWN_PARAMLEN - 1) != argc)) {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count.\n");
        return;
    }

    if (CMD_DISCUNKNOWN_PARAMLEN == argc) {
        /* Get port param and check whether a valid port */
        res = goal_cliParamGet(pData, CMD_DISCUNKNOWN_PARAMLEN - 1, &pPortStr, &portStrLen);
        port = (uint32_t) GOAL_STRTOUL(pPortStr, &pEndPtr, 10);

        if ((portCount + 1) < port) {
            goal_cliPrintf("Invalid port number %u. Max. port number is %u.\n", port, portCount);
            return;
        }
    }

    if (CMD_DISCUNKNOWN_PARAMLEN == argc) {
        res = goal_ethCmd(GOAL_ETH_CMD_VLAN_DISCUNK, GOAL_FALSE, port, &mode);
        if (GOAL_RES_ERR(res)) {
            goal_cliPrintf("Getting VLAN discartion for port %u failed.\n", pPortStr);
        } else {
            goal_cliPrintf("VLAN discartion for port %s is %s.\n", pPortStr, (GOAL_TRUE == mode) ? "on" : "off");
        }
    } else {
        goal_cliPrintf(" Port    Mode \n");
        goal_cliPrintf("===============\n");
        for (idx = 0; idx < (portCount + 1); idx++) {
            res = goal_ethCmd(GOAL_ETH_CMD_VLAN_DISCUNK, GOAL_FALSE, (portCount == idx) ? GOAL_ETH_PORT_HOST : GOAL_ETH_PORT(idx), &mode);
            if (GOAL_RES_ERR(res)) {
                goal_cliPrintf("Getting VLAN discartion for port %u failed.\n", idx);
            } else {
                goal_cliPrintf("%3u       %s\n", idx, (GOAL_TRUE == mode) ? "on" : "off");
            }
        }
    }
}


/****************************************************************************/
/** VLAN command execution handler
 */
static void goal_ethCliVlan(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    unsigned int strLen;                        /* Param length */
    const char *pStr = NULL;                    /* Param string pointer */
    uint32_t argc = goal_cliParamCount(pData);  /* Argument count */

    if (CMD_VLAN_PARAMLEN > argc) {
        goal_cliPrintf("Invalid syntax\n");
        goal_ethCliPrintHelp();
        return;
    }

    res = goal_cliParamGet(pData, 2, &pStr, &strLen);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Syntax error");
        return;
    }
    if (ethStrMode == pStr) {

        if ((CMD_VLAN_PARAMLEN + 1) < argc) {
            res = goal_cliParamGet(pData, 3, &pStr, &strLen);
            /* eth vlan mode in */
            if (ethStrIn == pStr) {
                res = goal_cliParamGet(pData, 4, &pStr, &strLen);
                if (ethStrSet == pStr) {
                    goal_ethCliVlanInModeSet(pData);
                    return;
                } else if (ethStrShow == pStr) {
                    goal_ethCliVlanInModeShow(pData);
                    return;
                }
            /* eth vlan mode out */
            } else if (ethStrOut == pStr) {
                res = goal_cliParamGet(pData, 4, &pStr, &strLen);
                if (ethStrSet == pStr) {
                    goal_ethCliVlanOutModeSet(pData);
                    return;
                } else if (ethStrShow == pStr) {
                    goal_ethCliVlanOutModeShow(pData);
                    return;
                }
            }
        }
    } else if (ethStrPort == pStr) {
        if (CMD_VLAN_PARAMLEN < argc) {
            res = goal_cliParamGet(pData, 3, &pStr, &strLen);
            /* eth vlan port add */
            if (ethStrAdd == pStr) {
                goal_ethCliVlanPortAdd(pData);
                return;
            /* eth vlan port rem */
            } else if (ethStrRem == pStr) {
                goal_ethCliVlanPortRem(pData);
                return;
            }
        }
    } else if (ethStrDef == pStr) {
        if (CMD_VLAN_PARAMLEN < argc) {
            res = goal_cliParamGet(pData, 3, &pStr, &strLen);
            /* eth vlan default set */
            if (ethStrSet == pStr) {
                goal_ethCliVlanDefaultSet(pData);
                return;
            /* eth vlan default show */
            } else if (ethStrShow == pStr) {
                goal_ethCliVlanDefaultShow(pData);
                return;
            }
        }
    } else if (ethStrTable == pStr) {
        if (CMD_VLAN_PARAMLEN < argc) {
            res = goal_cliParamGet(pData, 3, &pStr, &strLen);
            /* eth vlan table show */
            if (ethStrShow == pStr) {
                goal_ethCliVlanTableShow(pData);
                return;
            }
        }
    } else if (ethStrVerify == pStr) {
        if (CMD_VLAN_PARAMLEN < argc) {
            res = goal_cliParamGet(pData, 3, &pStr, &strLen);
            if (ethStrSet == pStr) {
                goal_ethCliVlanVerifySet(pData);
                return;
            } else if (ethStrShow == pStr) {
                goal_ethCliVlanVerifyShow(pData);
                return;
            }
        }
    } else if (ethStrDisc == pStr) {
        if (CMD_VLAN_PARAMLEN < argc) {
            res = goal_cliParamGet(pData, 3, &pStr, &strLen);
            if (ethStrSet == pStr) {
                goal_ethCliVlanDiscunkSet(pData);
                return;
            } else if (ethStrShow == pStr) {
                goal_ethCliVlanDiscunkShow(pData);
                return;
            }
        }
    }
    goal_cliPrintf("Unknown command\n");
}


/****************************************************************************/
/** MAC table config set command execution handler
 */
static void goal_ethCliMactabConfSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    unsigned int strLen;                        /* Param length */
    const char *pStr = NULL;                    /* Param string pointer */
    unsigned int strStateLen;                   /* Param length */
    GOAL_ETH_MACTAB_CONF_T macConf;             /* MAC table config */
    GOAL_BOOL_T mode;                           /* Mode */
    const char *pStrState = NULL;               /* Param string pointer */

    res = goal_cliParamGet(pData, 4, &pStr, &strLen);
    if (GOAL_RES_OK(res)) {
        res = goal_cliParamGet(pData, 5, &pStrState, &strStateLen);

        /* Set mode */
        if (CMD_PARAM_ON_LEN == strStateLen && !strcmp(pStrState, CMD_PARAM_ON)) {
            mode = GOAL_TRUE;
        } else if (CMD_PARAM_OFF_LEN == strStateLen && !strcmp(pStrState, CMD_PARAM_OFF)) {
            mode = GOAL_FALSE;
        } else {
            goal_cliPrintf("Syntax error\n");
            return;
        }

        /* Retrieve current settings */
        res = goal_ethCmd(GOAL_ETH_CMD_MACTAB_CONF, GOAL_FALSE, 0, &macConf);
        if (GOAL_RES_ERR(res)) {
            goal_cliPrintf("Could not retrieve current MAC table config settings\n");
            return;
        }

        /* Update requested setting */
        if (CMD_MACTAB_CONF_LEARNING_LEN == strLen && !strncmp(pStr, CMD_MACTAB_CONF_LEARNING, CMD_MACTAB_CONF_LEARNING_LEN)) {
            macConf.learn = mode;
        } else if (CMD_MACTAB_CONF_AGEING_LEN == strLen && !strncmp(pStr, CMD_MACTAB_CONF_AGEING, CMD_MACTAB_CONF_AGEING_LEN)) {
            macConf.ageing = mode;
        } else if (CMD_MACTAB_CONF_MIGRATION_LEN == strLen && !strncmp(pStr, CMD_MACTAB_CONF_MIGRATION, CMD_MACTAB_CONF_MIGRATION_LEN)) {
            macConf.migration = mode;
        } else if (CMD_MACTAB_CONF_DISCUNKNOWN_LEN == strLen && !strncmp(pStr, CMD_MACTAB_CONF_DISCUNKNOWN, CMD_MACTAB_CONF_DISCUNKNOWN_LEN)) {
            macConf.discunknown = mode;
        } else if (CMD_MACTAB_CONF_PERVLAN_LEN == strLen && !strncmp(pStr, CMD_MACTAB_CONF_PERVLAN, CMD_MACTAB_CONF_PERVLAN_LEN)) {
            macConf.pervlan = mode;
        } else {
            goal_cliPrintf("Syntax error\n");
            return;
        }

        /* Update settings */
        res = goal_ethCmd(GOAL_ETH_CMD_MACTAB_CONF, GOAL_TRUE, 0, &macConf);
        if (GOAL_RES_ERR(res)) {
            goal_cliPrintf("Could update MAC table config settings\n");
            return;
        }
        goal_cliPrintf("MAC table feature \"%.*s\" now set to %s\n", strLen, pStr, pStrState);
        return;
    }

    goal_cliPrintf("Syntax error\n");
}


/****************************************************************************/
/** MAC table config show command execution handler
 */
static void goal_ethCliMactabConfShow(
    void
)
{
    GOAL_STATUS_T res;                          /* Result */
    GOAL_ETH_MACTAB_CONF_T macConf;             /* MAC table config */

    /* Retrieve current settings */
    res = goal_ethCmd(GOAL_ETH_CMD_MACTAB_CONF, GOAL_FALSE, 0, &macConf);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve current MAC table config settings\n");
        return;
    }

    /* Show current settings */
    goal_cliPrintf("  Feature      State\n");
    goal_cliPrintf("======================\n");
    goal_cliPrintf(" %-15s%s\n", CMD_MACTAB_CONF_LEARNING, (macConf.learn) ? "on" : "off");
    goal_cliPrintf(" %-15s%s\n", CMD_MACTAB_CONF_AGEING, (macConf.ageing) ? "on" : "off");
    goal_cliPrintf(" %-15s%s\n", CMD_MACTAB_CONF_MIGRATION, (macConf.migration) ? "on" : "off");
    goal_cliPrintf(" %-15s%s\n", CMD_MACTAB_CONF_DISCUNKNOWN, (macConf.discunknown) ? "on" : "off");
    goal_cliPrintf(" %-15s%s\n", CMD_MACTAB_CONF_PERVLAN, (macConf.pervlan) ? "on" : "off");
}


/****************************************************************************/
/** Convert MAC address string to MAC address
 *
 * @returns GOAL_OK if conversion was successfull, otherwise error code
 */
static GOAL_STATUS_T goal_ethCliParamAsMac(
    const char *pParam,                         /**< parameter */
    const unsigned int lenParam,                /**< parameter length */
    GOAL_ETH_MAC_ADDR_T *pMac                   /**< MAC address buffer */
)
{
    unsigned int cntElem;                       /* MAC address element count */
    const char *pPosParam;                      /* parameter position */
    char *pPosParamSep;                         /* separator position */
    const char *pPosParamEnd;                   /* parameter end position */

    /* reset errno */
    errno = 0;

    /* initialize positions */
    pPosParam = pParam;
    pPosParamEnd = pParam + lenParam;

    for (cntElem = 0; (pPosParam < pPosParamEnd); cntElem++) {

        /* convert MAC address element */
        (*pMac)[cntElem] = (uint8_t) GOAL_STRTOUL(pPosParam, &pPosParamSep, 16);

        /* skip colon */
        pPosParam = pPosParamSep + 1;
    }

    /* check if MAC address was parsed correctly */
    if (MAC_ADDR_LEN != cntElem) {
        return GOAL_ERR_PARAM;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** MAC table MAC address set command execution handler
 */
static void goal_ethCliMactabMacSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    unsigned int strLen;                        /* Param length */
    const char *pStr = NULL;                    /* Param string pointer */
    unsigned int portStrLen;                    /* Param length */
    const char *pPortStr = NULL;                /* Param string pointer */
    GOAL_ETH_MAC_ADDR_T macAddr;                /* MAC address */
    GOAL_ETH_MACTAB_ENTRY_T macTabEntry;        /* MAC table entry */
    uint32_t portCount;                         /* port count */
    char *pEndPtr;                              /* End pointer */
    uint32_t port = 0;                          /* Port */

    /* Get port count */
    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count\n");
        return;
    }

    /* Get MAC address param */
    res = goal_cliParamGet(pData, 4, &pStr, &strLen);
    if (GOAL_RES_OK(res)) {
        res = goal_ethCliParamAsMac(pStr, strLen, &macAddr);
        if (GOAL_RES_ERR(res)) {
            goal_cliPrintf("Invalid MAC address\n");
            return;
        }
    }

    if (GOAL_RES_OK(res)) {
        /* Get port param and check whether a valid port */
        res = goal_cliParamGet(pData, 5, &pPortStr, &portStrLen);
        port = (uint32_t) GOAL_STRTOUL(pPortStr, &pEndPtr, 10);

        if (portCount < port) {
            goal_cliPrintf("Invalid port number %u. Max. port number is %u.\n", port, portCount);
            return;
        }
    }

    if (GOAL_RES_OK(res)) {

        macTabEntry.addr = &macAddr;
        res = goal_ethCmd(GOAL_ETH_CMD_MACTAB_GET, GOAL_FALSE, (port != portCount) ? GOAL_ETH_PORT(port) : GOAL_ETH_PORT_HOST, &macTabEntry);

        /* If we get the GOAL_ERR_PARAM error code it simply means it is not contained in table yet */
        if (GOAL_RES_ERR(res)) {
            if (GOAL_ERR_PARAM == res) {
                macTabEntry.portmask = GOAL_ETH_PORT_MASK;
            } else {
                goal_cliPrintf("Could not retrieve MAC addr table entry\n");
                return;
            }
        }

        /* Update port mask */
        macTabEntry.portmask |= (port == portCount) ? GOAL_ETH_PORT_HOST : GOAL_ETH_PORT_BIT(port);
        res = goal_ethCmd(GOAL_ETH_CMD_MACTAB_SET, GOAL_TRUE, (port != portCount) ? GOAL_ETH_PORT(port) : GOAL_ETH_PORT_HOST, &macTabEntry);
        if (GOAL_RES_OK(res)) {
            goal_cliPrintf("Added port %u to MAC address %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n", port, macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
        } else {
            goal_cliPrintf("Failed to add port %u to MAC address %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n", port, macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
        }
        return;
    }

    goal_cliPrintf("Syntax error\n");
}


/****************************************************************************/
/** MAC table MAC address remove command execution handler
 */
static void goal_ethCliMactabMacRem(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    unsigned int strLen;                        /* Param length */
    const char *pStr = NULL;                    /* Param string pointer */
    unsigned int portStrLen;                    /* Param length */
    const char *pPortStr = NULL;                /* Param string pointer */
    GOAL_ETH_MAC_ADDR_T macAddr;                /* MAC address */
    GOAL_ETH_MACTAB_ENTRY_T macTabEntry;        /* MAC table entry */
    uint32_t portCount;                         /* port count */
    char *pEndPtr;                              /* End pointer */
    uint32_t port = 0;                          /* Port */

    /* Get port count */
    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count\n");
        return;
    }

    /* Get MAC address param */
    res = goal_cliParamGet(pData, 4, &pStr, &strLen);
    if (GOAL_RES_OK(res)) {
        res = goal_ethCliParamAsMac(pStr, strLen, &macAddr);
        if (GOAL_RES_ERR(res)) {
            goal_cliPrintf("Invalid MAC address\n");
            return;
        }
    }

    if (GOAL_RES_OK(res)) {
        /* Get port param and check whether a valid port */
        res = goal_cliParamGet(pData, 5, &pPortStr, &portStrLen);
        port = (uint32_t) GOAL_STRTOUL(pPortStr, &pEndPtr, 10);

        if (portCount < port) {
            goal_cliPrintf("Invalid port number %u. Max. port number is %u.\n", port, portCount);
            return;
        }
    }

    if (GOAL_RES_OK(res)) {
        macTabEntry.addr = &macAddr;
        res = goal_ethCmd(GOAL_ETH_CMD_MACTAB_GET, GOAL_FALSE, (port != portCount) ? GOAL_ETH_PORT(port) : GOAL_ETH_PORT_HOST, &macTabEntry);

        /* Get the MAC address entry from MAC table */
        if (GOAL_RES_ERR(res)) {
            goal_cliPrintf("MAC address %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx is not in MAC table\n", port, macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
            return;
        }

        /* Update port mask */
        macTabEntry.portmask &= ~((port == portCount) ? GOAL_ETH_PORT_HOST : (GOAL_ETH_PORT_BIT(port) & ~GOAL_ETH_PORT_MASK));
        res = goal_ethCmd(GOAL_ETH_CMD_MACTAB_SET, GOAL_TRUE, (port != portCount) ? GOAL_ETH_PORT(port) : GOAL_ETH_PORT_HOST, &macTabEntry);
        if (GOAL_RES_OK(res)) {
            goal_cliPrintf("Removed port %u from MAC address %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n", port, macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
        } else {
            goal_cliPrintf("Failed to remove port %u from MAC address %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n", port, macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
        }
        return;
    }

    goal_cliPrintf("Syntax error\n");
}


/****************************************************************************/
/** MAC table MAC address clear command execution handler
 */
static void goal_ethCliMactabMacClr(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    unsigned int strLen;                        /* Param length */
    const char *pStr = NULL;                    /* Param string pointer */
    GOAL_ETH_MACTAB_CLR_MODES_T mode;           /* Mac table clear mode */

    /* Get param */
    res = goal_cliParamGet(pData, 4, &pStr, &strLen);
    if (!GOAL_RES_OK(res)) {
        goal_cliPrintf("Missing clear mode.\n");
    }

    if (pStr == ethStrAll) {
        mode = GOAL_ETH_MACTAB_CLR_ALL;
    }
    else if (pStr == ethStrDynamic) {
        mode = GOAL_ETH_MACTAB_CLR_DYNAMIC;
    }
    else if (pStr == ethStrStatic) {
        mode = GOAL_ETH_MACTAB_CLR_STATIC;
    }
    else {
        goal_cliPrintf("Unknown Mac table clear command.\n");
        return;
    }

    res = goal_ethCmd(GOAL_ETH_CMD_MACTAB_CLR, GOAL_TRUE, GOAL_ETH_PORT_HOST, &mode);
    if (GOAL_RES_OK(res)) {
        goal_cliPrintf("Cleared %s mac table.\n", (mode == GOAL_ETH_MACTAB_CLR_ALL) ? "complete" : (mode == GOAL_ETH_MACTAB_CLR_DYNAMIC) ? "dynamic" : "static");
    }
    else {
        goal_cliPrintf("Failed clearing %s mac table.\n", (mode == GOAL_ETH_MACTAB_CLR_ALL) ? "complete" : (mode == GOAL_ETH_MACTAB_CLR_DYNAMIC) ? "dynamic" : "static");
    }
}


/****************************************************************************/
/** MAC table MAC address show command execution handler
 */
static void goal_ethCliMactabMacShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    unsigned int strLen;                        /* Param length */
    const char *pStr = NULL;                    /* Param string pointer */
    GOAL_ETH_MACTAB_ENTRY_T macTabEntry;        /* MAC table entry */
    GOAL_ETH_MAC_ADDR_T macAddr;                /* MAC address */
    uint32_t portCount;                         /* port count */
    uint32_t idx;                               /* index */

    /* Get port count */
    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count\n");
        return;
    }

    /* Get MAC address param */
    res = goal_cliParamGet(pData, 4, &pStr, &strLen);
    if (GOAL_RES_OK(res)) {
        res = goal_ethCliParamAsMac(pStr, strLen, &macAddr);
        if (GOAL_RES_ERR(res)) {
            goal_cliPrintf("Invalid MAC address\n");
            return;
        }
    }

    /* Show the ports for this mac address */
    if (GOAL_RES_OK(res)) {
        macTabEntry.addr = &macAddr;
        res = goal_ethCmd(GOAL_ETH_CMD_MACTAB_GET, GOAL_FALSE, 0, &macTabEntry);
        if (GOAL_RES_ERR(res)) {
            goal_cliPrintf("MAC address %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx is not in the MAC table.\n", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
            return;
        } else {
            goal_cliPrintf("MAC address %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx is forwarded to ports:\n", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
            if (!macTabEntry.portmask) {
                goal_cliPrintf("None\n");
            } else {
                for (idx = 0; idx < portCount; idx++) {
                    if (macTabEntry.portmask & (~ GOAL_ETH_PORT_MASK) & GOAL_ETH_PORT_BIT(idx)) {
                        goal_cliPrintf("Port %u\n", GOAL_ETH_PORT(idx));
                    }
                }
                if (macTabEntry.portmask & GOAL_ETH_PORT_HOST) {
                    goal_cliPrintf("Host Port\n");
                }
            }
            return;
        }
    }

    goal_cliPrintf("Syntax error\n");
}


/****************************************************************************/
/** MAC table command execution handler
 */
static void goal_ethCliMactab(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    unsigned int strLen;                        /* Param length */
    const char *pStr = NULL;                    /* Param string pointer */

    res = goal_cliParamGet(pData, 2, &pStr, &strLen);
    if (GOAL_RES_OK(res)) {
        if (pStr == ethStrConf) {
            res = goal_cliParamGet(pData, 3, &pStr, &strLen);
            if (GOAL_RES_OK(res)) {
                if (pStr == ethStrSet) {
                    goal_ethCliMactabConfSet(pData);
                    return;
                } else if (pStr == ethStrShow) {
                    goal_ethCliMactabConfShow();
                    return;
                }
            }
        } else if (pStr == ethStrMac) {
            res = goal_cliParamGet(pData, 3, &pStr, &strLen);
            if (GOAL_RES_OK(res)) {
                if (pStr == ethStrAdd) {
                    goal_ethCliMactabMacSet(pData);
                    return;
                } else if (pStr == ethStrRem) {
                    goal_ethCliMactabMacRem(pData);
                    return;
                } else if (pStr == ethStrShow) {
                    goal_ethCliMactabMacShow(pData);
                    return;
                } else if (pStr == ethStrClear) {
                    goal_ethCliMactabMacClr(pData);
                    return;
                }
            }
        }
    }

    goal_cliPrintf("Syntax error\n");
}


/****************************************************************************/
/** TX rate set command execution handler
 */
static void goal_ethCliDosTxrateSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    unsigned int strLen;                        /* Param length */
    const char *pStr = NULL;                    /* Param string pointer */
    unsigned int portStrLen;                    /* Param length */
    const char *pPortStr = NULL;                /* Param string pointer */
    uint32_t port;                              /* Port */
    float limit;                                /* Limit */
    uint32_t portCount;                         /* Port count */
    char *pEndPtr;                              /* End pointer */

    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count.\n");
        return;
    }

    /* Get port param and check whether a valid port */
    res = goal_cliParamGet(pData, 4, &pPortStr, &portStrLen);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    port = (uint32_t) GOAL_STRTOUL(pPortStr, &pEndPtr, 10);
    if (pEndPtr != (pPortStr + portStrLen)) {
        goal_cliPrintf("Invalid port format\n");
        return;
    }

    if (portCount < port) {
        goal_cliPrintf("Invalid port number %u. Max. port number is %u.\n", port, portCount);
        return;
    }

    /* Get rate limit parameter */
    res = goal_cliParamGet(pData, 5, &pStr, &strLen);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    limit = (float) GOAL_STRTOUL(pStr, &pEndPtr, 10);
    if (pEndPtr != (pStr + strLen)) {
        goal_cliPrintf("Invalid limit format\n");
        return;
    }

    if (CMD_TXRATE_LIMIT_MAX < limit) {
        goal_cliPrintf("TX rate limit valid range 0 - %f\n", CMD_TXRATE_LIMIT_MAX);
        return;
    }

    /* Update rate limiting */
    res = goal_ethCmd(GOAL_ETH_CMD_TXRATE, GOAL_TRUE, (port != portCount) ? GOAL_ETH_PORT(port) : GOAL_ETH_PORT_HOST, &limit);
    if (GOAL_RES_OK(res)) {
        goal_cliPrintf("TX rate limit now set to %u\n", (uint32_t) limit);
    } else {
        goal_cliPrintf("Failed to update tx rate limiting\n");
    }

}


/****************************************************************************/
/** TX rate show command execution handler
 */
static void goal_ethCliDosTxrateShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    unsigned int portStrLen;                    /* Param length */
    const char *pPortStr = NULL;                /* Param string pointer */
    uint32_t port = 0;                          /* Port */
    GOAL_BOOL_T showAll = GOAL_TRUE;            /* Limit */
    uint32_t portCount;                         /* Port count */
    char *pEndPtr;                              /* End pointer */
    float limit;                                /* Rate limit */
    uint32_t idx;                               /* Index */

    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count.\n");
        return;
    }

    /* Get port param and check whether a valid port */
    res = goal_cliParamGet(pData, 4, &pPortStr, &portStrLen);
    if (GOAL_RES_OK(res)) {
        port = (uint32_t) GOAL_STRTOUL(pPortStr, &pEndPtr, 10);
        if (pEndPtr != (pPortStr + portStrLen)) {
            goal_cliPrintf("Invalid port format\n");
            return;
        }

        if (portCount < port) {
            goal_cliPrintf("Invalid port number %u. Max. port number is %u.\n", port, portCount);
            return;
        }

        showAll = GOAL_FALSE;
    }

    if (!showAll) {
        /* Update rate limiting */
        res = goal_ethCmd(GOAL_ETH_CMD_TXRATE, GOAL_FALSE, (port != portCount) ? GOAL_ETH_PORT(port) : GOAL_ETH_PORT_HOST, &limit);
        if (GOAL_RES_OK(res)) {
            goal_cliPrintf("TX rate limit is set to %u\n", (uint32_t) limit);
        } else {
            goal_cliPrintf("Failed to update tx rate limiting\n");
        }
    } else {
        goal_cliPrintf("  Port    TX rate limit\n");
        goal_cliPrintf("=========================\n");
        for (idx = 0; idx < portCount + 1; idx++) {
            res = goal_ethCmd(GOAL_ETH_CMD_TXRATE, GOAL_FALSE, (idx != portCount) ? GOAL_ETH_PORT(idx) : GOAL_ETH_PORT_HOST, &limit);
            if (GOAL_RES_ERR(res)) {
                goal_cliPrintf("Failed to get rate limit for port %u%\n", idx);
                return;
            }
            goal_cliPrintf("   %2u      %3u\n", idx, (uint32_t) limit);
        }
    }

}


/****************************************************************************/
/** Time base set command execution handler
 */
static void goal_ethCliDosTimeframeSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    unsigned int strLen;                        /* Param length */
    const char *pStr = NULL;                    /* Param string pointer */
    char *pEndPtr;                              /* End pointer */
    uint16_t timebase;                          /* Time base in ms */

    /* Get time base param */
    res = goal_cliParamGet(pData, 4, &pStr, &strLen);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    /* Parse and check time base param */
    errno = 0;
    timebase = (uint16_t) GOAL_STRTOUL(pStr, &pEndPtr, 0);
    if (pEndPtr != (pStr + strLen) || ERANGE == errno) {
        goal_cliPrintf("Invalid timebase format\n");
        return;
    }

    /* Set the time base */
    res = goal_ethCmd(GOAL_ETH_CMD_STORMTIME, GOAL_TRUE, 0, &timebase);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Failed to retrieve storm time.\n");
        return;
    }

    if (timebase) {
        goal_cliPrintf("Timebase for broadcast/mcast storm protection set to %u ms.\n", timebase);
    } else {
        goal_cliPrintf("Broadcast/mcast storm protection now disabled (timebase == 0).\n");
    }
}


/****************************************************************************/
/** Time base show command execution handler
 */
static void goal_ethCliDosTimeframeShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    uint16_t timebase;                          /* Time base in ms */

    UNUSEDARG(pData);

    /* Get the time base */
    res = goal_ethCmd(GOAL_ETH_CMD_STORMTIME, GOAL_FALSE, 0, &timebase);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Failed to retrieve storm time.\n");
        return;
    }
    if (timebase) {
        goal_cliPrintf("Timebase for broadcast/mcast storm protection is %u ms.\n", timebase);
    } else {
        goal_cliPrintf("Broadcast/mcast storm protection disabled (timebase == 0).\n");
    }
}


/****************************************************************************/
/** Mlimit set command execution handler
 */
static void goal_ethCliDosMlimitSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    unsigned int strLen;                        /* Param length */
    const char *pStr = NULL;                    /* Param string pointer */
    char *pEndPtr;                              /* End pointer */
    uint16_t mlimit;                            /* Multicast limit in frames */
    uint16_t timebase;                          /* Time base */

    /* Get mlimit param */
    res = goal_cliParamGet(pData, 4, &pStr, &strLen);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    /* Parse and check mlimit param */
    errno = 0;
    mlimit = (uint16_t) GOAL_STRTOUL(pStr, &pEndPtr, 0);
    if (pEndPtr != (pStr + strLen) || ERANGE == errno) {
        goal_cliPrintf("Invalid timebase format\n");
        return;
    }

    /* Set the mlimit */
    res = goal_ethCmd(GOAL_ETH_CMD_MCAST_LIMIT, GOAL_TRUE, 0, &mlimit);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Failed to set mcast limit.\n");
        return;
    }

    /* Get the timebase */
    res = goal_ethCmd(GOAL_ETH_CMD_STORMTIME, GOAL_FALSE, 0, &timebase);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Failed to retrieve timebase.\n");
        return;
    }

    /* Show the result */
    if (mlimit) {
        if (timebase) {
            goal_cliPrintf("Limit of mcast frames per port set to %u frames/%u ms.\n", mlimit, timebase);
        } else {
            goal_cliPrintf("Limit of mcast frames per port set to %u frames.\n", mlimit);
            goal_cliPrintf("Mcast limiting still disabled as timebase is set to 0.\n");
        }
    } else {
        goal_cliPrintf("Mcast storm protection now disabled (mlimit == 0).\n");
    }
}


/****************************************************************************/
/** Mlimit show command execution handler
 */
static void goal_ethCliDosMlimitShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    uint16_t mlimit;                            /* Multicast limit in frames */
    uint16_t timebase;                          /* Time base */

    UNUSEDARG(pData);

    /* Get the mlimit */
    res = goal_ethCmd(GOAL_ETH_CMD_MCAST_LIMIT, GOAL_FALSE, 0, &mlimit);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Failed to retrieve multicast frame limit.\n");
        return;
    }

    /* Get the timebase */
    res = goal_ethCmd(GOAL_ETH_CMD_STORMTIME, GOAL_FALSE, 0, &timebase);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Failed to retrieve timebase.\n");
        return;
    }

    /* Show the result */
    if (mlimit) {
        if (timebase) {
            goal_cliPrintf("Limit of mcast frames per port set to %u frames/%u ms.\n", mlimit, timebase);
        } else {
            goal_cliPrintf("Limit of mcast frames port is set to %u frames.\n", mlimit);
            goal_cliPrintf("Mcast limiting still disabled as timebase is set to 0.\n");
        }
    } else {
        goal_cliPrintf("Mcast storm protection is disabled (mlimit == 0).\n");
    }
}


/****************************************************************************/
/** Blimit set command execution handler
 */
static void goal_ethCliDosBlimitSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    unsigned int strLen;                        /* Param length */
    const char *pStr = NULL;                    /* Param string pointer */
    char *pEndPtr;                              /* End pointer */
    uint16_t blimit;                            /* Broadcast limit in frames */
    uint16_t timebase;                          /* Time base */

    /* Get mlimit param */
    res = goal_cliParamGet(pData, 4, &pStr, &strLen);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    /* Parse and check blimit param */
    errno = 0;
    blimit = (uint16_t) GOAL_STRTOUL(pStr, &pEndPtr, 0);
    if (pEndPtr != (pStr + strLen) || ERANGE == errno) {
        goal_cliPrintf("Invalid timebase format\n");
        return;
    }

    /* Set the blimit */
    res = goal_ethCmd(GOAL_ETH_CMD_BCAST_LIMIT, GOAL_TRUE, 0, &blimit);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Failed to set mcast limit.\n");
        return;
    }

    /* Get the timebase */
    res = goal_ethCmd(GOAL_ETH_CMD_STORMTIME, GOAL_FALSE, 0, &timebase);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Failed to retrieve timebase.\n");
        return;
    }

    /* Show the result */
    if (blimit) {
        if (timebase) {
            goal_cliPrintf("Limit of bcast frames per port set to %u frames/%u ms.\n", blimit, timebase);
        } else {
            goal_cliPrintf("Limit of Bcast frames per port set to %u frames.\n", blimit);
            goal_cliPrintf("Bcast limiting still disabled as timebase is set to 0.\n");
        }
    } else {
        goal_cliPrintf("Bcast storm protection now disabled (blimit == 0).\n");
    }
}


/****************************************************************************/
/** Mlimit show command execution handler
 */
static void goal_ethCliDosBlimitShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    uint16_t blimit;                            /* Broadcast limit in frames */
    uint16_t timebase;                          /* Time base */

    UNUSEDARG(pData);

    /* Get the blimit */
    res = goal_ethCmd(GOAL_ETH_CMD_BCAST_LIMIT, GOAL_FALSE, 0, &blimit);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Failed to retrieve multicast frame limit.\n");
        return;
    }

    /* Get the timebase */
    res = goal_ethCmd(GOAL_ETH_CMD_STORMTIME, GOAL_FALSE, 0, &timebase);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Failed to retrieve timebase.\n");
        return;
    }

    /* Show the result */
    if (blimit) {
        if (timebase) {
            goal_cliPrintf("Limit of bcast frames per port set to %u frames/%u ms.\n", blimit, timebase);
        } else {
            goal_cliPrintf("Limit of bcast frames port is set to %u frames.\n", blimit);
            goal_cliPrintf("Bcast limiting still disabled as timebase is set to 0.\n");
        }
    } else {
        goal_cliPrintf("Bcast storm protection is disabled (blimit == 0).\n");
    }
}


/****************************************************************************/
/** DoS command execution handler
 */
static void goal_ethCliDos(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    unsigned int strLen;                        /* Param length */
    const char *pStr = NULL;                    /* Param string pointer */

    res = goal_cliParamGet(pData, 2, &pStr, &strLen);
    if (GOAL_RES_OK(res)) {
        if (pStr == ethStrTxrate) {
            res = goal_cliParamGet(pData, 3, &pStr, &strLen);
            if (GOAL_RES_OK(res)) {
                if (pStr == ethStrSet) {
                    goal_ethCliDosTxrateSet(pData);
                    return;
                } else if (pStr == ethStrShow) {
                    goal_ethCliDosTxrateShow(pData);
                    return;
                }
            }
        } else if (pStr == ethStrTimebase) {
            res = goal_cliParamGet(pData, 3, &pStr, &strLen);
            if (GOAL_RES_OK(res)) {
                if (pStr == ethStrSet) {
                    goal_ethCliDosTimeframeSet(pData);
                    return;
                } else if (pStr == ethStrShow) {
                    goal_ethCliDosTimeframeShow(pData);
                    return;
                }
            }
        } else if (pStr == ethStrMlimit) {
            res = goal_cliParamGet(pData, 3, &pStr, &strLen);
            if (GOAL_RES_OK(res)) {
                if (pStr == ethStrSet) {
                    goal_ethCliDosMlimitSet(pData);
                    return;
                } else if (pStr == ethStrShow) {
                    goal_ethCliDosMlimitShow(pData);
                    return;
                }
            }
        } else if (pStr == ethStrBlimit) {
            res = goal_cliParamGet(pData, 3, &pStr, &strLen);
            if (GOAL_RES_OK(res)) {
                if (pStr == ethStrSet) {
                    goal_ethCliDosBlimitSet(pData);
                    return;
                } else if (pStr == ethStrShow) {
                    goal_ethCliDosBlimitShow(pData);
                    return;
                }
            }
        }
    }

    goal_cliPrintf("Syntax error\n");
}


/****************************************************************************/
/** Port state set command execution handler
 */
static void goal_ethCliPortAdminStateSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    unsigned int modeStrLen;                    /* Param length */
    const char *pModeStr = NULL;                /* Param string pointer */
    unsigned int portStrLen;                    /* Param length */
    const char *pPortStr = NULL;                /* Param string pointer */
    uint32_t port;                              /* Port */
    uint32_t portCount;                         /* Port count */
    char *pEndPtr;                              /* End pointer */
    GOAL_BOOL_T mode;                           /* Mode */

    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count.\n");
        return;
    }

    /* Get port param and check whether a valid port */
    res = goal_cliParamGet(pData, 4, &pPortStr, &portStrLen);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    port = (uint32_t) GOAL_STRTOUL(pPortStr, &pEndPtr, 10);
    if (pEndPtr != (pPortStr + portStrLen)) {
        goal_cliPrintf("Invalid port format\n");
        return;
    }

    if (portCount < port) {
        goal_cliPrintf("Invalid port number %u. Max. port number is %u.\n", port, portCount);
        return;
    }

    /* Get state parameter */
    res = goal_cliParamGet(pData, 5, &pModeStr, &modeStrLen);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    /* Set mode */
    if (CMD_PARAM_ON_LEN == modeStrLen && !strcmp(pModeStr, CMD_PARAM_ON)) {
        mode = GOAL_TRUE;
    } else if (CMD_PARAM_OFF_LEN == modeStrLen && !strcmp(pModeStr, CMD_PARAM_OFF)) {
        mode = GOAL_FALSE;
    } else {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    /* Update admin state */
    res = goal_ethCmd(GOAL_ETH_CMD_PORT_ADMIN_STATE, GOAL_TRUE, (port != portCount) ? GOAL_ETH_PORT(port) : GOAL_ETH_PORT_HOST, &mode);
    if (GOAL_RES_OK(res)) {
        goal_cliPrintf("Admin state for port %u now set to %s\n", port, (mode) ? "on" : "off");
    } else {
        goal_cliPrintf("Failed to update admin port state.\n");
    }

}


/****************************************************************************/
/** Port admin state show command execution handler
 */
static void goal_ethCliPortAdminStateShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    unsigned int portStrLen;                    /* Param length */
    const char *pPortStr = NULL;                /* Param string pointer */
    uint32_t port = 0;                          /* Port */
    GOAL_BOOL_T showAll = GOAL_TRUE;            /* Limit */
    uint32_t portCount;                         /* Port count */
    char *pEndPtr;                              /* End pointer */
    uint32_t idx;                               /* Index */
    GOAL_BOOL_T mode;                           /* Mode */

    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count.\n");
        return;
    }

    /* Get port param and check whether a valid port */
    res = goal_cliParamGet(pData, 4, &pPortStr, &portStrLen);
    if (GOAL_RES_OK(res)) {
        port = (uint32_t) GOAL_STRTOUL(pPortStr, &pEndPtr, 10);
        if (pEndPtr != (pPortStr + portStrLen)) {
            goal_cliPrintf("Invalid port format\n");
            return;
        }

        if (portCount < port) {
            goal_cliPrintf("Invalid port number %u. Max. port number is %u.\n", port, portCount);
            return;
        }

        showAll = GOAL_FALSE;
    }

    if (!showAll) {
        /* Update port state */
        res = goal_ethCmd(GOAL_ETH_CMD_PORT_ADMIN_STATE, GOAL_FALSE, (port != portCount) ? GOAL_ETH_PORT(port) : GOAL_ETH_PORT_HOST, &mode);
        if (GOAL_RES_OK(res)) {
            goal_cliPrintf("Port admin state for port %u is set to %s\n", port, (mode) ? "on" : "off");
        } else {
            goal_cliPrintf("Failed to update port admin state.\n");
        }
    } else {
        goal_cliPrintf("  Port    State\n");
        goal_cliPrintf("=================\n");
        for (idx = 0; idx < portCount + 1; idx++) {
            res = goal_ethCmd(GOAL_ETH_CMD_PORT_ADMIN_STATE, GOAL_FALSE, (idx != portCount) ? GOAL_ETH_PORT(idx) : GOAL_ETH_PORT_HOST, &mode);
            if (GOAL_RES_ERR(res)) {
                goal_cliPrintf("Failed to get admin state for port %u%\n", idx);
                return;
            }
            goal_cliPrintf("   %2u      %-3s\n", idx, (mode) ? "on" : "off");
        }
    }
}


/****************************************************************************/
/** Speed show command execution handler
 */
static void goal_ethCliPortSpeedShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    unsigned int portStrLen;                    /* Param length */
    const char *pPortStr = NULL;                /* Param string pointer */
    uint32_t port = 0;                          /* Port */
    GOAL_BOOL_T showAll = GOAL_TRUE;            /* Limit */
    uint32_t portCount;                         /* Port count */
    char *pEndPtr;                              /* End pointer */
    uint32_t idx;                               /* Index */
    uint32_t speed;                             /* Speed */
    GOAL_ETH_CLI_SPEEDMODE_T *pMode;            /* Speed mode */

    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count.\n");
        return;
    }

    /* Get port param and check whether a valid port */
    res = goal_cliParamGet(pData, 4, &pPortStr, &portStrLen);
    if (GOAL_RES_OK(res)) {
        port = (uint32_t) GOAL_STRTOUL(pPortStr, &pEndPtr, 10);
        if (pEndPtr != (pPortStr + portStrLen)) {
            goal_cliPrintf("Invalid port format\n");
            return;
        }

        if (portCount < port) {
            goal_cliPrintf("Invalid port number %u. Max. port number is %u.\n", port, portCount);
            return;
        }

        showAll = GOAL_FALSE;
    }

    if (!showAll) {
        /* Update speed state */
        speed = GOAL_ETH_SPEED_UNKNOWN;
        res = goal_ethCmd(GOAL_ETH_CMD_SPEED, GOAL_FALSE, (port != portCount) ? GOAL_ETH_PORT(port) : GOAL_ETH_PORT_HOST, &speed);
        if (GOAL_RES_OK(res)) {
            for (pMode = &(speedModes[0]); NULL != pMode->strMode; pMode++) {
                if (speed == pMode->speed) {
                    goal_cliPrintf("Port speed for port %u is set to %s\n", port, pMode->strMode);
                }
            }
        } else {
            goal_cliPrintf("Failed to update port admin state.\n");
        }
    } else {
        goal_cliPrintf("  Port    Speed\n");
        goal_cliPrintf("===================\n");
        for (idx = 0; idx < portCount + 1; idx++) {
            speed = GOAL_ETH_SPEED_UNKNOWN;
            res = goal_ethCmd(GOAL_ETH_CMD_SPEED, GOAL_FALSE, (idx != portCount) ? GOAL_ETH_PORT(idx) : GOAL_ETH_PORT_HOST, &speed);
            if (GOAL_RES_ERR(res)) {
                goal_cliPrintf("Failed to get admin state for port %u%\n", idx);
                return;
            }
            for (pMode = &(speedModes[0]); NULL != pMode->strMode; pMode++) {
                if (speed == pMode->speed) {
                    goal_cliPrintf("   %2u      %-8s\n", idx, pMode->strMode);
                }
            }
        }
    }
}


/****************************************************************************/
/** Speed set command execution handler
 */
static void goal_ethCliPortSpeedSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    unsigned int portStrLen;                    /* port param length */
    const char *pPortStr = NULL;                /* port param string pointer */
    unsigned int speedStrLen;                   /* speed param length */
    const char *pSpeedStr = NULL;               /* speed param string pointer */
    uint32_t port = 0;                          /* Port */
    uint32_t portCount;                         /* Port count */
    char *pEndPtr;                              /* End pointer */
    GOAL_ETH_CLI_SPEEDMODE_T *pMode;            /* Speed mode */

    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count.\n");
        return;
    }

    /* Get port param and check whether a valid port */
    res = goal_cliParamGet(pData, 4, &pPortStr, &portStrLen);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    port = (uint32_t) GOAL_STRTOUL(pPortStr, &pEndPtr, 10);
    if (pEndPtr != (pPortStr + portStrLen)) {
        goal_cliPrintf("Invalid port format\n");
        return;
    }

    if (portCount < port) {
        goal_cliPrintf("Invalid port number %u. Max. port number is %u.\n", port, portCount);
        return;
    }

    /* Get state parameter */
    res = goal_cliParamGet(pData, 5, &pSpeedStr, &speedStrLen);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    /* check for valid value, don't allow "unknown" */
    for (pMode = &(speedModes[0]); NULL != pMode->strMode; pMode++) {
        if ((GOAL_STRLEN(pMode->strMode) == speedStrLen) &&
            (0 == strncmp(pMode->strMode, pSpeedStr, speedStrLen)) &&
            (pMode != &speedModes[1])) {
            res = goal_ethCmd(GOAL_ETH_CMD_SPEED, GOAL_TRUE, (port != portCount) ? GOAL_ETH_PORT(port) : GOAL_ETH_PORT_HOST, &pMode->speed);
            if (GOAL_RES_ERR(res)) {
                goal_cliPrintf("Failed to update port speed.\n");
            }
            return;
        }
    }

    goal_cliPrintf("Syntax error\n");
}


/****************************************************************************/
/** Port state set command execution handler
 */
static void goal_ethCliPortStateSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    unsigned int modeStrLen;                    /* Param length */
    const char *pModeStr = NULL;                /* Param string pointer */
    unsigned int portStrLen;                    /* Param length */
    const char *pPortStr = NULL;                /* Param string pointer */
    uint32_t port;                              /* Port */
    uint32_t portCount;                         /* Port count */
    char *pEndPtr;                              /* End pointer */
    uint32_t mode;                              /* port state */

    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count.\n");
        return;
    }

    /* Get port param and check whether a valid port */
    res = goal_cliParamGet(pData, 4, &pPortStr, &portStrLen);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    port = (uint32_t) GOAL_STRTOUL(pPortStr, &pEndPtr, 10);
    if (pEndPtr != (pPortStr + portStrLen)) {
        goal_cliPrintf("Invalid port format\n");
        return;
    }

    if (portCount < port) {
        goal_cliPrintf("Invalid port number %u. Max. port number is %u.\n", port, portCount);
        return;
    }

    /* Get state parameter */
    res = goal_cliParamGet(pData, 5, &pModeStr, &modeStrLen);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    /* Set mode */
    if (0 == strcmp(pModeStr, "up")) {
        mode = GOAL_ETH_STATE_UP;
    } else if (0 == strcmp(pModeStr, "down")) {
        mode = GOAL_ETH_STATE_DOWN;
    } else if (0 == strcmp(pModeStr, "default")) {
        mode = GOAL_ETH_STATE_DEFAULT;
    } else {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    /* Update port state */
    res = goal_ethCmd(GOAL_ETH_CMD_PORT_STATE, GOAL_TRUE, (port != portCount) ? GOAL_ETH_PORT(port) : GOAL_ETH_PORT_HOST, &mode);
    if (GOAL_RES_OK(res)) {
        goal_cliPrintf("Port state for port %u now set to %s\n", port, (mode == GOAL_ETH_STATE_DEFAULT) ? "default"
                       : (mode == GOAL_ETH_STATE_UNKNOWN) ? "unknown"
                       : (mode == GOAL_ETH_STATE_DOWN) ? "down"
                       : (mode == GOAL_ETH_STATE_UP) ? "up"
                       : "???");
    } else {
        goal_cliPrintf("Failed to update port state.\n");
    }

}


/****************************************************************************/
/** Port state show command execution handler
 */
static void goal_ethCliPortStateShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    unsigned int portStrLen;                    /* Param length */
    const char *pPortStr = NULL;                /* Param string pointer */
    uint32_t port = 0;                          /* Port */
    GOAL_BOOL_T showAll = GOAL_TRUE;            /* Limit */
    uint32_t portCount;                         /* Port count */
    char *pEndPtr;                              /* End pointer */
    uint32_t idx;                               /* Index */
    uint32_t mode;                              /* port state */

    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count.\n");
        return;
    }

    /* Get port param and check whether a valid port */
    res = goal_cliParamGet(pData, 4, &pPortStr, &portStrLen);
    if (GOAL_RES_OK(res)) {
        port = (uint32_t) GOAL_STRTOUL(pPortStr, &pEndPtr, 10);
        if (pEndPtr != (pPortStr + portStrLen)) {
            goal_cliPrintf("Invalid port format\n");
            return;
        }

        if (portCount < port) {
            goal_cliPrintf("Invalid port number %u. Max. port number is %u.\n", port, portCount);
            return;
        }

        showAll = GOAL_FALSE;
    }

    if (!showAll) {
        /* Update port state */
        res = goal_ethCmd(GOAL_ETH_CMD_PORT_STATE, GOAL_FALSE, (port != portCount) ? GOAL_ETH_PORT(port) : GOAL_ETH_PORT_HOST, &mode);
        if (GOAL_RES_OK(res)) {
            goal_cliPrintf("Port state for port %u is set to %s\n", port, (mode == GOAL_ETH_STATE_DEFAULT) ? "default"
                           : (mode == GOAL_ETH_STATE_UNKNOWN) ? "unknown"
                           : (mode == GOAL_ETH_STATE_DOWN) ? "down"
                           : (mode == GOAL_ETH_STATE_UP) ? "up"
                           : "???");
        } else {
            goal_cliPrintf("Failed to update port state.\n");
        }
    } else {
        goal_cliPrintf("  Port    State\n");
        goal_cliPrintf("=================\n");
        for (idx = 0; idx < portCount; idx++) {
            res = goal_ethCmd(GOAL_ETH_CMD_PORT_STATE, GOAL_FALSE, (idx != portCount) ? GOAL_ETH_PORT(idx) : GOAL_ETH_PORT_HOST, &mode);
            if (GOAL_RES_ERR(res)) {
                goal_cliPrintf("Failed to get state for port %u%\n", idx);
                return;
            }
            goal_cliPrintf("   %2u      %-3s\n", idx, (mode == GOAL_ETH_STATE_DEFAULT) ? "default"
                           : (mode == GOAL_ETH_STATE_UNKNOWN) ? "unknown"
                           : (mode == GOAL_ETH_STATE_DOWN) ? "down"
                           : (mode == GOAL_ETH_STATE_UP) ? "up"
                           : "???");
        }
    }
}


/****************************************************************************/
/** State show command execution handler
 */
static void goal_ethCliPortLinkShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    unsigned int portStrLen;                    /* Param length */
    const char *pPortStr = NULL;                /* Param string pointer */
    uint32_t port = 0;                          /* Port */
    GOAL_BOOL_T showAll = GOAL_TRUE;            /* Limit */
    uint32_t portCount;                         /* Port count */
    char *pEndPtr;                              /* End pointer */
    uint32_t idx;                               /* Index */
    uint32_t link;                              /* Link */
    GOAL_ETH_CLI_LINKSTATE_T *pMode;            /* Speed mode */

    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count.\n");
        return;
    }

    /* Get port param and check whether a valid port */
    res = goal_cliParamGet(pData, 4, &pPortStr, &portStrLen);
    if (GOAL_RES_OK(res)) {
        port = (uint32_t) GOAL_STRTOUL(pPortStr, &pEndPtr, 10);
        if (pEndPtr != (pPortStr + portStrLen)) {
            goal_cliPrintf("Invalid port format\n");
            return;
        }

        if (portCount < port) {
            goal_cliPrintf("Invalid port number %u. Max. port number is %u.\n", port, portCount);
            return;
        }

        showAll = GOAL_FALSE;
    }

    if (!showAll) {
        /* Update link state */
        link = GOAL_ETH_STATE_UNKNOWN;
        res = goal_ethCmd(GOAL_ETH_CMD_LINK_STATE, GOAL_FALSE, (port != portCount) ? GOAL_ETH_PORT(port) : GOAL_ETH_PORT_HOST, &link);
        if (GOAL_RES_OK(res)) {
            for (pMode = &(linkModes[0]); NULL != pMode->strMode; pMode++) {
                if (link == pMode->link) {
                    goal_cliPrintf("Link state for port %u is set to %s\n", port, pMode->strMode);
                }
            }
        } else {
            goal_cliPrintf("Failed to update link state.\n");
        }
    } else {
        goal_cliPrintf("  Port    Link\n");
        goal_cliPrintf("===================\n");
        for (idx = 0; idx < portCount + 1; idx++) {
            link = GOAL_ETH_STATE_UNKNOWN;
            res = goal_ethCmd(GOAL_ETH_CMD_LINK_STATE, GOAL_FALSE, (idx != portCount) ? GOAL_ETH_PORT(idx) : GOAL_ETH_PORT_HOST, &link);
            if (GOAL_RES_ERR(res)) {
                goal_cliPrintf("Failed to get link state for port %u%\n", idx);
                return;
            }
            for (pMode = &(linkModes[0]); NULL != pMode->strMode; pMode++) {
                if (link == pMode->link) {
                    goal_cliPrintf("   %2u      %-8s\n", idx, pMode->strMode);
                }
            }
        }
    }
}


/****************************************************************************/
/** Duplex show command execution handler
 */
static void goal_ethCliPortDuplexShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    unsigned int portStrLen;                    /* Param length */
    const char *pPortStr = NULL;                /* Param string pointer */
    uint32_t port = 0;                          /* Port */
    GOAL_BOOL_T showAll = GOAL_TRUE;            /* Limit */
    uint32_t portCount;                         /* Port count */
    char *pEndPtr;                              /* End pointer */
    uint32_t idx;                               /* Index */
    uint32_t duplex;                            /* Duplex */
    GOAL_ETH_CLI_DUPLEXMODE_T *pMode;           /* Duplex mode */

    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count.\n");
        return;
    }

    /* Get port param and check whether a valid port */
    res = goal_cliParamGet(pData, 4, &pPortStr, &portStrLen);
    if (GOAL_RES_OK(res)) {
        port = (uint32_t) GOAL_STRTOUL(pPortStr, &pEndPtr, 10);
        if (pEndPtr != (pPortStr + portStrLen)) {
            goal_cliPrintf("Invalid port format\n");
            return;
        }

        if (portCount < port) {
            goal_cliPrintf("Invalid port number %u. Max. port number is %u.\n", port, portCount);
            return;
        }

        showAll = GOAL_FALSE;
    }

    if (!showAll) {
        /* Update duplex state */
        duplex = GOAL_ETH_DUPLEX_UNKNOWN;
        res = goal_ethCmd(GOAL_ETH_CMD_DUPLEX, GOAL_FALSE, (port != portCount) ? GOAL_ETH_PORT(port) : GOAL_ETH_PORT_HOST, &duplex);
        if (GOAL_RES_OK(res)) {
            for (pMode = &(duplexModes[0]); NULL != pMode->strMode; pMode++) {
                if (duplex == pMode->duplex) {
                    goal_cliPrintf("Duplex mode for port %u is set to %s\n", port, pMode->strMode);
                }
            }
        } else {
            goal_cliPrintf("Failed to update duplex mode.\n");
        }
    } else {
        goal_cliPrintf("  Port      Duplex\n");
        goal_cliPrintf("=======================\n");
        for (idx = 0; idx < portCount + 1; idx++) {
            duplex = GOAL_ETH_DUPLEX_UNKNOWN;
            res = goal_ethCmd(GOAL_ETH_CMD_DUPLEX, GOAL_FALSE, (idx != portCount) ? GOAL_ETH_PORT(idx) : GOAL_ETH_PORT_HOST, &duplex);
            if (GOAL_RES_ERR(res)) {
                goal_cliPrintf("Failed to get duplex mode for port %u%\n", idx);
                return;
            }
            for (pMode = &(duplexModes[0]); NULL != pMode->strMode; pMode++) {
                if (duplex == pMode->duplex) {
                    goal_cliPrintf("   %2u      %-11s\n", idx, pMode->strMode);
                }
            }
        }
    }
}


/****************************************************************************/
/** Duplex set command execution handler
 */
static void goal_ethCliPortDuplexSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    unsigned int portStrLen;                    /* port param length */
    const char *pPortStr = NULL;                /* port param string pointer */
    unsigned int duplexStrLen;                  /* duplex param length */
    const char *pDuplexStr = NULL;              /* duplex param string pointer */
    uint32_t port = 0;                          /* Port */
    uint32_t portCount;                         /* Port count */
    char *pEndPtr;                              /* End pointer */
    GOAL_ETH_CLI_DUPLEXMODE_T *pMode;           /* Duplex mode */

    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count.\n");
        return;
    }

    /* Get port param and check whether a valid port */
    res = goal_cliParamGet(pData, 4, &pPortStr, &portStrLen);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    port = (uint32_t) GOAL_STRTOUL(pPortStr, &pEndPtr, 10);
    if (pEndPtr != (pPortStr + portStrLen)) {
        goal_cliPrintf("Invalid port format\n");
        return;
    }

    if (portCount < port) {
        goal_cliPrintf("Invalid port number %u. Max. port number is %u.\n", port, portCount);
        return;
    }

    /* Get duplex parameter */
    res = goal_cliParamGet(pData, 5, &pDuplexStr, &duplexStrLen);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    /* check for valid value, don't allow "unknown" */
    for (pMode = &(duplexModes[0]); NULL != pMode->strMode; pMode++) {
        if ((GOAL_STRLEN(pMode->strMode) == duplexStrLen) &&
            (0 == strncmp(pMode->strMode, pDuplexStr, duplexStrLen)) &&
            (pMode != &duplexModes[1])) {
            res = goal_ethCmd(GOAL_ETH_CMD_DUPLEX, GOAL_TRUE, (port != portCount) ? GOAL_ETH_PORT(port) : GOAL_ETH_PORT_HOST, &pMode->duplex);
            if (GOAL_RES_ERR(res)) {
                goal_cliPrintf("Failed to update port duplex mode.\n");
            }
            return;
        }
    }

    goal_cliPrintf("Syntax error\n");
}


/****************************************************************************/
/** Autonegotiation  show command execution handler
 */
static void goal_ethCliPortAutonegShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    unsigned int portStrLen;                    /* Param length */
    const char *pPortStr = NULL;                /* Param string pointer */
    uint32_t port = 0;                          /* Port */
    GOAL_BOOL_T showAll = GOAL_TRUE;            /* Limit */
    uint32_t portCount;                         /* Port count */
    char *pEndPtr;                              /* End pointer */
    uint32_t idx;                               /* Index */
    uint32_t autoneg;                           /* autonegotiation status */
    GOAL_ETH_CLI_AUTONEG_T *pMode;              /* autonegotiation mode */

    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count.\n");
        return;
    }

    /* Get port param and check whether a valid port */
    res = goal_cliParamGet(pData, 4, &pPortStr, &portStrLen);
    if (GOAL_RES_OK(res)) {
        port = (uint32_t) GOAL_STRTOUL(pPortStr, &pEndPtr, 10);
        if (pEndPtr != (pPortStr + portStrLen)) {
            goal_cliPrintf("Invalid port format\n");
            return;
        }

        if (portCount < port) {
            goal_cliPrintf("Invalid port number %u. Max. port number is %u.\n", port, portCount);
            return;
        }

        showAll = GOAL_FALSE;
    }

    if (!showAll) {
        autoneg = GOAL_ETH_AUTONEG_UNKNOWN;
        res = goal_ethCmd(GOAL_ETH_CMD_AUTONEG, GOAL_FALSE, (port != portCount) ? GOAL_ETH_PORT(port) : GOAL_ETH_PORT_HOST, &autoneg);
        if (GOAL_RES_OK(res)) {
            for (pMode = &(autonegModes[0]); NULL != pMode->strMode; pMode++) {
                if (autoneg == pMode->autoneg) {
                    goal_cliPrintf("Autonigotiation for port %u is set to %s\n", port, pMode->strMode);
                }
            }
        } else {
            goal_cliPrintf("Failed to get Autonegotiation mode.\n");
        }
    } else {
        goal_cliPrintf("  Port      Autonegotiation\n");
        goal_cliPrintf("=======================\n");
        for (idx = 0; idx < portCount + 1; idx++) {
            autoneg = GOAL_ETH_AUTONEG_UNKNOWN;
            res = goal_ethCmd(GOAL_ETH_CMD_AUTONEG, GOAL_FALSE, (idx != portCount) ? GOAL_ETH_PORT(idx) : GOAL_ETH_PORT_HOST, &autoneg);
            if (GOAL_RES_ERR(res)) {
                goal_cliPrintf("Failed to get Autonegotiation mode for port %u\n", idx);
                return;
            }
            for (pMode = &(autonegModes[0]); NULL != pMode->strMode; pMode++) {
                if (autoneg == pMode->autoneg) {
                    goal_cliPrintf("   %2u      %-11s\n", idx, pMode->strMode);
                }
            }
        }
    }
}


/****************************************************************************/
/** Autonegotiation set command execution handler
 */
static void goal_ethCliPortAutonegSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    unsigned int portStrLen;                    /* port param length */
    const char *pPortStr = NULL;                /* port param string pointer */
    unsigned int anStrLen;                      /* autonegotiation param length */
    const char *pAnStr = NULL;                  /* autonegotiation param string pointer */
    uint32_t port = 0;                          /* Port */
    uint32_t portCount;                         /* Port count */
    char *pEndPtr;                              /* End pointer */
    GOAL_ETH_CLI_AUTONEG_T *pMode;              /* Duplex mode */

    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count.\n");
        return;
    }

    /* Get port param and check whether a valid port */
    res = goal_cliParamGet(pData, 4, &pPortStr, &portStrLen);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    port = (uint32_t) GOAL_STRTOUL(pPortStr, &pEndPtr, 10);
    if (pEndPtr != (pPortStr + portStrLen)) {
        goal_cliPrintf("Invalid port format\n");
        return;
    }

    if (portCount < port) {
        goal_cliPrintf("Invalid port number %u. Max. port number is %u.\n", port, portCount);
        return;
    }

    /* Get autonegotiation parameter */
    res = goal_cliParamGet(pData, 5, &pAnStr, &anStrLen);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    /* check for valid value, don't allow "unknown" */
    for (pMode = &(autonegModes[0]); NULL != pMode->strMode; pMode++) {
        if ((GOAL_STRLEN(pMode->strMode) == anStrLen) &&
            (0 == strncmp(pMode->strMode, pAnStr, anStrLen)) &&
            (pMode != &autonegModes[1])) {
            res = goal_ethCmd(GOAL_ETH_CMD_AUTONEG, GOAL_TRUE, (port != portCount) ? GOAL_ETH_PORT(port) : GOAL_ETH_PORT_HOST, &pMode->autoneg);
            if (GOAL_RES_ERR(res)) {
                goal_cliPrintf("Failed to update port autonegotiation mode.\n");
            }
            goal_ethCmd(GOAL_ETH_CMD_AUTONEG_RESTART, GOAL_TRUE, port, NULL);
            return;
        }
    }

    goal_cliPrintf("Syntax error\n");
}


/****************************************************************************/
/** Mirror set command execution handler
 */
static void goal_ethCliPortMirrorSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    unsigned int portStrLen;                    /* Param length */
    const char *pPortStr = NULL;                /* Param string pointer */
    const char *pPortStrMac = NULL;             /* Param string pointer for mac addr or portmask */
    uint32_t portCount;                         /* Port count */
    char *pEndPtr;                              /* End pointer */
    GOAL_ETH_MAC_ADDR_T macAddr;                /* array for MAC address */
    GOAL_ETH_MIRROR_CONF_T mirror;              /* Port mirror conf */
    GOAL_ETH_CLI_MIRRORMODE_T *pMode;           /* mirror mode */
    uint32_t cnt;                               /* count */


    /* check number of parameters */
    res = goal_cliParamGet(pData, CMD_MIRRORSET_PARAMLEN - 1, &pPortStr, &portStrLen);
    if (!GOAL_RES_OK(res)) {
        goal_cliPrintf("Missing parameter for eth port mirror set\n");
        return;
    }

    /* Get port count */
    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count.\n");
        return;
    }

    /* Get port param and check whether a valid port */
    res = goal_cliParamGet(pData, CMD_MIRRORSET_PARAMLEN - 3, &pPortStr, &portStrLen);
    if (!GOAL_RES_OK(res)) {
        goal_cliPrintf("Failed to read port\n");
        return;
    }

    mirror.port = (uint32_t) GOAL_STRTOUL(pPortStr, &pEndPtr, 10);
    if (pEndPtr != (pPortStr + portStrLen)) {
        goal_cliPrintf("Invalid port format\n");
        return;
    }

    if (portCount < mirror.port) {
        goal_cliPrintf("Invalid port number %u. Max. port number is %u.\n", mirror.port, portCount);
        return;
    }

    /* Get mode */
    res = goal_cliParamGet(pData, CMD_MIRRORSET_PARAMLEN - 1, &pPortStr, &portStrLen);
    if (!GOAL_RES_OK(res)) {
        goal_cliPrintf("Failed to read mirror mode\n");
        return;
    }

    pMode = mirrorModes;
    cnt = 0;
    while (NULL != pMode->strMode) {
        if ((GOAL_STRLEN(pMode->strMode) == portStrLen) && !strncmp(pMode->strMode, pPortStr, portStrLen)) {
            mirror.mirrortype = pMode->mirrortype;
            if (cnt < CMD_PORT_MIRROR_PORT) {

                /* Get port map param */

                res = goal_cliParamGet(pData, CMD_MIRRORSET_PARAMLEN - 2, &pPortStrMac, &portStrLen);
                if (GOAL_RES_OK(res)) {
                    mirror.portmap = (uint8_t) GOAL_STRTOUL(pPortStrMac, &pEndPtr, 2);
                    if (GOAL_RES_ERR(res)) {
                        goal_cliPrintf("Invalid port map\n");
                        return;
                    }
                }
            }
            else {

                /* Get Mac addr param */

                res = goal_cliParamGet(pData, CMD_MIRRORSET_PARAMLEN - 2, &pPortStrMac, &portStrLen);
                if (GOAL_RES_OK(res)) {
                    mirror.macaddr = (GOAL_ETH_MAC_ADDR_T *) macAddr;
                    res = goal_ethCliParamAsMac(pPortStrMac, portStrLen, mirror.macaddr);
                    if (GOAL_RES_ERR(res)) {
                        goal_cliPrintf("Invalid MAC address\n");
                        return;
                    }
                }
                else {
                    goal_cliPrintf("Mac adress is needed for all mode not using ports directly\n");
                    return;
                }
            }
            break;
        }
        cnt++;
        pMode++;
    }

    if (NULL == pMode->strMode) {
        goal_cliPrintf("Unknown mirror type\n");
        return;
    }

    res = goal_ethCmd(GOAL_ETH_CMD_PORT_MIRROR, GOAL_TRUE, mirror.port, &mirror);
    if (GOAL_RES_OK(res)) {
        goal_cliPrintf("Port %d's mirror mode is '%s'.\n", mirror.port, pPortStr);
    }
    else {
        goal_cliPrintf("Error setting mirror mode: Port %d, mode '%s'.\n", mirror.port, pPortStr);
    }
}


/****************************************************************************/
/** Mirror show command execution handler
 *
 */
static void goal_ethCliPortMirrorShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    GOAL_ETH_CLI_MIRRORMODE_T *pMode;           /* mirror mode */
    uint32_t port = 0;                          /* Port */
    uint32_t portCount;                         /* Port count */
    const char *pPortStr;                       /* Port string */
    unsigned int portStrLen;                    /* Port string length */
    char *pEndPtr;                              /* End pointer */
    GOAL_ETH_MIRROR_CONF_T mirror;              /* mirror */
    uint32_t idx;                               /* Index */
    uint32_t argc = goal_cliParamCount(pData);  /* Argument count */

    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count.\n");
        return;
    }

    if (CMD_MIRRORSHOW_PARAMLEN == argc) {

        /* Output single mirror mode */

        /* Get port param and check whether a valid port */
        res = goal_cliParamGet(pData, CMD_MODESHOW_PARAMLEN - 1, &pPortStr, &portStrLen);
        if (GOAL_RES_OK(res)) {
            port = (uint32_t) GOAL_STRTOUL(pPortStr, &pEndPtr, 10);
            if (pEndPtr != (pPortStr + portStrLen)) {
                goal_cliPrintf("Invalid port format\n");
                return;
            }

            if ((portCount + 1) < port) {
                goal_cliPrintf("Invalid port number %u. Max. port number is %u.\n", port, portCount);
                return;
            }
        }

        res = goal_ethCmd(GOAL_ETH_CMD_PORT_MIRROR, GOAL_FALSE, (portCount == port) ? GOAL_ETH_PORT_HOST : GOAL_ETH_PORT(port), &mirror);
        if (GOAL_RES_ERR(res)) {
            goal_cliPrintf("Getting mirror type failed with code: 0x08%"FMT_x32"\n", res);
            return;
        }
        else {
            pMode = mirrorModes;
            while (NULL != pMode->strMode) {
                if (mirror.mirrortype == pMode->mirrortype) {
                    goal_cliPrintf("Mirror mode of port %u is %s.\n", port, pMode->strMode);
                    return;
                }
                pMode++;
            }
        }
    }
    else {

        /* Output modes of all ports */
        goal_cliPrintf("  Port       Mirror Mode\n");
        goal_cliPrintf("========================================\n");
        for (idx = 0; idx < portCount + 1; idx++) {
            res = goal_ethCmd(GOAL_ETH_CMD_PORT_MIRROR, GOAL_FALSE, (portCount == idx) ? GOAL_ETH_PORT_HOST : GOAL_ETH_PORT(idx), &mirror);
            if (GOAL_RES_ERR(res)) {
                goal_cliPrintf("Getting input mode failed with code: 0x08%"FMT_x32"\n", res);
                return;
            }
            else {
                pMode = mirrorModes;
                while (NULL != pMode->strMode) {
                    if (mirror.mirrortype == pMode->mirrortype) {
                        goal_cliPrintf("%4u         %s\n", GOAL_ETH_PORT(idx), pMode->strMode);
                        break;
                    }
                    pMode++;
                }
            }
        }
        return;
    }
}


/****************************************************************************/
/** Command execution handler
 */
static void goal_ethCliFunc(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    unsigned int strLen;                        /* Param length */
    const char *pStr = NULL;                    /* Param string pointer */
    uint32_t argc = goal_cliParamCount(pData);  /* Argument count */

    /* Need at least 2 arguments */
    if (CMD_ETH_PARAMLEN > argc) {
        goal_cliPrintf("Invalid syntax\n");
        goal_ethCliPrintHelp();
        return;
    }

    /* retrieve control command */
    res = goal_cliParamGet(pData, 1, &pStr, &strLen);
    if (GOAL_RES_OK(res)) {

        /* help command */
        if (ethStrHelp == pStr) {
            goal_ethCliPrintHelp();
            return;
        }

        /* vlan command */
        if (ethStrVlan == pStr) {
            goal_ethCliVlan(pData);
            return;
        }

        /* mactab command */
        if (ethStrMactab == pStr) {
            goal_ethCliMactab(pData);
            return;
        }

        /* dos command */
        if (ethStrDos == pStr) {
            goal_ethCliDos(pData);
            return;
        }

        /* port command */
        if (ethStrPort == pStr) {
            goal_ethCliPort(pData);
            return;
        }

        /* qos command */
        if (ethStrQos == pStr) {
            goal_ethCliQos(pData);
            return;
        }

#if GOAL_CONFIG_TDMA == 1
        /* tdma command */
        if (ethStrTdma == pStr) {
            goal_ethCliTdma(pData);
            return;
        }
#endif

        /* rxrslv command */
        if (ethStrRxRslv == pStr) {
            goal_ethCliRxRslv(pData);
            return;
        }
    }

    goal_cliPrintf("Syntax error\n");
}


/****************************************************************************/
/** Port command execution handler
 */
static void goal_ethCliPort(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    unsigned int strLen;                        /* Param length */
    const char *pStr = NULL;                    /* Param string pointer */

    res = goal_cliParamGet(pData, 2, &pStr, &strLen);
    if (GOAL_RES_OK(res)) {
        if (pStr == ethStrAdstate) {
            res = goal_cliParamGet(pData, 3, &pStr, &strLen);
            if (GOAL_RES_OK(res)) {
                if (pStr == ethStrSet) {
                    goal_ethCliPortAdminStateSet(pData);
                    return;
                } else if (pStr == ethStrShow) {
                    goal_ethCliPortAdminStateShow(pData);
                    return;
                }
            }
        } else if (pStr == ethStrSpeed) {
            res = goal_cliParamGet(pData, 3, &pStr, &strLen);
            if (GOAL_RES_OK(res)) {
                if (pStr == ethStrShow) {
                    goal_ethCliPortSpeedShow(pData);
                    return;
                }
                else if (pStr == ethStrSet) {
                    goal_ethCliPortSpeedSet(pData);
                    return;
                }
            }
        } else if (pStr == ethStrLink) {
            res = goal_cliParamGet(pData, 3, &pStr, &strLen);
            if (GOAL_RES_OK(res)) {
                if (pStr == ethStrShow) {
                    goal_ethCliPortLinkShow(pData);
                    return;
                }
            }
        } else if (pStr == ethStrDuplex) {
            res = goal_cliParamGet(pData, 3, &pStr, &strLen);
            if (GOAL_RES_OK(res)) {
                if (pStr == ethStrShow) {
                    goal_ethCliPortDuplexShow(pData);
                    return;
                }
                else if (pStr == ethStrSet) {
                    goal_ethCliPortDuplexSet(pData);
                    return;
                }
            }
        } else if (pStr == ethStrAutoneg) {
            res = goal_cliParamGet(pData, 3, &pStr, &strLen);
            if (GOAL_RES_OK(res)) {
                if (pStr == ethStrShow) {
                    goal_ethCliPortAutonegShow(pData);
                    return;
                }
                else if (pStr == ethStrSet) {
                    goal_ethCliPortAutonegSet(pData);
                    return;
                }
            }
        } else if (pStr == ethStrMirror) {
            res = goal_cliParamGet(pData, 3, &pStr, &strLen);
            if (GOAL_RES_OK(res)) {
                if (pStr == ethStrSet) {
                    goal_ethCliPortMirrorSet(pData);
                    return;
                }
                else if (pStr == ethStrShow) {
                    goal_ethCliPortMirrorShow(pData);
                    return;
                }
            }
        }
        else if (pStr == ethStrState) {
            res = goal_cliParamGet(pData, 3, &pStr, &strLen);
            if (GOAL_RES_OK(res)) {
                if (pStr == ethStrSet) {
                    goal_ethCliPortStateSet(pData);
                    return;
                } else if (pStr == ethStrShow) {
                    goal_ethCliPortStateShow(pData);
                    return;
                }
            }
        }
        else if (pStr == ethStrMdi) {
            res = goal_cliParamGet(pData, 3, &pStr, &strLen);
            if (GOAL_RES_OK(res)) {
                if (pStr == ethStrState) {
                    res = goal_cliParamGet(pData, 4, &pStr, &strLen);
                    if (GOAL_RES_OK(res)) {
                        if (pStr == ethStrShow) {
                            goal_ethCliPortMdiStateShow(pData);
                            return;
                        }
                        else if (pStr == ethStrSet) {
                            goal_ethCliPortMdiStateSet(pData);
                            return;
                        }
                    }
                } else if (pStr == ethStrMode) {
                    res = goal_cliParamGet(pData, 4, &pStr, &strLen);
                    if (GOAL_RES_OK(res)) {
                        if (pStr == ethStrShow) {
                            goal_ethCliPortMdiModeShow(pData);
                            return;
                        }
                        else if (pStr == ethStrSet) {
                            goal_ethCliPortMdiModeSet(pData);
                            return;
                        }
                    }
                }
            }
        }
    }

    goal_cliPrintf("Syntax error\n");
}


/****************************************************************************/
/** QoS command execution handler
 */
static void goal_ethCliQos(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    unsigned int strLen;                        /* Param length */
    const char *pStr = NULL;                    /* Param string pointer */

    res = goal_cliParamGet(pData, 2, &pStr, &strLen);
    if (GOAL_RES_OK(res)) {
        if (pStr == ethStrMode) {
            res = goal_cliParamGet(pData, 3, &pStr, &strLen);
            if (GOAL_RES_OK(res)) {
                if (pStr == ethStrSet) {
                    goal_ethCliQosModeSet(pData);
                    return;
                } else if (pStr == ethStrShow) {
                    goal_ethCliQosModeShow(pData);
                    return;
                }
            }
        } else if (pStr == ethStrDefprio) {
            res = goal_cliParamGet(pData, 3, &pStr, &strLen);
            if (GOAL_RES_OK(res)) {
                if (pStr == ethStrSet) {
                    goal_ethCliQosDefprioSet(pData);
                    return;
                } else if (pStr == ethStrShow) {
                    goal_ethCliQosDefprioShow(pData);
                    return;
                }
            }
        } else if (pStr == ethStrVlanprio) {
            res = goal_cliParamGet(pData, 3, &pStr, &strLen);
            if (GOAL_RES_OK(res)) {
                if (pStr == ethStrSet) {
                    goal_ethCliQosVlanPrioSet(pData);
                    return;
                } else if (pStr == ethStrShow) {
                    goal_ethCliQosVlanPrioShow(pData);
                    return;
                }
            }
        }
        else if (pStr == ethStrIpprio) {
            res = goal_cliParamGet(pData, 3, &pStr, &strLen);
            if (GOAL_RES_OK(res)) {
                if (pStr == ethStrSet) {
                    goal_ethCliQosIpPrioSet(pData);
                    return;
                } else if (pStr == ethStrShow) {
                    goal_ethCliQosIpPrioShow(pData);
                    return;
                }
            }
        }
    }

    goal_cliPrintf("Syntax error\n");
}


/****************************************************************************/
/** QoS mode set handler
 *
 */
static void goal_ethCliQosModeSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    uint32_t port;                              /* Port */
    uint32_t portCount;                         /* Port count */
    const char *pPortStr;                       /* Port string */
    unsigned int portStrLen;                    /* Port string length */
    const char *pModeStr;                       /* Mode string */
    unsigned int modeStrLen;                    /* Mode string length */
    const char *pEnableStr;                     /* Enable string */
    unsigned int enableStrLen;                  /* Enable string length */
    char *pEndPtr;                              /* End pointer */
    uint32_t qos;                               /* QoS Mode */
    GOAL_ETH_CLI_QOSMODE_T *pMode;              /* QoS Mode ptr */
    GOAL_BOOL_T enable;                         /* Enable/disable mode */

    UNUSEDARG(pData);

    /* Get port param and check whether a valid port */
    res = goal_cliParamGet(pData, 4, &pPortStr, &portStrLen);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Syntax error\n");
        return;
    }
    port = (uint32_t) GOAL_STRTOUL(pPortStr, &pEndPtr, 10);

    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count.\n");
        return;
    }

    if ((portCount + 1) < port) {
        goal_cliPrintf("Invalid port number %u. Max. port number is %u.\n", port, portCount);
        return;
    }

    /* Get QoS mode */
    res = goal_cliParamGet(pData, 5, &pModeStr, &modeStrLen);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    res = goal_cliParamGet(pData, 6, &pEnableStr, &enableStrLen);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    /* Get mode */
    if (CMD_PARAM_ON_LEN == enableStrLen && !strcmp(pEnableStr, CMD_PARAM_ON)) {
        enable = GOAL_TRUE;
    } else if (CMD_PARAM_OFF_LEN == enableStrLen && !strcmp(pEnableStr, CMD_PARAM_OFF)) {
        enable = GOAL_FALSE;
    } else {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    res = goal_ethCmd(GOAL_ETH_CMD_QOS_MODE, GOAL_FALSE, (port != portCount) ? GOAL_ETH_PORT(port) : GOAL_ETH_PORT_HOST, &(qos));
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Failed to retrieve qos mode for port %u\n", pPortStr);
        return;
    }

    for (pMode = &(qosModes[0]); pMode->strMode != NULL; pMode++) {
        if ((GOAL_STRLEN(pMode->strMode) == modeStrLen) && !strncmp(pMode->strMode, pModeStr, modeStrLen)) {
            if (enable) {
                qos |= pMode->qosMode;
            } else {
                qos &= ~(pMode->qosMode);
            }
            res = goal_ethCmd(GOAL_ETH_CMD_QOS_MODE, GOAL_TRUE, (port != portCount) ? GOAL_ETH_PORT(port) : GOAL_ETH_PORT_HOST, &qos);
            if (GOAL_RES_ERR(res)) {
                goal_cliPrintf("Failed to update qos mode for port %s\n.", pPortStr);
            } else {
                goal_cliPrintf("Set qos mode \"%s\" for port %s to %s.\n", pModeStr, pPortStr, pEnableStr);
            }
            return;
        }
    }

    goal_cliPrintf("Invalid QoS mode\n");
}


/****************************************************************************/
/** QoS mode show command execution handler
 */
static void goal_ethCliQosModeShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    unsigned int portStrLen;                    /* Param length */
    const char *pPortStr = NULL;                /* Param string pointer */
    uint32_t port = 0;                          /* Port */
    GOAL_BOOL_T showAll = GOAL_TRUE;            /* Limit */
    uint32_t portCount;                         /* Port count */
    char *pEndPtr;                              /* End pointer */
    GOAL_ETH_QOS_MODE_T mode;                   /* QoS mode */
    uint32_t idx;                               /* Index */
    GOAL_ETH_CLI_QOSMODE_T *pMode;              /* QoS Mode pointer */

    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count.\n");
        return;
    }

    /* Get port param and check whether a valid port */
    res = goal_cliParamGet(pData, 4, &pPortStr, &portStrLen);
    if (GOAL_RES_OK(res)) {
        port = (uint32_t) GOAL_STRTOUL(pPortStr, &pEndPtr, 10);
        if (pEndPtr != (pPortStr + portStrLen)) {
            goal_cliPrintf("Invalid port format\n");
            return;
        }

        if (portCount < port) {
            goal_cliPrintf("Invalid port number %u. Max. port number is %u.\n", port, portCount);
            return;
        }

        showAll = GOAL_FALSE;
    }

    if (!showAll) {
        /* Update rate limiting */
        res = goal_ethCmd(GOAL_ETH_CMD_QOS_MODE, GOAL_FALSE, (port != portCount) ? GOAL_ETH_PORT(port) : GOAL_ETH_PORT_HOST, &mode);
        if (GOAL_RES_OK(res)) {
            goal_cliPrintf("  Mode  State \n");
            goal_cliPrintf("==============\n");
            for (pMode = &(qosModes[0]); pMode->strMode != NULL; pMode++) {
                goal_cliPrintf("  %-6s  %-6s\n", pMode->strMode, (mode.mode & pMode->qosMode) ? "on" : "off");
            }
        } else {
            goal_cliPrintf("Failed to update qos mode.\n");
        }
    } else {
        goal_cliPrintf("  Port  ");
        for (pMode = &(qosModes[0]); pMode->strMode != NULL; pMode++) {
            goal_cliPrintf("%-10s", pMode->strMode);
        }
        goal_cliPrintf("\n");
        goal_cliPrintf("========");
        for (pMode = &(qosModes[0]); pMode->strMode != NULL; pMode++) {
            goal_cliPrintf("==========");
        }
        goal_cliPrintf("\n");

        for (idx = 0; idx < portCount + 1; idx++) {
            res = goal_ethCmd(GOAL_ETH_CMD_QOS_MODE, GOAL_FALSE, (idx != portCount) ? GOAL_ETH_PORT(idx) : GOAL_ETH_PORT_HOST, &mode);
            if (GOAL_RES_ERR(res)) {
                goal_cliPrintf("Failed to get qos mode for port %u%\n", idx);
                return;
            }
            goal_cliPrintf("  %-4u  ", idx);
            for (pMode = &(qosModes[0]); pMode->strMode != NULL; pMode++) {
                goal_cliPrintf("%-10s", (mode.mode & pMode->qosMode) ? "on" : "off");
            }
            goal_cliPrintf("\n");
        }
    }

}


/****************************************************************************/
/** QoS defprio set handler
 *
 */
static void goal_ethCliQosDefprioSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    uint32_t port;                              /* Port */
    uint32_t portCount;                         /* Port count */
    const char *pPortStr;                       /* Port string */
    unsigned int portStrLen;                    /* Port string length */
    const char *pPrioStr;                       /* Prio string */
    unsigned int prioStrLen;                    /* Prio string length */
    char *pEndPtr;                              /* End pointer */
    GOAL_ETH_QOS_MODE_T qosMode;                /* QoS mode */
    uint32_t defprio;                           /* Default priority */

    /* Get port param and check whether a valid port */
    res = goal_cliParamGet(pData, 4, &pPortStr, &portStrLen);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Syntax error\n");
        return;
    }
    port = (uint32_t) GOAL_STRTOUL(pPortStr, &pEndPtr, 10);

    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count.\n");
        return;
    }

    if ((portCount + 1) < port) {
        goal_cliPrintf("Invalid port number %u. Max. port number is %u.\n", port, portCount);
        return;
    }

    /* Get QoS defprio */
    res = goal_cliParamGet(pData, 5, &pPrioStr, &prioStrLen);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    res = goal_ethCmd(GOAL_ETH_CMD_QOS_MODE, GOAL_FALSE, (port != portCount) ? GOAL_ETH_PORT(port) : GOAL_ETH_PORT_HOST, &(qosMode));
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Failed to retrieve qos mode for port %u\n", pPortStr);
        return;
    }

    defprio = (uint32_t) GOAL_STRTOUL(pPrioStr, &pEndPtr, 10);
    if (CMD_QOS_DEFPRIO_MAX < defprio) {
        goal_cliPrintf("Invalid priority. Valid priority between 0 an %u\n", CMD_QOS_DEFPRIO_MAX);
        return;
    }

    qosMode.defprio = (uint8_t) defprio;

    res = goal_ethCmd(GOAL_ETH_CMD_QOS_MODE, GOAL_TRUE, (port != portCount) ? GOAL_ETH_PORT(port) : GOAL_ETH_PORT_HOST, &(qosMode));
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Failed to retrieve qos mode for port %u\n", pPortStr);
        return;
    }

    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Failed to update default priority for port %s\n.", pPortStr);
    } else {
        goal_cliPrintf("Set qos default priority for port %.*s to %s.\n", portStrLen, pPortStr, pPrioStr);
    }
}


/****************************************************************************/
/** QoS defprio show command execution handler
 */
static void goal_ethCliQosDefprioShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    unsigned int portStrLen;                    /* Param length */
    const char *pPortStr = NULL;                /* Param string pointer */
    uint32_t port = 0;                          /* Port */
    GOAL_BOOL_T showAll = GOAL_TRUE;            /* Limit */
    uint32_t portCount;                         /* Port count */
    char *pEndPtr;                              /* End pointer */
    GOAL_ETH_QOS_MODE_T mode;                   /* QoS mode */
    uint32_t idx;                               /* Index */

    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count.\n");
        return;
    }

    /* Get port param and check whether a valid port */
    res = goal_cliParamGet(pData, 4, &pPortStr, &portStrLen);
    if (GOAL_RES_OK(res)) {
        port = (uint32_t) GOAL_STRTOUL(pPortStr, &pEndPtr, 10);
        if (pEndPtr != (pPortStr + portStrLen)) {
            goal_cliPrintf("Invalid port format\n");
            return;
        }

        if (portCount < port) {
            goal_cliPrintf("Invalid port number %u. Max. port number is %u.\n", port, portCount);
            return;
        }

        showAll = GOAL_FALSE;
    }

    if (!showAll) {
        /* Update rate limiting */
        res = goal_ethCmd(GOAL_ETH_CMD_QOS_MODE, GOAL_FALSE, (port != portCount) ? GOAL_ETH_PORT(port) : GOAL_ETH_PORT_HOST, &mode);
        if (GOAL_RES_OK(res)) {
            goal_cliPrintf("Default priority: %u\n", mode.defprio);
        } else {
            goal_cliPrintf("Failed to update default priority.\n");
        }
    } else {
        goal_cliPrintf("  Port   Priority\n");
        goal_cliPrintf("===================\n");

        for (idx = 0; idx < portCount + 1; idx++) {
            res = goal_ethCmd(GOAL_ETH_CMD_QOS_MODE, GOAL_FALSE, (idx != portCount) ? GOAL_ETH_PORT(idx) : GOAL_ETH_PORT_HOST, &mode);
            if (GOAL_RES_ERR(res)) {
                goal_cliPrintf("Failed to get default priority for port %u%\n", idx);
                return;
            }
            goal_cliPrintf("  %-4u    %-4u\n", idx, mode.defprio);
        }
    }
}


/****************************************************************************/
/** QoS vlan prio set handler
 *
 */
static void goal_ethCliQosVlanPrioSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    uint32_t portCount;                         /* Port count */
    const char *pPortStr;                       /* Port string */
    unsigned int portStrLen;                    /* Port string length */
    const char *pPrioStr;                       /* Prio string */
    unsigned int prioStrLen;                    /* Prio string length */
    const char *pMappingStr;                    /* Mapping string */
    unsigned int mappingStrLen;                 /* Mapping string length */
    char *pEndPtr;                              /* End pointer */
    uint32_t port;                              /* Port */
    GOAL_ETH_QOS_PRIO_VLAN_T vlanPrioCfg;       /* VLAN priority configuration */

    /* Get port param and check whether a valid port */
    res = goal_cliParamGet(pData, 4, &pPortStr, &portStrLen);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Syntax error\n");
        return;
    }
    port = (uint32_t) GOAL_STRTOUL(pPortStr, &pEndPtr, 10);

    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count.\n");
        return;
    }

    if ((portCount + 1) < port) {
        goal_cliPrintf("Invalid port number %u. Max. port number is %u.\n", port, portCount);
        return;
    }

    /* Get QoS vlan prio */
    res = goal_cliParamGet(pData, 5, &pPrioStr, &prioStrLen);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    vlanPrioCfg.pcp = (uint8_t) GOAL_STRTOUL(pPrioStr, &pEndPtr, 10);
    if (CMD_QOS_DEFPRIO_MAX < vlanPrioCfg.pcp) {
        goal_cliPrintf("Invalid priority. Valid priority between 0 an %u\n", CMD_QOS_DEFPRIO_MAX);
        return;
    }

    res = goal_cliParamGet(pData, 6, &pMappingStr, &mappingStrLen);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    vlanPrioCfg.prio = (uint8_t) GOAL_STRTOUL(pMappingStr, &pEndPtr, 10);
    if (CMD_QOS_DEFPRIO_MAX < vlanPrioCfg.prio) {
        goal_cliPrintf("Invalid mapping priority. Valid mapping priority between 0 an %u\n", CMD_QOS_DEFPRIO_MAX);
        return;
    }

    res = goal_ethCmd(GOAL_ETH_CMD_QOS_PRIO_VLAN, GOAL_TRUE, (port != portCount) ? GOAL_ETH_PORT(port) : GOAL_ETH_PORT_HOST, &vlanPrioCfg);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Failed to update VLAN priority mapping for port %s\n.", pPortStr);
    } else {
        goal_cliPrintf("VLAN priority %u for port %u now maps to %u.\n", vlanPrioCfg.pcp, port, vlanPrioCfg.prio);
    }
}


/****************************************************************************/
/** QoS VLAN prio show command execution handler
 */
static void goal_ethCliQosVlanPrioShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    unsigned int portStrLen;                    /* Param length */
    const char *pPortStr = NULL;                /* Param string pointer */
    uint32_t port = 0;                          /* Port */
    uint32_t portCount;                         /* Port count */
    char *pEndPtr;                              /* End pointer */
    uint8_t pcp;                                /* QoS VLAN priority */
    uint32_t cntPort;                           /* port loop counter */
    uint32_t cntPortStart;                      /* port loop counter start value */
    uint32_t cntPortEnd;                        /* port loop counter end value */
    GOAL_ETH_QOS_PRIO_VLAN_T vlanPrioCfg;       /* VLAN priority configuration */

    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count.\n");
        return;
    }

    /* Get port param and check whether a valid port */
    res = goal_cliParamGet(pData, 4, &pPortStr, &portStrLen);
    if (GOAL_RES_OK(res)) {
        port = (uint32_t) GOAL_STRTOUL(pPortStr, &pEndPtr, 10);
        if (pEndPtr != (pPortStr + portStrLen)) {
            goal_cliPrintf("Invalid port format\n");
            return;
        }

        if (portCount < port) {
            goal_cliPrintf("Invalid port number %u. Max. port number is %u.\n", port, portCount);
            return;
        }
        cntPortStart = port;
        cntPortEnd = port + 1;
    }
    else {
        cntPortStart = 0;
        cntPortEnd = portCount + 1;
    }

    goal_cliPrintf("Port  Vlan Prio   Mapped Prio\n");
    goal_cliPrintf("============================\n");

    for (cntPort = cntPortStart; cntPort < cntPortEnd; cntPort++) {
        for (pcp = 0; pcp <= CMD_QOS_DEFPRIO_MAX; pcp++) {
            vlanPrioCfg.pcp = pcp;
            res = goal_ethCmd(GOAL_ETH_CMD_QOS_PRIO_VLAN, GOAL_FALSE, (cntPort != portCount) ? GOAL_ETH_PORT(cntPort) : GOAL_ETH_PORT_HOST, &vlanPrioCfg);
            if (GOAL_RES_ERR(res)) {
                goal_cliPrintf("Failed to get VLAN priority for port %u\n", cntPort);
                return;
            }
            goal_cliPrintf("%2u    %2u          %u\n", cntPort, pcp, vlanPrioCfg.prio);

        }
        goal_cliPrintf("============================\n");
    }
}


/****************************************************************************/
/** QoS IP prio show command execution handler
 */
static void goal_ethCliQosIpPrioShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    GOAL_ETH_QOS_PRIO_IP_T prio;                /* Priority */
    GOAL_BOOL_T showAll = GOAL_TRUE;            /* Limit */
    uint32_t port = 0;                          /* port */
    uint32_t portCount = 0;                     /* port count */
    uint32_t diffserv = 0;                      /* DSCP */
    uint32_t idx = 0;                           /* Index */

    /* Check param count */
    if (goal_cliParamCount(pData) < 5) {
       goal_cliPrintf("Syntax error.\n");
       goal_cliPrintf("Usage: eth qos ipprio show <port> [dscp]\n");
       return;
    }

    /* Get port and port count */
    res = goal_cliParamAsPortGet(pData, 4, &port, &portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count.\n");
        return;
    }

    /* Get entry */
    if (goal_cliParamCount(pData) == 6) {
        res = goal_cliParamAsUint32Get(pData, 5, &diffserv);
        if (GOAL_RES_OK(res)) {
            showAll = GOAL_FALSE;
        }

        if (diffserv > CMD_QOS_IPPRIO_MAX) {
            goal_cliPrintf("Syntax error. Max. DiffServ is %"FMT_u32, CMD_QOS_IPPRIO_MAX);
            return;
        }

        prio.diffserv = (uint8_t) diffserv;

    }

    /* Print the priorities */
    if (!showAll) {
        res = goal_ethCmd(GOAL_ETH_CMD_QOS_PRIO_IP, GOAL_FALSE, (port != portCount) ? GOAL_ETH_PORT(port) : GOAL_ETH_PORT_HOST, &prio);
        goal_cliPrintf("  IP Prio   Mapped Prio\n");
        goal_cliPrintf("============================\n");
        goal_cliPrintf("      %-4u        %-4u\n", prio.diffserv, prio.priority);
        goal_cliPrintf("\n");
    } else {
        /* Update rate limiting */
        if (GOAL_RES_OK(res)) {
            goal_cliPrintf("  IP Prio   Mapped Prio\n");
            goal_cliPrintf("============================\n");
            for (idx = 0; idx <= CMD_QOS_IPPRIO_MAX; idx++) {
                prio.diffserv = (uint8_t) idx;
                res = goal_ethCmd(GOAL_ETH_CMD_QOS_PRIO_IP, GOAL_FALSE, (port != portCount) ? GOAL_ETH_PORT(port) : GOAL_ETH_PORT_HOST, &prio);
                goal_cliPrintf("      %-4u        %-4u\n", prio.diffserv, prio.priority);
            }
        } else {
            goal_cliPrintf("Failed to get IP priority mapping.\n");
        }
    }

}


/****************************************************************************/
/** QoS IP prio set command execution handler
 */
static void goal_ethCliQosIpPrioSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    GOAL_ETH_QOS_PRIO_IP_T prio;                /* Priority */
    uint32_t port = 0;                          /* port */
    uint32_t portCount = 0;                     /* port count */
    uint32_t diffserv = 0;                      /* DSCP */
    uint32_t priority = 0;                      /* Priority */
    uint32_t queueCnt = 0;                      /* Queue count */

    /* Check number of params */
    if (goal_cliParamCount(pData) != 7) {
       goal_cliPrintf("Syntax error.\n");
       goal_cliPrintf("Usage: eth qos ipprio show <port> <dscp> <prio>\n");
       return;
    }

    /* Get queue count */
    res = goal_ethCmd(GOAL_ETH_CMD_QUEUES_PER_PORT, GOAL_FALSE, GOAL_ETH_PORT_HOST, &queueCnt);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve queue count.\n");
        return;
    }

    /* Get port and port count */
    res = goal_cliParamAsPortGet(pData, 4, &port, &portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count.\n");
        return;
    }

    /* Get diffserv */
    res = goal_cliParamAsUint32Get(pData, 5, &diffserv);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Invalid diffserv value\n");
        return;
    }

    if (diffserv > CMD_QOS_IPPRIO_MAX) {
        goal_cliPrintf("Syntax error. Max. DiffServ is %"FMT_u32, CMD_QOS_IPPRIO_MAX);
        return;
    }

    /* Get priority */
    res = goal_cliParamAsUint32Get(pData, 6, &priority);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Invalid priority value");
        return;
    }

     if (priority >= queueCnt) {
        goal_cliPrintf("Syntax error. Max. priority is %"FMT_u32, priority);
        return;
    }

    prio.diffserv = (uint8_t) diffserv;
    prio.priority = (uint8_t) priority;

    /* Set the IP priority */
    res = goal_ethCmd(GOAL_ETH_CMD_QOS_PRIO_IP, GOAL_TRUE, (port != portCount) ? GOAL_ETH_PORT(port) : GOAL_ETH_PORT_HOST, &prio);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Failed to set ip priority\n");
    } else {
        goal_cliPrintf("Updated ip priority.\n");
    }
}


#if GOAL_CONFIG_TDMA == 1
/****************************************************************************/
/** TDMA command execution handler
 */
static void goal_ethCliTdma(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    unsigned int strLen;                        /* Param length */
    const char *pStr = NULL;                    /* Param string pointer */

    res = goal_cliParamGet(pData, 2, &pStr, &strLen);
    if (GOAL_RES_OK(res)) {
        if (GOAL_ETH_CLI_STR_MATCH(pStr, ethStrState)) {
            res = goal_cliParamGet(pData, 3, &pStr, &strLen);
            if (GOAL_RES_OK(res)) {
                if (GOAL_ETH_CLI_STR_MATCH(pStr, ethStrSet)) {
                    goal_ethCliTdmaStateSet(pData);
                    return;
                } else if (GOAL_ETH_CLI_STR_MATCH(pStr, ethStrShow)) {
                    goal_ethCliTdmaStateShow(pData);
                    return;
                }
            }
        } else if (GOAL_ETH_CLI_STR_MATCH(pStr, ethStrConfig)) {
            res = goal_cliParamGet(pData, 3, &pStr, &strLen);
            if (GOAL_RES_OK(res)) {
                if (GOAL_ETH_CLI_STR_MATCH(pStr, ethStrShow)) {
                    goal_ethCliTdmaConfigFullShow(pData);
                    return;
                } else if (GOAL_ETH_CLI_STR_MATCH(pStr, ethStrSet)) {
                    goal_ethCliTdmaConfigSet(pData);
                    return;
                }

            }
        }
        else if (GOAL_ETH_CLI_STR_MATCH(pStr, ethStrTdmaTslt)) {
            res = goal_cliParamGet(pData, 3, &pStr, &strLen);
            if (GOAL_RES_OK(res)) {
                if (GOAL_ETH_CLI_STR_MATCH(pStr, ethStrShow)) {
                    goal_ethCliTdmaTimeslotShow(pData);
                    return;
                } else if (GOAL_ETH_CLI_STR_MATCH(pStr, ethStrSet)) {
                    goal_ethCliTdmaTimeslotSet(pData);
                    return;
                }
            }
        }
    }

    goal_cliPrintf("Syntax error\n");
    goal_ethCliPrintHelp();
}


/****************************************************************************/
/** TDMA state show command execution handler
 */
static void goal_ethCliTdmaStateShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    GOAL_ETH_TDMA_ENABLE_T tdmaEnable;          /* TDMA enable settings */

    UNUSEDARG(pData);

    res = goal_ethCmd(GOAL_ETH_CMD_TDMA_ENABLE, GOAL_FALSE, 0, &tdmaEnable);
    if (GOAL_RES_OK(res)) {
        goal_cliPrintf("TDMA scheduler: %s\n", ((tdmaEnable.enable) ? "on" : "off"));
    } else {
        goal_cliPrintf("Error reading scheduler info: 0x08%"FMT_x32"\n", res);
    }
}


/****************************************************************************/
/** TDMA state set command execution handler
 */
static void goal_ethCliTdmaStateSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    GOAL_ETH_TDMA_ENABLE_T tdmaEnable;          /* TDMA enable settings */
    const char *pStateStr;                      /* state str */
    unsigned int stateStrLen;                   /* state str len */
    GOAL_ETH_TDMA_CONFIG_T config;              /* TDMA config */
    GOAL_TS_TIMESTAMP_T ts;                     /* current time */
    GOAL_TS_HANDLE_T *pGts = NULL;              /* GOAL TS handle */

    /* Get state param */
    res = goal_cliParamGet(pData, 4, &pStateStr, &stateStrLen);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Syntax error\n");
        goal_ethCliPrintHelp();
        return;
    }

    /* Parse state */
    if (CMD_PARAM_ON_LEN == stateStrLen && !strcmp(pStateStr, CMD_PARAM_ON)) {
        tdmaEnable.enable = GOAL_TRUE;
    } else if (CMD_PARAM_OFF_LEN == stateStrLen && !strcmp(pStateStr, CMD_PARAM_OFF)) {
        tdmaEnable.enable = GOAL_FALSE;
    } else {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    if (GOAL_TRUE == tdmaEnable.enable) {
        res = goal_ethCmd(GOAL_ETH_CMD_TDMA_CONFIG, GOAL_FALSE, 0, &config);
        if (GOAL_RES_ERR(res)) {
            goal_cliPrintf("failed to get current cycle time\n");
            return;
        }

        res = goal_instGetById((GOAL_INSTANCE_T **) &pGts, GOAL_ID_PTP, GOAL_TS_INSTANCE_DEFAULT);
        if (GOAL_RES_ERR(res)) {
            goal_cliPrintf("failed to retrieve GOAL TS instance %u\n", GOAL_TS_INSTANCE_DEFAULT);
            return;
        }

        res = goal_tsTimeGet(pGts, &ts.sec, &ts.nsec);
        if (GOAL_RES_ERR(res)) {
            goal_cliPrintf("failes to get local time\n");
            return;
        } else {
            tdmaEnable.tdmaStartTime = ts.nsec / GOAL_TIME_US_TO_NS(config.tdmaCycleTime);
            tdmaEnable.tdmaStartTime = (tdmaEnable.tdmaStartTime + CMD_TDMA_CYCLE_OFFSET) * GOAL_TIME_US_TO_NS(config.tdmaCycleTime);
        }
    }

    /* Set the state via eth command */
    res = goal_ethCmd(GOAL_ETH_CMD_TDMA_ENABLE, GOAL_TRUE, 0, &tdmaEnable);
    if (GOAL_RES_OK(res)) {
        goal_cliPrintf("TDMA scheduler set to %s\n", ((tdmaEnable.enable) ? "on" : "off"));
    } else {
        goal_cliPrintf("Error setting scheduler state: 0x08%"FMT_x32"\n", res);
    }
}


/****************************************************************************/
/** TDMA config show command execution handler
 */
static void goal_ethCliTdmaConfigFullShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    GOAL_ETH_TDMA_CONFIG_T config;              /* TDMA config */
    uint32_t numOfPorts = 0;                    /* number of ethernet ports */

    UNUSEDARG(pData);

    /* get number of ethernet ports */
    res = goal_ethCmd(GOAL_ETH_CMD_PORT_COUNT, GOAL_FALSE, GOAL_ETH_PORT_HOST, &numOfPorts);
    if (GOAL_RES_ERR(res)) {
        numOfPorts = 0;
    }

    GOAL_MEMSET(&config, 0, sizeof(GOAL_ETH_TDMA_CONFIG_T));
    res = goal_ethCmd(GOAL_ETH_CMD_TDMA_CONFIG, GOAL_FALSE, 0, &config);
    if (GOAL_RES_OK(res)) {
        goal_cliPrintf("\n");
        goal_cliPrintf("       Parameter         |     Value\n");
        goal_cliPrintf("=========================================\n");
        goal_cliPrintf(" Cycle time              |  %"FMT_u32"\n", config.tdmaCycleTime);
        goal_cliPrintf(" Port mask               |  ");
        goal_cliPrintMaskParam(config.tdmaPorts, numOfPorts);
        goal_cliPrintf("\n");
        goal_cliPrintf("=========================================\n");
        goal_cliPrintf("\n");
    }
}


/****************************************************************************/
/** TDMA config set command execution handler
 */
static void goal_ethCliTdmaConfigSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    const char *pConfigStr;                     /* Config string pointer */
    unsigned int configStrLen;                  /* Config string length */
    uint32_t value;                             /* Parsed value */
    GOAL_ETH_TDMA_CONFIG_T config;              /* Config to read/write */
    uint32_t portCount;                         /* Port count */

    GOAL_MEMSET(&config, 0, sizeof(GOAL_ETH_TDMA_CONFIG_T));

    /* Get current config */
    res = goal_ethCmd(GOAL_ETH_CMD_TDMA_CONFIG, GOAL_FALSE, 0, &config);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve scheduler config: 0x08%"FMT_x32"\n", res);
        return;
    }

    /* Get port count */
    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count.\n");
        return;
    }

      /* Get config type param */
    res = goal_cliParamGet(pData, 4, &pConfigStr, &configStrLen);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    /* Parse param */
    if (GOAL_ETH_CLI_STR_MATCH(pConfigStr, ethStrTdmaCt)) {
        res = goal_cliParamAsUint32Get(pData, 5, &value);
        if (GOAL_RES_ERR(res)) {
            goal_cliPrintf("Syntax error\n");
            return;
        }
        config.tdmaCycleTime = value;
    } else if (GOAL_ETH_CLI_STR_MATCH(pConfigStr, ethStrTdmaPorts)) {
        res = goal_cliParamAsMaskGet(pData, 5, &value, portCount);
        if (GOAL_RES_ERR(res)) {
            goal_cliPrintf("Syntax error\n");
            return;
        }
        config.tdmaPorts = value;
        config.tdmaPorts |= GOAL_ETH_PORT_MASK;
    } else {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    /* Set the config via eth command */
    res = goal_ethCmd(GOAL_ETH_CMD_TDMA_CONFIG, GOAL_TRUE, 0, &config);
    if (GOAL_RES_OK(res)) {
        goal_cliPrintf("TDMA scheduler settings updated.\n");
    } else {
        goal_cliPrintf("Error settintg scheduler config: 0x08%"FMT_x32"\n", res);
    }
}


/****************************************************************************/
/** TDMA timeslot show command execution handler
 */
static void goal_ethCliTdmaTimeslotShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* result */
    uint32_t tslotNum;                          /* Parsed timeslot number */
    GOAL_ETH_TDMA_TSLT_CFG_T tsltCfg;           /* TDMA timeslot configuration */
    uint32_t queues;                            /* Number of out queues */

    res = goal_cliParamAsUint32Get(pData, 4, &tslotNum);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Syntax error\n");
        goal_ethCliPrintHelp();
        return;
    }

    /* Get number of output queues */
    res = goal_ethCmd(GOAL_ETH_CMD_QUEUES_PER_PORT, GOAL_FALSE, 0, &queues);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve output queue count: 0x08%"FMT_x32"\n", res);
        return;
    }

    /* fetch configuration */
    tsltCfg.timeslotNum = (uint8_t) tslotNum;
    res = goal_ethCmd(GOAL_ETH_CMD_TDMA_TSLT_CFG, GOAL_FALSE, GOAL_ETH_PORT_HOST, &tsltCfg);
    if (GOAL_RES_OK(res)) {
        goal_cliPrintf("\n");
        goal_cliPrintf("       Parameter         |     Value\n");
        goal_cliPrintf("=========================================\n");
        goal_cliPrintf(" time slot               |  %u\n", tsltCfg.timeslotNum);
        goal_cliPrintf(" start offset            |  %"FMT_u32" us\n", tsltCfg.startOffset);
        goal_cliPrintf(" queue mask              |  ");
        goal_cliPrintMaskParam(tsltCfg.queueMsk, queues);
        goal_cliPrintf("\n");
        goal_cliPrintf(" guard band              |  %"FMT_u32" us\n", tsltCfg.guardBand);
        goal_cliPrintf("=========================================\n");
        goal_cliPrintf("\n");
    }
    else {
        goal_cliPrintf("failed to get config for time slot: %"FMT_u32, tslotNum);
    }
}


/****************************************************************************/
/** TDMA timeslot set command execution handler
 */
static void goal_ethCliTdmaTimeslotSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* result */
    uint32_t tslotNum;                          /* Parsed timeslot number */
    GOAL_ETH_TDMA_TSLT_CFG_T tsltCfg;           /* TDMA timeslot configuration */
    uint32_t queues;                            /* Number of out queues */

    /* Get number of output queues */
    res = goal_ethCmd(GOAL_ETH_CMD_QUEUES_PER_PORT, GOAL_FALSE, 0, &queues);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve output queue count: 0x08%"FMT_x32"\n", res);
        return;
    }

    res = goal_cliParamAsUint32Get(pData, 4, &tslotNum);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Syntax error\n");
        goal_ethCliPrintHelp();
        return;
    }
    tsltCfg.timeslotNum = (uint8_t) tslotNum;

    res = goal_cliParamAsUint32Get(pData, 5, &(tsltCfg.startOffset));
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Syntax error\n");
        goal_ethCliPrintHelp();
        return;
    }

    res = goal_cliParamAsMaskGet(pData, 6, &(tsltCfg.queueMsk), queues);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Syntax error\n");
        goal_ethCliPrintHelp();
        return;
    }

    res = goal_cliParamAsUint32Get(pData, 7, &(tsltCfg.guardBand));
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Syntax error\n");
        goal_ethCliPrintHelp();
        return;
    }

    res = goal_ethCmd(GOAL_ETH_CMD_TDMA_TSLT_CFG, GOAL_TRUE, GOAL_ETH_PORT_HOST, &tsltCfg);
    if (GOAL_RES_OK(res)) {
        goal_cliPrintf("TDMA time slot %u settings updated.\n", tsltCfg.timeslotNum);
    } else {
        goal_cliPrintf("TDMA time slot %u settings failed: 0x%08"FMT_x32"\n", tsltCfg.timeslotNum, res);
    }
}
#endif /* GOAL_CONFIG_TDMA */


/****************************************************************************/
/** Command execution handler for 'rxrslv'
 */
static void goal_ethCliRxRslv(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    unsigned int strLen;                        /* Param length */
    const char *pStr = NULL;                    /* Param string pointer */

    res = goal_cliParamGet(pData, 2, &pStr, &strLen);
    if (GOAL_RES_OK(res)) {
        if (pStr == ethStrAdd) {
            goal_ethCliRxRslvAdd(pData);
            return;
        } else if (pStr == ethStrRem) {
            goal_ethCliRxRslvRem(pData);
            return;
        }
    }

    goal_cliPrintf("Syntax error\n");
}


/****************************************************************************/
/** Command execution handler for 'rxrslv add'
 */
static void goal_ethCliRxRslvAdd(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    unsigned int strLen;                        /* Param length */
    const char *pStr = NULL;                    /* Param string pointer */
    GOAL_ETH_MAC_ADDR_T macAddr;                /* MAC address */
    GOAL_ETH_RX_PORT_FILTER_T filter;           /* Rx port filter data */

    /* Get MAC address param */
    res = goal_cliParamGet(pData, 3, &pStr, &strLen);
    if (GOAL_RES_OK(res)) {
        res = goal_ethCliParamAsMac(pStr, strLen, &macAddr);
        if (GOAL_RES_ERR(res)) {
            goal_cliPrintf("Invalid MAC address\n");
            return;
        }
    }

    if (GOAL_RES_OK(res)) {
        filter.type = GOAL_ETH_RX_PORT_FILTER_MAC;
        filter.data.pMacAddr = &macAddr;
        res = goal_ethCmd(GOAL_ETH_CMD_RX_PORT_RES_ADD, GOAL_TRUE, GOAL_ETH_PORT_HOST, &filter);
        if (GOAL_RES_ERR(res)) {
            goal_cliPrintf("Could not add MAC addr to Rx port resolver filter\n");
        }
        return;
    }

    goal_cliPrintf("Syntax error\n");
}


/****************************************************************************/
/** Command execution handler for 'rxrslv rem'
 */
static void goal_ethCliRxRslvRem(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    unsigned int strLen;                        /* Param length */
    const char *pStr = NULL;                    /* Param string pointer */
    GOAL_ETH_MAC_ADDR_T macAddr;                /* MAC address */
    GOAL_ETH_RX_PORT_FILTER_T filter;           /* Rx port filter data */

    /* Get MAC address param */
    res = goal_cliParamGet(pData, 3, &pStr, &strLen);
    if (GOAL_RES_OK(res)) {
        res = goal_ethCliParamAsMac(pStr, strLen, &macAddr);
        if (GOAL_RES_ERR(res)) {
            goal_cliPrintf("Invalid MAC address\n");
            return;
        }
    }

    if (GOAL_RES_OK(res)) {
        filter.type = GOAL_ETH_RX_PORT_FILTER_MAC;
        filter.data.pMacAddr = &macAddr;
        res = goal_ethCmd(GOAL_ETH_CMD_RX_PORT_RES_CLR, GOAL_TRUE, GOAL_ETH_PORT_HOST, &filter);
        if (GOAL_RES_ERR(res)) {
            goal_cliPrintf("Could not remove MAC addr from Rx port resolver filter\n");
        }
        return;
    }

    goal_cliPrintf("Syntax error\n");
}


/****************************************************************************/
/** Command execution handler for 'port mdi state show'
 */
static void goal_ethCliPortMdiStateShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    uint32_t portCount;                         /* Port count */
    unsigned int portStrLen;                    /* Param length */
    const char *pPortStr = NULL;                /* Param string pointer */
    uint32_t cntStart;                          /* loop counter start value */
    uint32_t cntEnd;                            /* loop counter end value */
    uint32_t cntPort;                           /* loop counter value */
    char *pEndPtr;                              /* End pointer */
    uint32_t state;                             /* mdi state */
    GOAL_ETH_CLI_MDISTATE_T *pState;            /* state name resolver */

    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count.\n");
        return;
    }

    /* get port param and check whether it is a valid port */
    res = goal_cliParamGet(pData, 5, &pPortStr, &portStrLen);
    if (GOAL_RES_OK(res)) {
        cntStart = (uint32_t) GOAL_STRTOUL(pPortStr, &pEndPtr, 10);
        if (pEndPtr != (pPortStr + portStrLen)) {
            goal_cliPrintf("Invalid port format\n");
            return;
        }

        /* only external ports allowed */
        if (portCount <= cntStart) {
            goal_cliPrintf("Invalid port number %u. Max. port number is %u.\n", cntStart, portCount - 1);
            return;
        }

        cntEnd = cntStart + 1;
    }
    else {
        cntStart = 0;
        cntEnd = portCount;
    }

    goal_cliPrintf("  Port      MDI state\n");
    goal_cliPrintf("=======================\n");

    for (cntPort = cntStart; cntPort < cntEnd; cntPort++) {
        state = GOAL_ETH_MDI_STATE_UNKNOWN;
        res = goal_ethMdiStateGet(cntPort, &state);
        if (GOAL_RES_ERR(res)) {
            goal_cliPrintf("Failed to get mdi state for port %u\n", cntPort);
            return;
        }
        for (pState = mdiState; NULL != pState->strState; pState++) {
            if (state == pState->state) {
                goal_cliPrintf("   %2u      %-11s\n", cntPort, pState->strState);
            }
        }
    }
}


/****************************************************************************/
/** Command execution handler for 'port mdi state set'
 */
static void goal_ethCliPortMdiStateSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    unsigned int portStrLen;                    /* port param length */
    const char *pPortStr = NULL;                /* port param string pointer */
    unsigned int mdiStateStrLen;                /* mdi state param length */
    const char *pMdiStateStr = NULL;            /* mdi state param string pointer */
    uint32_t port = 0;                          /* Port */
    uint32_t portCount;                         /* Port count */
    char *pEndPtr;                              /* End pointer */
    GOAL_ETH_CLI_MDISTATE_T *pState;            /* state name resolver */

    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count.\n");
        return;
    }

    /* get port param and check whether a valid port */
    res = goal_cliParamGet(pData, 5, &pPortStr, &portStrLen);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    port = (uint32_t) GOAL_STRTOUL(pPortStr, &pEndPtr, 10);
    if (pEndPtr != (pPortStr + portStrLen)) {
        goal_cliPrintf("Invalid port format\n");
        return;
    }

    /* external ports only */
    if (portCount <= port) {
        goal_cliPrintf("Invalid port number %u. Max. port number is %u.\n", port, portCount - 1);
        return;
    }

    /* get mdi state parameter */
    res = goal_cliParamGet(pData, 6, &pMdiStateStr, &mdiStateStrLen);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    /* check for valid value, don't allow "unknown" */
    for (pState = mdiState; NULL != pState->strState; pState++) {
        if ((GOAL_STRLEN(pState->strState) == mdiStateStrLen) &&
            (0 == strncmp(pState->strState, pMdiStateStr, mdiStateStrLen)) &&
            (pState != &mdiState[1])) {
            res = goal_ethMdiStateSet(port, pState->state);
            if (GOAL_RES_ERR(res)) {
                goal_cliPrintf("Failed to update port MDI state.\n");
            }
            return;
        }
    }

    goal_cliPrintf("Syntax error\n");
}


/****************************************************************************/
/** Command execution handler for 'port mdi mode show'
 */
static void goal_ethCliPortMdiModeShow(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    uint32_t portCount;                         /* Port count */
    unsigned int portStrLen;                    /* Param length */
    const char *pPortStr = NULL;                /* Param string pointer */
    uint32_t cntStart;                          /* loop counter start value */
    uint32_t cntEnd;                            /* loop counter end value */
    uint32_t cntPort;                           /* loop counter value */
    char *pEndPtr;                              /* End pointer */
    uint32_t mode;                              /* mdi mode */
    GOAL_ETH_CLI_MDIMODE_T *pMode;              /* mode name resolver */

    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count.\n");
        return;
    }

    /* get port param and check whether it is a valid port */
    res = goal_cliParamGet(pData, 5, &pPortStr, &portStrLen);
    if (GOAL_RES_OK(res)) {
        cntStart = (uint32_t) GOAL_STRTOUL(pPortStr, &pEndPtr, 10);
        if (pEndPtr != (pPortStr + portStrLen)) {
            goal_cliPrintf("Invalid port format\n");
            return;
        }

        /* only external ports allowed */
        if (portCount <= cntStart) {
            goal_cliPrintf("Invalid port number %u. Max. port number is %u.\n", cntStart, portCount - 1);
            return;
        }

        cntEnd = cntStart + 1;
    }
    else {
        cntStart = 0;
        cntEnd = portCount;
    }

    goal_cliPrintf("  Port      MDI mode\n");
    goal_cliPrintf("=======================\n");

    for (cntPort = cntStart; cntPort < cntEnd; cntPort++) {
        mode = GOAL_ETH_MDI_MODE_UNKNOWN;
        res = goal_ethMdiModeGet(cntPort, &mode);
        if (GOAL_RES_ERR(res)) {
            goal_cliPrintf("Failed to get mdi mode for port %u\n", cntPort);
            return;
        }
        for (pMode = mdiMode; NULL != pMode->strMode; pMode++) {
            if (mode == pMode->mode) {
                goal_cliPrintf("   %2u      %-11s\n", cntPort, pMode->strMode);
            }
        }
    }
}


/****************************************************************************/
/** Command execution handler for 'port mdi mode set'
 */
static void goal_ethCliPortMdiModeSet(
    GOAL_CLI_DATA_T *pData                      /**< data pointer */
)
{
    GOAL_STATUS_T res;                          /* Result */
    unsigned int portStrLen;                    /* port param length */
    const char *pPortStr = NULL;                /* port param string pointer */
    unsigned int mdiModeStrLen;                 /* mdi mode param length */
    const char *pMdiModeStr = NULL;             /* mdi mode param string pointer */
    uint32_t port = 0;                          /* Port */
    uint32_t portCount;                         /* Port count */
    char *pEndPtr;                              /* End pointer */
    GOAL_ETH_CLI_MDIMODE_T *pMode;              /* mode name resolver */

    res = goal_ethPortsGet(&portCount);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Could not retrieve port count.\n");
        return;
    }

    /* get port param and check whether a valid port */
    res = goal_cliParamGet(pData, 5, &pPortStr, &portStrLen);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    port = (uint32_t) GOAL_STRTOUL(pPortStr, &pEndPtr, 10);
    if (pEndPtr != (pPortStr + portStrLen)) {
        goal_cliPrintf("Invalid port format\n");
        return;
    }

    /* external ports only */
    if (portCount <= port) {
        goal_cliPrintf("Invalid port number %u. Max. port number is %u.\n", port, portCount - 1);
        return;
    }

    /* get mdi mode parameter */
    res = goal_cliParamGet(pData, 6, &pMdiModeStr, &mdiModeStrLen);
    if (GOAL_RES_ERR(res)) {
        goal_cliPrintf("Syntax error\n");
        return;
    }

    /* check for valid value, don't allow "unknown" */
    for (pMode = mdiMode; NULL != pMode->strMode; pMode++) {
        if ((GOAL_STRLEN(pMode->strMode) == mdiModeStrLen) &&
            (0 == strncmp(pMode->strMode, pMdiModeStr, mdiModeStrLen)) &&
            (pMode != &mdiMode[1])) {
            res = goal_ethMdiModeSet(port, pMode->mode);
            if (GOAL_RES_ERR(res)) {
                goal_cliPrintf("Failed to update port MDI mode.\n");
            }
            return;
        }
    }

    goal_cliPrintf("Syntax error\n");
}


/****************************************************************************/
/** Register 'eth' CLI command
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_ethInitCli(
    void
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_CLI_CMD_T *pHdlCm = NULL;              /* 'cm' handle */
    GOAL_CLI_CMD_T *pHdlTmp = NULL;             /* temporary handle */
    GOAL_CLI_CMD_T *pHdlMac = NULL;             /* 'mac' handle */
    GOAL_CLI_CMD_T *pHdlMode = NULL;            /* 'mode' handle */
    GOAL_CLI_CMD_T *pHdlVlan = NULL;            /* 'vlan' handle */
    GOAL_CLI_CMD_T *pHdlPort = NULL;            /* 'port' handle */
    GOAL_CLI_CMD_T *pHdlDef = NULL;             /* 'default' handle */
    GOAL_CLI_CMD_T *pHdlMactab = NULL;          /* 'mactab' handle */
    GOAL_CLI_CMD_T *pHdlDos = NULL;             /* 'dos' handle */
    GOAL_CLI_CMD_T *pHdlEthPort = NULL;         /* 'port' handle */
    GOAL_CLI_CMD_T *pHdlQos = NULL;             /* 'qos' handle */
#if GOAL_CONFIG_TDMA == 1
    GOAL_CLI_CMD_T *pHdlTdma = NULL;            /* 'tdma' handle */
    GOAL_CLI_CMD_T *pHdlTdmaParam = NULL;       /* tmda param handle */
#endif
    GOAL_CLI_CMD_T *pHdlRxRes = NULL;           /* 'rxrslv' handle */
    GOAL_CLI_CMD_T *pHdlMdi = NULL;             /* 'mdi' handle */

    /* 'eth' main command */
    res = goal_cliCmdReg(GOAL_ETH_CLI_CMD, GOAL_ETH_CLI_HELP, goal_ethCliFunc, NULL, &pHdlCm);

    /* 'eth help' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrHelp, NULL, NULL, pHdlCm, NULL);
    }

    /* 'eth vlan' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrVlan, NULL, NULL, pHdlCm, &pHdlVlan);
    }

    /* 'eth vlan mode' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrMode, NULL, NULL, pHdlVlan, &pHdlMode);
    }

    /* 'eth vlan mode in' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrIn, NULL, NULL, pHdlMode, &pHdlTmp);
    }

    /* 'eth vlan mode in show' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrShow, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth vlan mode in set' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrSet, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth vlan mode out' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrOut, NULL, NULL, pHdlMode, &pHdlTmp);
    }

    /* 'eth vlan mode out show' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrShow, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth vlan mode out set' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrSet, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth vlan port' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrPort, NULL, NULL, pHdlVlan, &pHdlPort);
    }

    /* 'eth vlan port add' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrAdd, NULL, NULL, pHdlPort, NULL);
    }

    /* 'eth vlan port rem' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrRem, NULL, NULL, pHdlPort, NULL);
    }

    /* 'eth vlan default' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrDef, NULL, NULL, pHdlVlan, &pHdlDef);
    }

    /* 'eth vlan default set' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrSet, NULL, NULL, pHdlDef, NULL);
    }

    /* 'eth vlan default show' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrShow, NULL, NULL, pHdlDef, NULL);
    }

    /* 'eth vlan table' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrTable, NULL, NULL, pHdlVlan, &pHdlTmp);
    }

    /* 'eth vlan table show' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrShow, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth vlan verify */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrVerify, NULL, NULL, pHdlVlan, &pHdlTmp);
    }

    /* 'eth vlan verify set */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrSet, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth vlan verify show */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrShow, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth vlan discunknown */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrDisc, NULL, NULL, pHdlVlan, &pHdlTmp);
    }

    /* 'eth vlan discunknown set */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrSet, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth vlan discunknown show */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrShow, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth mactab' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrMactab, NULL, NULL, pHdlCm, &pHdlMactab);
    }

    /* 'eth mactab conf' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrConf, NULL, NULL, pHdlMactab, &pHdlTmp);
    }

    /* 'eth mactab conf set' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrSet, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth mactab conf show' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrShow, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth mactab mac' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrMac, NULL, NULL, pHdlMactab, &pHdlMac);
    }

    /* 'eth mactab mac add' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrAdd, NULL, NULL, pHdlMac, NULL);
    }

    /* 'eth mactab mac rem' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrRem, NULL, NULL, pHdlMac, NULL);
    }

    /* 'eth mactab mac clear' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrClear, NULL, NULL, pHdlMac, &pHdlTmp);
    }

    /* 'eth mactab mac clear all' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrAll, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth mactab mac clear dynamic' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrDynamic, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth mactab mac clear static' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrStatic, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth mactab mac show' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrShow, NULL, NULL, pHdlMac, NULL);
    }

    /* 'eth dos' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrDos, NULL, NULL, pHdlCm, &pHdlDos);
    }

    /* 'eth dos txrate' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrTxrate, NULL, NULL, pHdlDos, &pHdlTmp);
    }

    /* 'eth dos txrate set' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrSet, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth dos txrate show' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrShow, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth dos timebase' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrTimebase, NULL, NULL, pHdlDos, &pHdlTmp);
    }

    /* 'eth dos timebase set' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrSet, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth dos timebase show' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrShow, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth dos mlimit' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrMlimit, NULL, NULL, pHdlDos, &pHdlTmp);
    }

    /* 'eth dos mlimit set' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrSet, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth dos mlimit show' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrShow, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth dos blimit' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrBlimit, NULL, NULL, pHdlDos, &pHdlTmp);
    }

    /* 'eth dos blimit set' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrSet, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth dos blimit show' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrShow, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth port' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrPort, NULL, NULL, pHdlCm, &pHdlEthPort);
    }

    /* 'eth port adstate' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrAdstate, NULL, NULL, pHdlEthPort, &pHdlTmp);
    }

    /* 'eth port adstate set' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrSet, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth port adstate show' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrShow, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth port state' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrState, NULL, NULL, pHdlEthPort, &pHdlTmp);
    }

    /* 'eth port state set' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrSet, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth port state show' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrShow, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth port speed' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrSpeed, NULL, NULL, pHdlEthPort, &pHdlTmp);
    }

    /* 'eth port speed show' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrShow, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth port speed set' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrSet, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth port link' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrLink, NULL, NULL, pHdlEthPort, &pHdlTmp);
    }

    /* 'eth port link show' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrShow, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth port duplex' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrDuplex, NULL, NULL, pHdlEthPort, &pHdlTmp);
    }

    /* 'eth port duplex show' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrShow, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth port duplex set' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrSet, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth port autoneg' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrAutoneg, NULL, NULL, pHdlEthPort, &pHdlTmp);
    }

    /* 'eth port autoneg show' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrShow, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth port autoneg set' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrSet, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth port mirror' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrMirror, NULL, NULL, pHdlEthPort, &pHdlTmp);
    }

    /* 'eth port mirror set' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrSet, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth port mirror show' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrShow, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth port mdi' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrMdi, NULL, NULL, pHdlEthPort, &pHdlMdi);
    }

    /* 'eth port mdi mode' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrMode, NULL, NULL, pHdlMdi, &pHdlTmp);
    }

    /* 'eth port mdi mode set' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrSet, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth port mdi mode show' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrShow, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth port mdi state' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrState, NULL, NULL, pHdlMdi, &pHdlTmp);
    }

    /* 'eth port mdi state set' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrSet, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth port mdi state show' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrShow, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth qos' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrQos, NULL, NULL, pHdlCm, &pHdlQos);
    }

    /* 'eth qos mode' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrMode, NULL, NULL, pHdlQos, &pHdlTmp);
    }

    /* 'eth qos mode set' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrSet, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth qos mode show' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrShow, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth qos defprio' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrDefprio, NULL, NULL, pHdlQos, &pHdlTmp);
    }

    /* 'eth qos defprio set' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrSet, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth qos defprio show' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrShow, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth qos vlanprio' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrVlanprio, NULL, NULL, pHdlQos, &pHdlTmp);
    }

    /* 'eth qos vlanprio set' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrSet, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth qos vlanprio show' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrShow, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth qos ipprio' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrIpprio, NULL, NULL, pHdlQos, &pHdlTmp);
    }

    /* 'eth qos ipprio set' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrSet, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth qos ipprio show' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrShow, NULL, NULL, pHdlTmp, NULL);
    }

#if GOAL_CONFIG_TDMA == 1
    /* 'eth tdma' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrTdma, NULL, NULL, pHdlCm, &pHdlTdma);
    }

    /* 'eth tdma state' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrState, NULL, NULL, pHdlTdma, &pHdlTmp);
    }

    /* 'eth tdma state show' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrShow, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth tdma state set' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrSet, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth tdma config' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrConfig, NULL, NULL, pHdlTdma, &pHdlTdmaParam);
    }

    /* 'eth tdma config show' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrShow, NULL, NULL, pHdlTdmaParam, NULL);
    }

    /* 'eth tdma config ct' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrTdmaCt, NULL, NULL, pHdlTdmaParam, &pHdlTmp);
    }

    /* 'eth tdma config ct set' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrSet, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth tdma config ports' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrTdmaPorts, NULL, NULL, pHdlTdmaParam, &pHdlTmp);
    }

    /* 'eth tdma config ports set' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrSet, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth tdma timeslot' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrTdmaTslt, NULL, NULL, pHdlTdma, &pHdlTmp);
    }

    /* 'eth tdma timeslot show' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrShow, NULL, NULL, pHdlTmp, NULL);
    }

    /* 'eth tdma timeslot set' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrSet, NULL, NULL, pHdlTmp, NULL);
    }




#endif /* GOAL_CONFIG_TDMA */

    /* 'eth rxrslv' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrRxRslv, NULL, NULL, pHdlCm, &pHdlRxRes);
    }

    /* 'eth rxrslv add' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrAdd, NULL, NULL, pHdlRxRes, NULL);
    }

    /* 'eth rxrslv rem' */
    if (GOAL_RES_OK(res)) {
        res = goal_cliCmdReg(ethStrRem, NULL, NULL, pHdlRxRes, NULL);
    }

    return res;
}

#endif /* GOAL_CONFIG_CLI */

