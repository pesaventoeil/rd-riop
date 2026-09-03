/** @file
 *
 * @brief csap configuration - generated
 *
 * @copyright
 * Copyright 2010-2021.
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

#ifndef EIP_CSAP_H
#define EIP_CSAP_H


/****************************************************************************/
/* Defines */
/****************************************************************************/
#define EIP_CSAP_NEW 1
#define EIP_CSAP_CREATE_ASSEMBLY_OBJECT 2
#define EIP_CSAP_ADD_EXCLUSIVE_OWNER_CONNECTION 3
#define EIP_CSAP_ADD_INPUT_ONLY_CONNECTION 4
#define EIP_CSAP_ADD_LISTEN_ONLY_CONNECTION 5
#define EIP_CSAP_DM_DP_ADD 6
#define EIP_CSAP_CFG_VENDOR_ID_SET 7
#define EIP_CSAP_CFG_DEVICE_TYPE_SET 8
#define EIP_CSAP_CFG_PRODUCT_CODE_SET 9
#define EIP_CSAP_CFG_REVISION_SET 10
#define EIP_CSAP_CFG_SERIAL_NUM_SET 11
#define EIP_CSAP_CFG_PRODUCT_NAME_SET 12
#define EIP_CSAP_CFG_DOMAIN_NAME_SET 13
#define EIP_CSAP_CFG_HOST_NAME_SET 14
#define EIP_CSAP_CFG_NUM_EXPLICIT_CON_SET 15
#define EIP_CSAP_CFG_NUM_IMPLICIT_CON_SET 16
#define EIP_CSAP_CFG_ETH_LINK_COUNTERS_ON 17
#define EIP_CSAP_CFG_ETH_LINK_CONTROL_ON 18
#define EIP_CSAP_CFG_CHANGE_ETH_AFTER_RESET_ON 19
#define EIP_CSAP_CFG_CHANGE_IP_AFTER_RESET_ON 20
#define EIP_CSAP_CFG_NUM_SESSIONS_SET 21
#define EIP_CSAP_CFG_TICK_SET 22
#define EIP_CSAP_CFG_O2_T_RUN_IDLE_HEADER_ON 23
#define EIP_CSAP_CFG_T2_O_RUN_IDLE_HEADER_ON 24
#define EIP_CSAP_CFG_QO_S_ON 25
#define EIP_CSAP_CFG_NUM_DELAYED_ENCAP_MSG_SET 26
#define EIP_CSAP_CFG_DHCP_ON 27
#define EIP_CSAP_CFG_DLR_ON 28
#define EIP_CSAP_CFG_ACD_ON 29


/****************************************************************************/
/* Global Prototypes */
/****************************************************************************/
GOAL_STATUS_T eip_csapReg(
    void
);

#endif /* EIP_CSAP_H */
