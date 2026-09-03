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

#define GOAL_ID GOAL_ID_MI_MCTC
#include <goal_includes.h>
#include <goal_media/goal_mi_mctc.h>
#include <goal_media/goal_mi_mctc_ram.h>


/****************************************************************************/
/* Defines */
/****************************************************************************/
#define GOAL_MI_MCTC_RAM_ALIGN 4                /**< RAM alignment */
#define GOAL_MI_MCTC_DATA_NEW 1                 /**< new data flag */
#define GOAL_MI_MCTC_DATA_OLD 0                 /**< old data flag */


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static GOAL_TIMER_T* pTimerMiMctcPoll = NULL;   /**< timer for polling */
static uint32_t toutMedia = GOAL_MI_MCTC_RAM_TIMEOUT; /**< default media timeout */


/****************************************************************************/
/* Local prototypes */
/****************************************************************************/
static GOAL_STATUS_T goal_miMctcRamOpen(
    GOAL_MI_MCTC_INST_T *pInst,                 /**< MCTC instance */
    unsigned int sizeXfer                       /**< transfer size */
);

static GOAL_STATUS_T goal_miMctcDmCbWrite(
    GOAL_MI_DM_GROUP_T *pGroup,                 /**< [in] MI DM group handle */
    void *pPriv                                 /**< [in] private pointer */
);

static GOAL_STATUS_T goal_miMctcRamInfoSetup(
    GOAL_MI_MCTC_RAM_T *pMiMctcRam              /**< MI MCTC RAM */
);

static GOAL_STATUS_T goal_miMctcRamSwap(
    GOAL_BOOL_T flgWr,                          /**< swap write flag */
    GOAL_MI_MCTC_RAM_T *pMiMctcRam              /**< MI MCTC RAM */
);

static void goal_miMctcRamPoll(
    void *pArg                                  /**< argument */
);

static void goal_miMctcRamLoop(
    void
);

static GOAL_STATUS_T goal_miMctcRamNotifyCb(
    struct GOAL_MI_MCTC_INST_T *pInst,          /**< MCTC instance */
    uint8_t *pData,                             /**< notify data */
    uint16_t len                                /**< notify data length */
);


/****************************************************************************/
/** MCTC MI RAM - Registration
 *
 * Register a MCTC RAM instance in the global MCTC MI.
 *
 * @returns GOAL_STATUS_T result
 */
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
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MI_MCTC_INST_T *pMiMctc = NULL;        /* MI MCTC instance */
    GOAL_MI_MCTC_RAM_T *pMiMctcRam;             /* RAM data */

    /* register RAM in MI MCTC */
    res = goal_miMctcReg(&pMiMctc, idMctc, "MCTC RAM", goal_miMctcRamOpen, ((NULL == funcNotifyTx) ? (NULL) : (goal_miMctcRamNotifyCb)));
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to register MCTC RAM");
        return res;
    }

    /* allocate RAM data */
    res = goal_memCalloc(&pMiMctc->pPriv, sizeof(GOAL_MI_MCTC_RAM_T));
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to allocate MI MCTC RAM data");
        return res;
    }

    pMiMctcRam = pMiMctc->pPriv;

    /* create local lock */
    res = goal_lockCreate(GOAL_LOCK_BINARY, &pMiMctcRam->pLock, 0, 1, GOAL_ID_MI_MCTC);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed create lock");
        return res;
    }

    /* register MI DM Write sync callback */
    res = goal_miDmCbReg(pMiDmWrite, NULL, GOAL_MI_DM_CB_WRITE, goal_miMctcDmCbWrite, pMiMctcRam);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed register MI DM Write sync callback");
        return res;
    }

    /* assign RAM data */
    pMiMctcRam->status = GOAL_MI_MCTC_DPRAM_CLOSED;
    pMiMctcRam->pShm = pShm;
    pMiMctcRam->lenShm = len;
    pMiMctcRam->flgAlign = flgAlign;
    pMiMctcRam->funcNotifyTx = funcNotifyTx;
    pMiMctcRam->funcLock = funcLock;
    pMiMctcRam->funcUnlock = funcUnlock;
    pMiMctcRam->pMiDmRead = pMiDmRead;
    pMiMctcRam->pMiDmWrite = pMiDmWrite;

    /* unique: create timer for polling */
    if ((NULL == pMiMctcRam->funcNotifyTx) && (NULL == pTimerMiMctcPoll)) {
        res = goal_timerCreate(&pTimerMiMctcPoll, GOAL_TIMER_LOW);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("Unable to create timer for polling MI MCTC RAM");
            return res;
        }

    }

    /* set default configured timeout */
    goal_logInfo("setting media timeout to %"FMT_u32":", toutMedia);
    pMiMctc->cfgToutValue = toutMedia;

    return res;
}


/****************************************************************************/
/** MCTC MI RAM - Open
 *
 * Use RAM for CTC.
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_miMctcRamOpen(
    GOAL_MI_MCTC_INST_T *pInst,                 /**< MCTC instance */
    unsigned int sizeXfer                       /**< transfer size */
)
{
    unsigned int cnt;                           /* counter */
    GOAL_MI_MCTC_RAM_T *pMiMctcRam;             /* MI MCTC RAM */
    GOAL_STATUS_T res = GOAL_OK;                /* result */

    /* set transfer size */
    pMiMctcRam = (GOAL_MI_MCTC_RAM_T *) pInst->pPriv;

    if (GOAL_MI_MCTC_DPRAM_CLOSED != pMiMctcRam->status) {
        goal_logErr("MI DPRAM handle is already in use");
        return GOAL_ERR_ALREADY_USED;
    }

    /* check the buffer size */
    if ((2 * sizeof(GOAL_MI_MCTC_RAM_INFO_T)) + (3 * (pMiMctcRam->pMiDmRead->len + pMiMctcRam->pMiDmWrite->len)) > pMiMctcRam->lenShm) {
        goal_logErr("%u bytes transfer size doesn't fit into RAM", sizeXfer);
        return GOAL_ERR_PARAM;
    }


    /* allocate RAM buffer to handle aligned SHM access */
    if (pMiMctcRam->flgAlign) {
        res = goal_memCalloc(&pMiMctcRam->pBufRamWr, (sizeof(GOAL_MI_MCTC_DPRAM_CYCLIC_T) + pMiMctcRam->pMiDmWrite->len));
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to allocate aligned RAM buffer for writing");
            return res;
        }

        res = goal_memCalloc(&pMiMctcRam->pBufRamRd, (sizeof(GOAL_MI_MCTC_DPRAM_CYCLIC_T) + pMiMctcRam->pMiDmRead->len));
        if (GOAL_RES_ERR(res)) {
            goal_logErr("failed to allocate aligned RAM buffer for reading");
            return res;
        }
    }

    pMiMctcRam->size = sizeXfer;

#if (1 == GOAL_CONFIG_GEN_CTC_CC)
    /* split the RAM into read and write section */
    pMiMctcRam->pShmWr = (GOAL_MI_MCTC_RAM_INFO_T *) pMiMctcRam->pShm;
    pMiMctcRam->pShmRd = (GOAL_MI_MCTC_RAM_INFO_T *) ((pMiMctcRam->pShm) + (pMiMctcRam->lenShm / 2));
    pMiMctcRam->pShmRd = GOAL_alignPtr(GOAL_MI_MCTC_RAM_ALIGN, pMiMctcRam->pShmRd);
#else
    /* split the RAM into read and write section */
    pMiMctcRam->pShmWr = (GOAL_MI_MCTC_RAM_INFO_T *) (pMiMctcRam->pShm + (pMiMctcRam->lenShm / 2));
    pMiMctcRam->pShmWr = GOAL_alignPtr(GOAL_MI_MCTC_RAM_ALIGN, pMiMctcRam->pShmWr);
    pMiMctcRam->pShmRd = (GOAL_MI_MCTC_RAM_INFO_T *) pMiMctcRam->pShm;
#endif

    /* clear the write section */
    GOAL_MEMSET(pMiMctcRam->pShmWr, 0, pMiMctcRam->lenShm / 2);

    /* prepare the triple buffer */
    for (cnt = 0; cnt < 3; cnt++) {
        pMiMctcRam->pBufRd[cnt] = GOAL_alignPtr(GOAL_MI_MCTC_RAM_ALIGN, (((uint8_t *) (pMiMctcRam->pShmRd + 1)) + cnt * (sizeof(GOAL_MI_MCTC_DPRAM_CYCLIC_T) + pMiMctcRam->pMiDmRead->len)));
        pMiMctcRam->pBufWr[cnt] = GOAL_alignPtr(GOAL_MI_MCTC_RAM_ALIGN, (((uint8_t *) (pMiMctcRam->pShmWr + 1)) + cnt * (sizeof(GOAL_MI_MCTC_DPRAM_CYCLIC_T) + pMiMctcRam->pMiDmWrite->len)));
    }

    /* poll the handle cyclic if necessary */
    if (NULL != pTimerMiMctcPoll) {
        res = goal_timerSetup(pTimerMiMctcPoll,
                              GOAL_TIMER_PERIODIC,
                              GOAL_MI_MCTC_DPRAM_POLLING,
                              goal_miMctcRamPoll,
                              pMiMctcRam,
                              GOAL_TRUE);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("Unable to setup MCTC RAM polling timer");
            return res;
        }
    }

    /* register RAM main loop */
    res = goal_mainLoopReg(goal_miMctcRamLoop);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to register MCTC RAM loop");
        return res;
    }

    if (GOAL_RES_OK(res)) {
        /* mark the handle as opened */
        pMiMctcRam->status = GOAL_MI_MCTC_DPRAM_OPEN;
    }
    return res;
}


/****************************************************************************/
/** MCTC MI - DM Partition Write Update Callback
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_miMctcDmCbWrite(
    GOAL_MI_DM_GROUP_T *pGroup,                 /**< [in] MI DM group handle */
    void *pPriv                                 /**< [in] private pointer */
)
{
    GOAL_STATUS_T res;                          /* result */
    uint8_t *pBuf;                              /* buffer reference */
    GOAL_MI_MCTC_RAM_T *pMiMctcRam;             /* RAM data */
    GOAL_MI_MCTC_DPRAM_CYCLIC_T *pHead;         /* header of cyclic data */

    UNUSEDARG(pGroup);

    /* assign instance */
    pMiMctcRam = (GOAL_MI_MCTC_RAM_T *) pPriv;

    /* check if the handle has been opened and setup */
    switch (pMiMctcRam->status) {
        case GOAL_MI_MCTC_DPRAM_READY:
            /* all okay */
            break;

        case GOAL_MI_MCTC_DPRAM_OPEN:
            /* try to setup the write section */
            res = goal_miMctcRamInfoSetup(pMiMctcRam);
            if (GOAL_RES_ERR(res)) {
                return res;
            }
            break;

        case GOAL_MI_MCTC_DPRAM_CB:
            pMiMctcRam->flgWr = GOAL_TRUE;
            return GOAL_OK;

        case GOAL_MI_MCTC_DPRAM_CLOSED:
            /* intended fallthrough */
            GOAL_TARGET_FALLTHROUGH;
        default:
          return GOAL_ERR_WRONG_STATE;
    }

    /* assign the header and buffer reference */
    if (pMiMctcRam->flgAlign) {
        /* use RAM buffer when SHM alignment is necessary */
        pHead = (GOAL_MI_MCTC_DPRAM_CYCLIC_T *) pMiMctcRam->pBufRamWr;
        pBuf = pMiMctcRam->pBufRamWr;
    } else {
        /* use SHM if alignment is not necessary */
        pHead = (GOAL_MI_MCTC_DPRAM_CYCLIC_T *) pMiMctcRam->pBufWr[pMiMctcRam->pShmWr->idxWr];
        pBuf = pMiMctcRam->pBufWr[pMiMctcRam->pShmWr->idxWr];
    }

    /* set the new data flag */
    pHead->flgNew = GOAL_htobe32(GOAL_MI_MCTC_DATA_NEW);

    /* synchronize MI DM to output buffer */
    res = goal_miDmWriteSync(pMiMctcRam->pMiDmWrite, pBuf + sizeof(GOAL_MI_MCTC_DPRAM_CYCLIC_T));
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* copy the prepared message from RAM to SHM */
    if (pMiMctcRam->flgAlign) {
        GOAL_MEMCPY(pMiMctcRam->pBufWr[pMiMctcRam->pShmWr->idxWr], pBuf, (sizeof(GOAL_MI_MCTC_DPRAM_CYCLIC_T) + pMiMctcRam->pMiDmWrite->len));
    }

    /* swap the write buffer on the write side */
    res = goal_miMctcRamSwap(GOAL_TRUE, pMiMctcRam);
    if (GOAL_RES_OK(res)) {
        /* send a notification signal - content doesn't matter */
        if (pMiMctcRam->funcNotifyTx) {
            res = pMiMctcRam->funcNotifyTx(pMiMctcRam, (uint8_t *) &res, sizeof(res));
        }
    }

    if (GOAL_RES_OK(res)) {
        /* re-arm heart beat timer */
        pMiMctcRam->tsHeartBeat = goal_timerTsGet() + GOAL_MI_MCTC_RAM_HEARTBEAT_MS;
    }
    return res;
}


/****************************************************************************/
/** Setup the info map
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_miMctcRamInfoSetup(
    GOAL_MI_MCTC_RAM_T *pMiMctcRam              /**< MI MCTC RAM */
)
{
    volatile GOAL_MI_MCTC_RAM_INFO_T *pInfo;    /* info structure for swapping */
    GOAL_STATUS_T res;                          /* result */

    pInfo = pMiMctcRam->pShmWr;

    /* lock the write struct */
    res = pMiMctcRam->funcLock(GOAL_TRUE);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    pInfo->idxRd = 0;
    pInfo->idxWr = 1;
    pInfo->idxSw = 2;

    /* unlock the write struct */
    res = pMiMctcRam->funcUnlock(GOAL_TRUE);
    if (GOAL_RES_OK(res)) {
        pMiMctcRam->status = GOAL_MI_MCTC_DPRAM_READY;
    }
    return res;
}


/****************************************************************************/
/** Swap partitions
 *
 * Based on flgWr, the read or write partition is swapped
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_miMctcRamSwap(
    GOAL_BOOL_T flgWr,                          /**< swap write flag */
    GOAL_MI_MCTC_RAM_T *pMiMctcRam              /**< MI MCTC RAM */
)
{
    volatile GOAL_MI_MCTC_RAM_INFO_T *pInfo;    /* info structure for swapping */
    uint8_t idx;                                /* index for swapping */
    GOAL_STATUS_T res;                          /* result */

    res = goal_lockGet(pMiMctcRam->pLock, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* lock the struct */
    res = pMiMctcRam->funcLock(flgWr);
    if (GOAL_RES_ERR(res)) {
        goal_lockPut(pMiMctcRam->pLock);
        return res;
    }

    /* swap the partitions */
    if (GOAL_TRUE == flgWr) {
        pInfo = pMiMctcRam->pShmWr;
        idx = pInfo->idxWr;
        pInfo->idxWr = pInfo->idxSw;
        pInfo->idxSw = idx;
    } else {
        pInfo = pMiMctcRam->pShmRd;
        idx = pInfo->idxRd;
        pInfo->idxRd = pInfo->idxSw;
        pInfo->idxSw = idx;
    }

    /* unlock the struct */
    res = pMiMctcRam->funcUnlock(flgWr);

    /* unlock the local lock */
    goal_lockPut(pMiMctcRam->pLock);
    return res;
}


/****************************************************************************/
/** Timer callback for polling
 *
 */
static void goal_miMctcRamPoll(
    void *pArg                                  /**< argument */
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_MI_MCTC_RAM_T *pMiMctcRam;             /* MI MCTC RAM */
    GOAL_MI_MCTC_DPRAM_CYCLIC_T *pHead;         /* header of cyclic data */
    GOAL_MI_MCTC_INST_T *pMiMctc;               /* MCTC instance */

    pMiMctcRam = (GOAL_MI_MCTC_RAM_T *) pArg;

    /* check if the handle has been opened and setup */
    switch (pMiMctcRam->status) {
        case GOAL_MI_MCTC_DPRAM_READY:
            /* all okay */
            break;

        case GOAL_MI_MCTC_DPRAM_OPEN:
            /* try to setup the write section */
            res = goal_miMctcRamInfoSetup(pMiMctcRam);
            if (GOAL_RES_ERR(res)) {
                return;
            }
            break;

        case GOAL_MI_MCTC_DPRAM_CLOSED:
            /* intended fallthrough */
            GOAL_TARGET_FALLTHROUGH;
        default:
          return;
    }

    res = goal_miMctcRamSwap(GOAL_FALSE, pMiMctcRam);
    if (GOAL_RES_OK(res)) {

        pHead = (GOAL_MI_MCTC_DPRAM_CYCLIC_T *) pMiMctcRam->pBufRd[pMiMctcRam->pShmRd->idxRd];
        if (GOAL_MI_MCTC_DATA_NEW != GOAL_be32toh(pHead->flgNew)) {
            return;
        }

        /* get default MI MCTC instance */
        res = goal_instGetById((GOAL_INSTANCE_T **) &pMiMctc, GOAL_ID_MI_MCTC, GOAL_ID_DEFAULT);
        if (GOAL_RES_ERR(res)) {
            /* TODO: notify application */
            goal_targetHalt();
        }

        /* inform MI MCTC that connection is running */
        goal_miMctcMonitorRx(pMiMctc, GOAL_TRUE);

        /* re-arm watchdog */
        pMiMctcRam->tsTout = goal_timerTsGet() + pMiMctc->cfgToutValue;

        pMiMctcRam->status = GOAL_MI_MCTC_DPRAM_CB;

        /* copy the message to RAM when SHM aligned is necessary */
        if (pMiMctcRam->flgAlign) {
            GOAL_MEMCPY(pMiMctcRam->pBufRamRd, pMiMctcRam->pBufRd[pMiMctcRam->pShmRd->idxRd], (sizeof(GOAL_MI_MCTC_DPRAM_CYCLIC_T) + pMiMctcRam->pMiDmWrite->len));

            res = goal_miDmReadSync(pMiMctcRam->pMiDmRead, pMiMctcRam->pBufRamRd + sizeof(GOAL_MI_MCTC_DPRAM_CYCLIC_T));
        } else {
            res = goal_miDmReadSync(pMiMctcRam->pMiDmRead, pMiMctcRam->pBufRd[pMiMctcRam->pShmRd->idxRd] + sizeof(GOAL_MI_MCTC_DPRAM_CYCLIC_T));
        }

        if (GOAL_RES_OK(res)) {
            /* mark the data as read */
            pHead->flgNew = GOAL_htobe32(GOAL_MI_MCTC_DATA_OLD);
        }

        pMiMctcRam->status = GOAL_MI_MCTC_DPRAM_READY;

        if (GOAL_TRUE == pMiMctcRam->flgWr) {
            pMiMctcRam->flgWr = GOAL_FALSE;
            res = goal_miMctcDmCbWrite(NULL, pMiMctcRam);
        }
    }
}


/****************************************************************************/
/** MCTC MI RAM - loop for detection of timeout
 */
static void goal_miMctcRamLoop(
    void
)
{
    GOAL_MI_MCTC_RAM_T *pMiMctcRam;             /* MCTC RAM data */
    GOAL_MI_MCTC_INST_T *pMiMctc;               /* MCTC instance */
    GOAL_STATUS_T res;                          /* result */
    GOAL_TIMESTAMP_T tsNow;                     /* current timestamp */

    /* get default MI MCTC instance */
    res = goal_instGetById((GOAL_INSTANCE_T **) &pMiMctc, GOAL_ID_MI_MCTC, GOAL_ID_DEFAULT);
    if (GOAL_RES_ERR(res)) {
        /* TODO: notify application */
        goal_targetHalt();
    }

    /* assign MI MCTC RAM instance */
    pMiMctcRam = pMiMctc->pPriv;

    /* reset RAM if connection watchdog expired */
    tsNow = goal_timerTsGet();
    if ((0 != pMiMctc->cfgToutValue) && (pMiMctcRam->tsTout <= tsNow)) {
        pMiMctcRam->tsTout = tsNow + pMiMctc->cfgToutValue;
        /* inform MI MCTC */
        goal_miMctcMonitorRx(pMiMctc, GOAL_FALSE);

        /* TODO: convert to GOAL statistic */
        /* pMiMctc->cntResets++; */
    }

    /* sending heart beat if no data has been transmitted within the last GOAL_MI_MCTC_RAM_HEARTBEAT_MS ms */
    if (pMiMctcRam->tsHeartBeat <= tsNow) {
        /* trigger heart beat by rewriting latest data */
        res = goal_miMctcDmCbWrite(NULL, pMiMctcRam);
        if (GOAL_RES_ERR(res)) {
            /* re-arm heart beat rate to prevent write attempt every loop */
            pMiMctcRam->tsHeartBeat = tsNow + GOAL_MI_MCTC_RAM_HEARTBEAT_MS;
        }
    }
}


/****************************************************************************/
/** Notify callback
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_miMctcRamNotifyCb(
    struct GOAL_MI_MCTC_INST_T *pInst,          /**< MCTC instance */
    uint8_t *pData,                             /**< notify data */
    uint16_t len                                /**< notify data length */
)
{
    UNUSEDARG(pData);
    UNUSEDARG(len);

    if ((NULL == pInst) || (NULL == pInst->pPriv)) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* poll the new data */
    goal_miMctcRamPoll(pInst->pPriv);
    return GOAL_OK;
}


/****************************************************************************/
/** MCTC MI RAM - Configure initial timeout values
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_miMctcRamCfgTout(
    uint32_t toutMediaValue                     /**< initial rpc timeout */
)
{
    GOAL_STATUS_T res;                          /* result */

    toutMedia = toutMediaValue;

    res = GOAL_OK;

    return res;
}
