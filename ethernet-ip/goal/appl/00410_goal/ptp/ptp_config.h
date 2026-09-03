/** @file
 *
 * @brief PTP stack configuration
 *
 * @details
 * This module configures the PTP stack and sets application specific values.
 *
 * @copyright
 * Copyright 2016-2017.
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

#ifndef PTP_CONFIG_H
#define PTP_CONFIG_H


/****************************************************************************/
/* PTP configuration */
/****************************************************************************/

/* set Number of network ports 
 *
 * Define how many ports take part in the PTP communication.
 * If PTP_CFG_NUM_PORTS is 1, the device is an Ordinary Clock, else the device
 * is a Boundary Clock.
 *
 * valid range: 1..n
*/
#define PTP_CFG_NUM_PORTS GOAL_TARGET_ETH_PORT_COUNT


/* set clock type
 *
 * Define whether this clock is a two-step clock or a one-step clock. A one-step
 * clock sends its egress timestamp with the timestamped frame. This requires
 * special hardware that can alter the sent frame. A two-step clock sends the
 * egress timestamp in a separate frame.
 * If PTP_CFG_ONE_STEP_CLOCK is defined the stack expects the hardware to act as
 * a one-step clock. Otherwise the stack acts as a two-step clock.
 *
 * valid range: 1..n
*/
/* #define PTP_CFG_ONE_STEP_CLOCK */


/* PTP transport layer
 *
 * Define what network layer is used to transport PTP messages.
 * Only one macro can be defined at a time.
 *
 * valid macros:
 * - PTP_CFG_ETHERNET - raw Ethernet
 * - PTP_CFG_UDP_IP_V4 - UDP/IPv4
 * - PTP_CFG_PNIO - PROFINET IO
*/
#define PTP_CFG_ETHERNET

/****************************************************************************/
/* Local clock configuration */
/****************************************************************************/

/* Set clock class
 *
 * - see IEEE1588-2008, chapter 7.6.2.4
 * - valid range: 6..255
 */
#define PTP_CFG_CLK_CLASS 248


/* Set clock accuracy
 *
 * - use one of the accuracy macros in ptp.h
 * - see IEEE1588-2008, chapter 7.6.2.5
 */
#define PTP_CFG_CLK_ACCURACY PTP_CLOCK_ACCURACY_25ns


/* set the clock variance
 *
 * The clock variance can be calculated as defined in IEEE1588-2008,
 * chapter 7.6.3. A value of 0xFFFF means the variance has not been calculated.
 */
#define PTP_CFG_CLK_VARIANCE 0xFFFF


/* Set initial value of priority1
 *
 * The priority1 attribute is used in the best master clock algorithm. This
 * macro defines the initial value. For further details see IEEE1588-2008,
 * chapter 7.6.2.2.
 *
 * NOTE: The PTP profile can put further restrictions on the valid range.
 */
#define PTP_CFG_CLK_PRIO1 128


/* Set initial value of priority2
 *
 * The priority2 attribute is used in the best master clock algorithm. This
 * macro defines the initial value. For further details see IEEE1588-2008,
 * chapter 7.6.2.3.
 *
 * NOTE: The PTP profile can put further restrictions on the valid range.
 */
#define PTP_CFG_CLK_PRIO2 128


/* Set the domain number of the local clock
 *
 * - use one of the PTP_DOAMIN_XXX macros
 * - for details see IEEE1588-2008, chapter 7.1
 * - default value: PTP_DOAMIN_DEFAULT
 *
 * NOTE: The PTP profile can put further restrictions on the valid range.
 */
#define PTP_CFG_DOMAIN PTP_DOAMIN_DEFAULT


/* Synchronization Threshold
 *
 * Define the synchronization threshold in nanoseconds. Only offsets from the
 * master that are above this value will cause a synchronization, i.e. offsets
 * below this value will be tolerated.
 */
#define PTP_CFG_SYNC_THRESHOLD 90000


/****************************************************************************/
/* Message processing and generating */
/****************************************************************************/

/* Announce message interval
 *
 * - set the interval between to Announce messages
 * - value is log2 of interval in seconds
 *   e.g.: 3 -> 2^3 -> 8 seconds interval
 * - set min, max and default values
 * - for details see IEEE1588-2008, chapter 7.7.2.2
 *
 * NOTE: The PTP profile can put further restrictions on the valid range.
 */
#define PTP_CFG_LOGANNOUNCEINT_MIN 0
#define PTP_CFG_LOGANNOUNCEINT_DEF 1
#define PTP_CFG_LOGANNOUNCEINT_MAX 4


/* Announce Receipt timeout
 *
 * - set timeout value for announce message reception
 * - value is integral multiple of announceInterval
 *   e.g.: 3 -> timeout = 3 * announceInterval
 * - set min, max and default values
 * - for details see IEEE1588-2008, chapter 7.7.3.1
 *
 * NOTE: The PTP profile can put further restrictions on the valid range.
 */
#define PTP_CFG_ANNOUNCERXTIMEOUT_MIN 2
#define PTP_CFG_ANNOUNCERXTIMEOUT_DEF 3
#define PTP_CFG_ANNOUNCERXTIMEOUT_MAX 10


/* Sync message interval
 *
 * - set the interval between to Sync messages
 * - value is log2 of interval in seconds
 *   e.g.: 3 -> 2^3 -> 8 seconds interval
 * - set min, max and default values
 * - for details see IEEE1588-2008, chapter 7.7.2.3
 *
 * NOTE: The PTP profile can put further restrictions on the valid range.
 */
#define PTP_CFG_LOGSYNCINT_MIN -1
#define PTP_CFG_LOGSYNCINT_DEF 0
#define PTP_CFG_LOGSYNCINT_MAX 1


/* PDelay message interval
 *
 * - set the interval between to Sync messages
 * - value is log2 of interval in seconds
 *   e.g.: 3 -> 2^3 -> 8 seconds interval
 * - set default values
 *
 * NOTE: The PTP profile can put further restrictions on the valid range.
 */
#define PTP_CFG_LOGPDELAYINT_DEF -1


/****************************************************************************/
/* Device Properties */
/****************************************************************************/

/* Manufacturer ID
 *
 * This must be an OUI of the manufacturer represented as a 3 byte array.
 * An OUI is assigned by the IEEE.
 * --> e.g. the first three octets of a MAC address
 */
#define PTP_CFG_MANU_ID {0xAA, 0xBB, 0xCC}


/* Product Description
 *
 * Define the Company Name and a device name.
 * Both strings together must be smaller than 46 bytes.
 */
#define PTP_CFG_VENDOR "Generic Vendor"
#define PTP_CFG_DEVICE "PTP Demo Device"


/* Revision
 *
 * Define the version of Hardware, Firmware and Software.
 * All three strings combined must be smaller than 31 bytes.
 */
#define PTP_CFG_V_HW "1.0.0"
#define PTP_CFG_V_FW "1.0.0"
#define PTP_CFG_V_SW "1.0.0"


/* User Description
 *
 * Define the initial value of the User Description. The content can be
 * overwritten by the user. The string has a maximum size of 128 bytes.
 * It contains a user defined name for the device followed by a semicolon (;)
 * and a description of the location. Either of both values can be empty.
 */
#define PTP_CFG_USR_DESCRIPTION "ptpDemo; Somewhere"


/* Profile Identity
 *
 * This macro contains the identifier of the PTP Profile implemented in this
 * device. It is represented as a 6 byte array.
 */
#define PTP_CFG_PROFILE {0x00, 0x1B, 0x19, 0x00, 0x01, 0x00}


#endif /* PTP_CONFIG_H */
