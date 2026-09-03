/** @file
 *
 * @brief communication core for Ethernet/ IP (EIP) via remote procedure call (RPC)
 *
 * This module provides the EIP function using RPC.
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

#ifndef GOAL_EIP_RPC_H
#define GOAL_EIP_RPC_H

#include <goal_media/goal_mi_mctc.h>
#include "opener_includes.h"


/****************************************************************************/
/* Defines */
/****************************************************************************/
/* RPC buffer size */
#define RPC_BUFF_SIZE_EIP 534                   /**< size of RPC buffer */

/* Function ID */
#define GOAL_RPC_FUNC_APPL_INIT (0)             /**< A<--C: application init */
#define GOAL_RPC_FUNC_CREATE_ASSEMBLY_OBJECT (1) /**< A-->C: creating assembly object */
#define GOAL_RPC_FUNC_ADD_EXCL_OWN_CONN (2)     /**< A-->C: add an exclusive owner connection point */
#define GOAL_RPC_FUNC_ADD_INPUT_ONLY_CONN (3)   /**< A-->C: add an input only connection point */
#define GOAL_RPC_FUNC_ADD_LISTEN_ONLY_CONN (4)  /**< A-->C: add an listen only connection point */
#define GOAL_RPC_FUNC_CREATE_CIP_CLASS (5)      /**< A-->C: create a CIP class */
#define GOAL_RPC_FUNC_INSERT_ATTRIBUTE (6)      /**< A-->C: insert an attribute */
#define GOAL_RPC_FUNC_GET_ATTRIBUTE (7)         /**< A-->C: get an attribute */
#define GOAL_RPC_FUNC_ADD_CIP_INSTANCE (8)      /**< A-->C: add a CIP instance */
#define GOAL_RPC_FUNC_ADD_CIP_INSTANCES (9)     /**< A-->C: get more CIP instances */
#define GOAL_RPC_FUNC_ADD_CIP_SERVICE (10)      /**< A-->C: insert a service */
#define GOAL_RPC_FUNC_CALL_CIP_SERVICE (11)     /**< A<--C: call the CIP servie */
#define GOAL_RPC_FUNC_IO_CONNECTION_EVENT (12)  /**< A<--C: inform. about an IO connection event */
#define GOAL_RPC_FUNC_GET_ATTRIBUTE_DATA (13)   /**< A<--C: get the data of the assembly class */
#define GOAL_RPC_FUNC_RUN_IDLE_CHANGED (14)     /**< A<--C: inform. about an changed run/ idle */
#define GOAL_RPC_FUNC_RESET_DEVICE (15)         /**< A<--C: device reset */
#define GOAL_RPC_FUNC_CFG_SET_VENDORID (16)     /**< A-->C: set the vendor id in identity of the device */
#define GOAL_RPC_FUNC_CFG_SET_DEVTYPE (17)      /**< A-->C: set the device type in identity of the device */
#define GOAL_RPC_FUNC_GET_EIP_VERSION (18)      /**< A-->C: get the version of the eip protocol stack */
#define GOAL_RPC_FUNC_CFG_SET_PRODUCTCODE (19)  /**< A-->C: set the device product code in identity of the device */
#define GOAL_RPC_FUNC_CFG_SET_REVISION (20)     /**< A-->C: set the revision in identity of the device */
#define GOAL_RPC_FUNC_CFG_SET_STATUS (21)       /**< A-->C: set the id status in identity of the device */
#define GOAL_RPC_FUNC_CFG_SET_SERIAL (22)       /**< A-->C: set the serial number in identity of the device */
#define GOAL_RPC_FUNC_CFG_SET_NAME (23)         /**< A-->C: set the product name in identity of the device */
#define GOAL_RPC_FUNC_CFG_SET_DOMAIN_NAME (24)  /**< A-->C: set the default domain name in identity of the device */
#define GOAL_RPC_FUNC_CFG_SET_HOST_NAME (25)    /**< A-->C: set the default host name in identity of the device */
#define GOAL_RPC_FUNC_CFG_SET_EXPL_CONN (26)    /**< A-->C: set thenmber of explicit connections in identity of the device */
#define GOAL_RPC_FUNC_CFG_SET_IMPL_CONN (27)    /**< A-->C: set thenmber of implicit connections in identity of the device */
#define GOAL_RPC_FUNC_CFG_SET_ETH_LINK_CNT (28) /**< A-->C: set the eth link counter support in identity of the device */
#define GOAL_RPC_FUNC_CFG_SET_ETH_LINK_CRTL (29) /**< A-->C: set the eth link control (forcing speed & duplex mode) support in identity of the device */
#define GOAL_RPC_FUNC_CFG_SET_CHANGE_ETH_AFTER_RESET (30) /**< A-->C: set the flag for applying of Link speed & duplex after device reset only of the device */
#define GOAL_RPC_FUNC_CFG_SET_CHANGE_IP_AFTER_RESET (31) /**< A-->C: set the flag for applying IP configuration after device reset only of the device */
#define GOAL_RPC_FUNC_CFG_SET_NUM_SESSIONS (32) /**< A-->C: set the number of Encapsulation sessions in identity of the device */
#define GOAL_RPC_FUNC_CFG_SET_TICK (33)         /**< A-->C: set the size of one tick (multiple of 1 ms) of the device */
#define GOAL_RPC_FUNC_CFG_SET_O2T_IDLE_HEADER (34) /**< A-->C: set the RunIdle Header mode for consumed connections in identity of the device */
#define GOAL_RPC_FUNC_CFG_SET_T2O_IDLE_HEADER (35) /**< A-->C: set the RunIdle Header mode for produced connections in identity of the device */
#define GOAL_RPC_FUNC_CFG_SET_QOS (36)          /**< A-->C: enable/ disable the QoS object in identity of the device */
#define GOAL_RPC_FUNC_CFG_SET_NUM_DELAYED_MSG (37) /**< A-->C: set the number of simultaneously delayable Encapsulation messages in identity of the device */
#define GOAL_RPC_FUNC_CFG_SET_DHCP (38)         /**< A-->C: enable/ disable the DHCP support in identity of the device */
#define GOAL_RPC_FUNC_CFG_SET_DLR (39)          /**< A-->C: enable/ disable the DLR support in identity of the device */
#define GOAL_RPC_FUNC_NEW (40)                  /**< A-->C: create new EtherNet/IP CC instance */
#define GOAL_RPC_FUNC_CB (41)                   /**< C-->A: Callback from EtherNet/IP stack to application */
#define GOAL_RPC_FUNC_DM_CONNECTION_ADD (42)    /**< A-->C: register DM indexes for new connection */
#define GOAL_RPC_FUNC_DM_DP_ADD (43)            /**< A-->C: configure data provider into cyclic stream */
#define GOAL_RPC_FUNC_SERIAL_NUM_SET (44)       /**< A-->C: set serial number after iniitalization */
#define GOAL_RPC_FUNC_CFG_SET_ACD (45)          /**< A-->C: enable/ disable the ACD support in identity of the device */
#define GOAL_RPC_FUNC_CIP_CLASS_REGISTER (46)   /**< A-->C: registering virtual CIP class callback function */
#define GOAL_RPC_FUNC_VIRTUAL_PARAM_CLASS_HANDLER_CB (47) /**< C-->A: virtual CIP class callback function */
#define GOAL_RPC_FUNC_CFG_SET_ACD_BEHAVIOR (48) /**< A-->C: configure ACD behavior if address conflict detected */
#define GOAL_RPC_FUNC_ACD_LAST_CONFLICT_RESET (49) /**< A-->c: reset attribute last conflict detected */
#define GOAL_RPC_FUNC_ASM_OBJECT_WRITE (50)     /**< A-->C: write an assembly object */
#define GOAL_RPC_FUNC_ASM_OBJECT_READ (51)      /**< A-->C: read an assembly object */
#define GOAL_RPC_FUNC_DM_ASM_ADD (52)           /**< A-->C: register ASM to DM indexes */
#define GOAL_RPC_FUNC_ACD_LAST_CONFLICT_GET (53) /**< A-->C: get attribute last conflict detected */
#define GOAL_RPC_FUNC_IDENTITY_RESET (54)       /**< A-->C: reset service for idenity object */
#define GOAL_RPC_FUNC_CFG_SET_HW_CFG (55)       /**< A-->C: enable/disable HW configurability */
#define GOAL_RPC_FUNC_NEW_IF_ASSIGN (56)        /**< create new EtherNet/IP CC instance for a specific Interface */

#define GOAL_EIP_RPC_PUSH_INST(pInst) \
    if (GOAL_RES_OK(res)) { \
        GOAL_RPC_PUSH(pInst->idInst); \
    }

#define GOAL_EIP_RPC_POP_INST(_tmp) \
    if (GOAL_RES_OK(res)) { \
        GOAL_INSTANCE_T *_pInst; \
        uint32_t idInst = 0; \
        GOAL_RPC_POP(idInst, uint32_t); \
        if (GOAL_RES_OK(res)) { \
            GOAL_INSTANCE_FOREACH(mpListInst->pInst, _pInst) { \
                if (idInst == _pInst->idInst) { \
                    _tmp = (EIP_INSTANCE_T *) _pInst; \
                    break; \
                } \
            } \
            if (NULL == _tmp) { \
                res = GOAL_ERR_NOT_FOUND; \
            } \
        } \
    }


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_eipRpcInit(
    void
);

#endif /* GOAL_EIP_RPC_H */

