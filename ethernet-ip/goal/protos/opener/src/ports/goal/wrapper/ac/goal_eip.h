/** @file
 *
 * @brief common header for Ethernet/IP
 *
 * This header file must be included by all files that are using Ethernet/IP.
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


#ifndef GOAL_EIP_H
#define GOAL_EIP_H

#include <goal_eip_rpc.h>
#include <goal_ciptypes.h>


/****************************************************************************/
/* Defines */
/****************************************************************************/
#define GOAL_OK_EIP_SEND                (GOAL_RES_EIP) /**< OK EIP send */
#define GOAL_ERR_EIP                    (GOAL_ERROR | GOAL_RES_EIP) /**< general EIP error */

#define GOAL_UNUSED_EIP_HDL (NULL)              /**< define for unused EIP handle */
#define GOAL_EIP_CB_DATA_MAX                10  /**< max count of callback data */
#define GOAL_EIP_CONNECTION_EVENT_OPENED     1  /**< connection event: opened */
#define GOAL_EIP_CONNECTION_EVENT_TIMEOUT    2  /**< connection event: timed out */
#define GOAL_EIP_CONNECTION_EVENT_CLOSED     3  /**< connection event: closed */

#define GOAL_EIP_RESET_NO                    1  /**< reset : no reset */
#define GOAL_EIP_RESET_POWER                 2  /**< reset : power cycle */
#define GOAL_EIP_RESET_FACTORY               3  /**< reset : factory reset */
#define GOAL_EIP_RESET_FACTORY_OTHER         4  /**< reset : factory reset */
#define GOAL_EIP_RESET_FACTORY_COMM          5  /**< reset : factory reset communication parameters */

#define GOAL_EIP_MR_ADD_STATUS_SIZE  2          /**< number of words for Additional Status */
#define GOAL_EIP_MR_DATA_SIZE 500               /**< maximum data size of a MR Request/Response */

#define GOAL_EIP_VERSION_LENGTH 8               /**< length of the version string */

/* General Status Codes for Message Router Requests */
#define GOAL_EIP_MR_STATUS_SUCCESS   0x00       /**< success */
#define GOAL_EIP_MR_STATUS_PATH_UNKNOWN  0x05       /**< invalid class or instance */
#define GOAL_EIP_MR_STATUS_INVALID_SRVC_ID 0x08 /**< service not implemented */
#define GOAL_EIP_MR_STATUS_INVALID_ATTR_VAL 0x09 /**< invalid attribute value */
#define GOAL_EIP_MR_STATUS_OBJ_STATE_CONFLICT 0x0C /**< object state conflict */
#define GOAL_EIP_MR_STATUS_ATTR_NOT_SETTABLE 0x0E /**< attribute not settable */
#define GOAL_EIP_MR_STATUS_NOT_ENOUGH_DATA 0x13 /**< not enough data in request */
#define GOAL_EIP_MR_STATUS_INVALID_ATTR_ID 0x14 /**< attribute not implemented */
#define GOAL_EIP_MR_STATUS_TOO_MUCH_DATA 0x15   /**< too much data in request */
#define GOAL_EIP_MR_STATUS_ATTR_NOT_GETTABLE 0x2C /**< attribute not gettable */

#define GOAL_EIP_LED_MS_GREEN      (1u<<0)      /**< Module Status Green is on */
#define GOAL_EIP_LED_MS_RED        (1u<<1)      /**< Module Status Red is on */
#define GOAL_EIP_LED_NS_GREEN      (1u<<2)      /**< Network Status Green is on */
#define GOAL_EIP_LED_NS_RED        (1u<<3)      /**< Network Status Red is on */


/****************************************************************************/
/* Typedefs */
/****************************************************************************/
/**< status of the EIP application */
typedef enum {
    EIP_ERROR = -1,
    EIP_OK = 0,
    EIP_OK_SEND = 1
} EIP_STATUS;

/**< indicate if some kind of reset must be executed */
typedef enum
{
    enNoReset = 0,
    enReset = 1,
    enResetAll = 2,
    enResetOther = 3,
    enResetIpCfg = 4,
} EResetState;

/**< Ethernet/IP handle type */
typedef void *GOAL_EIP_T;

/* typedef for the TCP/IP service function */
typedef GOAL_STATUS_T (*goal_eipTcpIpServiceFunc)(
    GOAL_EIP_T *pHdlEip,                        /* GOAL Ethernet/ IP handle */
    uint32_t classId,                           /* class ID */
    uint32_t instanceId,                        /* instance ID */
    S_CIP_MR_Request *pMsgReq,                  /* request message to the service function */
    S_CIP_MR_Response *pMsgRes                  /* response message of the service function */
);

/* service descriptor. These are stored in an array */
typedef struct CIP_service_struct
{
    uint8_t CIP_ServiceNr;                      /* service number */
    goal_eipTcpIpServiceFunc m_ptfuncService;   /* pointer to a function call */
    char *name;                                 /* name of the service */
} S_CIP_service_struct;

/**< EIP callback id */
typedef uint32_t GOAL_EIP_CB_ID_T;

/**< Callback Data Content Structure */
typedef union {
    /* NOTE: do not increase the maximum size of 10 * 4 Byte to
     * ensure compatibility with older versions of the AC software
     */
    uint32_t connectionEvent;                   /**< event ID */

    uint32_t instanceNr;                        /**< instance number of the assembly object */

    uint32_t outputAssembly;                    /**< output assembly connection point */
    uint32_t inputAssembly;                     /**< input assembly connection point */
    uint32_t runIdleValue;                      /**< current value of the run/idle flag */

    uint32_t resetState;                        /**< reset state */

    uint32_t leds;                              /**< active LEDs */

    uint8_t minorRevision;                      /**< minor revision */

    uint8_t dhcpEnabled;                        /**< DHCP enabled */
    uint8_t acdActivity;                        /**< ACD status */
    uint32_t acdRemoteMacHigh;                  /**< conflicted device address */
    uint16_t acdRemoteMacLow;                   /**< conflicted device address */
} GOAL_EIP_CB_DATA_ELEM_T;


/**< Callback Data Structure */
typedef struct {
    GOAL_EIP_CB_DATA_ELEM_T data[GOAL_EIP_CB_DATA_MAX]; /**< callback data elements */
} GOAL_EIP_CB_DATA_T;


/**< EIP callback function */
typedef GOAL_STATUS_T (* GOAL_EIP_FUNC_CB_T)(
    GOAL_EIP_T *,                               /**< EIP handle */
    GOAL_EIP_CB_ID_T,                           /**< callback id */
    GOAL_EIP_CB_DATA_T *                        /**< callback parameters */
);

/**< EtherNet/IP cyclic data type */
typedef struct GOAL_EIP_CYCLIC_DATA_T {
    struct GOAL_EIP_CYCLIC_DATA_T *pNext;       /**< next list element */
    GOAL_BOOL_T flgDm;                          /**< flag for mapping assembly to DM */
    uint32_t instanceNr;                        /**< instance number of the assembly object */
    uint16_t len;                               /**< length of the assembly object's data */
#if GOAL_CONFIG_MEDIA_MI_DM == 1
    uint32_t direction;                         /**< assembly direction */
    GOAL_MI_DM_PART_T dmData;                   /**< assembly data */
#endif /* GOAL_CONFIG_MEDIA_MI_DM == 1 */
} GOAL_EIP_CYCLIC_DATA_T;

/** CIP Message Router Request */
typedef struct {
    uint8_t serviceId;
    uint16_t classId;
    uint16_t instanceId;
    uint16_t attributeId;
    uint16_t memberId;
    uint16_t dataLen;
    uint8_t *data;
} GOAL_EIP_MR_REQ_T;

/** CIP Message Router Response */
typedef struct {
    uint8_t generalStatus;
    uint8_t additionalStatusSize;
    uint16_t additionalStatus[GOAL_EIP_MR_ADD_STATUS_SIZE];
    uint16_t attributeId;
    uint16_t dataLen;
    uint8_t *data;
} GOAL_EIP_MR_RES_T;

/** Request handler for registered classes */
typedef GOAL_STATUS_T (*GOAL_EIP_REQ_HANDLER_T) (
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/IP handle */
    GOAL_EIP_MR_REQ_T *pMsgReq,                 /**< request to CIP object */
    GOAL_EIP_MR_RES_T *pMsgRes                  /**< response from CIP object */
);

/** Registered class List element */
typedef struct GOAL_EIP_CIP_CLASS_T {
    struct GOAL_EIP_CIP_CLASS_T *pNext;         /**< next element */
    uint16_t classId;                           /**< Id of registered class */
    GOAL_EIP_REQ_HANDLER_T pFunc;               /**< registered class */
} GOAL_EIP_CIP_CLASS_T;

/**< ARP PDU */
GOAL_TARGET_PACKED_STRUCT_PRE
typedef struct {
    uint16_t hwAddressType_be16;                /**< 1 for Ethernet H/W */
    uint16_t protoAddressType_be16;             /**< 0x800 for IP */
    uint8_t hwAddrLen;                          /**< 6 for Ethernet h/w */
    uint8_t protoAddrLen;                       /**< 4 for IP */
    uint16_t operation_be16;                    /**< 1 for Req or 2 for Rsp */
    uint8_t senderHwAddress[6];                 /**< Sender's h/w addr */
    uint8_t senderProtoAddress[4];              /**< Sender's proto addr */
    uint8_t targetHwAddress[6];                 /**< Target's h/w addr */
    uint8_t targetProtodAdress[4];              /**< Target's proto addr */
} GOAL_TARGET_PACKED GOAL_ACD_ARP_PDU_T;

/**< last ACD conflict data */
typedef GOAL_TARGET_PACKED_PRE struct {
    uint8_t acdActivity;                        /**< State of ACD activity when last conflict detected */
    uint8_t remoteMac[6];                       /**< MAC address of remote node from the ARP PDU in which a conflict was detected */
    GOAL_ACD_ARP_PDU_T arpPru;                  /**< Copy of the raw ARP PDU in which a conflict was detected. */
} GOAL_TARGET_PACKED GOAL_ACD_LAST_CONFLICT_DETECTED_T;

/** EtherNet/IP instance data */
typedef struct EIP_INSTANCE_T {
    GOAL_INSTANCE_HEADER(EIP_INSTANCE_T);       /**< instance header */
    GOAL_EIP_FUNC_CB_T goal_eipCb;              /**< application callback */
    GOAL_EIP_CYCLIC_DATA_T *pEipCyclic;         /**< list of registered assembly objects */
    GOAL_EIP_CIP_CLASS_T *pCipClass;            /**< CIP Class objects */
    char versionBuf[GOAL_EIP_VERSION_LENGTH];   /**< Ethernet/IP version */
#if GOAL_CONFIG_MEDIA_MI_DM == 1
    GOAL_MI_DM_GROUP_T *pDmGroupInput;          /**< MI DM Input group handle */
    GOAL_MI_DM_GROUP_T *pDmGroupOutput;         /**< MI DM Output group handle */
    GOAL_MI_DM_PART_T dmDp;                     /**< data provider dm handle */
    GOAL_MI_MCTC_DP_T dataDp;                   /**< generic data provider */
    GOAL_BOOL_T flgRpcLoop;                     /**< run EIP RPC loop */
#endif
} EIP_INSTANCE_T;

/**< Ethernet IP Assembly type */
typedef enum {
    GOAL_EIP_ASM_EXCLUSIVE_OWNER,               /**< exclusive owner */
    GOAL_EIP_ASM_INPUT_ONLY,                    /**< input only assembly */
    GOAL_EIP_ASM_LISTEN_ONLY                    /**< listen only assembly */
} GOAL_EIP_CONNECTION_T;


/****************************************************************************/
/* Function and Parameter Mapping */
/****************************************************************************/
#define GOAL_EIP_FUNC_CALLBACK 0               /**< callback function id */

/* callback function list */
#define GOAL_EIP_FUNC_CB_LIST \
    /*                      id, name,                                       function */ \
    GOAL_EIP_FUNC_CB_ENTRY(1, GOAL_EIP_CB_ID_DEVICE_RESET,        NULL), \
    GOAL_EIP_FUNC_CB_ENTRY(2, GOAL_EIP_CB_ID_RUN_IDLE_CHANGED,    NULL), \
    GOAL_EIP_FUNC_CB_ENTRY(3, GOAL_EIP_CB_ID_ASSEMBLY_DATA_SEND,  NULL), \
    GOAL_EIP_FUNC_CB_ENTRY(4, GOAL_EIP_CB_ID_ASSEMBLY_DATA_RECV,  NULL), \
    GOAL_EIP_FUNC_CB_ENTRY(5, GOAL_EIP_CB_ID_CONNECT_EVENT,       NULL), \
    GOAL_EIP_FUNC_CB_ENTRY(6, GOAL_EIP_CB_ID_READY,               NULL), \
    GOAL_EIP_FUNC_CB_ENTRY(7, GOAL_EIP_CB_ID_LED_CHANGED,         NULL), \
    GOAL_EIP_FUNC_CB_ENTRY(8, GOAL_EIP_CB_ID_REVISION_CHECK,      NULL), \
    GOAL_EIP_FUNC_CB_ENTRY(9, GOAL_EIP_CB_ID_ACD_CONFLICT,       NULL)

/* create callback function id enum */
#undef GOAL_EIP_FUNC_CB_ENTRY
#define GOAL_EIP_FUNC_CB_ENTRY(id, name, ...) name = id
typedef enum {
    GOAL_EIP_FUNC_CB_LIST
} GOAL_EIP_FUNC_CB_ID_T;

/* map RPC assembly creation as default */
#define goal_eipCreateAssemblyObject goal_eipCreateAssemblyObjectDm


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_eipInit(
    void
);

GOAL_STATUS_T goal_eipNew(
    GOAL_EIP_T **ppEip,                         /**< EtherNet/IP instance ref */
    const uint32_t id,                          /**< instance id */
    GOAL_EIP_FUNC_CB_T pFunc                    /**< EtherNet/IP callback function */
);

GOAL_STATUS_T goal_eipNewIfaceAssign(
    GOAL_EIP_T **ppEip,                         /**< EtherNet/IP instance ref */
    const uint32_t id,                          /**< instance id */
    GOAL_EIP_FUNC_CB_T pFunc,                   /**< EtherNet/IP callback function */
    uint32_t ethMiId,                           /**< ID of assigned ETH MI */
    uint32_t netMiId                            /**< ID of assigned NET MI */
);

GOAL_STATUS_T goal_eipGetVersion(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    char **ppVersion                            /**< Ethernet/IP version */
);

GOAL_STATUS_T goal_eipCreateAssemblyObjectRpc(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t instanceNr,                        /**< instance number of the assembly object to create */
    uint16_t len                                /**< length of the assembly object's data */
);

GOAL_STATUS_T goal_eipCreateAssemblyObjectDm(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t instanceNr,                        /**< instance number of the assembly object to create */
    uint16_t len                                /**< length of the assembly object's data */
);

GOAL_STATUS_T goal_eipAssemblyObjectGet(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t instanceId,                        /**< instance number of the assembly object to create */
    uint8_t **ppData,                           /**< data pointer of assembly */
    uint16_t *pLen                              /**< length of assembly data */
);

GOAL_STATUS_T goal_eipAssemblyObjectWrite(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t instanceId,                        /**< instance number of the assembly object to create */
    uint8_t *pData,                             /**< data pointer to write */
    uint16_t len                                /**< length of data to write */
);

GOAL_STATUS_T goal_eipAssemblyObjectRead(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t instanceId,                        /**< instance number of the assembly object to create */
    uint8_t *pData,                             /**< data pointer to store read data */
    uint16_t len                                /**< length of data to read */
);

GOAL_STATUS_T goal_eipAddExclusiveOwnerConnection(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t outputAssembly,                    /**< the O-to-T point to be used for this connection */
    uint32_t inputAssembly,                     /**< the T-to-O point to be used for this connection */
    uint32_t configAssembly                     /**< the configuration point to be used for this connection */
);

GOAL_STATUS_T goal_eipAddInputOnlyConnection(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t connNum,                           /**< the number of the input only connection. */
    uint32_t outputAssembly,                    /**< the O-to-T point to be used for this connection */
    uint32_t inputAssembly,                     /**< the T-to-O point to be used for this connection */
    uint32_t configAssembly                     /**< the configuration point to be used for this connection */
);

GOAL_STATUS_T goal_eipAddListenOnlyConnection(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t connNum,                           /**< the number of the input only connection. */
    uint32_t outputAssembly,                    /**< the O-to-T point to be used for this connection */
    uint32_t inputAssembly,                     /**< the T-to-O point to be used for this connection */
    uint32_t configAssembly                     /**< the configuration point to be used for this connection */
);

#if GOAL_CONFIG_MEDIA_MI_DM == 1
GOAL_STATUS_T goal_eipDmAddConnection(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    GOAL_MI_DM_PART_T **ppPartDataOut,          /**< [out] partition for output data */
    GOAL_MI_DM_PART_T **ppPartDataIn,           /**< [out] partition for input data */
    uint32_t outputAssembly,                    /**< the O-to-T point to be used for this connection */
    uint32_t inputAssembly                      /**< the T-to-O point to be used for this connection */
);

GOAL_STATUS_T goal_eipDmAssemblyAdd(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    GOAL_MI_DM_PART_T **ppPartData,             /**< [out] partition for data */
    uint32_t instanceId,                        /**< instance number of the assembly object */
    uint32_t direction                          /**< DM direction of assembly object (iGOAL_MI_MCTC_DIR_PEER_FROM, GOAL_MI_MCTC_DIR_PEER_TO) */
);

GOAL_STATUS_T goal_eipDmDpAdd(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t idMiDmPeerTo,                      /**< MI DM 'to peer' handle */
    GOAL_MI_DM_PART_T **ppPartDp                /**< [out] partition for data provider status */
);
#endif /* GOAL_CONFIG_MEDIA_MI_DM == 1 */

GOAL_STATUS_T goal_eipIdentitySerialNumberSet(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t serialNum                          /**< serial number */
);

GOAL_STATUS_T goal_eipCfgVendorIdSet(
    uint16_t vendorId                           /**< vendor id */
);

GOAL_STATUS_T goal_eipCfgDeviceTypeSet(
    uint16_t deviceType                         /**< device type */
);

GOAL_STATUS_T goal_eipCfgProductCodeSet(
    uint16_t productCode                        /**< product code */
);

GOAL_STATUS_T goal_eipCfgRevisionSet(
    uint8_t revMajor,                           /**< major revision */
    uint8_t revMinor                            /**< minor revision */
);

GOAL_STATUS_T goal_eipCipIdStatusSet(
    uint16_t idStatus                           /**< product code */
);

GOAL_STATUS_T goal_eipCfgSerialNumSet(
    uint32_t serial                             /**< product code */
);

GOAL_STATUS_T goal_eipCfgProductNameSet(
    const char *strName                         /**< product code */
);

GOAL_STATUS_T goal_eipCfgDomainNameSet(
    const char *strDomainName                   /**< default domain name */
);

GOAL_STATUS_T goal_eipCfgHostNameSet(
    const char *strHostName                     /**< default host name */
);

GOAL_STATUS_T goal_eipCfgNumExplicitConSet(
    uint16_t numConn                            /**< number of Explicit Connections */
);

GOAL_STATUS_T goal_eipCfgNumImplicitConSet(
    uint16_t numConn                            /**< number of Implicit Connections */
);

GOAL_STATUS_T goal_eipCfgEthLinkCountersOn(
    GOAL_BOOL_T enable                          /**< enable flag */
);

GOAL_STATUS_T goal_eipCfgEthLinkControlOn(
    GOAL_BOOL_T enable                          /**< enable flag */
);

GOAL_STATUS_T goal_eipCfgChangeEthAfterResetOn(
    GOAL_BOOL_T enable                          /**< enable flag */
);

GOAL_STATUS_T goal_eipCfgChangeIpAfterResetOn(
    GOAL_BOOL_T enable                          /**< enable flag */
);

GOAL_STATUS_T goal_eipCfgNumSessionsSet(
    uint16_t numSess                            /**< number of encapsulation sessions */
);

GOAL_STATUS_T goal_eipCfgTickSet(
    uint32_t sizeTick                           /**< tick size (multiple of 1ms) */
);

GOAL_STATUS_T goal_eipCfgO2TRunIdleHeaderOn(
    GOAL_BOOL_T enable                          /**< enable flag */
);

GOAL_STATUS_T goal_eipCfgT2ORunIdleHeaderOn(
    GOAL_BOOL_T enable                          /**< enable flag */
);

GOAL_STATUS_T goal_eipCfgQoSOn(
    GOAL_BOOL_T enable                          /**< enable flag */
);

GOAL_STATUS_T goal_eipCfgNumDelayedEncapMsgSet(
    uint16_t numMsg                             /**< number of simultaneously delayable Encapsulation messages */
);

GOAL_STATUS_T goal_eipCfgDhcpOn(
    GOAL_BOOL_T enable                          /**< enable flag */
);

GOAL_STATUS_T goal_eipCfgDlrOn(
    GOAL_BOOL_T enable                          /**< enable flag */
);

GOAL_STATUS_T goal_eipCfgAcdOn(
    GOAL_BOOL_T enable                          /**< enable flag */
);

GOAL_STATUS_T goal_eipCipClassRegister(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/IP handle */
    uint16_t classId,                           /**< class ID to be registered */
    GOAL_EIP_REQ_HANDLER_T pFunc                /**< request handler */
);

GOAL_STATUS_T goal_eipCfgAcdConflictFallbackIp(
    uint32_t ipAddress,                         /**< configured ip address */
    uint32_t subnetMask,                        /**< configured subnetmask */
    uint32_t gateway                            /**< configured gateway */
);

GOAL_STATUS_T goal_eipCfgHwConfigOn(
    GOAL_BOOL_T enable                          /**< enable or disable feature */
);

GOAL_STATUS_T goal_eipAcdLastConflictDetectedReset(
    GOAL_EIP_T *pHdlEip                         /**< GOAL Ethernet/IP handle */
);

GOAL_STATUS_T goal_eipAcdLastConflictDetectedGet(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/IP handle */
    GOAL_ACD_LAST_CONFLICT_DETECTED_T *pLastConflict /**< ACD attribute */
);

GOAL_STATUS_T goal_eipReset(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/IP handle */
    uint32_t resetService                       /**< requested reset service */
);

#endif /* GOAL_EIP_H */
