/* portevent.c - FreeRTOS event layer for the FreeModbus RTU master.
 *
 * Location: C:\rd-riop\appl\ethernet-ip\riop\modbus\portevent.c
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Rewritten from NXP's bare-metal version, which had three defects:
 *
 *  1. xMBMasterPortEventGet() never cleared EV_MASTER_ERROR_RESPOND_TIMEOUT,
 *     EV_MASTER_ERROR_RECEIVE_DATA or EV_MASTER_ERROR_EXECUTE_FUNCTION. After
 *     the first timeout those bits stayed set forever, xEventInQueue never
 *     returned to FALSE, and every later eMBMasterWaitRequestFinish() call
 *     returned instantly.
 *  2. eMBMasterWaitRequestFinish() read its local 'event' uninitialised when
 *     xMBMasterPortEventGet() returned FALSE.
 *  3. It spun in while(1) with no yield and no timeout, which starves every
 *     lower-priority task -- including GOAL's EtherNet/IP stack.
 *
 * The fix is to stop overloading one flag word for two unrelated things.
 * Protocol state-machine events go in a queue; request completion results go
 * through a separate semaphore plus a result variable.
 */

#include <stdint.h>

/* For __get_IPSR(). Without this the CMSIS core intrinsics are never
 * declared here, and -Werror turns the implicit declaration into an error. */
#include "fsl_device_registers.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

#include "mb.h"
#include "mb_m.h"
#include "mbport.h"

/* Deep enough that a burst (FRAME_RECEIVED then EXECUTE) cannot be dropped. */
#define MB_EVENT_QUEUE_LEN      ( 8 )

/* Safety net only. The TPM enforces the real response timeout and the stack
 * always posts a result, so this should never fire -- but without it a lost
 * event would deadlock the requester and leak the run resource. */
#define MB_REQ_FINISH_TIMEOUT_MS ( 1000u )

static QueueHandle_t     xMasterEventQueue;   /* protocol events           */
static SemaphoreHandle_t xMasterRunRes;       /* one request at a time     */
static SemaphoreHandle_t xMasterResultSem;    /* request completion signal */
static volatile eMBMasterReqErrCode eMasterResult;

/* The RTU state machine posts events from the LPUART and TPM ISRs as well as
 * from the poll task, so every queue operation has to pick its API by
 * context. IPSR is non-zero inside an exception handler. */
static inline BOOL prvInIsr(void)
{
    return (__get_IPSR() != 0u) ? TRUE : FALSE;
}

/* ----------------------- Protocol events ----------------------------------*/

BOOL
xMBMasterPortEventInit( void )
{
    xMasterEventQueue = xQueueCreate(MB_EVENT_QUEUE_LEN, sizeof(eMBMasterEventType));

    return (NULL != xMasterEventQueue) ? TRUE : FALSE;
}

BOOL
xMBMasterPortEventPost( eMBMasterEventType eEvent )
{
    BOOL xResult = FALSE;

    if (prvInIsr()) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        if (pdTRUE == xQueueSendFromISR(xMasterEventQueue, &eEvent,
                                        &xHigherPriorityTaskWoken)) {
            xResult = TRUE;
        }
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    } else {
        if (pdTRUE == xQueueSend(xMasterEventQueue, &eEvent, 0)) {
            xResult = TRUE;
        }
    }

    return xResult;
}

BOOL
xMBMasterPortEventGet( eMBMasterEventType * eEvent )
{
    /* Blocks the mbpoll task rather than spinning. The bounded wait means a
     * lost event costs one idle period instead of hanging the task. */
    if (pdTRUE == xQueueReceive(xMasterEventQueue, eEvent, pdMS_TO_TICKS(50))) {
        return TRUE;
    }

    return FALSE;
}

/* ----------------------- Request serialisation ----------------------------*/

void vMBMasterOsResInit( void )
{
    /* Binary semaphores, NOT mutexes: xMBMasterRunResTake() runs in the
     * requesting task (dce) while vMBMasterRunResRelease() runs in the poll
     * task (mbpoll). FreeRTOS mutexes may only be given by the owning task. */
    xMasterRunRes    = xSemaphoreCreateBinary();
    xMasterResultSem = xSemaphoreCreateBinary();

    if (NULL != xMasterRunRes) {
        (void) xSemaphoreGive(xMasterRunRes);   /* starts available */
    }
}

BOOL xMBMasterRunResTake( LONG lTimeOut )
{
    TickType_t xWait = (lTimeOut < 0) ? portMAX_DELAY : (TickType_t) lTimeOut;

    if (pdTRUE != xSemaphoreTake(xMasterRunRes, xWait)) {
        return FALSE;
    }

    /* Discard any completion left over from a request that timed out on our
     * side. Without this a stale give would satisfy the next wait instantly. */
    (void) xSemaphoreTake(xMasterResultSem, 0);
    eMasterResult = MB_MRE_NO_ERR;

    return TRUE;
}

void vMBMasterRunResRelease( void )
{
    (void) xSemaphoreGive(xMasterRunRes);
}

/* ----------------------- Request result callbacks -------------------------*/
/* Called from the mbpoll task by eMBMasterPoll(). Each sets the result and
 * wakes the requester; mb_m.c calls vMBMasterRunResRelease() separately. */

void vMBMasterCBRequestScuuess( void )      /* upstream spelling, leave it */
{
    eMasterResult = MB_MRE_NO_ERR;
    (void) xSemaphoreGive(xMasterResultSem);
}

void vMBMasterErrorCBRespondTimeout(UCHAR ucDestAddress, const UCHAR *pucPDUData,
                                    USHORT ucPDULength)
{
    (void)ucDestAddress; (void)pucPDUData; (void)ucPDULength;
    eMasterResult = MB_MRE_TIMEDOUT;
    (void) xSemaphoreGive(xMasterResultSem);
}

void vMBMasterErrorCBReceiveData(UCHAR ucDestAddress, const UCHAR *pucPDUData,
                                 USHORT ucPDULength)
{
    (void)ucDestAddress; (void)pucPDUData; (void)ucPDULength;
    eMasterResult = MB_MRE_REV_DATA;
    (void) xSemaphoreGive(xMasterResultSem);
}

void vMBMasterErrorCBExecuteFunction(UCHAR ucDestAddress, const UCHAR *pucPDUData,
                                     USHORT ucPDULength)
{
    (void)ucDestAddress; (void)pucPDUData; (void)ucPDULength;
    eMasterResult = MB_MRE_EXE_FUN;
    (void) xSemaphoreGive(xMasterResultSem);
}

eMBMasterReqErrCode eMBMasterWaitRequestFinish( void )
{
    if (pdTRUE != xSemaphoreTake(xMasterResultSem,
                                 pdMS_TO_TICKS(MB_REQ_FINISH_TIMEOUT_MS))) {
        /* Should be unreachable. Release the run resource ourselves so a
         * single lost event cannot wedge the master permanently. Giving an
         * already-available binary semaphore is harmless. */
        vMBMasterRunResRelease();
        return MB_MRE_TIMEDOUT;
    }

    return eMasterResult;
}
