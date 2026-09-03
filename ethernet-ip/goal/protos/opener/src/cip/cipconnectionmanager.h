/** @file
 *
 * @brief Connection Manager Class
 *
 * This module implements the Connection Manager class
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

#ifndef CIPCONNECTIONMANAGER_H
#define CIPCONNECTIONMANAGER_H


/****************************************************************************/
/* Public Defines */
/****************************************************************************/
/* Connection Types */
#define CIP_CM_CONN_TYPE_NULL   0x00            /**< Connection Type: Null */
#define CIP_CM_CONN_TYPE_MCAST  0x01            /**< Connection Type: Multicast */
#define CIP_CM_CONN_TYPE_P2P    0x02            /**< Connection Type: Point to Point */
#define CIP_CM_CONN_TYPE_RSRVD  0x03            /**< Connection Type: <RESERVED> */


/* Connection Manager Error codes */
#define CIP_CON_MGR_SUCCESS 0x00
#define CIP_CON_MGR_ERROR_CONNECTION_IN_USE 0x0100
#define CIP_CON_MGR_ERROR_TRANSPORT_TRIGGER_NOT_SUPPORTED 0x0103
#define CIP_CON_MGR_ERROR_OWNERSHIP_CONFLICT 0x0106
#define CIP_CON_MGR_ERROR_CONNECTION_NOT_FOUND_AT_TARGET_APPLICATION 0x0107
#define CIP_CON_MGR_ERROR_RPI_NOT_ACCEPTED 0x0112
#define CIP_CON_MGR_ERROR_NO_MORE_CONNECTIONS_AVAILABLE 0x0113
#define CIP_CON_MGR_ERROR_VENDERID_OR_PRODUCTCODE_ERROR 0x0114
#define CIP_CON_MGR_ERROR_VENDERID_OR_PRODUCT_TYPE_ERROR 0x0115
#define CIP_CON_MGR_ERROR_REVISION_MISMATCH 0x0116
#define CIP_CON_MGR_ERROR_PIT_GREATER_THAN_RPI 0x011B
#define CIP_CON_MGR_ERROR_TRANS_CLASS_NOT_SUPPORTED 0x011C
#define CIP_CON_MGR_ERROR_TRIGGER_NOT_SUPPORTED 0x011D
#define CIP_CON_MGR_ERROR_INVALID_ORIGINATOR_TO_TARGET_NETWORK_CONNECTION_TYPE 0x0123
#define CIP_CON_MGR_ERROR_INVALID_TARGET_TO_ORIGINATOR_NETWORK_CONNECTION_TYPE 0x0124
#define CIP_CON_MGR_ERROR_INVALID_ORIGINATOR_TO_TARGET_SIZE 0x0127
#define CIP_CON_MGR_ERROR_INVALID_TARGET_TO_ORIGINATOR_SIZE 0x0128
#define CIP_CON_MGR_INVALID_CONFIGURATION_APP_PATH      0x0129
#define CIP_CON_MGR_INVALID_CONSUMING_APPLICATION_PATH  0x012A
#define CIP_CON_MGR_INVALID_PRODUCING_APPLICATION_PATH  0x012B
#define CIP_CON_MGR_INCONSISTENT_APPLICATION_PATH_COMBO 0x012F
#define CIP_CON_MGR_NON_LISTEN_ONLY_CONNECTION_NOT_OPENED 0x0119
#define CIP_CON_MGR_T2O_PARAM_MISMATCH_CONSIZE 0x0134
#define CIP_CON_MGR_T2O_PARAM_MISMATCH_FIXVAR 0x0135
#define CIP_CON_MGR_T2O_PARAM_MISMATCH_PRIO 0x0136
#define CIP_CON_MGR_T2O_PARAM_MISMATCH_TRANSPORTCLASS 0x0137
#define CIP_CON_MGR_T2O_PARAM_MISMATCH_TRIGGGER 0x0138
#define CIP_CON_MGR_T2O_PARAM_MISMATCH_PRODINHIBITTIME 0x0139
#define CIP_CON_MGR_ERROR_PARAMETER_ERROR_IN_UNCONNECTED_SEND_SERVICE 0x0205
#define CIP_CON_MGR_ERROR_INVALID_SEGMENT_TYPE_IN_PATH 0x0315
#define CIP_CON_MGR_TARGET_OBJECT_OUT_OF_CONNECTIONS 0x011A
#define CIP_CON_MGR_ERROR_NO_TARGET_APPLICATION_DATA_AVAILABLE 0x0810
#define CIP_CON_MGR_ERROR_NOT_CONF_FOR_OFF_SUBNET_MULTICAST 0x0813

/* dummy extended error codes, will be replaced by general error codes */
#define CIP_CON_MGR_ERROR_DUMMY_PERMISSION 0xffff

/*macros for comparing sequence numbers according to CIP spec vol 2 3-4.2 */
#define SEQ_LEQ32(a, b) ((int)((a) - (b)) <= 0)
#define SEQ_GEQ32(a, b) ((int)((a) - (b)) >= 0)
#define SEQ_GT32(a, b) ((int)((a) - (b)) > 0)

/* similar macros for comparing 16 bit sequence numbers */
#define SEQ_LEQ16(a, b) ((short)((a) - (b)) <= 0)
#define SEQ_GEQ16(a, b) ((short)((a) - (b)) >= 0)

/* Bit Positions in Network Connection Parameters for ForwardOpen */
#define CIP_NCP_FO_SIZE     0x01ff              /**< FO: connection size */
#define CIP_NCP_FO_VARSIZE  (1<<9)              /**< FO: Variable size */
#define CIP_NCP_FO_PRIO_POS 10                  /**< FO: priority */
#define CIP_NCP_FO_TYPE_POS 13                  /**< FO: connection type */
#define CIP_NCP_FO_RO       (1<<15)             /**< FO: redundant owner */

/* Bit Positions in Network Connection Parameters for LargeForwardOpen */
#define CIP_NCP_LFO_SIZE     0x0000ffff         /**< LFO: connection size */
#define CIP_NCP_LFO_VARSIZE  (1<<25)            /**< LFO: Variable size */
#define CIP_NCP_LFO_PRIO_POS 26                 /**< LFO: priority */
#define CIP_NCP_LFO_TYPE_POS 29                 /**< LFO: connection type */
#define CIP_NCP_LFO_RO       (1u<<31)           /**< LFO: redundant owner */

/* Bit Positions in Network Connection Parameters (common) */
#define CIP_NCP_MASK_PRIO    0x03               /**< priority mask */
#define CIP_NCP_MASK_TYPE    0x03               /**< connection type mask */

/* internal flags for connection data */
#define CIP_CM_FLAG_O2T_VAR     (1u<<0)         /**< O2T: variable size */
#define CIP_CM_FLAG_T2O_VAR     (1u<<1)         /**< T2O: variable size */
#define CIP_CM_FLAG_O2T_RO      (1u<<2)         /**< O2T: redundant owner */
#define CIP_CM_FLAG_T2O_RO      (1u<<3)         /**< T2O: redundant owner */
#define CIP_CM_FLAG_FIRST_RX    (1u<<4)         /**< first rx of data has not happened yet */
#define CIP_CM_FLAG_PRODUCING   (1u<<5)         /**< connection is producing data */
#define CIP_CM_FLAG_NO_PRODUCING (0u<<5)        /**< connection is not producing data */
#define CIP_CM_FLAG_PT_CYCLIC   (1u<<6)         /**< Production trigger: cyclic */
#define CIP_CM_FLAG_PT_COS      (1u<<7)         /**< Production trigger: change-of-state */
#define CIP_CM_FLAG_PT_APP      (1u<<8)         /**< Production trigger: application triggered */
#define CIP_CM_FLAG_TC_0        (1u<<9)         /**< Transport Class: 0 */
#define CIP_CM_FLAG_TC_1        (1u<<10)        /**< Transport Class: 1 */
#define CIP_CM_FLAG_TC_2        (1u<<11)        /**< Transport Class: 2 */
#define CIP_CM_FLAG_TC_3        (1u<<12)        /**< Transport Class: 3 */
#define CIP_CM_FLAG_SERVER      (1u<<13)        /**< Connection Direction Server */
#define CIP_CM_FLAG_NEW_DATA    (1u<<14)        /**< new data for non-cyclic connections */

/* TransportClassTrigger fields */
#define CIP_CM_TCT_DIR_SRVR    0x80             /**< direction: server */
#define CIP_CM_TCT_PROD_TRIG   0x70             /**< productiuon trigger mask */
#define CIP_CM_TCT_TRANS_CLASS 0x0F             /**< transport class mask */
#define CIP_CM_TCT_PT_CYCLIC   0x00             /**< Production trigger: cyclic */
#define CIP_CM_TCT_PT_COS      0x10             /**< Production trigger: change-of-state */
#define CIP_CM_TCT_PT_APP      0x20             /**< Production trigger: application triggered */
#define CIP_CM_TCT_TC_0        0x00             /**< Transport Class: 0 */
#define CIP_CM_TCT_TC_1        0x01             /**< Transport Class: 1 */
#define CIP_CM_TCT_TC_2        0x02             /**< Transport Class: 2 */
#define CIP_CM_TCT_TC_3        0x03             /**< Transport Class: 3 */

/* Logical segment IDs */
#define CIP_CM_SEG_LOG_CLASS_ID_8      0x20     /**< Logical Segment: Class ID (8 bit) */
#define CIP_CM_SEG_LOG_CLASS_ID_16     0x21     /**< Logical Segment: Class ID (16 bit) */
#define CIP_CM_SEG_LOG_INSTANCE_ID_8   0x24     /**< Logical Segment: Instance ID (8 bit) */
#define CIP_CM_SEG_LOG_INSTANCE_ID_16  0x25     /**< Logical Segment: Instance ID (16 bit) */
#define CIP_CM_SEG_LOG_INSTANCE_ID_32  0x26     /**< Logical Segment: Instance ID (32 bit) */
#define CIP_CM_SEG_LOG_MEMBER_ID_8     0x28     /**< Logical Segment: Member ID (8 bit) */
#define CIP_CM_SEG_LOG_MEMBER_ID_16    0x29     /**< Logical Segment: Member ID (16 bit) */
#define CIP_CM_SEG_LOG_MEMBER_ID_32    0x2A     /**< Logical Segment: Member ID (32 bit) */
#define CIP_CM_SEG_LOG_CON_POINT_8     0x2C     /**< Logical Segment: Connection ID (8 bit) */
#define CIP_CM_SEG_LOG_CON_POINT_16    0x2D     /**< Logical Segment: Connection ID (16 bit) */
#define CIP_CM_SEG_LOG_CON_POINT_32    0x2E     /**< Logical Segment: Connection ID (32 bit) */
#define CIP_CM_SEG_LOG_ATTR_ID_8       0x30     /**< Logical Segment: Attribute ID (8 bit) */
#define CIP_CM_SEG_LOG_ATTR_ID_16      0x31     /**< Logical Segment: Attribute ID (16 bit) */
#define CIP_CM_SEG_LOG_ELECTRONIC_KEY  0x34     /**< Logical Segment: Electronic key */
#define CIP_CM_SEG_LOG_SERVICE         0x38     /**< Logical Segment: Service */
#define CIP_CM_SEG_LOG_EXTENDED        0x3C     /**< Logical Segment: Service */

/* length of logical segments */
#define CIP_CM_SEG_LOG_8_LEN           1        /**< length of an 8 bit logical segment in words */
#define CIP_CM_SEG_LOG_16_LEN          2        /**< length of an 16 bit logical segment in words */
#define CIP_CM_SEG_LOG_32_LEN          3        /**< length of an 32 bit logical segment in words */

/* Electronic Key Segment */
#define CIP_CM_ELEC_KEY_FORMAT_STD     0x04     /**< standard key format */
#define CIP_CM_ELEC_KEY_FORMAT_STD_LEN 0x05     /**< lenght of standard key format in words */
#define CIP_CM_ELEC_KEY_MAJ_REV_MASK   0x7F     /**< Major Revision mask */
#define CIP_CM_ELEC_KEY_COMPAT_FLAG    0x80     /**< compatibility mode */

/* Network segment IDs */
#define CIP_CM_SEG_NET_PIT_MS          0x43     /**< Network Segment: Production Inhibit Time in ms */
#define CIP_CM_SEG_NET_SAFETY          0x50     /**< Network Segment: Safety */
#define CIP_CM_SEG_NET_PIT_US          0x51     /**< Network Segment: Production Inhibit Time in us */

/* Production Inhibit Time Segment */
#define CIP_CM_PIT_MS_LEN              1        /**< length of PIT in ms segment in words */
#define CIP_CM_PIT_US_LEN              3        /**< length of PIT in us segment in words */
#define CIP_CM_PIT_EMPTY      0xffffffff        /**< no PIT segment received */

/* Data Segment */
#define CIP_CM_SEG_DATA_SIMPLE         0x80     /**< Data Segment Simple */

#define CIP_CONNECTION_MANAGER_CLASS_CODE 0x06
#define CIP_CM_REVISION 1


/****************************************************************************/
/* Public functions */
/****************************************************************************/
EIP_STATUS eip_cmInit(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_UINT16 uniqueConnId                     /**< unique connection Id */
);

void eip_cmShutdown(
    EIP_INSTANCE_T *pEip                        /**< EtherNet/IP instance */
);

S_CIP_ConnectionObject *eip_cmConnectedObjectGetById(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_UINT32 connectionId                     /**< connection ID */
);


S_CIP_ConnectionObject *eip_cmConnectedObjectGetByOutAsm(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_UINT32 outAsmId,                        /**< Id of Output Assembly */
    EConnType conType                           /**< connection type */
);

void eip_cmConnectionDataCopy(
    S_CIP_ConnectionObject *pDst,               /**< copy destination */
    S_CIP_ConnectionObject *pSrc                /**< copy source */
);

void eip_cmConnectionCloseAll(
    EIP_INSTANCE_T *pEip                        /**< EtherNet/IP instance */
);

EIP_BOOL8 eip_cmOutputAssemblyConnected(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_UINT32 outAsmId                         /**< Id of assembly */
);

void eip_cmCommonConfigSet(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    S_CIP_ConnectionObject *pNewConnObj         /**< data of new connection */
);

void eip_cmActiveConnectionAdd(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    S_CIP_ConnectionObject *pConObj             /**< new connection */
);

void eip_cmActiveConnectionRemove(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    S_CIP_ConnectionObject *pConObj             /**< connection to remove */
);

void eip_cmConnectionIdInit(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_UINT32 uniqueConnId                     /**< Unique Connection ID */
);

void eip_cmOriginatorTimeoutCheck(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_SOCKT_T sock                            /**< originator socket */
);

EIP_STATUS eip_cmConnectionTrigger(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_UINT32 outputAsm,                       /**< output assembly id */
    EIP_UINT32 inputAsm                         /**< input assembly id */
);

EIP_UINT8 eip_cmElectronicKeyDataCheck(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    S_CIP_KeyData *pKeyData,                    /**< electronic key data */
    EIP_UINT16 *pExtStatus                      /**< extended error code buffer */
);

void eip_cmConnectionTimerCheck(
    EIP_INSTANCE_T *pEip                        /**< EtherNet/IP instance */
);

void eip_cmExplConnectionRemoteClose(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_SOCKT_T sock                            /**< closed socket */
);

#endif /* CIPCONNECTIONMANAGER_H */
