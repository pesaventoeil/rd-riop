/** @file
 *
 * @brief Ethernet/IP user configuration
 *
 * This file lists the general Ethernet/IP configuration.
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

#ifndef EIP_USER_CONF_H
#define EIP_USER_CONF_H


/****************************************************************************/
/* Defines */
/****************************************************************************/
/**< Identity configuration of the device */
#define EIP_DEVICE_VENDOR_ID          1114
#define EIP_DEVICE_TYPE               12
#define EIP_DEVICE_PRODUCT_CODE       0001
#define EIP_DEVICE_MAJOR_REVISION     1
#define EIP_DEVICE_MINOR_REVISION     1
#define EIP_DEVICE_NAME               "EtherNet/IP ComCore Device"
#define EIP_DEVICE_DOMAINNAME         "test.com"
#define EIP_DEVICE_HOSTNAME           "eipdevice"

#endif /* EIP_USER_CONF_H */
