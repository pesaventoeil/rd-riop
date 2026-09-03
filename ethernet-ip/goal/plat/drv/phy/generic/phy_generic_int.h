/** @file
 *
 * @brief Generic PHY Driver - Internal
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

#ifndef GOAL_PHY_GENERIC_INT_H
#define GOAL_PHY_GENERIC_INT_H


/****************************************************************************/
/* Configuration */
/****************************************************************************/
#ifndef GOAL_CONFIG_PHY_MARVELL_88E1512
#  define GOAL_CONFIG_PHY_MARVELL_88E1512 0
#endif

#ifndef GOAL_CONFIG_PHY_TI_DP83822
#  define GOAL_CONFIG_PHY_TI_DP83822 0
#endif

#ifndef GOAL_CONFIG_PHY_MC_KSZ8041
#  define GOAL_CONFIG_PHY_MC_KSZ8041 0
#endif

#ifndef GOAL_CONFIG_PHY_MC_KSZ8081
#  define GOAL_CONFIG_PHY_MC_KSZ8081 0
#endif

#ifndef GOAL_CONFIG_PHY_MC_LAN8720A
#  define GOAL_CONFIG_PHY_MC_LAN8720A 0
#endif

#ifndef GOAL_CONFIG_PHY_SMSC_LAN8742A
#  define GOAL_CONFIG_PHY_SMSC_LAN8742A 0
#endif

#ifndef GOAL_CONFIG_PHY_RN_RIN32M3
#  define GOAL_CONFIG_PHY_RN_RIN32M3 0
#endif


/****************************************************************************/
/* PHY driver headers */
/****************************************************************************/
#if GOAL_CONFIG_PHY_DETECTION == 1
#  if GOAL_CONFIG_PHY_MARVELL_88E1512 == 1
#    include <drv/phy/marvell_88e1512/marvell_88e1512.h>
#  endif
#  if GOAL_CONFIG_PHY_TI_DP83822 == 1
#    include <drv/phy/ti_dp83822/ti_dp83822.h>
#  endif
#  if GOAL_CONFIG_PHY_MC_KSZ8041 == 1
#    include <drv/phy/ksz8041/ksz8041.h>
#  endif
#  if GOAL_CONFIG_PHY_MC_KSZ8081 == 1
#    include <drv/phy/ksz8081/ksz8081.h>
#  endif
#  if GOAL_CONFIG_PHY_RN_RIN32M3 == 1
#    include <drv/phy/rin32m3/phy_rin32m3.h>
#  endif
#  if GOAL_CONFIG_PHY_SMSC_LAN8742A == 1
#    include <drv/phy/smsc_lan8742a/smsc_lan8742a.h>
#  endif
#endif


/****************************************************************************/
/* PHY data */
/****************************************************************************/
/* Ethernet Phy register table
 *
 * This table is used as a generic interface to read and write specific register
 * fields. They are accessed via IDs. The table provides the register address
 * and the location within the register.
 */
#define GOAL_PHY_GENERIC_RV_LIST \
    /*                  reg,  valId,                                    shift, mask,  flags */  \
    GOAL_PHY_GENERIC_RV(0x00, GOAL_PHY_GENERIC_REG_00                   ,  0, 0xFFFF, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x01, GOAL_PHY_GENERIC_REG_01                   ,  0, 0xFFFF, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x02, GOAL_PHY_GENERIC_REG_02                   ,  0, 0xFFFF, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x03, GOAL_PHY_GENERIC_REG_03                   ,  0, 0xFFFF, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x04, GOAL_PHY_GENERIC_REG_04                   ,  0, 0xFFFF, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x05, GOAL_PHY_GENERIC_REG_05                   ,  0, 0xFFFF, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x06, GOAL_PHY_GENERIC_REG_06                   ,  0, 0xFFFF, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x07, GOAL_PHY_GENERIC_REG_07                   ,  0, 0xFFFF, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x08, GOAL_PHY_GENERIC_REG_08                   ,  0, 0xFFFF, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x09, GOAL_PHY_GENERIC_REG_09                   ,  0, 0xFFFF, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x0a, GOAL_PHY_GENERIC_REG_10                   ,  0, 0xFFFF, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x0b, GOAL_PHY_GENERIC_REG_11                   ,  0, 0xFFFF, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x0c, GOAL_PHY_GENERIC_REG_12                   ,  0, 0xFFFF, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x0d, GOAL_PHY_GENERIC_REG_13                   ,  0, 0xFFFF, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x0e, GOAL_PHY_GENERIC_REG_14                   ,  0, 0xFFFF, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x0f, GOAL_PHY_GENERIC_REG_15                   ,  0, 0xFFFF, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x10, GOAL_PHY_GENERIC_REG_16                   ,  0, 0xFFFF, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x11, GOAL_PHY_GENERIC_REG_17                   ,  0, 0xFFFF, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x12, GOAL_PHY_GENERIC_REG_18                   ,  0, 0xFFFF, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x13, GOAL_PHY_GENERIC_REG_19                   ,  0, 0xFFFF, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x14, GOAL_PHY_GENERIC_REG_20                   ,  0, 0xFFFF, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x15, GOAL_PHY_GENERIC_REG_21                   ,  0, 0xFFFF, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x16, GOAL_PHY_GENERIC_REG_22                   ,  0, 0xFFFF, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x17, GOAL_PHY_GENERIC_REG_23                   ,  0, 0xFFFF, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x18, GOAL_PHY_GENERIC_REG_24                   ,  0, 0xFFFF, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x19, GOAL_PHY_GENERIC_REG_25                   ,  0, 0xFFFF, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x1a, GOAL_PHY_GENERIC_REG_26                   ,  0, 0xFFFF, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x1b, GOAL_PHY_GENERIC_REG_27                   ,  0, 0xFFFF, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x1c, GOAL_PHY_GENERIC_REG_28                   ,  0, 0xFFFF, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x1d, GOAL_PHY_GENERIC_REG_29                   ,  0, 0xFFFF, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x1e, GOAL_PHY_GENERIC_REG_30                   ,  0, 0xFFFF, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x1f, GOAL_PHY_GENERIC_REG_31                   ,  0, 0xFFFF, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x00, GOAL_PHY_GENERIC_REG_DUPLEX_MODE          ,  8, 0x0001, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x00, GOAL_PHY_GENERIC_REG_AUTONEG_RESTART      ,  9, 0x0001, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x00, GOAL_PHY_GENERIC_REG_PWR_DWN              , 11, 0x0001, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x00, GOAL_PHY_GENERIC_REG_AUTONEG_ENBL         , 12, 0x0001, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x00, GOAL_PHY_GENERIC_REG_SPEED_SELECT         ,  6, 0x0081, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x00, GOAL_PHY_GENERIC_REG_RESET                , 15, 0x0001, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x01, GOAL_PHY_GENERIC_REG_EXTEDED_CAPABL       ,  0, 0x0001, 0                             ), \
    GOAL_PHY_GENERIC_RV(0x01, GOAL_PHY_GENERIC_REG_JABBER               ,  1, 0x0001, 0                             ), \
    GOAL_PHY_GENERIC_RV(0x01, GOAL_PHY_GENERIC_REG_LINK_STATUS          ,  2, 0x0001, 0                             ), \
    GOAL_PHY_GENERIC_RV(0x01, GOAL_PHY_GENERIC_REG_AUTONEG_ABLTY        ,  3, 0x0001, 0                             ), \
    GOAL_PHY_GENERIC_RV(0x01, GOAL_PHY_GENERIC_REG_REMOTE_FAULT         ,  4, 0x0001, 0                             ), \
    GOAL_PHY_GENERIC_RV(0x01, GOAL_PHY_GENERIC_REG_AUTONEG_CMPLT        ,  5, 0x0001, 0                             ), \
    GOAL_PHY_GENERIC_RV(0x01, GOAL_PHY_GENERIC_REG_CAPABL_10HD          , 11, 0x0001, 0                             ), \
    GOAL_PHY_GENERIC_RV(0x01, GOAL_PHY_GENERIC_REG_CAPABL_10FD          , 12, 0x0001, 0                             ), \
    GOAL_PHY_GENERIC_RV(0x01, GOAL_PHY_GENERIC_REG_CAPABL_100HD         , 13, 0x0001, 0                             ), \
    GOAL_PHY_GENERIC_RV(0x01, GOAL_PHY_GENERIC_REG_CAPABL_100FD         , 14, 0x0001, 0                             ), \
    GOAL_PHY_GENERIC_RV(0x01, GOAL_PHY_GENERIC_REG_CAPABL_100T4         , 15, 0x0001, 0                             ), \
    GOAL_PHY_GENERIC_RV(0x02, GOAL_PHY_GENERIC_REG_ID1                  ,  0, 0xffff, 0                             ), \
    GOAL_PHY_GENERIC_RV(0x03, GOAL_PHY_GENERIC_REG_ID2                  ,  0, 0xffff, 0                             ), \
    GOAL_PHY_GENERIC_RV(0x04, GOAL_PHY_GENERIC_REG_AN_AD_10_HD          ,  5, 0x0001, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x04, GOAL_PHY_GENERIC_REG_AN_AD_10_FD          ,  6, 0x0001, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x04, GOAL_PHY_GENERIC_REG_AN_AD_100_HD         ,  7, 0x0001, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x04, GOAL_PHY_GENERIC_REG_AN_AD_100_FD         ,  8, 0x0001, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x06, GOAL_PHY_GENERIC_REG_LP_AN_ENABLE         ,  0, 0x0001, 0                             ), \
    GOAL_PHY_GENERIC_RV(0x06, GOAL_PHY_GENERIC_REG_PARALLEL_FAULT       ,  4, 0x0001, 0                             ), \
    GOAL_PHY_GENERIC_RV(0x09, GOAL_PHY_GENERIC_REG_AN_AD_1000_HD        ,  8, 0x0001, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x09, GOAL_PHY_GENERIC_REG_AN_AD_1000_FD        ,  9, 0x0001, GOAL_REG_FLG_RW               ), \
    GOAL_PHY_GENERIC_RV(0x0F, GOAL_PHY_GENERIC_REG_CAPABL_1000HD        , 12, 0x0001, 0                             ), \
    GOAL_PHY_GENERIC_RV(0x0F, GOAL_PHY_GENERIC_REG_CAPABL_1000FD        , 13, 0x0001, 0                             )

/* port count */
#define GOAL_PHY_GENERIC_PORT_COUNT       1

/* power down mode */
#define GOAL_PHY_GENERIC_POWER_NORMAL     0x0
#define GOAL_PHY_GENERIC_POWER_DOWN       0x1

/* Auto Negotiation */
#define GOAL_PHY_GENERIC_AUTONEG_ENABLE   0x1
#define GOAL_PHY_GENERIC_AUTONEG_DISABLE  0x0
#define GOAL_PHY_GENERIC_AUTONEG_COMPLETE 0x1
#define GOAL_PHY_GENERIC_LP_AN_ENABLED    0x1
#define GOAL_PHY_GENERIC_PARALLEL_FAULT   0x1
#define GOAL_PHY_GENERIC_AUTONEG_RESTART  0x1

/* Autonegotiation Advertisement */
#define GOAL_PHY_GENERIC_AN_AD_ON         0x1
#define GOAL_PHY_GENERIC_AN_AD_OFF        0x0

/* speed select */
#define GOAL_PHY_GENERIC_SPEED_10         0x00
#define GOAL_PHY_GENERIC_SPEED_100        0x80
#define GOAL_PHY_GENERIC_SPEED_1000       0x01

/* duplex mode */
#define GOAL_PHY_GENERIC_DUPLEX_HALF      0x0
#define GOAL_PHY_GENERIC_DUPLEX_FULL      0x1

/* PHY identifier */
#define GOAL_PHY_GENERIC_ID1_SHIFT      16      /**< PHY ID1 shift */
#define GOAL_PHY_GENERIC_ID1_MASK       0xffff0000 /**< PHY ID1 mask */
#define GOAL_PHY_GENERIC_ID2_MASK       0x0000ffff /**< PHY ID2 mask */
#define GOAL_PHY_GENERIC_ID_EMPTY       0x00000000 /**< empty PHY ID */
#define GOAL_PHY_GENERIC_ID_MAX         0xffffffff /**< max PHY ID */


/****************************************************************************/
/* Structures */
/****************************************************************************/
typedef struct {
    uint32_t idPhy;                             /**< PHY id */
    GOAL_STATUS_T (* funcInit)(GOAL_ETH_IFACE_T *); /**< init function */
} GOAL_PHY_GENERIC_DETECT_T;


/****************************************************************************/
/* PHY Auto-Detection */
/****************************************************************************/

#if GOAL_CONFIG_PHY_DETECTION == 1
/**< detection list */
static GOAL_PHY_GENERIC_DETECT_T phyList[] = {
#  if GOAL_CONFIG_PHY_MARVELL_88E1512 == 1
    /* Marvell 88E1512 */
    { 0x01410dd4, goal_targetMarv88E1512Open },
#  endif
#  if GOAL_CONFIG_PHY_TI_DP83822 == 1
    /* Texas Instruments DP83822 */
    { 0x2000a240, goal_targetTiDp83822Open },
#  endif
#  if GOAL_CONFIG_PHY_MC_KSZ8041 == 1
    /* Microchip KSZ 8041 */
    { 0x00220013, goal_targetPhyKsz8041Open },
    { 0x00221513, goal_targetPhyKsz8041Open },
#  endif
#  if GOAL_CONFIG_PHY_MC_KSZ8081 == 1
    /* Microchip KSZ 8081 */
    { 0x00221560, goal_targetPhyKsz8081Open },
#  endif
#  if GOAL_CONFIG_PHY_RN_RIN32M3 == 1
    { 0x00332002, goal_targetPhyRin32m3Open },
#  endif
#  if GOAL_CONFIG_PHY_MC_LAN8720A == 1
    /* Microchip LAN8720A */
    { 0x0007C0F0, goal_targetPhyLan8720aOpen },
#  endif
#  if GOAL_CONFIG_PHY_SMSC_LAN8742A == 1
    /* SMSC LAN8742A */
    { 0x0007C130, goal_tgtPhySmscLan8742aOpen },
    /* LAN9354 internal PHYs */
    { 0x0007C140, goal_tgtPhySmscLan8742aOpen },
#  endif
    { 0, NULL }
};
#endif /* GOAL_CONFIG_PHY_DETECTION == 1 */

#endif /* GOAL_PHY_GENERIC_INT_H */
