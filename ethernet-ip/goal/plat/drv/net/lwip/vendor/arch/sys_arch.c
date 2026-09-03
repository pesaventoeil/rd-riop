/** @file
 *
 * @brief
 * GOAL Connector for lwIP TCP/IP Stack
 *
 * @details
 * This module connects the lwIP TCP/IP stack to GOAL.
 *
 * @copyright
 * Copyright 2010-2021.
 * This software is protected Intellectual Property and may only be used
 * according to the license agreement.
 */

#define GOAL_ID GOAL_ID_DRV_NET
#include <goal_includes.h>
#include "lwip/opt.h"
#include "lwip/sys.h"


/****************************************************************************/
/* Local data types */
/****************************************************************************/
#if NO_SYS == 0

#if GOAL_CONFIG_TASK == 1
/** lwIP thread wrapper data */
typedef struct {
    lwip_thread_fn func;                        /**< thread function */
    void *pArg;                                 /**< thread function argument */
} GOAL_TARGET_NET_THREAD_DATA_T;
#endif /* GOAL_CONFIG_TASK == 1 */

/** mailbox pool */
typedef struct {
    GOAL_MBOX_T *pMbox;                         /**< mailbox */
    GOAL_BOOL_T used;                           /**< use flag */
} GOAL_TARGET_NET_MBOX_POOL_T;

/** lock pool */
typedef struct {
    GOAL_LOCK_T *pLock;                         /**< lock */
    GOAL_BOOL_T used;                           /**< use flag */
} GOAL_TARGET_NET_LOCK_POOL_T;
#endif /* NO_SYS */


/****************************************************************************/
/* Local functions */
/****************************************************************************/
#if NO_SYS == 0 && GOAL_CONFIG_TASK == 1
static GOAL_STATUS_T goal_targetThreadWrapper(
    void *pArg                                  /**< thread argument */
);
#endif /* NO_SYS == 0 && GOAL_CONFIG_TASK == 1 */


/****************************************************************************/
/* Local variables */
/****************************************************************************/
#if NO_SYS == 0
static GOAL_TARGET_NET_MBOX_POOL_T mboxPool[MEMP_NUM_NETCONN]; /**< pool of mailboxes */
static GOAL_TARGET_NET_LOCK_POOL_T lockPool[MEMP_NUM_NETCONN]; /**< pool of locks */
static GOAL_LOCK_T *pPoolLock = NULL;           /**< pool access lock */
#endif /* NO_SYS */


/****************************************************************************/
/* Global Variables */
/****************************************************************************/
#if NO_SYS == 0
int errno;                                      /**< current error of BSD socket API */
#endif /* NO_SYS */


#if NO_SYS == 0
/****************************************************************************/
/** Initialize the sys_arch layer.
 *
 * @ingroup sys_misc
 *
 * sys_init() must be called before anything else.
 */
void sys_init(
    void
)
{
    GOAL_STATUS_T res;                          /* result */
    int cnt;                                    /* loop counter */

    res = goal_lockCreate(GOAL_LOCK_BINARY, &pPoolLock, 0, 1, GOAL_ID_DRV_NET);

    for (cnt = 0; (cnt < MEMP_NUM_NETCONN) && GOAL_RES_OK(res); cnt++) {
        lockPool[cnt].pLock = NULL;
        res = goal_lockCreate(GOAL_LOCK_BINARY, &(lockPool[cnt].pLock), 0, 1, GOAL_ID_DRV_NET);
        lockPool[cnt].used = GOAL_FALSE;
    }

    for (cnt = 0; (cnt < MEMP_NUM_NETCONN) && GOAL_RES_OK(res); cnt++) {
        mboxPool[cnt].pMbox = NULL;
        res = goal_mboxCreate(&(mboxPool[cnt].pMbox), (uint16_t) DEFAULT_UDP_RECVMBOX_SIZE, sizeof(void *));
        mboxPool[cnt].used = GOAL_FALSE;
    }

    GOAL_ASSERT(GOAL_RES_OK(res));
}


/****************************************************************************/
/** Create a new semaphore
 *
 * @ingroup sys_sem
 *
 * Creates a new semaphore. The semaphore is allocated to the memory that 'sem'
 * points to (which can be both a pointer or the actual OS structure).
 * The "count" argument specifies the initial state of the semaphore (which is
 * either 0 or 1).
 * If the semaphore has been created, ERR_OK should be returned. Returning any
 * other error will provide a hint what went wrong, but except for assertions,
 * no real error handling is implemented.
 *
 * @return ERR_OK if successful, another err_t otherwise
 */
err_t sys_sem_new(
    sys_sem_t *sem,                             /**< pointer to the semaphore to create */
    u8_t count                                  /**< initial count of the semaphore */
)
{
    int cnt;                                    /* loop counter */

    UNUSEDARG(count);
/*TEST*/goal_logInfo("!!!!!!!! count %u", count);

    goal_lockGet(pPoolLock, GOAL_LOCK_INFINITE);

    for (cnt = 0; cnt < MEMP_NUM_NETCONN; cnt++) {
        if (GOAL_FALSE == lockPool[cnt].used) {
            *sem = lockPool[cnt].pLock;
            lockPool[cnt].used = GOAL_TRUE;
            break;
        }
    }

    goal_lockPut(pPoolLock);

    return (cnt < MEMP_NUM_NETCONN) ? ERR_OK : ERR_MEM;
}


/****************************************************************************/
/** Signals a semaphore
 *
 * @ingroup sys_sem
 */
void sys_sem_signal(
    sys_sem_t *sem                              /**< the semaphore to signal */
)
{
    goal_lockPut(*sem);
}


/****************************************************************************/
/** Blocks the thread while waiting for the semaphore to be signaled.
 *
 * @ingroup sys_sem
 *
 * If "timeout" argument is non-zero, the thread should only be blocked for the
 * specified time (measured in milliseconds). If the "timeout" argument is zero,
 * the thread should be blocked until the semaphore is signalled.
 *
 * The return value is SYS_ARCH_TIMEOUT if the semaphore wasn't signaled within
 * the specified time or any other value if it was signaled (with or without
 * waiting).
 * Notice that lwIP implements a function with a similar name,
 * sys_sem_wait(), that uses the sys_arch_sem_wait() function.
 *
 * @return SYS_ARCH_TIMEOUT on timeout, any other value on success
 */
u32_t sys_arch_sem_wait(
    sys_sem_t *sem,                             /**< the semaphore to wait for */
    u32_t timeout                               /**< timeout in milliseconds to wait */
)
{
    GOAL_STATUS_T res;                          /* result */

    /* timeout parameter is compatible to GOAL API */
    res = goal_lockGet(*sem, timeout);

    return (GOAL_ERR_LOCK_GET_TIMEOUT == res) ? SYS_ARCH_TIMEOUT : ~SYS_ARCH_TIMEOUT;
}


/****************************************************************************/
/** Deallocates a semaphore
 *
 * @ingroup sys_sem
 */
void sys_sem_free(
    sys_sem_t *sem                              /**< semaphore to delete */
)
{
    int cnt;                                    /* loop counter */

/*TEST*/ goal_logInfo("!!!!!! free sem");

    goal_lockGet(pPoolLock, GOAL_LOCK_INFINITE);

    for (cnt = 0; cnt < MEMP_NUM_NETCONN; cnt++) {
        if (*sem == lockPool[cnt].pLock) {
            lockPool[cnt].used = GOAL_FALSE;
            break;
        }
    }

    goal_lockPut(pPoolLock);

    GOAL_ASSERT(cnt < MEMP_NUM_NETCONN);
}


/****************************************************************************/
/** Returns 1 if the semaphore is valid, 0 if it is not valid
 *
 * @ingroup sys_sem
 * .
 * When using pointers, a simple way is to check the pointer for != NULL.
 * When directly using OS structures, implementing this may be more complex.
 * This may also be a define, in which case the function is not prototyped.
 */
int sys_sem_valid(
    sys_sem_t *sem                              /**< semaphore to validate */
)
{
    return (*sem == NULL) ? 0 : 1;
}


/****************************************************************************/
/** Invalidate a semaphore
 *
 * @ingroup sys_sem
 *
 * Invalidate a semaphore so that sys_sem_valid() returns 0.
 * ATTENTION: This does NOT mean that the semaphore shall be deallocated:
 * sys_sem_free() is always called before calling this function!
 * This may also be a define, in which case the function is not prototyped.
 */
void sys_sem_set_invalid(
    sys_sem_t *sem                              /**< semaphore to invalidate */
)
{
    *sem = NULL;
}


/****************************************************************************/
/** Creates an empty mailbox for maximum "size" elements
 *
 * @ingroup sys_mbox
 *
 * Elements stored
 * in mailboxes are pointers. You have to define macros "_MBOX_SIZE"
 * in your lwipopts.h, or ignore this parameter in your implementation
 * and use a default size.
 * If the mailbox has been created, ERR_OK should be returned. Returning any
 * other error will provide a hint what went wrong, but except for assertions,
 * no real error handling is implemented.
 *
 * @return ERR_OK if successful, another err_t otherwise
 */
err_t sys_mbox_new(
    sys_mbox_t *mbox,                           /**< pointer to the mbox to create */
    int size                                    /**< (minimum) number of messages in this mbox */
)
{
    int cnt;                                    /* loop counter */

    UNUSEDARG(size);
/*TEST*/goal_logInfo("!!!!!!!!");

    goal_lockGet(pPoolLock, GOAL_LOCK_INFINITE);

    for (cnt = 0; cnt < MEMP_NUM_NETCONN; cnt++) {
        if (GOAL_FALSE == mboxPool[cnt].used) {
            *mbox = mboxPool[cnt].pMbox;
            mboxPool[cnt].used = GOAL_TRUE;
            break;
        }
    }

    goal_lockPut(pPoolLock);

    return (cnt < MEMP_NUM_NETCONN) ? ERR_OK : ERR_MEM;
}


/****************************************************************************/
/** Post a message to an mbox - may not fail
 *
 * @ingroup sys_mbox
 *
 * -> blocks if full, only to be used from tasks NOT from ISR!
 */
void sys_mbox_post(
    sys_mbox_t *mbox,                           /**< mbox to posts the message */
    void *msg                                   /**< message to post (ATTENTION: can be NULL) */
)
{
    if (NULL == msg) {
        return;
    }

    while (goal_mboxIsFull(*mbox)) {
#if GOAL_CONFIG_TASK == 1
        goal_taskMsSleep(500, NULL);
#else /* GOAL_CONFIG_TASK */
        /* execute other loop functions, current function is excluded */
        goal_loop();
#endif /* GOAL_CONFIG_TASK */

    }

    goal_mboxMsgPut(*mbox, (void *) &msg);
}


/****************************************************************************/
/** Try to post a message to an mbox - may fail if full.
 *
 * @ingroup sys_mbox
 *
 * Can be used from ISR (if the sys arch layer allows this).
 *
 * @return ERR_MEM if it is full, else, ERR_OK if the "msg" is posted.
 */
err_t sys_mbox_trypost(
    sys_mbox_t *mbox,                           /**< mbox to posts the message */
    void *msg                                   /**< message to post (ATTENTION: can be NULL) */
)
{
    if (NULL == msg) {
        return ERR_OK;
    }

    if (goal_mboxIsFull(*mbox)) {
        return ERR_MEM;
    }

    goal_mboxMsgPut(*mbox, (void *) &msg);

    return ERR_OK;
}


/****************************************************************************/
/** Try to post a message to an mbox - may fail if full.
 *
 * @ingroup sys_mbox
 *
 * To be be used from ISR.
 * Returns ERR_MEM if it is full, else, ERR_OK if the "msg" is posted.
 *
 * @param mbox mbox to posts the message
 * @param msg message to post (ATTENTION: can be NULL)
 */
err_t sys_mbox_trypost_fromisr(
    sys_mbox_t *mbox,                           /**< mbox to posts the message */
    void *msg                                   /**< message to post (ATTENTION: can be NULL) */
)
{
    return sys_mbox_trypost(mbox, msg);
}


/****************************************************************************/
/** Blocks the thread until a message arrives in the mailbox
 *
 * @ingroup sys_mbox
 *
 * but does
 * not block the thread longer than "timeout" milliseconds (similar to
 * the sys_arch_sem_wait() function). If "timeout" is 0, the thread should
 * be blocked until a message arrives. The "msg" argument is a result
 * parameter that is set by the function (i.e., by doing "*msg =
 * ptr"). The "msg" parameter maybe NULL to indicate that the message
 * should be dropped.
 * The return values are the same as for the sys_arch_sem_wait() function:
 * SYS_ARCH_TIMEOUT if there was a timeout, any other value if a messages
 * is received.
 *
 * Note that a function with a similar name, sys_mbox_fetch(), is
 * implemented by lwIP.
 *
 * @return SYS_ARCH_TIMEOUT on timeout, any other value if a message has been received
 */
u32_t sys_arch_mbox_fetch(
    sys_mbox_t *mbox,                           /**< mbox to get a message from */
    void **msg,                                 /**< pointer where the message is stored */
    u32_t timeout                               /**< maximum time (in milliseconds) to wait for a message (0 = wait forever) */
)
{
    GOAL_STATUS_T res;                          /* result */
    PtrCast ptrVal = 0;                         /* pointer value */
#if GOAL_CONFIG_TASK == 0
    GOAL_TIMESTAMP_T start;                     /* start time */
    GOAL_TIMESTAMP_T now;                       /* current time */
    GOAL_TIMESTAMP_T waitTime;                  /* wait time */

    start = goal_targetGetTimestamp();
    waitTime = (0 == timeout) ? UINT64_MAX : timeout;
#endif /* GOAL_CONFIG_TASK == 0 */

    res = goal_mboxMsgGet(*mbox, (uint8_t *) &ptrVal);

    if (GOAL_RES_ERR(res)) {
        do {
#if GOAL_CONFIG_TASK == 1
            goal_taskMsSleep((0 == timeout) ? 800 : timeout, NULL);
#else /* GOAL_CONFIG_TASK */
            /* execute other loop functions, current function is excluded */
            goal_loop();
            now = goal_targetGetTimestamp();
            if ((UINT64_MAX != waitTime) && ((now - start) >= waitTime)) {
                /* leave loop after this iteration */
                timeout = 1;
            }
            else {
                /* another iteration */
                timeout = 0;
            }
#endif /* GOAL_CONFIG_TASK == 1 */
            res = goal_mboxMsgGet(*mbox, (uint8_t *) &ptrVal);
        } while ((0 == timeout) && (GOAL_RES_ERR(res)));
    }

    *msg = (void *) ptrVal;

    return (GOAL_RES_ERR(res)) ? SYS_ARCH_TIMEOUT : ~SYS_ARCH_TIMEOUT;
}


/****************************************************************************/
/** Fetch a message from Mailbox
 *
 * @ingroup sys_mbox
 *
 * This is similar to sys_arch_mbox_fetch, however if a message is not
 * present in the mailbox, it immediately returns with the code
 * SYS_MBOX_EMPTY. On success 0 is returned.
 * To allow for efficient implementations, this can be defined as a
 * function-like macro in sys_arch.h instead of a normal function. For
 * example, a naive implementation could be:
 * \#define sys_arch_mbox_tryfetch(mbox,msg) sys_arch_mbox_fetch(mbox,msg,1)
 * although this would introduce unnecessary delays.
 *
 * @return 0 (milliseconds) if a message has been received
 *         or SYS_MBOX_EMPTY if the mailbox is empty
 */
u32_t sys_arch_mbox_tryfetch(
    sys_mbox_t *mbox,                           /**< mbox to get a message from */
    void **msg                                  /**< pointer where the message is stored */
)
{
    GOAL_STATUS_T res;                          /* result */
    PtrCast ptrVal = 0;                         /* pointer value */

    res = goal_mboxMsgGet(*mbox, (uint8_t *) &ptrVal);

    *msg = (void *) ptrVal;

    return (GOAL_ERR_QUEUE_EMPTY == res) ? SYS_MBOX_EMPTY : 0;
}


/****************************************************************************/
/** Deallocates a mailbox
 *
 * @ingroup sys_mbox
 *
 * If there are messages still present in the
 * mailbox when the mailbox is deallocated, it is an indication of a
 * programming error in lwIP and the developer should be notified.
 */
void sys_mbox_free(
    sys_mbox_t *mbox                            /**< mbox to delete */
)
{
    int cnt;                                    /* loop counter */

    GOAL_ASSERT(0 != goal_mboxIsEmpty(*mbox));

/*TEST*/ goal_logInfo("!!!!!! free mbox");

    goal_lockGet(pPoolLock, GOAL_LOCK_INFINITE);

    for (cnt = 0; cnt < MEMP_NUM_NETCONN; cnt++) {
        if (*mbox == mboxPool[cnt].pMbox) {
            mboxPool[cnt].used = GOAL_FALSE;
            break;
        }
    }

    goal_lockPut(pPoolLock);

    GOAL_ASSERT(cnt < MEMP_NUM_NETCONN);
}


/****************************************************************************/
/** Returns 1 if the mailbox is valid, 0 if it is not valid.
 *
 * @ingroup sys_mbox
 *
 * When using pointers, a simple way is to check the pointer for != NULL.
 * When directly using OS structures, implementing this may be more complex.
 * This may also be a define, in which case the function is not prototyped.
 */
int sys_mbox_valid(
    sys_mbox_t *mbox                            /**< mailbox to validate */
)
{
    return (*mbox == NULL) ? 0 : 1;
}


/****************************************************************************/
/** Invalidate a mailbox so that sys_mbox_valid() returns 0.
 *
 * @ingroup sys_mbox
 *
 * ATTENTION: This does NOT mean that the mailbox shall be deallocated:
 * sys_mbox_free() is always called before calling this function!
 * This may also be a define, in which case the function is not prototyped.
 */
void sys_mbox_set_invalid(
    sys_mbox_t *mbox                            /**< mailbox to invalidate */
)
{
    *mbox = NULL;
}


/****************************************************************************/
/** Start a new thread
 *
 * @ingroup sys_misc
 *
 * The only thread function:
 * Starts a new thread named "name" with priority "prio" that will begin its
 * execution in the function "thread()". The "arg" argument will be passed as an
 * argument to the thread() function. The stack size to used for this thread is
 * the "stacksize" parameter. The id of the new thread is returned. Both the id
 * and the priority are system dependent.
 * ATTENTION: although this function returns a value, it MUST NOT FAIL (ports have to assert this!)
 */
sys_thread_t sys_thread_new(
    const char *name,                           /**< human-readable name for the thread (used for debugging purposes) */
    lwip_thread_fn thread,                      /**< thread-function */
    void *arg,                                  /**< parameter passed to 'thread' */
    int stacksize,                              /**< stack size in bytes for the new thread (may be ignored by ports) */
    int prio                                    /**< priority of the new thread (may be ignored by ports) */
)
{
    GOAL_STATUS_T res;                          /* result */
#if GOAL_CONFIG_TASK == 1
    GOAL_TARGET_NET_THREAD_DATA_T *pData = NULL; /* thread wrapper data */
    GOAL_TASK_T *pTask = NULL;                  /* task handle */

    UNUSEDARG(stacksize);
    UNUSEDARG(prio);

    res = goal_memAlloc(&pData, sizeof(GOAL_TARGET_NET_THREAD_DATA_T));

    if (GOAL_RES_OK(res)) {
        pData->func = thread;
        pData->pArg = arg;
        res = goal_taskCreate(&pTask, name, goal_targetThreadWrapper, pData,
                              GOAL_TASK_PRIO_DEFAULT, GOAL_TASK_STACK_DEFAULT,
                              GOAL_TRUE);
    }
#else /* GOAL_CONFIG_TASK */
    UNUSEDARG(name);
    UNUSEDARG(stacksize);
    UNUSEDARG(prio);

    res = goal_mainLoopParamReg(thread, arg);
#endif /* GOAL_CONFIG_TASK */

    GOAL_ASSERT(GOAL_RES_OK(res));

    return 0;
}


#if GOAL_CONFIG_TASK == 1
/****************************************************************************/
/** lwIP Thread wrapper
 *
 * @retval GOAL_OK - success
 * @retval other - fail
 */
static GOAL_STATUS_T goal_targetThreadWrapper(
    void *pArg                                  /**< thread argument */
)
{
    GOAL_TARGET_NET_THREAD_DATA_T *pData;       /* thread wrapper data */

    pData = (GOAL_TARGET_NET_THREAD_DATA_T *) pArg;

    pData->func(pData->pArg);

    return GOAL_OK;
}
#endif /* GOAL_CONFIG_TASK == 1 */
#endif /* NO_SYS == 0 */
