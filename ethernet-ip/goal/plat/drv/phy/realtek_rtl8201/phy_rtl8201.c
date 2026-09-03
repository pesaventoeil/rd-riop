/** @file
 *
 * @brief Phy driver for Realtek RTL8201FI-VC-CG
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
#include <drv/phy/generic/phy_generic.h>
#include <drv/phy/realtek_rtl8201/phy_rtl8201.h>


/****************************************************************************/
/* PHY register data */
/****************************************************************************/
/* Ethernet Phy register table
 *
 * This table is used as a generic interface to read and write specific register
 * fields. They are accessed via IDs. The table provides the register address
 * and the location within the register.
 */
#define GOAL_PHY_RTL8201_RV_LIST \
    /*                  reg,  valId,                           shift,   mask, flags              */  \
    GOAL_PHY_RTL8201_RV(0x00, GOAL_PHY_RTL8201_REG_LINK_SPEED   , 13, 0x0001, GOAL_REG_FLG_RW     ), \
    GOAL_PHY_RTL8201_RV(0x01, GOAL_PHY_RTL8201_REG_LINK_STATUS  ,  2, 0x0001, 0                   ), \
    GOAL_PHY_RTL8201_RV(0x00, GOAL_PHY_RTL8201_REG_DUPLEX_MODE  ,  8, 0x0001, GOAL_REG_FLG_RW     ), \
    GOAL_PHY_RTL8201_RV(0x1C, GOAL_PHY_RTL8201_REG_MDI_MODE     ,  2, 0x0001, GOAL_REG_FLG_RW     ), \
    GOAL_PHY_RTL8201_RV(0x1C, GOAL_PHY_RTL8201_REG_MDI_STATE    ,  1, 0x0001, GOAL_REG_FLG_RW     )


/****************************************************************************/
/* Local defines */
/****************************************************************************/
/* link speed */
#define GOAL_PHY_RTL8201_LINK_SPEED_100       1 /**< link speed is 100 Mbit/s */
#define GOAL_PHY_RTL8201_LINK_SPEED_10        0 /**< link speed is 10 Mbit/s */

/* link status */
#define GOAL_PHY_RTL8201_LINK_STATUS_UP       1 /**< link established */
#define GOAL_PHY_RTL8201_LINK_STATUS_DOWN     0 /**< link not established */

/* duplex mode */
#define GOAL_PHY_RTL8201_DUPLEX_MODE_FULL     1 /**< full duplex mode */
#define GOAL_PHY_RTL8201_DUPLEX_MODE_HALF     0 /**< half duplex mode */

/* MDI Mode */
#define GOAL_PHY_RTL8201_MDI_MODE_AUTO        1 /**< MDI Mode Auto */
#define GOAL_PHY_RTL8201_MDI_MODE_MANUAL      0 /**< MDI Mode Manual */

/* MDI State */
#define GOAL_PHY_RTL8201_MDI_STATE_MDI        1 /**< MDI State: uncrossed */
#define GOAL_PHY_RTL8201_MDI_STATE_MDIX       0 /**< MDI State: crossed */

/* Phy ID */
#define GOAL_PHY_RTL8201_ID          0x001CC816 /**< Phy ID value */


/****************************************************************************/
/* Local data types */
/****************************************************************************/
/**< phy specific data */
typedef struct {
    GOAL_BUS_T *pBus;                           /**< bus handle */
    GOAL_ETH_IFACE_T phyGeneric;                /**< generic PHY data */
} GOAL_PHY_RTL8201_T;


/****************************************************************************/
/* Local variables */
/****************************************************************************/
/* create Register IDs from Phy register data  */
#undef GOAL_PHY_RTL8201_RV
#define GOAL_PHY_RTL8201_RV(reg, val, shift, mask, flags) val
typedef enum {
    GOAL_PHY_RTL8201_RV_LIST,
    GOAL_PHY_RTL8201_VAL_END
} GOAL_PHY_RTL8201_REG_ID_T;


/* implement Phy register table */
#undef GOAL_PHY_RTL8201_RV
#define GOAL_PHY_RTL8201_RV(reg, val, shift, mask, flags) { reg, shift, mask, flags }
static GOAL_REG_T regTable[] = {
    GOAL_PHY_RTL8201_RV_LIST,
};


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/
static GOAL_STATUS_T goal_targetPhyRtl8201RegGet(
    GOAL_ETH_IFACE_T *pIface,                   /**< interface data */
    GOAL_PHY_RTL8201_REG_ID_T reg,              /**< register */
    uint32_t *pVal                              /**< value ptr */
);

static GOAL_STATUS_T goal_targetPhyRtl8201RegSet(
    GOAL_ETH_IFACE_T *pIface,                   /**< interface data */
    GOAL_PHY_RTL8201_REG_ID_T reg,              /**< register */
    uint32_t val                                /**< value */
);

static GOAL_STATUS_T goal_targetPhyRtl8201Cmd(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T goal_targetPhyRtl8201SpeedGet(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T goal_targetPhyRtl8201DuplexGet(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T goal_targetPhyRtl8201DuplexSet(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T goal_targetPhyRtl8201MdiMode(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T goal_targetPhyRtl8201MdiState(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T goal_targetPhyRtl8201LinkStateGet(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T goal_targetPhyRtl8201SpeedSet(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);


/****************************************************************************/
/** Open PHY interface for Realtek RTL8201
 *
 * @retval GOAL_OK success
 * @retval others fail
 */
GOAL_STATUS_T goal_targetPhyRtl8201Open(
    GOAL_ETH_IFACE_T *pIface                    /**< interface data */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_PHY_RTL8201_T *pData = NULL;           /* data pointer */
    uint32_t idPhy;                             /* PHY OUI */

    /* check PHY id */
    res = goal_targetPhyGenericId(&pIface->bus, pIface->addr, &idPhy);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* check PHY id */
    if (GOAL_PHY_RTL8201_ID != idPhy) {
        goal_logErr("unknown PHY ID 0%08"FMT_x32, idPhy);
        return GOAL_ERR_UNSUPPORTED;
    }

    /* allocate phy data */
    res = goal_memCalloc(&pData, sizeof(GOAL_PHY_RTL8201_T));
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* inherit generic PHY driver */
    pData->phyGeneric.bus = pIface->bus;
    pData->phyGeneric.addr = pIface->addr;
    res = goal_targetPhyGenericOpen(&pData->phyGeneric, GOAL_ETH_SPEED_100);
    if (GOAL_RES_ERR(res)) {
        goal_memFree(&pData);
        return res;
    }

    /* assign interface data */
    pIface->pCmd = goal_targetPhyRtl8201Cmd;
    pIface->pData = pData;

    goal_logInfo("PHY found at address %"FMT_u32": Realtek RTL8201", pIface->addr);

    return GOAL_OK;
}


/****************************************************************************/
/** Get register value
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
static GOAL_STATUS_T goal_targetPhyRtl8201RegGet(
    GOAL_ETH_IFACE_T *pIface,                   /**< interface data */
    GOAL_PHY_RTL8201_REG_ID_T reg,              /**< register */
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
static GOAL_STATUS_T goal_targetPhyRtl8201RegSet(
    GOAL_ETH_IFACE_T *pIface,                   /**< interface data */
    GOAL_PHY_RTL8201_REG_ID_T reg,              /**< register */
    uint32_t val                                /**< value */
)
{
    /* write PHY register */
    return goal_regSet(regTable, &pIface->bus, pIface->addr, reg, val);
}


/****************************************************************************/
/** RTL8201 Phy command handler
 *
 * This handler only handles commands that access device specific registers.
 * Other commands are passed to the Generic Phy driver.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
static GOAL_STATUS_T goal_targetPhyRtl8201Cmd(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    switch (pCmd->cmd) {
        case GOAL_ETH_CMD_SPEED:
            if (pCmd->flgSet) {
                return goal_targetPhyRtl8201SpeedSet(pCmd);
                break;
                UNUSEDARG(goal_targetPhyRtl8201SpeedSet);
            }
            else {
                return goal_targetPhyRtl8201SpeedGet(pCmd);
            }

        case GOAL_ETH_CMD_DUPLEX:
            if (pCmd->flgSet) {
                return goal_targetPhyRtl8201DuplexSet(pCmd);
            }
            else {
                return goal_targetPhyRtl8201DuplexGet(pCmd);
            }

        case GOAL_ETH_CMD_LINK_STATE:
            /* special handling of link sate, overwrite Generic Phy driver */
            return goal_targetPhyRtl8201LinkStateGet(pCmd);

        case GOAL_ETH_CMD_MDI_MODE:
            return goal_targetPhyRtl8201MdiMode(pCmd);

        case GOAL_ETH_CMD_MDI_STATE:
            return goal_targetPhyRtl8201MdiState(pCmd);

        default:
            break;
    }

    /* pass command to generic Phy driver */
    pCmd->pIface = &((GOAL_PHY_RTL8201_T *) pCmd->pIface->pData)->phyGeneric;
    return pCmd->pIface->pCmd(pCmd);
}


/****************************************************************************/
/** RTL8201 Phy - Get link speed
 *
 * @retval GOAL_OK success
 * @retval others fail
 */
static GOAL_STATUS_T goal_targetPhyRtl8201SpeedGet(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    GOAL_STATUS_T res;                          /* result */
    uint32_t speed;                             /* speed tmp */

    /* get speed */
    res = goal_targetPhyRtl8201RegGet(pCmd->pIface, GOAL_PHY_RTL8201_REG_LINK_SPEED, &speed);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    if (GOAL_PHY_RTL8201_LINK_SPEED_100 == speed) {
        *pCmd->data.pSpeed = GOAL_ETH_SPEED_100;
    }
    else {
        *pCmd->data.pSpeed = GOAL_ETH_SPEED_10;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** RTL8201 Phy - Set link speed
 *
 * @retval GOAL_OK success
 * @retval others fail
 */
static GOAL_STATUS_T goal_targetPhyRtl8201SpeedSet(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    uint32_t speed;                             /* speed tmp */
    GOAL_ETH_CMD_DATA_T cmd;                    /* command data */
    uint32_t state;                             /* autonet state */

    /*
     * special handling is required for for this PHY,
     * where autoneg needs to be disabled before setting a speed
     *
     * this is done here by calling the underlying generic driver
     */
    cmd.cmd = GOAL_ETH_CMD_AUTONEG;
    cmd.flgSet = GOAL_TRUE;
    state = GOAL_ETH_AUTONEG_OFF;
    cmd.data.pStateAutoNeg = &state;
    cmd.pIface = (GOAL_ETH_IFACE_T *) &((GOAL_PHY_RTL8201_T *) pCmd->pIface->pData)->phyGeneric;
    res = cmd.pIface->pCmd(&cmd);

    /* set speed value */
    if (GOAL_RES_OK(res)) {
        if (*pCmd->data.pSpeed == GOAL_ETH_SPEED_100) {
            speed = GOAL_PHY_RTL8201_LINK_SPEED_100;
        }
        else {
            speed = GOAL_PHY_RTL8201_LINK_SPEED_10;
        }

        res = goal_targetPhyRtl8201RegSet(pCmd->pIface, GOAL_PHY_RTL8201_REG_LINK_SPEED, speed);
    }

    return res;
}


/****************************************************************************/
/** RTL8201 Phy - Get link duplex
 *
 * @retval GOAL_OK success
 * @retval others fail
 */
static GOAL_STATUS_T goal_targetPhyRtl8201DuplexGet(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    GOAL_STATUS_T res;                          /* result */
    uint32_t duplex;                            /* duplex tmp */

    /* get duplex */
    res = goal_targetPhyRtl8201RegGet(pCmd->pIface, GOAL_PHY_RTL8201_REG_DUPLEX_MODE, &duplex);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    if (GOAL_PHY_RTL8201_DUPLEX_MODE_FULL == duplex) {
        *pCmd->data.pModeDuplex = GOAL_ETH_DUPLEX_FULL;
    }
    else {
        *pCmd->data.pModeDuplex = GOAL_ETH_DUPLEX_HALF;
    }

    return GOAL_OK;
}


/****************************************************************************/
/** RTL8201 Phy - Set link duplex
 *
 * @retval GOAL_OK success
 * @retval others fail
 */
static GOAL_STATUS_T goal_targetPhyRtl8201DuplexSet(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */
    uint32_t duplex;                            /* duplex tmp */
    GOAL_ETH_CMD_DATA_T cmd;                    /* command data */
    uint32_t state;                             /* autonet state */

    /*
     * special handling is required for for this PHY,
     * where autoneg needs to be disabled before setting a speed
     *
     * this is done here by calling the underlying generic driver
     */
    cmd.cmd = GOAL_ETH_CMD_AUTONEG;
    cmd.flgSet = GOAL_TRUE;
    state = GOAL_ETH_AUTONEG_OFF;
    cmd.data.pStateAutoNeg = &state;
    cmd.pIface = (GOAL_ETH_IFACE_T *) &((GOAL_PHY_RTL8201_T *) pCmd->pIface->pData)->phyGeneric;
    res = cmd.pIface->pCmd(&cmd);

    /* get duplex */
    if (*pCmd->data.pModeDuplex == GOAL_ETH_DUPLEX_FULL) {
        duplex = GOAL_PHY_RTL8201_DUPLEX_MODE_FULL;
    }
    else {
        duplex = GOAL_PHY_RTL8201_DUPLEX_MODE_HALF;
    }

    res = goal_targetPhyRtl8201RegSet(pCmd->pIface, GOAL_PHY_RTL8201_REG_DUPLEX_MODE, duplex);

    return res;
}


/****************************************************************************/
/** RTL8201 Phy - Get/Set MDI mode
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_targetPhyRtl8201MdiMode(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    GOAL_STATUS_T res;                          /* result */
    uint32_t val;                               /* register value */

    res = goal_targetPhyRtl8201RegGet(pCmd->pIface, GOAL_PHY_RTL8201_REG_MDI_MODE, &val);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* translate value to GOAL enum */
    val = (GOAL_PHY_RTL8201_MDI_MODE_AUTO == val) ? GOAL_ETH_MDI_MODE_AUTO : GOAL_ETH_MDI_MODE_MANUAL;

    /* read mode */
    if (GOAL_FALSE == pCmd->flgSet) {

        /* report current MDI mode */
        *pCmd->data.pModeMdi = val;

        return GOAL_OK;
    }

    /* write mode */
    if (((GOAL_ETH_MDI_MODE_AUTO == val) && (GOAL_ETH_MDI_MODE_DEFAULT == *pCmd->data.pModeMdi)) ||
        (val == *pCmd->data.pModeMdi)) {
            /* requested mode is already set */
            return GOAL_OK;
    }

    /* apply new mode */
    switch (*pCmd->data.pModeMdi) {

        case GOAL_ETH_MDI_MODE_DEFAULT:
        case GOAL_ETH_MDI_MODE_AUTO:
            val = GOAL_PHY_RTL8201_MDI_MODE_AUTO;
            break;

        case GOAL_ETH_MDI_MODE_MANUAL:
            val = GOAL_PHY_RTL8201_MDI_MODE_MANUAL;
            break;

        default:
            goal_logErr("unknown MDI mode");
            return GOAL_ERR_UNSUPPORTED;
    }

    res = goal_targetPhyRtl8201RegSet(pCmd->pIface, GOAL_PHY_RTL8201_REG_MDI_MODE, val);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to set MDI mode");
    }

    return res;
}


/****************************************************************************/
/** RTL8201 Phy - Get/Set MDI state
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_targetPhyRtl8201MdiState(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    GOAL_STATUS_T res;                          /* result */
    uint32_t val;                               /* register value */

    res = goal_targetPhyRtl8201RegGet(pCmd->pIface, GOAL_PHY_RTL8201_REG_MDI_STATE, &val);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* translate value to GOAL enum */
    val = (GOAL_PHY_RTL8201_MDI_STATE_MDI == val) ? GOAL_ETH_MDI_STATE_UNCROSSED : GOAL_ETH_MDI_STATE_CROSSED;

    /* read state */
    if (GOAL_FALSE == pCmd->flgSet) {

        /* report current MDI state */
        *pCmd->data.pStateMdi = val;

        return GOAL_OK;
    }

    /* write state */
    if (((GOAL_ETH_MDI_STATE_UNCROSSED == val) && (GOAL_ETH_MDI_STATE_DEFAULT == *pCmd->data.pStateMdi)) ||
        (val == *pCmd->data.pStateMdi)) {
            /* requested state is already set */
            return GOAL_OK;
    }

    /* apply new state */
    switch (*pCmd->data.pStateMdi) {

        case GOAL_ETH_MDI_STATE_DEFAULT:
        case GOAL_ETH_MDI_STATE_UNCROSSED:
            val = GOAL_PHY_RTL8201_MDI_STATE_MDI;
            break;

        case GOAL_ETH_MDI_STATE_CROSSED:
            val = GOAL_PHY_RTL8201_MDI_STATE_MDIX;
            break;

        default:
            goal_logErr("unknown MDI state");
            return GOAL_ERR_UNSUPPORTED;
    }

    res = goal_targetPhyRtl8201RegSet(pCmd->pIface, GOAL_PHY_RTL8201_REG_MDI_STATE, val);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to set MDI state");
    }

    return res;
}


/****************************************************************************/
/** RTL8201 Phy - Get link state
 *
 * @retval GOAL_OK success
 * @retval others fail
 */
static GOAL_STATUS_T goal_targetPhyRtl8201LinkStateGet(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    GOAL_STATUS_T res;                          /* result */
    uint32_t state;                             /* link state */

    /* get state */
    if (GOAL_TRUE == pCmd->flgSet) {
        /* set not supported */
        return GOAL_ERR_ACCESS;
    }

    /* read link state twice to get current state */
    res = goal_targetPhyRtl8201RegGet(pCmd->pIface, GOAL_PHY_RTL8201_REG_LINK_STATUS, &state);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    res = goal_targetPhyRtl8201RegGet(pCmd->pIface, GOAL_PHY_RTL8201_REG_LINK_STATUS, &state);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    *pCmd->data.pStateLink = (GOAL_PHY_RTL8201_LINK_STATUS_UP == state) ? GOAL_ETH_STATE_UP : GOAL_ETH_STATE_DOWN;

    return GOAL_OK;
}
