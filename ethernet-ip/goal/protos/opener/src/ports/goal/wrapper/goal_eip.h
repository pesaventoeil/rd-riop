/** @file
 *
 * @brief Ethernet IP interface of GOAL
 *
 * This module provides a GOAL EtherNet/IP  interface for the OpENer stack.
 *
 * @copyright
 * Copyright 2010-2018 port GmbH Halle/Saale.
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

#ifdef __cplusplus
extern "C" {
#endif

#include "goal_includes.h"

#include "goal_acd.h"

#if GOAL_CONFIG_MCTC == 1
#  include <goal_media/goal_mi_mctc.h>
#endif


/****************************************************************************/
/* Defines */
/****************************************************************************/
#define GOAL_OK_EIP_SEND                (GOAL_RES_EIP) /**< OK EIP send */
#define GOAL_ERR_EIP                    (GOAL_ERROR | GOAL_RES_EIP) /**< general EIP error */

#define GOAL_UNUSED_EIP_HDL (NULL)              /**< define for unused EIP handle */
#define GOAL_EIP_CB_DATA_MAX                10  /**< max count of callback data */

/* device status bits */
#define GOAL_EIP_STATUS_OWNED        (1<<0)     /**< device is owned */
#define GOAL_EIP_STATUS_CFGRD        (1<<2)     /**< device has valid configuration */
#define GOAL_EIP_STATUS_MIN_R_FAULT  (1<<8)     /**< minor recoverable fault */
#define GOAL_EIP_STATUS_MIN_U_FAULT  (1<<9)     /**< minor unrecoverable fault */
#define GOAL_EIP_STATUS_MAJ_R_FAULT  (1<<10)    /**< major recoverable fault */
#define GOAL_EIP_STATUS_MAJ_U_FAULT  (1<<11)    /**< major unrecoverable fault */

#define GOAL_EIP_CONNECTION_EVENT_OPENED     1  /**< connection event: opened */
#define GOAL_EIP_CONNECTION_EVENT_TIMEOUT    2  /**< connection event: timed out */
#define GOAL_EIP_CONNECTION_EVENT_CLOSED     3  /**< connection event: closed */

#define GOAL_EIP_RESET_NO                    1  /**< reset : no reset */
#define GOAL_EIP_RESET_POWER                 2  /**< reset : power cycle */
#define GOAL_EIP_RESET_FACTORY               3  /**< reset : factory reset */
#define GOAL_EIP_RESET_FACTORY_OTHER         4  /**< reset : factory reset */
#define GOAL_EIP_RESET_FACTORY_COMM          5  /**< reset : factory reset communication parameters */

#define GOAL_EIP_MR_ADD_STATUS_SIZE  2          /**< number of words for Additional Status */
#define GOAL_EIP_MR_DATA_SIZE  500              /**< maximum data size of a MR Request/Response */

/* LED bitmap */
#define GOAL_EIP_LED_MS_GREEN      (1u<<0)      /**< Module Status Green is on */
#define GOAL_EIP_LED_MS_RED        (1u<<1)      /**< Module Status Red is on */
#define GOAL_EIP_LED_NS_GREEN      (1u<<2)      /**< Network Status Green is on */
#define GOAL_EIP_LED_NS_RED        (1u<<3)      /**< Network Status Red is on */

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


/****************************************************************************/
/* Typedefs */
/****************************************************************************/
/** GOAL Ethernet/IP handle */
typedef void *GOAL_EIP_T;

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
typedef GOAL_STATUS_T (* GOAL_EIP_FUNC_CB_T) (
    GOAL_EIP_T *,                               /**< EIP handle */
    GOAL_EIP_CB_ID_T,                           /**< callback id */
    GOAL_EIP_CB_DATA_T *                        /**< callback parameters */
);

/** CIP object instance */
typedef void *GOAL_EIP_CIP_INSTANCE_T;

/** CIP Attribute */
typedef struct {
    uint16_t attributeId;                       /**< attribute Id */
    uint8_t cipType;                            /**< CIP data type */
    uint8_t flags;                              /**< access flags */
    uint8_t *pData;                             /**< attribute data */
} GOAL_EIP_CIP_ATTRIBUTE_T;

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

/**< assembly list element */
typedef struct GOAL_EIP_ASSEMBLY_T {
    struct GOAL_EIP_ASSEMBLY_T *pNext;          /**< next pointer */
    uint32_t instanceId;                        /**< assembly instance ID */
    uint16_t len;                               /**< assembly data length */
#if GOAL_CONFIG_MEDIA_MI_DM == 1
    GOAL_MI_DM_PART_T dmData;                   /**< assembly data */
    uint16_t cntCon;                            /**< number of connections using this assembly */
#endif /* GOAL_CONFIG_MEDIA_MI_DM == 1 */
} GOAL_EIP_ASSEMBLY_T;

/**< GOAL EIP wrapper instance data */
typedef struct GOAL_EIP_WRAPPER_T {
    GOAL_EIP_FUNC_CB_T goal_eipCb;              /**< application callback */
    GOAL_EIP_CIP_CLASS_T *pListCipClass;        /**< registered CIP classes */
    GOAL_EIP_ASSEMBLY_T *pListAssembly;         /**< assembly object list */
    GOAL_LOCK_T *pLockAssembly;                 /**< Assembly read/ write lock */
    GOAL_EIP_MR_REQ_T mrReq;                    /**< Message Router Request */
    GOAL_EIP_MR_RES_T mrRes;                    /**< Message Router Response */
} GOAL_EIP_WRAPPER_T;


/****************************************************************************/
/* Function and Parameter Mapping */
/****************************************************************************/
#define GOAL_EIP_FUNC_CALLBACK 0                /**< callback function id */

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


/****************************************************************************/
/* Public Functions */
/****************************************************************************/
GOAL_STATUS_T goal_eipInitImpl(
    void
);

GOAL_STATUS_T goal_eipNewIfaceAssignImpl(
    GOAL_EIP_T **ppEip,                         /**< EtherNet/IP instance ref */
    const uint32_t id,                          /**< instance id */
    GOAL_EIP_FUNC_CB_T pFunc,                   /**< EtherNet/IP callback function */
    uint32_t ethMiId,                           /**< ID of assigned ETH MI */
    uint32_t netMiId                            /**< ID of assigned NET MI */
);

GOAL_STATUS_T goal_eipCipClassRegisterImpl(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/IP handle */
    uint16_t classId,                           /**< class ID to be registered */
    GOAL_EIP_REQ_HANDLER_T pFunc                /**< request handler */
);

GOAL_STATUS_T goal_eipCreateAssemblyObjectImpl(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/IP handle */
    uint32_t instanceId,                        /**< instance number of the assembly object to create */
    uint16_t len                                /**< length of the assembly object's data */
);

GOAL_STATUS_T goal_eipAssemblyObjectGetImpl(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t instanceId,                        /**< instance number of the assembly object to create */
    uint8_t **ppData,                           /**< data pointer of assembly */
    uint16_t *pLen                              /**< length of assembly data */
);

GOAL_STATUS_T goal_eipAssemblyObjectWriteImpl(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t instanceId,                        /**< instance number of the assembly object to create */
    uint8_t *pData,                             /**< data pointer to write */
    uint16_t len                                /**< length of data to write */
);

GOAL_STATUS_T goal_eipAssemblyObjectReadImpl(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t instanceId,                        /**< instance number of the assembly object to create */
    uint8_t *pData,                             /**< data pointer to store read data */
    uint16_t len                                /**< length of data to read */
);

GOAL_STATUS_T goal_eipAssemblyInstanceGetImpl(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t instanceId,                        /**< instance number of the assembly object to create */
    GOAL_EIP_ASSEMBLY_T **ppAssembly            /**< return pointer of assembly */
);

GOAL_STATUS_T goal_eipAddExclusiveOwnerConnectionImpl(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t outputAssembly,                    /**< the O-to-T point to be used for this connection */
    uint32_t inputAssembly,                     /**< the T-to-O point to be used for this connection */
    uint32_t configAssembly                     /**< the configuration point to be used for this connection */
);

GOAL_STATUS_T goal_eipAddInputOnlyConnectionImpl(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t connNum,                           /**< the number of the input only connection. */
    uint32_t outputAssembly,                    /**< the O-to-T point to be used for this connection */
    uint32_t inputAssembly,                     /**< the T-to-O point to be used for this connection */
    uint32_t configAssembly                     /**< the configuration point to be used for this connection */
);

GOAL_STATUS_T goal_eipAddListenOnlyConnectionImpl(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t connNum,                           /**< the number of the input only connection. */
    uint32_t outputAssembly,                    /**< the O-to-T point to be used for this connection */
    uint32_t inputAssembly,                     /**< the T-to-O point to be used for this connection */
    uint32_t configAssembly                     /**< the configuration point to be used for this connection */
);

GOAL_STATUS_T goal_eipGetVersionImpl(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    const char **ppVersion                      /**< Ethernet/IP version */
);

void goal_eipDeviceStatusSetImpl(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint16_t statusBits                         /**< status bitmap */
);

void goal_eipDeviceStatusClearImpl(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint16_t statusBits                         /**< status bitmap */
);

GOAL_STATUS_T goal_eipIdentitySerialNumberSetImpl(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    uint32_t serialNum                          /**< serial number */
);

GOAL_STATUS_T goal_eipAcdLastConflictDetectedResetImpl(
    GOAL_EIP_T *pHdlEip                         /**< GOAL Ethernet/ IP handle */
);

GOAL_STATUS_T goal_eipAcdLastConflictDetectedGetImpl(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    GOAL_ACD_LAST_CONFLICT_DETECTED_T *pAttribute /**< ACD attribute */
);

GOAL_STATUS_T goal_eipResetImpl(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/IP handle */
    uint32_t resetService                       /**< requested reset service */
);

GOAL_STATUS_T goal_eipCfgVendorIdSetImpl(
    uint16_t vendorId                           /**< vendor id */
);

GOAL_STATUS_T goal_eipCfgDeviceTypeSetImpl(
    uint16_t deviceType                         /**< device type */
);

GOAL_STATUS_T goal_eipCfgProductCodeSetImpl(
    uint16_t productCode                        /**< product code */
);

GOAL_STATUS_T goal_eipCfgRevisionSetImpl(
    uint8_t revMajor,                           /**< major revision */
    uint8_t revMinor                            /**< minor revision */
);

GOAL_STATUS_T goal_eipCfgSerialNumSetImpl(
    uint32_t serial                             /**< serial number */
);

GOAL_STATUS_T goal_eipCfgProductNameSetImpl(
    const char *strName                         /**< product name */
);

GOAL_STATUS_T goal_eipCfgDomainNameSetImpl(
    const char *strName                         /**< domain name */
);

GOAL_STATUS_T goal_eipCfgHostNameSetImpl(
    const char *strName                         /**< host name */
);

GOAL_STATUS_T goal_eipCfgNumExplicitConSetImpl(
    uint16_t num                                /**< number of explicit connections */
);

GOAL_STATUS_T goal_eipCfgNumImplicitConSetImpl(
    uint16_t num                                /**< number of implicit connections */
);

GOAL_STATUS_T goal_eipCfgEthLinkCountersOnImpl(
    GOAL_BOOL_T enable                          /**< enable or disable feature */
);

GOAL_STATUS_T goal_eipCfgEthLinkControlOnImpl(
    GOAL_BOOL_T enable                          /**< enable or disable feature */
);

GOAL_STATUS_T goal_eipCfgChangeEthAfterResetOnImpl(
    GOAL_BOOL_T enable                          /**< enable or disable feature */
);

GOAL_STATUS_T goal_eipCfgChangeIpAfterResetOnImpl(
    GOAL_BOOL_T enable                          /**< enable or disable feature */
);

GOAL_STATUS_T goal_eipCfgNumSessionsSetImpl(
    uint16_t num                                /**< number of sessions */
);

GOAL_STATUS_T goal_eipCfgTickSetImpl(
    uint32_t ticks                              /**< size of 1 tick */
);

GOAL_STATUS_T goal_eipCfgO2TRunIdleHeaderOnImpl(
    GOAL_BOOL_T enable                          /**< enable or disable feature */
);

GOAL_STATUS_T goal_eipCfgT2ORunIdleHeaderOnImpl(
    GOAL_BOOL_T enable                          /**< enable or disable feature */
);

GOAL_STATUS_T goal_eipCfgQoSOnImpl(
    GOAL_BOOL_T enable                          /**< enable or disable feature */
);

GOAL_STATUS_T goal_eipCfgNumDelayedEncapMsgSetImpl(
    uint16_t num                                /**< size of delay buffer */
);

GOAL_STATUS_T goal_eipCfgDhcpOnImpl(
    GOAL_BOOL_T enable                          /**< enable or disable feature */
);

GOAL_STATUS_T goal_eipCfgAcdOnImpl(
    GOAL_BOOL_T enable                          /**< enable or disable feature */
);

GOAL_STATUS_T goal_eipCfgAcdConflictFallbackIpImpl(
    uint32_t ipAddress,                         /**< configured ip address */
    uint32_t subnetMask,                        /**< configured subnetmask */
    uint32_t gateway                            /**< configured gateway */
);

GOAL_STATUS_T goal_eipCfgNumLldpDataTableEntriesSetImpl(
    uint32_t numEntries                         /**< number of LLDP Data Table entries */
);

GOAL_STATUS_T goal_eipCfgDlrOnImpl(
    GOAL_BOOL_T enable                          /**< enable or disable feature */
);

GOAL_STATUS_T goal_eipCfgHwConfigOnImpl(
    GOAL_BOOL_T enable                          /**< enable or disable feature */
);

GOAL_STATUS_T IApp_InitImpl(
    GOAL_EIP_T *pHdlEip                         /**< GOAL Ethernet/ IP handle */
);

GOAL_STATUS_T IApp_AcdConflict(
    GOAL_EIP_T *pHdlEip                         /**< EtherNet/IP instance */
);

#if GOAL_CONFIG_GEN_CTC_CC == 1
# define goal_eipInit goal_eipInitCc

GOAL_STATUS_T goal_eipInitCc(
    void
);
#endif

#if GOAL_CONFIG_MEDIA_MI_DM == 1
GOAL_STATUS_T goal_eipDmAssemblyAddImpl(
    GOAL_EIP_T *pHdlEip,                        /**< GOAL Ethernet/ IP handle */
    GOAL_MI_DM_PART_T **ppPartData,             /**< [out] partition for data */
    uint32_t instanceId,                        /**< instance number of the assembly object */
    uint32_t direction                          /**< DM direction of assembly object (iGOAL_MI_MCTC_DIR_PEER_FROM, GOAL_MI_MCTC_DIR_PEER_TO) */
);

GOAL_STATUS_T goal_eipDmDpAddImpl(
    GOAL_EIP_T *pEip,                           /**< EIP instance */
    uint32_t idMiDmPeerTo,                      /**< MI DM 'to peer' handle */
    GOAL_MI_DM_PART_T **ppPartDp                /**< [out] partition for data provider status */
);

GOAL_STATUS_T goal_eipDmDataRecv(
    GOAL_EIP_T *pHdlEip,                        /**< EIP goal instance */
    uint32_t instanceId,                        /**< Assembly instance ID */
    uint8_t *pData,                             /**< received data */
    uint16_t len                                /**< length of data */
);

GOAL_STATUS_T goal_eipDmDataSend(
    GOAL_EIP_T *pHdlEip,                        /**< EIP goal instance */
    uint32_t instanceId,                        /**< Assembly instance ID */
    uint8_t *pData,                             /**< data to send */
    uint16_t len                                /**< length of data */
);

void goal_eipDmCycleStart(
    GOAL_EIP_T *pHdlEip                         /**< EIP goal instance */
);

void goal_eipDmCycleEnd(
    GOAL_EIP_T *pHdlEip                         /**< EIP goal instance */
);

void goal_eipDmDpSet(
    GOAL_EIP_T *pHdlEip,                        /**< EIP goal instance */
    uint32_t connectionEvent                    /**< connection event */
);
#endif /* GOAL_CONFIG_MEDIA_MI_DM == 1 */


/****************************************************************************/
/* Helper Functions */
/****************************************************************************/


/****************************************************************************/
/** Create new EtherNet/IP instance for the Default interface
 *
 * Create a GOAL EtherNet/IP instance for the given ID and register a callback.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static inline GOAL_STATUS_T goal_eipNew(
    GOAL_EIP_T **ppEip,                         /**< EtherNet/IP instance ref */
    const uint32_t id,                          /**< instance id */
    GOAL_EIP_FUNC_CB_T pFunc                    /**< EtherNet/IP callback function */
)
{
    return goal_eipNewIfaceAssignImpl(ppEip, id, pFunc, GOAL_ID_DEFAULT, GOAL_ID_DEFAULT);
}


/****************************************************************************/
/* Mapping of functions which can be overload */
/****************************************************************************/
#ifndef goal_eipInit
# define goal_eipInit goal_eipInitImpl
#endif

#ifndef goal_eipNewIfaceAssign
# define goal_eipNewIfaceAssign goal_eipNewIfaceAssignImpl
#endif

#ifndef goal_eipCipClassRegister
# define goal_eipCipClassRegister goal_eipCipClassRegisterImpl
#endif

#ifndef goal_eipCreateAssemblyObject
# define goal_eipCreateAssemblyObject goal_eipCreateAssemblyObjectImpl
#endif

#ifndef goal_eipCreateAssemblyObjectDm
# define goal_eipCreateAssemblyObjectDm goal_eipCreateAssemblyObjectImpl
#endif

#ifndef goal_eipCreateAssemblyObjectRpc
# define goal_eipCreateAssemblyObjectRpc goal_eipCreateAssemblyObjectImpl
#endif

#ifndef goal_eipAssemblyObjectGet
# define goal_eipAssemblyObjectGet goal_eipAssemblyObjectGetImpl
#endif

#ifndef goal_eipAssemblyObjectWrite
# define goal_eipAssemblyObjectWrite goal_eipAssemblyObjectWriteImpl
#endif

#ifndef goal_eipAssemblyObjectRead
# define goal_eipAssemblyObjectRead goal_eipAssemblyObjectReadImpl
#endif

#ifndef goal_eipAssemblyInstanceGet
# define goal_eipAssemblyInstanceGet goal_eipAssemblyInstanceGetImpl
#endif

#ifndef goal_eipAddExclusiveOwnerConnection
# define goal_eipAddExclusiveOwnerConnection goal_eipAddExclusiveOwnerConnectionImpl
#endif

#ifndef goal_eipAddInputOnlyConnection
# define goal_eipAddInputOnlyConnection goal_eipAddInputOnlyConnectionImpl
#endif

#ifndef goal_eipAddListenOnlyConnection
# define goal_eipAddListenOnlyConnection goal_eipAddListenOnlyConnectionImpl
#endif

#ifndef goal_eipGetVersion
# define goal_eipGetVersion goal_eipGetVersionImpl
#endif

#ifndef goal_eipDeviceStatusSet
# define goal_eipDeviceStatusSet goal_eipDeviceStatusSetImpl
#endif

#ifndef goal_eipDeviceStatusClear
# define goal_eipDeviceStatusClear goal_eipDeviceStatusClearImpl
#endif

#ifndef goal_eipIdentitySerialNumberSet
# define goal_eipIdentitySerialNumberSet goal_eipIdentitySerialNumberSetImpl
#endif

#ifndef goal_eipAcdLastConflictDetectedReset
# define goal_eipAcdLastConflictDetectedReset goal_eipAcdLastConflictDetectedResetImpl
#endif

#ifndef goal_eipAcdLastConflictDetectedGet
# define goal_eipAcdLastConflictDetectedGet goal_eipAcdLastConflictDetectedGetImpl
#endif

#ifndef goal_eipReset
# define goal_eipReset goal_eipResetImpl
#endif

#ifndef goal_eipCfgVendorIdSet
# define goal_eipCfgVendorIdSet goal_eipCfgVendorIdSetImpl
#endif

#ifndef goal_eipCfgDeviceTypeSet
# define goal_eipCfgDeviceTypeSet goal_eipCfgDeviceTypeSetImpl
#endif

#ifndef goal_eipCfgProductCodeSet
# define goal_eipCfgProductCodeSet goal_eipCfgProductCodeSetImpl
#endif

#ifndef goal_eipCfgRevisionSet
# define goal_eipCfgRevisionSet goal_eipCfgRevisionSetImpl
#endif

#ifndef goal_eipCfgSerialNumSet
# define goal_eipCfgSerialNumSet goal_eipCfgSerialNumSetImpl
#endif

#ifndef goal_eipCfgProductNameSet
# define goal_eipCfgProductNameSet goal_eipCfgProductNameSetImpl
#endif

#ifndef goal_eipCfgDomainNameSet
# define goal_eipCfgDomainNameSet goal_eipCfgDomainNameSetImpl
#endif

#ifndef goal_eipCfgHostNameSet
# define goal_eipCfgHostNameSet goal_eipCfgHostNameSetImpl
#endif

#ifndef goal_eipCfgNumExplicitConSet
# define goal_eipCfgNumExplicitConSet goal_eipCfgNumExplicitConSetImpl
#endif

#ifndef goal_eipCfgNumititConSet
# define goal_eipCfgNumititConSet goal_eipCfgNumImplicitConSetImpl
#endif

#ifndef goal_eipCfgEthLinkCountersOn
# define goal_eipCfgEthLinkCountersOn goal_eipCfgEthLinkCountersOnImpl
#endif

#ifndef goal_eipCfgEthLinkControlOn
# define goal_eipCfgEthLinkControlOn goal_eipCfgEthLinkControlOnImpl
#endif

#ifndef goal_eipCfgChangeEthAfterResetOn
# define goal_eipCfgChangeEthAfterResetOn goal_eipCfgChangeEthAfterResetOnImpl
#endif

#ifndef goal_eipCfgChangeIpAfterResetOn
# define goal_eipCfgChangeIpAfterResetOn goal_eipCfgChangeIpAfterResetOnImpl
#endif

#ifndef goal_eipCfgNumSessionsSet
# define goal_eipCfgNumSessionsSet goal_eipCfgNumSessionsSetImpl
#endif

#ifndef goal_eipCfgTickSet
# define goal_eipCfgTickSet goal_eipCfgTickSetImpl
#endif

#ifndef goal_eipCfgO2TRunIdleHeaderOn
# define goal_eipCfgO2TRunIdleHeaderOn goal_eipCfgO2TRunIdleHeaderOnImpl
#endif

#ifndef goal_eipCfgT2ORunIdleHeaderOn
# define goal_eipCfgT2ORunIdleHeaderOn goal_eipCfgT2ORunIdleHeaderOnImpl
#endif

#ifndef goal_eipCfgQoSOn
# define goal_eipCfgQoSOn goal_eipCfgQoSOnImpl
#endif

#ifndef goal_eipCfgNumDelayedEncapMsgSet
# define goal_eipCfgNumDelayedEncapMsgSet goal_eipCfgNumDelayedEncapMsgSetImpl
#endif

#ifndef goal_eipCfgDhcpOn
# define goal_eipCfgDhcpOn goal_eipCfgDhcpOnImpl
#endif

#ifndef goal_eipCfgDlrOn
# define goal_eipCfgDlrOn goal_eipCfgDlrOnImpl
#endif

#ifndef goal_eipCfgAcdOn
# define goal_eipCfgAcdOn goal_eipCfgAcdOnImpl
#endif

#ifndef goal_eipCfgAcdConflictFallbackIp
# define goal_eipCfgAcdConflictFallbackIp goal_eipCfgAcdConflictFallbackIpImpl
#endif

#ifndef goal_eipCfgNumLldpDataTableEntriesSet
# define goal_eipCfgNumLldpDataTableEntriesSet goal_eipCfgNumLldpDataTableEntriesSetImpl
#endif

#ifndef goal_eipCfgHwConfigOn
# define goal_eipCfgHwConfigOn goal_eipCfgHwConfigOnImpl
#endif

#ifndef goal_eipDmDpAdd
# define goal_eipDmDpAdd goal_eipDmDpAddImpl
#endif


/* C++ compatibility */
#ifdef __cplusplus
}
#endif


#endif /* GOAL_EIP_H */
