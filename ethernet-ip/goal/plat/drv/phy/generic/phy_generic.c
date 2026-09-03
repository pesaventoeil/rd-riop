/** @file
 *
 * @brief Generic PHY Driver
 *
 * @copyright
 * Copyright 2010-2017.
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
#include <drv/phy/generic/phy_generic.h>
#include <drv/phy/generic/phy_generic_int.h>


/****************************************************************************/
/* Local variables */
/****************************************************************************/
/* register enums */
#undef GOAL_PHY_GENERIC_RV
#define GOAL_PHY_GENERIC_RV(reg, val, shift, mask, flags) val
typedef enum {
    GOAL_PHY_GENERIC_RV_LIST,
    GOAL_PHY_GENERIC_VAL_END
} GOAL_PHY_GENERIC_REG_ID_T;


/* register entries */
#undef GOAL_PHY_GENERIC_RV
#define GOAL_PHY_GENERIC_RV(reg, val, shift, mask, flags) { reg, shift, mask, flags }
static GOAL_REG_T regTable[] = {
    GOAL_PHY_GENERIC_RV_LIST,
};


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/
static GOAL_STATUS_T regGet(
    struct GOAL_ETH_IFACE_T *pIface,            /**< interface data */
    GOAL_PHY_GENERIC_REG_ID_T reg,              /**< register */
    uint32_t *pVal                              /**< value ptr */
);

static GOAL_STATUS_T regSet(
    struct GOAL_ETH_IFACE_T *pIface,            /**< interface data */
    GOAL_PHY_GENERIC_REG_ID_T reg,              /**< register */
    uint32_t val                                /**< value */
);

static GOAL_STATUS_T cmd(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T linkState(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T portState(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T autoNegState(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T hwFaultGet(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T speedSet(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T duplexSet(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T capabilitiesGet(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T autoNegProgress(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T advertisement(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T genericRegSet(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T genericRegGet(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);


/****************************************************************************/
/** Open PHY interface
 *
 * @retval GOAL_OK success
 * @retval others fail
 */
GOAL_STATUS_T goal_targetPhyGenericOpen(
    GOAL_ETH_IFACE_T *pIface,                   /**< interface data */
    uint32_t maxSpeed                           /**< maximum speed of Phy in Mbit/s */
)
{
    GOAL_STATUS_T res;                          /* result */
    uint32_t idPhy;                             /* phy id */
    uint32_t *pMaxSpeed;                        /* maximum speed of Phy */

    if (!pIface) {
        /* GG_LOG ID:448: */
        goal_lmLog(GOAL_ID_DRV_PHY, 448, 371, 0, 0, GOAL_LOG_SEV_ERROR, "interface data is null");
        return GOAL_ERR_NULL_POINTER;
    }

    /* allocate Generic Phy data */
    res = goal_memCalloc(&pMaxSpeed, sizeof(uint32_t));
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    *pMaxSpeed = maxSpeed;

    /* read PHY ID */
    res = goal_targetPhyGenericId(&pIface->bus, pIface->addr, &idPhy);
    if (GOAL_OK != res) {
         return res;
    }

    /* GG_LOG ID:449: */
    goal_lmLog(GOAL_ID_DRV_PHY, 449, 372, 4, 4, GOAL_LOG_SEV_INFO, "PHY $1 found at address $2");
    goal_lmLogParamUINT32(idPhy);
    goal_lmLogParamUINT32(pIface->addr);
    goal_lmLogFinish();

    /* assign interface data */
    pIface->pCmd = cmd;
    pIface->pData = pMaxSpeed;

    goal_logInfo("phy register access index starts at %"FMT_u32".", (uint32_t) GOAL_PHY_GENERIC_REG_00);

    return GOAL_OK;
}


/****************************************************************************/
/** Close PHY interface
 *
 * @retval GOAL_OK success
 * @retval others fail
 */
GOAL_STATUS_T goal_targetPhyGenericClose(
    GOAL_ETH_IFACE_T *pIface                    /**< interface data */
)
{
    UNUSEDARG(pIface);
    return GOAL_OK;
}


/****************************************************************************/
/** Read PHY ID
 *
 * @retval GOAL_OK success
 * @retval others fail
 */
GOAL_STATUS_T goal_targetPhyGenericId(
    GOAL_BUS_T *pBus,                           /**< PHY bus */
    uint32_t addr,                              /**< bus address */
    uint32_t *pIdPhy                            /**< PHY id ref */
)
{
    GOAL_STATUS_T res;                          /* result */
    uint32_t id1;                               /* PHY identifier 1 */
    uint32_t id2;                               /* PHY identifier 2 */

    /* read PHY ID */
    res = goal_regGet(regTable, pBus, addr, GOAL_PHY_GENERIC_REG_ID1, &id1);
    if (GOAL_OK != res) {
        goal_logDbg("error while reading PHY identifier 1 at address: %"FMT_u32" ", addr);
        return res;
    }

    res = goal_regGet(regTable, pBus, addr, GOAL_PHY_GENERIC_REG_ID2, &id2);
    if (GOAL_OK != res) {
        goal_logDbg("error while reading PHY identifier 2 at address: %"FMT_u32" ", addr);
        return res;
    }

    /* calculate OUI if requested */
    if (pIdPhy) {
        *pIdPhy = (id1 << GOAL_PHY_GENERIC_ID1_SHIFT) & GOAL_PHY_GENERIC_ID1_MASK;
        *pIdPhy |= (id2 & GOAL_PHY_GENERIC_ID2_MASK);
    }

    return GOAL_OK;
}


#if GOAL_CONFIG_PHY_DETECTION == 1
/****************************************************************************/
/** PHY detection
 *
 * @retval GOAL_OK success
 * @retval others fail
 */
GOAL_STATUS_T goal_targetPhyGenericDetect(
    GOAL_ETH_IFACE_T **ppIface,                 /**< interface ptr ref */
    uint32_t *pNumPhys,                         /**< number of found PHYs */
    GOAL_BUS_T *pBus,                           /**< bus handle */
    uint32_t addrMin,                           /**< start at address */
    uint32_t addrMax                            /**< end at address */
)
{
    GOAL_STATUS_T res;                          /* result */
    uint32_t idPhy;                             /* PHY id */
    unsigned int cntPhy = 0;                    /* PHY data index */
    unsigned int cntPhyDrv = 0;                 /* PHY driver index */
    unsigned int cntPhyFound = 0;               /* found PHY count */
    uint32_t addr;                              /* PHY addr */

    /* find out PHY count */
    for (addr = addrMin; addr < addrMax; addr++) {
        res = goal_targetPhyGenericId(pBus, addr, NULL);
        if (GOAL_RES_OK(res)) {
            cntPhyFound++;
            goal_logDbg("found PHY identifier at bus address: %"FMT_u32" ", addr);
        }
    }

    /* leave if no PHY was found */
    if (!cntPhyFound) {
        /* GG_LOG ID:450: */
        goal_lmLog(GOAL_ID_DRV_PHY, 450, 373, 0, 0, GOAL_LOG_SEV_INFO, "no PHYs found");
        return GOAL_ERR_NOT_FOUND;
    }

    goal_logDbg("found %"FMT_u32" identifier in total", addr);

    /* allocate PHY management structure */
    res = goal_memCalloc(ppIface, cntPhyFound * sizeof(GOAL_ETH_IFACE_T));
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:451: */
        goal_lmLog(GOAL_ID_DRV_PHY, 451, 289, 0, 0, GOAL_LOG_SEV_ERROR, "out of memory: allocating PHY data structures failed");
        return res;
    }

    /* initialize PHYs */
    for (addr = addrMin; (cntPhy < cntPhyFound) && (addr < addrMax); addr++) {

        /* request PHY id */
        res = goal_targetPhyGenericId(pBus, addr, &idPhy);
        if (GOAL_RES_ERR(res)) {
            continue;
        }

        /* setup specific interface data */
        (*ppIface)[cntPhy].bus = *pBus;
        (*ppIface)[cntPhy].addr = addr;

        /* try to find and initialize specific PHY driver */
        for (cntPhyDrv = 0; cntPhyDrv < ARRAY_ELEMENTS(phyList); cntPhyDrv++) {
            if (phyList[cntPhyDrv].idPhy == idPhy) {
                res = phyList[cntPhyDrv].funcInit(&(*ppIface)[cntPhy]);
                break;
            }
        }

        /* if no driver was detected use generic PHY driver */
        if (GOAL_RES_ERR(res) || (ARRAY_ELEMENTS(phyList) <= cntPhyDrv)) {
            res = goal_targetPhyGenericOpen(&(*ppIface)[cntPhy], GOAL_ETH_SPEED_100);

            if (GOAL_RES_ERR(res)) {
                /* GG_LOG ID:452: */
                goal_lmLog(GOAL_ID_DRV_PHY, 452, 374, 4, 0, GOAL_LOG_SEV_ERROR, "failed to initialize PHY id $1 with generic PHY driver");
                goal_lmLogParamUINT32(idPhy);
                goal_lmLogFinish();
                break;
            }
        }

        /* increase PHY count */
        cntPhy++;
    }

    /* return PHYs to caller */
    if (GOAL_RES_OK(res)) {
        *pNumPhys = cntPhyFound;
    }

    return res;
}
#endif /* GOAL_CONFIG_PHY_DETECTION == 1 */


/****************************************************************************/
/** Get register value
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
static GOAL_STATUS_T regGet(
    GOAL_ETH_IFACE_T *pIface,                   /**< interface data */
    GOAL_PHY_GENERIC_REG_ID_T reg,              /**< register */
    uint32_t *pVal                              /**< value ptr */
)
{
    /* read PHY register */
    return goal_regGet(regTable, &pIface->bus, pIface->addr, reg, pVal);
}


/****************************************************************************/
/** Set register value
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
static GOAL_STATUS_T regSet(
    GOAL_ETH_IFACE_T *pIface,                   /**< interface data */
    GOAL_PHY_GENERIC_REG_ID_T reg,              /**< register */
    uint32_t val                                /**< value */
)
{
    /* write PHY register */
    return goal_regSet(regTable, &pIface->bus, pIface->addr, reg, val);
}


/****************************************************************************/
/** Generic command handler
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
static GOAL_STATUS_T cmd(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    switch (pCmd->cmd) {
        case GOAL_ETH_CMD_PORT_COUNT:
            *pCmd->data.pCntPorts = GOAL_PHY_GENERIC_PORT_COUNT;
            return GOAL_OK;

        case GOAL_ETH_CMD_SPEED:
            if (!pCmd->flgSet) {
                return GOAL_ERR_UNSUPPORTED;
            }
            return speedSet(pCmd);

        case GOAL_ETH_CMD_SPEED_MAX:
            if (pCmd->flgSet) {
                return GOAL_ERR_UNSUPPORTED;
            }
            *pCmd->data.pSpeed = *((uint32_t *) pCmd->pIface->pData);
            return GOAL_OK;

        case GOAL_ETH_CMD_DUPLEX:
            return duplexSet(pCmd);

        case GOAL_ETH_CMD_LINK_STATE:
            return linkState(pCmd);

        case GOAL_ETH_CMD_PORT_STATE:
            return portState(pCmd);

        case GOAL_ETH_CMD_AUTONEG:
            return autoNegState(pCmd);

        case GOAL_ETH_CMD_HW_FAULT:
            return hwFaultGet(pCmd);

        case GOAL_ETH_CMD_AUTONEG_PROGRESS:
            if (pCmd->flgSet) {
                return GOAL_ERR_UNSUPPORTED;
            }
            return autoNegProgress(pCmd);

        case GOAL_ETH_CMD_LINK_CAPABILITIES:
            if (pCmd->flgSet) {
                return GOAL_ERR_UNSUPPORTED;
            }
            return capabilitiesGet(pCmd);

        case GOAL_ETH_CMD_AUTONEG_ADVERTISEMENT:
            return advertisement(pCmd);

        case GOAL_ETH_CMD_AUTONEG_RESTART:
            return regSet(pCmd->pIface, GOAL_PHY_GENERIC_REG_AUTONEG_RESTART,
                          GOAL_PHY_GENERIC_AUTONEG_RESTART);

        case GOAL_ETH_CMD_PHY_REG_SET:
            return genericRegSet(pCmd);

        case GOAL_ETH_CMD_PHY_REG_GET:
            return genericRegGet(pCmd);

        default:
            return GOAL_ERR_UNSUPPORTED;
    }
}


/****************************************************************************/
/** Get link state
 *
 * @retval GOAL_OK success
 * @retval others fail
 */
static GOAL_STATUS_T linkState(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    GOAL_STATUS_T res;                          /* result */
    uint32_t state;                             /* state tmp */

    /* get state */
    if (GOAL_FALSE == pCmd->flgSet) {
        res = regGet(pCmd->pIface, GOAL_PHY_GENERIC_REG_LINK_STATUS, &state);
        if (GOAL_OK != res) {
            return res;
        }

        *pCmd->data.pStateLink = (state) ? GOAL_ETH_STATE_UP : GOAL_ETH_STATE_DOWN;
        return GOAL_OK;
    }

    /* set not supported */
    return GOAL_ERR_ACCESS;
}


/****************************************************************************/
/** Get/set port state
 *
 * @retval GOAL_OK success
 * @retval others fail
 */
static GOAL_STATUS_T portState(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    GOAL_STATUS_T res;                          /* result */
    uint32_t state;                             /* state tmp */

    res = regGet(pCmd->pIface, GOAL_PHY_GENERIC_REG_PWR_DWN, &state);
    if (GOAL_OK != res) {
        return res;
    }

    /* translate value to GOAL enum */
    state = (state) ? GOAL_ETH_STATE_DOWN : GOAL_ETH_STATE_UP;

    /* get state */
    if (GOAL_FALSE == pCmd->flgSet) {
        /* report current state */
        *pCmd->data.pStatePort = state;
        return GOAL_OK;
    }

    /* set state */
    if (((GOAL_ETH_STATE_UP == state) && (GOAL_ETH_STATE_DEFAULT == *pCmd->data.pStatePort)) ||
        (state == *pCmd->data.pStatePort)) {
            /* requested state is already set */
            return GOAL_OK;
    }

    /* apply new state */
    if ((GOAL_ETH_STATE_DEFAULT == *pCmd->data.pStatePort) ||
        (GOAL_ETH_STATE_UP == *pCmd->data.pStatePort)) {
        state = GOAL_PHY_GENERIC_POWER_NORMAL;
    } else {
        state = GOAL_PHY_GENERIC_POWER_DOWN;
    }

    return regSet(pCmd->pIface, GOAL_PHY_GENERIC_REG_PWR_DWN, state);
}


/****************************************************************************/
/** Get/set Autonegotiation Status
 *
 * @retval GOAL_OK success
 * @retval others fail
 */
static GOAL_STATUS_T autoNegState(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    GOAL_STATUS_T res;                          /* result */
    uint32_t state;                             /* state tmp */

    res = regGet(pCmd->pIface, GOAL_PHY_GENERIC_REG_AUTONEG_ENBL, &state);
    if (GOAL_OK != res) {
        return res;
    }

    /* translate value to GOAL enum */
    state = (GOAL_PHY_GENERIC_AUTONEG_ENABLE == state) ? GOAL_ETH_AUTONEG_ON
                                                       : GOAL_ETH_AUTONEG_OFF;

    /* get state */
    if (GOAL_FALSE == pCmd->flgSet) {
        /* report current state */
        *pCmd->data.pStateAutoNeg = state;
        return GOAL_OK;
    }

    /* set state */
    if (((GOAL_ETH_AUTONEG_ON == state) && (GOAL_ETH_AUTONEG_DEFAULT == *pCmd->data.pStateAutoNeg)) ||
        (state == *pCmd->data.pStateAutoNeg)) {
            /* requested state is already set */
            return GOAL_OK;
    }

    if ((GOAL_ETH_AUTONEG_DEFAULT == *pCmd->data.pStateAutoNeg) ||
        (GOAL_ETH_AUTONEG_ON == *pCmd->data.pStateAutoNeg)) {
        state = GOAL_PHY_GENERIC_AUTONEG_ENABLE;
    } else {
        state = GOAL_PHY_GENERIC_AUTONEG_DISABLE;
    }

    return regSet(pCmd->pIface, GOAL_PHY_GENERIC_REG_AUTONEG_ENBL, state);
}


/****************************************************************************/
/** Get hardware fault indicator
 *
 * @retval GOAL_OK success
 * @retval others fail
 */
static GOAL_STATUS_T hwFaultGet(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    GOAL_STATUS_T res;                          /* result */

    res = regGet(pCmd->pIface, GOAL_PHY_GENERIC_REG_JABBER, pCmd->data.pStateHwFault);
    if (GOAL_OK != res) {
        return res;
    }

    if (*pCmd->data.pStateHwFault) {
        return GOAL_OK;
    }

    res = regGet(pCmd->pIface, GOAL_PHY_GENERIC_REG_REMOTE_FAULT, pCmd->data.pStateHwFault);
    if (GOAL_OK != res) {
        return res;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Set link speed
 *
 * @retval GOAL_OK success
 * @retval others fail
 */
static GOAL_STATUS_T speedSet(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    uint32_t speed;                             /* speed */
    GOAL_STATUS_T res;                          /* result */

    /* get current speed setting (if AutoNeg is on this is ignored) */
    res = regGet(pCmd->pIface, GOAL_PHY_GENERIC_REG_SPEED_SELECT, &speed);
    if (GOAL_OK != res) {
        return res;
    }

    /* translate value to GOAL enum */
    speed = (GOAL_PHY_GENERIC_SPEED_10 == speed) ? GOAL_ETH_SPEED_10
            : (GOAL_PHY_GENERIC_SPEED_100 == speed) ? GOAL_ETH_SPEED_100
            : (GOAL_PHY_GENERIC_SPEED_1000 == speed) ? GOAL_ETH_SPEED_1000
            : GOAL_ETH_SPEED_UNKNOWN;

    /* set speed */
    if (((GOAL_ETH_SPEED_100 == speed) && (GOAL_ETH_SPEED_DEFAULT == *pCmd->data.pSpeed)) ||
        (speed == *pCmd->data.pSpeed)) {
            /* requested speed is already set */
            return GOAL_OK;
    }

    /* apply new speed */
    if ((GOAL_ETH_SPEED_DEFAULT == *pCmd->data.pSpeed) ||
        (GOAL_ETH_SPEED_100 == *pCmd->data.pSpeed)) {
        speed = GOAL_PHY_GENERIC_SPEED_100;
    }
    else if (GOAL_ETH_SPEED_10 == *pCmd->data.pSpeed) {
        speed = GOAL_PHY_GENERIC_SPEED_10;
    }
    else if ((GOAL_ETH_SPEED_1000 == *pCmd->data.pSpeed) &&
             (GOAL_ETH_SPEED_1000 == *((uint32_t *) pCmd->pIface->pData))) {
            speed = GOAL_PHY_GENERIC_SPEED_1000;
    }
    else {
        /* GG_LOG ID:453: */
        goal_lmLog(GOAL_ID_DRV_PHY, 453, 375, 0, 0, GOAL_LOG_SEV_ERROR, "unknown speed setting");
        return GOAL_ERROR;
    }

    return regSet(pCmd->pIface, GOAL_PHY_GENERIC_REG_SPEED_SELECT, speed);
}


/****************************************************************************/
/** Set link duplex
 *
 * @retval GOAL_OK success
 * @retval others fail
 */
static GOAL_STATUS_T duplexSet(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    uint32_t duplex;                            /* duplex tmp */
    GOAL_STATUS_T res;                          /* result */

    /* get current duplex mode (if AutoNeg is on this is ignored) */
    res = regGet(pCmd->pIface, GOAL_PHY_GENERIC_REG_DUPLEX_MODE, &duplex);
    if (GOAL_OK != res) {
        return res;
    }

    /* translate value to GOAL enum */
    duplex = (GOAL_PHY_GENERIC_DUPLEX_HALF == duplex) ? GOAL_ETH_DUPLEX_HALF
             : (GOAL_PHY_GENERIC_DUPLEX_FULL == duplex) ? GOAL_ETH_DUPLEX_FULL
             : GOAL_ETH_DUPLEX_UNKNOWN;

    /* set duplex */
    if (((GOAL_ETH_DUPLEX_FULL == duplex) && (GOAL_ETH_DUPLEX_DEFAULT == *pCmd->data.pModeDuplex)) ||
        (duplex == *pCmd->data.pModeDuplex)) {
            /* requested duplex mode is already set */
            return GOAL_OK;
    }

    /* apply new duplex mode */
    if ((GOAL_ETH_DUPLEX_DEFAULT == *pCmd->data.pModeDuplex) ||
        (GOAL_ETH_DUPLEX_FULL == *pCmd->data.pModeDuplex)) {
        duplex = GOAL_PHY_GENERIC_DUPLEX_FULL;
    }
    else if (GOAL_ETH_DUPLEX_HALF == *pCmd->data.pModeDuplex) {
        duplex = GOAL_PHY_GENERIC_DUPLEX_HALF;
    }
    else {
        /* GG_LOG ID:454: */
        goal_lmLog(GOAL_ID_DRV_PHY, 454, 376, 0, 0, GOAL_LOG_SEV_ERROR, "unknown duplex mode");
        return GOAL_ERROR;
    }

    return regSet(pCmd->pIface, GOAL_PHY_GENERIC_REG_DUPLEX_MODE, duplex);
}


/****************************************************************************/
/** Get the supported combinations of speeds and duplex modes
 *
 * @retval GOAL_OK success
 * @retval others fail
 */
static GOAL_STATUS_T capabilitiesGet(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    GOAL_STATUS_T res;                          /* result */
    uint32_t capa;                              /* capability */

    *pCmd->data.pCapa = 0;

    res = regGet(pCmd->pIface, GOAL_PHY_GENERIC_REG_CAPABL_10HD, &capa);
    if (GOAL_RES_ERR(res)) {
        return res;
    }
    if (0 != capa) {
        *pCmd->data.pCapa |= GOAL_ETH_CAPABL_10HD;
    }

    res = regGet(pCmd->pIface, GOAL_PHY_GENERIC_REG_CAPABL_10FD, &capa);
    if (GOAL_RES_ERR(res)) {
        return res;
    }
    if (0 != capa) {
        *pCmd->data.pCapa |= GOAL_ETH_CAPABL_10FD;
    }

    res = regGet(pCmd->pIface, GOAL_PHY_GENERIC_REG_CAPABL_100HD, &capa);
    if (GOAL_RES_ERR(res)) {
        return res;
    }
    if (0 != capa) {
        *pCmd->data.pCapa |= GOAL_ETH_CAPABL_100HD;
    }

    res = regGet(pCmd->pIface, GOAL_PHY_GENERIC_REG_CAPABL_100FD, &capa);
    if (GOAL_RES_ERR(res)) {
        return res;
    }
    if (0 != capa) {
        *pCmd->data.pCapa |= GOAL_ETH_CAPABL_100FD;
    }

    if (GOAL_ETH_SPEED_1000 == *((uint32_t *) pCmd->pIface->pData)) {
        res = regGet(pCmd->pIface, GOAL_PHY_GENERIC_REG_CAPABL_1000HD, &capa);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
        if (0 != capa) {
            *pCmd->data.pCapa |= GOAL_ETH_CAPABL_1000HD;
        }

        res = regGet(pCmd->pIface, GOAL_PHY_GENERIC_REG_CAPABL_1000FD, &capa);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
        if (0 != capa) {
            *pCmd->data.pCapa |= GOAL_ETH_CAPABL_1000FD;
        }
    }

    return GOAL_OK;
}


/****************************************************************************/
/** Get Autonegotiation Progress
 *
 * @retval GOAL_OK success
 * @retval others fail
 */
static GOAL_STATUS_T autoNegProgress(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    GOAL_STATUS_T res;                          /* result */
    uint32_t state;                             /* progress state tmp */

    res = regGet(pCmd->pIface, GOAL_PHY_GENERIC_REG_AUTONEG_ENBL, &state);
    if (GOAL_RES_ERR(res)) {
        return res;
    }
    if (GOAL_PHY_GENERIC_AUTONEG_ENABLE != state) {
        *pCmd->data.pStateAutoNegProgress = GOAL_ETH_AUTONEG_SKIPPED;
        return GOAL_OK;
    }

    res = regGet(pCmd->pIface, GOAL_PHY_GENERIC_REG_LP_AN_ENABLE, &state);
    if (GOAL_RES_ERR(res)) {
        return res;
    }
    if (GOAL_PHY_GENERIC_LP_AN_ENABLED != state) {
        /* remote endpoint forced settings */
        res = regGet(pCmd->pIface, GOAL_PHY_GENERIC_REG_PARALLEL_FAULT, &state);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
        if (GOAL_PHY_GENERIC_PARALLEL_FAULT != state) {
            *pCmd->data.pStateAutoNegProgress = GOAL_ETH_AUTONEG_FAIL_DUPLEX;
            return GOAL_OK;
        }
        else {
            *pCmd->data.pStateAutoNegProgress = GOAL_ETH_AUTONEG_FAIL_ALL;
            return GOAL_OK;
        }
    }
    else {
        /* both endpoints support AutoNeg */
        res = regGet(pCmd->pIface, GOAL_PHY_GENERIC_REG_AUTONEG_CMPLT, &state);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
        if (GOAL_PHY_GENERIC_AUTONEG_COMPLETE != state) {
            *pCmd->data.pStateAutoNegProgress = GOAL_ETH_AUTONEG_INPROGRESS;
            return GOAL_OK;
        }
        else {
            *pCmd->data.pStateAutoNegProgress = GOAL_ETH_AUTONEG_DONE;
            return GOAL_OK;
        }
    }
}


/****************************************************************************/
/** Get & Set advertised speeds and duplex modes
 *
 * @retval GOAL_OK success
 * @retval others fail
 */
static GOAL_STATUS_T advertisement(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    GOAL_STATUS_T res;                          /* result */
    uint32_t advertised;                        /* is advertised */
    uint32_t modeAdv = 0;                       /* currently advertised modes */
    uint32_t diffAdv;                           /* different advertisement settings */

    /* 10 Mbit/s HD */
    res = regGet(pCmd->pIface, GOAL_PHY_GENERIC_REG_AN_AD_10_HD, &advertised);
    if (GOAL_RES_ERR(res)) {
        return res;
    }
    if (GOAL_PHY_GENERIC_AN_AD_ON == advertised) {
        modeAdv |= GOAL_ETH_AUTONEG_AD_10_HD;
    }

    /* 10 Mbit/s FD */
    res = regGet(pCmd->pIface, GOAL_PHY_GENERIC_REG_AN_AD_10_FD, &advertised);
    if (GOAL_RES_ERR(res)) {
        return res;
    }
    if (GOAL_PHY_GENERIC_AN_AD_ON == advertised) {
        modeAdv |= GOAL_ETH_AUTONEG_AD_10_FD;
    }

    /* 100 Mbit/s HD */
    res = regGet(pCmd->pIface, GOAL_PHY_GENERIC_REG_AN_AD_100_HD, &advertised);
    if (GOAL_RES_ERR(res)) {
        return res;
    }
    if (GOAL_PHY_GENERIC_AN_AD_ON == advertised) {
        modeAdv |= GOAL_ETH_AUTONEG_AD_100_HD;
    }

    /* 100 Mbit/s FD */
    res = regGet(pCmd->pIface, GOAL_PHY_GENERIC_REG_AN_AD_100_FD, &advertised);
    if (GOAL_RES_ERR(res)) {
        return res;
    }
    if (GOAL_PHY_GENERIC_AN_AD_ON == advertised) {
        modeAdv |= GOAL_ETH_AUTONEG_AD_100_FD;
    }

    if (GOAL_ETH_SPEED_1000 == *((uint32_t *) pCmd->pIface->pData)) {
        /* 1000 Mbit/s HD */
        res = regGet(pCmd->pIface, GOAL_PHY_GENERIC_REG_AN_AD_1000_HD, &advertised);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
        if (GOAL_PHY_GENERIC_AN_AD_ON == advertised) {
            modeAdv |= GOAL_ETH_AUTONEG_AD_1000_HD;
        }

        /* 1000 Mbit/s FD */
        res = regGet(pCmd->pIface, GOAL_PHY_GENERIC_REG_AN_AD_1000_FD, &advertised);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
        if (GOAL_PHY_GENERIC_AN_AD_ON == advertised) {
            modeAdv |= GOAL_ETH_AUTONEG_AD_1000_FD;
        }
    }


    if (GOAL_FALSE == pCmd->flgSet) {
        /* get currently advertised modes */
        *pCmd->data.pModeAdv = modeAdv;
        return GOAL_OK;
    }

    /* set modes to be advertised */

    /* check what must be changed */
    diffAdv = modeAdv ^ *pCmd->data.pModeAdv;

    if (diffAdv & GOAL_ETH_AUTONEG_AD_10_HD) {
        /* 10 Mbit/s HD */
        res = regSet(pCmd->pIface, GOAL_PHY_GENERIC_REG_AN_AD_10_HD,
                     (*pCmd->data.pModeAdv & GOAL_ETH_AUTONEG_AD_10_HD) ? GOAL_PHY_GENERIC_AN_AD_ON
                                                                        : GOAL_PHY_GENERIC_AN_AD_OFF);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
    }

    if (diffAdv & GOAL_ETH_AUTONEG_AD_10_FD) {
        /* 10 Mbit/s FD */
        res = regSet(pCmd->pIface, GOAL_PHY_GENERIC_REG_AN_AD_10_FD,
                     (*pCmd->data.pModeAdv & GOAL_ETH_AUTONEG_AD_10_FD) ? GOAL_PHY_GENERIC_AN_AD_ON
                                                                        : GOAL_PHY_GENERIC_AN_AD_OFF);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
    }

    if (diffAdv & GOAL_ETH_AUTONEG_AD_100_HD) {
        /* 100 Mbit/s HD */
        res = regSet(pCmd->pIface, GOAL_PHY_GENERIC_REG_AN_AD_100_HD,
                     (*pCmd->data.pModeAdv & GOAL_ETH_AUTONEG_AD_100_HD) ? GOAL_PHY_GENERIC_AN_AD_ON
                                                                         : GOAL_PHY_GENERIC_AN_AD_OFF);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
    }

    if (diffAdv & GOAL_ETH_AUTONEG_AD_100_FD) {
        /* 100 Mbit/s FD */
        res = regSet(pCmd->pIface, GOAL_PHY_GENERIC_REG_AN_AD_100_FD,
                     (*pCmd->data.pModeAdv & GOAL_ETH_AUTONEG_AD_100_FD) ? GOAL_PHY_GENERIC_AN_AD_ON
                                                                         : GOAL_PHY_GENERIC_AN_AD_OFF);
        if (GOAL_RES_ERR(res)) {
            return res;
        }
    }

    if (GOAL_ETH_SPEED_1000 == *((uint32_t *) pCmd->pIface->pData)) {
        if (diffAdv & GOAL_ETH_AUTONEG_AD_1000_HD) {
            /* 1000 Mbit/s HD */
            res = regSet(pCmd->pIface, GOAL_PHY_GENERIC_REG_AN_AD_1000_HD,
                        (*pCmd->data.pModeAdv & GOAL_ETH_AUTONEG_AD_1000_HD) ? GOAL_PHY_GENERIC_AN_AD_ON
                                                                             : GOAL_PHY_GENERIC_AN_AD_OFF);
            if (GOAL_RES_ERR(res)) {
                return res;
            }
        }

        if (diffAdv & GOAL_ETH_AUTONEG_AD_1000_FD) {
            /* 1000 Mbit/s FD */
            res = regSet(pCmd->pIface, GOAL_PHY_GENERIC_REG_AN_AD_1000_FD,
                        (*pCmd->data.pModeAdv & GOAL_ETH_AUTONEG_AD_1000_FD) ? GOAL_PHY_GENERIC_AN_AD_ON
                                                                             : GOAL_PHY_GENERIC_AN_AD_OFF);
            if (GOAL_RES_ERR(res)) {
                return res;
            }
        }
    }


    return GOAL_OK;
}


/****************************************************************************/
/** generic register write function
 *
 * @retval GOAL_OK success
 * @retval others fail
 */
static GOAL_STATUS_T genericRegSet(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    uint64_t *pVal;                             /* ptr to arguments */
    uint32_t regNr;                             /* register number */
    uint32_t regVal;                            /* register value */

    pVal = (uint64_t *) pCmd->data.pRaw;
    regNr = (uint32_t) (*pVal >> 32);
    regVal = (uint32_t) (*pVal & 0xFFFFFFFF);

    return regSet(pCmd->pIface, (GOAL_PHY_GENERIC_REG_ID_T) regNr, regVal);
}


/****************************************************************************/
/** generic register write function
 *
 * @retval GOAL_OK success
 * @retval others fail
 */
static GOAL_STATUS_T genericRegGet(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    uint32_t *pValRes;                          /* ptr to result */
    uint32_t regNr;                             /* register number */

    pValRes = (uint32_t *) pCmd->data.pRaw;
    regNr = (uint32_t) (*pValRes);

    return regGet(pCmd->pIface, (GOAL_PHY_GENERIC_REG_ID_T) regNr, pValRes);
}
