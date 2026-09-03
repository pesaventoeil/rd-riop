/** @file
 *
 * @brief Interface Info Example
 *
 * This application polls the Ethernet interfaces for changes in link state,
 * duplex and speed. If a change is detected, it waits the
 * APPL_ETH_CHANGE_DELAY delay and checks again to avoid flicker.
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

#include "goal_includes.h"


/****************************************************************************/
/* Defines */
/****************************************************************************/
#define APPL_ETH_CHANGE_DELAY   (500 * GOAL_TIMER_MSEC) /**< 500 ms stabilization delay */


/****************************************************************************/
/* Structures */
/****************************************************************************/
typedef struct {
    uint32_t stateLink;                         /**< link state */
    uint32_t speed;                             /**< port speed */
    uint32_t duplex;                            /**< port duplex */
    uint32_t stateMdi;                          /**< MDI state */
    uint32_t modeMdi;                           /**< MDI mode */
} IFACE_INFO_T;


/****************************************************************************/
/* Variables */
/****************************************************************************/
static IFACE_INFO_T *pIfaceInfo;                /**< interface info array */
static GOAL_TIMESTAMP_T *pIfaceTimeout;         /**< interface timeout array */
static uint32_t ports = 0;                      /**< count of ports */


/****************************************************************************/
/** Application Setup
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T appl_setup(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    /* get port count */
    res = goal_ethPortsGet(&ports);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("error while retrieving count of ports");
        return res;
    }

    /* allocate interface info array */
    res = goal_memCalloc(&pIfaceInfo, ports * sizeof(IFACE_INFO_T));
    if (GOAL_RES_ERR(res)) {
        goal_logErr("error while allocating interface info array");
        return res;
    }

    /* allocate interface timeout array */
    res = goal_memCalloc(&pIfaceTimeout, ports * sizeof(GOAL_TIMESTAMP_T));
    if (GOAL_RES_ERR(res)) {
        goal_logErr("error while allocating interface timeout array");
        return res;
    }

    return res;
}


/****************************************************************************/
/** Application Loop
 */
void appl_loop(
    void
)
{
    GOAL_STATUS_T res;                          /* result */
    uint32_t cnt;                               /* counter */
    IFACE_INFO_T ifaceCur;                      /* current iface info */

    /* iterate over all interface and show information */
    for (cnt = 0; cnt < ports; cnt++) {

        res = goal_ethLinkSpeedGet(cnt, &ifaceCur.speed);
        if (GOAL_OK != res) {
            goal_logErr("port %" FMT_u32 ": error while retrieving link speed", cnt);
            continue;
        }

        res = goal_ethLinkDuplexGet(cnt, &ifaceCur.duplex);
        if (GOAL_OK != res) {
            goal_logErr("port %" FMT_u32 ": error while retrieving link duplex", cnt);
            continue;
        }

        res = goal_ethLinkStateGet(cnt, &ifaceCur.stateLink);
        if (GOAL_OK != res) {
            goal_logErr("port %" FMT_u32 ": error while retrieving link state", cnt);
            continue;
        }

        res = goal_ethMdiStateGet(cnt, &ifaceCur.stateMdi);
        if (GOAL_OK != res) {
            goal_logErr("port %" FMT_u32 ": error while retrieving MDI state", cnt);
            continue;
        }

        res = goal_ethMdiModeGet(cnt, &ifaceCur.modeMdi);
        if (GOAL_OK != res) {
            goal_logErr("port %" FMT_u32 ": error while retrieving MDI mode", cnt);
            continue;
        }

        /* compare previous and current state */
        if (0 != GOAL_MEMCMP(&ifaceCur, &pIfaceInfo[cnt], sizeof(IFACE_INFO_T))) {

            /* if no timeout is set, start new wait cycle */
            if (!pIfaceTimeout[cnt]) {
                pIfaceTimeout[cnt] = goal_timerTsGet() + APPL_ETH_CHANGE_DELAY;
                continue;
            }

            /* check if stabilization timeout is reached */
            if (goal_timerTsGet() < pIfaceTimeout[cnt]) {
                continue;
            }

            /* show state change */
            goal_logInfo("----------------");
            goal_logInfo("port %" FMT_u32 " state changed", cnt);

            if (ifaceCur.stateLink != pIfaceInfo[cnt].stateLink) {
                goal_logInfo("-> link state: %s (%" FMT_u32 ") -> %s (%" FMT_u32 ")",
                             (GOAL_ETH_STATE_DEFAULT == pIfaceInfo[cnt].stateLink) ? "default" : (
                             (GOAL_ETH_STATE_UNKNOWN == pIfaceInfo[cnt].stateLink) ? "unknown" : (
                             (GOAL_ETH_STATE_UP == pIfaceInfo[cnt].stateLink) ? "up" : "down")),
                             pIfaceInfo[cnt].stateLink,
                             (GOAL_ETH_STATE_DEFAULT == ifaceCur.stateLink) ? "default" : (
                             (GOAL_ETH_STATE_UNKNOWN == ifaceCur.stateLink) ? "unknown" : (
                             (GOAL_ETH_STATE_UP == ifaceCur.stateLink) ? "up" : "down")),
                             ifaceCur.stateLink);
            }

            if (ifaceCur.speed != pIfaceInfo[cnt].speed) {
                goal_logInfo("-> link speed: %" FMT_u32 " -> %" FMT_u32,
                             pIfaceInfo[cnt].speed, ifaceCur.speed);
            }

            if (ifaceCur.duplex != pIfaceInfo[cnt].duplex) {
                goal_logInfo("-> link duplex: %s (%" FMT_u32 ") -> %s (%" FMT_u32 ")",
                             (GOAL_ETH_DUPLEX_DEFAULT == pIfaceInfo[cnt].duplex) ? "default" : (
                             (GOAL_ETH_DUPLEX_UNKNOWN == pIfaceInfo[cnt].duplex) ? "unknown" : (
                             (GOAL_ETH_DUPLEX_FULL == pIfaceInfo[cnt].duplex) ? "full" : "half")),
                             pIfaceInfo[cnt].duplex,
                             (GOAL_ETH_DUPLEX_DEFAULT == ifaceCur.duplex) ? "default" : (
                             (GOAL_ETH_DUPLEX_UNKNOWN == ifaceCur.duplex) ? "unknown" : (
                             (GOAL_ETH_DUPLEX_FULL == ifaceCur.duplex) ? "full" : "half")),
                             ifaceCur.duplex);
            }

            if (ifaceCur.stateMdi != pIfaceInfo[cnt].stateMdi) {
                goal_logInfo("-> MDI state: %s (%" FMT_u32 ") -> %s (%" FMT_u32 ")",
                             (GOAL_ETH_MDI_STATE_DEFAULT == pIfaceInfo[cnt].stateMdi) ? "default" : (
                             (GOAL_ETH_MDI_STATE_UNKNOWN == pIfaceInfo[cnt].stateMdi) ? "unknown" : (
                             (GOAL_ETH_MDI_STATE_UNCROSSED == pIfaceInfo[cnt].stateMdi) ? "uncrossed" : "crossed")),
                             pIfaceInfo[cnt].stateMdi,
                             (GOAL_ETH_MDI_STATE_DEFAULT == ifaceCur.stateMdi) ? "default" : (
                             (GOAL_ETH_MDI_STATE_UNKNOWN == ifaceCur.stateMdi) ? "unknown" : (
                             (GOAL_ETH_MDI_STATE_UNCROSSED == ifaceCur.stateMdi) ? "uncrossed" : "crossed")),
                             ifaceCur.stateMdi);
            }

            if (ifaceCur.modeMdi != pIfaceInfo[cnt].modeMdi) {
                goal_logInfo("-> MDI mode: %s (%" FMT_u32 ") -> %s (%" FMT_u32 ")",
                             (GOAL_ETH_MDI_MODE_DEFAULT == pIfaceInfo[cnt].modeMdi) ? "default" : (
                             (GOAL_ETH_MDI_MODE_UNKNOWN == pIfaceInfo[cnt].modeMdi) ? "unknown" : (
                             (GOAL_ETH_MDI_MODE_AUTO == pIfaceInfo[cnt].modeMdi) ? "auto" : "manual")),
                             pIfaceInfo[cnt].modeMdi,
                             (GOAL_ETH_MDI_MODE_DEFAULT == ifaceCur.modeMdi) ? "default" : (
                             (GOAL_ETH_MDI_MODE_UNKNOWN == ifaceCur.modeMdi) ? "unknown" : (
                             (GOAL_ETH_MDI_MODE_AUTO == ifaceCur.modeMdi) ? "auto" : "manual")),
                             ifaceCur.modeMdi);
            }

            GOAL_MEMCPY(&pIfaceInfo[cnt], &ifaceCur, sizeof(IFACE_INFO_T));
        }

        /* clear timeout */
        pIfaceTimeout[cnt] = 0;
    }
}
