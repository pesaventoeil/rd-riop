/** @file
 *
 * @brief Common CIP functions
 *
 * This module contains functions used by the CIP layer.
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

#ifndef CIPCOMMON_H
#define CIPCOMMON_H


/****************************************************************************/
/* Public Defines */
/****************************************************************************/
/** CIP class attribute IDs */
#define EIP_CIP_CLASS_ATTR_REV            1     /**< #1: revision */
#define EIP_CIP_CLASS_ATTR_MAX_INST       2     /**< #2: maximum instance */
#define EIP_CIP_CLASS_ATTR_NUM_INST       3     /**< #3: number of instances */
#define EIP_CIP_CLASS_ATTR_OPT_ATTR       4     /**< #4: optional attributes list */
#define EIP_CIP_CLASS_ATTR_OPT_SRVC       5     /**< #5: optional services list */
#define EIP_CIP_CLASS_ATTR_MAX_ATTR_CLASS 6     /**< #6: maximum class attribute ID */
#define EIP_CIP_CLASS_ATTR_MAX_ATTR_INST  7     /**< #7: maximum instance attribute ID */


/****************************************************************************/
/* Utility functions */
/****************************************************************************/


/****************************************************************************/
/** Calculate the byte size of the bit array based on its bit size
 *
 * @return number of bytes in array data
 */
static inline EIP_UINT16 eip_cipBitArrayByteSizeGet(
    EIP_UINT16 bitlength                        /**< number of valid bits */
)
{
    return (bitlength) ? (bitlength / 8) + 1 : 0;
}


/****************************************************************************/
/* Public functions */
/****************************************************************************/
EIP_STATUS eip_cipInit(
    EIP_INSTANCE_T *pEip                        /**< EtherNet/IP instance */
);

void eip_cipShutdown(
    EIP_INSTANCE_T *pEip                        /**< EtherNet/IP instance */
);

EIP_STATUS eip_cipClassCreate(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_UINT32 classId,                         /**< class ID */
    EIP_UINT16 revision,                        /**< class revision */
    EIP_CIP_CLASS_T **ppNewClass                /**< [out] new CIP class */
);

EIP_STATUS eip_cipInstanceCreate(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_CIP_CLASS_T *pClass,                    /**< CIP class */
    EIP_UINT32 instanceId,                      /**< instance ID */
    EIP_CIP_INSTANCE_T **ppInstance             /**< [out] new instance */
);

EIP_STATUS eip_cipInstanceAttributeAdd(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_CIP_INSTANCE_T *pInstance,              /**< CIP instance */
    EIP_UINT16 attributeId,                     /**< attribute Id */
    EIP_UINT8 dataType,                         /**< CIP data type */
    EIP_INT16 minSize,                          /**< minimum attribute size */
    EIP_INT16 maxSize,                          /**< maximum attribute size */
    void *pData,                                /**< attribute data */
    EIP_UINT8 flags                             /**< access flags */
);

EIP_STATUS eip_cipClassAttributeAdd(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_CIP_CLASS_T *pClass,                    /**< CIP class */
    EIP_UINT16 attributeId,                     /**< attribute Id */
    EIP_UINT8 dataType,                         /**< CIP data type */
    EIP_INT16 minSize,                          /**< minimum attribute size */
    EIP_INT16 maxSize,                          /**< maximum attribute size */
    void *pData,                                /**< attribute data */
    EIP_UINT8 flags                             /**< access flags */
);

EIP_STATUS eip_cipInstanceServiceAdd(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_CIP_CLASS_T *pClass,                    /**< CIP class */
    EIP_UINT8 serviceId,                        /**< CIP service ID */
    EIP_CIP_SERVICE_FUNC_T funcService          /**< CIP service handler */
);

EIP_STATUS eip_cipClassServiceAdd(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_CIP_CLASS_T *pClass,                    /**< CIP class */
    EIP_UINT8 serviceId,                        /**< CIP service ID */
    EIP_CIP_CLASS_SERVICE_FUNC_T funcService    /**< CIP class service handler */
);

EIP_STATUS eip_cipInstanceAttributeUpdateCbSet(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_CIP_CLASS_T *pClass,                    /**< CIP class */
    EIP_CB_ATTR_UPDATE_T pCbInstAttrUpdate      /**< instance attribute update callback */
);

EIP_STATUS eip_cipClassAttributeUpdateCbSet(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_CIP_CLASS_T *pClass,                    /**< CIP class */
    EIP_CB_ATTR_UPDATE_T pCbClassAttrUpdate     /**< class attribute update callback */
);

EIP_STATUS eip_cipInstanceAttributeTestSetCbSet(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_CIP_CLASS_T *pClass,                    /**< CIP class */
    EIP_CB_ATTR_TEST_SET_T pCbInstAttrTestSet   /**< instance attribute test and set callback */
);

EIP_CIP_INSTANCE_T *eip_cipInstanceGet(
    EIP_CIP_CLASS_T* pClass,                    /**< CIP class */
    EIP_UINT32 instanceId                       /**< instance Id */
);

EIP_CIP_ATTR_T *eip_cipInstanceAttributeGet(
    EIP_CIP_INSTANCE_T *pInstance,              /**< CIP instance */
    EIP_UINT16 attributeId                      /**< attribute Id */
);

EIP_STATUS eip_cipInstanceGasGeneric(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_CIP_INSTANCE_T *pInstance               /**< CIP instance */
);

EIP_STATUS eip_cipInstanceGaaGeneric(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_CIP_INSTANCE_T *pInstance               /**< CIP instance */
);

EIP_STATUS eip_cipClassGasGeneric(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_CIP_CLASS_T *pClass                     /**< CIP instance */
);

EIP_STATUS eip_cipClassGaaGeneric(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_CIP_CLASS_T *pClass                     /**< CIP instance */
);

EIP_STATUS eip_cipInstanceSasGeneric(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_CIP_INSTANCE_T *pInstance               /**< CIP instance */
);

EIP_INT16 eip_cipDataEncode(
    EIP_UINT8 cipType,                          /**< CIP data type */
    void *pData,                                /**< data to be encoded */
    EIP_UINT8 **ppBuf                           /**< [out] response message buffer */
);

EIP_UINT16 eip_cipDataDecode(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    EIP_UINT8 cipType,                          /**< CIP data type */
    void *pData,                                /**< data to be encoded */
    EIP_UINT8 **ppBuf                           /**< [out] request message buffer */
);

EIP_UINT16 eip_cipEpathDecode(
    EIP_INSTANCE_T *pEip,                       /**< EtherNet/IP instance */
    S_CIP_EPATH *pEPath,                        /**< padded EPath */
    EIP_UINT8 **ppBuf,                          /**< [out] buffer of Message Router request */
    EIP_BYTE *pStatus                           /**< [out] General Status */
);

#endif /* CIPCOMMON_H */
