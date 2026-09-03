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

#define GOAL_ID GOAL_ID_TGT
#include <goal_includes.h>
#include "goal_ecat.h"
#include "drv/bus/mdio/esc/mdio_esc.h"


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/
static GOAL_STATUS_T goal_drvBusMdioEscRead(
    GOAL_BUS_T *pBus,                           /**< bus handle */
    uint32_t addrPhy,                           /**< PHY address */
    uint32_t addrReg,                           /**< register address */
    char *pBuffer,                              /**< read buffer */
    uint32_t len                                /**< read buffer length */
);

static GOAL_STATUS_T goal_drvBusMdioEscWrite(
    GOAL_BUS_T *pBus,                           /**< bus handle */
    uint32_t addrPhy,                           /**< PHY address */
    uint32_t addrReg,                           /**< register address */
    char *pBuffer,                              /**< write buffer */
    uint32_t len                                /**< write buffer length */
);


/****************************************************************************/
/** Initializes the MDIO bus for an EtherCAT Slave Controller
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_drvBusMdioEscOpen(
    GOAL_BUS_T *pBus,                           /**< bus handle */
    GOAL_ECAT_T *pEcat                          /**< EtherCAT instance handle */
)
{
    /* assign bus functions */
    pBus->read = goal_drvBusMdioEscRead;
    pBus->write = goal_drvBusMdioEscWrite;
    pBus->pData = pEcat;

    return GOAL_OK;
}


/****************************************************************************/
/** Read a Phy register via ESC registers
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_drvBusMdioEscRead(
    GOAL_BUS_T *pBus,                           /**< bus handle */
    uint32_t addr,                              /**< read address */
    uint32_t reg,                               /**< register */
    char *pVal,                                 /**< buffer address */
    uint32_t len                                /**< read length */
)
{
    EC_RET_T res;                               /* result */
    uint16_t *pValU16;                          /* U16 pointer */
    EC_INSTANCE_T *pEcat;                       /* ECAT instance data */

    UNUSEDARG(len);

    if (!pVal) {
        /* GG_LOG ID:349: */
        goal_lmLog(GOAL_ID_DRV_MDIO, 349, 297, 0, 0, GOAL_LOG_SEV_ERROR, "value points to null");
        return GOAL_ERR_NULL_POINTER;
    }

    pValU16 = (uint16_t *) pVal;
    pEcat = (EC_INSTANCE_T *) pBus->pData;

    if (NULL == pEcat) {
        return GOAL_ERR_WRONG_STATE;
    }

    res = ec_phyRead(pEcat, (uint8_t) addr, (uint8_t) reg, pValU16);

    return (RET_OK == res) ? GOAL_OK : GOAL_ERROR;
}


/****************************************************************************/
/** Write a Phy register via ESC registers
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_drvBusMdioEscWrite(
    GOAL_BUS_T *pBus,                           /**< bus handle */
    uint32_t addr,                              /**< write address */
    uint32_t reg,                               /**< register */
    char *pVal,                                 /**< buffer address */
    uint32_t len                                /**< write length */
)
{
    EC_RET_T res;                               /* result */
    uint16_t *pValU16;                          /* U16 pointer */
    EC_INSTANCE_T *pEcat;                       /* ECAT instance data */

    UNUSEDARG(len);

    if (!pVal) {
        /* GG_LOG ID:349: */
        goal_lmLog(GOAL_ID_DRV_MDIO, 349, 297, 0, 0, GOAL_LOG_SEV_ERROR, "value points to null");
        return GOAL_ERR_NULL_POINTER;
    }

    pValU16 = (uint16_t *) pVal;
    pEcat = (EC_INSTANCE_T *) pBus->pData;

    if (NULL == pEcat) {
        return GOAL_ERR_WRONG_STATE;
    }

    res = ec_phyWrite(pEcat, (uint8_t) addr, (uint8_t) reg, *pValU16);

    return (RET_OK == res) ? GOAL_OK : GOAL_ERROR;
}
