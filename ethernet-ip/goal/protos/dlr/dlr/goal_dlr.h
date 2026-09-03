/** @file
 *
 * @brief Device Level Ring Protocol (DLR)
 *
 * This module implements the DLR protocol for a Beacon-based Ring Node.
 * It handles the State Machine and processes incoming DLR frames.
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

#ifndef GOAL_DLR_H
#define GOAL_DLR_H

#include "goal_includes.h"


/****************************************************************************/
/* Configuration */
/****************************************************************************/
#ifndef DLR_CONFIG_BEACON_RINGNODE
#  define DLR_CONFIG_BEACON_RINGNODE 0
#  define DLR_CONFIG_ANNOUNCE_RINGNODE 1
#else
#  define DLR_CONFIG_ANNOUNCE_RINGNODE 0
#endif

#ifndef DLR_CONFIG_BEACON_NO_HW_SUPPORT
#  define DLR_CONFIG_BEACON_NO_HW_SUPPORT 0
#endif


/****************************************************************************/
/* Public Defines */
/****************************************************************************/
#define DLR_STACK_VERSION               "2.0.0" /**< current version */
#define DLR_INSTANCE_DEFAULT                  0 /**< default instance Id */

#define GOAL_DLR_CB_DATA_MAX                 10 /**< max count of callback data */

#define DLR_SUVI_STATUS_BACKUP                0 /**< node is a Backup Supervisor */
#define DLR_SUVI_STATUS_ACTIVE                1 /**< node is the Active Supervisor */
#define DLR_SUVI_STATUS_DISABLED              2 /**< Supervisor functon disabled */
#define DLR_SUVI_STATUS_NON_DLR               3 /**< no Supervisor Node present */
#define DLR_SUVI_STATUS_CFG_UNSUP             4 /**< Supervisor configuration not supported */


/****************************************************************************/
/* Public Data types */
/****************************************************************************/
typedef void *GOAL_DLR_T;                       /**< GOAL DLR handle */

/**< DLR callback id */
typedef uint32_t GOAL_DLR_CB_ID_T;

/**< Callback Data Content Structure */
typedef union {
    GOAL_BOOL_T suViCfgStatus;                  /**< Supervisor configuration Status */
} GOAL_DLR_CB_DATA_ELEM_T;


/**< Callback Data Structure */
typedef struct {
    GOAL_DLR_CB_DATA_ELEM_T data[GOAL_DLR_CB_DATA_MAX]; /**< callback data elements */
} GOAL_DLR_CB_DATA_T;


/**< DLR callback function */
typedef GOAL_STATUS_T (* GOAL_DLR_FUNC_CB_T) (
    GOAL_DLR_T *,                               /**< DLR handle */
    void *,                                     /**< user callback argument */
    GOAL_DLR_CB_ID_T,                           /**< callback id */
    GOAL_DLR_CB_DATA_T *                        /**< callback parameters */
);

/** DLR events for the DLR State Machine */
typedef enum {
    DLR_EVENT_LINKLOST,                         /**< Link was lost */
    DLR_EVENT_OWNFRAME,                         /**< the device received its own frame */
    DLR_EVENT_LINKRESTORED,                     /**< Link is restored */
    DLR_EVENT_BEACONFRAME,                      /**< Beacon Frame received */
    DLR_EVENT_BEACONTIMEOUT,                    /**< Beacon Timer timed out */
    DLR_EVENT_NEWSUPERVISOR,                    /**< a new Ring Supervisor was detected */
    DLR_EVENT_LOCATEFAULT,                      /**< locate Fault frame received */
    DLR_EVENT_NEIGHBORCHECKREQ,                 /**< Neighbor_Check_Request received */
    DLR_EVENT_NEIGHBORCHECKRES,                 /**< Neighbor_Check_Response received */
    DLR_EVENT_NEIGHBORTIMEOUT,                  /**< NeighborCheck Timer expired */
    DLR_EVENT_FLUSHTABLES,                      /**< Flush_Tables Frame received */
    DLR_EVENT_SIGNON,                           /**< Sign_On Frame received */
    DLR_EVENT_NEWSTATE,                         /**< Beacon Hardware detected new state */
    DLR_EVENT_ANNOUNCE_RX,                      /**< Announce frame received */
    DLR_EVENT_ANNOUNCE_TMO,                     /**< Announce reception timeout */
    DLR_EVENT_LINKSTATUS_RX,                    /**< Link_Status frame received */
    DLR_EVENT_LOCATE_FAULT_CMD,                 /**< received Verify_Fault_Location command */
    DLR_EVENT_PARTIAL_FAULT,                    /**< partial network fault detected */
} DLR_EVENT_T;


/****************************************************************************/
/* Function and Parameter Mapping */
/****************************************************************************/
/* callback function list */
#define GOAL_DLR_FUNC_CB_LIST \
    /*                      id, name,                                       function */ \
    GOAL_DLR_FUNC_CB_ENTRY(1, GOAL_DLR_CB_ID_SUVICFG_CHANGE_REMOTE,       NULL)

/* create callback function id enum */
#undef GOAL_DLR_FUNC_CB_ENTRY
#define GOAL_DLR_FUNC_CB_ENTRY(id, name, ...) name = id
typedef enum {
    GOAL_DLR_FUNC_CB_LIST
} GOAL_DLR_FUNC_CB_ID_T;


/****************************************************************************/
/* Public Functions */
/****************************************************************************/
GOAL_STATUS_T goal_dlrNew(
    GOAL_DLR_T **ppHdlDlr,                      /**< [out] DLR handle reference */
    uint32_t instanceId,                        /**< instance ID */
    GOAL_DLR_FUNC_CB_T pFunc,                   /**< callback function */
    void *pCbArg,                               /**< callback function argument */
    GOAL_ETH_DLR_SUVI_CFG_T *pSuViCfg,          /**< [in] Supervisor configuration */
    GOAL_MI_ETH_T *pMiEthHdl,                   /**< Eth Mi handle of interface */
    GOAL_MI_NET_T *pMiNetHdl                    /**< Net MI handle of interface */
);

GOAL_STATUS_T goal_dlrReset(
    GOAL_DLR_T *pHdlDlr                         /**< DLR handle */
);

void goal_dlrStateMachineExec(
    GOAL_DLR_T *pHdlDlr,                        /**< DLR handle */
    DLR_EVENT_T event,                          /**< event ID */
    uint32_t port                               /**< source port of DLR event */
);

uint32_t goal_dlrCapabilityGet(
    GOAL_DLR_T *pHdlDlr                         /**< DLR handle */
);

GOAL_STATUS_T goal_dlrNetStatusGet(
    GOAL_DLR_T *pHdlDlr,                        /**< DLR handle */
    uint8_t *pNetStatus                         /**< [out] current network status */
);

GOAL_STATUS_T goal_dlrNetTopologyGet(
    GOAL_DLR_T *pHdlDlr,                        /**< DLR handle */
    uint8_t *pNetTopology                       /**< [out] current network topology */
);

GOAL_STATUS_T goal_dlrSupervisorAddrGet(
    GOAL_DLR_T *pHdlDlr,                        /**< DLR handle */
    uint32_t *pIPAddr,                          /**< Supervisor IP address */
    uint8_t *pMAC                               /**< Supervisor MAC address */
);


#endif /* GOAL_DLR_H */
