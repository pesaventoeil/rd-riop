/** @file
 *
 * @brief OpENer Data Types
 *
 * This module defines data types used by OpENer
 *
 * @copyright
 * This software consists of the OpENer by Rockwell Automation, Inc. and
 * extensive fixes, changes and improvements by port GmbH.
 *
 * Copyright (c) 2019, port GmbH for all fixes, changes and improvements in
 * this software, added to the original, unchanged OpENer
 *
 * Copyright (c) 2009, Rockwell Automation, Inc. for the original, unchanged
 * OpENer source code
 *
 * All rights reserved.
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

#ifndef EIP_TYPES_H
#define EIP_TYPES_H


#include "goal_acd.h"
#include "goal_lldp.h"
#if GOAL_DLR == 1
#include "goal_dlr.h"
#endif /* GOAL_DLR */


/****************************************************************************/
/* Public defines */
/****************************************************************************/
#define SENDER_CONTEXT_SIZE 8                   /* size of sender context in encapsulation header */

/* string buffer sizes */
#define EIP_CFG_LEN_PROD_NAME   33              /**< size of product name buffer */
#define EIP_CFG_LEN_DOMAIN_NAME 49              /**< size of domain name buffer */
#define EIP_CFG_LEN_HOST_NAME   65              /**< size of domain name buffer */
#define EIP_LLDP_IFACE_NAME_BUF 65              /**< size of interface lable buffer */


/****************************************************************************/
/* Forward Declarations */
/****************************************************************************/
struct EIP_INSTANCE_T;
struct CIP_ConnectionObject;
struct GOAL_EIP_WRAPPER_T;


/****************************************************************************/
/* Public data types */
/****************************************************************************/
/* basic data types */
#define EIP_BYTE    uint8_t
#define EIP_INT8    int8_t
#define EIP_INT16   int16_t
#define EIP_INT32   int32_t
#define EIP_UINT8   uint8_t
#define EIP_UINT16  uint16_t
#define EIP_UINT32  uint32_t
#define EIP_REAL    float
#define EIP_LREAL   double
#define EIP_BOOL8   bool
#define EIP_INT64   int64_t
#define EIP_UINT64  uint64_t

/** Status type */
typedef enum
{
  EIP_OK = 0,
  EIP_OK_SEND = 1,
  EIP_ERROR = -1,
  EIP_OK_RESEND = -2,
} EIP_STATUS;

/**< device reset state */
typedef enum
{
    enNoReset = 0,
    enReset = 1,
    enResetAll = 2,
    enResetOther = 3,
    enResetIpCfg = 4,
} EResetState;

#if !defined(__cplusplus) && !defined(bool)
typedef enum {
  false = 0,
  true = 1
} bool;
#endif

/* array indices */
#define CONSUMING 0                             /**< data for producing instance */
#define PRODUCING 1                             /**< data for consuming instance */
#define CONFIG    2                             /**< data for configuration instance */

/* definition of CIP basic data types */
#define CIP_ANY             0x00  /*data type that can not be directly encoded */
#define CIP_BOOL            0xC1
#define CIP_SINT            0xC2
#define CIP_INT             0xC3
#define CIP_DINT            0xC4
#define CIP_LINT            0xC5
#define CIP_USINT           0xC6
#define CIP_UINT            0xC7
#define CIP_UDINT           0xC8
#define CIP_ULINT           0xC9
#define CIP_REAL            0xCA
#define CIP_LREAL           0xCB
#define CIP_STIME           0xCC
#define CIP_DATE            0xCD
#define CIP_TIME_OF_DAY     0xCE
#define CIP_DATE_AND_TIME   0xCF
#define CIP_STRING          0xD0
#define CIP_BYTE            0xD1
#define CIP_WORD            0xD2
#define CIP_DWORD           0xD3
#define CIP_LWORD           0xD4
#define CIP_STRING2         0xD5
#define CIP_FTIME           0xD6
#define CIP_LTIME           0xD7
#define CIP_ITIME           0xD8
#define CIP_STRINGN         0xD9
#define CIP_SHORT_STRING    0xDA
#define CIP_TIME            0xDB
#define CIP_EPATH           0xDC
#define CIP_ENGUNIT         0xDD

/* definition of some CIP structs */
/* need to be validated in IEC 1131-3 subclause 2.3.3 */
#define CIP_USINT_USINT         0xA0
#define CIP_5UDINT_STRING       0xA1
#define CIP_6USINT              0xA2            /* for MAC Address */
#define CIP_MEMBER_LIST         0xA3
#define CIP_BYTE_ARRAY          0xA4
#define CIP_BIT_ARRAY           0xA5

#define INTERNAL_UINT16_6       0xf0                /* bogus hack, for port class attribute 9, TODO figure out the right way to handle it */
#define CIP_UDINT_6USINT        0xf1
#define CIP_11UDINT             0xf2
#define CIP_12UDINT             0xf3
#define CIP_WORD_UINT           0xf4
#define CIP_ETH_ATTR_11         0xf5
#define CIP_TCPIP_ATTR9         0xf6
#define CIP_8ULINT              0xf7
#define CIP_6ULINT              0xf8
#define CIP_TCPIP_ATTR11        0xf9

/* definition of CIP service codes */
#define CIP_GET_ATTRIBUTE_SINGLE    0x0E
#define CIP_SET_ATTRIBUTE_SINGLE    0x10
#define CIP_RESET                   0x05
#define CIP_CREATE                  0x08
#define CIP_GET_ATTRIBUTE_ALL       0x01
#define CIP_FORWARD_OPEN            0x54
#define CIP_FORWARD_CLOSE           0x4E
#define CIP_UNCONNECTED_SEND        0x52
#define CIP_GET_CONNECTION_OWNER    0x5A
#define CIP_LARGE_FORWARD_OPEN      0x5B
#define CIP_GET_AND_CLEAR           0x4C
#define CIP_FIND_NEXT_OBJECT_INSTANCE 0x11

/* definition of Flags for CIP Attributes */
#define CIP_ATTRIB_GETABLEALL              0x01 /**< attribute is part of GAA response */
#define CIP_ATTRIB_GETABLESINGLE           0x02 /**< attribute is part of GAS response */
#define CIP_ATTRIB_GETABLE                 0x03 /**< attribute is part of both GAA & GAS response */
#define CIP_ATTRIB_SETABLE                 0x04 /**< attribute can be set */
#define CIP_ATTRIB_SETGETABLE              0x07 /**< attribute is setable and getable */
#define CIP_ATTRIB_REJECTABLE              0x08 /**< attribute access can be rejected by callback */
#define CIP_ATTRIB_GETALLPADDING           0x10 /**< virtual attribute to pad GAA response */

/** common CIP class attribute Ids */
#define CIP_CLASS_ATTR_REVISION        1        /**< revision */
#define CIP_CLASS_ATTR_MAXINSTANCE     2        /**< maximum instance created */
#define CIP_CLASS_ATTR_INSTANCES       3        /**< number of instances */
#define CIP_CLASS_ATTR_OPTATTR         4        /**< optional attribute list */
#define CIP_CLASS_ATTR_OPTSERVICE      5        /**< optional service list */
#define CIP_CLASS_ATTR_MAXCLASSATTR    6        /**< maximum class attribute */
#define CIP_CLASS_ATTR_MAXINSTANCEATTR 7        /**< maximum instance attribute */

typedef enum
{
  enOpened, enTimedOut, enClosed
} EIOConnectionEvent;

typedef struct
{
  EIP_UINT32 ipAddr;
  EIP_UINT16 port;
} S_CIP_Net_Addr;

typedef struct
{
  EIP_UINT16 len;
  EIP_BYTE *Data;
} S_CIP_Byte_Array;

/** CIP Bit array */
typedef struct {
    EIP_UINT16 len;                             /**< number of valid bits */
    EIP_UINT8 *pData;                           /**< byte array with valid bits */
} EIP_BIT_ARRAY_T;

typedef struct
{
  EIP_UINT8 Length;
  EIP_BYTE *String;
} S_CIP_Short_String;

typedef struct
{
  EIP_UINT16 Length;
  EIP_BYTE *String;
} S_CIP_String;

typedef struct
{
  EIP_UINT8 PathSize;
  EIP_UINT16 ClassID;
  EIP_UINT16 InstanceNr;
  EIP_UINT16 MemberID;
  EIP_UINT16 AttributNr;
} S_CIP_EPATH;

typedef struct
{
  EIP_UINT32 ClassID;
  EIP_UINT32 ConnectionPoint[3];
} S_CIP_ConnectionPath;

typedef struct
{
  EIP_UINT16 VendorID;
  EIP_UINT16 DeviceType;
  EIP_UINT16 ProductCode;
  EIP_BYTE MajorRevision;
  EIP_UINT8 MinorRevision;
} S_CIP_KeyData;

typedef struct
{
  EIP_UINT8 MajorRevision;
  EIP_UINT8 MinorRevision;
} S_CIP_Revision;

typedef struct
{
  EIP_UINT8 Service;
  S_CIP_EPATH RequestPath;
  EIP_INT16 DataLength;
  EIP_UINT32 originatorIp;
  EIP_SOCKT_T rxSock;
  EIP_UINT8 *Data;
} S_CIP_MR_Request;

#define MAX_SIZE_OF_ADD_STATUS 2 /* for now we support extended status codes up to 2 16bit values
                                    there is mostly only one 16bit value used */
typedef struct
{
  EIP_UINT8 ReplyService;
  EIP_UINT8 GeneralStatus;
  EIP_UINT8 SizeofAdditionalStatus;
  EIP_UINT16 AdditionalStatus[MAX_SIZE_OF_ADD_STATUS];
  EIP_INT16 DataLength;
  EIP_UINT8 *Data;
} S_CIP_MR_Response;

/** CIP attribute */
typedef struct EIP_CIP_ATTR_T {
    struct EIP_CIP_ATTR_T *pNext;               /**< next attribute */
    EIP_UINT16 attributeId;                     /**< attribute Id */
    EIP_UINT8 dataType;                         /**< data type */
    EIP_UINT8 flags;                            /**< internal flags */
    EIP_INT16 minSize;                          /**< minimum attribute size */
    EIP_INT16 maxSize;                          /**< maximum attribute size */
    void *pData;                                /**< attribute data */
} EIP_CIP_ATTR_T;

/** CIP instance */
typedef struct EIP_CIP_INSTANCE_T {
    struct EIP_CIP_INSTANCE_T *pNext;           /**< next instance */
    struct EIP_CIP_CLASS_T *pClass;             /**< class of instance */
    EIP_UINT32 instanceId;                      /**< instance Id */
    EIP_CIP_ATTR_T *pAttributes;                /**< attributes of instance */
} EIP_CIP_INSTANCE_T;

/** CIP instance service handler */
typedef EIP_STATUS (*EIP_CIP_SERVICE_FUNC_T)(
    struct EIP_INSTANCE_T *pEip,                /**< EtherNet/IP instance */
    EIP_CIP_INSTANCE_T *pInstance               /**< instance data */
);

/** CIP instance service */
typedef struct EIP_CIP_SERVICE_T {
    struct EIP_CIP_SERVICE_T *pNext;            /**< next service */
    EIP_UINT8 serviceId;                        /**< service Id */
    EIP_CIP_SERVICE_FUNC_T funcService;         /**< service handler */
} EIP_CIP_SERVICE_T;

/** CIP class service handler */
typedef EIP_STATUS (*EIP_CIP_CLASS_SERVICE_FUNC_T)(
    struct EIP_INSTANCE_T *pEip,                /**< EtherNet/IP instance */
    struct EIP_CIP_CLASS_T *pClass              /**< class data */
);

/** CIP class service */
typedef struct EIP_CIP_CLASS_SERVICE_T {
    struct EIP_CIP_CLASS_SERVICE_T *pNext;      /**< next service */
    EIP_UINT8 serviceId;                        /**< service Id */
    EIP_CIP_CLASS_SERVICE_FUNC_T funcService;   /**< service handler */
} EIP_CIP_CLASS_SERVICE_T;

/** attribute update callback for GetAttribute services */
typedef void (* EIP_CB_ATTR_UPDATE_T)(
    struct EIP_INSTANCE_T *pEip,                /**< EtherNet/IP instance */
    EIP_UINT16 instanceId,                      /**< instance ID */
    EIP_UINT16 attributeId,                     /**< attribute ID */
    void *pData                                 /**< attribute data */
);

/** attribute test and set callback for SetAttribute services */
typedef EIP_UINT8 (* EIP_CB_ATTR_TEST_SET_T)(
    struct EIP_INSTANCE_T *pEip,                /**< EtherNet/IP instance */
    EIP_UINT16 instanceId,                      /**< instance ID */
    EIP_UINT16 attributeId,                     /**< attribute ID */
    void *pDataNew,                             /**< new attribute data */
    void *pAttrData                             /**< attribute data buffer */
);

/** CIP class */
typedef struct EIP_CIP_CLASS_T {
    struct EIP_CIP_CLASS_T *pNext;              /**< next class */
    EIP_UINT32 classId;                         /**< class ID */
    EIP_UINT16 revision;                        /**< class revision */
    EIP_UINT32 maxInstance;                     /**< maximum instance Id */
    EIP_UINT16 noInstances;                     /**< number of instances */
    EIP_UINT16 maxClassAttr;                    /**< maximum class attribute Id */
    EIP_UINT16 maxInstAttr;                     /**< maximum instance attribute Id */
    EIP_CIP_INSTANCE_T *pInstances;             /**< list of instances */
    EIP_CIP_ATTR_T *pClassAttr;                 /**< additional CIP class attributes */
    EIP_CIP_CLASS_SERVICE_T *pClassServices;    /**< CIP class services */
    EIP_CIP_SERVICE_T *pServices;               /**< CIP services */
    EIP_CB_ATTR_UPDATE_T pCbInstAttrUpdate;     /**< instance attribute update callback */
    EIP_CB_ATTR_UPDATE_T pCbClassAttrUpdate;    /**< class attribute update callback */
    EIP_CB_ATTR_TEST_SET_T pCbInstAttrTestSet;  /**< instance attribute test and set callback */
} EIP_CIP_CLASS_T;

typedef EIP_UINT8 (*TConnOpenFunc)(struct EIP_INSTANCE_T *pEip, struct CIP_ConnectionObject *pa_pstConnObj, EIP_UINT16 *pa_pnExtendedError);
typedef void (*TConnCloseFunc)(struct EIP_INSTANCE_T *pEip, struct CIP_ConnectionObject *pa_pstConnObj);
typedef void (*TConnTimeOutFunc)(struct EIP_INSTANCE_T *pEip, struct CIP_ConnectionObject *pa_pstConnObj);
typedef EIP_STATUS (*TConnSendDataFunc)(struct EIP_INSTANCE_T *pEip, struct CIP_ConnectionObject *pa_pstConnection);
typedef EIP_STATUS (*TConnRecvDataFunc)(struct EIP_INSTANCE_T *pEip, struct CIP_ConnectionObject *pa_pstConnection, EIP_UINT8 * pa_pnData, EIP_UINT16 pa_nDataLength);




typedef struct
{
  EIP_UINT32 IPAddress;
  EIP_UINT32 NetworkMask;
  EIP_UINT32 Gateway;
  EIP_UINT32 NameServer;
  EIP_UINT32 NameServer2;
  struct {
    EIP_INT16 Length;
    EIP_BYTE String[48];
  } DomainName ;
} S_CIP_TCPIPNetworkInterfaceConfiguration;

typedef struct S_CIP_IdentityAtributes
{
  EIP_UINT16 VendorID;
  EIP_UINT16 DeviceType;
  EIP_UINT16 ProductCode;
  S_CIP_Revision Revison;
  EIP_UINT16 ID_Status;
  EIP_UINT32 SerialNumber;
  S_CIP_Short_String ProductName;
} S_CIP_IdentityAtributes;

/** Virtual class request handler */
typedef EIP_STATUS (* TCIPVirtClassHandlerFunc)(
    struct EIP_INSTANCE_T *pEip,                /**< EtherNet/IP instance */
    S_CIP_MR_Request *pa_MRRequest,             /**< request data */
    S_CIP_MR_Response *pa_MRResponse            /**< response data */
);

/** Virtual Class */
typedef struct S_CIP_VirtualClass {
    EIP_UINT16 classId;                         /**< class ID */
    TCIPVirtClassHandlerFunc pFunc;             /**< request handler */
    struct S_CIP_VirtualClass *pNext;           /**< next element */
} S_CIP_VirtualClass;

/** Encapsulation Header */
typedef struct
{
    EIP_UINT16 nCommand_code;
    EIP_UINT16 nData_length;
    EIP_UINT32 nSession_handle;
    EIP_UINT32 nStatus;
    EIP_UINT8 anSender_context[SENDER_CONTEXT_SIZE];
    EIP_UINT32 nOptions;
    EIP_UINT8 *m_acCommBufferStart;       /* start of rx buffer */
    EIP_UINT8 *m_acCurrentCommBufferPos;  /* unprocessed data in rx buffer */
    GOAL_BUFFER_T *pTxBuf;                /* tx buffer */
    EIP_UINT8 *m_acTxBufPos;              /* current position of tx buffer */
} S_Encap_Header;

/** Address Item Data */
typedef struct
{
  EIP_UINT32 ConnectionIdentifier;
  EIP_UINT32 SequenceNumber;
} S_Address_Data;

/** Address Item */
typedef struct
{
  EIP_UINT16 TypeID;
  EIP_UINT16 Length;
  S_Address_Data Data;
} S_Address_Item;

/** Data Item */
typedef struct
{
  EIP_UINT16 TypeID;
  EIP_UINT16 Length;
  EIP_UINT8 *Data;
} S_Data_Item;

/** SocketAddressInfo Item */
typedef struct
{
  EIP_UINT16 TypeID;
  EIP_UINT16 Length;
  EIP_INT16 nsin_family;
  EIP_UINT16 nsin_port;
  EIP_UINT32 nsin_addr;
} S_SockAddrInfo_Item;

/** encapsulation data */
typedef struct
{
  EIP_UINT32 originatorIp;
  EIP_SOCKT_T rxSock;
  EIP_BOOL8 isClass3;
  S_Encap_Header header;
  EIP_UINT16 ItemCount;
  S_Address_Item stAddr_Item;
  S_Data_Item stDataI_Item;
  S_SockAddrInfo_Item AddrInfo[2];
  uint8_t *pBufData;                            /**< buffer for large data */
  uint16_t len;                                 /**< used bytes of buffer */
  uint32_t dropLen;                             /**< bytes to be dropped */
} EIP_ENCAP_DATA_T;

/** state of session pool entry */
typedef enum {
    EIP_SESSION_STATE_UNUSED = 0,               /**< session handle is uninitialized */
    EIP_SESSION_STATE_READY = 1,                /**< session handle is ready to be opened */
    EIP_SESSION_STATE_OPEN = 2,                 /**< session handle is open */
    EIP_SESSION_STATE_REGISTERED = 3,           /**< session handle is open & registered */
} EIP_SESSION_STATE_T;

/** session handle */
typedef struct {
    EIP_SOCKT_T sock;                           /**< TCP server socket */
    EIP_SESSION_STATE_T state;                  /**< state of session entry */
    EIP_UINT32 explCnt;                         /**< number of explicit connection for this session */
    OPENER_TIMESTAMP_T timeout;                 /**< next timeout timestamp */
} EIP_SESSION_T;

/** Delayed Encapsulation Message buffer entry */
typedef struct {
    OPENER_TIMESTAMP_T timeout;                 /**< next timeout timestamp */
    EIP_SOCKT_T sock;                           /**< socket of originator */
    S_CIP_Net_Addr originatorAddr;              /**< address of originator */
    GOAL_BUFFER_T *pTxBuf;                      /**< delayed message data */
} EIP_DELAYED_MSG_T;

/** Encapsulation data for EIP instance */
typedef struct {
    EIP_DELAYED_MSG_T *delayedMsg;              /**< buffer for delayed Encapsulation messages */
    EIP_SESSION_T *sessionIdPool;               /**< Encapsulation Layer session pool */
    int listIdentityDelay;                      /**< delay factor */
    GOAL_BUFFER_T **ppClass3Retrans;            /**< Class 3 message Retransmit buffer reference */
    EIP_ENCAP_DATA_T explData;                  /**< Explicit Encapsulation Data */
    EIP_ENCAP_DATA_T implData;                  /**< Implicit Encapsulation Data */
} EIP_ENCAP_INSTANCE_T;

/** States of a connection */
typedef enum {
    EIP_CON_STATE_NONEXISTENT = 0,              /**< does not exist */
    EIP_CON_STATE_CONFIGURING = 1,              /**< connection is being configured */
    EIP_CON_STATE_WAITINGFORCONNECTIONID = 2,   /**< only used in DeviceNet */
    EIP_CON_STATE_ESTABLISHED = 3,              /**< connection established */
    EIP_CON_STATE_TIMEDOUT = 4,                 /**< connection timed out */
    EIP_CON_STATE_DEFERREDDELETE = 5,           /**< only used in DeviceNet */
    EIP_CON_STATE_CLOSING = 6,                  /**< connection is closing */
} EIP_CON_STATE_T;

/* instance_type attributes */
typedef enum
{
  enConnTypeExplicit = 0,
  enConnTypeIOExclusiveOwner = 0x01,
  enConnTypeIOInputOnly = 0x11,
  enConnTypeIOListenOnly = 0x21,
} EConnType;

/*! The data needed for handling connections. This data is strongly related to
 * the connection object defined in the CIP-specification. However the full
 * functionality of the connection object is not implemented. Therefore this
 * data can not be accessed with CIP means.
 */
typedef struct CIP_ConnectionObject
{
  EIP_CON_STATE_T State;
  EConnType m_eInstanceType;
  EIP_UINT32 ExpectedPacketRate;
  EIP_UINT32 CIPProducedConnectionID;
  EIP_UINT32 CIPConsumedConnectionID;

  /* non CIP Attributes, only relevant for opened connections */
  EIP_UINT16 ConnectionSerialNumber;
  EIP_UINT16 OriginatorVendorID;
  EIP_UINT32 OriginatorSerialNumber;
  EIP_UINT32 O_to_T_RPI;
  EIP_UINT16 o2tSize;
  EIP_UINT8 o2tType;
  EIP_UINT32 T_to_O_RPI;
  EIP_UINT16 t2oSize;
  EIP_UINT8 t2oPrio;
  EIP_UINT8 t2oType;
  EIP_UINT32 flags;
  S_CIP_ConnectionPath ConnectionPath; /* padded EPATH */
  EIP_CIP_INSTANCE_T *p_stConsumingInstance;
  EIP_CIP_INSTANCE_T *p_stProducingInstance;
  EIP_UINT32 encapSeqCntProducing;              /**< ENCAP sequence count for class 0/1 producing connections */
  EIP_UINT32 encapSeqCntConsuming;              /**< ENCAP sequence count for class 0/1 consuming connections */
  EIP_UINT16 cipSeqCntProducing;                /**< CIP sequence count for class 1 producing connections */
  EIP_UINT16 cipSeqCntConsuming;                /**< CIP sequence count for Class 1 consuming connections */
  OPENER_TIMESTAMP_T txTriggerTimer;            /**< transmission trigger timer */
  OPENER_TIMESTAMP_T watchdogTimer;             /**< next timeout of inactivity watchdog */
  OPENER_TIMESTAMP_T watchdogTime;              /**< inactivity watchdog interval in ms */
  EIP_UINT32 pitTime;                           /**< production inhibit time */
  OPENER_TIMESTAMP_T endOfPit;                  /**< end of production inhibit time */

  S_CIP_Net_Addr produce_addr; /* socket address for produce */
  S_CIP_Net_Addr consume_addr;  /* the address of the originator that established the connection. needed for scanning if the right packet is arriving */
  EIP_SOCKT_T sockfd; /* socket handle  for */

  /* pointers to connection handling functions */
  TConnCloseFunc m_pfCloseFunc;
  TConnTimeOutFunc m_pfTimeOutFunc;
  TConnSendDataFunc m_pfSendDataFunc;
  TConnRecvDataFunc m_pfReceiveDataFunc;

  /* pointers to be used in the active connection list */
  struct CIP_ConnectionObject *m_pstNext;
  struct CIP_ConnectionObject *m_pstFirst;

  EIP_UINT32 OriginatorAddr;
  EIP_SOCKT_T OriginatorSock;

  GOAL_BUFFER_T *pClass3Retrans;              /**< Class 3 message Retransmit buffer */

} S_CIP_ConnectionObject;

/** connection point management */
typedef struct S_CIP_Connection
{
  unsigned int m_unOutputAssembly;              /**< O2T endpoint */
  unsigned int m_unInputAssembly;               /**< T2O endpoint */
  unsigned int m_unConfigAssembly;              /**< config endpoint */
  unsigned int m_unNumConData;                  /**< number of connection data instances */
  S_CIP_ConnectionObject *pConnectionData;      /**< connection data list */
  struct S_CIP_Connection *pNext;               /**< next element */
} S_CIP_Connection;

/** list element for connectible object classes */
typedef struct TConnMgmHandling
{
  EIP_UINT32 m_nClassID;
  TConnOpenFunc m_pfOpenFunc;
  struct TConnMgmHandling *pNext;
} TConnMgmHandling;

/** Connection Manager Instance data */
typedef struct {
    S_CIP_Connection *pEOConnections;           /**< list of Exclusive Owner connections */
    S_CIP_Connection *pIOConnections;           /**< list of Input Only connections */
    S_CIP_Connection *pLOConnections;           /**< list of Listen Only connections */
    S_CIP_ConnectionObject *explicitConnections; /**< available explicit connections */
    TConnMgmHandling *pConnMgmList;             /**< list of connectible objects */
    S_CIP_ConnectionObject *activeConnectionList; /**< all currently active connections */
    S_CIP_ConnectionObject newConnection;       /**< parse buffer for [L]FWDOpen requests */
    EIP_UINT8 timeoutMultiplier;                /**< connection timeout multiplier */
    S_CIP_KeyData ekData;                       /**< Electronic Key data */
    EIP_UINT8 *pCfgData;                        /**< data for config assembly */
    EIP_UINT16 cfgDataLen;                      /**< length of config assembly data */
    EIP_UINT32 o2tRundIdleState;                /**< consumed Run/Idle State */
    EIP_UINT32 t2oRundIdleState;                /**< produced Run/Idle State */
    EIP_UINT32 incarnationId;                   /**< upper word of connection IDs */
    EIP_UINT32 connectionId;                    /**< lower word of connection IDs */
    EIP_UINT16 cntOpenReq;                      /**< open requests */
    EIP_UINT16 cntOpenFailFormat;               /**< failed open requests: bad format */
    EIP_UINT16 cntOpenFailResource;             /**< failed open requests: no resources */
    EIP_UINT16 cntOpenFailOther;                /**< failed open requests: others */
    EIP_UINT16 cntCloseReq;                     /**< close requests */
    EIP_UINT16 cntCloseFailFormat;              /**< failed close requests: bad format */
    EIP_UINT16 cntCloseFailOther;               /**< failed close requests: others */
    EIP_UINT16 cntTimeouts;                     /**< connection timeouts */
    EIP_UINT32 eoTimeouts;                      /**< active Exclusive Owner timeouts */
} EIP_CM_INSTANCE_T;

#if GOAL_DLR == 1
typedef struct {
    EIP_UINT32 ipAddr;
    EIP_UINT8 macAddr[6];
} EIP_DLR_SUVI_ADDR_T;

typedef struct {
    EIP_UINT8 dlrNetworkTopology;
    EIP_UINT8 dlrNetworkStatus;
    EIP_DLR_SUVI_ADDR_T dlrSuViAddr;
    EIP_UINT32 dlrCapability;
    EIP_UINT16 padding;
    EIP_BOOL8 padBool;
    EIP_UINT16 ringPort1InstanceId;
    EIP_UINT16 ringPort2InstanceId;
} EIP_DLR_ATTRIBUTES_T;

/** DLR instance data */
typedef struct {
    GOAL_DLR_T *pDlr;                           /**< DLR stack instance handle */
    EIP_DLR_ATTRIBUTES_T attr;                  /**< DLR object attributes */
} EIP_DRL_INSTANCE_T;
#endif /* GOAL_DLR */

/** ACD instance data */
typedef void EIP_ACD_INSTANCE_T;

typedef struct {
    EIP_UINT32 InOctets;
    EIP_UINT32 InUcastPackets;
    EIP_UINT32 InNUcastPackets;
    EIP_UINT32 InDiscards;
    EIP_UINT32 InErrors;
    EIP_UINT32 InUnknownProtos;
    EIP_UINT32 OutOctets;
    EIP_UINT32 OutUcastPackets;
    EIP_UINT32 OutNUcastPackets;
    EIP_UINT32 OutDiscards;
    EIP_UINT32 OutErrors;
} S_CIP_InterfaceCounters;

typedef struct {
    EIP_UINT32 AlignmentErrors;
    EIP_UINT32 FCSErrors;
    EIP_UINT32 SingleCollisions;
    EIP_UINT32 MultipleCollisions;
    EIP_UINT32 SQETestErrors;
    EIP_UINT32 DeferredTransmissions;
    EIP_UINT32 LateCollisions;
    EIP_UINT32 ExcessiveCollisions;
    EIP_UINT32 MACTransmitErrors;
    EIP_UINT32 CarrierSenseErrors;
    EIP_UINT32 FrameTooLong;
    EIP_UINT32 MACReceiveErrors;
} S_CIP_MediaCounters;

typedef struct {
    EIP_UINT64 InOctets;
    EIP_UINT64 InUcastPackets;
    EIP_UINT64 InMcastPackets;
    EIP_UINT64 InBcastPackets;
    EIP_UINT64 OutOctets;
    EIP_UINT64 OutUcastPackets;
    EIP_UINT64 OutMcastPackets;
    EIP_UINT64 OutBcastPackets;
} S_CIP_HCInterfaceCounters;

typedef struct {
    EIP_UINT64 AlignmentErrors;
    EIP_UINT64 FCSErrors;
    EIP_UINT64 MACTransmitErrors;
    EIP_UINT64 FrameTooLong;
    EIP_UINT64 MACReceiveErrors;
    EIP_UINT64 SymbolErrors;
} S_CIP_HCMediaCounters;

typedef struct {
    EIP_UINT16 control;
    EIP_UINT16 speed;
} S_CIP_InterfaceControl;

typedef struct S_CIP_InterfaceCapability
{
  EIP_UINT32 Capabilities;
  EIP_UINT8 SpeedDuplexCount;
  EIP_UINT16 Speed[6];
  EIP_UINT8 DuplexMode[6];
} S_CIP_InterfaceCapability;

typedef struct S_CIP_EthernetLinkAttributes
{
    EIP_UINT32 InterfaceSpeed;
    EIP_UINT32 InterfaceFlags;
    EIP_UINT8 PhysicalAddress[6];
    S_CIP_Short_String InterfaceLabel;
    S_CIP_InterfaceCounters *pInterfaceCounters;
    S_CIP_MediaCounters *pMediaCounters;
    S_CIP_InterfaceControl InterfaceControl;
    EIP_UINT8 adminState;
    S_CIP_InterfaceCapability InterfaceCapability;
    S_CIP_HCInterfaceCounters *pHCIfaceCounters;
    S_CIP_HCMediaCounters *pHCMediaCounters;
} S_CIP_EthernetLinkAttributes;

/** Ethernet port configuration Element */
GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct {
    uint16_t control;                           /**< InterfaceControl.ControlBits */
    uint16_t speed;                             /**< InterfaceControl.ForcedSpeed */
    uint8_t enabled;                            /**< Admin State */
} GOAL_TARGET_PACKED EIP_PORT_T;
GOAL_TARGET_PACKED_STRUCT_POST

/** Ethernet port configuration */
GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct {
    EIP_PORT_T ports[GOAL_CONFIG_MAX_PORT_COUNT]; /**< port configuration */
} GOAL_TARGET_PACKED EIP_PORT_CFG_T;
GOAL_TARGET_PACKED_STRUCT_POST

/** Ethernet Link Instance data */
typedef struct {
    S_CIP_EthernetLinkAttributes attr;          /**< Ethernet Link instance attributes */
    EIP_UINT16 cfgInstance;                     /**< instance that must be reconfigured */
    EIP_UINT8 flgUpdate;                        /**< an instance needs a reconfiguration */
    EIP_PORT_CFG_T portCfg;                     /**< port configuration */
    GOAL_ETH_PORT_STATE_T *portState;           /**< port properties */
    uint32_t portLinkUp;                        /**< link map of external ports */
    uint32_t *portCapas;                        /**< capabilities of external port */
    uint32_t portCapasInt;                      /**< capabilities of internal port */
} EIP_ETH_INSTANCE_T;

/** Identity instance data */
typedef struct {
    S_CIP_IdentityAtributes attr;               /**< CIP instance attributes */
    EResetState resetState;                     /**< device's reset state */
} EIP_ID_INSTANCE_T;

/** Message Router Instance data */
typedef struct {
    S_CIP_MR_Request req;                       /**< Message Router request data */
    S_CIP_MR_Response res;                      /**< Message Router response data */
    S_CIP_VirtualClass *pVirtualClassList;      /**< List of virtual CIP classes */
    EIP_CIP_CLASS_T *pClassList;                /**< List of allocated CIP classes */
} EIP_MR_INSTANCE_T;

/** QoS instance attributes */
typedef struct {
    EIP_UINT8 qosVLANenable;
    EIP_UINT8 qosDSCPurgent;
    EIP_UINT8 qosDSCPscheduled;
    EIP_UINT8 qosDSCPhigh;
    EIP_UINT8 qosDSCPlow;
    EIP_UINT8 qosDSCPexplicit;
} EIP_QOS_ATTRIBUTES_T;

/** QoS instance data */
typedef struct {
    EIP_QOS_ATTRIBUTES_T attr;                  /**< attributes altered via EIP */
    EIP_QOS_ATTRIBUTES_T attrActive;            /**< attributes used for DSCP values */
} EIP_QOS_INSTANCE_T;

typedef struct
{
  EIP_UINT8  m_unAllocControl;
  EIP_UINT16 m_unNumMcast;
  EIP_UINT32 m_unMcastStartAddr;
} SMcastConfig;

/** CIP LastConflictDetected (Attr 11) */
typedef struct S_CIP_ACDLastConflictDetected
{
  EIP_UINT8 AcdActivity;
  EIP_UINT8 RemoteMac[6];
  EIP_UINT8 ArpPru[28];
} S_CIP_ACDLastConflictDetected;

typedef struct S_CIP_TCPIPAttributes
{
    EIP_UINT32 Status;
    EIP_UINT32 ConfigCapability;
    EIP_UINT32 ConfigControl;
    S_CIP_EPATH PhysicalLinkObject;
    S_CIP_TCPIPNetworkInterfaceConfiguration InterfaceConfig;
    S_CIP_String HostName;
    EIP_UINT8 TTLValue;
    SMcastConfig MultiCastConfig;
    EIP_UINT8 SelectAcd;
    S_CIP_ACDLastConflictDetected LastConflictDetected;
    EIP_UINT16 EncapTimeout;
} S_CIP_TCPIPAttributes;

/** TCP/IP instance data */
typedef struct {
    S_CIP_TCPIPAttributes attr;                 /**< CIP instance attributes */
    EIP_UINT8 flgUpdate;                        /**< IP address must be updated */
    EIP_UINT32 mcAddrUsed;                      /**< Active Multicast addresses */
} EIP_TCPIPIF_INSTANCE_T;

/** ACD Conflict fallback configuration */
typedef struct EIP_ACD_CFG_T {
    uint32_t ipAddress;                         /**< fallback IP address */
    uint32_t subnetMask;                        /**< fallback subnet mask */
    uint32_t gateway;                           /**< fallback gateway */
} EIP_ACD_CFG_T;

/** EtherNet/IP instance configuration */
typedef struct {
    uint16_t vendorId;                          /**< ODVA Vendor ID */
    uint16_t deviceType;                        /**< CIP device type */
    uint16_t productCode;                       /**< vendor specific product code */
    uint8_t revisionMajor;                      /**< major revision */
    uint8_t revisionMinor;                      /**< minor revision */
    uint32_t serialNum;                         /**< serial number */
    char productName[EIP_CFG_LEN_PROD_NAME];    /**< product name */
    char domainName[EIP_CFG_LEN_DOMAIN_NAME];   /**< domain name */
    char hostName[EIP_CFG_LEN_HOST_NAME];       /**< host name */
    uint16_t numExplicitCon;                    /**< number of explicit connections */
    GOAL_BOOL_T ethCounters;                    /**< support of Ethernet Link counters */
    GOAL_BOOL_T ethIfControl;                   /**< support of Ethernet Link Interface Control */
    GOAL_BOOL_T ethChangeAfterReset;            /**< change of interface config only after reset */
    GOAL_BOOL_T ipChangeAfterReset;             /**< change of IP address only after reset */
    uint16_t numSessions;                       /**< number of supported encapsulation sessions */
    GOAL_BOOL_T o2tRunIdleHeader;               /**< consumed data has Run/Idle Header */
    GOAL_BOOL_T t2oRunIdleHeader;               /**< produced data has Run/Idle Header */
    GOAL_BOOL_T qosSupport;                     /**< QoS object is supported */
    uint16_t numDelayedMsg;                     /**< size of Delayed Encapsulation Message buffer */
    GOAL_BOOL_T dhcpSupport;                    /**< device is DHCP client */
    GOAL_BOOL_T dlrSupport;                     /**< device is DLR node */
    GOAL_BOOL_T acdSupport;                     /**< support of ACD */
    EIP_ACD_CFG_T acdConflictFallback;          /**< configured fallback if conflict detected */
    uint32_t numLldpTableEntries;               /**< number of LLDP Data Table entries */
    GOAL_BOOL_T hwConfigSupport;                /**< support of hardware configurable IP */
} EIP_CFG_T;

#if GOAL_CONFIG_GEN_RPC == 1
typedef struct GOAL_EIP_CYCLIC_DATA_T {
    struct GOAL_EIP_CYCLIC_DATA_T *pNext;       /**< next entry */
    GOAL_BOOL_T flgNewData;                     /**< indicate updated data */
    uint32_t nInstanceNr;                       /**< this instance's number (unique within the class) */
} GOAL_EIP_CYCLIC_DATA_T;
#endif

/** LLDP Data table entry */
typedef struct {
    GOAL_ETH_MAC_ADDR_T srcMac;                 /**< SRC MAC of LLDP frame */
    uint16_t ethLinkInst;                       /**< Ethernet Link instance */
    GOAL_ETH_MAC_ADDR_T cipMac;                 /**< CIP MAC address of sender */
    char ifaceLable[EIP_LLDP_IFACE_NAME_BUF];   /**< CIP interface Lable */
    uint16_t ttl;                               /**< tie to live */
    uint16_t sysCapas;                          /**< system capabilities */
    uint16_t enabledCapas;                      /**< enabled capabilities */
    uint32_t managementAddr;                    /**< CIP Management address (IPv4) */
    struct {
        uint16_t vendorId;                      /**< Vendor ID */
        uint16_t devType;                       /**< device Type */
        uint16_t productCode;                   /**< product code */
        uint8_t revMajor;                       /**< Major Revision */
        uint8_t revMinor;                       /**< Minor Revision */
        uint32_t serialNum;                     /**< Serial Number */
    } cipId;
    uint32_t lastChange;                        /**< sysUpTime when an entry changed */
    GOAL_TIMESTAMP_T expTimeout;                /**< expiration timeout of entry */
} EIP_LLDP_DATA_TABLE_T;

/** LLDP instance data */
typedef struct {
    GOAL_LLDP_HANDLE_T *pLldp;                  /**< GOAL_LLDP_HANDLE_T */
    EIP_LLDP_DATA_TABLE_T rxData;               /**< entry based on last received frame */
    uint32_t rxDataValid;                       /**< valid entries in rxData */
    uint32_t enableFlags;                       /**< LLDP Rx/Tx Enable FLags */
    uint16_t txInt;                             /**< LLDP Tx interval in seconds */
    uint8_t msgTxHold;                          /**< validity factor */
    EIP_BIT_ARRAY_T enableArray;                /**< LLDP enable flags as CIP data type */
    uint16_t dataStore;                         /**< type of LLDP data store */
    uint32_t lastChange;                        /**< last change in LLDP data storage */
    uint32_t managementAddrOld;                 /**< former Management address */
} EIP_LLDP_INSTANCE_T;

/** LLDP data store */
typedef struct {
    EIP_LLDP_DATA_TABLE_T *dataTable;           /**< LLDP Data table */
    EIP_UINT16 maxInstance;                     /**< maximum instance that is currently active */
    EIP_UINT16 numInstance;                     /**< number of instances that are currently active */
} EIP_LLDP_DATA_STORE_T;

/** EtherNet/IP instance data */
typedef struct EIP_INSTANCE_T {
    GOAL_INSTANCE_HEADER(EIP_INSTANCE_T);       /**< instance header */
    struct GOAL_EIP_WRAPPER_T *pWrapper;        /**< GOAL wrapper instance data */
    GOAL_MI_ETH_T *pMiEthHdl;                   /**< Eth MI handle */
    GOAL_MI_NET_T *pMiNetHdl;                   /**< Net MI handle */
    uint32_t cmVarInstId;                       /**< instance ID for CM variables */
    EIP_CFG_T cfg;                              /**< configuration of this instance */
    EIP_DRV_DATA_T drv;                         /**< driver instance data */
    EIP_ENCAP_INSTANCE_T encap;                 /**< encapsulation layer instance data */
    EIP_CM_INSTANCE_T cm;                       /**< connection manager instance data */
#if GOAL_DLR == 1
    EIP_DRL_INSTANCE_T *pDlr;                   /**< DLR instance data */
#endif /* GOAL_DLR */
    EIP_ETH_INSTANCE_T eth;                     /**< Ethernet Link instance data */
    EIP_ID_INSTANCE_T id;                       /**< Identity instance data */
    EIP_MR_INSTANCE_T mr;                       /**< message router instance data */
    EIP_QOS_INSTANCE_T qos;                     /**< QoS instance data */
    EIP_TCPIPIF_INSTANCE_T tcpIpIf;             /**< TCP/IP Interface instance data */
    EIP_LLDP_INSTANCE_T lldp;                   /**< LLDP instance data */
    EIP_LLDP_DATA_STORE_T lldpDataStore;        /**< LLDP data store */
    EIP_LED_DATA_T led;                         /**< LED handler instance data */
    GOAL_ACD_HANDLE_T *pAcd;                    /**< ACD instance data */
    GOAL_TIMER_T *pTimer;                       /**< generic timer */
#if GOAL_CONFIG_GEN_RPC == 1
    GOAL_EIP_CYCLIC_DATA_T *pEipCyclic;         /**< list of registered assembly objects */
    uint8_t *asmBuffer;                         /**< buffer for assembly data header insertion */
    uint16_t asmBufferSize;                     /**< buffer length for assembly data header insertion */
#endif
#if GOAL_CONFIG_MEDIA_MI_DM == 1
    GOAL_MI_MCTC_DP_T mctcDp;                   /**< MCTC data provider status */
    uint16_t mctcDpSeqCnt;                      /**< DP sequence Counter (equals cntUpdate) */
    GOAL_MI_DM_PART_T dmTxDp;                   /**< TX data provider status */
    GOAL_MI_DM_GROUP_T *pDmGroupInput;          /**< MI DM Input group handle */
    GOAL_MI_DM_GROUP_T *pDmGroupOutput;         /**< MI DM Output group handle */
#endif
} EIP_INSTANCE_T;


#endif /* EIP_TYPES_H */
