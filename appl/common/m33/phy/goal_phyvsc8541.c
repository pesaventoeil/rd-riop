/** @file
 *
 * @brief Phy driver for VSC8541 
 *
 * @copyright
 * Copyright 2010-2022 port GmbH.
 * This software is protected Intellectual Property and may only be used
 * according to the license agreement.
 *
 * Copyright 2025-2026 NXP
 *
 * NXP Confidential and Proprietary. This software is owned or controlled by NXP
 * and may only be used strictly in accordance with the applicable license
 * terms. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you
 * have read, and that you agree to comply with and are bound by, such license
 * terms. If you do not agree to be bound by the applicable license terms, then
 * you may not retain, install, activate or otherwise use the software.
 */

#include "fsl_phy.h"
#include <goal_includes.h>
#include <drv/phy/generic/phy_generic.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief Defines the PHY VSC8541 vendor defined registers. */
#define PHY_AUXILIARY_CTRL_STATUS_REG 0x1CU /*!< The PHY auxiliary control and status register. */
#define PHY_PAGE_SELECT_REG           0x1FU /*!< The PHY page select register. */
#define PHY_EXT_MODE_CTRL_REG_P1      0x13U /*!< 19E1: Extended Mode Control Page1*/

/*! @brief Defines the PHY VSC8541 device ID information. */
#define PHY_OUI                 0x1C1U /*!< The PHY organizationally unique identifier. */
#define PHY_MODEL_NUM           0x37U  /*!< The PHY organizationally unique identifier. */
#define PHY_DEVICE_REVISION_NUM 0x2U   /*!< The PHY organizationally unique identifier. */
#define PHY_DEVICE_ID           ((PHY_OUI << 10U) | (PHY_MODEL_NUM << 4U) | (PHY_DEVICE_REVISION_NUM))


#define LINKSPEED_SHIFT   3U         /*!< The link speed shift */
#define LINKDUPLEX_SHIFT  5U         /*!< The link duplex shift */
#define MDIMODE_SHIFT     1U         /*!< The MDI Mode shift */
#define LINKSTATUS_SHIFT  2U         /*!< The link status shift */
#define GOAL_PHY_BCTL_RESET_SHIFT 15U
#define GOAL_PHY_BCTL_FULL_SHIFT 0U
#define GOAL_PHY_BCTL_RESTART_AUTONEG_SHIFT  9U
#define GOAL_PHY_BCTL_AUTONEG_SHIFT 12U	   

#define GOAL_PHY_100BASETX_FULLDUPLEX_SHIFT 8U /*!< The PHY has the 100M full duplex ability.*/
#define GOAL_PHY_100BASETX_HALFDUPLEX_SHIFT  7U /*!< The PHY has the 100M full duplex ability.*/
#define GOAL_PHY_10BASETX_FULLDUPLEX_SHIFT   6U /*!< The PHY has the 10M full duplex ability.*/
#define GOAL_PHY_10BASETX_HALFDUPLEX_SHIFT   5U /*!< The PHY has the 10M full duplex ability.*/
#define GOAL_PHY_IEEE802_3_SELECTOR_SHIFT    0U /*!< The message type being sent by Auto-Nego.*/
#define GOAL_PHY_1000BASET_FULLDUPLEX_SHIFT  9U /*!< The PHY has the 1000M full duplex ability.*/

/*! @brief Defines the mask flag in specific status register. */
#if 0 /* Register masks as per driver manual */
#define LINKSPEED_MASK    0x0018U    /*!< The PHY link speed mask. */
#define LINKDUPLEX_MASK   0x0020U    /*!< The PHY link duplex mask. */
#define MDIMODE_MASK      0x0006U    /*!< The MDI Mode mask. */
#define LINKSTATUS_MASK   0x0004U
#define PHY_BCTL_RESET_MASK           ((uint16_t)0x8000U) /*!< The PHY reset bit mask. */
#define GOAL_PHY_BCTL_FULL_MASK       ((uint16_t)0x0000U)	
#define PHY_BCTL_RESTART_AUTONEG_MASK ((uint16_t)0x0200U) /*!< The PHY restart auto negotiation mask. */
#define PHY_BCTL_AUTONEG_MASK         ((uint16_t)0x1000U) /*!< The PHY auto negotiation bit mask. */

#define PHY_100BASETX_FULLDUPLEX_MASK ((uint16_t)0x100U) /*!< The PHY has the 100M full duplex ability.*/
#define PHY_100BASETX_HALFDUPLEX_MASK ((uint16_t)0x080U) /*!< The PHY has the 100M full duplex ability.*/
#define PHY_10BASETX_FULLDUPLEX_MASK  ((uint16_t)0x040U) /*!< The PHY has the 10M full duplex ability.*/
#define PHY_10BASETX_HALFDUPLEX_MASK  ((uint16_t)0x020U) /*!< The PHY has the 10M full duplex ability.*/
#define PHY_IEEE802_3_SELECTOR_MASK   ((uint16_t)0x001U) /*!< The message type being sent by Auto-Nego.*/
#define PHY_1000BASET_FULLDUPLEX_MASK ((uint16_t)0x200U) /*!< The PHY has the 1000M full duplex ability.*/
#else /* Register masks as per GOAL driver format */
#define LINKSPEED_MASK    0x0003U    /*!< The PHY link speed mask. */
#define LINKDUPLEX_MASK   0x0001U    /*!< The PHY link duplex mask. */
#define MDIMODE_MASK      0x0003U    /*!< The MDI Mode mask. */
#define LINKSTATUS_MASK   0x0001U
#define GOAL_PHY_BCTL_RESET_MASK 	   0x0001U
#define GOAL_PHY_BCTL_FULL_MASK 	   0xFFFFU
#define GOAL_PHY_BCTL_RESTART_AUTONEG_MASK 0x0001U
#define GOAL_PHY_BCTL_AUTONEG_MASK 	   0x0001U

#define GOAL_PHY_100BASETX_FULLDUPLEX_MASK 0x0001U /*!< The PHY has the 100M full duplex ability.*/
#define GOAL_PHY_100BASETX_HALFDUPLEX_MASK 0x0001U /*!< The PHY has the 100M full duplex ability.*/
#define GOAL_PHY_10BASETX_FULLDUPLEX_MASK  0x0001U /*!< The PHY has the 10M full duplex ability.*/
#define GOAL_PHY_10BASETX_HALFDUPLEX_MASK  0x0001U /*!< The PHY has the 10M full duplex ability.*/
#define GOAL_PHY_IEEE802_3_SELECTOR_MASK   0x0001U /*!< The message type being sent by Auto-Nego.*/
#define GOAL_PHY_1000BASET_FULLDUPLEX_MASK 0x0001U /*!< The PHY has the 1000M full duplex ability.*/
#endif

/*! @brief Defines the PHY VSC8541 extra page and the registers in specified page. */
#define PHY_RGMII_TXRX_DELAY_REG 0x14U /*!< The RGMII TXC/RXC delay register. */
#define PHY_RGMII_TX_DELAY_SHIFT 0U    /*!< The RGMII TXC delay mask. */
#define PHY_RGMII_RX_DELAY_SHIFT 4U    /*!< The RGMII RXC delay mask. */

/*! @brief Defines the timeout macro. */
#define PHY_READID_TIMEOUT_COUNT 1000U

/*! @brief Defines the PHY resource interface. */
#define PHY_VSC8541_WRITE(handle, regAddr, data) \
    ((phy_vsc8541_resource_t *)(handle)->resource)->write((handle)->phyAddr, regAddr, data)
#define PHY_VSC8541_READ(handle, regAddr, pData) \
    ((phy_vsc8541_resource_t *)(handle)->resource)->read((handle)->phyAddr, regAddr, pData)

/* link speed */
#define GOAL_PHY_VSC8541_LINK_SPEED_1000      2 /**< link speed is 1000 Mbit/s */
#define GOAL_PHY_VSC8541_LINK_SPEED_100       1 /**< link speed is 100 Mbit/s */
#define GOAL_PHY_VSC8541_LINK_SPEED_10        0 /**< link speed is 10 Mbit/s */

/* link status */
#define GOAL_PHY_VSC8541_LINK_STATUS_UP       1 /**< link established */
#define GOAL_PHY_VSC8541_LINK_STATUS_DOWN     0 /**< link not established */

/* duplex mode */
#define GOAL_PHY_VSC8541_DUPLEX_MODE_FULL     1 /**< full duplex mode */
#define GOAL_PHY_VSC8541_DUPLEX_MODE_HALF     0 /**< half duplex mode */

/* MDI Mode */
#define GOAL_PHY_VSC8541_MDI_MODE_MANUAL_X    3 /**< MDI-X Mode Manual */
#define GOAL_PHY_VSC8541_MDI_MODE_MANUAL      2 /**< MDI Mode Manual */
#define GOAL_PHY_VSC8541_MDI_MODE_RESERVED    1 /**< MDI Mode Reserved */
#define GOAL_PHY_VSC8541_MDI_MODE_AUTO        0 /**< MDI/MDI-X Mode Auto */

/* MDI State */
#define GOAL_PHY_VSC8541_MDI_STATE_MDI        1 /**< MDI State: uncrossed */
#define GOAL_PHY_VSC8541_MDI_STATE_MDIX       0 /**< MDI State: crossed */

/* Phy ID */
#define GOAL_PHY_VSC8541_ID          PHY_DEVICE_ID /**< Phy ID value */


/****************************************************************************/
/* PHY register data */
/****************************************************************************/
/*
#define GOAL_REG_LINK_SPEED    PHY_AUXILIARY_CTRL_STATUS_REG
#define GOAL_REG_LINK_STATUS   PHY_BASICSTATUS_REG
#define GOAL_REG_DUPLEX_MODE   PHY_AUXILIARY_CTRL_STATUS_REG
#define GOAL_REG_MDI_MODE      PHY_EXT_MODE_CTRL_REG
#define GOAL_PHY_VSC8541_REG_MDI_STATE     PHY_AUXILIARY_CTRL_STATUS_REG
*/
/* Ethernet Phy register table
 *
 * This table is used as a generic interface to read and write specific register
 * fields. They are accessed via IDs. The table provides the register address
 * and the location within the register.
 */

#define GOAL_PHY_VSC8541_RV_LIST \
    /*                  reg,                           valId,                shift,            mask,                        flags          */  \
    GOAL_PHY_VSC8541_RV(PHY_AUXILIARY_CTRL_STATUS_REG, GOAL_REG_LINK_SPEED,  LINKSPEED_SHIFT,  LINKSPEED_MASK ,             GOAL_REG_FLG_RO ), \
    GOAL_PHY_VSC8541_RV(PHY_AUXILIARY_CTRL_STATUS_REG, GOAL_REG_DUPLEX_MODE, LINKDUPLEX_SHIFT, LINKDUPLEX_MASK,             GOAL_REG_FLG_RO ), \
    GOAL_PHY_VSC8541_RV(PHY_BASICSTATUS_REG,           GOAL_REG_LINK_STATUS, LINKSTATUS_SHIFT, LINKSTATUS_MASK, 	    GOAL_REG_FLG_RO ), \
    GOAL_PHY_VSC8541_RV(PHY_EXT_MODE_CTRL_REG_P1,      GOAL_REG_MDI_MODE,    MDIMODE_SHIFT,    MDIMODE_MASK,                GOAL_REG_FLG_RW ), \
    GOAL_PHY_VSC8541_RV(PHY_BASICCONTROL_REG, GOAL_REG_BASIC_CONTROL_RESET, GOAL_PHY_BCTL_RESET_SHIFT, GOAL_PHY_BCTL_RESET_MASK,  GOAL_REG_FLG_RW ), \
    GOAL_PHY_VSC8541_RV(PHY_BASICCONTROL_REG, GOAL_REG_BASIC_CONTROL_FULL, GOAL_PHY_BCTL_FULL_SHIFT, GOAL_PHY_BCTL_FULL_MASK, GOAL_REG_FLG_RW ), \
    GOAL_PHY_VSC8541_RV(PHY_BASICCONTROL_REG, GOAL_REG_BASIC_CONTROL_AUTONEG, GOAL_PHY_BCTL_AUTONEG_SHIFT, GOAL_PHY_BCTL_AUTONEG_MASK, GOAL_REG_FLG_RW ), \
    GOAL_PHY_VSC8541_RV(PHY_BASICCONTROL_REG, GOAL_REG_BASIC_CONTROL_AUTONEG_RESTART, GOAL_PHY_BCTL_RESTART_AUTONEG_SHIFT, GOAL_PHY_BCTL_RESTART_AUTONEG_MASK, GOAL_REG_FLG_RW ), \
    GOAL_PHY_VSC8541_RV(PHY_AUTONEG_ADVERTISE_REG, GOAL_REG_AUTONEG_ADV_100BT_FD, GOAL_PHY_100BASETX_FULLDUPLEX_SHIFT, GOAL_PHY_100BASETX_FULLDUPLEX_MASK, GOAL_REG_FLG_RW ), \
    GOAL_PHY_VSC8541_RV(PHY_AUTONEG_ADVERTISE_REG, GOAL_REG_AUTONEG_ADV_100BT_HD, GOAL_PHY_100BASETX_HALFDUPLEX_SHIFT, GOAL_PHY_100BASETX_HALFDUPLEX_MASK, GOAL_REG_FLG_RW ), \
    GOAL_PHY_VSC8541_RV(PHY_AUTONEG_ADVERTISE_REG, GOAL_REG_AUTONEG_ADV_10BT_FD, GOAL_PHY_10BASETX_FULLDUPLEX_SHIFT, GOAL_PHY_10BASETX_FULLDUPLEX_MASK, GOAL_REG_FLG_RW ), \
    GOAL_PHY_VSC8541_RV(PHY_AUTONEG_ADVERTISE_REG, GOAL_REG_AUTONEG_ADV_10BT_HD, GOAL_PHY_10BASETX_HALFDUPLEX_SHIFT, GOAL_PHY_10BASETX_HALFDUPLEX_MASK, GOAL_REG_FLG_RW ), \
    GOAL_PHY_VSC8541_RV(PHY_AUTONEG_ADVERTISE_REG, GOAL_REG_AUTONEG_ADV_802_3, GOAL_PHY_IEEE802_3_SELECTOR_SHIFT, GOAL_PHY_IEEE802_3_SELECTOR_MASK, GOAL_REG_FLG_RW ),  \
    GOAL_PHY_VSC8541_RV(PHY_1000BASET_CONTROL_REG, GOAL_REG_1000BT_CONTROL_FD, GOAL_PHY_1000BASET_FULLDUPLEX_SHIFT, GOAL_PHY_1000BASET_FULLDUPLEX_MASK, GOAL_REG_FLG_RW )

/* 
 * Stuff copied from phy_rtl8211.c
 */
/****************************************************************************/
/* Local variables */
/****************************************************************************/
/* create Register IDs from Phy register data  */
#undef GOAL_PHY_VSC8541_RV
#define GOAL_PHY_VSC8541_RV(reg, val, shift, mask, flags) val
typedef enum {
    GOAL_PHY_VSC8541_RV_LIST,
    GOAL_PHY_VSC8541_VAL_END
} GOAL_PHY_VSC8541_REG_ID_T;


/* implement Phy register table */
#undef GOAL_PHY_VSC8541_RV
#define GOAL_PHY_VSC8541_RV(reg, val, shift, mask, flags) { reg, shift, mask, flags }
static GOAL_REG_T regTable[] = {
    GOAL_PHY_VSC8541_RV_LIST,
};
/****************************************************************************/
/* Local data types */
/****************************************************************************/
/**< phy specific data */
typedef struct {
    GOAL_BUS_T *pBus;                           /**< bus handle */
    GOAL_ETH_IFACE_T phyGeneric;                /**< generic PHY data */
} GOAL_PHY_VSC8541_T;
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/****************************************************************************/
/* Local prototypes */
/****************************************************************************/
static GOAL_STATUS_T goal_targetPhyVsc8541RegGet(
    GOAL_ETH_IFACE_T *pIface,                   /**< interface data */
    GOAL_PHY_VSC8541_REG_ID_T reg,              /**< register */
    uint32_t *pVal                              /**< value ptr */
);

static GOAL_STATUS_T goal_targetPhyVsc8541RegSet(
    GOAL_ETH_IFACE_T *pIface,                   /**< interface data */
    GOAL_PHY_VSC8541_REG_ID_T reg,              /**< register */
    uint32_t val                                /**< value */
);
static GOAL_STATUS_T goal_targetPhyVsc8541Cmd(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T goal_targetPhyVsc8541SpeedGet(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T goal_targetPhyVsc8541DuplexGet(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

static GOAL_STATUS_T goal_targetPhyVsc8541MdiMode(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);
#if 0
static GOAL_STATUS_T goal_targetPhyVsc8541MdiState(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);
#endif

static GOAL_STATUS_T goal_targetPhyVsc8541LinkStateGet(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
);

/*******************************************************************************
 * Code
 ******************************************************************************/

/****************************************************************************/
/** Open PHY interface for MicroSemi  VSC8541
 *
 * @retval GOAL_OK success
 * @retval others fail
 */
GOAL_STATUS_T goal_targetPhyVsc8541Open(
    GOAL_ETH_IFACE_T *pIface                    /**< interface data */
)
{
    GOAL_STATUS_T res, result;                          /* result */
    GOAL_PHY_VSC8541_T *pData = NULL;           /* data pointer */
    uint32_t idPhy, regValue;                             /* PHY OUI */

    /* check PHY id */
    res = goal_targetPhyGenericId(&pIface->bus, pIface->addr, &idPhy);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* check PHY id */
    if (PHY_DEVICE_ID != idPhy) {
        goal_logErr("unknown PHY ID 0%08"FMT_x32, idPhy);
        return GOAL_ERR_UNSUPPORTED;
    }

    /* allocate phy data */
    res = goal_memCalloc(&pData, sizeof(GOAL_PHY_VSC8541_T));
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* inherit generic PHY driver */
    pData->phyGeneric.bus = pIface->bus;
    pData->phyGeneric.addr = pIface->addr;
    res = goal_targetPhyGenericOpen(&pData->phyGeneric, GOAL_ETH_SPEED_1000);
    if (GOAL_RES_ERR(res)) {
        goal_memFree(&pData);
        return res;
    }

    /* assign interface data */
    pIface->pCmd = goal_targetPhyVsc8541Cmd;
    pIface->pData = pData;

    goal_logInfo("PHY found at address %"FMT_u32": VSC8541", pIface->addr);

    /* Reset PHY. */
    result = goal_targetPhyVsc8541RegSet(pIface, GOAL_REG_BASIC_CONTROL_RESET, 1);
    if (result != kStatus_Success)
    {
        goal_logErr("failed to reset PHY");
        return result;
    }

    //if (config->autoNeg)
    {
        /* Set the auto-negotiation. */
    	result = goal_targetPhyVsc8541RegSet(pIface, GOAL_REG_AUTONEG_ADV_100BT_FD, 1);
    	result = goal_targetPhyVsc8541RegSet(pIface, GOAL_REG_AUTONEG_ADV_100BT_HD, 1);
    	result = goal_targetPhyVsc8541RegSet(pIface, GOAL_REG_AUTONEG_ADV_10BT_FD, 1);
    	result = goal_targetPhyVsc8541RegSet(pIface, GOAL_REG_AUTONEG_ADV_10BT_HD, 1);
    	result = goal_targetPhyVsc8541RegSet(pIface, GOAL_REG_AUTONEG_ADV_802_3, 1);
    	result = goal_targetPhyVsc8541RegSet(pIface, GOAL_REG_AUTONEG_ADV_802_3, 1);
        if (result == kStatus_Success)
        {
    	    result = goal_targetPhyVsc8541RegSet(pIface, GOAL_REG_1000BT_CONTROL_FD, 1);
            if (result == kStatus_Success)
            {
    		result = goal_targetPhyVsc8541RegGet(pIface, GOAL_REG_BASIC_CONTROL_FULL, &regValue);
                if (result == kStatus_Success)
                {
    		    result = goal_targetPhyVsc8541RegSet(pIface, GOAL_REG_BASIC_CONTROL_AUTONEG, 1);
    		    result = goal_targetPhyVsc8541RegSet(pIface, GOAL_REG_BASIC_CONTROL_AUTONEG_RESTART, 1);
                }
            }
        }
    }

    return GOAL_OK;
}

/** VSC8541 Phy command handler
 *
 * This handler only handles commands that access device specific registers.
 * Other commands are passed to the Generic Phy driver.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
static GOAL_STATUS_T goal_targetPhyVsc8541Cmd(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    switch (pCmd->cmd) {
        case GOAL_ETH_CMD_SPEED:
            if (pCmd->flgSet) {
                break;
            }
            return goal_targetPhyVsc8541SpeedGet(pCmd);

        case GOAL_ETH_CMD_DUPLEX:
            if (pCmd->flgSet) {
                break;
            }
            return goal_targetPhyVsc8541DuplexGet(pCmd);

        case GOAL_ETH_CMD_LINK_STATE:
            /* special handling of link sate, overwrite Generic Phy driver */
            return goal_targetPhyVsc8541LinkStateGet(pCmd);

        case GOAL_ETH_CMD_MDI_MODE:
            return goal_targetPhyVsc8541MdiMode(pCmd);

#if 0
        case GOAL_ETH_CMD_MDI_STATE:
            return goal_targetPhyVsc8541MdiState(pCmd);
#endif

        default:
            break;

    }

    /* pass command to generic Phy driver */
    pCmd->pIface = &((GOAL_PHY_VSC8541_T *) pCmd->pIface->pData)->phyGeneric;
    return pCmd->pIface->pCmd(pCmd);
}

static GOAL_STATUS_T goal_targetPhyVsc8541RegGet(
    GOAL_ETH_IFACE_T *pIface,                   /**< interface data */
    GOAL_PHY_VSC8541_REG_ID_T reg,              /**< register */
    uint32_t *pVal                              /**< value ptr */
)
{
    /* read PHY register */
    return goal_regGet(regTable, &pIface->bus, pIface->addr, reg, pVal);
}

static GOAL_STATUS_T goal_targetPhyVsc8541RegSet(
    GOAL_ETH_IFACE_T *pIface,                   /**< interface data */
    GOAL_PHY_VSC8541_REG_ID_T reg,              /**< register */
    uint32_t val                                /**< value */
)
{
    /* write PHY register */
    return goal_regSet(regTable, &pIface->bus, pIface->addr, reg, val);
}

static GOAL_STATUS_T goal_targetPhyVsc8541SpeedGet(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    GOAL_STATUS_T res;                          /* result */
    uint32_t regValue;                          /* RegValue */
    uint32_t speed;                             /* speed tmp */

    /* get speed */
    res = goal_targetPhyVsc8541RegGet(pCmd->pIface, GOAL_REG_LINK_SPEED, &regValue);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    speed = regValue;
    switch (speed) {
        case GOAL_PHY_VSC8541_LINK_SPEED_10:
            *pCmd->data.pSpeed = GOAL_ETH_SPEED_10;
            break;

        case GOAL_PHY_VSC8541_LINK_SPEED_100:
            *pCmd->data.pSpeed = GOAL_ETH_SPEED_100;
            break;

        case GOAL_PHY_VSC8541_LINK_SPEED_1000:
            *pCmd->data.pSpeed = GOAL_ETH_SPEED_1000;
            break;

        default:
            *pCmd->data.pSpeed = GOAL_ETH_SPEED_UNKNOWN;
            break;
    }
    return GOAL_OK;
}

static GOAL_STATUS_T goal_targetPhyVsc8541DuplexGet(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    GOAL_STATUS_T res;                          /* result */
    uint32_t duplex;                            /* duplex tmp */
    uint32_t regValue;                          /* RegValue */

    /* get duplex */
    res = goal_targetPhyVsc8541RegGet(pCmd->pIface, GOAL_REG_DUPLEX_MODE, &regValue);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    duplex = regValue;
    if (GOAL_PHY_VSC8541_DUPLEX_MODE_FULL == duplex) {
        *pCmd->data.pModeDuplex = GOAL_ETH_DUPLEX_FULL;
    }
    else {
        *pCmd->data.pModeDuplex = GOAL_ETH_DUPLEX_HALF;
    }

    return GOAL_OK;
}

static GOAL_STATUS_T goal_targetPhyVsc8541MdiMode(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    GOAL_STATUS_T res;                          /* result */
    uint32_t val;                               /* register value */
    uint32_t mdi_mode;                          /* mode value */

    /* set page 1 */
    val = 1;
    /* likely routine: goal_drvBusMdioNetcWrite(bus, addr, reg, value, len) */
    res = pCmd->pIface->bus.write(&pCmd->pIface->bus, pCmd->pIface->addr, PHY_PAGE_SELECT_REG, (char *) &val, 2);
    if (GOAL_RES_ERR(res)) {
	    goal_logErr("failed to set Phy page to 1, port %"FMT_u32"VSC8541", pCmd->pIface->addr);
	    return res;
    }

    res = goal_targetPhyVsc8541RegGet(pCmd->pIface, GOAL_REG_MDI_MODE, &val);
    if (GOAL_RES_ERR(res)) {
	goto ret;
    }

    //mdi_mode = ((val & MDIMODE_MASK) >> MDIMODE_SHIFT);
    mdi_mode = val;

    /* translate value to GOAL enum */
    switch (mdi_mode) {
        case GOAL_PHY_VSC8541_MDI_MODE_AUTO:
            val = GOAL_ETH_MDI_MODE_AUTO;
            break;
        case GOAL_PHY_VSC8541_MDI_MODE_MANUAL:
        case GOAL_PHY_VSC8541_MDI_MODE_MANUAL_X:
            val = GOAL_ETH_MDI_MODE_MANUAL;
            break;
        case GOAL_PHY_VSC8541_MDI_MODE_RESERVED:
            val = GOAL_ETH_MDI_MODE_UNKNOWN;
            break;
        default:
            goal_logErr("unknown MDI mode");
	    res = GOAL_ERR_UNSUPPORTED;
	    goto ret;
    }

    /* read mode */
    if (GOAL_FALSE == pCmd->flgSet) {

        /* report current MDI mode */
        *pCmd->data.pModeMdi = val;

	res = GOAL_OK;
	goto ret;
    }

    /* write mode */
    if (((GOAL_ETH_MDI_MODE_AUTO == val) && (GOAL_ETH_MDI_MODE_DEFAULT == *pCmd->data.pModeMdi)) ||
        (val == *pCmd->data.pModeMdi)) {
            /* requested mode is already set */
            res = GOAL_OK;
            goto ret;
    }

    /* apply new mode */
    switch (*pCmd->data.pModeMdi) {

        case GOAL_ETH_MDI_MODE_DEFAULT:
        case GOAL_ETH_MDI_MODE_AUTO:
            val = GOAL_PHY_VSC8541_MDI_MODE_AUTO;
            break;

        case GOAL_ETH_MDI_MODE_MANUAL:
            val = GOAL_PHY_VSC8541_MDI_MODE_MANUAL;
            break;

        default:
            goal_logErr("unknown MDI mode");
	    res = GOAL_ERR_UNSUPPORTED;
            goto ret;
    }

    res = goal_targetPhyVsc8541RegSet(pCmd->pIface, GOAL_REG_MDI_MODE, val);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to set MDI mode");
    }
ret:
    /* set page 0 */
    val = 0;
    /* likely routine: goal_drvBusMdioNetcWrite(bus, addr, reg, value, len) */
    res = pCmd->pIface->bus.write(&pCmd->pIface->bus, pCmd->pIface->addr, PHY_PAGE_SELECT_REG, (char *) &val, 2);
    if (GOAL_RES_ERR(res)) {
	    goal_logErr("failed to reset Phy page to 0, port %"FMT_u32"VSC8541", pCmd->pIface->addr);
    }
    return res;
}

#if 0
static GOAL_STATUS_T goal_targetPhyVsc8541MdiState(
    GOAL_ETH_CMD_DATA_T *pCmd                   /**< command data */
)
{
    GOAL_STATUS_T res;                          /* result */
    uint32_t val;                               /* register value */

    if (GOAL_FALSE == pCmd->flgSet) {
        /* read state */
        res = goal_targetPhyVsc8541RegGet(pCmd->pIface, GOAL_PHY_VSC8541_REG_MDI_STATE_GET, &val);
        if (GOAL_RES_ERR(res)) {
            return res;
        }

        /* translate value to GOAL enum */
        val = (GOAL_PHY_VSC8541_MDI_STATE_MDI == val) ? GOAL_ETH_MDI_STATE_UNCROSSED : GOAL_ETH_MDI_STATE_CROSSED;

        /* report current MDI state */
        *pCmd->data.pStateMdi = val;

        return GOAL_OK;
    }

    /* write state */
    res = goal_targetPhyVsc8541RegGet(pCmd->pIface, GOAL_PHY_VSC8541_REG_MDI_STATE_SET, &val);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    if (((GOAL_ETH_MDI_STATE_UNCROSSED == val) && (GOAL_ETH_MDI_STATE_DEFAULT == *pCmd->data.pStateMdi)) ||
        (val == *pCmd->data.pStateMdi)) {
            /* requested state is already set */
            return GOAL_OK;
    }

    /* apply new state */
    switch (*pCmd->data.pStateMdi) {

        case GOAL_ETH_MDI_STATE_DEFAULT:
        case GOAL_ETH_MDI_STATE_UNCROSSED:
            val = GOAL_PHY_VSC8541_MDI_STATE_MDI;
            break;

        case GOAL_ETH_MDI_STATE_CROSSED:
            val = GOAL_PHY_VSC8541_MDI_STATE_MDIX;
            break;

        default:
            goal_logErr("unknown MDI state");
            return GOAL_ERR_UNSUPPORTED;
    }

    res = goal_targetPhyVsc8541RegSet(pCmd->pIface, GOAL_PHY_VSC8541_REG_MDI_STATE_SET, val);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to set MDI state");
    }

    return res;
}
#endif

static GOAL_STATUS_T goal_targetPhyVsc8541LinkStateGet(
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

    res = goal_targetPhyVsc8541RegGet(pCmd->pIface, GOAL_REG_LINK_STATUS, &state);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    *pCmd->data.pStateLink = (GOAL_PHY_VSC8541_LINK_STATUS_UP == state) ? GOAL_ETH_STATE_UP : GOAL_ETH_STATE_DOWN;

    return GOAL_OK;
}
