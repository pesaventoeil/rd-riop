/** @file
 *
 * @brief application template
 *
 * This module provides a micro core to core application using GOAL for
 * demonstrating DM group handling.
 * The application handles twe groups. First contains two partitions with even
 * counter values, while the second group contains one partition with odd counter
 * values at a fixed position within DM. The partition index is choosen thus group1
 * partitions are split (prior and post) group2.
 *
 * | part1 of group1 | unused | part1 of group2 | part2 of group1 | unused | RPC |
 *
 * The partitions of group1 are un- and reregisterd in an APPL_UNREG_TIMOUT ms period.
 * This application is used for AC and CC.
 *
 * @copyright
 * Copyright 2010-2024.
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
#include <goal_media/goal_mi_mctc.h>


/****************************************************************************/
/* Defines */
/****************************************************************************/
#define APPL_RW_TIMEOUT 2000                    /**< IO timeout in ms */
#define APPL_SM_TIMOUT 10000                    /**< State Machine timeout in ms */

#define GOAL_ID_APPL_GROUP_1 ((GOAL_ID_T) ((1 << 8) | GOAL_ID_APPL)) /**< GOAL ID for group 1 */
#define GOAL_ID_APPL_GROUP_2 ((GOAL_ID_T) ((2 << 8) | GOAL_ID_APPL)) /**< GOAL ID for group 2 */

#define APPL_GROUP_2_OFFSET 5                   /**< group 2 offset within DM */


/****************************************************************************/
/* Typedefs */
/****************************************************************************/
GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct {
    uint32_t value;                             /**< message counter */
} GOAL_TARGET_PACKED APPL_MSG_T;
GOAL_TARGET_PACKED_STRUCT_POST

typedef enum {
    APPL_SM_WAIT_0 = 0,                         /**< Waiting state 0 */
    APPL_SM_REG,                                /**< Register Group 1 */
    APPL_SM_WAIT_1,                             /**< Waiting state 1 */
    APPL_SM_UNREG,                              /**< Unregister Group 1 */
} APPL_SM_T;


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_TIMER_T *pTmrIo;                    /**< IO timer handle */
static GOAL_TIMER_T *pTmrSm;                    /**< State machine timer handle */

static APPL_MSG_T msgData1;                     /**< message data 1 (group 1) */
static APPL_MSG_T msgData2;                     /**< message data 2 (group 1) */
static APPL_MSG_T msgData3;                     /**< message data 3 (group 2) */

static GOAL_MI_DM_PART_T mMiDmRead1;            /**< data 1 read partition */
static GOAL_MI_DM_PART_T mMiDmWrite1;           /**< data 1 write partition */
static GOAL_MI_DM_PART_T mMiDmRead2;            /**< data 2 read partition */
static GOAL_MI_DM_PART_T mMiDmWrite2;           /**< data 2 write partition */
static GOAL_MI_DM_PART_T mMiDmRead3;            /**< data 3 read partition */
static GOAL_MI_DM_PART_T mMiDmWrite3;           /**< data 3 write partition */

static APPL_SM_T stageAppl;                     /**< application state machine */


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/
static GOAL_STATUS_T appl_cfgDmParts(
    GOAL_ID_T idGroup,                          /**< [in] group id */
    GOAL_BOOL_T flgReg                          /**< [in] flag: GOAL_TRUE - register partitions, GOAL_FALSE - unregister partitions */
);

static GOAL_STATUS_T appl_dataCheck(
    GOAL_MI_DM_PART_T *pR,                      /**< [in] data read partition */
    GOAL_MI_DM_PART_T *pW,                      /**< [in] data write partition */
    APPL_MSG_T *pData                           /**< [in] process data reference */
);

static void appl_rw(
    void *pArg                                  /**< argument */
);

static void appl_sm(
    void *pArg                                  /**< argument */
);


/****************************************************************************/
/** Application configure DM partitions
 *
 * Registration or unregistration of DM partitions.
 * One partition of GOAL_ID_APPL_GROUP_2 is registered to APPL_GROUP_2_OFFSET.
 * Two partitions of GOAL_ID_APPL_GROUP_1 are registered dynamic. Due to
 * group2 partition, they get registered prior and post group2.
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T appl_cfgDmParts(
    GOAL_ID_T idGroup,                          /**< [in] group id */
    GOAL_BOOL_T flgReg                          /**< [in] flag: GOAL_TRUE - register partitions, GOAL_FALSE - unregister partitions */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    if (GOAL_TRUE == flgReg) {
        /* register partitions */
        if (GOAL_ID_APPL_GROUP_1 == idGroup) {
            /* register MI DM partitions 1 (group 1) */
            res = goal_miDmPartReg(GOAL_MI_MCTC_DIR_PEER_FROM, idGroup, &mMiDmRead1, sizeof(APPL_MSG_T));
            if (GOAL_RES_ERR(res)) {
                goal_logErr("failed to register cyclic read part");
            }

            if (GOAL_RES_OK(res)) {
                res = goal_miDmPartReg(GOAL_MI_MCTC_DIR_PEER_TO, idGroup, &mMiDmWrite1, sizeof(APPL_MSG_T));
                if (GOAL_RES_ERR(res)) {
                    goal_logErr("failed to register cyclic write part");
                }
            }

            /* register MI DM partitions 2 (group 1) */
            if (GOAL_RES_OK(res)) {
                res = goal_miDmPartReg(GOAL_MI_MCTC_DIR_PEER_FROM, idGroup, &mMiDmRead2, sizeof(APPL_MSG_T));
                if (GOAL_RES_ERR(res)) {
                    goal_logErr("failed to register cyclic read part");
                }
            }

            if (GOAL_RES_OK(res)) {
                res = goal_miDmPartReg(GOAL_MI_MCTC_DIR_PEER_TO, idGroup, &mMiDmWrite2, sizeof(APPL_MSG_T));
                if (GOAL_RES_ERR(res)) {
                    goal_logErr("failed to register cyclic write part");
                }
            }
        } else if (GOAL_ID_APPL_GROUP_2 == idGroup) {
            /* register MI DM partition 1 (group 2) */
            res = goal_miDmPartIdxReg(GOAL_MI_MCTC_DIR_PEER_FROM, idGroup, &mMiDmRead3, sizeof(APPL_MSG_T), APPL_GROUP_2_OFFSET);
            if (GOAL_RES_ERR(res)) {
                goal_logErr("failed to register cyclic read part");
            }

            if (GOAL_RES_OK(res)) {
                res = goal_miDmPartIdxReg(GOAL_MI_MCTC_DIR_PEER_TO, idGroup, &mMiDmWrite3, sizeof(APPL_MSG_T), APPL_GROUP_2_OFFSET);
                if (GOAL_RES_ERR(res)) {
                    goal_logErr("failed to register cyclic write part");
                    return res;
                }
            }
        }
    } else {
        /* remove all group parts from DM */
        res = goal_miDmGroupPartsRemove(GOAL_MI_MCTC_DIR_PEER_TO, idGroup);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to remove all group parts from DM");
        } else {
            /* remove all group parts from DM */
            res = goal_miDmGroupPartsRemove(GOAL_MI_MCTC_DIR_PEER_FROM, idGroup);
            if (GOAL_RES_ERR(res)) {
                goal_logErr("failed to remove all group parts from DM");
            }
        }
    }
    return res;
}


/****************************************************************************/
/** Application Setup
 *
 * Setup the application by creating timers and DM groups. This is required
 * if partitions of a new group get registered after appl_setup.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T appl_setup(
    void
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MI_DM_GROUP_T *pGroup = NULL;          /* group handle */

    /* create offset between data of group 1 and 2 */
    msgData1.value = 0;
    msgData2.value = 0;
    msgData3.value = 1;

    /* set initial state to wait.. followed by registering */
    stageAppl = APPL_SM_WAIT_0;

    /* create timers */
    res = goal_timerCreate(&pTmrIo, GOAL_TIMER_LOW);
    if (GOAL_RES_OK(res)) {
        res = goal_timerCreate(&pTmrSm, GOAL_TIMER_LOW);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_timerSetup(pTmrIo,
                              GOAL_TIMER_PERIODIC,
                              APPL_RW_TIMEOUT,
                              appl_rw,
                              NULL,
                              GOAL_TRUE);
    }

    if (GOAL_RES_OK(res)) {
        res = goal_timerSetup(pTmrSm,
                              GOAL_TIMER_PERIODIC,
                              APPL_SM_TIMOUT,
                              appl_sm,
                              NULL,
                              GOAL_TRUE);
    }

    /* create application groups */
    res = goal_miDmGroupNew(&pGroup, GOAL_MI_MCTC_DIR_PEER_FROM, GOAL_ID_APPL_GROUP_1);
    if (GOAL_RES_OK(res)) {
        res = goal_miDmGroupNew(&pGroup, GOAL_MI_MCTC_DIR_PEER_TO, GOAL_ID_APPL_GROUP_1);
    }
    res = goal_miDmGroupNew(&pGroup, GOAL_MI_MCTC_DIR_PEER_FROM, GOAL_ID_APPL_GROUP_2);
    if (GOAL_RES_OK(res)) {
        res = goal_miDmGroupNew(&pGroup, GOAL_MI_MCTC_DIR_PEER_TO, GOAL_ID_APPL_GROUP_2);
    }

    /* register MI DM partition 3 (group 2 for data3) */
    if (GOAL_RES_OK(res)) {
        res = appl_cfgDmParts(GOAL_ID_APPL_GROUP_2, GOAL_TRUE);
    }

    return res;
}


/****************************************************************************/
/** Data Exchange
 *
 * Write data to DM and read new content afterwards.
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T appl_dataCheck(
    GOAL_MI_DM_PART_T *pR,                      /**< [in] data read partition */
    GOAL_MI_DM_PART_T *pW,                      /**< [in] data write partition */
    APPL_MSG_T *pData                           /**< [in] process data reference */
)
{
    GOAL_STATUS_T res;                          /* result */
    APPL_MSG_T data_be;                         /* process data in big endianess format */
    APPL_MSG_T rxData;                          /* received message data */

    /* cast data from host to big endianess */
    data_be.value = GOAL_htobe32(pData->value);

    /* write input partition data */
    res = goal_miDmSingleWrite(pW, (uint8_t *) &data_be, sizeof(APPL_MSG_T));

    /* read output partition data even if pW is in wrong state (aka. not active) */
    if (GOAL_RES_OK(res) || (GOAL_ERR_WRONG_STATE == res) || (GOAL_ERR_NULL_POINTER == res)) {
        res = goal_miDmSingleRead(pR, (uint8_t *) &data_be);
    }
    if (GOAL_RES_OK(res)) {
        rxData.value = GOAL_be32toh(data_be.value);

        /* log data */
        goal_logInfo("RX %"FMT_u32" [group: 0x%"FMT_x32" index: %"FMT_u32"]", rxData.value, pR->pGroup->id, pR->idxPart);
    }

    UNUSEDARG(rxData);
    return res;

}


/****************************************************************************/
/** Application read/ write data
 *
 * Increase data value and trigger data check.
 */
void appl_rw(
    void *pArg                                  /**< argument */
)
{
    GOAL_STATUS_T res;                          /* result */

    UNUSEDARG(pArg);

    /* check if MCTC is ready */
    res = goal_miMctcStatusGet(GOAL_ID_DEFAULT);
    if (GOAL_RES_ERR(res)) {
        return;
    }

    /* increase data by 2 to differ between group1 and group2 */
    msgData1.value += 2;
    res = appl_dataCheck(&mMiDmRead1, &mMiDmWrite1, &msgData1);
    if (GOAL_ERR_BUSY == res) {
        goal_logDbg("MCTC is busy.");
    } else if (GOAL_RES_ERR(res) && GOAL_ERR_WRONG_STATE != res && GOAL_ERR_NULL_POINTER != res) {
        goal_logErr("communication failed err: %"FMT_x32, res);
    }

    msgData2.value += 2;
    res = appl_dataCheck(&mMiDmRead2, &mMiDmWrite2, &msgData2);
    if (GOAL_ERR_BUSY == res) {
        goal_logDbg("MCTC is busy.");
    } else if (GOAL_RES_ERR(res) && GOAL_ERR_WRONG_STATE != res && GOAL_ERR_NULL_POINTER != res) {
        goal_logErr("communication failed err: %"FMT_x32, res);
    }

    msgData3.value += 2;
    res = appl_dataCheck(&mMiDmRead3, &mMiDmWrite3, &msgData3);
    if (GOAL_ERR_BUSY == res) {
        goal_logDbg("MCTC is busy.");
    } else if (GOAL_RES_ERR(res) && GOAL_ERR_WRONG_STATE != res && GOAL_ERR_NULL_POINTER != res) {
        goal_logErr("communication failed err: %"FMT_x32, res);
    }
}


/****************************************************************************/
/** Application State Machine
 *
 * State machine to unregister and reregister partitions.
 */
static void appl_sm(
    void *pArg                                  /**< argument */
)
{
    UNUSEDARG(pArg);

    switch (stageAppl) {
        case APPL_SM_WAIT_0:
            /* wait state - do nothing */

            /* set next state */
            stageAppl = APPL_SM_REG;
            break;

        case APPL_SM_REG:
            /* register MI DM partitions (group 1 for data1 and data2) */
            (void) appl_cfgDmParts(GOAL_ID_APPL_GROUP_1, GOAL_TRUE);

            /* set next state */
            stageAppl = APPL_SM_WAIT_1;
            break;

        case APPL_SM_WAIT_1:
            /* wait state - do nothing */

            /* set next state */
            stageAppl = APPL_SM_UNREG;
            break;

        case APPL_SM_UNREG:
            /* unregister group 1 */
            (void) appl_cfgDmParts(GOAL_ID_APPL_GROUP_1, GOAL_FALSE);

            /* set next state */
            stageAppl = APPL_SM_WAIT_0;
          break;
    }
}
