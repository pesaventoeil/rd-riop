/** @file
 *
 * @brief NETC MDIO bus handler
 *
 * This module implements a bus handler for NETC MDIO access.
 *
 * @copyright
 * Copyright 2010-2022 port GmbH.
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

#define GOAL_ID GOAL_ID_TGT
#include <goal_includes.h>
#include "drv/bus/mdio/netc/mdio.h"
#include "fsl_netc_mdio.h"
#include "fsl_clock.h"


/****************************************************************************/
/* Local functions */
/****************************************************************************/
static GOAL_STATUS_T goal_drvBusMdioNetcRead(
    GOAL_BUS_T *pBus,                           /**< bus handle */
    uint32_t addrPhy,                           /**< PHY address */
    uint32_t addrReg,                           /**< register address */
    char *pBuffer,                              /**< read buffer */
    uint32_t len                                /**< read buffer length */
);

static GOAL_STATUS_T goal_drvBusMdioNetcWrite(
    GOAL_BUS_T *pBus,                           /**< bus handle */
    uint32_t addrPhy,                           /**< PHY address */
    uint32_t addrReg,                           /**< register address */
    char *pBuffer,                              /**< write buffer */
    uint32_t len                                /**< write buffer length */
);


/****************************************************************************/
/** Initialize the MDIO bus for the NETC IP core
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_drvBusMdioNetcOpen(
    GOAL_BUS_T *pBus                            /**< bus handle */
)
{
    GOAL_STATUS_T res;                          /* GOAL result */
    netc_mdio_config_t mdioConfig;              /* MDIO configuration */
    status_t resSdk;                            /* SDK result */

    res = goal_memCalloc(&(pBus->pData), sizeof(netc_mdio_handle_t));
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to allocate mdio handle");
    }

    if (GOAL_RES_OK(res)) {
        mdioConfig.isPreambleDisable = false;
        mdioConfig.isNegativeDriven = false;
        mdioConfig.srcClockHz = CLOCK_GetRootClockFreq(kCLOCK_Root_Netc);
        mdioConfig.mdio.type = kNETC_EMdio;
#ifdef RIOP
    	mdioConfig.mdio.port = (netc_hw_eth_port_idx_t)kNETC_ENETC0EthPort;
#endif

        resSdk = NETC_MDIOInit((netc_mdio_handle_t *) pBus->pData, &mdioConfig);
        if (kStatus_Success != resSdk) {
            goal_logErr("failed to initialize mdio bus");
        }
    }

    if (GOAL_RES_OK(res)) {
        pBus->read = goal_drvBusMdioNetcRead;
        pBus->write = goal_drvBusMdioNetcWrite;
    }

    return res;
}


/****************************************************************************/
/** Read a Phy register via the EMDIO module of the NETC IP core
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_drvBusMdioNetcRead(
    GOAL_BUS_T *pBus,                           /**< bus handle */
    uint32_t addrPhy,                           /**< PHY address */
    uint32_t addrReg,                           /**< register address */
    char *pBuffer,                              /**< read buffer */
    uint32_t len                                /**< read buffer length */
)
{
    status_t resSdk;                            /* SDK result */

    UNUSEDARG(len);

    resSdk = NETC_MDIORead((netc_mdio_handle_t *) pBus->pData, (uint8_t) addrPhy,
                           (uint8_t) addrReg, (uint16_t *) pBuffer);

    return (kStatus_Success == resSdk) ? GOAL_OK : GOAL_ERROR;
}


/****************************************************************************/
/** Write a Phy register via the EMDIO module of the NETC IP core
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_drvBusMdioNetcWrite(
    GOAL_BUS_T *pBus,                           /**< bus handle */
    uint32_t addrPhy,                           /**< PHY address */
    uint32_t addrReg,                           /**< register address */
    char *pBuffer,                              /**< write buffer */
    uint32_t len                                /**< write buffer length */
)
{
    status_t resSdk;                            /* SDK result */

    UNUSEDARG(len);

    resSdk = NETC_MDIOWrite((netc_mdio_handle_t *) pBus->pData, (uint8_t) addrPhy,
                            (uint8_t) addrReg, *((uint16_t *) pBuffer));

    return (kStatus_Success == resSdk) ? GOAL_OK : GOAL_ERROR;
}
