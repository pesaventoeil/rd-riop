/** @file
 *
 * @brief GOAL Network Interface Handling
 *
 * This module implements the interface for managing the TCP/IP stack.
 *
 * @copyright
 * Copyright 2010-2025.
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

#define GOAL_ID GOAL_ID_NET
#include <goal_includes.h>
#include "cm/goal_net_cm.h"

#if GOAL_CONFIG_TCPIP_STACK == 1


/****************************************************************************/
/* local variables */
/****************************************************************************/
static GOAL_STAGE_HANDLER_T stageInit;          /**< init stage handler */
#if GOAL_CONFIG_CLI == 1
static GOAL_STAGE_HANDLER_T stageCli;           /**< CLI init stage handler */
#endif
#if GOAL_CONFIG_MEDIA_MI_ETH == 0
static GOAL_STAGE_HANDLER_T stageMiNetOpen;     /**< open stage handler */
#endif
static GOAL_STAGE_HANDLER_T stageShutdown;      /**< shutdown stage handler */
static uint32_t m_cmVarIdMask = 0;              /**< mask of used CM Var instance IDs */
static const uint32_t m_virtCmVarOffset = (uint32_t) (GOAL_CM_NET_VIRT_VAR_LAST - NET_CM_VAR_COMMIT);


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/
static GOAL_STATUS_T goal_miNetInit(
    void
);

static GOAL_STATUS_T goal_miNetShutdown(
    void
);

static GOAL_STATUS_T goal_miNetIpDefaultValidCheck(
    uint32_t addrIp,                            /**< IP address */
    uint32_t addrMask,                          /**< subnet mask */
    uint32_t addrGw                             /**< gateway */
);

#if GOAL_CONFIG_MEDIA_MI_ETH == 0
static GOAL_STATUS_T goal_miNetOpenReg(
    void
);
#endif


/****************************************************************************/
/** Get CM Variable for a specific Net MI instance
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static inline GOAL_STATUS_T goal_miNetCmVarGetById(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI handle */
    uint32_t modId,                             /**< Module ID of the var to search for */
    uint32_t varId,                             /**< Variable ID of the var to search for */
    GOAL_CM_VAR_T **ppVar                       /**< Pointer to the var to search for */
)
{
    if (NULL == pMiNetHdl) {
        return GOAL_ERR_NULL_POINTER;
    }

    return goal_cmGetVarById(modId, varId + (pMiNetHdl->cmVarInstId * GOAL_CM_NET_VAR_LAST), ppVar);
}


/****************************************************************************/
/** Get Virtual CM Variable for a specific Net MI instance
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static inline GOAL_STATUS_T goal_miNetCmVirtVarGetById(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI handle */
    uint32_t modId,                             /**< Module ID of the var to search for */
    uint32_t varId,                             /**< Variable ID of the var to search for */
    GOAL_CM_VAR_T **ppVar                       /**< Pointer to the var to search for */
)
{

    if (NULL == pMiNetHdl) {
        return GOAL_ERR_NULL_POINTER;
    }

    return goal_cmGetVarById(modId, varId + (pMiNetHdl->cmVarInstId * m_virtCmVarOffset), ppVar);
}


/****************************************************************************/
/** Register stage handler
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_netInitPre(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* init stage */
    res = goal_mainStageReg(GOAL_STAGE_NET_PRE, &stageInit, GOAL_STAGE_INIT, goal_miNetInit);

#if GOAL_CONFIG_CLI == 1
    /* CLI stage */
    if (GOAL_RES_OK(res)) {
        res = goal_mainStageReg(GOAL_STAGE_CLI, &stageCli, GOAL_STAGE_INIT, goal_netCliInit);
    }
#endif

#if GOAL_CONFIG_MEDIA_MI_ETH == 0
    /* mi open stage */
    if (GOAL_RES_OK(res)) {
        res = goal_mainStageReg(GOAL_STAGE_NET_PRE, &stageMiNetOpen, GOAL_STAGE_INIT, goal_miNetOpenReg);
    }
#endif

    /* shutdown stage */
    if (GOAL_RES_OK(res)) {
        res = goal_mainStageReg(GOAL_STAGE_NET_PRE, &stageShutdown, GOAL_STAGE_SHUTDOWN, goal_miNetShutdown);
    }

    goal_netRegCmVars();

    return res;
}


/****************************************************************************/
/** Initialize Network Handling
 *
 * Initialize all channels for the TCP/IP stack.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
static GOAL_STATUS_T goal_miNetInit(
    void
)
{
    return goal_queuePoolBufsReq(GOAL_ID_NET, GOAL_NETBUF_SIZE, GOAL_CONFIG_BUF_NUM, 0);
}


/****************************************************************************/
/** Bind Media Adapter Handle and Media Interface Handle
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_miNetMaReg(
    uint32_t idMa,                              /**< ID MI */
    GOAL_MI_NET_T *pMiNetHdl                    /**< [in] MI handle */
)
{
    GOAL_MA_NET_T *pMaHdl;                      /* MA handle */
    GOAL_STATUS_T res;                          /* result */

    /* catch nullpointer */
    if (NULL == pMiNetHdl) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* get ma handle and bind together */
    res = goal_maNetGetById(&pMaHdl, idMa);
    if (GOAL_RES_OK(res)) {
        pMiNetHdl->pMaNet = pMaHdl;
        pMaHdl->pMiNet = pMiNetHdl;
    }

    return res;
}

#if GOAL_CONFIG_MEDIA_MI_ETH == 0
/****************************************************************************/
/** Open default Network Instance
 *
 * This function opens a default Network instance and assign it to
 * the board specific Network driver. This function is necessary to
 * keep the old API usable.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_miNetOpenReg(
    void
)
{
    return goal_miNetOpen(GOAL_ID_DEFAULT, GOAL_ID_DEFAULT);
}
#endif


/****************************************************************************/
/** Open Media Interface Net
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_miNetOpen(
    uint32_t idMi,                              /**< MI id */
    uint32_t idMa                               /**< MA id */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    uint32_t addrIp = 0;                        /* IP address */
    uint32_t addrMask = 0;                      /* netmask */
    uint32_t addrGw = 0;                        /* gateway */
    GOAL_BOOL_T flgValid = GOAL_FALSE;          /* IP valid flag */
    GOAL_BOOL_T flgTemp = GOAL_TRUE;            /* IP temp flag */
    GOAL_BOOL_T flgDefaultIpUse = GOAL_FALSE;   /* usage of default IP flag */
    GOAL_BOOL_T flgDhcpEnabled = GOAL_FALSE;    /* DHCP enabled flag */
    GOAL_MI_NET_T *pMiNetHdl = NULL;            /* MI handle */
    GOAL_MA_NET_T *pMaHdl = NULL;               /* MA handle */
    GOAL_NET_CB_LIST_NET_UP_T *pEntry;          /* callback list entry */
    uint32_t cntCmVarId = UINT32_MAX;           /* counter of available CM Var instance IDs */

    /* check if instance was already created */
    if (GOAL_RES_OK(res)) {
        res = goal_instGetById((GOAL_INSTANCE_T **) &pMiNetHdl, GOAL_ID, idMi);
        if (GOAL_RES_OK(res)) {
            if (GOAL_TRUE == pMiNetHdl->flgOpen) {
                /* don't open twice */
                return GOAL_OK;
            }
        }
    }

    /* allocate memory and create instance */
    if (GOAL_RES_ERR(res)) {
        res = goal_instNew((GOAL_INSTANCE_T **) &pMiNetHdl, sizeof(GOAL_MI_NET_T), GOAL_ID,
                           idMi, "Net Instance");
    }

    /* store data in instance */
    if (GOAL_RES_OK(res)) {
        pMiNetHdl->pMaNet = NULL;

        /* bind MA to MI */
        res = goal_miNetMaReg(idMa, pMiNetHdl);
    }

    /*  open Net MA */
    if (GOAL_RES_OK(res)) {
        pMaHdl = (GOAL_MA_NET_T *) pMiNetHdl->pMaNet;
        res = goal_maNetOpen(pMaHdl);
    }

    if (GOAL_RES_OK(res)) {
        for (cntCmVarId = 0; cntCmVarId < GOAL_CONFIG_NUM_INTERFACES; cntCmVarId++) {
            if (0 == (m_cmVarIdMask & (1 << cntCmVarId))) {
                m_cmVarIdMask |= (1 << cntCmVarId);
                pMiNetHdl->cmVarInstId = cntCmVarId;
                break;
            }
        }

        if (GOAL_CONFIG_NUM_INTERFACES <= cntCmVarId) {
            goal_logErr("no free set of CM variables available. Increase GOAL_CONFIG_NUM_INTERFACES.");
            res = GOAL_ERROR;
        }
    }

#if GOAL_CONFIG_DHCP == 1
    /* initialize DHCP client */
    if (GOAL_RES_OK(res)) {
        res = goal_netDhcpInit(pMiNetHdl);
    }
#endif

    /* read IP settings from TCP/IP stack */
    res = goal_maNetIpGet(pMaHdl, &addrIp, &addrMask, &addrGw, NULL);
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:158: */
        goal_lmLog(GOAL_ID_NET, 158, 127, 0, 0, GOAL_LOG_SEV_ERROR, "failed to get IP configuration");
    }

    /* if we get no valid settings, check the non volatile settings */
    if (GOAL_RES_ERR(res) || (0 == addrIp)) {

        /* check DHCP status, ignore result */
        goal_miNetCmDhcpGet(pMiNetHdl, &flgDhcpEnabled);

        /* get ip address from CM */
        res = goal_miNetIpGet(pMiNetHdl, &addrIp, &addrMask, &addrGw, &flgTemp);
        if (GOAL_RES_OK(res) && (GOAL_TRUE == flgTemp)) {
            if (GOAL_FALSE == flgDhcpEnabled) {
                /* if temp. flag is set (IP not valid) and DHCP is not set, use default IP */
                flgDefaultIpUse = GOAL_TRUE;
            }
        } else if (GOAL_RES_ERR(res)) {
            /* use default IP, if reading ip address from CM failed */
            flgDefaultIpUse = GOAL_TRUE;
        }
    }

    /* check IP configuration of CM or target */
    if (GOAL_RES_OK(res) && (GOAL_FALSE == flgDefaultIpUse) && (GOAL_FALSE == flgDhcpEnabled)) {
        res = goal_miNetIpValidCheck(pMiNetHdl, addrIp, addrMask, addrGw, GOAL_TRUE);
        if (GOAL_RES_ERR(res)) {
            /* use default IP */
            flgDefaultIpUse = GOAL_TRUE;
        }
    }

    /* if either system or nvs don't give use valid settings, set the default IP */
    if (GOAL_RES_ERR(res) || (GOAL_TRUE == flgDefaultIpUse)) {
        /* use default net address */
        addrIp = GOAL_CONFIG_NET_ADDR_IP_DEFAULT;
        addrMask = GOAL_CONFIG_NET_ADDR_MASK_DEFAULT;
        addrGw = GOAL_CONFIG_NET_ADDR_GATEWAY_DEFAULT;
        flgTemp = GOAL_TRUE;

        res = GOAL_OK;
    }

    /* sync config with CM */
    if (GOAL_RES_OK(res)) {
        flgValid = (GOAL_TRUE == flgTemp) ? GOAL_FALSE : GOAL_TRUE;
        res = goal_miNetCmIpSet(pMiNetHdl, addrIp, addrMask, addrGw, flgValid);
        if (GOAL_RES_ERR(res)) {
            /* GG_LOG ID:159: */
            goal_lmLog(GOAL_ID_NET, 159, 128, 0, 0, GOAL_LOG_SEV_ERROR, "failed to update IP configuration");
        }
    }

    if (GOAL_RES_OK(res)) {
        /* use stored IP, except DHCP is enabled */
        res = goal_miNetCmIpCommit(pMiNetHdl);

        if (GOAL_FALSE == flgDhcpEnabled) {
            /* GG_LOG ID:30: */
            goal_lmLog(GOAL_ID_NET, 30, 20, 4, 0, GOAL_LOG_SEV_INFO, "ip address: $1");
            goal_lmLogParamIPV4(addrIp);
            goal_lmLogFinish();
            /* GG_LOG ID:31: */
            goal_lmLog(GOAL_ID_NET, 31, 21, 4, 0, GOAL_LOG_SEV_INFO, "netmask: $1");
            goal_lmLogParamIPV4(addrMask);
            goal_lmLogFinish();
            /* GG_LOG ID:32: */
            goal_lmLog(GOAL_ID_NET, 32, 22, 4, 0, GOAL_LOG_SEV_INFO, "gateway: $1");
            goal_lmLogParamIPV4(addrGw);
            goal_lmLogFinish();
            /* GG_LOG ID:33: */
            goal_lmLog(GOAL_ID_NET, 33, 23, 1, 0, GOAL_LOG_SEV_INFO, "IP valid flag: $1");
            goal_lmLogParamBOOLEAN(flgValid);
            goal_lmLogFinish();
        }
    }

    if (GOAL_RES_OK(res)) {
        res = goal_miNetCbIpVerifyReg(pMiNetHdl, goal_miNetIpDefaultValidCheck);
    }

    if (GOAL_RES_OK(res)) {
        pMiNetHdl->flgOpen = GOAL_TRUE;
    }
    else if (UINT32_MAX != cntCmVarId) {
        /* flag was already set, clear it */
        m_cmVarIdMask &= ~(1 << cntCmVarId);
    }

    if (GOAL_RES_OK(res)) {
        /* inform callback listeners */
        GOAL_LL_FOREACH(pMiNetHdl->pListCbNetUp, pEntry) {
            pEntry->func(pMiNetHdl, pMiNetHdl->idInst);
        }
    }

    return res;
}


/****************************************************************************/
/** Shutdown network handling
 *
 * This function closes all open channels.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
static GOAL_STATUS_T goal_miNetShutdown(
    void
)
{
    /* GG_LOG ID:183: */
    goal_lmLog(GOAL_ID_NET, 183, 148, 0, 0, GOAL_LOG_SEV_INFO, "network shutdown");

    return GOAL_OK;
}


/****************************************************************************/
/** Generic network command caller
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_miNetCmd(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI handle */
    GOAL_NET_CMD_T cmd,                         /**< command */
    GOAL_BOOL_T wrFlag,                         /**< write flag */
    void *pArg                                  /**< argument */
)
{
    GOAL_STATUS_T res;                          /* result */

    if (NULL == pMiNetHdl) {
        /* get handle of default interface */
        res = goal_instGetById((GOAL_INSTANCE_T **) &pMiNetHdl, GOAL_ID_NET, GOAL_ID_DEFAULT);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
    }

#if GOAL_CONFIG_DHCP == 1
    switch (cmd) {
        case GOAL_NET_CMD_DHCP_START:
            return goal_netDhcpStart(pMiNetHdl);

        case GOAL_NET_CMD_DHCP_STOP:
            return goal_netDhcpStop(pMiNetHdl);

        case GOAL_NET_CMD_DHCP_RELEASE:
            return goal_netDhcpRelease(pMiNetHdl, (GOAL_BOOL_T *) pArg);

        case GOAL_NET_CMD_DHCP_STATE:
            return goal_netDhcpGetState(pMiNetHdl, (uint32_t *) pArg);

        case GOAL_NET_CMD_DHCP_ADDR:
            return goal_netDhcpGetAddr(pMiNetHdl, (GOAL_NET_DHCP_T *) pArg);

        default:
            /* pass command to net driver */
            break;
    }
#endif /* GOAL_CONFIG_DHCP == 1 */

    /* call network command handler */
    return goal_maNetCmd(pMiNetHdl->pMaNet, cmd, wrFlag, pArg);
}


/****************************************************************************/
/** Set IP Address
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_miNetIpSet(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI handle */
    uint32_t addrIp,                            /**< IP address */
    uint32_t addrMask,                          /**< subnet mask */
    uint32_t addrGw,                            /**< gateway */
    GOAL_BOOL_T flgTemp                         /**< temporary IP config flag */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_NET_CB_LIST_IP_SET_T *pEntry;          /* callback list entry */

    if (NULL == pMiNetHdl) {
        /* get handle of default interface */
        res = goal_instGetById((GOAL_INSTANCE_T **) &pMiNetHdl, GOAL_ID_NET, GOAL_ID_DEFAULT);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
    }

    res = goal_miNetIpValidCheck(pMiNetHdl, addrIp, addrMask, addrGw, flgTemp);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* GG_LOG ID:34: */
    goal_lmLog(GOAL_ID_NET, 34, 20, 4, 0, GOAL_LOG_SEV_INFO, "ip address: $1");
    goal_lmLogParamIPV4(addrIp);
    goal_lmLogFinish();
    /* GG_LOG ID:35: */
    goal_lmLog(GOAL_ID_NET, 35, 21, 4, 0, GOAL_LOG_SEV_INFO, "netmask: $1");
    goal_lmLogParamIPV4(addrMask);
    goal_lmLogFinish();
    /* GG_LOG ID:36: */
    goal_lmLog(GOAL_ID_NET, 36, 22, 4, 0, GOAL_LOG_SEV_INFO, "gateway: $1");
    goal_lmLogParamIPV4(addrGw);
    goal_lmLogFinish();
    /* GG_LOG ID:37: */
    goal_lmLog(GOAL_ID_NET, 37, 23, 1, 0, GOAL_LOG_SEV_INFO, "IP valid flag: $1");
    goal_lmLogParamBOOLEAN((GOAL_BOOL_T) !flgTemp);
    goal_lmLogFinish();

    /* update IP address */
    res = goal_maNetIpSet(pMiNetHdl->pMaNet, addrIp, addrMask, addrGw, flgTemp);
    if (GOAL_RES_OK(res)) {
        /* GG_LOG ID:195: */
        goal_lmLog(GOAL_ID_NET, 195, 156, 0, 0, GOAL_LOG_SEV_INFO, "successfully updated IP configuration");
    } else {
        /* GG_LOG ID:196: */
        goal_lmLog(GOAL_ID_NET, 196, 157, 0, 0, GOAL_LOG_SEV_ERROR, "failed to set IP configuration");
    }

    /* sync config with CM */
    res = goal_miNetCmIpSet(pMiNetHdl, addrIp, addrMask, addrGw, (GOAL_FALSE == flgTemp) ? GOAL_TRUE : GOAL_FALSE);
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:197: */
        goal_lmLog(GOAL_ID_NET, 197, 128, 0, 0, GOAL_LOG_SEV_ERROR, "failed to update IP configuration");
        return res;
    }

    /* inform callback listeners */
    GOAL_LL_FOREACH(pMiNetHdl->pListCbIpSet, pEntry) {
        pEntry->func(pMiNetHdl, addrIp, addrMask, addrGw, flgTemp, pEntry->pArg);
    }

    return res;
}


/****************************************************************************/
/** Get IP Address
 *
 * Unneeded pointers can be set to NULL.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miNetIpGet(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI handle */
    uint32_t *pAddrIp,                          /**< IP address */
    uint32_t *pAddrMask,                        /**< subnet mask */
    uint32_t *pAddrGw,                          /**< gateway */
    GOAL_BOOL_T *pFlgTemp                       /**< temporary IP config flag */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_BOOL_T flgValid = GOAL_FALSE;          /* IP valid flag */
    GOAL_CM_VAR_T *pCmVar = NULL;               /* CM variable */

    if (NULL == pMiNetHdl) {
        /* get handle of default interface */
        res = goal_instGetById((GOAL_INSTANCE_T **) &pMiNetHdl, GOAL_ID_NET, GOAL_ID_DEFAULT);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
    }

    /* check NULL pointers */
    if (pAddrIp) {
        res = goal_miNetCmVarGetById(pMiNetHdl, GOAL_CM_NET_MOD_ID, NET_CM_VAR_IP, &pCmVar);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
        *pAddrIp = GOAL_CM_VAR_UINT32(pCmVar);
    }

    if (pAddrMask) {
        res = goal_miNetCmVarGetById(pMiNetHdl, GOAL_CM_NET_MOD_ID, NET_CM_VAR_NETMASK, &pCmVar);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
        *pAddrMask = GOAL_CM_VAR_UINT32(pCmVar);
    }

    if (pAddrGw) {
        res = goal_miNetCmVarGetById(pMiNetHdl, GOAL_CM_NET_MOD_ID, NET_CM_VAR_GW, &pCmVar);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
        *pAddrGw = GOAL_CM_VAR_UINT32(pCmVar);
    }

    if (pFlgTemp) {
        res = goal_miNetFlgValidGet(pMiNetHdl, &flgValid);
        if (GOAL_RES_ERR(res)) {
            /* GG_LOG ID:198: */
            goal_lmLog(GOAL_ID_NET, 198, 158, 0, 0, GOAL_LOG_SEV_ERROR, "failed to retrieve valid flag");
            return res;
        }
        *pFlgTemp = (GOAL_FALSE == flgValid) ? GOAL_TRUE : GOAL_FALSE;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Check IP Address for Validity
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miNetIpValidCheck(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI handle */
    uint32_t addrIp,                            /**< IP address */
    uint32_t addrMask,                          /**< subnet mask */
    uint32_t addrGw,                            /**< gateway */
    GOAL_BOOL_T flgTemp                         /**< temporary IP config flag */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_NET_CB_LIST_IP_VERIFY_T *pEntry;       /* callback list entry */

    if (NULL == pMiNetHdl) {
        /* get handle of default interface */
        res = goal_instGetById((GOAL_INSTANCE_T **) &pMiNetHdl, GOAL_ID_NET, GOAL_ID_DEFAULT);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
    }

    UNUSEDARG(flgTemp);

    /* verify IP settings */
    GOAL_LL_FOREACH(pMiNetHdl->pListCbIpVerify, pEntry) {
        res = pEntry->func(addrIp, addrMask, addrGw);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("verification of IP settings failed");
            return res;
        }
    }

    return res;
}


/****************************************************************************/
/** Update DHCP Configuration in CM
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_miNetCmDhcpSet(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI handle */
    GOAL_BOOL_T enableDhcp                      /**< flag to enable DHCP */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_CM_VAR_T *pCmVar = NULL;               /* CM variable */

    if (NULL == pMiNetHdl) {
        /* get handle of default interface */
        res = goal_instGetById((GOAL_INSTANCE_T **) &pMiNetHdl, GOAL_ID_NET, GOAL_ID_DEFAULT);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
    }

    res = goal_miNetCmVarGetById(pMiNetHdl, GOAL_CM_NET_MOD_ID, NET_CM_VAR_DHCP_ENABLED, &pCmVar);
    if (GOAL_RES_OK(res)) {
        GOAL_CM_SETVAR_UINT8(pCmVar, (uint8_t) enableDhcp);
    }

    return res;
}


/****************************************************************************/
/** Get DHCP Configuration in CM
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_miNetCmDhcpGet(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI handle */
    GOAL_BOOL_T *pEnableDhcp                    /**< DHCP enable status */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_CM_VAR_T *pCmVar = NULL;               /* CM variable */

    if (NULL == pMiNetHdl) {
        /* get handle of default interface */
        res = goal_instGetById((GOAL_INSTANCE_T **) &pMiNetHdl, GOAL_ID_NET, GOAL_ID_DEFAULT);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
    }

    if (NULL == pEnableDhcp) {
        res = GOAL_ERR_NULL_POINTER;
    }

    if (GOAL_RES_OK(res)) {
        res = goal_miNetCmVarGetById(pMiNetHdl, GOAL_CM_NET_MOD_ID, NET_CM_VAR_DHCP_ENABLED, &pCmVar);
    }

    if (GOAL_RES_OK(res)) {
        *pEnableDhcp = (GOAL_CM_VAR_UINT8(pCmVar)) ? GOAL_TRUE : GOAL_FALSE;
    }

    return res;
}
/****************************************************************************/
/** Update IP Configuration in CM
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_miNetCmIpSet(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI handle */
    uint32_t addrIp,                            /**< IP address */
    uint32_t addrMask,                          /**< subnet mask */
    uint32_t addrGw,                            /**< gateway */
    GOAL_BOOL_T flgValid                        /**< valid flag */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_CM_VAR_T *pCmVar = NULL;               /* CM variable */

    if (NULL == pMiNetHdl) {
        /* get handle of default interface */
        res = goal_instGetById((GOAL_INSTANCE_T **) &pMiNetHdl, GOAL_ID_NET, GOAL_ID_DEFAULT);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
    }

    res = goal_miNetCmVarGetById(pMiNetHdl, GOAL_CM_NET_MOD_ID, NET_CM_VAR_IP, &pCmVar);
    if (GOAL_RES_ERR(res)) {
        return res;
    }
    GOAL_CM_SETVAR_UINT32(pCmVar, addrIp);

    res = goal_miNetCmVarGetById(pMiNetHdl, GOAL_CM_NET_MOD_ID, NET_CM_VAR_NETMASK, &pCmVar);
    if (GOAL_RES_ERR(res)) {
        return res;
    }
    GOAL_CM_SETVAR_UINT32(pCmVar, addrMask);

    res = goal_miNetCmVarGetById(pMiNetHdl, GOAL_CM_NET_MOD_ID, NET_CM_VAR_GW, &pCmVar);
    if (GOAL_RES_ERR(res)) {
        return res;
    }
    GOAL_CM_SETVAR_UINT32(pCmVar, addrGw);

    return goal_miNetFlgValidSet(pMiNetHdl, flgValid);
}


/****************************************************************************/
/** Apply IP Configuration from applications
 *
 * This function applies the IP settings from CM and can be called from
 * applications.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_miNetCmIpCommit(
    GOAL_MI_NET_T *pMiNetHdl                    /**< MI handle */
)
{
    GOAL_STATUS_T res;                          /* result */

    if (NULL == pMiNetHdl) {
        /* get handle of default interface */
        res = goal_instGetById((GOAL_INSTANCE_T **) &pMiNetHdl, GOAL_ID_NET, GOAL_ID_DEFAULT);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
    }

    return goal_miNetCmIpCb(GOAL_CM_NET_MOD_ID, NET_CM_VAR_COMMIT + (pMiNetHdl->cmVarInstId * m_virtCmVarOffset), NULL);
}


/****************************************************************************/
/** Apply IP Configuration from CM
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_miNetCmIpCb(
    uint32_t modId,                             /**< module ID */
    uint32_t varId,                             /**< variable Id */
    GOAL_CM_VAR_T *pVar                         /**< variable pointer */
)
{
    uint32_t addrIp = 0;                        /* IP address */
    uint32_t addrMask = 0;                      /* subnet mask */
    uint32_t addrGw = 0;                        /* gateway */
    uint8_t flgValid = 0;                       /* valid flag */
    GOAL_STATUS_T res;                          /* result */
    GOAL_CM_VAR_T *pCmVar;                      /* CM variable */
    uint8_t dhcpEnable = 0;                     /* DHCP enable flag */
    GOAL_BOOL_T dhcpReleaseTx;                  /* transmit DHCP release message */
    uint32_t instId;                            /* CM variable instance ID */
    GOAL_INSTANCE_LIST_T *pInstList = NULL;     /* list of Net MIs */
    GOAL_INSTANCE_T *pInst;                     /* instance entry */
    GOAL_MI_NET_T *pMiNetHdl = NULL;            /* MI handle */

    if (GOAL_CM_NET_MOD_ID != modId) {
        goal_logErr("unexpected Variable ID: %"FMT_u32, modId);
        return GOAL_ERROR;
    }

    if (NET_CM_VAR_COMMIT > varId) {
        goal_logErr("unexpected Variable ID: %"FMT_u32, varId);
        return GOAL_ERROR;
    }

    if (NULL != pVar) {
        /* clear trigger */
        GOAL_CM_SETVAR_UINT8(pVar, 0);
    }

    instId = (varId - NET_CM_VAR_COMMIT) / m_virtCmVarOffset;

    res = goal_instListGetByType(&pInstList, GOAL_ID_NET);
    if (GOAL_RES_ERR(res) || (NULL == pInstList)) {
        goal_logErr("failed to get Net MI list");
        return GOAL_ERROR;
    }

    GOAL_LL_FOREACH(pInstList->pInst, pInst) {
        if (instId == ((GOAL_MI_NET_T *) pInst)->cmVarInstId) {
            pMiNetHdl = (GOAL_MI_NET_T *) pInst;
            break;
        }
    }

    if (NULL == pMiNetHdl) {
        goal_logErr("no NET MI found for CM Var instance ID %"FMT_u32, instId);
        return GOAL_ERROR;
    }

    /* DHCP is priorized */
    res = goal_miNetCmVarGetById(pMiNetHdl, GOAL_CM_NET_MOD_ID, NET_CM_VAR_DHCP_ENABLED, &pCmVar);
    if (GOAL_RES_OK(res)) {
        dhcpEnable = GOAL_CM_VAR_UINT8(pCmVar);
        if (0 != dhcpEnable) {
            dhcpReleaseTx = GOAL_FALSE;
            goal_miNetCmd(pMiNetHdl, GOAL_NET_CMD_DHCP_RELEASE, GOAL_FALSE, &dhcpReleaseTx);
            goal_miNetCmd(pMiNetHdl, GOAL_NET_CMD_DHCP_START, GOAL_FALSE, NULL);
        }
    }

    /* DHCP not configured */
    if (GOAL_RES_OK(res) && 0 == dhcpEnable) {

#if GOAL_CONFIG_DHCP == 1
        goal_netCmd(GOAL_NET_CMD_DHCP_STOP, GOAL_FALSE, NULL);
#endif

        res = goal_miNetCmVarGetById(pMiNetHdl, GOAL_CM_NET_MOD_ID, NET_CM_VAR_IP, &pCmVar);
        if (GOAL_RES_OK(res)) {
            addrIp = GOAL_CM_VAR_UINT32(pCmVar);
        }

        res = goal_miNetCmVarGetById(pMiNetHdl, GOAL_CM_NET_MOD_ID, NET_CM_VAR_NETMASK, &pCmVar);
        if (GOAL_RES_OK(res)) {
            addrMask = GOAL_CM_VAR_UINT32(pCmVar);
        }

        res = goal_miNetCmVarGetById(pMiNetHdl, GOAL_CM_NET_MOD_ID, NET_CM_VAR_GW, &pCmVar);
        if (GOAL_RES_OK(res)) {
            addrGw = GOAL_CM_VAR_UINT32(pCmVar);
        }

        res = goal_miNetCmVarGetById(pMiNetHdl, GOAL_CM_NET_MOD_ID, NET_CM_VAR_VALID, &pCmVar);
        if (GOAL_RES_OK(res)) {
            flgValid = GOAL_CM_VAR_UINT8(pCmVar);
        }

        res = goal_miNetCmVirtVarGetById(pMiNetHdl, GOAL_CM_NET_MOD_ID, NET_CM_VAR_COMMIT, &pCmVar);
        if (GOAL_RES_OK(res)) {
            GOAL_CM_SETVAR_UINT8(pCmVar, 0);
        }

        res = goal_miNetIpSet(pMiNetHdl, addrIp, addrMask, addrGw, (GOAL_FALSE == flgValid) ? GOAL_TRUE : GOAL_FALSE);
    }

    return res;
}


/****************************************************************************/
/** Check if IP configuration is valid
 *
 * @retval GOAL_OK successful, flag is set
 * @retval other failed, flag isn't set
 */
GOAL_STATUS_T goal_miNetFlgValidGet(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI handle */
    GOAL_BOOL_T *pFlgValid                      /**< valid flag ptr */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_CM_VAR_T *pVarCm = NULL;               /* CM variable */

    if (NULL == pMiNetHdl) {
        /* get handle of default interface */
        res = goal_instGetById((GOAL_INSTANCE_T **) &pMiNetHdl, GOAL_ID_NET, GOAL_ID_DEFAULT);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
    }

    if (!pFlgValid) {
        return GOAL_ERR_NULL_POINTER;
    }

    res = goal_miNetCmVarGetById(pMiNetHdl, GOAL_CM_NET_MOD_ID, NET_CM_VAR_VALID, &pVarCm);
    if (GOAL_RES_OK(res)) {
        *pFlgValid = (GOAL_CM_VAR_UINT8(pVarCm)) ? GOAL_TRUE : GOAL_FALSE;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Set IP configuration state
 *
 * @retval GOAL_OK successful, flag is set
 * @retval other failed, flag isn't set
 */
GOAL_STATUS_T goal_miNetFlgValidSet(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI handle */
    GOAL_BOOL_T flgValid                        /**< valid flag */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_CM_VAR_T *pVarCm = NULL;               /* CM variable */

    if (NULL == pMiNetHdl) {
        /* get handle of default interface */
        res = goal_instGetById((GOAL_INSTANCE_T **) &pMiNetHdl, GOAL_ID_NET, GOAL_ID_DEFAULT);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
    }

    res = goal_miNetCmVarGetById(pMiNetHdl, GOAL_CM_NET_MOD_ID, NET_CM_VAR_VALID, &pVarCm);
    if (GOAL_RES_OK(res)) {
        GOAL_CM_SETVAR_UINT8(pVarCm, (uint8_t) flgValid);
    }

    return res;
}


/****************************************************************************/
/** Update DNS Configuration in CM
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_miNetCmDnsSet(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI handle */
    uint32_t dns1,                              /**< nameserver 1 */
    uint32_t dns2                               /**< nameserver 2 */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_CM_VAR_T *pCmVar = NULL;               /* CM variable */

    if (NULL == pMiNetHdl) {
        /* get handle of default interface */
        res = goal_instGetById((GOAL_INSTANCE_T **) &pMiNetHdl, GOAL_ID_NET, GOAL_ID_DEFAULT);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
    }

    res = goal_miNetCmVarGetById(pMiNetHdl, GOAL_CM_NET_MOD_ID, NET_CM_VAR_DNS0, &pCmVar);
    if (GOAL_RES_OK(res)) {
        GOAL_CM_SETVAR_UINT32(pCmVar, dns1);
    }

    res = goal_miNetCmVarGetById(pMiNetHdl, GOAL_CM_NET_MOD_ID, NET_CM_VAR_DNS1, &pCmVar);
    if (GOAL_RES_OK(res)) {
        GOAL_CM_SETVAR_UINT32(pCmVar, dns2);
    }

    return res;
}


/****************************************************************************/
/** Get DNS Configuration from CM
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_miNetCmDnsGet(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI handle */
    uint32_t *pDns1,                            /**< nameserver 1 */
    uint32_t *pDns2                             /**< nameserver 2 */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_CM_VAR_T *pCmVar = NULL;               /* CM variable */

    if (NULL == pMiNetHdl) {
        /* get handle of default interface */
        res = goal_instGetById((GOAL_INSTANCE_T **) &pMiNetHdl, GOAL_ID_NET, GOAL_ID_DEFAULT);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
    }

    if (NULL == pDns1 || NULL == pDns2) {
        res = GOAL_ERR_NULL_POINTER;
    }
    if (GOAL_RES_OK(res)) {
        res = goal_miNetCmVarGetById(pMiNetHdl, GOAL_CM_NET_MOD_ID, NET_CM_VAR_DNS0, &pCmVar);
    }

    if (GOAL_RES_OK(res)) {
        *pDns1 = GOAL_CM_VAR_UINT32(pCmVar);
    }

    res = goal_miNetCmVarGetById(pMiNetHdl, GOAL_CM_NET_MOD_ID, NET_CM_VAR_DNS1, &pCmVar);
    if (GOAL_RES_OK(res)) {
        *pDns2 = GOAL_CM_VAR_UINT32(pCmVar);
    }

    return res;
}


/****************************************************************************/
/** Default IP address check function
 *
 * This function is called each time a IP address change (static ip) is requested.
 * It checks for validity of the IP address.
 *
 * Sources: - EtherNet/IP Stack
 *          - RFC 6890
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_miNetIpDefaultValidCheck(
    uint32_t addrIp,                            /**< IP address */
    uint32_t addrMask,                          /**< subnet mask */
    uint32_t addrGw                             /**< gateway */
)
{
    /* allow special case (0.0.0.0/0.0.0.0) */
    if ((addrIp == 0) && (addrGw == 0)) {
        return GOAL_OK;
    }

    /* check for reserved net (0.0.0.0/8) */
    if (((addrIp & 0xff000000) == 0) ||
        ((addrIp & 0xffffff00) == 0)) {
        goal_logErr("reserved net 0.0.0.0/8 not allowed");
        return GOAL_ERR_IP_SET;
    }

    /* check for loopback (127.0.0.0/8) */
    if (((addrIp & 0xffffff00) == 0x7F000000) ||
        ((addrGw & 0xffffff00) == 0x7F000000)) {
        goal_logErr("local ip address 127.0.0.0/8 not allowed");
        return GOAL_ERR_IP_SET;
    }

    /* check for test net (192.0.2.0/24) */
    if (((addrIp & 0xffffff00) == 0xC0000200) ||
        ((addrGw & 0xffffff00) == 0xC0000200)) {
        goal_logErr("address from test net (rfc 5737) not allowed");
        return GOAL_ERR_IP_SET;
    }

    /* hostID == 0 || hostID == ~0 is invalid */
    if (((addrIp & ~addrMask) == 0) ||
        ((addrIp & ~addrMask) == ~addrMask)) {
        goal_logErr("host id is invalid (address & netmask combination)");
        return GOAL_ERR_IP_SET;
    }

    /* GW must be in same subnet as device */
    if ((addrGw != 0) &&
        ((addrIp & addrMask) != (addrGw & addrMask))) {
        goal_logErr("gateway is outside of network");
        return GOAL_ERR_IP_SET;
    }

    /* check net class, D is invalid */
    if (((addrIp & 0xff000000) >= 0xE0000000) ||
        ((addrGw & 0xff000000) >= 0xE0000000)) {
        goal_logErr("class D net not allowed as IP address");
        return GOAL_ERR_IP_SET;
    }

    /* check if address or gateway is a broadcast address */
    if (((addrIp & 0x000000ff) == 0) || ((addrIp & 0x000000ff) == 0xff) ||
        (addrGw != 0 && (((addrGw & 0x000000ff) == 0) || ((addrGw & 0x000000ff) == 0xff)))) {
        goal_logErr("address or gateway is a broadcast address");
        return GOAL_ERR_IP_SET;
    }

    /* check for valid netmask */
    if (0 != (addrMask & (~addrMask >> 1))) {
        goal_logErr("netmask contains invalid bits set");
        return GOAL_ERR_IP_SET;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Register a callback that is invoked if the NET MI was brought up
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_miNetCbNetworkUpReg(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI handle */
    GOAL_NET_CB_NET_UP_T func                   /**< callback function */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_NET_CB_LIST_NET_UP_T *pEntry;          /* callback list entry */

    if (NULL == pMiNetHdl) {
        /* get handle of default interface */
        res = goal_instGetById((GOAL_INSTANCE_T **) &pMiNetHdl, GOAL_ID_NET, GOAL_ID_DEFAULT);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
    }

    res = goal_memCalloc(&pEntry, sizeof(GOAL_NET_CB_LIST_NET_UP_T));
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to allocate list entry");
    }

    if (GOAL_RES_OK(res)) {
        pEntry->func = func;
        pEntry->pNext = pMiNetHdl->pListCbNetUp;
        pMiNetHdl->pListCbNetUp = pEntry;
    }

    return res;
}


/****************************************************************************/
/** Register a callback that is invoked if a new IP address is set
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_miNetCbIpSetReg(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI handle */
    GOAL_NET_CB_IP_SET_T func,                  /**< callback function */
    void *pArg                                  /**< callback argument */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_NET_CB_LIST_IP_SET_T *pEntry;          /* callback list entry */

    if (NULL == pMiNetHdl) {
        /* get handle of default interface */
        res = goal_instGetById((GOAL_INSTANCE_T **) &pMiNetHdl, GOAL_ID_NET, GOAL_ID_DEFAULT);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
    }

    res = goal_memCalloc(&pEntry, sizeof(GOAL_NET_CB_LIST_IP_SET_T));
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to allocate list entry");
    }

    if (GOAL_RES_OK(res)) {
        pEntry->func = func;
        pEntry->pArg = pArg;
        pEntry->pNext = pMiNetHdl->pListCbIpSet;
        pMiNetHdl->pListCbIpSet = pEntry;
    }

    return res;
}


/****************************************************************************/
/** Register a callback that is invoked to verify a new IP address
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_miNetCbIpVerifyReg(
    GOAL_MI_NET_T *pMiNetHdl,                   /**< MI handle */
    GOAL_NET_CB_IP_SET_VERIFY_T func            /**< callback function */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_NET_CB_LIST_IP_VERIFY_T *pEntry;       /* callback list entry */

    if (NULL == pMiNetHdl) {
        /* get handle of default interface */
        res = goal_instGetById((GOAL_INSTANCE_T **) &pMiNetHdl, GOAL_ID_NET, GOAL_ID_DEFAULT);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
    }

    res = goal_memCalloc(&pEntry, sizeof(GOAL_NET_CB_LIST_IP_VERIFY_T));
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to allocate list entry");
    }

    if (GOAL_RES_OK(res)) {
        pEntry->func = func;
        pEntry->pNext = pMiNetHdl->pListCbIpVerify;
        pMiNetHdl->pListCbIpVerify = pEntry;
    }

    return res;
}
#endif /* GOAL_CONFIG_TCPIP_STACK */
