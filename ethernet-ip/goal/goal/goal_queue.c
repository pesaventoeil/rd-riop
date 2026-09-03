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

#define GOAL_ID GOAL_ID_QUEUE
#include <goal_includes.h>
#include "cm/goal_queue_cm.h"


/****************************************************************************/
/* Local Defines */
/****************************************************************************/
/* number of queues */
#define GOAL_QUEUE_POOL_NUM            3

/* queue id */
#define GOAL_QUEUE_POOL_SMALL          0
#define GOAL_QUEUE_POOL_MED            1
#define GOAL_QUEUE_POOL_BIG            2

/* default defines for size and buffer number */
#ifndef GOAL_QUEUE_SMALL_SIZE
#   define GOAL_QUEUE_SMALL_SIZE       128
#endif
#ifndef GOAL_QUEUE_SMALL_NUM
#   define GOAL_QUEUE_SMALL_NUM        0
#endif
#ifndef GOAL_QUEUE_MED_SIZE
#   define GOAL_QUEUE_MED_SIZE         512
#endif
#ifndef GOAL_QUEUE_MED_NUM
#   define GOAL_QUEUE_MED_NUM          0
#endif
#ifndef GOAL_QUEUE_BIG_SIZE
#   define GOAL_QUEUE_BIG_SIZE         GOAL_ETH_MTU_SIZE_DFL
#endif
#ifndef GOAL_QUEUE_BIG_NUM
#   define GOAL_QUEUE_BIG_NUM          0
#endif


typedef struct GOAL_QUEUE_POOL_DESC_T {
    uint8_t pool;                               /**< pool id */
    uint16_t cntBuf;                            /**< number of buffers */
    uint16_t sizeBuf;                           /**< size of buffers */
    uint16_t reqBufCntFixed;                    /**< fixed requested buffers */
    uint16_t reqBufCntTemp;                     /**< temporarily requested buffers */
#if GOAL_CONFIG_DEBUG_QUEUE_USAGE == 1
    uint16_t minBuffersFree;                    /**< minimal number of free buffers at runtime */
    uint16_t buffersFree;                       /**< number of free buffers at runtime */
#endif /* GOAL_CONFIG_DEBUG_QUEUE_USAGE */
} GOAL_QUEUE_POOL_DESC_T;


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
static GOAL_STATUS_T goal_queueReleaseBufInternal(
    GOAL_BUFFER_T **ppBuf                       /**< buffer to release */
);

static GOAL_STATUS_T goal_queuePoolInit(
    void
);

static GOAL_STATUS_T goal_queueCmVarsInit(
    void
);

static GOAL_STATUS_T goal_queuePoolBufferAdd(
    GOAL_QUEUE_T *pQueue,                       /**< pool queue */
    uint16_t bufSize,                           /**< buffer size */
    uint16_t numElem                            /**< number of eleements to add */
);

#if GOAL_CONFIG_DEBUG_QUEUE_USAGE == 1
static void goal_queueLoop(
    void
);
#endif /* GOAL_CONFIG_DEBUG_QUEUE_USAGE */


/****************************************************************************/
/* local variables */
/****************************************************************************/
static GOAL_QUEUE_T *pPoolQueues[GOAL_QUEUE_POOL_NUM]; /**< buffer pol queues */
static GOAL_STAGE_HANDLER_T stageInit;          /**< init stage handler */
static GOAL_CM_VAR_T *pVarQueueSize[GOAL_QUEUE_POOL_NUM]; /**< buffer pool queue sizes */
static GOAL_CM_VAR_T *pVarQueueCnt[GOAL_QUEUE_POOL_NUM]; /**< buffer pool queue element number */

static GOAL_QUEUE_POOL_DESC_T poolDesc[GOAL_QUEUE_POOL_NUM] = {
    {
        .pool = GOAL_QUEUE_POOL_SMALL,
        .cntBuf = GOAL_QUEUE_SMALL_NUM,
        .sizeBuf = GOAL_QUEUE_SMALL_SIZE
    },
    {
        .pool = GOAL_QUEUE_POOL_MED,
        .cntBuf = GOAL_QUEUE_MED_NUM,
        .sizeBuf = GOAL_QUEUE_MED_SIZE
    },
    {
        .pool = GOAL_QUEUE_POOL_BIG,
        .cntBuf = GOAL_QUEUE_BIG_NUM,
        .sizeBuf = GOAL_QUEUE_BIG_SIZE
    }
};


/****************************************************************************/
/** Register stage handler
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_queuePoolInitPre(
    void
)
{
    GOAL_STATUS_T res;                          /* result */

    res = goal_queueRegCmVars();
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:221: */
        goal_lmLog(GOAL_ID_MEM, 221, 176, 0, 0, GOAL_LOG_SEV_ERROR, "Failed to register CM variables.");
        return res;
    }

    /* init stage */
    res = goal_mainStageReg(GOAL_STAGE_QUEUE_PRE, &stageInit, GOAL_STAGE_INIT, goal_queuePoolInit);

    return res;
}


#if GOAL_CONFIG_DEBUG_QUEUE_USAGE == 1
/****************************************************************************/
/** Memory pool queues loop
 *
 * This function checks the queue usage
 *
 * @returns GOAL_STATUS_T result
 */
static void goal_queueLoop(
    void
)
{
    static GOAL_TIMESTAMP_T tsTout;             /* timestamp */
    GOAL_TIMESTAMP_T tsNow;                     /* current timestamp */
    uint8_t poolId;                             /* pool number */

    tsNow = goal_timerTsGet();

    if (tsNow > tsTout) {
        for (poolId = 0; poolId < GOAL_QUEUE_POOL_NUM; poolId++) {
            goal_logInfo("pool %d :", poolId);
            goal_logInfo("  cntBuf = %d", poolDesc[poolId].cntBuf);
            goal_logInfo("  sizeBuf = %d", poolDesc[poolId].sizeBuf);
            goal_logInfo("  reqBufCntFixed = %d", poolDesc[poolId].reqBufCntFixed);
            goal_logInfo("  reqBufCntTemp = %d", poolDesc[poolId].reqBufCntTemp);
            goal_logInfo("  minBuffersFree = %d", poolDesc[poolId].minBuffersFree);
            goal_logInfo("  buffersFree = %d", poolDesc[poolId].buffersFree);
        }

        tsTout = tsNow + (5 * GOAL_TIMER_SEC);
    }
}
#endif /* GOAL_CONFIG_DEBUG_QUEUE_USAGE */


/****************************************************************************/
/** Memory pool queues init
 *
 * This function initializes queues with buffers of different sizes. These
 * buffers can be used by any GOAL module.
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_queuePoolInit(
    void
)
{
    uint32_t cnt;                               /* counter */
    uint16_t size = 0;                          /* buffer size of current queue */
    uint16_t num = 0;                           /* buffer number of current queue */
    GOAL_STATUS_T res;                          /* result */
    unsigned int sizeMtu;                       /* MTU size */

    /* init cm variables */
    res = goal_queueCmVarsInit();
    if (GOAL_RES_ERR(res)) {
        /* GG_LOG ID:222: */
        goal_lmLog(GOAL_ID_MEM, 222, 177, 0, 0, GOAL_LOG_SEV_ERROR, "Could not initialize CM variables.");
        return res;
    }

    /* update GOAL big queue element size to at least MTU size */
    sizeMtu = GOAL_NETBUF_SIZE;
    if (poolDesc[GOAL_QUEUE_POOL_BIG].sizeBuf < sizeMtu) {
        poolDesc[GOAL_QUEUE_POOL_BIG].sizeBuf = (uint16_t) sizeMtu;
    }

    /* init memory buffer pool queues */
    for (cnt = 0; GOAL_QUEUE_POOL_NUM > cnt; cnt++) {
        GOAL_CM_GETVAR_UINT16(pVarQueueSize[cnt], size);
        GOAL_CM_GETVAR_UINT16(pVarQueueCnt[cnt], num);

        res = goal_queueInit(&pPoolQueues[cnt], num, num, size);
        if (GOAL_RES_ERR(res)) {
            /* GG_LOG ID:223: */
            goal_lmLog(GOAL_ID_MEM, 223, 178, 4, 0, GOAL_LOG_SEV_ERROR, "Could not initialize queue no $1");
            goal_lmLogParamUINT32(cnt);
            goal_lmLogFinish();
            return res;
        }
    }

#if GOAL_CONFIG_DEBUG_QUEUE_USAGE == 1
    if (GOAL_RES_OK(res)) {
        res = goal_mainLoopReg(goal_queueLoop);
    }
#endif /* GOAL_CONFIG_DEBUG_QUEUE_USAGE */

    return res;
}


/****************************************************************************/
/** Initializes the CM vars of the goal Queue module
 *
 * @returns GOAL_STATUS_T value
 */
static GOAL_STATUS_T goal_queueCmVarsInit(
    void
)
{
    /* get var pointers */
    goal_cmGetVarById(GOAL_CM_QUEUE_MOD_ID, QUEUE_CM_VAR_SMALLBUFSIZE, &pVarQueueSize[GOAL_QUEUE_POOL_SMALL]);
    goal_cmGetVarById(GOAL_CM_QUEUE_MOD_ID, QUEUE_CM_VAR_MEDBUFSIZE, &pVarQueueSize[GOAL_QUEUE_POOL_MED]);
    goal_cmGetVarById(GOAL_CM_QUEUE_MOD_ID, QUEUE_CM_VAR_BIGBUFSIZE, &pVarQueueSize[GOAL_QUEUE_POOL_BIG]);
    goal_cmGetVarById(GOAL_CM_QUEUE_MOD_ID, QUEUE_CM_VAR_SMALLBUFNUM, &pVarQueueCnt[GOAL_QUEUE_POOL_SMALL]);
    goal_cmGetVarById(GOAL_CM_QUEUE_MOD_ID, QUEUE_CM_VAR_MEDBUFNUM, &pVarQueueCnt[GOAL_QUEUE_POOL_MED]);
    goal_cmGetVarById(GOAL_CM_QUEUE_MOD_ID, QUEUE_CM_VAR_BIGBUFNUM, &pVarQueueCnt[GOAL_QUEUE_POOL_BIG]);

    /* check if default must be restored */
    if (GOAL_TRUE == goal_cmVarIsDefault(GOAL_ID)) {
        /* set default values */
        GOAL_CM_SETVAR_UINT16(pVarQueueSize[GOAL_QUEUE_POOL_SMALL], GOAL_QUEUE_SMALL_SIZE);
        GOAL_CM_SETVAR_UINT16(pVarQueueSize[GOAL_QUEUE_POOL_MED], GOAL_QUEUE_MED_SIZE);
        GOAL_CM_SETVAR_UINT16(pVarQueueSize[GOAL_QUEUE_POOL_BIG], GOAL_QUEUE_BIG_SIZE);
        GOAL_CM_SETVAR_UINT16(pVarQueueCnt[GOAL_QUEUE_POOL_SMALL], GOAL_QUEUE_SMALL_NUM);
        GOAL_CM_SETVAR_UINT16(pVarQueueCnt[GOAL_QUEUE_POOL_MED], GOAL_QUEUE_MED_NUM);
        GOAL_CM_SETVAR_UINT16(pVarQueueCnt[GOAL_QUEUE_POOL_BIG], GOAL_QUEUE_BIG_NUM);
    }
#if GOAL_CONFIG_LOGGING == 1
    else {
        /* GG_LOG ID:224: */
        goal_lmLog(GOAL_ID_MEM, 224, 179, 0, 0, GOAL_LOG_SEV_INFO, "Use config manager variables for buffer number and sizes instead of default value defines");
    }
#endif

    return GOAL_OK;
}


/****************************************************************************/
/** Get buffer from memory pool queue
 *
 * This functions provides a buffer from memory pool queues. It returns the
 * smalles existing buffer in the pool queues which size is at least the
 * given minimal size.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_queuePoolGetBuf(
    GOAL_BUFFER_T **ppBuf,                      /**< buffer ref ptr */
    uint16_t sizeMin,                           /**< minimal size of requested buffer */
    GOAL_ID_T usage                             /**< buffer usage */
)
{
    uint32_t cnt;                               /* counter */
    GOAL_STATUS_T res;                          /* result */

    for (cnt = 0; cnt < GOAL_QUEUE_POOL_NUM; cnt++) {
        /* check if queue is was initialized */
        if (NULL == pPoolQueues[cnt]) {
            continue;
        }

        /* check if buffer is available */
        if (goal_queueIsEmpty(pPoolQueues[cnt])) {
            continue;
        }

        /* check if buffer size suffices */
        if (sizeMin > poolDesc[cnt].sizeBuf) {
            continue;
        }

        /* get buffer */
        res = goal_queueGetNewBuf(ppBuf, pPoolQueues[cnt], usage);
        if (GOAL_RES_ERR(res)) {
            goal_logDbg("failed to get queue element, try next queue");
            continue;
        }

        /* check for correct buffer size */
        if ((*ppBuf)->bufSize < sizeMin) {
            res = goal_queueReleaseBuf(ppBuf);
            if (GOAL_RES_ERR(res)) {
                /* GG_LOG ID:225: */
                goal_lmLog(GOAL_ID_MEM, 225, 180, 0, 0, GOAL_LOG_SEV_ERROR, "error during buffer release");
            }
            continue;
        }

        /* found buffer */
        return res;
    }

    /* no buffer of correct size availabe */
    goal_logDbg("no buffer of size %u or bigger available", sizeMin);
    return GOAL_ERR_NOT_FOUND;
}


/****************************************************************************/
/** Initialize a queue with memory buffers.
 *
 * This function allocates and initializes a queue. The queue's size depends on
 * the maximum number of elements. If @em numElem is not zero, the specified
 * number off buffers is allocated and added to the queue. The size of each
 * buffer must be given via @em bufSize.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_queueInit(
    GOAL_QUEUE_T **ppQueue,                     /**< pointer to queue pointer */
    uint16_t numElemMax,                        /**< max number of elements */
    uint16_t numElem,                           /**< number of frames to be added to the pool */
    uint16_t bufSize                            /**< size of each buffer in the pool */
)
{
    GOAL_STATUS_T retVal = GOAL_OK;             /* return value */

    /* The arugment numElemMax is a legacy argumente and not required anymore,
     * because the queue is a linked list without a max number of elements.
     * It's just available to keep API compatibility. */
    UNUSEDARG(numElemMax);

    /* allocate queue */
    retVal = goal_memCalloc(ppQueue, sizeof(GOAL_QUEUE_T));
    if (GOAL_OK != retVal) {
        return retVal;
    }

    /* set empty flag */
    (*ppQueue)->flagEmpty = 1;

    /* create mutex */
    retVal = goal_lockCreate(GOAL_LOCK_BINARY, &(*ppQueue)->pMutex, 0, 1, GOAL_ID_MEM_POOL);
    if (GOAL_RES_ERR(retVal)) {
        /* GG_LOG ID:226: */
        goal_lmLog(GOAL_ID_MEM, 226, 181, 0, 0, GOAL_LOG_SEV_ERROR, "failed to create queue lock");
        return retVal;
    }

    retVal = goal_queuePoolBufferAdd(*ppQueue, bufSize, numElem);

    return retVal;
}


/****************************************************************************/
/** Get and initialize buffer from a given queue pointer
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_queueGetNewBuf(
    GOAL_BUFFER_T **ppBuf,                      /**< buffer ref */
    GOAL_QUEUE_T *pQueue,                       /**< queue pointer */
    GOAL_ID_T usage                             /**< buffer usage */
)
{
    GOAL_STATUS_T res;                          /* result */
#if GOAL_CONFIG_DEBUG_QUEUE_USAGE == 1
    uint8_t poolId;                             /* pool id */
#endif /* GOAL_CONFIG_DEBUG_QUEUE_USAGE */

    /* make sure we don't overwrite an existing buffer */
    GOAL_ASSERT(NULL == *ppBuf);

    if (GOAL_OK != goal_queueGetElem(pQueue, (void **) ppBuf)) {
        /* GG_LOG ID:227: */
        goal_lmLog(GOAL_ID_MEM, 227, 182, 8, 0, GOAL_LOG_SEV_WARNING, "Failed to get the buffer from the queue: $1");
        goal_lmLogParamPTR((void *) pQueue);
        goal_lmLogFinish();
#if GOAL_CONFIG_DEBUG_QUEUE_USAGE == 1
    for (poolId = 0; poolId < GOAL_QUEUE_POOL_NUM; poolId++) {
        if (pQueue == pPoolQueues[poolId]) {
            goal_logInfo("full queue id : %d", poolId);
        }
    }
#endif /* GOAL_CONFIG_DEBUG_QUEUE_USAGE */
        return GOAL_ERR_BUFF_GET;
    }

    /* lock buffer metadata */
    res = goal_lockGet((*ppBuf)->pMtx, GOAL_LOCK_INFINITE);
    if (GOAL_OK != res) {
        /* GG_LOG ID:228: */
        goal_lmLog(GOAL_ID_MEM, 228, 183, 0, 0, GOAL_LOG_SEV_ERROR, "error requesting buffer mutex");
        goal_queueReleaseBuf(ppBuf);
        return res;
    }

    /* initialize buffer */
    (*ppBuf)->flags = GOAL_QUEUE_FLG_USED;
    (*ppBuf)->dataLen = 0;
    (*ppBuf)->usage = usage;
    (*ppBuf)->netPort = GOAL_ETH_PORT_HOST;
    (*ppBuf)->relCb = NULL;
    (*ppBuf)->pQueue = pQueue;
    (*ppBuf)->pEthBufHdlr = NULL;

#if GOAL_CONFIG_DEBUG_QUEUE_USAGE == 1
    for (poolId = 0; poolId < GOAL_QUEUE_POOL_NUM; poolId++) {
        if (pQueue == pPoolQueues[poolId]) {
            poolDesc[poolId].buffersFree--;

            if (poolDesc[poolId].buffersFree < poolDesc[poolId].minBuffersFree) {
                poolDesc[poolId].minBuffersFree = poolDesc[poolId].buffersFree;
            }
        }
    }
#endif /* GOAL_CONFIG_DEBUG_QUEUE_USAGE */

    /* unlock metadata */
    goal_lockPut((*ppBuf)->pMtx);

    return GOAL_OK;
}


/****************************************************************************/
/** Release a previously allocated buffer
 *
 * After the buffer is released, the buffer pointer is set to NULL.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_queueReleaseBuf(
    GOAL_BUFFER_T **ppBuf                       /**< buffer to release */
)
{
    GOAL_STATUS_T res;                          /* result */

    /* halt on empty buffer or already released buffer */
    GOAL_ASSERT(NULL != ppBuf);
    GOAL_ASSERT(NULL != *ppBuf);

    /* lock buffer metadata */
    res = goal_lockGet((*ppBuf)->pMtx, GOAL_LOCK_INFINITE);
    if (GOAL_OK != res) {
        return res;
    }

    return goal_queueReleaseBufInternal(ppBuf);
}


/****************************************************************************/
/** Release a previously allocated buffer
 *
 * After the buffer is released the buffer is set to NULL. This is also the
 * case if the buffer is marked as NO_RELEASE.
 *
 * Note: This function assumes that the buffer mutex is already locked.
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_queueReleaseBufInternal(
    GOAL_BUFFER_T **ppBuf                       /**< buffer to release */
)
{
    GOAL_QUEUE_CB_REL_T relCb;                  /* release callback */
    GOAL_BUFFER_T *pBuf;                        /* temporary buffer */
#if GOAL_CONFIG_DEBUG_QUEUE_USAGE == 1
    uint8_t poolId;                             /* pool id */
    GOAL_QUEUE_T *pQueue;                       /* queue */
#endif /* GOAL_CONFIG_DEBUG_QUEUE_USAGE */

    /* remember and clear storage pointer */
    pBuf = *ppBuf;
#if GOAL_CONFIG_DEBUG_QUEUE_USAGE == 1
    pQueue = pBuf->pQueue;
#endif /* GOAL_CONFIG_DEBUG_QUEUE_USAGE */

    *ppBuf = NULL;

    /* detect buffers that are already in transmission */
    if (GOAL_QUEUE_FLG_TX & pBuf->flags) {
        goal_logDbg("ignoring buffer that is marked with TX flag");

        /* unlock metadata */
        goal_lockPut(pBuf->pMtx);

        return GOAL_ERR_FRAME_IN_TRANSMISSION;
    }

    /* skip buffer if no-release is set */
    if (GOAL_QUEUE_FLG_NO_RELEASE & pBuf->flags) {

        goal_logDbg("buffer marked as no-release");

        /* unlock metadata */
        goal_lockPut(pBuf->pMtx);

        return GOAL_OK;
    }

    /* if no callback is attached release buffer */
    if (NULL == pBuf->relCb) {

        /* clear buffer-used flag */
        pBuf->flags &= (uint32_t) ~GOAL_QUEUE_FLG_USED;

        /* check if buffer must be returned to a queue */
        if (NULL != pBuf->pQueue) {

            if (GOAL_RES_ERR(goal_queueAddElem(pBuf->pQueue, pBuf))) {
                /* GG_LOG ID:229: */
                goal_lmLog(GOAL_ID_MEM, 229, 184, 0, 0, GOAL_LOG_SEV_ERROR, "failed to release buffer to queue");
            }

            /* prevent that buffer is released to same queue again */
            pBuf->pQueue = NULL;
        }

#if GOAL_CONFIG_DEBUG_QUEUE_USAGE == 1
    for (poolId = 0; poolId < GOAL_QUEUE_POOL_NUM; poolId++) {
        if (pQueue == pPoolQueues[poolId]) {
            poolDesc[poolId].buffersFree++;
        }
    }
#endif /* GOAL_CONFIG_DEBUG_QUEUE_USAGE */

        /* unlock metadata */
        goal_lockPut(pBuf->pMtx);

        return GOAL_OK;
    }

    /* only call callback and remove it to not create a recursion if the
     * callback decides to release the buffer */
    relCb = pBuf->relCb;
    pBuf->relCb = NULL;

    /* unlock metadata */
    goal_lockPut(pBuf->pMtx);

    /* check if callback needs to be called */
    return relCb(pBuf, pBuf->pArg);
}


/****************************************************************************/
/** Return first element from a given queue
 *
 * This function is thread-safe.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_queueGetElem(
    GOAL_QUEUE_T *pQueue,                       /**< queue pointer */
    void **pMem                                 /**< returned pointer to memory */
)
{
    GOAL_BUFFER_T *pBuf = NULL;                 /* memory buffer */
    GOAL_STATUS_T retVal = GOAL_OK;             /* return value */

    /* empty memory pointer to keep compatibility */
    *pMem = NULL;

    /* fast-return if queue is empty */
    if (pQueue->flagEmpty) {
        return GOAL_ERR_QUEUE_EMPTY;
    }

    /* get read mutex */
    goal_lockGet(pQueue->pMutex, GOAL_LOCK_INFINITE);

    /* get element if queue isn't empty */
    if (!(pQueue->flagEmpty)) {

        pBuf = pQueue->pBufList;

        GOAL_LL_DELETE(pQueue->pBufList, pBuf);

        /* decrease element counter */
        pQueue->cnt--;

        /* set empty flag if queue is now empty */
        if (0 == pQueue->cnt) {
            pQueue->flagEmpty = 1;
        }

        /* halt on empty buffer */
        GOAL_ASSERT(NULL != pBuf);
    }
    else {
        retVal = GOAL_ERR_QUEUE_EMPTY;
        goal_logDbg("queue empty: %p", (void *) pQueue);
    }

    /* release mutex */
    goal_lockPut(pQueue->pMutex);

    /* set return pointer */
    *pMem = pBuf;

    return retVal;
}


/****************************************************************************/
/** Insert an element into a given queue
 *
 * If queue is not full add a new element to the end of the queue.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_queueAddElem(
    GOAL_QUEUE_T *pQueue,                       /**< queue pointer */
    GOAL_BUFFER_T *pElem                        /**< pointer to the element */
)
{
    GOAL_STATUS_T retVal = GOAL_OK;             /* return value */
    GOAL_BUFFER_T *pBuf = NULL;                 /* iteration element */

    /* halt on empty buffer */
    GOAL_ASSERT(NULL != pElem);

    /* loop while waiting for write mutex */
    retVal = goal_lockGet(pQueue->pMutex, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(retVal)) {
        /* exit here in case of an error to ensure that the lock is received */
        return retVal;
    }

    /* check if buffer is already in pool, thus preventing double insertion */
    if (GOAL_RES_OK(retVal)) {
        GOAL_LL_FOREACH(pQueue->pBufList, pBuf) {
            if (pElem == pBuf) {
                retVal = GOAL_ERR_QUEUE_FULL;
            }
        }
    }

    if (GOAL_RES_OK(retVal)) {
        GOAL_LL_APPEND(pQueue->pBufList, pElem);

        /* remove queue empty flag */
        pQueue->flagEmpty = 0;

        /* increase element counter */
        pQueue->cnt++;
    }

    /* release mutex */
    goal_lockPut(pQueue->pMutex);

    return retVal;
}


/****************************************************************************/
/** Prepend an element to a given queue
 *
 * The element is added to the start of the queue.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_queuePrependElem(
    GOAL_QUEUE_T *pQueue,                       /**< queue pointer */
    GOAL_BUFFER_T *pElem                        /**< pointer to the element */
)
{
    GOAL_STATUS_T retVal = GOAL_OK;             /* return value */
    GOAL_BUFFER_T *pBuf = NULL;                 /* iteration element */

    /* halt on empty buffer */
    GOAL_ASSERT(NULL != pElem);

    /* loop while waiting for write mutex */
    retVal = goal_lockGet(pQueue->pMutex, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(retVal)) {
        /* exit here in case of an error to ensure that the lock is received */
        return retVal;
    }

    /* check if buffer is already in pool, thus preventing double insertion */
    if (GOAL_RES_OK(retVal)) {
        GOAL_LL_FOREACH(pQueue->pBufList, pBuf) {
            if (pElem == pBuf) {
                retVal = GOAL_ERR_QUEUE_FULL;
            }
        }
    }

    if (GOAL_RES_OK(retVal)) {
        GOAL_LL_PREPEND(pQueue->pBufList, pElem);

        /* remove queue empty flag */
        pQueue->flagEmpty = 0;

        /* increase element counter */
        pQueue->cnt++;
    }

    /* release mutex */
    goal_lockPut(pQueue->pMutex);

    return retVal;
}


/****************************************************************************/
/** Check if a Queue is Full
 *
 * Returns the the queue full state.
 *
 * @retval zero queue not full
 * @retval other queue is full
 */
uint8_t goal_queueIsFull(
    GOAL_QUEUE_T *pQueue                        /**< queue pointer */
)
{
    UNUSEDARG(pQueue);

    /* The queue is a linked list and will never reach the full level. */
    return 0;
}


/****************************************************************************/
/** Check if a Queue is Empty
 *
 * Returns the the queue empty state.
 *
 * @retval zero queue not empty
 * @retval other queue is empty
 */
uint8_t goal_queueIsEmpty(
    GOAL_QUEUE_T *pQueue                        /**< queue pointer */
)
{
    return pQueue->flagEmpty;
}


/****************************************************************************/
/** Release buffer to a new queue.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_queueReleaseBufToNewQueue(
    GOAL_QUEUE_T *pQueue,                       /**< new queue */
    GOAL_BUFFER_T **ppBuf                       /**< buffer location pointer */
)
{
    GOAL_STATUS_T res;                          /* result */

    GOAL_ASSERT(NULL != ppBuf);
    GOAL_ASSERT(NULL != *ppBuf);

    /* lock buffer metadata */
    res = goal_lockGet((*ppBuf)->pMtx, GOAL_LOCK_INFINITE);
    if (GOAL_OK != res) {
        return res;
    }

    (*ppBuf)->pQueue = pQueue;

    return goal_queueReleaseBufInternal(ppBuf);
}


/****************************************************************************/
/** Release the buffer to its original queue
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T goal_queueReleaseBufToOrigQueue(
    GOAL_BUFFER_T **ppBuf                       /**< buffer location pointer */
)
{
    GOAL_STATUS_T res;                          /* result */

    GOAL_ASSERT(NULL != ppBuf);
    GOAL_ASSERT(NULL != *ppBuf);

    /* lock buffer metadata */
    res = goal_lockGet((*ppBuf)->pMtx, GOAL_LOCK_INFINITE);
    if (GOAL_OK != res) {
        return res;
    }

    (*ppBuf)->pQueue = (*ppBuf)->pOrigQueue;

    return goal_queueReleaseBufInternal(ppBuf);
}


/****************************************************************************/
/** Set the buffers release callback
 */
void goal_queueSetReleaseCallback(
    GOAL_BUFFER_T *pBuf,                        /**< buffer pointer */
    GOAL_QUEUE_CB_REL_T relCb,                  /**< release callback */
    void *pArg                                  /**< release callback argument */
)
{
    GOAL_STATUS_T res;                          /* result */

    GOAL_ASSERT(NULL != pBuf);
    GOAL_ASSERT(NULL == pBuf->relCb);

    /* lock buffer metadata */
    res = goal_lockGet(pBuf->pMtx, GOAL_LOCK_INFINITE);
    if (GOAL_OK != res) {
        return;
    }

    pBuf->relCb = relCb;
    pBuf->pArg = pArg;

    /* unlock metadata */
    goal_lockPut(pBuf->pMtx);
}


/****************************************************************************/
/** Clear Buffer Flags
 *
 * Clear given buffer flags.
 */
void goal_queueFlagsClr(
    GOAL_BUFFER_T *pBuf,                        /**< buffer pointer */
    uint32_t flags                              /**< buffer flags */
)
{
    GOAL_STATUS_T res;                          /* result */

    GOAL_ASSERT(NULL != pBuf);

    /* lock buffer meta data */
    res = goal_lockGet(pBuf->pMtx, GOAL_LOCK_INFINITE);
    if (GOAL_OK != res) {
        return;
    }

    pBuf->flags &= ~flags;

    /* unlock meta data */
    goal_lockPut(pBuf->pMtx);
}


/****************************************************************************/
/** Check if given Flags are set
 *
 * Check if the given buffer flags are set.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_BOOL_T goal_queueFlagsGet(
    GOAL_BUFFER_T *pBuf,                        /**< buffer pointer */
    uint32_t flags                              /**< flags */
)
{
    GOAL_ASSERT(NULL != pBuf);

    return ((pBuf->flags & flags) == flags) ? GOAL_TRUE : GOAL_FALSE;
}


/****************************************************************************/
/** Set Buffer Flags
 *
 * Set given buffer flags.
 */
void goal_queueFlagsSet(
    GOAL_BUFFER_T *pBuf,                        /**< buffer pointer */
    uint32_t flags                              /**< buffer flags */
)
{
    GOAL_STATUS_T res;                          /* result */

    GOAL_ASSERT(NULL != pBuf);

    /* lock buffer meta data */
    res = goal_lockGet(pBuf->pMtx, GOAL_LOCK_INFINITE);
    if (GOAL_OK != res) {
        return;
    }

    pBuf->flags |= flags;

    /* unlock meta data */
    goal_lockPut(pBuf->pMtx);
}


/****************************************************************************/
/** Request buffers for allocation at startup
 *
 * This function can be called by every application to request a certain amount
 * of GOAL_BUFFER_T buffers to be allocated at start of GOAL.
 *
 * The caller needs to specifiy a buffer size it requires. Further it must
 * specify how many of those buffers will be required implicitly (cntBufFixed)
 * and how many additional buffers it may require at specific times.
 *
 * Internally at first a fitting queue pool (regarding requested buffer size) is
 * searched. This is successful, if the requests size fits into one of the
 * available buffers.
 *
 * Next step is to allocate buffers. Fixed buffers are always allocated.
 * Temporarily required buffers are only allocated at the maximum amount one
 * requested. So if application A requires 10 temporary buffers and application
 * B requrires 7 temporary buffers, a total of additional 10 buffers is allocated.
 *
 */
GOAL_STATUS_T goal_queuePoolBufsReq(
    GOAL_ID_T usageId,                          /**< goal id of user */
    uint16_t bufSize,                           /**< requested buffer size */
    uint16_t numBufFixed,                       /**< required number of buffers */
    uint16_t numBufTemp                         /**< at times required amount of buffers */
)
{
    uint8_t poolId;                             /* pool number */
    GOAL_STATUS_T res;                          /* result */
    GOAL_QUEUE_T *pQueue = NULL;                /* queue handle */

    /* only used when logging is enabled */
    UNUSEDARG(usageId);

    goal_logDbg("ID(%u) requests buffers[%d]", usageId, bufSize);
    goal_logDbg("ID(%u) fixed = %d", usageId, numBufFixed);
    goal_logDbg("ID(%u) temp = %d", usageId, numBufTemp);

    /* evaluate slot where requested buffers fit */
    for (poolId = 0; poolId < GOAL_QUEUE_POOL_NUM; poolId++) {

        /* find fitting pool */
        if (bufSize <= poolDesc[poolId].sizeBuf) {
            break;
        }
    }

    /* set pQueue to found queue or handle error */
    if (poolId < GOAL_QUEUE_POOL_NUM) {

        if (NULL == pPoolQueues[poolId]) {
            res = GOAL_ERR_NULL_POINTER;
        }
        else {
            pQueue = pPoolQueues[poolId];
            res = GOAL_OK;
        }
    }
    else {
        res = GOAL_ERR_NOT_FOUND;
    }

    /* add requested buffers to the fitting queue pool */
    if (GOAL_RES_OK(res)) {
        res = goal_lockGet(pQueue->pMutex, GOAL_LOCK_INFINITE);

        if (GOAL_RES_OK(res)) {
            /* add required pools */
            poolDesc[poolId].reqBufCntFixed += numBufFixed;
            res = goal_queuePoolBufferAdd(pQueue, bufSize, numBufFixed);
        }

        if (GOAL_RES_OK(res)) {
            if (numBufTemp > poolDesc[poolId].reqBufCntTemp) {
                /* add temporarily required pools */
                res = goal_queuePoolBufferAdd(pQueue, bufSize, numBufTemp - poolDesc[poolId].reqBufCntTemp);
                if (GOAL_RES_OK(res)) {
                    poolDesc[poolId].reqBufCntTemp = numBufTemp;
                }
            }
        }

        goal_lockPut(pQueue->pMutex);
    }

    return res;
}


/****************************************************************************/
/** Allocate buffer and add to given pool
 *
 * Note: This function assumes that the queue mutex is already locked.
 *
 * @returns GOAL_STATUS_T result
 */
static GOAL_STATUS_T goal_queuePoolBufferAdd(
    GOAL_QUEUE_T *pQueue,                       /**< pool queue */
    uint16_t bufSize,                           /**< buffer size */
    uint16_t numElem                            /**< number of eleements to add */
)
{
    GOAL_BUFFER_T *pBuf;                        /* memory buffer pointer */
    uint32_t cnt = 0;                           /* element counter */
    GOAL_STATUS_T retVal = GOAL_OK;             /* result */
#if GOAL_CONFIG_DEBUG_QUEUE_USAGE == 1
    uint8_t poolId;                             /* pool id */
#endif /* GOAL_CONFIG_DEBUG_QUEUE_USAGE */

    /* add elements to queue */
    for (cnt = 0; (cnt < numElem) && (GOAL_OK == retVal); cnt++) {

        /* allocate queue element from static storage */
        retVal = goal_memCalloc(&pBuf, sizeof(GOAL_BUFFER_T));
        if (GOAL_OK == retVal) {

            if (0 != bufSize) {
                /* allocate actual data buffer */
                retVal = goal_memCallocAlign(&pBuf->ptrData, bufSize, GOAL_TARGET_MEM_ALIGN_NET);
            } else {
                /* meta buffer only, data buffer will be added later */
                pBuf->ptrData = NULL;
            }
        }

        if (GOAL_OK != retVal) {
            break;
        }

        /* init buffer values */
        retVal = goal_lockCreate(GOAL_LOCK_BINARY, &pBuf->pMtx, 0, 1, GOAL_ID_MEM_POOL);
        if (GOAL_RES_ERR(retVal)) {
            /* GG_LOG ID:230: */
            goal_lmLog(GOAL_ID_MEM, 230, 185, 0, 0, GOAL_LOG_SEV_ERROR, "failed to create queue element lock");
            return retVal;
        }

        pBuf->bufSize = bufSize;
        pBuf->pOrigQueue = pQueue;
        pBuf->pQueue = pQueue;

        /* add buffer to queue */
        retVal = goal_queueAddElem(pQueue, pBuf);
    }

#if GOAL_CONFIG_DEBUG_QUEUE_USAGE == 1
    for (poolId = 0; poolId < GOAL_QUEUE_POOL_NUM; poolId++) {
        if (pPoolQueues[poolId] == pQueue) {
            poolDesc[poolId].minBuffersFree += numElem;
            poolDesc[poolId].buffersFree += numElem;
        }
    }
#endif /* GOAL_CONFIG_DEBUG_QUEUE_USAGE */

    return retVal;
}
