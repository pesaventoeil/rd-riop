/** @file
 *
 *
 * @brief GOAL MCTC - RAM Media Interface
 *
 * @details
 * This module adapts MCTC to RAM.
 *
 * @copyright
 * Copyright 2010-2018.
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

#ifndef GOAL_MI_MCTC_RAM_H
#define GOAL_MI_MCTC_RAM_H

#include <goal_media/goal_mi_mctc.h>
#include <goal_media/goal_mi_dm.h>


/****************************************************************************/
/* Defines */
/****************************************************************************/
#define GOAL_MI_MCTC_DPRAM_POLLING GOAL_TIMER_MSEC /**< periodicity of polling */
#define GOAL_MI_MCTC_RAM_TIMEOUT (1 * GOAL_TIMER_SEC) /**< RAM timeout */
#define GOAL_MI_MCTC_RAM_HEARTBEAT_MS (200 * GOAL_TIMER_MSEC) /**< heartbeat periodicity in ms */


/****************************************************************************/
/* Typedefs */
/****************************************************************************/
typedef GOAL_STATUS_T (*GOAL_MI_MCTC_FUNC_LOCK_T)(
    GOAL_BOOL_T flgWr                           /**< lock write flag */
);

typedef GOAL_STATUS_T (*GOAL_MI_MCTC_FUNC_UNLOCK_T)(
    GOAL_BOOL_T flgWr                           /**< unlock write flag */
);

typedef GOAL_STATUS_T (*GOAL_MI_MCTC_FUNC_NOTIFY_TX_T)(
    void *pArg,                                 /**< argument */
    uint8_t *pData,                             /**< notify data */
    uint16_t len                                /**< notify data length */
);


/****************************************************************************/
/* Data types */
/****************************************************************************/
typedef enum {
    GOAL_MI_MCTC_DPRAM_CLOSED = 0,              /**< handle is closed (unused) */
    GOAL_MI_MCTC_DPRAM_OPEN,                    /**< handle is opened */
    GOAL_MI_MCTC_DPRAM_READY,                   /**< handle is ready (setup done) */
    GOAL_MI_MCTC_DPRAM_CB,                      /**< callback is active */
} GOAL_MI_MCTC_DPRAM_STATUS;

/**< DPRAM cyclic data header */
GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct {
    uint32_t flgNew;                            /**< new data flag */
} GOAL_TARGET_PACKED GOAL_MI_MCTC_DPRAM_CYCLIC_T;
GOAL_TARGET_PACKED_STRUCT_POST

/**< management part of the memory map */
GOAL_TARGET_PACKED_STRUCT_PRE
typedef GOAL_TARGET_PACKED_PRE struct {
    uint8_t idxRd;                              /**< read index */
    uint8_t idxWr;                              /**< write index */
    uint8_t idxSw;                              /**< swap index */
}  GOAL_TARGET_PACKED GOAL_MI_MCTC_RAM_INFO_T;
GOAL_TARGET_PACKED_STRUCT_POST

/**< RAM data */
typedef struct GOAL_MI_MCTC_RAM_T {
    GOAL_MI_MCTC_DPRAM_STATUS status;           /**< handle status */

    unsigned int size;                          /**< transfer size */
    uint8_t *pShm;                              /**< SHM reference */
    unsigned int lenShm;                        /**< SHM length */

    GOAL_MI_MCTC_RAM_INFO_T *pShmRd;            /**< management part of read section */
    GOAL_MI_MCTC_RAM_INFO_T *pShmWr;            /**< management part of write section */

    uint8_t *pBufRd[3];                         /**< tripple reading buffer */
    uint8_t *pBufWr[3];                         /**< tripple writing buffer */

    GOAL_BOOL_T flgAlign;                       /**< aligned access */
    GOAL_BOOL_T flgWr;                          /**< initiate writing after callback */
    uint8_t *pBufRamRd;                         /**< buffer for aligned reading */
    uint8_t *pBufRamWr;                         /**< buffer for aligned writing */

    GOAL_MI_DM_T *pMiDmRead;                    /**< MI DM read handle */
    GOAL_MI_DM_T *pMiDmWrite;                   /**< MI DM write handle */

    GOAL_MI_MCTC_FUNC_NOTIFY_TX_T funcNotifyTx; /**< notify send function */
    GOAL_MI_MCTC_FUNC_LOCK_T funcLock;          /**< lock function */
    GOAL_MI_MCTC_FUNC_UNLOCK_T funcUnlock;      /**< unlock function */

    GOAL_LOCK_T *pLock;                         /**< local lock */
    GOAL_TIMESTAMP_T tsTout;                    /**< timeout time stamp */
    GOAL_TIMESTAMP_T tsHeartBeat;               /**< heart beat time stamp */
} GOAL_MI_MCTC_RAM_T;


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_miMctcRamReg(
    unsigned int idMctc,                        /**< MCTC instance id */
    uint8_t *pShm,                              /**< SHM */
    unsigned int len,                           /**< length of SHM */
    GOAL_BOOL_T flgAlign,                       /**< aligned SHM access */
    GOAL_MI_MCTC_FUNC_NOTIFY_TX_T funcNotifyTx, /**< notify send function */
    GOAL_MI_MCTC_FUNC_LOCK_T funcLock,          /**< lock function */
    GOAL_MI_MCTC_FUNC_UNLOCK_T funcUnlock,      /**< unlock function */
    GOAL_MI_DM_T *pMiDmRead,                    /**< MI DM read handle */
    GOAL_MI_DM_T *pMiDmWrite                    /**< MI DM write handle */
);

GOAL_STATUS_T goal_miMctcRamCfgTout(
    uint32_t toutMediaValue                     /**< initial rpc timeout */
);

#endif /* GOAL_MI_MCTC_RAM_H */
