/** @file
 *
 * @brief Types and Defines for the CIP at the assembly core
 *
 * This module defines the GOAL - CIP data types used by the assembly core.
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

#ifndef GOAL_CIPTYPES_H
#define GOAL_CIPTYPES_H


/****************************************************************************/
/* Defines */
/****************************************************************************/
/* maximal length of the product name */
#define CIP_ID_PRODUCTNAME_LEN 32

#define CIP_ANY 0x00
#define CIP_BOOL 0xC1
#define CIP_SINT 0xC2
#define CIP_INT 0xC3
#define CIP_DINT 0xC4
#define CIP_LINT 0xC5
#define CIP_USINT 0xC6
#define CIP_UINT 0xC7
#define CIP_UDINT 0xC8
#define CIP_ULINT 0xC9
#define CIP_REAL 0xCA
#define CIP_LREAL 0xCB
#define CIP_STIME 0xCC
#define CIP_DATE 0xCD
#define CIP_TIME_OF_DAY 0xCE
#define CIP_DATE_AND_TIME 0xCF
#define CIP_STRING 0xD0
#define CIP_BYTE 0xD1
#define CIP_WORD 0xD2
#define CIP_DWORD 0xD3
#define CIP_LWORD 0xD4
#define CIP_STRING2 0xD5
#define CIP_FTIME 0xD6
#define CIP_LTIME 0xD7
#define CIP_ITIME 0xD8
#define CIP_STRINGN 0xD9
#define CIP_SHORT_STRING 0xDA
#define CIP_TIME 0xDB
#define CIP_EPATH 0xDC
#define CIP_ENGUNIT 0xDD

/* definition of some CIP structs */
/* need to be validated in IEC 1131-3 subclause 2.3.3 */
#define CIP_BYTE_ARRAY 0xA4

#define INTERNAL_UINT16_6 0xf0

/* definition of CIP service codes */
#define CIP_GET_ATTRIBUTE_SINGLE 0x0E
#define CIP_SET_ATTRIBUTE_SINGLE 0x10
#define CIP_RESET 0x05
#define CIP_CREATE 0x08
#define CIP_GET_ATTRIBUTE_ALL 0x01
#define CIP_FORWARD_OPEN 0x54
#define CIP_FORWARD_CLOSE 0x4E
#define CIP_UNCONNECTED_SEND 0x52
#define CIP_GET_CONNECTION_OWNER 0x5A
#define CIP_LARGE_FORWARD_OPEN 0x5B
#define CIP_GET_AND_CLEAR 0x4C

/* definition of Flags for CIP Attributes */
#define CIP_ATTRIB_NONE 0x00
#define CIP_ATTRIB_GETABLEALL 0x01
#define CIP_ATTRIB_GETABLESINGLE 0x02
#define CIP_ATTRIB_SETABLE 0x04
/*combined for conveniance */
#define CIP_ATTRIB_SETGETABLE 0x07              /* both set and getable */
#define CIP_ATTRIB_GETABLE 0x03                 /* both single and all */

/* for now we support extended status codes up to 2 16bit values
 * there is mostly only one 16bit value used */
#define MAX_SIZE_OF_ADD_STATUS 2


/****************************************************************************/
/* Typedefs */
/****************************************************************************/
typedef enum
{
    enOpened,
    enTimedOut,
    enClosed
} EIOConnectionEvent;

typedef struct
{
    uint32_t ipAddr;
    uint16_t port;
} S_CIP_Net_Addr;

typedef struct
{
    uint16_t len;
    uint8_t *Data;
} S_CIP_Byte_Array;

typedef struct
{
    uint8_t Length;
    uint8_t *String;
} S_CIP_Short_String;

typedef struct
{
    int16_t Length;
    uint8_t *String;
} S_CIP_String;

typedef struct
{
    uint8_t PathSize;
    uint16_t ClassID;
    uint16_t InstanceNr;
    uint16_t MemberID;
    uint16_t AttributNr;
} S_CIP_EPATH;

typedef struct
{
    uint8_t PathSize;
    uint32_t ClassID;
    uint32_t ConnectionPoint[3];
    uint8_t DataSegment;
    uint8_t *SegmentData;
} S_CIP_ConnectionPath;

typedef struct
{
    uint16_t VendorID;
    uint16_t DeviceType;
    uint16_t ProductCode;
    uint8_t MajorRevision;
    uint8_t MinorRevision;
} S_CIP_KeyData;

typedef struct
{
    uint8_t MajorRevision;
    uint8_t MinorRevision;
} S_CIP_Revision;

typedef struct
{
    uint8_t SegmentType;
    uint8_t KeyFormat;
    S_CIP_KeyData KeyData;
} S_CIP_ElectronicKey;

typedef struct
{
    uint8_t service;
    S_CIP_EPATH requestPath;
    int16_t dataLength;
    uint8_t *pData;
} S_CIP_MR_Request;

typedef struct
{
    uint8_t replyService;
    uint8_t reserved;
    uint8_t generalStatus;
    uint8_t sizeofAdditionalStatus;
    uint16_t additionalStatus[MAX_SIZE_OF_ADD_STATUS];
    int16_t dataLength;
    uint8_t *pData;
} S_CIP_MR_Response;

typedef struct S_CIP_IdentityAtributes
{
    uint16_t VendorID;
    uint16_t DeviceType;
    uint16_t ProductCode;
    S_CIP_Revision Revison;
    uint16_t ID_Status;
    uint32_t SerialNumber;
    S_CIP_Short_String ProductName;
} S_CIP_IdentityAtributes;

typedef struct
{
    uint16_t CIP_AttributNr;
    uint8_t CIP_Type;
    uint8_t CIP_AttributeFlags;                 /* bit 0: getable_all; bit 1: getable_single; bit 2: setable_single; bits 3-7 reserved */
    void *pt2data;
} S_CIP_attribute_struct;

/* type definition of CIP service sructure */

/* instances are stored in a linked list */
typedef struct CIP_Instance
{
    uint32_t nInstanceNr;                       /* this instance's number (unique within the class) */
    S_CIP_attribute_struct *pstAttributes;      /* pointer to an array of attributes which is unique to this instance */
    struct CIP_Class *pstClass;                 /* class the instance belongs to */
    struct CIP_Instance *pstNext;               /* next instance, all instances of a class live in a linked list */
} S_CIP_Instance;

typedef struct CIP_Class
{ /* Class is a subclass of Instance */
    struct CIP_Instance m_stSuper;

    /* the rest of theswe are specific to the Class class only. */
    uint32_t nClassID;                          /* class ID */
    uint16_t nRevision;                         /* class revision */
    uint16_t nNr_of_Instances;                  /* number of instances in the class (not including instance 0) */
    uint16_t nNr_of_Attributes;                 /* number of attributes of each instance */
    uint16_t nMaxAttribute;                     /* highest defined attribute number (attribute numbers are not necessarily consecutive) */
    uint32_t nGetAttrAllMask;                   /* mask indicating which attributes are returned by getAttributeAll */
    uint16_t nNr_of_Services;                   /* number of services supported */
    S_CIP_Instance *pstInstances;               /* pointer to the list of instances */
    struct CIP_service_struct *pstServices;     /* pointer to the array of services */
    char *acName;                               /* class name */
} S_CIP_Class;


#endif /* GOAL_CIPTYPES_H */
