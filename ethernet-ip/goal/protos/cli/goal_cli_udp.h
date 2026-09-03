/** @file
 *
 * @brief GOAL CLI over UDP
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

#include <goal_config.h>

#if (GOAL_CONFIG_CLI_UDP == 1)
GOAL_STATUS_T goal_cliInitUdp(
    void
);

GOAL_NET_CHAN_T *goal_cliUdpChanGet(
    void
);

GOAL_NET_ADDR_T *goal_cliUdpAddrGet(
    void
);

GOAL_STATUS_T goal_cliUdpCharPut(
    char c                                      /**< character */
);

GOAL_STATUS_T goal_cliUdpCmdCharGet(
    char *pKey                                  /**< key */
);
#endif /* GOAL_CONFIG_CLI_UDP */
