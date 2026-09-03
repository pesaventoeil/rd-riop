/** @file
 *
 * @brief application core for Ethernet/ IP (EIP) via remote procedure call (RPC)
 *
 * This module handles the application core for EIP via RPC on GOAL.
 *
 * @copyright
 * Copyright 2010-2020 port GmbH Halle/Saale.
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

#define GOAL_ID GOAL_ID_EIP
#include <goal_includes.h>
#include <goal_eip_rpc.h>
#include <goal_eip.h>
#if GOAL_CONFIG_GEN_RPC == 1


/****************************************************************************/
/* Typedefs */
/****************************************************************************/
typedef struct {
    S_CIP_attribute_struct cipAttr;             /**< CIP attribute */
    GOAL_BOOL_T flgFree;                        /**< free flag */
} GOAL_EIP_CIP_ATTRIBUTE_T;


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_STAGE_HANDLER_T stageInit;          /**< init stage handler */
static GOAL_RPC_HDL_CHN_T *pHdlRpcChn = GOAL_RPC_HDL_NONE; /**< RPC channel handle */

static GOAL_INSTANCE_LIST_T *mpListInst = NULL; /**< instance list */


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
static GOAL_STATUS_T goal_eipRpcInit(
    void
);

static GOAL_STATUS_T goal_eipRpcPastInit(
    void
);

static GOAL_STATUS_T goal_eipRpcRegisterServices(
    void
);

static GOAL_STATUS_T goal_eipRpcSetupChannels(
    void
);

#if GOAL_CONFIG_MEDIA_MI_DM == 1
static GOAL_STATUS_T goal_eipDmCb(
    GOAL_MI_DM_GROUP_T *pGroup,                 /**< [in] MI DM group handle */
    void *pPriv                                 /**< [in] private pointer */
);
#endif

static GOAL_STATUS_T goal_eipRpcStoreAssemblyObject(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    uint32_t instanceNr,                        /**< instance number of the assembly object */
    uint16_t len,                               /**< length of the data the assembly object contains */
    GOAL_BOOL_T flgDm                           /**< flag for mapping to DM */
);

static GOAL_STATUS_T goal_eipRpcGetAssemblyObject(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    uint32_t instanceNr,                        /**< instance number of the assembly object */
    GOAL_EIP_CYCLIC_DATA_T **pHdlCyclData       /**< [out] pointer to return the assembly object handle */
);

#if GOAL_CONFIG_MEDIA_MI_DM == 1
static GOAL_STATUS_T goal_eipDmInput(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/Ip instance */
    uint32_t instanceNr                         /**< this instance's number (unique within the class) */
);

static GOAL_STATUS_T goal_eipDmOutput(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/Ip instance */
    uint32_t instanceNr                         /**< this instance's number (unique within the class) */
);

static GOAL_STATUS_T goal_eipDmLedEvent(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/Ip instance */
    uint32_t leds                               /**< active LEDs */
);
#endif /* GOAL_CONFIG_MEDIA_MI_DM == 1 */

static GOAL_STATUS_T goal_eipCreateAssemblyObjectInt(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t instanceNr,                        /**< instance number of the assembly object to create */
    uint16_t len                                /**< length of the assembly object's data */
);

static GOAL_STATUS_T goal_eipAddConnection(
    GOAL_EIP_CONNECTION_T type,                 /**< connection type */
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t connNum,                           /**< the number of the input only connection. */
    uint32_t outputAssembly,                    /**< the O-to-T point to be used for this connection */
    uint32_t inputAssembly,                     /**< the T-to-O point to be used for this connection */
    uint32_t configAssembly                     /**< the configuration point to be used for this connection */
);

static GOAL_STATUS_T goal_eipCfStringSet(
    const char *pStr,                           /**< string to set */
    GOAL_RPC_FUNC_ID funcId                     /**< attribute set function ID */
);

static GOAL_STATUS_T goal_eipCbServer(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< RPC handle */
);

static GOAL_STATUS_T goal_eipParamClassHandlerCbServer(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< RPC handle */
);

static GOAL_STATUS_T goal_eipAssemblyObjectReadRpc(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t instanceId,                        /**< instance number of the assembly object to read */
    uint8_t *pData,                             /**< data pointer to store read data */
    uint16_t len                                /**< length of data to read */
);

static GOAL_STATUS_T goal_eipAssemblyObjectWriteRpc(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t instanceId,                        /**< instance number of the assembly object to read */
    uint8_t *pData,                             /**< data pointer to store read data */
    uint16_t len                                /**< length of data to read */
);

static GOAL_STATUS_T goal_eipNewInternal(
    GOAL_EIP_T **ppEip,                         /**< EtherNet/IP instance ref */
    const uint32_t id,                          /**< instance id */
    GOAL_EIP_FUNC_CB_T pFunc,                   /**< EtherNet/IP callback function */
    uint32_t ethMiId,                           /**< ID of assigned ETH MI */
    uint32_t netMiId,                           /**< ID of assigned NET MI */
    uint32_t rpcFuncId                          /**< RPC function ID */
);


/****************************************************************************/
/** Initialize of the Ethernet/IP stack
 *
 * @retval GOAL_OK success
 * @retval other failure
 */
GOAL_STATUS_T goal_eipInit(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* initialize RPC */
    res = goal_eipRpcInit();
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to initialize EtherNet/IP RPC");
        return res;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Ethernet/ IP via RPC initialization
 *
 * This function initialize all necessary function to use EIP via RPC.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_eipRpcInit(
    void
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    /* set  the size of the RPC stack */
    res = goal_rpcSetStackMax(RPC_BUFF_SIZE_EIP);

    /* register the EIP via RPC init function */
    if (GOAL_RES_OK(res)) {
        res = goal_mainStageReg(GOAL_STAGE_CTC, &stageInit, GOAL_STAGE_INIT, goal_eipRpcPastInit);
    }

    return res;
}


/****************************************************************************/
/** Create new EtherNet/IP instance for the Default Interface
 *
 * Create a GOAL EtherNet/IP instance for the given ID and register a callback.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_eipNew(
    GOAL_EIP_T **ppEip,                         /**< EtherNet/IP instance ref */
    const uint32_t id,                          /**< instance id */
    GOAL_EIP_FUNC_CB_T pFunc                    /**< EtherNet/IP callback function */
)
{
    return goal_eipNewInternal(ppEip, id, pFunc, GOAL_ID_DEFAULT, GOAL_ID_DEFAULT, GOAL_RPC_FUNC_NEW);
}


/****************************************************************************/
/** Create new EtherNet/IP instance and assign a specific interface to it.
 *
 * Create a GOAL EtherNet/IP instance for the given ID and register a callback.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_eipNewIfaceAssign(
    GOAL_EIP_T **ppEip,                         /**< EtherNet/IP instance ref */
    const uint32_t id,                          /**< instance id */
    GOAL_EIP_FUNC_CB_T pFunc,                   /**< EtherNet/IP callback function */
    uint32_t ethMiId,                           /**< ID of assigned ETH MI */
    uint32_t netMiId                            /**< ID of assigned NET MI */
)
{
    return goal_eipNewInternal(ppEip, id, pFunc, ethMiId, netMiId, GOAL_RPC_FUNC_NEW_IF_ASSIGN);
}


/****************************************************************************/
/** Create new EtherNet/IP instance and assign a specific interface to it.
 *
 * Create a GOAL EtherNet/IP instance for the given ID and register a callback.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_eipNewInternal(
    GOAL_EIP_T **ppEip,                         /**< EtherNet/IP instance ref */
    const uint32_t id,                          /**< instance id */
    GOAL_EIP_FUNC_CB_T pFunc,                   /**< EtherNet/IP callback function */
    uint32_t ethMiId,                           /**< ID of assigned ETH MI */
    uint32_t netMiId,                           /**< ID of assigned NET MI */
    uint32_t rpcFuncId                          /**< RPC function ID */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */
    EIP_INSTANCE_T *pEip;                       /* EtherNet/IP instance */

    /* check if EtherNet/IP init is done */
    if (NULL == mpListInst) {
        goal_logErr("EtherNet/IP not initialized");
        return GOAL_RES_ID(GOAL_ID_EIP, GOAL_ERR_NOT_INITIALIZED);
    }

    /* request 2 buffers for handling MR requst/response data */
    res = goal_queuePoolBufsReq(GOAL_ID, GOAL_EIP_MR_DATA_SIZE, 2, 0);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* create generic GOAL instance */
    res = goal_instNew((GOAL_INSTANCE_T **) ppEip, sizeof(EIP_INSTANCE_T), GOAL_ID_EIP, id, "EtherNet/IP AC");
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create generic instance");
        return res;
    }

    pEip = (EIP_INSTANCE_T *) *ppEip;

    /* set callback */
    if (NULL == pFunc) {
        goal_logInfo("No callback handler set - installing default handler");
        pEip->goal_eipCb = NULL;
    }
    else {
        pEip->goal_eipCb = pFunc;
    }

    /* get a new rpc handle */
    res = goal_rpcNew(&pHdlRpc, pHdlRpcChn);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    if (GOAL_RPC_FUNC_NEW_IF_ASSIGN == rpcFuncId) {
        GOAL_RPC_PUSH(netMiId);
        GOAL_RPC_PUSH(ethMiId);
    }

    GOAL_RPC_PUSH(id);

    /* call the procedure at the server */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_EIP, rpcFuncId);
    }

    /* rpc is finished - close the handle */
    GOAL_RPC_CLOSE();


#if GOAL_CONFIG_MEDIA_MI_DM == 1

    /* create group for PEER_FROM direction */
    if (GOAL_RES_OK(res)) {
        res = goal_miDmGroupNew(&pEip->pDmGroupOutput, GOAL_MI_MCTC_DIR_PEER_FROM, GOAL_ID_EIP);
    }

    /* add the Data Provider */
    if (GOAL_RES_OK(res)) {
        res = goal_eipDmDpAdd((*ppEip), GOAL_MI_MCTC_DIR_PEER_FROM, NULL);
    }

    /* registration of DM callback to the PEER_FROM group */
    if (GOAL_RES_OK(res)) {
        res = goal_miDmCbReg(NULL, pEip->pDmGroupOutput, GOAL_MI_DM_CB_READ, goal_eipDmCb, (*ppEip));
    }

    /* create group for PEER_TO direction */
    if (GOAL_RES_OK(res)) {
        res = goal_miDmGroupNew(&pEip->pDmGroupInput, GOAL_MI_MCTC_DIR_PEER_TO, GOAL_ID_EIP);
    }

#endif /* GOAL_CONFIG_MEDIA_MI_DM */

    /* info */
    if (GOAL_RES_OK(res)) {
        goal_logInfo("EtherNet/IP Application Core successfully started");
    }

    return res;
}


/****************************************************************************/
/** EIP via RPC past initialization
 *
 * This function is called during the init stage of the GOAL_STAGE_CTC_PRE.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_eipRpcPastInit(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* create the EtherNet/IP instance list */
    res = goal_instListNew(&mpListInst, GOAL_ID_EIP);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create EtherNet/IP instance list");
        return res;
    }

    /* setup the service function */
    res = goal_eipRpcRegisterServices();

    /* setup all necessary RPC channels for the EIP application */
    if (GOAL_RES_OK(res)) {
        res = goal_eipRpcSetupChannels();
    }

    return res;
}


/****************************************************************************/
/** Register server function
 *
 * This function registers all server functions for using the EIP via RPC.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_eipRpcRegisterServices(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* setup the service function */
    res = goal_rpcRegisterService(GOAL_ID_EIP, GOAL_RPC_FUNC_CB, &goal_eipCbServer);

    if (GOAL_RES_OK(res)) {
        res = goal_rpcRegisterService(GOAL_ID_EIP, GOAL_RPC_FUNC_VIRTUAL_PARAM_CLASS_HANDLER_CB, &goal_eipParamClassHandlerCbServer);
    }

    return res;
}


/****************************************************************************/
/** Setup channels for EIP via RPC
 *
 * This function initialize the ctc channels for RPC and the cyclic data.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_eipRpcSetupChannels(
    void
)
{
    /* setup a channel for RPC */
    return goal_rpcSetupChannel(&pHdlRpcChn, GOAL_ID_MI_CTC_DEFAULT);
}


#if GOAL_CONFIG_MEDIA_MI_DM == 1
/****************************************************************************/
/** EtherNet/IP Data Mapper Callback
 *
 * This data mapper callback is executed by every group update. It forwards
 * the content of Data Provider to application layer. This includes LED events
 * and assembly callbacks when the cycle counter is updated.
 * If there is no Data Provider available, the assembly callbacks are
 * triggered by every group update.
 * Assembly callbacks for objects which are exchanged by RPC, are not triggered
 * by this function, because they are forwarded by RPC.
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_eipDmCb(
    GOAL_MI_DM_GROUP_T *pGroup,                 /**< [in] MI DM group handle */
    void *pPriv                                 /**< [in] private pointer */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MI_MCTC_DP_T dataDp;                   /* generic data provider */
    GOAL_BOOL_T flgNewIoDataCb = GOAL_FALSE;    /* flag for running new IO Data CB */
    EIP_INSTANCE_T *pEip;                       /* EtherNet/IP handle */
    GOAL_EIP_CYCLIC_DATA_T *pEipCyclic;         /* cyclic data instance */
    uint32_t leds = 0;                          /* stack LEDs */

    /* get private data */
    pEip = (EIP_INSTANCE_T *) pPriv;

    /* lock partition group */
    res = goal_miDmGroupReadStart(pGroup);
    if (GOAL_RES_OK(res)) {
        /* fetch status of generic data provider */
        res = goal_miDmGroupPartRead(&pEip->dmDp, (uint8_t *) &dataDp);
        if (GOAL_RES_OK(res)) {
            /* check LEDs */
            if (pEip->dataDp.leds != dataDp.leds) {
                leds = 0;
                if (GOAL_MASK_IS_SET(dataDp.leds, GOAL_MCTC_DP_LED_RED_1)) {
                    leds |= GOAL_EIP_LED_MS_RED;
                }
                if (GOAL_MASK_IS_SET(dataDp.leds, GOAL_MCTC_DP_LED_RED_2)) {
                    leds |= GOAL_EIP_LED_NS_RED;
                }
                if (GOAL_MASK_IS_SET(dataDp.leds, GOAL_MCTC_DP_LED_GREEN_1)) {
                    leds |= GOAL_EIP_LED_MS_GREEN;
                }
                if (GOAL_MASK_IS_SET(dataDp.leds, GOAL_MCTC_DP_LED_GREEN_2)) {
                    leds |= GOAL_EIP_LED_NS_GREEN;
                }

                /* inform the application */
                res = goal_eipDmLedEvent(pEip, leds);
                if (GOAL_RES_OK(res)) {
                    pEip->dataDp.leds = dataDp.leds;
                }
            }

            /* DP available, run callback only for new cycle counter */
            if (pEip->dataDp.cntUpdate != dataDp.cntUpdate) {
                pEip->dataDp.cntUpdate = dataDp.cntUpdate;
                flgNewIoDataCb = GOAL_TRUE;
            }
        } else if (GOAL_ERR_WRONG_STATE == res) {
            /* DP not available, run callback always */
            pEip->dataDp.cntUpdate++;
            flgNewIoDataCb = GOAL_TRUE;
        }

        /* unlock partition group */
        res = goal_miDmGroupReadEnd(pGroup);
    }

    if (GOAL_RES_OK(res) && (GOAL_TRUE == flgNewIoDataCb)) {
        GOAL_LL_FOREACH(pEip->pEipCyclic, pEipCyclic) {
            /* check if the object is mapped to DM (is active) and the length is
             * greater zero */
            if (0 == goal_miDmPartSizeGet(&pEipCyclic->dmData)) {
                continue;
            }
            switch (pEipCyclic->direction) {
                case GOAL_MI_MCTC_DIR_PEER_TO:
                    goal_eipDmInput(pEip, pEipCyclic->instanceNr);
                    break;
                case GOAL_MI_MCTC_DIR_PEER_FROM:
                    goal_eipDmOutput(pEip, pEipCyclic->instanceNr);
                    break;
                default:
                    break;
            }
        }
    }
    return res;
}
#endif /* GOAL_CONFIG_MEDIA_MI_DM == 1 */


/****************************************************************************/
/** Store assembly object
 *
 * This function adds the instance number, the data length of the assembly
 * object and a flag sign DM mapping to a list. Thus all information to add a
 * connection are bundled.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_eipRpcStoreAssemblyObject(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    uint32_t instanceNr,                        /**< instance number of the assembly object */
    uint16_t len,                               /**< length of the data the assembly object contains */
    GOAL_BOOL_T flgDm                           /**< flag for mapping to DM */
)
{
    GOAL_EIP_CYCLIC_DATA_T *pInstNew = NULL;    /* new instance */
    GOAL_STATUS_T res;                          /* result */

    /* check for double InstanceIDs */
    res = goal_eipRpcGetAssemblyObject(pEip, instanceNr, &pInstNew);
    if (GOAL_RES_OK(res)) {
        /* entry already exist */
        return res;
    }

    /* allocate new instance for cyclic data */
    res = goal_memCalloc(&pInstNew, sizeof(GOAL_EIP_CYCLIC_DATA_T));
    if (GOAL_RES_OK(res)) {
        /* store the new object */
        pInstNew->instanceNr = instanceNr;
        pInstNew->len = len;
        pInstNew->flgDm = flgDm;
#if GOAL_CONFIG_MEDIA_MI_DM == 1
        /* set direction to invalid */
        pInstNew->direction = (uint32_t) (-1);
#endif /* GOAL_CONFIG_MEDIA_MI_DM == 1 */

        /* append */
        GOAL_LL_APPEND(pEip->pEipCyclic, pInstNew);
    }

    return res;
}


/****************************************************************************/
/** Check assembly object
 *
 * Based on the instance number, this function will check if the assembly
 * object already stored. The pointer to the assembly object data and their
 * length are hand back.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_eipRpcGetAssemblyObject(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    uint32_t instanceNr,                        /**< instance number of the assembly object */
    GOAL_EIP_CYCLIC_DATA_T **pHdlCyclData       /**< [out] pointer to return the assembly object handle */
)
{
    GOAL_EIP_CYCLIC_DATA_T *pEipCyclic;         /* cyclic data instance */

    if (NULL == pHdlCyclData) {
        goal_logErr("null pointer detected");
        return GOAL_ERR_NULL_POINTER;
    }

    GOAL_LL_FOREACH(pEip->pEipCyclic, pEipCyclic) {
        if (pEipCyclic->instanceNr == instanceNr) {
            *pHdlCyclData = pEipCyclic;
            return GOAL_OK;
        }
    }

    return GOAL_ERROR;
}


#if GOAL_CONFIG_MEDIA_MI_DM == 1
/****************************************************************************/
/** Update input assemblies
 *
 * This function forwards an input assembly instance to application for update.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_eipDmInput(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/Ip instance */
    uint32_t instanceNr                         /**< this instance's number (unique within the class) */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_EIP_CB_DATA_T cbData;                  /* callback parameters */

    /* call the function at the application core */
    cbData.data[0].instanceNr = instanceNr;

    res = (NULL != pEip->goal_eipCb) ? pEip->goal_eipCb((GOAL_EIP_T *) pEip, GOAL_EIP_CB_ID_ASSEMBLY_DATA_SEND, &cbData) : GOAL_ERR_NULL_POINTER;
    if (GOAL_RES_ERR(res)) {
        goal_logErr("error [%"FMT_x32"] on assembly data send callback for instance %"FMT_u32, res, instanceNr);
    }
    return res;
}


/****************************************************************************/
/** Update output assemblies
 *
 * This function forwards an output assembly instance to application for update.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_eipDmOutput(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/Ip instance */
    uint32_t instanceNr                         /**< this instance's number (unique within the class) */
)
{
    GOAL_EIP_CB_DATA_T cbData;                  /* callback parameters */
    GOAL_STATUS_T res;                          /* result */

    /* inform application */
    cbData.data[0].instanceNr = instanceNr;

    res = (NULL != pEip->goal_eipCb) ? pEip->goal_eipCb((GOAL_EIP_T *) pEip, GOAL_EIP_CB_ID_ASSEMBLY_DATA_RECV, &cbData) : GOAL_ERR_NULL_POINTER;
    if (GOAL_RES_ERR(res)) {
        goal_logErr("error [%"FMT_x32"] on assembly data receive callback for instance %"FMT_u32, res, instanceNr);
    }
    return res;
}


/****************************************************************************/
/** LEDs Event
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_eipDmLedEvent(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/Ip instance */
    uint32_t leds                               /**< active LEDs */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_EIP_CB_DATA_T cbData;                  /* callback parameters */

    /* call the function at the application core */
    cbData.data[0].leds = leds;

    res = (NULL != pEip->goal_eipCb) ? pEip->goal_eipCb((GOAL_EIP_T *) pEip, GOAL_EIP_CB_ID_LED_CHANGED, &cbData) : GOAL_ERR_NULL_POINTER;
    if (GOAL_RES_ERR(res)) {
        goal_logErr("error [%"FMT_x32"] on LED event", res);
    }
    return res;
}
#endif /* GOAL_CONFIG_MEDIA_MI_DM == 1 */


/****************************************************************************/
/** Get the Ethernet/IP Version
 *
 * Send a request to the communication core for execute the server function.
 *
 * @retval GOAL_OK success
 * @retval other failure
 */
GOAL_STATUS_T goal_eipGetVersion(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    char **ppVersion                            /**< Ethernet/IP version */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */
    uint8_t len = 0;                            /* length of the version string */

    if (NULL == ppVersion) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* get a new rpc handle */
    res = goal_rpcNew(&pHdlRpc, pHdlRpcChn);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* push the EtherNet/IP handle */
    GOAL_EIP_RPC_PUSH_INST(((EIP_INSTANCE_T *) pHdlEip));

    /* call the procedure at the server */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_EIP, GOAL_RPC_FUNC_GET_EIP_VERSION);
    }

    /* pop the length from the stack */
    GOAL_RPC_POP(len, uint8_t);

    /* pop the version from the stack */
    if (GOAL_RES_OK(res)) {
        if (GOAL_EIP_VERSION_LENGTH >= len) {
            res = goal_rpcArgPop(pHdlRpc, (void *) ((EIP_INSTANCE_T *) pHdlEip)->versionBuf, len);
        }
        else {
            res = GOAL_ERR_OVERFLOW;
        }
    }

    if (GOAL_RES_OK(res)) {
        *ppVersion = ((EIP_INSTANCE_T *) pHdlEip)->versionBuf;
    }

    /* rpc is finished - close the handle */
    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** Create assembly object (RPC)
 *
 * Send a request to the communication core for creating an assembly object.
 * This object will be read by RPC when using in a connection.
 *
 * @retval GOAL_OK success
 * @retval other failure
 */
GOAL_STATUS_T goal_eipCreateAssemblyObjectRpc(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t instanceNr,                        /**< instance number of the assembly object to create */
    uint16_t len                                /**< length of the assembly object's data */
)
{
    GOAL_STATUS_T res;                          /* result */

    res = goal_eipCreateAssemblyObjectInt(pHdlEip, instanceNr, len);

    if (GOAL_RES_OK(res)) {
        /* store the new object */
        res = goal_eipRpcStoreAssemblyObject((EIP_INSTANCE_T *) pHdlEip, instanceNr, len, GOAL_FALSE);
    }

    return res;
}


/****************************************************************************/
/** Create assembly object (DM)
 *
 * Send a request to the communication core for creating an assembly object.
 * This object will be mapped to Data Mapper when adding to a connection.
 *
 * @retval GOAL_OK success
 * @retval other failure
 */
GOAL_STATUS_T goal_eipCreateAssemblyObjectDm(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t instanceNr,                        /**< instance number of the assembly object to create */
    uint16_t len                                /**< length of the assembly object's data */
)
{
    GOAL_STATUS_T res;                          /* result */
#if GOAL_CONFIG_MEDIA_MI_DM == 1

    res = goal_eipCreateAssemblyObjectInt(pHdlEip, instanceNr, len);

    if (GOAL_RES_OK(res)) {
        /* store the new object */
        res = goal_eipRpcStoreAssemblyObject((EIP_INSTANCE_T *) pHdlEip, instanceNr, len, GOAL_TRUE);
    }
#else
    /* this function is not supported if DM is not available */
    res = GOAL_ERR_UNSUPPORTED;
#endif
    return res;
}


/****************************************************************************/
/** Create assembly object (internal function)
 *
 * Send a request to the communication core for creating an assembly object.
 *
 * @retval GOAL_OK success
 * @retval other failure
 */
static GOAL_STATUS_T goal_eipCreateAssemblyObjectInt(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t instanceNr,                        /**< instance number of the assembly object to create */
    uint16_t len                                /**< length of the assembly object's data */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */

    /* get a new rpc handle */
    res = goal_rpcNew(&pHdlRpc, pHdlRpcChn);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* push the data length to the stack */
    GOAL_RPC_PUSH(len);

    /* push the instance ID to the stack */
    GOAL_RPC_PUSH(instanceNr);

    /* push the EtherNet/IP handle */
    GOAL_EIP_RPC_PUSH_INST(((EIP_INSTANCE_T *) pHdlEip));

    /* call the procedure at the server */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_EIP, GOAL_RPC_FUNC_CREATE_ASSEMBLY_OBJECT);
    }

    /* rpc is finished - close the handle */
    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** Get pointer to data of assembly object
 *
 * @retval GOAL_OK success
 * @retval other failure
 */
GOAL_STATUS_T goal_eipAssemblyObjectGet(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t instanceId,                        /**< instance number of the assembly object to create */
    uint8_t **ppData,                           /**< data pointer of assembly */
    uint16_t *pLen                              /**< length of assembly data */
)
{
    GOAL_STATUS_T res;                          /* return value */
    GOAL_EIP_CYCLIC_DATA_T *pHdlCyclData = NULL; /* handle of cyclic data */
#if GOAL_CONFIG_MEDIA_MI_DM == 1
    uint32_t len = 0;                           /* data length */
#endif

    res = goal_eipRpcGetAssemblyObject((EIP_INSTANCE_T *) pHdlEip, instanceId, &pHdlCyclData);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

#if GOAL_CONFIG_MEDIA_MI_DM == 1
    if (GOAL_TRUE == pHdlCyclData->flgDm) {
        /* assembly was mapped to DM */
        if (GOAL_MI_MCTC_DIR_PEER_TO == pHdlCyclData->direction) {
            res = goal_miDmGroupWriteBufGet(ppData, &len, &pHdlCyclData->dmData);
        } else if (GOAL_MI_MCTC_DIR_PEER_FROM == pHdlCyclData->direction) {
            res = goal_miDmGroupReadBufGet(ppData, &len, &pHdlCyclData->dmData);
        } else {
            /* no direction defined - error */
            res = GOAL_ERROR;
        }
        if (GOAL_RES_OK(res)) {
            if (!GOAL_MASK_IS_SET(len, 0x0000FFFF)) {
                goal_logErr("truncating assembly length");
                return GOAL_ERR_OVERFLOW;
            }
            if (NULL != pLen) {
                *pLen = (uint16_t) len;
            }
            return GOAL_OK;
        }
    }
#endif
    /* getting object via RPC is not provided use goal_eipAssemblyObjectRead instead */
    return GOAL_ERR_UNSUPPORTED;
}


/****************************************************************************/
/** Write data into assembly object
 *
 * This function checks if the assembly is mapped to DM or not and writes
 * the data to DM or forwards a RPC to peer core.
 *
 * @retval GOAL_OK success
 * @retval other failure
 */
GOAL_STATUS_T goal_eipAssemblyObjectWrite(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t instanceId,                        /**< instance number of the assembly object to write */
    uint8_t *pData,                             /**< data pointer to write */
    uint16_t len                                /**< length of data to write */
)
{
    GOAL_STATUS_T res;                          /* return value */
    GOAL_EIP_CYCLIC_DATA_T *pHdlCyclData = NULL; /* handle of cyclic data */

    /* check for valid pointers */
    if ((NULL == pHdlEip) || (NULL == pData)) {
        return GOAL_ERR_NULL_POINTER;
    }

    res = goal_eipRpcGetAssemblyObject((EIP_INSTANCE_T *) pHdlEip, instanceId, &pHdlCyclData);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

#if GOAL_CONFIG_MEDIA_MI_DM == 1
    if (GOAL_TRUE == goal_miDmPartStatusGet(&pHdlCyclData->dmData)) {
        /* check length */
        if (len != goal_miDmPartSizeGet(&pHdlCyclData->dmData)) {
            return GOAL_ERR_PARAM;
        }

        return goal_miDmSingleWrite(&pHdlCyclData->dmData, pData, len);
    }
#endif

    /* assembly is not mapped to DM, so write by RPC */
    return goal_eipAssemblyObjectWriteRpc(pHdlEip, instanceId, pData, len);
}


/****************************************************************************/
/** Write data to an assembly object (RPC)
 *
 * Send a request to the communication core for execute the server function.
 *
 * The following parameters are pushed to the request:
 *   1. EtherNet/IP handle ID
 *   2. assemmbly ID
 *   3. assembly data length
 *   3. assembly data
 *
 * RPC response parameters:
 *   -none-
 *
 * @retval GOAL_OK success
 * @retval other failure
 */
static GOAL_STATUS_T goal_eipAssemblyObjectWriteRpc(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t instanceId,                        /**< instance number of the assembly object to read */
    uint8_t *pData,                             /**< data pointer to store read data */
    uint16_t len                                /**< length of data to read */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */

    /* get a new rpc handle */
    res = goal_rpcNew(&pHdlRpc, pHdlRpcChn);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* push the data to the RPC stack */
    GOAL_RPC_PUSH_PTR(pData, len);
    /* push the data length */
    GOAL_RPC_PUSH(len);

    /* push the instance number of the assembly object */
    GOAL_RPC_PUSH(instanceId);

    /* push the EtherNet/IP handle */
    GOAL_EIP_RPC_PUSH_INST(((EIP_INSTANCE_T *) pHdlEip));

    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_EIP, GOAL_RPC_FUNC_ASM_OBJECT_WRITE);
    }

    /* rpc is finished - close the handle */
    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** Read data from assembly object
 *
 * This function checks if the assembly is mapped to DM or not and exctracts
 * the data from DM or forwards a RPC to peer core.
 *
 * @retval GOAL_OK success
 * @retval other failure
 */
GOAL_STATUS_T goal_eipAssemblyObjectRead(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t instanceId,                        /**< instance number of the assembly object to read */
    uint8_t *pData,                             /**< data pointer to store read data */
    uint16_t len                                /**< length of data to read */
)
{
    GOAL_STATUS_T res;                          /* return value */
    GOAL_EIP_CYCLIC_DATA_T *pHdlCyclData = NULL; /* handle of cyclic data */

    /* check for valid pointers */
    if ((NULL == pHdlEip) || (NULL == pData)) {
        return GOAL_ERR_NULL_POINTER;
    }

    res = goal_eipRpcGetAssemblyObject((EIP_INSTANCE_T *) pHdlEip, instanceId, &pHdlCyclData);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

#if GOAL_CONFIG_MEDIA_MI_DM == 1
    if (GOAL_TRUE == goal_miDmPartStatusGet(&pHdlCyclData->dmData)) {
        /* check length */
        if (len != goal_miDmPartSizeGet(&pHdlCyclData->dmData)) {
            return GOAL_ERR_PARAM;
        }

        return goal_miDmSingleRead(&pHdlCyclData->dmData, pData);
    }
#endif

    /* assembly is not mapped to DM, so read by RPC */
    return goal_eipAssemblyObjectReadRpc(pHdlEip, instanceId, pData, len);
}


/****************************************************************************/
/** Read data from assembly object (RPC)
 *
 * Send a request to the communication core for execute the server function.
 *
 * The following parameters are pushed to the request:
 *   1. EtherNet/IP handle ID
 *   2. assemmbly ID
 *   3. length
 *
 * RPC response parameters:
 *   1. assembly data
 *
 * @retval GOAL_OK success
 * @retval other failure
 */
static GOAL_STATUS_T goal_eipAssemblyObjectReadRpc(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t instanceId,                        /**< instance number of the assembly object to read */
    uint8_t *pData,                             /**< data pointer to store read data */
    uint16_t len                                /**< length of data to read */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */

    /* get a new rpc handle */
    res = goal_rpcNew(&pHdlRpc, pHdlRpcChn);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* push the length of data to read */
    GOAL_RPC_PUSH(len);

    /* push the instance number of the assembly object */
    GOAL_RPC_PUSH(instanceId);

    /* push the EtherNet/IP handle */
    GOAL_EIP_RPC_PUSH_INST(((EIP_INSTANCE_T *) pHdlEip));

    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_EIP, GOAL_RPC_FUNC_ASM_OBJECT_READ);
    }

    /* pop the requested length from RPC stack */
    GOAL_RPC_POP_PTR(pData, len);

    /* rpc is finished - close the handle */
    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** Add exclusive owner connection via RPC
 *
 * Send a request to the communication core for execute the server function.
 *
 * @retval GOAL_OK success
 * @retval other failure
 */
GOAL_STATUS_T goal_eipAddExclusiveOwnerConnection(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t outputAssembly,                    /**< the O-to-T point to be used for this connection */
    uint32_t inputAssembly,                     /**< the T-to-O point to be used for this connection */
    uint32_t configAssembly                     /**< the configuration point to be used for this connection */
)
{
    return goal_eipAddConnection(GOAL_EIP_ASM_EXCLUSIVE_OWNER, pHdlEip, 0, outputAssembly, inputAssembly, configAssembly);
}


/****************************************************************************/
/** Add input only connection via RPC
 *
 * Send a request to the application core for execute the server function.
 *
 * @retval GOAL_OK success
 * @retval other failure
 */
GOAL_STATUS_T goal_eipAddInputOnlyConnection(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t connNum,                           /**< the number of the input only connection. */
    uint32_t outputAssembly,                    /**< the O-to-T point to be used for this connection */
    uint32_t inputAssembly,                     /**< the T-to-O point to be used for this connection */
    uint32_t configAssembly                     /**< the configuration point to be used for this connection */
)
{
    return goal_eipAddConnection(GOAL_EIP_ASM_INPUT_ONLY, pHdlEip, connNum, outputAssembly, inputAssembly, configAssembly);
}


/****************************************************************************/
/** Add listen only connection via RPC
 *
 * Send a request to the communication core for execute the server function.
 *
 * @retval GOAL_OK success
 * @retval other failure
 */
GOAL_STATUS_T goal_eipAddListenOnlyConnection(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t connNum,                           /**< the number of the input only connection. */
    uint32_t outputAssembly,                    /**< the O-to-T point to be used for this connection */
    uint32_t inputAssembly,                     /**< the T-to-O point to be used for this connection */
    uint32_t configAssembly                     /**< the configuration point to be used for this connection */
)
{
    return goal_eipAddConnection(GOAL_EIP_ASM_LISTEN_ONLY, pHdlEip, connNum, outputAssembly, inputAssembly, configAssembly);
}


/****************************************************************************/
/** Add connection via RPC
 *
 * Send a request to the application core for execute the server function.
 *
 * @retval GOAL_OK success
 * @retval other failure
 */
static GOAL_STATUS_T goal_eipAddConnection(
    GOAL_EIP_CONNECTION_T type,                 /**< connection type */
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t connNum,                           /**< the number of the input only connection. */
    uint32_t outputAssembly,                    /**< the O-to-T point to be used for this connection */
    uint32_t inputAssembly,                     /**< the T-to-O point to be used for this connection */
    uint32_t configAssembly                     /**< the configuration point to be used for this connection */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */
#if GOAL_CONFIG_MEDIA_MI_DM == 1
    GOAL_EIP_CYCLIC_DATA_T *pHdlCyclOutput = NULL; /* Output Assembly handle */
    GOAL_EIP_CYCLIC_DATA_T *pHdlCyclInput = NULL; /* Input Assembly handle */
    GOAL_EIP_CYCLIC_DATA_T *pHdlCyclConfig = NULL; /* Config Assembly handle */
#endif

    /* get a new rpc handle */
    res = goal_rpcNew(&pHdlRpc, pHdlRpcChn);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* push the configuration point to the stack */
    GOAL_RPC_PUSH(configAssembly);

    /* push the T-to-O point to the stack */
    GOAL_RPC_PUSH(inputAssembly);

    /* push the O-to-T point to the stack */
    GOAL_RPC_PUSH(outputAssembly);

    /* push the number of the input only connection to the stack */
    if ((GOAL_EIP_ASM_INPUT_ONLY == type) || (GOAL_EIP_ASM_LISTEN_ONLY == type)) {
        GOAL_RPC_PUSH(connNum);
    }

    /* push the EtherNet/IP handle */
    GOAL_EIP_RPC_PUSH_INST(((EIP_INSTANCE_T *) pHdlEip));

    /* call the procedure at the server */
    if (GOAL_RES_OK(res)) {
        switch (type) {
            case GOAL_EIP_ASM_EXCLUSIVE_OWNER:
                res = goal_rpcCall(pHdlRpc, GOAL_ID_EIP, GOAL_RPC_FUNC_ADD_EXCL_OWN_CONN);
                break;
            case GOAL_EIP_ASM_INPUT_ONLY:
                res = goal_rpcCall(pHdlRpc, GOAL_ID_EIP, GOAL_RPC_FUNC_ADD_INPUT_ONLY_CONN);
                break;
            case GOAL_EIP_ASM_LISTEN_ONLY:
                res = goal_rpcCall(pHdlRpc, GOAL_ID_EIP, GOAL_RPC_FUNC_ADD_LISTEN_ONLY_CONN);
                break;
            default:
                res = GOAL_ERR_PARAM;
                break;
        }
    }

    /* rpc is finished - close the handle */
    GOAL_RPC_CLOSE();

#if GOAL_CONFIG_MEDIA_MI_DM == 1
    if (GOAL_RES_OK(res)) {
        /* get the output assembly handle to check configuration */
        res = goal_eipRpcGetAssemblyObject((EIP_INSTANCE_T *) pHdlEip, outputAssembly, &pHdlCyclOutput);
    }
    if (GOAL_RES_OK(res)) {
        /* get the input assembly handle to check configuration */
        res = goal_eipRpcGetAssemblyObject((EIP_INSTANCE_T *) pHdlEip, inputAssembly, &pHdlCyclInput);
    }

    if (GOAL_RES_OK(res)) {
        /* get the config assembly handle to check configuration */
        res = goal_eipRpcGetAssemblyObject((EIP_INSTANCE_T *) pHdlEip, configAssembly, &pHdlCyclConfig);
    }
    if (GOAL_RES_OK(res)) {
        /* add the structures of assemblies one by one if the DM flag is set */
        if (GOAL_TRUE == pHdlCyclOutput->flgDm) {
            res = goal_eipDmAssemblyAdd(pHdlEip,
                                        NULL,
                                        outputAssembly,
                                        GOAL_MI_MCTC_DIR_PEER_FROM);
        }
        if (GOAL_RES_OK(res) && (GOAL_TRUE == pHdlCyclInput->flgDm)) {
            res = goal_eipDmAssemblyAdd(pHdlEip,
                                    NULL,
                                    inputAssembly,
                                    GOAL_MI_MCTC_DIR_PEER_TO);
        }
        if (GOAL_RES_OK(res) && (GOAL_TRUE == pHdlCyclConfig->flgDm)) {
            res = goal_eipDmAssemblyAdd(pHdlEip,
                                    NULL,
                                    configAssembly,
                                    GOAL_MI_MCTC_DIR_PEER_FROM);
        }

        /* if the CC doesn't support goal_eipDmAssemblyAdd, call legacy goal_eipDmAddConnection instead */
        if (GOAL_ERR_NOT_FOUND == res) {
            if ((GOAL_TRUE == pHdlCyclOutput->flgDm) && (GOAL_TRUE == pHdlCyclInput->flgDm)) {
                /* Add assemblies to DM at once if possible. This keeps conformity to older versions.
                 * The configAssembly will not be added and can be access by RPC. */
                res = goal_eipDmAddConnection(pHdlEip, NULL,  NULL, outputAssembly, inputAssembly);
            } else {
                goal_logErr("invalid transfer configuration of assemblies");
                res = GOAL_ERR_INIT;
            }
        }
    }
#endif
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add connection");
    }
    return res;
}


/****************************************************************************/
/** Set the Serial Number via RPC
 *
 * Send a request to the communication core to execute the server function.
 *
 * @retval GOAL_OK success
 * @retval other failure
 */
GOAL_STATUS_T goal_eipIdentitySerialNumberSet(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t serialNum                          /**< serial number */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */

    /* get a new rpc handle */
    res = goal_rpcNew(&pHdlRpc, pHdlRpcChn);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* push the serial number to the stack */
    GOAL_RPC_PUSH(serialNum);

    /* push the EtherNet/IP handle */
    GOAL_EIP_RPC_PUSH_INST(((EIP_INSTANCE_T *) pHdlEip));

    res = goal_rpcCall(pHdlRpc, GOAL_ID_EIP, GOAL_RPC_FUNC_SERIAL_NUM_SET);

    /* rpc is finished - close the handle */
    GOAL_RPC_CLOSE();

    return res;
}


#if GOAL_CONFIG_MEDIA_MI_DM == 1
/****************************************************************************/
/** Add DM for connection
 *
 * This function maps input and output assemblies to DM for a connection.
 *
 * Send a request to the communication core to execute the server function.
 * The following parameters are pushed to the request:
 *   1. EtherNet/IP handle ID
 *   2. output assemmbly ID
 *   3. input assembly ID
 *
 * RPC response parameters:
 *   1. flag of valid output data index
 *   2. DM index for output data
 *   3. flag of valid input data index
 *   4. DM index for input data
 *
 * @retval GOAL_OK success
 * @retval other failure
 */
GOAL_STATUS_T goal_eipDmAddConnection(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    GOAL_MI_DM_PART_T **ppPartDataOut,          /**< [out] partition for output data */
    GOAL_MI_DM_PART_T **ppPartDataIn,           /**< [out] partition for input data */
    uint32_t outputAssembly,                    /**< the O-to-T point to be used for this connection */
    uint32_t inputAssembly                      /**< the T-to-O point to be used for this connection */
)
{
    GOAL_STATUS_T res;                          /* result */
    uint32_t idxDataOut = 0;                    /* index for output data */
    uint32_t idxDataIn = 0;                     /* index for input data */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */
    GOAL_EIP_CYCLIC_DATA_T *pHdlCyclData = NULL; /* handle of cyclic data */
    GOAL_BOOL_T flgDataOut = GOAL_FALSE;        /* partition out valid flag */
    GOAL_BOOL_T flgDataIn = GOAL_FALSE;         /* partition in valid flag */

    /* get a new rpc handle */
    res = goal_rpcNew(&pHdlRpc, pHdlRpcChn);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* push parameters */
    GOAL_RPC_PUSH(inputAssembly);
    GOAL_RPC_PUSH(outputAssembly);

    /* push the EtherNet/IP handle */
    GOAL_EIP_RPC_PUSH_INST(((EIP_INSTANCE_T *) pHdlEip));

    /* call the procedure at the server */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_EIP, GOAL_RPC_FUNC_DM_CONNECTION_ADD);
    }

    GOAL_RPC_POP(flgDataOut, GOAL_BOOL_T);
    GOAL_RPC_POP(idxDataOut, uint32_t);
    GOAL_RPC_POP(flgDataIn, GOAL_BOOL_T);
    GOAL_RPC_POP(idxDataIn, uint32_t);
    GOAL_RPC_CLOSE();

    if (GOAL_RES_OK(res) && (GOAL_TRUE == flgDataOut)) {
        /* get assembly */
        res = goal_eipRpcGetAssemblyObject((EIP_INSTANCE_T *) pHdlEip, outputAssembly, &pHdlCyclData);
        if ((GOAL_RES_OK(res)) && (0 != pHdlCyclData->len)) {
            /* keep data direction */
            pHdlCyclData->direction = GOAL_MI_MCTC_DIR_PEER_FROM;

            /* register dm partition */
            res = goal_miDmPartIdxReg(pHdlCyclData->direction, GOAL_ID_EIP, &pHdlCyclData->dmData, pHdlCyclData->len, idxDataOut);

            /* store MI DM Output group handle */
            goal_miDmGroupGetByIdx(&((EIP_INSTANCE_T *) pHdlEip)->pDmGroupOutput, pHdlCyclData->direction, GOAL_ID_EIP);

            /* store dm data handle */
            if (NULL != ppPartDataOut) {
                *ppPartDataOut = &pHdlCyclData->dmData;
            }
        }
    }

    if (GOAL_RES_OK(res) && (GOAL_TRUE == flgDataIn)) {
        /* get assembly */
        res = goal_eipRpcGetAssemblyObject((EIP_INSTANCE_T *) pHdlEip, inputAssembly, &pHdlCyclData);
        if ((GOAL_RES_OK(res))  && (0 != pHdlCyclData->len)) {
            /* keep data direction */
            pHdlCyclData->direction = GOAL_MI_MCTC_DIR_PEER_TO;

            /* register dm partition */
            res = goal_miDmPartIdxReg(pHdlCyclData->direction, GOAL_ID_EIP, &pHdlCyclData->dmData, pHdlCyclData->len, idxDataIn);

            /* store MI DM Output group handle */
            goal_miDmGroupGetByIdx(&((EIP_INSTANCE_T *) pHdlEip)->pDmGroupInput, pHdlCyclData->direction, GOAL_ID_EIP);

            /* store dm data handle */
            if (NULL != ppPartDataIn) {
                *ppPartDataIn = &pHdlCyclData->dmData;
            }
        }
    }

    return res;
}


/****************************************************************************/
/** Add an assembly for connection
 *
 * This function maps a single assembly to DM for a connection.
 * The direction has to be set manual and the assembly is only added, if the
 * flag flgDm is set.
 *
 * Send a request to the communication core to execute the server function.
 * The following parameters are pushed to the request:
 *   1. EtherNet/IP handle ID
 *   2. assemmbly ID
 *   3. direction
 *
 * RPC response parameters:
 *   1. index for data on DM
 *
 * @retval GOAL_OK success
 * @retval other failure
 */

GOAL_STATUS_T goal_eipDmAssemblyAdd(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    GOAL_MI_DM_PART_T **ppPartData,             /**< [out] partition for data */
    uint32_t instanceId,                        /**< instance number of the assembly object */
    uint32_t direction                          /**< direction of assembly object (GOAL_MI_MCTC_DIR_PEER_FROM, GOAL_MI_MCTC_DIR_PEER_TO) */
)
{
    GOAL_STATUS_T res;                          /* result */
    uint32_t idxData = 0;                       /* index for data */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */
    GOAL_EIP_CYCLIC_DATA_T *pHdlCyclData = NULL; /* handle of cyclic data */

    /* get structures ob assemblies to check their configuration */
    res = goal_eipRpcGetAssemblyObject((EIP_INSTANCE_T *) pHdlEip, instanceId, &pHdlCyclData);
    if (GOAL_RES_OK(res)) {
        if (GOAL_TRUE == goal_miDmPartStatusGet(&pHdlCyclData->dmData)) {
            /* object is already mapped - no not map again */
            if (NULL != ppPartData) {
                *ppPartData = &pHdlCyclData->dmData;
            }
            return GOAL_OK;
        }

        if (GOAL_TRUE != pHdlCyclData->flgDm) {
            /* assembly shall not be mapped */
            return GOAL_ERR_PARAM;
        }

        if ((GOAL_MI_MCTC_DIR_PEER_FROM != direction) && (GOAL_MI_MCTC_DIR_PEER_TO != direction)) {
            /* there are only these two directions allowed */
            return GOAL_ERR_PARAM;
        }

        /* store the direction of the assembly */
        pHdlCyclData->direction = direction;
    }


    /* get a new rpc handle */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcNew(&pHdlRpc, pHdlRpcChn);
    }

    /* push parameters */
    GOAL_RPC_PUSH(direction);
    GOAL_RPC_PUSH(instanceId);

    /* push the EtherNet/IP handle */
    GOAL_EIP_RPC_PUSH_INST(((EIP_INSTANCE_T *) pHdlEip));

    /* call the procedure at the server */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_EIP, GOAL_RPC_FUNC_DM_ASM_ADD);
    }

    GOAL_RPC_POP(idxData, uint32_t);
    GOAL_RPC_CLOSE();

    if (GOAL_RES_OK(res)) {
        /* register dm partition */
        res = goal_miDmPartIdxReg(direction, GOAL_ID_EIP, &pHdlCyclData->dmData, pHdlCyclData->len, idxData);
    }

    if (GOAL_RES_OK(res)) {
        /* store dm data handle */
        if (NULL != ppPartData) {
            *ppPartData = &pHdlCyclData->dmData;
        }
    }

    return res;
}


/****************************************************************************/
/** EIP CTC Add Data Provider To Cyclic Data
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_eipDmDpAdd(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t idMiDmPeerTo,                      /**< MI DM 'to peer' handle */
    GOAL_MI_DM_PART_T **ppPartDp                /**< [out] partition for data provider status */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */
    uint32_t posDp = 0;                         /* cyclic index for data provider status */
    EIP_INSTANCE_T *pEip;                       /* EtherNet/IP instance */

    /* The direction will be forced to GOAL_MI_MCTC_DIR_PEER_TO, because the CC sends the DP. */
    UNUSEDARG(idMiDmPeerTo);

    /* get EtherNet/IP instance */
    pEip = (EIP_INSTANCE_T *) pHdlEip;

    /* test if the DP has already been mapped to DM */
    if (GOAL_TRUE == goal_miDmPartStatusGet(&pEip->dmDp)) {
        /* DP is already mapped - no not map again */
        if (NULL != ppPartDp) {
            *ppPartDp = &pEip->dmDp;
        }
        return GOAL_OK;
    }

    /* get a new rpc handle */
    res = goal_rpcNew(&pHdlRpc, pHdlRpcChn);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* push the EtherNet/IP handle */
    GOAL_EIP_RPC_PUSH_INST(pEip);

    /* push arguments */
    /* The direction need to be pushed from CC point of view. */
    GOAL_RPC_PUSH(GOAL_MI_MCTC_DIR_PEER_TO);

    /* call the procedure at the server */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_EIP, GOAL_RPC_FUNC_DM_DP_ADD);
    }

    GOAL_RPC_POP(posDp, uint32_t);
    GOAL_RPC_CLOSE();

    if (GOAL_RES_OK(res)) {
        /* CC: Provider Status -> DM, add partition with fixed index */
        res = goal_miDmPartIdxReg(GOAL_MI_MCTC_DIR_PEER_FROM, GOAL_ID_EIP, &pEip->dmDp, sizeof(GOAL_MI_MCTC_DP_T), posDp);
        if (GOAL_RES_OK(res)) {
            /* partition for data provider status */
            if (NULL != ppPartDp) {
                  *ppPartDp = &pEip->dmDp;
            }
            /* store MI DM Output group handle */
            goal_miDmGroupGetByIdx(&pEip->pDmGroupOutput, GOAL_MI_MCTC_DIR_PEER_FROM, GOAL_ID_EIP);
        }
    }

    return res;
}
#endif /* GOAL_CONFIG_MEDIA_MI_DM == 1 */


/****************************************************************************/
/** Set the vendor id in device identity
 *
 * @retval GOAL_OK success
 * @retval other failure
 */
GOAL_STATUS_T goal_eipCfgVendorIdSet(
    uint16_t vendorId                           /**< vendor id */
)
{
    GOAL_STATUS_T res;                          /* return value */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */

    /* get a new rpc handle */
    res = goal_rpcNew(&pHdlRpc, pHdlRpcChn);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    GOAL_RPC_PUSH(vendorId);

    /* call the procedure at the server */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_EIP, GOAL_RPC_FUNC_CFG_SET_VENDORID);
    }

    /* rpc is finished - close the handle */
    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** Set the device type in device identity
 *
 * @retval GOAL_OK success
 * @retval other failure
 */
GOAL_STATUS_T goal_eipCfgDeviceTypeSet(
    uint16_t deviceType                         /**< device id */
)
{
    GOAL_STATUS_T res;                          /* return value */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */

    /* get a new rpc handle */
    res = goal_rpcNew(&pHdlRpc, pHdlRpcChn);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    GOAL_RPC_PUSH(deviceType);

    /* call the procedure at the server */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_EIP, GOAL_RPC_FUNC_CFG_SET_DEVTYPE);
    }

    /* rpc is finished - close the handle */
    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** Set the product code in device identity
 *
 * @retval GOAL_OK success
 * @retval other failure
 */
GOAL_STATUS_T goal_eipCfgProductCodeSet(
    uint16_t productCode                        /**< product code */
)
{
    GOAL_STATUS_T res;                          /* return value */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */

    /* get a new rpc handle */
    res = goal_rpcNew(&pHdlRpc, pHdlRpcChn);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    GOAL_RPC_PUSH(productCode);

    /* call the procedure at the server */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_EIP, GOAL_RPC_FUNC_CFG_SET_PRODUCTCODE);
    }

    /* rpc is finished - close the handle */
    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** Set the revision in device identity
 *
 * @retval GOAL_OK success
 * @retval other failure
 */
GOAL_STATUS_T goal_eipCfgRevisionSet(
    uint8_t revMajor,                           /**< major revision */
    uint8_t revMinor                            /**< minor revision */
)
{
    GOAL_STATUS_T res;                          /* return value */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */
    uint16_t revision;                          /* revision */

    /* get a new rpc handle */
    res = goal_rpcNew(&pHdlRpc, pHdlRpcChn);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    revision = (uint16_t) (((uint16_t) revMajor << 8) | (uint16_t) revMinor);
    GOAL_RPC_PUSH(revision);

    /* call the procedure at the server */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_EIP, GOAL_RPC_FUNC_CFG_SET_REVISION);
    }

    /* rpc is finished - close the handle */
    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** Set the serial number in device identity
 *
 * @retval GOAL_OK success
 * @retval other failure
 */
GOAL_STATUS_T goal_eipCfgSerialNumSet(
    uint32_t serial                             /**< serial number */
)
{
    GOAL_STATUS_T res;                          /* return value */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */

    /* get a new rpc handle */
    res = goal_rpcNew(&pHdlRpc, pHdlRpcChn);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    GOAL_RPC_PUSH(serial);

    /* call the procedure at the server */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_EIP, GOAL_RPC_FUNC_CFG_SET_SERIAL);
    }

    /* rpc is finished - close the handle */
    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** Set a generic string in device identity
 *
 * @retval GOAL_OK success
 * @retval other failure
 */
static GOAL_STATUS_T goal_eipCfStringSet(
    const char *pStr,                           /**< string to set */
    GOAL_RPC_FUNC_ID funcId                     /**< attribute set function ID */
)
{
    GOAL_STATUS_T res;                          /* return value */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */

    /* get a new rpc handle */
    res = goal_rpcNew(&pHdlRpc, pHdlRpcChn);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* push the product name to the stack */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcArgPush(pHdlRpc, (const void *) pStr, (uint16_t) GOAL_STRLEN(pStr));
    }

    /* push the string length to the stack */
    GOAL_RPC_PUSH((uint16_t) GOAL_STRLEN(pStr));

    /* call the procedure at the server */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_EIP, funcId);
    }

    /* rpc is finished - close the handle */
    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** Set the product name in device identity
 *
 * @retval GOAL_OK success
 * @retval other failure
 */
GOAL_STATUS_T goal_eipCfgProductNameSet(
    const char *strName                         /**< product name */
)
{
    GOAL_STATUS_T res;                          /* return value */

    res = goal_eipCfStringSet(strName, GOAL_RPC_FUNC_CFG_SET_NAME);

    return res;
}


/****************************************************************************/
/** Set the default domain name in TCP/IP Interface
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_eipCfgDomainNameSet(
    const char *strDomainName                   /**< default domain name */
)
{
    GOAL_STATUS_T res;                          /* return value */

    res = goal_eipCfStringSet(strDomainName, GOAL_RPC_FUNC_CFG_SET_DOMAIN_NAME);

    return res;
}


/****************************************************************************/
/** Set the default host name in TCP/IP Interface
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_eipCfgHostNameSet(
    const char *strHostName                     /**< default host name */
)
{
    GOAL_STATUS_T res;                          /* return value */

    res = goal_eipCfStringSet(strHostName, GOAL_RPC_FUNC_CFG_SET_HOST_NAME);

    return res;
}


/****************************************************************************/
/** Set the number of Explicit Connections attribute
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_eipCfgNumExplicitConSet(
    uint16_t numConn                            /**< number of Explicit Connections */
)
{
    GOAL_STATUS_T res;                          /* return value */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */

    /* get a new rpc handle */
    res = goal_rpcNew(&pHdlRpc, pHdlRpcChn);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    GOAL_RPC_PUSH(numConn);

    /* call the procedure at the server */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_EIP, GOAL_RPC_FUNC_CFG_SET_EXPL_CONN);
    }

    /* rpc is finished - close the handle */
    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** Set the number of Implicit Connections attribute
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_eipCfgNumImplicitConSet(
    uint16_t numConn                            /**< number of Implicit Connections */
)
{
    GOAL_STATUS_T res;                          /* return value */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */

    /* get a new rpc handle */
    res = goal_rpcNew(&pHdlRpc, pHdlRpcChn);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    GOAL_RPC_PUSH(numConn);

    /* call the procedure at the server */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_EIP, GOAL_RPC_FUNC_CFG_SET_IMPL_CONN);
    }

    /* rpc is finished - close the handle */
    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** Set the support of ethernet link counter attribute
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_eipCfgEthLinkCountersOn(
    GOAL_BOOL_T enable                          /**< enable flag */
)
{
    GOAL_STATUS_T res;                          /* return value */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */

    /* get a new rpc handle */
    res = goal_rpcNew(&pHdlRpc, pHdlRpcChn);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    GOAL_RPC_PUSH(enable);

    /* call the procedure at the server */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_EIP, GOAL_RPC_FUNC_CFG_SET_ETH_LINK_CNT);
    }

    /* rpc is finished - close the handle */
    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** Set the support of forcing of Ethernet link speed & duplex mode attribute
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_eipCfgEthLinkControlOn(
    GOAL_BOOL_T enable                          /**< enable flag */
)
{
    GOAL_STATUS_T res;                          /* return value */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */

    /* get a new rpc handle */
    res = goal_rpcNew(&pHdlRpc, pHdlRpcChn);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    GOAL_RPC_PUSH(enable);

    /* call the procedure at the server */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_EIP, GOAL_RPC_FUNC_CFG_SET_ETH_LINK_CRTL);
    }

    /* rpc is finished - close the handle */
    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** Set the state of appling new Link speed & duplex settings after a
 * device reset only attribute
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_eipCfgChangeEthAfterResetOn(
    GOAL_BOOL_T enable                          /**< enable flag */
)
{
    GOAL_STATUS_T res;                          /* return value */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */

    /* get a new rpc handle */
    res = goal_rpcNew(&pHdlRpc, pHdlRpcChn);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    GOAL_RPC_PUSH(enable);

    /* call the procedure at the server */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_EIP, GOAL_RPC_FUNC_CFG_SET_CHANGE_ETH_AFTER_RESET);
    }

    /* rpc is finished - close the handle */
    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** Set the state of appling new IP configurations after a
 * device reset only attribute
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_eipCfgChangeIpAfterResetOn(
    GOAL_BOOL_T enable                          /**< enable flag */
)
{
    GOAL_STATUS_T res;                          /* return value */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */

    /* get a new rpc handle */
    res = goal_rpcNew(&pHdlRpc, pHdlRpcChn);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    GOAL_RPC_PUSH(enable);

    /* call the procedure at the server */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_EIP, GOAL_RPC_FUNC_CFG_SET_CHANGE_IP_AFTER_RESET);
    }

    /* rpc is finished - close the handle */
    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** Set number of Encapsulation sessions
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_eipCfgNumSessionsSet(
    uint16_t numSess                            /**< number of encapsulation sessions */
)
{
    GOAL_STATUS_T res;                          /* return value */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */

    /* get a new rpc handle */
    res = goal_rpcNew(&pHdlRpc, pHdlRpcChn);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    GOAL_RPC_PUSH(numSess);

    /* call the procedure at the server */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_EIP, GOAL_RPC_FUNC_CFG_SET_NUM_SESSIONS);
    }

    /* rpc is finished - close the handle */
    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** Set size of one tick (multiple of 1 ms)
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_eipCfgTickSet(
    uint32_t sizeTick                           /**< tick size (multiple of 1ms) */
)
{
    GOAL_STATUS_T res;                          /* return value */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */

    /* get a new rpc handle */
    res = goal_rpcNew(&pHdlRpc, pHdlRpcChn);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    GOAL_RPC_PUSH(sizeTick);

    /* call the procedure at the server */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_EIP, GOAL_RPC_FUNC_CFG_SET_TICK);
    }

    /* rpc is finished - close the handle */
    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** Enable the RunIdle Header for consumed connections
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_eipCfgO2TRunIdleHeaderOn(
    GOAL_BOOL_T enable                          /**< enable flag */
)
{
    GOAL_STATUS_T res;                          /* return value */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */

    /* get a new rpc handle */
    res = goal_rpcNew(&pHdlRpc, pHdlRpcChn);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    GOAL_RPC_PUSH(enable);

    /* call the procedure at the server */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_EIP, GOAL_RPC_FUNC_CFG_SET_O2T_IDLE_HEADER);
    }

    /* rpc is finished - close the handle */
    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** Enable the RunIdle Header for produced connections
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_eipCfgT2ORunIdleHeaderOn(
    GOAL_BOOL_T enable                          /**< enable flag */
)
{
    GOAL_STATUS_T res;                          /* return value */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */

    /* get a new rpc handle */
    res = goal_rpcNew(&pHdlRpc, pHdlRpcChn);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    GOAL_RPC_PUSH(enable);

    /* call the procedure at the server */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_EIP, GOAL_RPC_FUNC_CFG_SET_T2O_IDLE_HEADER);
    }

    /* rpc is finished - close the handle */
    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** Enable the QoS object
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_eipCfgQoSOn(
    GOAL_BOOL_T enable                          /**< enable flag */
)
{
    GOAL_STATUS_T res;                          /* return value */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */

    /* get a new rpc handle */
    res = goal_rpcNew(&pHdlRpc, pHdlRpcChn);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    GOAL_RPC_PUSH(enable);

    /* call the procedure at the server */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_EIP, GOAL_RPC_FUNC_CFG_SET_QOS);
    }

    /* rpc is finished - close the handle */
    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** Set number of simultaneously delayable Encapsulation messages
 *
 * @returns GOAL_STATUS_T resultl
 * @retval other failed
 */
GOAL_STATUS_T goal_eipCfgNumDelayedEncapMsgSet(
    uint16_t numMsg                             /**< number of simultaneously delayable Encapsulation messages */
)
{
    GOAL_STATUS_T res;                          /* return value */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */

    /* get a new rpc handle */
    res = goal_rpcNew(&pHdlRpc, pHdlRpcChn);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    GOAL_RPC_PUSH(numMsg);

    /* call the procedure at the server */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_EIP, GOAL_RPC_FUNC_CFG_SET_NUM_DELAYED_MSG);
    }

    /* rpc is finished - close the handle */
    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** Enable DHCP support
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_eipCfgDhcpOn(
    GOAL_BOOL_T enable                          /**< enable flag */
)
{
    GOAL_STATUS_T res;                          /* return value */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */

    /* get a new rpc handle */
    res = goal_rpcNew(&pHdlRpc, pHdlRpcChn);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    GOAL_RPC_PUSH(enable);

    /* call the procedure at the server */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_EIP, GOAL_RPC_FUNC_CFG_SET_DHCP);
    }

    /* rpc is finished - close the handle */
    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** Enable DLR support
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_eipCfgDlrOn(
    GOAL_BOOL_T enable                          /**< enable flag */
)
{
    GOAL_STATUS_T res;                          /* return value */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */

    /* get a new rpc handle */
    res = goal_rpcNew(&pHdlRpc, pHdlRpcChn);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    GOAL_RPC_PUSH(enable);

    /* call the procedure at the server */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_EIP, GOAL_RPC_FUNC_CFG_SET_DLR);
    }

    /* rpc is finished - close the handle */
    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** Enable ACD support
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_eipCfgAcdOn(
    GOAL_BOOL_T enable                          /**< enable flag */
)
{
    GOAL_STATUS_T res;                          /* return value */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */

    /* get a new rpc handle */
    res = goal_rpcNew(&pHdlRpc, pHdlRpcChn);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    GOAL_RPC_PUSH(enable);

    /* call the procedure at the server */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_EIP, GOAL_RPC_FUNC_CFG_SET_ACD);
    }

    /* rpc is finished - close the handle */
    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** Enable HW configurability support
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_eipCfgHwConfigOn(
    GOAL_BOOL_T enable                          /**< enable flag */
)
{
    GOAL_STATUS_T res;                          /* return value */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */

    /* get a new rpc handle */
    res = goal_rpcNew(&pHdlRpc, pHdlRpcChn);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    GOAL_RPC_PUSH(enable);

    /* call the procedure at the server */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_EIP, GOAL_RPC_FUNC_CFG_SET_HW_CFG);
    }

    /* rpc is finished - close the handle */
    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** Configure ACD Behavior in Conflict Situation
 *
 * @retval GOAL_OK success
 * @retval other failure
 */
GOAL_STATUS_T goal_eipCfgAcdConflictFallbackIp(
    uint32_t ipAddress,                         /**< configured ip address */
    uint32_t subnetMask,                        /**< configured subnetmask */
    uint32_t gateway                            /**< configured gateway */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */

    /* get a new rpc handle */
    res = goal_rpcNew(&pHdlRpc, pHdlRpcChn);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* push gateway, subnetmask and ip address */
    GOAL_RPC_PUSH(gateway);
    GOAL_RPC_PUSH(subnetMask);
    GOAL_RPC_PUSH(ipAddress);

    /* call the procedure at the server */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_EIP, GOAL_RPC_FUNC_CFG_SET_ACD_BEHAVIOR);
    }

    /* rpc is finished - close the handle */
    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** Reset ACD last conflict detected attribute
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_eipAcdLastConflictDetectedReset(
    GOAL_EIP_T *pHdlEip                         /**< GOAL Ethernet/IP handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */

    /* get a new rpc handle */
    res = goal_rpcNew(&pHdlRpc, pHdlRpcChn);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* push the EtherNet/IP handle */
    GOAL_EIP_RPC_PUSH_INST(((EIP_INSTANCE_T *) pHdlEip));

    /* call the procedure at the server */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_EIP, GOAL_RPC_FUNC_ACD_LAST_CONFLICT_RESET);
    }

    /* rpc is finished - close the handle */
    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** Get ACD last conflict detected attribute
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_eipAcdLastConflictDetectedGet(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/IP handle */
    GOAL_ACD_LAST_CONFLICT_DETECTED_T *pLastConflict /**< ACD attribute */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */
    GOAL_ACD_LAST_CONFLICT_DETECTED_T lastConflict; /* ACD attribute */

    /* get a new rpc handle */
    res = goal_rpcNew(&pHdlRpc, pHdlRpcChn);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* push the EtherNet/IP handle */
    GOAL_EIP_RPC_PUSH_INST(((EIP_INSTANCE_T *) pHdlEip));

    /* call the procedure at the server */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_EIP, GOAL_RPC_FUNC_ACD_LAST_CONFLICT_GET);
    }

    /* pop ACD data */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcArgPop(pHdlRpc, (void *) &lastConflict.arpPru, sizeof(GOAL_ACD_ARP_PDU_T));
    }
    if (GOAL_RES_OK(res)) {
        res = goal_rpcArgPop(pHdlRpc, (void *) &lastConflict.remoteMac, 6 * sizeof(uint8_t));
    }

    /* pop and pass data to application */
    GOAL_RPC_POP(lastConflict.acdActivity, uint8_t);
    if (GOAL_RES_OK(res)) {
        GOAL_MEMCPY(pLastConflict, &lastConflict, sizeof(GOAL_ACD_LAST_CONFLICT_DETECTED_T));
    }

    /* rpc is finished - close the handle */
    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** Reset Identity Object - Server
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_eipReset(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/IP handle */
    uint32_t resetService                       /**< requested reset service */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */

    /* get a new rpc handle */
    res = goal_rpcNew(&pHdlRpc, pHdlRpcChn);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* push the reset service and handle */
    GOAL_RPC_PUSH(resetService);
    GOAL_EIP_RPC_PUSH_INST(((EIP_INSTANCE_T *) pHdlEip));

    /* call the procedure at the server */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_EIP, GOAL_RPC_FUNC_IDENTITY_RESET);
    }

    /* rpc is finished - close the handle */
    GOAL_RPC_CLOSE();

    return res;
}


/****************************************************************************/
/** EtherNet/IP Callback
 *
 * Pop the data from the RPC stack, call the selected function and send a
 * response to the client.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_eipCbServer(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< RPC handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_EIP_CB_ID_T cbId = 0;                  /* Callback ID */
    uint16_t sizeData = 0;                      /* size of callback data */
    GOAL_EIP_CB_DATA_T cbData;                  /* callback parameters */
    GOAL_EIP_CB_DATA_T *pCbData = NULL;         /* callback parameters pointer */
    EIP_INSTANCE_T *pEip = NULL;                /* EtherNet/IP Instance */


    GOAL_EIP_RPC_POP_INST(pEip);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to get EtherNet/IP instance");
        return res;
    }

    GOAL_RPC_POP(cbId, GOAL_EIP_CB_ID_T);
    GOAL_RPC_POP(sizeData, uint16_t);

    /* no data will be poped when calling GOAL_RPC_PUSH_PTR with sizeData == 0 */
    GOAL_RPC_POP_PTR(&cbData, sizeData);

    if (GOAL_RES_OK(res) && (0 != sizeData)) {
        pCbData = &cbData;
    }

    /* call callback */
    if (GOAL_RES_OK(res)) {
        res = pEip->goal_eipCb((GOAL_EIP_T *) pEip, cbId, pCbData);
    }

    /* inform CC for changes in data
     * no data will be pushed when calling GOAL_RPC_PUSH_PTR with sizeData == 0 */
    GOAL_RPC_PUSH_PTR(pCbData, sizeData)

    GOAL_RPC_PUSH(sizeData);

    return res;
}


/****************************************************************************/
/** Register an application specific CIP class via RPC
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_eipCipClassRegister(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/IP handle */
    uint16_t classId,                           /**< class ID to be registered */
    GOAL_EIP_REQ_HANDLER_T pFunc                /**< request handler */
)
{
    GOAL_STATUS_T res;                          /* return value */
    GOAL_RPC_HDL_T *pHdlRpc = GOAL_RPC_HDL_NONE; /* call handle */
    GOAL_EIP_CIP_CLASS_T *pNew = NULL;          /* new CIP class entry */

    /* get a new rpc handle */
    res = goal_rpcNew(&pHdlRpc, pHdlRpcChn);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    GOAL_RPC_PUSH(classId);

    /* push the EtherNet/IP handle */
    GOAL_EIP_RPC_PUSH_INST(((EIP_INSTANCE_T *) pHdlEip));

    /* call the procedure at the server */
    if (GOAL_RES_OK(res)) {
        res = goal_rpcCall(pHdlRpc, GOAL_ID_EIP, GOAL_RPC_FUNC_CIP_CLASS_REGISTER);
    }

    /* close the handle */
    GOAL_RPC_CLOSE();

    if (GOAL_RES_OK(res)) {
        /* create new CIP class instance */
        res = goal_memCalloc(&pNew, sizeof(GOAL_EIP_CIP_CLASS_T));
    }

    if (GOAL_RES_OK(res)) {
        /* write data to instace */
        pNew->classId = classId;
        pNew->pFunc = pFunc;

        GOAL_LL_APPEND(((EIP_INSTANCE_T *) pHdlEip)->pCipClass, pNew);
    }

    return res;
}


/****************************************************************************/
/** Request handler for Virtual Parameter Class - Server
 *
 * Pop the data from the RPC stack, call the selected function and send a
 * response to the client.
 * The following parameters are pushed to the request:
 *   1. EtherNet/IP handle ID
 *   2. service ID
 *   3. class ID
 *   3. instance ID
 *   4. attribute ID
 *   5. member ID
 *   6. request data length
 *   7. request data
 *   8. response data length (available)
 *
 * RPC response parameters:
 *   1. general status
 *   2. additional status size
 *   3. additional status
 *   4. attribute ID
 *   5. response data length (used)
 *   6. response data
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_eipParamClassHandlerCbServer(
    GOAL_RPC_HDL_T *pHdlRpc                     /**< RPC handle */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    GOAL_STATUS_T resCb = GOAL_OK;              /* result from callback */
    EIP_INSTANCE_T *pEip = NULL;                /* GOAL EtherNet/IP instance */
    GOAL_EIP_MR_REQ_T hdlReq;                   /* request structure */
    GOAL_EIP_MR_RES_T hdlRes;                   /* response structure */
    GOAL_EIP_CIP_CLASS_T *pIdxCipClass = NULL;  /* index of virtual CIP class */
    GOAL_BUFFER_T *pBufReq = NULL;              /* request buffer */
    GOAL_BUFFER_T *pBufRes = NULL;              /* response buffer */
    GOAL_BOOL_T flgBufReq = GOAL_FALSE;         /* flag: request buffer available */
    GOAL_BOOL_T flgBufRes = GOAL_FALSE;         /* flag: response buffer available */

    /* prepare handles */
    GOAL_MEMSET(&hdlReq, 0, sizeof(GOAL_EIP_MR_REQ_T));
    GOAL_MEMSET(&hdlRes, 0, sizeof(GOAL_EIP_MR_RES_T));

    /* pop the EIP instance */
    GOAL_EIP_RPC_POP_INST(pEip);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to get EtherNet/IP instance");
        return res;
    }

    /* pop elements of request structure */
    GOAL_RPC_POP(hdlReq.serviceId, uint8_t);
    GOAL_RPC_POP(hdlReq.classId, uint16_t);

    /* get meta data of virtual CIP class to check if the class is available */
    if (GOAL_RES_OK(res)) {
        GOAL_LL_FOREACH(pEip->pCipClass, pIdxCipClass) {
            if (pIdxCipClass->classId == hdlReq.classId) {
              /* class found */
              break;
            }
        }
    }

    if (GOAL_RES_OK(res) && (NULL == pIdxCipClass)) {
        /* CIP class not available */
        res = GOAL_ERR_PARAM;
    }

    GOAL_RPC_POP(hdlReq.instanceId, uint16_t);
    GOAL_RPC_POP(hdlReq.attributeId, uint16_t);
    GOAL_RPC_POP(hdlReq.memberId, uint16_t);
    GOAL_RPC_POP(hdlReq.dataLen, uint16_t);

    /* validation of data length */
    if (GOAL_RES_OK(res)) {
        if (GOAL_EIP_MR_DATA_SIZE < hdlReq.dataLen) {
            /* invalid buffer length */
            res = GOAL_ERR_PARAM;
        }
    }

    /* get data buffer for request */
    if (GOAL_RES_OK(res)) {
        res = goal_queuePoolGetBuf(&pBufReq, hdlReq.dataLen, GOAL_ID);
        if (GOAL_RES_OK(res)) {
            /* set flag of available buffer */
            flgBufReq = GOAL_TRUE;

            /* link the request data buffer */
            hdlReq.data = pBufReq->ptrData;
        }
    }

    /* pop the data */
    GOAL_RPC_POP_PTR(hdlReq.data, hdlReq.dataLen);

    /* pop length of response */
    GOAL_RPC_POP(hdlRes.dataLen, uint16_t);

    /* validation of response data length */
    if (GOAL_RES_OK(res)) {
        if (GOAL_EIP_MR_DATA_SIZE < hdlRes.dataLen) {
            /* invalid buffer length */
            res = GOAL_ERR_PARAM;
        }
    }

    /* get data buffer for response */
    if (GOAL_RES_OK(res)) {
        res = goal_queuePoolGetBuf(&pBufRes, hdlRes.dataLen, GOAL_ID);
        if (GOAL_RES_OK(res)) {
            /* set flag of available buffer */
            flgBufRes = GOAL_TRUE;

            /* link the response data buffer */
            hdlRes.data = pBufRes->ptrData;
        }
    }
    if (GOAL_RES_OK(res)) {
        /* call the callback */
        res = pIdxCipClass->pFunc((GOAL_EIP_T *) pEip, &hdlReq, &hdlRes);

        /* keep result for return value  */
        resCb = res;
    }

    /* check for data overflow */
    if (GOAL_RES_OK(res) && (GOAL_EIP_MR_DATA_SIZE < hdlRes.dataLen)) {
        res = GOAL_ERR_OVERFLOW;
    }

    /* push elements of response structure in reverse order */
    GOAL_RPC_PUSH_PTR(hdlRes.data, hdlRes.dataLen);
    GOAL_RPC_PUSH(hdlRes.dataLen);
    GOAL_RPC_PUSH(hdlRes.attributeId);
    /* additionalStatusSize is number of 16 bit words in additionalStatus */
    GOAL_RPC_PUSH_PTR(hdlRes.additionalStatus, 2 * hdlRes.additionalStatusSize);
    GOAL_RPC_PUSH(hdlRes.additionalStatusSize);
    GOAL_RPC_PUSH(hdlRes.generalStatus);

    /* free the buffers */
    if (GOAL_TRUE == flgBufRes) {
        goal_queueReleaseBuf(&pBufRes);
    }
    if (GOAL_TRUE == flgBufReq) {
        goal_queueReleaseBuf(&pBufReq);
    }

    return GOAL_RES_OK(res) ? (resCb) : (res);
}

#endif /* GOAL_CONFIG_GEN_RPC */
