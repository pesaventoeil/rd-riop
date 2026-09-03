/** @file
 *
 * @brief DLR Configuration File
 *
 * This file configures the DLR stack. It is used to enable certain features.
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

#ifndef DLR_CONFIG_H
#define DLR_CONFIG_H


/****************************************************************************/
/* Configuration Section */
/****************************************************************************/

/* Beacon-based Ring Node
 *
 * This options are used to configure the DLR stack as a Beacon-based Ring Node.
 *
 * - DLR_CONFIG_BEACON_RINGNODE
 *   --> activate support for a Beacon-based Ring Node
 *
 * - DLR_CONFIG_BEACON_HW_SUPPORT
 *   --> activate this, if Beacon Frames are completely processed in hardware
 */
#define DLR_CONFIG_BEACON_RINGNODE 1
#define DLR_CONFIG_BEACON_HW_SUPPORT 1


/* Announce-based Ring Node
 *
 * This options are used to configure the DLR stack as an Announce-based Ring
 * Node.
 *
 * - DLR_CONFIG_ANNOUNCE_RINGNODE
 *   --> activate support for an Announce-based Ring Node
 *
 */
/* #define DLR_CONFIG_ANNOUNCE_RINGNODE 1 */


/* Miscellaneous
 *
 * - DLR_CONFIG_FLUSHTABLES
 *   --> activate the processing of FlushTables Frames
 */
#define DLR_CONFIG_FLUSHTABLES 1


/****************************************************************************/
/* Checking Section */
/****************************************************************************/
#if (DLR_CONFIG_BEACON_RINGNODE == 1) && (DLR_CONFIG_ANNOUNCE_RINGNODE == 1)
# error Only one kind of Ring Node at a time is supported!
#endif

#if (DLR_CONFIG_BEACON_RINGNODE == 0) && (DLR_CONFIG_ANNOUNCE_RINGNODE == 0)
# error Please define the kind of DLR Ring Node!
#endif


#endif /* DLR_CONFIG_H */
