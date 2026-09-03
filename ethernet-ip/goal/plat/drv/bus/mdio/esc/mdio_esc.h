/** @file
 *
 * @brief ESC MDIO bus handler
 *
 * This module implements a bus handler for MDIO access via an EtherCAT Slave
 * Controller.
 *
 * @copyright
 * Copyright 2010-2019.
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

#ifndef MDIO_ESC_H
#define MDIO_ESC_H


/****************************************************************************/
/* prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_drvBusMdioEscOpen(
    GOAL_BUS_T *pBus,                           /**< bus handle */
    GOAL_ECAT_T *pEcat                          /**< EtherCAT instance handle */
);


#endif /* MDIO_ESC_H */
