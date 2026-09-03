/** @file
 *
 * @brief Internal Buffer and Queue Management
 *
 * This module implements buffer handling mechanisms. All buffers are allocated
 * at start.
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

#ifndef GOAL_QUEUE_H
#define GOAL_QUEUE_H

#include "goal_includes.h"


/****************************************************************************/
/* Definitions */
/****************************************************************************/
#define GOAL_QUEUE_FLG_NONE                     0
#define GOAL_QUEUE_FLG_TX                       (1u << 0) /**< transmit flag */
#define GOAL_QUEUE_FLG_NO_RELEASE               (1u << 1) /**< no-release flag */
#define GOAL_QUEUE_FLG_VLAN                     (1u << 2) /**< VLAN flag */
#define GOAL_QUEUE_FLG_KEEP_CONTENT             (1u << 3) /**< keep content after sending */
#define GOAL_QUEUE_FLG_USED                     (1u << 4) /**< buffer currently in use */
#define GOAL_QUEUE_FLG_TIMESTAMP                (1u << 5) /**< generate Tx timestamp */
#define GOAL_QUEUE_FLG_VLAN_TCI                 (1u << 6) /**< VLAN TCI is appended to frame data */
#define GOAL_QUEUE_FLG_QUEUE                    (1u << 7) /**< force usage of netQueue */


/****************************************************************************/
/* Data types */
/****************************************************************************/
/** Queue / Ringbuffer Structure */
typedef struct {
    GOAL_LOCK_T         *pMutex;                /**< queue lock */
    uint16_t            cnt;                    /**< element count */
    uint8_t             flagEmpty;              /**< empty flag */
    struct GOAL_BUFFER_T *pBufList;             /**< list of free buffers */
} GOAL_QUEUE_T;


/** Buffer Release Callback */
typedef GOAL_STATUS_T (* GOAL_QUEUE_CB_REL_T)(struct GOAL_BUFFER_T *, void *);


/** Buffer / Unit Structure */
typedef struct GOAL_BUFFER_T {
    struct GOAL_BUFFER_T *pNext;                /**< next buffer */
    GOAL_LOCK_T *pMtx;                          /**< buffer mutex */

    uint16_t    bufSize;                        /**< buffer max size */
    uint16_t    dataLen;                        /**< real data length */
    uint32_t    flags;                          /**< flags */
    uint8_t     *ptrData;                       /**< data pointer */
    GOAL_QUEUE_T  *pOrigQueue;                  /**< originator queue */
    GOAL_QUEUE_T  *pQueue;                      /**< return queue pointer */
    GOAL_ID_T usage;                            /**< usage describer */
    uint32_t    netPort;                        /**< network port */
    uint32_t    netQueue;                       /**< queue within network port */
    uint16_t    etherType;                      /**< EtherType */
    struct GOAL_ETH_BUF_HANDLER_T *pEthBufHdlr; /**< Ethernet buffer handler */
    GOAL_QUEUE_CB_REL_T relCb;                  /**< release callback */
    void         *pArg;                         /**< release callback argument */
    uint64_t    tsSec;                          /**< rx timestamp: seconds */
    uint32_t    tsNsec;                         /**< rx timestamp: nanoseconds */
} GOAL_BUFFER_T;


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_queuePoolInitPre(
    void
);

GOAL_STATUS_T goal_queuePoolGetBuf(
    GOAL_BUFFER_T **ppBuf,                      /**< buffer ref ptr */
    uint16_t sizeMin,                           /**< minimal size of requested buffer */
    GOAL_ID_T usage                             /**< buffer usage */
);

GOAL_STATUS_T goal_queueInit(
    GOAL_QUEUE_T **ppQueue,                     /**< pointer to queue pointer */
    uint16_t numElemMax,                        /**< max number of elements */
    uint16_t numElem,                           /**< number of frames to be added to the pool */
    uint16_t bufSize                            /**< size of each buffer in the pool */
);

GOAL_STATUS_T goal_queueGetNewBuf(
    GOAL_BUFFER_T **ppBuf,                      /**< buffer ref */
    GOAL_QUEUE_T *pQueue,                       /**< queue pointer */
    GOAL_ID_T usage                             /**< buffer usage */
);

GOAL_STATUS_T goal_queueReleaseBuf(
    GOAL_BUFFER_T **ppBuf                       /**< buffer to release */
);

GOAL_STATUS_T goal_queueReleaseBufToOrigQueue(
    GOAL_BUFFER_T **ppBuf                       /**< buffer location pointer */
);

GOAL_STATUS_T goal_queueReleaseBufToNewQueue(
    GOAL_QUEUE_T *pQueue,                       /**< new queue */
    GOAL_BUFFER_T **ppBuf                       /**< buffer location pointer */
);

GOAL_STATUS_T goal_queueGetElem(
    GOAL_QUEUE_T *pQueue,                       /**< queue pointer */
    void **pMem                                 /**< returned pointer to memory */
);

GOAL_STATUS_T goal_queueAddElem(
    GOAL_QUEUE_T *pQueue,                       /**< queue pointer */
    GOAL_BUFFER_T *pElem                        /**< pointer to the element */
);

GOAL_STATUS_T goal_queuePrependElem(
    GOAL_QUEUE_T *pQueue,                       /**< queue pointer */
    GOAL_BUFFER_T *pElem                        /**< pointer to the element */
);

uint8_t goal_queueIsFull(
    GOAL_QUEUE_T *pQueue                        /**< queue pointer */
);

uint8_t goal_queueIsEmpty(
    GOAL_QUEUE_T *pQueue                        /**< queue pointer */
);

void goal_queueSetReleaseCallback(
    GOAL_BUFFER_T *pBuf,                        /**< buffer pointer */
    GOAL_QUEUE_CB_REL_T relCb,                  /**< release callback */
    void *pArg                                  /**< release callback argument */
);

void goal_queueFlagsClr(
    GOAL_BUFFER_T *pBuf,                        /**< buffer pointer */
    uint32_t flags                              /**< buffer flags */
);

GOAL_BOOL_T goal_queueFlagsGet(
    GOAL_BUFFER_T *pBuf,                        /**< buffer pointer */
    uint32_t flags                              /**< flags */
);

void goal_queueFlagsSet(
    GOAL_BUFFER_T *pBuf,                        /**< buffer pointer */
    uint32_t flags                              /**< buffer flags */
);

GOAL_STATUS_T goal_queuePoolBufsReq(
    GOAL_ID_T usageId,                          /**< goal id of user */
    uint16_t bufSize,                           /**< requested buffer size */
    uint16_t numBufFixed,                       /**< required number of buffers */
    uint16_t numBufTemp                         /**< at times required amount of buffers */
);

#endif /* GOAL_QUEUE_H */
