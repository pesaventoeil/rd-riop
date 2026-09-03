/** @file
 *
 * @brief GOAL lm logging db list
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
#ifndef GOAL_LM_DB_H
#define GOAL_LM_DB_H


/****************************************************************************/
/* Defines */
/****************************************************************************/

#define GOAL_ID_ZERO 0

/**< goal lm logging entries */
static const GOAL_LM_DB_ENTRY_T goal_lm_db[] = {
    {
        .id = 1,
        .modId = GOAL_ID_DD,
        .textId = 2,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_UINT32,
        .p1Pos = 47,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Error while opening UDP server channel on port $1",
        .seg = {
            { 0, 47 },
            { 49, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 2,
        .modId = GOAL_ID_DD,
        .textId = 3,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Error while setting UDP channel to non-blocking",
        .seg = {
            { 0, 47 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 3,
        .modId = GOAL_ID_DD,
        .textId = 4,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Error while setting UDP channel to receiving broadcasts",
        .seg = {
            { 0, 55 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 4,
        .modId = GOAL_ID_DD,
        .textId = 5,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Error while enabling UDP channel",
        .seg = {
            { 0, 32 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 5,
        .modId = GOAL_ID_DD,
        .textId = 6,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Registration of DD configuration variables failed",
        .seg = {
            { 0, 49 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 6,
        .modId = GOAL_ID_DD,
        .textId = 7,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Error reading DD configuration",
        .seg = {
            { 0, 30 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 7,
        .modId = GOAL_ID_DD,
        .textId = 7,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Error reading DD configuration",
        .seg = {
            { 0, 30 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 8,
        .modId = GOAL_ID_DD,
        .textId = 7,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Error reading DD configuration",
        .seg = {
            { 0, 30 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 9,
        .modId = GOAL_ID_DD,
        .textId = 7,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Error reading DD configuration",
        .seg = {
            { 0, 30 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 10,
        .modId = GOAL_ID_DD,
        .textId = 8,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Error setting variable",
        .seg = {
            { 0, 22 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 11,
        .modId = GOAL_ID_DD,
        .textId = 8,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Error setting variable",
        .seg = {
            { 0, 22 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 12,
        .modId = GOAL_ID_DD,
        .textId = 8,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Error setting variable",
        .seg = {
            { 0, 22 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 13,
        .modId = GOAL_ID_DD,
        .textId = 8,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Error setting variable",
        .seg = {
            { 0, 22 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 14,
        .modId = GOAL_ID_DD,
        .textId = 9,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Failed to set IP address",
        .seg = {
            { 0, 24 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 15,
        .modId = GOAL_ID_DD,
        .textId = 10,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_UINT32,
        .p1Pos = 9,
        .p2Type = LM_UINT32,
        .p2Pos = 12,
        .text = "Variable $1:$2 was changed",
        .seg = {
            { 0, 9 },
            { 11, 1 },
            { 14, 12 }
        }
    },
    {
        .id = 16,
        .modId = GOAL_ID_APPL,
        .textId = 11,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_UINT32,
        .p1Pos = 13,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "task: time = $1",
        .seg = {
            { 0, 13 },
            { 15, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 17,
        .modId = GOAL_ID_LM,
        .textId = 12,
        .severity = GOAL_LOG_SEV_DEBUG,
        .p1Type = LM_GENERIC,
        .p1Pos = 0,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "$1",
        .seg = {
            { 0, 0 },
            { 2, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 18,
        .modId = GOAL_ID_LM,
        .textId = 12,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_GENERIC,
        .p1Pos = 0,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "$1",
        .seg = {
            { 0, 0 },
            { 2, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 19,
        .modId = GOAL_ID_LM,
        .textId = 12,
        .severity = GOAL_LOG_SEV_WARNING,
        .p1Type = LM_GENERIC,
        .p1Pos = 0,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "$1",
        .seg = {
            { 0, 0 },
            { 2, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 20,
        .modId = GOAL_ID_LM,
        .textId = 12,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_GENERIC,
        .p1Pos = 0,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "$1",
        .seg = {
            { 0, 0 },
            { 2, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 21,
        .modId = GOAL_ID_LM,
        .textId = 12,
        .severity = GOAL_LOG_SEV_EXCEPTION,
        .p1Type = LM_GENERIC,
        .p1Pos = 0,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "$1",
        .seg = {
            { 0, 0 },
            { 2, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 22,
        .modId = GOAL_ID_DD,
        .textId = 13,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_UINT32,
        .p1Pos = 34,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Started goal dd with feature mask $1",
        .seg = {
            { 0, 34 },
            { 36, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 23,
        .modId = GOAL_ID_DD,
        .textId = 14,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Failed to start goal dd",
        .seg = {
            { 0, 23 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 24,
        .modId = GOAL_ID_ETH,
        .textId = 15,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_MAC,
        .p1Pos = 4,
        .p2Type = LM_MAC,
        .p2Pos = 13,
        .text = "src $1 : dst $2",
        .seg = {
            { 0, 4 },
            { 6, 7 },
            { 15, 0 }
        }
    },
    {
        .id = 25,
        .modId = GOAL_ID_ETH,
        .textId = 15,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_MAC,
        .p1Pos = 4,
        .p2Type = LM_MAC,
        .p2Pos = 13,
        .text = "src $1 : dst $2",
        .seg = {
            { 0, 4 },
            { 6, 7 },
            { 15, 0 }
        }
    },
    {
        .id = 26,
        .modId = GOAL_ID_ETH,
        .textId = 16,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_MAC,
        .p1Pos = 25,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "added multicast address: $1",
        .seg = {
            { 0, 25 },
            { 27, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 27,
        .modId = GOAL_ID_ETH,
        .textId = 17,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_MAC,
        .p1Pos = 33,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to add multicast address: $1",
        .seg = {
            { 0, 33 },
            { 35, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 28,
        .modId = GOAL_ID_ETH,
        .textId = 18,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_MAC,
        .p1Pos = 27,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "removed multicast address: $1",
        .seg = {
            { 0, 27 },
            { 29, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 29,
        .modId = GOAL_ID_ETH,
        .textId = 19,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_MAC,
        .p1Pos = 36,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to remove multicast address: $1",
        .seg = {
            { 0, 36 },
            { 38, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 30,
        .modId = GOAL_ID_NET,
        .textId = 20,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_IPV4,
        .p1Pos = 12,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "ip address: $1",
        .seg = {
            { 0, 12 },
            { 14, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 31,
        .modId = GOAL_ID_NET,
        .textId = 21,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_IPV4,
        .p1Pos = 9,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "netmask: $1",
        .seg = {
            { 0, 9 },
            { 11, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 32,
        .modId = GOAL_ID_NET,
        .textId = 22,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_IPV4,
        .p1Pos = 9,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "gateway: $1",
        .seg = {
            { 0, 9 },
            { 11, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 33,
        .modId = GOAL_ID_NET,
        .textId = 23,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_BOOLEAN,
        .p1Pos = 15,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "IP valid flag: $1",
        .seg = {
            { 0, 15 },
            { 17, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 34,
        .modId = GOAL_ID_NET,
        .textId = 20,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_IPV4,
        .p1Pos = 12,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "ip address: $1",
        .seg = {
            { 0, 12 },
            { 14, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 35,
        .modId = GOAL_ID_NET,
        .textId = 21,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_IPV4,
        .p1Pos = 9,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "netmask: $1",
        .seg = {
            { 0, 9 },
            { 11, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 36,
        .modId = GOAL_ID_NET,
        .textId = 22,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_IPV4,
        .p1Pos = 9,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "gateway: $1",
        .seg = {
            { 0, 9 },
            { 11, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 37,
        .modId = GOAL_ID_NET,
        .textId = 23,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_BOOLEAN,
        .p1Pos = 15,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "IP valid flag: $1",
        .seg = {
            { 0, 15 },
            { 17, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 38,
        .modId = GOAL_ID_DEFAULT,
        .textId = 24,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_STRING0,
        .p1Pos = 6,
        .p2Type = LM_INT8,
        .p2Pos = 17,
        .text = "type: $1, value: $2",
        .seg = {
            { 0, 6 },
            { 8, 9 },
            { 19, 0 }
        }
    },
    {
        .id = 39,
        .modId = GOAL_ID_DEFAULT,
        .textId = 24,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_STRING0,
        .p1Pos = 6,
        .p2Type = LM_UINT8,
        .p2Pos = 17,
        .text = "type: $1, value: $2",
        .seg = {
            { 0, 6 },
            { 8, 9 },
            { 19, 0 }
        }
    },
    {
        .id = 40,
        .modId = GOAL_ID_DEFAULT,
        .textId = 24,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_STRING0,
        .p1Pos = 6,
        .p2Type = LM_INT16,
        .p2Pos = 17,
        .text = "type: $1, value: $2",
        .seg = {
            { 0, 6 },
            { 8, 9 },
            { 19, 0 }
        }
    },
    {
        .id = 41,
        .modId = GOAL_ID_DEFAULT,
        .textId = 24,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_STRING0,
        .p1Pos = 6,
        .p2Type = LM_UINT16,
        .p2Pos = 17,
        .text = "type: $1, value: $2",
        .seg = {
            { 0, 6 },
            { 8, 9 },
            { 19, 0 }
        }
    },
    {
        .id = 42,
        .modId = GOAL_ID_DEFAULT,
        .textId = 24,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_STRING0,
        .p1Pos = 6,
        .p2Type = LM_INT32,
        .p2Pos = 17,
        .text = "type: $1, value: $2",
        .seg = {
            { 0, 6 },
            { 8, 9 },
            { 19, 0 }
        }
    },
    {
        .id = 43,
        .modId = GOAL_ID_DEFAULT,
        .textId = 24,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_STRING0,
        .p1Pos = 6,
        .p2Type = LM_UINT32,
        .p2Pos = 17,
        .text = "type: $1, value: $2",
        .seg = {
            { 0, 6 },
            { 8, 9 },
            { 19, 0 }
        }
    },
    {
        .id = 44,
        .modId = GOAL_ID_DEFAULT,
        .textId = 24,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_STRING0,
        .p1Pos = 6,
        .p2Type = LM_INT64,
        .p2Pos = 17,
        .text = "type: $1, value: $2",
        .seg = {
            { 0, 6 },
            { 8, 9 },
            { 19, 0 }
        }
    },
    {
        .id = 45,
        .modId = GOAL_ID_DEFAULT,
        .textId = 24,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_STRING0,
        .p1Pos = 6,
        .p2Type = LM_UINT64,
        .p2Pos = 17,
        .text = "type: $1, value: $2",
        .seg = {
            { 0, 6 },
            { 8, 9 },
            { 19, 0 }
        }
    },
    {
        .id = 46,
        .modId = GOAL_ID_DEFAULT,
        .textId = 24,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_STRING0,
        .p1Pos = 6,
        .p2Type = LM_FLOAT,
        .p2Pos = 17,
        .text = "type: $1, value: $2",
        .seg = {
            { 0, 6 },
            { 8, 9 },
            { 19, 0 }
        }
    },
    {
        .id = 47,
        .modId = GOAL_ID_DEFAULT,
        .textId = 24,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_STRING0,
        .p1Pos = 6,
        .p2Type = LM_PTR,
        .p2Pos = 17,
        .text = "type: $1, value: $2",
        .seg = {
            { 0, 6 },
            { 8, 9 },
            { 19, 0 }
        }
    },
    {
        .id = 48,
        .modId = GOAL_ID_DEFAULT,
        .textId = 24,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_STRING0,
        .p1Pos = 6,
        .p2Type = LM_PTRDIFF,
        .p2Pos = 17,
        .text = "type: $1, value: $2",
        .seg = {
            { 0, 6 },
            { 8, 9 },
            { 19, 0 }
        }
    },
    {
        .id = 49,
        .modId = GOAL_ID_DEFAULT,
        .textId = 24,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_STRING0,
        .p1Pos = 6,
        .p2Type = LM_IPV4,
        .p2Pos = 17,
        .text = "type: $1, value: $2",
        .seg = {
            { 0, 6 },
            { 8, 9 },
            { 19, 0 }
        }
    },
    {
        .id = 50,
        .modId = GOAL_ID_DEFAULT,
        .textId = 24,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_STRING0,
        .p1Pos = 6,
        .p2Type = LM_MAC,
        .p2Pos = 17,
        .text = "type: $1, value: $2",
        .seg = {
            { 0, 6 },
            { 8, 9 },
            { 19, 0 }
        }
    },
    {
        .id = 51,
        .modId = GOAL_ID_DEFAULT,
        .textId = 24,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_STRING0,
        .p1Pos = 6,
        .p2Type = LM_BOOLEAN,
        .p2Pos = 17,
        .text = "type: $1, value: $2",
        .seg = {
            { 0, 6 },
            { 8, 9 },
            { 19, 0 }
        }
    },
    {
        .id = 52,
        .modId = GOAL_ID_DEFAULT,
        .textId = 24,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_STRING0,
        .p1Pos = 6,
        .p2Type = LM_GENERIC,
        .p2Pos = 17,
        .text = "type: $1, value: $2",
        .seg = {
            { 0, 6 },
            { 8, 9 },
            { 19, 0 }
        }
    },
    {
        .id = 53,
        .modId = GOAL_ID_DEFAULT,
        .textId = 24,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_STRING0,
        .p1Pos = 6,
        .p2Type = LM_STRING0,
        .p2Pos = 17,
        .text = "type: $1, value: $2",
        .seg = {
            { 0, 6 },
            { 8, 9 },
            { 19, 0 }
        }
    },
    {
        .id = 54,
        .modId = GOAL_ID_CTC,
        .textId = 25,
        .severity = GOAL_LOG_SEV_EXCEPTION,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to remove data from ringbuffer",
        .seg = {
            { 0, 37 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 55,
        .modId = GOAL_ID_CCM,
        .textId = 26,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "eeprom access suceeded",
        .seg = {
            { 0, 22 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 56,
        .modId = GOAL_ID_CCM,
        .textId = 27,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "eeprom access failed",
        .seg = {
            { 0, 20 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 57,
        .modId = GOAL_ID_CCM,
        .textId = 28,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Update denied by AC",
        .seg = {
            { 0, 19 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 58,
        .modId = GOAL_ID_CCM,
        .textId = 29,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_UINT32,
        .p1Pos = 18,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "raw data size   = $1 bytes",
        .seg = {
            { 0, 18 },
            { 20, 6 },
            { 0, 0 }
        }
    },
    {
        .id = 59,
        .modId = GOAL_ID_CCM,
        .textId = 30,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_UINT32,
        .p1Pos = 18,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "aligning bytes  = $1 bytes",
        .seg = {
            { 0, 18 },
            { 20, 6 },
            { 0, 0 }
        }
    },
    {
        .id = 60,
        .modId = GOAL_ID_CCM,
        .textId = 31,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_UINT32,
        .p1Pos = 18,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "file rec. count = $1 records",
        .seg = {
            { 0, 18 },
            { 20, 8 },
            { 0, 0 }
        }
    },
    {
        .id = 61,
        .modId = GOAL_ID_CCM,
        .textId = 32,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_UINT32,
        .p1Pos = 18,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "align data size = $1 bytes",
        .seg = {
            { 0, 18 },
            { 20, 6 },
            { 0, 0 }
        }
    },
    {
        .id = 62,
        .modId = GOAL_ID_CCM,
        .textId = 33,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_UINT32,
        .p1Pos = 18,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "lowest address  = $1",
        .seg = {
            { 0, 18 },
            { 20, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 63,
        .modId = GOAL_ID_CCM,
        .textId = 34,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_UINT32,
        .p1Pos = 18,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "highest address = $1",
        .seg = {
            { 0, 18 },
            { 20, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 64,
        .modId = GOAL_ID_CCM,
        .textId = 35,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Commit firmware update",
        .seg = {
            { 0, 22 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 65,
        .modId = GOAL_ID_CCM,
        .textId = 36,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Enter bootloader cli",
        .seg = {
            { 0, 20 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 66,
        .modId = GOAL_ID_CCM,
        .textId = 37,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_STRING0,
        .p1Pos = 24,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Signature string found: $1",
        .seg = {
            { 0, 24 },
            { 26, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 67,
        .modId = GOAL_ID_CCM,
        .textId = 38,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_INT32,
        .p1Pos = 18,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Signature invalid $1",
        .seg = {
            { 0, 18 },
            { 20, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 68,
        .modId = GOAL_ID_CCM,
        .textId = 39,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "firmware update enabled by AC",
        .seg = {
            { 0, 29 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 69,
        .modId = GOAL_ID_CCM,
        .textId = 40,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "firmware update disabled by AC",
        .seg = {
            { 0, 30 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 70,
        .modId = GOAL_ID_MEM,
        .textId = 41,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to create memory area lock",
        .seg = {
            { 0, 33 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 71,
        .modId = GOAL_ID_MEM,
        .textId = 42,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_UINT32,
        .p1Pos = 20,
        .p2Type = LM_UINT32,
        .p2Pos = 23,
        .text = "fixed memory usage: $1/$2 bytes",
        .seg = {
            { 0, 20 },
            { 22, 1 },
            { 25, 6 }
        }
    },
    {
        .id = 72,
        .modId = GOAL_ID_MEM,
        .textId = 43,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_UINT32,
        .p1Pos = 21,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "fixed memory usage: ($1%)",
        .seg = {
            { 0, 21 },
            { 23, 2 },
            { 0, 0 }
        }
    },
    {
        .id = 73,
        .modId = GOAL_ID_MEM,
        .textId = 44,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "GOAL fully initialized -> alloc denied",
        .seg = {
            { 0, 38 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 74,
        .modId = GOAL_ID_MEM,
        .textId = 45,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_UINT32,
        .p1Pos = 19,
        .p2Type = LM_UINT32,
        .p2Pos = 35,
        .text = "could not allocate $1 bytes (gross:$2bytes), out of memory",
        .seg = {
            { 0, 19 },
            { 21, 14 },
            { 37, 21 }
        }
    },
    {
        .id = 75,
        .modId = GOAL_ID_MEM,
        .textId = 46,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "memory free not implemented",
        .seg = {
            { 0, 27 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 76,
        .modId = GOAL_ID_MEM,
        .textId = 47,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "main pre and post fence not detected",
        .seg = {
            { 0, 36 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 77,
        .modId = GOAL_ID_MEM,
        .textId = 48,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_PTR,
        .p1Pos = 24,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "pre-fence error at mem: $1",
        .seg = {
            { 0, 24 },
            { 26, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 78,
        .modId = GOAL_ID_MEM,
        .textId = 49,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_PTR,
        .p1Pos = 25,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "post-fence error at mem: $1",
        .seg = {
            { 0, 25 },
            { 27, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 79,
        .modId = GOAL_ID_MEM,
        .textId = 50,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_UINT32,
        .p1Pos = 0,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "$1",
        .seg = {
            { 0, 0 },
            { 2, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 80,
        .modId = GOAL_ID_MEM,
        .textId = 51,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_PTR,
        .p1Pos = 11,
        .p2Type = LM_PTR,
        .p2Pos = 20,
        .text = "    : raw: $1, ptr: $2",
        .seg = {
            { 0, 11 },
            { 13, 7 },
            { 22, 0 }
        }
    },
    {
        .id = 81,
        .modId = GOAL_ID_MEM,
        .textId = 52,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_UINT32,
        .p1Pos = 11,
        .p2Type = LM_UINT32,
        .p2Pos = 22,
        .text = "    : len: $1, align: $2",
        .seg = {
            { 0, 11 },
            { 13, 9 },
            { 24, 0 }
        }
    },
    {
        .id = 82,
        .modId = GOAL_ID_MEM,
        .textId = 53,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_UINT32,
        .p1Pos = 14,
        .p2Type = LM_UINT32,
        .p2Pos = 17,
        .text = "memory usage: $1/$2 bytes",
        .seg = {
            { 0, 14 },
            { 16, 1 },
            { 19, 6 }
        }
    },
    {
        .id = 83,
        .modId = GOAL_ID_MEM,
        .textId = 54,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_UINT32,
        .p1Pos = 15,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "memory usage: ($1%)",
        .seg = {
            { 0, 15 },
            { 17, 2 },
            { 0, 0 }
        }
    },
    {
        .id = 84,
        .modId = GOAL_ID_BM,
        .textId = 55,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to allocate bitmap structure",
        .seg = {
            { 0, 35 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 85,
        .modId = GOAL_ID_BM,
        .textId = 56,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_UINT32,
        .p1Pos = 19,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to allocate $1 bytes",
        .seg = {
            { 0, 19 },
            { 21, 6 },
            { 0, 0 }
        }
    },
    {
        .id = 86,
        .modId = GOAL_ID_BM,
        .textId = 57,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "bit already released",
        .seg = {
            { 0, 20 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 87,
        .modId = GOAL_ID_CM,
        .textId = 58,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_UINT32,
        .p1Pos = 19,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Could not allocate $1 bytes of config data buffer!",
        .seg = {
            { 0, 19 },
            { 21, 29 },
            { 0, 0 }
        }
    },
    {
        .id = 88,
        .modId = GOAL_ID_CM,
        .textId = 59,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_UINT32,
        .p1Pos = 19,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Could not allocate $1 bytes of mirrored config data buffer!",
        .seg = {
            { 0, 19 },
            { 21, 38 },
            { 0, 0 }
        }
    },
    {
        .id = 89,
        .modId = GOAL_ID_CM,
        .textId = 60,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_UINT32,
        .p1Pos = 26,
        .p2Type = LM_UINT32,
        .p2Pos = 32,
        .text = "Calculated config size of $1 in $2 modules",
        .seg = {
            { 0, 26 },
            { 28, 4 },
            { 34, 8 }
        }
    },
    {
        .id = 90,
        .modId = GOAL_ID_CM,
        .textId = 61,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_UINT32,
        .p1Pos = 36,
        .p2Type = LM_UINT32,
        .p2Pos = 39,
        .text = "Tried to access UNKNOWN var with id $1-$2",
        .seg = {
            { 0, 36 },
            { 38, 1 },
            { 41, 0 }
        }
    },
    {
        .id = 91,
        .modId = GOAL_ID_CM,
        .textId = 62,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to write to NVS",
        .seg = {
            { 0, 22 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 92,
        .modId = GOAL_ID_CM,
        .textId = 63,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_UINT32,
        .p1Pos = 29,
        .p2Type = LM_UINT32,
        .p2Pos = 42,
        .text = "Versions DO NOT match (found $1, expected $2)!",
        .seg = {
            { 0, 29 },
            { 31, 11 },
            { 44, 2 }
        }
    },
    {
        .id = 93,
        .modId = GOAL_ID_CM,
        .textId = 64,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_UINT32,
        .p1Pos = 27,
        .p2Type = LM_UINT32,
        .p2Pos = 34,
        .text = "Config sizes DO NOT match ($1 vs. $2)!",
        .seg = {
            { 0, 27 },
            { 29, 5 },
            { 36, 2 }
        }
    },
    {
        .id = 94,
        .modId = GOAL_ID_CM,
        .textId = 65,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_UINT32,
        .p1Pos = 10,
        .p2Type = LM_UINT32,
        .p2Pos = 17,
        .text = "Checksums $1 and $2 DONT match!",
        .seg = {
            { 0, 10 },
            { 12, 5 },
            { 19, 12 }
        }
    },
    {
        .id = 95,
        .modId = GOAL_ID_CM,
        .textId = 66,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_UINT32,
        .p1Pos = 10,
        .p2Type = LM_UINT32,
        .p2Pos = 17,
        .text = "Checksums $1 and $2 match!",
        .seg = {
            { 0, 10 },
            { 12, 5 },
            { 19, 7 }
        }
    },
    {
        .id = 96,
        .modId = GOAL_ID_CM,
        .textId = 67,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_UINT32,
        .p1Pos = 28,
        .p2Type = LM_UINT32,
        .p2Pos = 42,
        .text = "Var type of var with mod ID $1 and var ID $2",
        .seg = {
            { 0, 28 },
            { 30, 12 },
            { 44, 0 }
        }
    },
    {
        .id = 97,
        .modId = GOAL_ID_CM,
        .textId = 68,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_UINT32,
        .p1Pos = 22,
        .p2Type = LM_UINT32,
        .p2Pos = 29,
        .text = "    : DOES NOT match ($1 vs. $2)!",
        .seg = {
            { 0, 22 },
            { 24, 5 },
            { 31, 2 }
        }
    },
    {
        .id = 98,
        .modId = GOAL_ID_CM,
        .textId = 69,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_UINT32,
        .p1Pos = 16,
        .p2Type = LM_UINT32,
        .p2Pos = 30,
        .text = "Var with mod id $1 and var id $2",
        .seg = {
            { 0, 16 },
            { 18, 12 },
            { 32, 0 }
        }
    },
    {
        .id = 99,
        .modId = GOAL_ID_CM,
        .textId = 70,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_PTRDIFF,
        .p1Pos = 29,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "    : NOT found (searched at $1)!",
        .seg = {
            { 0, 29 },
            { 31, 2 },
            { 0, 0 }
        }
    },
    {
        .id = 100,
        .modId = GOAL_ID_CM,
        .textId = 71,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_UINT32,
        .p1Pos = 0,
        .p2Type = LM_UINT32,
        .p2Pos = 3,
        .text = "$1-$2: ",
        .seg = {
            { 0, 0 },
            { 2, 1 },
            { 5, 2 }
        }
    },
    {
        .id = 101,
        .modId = GOAL_ID_CM,
        .textId = 72,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_UINT32,
        .p1Pos = 11,
        .p2Type = LM_UINT32,
        .p2Pos = 22,
        .text = "    : type $1 curSize $2",
        .seg = {
            { 0, 11 },
            { 13, 9 },
            { 24, 0 }
        }
    },
    {
        .id = 102,
        .modId = GOAL_ID_CM,
        .textId = 73,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_UINT32,
        .p1Pos = 16,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "    : max. size $1 ",
        .seg = {
            { 0, 16 },
            { 18, 1 },
            { 0, 0 }
        }
    },
    {
        .id = 103,
        .modId = GOAL_ID_CM,
        .textId = 74,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_STRING0,
        .p1Pos = 7,
        .p2Type = LM_STRING0,
        .p2Pos = 10,
        .text = "    : ($1:$2)",
        .seg = {
            { 0, 7 },
            { 9, 1 },
            { 12, 1 }
        }
    },
    {
        .id = 104,
        .modId = GOAL_ID_CM,
        .textId = 75,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_UINT32,
        .p1Pos = 0,
        .p2Type = LM_UINT32,
        .p2Pos = 3,
        .text = "$1-$2: ",
        .seg = {
            { 0, 0 },
            { 2, 1 },
            { 5, 2 }
        }
    },
    {
        .id = 105,
        .modId = GOAL_ID_CM,
        .textId = 76,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_UINT32,
        .p1Pos = 11,
        .p2Type = LM_UINT32,
        .p2Pos = 22,
        .text = "    : type $1 curSize $2",
        .seg = {
            { 0, 11 },
            { 13, 9 },
            { 24, 0 }
        }
    },
    {
        .id = 106,
        .modId = GOAL_ID_CM,
        .textId = 77,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_UINT32,
        .p1Pos = 16,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "    : max. size $1 ",
        .seg = {
            { 0, 16 },
            { 18, 1 },
            { 0, 0 }
        }
    },
    {
        .id = 107,
        .modId = GOAL_ID_CM,
        .textId = 78,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_UINT32,
        .p1Pos = 17,
        .p2Type = LM_UINT32,
        .p2Pos = 21,
        .text = "tmp var cleared: $1, $2",
        .seg = {
            { 0, 17 },
            { 19, 2 },
            { 23, 0 }
        }
    },
    {
        .id = 108,
        .modId = GOAL_ID_CM,
        .textId = 79,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_STRING0,
        .p1Pos = 7,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "    : ($1)",
        .seg = {
            { 0, 7 },
            { 9, 1 },
            { 0, 0 }
        }
    },
    {
        .id = 109,
        .modId = GOAL_ID_ETH,
        .textId = 80,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to create Ethernet send lock",
        .seg = {
            { 0, 35 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 110,
        .modId = GOAL_ID_ETH,
        .textId = 81,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to create Ethernet access lock",
        .seg = {
            { 0, 37 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 111,
        .modId = GOAL_ID_ETH,
        .textId = 82,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to allocate statistics values",
        .seg = {
            { 0, 36 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 112,
        .modId = GOAL_ID_ETH,
        .textId = 83,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to register statistics value",
        .seg = {
            { 0, 35 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 113,
        .modId = GOAL_ID_ETH,
        .textId = 84,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_UINT32,
        .p1Pos = 41,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "internally providing statistics counter: $1",
        .seg = {
            { 0, 41 },
            { 43, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 114,
        .modId = GOAL_ID_ETH,
        .textId = 85,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Ethernet shutdown",
        .seg = {
            { 0, 17 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 115,
        .modId = GOAL_ID_ETH,
        .textId = 86,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_UINT32,
        .p1Pos = 25,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "port index out of range: $1",
        .seg = {
            { 0, 25 },
            { 27, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 116,
        .modId = GOAL_ID_ETH,
        .textId = 87,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "frame still in transmit state",
        .seg = {
            { 0, 29 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 117,
        .modId = GOAL_ID_ETH,
        .textId = 88,
        .severity = GOAL_LOG_SEV_WARNING,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to send Ethernet frame",
        .seg = {
            { 0, 29 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 118,
        .modId = GOAL_ID_ETH,
        .textId = 89,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "handler function not set",
        .seg = {
            { 0, 24 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 119,
        .modId = GOAL_ID_ETH,
        .textId = 90,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to allocate protocol handler",
        .seg = {
            { 0, 35 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 120,
        .modId = GOAL_ID_ETH,
        .textId = 91,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to allocate MAC address in protocol handler",
        .seg = {
            { 0, 50 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 121,
        .modId = GOAL_ID_ETH,
        .textId = 92,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "NULL pointer argument not allowed",
        .seg = {
            { 0, 33 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 122,
        .modId = GOAL_ID_ETH,
        .textId = 93,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to aquire Ethernet access lock",
        .seg = {
            { 0, 37 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 123,
        .modId = GOAL_ID_ETH,
        .textId = 94,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_UINT32,
        .p1Pos = 22,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "invalid port argument $1",
        .seg = {
            { 0, 22 },
            { 24, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 124,
        .modId = GOAL_ID_ETH,
        .textId = 95,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_UINT32,
        .p1Pos = 14,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "port argument $1 contains invalid bits",
        .seg = {
            { 0, 14 },
            { 16, 22 },
            { 0, 0 }
        }
    },
    {
        .id = 125,
        .modId = GOAL_ID_ETH,
        .textId = 96,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_STRING0,
        .p1Pos = 8,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "command $1 only supports external ports",
        .seg = {
            { 0, 8 },
            { 10, 29 },
            { 0, 0 }
        }
    },
    {
        .id = 126,
        .modId = GOAL_ID_ETH,
        .textId = 97,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_UINT32,
        .p1Pos = 8,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "command $1 only supports external ports",
        .seg = {
            { 0, 8 },
            { 10, 29 },
            { 0, 0 }
        }
    },
    {
        .id = 127,
        .modId = GOAL_ID_ETH,
        .textId = 98,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_STRING0,
        .p1Pos = 8,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "command $1 only supports internal port",
        .seg = {
            { 0, 8 },
            { 10, 28 },
            { 0, 0 }
        }
    },
    {
        .id = 128,
        .modId = GOAL_ID_ETH,
        .textId = 99,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_UINT32,
        .p1Pos = 8,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "command $1 only supports internal port",
        .seg = {
            { 0, 8 },
            { 10, 28 },
            { 0, 0 }
        }
    },
    {
        .id = 129,
        .modId = GOAL_ID_ETH,
        .textId = 100,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "No Ethernet functions registered.",
        .seg = {
            { 0, 33 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 130,
        .modId = GOAL_ID_ETH,
        .textId = 101,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "need a portmask to proceed",
        .seg = {
            { 0, 26 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 131,
        .modId = GOAL_ID_INSTANCE,
        .textId = 102,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to create instance list entry",
        .seg = {
            { 0, 36 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 132,
        .modId = GOAL_ID_INSTANCE,
        .textId = 103,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_UINT32,
        .p1Pos = 9,
        .p2Type = LM_UINT32,
        .p2Pos = 20,
        .text = "instance $1 of type $2 already created",
        .seg = {
            { 0, 9 },
            { 11, 9 },
            { 22, 16 }
        }
    },
    {
        .id = 133,
        .modId = GOAL_ID_INSTANCE,
        .textId = 104,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to create instance entry",
        .seg = {
            { 0, 31 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 134,
        .modId = GOAL_ID_LM,
        .textId = 105,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_UINT32,
        .p1Pos = 43,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to allocate logging buffer of size: $1 bytes",
        .seg = {
            { 0, 43 },
            { 45, 6 },
            { 0, 0 }
        }
    },
    {
        .id = 135,
        .modId = GOAL_ID_LOCK,
        .textId = 106,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "allocating lock structure failed",
        .seg = {
            { 0, 32 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 136,
        .modId = GOAL_ID_LOCK,
        .textId = 107,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to create lock on target",
        .seg = {
            { 0, 31 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 137,
        .modId = GOAL_ID_LOCK,
        .textId = 108,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Could not get lock, halting device.",
        .seg = {
            { 0, 35 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 138,
        .modId = GOAL_ID_LOCK,
        .textId = 109,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Could not release lock, halting device.",
        .seg = {
            { 0, 39 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 139,
        .modId = GOAL_ID_LOCK,
        .textId = 110,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Lock could not be deleted - halt",
        .seg = {
            { 0, 32 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 140,
        .modId = GOAL_ID_LOG,
        .textId = 111,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_UINT32,
        .p1Pos = 14,
        .p2Type = LM_UINT32,
        .p2Pos = 17,
        .text = "GOAL Version: $1.$2",
        .seg = {
            { 0, 14 },
            { 16, 1 },
            { 19, 0 }
        }
    },
    {
        .id = 141,
        .modId = GOAL_ID_LOG,
        .textId = 112,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_UINT32,
        .p1Pos = 18,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "GOAL Sub-Version: $1",
        .seg = {
            { 0, 18 },
            { 20, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 142,
        .modId = GOAL_ID_LOG,
        .textId = 113,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to get MAC address",
        .seg = {
            { 0, 25 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 143,
        .modId = GOAL_ID_MAIN,
        .textId = 114,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "GOAL initialized",
        .seg = {
            { 0, 16 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 144,
        .modId = GOAL_ID_MAIN,
        .textId = 115,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "running csap setup",
        .seg = {
            { 0, 18 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 145,
        .modId = GOAL_ID_MAIN,
        .textId = 116,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "application setup failed",
        .seg = {
            { 0, 24 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 146,
        .modId = GOAL_ID_MAIN,
        .textId = 116,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "application setup failed",
        .seg = {
            { 0, 24 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 147,
        .modId = GOAL_ID_MAIN,
        .textId = 117,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "registering application loop failed",
        .seg = {
            { 0, 35 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 148,
        .modId = GOAL_ID_MAIN,
        .textId = 118,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to allocate main loop element",
        .seg = {
            { 0, 36 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 149,
        .modId = GOAL_ID_MAIN,
        .textId = 118,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to allocate main loop element",
        .seg = {
            { 0, 36 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 150,
        .modId = GOAL_ID_MAIN,
        .textId = 119,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Failed to init demo mode",
        .seg = {
            { 0, 24 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 151,
        .modId = GOAL_ID_MAIN,
        .textId = 120,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Failed to start demo mode",
        .seg = {
            { 0, 25 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 152,
        .modId = GOAL_ID_MAIN,
        .textId = 121,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_UINT32,
        .p1Pos = 46,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "This is a demo version. It is time limited to $1 min",
        .seg = {
            { 0, 46 },
            { 48, 4 },
            { 0, 0 }
        }
    },
    {
        .id = 153,
        .modId = GOAL_ID_MAIN,
        .textId = 122,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "End of demo mode. Lib is shutting down",
        .seg = {
            { 0, 38 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 154,
        .modId = GOAL_ID_MAIN,
        .textId = 123,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Halt device now.",
        .seg = {
            { 0, 16 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 155,
        .modId = GOAL_ID_MAIN,
        .textId = 124,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to allocate callback module data",
        .seg = {
            { 0, 39 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 156,
        .modId = GOAL_ID_MAIN,
        .textId = 125,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to find module callback list",
        .seg = {
            { 0, 35 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 157,
        .modId = GOAL_ID_MAIN,
        .textId = 126,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to allocate callback data",
        .seg = {
            { 0, 32 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 158,
        .modId = GOAL_ID_NET,
        .textId = 127,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to get IP configuration",
        .seg = {
            { 0, 30 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 159,
        .modId = GOAL_ID_NET,
        .textId = 128,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to update IP configuration",
        .seg = {
            { 0, 33 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 160,
        .modId = GOAL_ID_NET,
        .textId = 129,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Could not find free GOAL channel",
        .seg = {
            { 0, 32 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 161,
        .modId = GOAL_ID_NET,
        .textId = 130,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Channel in application directon is not active.",
        .seg = {
            { 0, 46 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 162,
        .modId = GOAL_ID_NET,
        .textId = 131,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "no free channel found",
        .seg = {
            { 0, 21 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 163,
        .modId = GOAL_ID_NET,
        .textId = 132,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "opening channel failed",
        .seg = {
            { 0, 22 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 164,
        .modId = GOAL_ID_NET,
        .textId = 133,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "No callback is given",
        .seg = {
            { 0, 20 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 165,
        .modId = GOAL_ID_NET,
        .textId = 131,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "no free channel found",
        .seg = {
            { 0, 21 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 166,
        .modId = GOAL_ID_NET,
        .textId = 134,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Chanel handle is not valid",
        .seg = {
            { 0, 26 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 167,
        .modId = GOAL_ID_NET,
        .textId = 135,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Could not find channel for target handle",
        .seg = {
            { 0, 40 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 168,
        .modId = GOAL_ID_NET,
        .textId = 136,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Could not find net Channel to given target channel",
        .seg = {
            { 0, 50 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 169,
        .modId = GOAL_ID_NET,
        .textId = 137,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Could not close target channel handle",
        .seg = {
            { 0, 37 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 170,
        .modId = GOAL_ID_NET,
        .textId = 138,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Could not deactivate target channel handle.",
        .seg = {
            { 0, 43 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 171,
        .modId = GOAL_ID_NET,
        .textId = 139,
        .severity = GOAL_LOG_SEV_WARNING,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Could not close channel after failed reopen call",
        .seg = {
            { 0, 48 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 172,
        .modId = GOAL_ID_NET,
        .textId = 140,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_UINT32,
        .p1Pos = 26,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to reopen channel: $1",
        .seg = {
            { 0, 26 },
            { 28, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 173,
        .modId = GOAL_ID_NET,
        .textId = 141,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Could not reactivate reopened channel",
        .seg = {
            { 0, 37 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 174,
        .modId = GOAL_ID_NET,
        .textId = 142,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Given channel isn't reserved.",
        .seg = {
            { 0, 29 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 175,
        .modId = GOAL_ID_NET,
        .textId = 143,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Invalid Channel given, no target channel found",
        .seg = {
            { 0, 46 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 176,
        .modId = GOAL_ID_NET,
        .textId = 144,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Could not find target handle to given channel",
        .seg = {
            { 0, 45 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 177,
        .modId = GOAL_ID_NET,
        .textId = 145,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "given channel isn't ready",
        .seg = {
            { 0, 25 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 178,
        .modId = GOAL_ID_NET,
        .textId = 146,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Could not send. Channel is disabled",
        .seg = {
            { 0, 35 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 179,
        .modId = GOAL_ID_NET,
        .textId = 146,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Could not send. Channel is disabled",
        .seg = {
            { 0, 35 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 180,
        .modId = GOAL_ID_NET,
        .textId = 146,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Could not send. Channel is disabled",
        .seg = {
            { 0, 35 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 181,
        .modId = GOAL_ID_NET,
        .textId = 146,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Could not send. Channel is disabled",
        .seg = {
            { 0, 35 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 182,
        .modId = GOAL_ID_NET,
        .textId = 147,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Internal error in sending function",
        .seg = {
            { 0, 34 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 183,
        .modId = GOAL_ID_NET,
        .textId = 148,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "network shutdown",
        .seg = {
            { 0, 16 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 184,
        .modId = GOAL_ID_NET,
        .textId = 149,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "setting option on inactive channel isn't possible",
        .seg = {
            { 0, 49 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 185,
        .modId = GOAL_ID_NET,
        .textId = 143,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Invalid Channel given, no target channel found",
        .seg = {
            { 0, 46 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 186,
        .modId = GOAL_ID_NET,
        .textId = 149,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "setting option on inactive channel isn't possible",
        .seg = {
            { 0, 49 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 187,
        .modId = GOAL_ID_NET,
        .textId = 150,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "setting callback on unallocated channel isn't possible",
        .seg = {
            { 0, 54 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 188,
        .modId = GOAL_ID_NET,
        .textId = 151,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "callback function already set on channel",
        .seg = {
            { 0, 40 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 189,
        .modId = GOAL_ID_NET,
        .textId = 152,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "not initialized",
        .seg = {
            { 0, 15 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 190,
        .modId = GOAL_ID_NET,
        .textId = 153,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "channel isn't ready yet",
        .seg = {
            { 0, 23 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 191,
        .modId = GOAL_ID_NET,
        .textId = 154,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "no callback function registered",
        .seg = {
            { 0, 31 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 192,
        .modId = GOAL_ID_NET,
        .textId = 152,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "not initialized",
        .seg = {
            { 0, 15 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 193,
        .modId = GOAL_ID_NET,
        .textId = 155,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "recieve function called wihour channel.",
        .seg = {
            { 0, 39 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 194,
        .modId = GOAL_ID_NET,
        .textId = 154,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "no callback function registered",
        .seg = {
            { 0, 31 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 195,
        .modId = GOAL_ID_NET,
        .textId = 156,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "successfully updated IP configuration",
        .seg = {
            { 0, 37 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 196,
        .modId = GOAL_ID_NET,
        .textId = 157,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to set IP configuration",
        .seg = {
            { 0, 30 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 197,
        .modId = GOAL_ID_NET,
        .textId = 128,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to update IP configuration",
        .seg = {
            { 0, 33 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 198,
        .modId = GOAL_ID_NET,
        .textId = 158,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to retrieve valid flag",
        .seg = {
            { 0, 29 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 199,
        .modId = GOAL_ID_DHCP,
        .textId = 159,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to request network buffers",
        .seg = {
            { 0, 33 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 200,
        .modId = GOAL_ID_DHCP,
        .textId = 160,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to allocate socket",
        .seg = {
            { 0, 25 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 201,
        .modId = GOAL_ID_DHCP,
        .textId = 161,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to set socket to non-blocking",
        .seg = {
            { 0, 36 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 202,
        .modId = GOAL_ID_DHCP,
        .textId = 162,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to make socket reusable",
        .seg = {
            { 0, 30 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 203,
        .modId = GOAL_ID_DHCP,
        .textId = 163,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to enable Broadcast for socket",
        .seg = {
            { 0, 37 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 204,
        .modId = GOAL_ID_DHCP,
        .textId = 113,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to get MAC address",
        .seg = {
            { 0, 25 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 205,
        .modId = GOAL_ID_DHCP,
        .textId = 164,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "no free buffer",
        .seg = {
            { 0, 14 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 206,
        .modId = GOAL_ID_DHCP,
        .textId = 165,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "DHCP Request rejected by server",
        .seg = {
            { 0, 31 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 207,
        .modId = GOAL_ID_DHCP,
        .textId = 165,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "DHCP Request rejected by server",
        .seg = {
            { 0, 31 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 208,
        .modId = GOAL_ID_DHCP,
        .textId = 165,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "DHCP Request rejected by server",
        .seg = {
            { 0, 31 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 209,
        .modId = GOAL_ID_DHCP,
        .textId = 166,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Lost IP Lease",
        .seg = {
            { 0, 13 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 210,
        .modId = GOAL_ID_DHCP,
        .textId = 164,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "no free buffer",
        .seg = {
            { 0, 14 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 211,
        .modId = GOAL_ID_DHCP,
        .textId = 164,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "no free buffer",
        .seg = {
            { 0, 14 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 212,
        .modId = GOAL_ID_DHCP,
        .textId = 167,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "invalid callback",
        .seg = {
            { 0, 16 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 213,
        .modId = GOAL_ID_DHCP,
        .textId = 168,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "invalid length",
        .seg = {
            { 0, 14 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 214,
        .modId = GOAL_ID_DHCP,
        .textId = 169,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "no BOOTRES",
        .seg = {
            { 0, 10 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 215,
        .modId = GOAL_ID_DHCP,
        .textId = 170,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_UINT32,
        .p1Pos = 12,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "invalid xid $1",
        .seg = {
            { 0, 12 },
            { 14, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 216,
        .modId = GOAL_ID_DHCP,
        .textId = 171,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "invalid HW address",
        .seg = {
            { 0, 18 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 217,
        .modId = GOAL_ID_DHCP,
        .textId = 172,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_UINT32,
        .p1Pos = 15,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "invalid cookie $1",
        .seg = {
            { 0, 15 },
            { 17, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 218,
        .modId = GOAL_ID_DHCP,
        .textId = 173,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "OFFER had no server ID",
        .seg = {
            { 0, 22 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 219,
        .modId = GOAL_ID_DHCP,
        .textId = 174,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_UINT32,
        .p1Pos = 27,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "unknown or no message type $1",
        .seg = {
            { 0, 27 },
            { 29, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 220,
        .modId = GOAL_ID_DHCP,
        .textId = 175,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "IP address lease time expired !!!",
        .seg = {
            { 0, 33 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 221,
        .modId = GOAL_ID_MEM,
        .textId = 176,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Failed to register CM variables.",
        .seg = {
            { 0, 32 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 222,
        .modId = GOAL_ID_MEM,
        .textId = 177,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Could not initialize CM variables.",
        .seg = {
            { 0, 34 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 223,
        .modId = GOAL_ID_MEM,
        .textId = 178,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_UINT32,
        .p1Pos = 30,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Could not initialize queue no $1",
        .seg = {
            { 0, 30 },
            { 32, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 224,
        .modId = GOAL_ID_MEM,
        .textId = 179,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Use config manager variables for buffer number and sizes instead of default value defines",
        .seg = {
            { 0, 89 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 225,
        .modId = GOAL_ID_MEM,
        .textId = 180,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "error during buffer release",
        .seg = {
            { 0, 27 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 226,
        .modId = GOAL_ID_MEM,
        .textId = 181,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to create queue lock",
        .seg = {
            { 0, 27 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 227,
        .modId = GOAL_ID_MEM,
        .textId = 182,
        .severity = GOAL_LOG_SEV_WARNING,
        .p1Type = LM_PTR,
        .p1Pos = 41,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Failed to get the buffer from the queue: $1",
        .seg = {
            { 0, 41 },
            { 43, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 228,
        .modId = GOAL_ID_MEM,
        .textId = 183,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "error requesting buffer mutex",
        .seg = {
            { 0, 29 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 229,
        .modId = GOAL_ID_MEM,
        .textId = 184,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to release buffer to queue",
        .seg = {
            { 0, 33 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 230,
        .modId = GOAL_ID_MEM,
        .textId = 185,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to create queue element lock",
        .seg = {
            { 0, 35 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 231,
        .modId = GOAL_ID_RB,
        .textId = 186,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to allocate rb handle",
        .seg = {
            { 0, 28 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 232,
        .modId = GOAL_ID_RB,
        .textId = 187,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to allocate logging buffer",
        .seg = {
            { 0, 33 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 233,
        .modId = GOAL_ID_REG,
        .textId = 188,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_UINT32,
        .p1Pos = 23,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "register is read only: $1",
        .seg = {
            { 0, 23 },
            { 25, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 234,
        .modId = GOAL_ID_REG,
        .textId = 189,
        .severity = GOAL_LOG_SEV_WARNING,
        .p1Type = LM_UINT32,
        .p1Pos = 30,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "unsupported command received: $1",
        .seg = {
            { 0, 30 },
            { 32, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 235,
        .modId = GOAL_ID_STAT,
        .textId = 190,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "no get function set",
        .seg = {
            { 0, 19 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 236,
        .modId = GOAL_ID_TASK,
        .textId = 191,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_STRING0,
        .p1Pos = 15,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "creating task: $1",
        .seg = {
            { 0, 15 },
            { 17, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 237,
        .modId = GOAL_ID_TASK,
        .textId = 192,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "error allocating task structure",
        .seg = {
            { 0, 31 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 238,
        .modId = GOAL_ID_TASK,
        .textId = 193,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "error creating task in target driver",
        .seg = {
            { 0, 36 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 239,
        .modId = GOAL_ID_TMR,
        .textId = 194,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to create timer lock",
        .seg = {
            { 0, 27 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 240,
        .modId = GOAL_ID_TMR,
        .textId = 195,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "timer not initialized",
        .seg = {
            { 0, 21 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 241,
        .modId = GOAL_ID_TMR,
        .textId = 196,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "timer pointer not properly initialized",
        .seg = {
            { 0, 38 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 242,
        .modId = GOAL_ID_TMR,
        .textId = 197,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to allocate timer",
        .seg = {
            { 0, 24 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 243,
        .modId = GOAL_ID_TMR,
        .textId = 194,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to create timer lock",
        .seg = {
            { 0, 27 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 244,
        .modId = GOAL_ID_TMR,
        .textId = 198,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to create target timer",
        .seg = {
            { 0, 29 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 245,
        .modId = GOAL_ID_TMR,
        .textId = 199,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "timer is null",
        .seg = {
            { 0, 13 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 246,
        .modId = GOAL_ID_TMR,
        .textId = 200,
        .severity = GOAL_LOG_SEV_WARNING,
        .p1Type = LM_UINT32,
        .p1Pos = 25,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "timer period larger than $1",
        .seg = {
            { 0, 25 },
            { 27, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 247,
        .modId = GOAL_ID_TMR,
        .textId = 201,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "timer callback not set",
        .seg = {
            { 0, 22 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 248,
        .modId = GOAL_ID_TMR,
        .textId = 199,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "timer is null",
        .seg = {
            { 0, 13 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 249,
        .modId = GOAL_ID_TMR,
        .textId = 202,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "timer already started",
        .seg = {
            { 0, 21 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 250,
        .modId = GOAL_ID_TMR,
        .textId = 199,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "timer is null",
        .seg = {
            { 0, 13 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 251,
        .modId = GOAL_ID_TMR,
        .textId = 199,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "timer is null",
        .seg = {
            { 0, 13 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 252,
        .modId = GOAL_ID_TMR,
        .textId = 203,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "timer not properly created",
        .seg = {
            { 0, 26 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 253,
        .modId = GOAL_ID_TMR,
        .textId = 204,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "timer could not be deleted in target",
        .seg = {
            { 0, 36 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 254,
        .modId = GOAL_ID_TMR,
        .textId = 205,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_UINT64,
        .p1Pos = 35,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "rescue timeout counter calibrated: $1 counts",
        .seg = {
            { 0, 35 },
            { 37, 7 },
            { 0, 0 }
        }
    },
    {
        .id = 255,
        .modId = GOAL_ID_UTIL,
        .textId = 206,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_STRING0,
        .p1Pos = 0,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "$1",
        .seg = {
            { 0, 0 },
            { 2, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 256,
        .modId = GOAL_ID_UTIL,
        .textId = 207,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_STRING0,
        .p1Pos = 0,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "$1",
        .seg = {
            { 0, 0 },
            { 2, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 257,
        .modId = GOAL_ID_GLOBAL,
        .textId = 208,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to allocate stage handle",
        .seg = {
            { 0, 31 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 258,
        .modId = GOAL_ID_GLOBAL,
        .textId = 209,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to get MA SPI for id 0",
        .seg = {
            { 0, 29 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 259,
        .modId = GOAL_ID_GLOBAL,
        .textId = 210,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to set MA SPI 0 mode 3",
        .seg = {
            { 0, 29 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 260,
        .modId = GOAL_ID_GLOBAL,
        .textId = 211,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "successfully set MA SPI 0 mode 3",
        .seg = {
            { 0, 32 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 261,
        .modId = GOAL_ID_MA_NVS,
        .textId = 212,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to allocate NVS handle",
        .seg = {
            { 0, 29 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 262,
        .modId = GOAL_ID_MA_NVS,
        .textId = 213,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "MA reference is NULL",
        .seg = {
            { 0, 20 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 263,
        .modId = GOAL_ID_MA_SPI,
        .textId = 214,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "open function not defined",
        .seg = {
            { 0, 25 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 264,
        .modId = GOAL_ID_MA_SPI,
        .textId = 215,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_UINT32,
        .p1Pos = 19,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "The SPI MA with ID $1 is already in use",
        .seg = {
            { 0, 19 },
            { 21, 18 },
            { 0, 0 }
        }
    },
    {
        .id = 265,
        .modId = GOAL_ID_MA_SPI,
        .textId = 216,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Unable to allocate SPI MA",
        .seg = {
            { 0, 25 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 266,
        .modId = GOAL_ID_MA_SPI,
        .textId = 217,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to allocate RX databuffer",
        .seg = {
            { 0, 32 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 267,
        .modId = GOAL_ID_MA_SPI,
        .textId = 218,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to allocate TX databuffer",
        .seg = {
            { 0, 32 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 268,
        .modId = GOAL_ID_MA_SPI,
        .textId = 219,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to create RX/TX lock",
        .seg = {
            { 0, 27 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 269,
        .modId = GOAL_ID_MA_WD,
        .textId = 214,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "open function not defined",
        .seg = {
            { 0, 25 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 270,
        .modId = GOAL_ID_MI_CMFS,
        .textId = 220,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_UINT32,
        .p1Pos = 13,
        .p2Type = LM_UINT32,
        .p2Pos = 23,
        .text = "cmfs usage : $1 bytes ($2%)",
        .seg = {
            { 0, 13 },
            { 15, 8 },
            { 25, 2 }
        }
    },
    {
        .id = 271,
        .modId = GOAL_ID_MI_MCTC,
        .textId = 221,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_UINT32,
        .p1Pos = 32,
        .p2Type = LM_STRING0,
        .p2Pos = 36,
        .text = "failed to create MCTC instance: $1 ($2)",
        .seg = {
            { 0, 32 },
            { 34, 2 },
            { 38, 1 }
        }
    },
    {
        .id = 272,
        .modId = GOAL_ID_MI_MCTC,
        .textId = 222,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to register generic RPC handler",
        .seg = {
            { 0, 38 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 273,
        .modId = GOAL_ID_MI_MCTC,
        .textId = 223,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to fetch instance list",
        .seg = {
            { 0, 29 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 274,
        .modId = GOAL_ID_MI_MCTC,
        .textId = 224,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to register MI MCTC loop",
        .seg = {
            { 0, 31 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 275,
        .modId = GOAL_ID_MI_MCTC,
        .textId = 225,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to register handshake request callback",
        .seg = {
            { 0, 45 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 276,
        .modId = GOAL_ID_MI_MCTC,
        .textId = 226,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to register handshake response callback",
        .seg = {
            { 0, 46 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 277,
        .modId = GOAL_ID_MI_MCTC,
        .textId = 227,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to register mctc mi cm variables",
        .seg = {
            { 0, 39 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 278,
        .modId = GOAL_ID_MI_MCTC,
        .textId = 228,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to allocate RPC function item",
        .seg = {
            { 0, 36 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 279,
        .modId = GOAL_ID_MI_MCTC,
        .textId = 229,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_UINT32,
        .p1Pos = 30,
        .p2Type = LM_UINT32,
        .p2Pos = 33,
        .text = "unregistered function called: $1 $2",
        .seg = {
            { 0, 30 },
            { 32, 1 },
            { 35, 0 }
        }
    },
    {
        .id = 280,
        .modId = GOAL_ID_MI_MCTC,
        .textId = 230,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "MCTC instance not found",
        .seg = {
            { 0, 23 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 281,
        .modId = GOAL_ID_MI_MCTC,
        .textId = 231,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to allocate MCTC target handle",
        .seg = {
            { 0, 37 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 282,
        .modId = GOAL_ID_MI_MCTC,
        .textId = 232,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to get an MCTC instance",
        .seg = {
            { 0, 30 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 283,
        .modId = GOAL_ID_MI_MCTC,
        .textId = 233,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to push data to MCTC stack",
        .seg = {
            { 0, 33 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 284,
        .modId = GOAL_ID_MI_MCTC,
        .textId = 234,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to pop data to MCTC stack",
        .seg = {
            { 0, 32 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 285,
        .modId = GOAL_ID_MI_MCTC,
        .textId = 235,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "TX: local handshake request",
        .seg = {
            { 0, 27 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 286,
        .modId = GOAL_ID_MI_MCTC,
        .textId = 236,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "local and remote handshake done",
        .seg = {
            { 0, 31 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 287,
        .modId = GOAL_ID_MI_MCTC,
        .textId = 237,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "running appl_setup",
        .seg = {
            { 0, 18 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 288,
        .modId = GOAL_ID_MI_MCTC,
        .textId = 238,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "RX: remote handshake request",
        .seg = {
            { 0, 28 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 289,
        .modId = GOAL_ID_MI_MCTC,
        .textId = 239,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "TX: remote handshake response",
        .seg = {
            { 0, 29 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 290,
        .modId = GOAL_ID_MI_MCTC,
        .textId = 240,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "remote handshake done",
        .seg = {
            { 0, 21 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 291,
        .modId = GOAL_ID_MI_MCTC,
        .textId = 241,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "RX: local handshake response",
        .seg = {
            { 0, 28 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 292,
        .modId = GOAL_ID_MI_MCTC,
        .textId = 242,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_UINT64,
        .p1Pos = 28,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "local handshake done, time: $1 ms",
        .seg = {
            { 0, 28 },
            { 30, 3 },
            { 0, 0 }
        }
    },
    {
        .id = 293,
        .modId = GOAL_ID_MI_CTC_SPI,
        .textId = 243,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to register MCTC SPI",
        .seg = {
            { 0, 27 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 294,
        .modId = GOAL_ID_MI_CTC_SPI,
        .textId = 244,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to allocate MI MCTC SPI data",
        .seg = {
            { 0, 35 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 295,
        .modId = GOAL_ID_MI_CTC_SPI,
        .textId = 245,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to allocate SPI target handle",
        .seg = {
            { 0, 36 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 296,
        .modId = GOAL_ID_MI_CTC_SPI,
        .textId = 246,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to allocate RX loopback ringbuffer",
        .seg = {
            { 0, 41 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 297,
        .modId = GOAL_ID_MI_CTC_SPI,
        .textId = 247,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to allocate TX loopback ringbuffer",
        .seg = {
            { 0, 41 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 298,
        .modId = GOAL_ID_MI_CTC_SPI,
        .textId = 248,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to open SPI MA",
        .seg = {
            { 0, 21 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 299,
        .modId = GOAL_ID_MI_CTC_SPI,
        .textId = 249,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to allocate RX buffer",
        .seg = {
            { 0, 28 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 300,
        .modId = GOAL_ID_MI_CTC_SPI,
        .textId = 250,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to allocate RX exchange buffer",
        .seg = {
            { 0, 37 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 301,
        .modId = GOAL_ID_MI_CTC_SPI,
        .textId = 251,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to allocate RX process buffer",
        .seg = {
            { 0, 36 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 302,
        .modId = GOAL_ID_MI_CTC_SPI,
        .textId = 252,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to allocate RX/TX resend buffer",
        .seg = {
            { 0, 38 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 303,
        .modId = GOAL_ID_MI_CTC_SPI,
        .textId = 253,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_UINT32,
        .p1Pos = 20,
        .p2Type = LM_UINT32,
        .p2Pos = 36,
        .text = "SPI split: cyclic = $1 bytes, RPC = $2 bytes",
        .seg = {
            { 0, 20 },
            { 22, 14 },
            { 38, 6 }
        }
    },
    {
        .id = 304,
        .modId = GOAL_ID_MI_CTC_SPI,
        .textId = 254,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to get SPI MA configuration",
        .seg = {
            { 0, 34 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 305,
        .modId = GOAL_ID_MI_CTC_SPI,
        .textId = 255,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to set SPI MA configuration",
        .seg = {
            { 0, 34 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 306,
        .modId = GOAL_ID_MI_CTC_SPI,
        .textId = 256,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to register SPI loop in GOAL loop",
        .seg = {
            { 0, 40 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 307,
        .modId = GOAL_ID_MI_CTC_SPI,
        .textId = 257,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to create buffer exchange lock",
        .seg = {
            { 0, 37 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 308,
        .modId = GOAL_ID_MI_CTC_SPI,
        .textId = 258,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to create SPI process task",
        .seg = {
            { 0, 33 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 309,
        .modId = GOAL_ID_MI_CTC_SPI,
        .textId = 259,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to create RPC process task",
        .seg = {
            { 0, 33 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 310,
        .modId = GOAL_ID_MI_CTC_SPI,
        .textId = 260,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to create cyclic timer on SPI master",
        .seg = {
            { 0, 43 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 311,
        .modId = GOAL_ID_MI_CTC_SPI,
        .textId = 261,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Failed to start cyclic timer on SPI master",
        .seg = {
            { 0, 42 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 312,
        .modId = GOAL_ID_MI_CTC_SPI,
        .textId = 262,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to allocate buffer filled with zeroes",
        .seg = {
            { 0, 44 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 313,
        .modId = GOAL_ID_MI_CTC_SPI,
        .textId = 263,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to put data into TX ringbuffer",
        .seg = {
            { 0, 37 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 314,
        .modId = GOAL_ID_MI_CTC_SPI,
        .textId = 264,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to put data into RX ringbuffer",
        .seg = {
            { 0, 37 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 315,
        .modId = GOAL_ID_MI_CTC_SPI,
        .textId = 265,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to exchange buffers",
        .seg = {
            { 0, 26 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 316,
        .modId = GOAL_ID_MI_NVS,
        .textId = 266,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to allocate NVS MI handle",
        .seg = {
            { 0, 32 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 317,
        .modId = GOAL_ID_MI_NVS,
        .textId = 267,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Could not register region",
        .seg = {
            { 0, 25 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 318,
        .modId = GOAL_ID_MI_NVS,
        .textId = 268,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to allocate NVS MI region handle",
        .seg = {
            { 0, 39 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 319,
        .modId = GOAL_ID_MI_NVS,
        .textId = 269,
        .severity = GOAL_LOG_SEV_WARNING,
        .p1Type = LM_UINT32,
        .p1Pos = 19,
        .p2Type = LM_UINT32,
        .p2Pos = 50,
        .text = "Region starting at $1 unaligned to sector size of $2",
        .seg = {
            { 0, 19 },
            { 21, 29 },
            { 52, 0 }
        }
    },
    {
        .id = 320,
        .modId = GOAL_ID_MI_NVS,
        .textId = 270,
        .severity = GOAL_LOG_SEV_WARNING,
        .p1Type = LM_UINT32,
        .p1Pos = 17,
        .p2Type = LM_UINT32,
        .p2Pos = 48,
        .text = "Region with size $1 unaligned to sector size of $2",
        .seg = {
            { 0, 17 },
            { 19, 29 },
            { 50, 0 }
        }
    },
    {
        .id = 321,
        .modId = GOAL_ID_MI_NVS,
        .textId = 271,
        .severity = GOAL_LOG_SEV_WARNING,
        .p1Type = LM_UINT32,
        .p1Pos = 17,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Region at offset $1 exceeds flash size",
        .seg = {
            { 0, 17 },
            { 19, 19 },
            { 0, 0 }
        }
    },
    {
        .id = 322,
        .modId = GOAL_ID_MI_NVS,
        .textId = 272,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to allocate buffer for nvs region",
        .seg = {
            { 0, 40 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 323,
        .modId = GOAL_ID_MI_NVS,
        .textId = 273,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_STRING0,
        .p1Pos = 28,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "update changed flash region $1",
        .seg = {
            { 0, 28 },
            { 30, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 324,
        .modId = GOAL_ID_MI_NVS,
        .textId = 274,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_STRING0,
        .p1Pos = 13,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "flash region $1 is up to date",
        .seg = {
            { 0, 13 },
            { 15, 14 },
            { 0, 0 }
        }
    },
    {
        .id = 325,
        .modId = GOAL_ID_MI_NVS,
        .textId = 275,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_UINT32,
        .p1Pos = 35,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Setting watchdog timer interval to $1 ms",
        .seg = {
            { 0, 35 },
            { 37, 3 },
            { 0, 0 }
        }
    },
    {
        .id = 326,
        .modId = GOAL_ID_MI_NVS,
        .textId = 276,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Failed to start cyclic timer",
        .seg = {
            { 0, 28 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 327,
        .modId = GOAL_ID_TGT,
        .textId = 277,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Could not create semaphore, halting device.",
        .seg = {
            { 0, 43 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 328,
        .modId = GOAL_ID_TGT,
        .textId = 278,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to allocate target data",
        .seg = {
            { 0, 30 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 329,
        .modId = GOAL_ID_TGT,
        .textId = 279,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "an unauthorized thread attempted to free a lock",
        .seg = {
            { 0, 47 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 330,
        .modId = GOAL_ID_TGT,
        .textId = 280,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Error while deleting semaphore, halting device.",
        .seg = {
            { 0, 47 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 331,
        .modId = GOAL_ID_TGT,
        .textId = 278,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to allocate target data",
        .seg = {
            { 0, 30 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 332,
        .modId = GOAL_ID_TGT,
        .textId = 281,
        .severity = GOAL_LOG_SEV_WARNING,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "show remaining time is currently not supported by this target",
        .seg = {
            { 0, 61 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 333,
        .modId = GOAL_ID_TGT,
        .textId = 282,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "unexpected task shutdown",
        .seg = {
            { 0, 24 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 334,
        .modId = GOAL_ID_TGT,
        .textId = 283,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to allocate interrupt resource",
        .seg = {
            { 0, 37 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 335,
        .modId = GOAL_ID_TGT_BOARD,
        .textId = 284,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to register task mapping table",
        .seg = {
            { 0, 37 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 336,
        .modId = GOAL_ID_TGT_BOARD,
        .textId = 285,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Could not register nvs driver",
        .seg = {
            { 0, 29 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 337,
        .modId = GOAL_ID_TGT_BOARD,
        .textId = 286,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Could not register nvs eeprom MA",
        .seg = {
            { 0, 32 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 338,
        .modId = GOAL_ID_TGT_BOARD,
        .textId = 287,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Unable to register MI MCTC SPI",
        .seg = {
            { 0, 30 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 339,
        .modId = GOAL_ID_TGT_BOARD,
        .textId = 288,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Failed to register TLS driver",
        .seg = {
            { 0, 29 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 340,
        .modId = GOAL_ID_TGT_BOARD,
        .textId = 289,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "out of memory: allocating PHY data structures failed",
        .seg = {
            { 0, 52 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 341,
        .modId = GOAL_ID_TGT_BOARD,
        .textId = 290,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "error opening MDIO bus",
        .seg = {
            { 0, 22 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 342,
        .modId = GOAL_ID_TGT_BOARD,
        .textId = 291,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "error opening MEM bus",
        .seg = {
            { 0, 21 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 343,
        .modId = GOAL_ID_TGT_BOARD,
        .textId = 292,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "3-port advanced switch not found",
        .seg = {
            { 0, 32 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 344,
        .modId = GOAL_ID_TGT_BOARD,
        .textId = 293,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Switch port count doesn't match board port count",
        .seg = {
            { 0, 48 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 345,
        .modId = GOAL_ID_TGT_BOARD,
        .textId = 294,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Region for CM config not found",
        .seg = {
            { 0, 30 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 346,
        .modId = GOAL_ID_TGT_BOARD,
        .textId = 294,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Region for CM config not found",
        .seg = {
            { 0, 30 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 347,
        .modId = GOAL_ID_TGT_BOARD,
        .textId = 295,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Writing of CM storage failed",
        .seg = {
            { 0, 28 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 348,
        .modId = GOAL_ID_DRV_MDIO,
        .textId = 296,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "couldn't allocate MDIO bus handle",
        .seg = {
            { 0, 33 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 349,
        .modId = GOAL_ID_DRV_MDIO,
        .textId = 297,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "value points to null",
        .seg = {
            { 0, 20 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 350,
        .modId = GOAL_ID_DRV_MDIO,
        .textId = 298,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "only 32-bit reads are allowed",
        .seg = {
            { 0, 29 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 351,
        .modId = GOAL_ID_DRV_MDIO,
        .textId = 299,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "timeout for read access",
        .seg = {
            { 0, 23 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 352,
        .modId = GOAL_ID_DRV_MDIO,
        .textId = 300,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "timeout while waiting for ready flag",
        .seg = {
            { 0, 36 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 353,
        .modId = GOAL_ID_DRV_MDIO,
        .textId = 297,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "value points to null",
        .seg = {
            { 0, 20 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 354,
        .modId = GOAL_ID_DRV_MDIO,
        .textId = 298,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "only 32-bit reads are allowed",
        .seg = {
            { 0, 29 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 355,
        .modId = GOAL_ID_DRV_MDIO,
        .textId = 301,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "timeout for write access",
        .seg = {
            { 0, 24 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 356,
        .modId = GOAL_ID_DRV_MDIO,
        .textId = 300,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "timeout while waiting for ready flag",
        .seg = {
            { 0, 36 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 357,
        .modId = GOAL_ID_TGT,
        .textId = 302,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "couldn't allocate MEM bus handle",
        .seg = {
            { 0, 32 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 358,
        .modId = GOAL_ID_TGT,
        .textId = 297,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "value points to null",
        .seg = {
            { 0, 20 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 359,
        .modId = GOAL_ID_TGT,
        .textId = 298,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "only 32-bit reads are allowed",
        .seg = {
            { 0, 29 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 360,
        .modId = GOAL_ID_TGT,
        .textId = 297,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "value points to null",
        .seg = {
            { 0, 20 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 361,
        .modId = GOAL_ID_TGT,
        .textId = 303,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "only 32-bit writes are allowed",
        .seg = {
            { 0, 30 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 362,
        .modId = GOAL_ID_NET_CTC,
        .textId = 304,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "called",
        .seg = {
            { 0, 6 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 363,
        .modId = GOAL_ID_NET_CTC,
        .textId = 304,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "called",
        .seg = {
            { 0, 6 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 364,
        .modId = GOAL_ID_NET_CTC,
        .textId = 304,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "called",
        .seg = {
            { 0, 6 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 365,
        .modId = GOAL_ID_NET_CTC,
        .textId = 304,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "called",
        .seg = {
            { 0, 6 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 366,
        .modId = GOAL_ID_NET_CTC,
        .textId = 304,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "called",
        .seg = {
            { 0, 6 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 367,
        .modId = GOAL_ID_NET_CTC,
        .textId = 304,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "called",
        .seg = {
            { 0, 6 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 368,
        .modId = GOAL_ID_NET_CTC,
        .textId = 304,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "called",
        .seg = {
            { 0, 6 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 369,
        .modId = GOAL_ID_NET_CTC,
        .textId = 304,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "called",
        .seg = {
            { 0, 6 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 370,
        .modId = GOAL_ID_DRV_ETH,
        .textId = 305,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to read MAC address",
        .seg = {
            { 0, 26 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 371,
        .modId = GOAL_ID_DRV_ETH,
        .textId = 306,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Failed to intialize DMA Tx buffer",
        .seg = {
            { 0, 33 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 372,
        .modId = GOAL_ID_DRV_ETH,
        .textId = 307,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Failed to request buffer",
        .seg = {
            { 0, 24 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 373,
        .modId = GOAL_ID_DRV_ETH,
        .textId = 308,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Failed to allocate Ethernet receive buffer",
        .seg = {
            { 0, 42 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 374,
        .modId = GOAL_ID_DRV_ETH,
        .textId = 309,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Error allocating transmission buffer.",
        .seg = {
            { 0, 37 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 375,
        .modId = GOAL_ID_DRV_ETH,
        .textId = 310,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Error waiting for flag",
        .seg = {
            { 0, 22 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 376,
        .modId = GOAL_ID_DRV_ETH,
        .textId = 311,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Error clearing flag.",
        .seg = {
            { 0, 20 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 377,
        .modId = GOAL_ID_DRV_ETH,
        .textId = 312,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_UINT32,
        .p1Pos = 17,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "MAC error status $1",
        .seg = {
            { 0, 17 },
            { 19, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 378,
        .modId = GOAL_ID_DRV_ETH,
        .textId = 313,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "No buffer and oversized frame: skipped",
        .seg = {
            { 0, 38 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 379,
        .modId = GOAL_ID_DRV_ETH,
        .textId = 314,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Reception was disabled: skipped",
        .seg = {
            { 0, 31 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 380,
        .modId = GOAL_ID_DRV_ETH,
        .textId = 315,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Oversized frame and disabled reception: skipped",
        .seg = {
            { 0, 47 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 381,
        .modId = GOAL_ID_DRV_ETH,
        .textId = 316,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Data in buffer is invalid: discarded",
        .seg = {
            { 0, 36 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 382,
        .modId = GOAL_ID_DRV_ETH,
        .textId = 317,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Buffer release failed.",
        .seg = {
            { 0, 22 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 383,
        .modId = GOAL_ID_DRV_ETH,
        .textId = 318,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "No buffer and disabled reception: discarded",
        .seg = {
            { 0, 43 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 384,
        .modId = GOAL_ID_DRV_ETH,
        .textId = 317,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Buffer release failed.",
        .seg = {
            { 0, 22 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 385,
        .modId = GOAL_ID_DRV_ETH,
        .textId = 318,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "No buffer and disabled reception: discarded",
        .seg = {
            { 0, 43 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 386,
        .modId = GOAL_ID_DRV_ETH,
        .textId = 317,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Buffer release failed.",
        .seg = {
            { 0, 22 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 387,
        .modId = GOAL_ID_DRV_ETH,
        .textId = 319,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "DMA error: discarded",
        .seg = {
            { 0, 20 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 388,
        .modId = GOAL_ID_DRV_ETH,
        .textId = 317,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Buffer release failed.",
        .seg = {
            { 0, 22 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 389,
        .modId = GOAL_ID_DRV_ETH,
        .textId = 320,
        .severity = GOAL_LOG_SEV_WARNING,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "> CRC error",
        .seg = {
            { 0, 11 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 390,
        .modId = GOAL_ID_DRV_ETH,
        .textId = 321,
        .severity = GOAL_LOG_SEV_WARNING,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "> encoding error",
        .seg = {
            { 0, 16 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 391,
        .modId = GOAL_ID_DRV_ETH,
        .textId = 322,
        .severity = GOAL_LOG_SEV_WARNING,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "> FIFO full",
        .seg = {
            { 0, 11 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 392,
        .modId = GOAL_ID_DRV_ETH,
        .textId = 323,
        .severity = GOAL_LOG_SEV_WARNING,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "> frame too short",
        .seg = {
            { 0, 17 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 393,
        .modId = GOAL_ID_DRV_ETH,
        .textId = 324,
        .severity = GOAL_LOG_SEV_WARNING,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "> frame too long",
        .seg = {
            { 0, 16 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 394,
        .modId = GOAL_ID_DRV_ETH,
        .textId = 317,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Buffer release failed.",
        .seg = {
            { 0, 22 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 395,
        .modId = GOAL_ID_DRV_ETH,
        .textId = 325,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Buffer release call not completed.",
        .seg = {
            { 0, 34 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 396,
        .modId = GOAL_ID_DRV_ETH,
        .textId = 326,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Invalid system call",
        .seg = {
            { 0, 19 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 397,
        .modId = GOAL_ID_DRV_ETH,
        .textId = 327,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Buffer adrress was invalid",
        .seg = {
            { 0, 26 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 398,
        .modId = GOAL_ID_DRV_ETH,
        .textId = 328,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "error while initializing Switch DLR handling",
        .seg = {
            { 0, 44 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 399,
        .modId = GOAL_ID_DRV_ETH,
        .textId = 329,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "error while enabling Switch DLR handling",
        .seg = {
            { 0, 40 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 400,
        .modId = GOAL_ID_DRV_ETH,
        .textId = 330,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_UINT32,
        .p1Pos = 41,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Direct memory transfer failed with code: $1. Nothing transmitted.",
        .seg = {
            { 0, 41 },
            { 43, 22 },
            { 0, 0 }
        }
    },
    {
        .id = 401,
        .modId = GOAL_ID_DRV_ETH,
        .textId = 331,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_INT32,
        .p1Pos = 34,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Error while waiting for DMA flag: $1",
        .seg = {
            { 0, 34 },
            { 36, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 402,
        .modId = GOAL_ID_DRV_ETH,
        .textId = 332,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = " DMA transmit error",
        .seg = {
            { 0, 19 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 403,
        .modId = GOAL_ID_DRV_ETH,
        .textId = 333,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "FIFO error",
        .seg = {
            { 0, 10 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 404,
        .modId = GOAL_ID_DRV_ETH,
        .textId = 334,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "FIFO underflow",
        .seg = {
            { 0, 14 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 405,
        .modId = GOAL_ID_DRV_ETH,
        .textId = 335,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "incomplete DMA transfer",
        .seg = {
            { 0, 23 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 406,
        .modId = GOAL_ID_DRV_ETH,
        .textId = 336,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "MAC not completed operation",
        .seg = {
            { 0, 27 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 407,
        .modId = GOAL_ID_DRV_ETH,
        .textId = 337,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "MAC error while sending",
        .seg = {
            { 0, 23 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 408,
        .modId = GOAL_ID_DRV_ETH,
        .textId = 338,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_UINT32,
        .p1Pos = 20,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Transmission error: $1",
        .seg = {
            { 0, 20 },
            { 22, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 409,
        .modId = GOAL_ID_DRV_NET,
        .textId = 339,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to create access lock",
        .seg = {
            { 0, 28 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 410,
        .modId = GOAL_ID_DRV_NET,
        .textId = 340,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to create resource lock",
        .seg = {
            { 0, 30 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 411,
        .modId = GOAL_ID_DRV_NET,
        .textId = 341,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Couldn't add network interface for lwIP",
        .seg = {
            { 0, 39 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 412,
        .modId = GOAL_ID_DRV_NET,
        .textId = 342,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to setup GOAL timer for lwIP",
        .seg = {
            { 0, 35 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 413,
        .modId = GOAL_ID_DRV_NET,
        .textId = 343,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to register EtherType IPv4",
        .seg = {
            { 0, 33 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 414,
        .modId = GOAL_ID_DRV_NET,
        .textId = 344,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to register EtherType ARP",
        .seg = {
            { 0, 32 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 415,
        .modId = GOAL_ID_DRV_NET,
        .textId = 345,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Couldn't read MAC address during lwIP initialization",
        .seg = {
            { 0, 52 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 416,
        .modId = GOAL_ID_DRV_NET,
        .textId = 346,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Allocation of lwIp buffer failed",
        .seg = {
            { 0, 32 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 417,
        .modId = GOAL_ID_DRV_NET,
        .textId = 347,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "No free target channel found.",
        .seg = {
            { 0, 29 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 418,
        .modId = GOAL_ID_DRV_NET,
        .textId = 348,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Couldn't create new UDP channel descriptor",
        .seg = {
            { 0, 42 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 419,
        .modId = GOAL_ID_DRV_NET,
        .textId = 349,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Couldn't connect UDP channel",
        .seg = {
            { 0, 28 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 420,
        .modId = GOAL_ID_DRV_NET,
        .textId = 350,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Couldn't create new TCP channel descriptor",
        .seg = {
            { 0, 42 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 421,
        .modId = GOAL_ID_DRV_NET,
        .textId = 351,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Couldn't bind TCP channel",
        .seg = {
            { 0, 25 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 422,
        .modId = GOAL_ID_DRV_NET,
        .textId = 352,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Couldn't set TCP Listener to listen mode",
        .seg = {
            { 0, 40 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 423,
        .modId = GOAL_ID_DRV_NET,
        .textId = 347,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "No free target channel found.",
        .seg = {
            { 0, 29 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 424,
        .modId = GOAL_ID_DRV_NET,
        .textId = 350,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Couldn't create new TCP channel descriptor",
        .seg = {
            { 0, 42 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 425,
        .modId = GOAL_ID_DRV_NET,
        .textId = 351,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Couldn't bind TCP channel",
        .seg = {
            { 0, 25 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 426,
        .modId = GOAL_ID_DRV_NET,
        .textId = 353,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to connect",
        .seg = {
            { 0, 17 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 427,
        .modId = GOAL_ID_DRV_NET,
        .textId = 354,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_PTR,
        .p1Pos = 31,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Could not close target channel $1",
        .seg = {
            { 0, 31 },
            { 33, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 428,
        .modId = GOAL_ID_DRV_NET,
        .textId = 355,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_PTR,
        .p1Pos = 33,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Could not re-open target channel $1",
        .seg = {
            { 0, 33 },
            { 35, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 429,
        .modId = GOAL_ID_DRV_NET,
        .textId = 346,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Allocation of lwIp buffer failed",
        .seg = {
            { 0, 32 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 430,
        .modId = GOAL_ID_DRV_NET,
        .textId = 356,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_INT32,
        .p1Pos = 15,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "invalid option $1",
        .seg = {
            { 0, 15 },
            { 17, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 431,
        .modId = GOAL_ID_DRV_NET,
        .textId = 346,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Allocation of lwIp buffer failed",
        .seg = {
            { 0, 32 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 432,
        .modId = GOAL_ID_DRV_NET,
        .textId = 357,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Unable to allocate a free GOAL buffer",
        .seg = {
            { 0, 37 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 433,
        .modId = GOAL_ID_DRV_NET,
        .textId = 358,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Unable to send frame",
        .seg = {
            { 0, 20 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 434,
        .modId = GOAL_ID_DRV_NET,
        .textId = 359,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Couldn't get a new GOAL buffer",
        .seg = {
            { 0, 30 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 435,
        .modId = GOAL_ID_DRV_NET,
        .textId = 360,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Couldn't free any lwIp buffer",
        .seg = {
            { 0, 29 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 436,
        .modId = GOAL_ID_DRV_NET,
        .textId = 361,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Did not find target channel handle belongig to listener descriptor",
        .seg = {
            { 0, 66 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 437,
        .modId = GOAL_ID_DRV_NET,
        .textId = 362,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Did not find target channel handle belongig to listener",
        .seg = {
            { 0, 55 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 438,
        .modId = GOAL_ID_DRV_NET,
        .textId = 363,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "error getting channel from GOAL",
        .seg = {
            { 0, 31 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 439,
        .modId = GOAL_ID_DRV_NET,
        .textId = 364,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Could not find target handle to given descriptor.",
        .seg = {
            { 0, 49 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 440,
        .modId = GOAL_ID_DRV_NET,
        .textId = 365,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_PTR,
        .p1Pos = 29,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "no channel handle for socket $1.",
        .seg = {
            { 0, 29 },
            { 31, 1 },
            { 0, 0 }
        }
    },
    {
        .id = 441,
        .modId = GOAL_ID_DRV_NET,
        .textId = 359,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Couldn't get a new GOAL buffer",
        .seg = {
            { 0, 30 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 442,
        .modId = GOAL_ID_DRV_NET,
        .textId = 360,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Couldn't free any lwIp buffer",
        .seg = {
            { 0, 29 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 443,
        .modId = GOAL_ID_DRV_NET,
        .textId = 366,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_PTR,
        .p1Pos = 29,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "no channel handle for socket $1 (target socket address)",
        .seg = {
            { 0, 29 },
            { 31, 24 },
            { 0, 0 }
        }
    },
    {
        .id = 444,
        .modId = GOAL_ID_DRV_NET,
        .textId = 367,
        .severity = GOAL_LOG_SEV_WARNING,
        .p1Type = LM_INT32,
        .p1Pos = 14,
        .p2Type = LM_PTR,
        .p2Pos = 28,
        .text = "connect error $1 for socket $2 (target socket address)",
        .seg = {
            { 0, 14 },
            { 16, 12 },
            { 30, 24 }
        }
    },
    {
        .id = 445,
        .modId = GOAL_ID_DRV_NET,
        .textId = 368,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_PTR,
        .p1Pos = 29,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "no channel handle for socket $1 (target socket address)",
        .seg = {
            { 0, 29 },
            { 31, 24 },
            { 0, 0 }
        }
    },
    {
        .id = 446,
        .modId = GOAL_ID_DRV_NET,
        .textId = 369,
        .severity = GOAL_LOG_SEV_WARNING,
        .p1Type = LM_INT32,
        .p1Pos = 14,
        .p2Type = LM_PTR,
        .p2Pos = 28,
        .text = "connect error $1 for socket $2 (target socket address)",
        .seg = {
            { 0, 14 },
            { 16, 12 },
            { 30, 24 }
        }
    },
    {
        .id = 447,
        .modId = GOAL_ID_DRV_NVS,
        .textId = 370,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Could not register nvs MA",
        .seg = {
            { 0, 25 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 448,
        .modId = GOAL_ID_DRV_PHY,
        .textId = 371,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "interface data is null",
        .seg = {
            { 0, 22 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 449,
        .modId = GOAL_ID_DRV_PHY,
        .textId = 372,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_UINT32,
        .p1Pos = 4,
        .p2Type = LM_UINT32,
        .p2Pos = 24,
        .text = "PHY $1 found at address $2",
        .seg = {
            { 0, 4 },
            { 6, 18 },
            { 26, 0 }
        }
    },
    {
        .id = 450,
        .modId = GOAL_ID_DRV_PHY,
        .textId = 373,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "no PHYs found",
        .seg = {
            { 0, 13 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 451,
        .modId = GOAL_ID_DRV_PHY,
        .textId = 289,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "out of memory: allocating PHY data structures failed",
        .seg = {
            { 0, 52 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 452,
        .modId = GOAL_ID_DRV_PHY,
        .textId = 374,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_UINT32,
        .p1Pos = 28,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to initialize PHY id $1 with generic PHY driver",
        .seg = {
            { 0, 28 },
            { 30, 24 },
            { 0, 0 }
        }
    },
    {
        .id = 453,
        .modId = GOAL_ID_DRV_PHY,
        .textId = 375,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "unknown speed setting",
        .seg = {
            { 0, 21 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 454,
        .modId = GOAL_ID_DRV_PHY,
        .textId = 376,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "unknown duplex mode",
        .seg = {
            { 0, 19 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 455,
        .modId = GOAL_ID_DRV_PHY,
        .textId = 377,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_UINT32,
        .p1Pos = 15,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "unknown PHY ID $1",
        .seg = {
            { 0, 15 },
            { 17, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 456,
        .modId = GOAL_ID_DRV_PHY,
        .textId = 378,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_UINT32,
        .p1Pos = 15,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "supported PHY: $1",
        .seg = {
            { 0, 15 },
            { 17, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 457,
        .modId = GOAL_ID_DRV_PHY,
        .textId = 379,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_UINT32,
        .p1Pos = 15,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "supported PHY: $1",
        .seg = {
            { 0, 15 },
            { 17, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 458,
        .modId = GOAL_ID_DRV_PHY,
        .textId = 380,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_UINT32,
        .p1Pos = 15,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "supported PHY: $1",
        .seg = {
            { 0, 15 },
            { 17, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 459,
        .modId = GOAL_ID_DRV_PHY,
        .textId = 381,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_UINT32,
        .p1Pos = 21,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "PHY found at address $1: Microchip KSZ 8041",
        .seg = {
            { 0, 21 },
            { 23, 20 },
            { 0, 0 }
        }
    },
    {
        .id = 460,
        .modId = GOAL_ID_DRV_SPI,
        .textId = 382,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "SPI master not supported",
        .seg = {
            { 0, 24 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 461,
        .modId = GOAL_ID_DRV_SPI,
        .textId = 383,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to allocate driver data",
        .seg = {
            { 0, 30 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 462,
        .modId = GOAL_ID_DRV_SPI,
        .textId = 384,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to register SPI driver in MA SPI",
        .seg = {
            { 0, 39 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 463,
        .modId = GOAL_ID_DRV_SPI,
        .textId = 385,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to create communication task",
        .seg = {
            { 0, 35 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 464,
        .modId = GOAL_ID_DRV_SWITCH,
        .textId = 371,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "interface data is null",
        .seg = {
            { 0, 22 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 465,
        .modId = GOAL_ID_DRV_SWITCH,
        .textId = 386,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "error while reading Switch identifier",
        .seg = {
            { 0, 37 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 466,
        .modId = GOAL_ID_DRV_SWITCH,
        .textId = 387,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_UINT32,
        .p1Pos = 17,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "found Switch ID: $1",
        .seg = {
            { 0, 17 },
            { 19, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 467,
        .modId = GOAL_ID_DRV_SWITCH,
        .textId = 388,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to set management port",
        .seg = {
            { 0, 29 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 468,
        .modId = GOAL_ID_DRV_SWITCH,
        .textId = 389,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to map port index to target",
        .seg = {
            { 0, 34 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 469,
        .modId = GOAL_ID_DRV_SWITCH,
        .textId = 390,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to create Learning Task",
        .seg = {
            { 0, 30 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 470,
        .modId = GOAL_ID_DRV_SWITCH,
        .textId = 390,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to create Learning Task",
        .seg = {
            { 0, 30 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 471,
        .modId = GOAL_ID_DRV_SWITCH,
        .textId = 113,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to get MAC address",
        .seg = {
            { 0, 25 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 472,
        .modId = GOAL_ID_DRV_SWITCH,
        .textId = 391,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "invalid port id",
        .seg = {
            { 0, 15 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 473,
        .modId = GOAL_ID_DRV_SWITCH,
        .textId = 101,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "need a portmask to proceed",
        .seg = {
            { 0, 26 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 474,
        .modId = GOAL_ID_DRV_SWITCH,
        .textId = 392,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to find oldest entry within block",
        .seg = {
            { 0, 40 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 475,
        .modId = GOAL_ID_DRV_SWITCH,
        .textId = 393,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "block contains no dynamic entry, dropping record",
        .seg = {
            { 0, 48 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 476,
        .modId = GOAL_ID_DRV_WD,
        .textId = 383,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to allocate driver data",
        .seg = {
            { 0, 30 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 477,
        .modId = GOAL_ID_DRV_WD,
        .textId = 394,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to register WD driver in MA WD",
        .seg = {
            { 0, 37 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 478,
        .modId = GOAL_ID_DRV_ETH,
        .textId = 395,
        .severity = GOAL_LOG_SEV_WARNING,
        .p1Type = LM_UINT32,
        .p1Pos = 33,
        .p2Type = LM_UINT32,
        .p2Pos = 42,
        .text = "frame validation failed, length: $1 (raw: $2)",
        .seg = {
            { 0, 33 },
            { 35, 7 },
            { 44, 1 }
        }
    },
    {
        .id = 479,
        .modId = GOAL_ID_DRV_SWITCH,
        .textId = 396,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_UINT32,
        .p1Pos = 22,
        .p2Type = LM_UINT32,
        .p2Pos = 42,
        .text = "unsupported Switch ID $1, supporting only $2",
        .seg = {
            { 0, 22 },
            { 24, 18 },
            { 44, 0 }
        }
    },
    {
        .id = 480,
        .modId = GOAL_ID_TGT,
        .textId = 397,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_STRING0,
        .p1Pos = 13,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "task create: $1",
        .seg = {
            { 0, 13 },
            { 15, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 481,
        .modId = GOAL_ID_TGT,
        .textId = 398,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_INT32,
        .p1Pos = 9,
        .p2Type = LM_UINT32,
        .p2Pos = 20,
        .text = "OS prio: $1, stack: $2",
        .seg = {
            { 0, 9 },
            { 11, 9 },
            { 22, 0 }
        }
    },
    {
        .id = 482,
        .modId = GOAL_ID_MEM,
        .textId = 399,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_UINT16,
        .p1Pos = 14,
        .p2Type = LM_UINT32,
        .p2Pos = 19,
        .text = "heap usage id($1): $2 Bytes",
        .seg = {
            { 0, 14 },
            { 16, 3 },
            { 21, 6 }
        }
    },
    {
        .id = 483,
        .modId = GOAL_ID_MEM,
        .textId = 400,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_UINT32,
        .p1Pos = 26,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "memory allocation number: $1",
        .seg = {
            { 0, 26 },
            { 28, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 484,
        .modId = GOAL_ID_MEM,
        .textId = 401,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_UINT32,
        .p1Pos = 38,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "memory allocation alignment overhead: $1",
        .seg = {
            { 0, 38 },
            { 40, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 485,
        .modId = GOAL_ID_DRV_PHY,
        .textId = 402,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_UINT32,
        .p1Pos = 21,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "PHY found at address $1: Renesas RIN32M3 Phy",
        .seg = {
            { 0, 21 },
            { 23, 21 },
            { 0, 0 }
        }
    },
    {
        .id = 486,
        .modId = GOAL_ID_DRV_PHY,
        .textId = 378,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_UINT32,
        .p1Pos = 15,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "supported PHY: $1",
        .seg = {
            { 0, 15 },
            { 17, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 487,
        .modId = GOAL_ID_DRV_SWITCH,
        .textId = 403,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "failed to add host MAC as static MAC Table entry",
        .seg = {
            { 0, 48 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 488,
        .modId = GOAL_ID_DD,
        .textId = 404,
        .severity = GOAL_LOG_SEV_DEBUG,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Encrypted data would not match into processing buffer",
        .seg = {
            { 0, 53 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 489,
        .modId = GOAL_ID_DD,
        .textId = 405,
        .severity = GOAL_LOG_SEV_DEBUG,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Failed to decrypt received package",
        .seg = {
            { 0, 34 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 490,
        .modId = GOAL_ID_DD,
        .textId = 406,
        .severity = GOAL_LOG_SEV_DEBUG,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Failed to encrypt reply package",
        .seg = {
            { 0, 31 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 491,
        .modId = GOAL_ID_DD,
        .textId = 407,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Failed to get cryptography options from CM variables",
        .seg = {
            { 0, 52 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 492,
        .modId = GOAL_ID_DD,
        .textId = 408,
        .severity = GOAL_LOG_SEV_DEBUG,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Encryption options of received dd package and device don't match, drop package",
        .seg = {
            { 0, 78 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 493,
        .modId = GOAL_ID_DD,
        .textId = 409,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "NULL pointer error",
        .seg = {
            { 0, 18 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 494,
        .modId = GOAL_ID_MA_TLS,
        .textId = 409,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "NULL pointer error",
        .seg = {
            { 0, 18 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 495,
        .modId = GOAL_ID_MI_TLS,
        .textId = 409,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "NULL pointer error",
        .seg = {
            { 0, 18 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 496,
        .modId = GOAL_ID_MI_TLS,
        .textId = 410,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Could not allocate memory for MI TLS session",
        .seg = {
            { 0, 44 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 497,
        .modId = GOAL_ID_MI_TLS,
        .textId = 411,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Can't initialize ring buffer for communication",
        .seg = {
            { 0, 46 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 498,
        .modId = GOAL_ID_MI_TLS,
        .textId = 412,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Could not clear MI TLS session",
        .seg = {
            { 0, 30 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 499,
        .modId = GOAL_ID_MI_TLS,
        .textId = 413,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Could not open TLS channel",
        .seg = {
            { 0, 26 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 500,
        .modId = GOAL_ID_MI_TLS,
        .textId = 414,
        .severity = GOAL_LOG_SEV_DEBUG,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "No channel to close",
        .seg = {
            { 0, 19 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 501,
        .modId = GOAL_ID_MI_TLS,
        .textId = 415,
        .severity = GOAL_LOG_SEV_DEBUG,
        .p1Type = LM_PTR,
        .p1Pos = 30,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "New TLS connection on channel $1",
        .seg = {
            { 0, 30 },
            { 32, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 502,
        .modId = GOAL_ID_MI_TLS,
        .textId = 416,
        .severity = GOAL_LOG_SEV_DEBUG,
        .p1Type = LM_PTR,
        .p1Pos = 24,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "New TLS data on channel $1",
        .seg = {
            { 0, 24 },
            { 26, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 503,
        .modId = GOAL_ID_MI_TLS,
        .textId = 417,
        .severity = GOAL_LOG_SEV_DEBUG,
        .p1Type = LM_PTR,
        .p1Pos = 41,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "TLS connection closed by peer on channel $1",
        .seg = {
            { 0, 41 },
            { 43, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 504,
        .modId = GOAL_ID_MI_TLS,
        .textId = 418,
        .severity = GOAL_LOG_SEV_DEBUG,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Did not find session for net channel",
        .seg = {
            { 0, 36 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 505,
        .modId = GOAL_ID_MI_TLS,
        .textId = 419,
        .severity = GOAL_LOG_SEV_DEBUG,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Could not get TLS channel of net channel, connection maybe closed",
        .seg = {
            { 0, 65 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 506,
        .modId = GOAL_ID_MI_TLS,
        .textId = 420,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Cannot store data in ring buffer",
        .seg = {
            { 0, 32 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 507,
        .modId = GOAL_ID_MI_TLS,
        .textId = 421,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "Failed to read from ring buffer",
        .seg = {
            { 0, 31 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 508,
        .modId = GOAL_ID_MI_TLS,
        .textId = 422,
        .severity = GOAL_LOG_SEV_INFO,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "No valid certificate or key stored, use default certificate and key",
        .seg = {
            { 0, 67 },
            { 0, 0 },
            { 0, 0 }
        }
    },
    {
        .id = 509,
        .modId = GOAL_ID_MI_TLS,
        .textId = 423,
        .severity = GOAL_LOG_SEV_DEBUG,
        .p1Type = LM_PTR,
        .p1Pos = 10,
        .p2Type = LM_UINT32,
        .p2Pos = 14,
        .text = "Event for $1: $2",
        .seg = {
            { 0, 10 },
            { 12, 2 },
            { 16, 0 }
        }
    },
    {
        .id = 510,
        .modId = GOAL_ID_MI_TLS,
        .textId = 424,
        .severity = GOAL_LOG_SEV_DEBUG,
        .p1Type = LM_PTR,
        .p1Pos = 21,
        .p2Type = LM_UINT32,
        .p2Pos = 25,
        .text = "New MI TLS state for $1: $2",
        .seg = {
            { 0, 21 },
            { 23, 2 },
            { 27, 0 }
        }
    },
    {
        .id = 511,
        .modId = GOAL_ID_MA_TLS,
        .textId = 425,
        .severity = GOAL_LOG_SEV_ERROR,
        .p1Type = LM_None,
        .p1Pos = -1,
        .p2Type = LM_None,
        .p2Pos = -1,
        .text = "TLS MA with ID is already in use",
        .seg = {
            { 0, 32 },
            { 0, 0 },
            { 0, 0 }
        }
    }
};

#endif /* GOAL_LM_DB_H */
