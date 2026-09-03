/** @file
 *
 * @brief Ethernet CLI command
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

#ifndef GOAL_ETH_CLI_H
#define GOAL_ETH_CLI_H


/****************************************************************************/
/* Structs */
/****************************************************************************/
/**< VLAN in mode */
typedef struct {
    char *strMode;                              /**< mode string */
    GOAL_ETH_VLANIN_MODE_T inmode;              /**< in mode */
} GOAL_ETH_CLI_VLANINMODE_T;


/**< VLAN out mode */
typedef struct {
    char *strMode;                              /**< mode string */
    GOAL_ETH_VLANOUT_MODE_T outmode;            /**< out mode */
} GOAL_ETH_CLI_VLANOUTMODE_T;


/**< Speed mode */
typedef struct {
    char *strMode;                              /**< mode string */
    uint32_t speed;                             /**< speed */
} GOAL_ETH_CLI_SPEEDMODE_T;


/**< Link mode */
typedef struct {
    char *strMode;                              /**< mode string */
    uint32_t link;                              /**< link */
} GOAL_ETH_CLI_LINKSTATE_T;


/**< Duplex mode */
typedef struct {
    char *strMode;                              /**< mode string */
    uint32_t duplex;                            /**< duplex mode */
} GOAL_ETH_CLI_DUPLEXMODE_T;

/**< Autonegotiation mode */
typedef struct {
    char *strMode;                              /**< mode string */
    uint32_t autoneg;                           /**< autonegotiation mode */
} GOAL_ETH_CLI_AUTONEG_T;

/**< QoS mode */
typedef struct {
    char *strMode;                              /**< mode string */
    uint32_t qosMode;                           /**< QoS mode */
} GOAL_ETH_CLI_QOSMODE_T;

/**< Mirror mode */
typedef struct {
    char *strMode;                              /**< mode string */
    GOAL_ETH_MIRR_MODE_T mirrortype;            /**< mirror */
} GOAL_ETH_CLI_MIRRORMODE_T;

/**< MDI state */
typedef struct {
    char *strState;                             /**< state string */
    uint32_t state;                             /**< state value */
} GOAL_ETH_CLI_MDISTATE_T;

/**< MDI mode */
typedef struct {
    char *strMode;                             /**< state string */
    uint32_t mode;                             /**< state value */
} GOAL_ETH_CLI_MDIMODE_T;


/****************************************************************************/
/* Structs */
/****************************************************************************/
GOAL_STATUS_T goal_ethInitCli(
    void
);


#endif /* GOAL_ETH_CLI_H */
