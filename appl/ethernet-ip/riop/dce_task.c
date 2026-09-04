/* dce_task.c - CentriFeeder DCE Modbus RTU master
 *
 * Location: C:\rd-riop\appl\ethernet-ip\riop\dce_task.c
 *
 * Replaces the dummy generator with a real FC03 poll of the DCE's holding
 * registers 504..507 over RS-485 on LPUART10.
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include "mb.h"
#include "mb_m.h"
#include "mbport.h"

#include "dce_app.h"
#include "modbus_cb.h"
#include "dce_task.h"

#define DCE_POLL_PERIOD_MS      100u
#define DCE_TASK_PRIORITY       4u
#define DCE_TASK_STACK_SIZE     (configMINIMAL_STACK_SIZE + 256)

/* The FreeModbus poll task must outrank the requester so responses are
 * processed promptly, but must stay below the GOAL EtherNet/IP task so a
 * stalled RS-485 link can never delay a CIP connection.
 * >>> VERIFY against the GOAL task priority before shipping. <<< */
#define MB_POLL_TASK_PRIORITY   5u
#define MB_POLL_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE + 256)

/* How long a request may wait for the stack to become free. The response
 * timeout itself is MB_MASTER_TIMEOUT_US_RESPOND, enforced by the TPM. */
#define DCE_REQ_WAIT_MS         500u

/* Publish zero and clear valid after this long without a good poll. Silence
 * is a NORMAL state: the DCE only speaks Modbus when enabled from its front
 * panel, and drops out of eMBSPoll() the moment that is switched off. */
#define DCE_STALE_TIMEOUT_MS    1000u

static DCE_DATA_T dceData;

void dce_dataGet(
    DCE_DATA_T *pOut
)
{
    taskENTER_CRITICAL();
    *pOut = dceData;
    taskEXIT_CRITICAL();
}

static void dce_dataSet(
    float flowRate,
    float totalization
)
{
    taskENTER_CRITICAL();
    dceData.flowRate     = flowRate;
    dceData.totalization = totalization;
    dceData.tsLastGood   = (uint32_t) xTaskGetTickCount();
    dceData.valid        = true;
    taskEXIT_CRITICAL();
}

static void dce_dataInvalidate(
    void
)
{
    taskENTER_CRITICAL();
    dceData.flowRate     = 0.0f;
    dceData.totalization = 0.0f;
    dceData.valid        = false;
    taskEXIT_CRITICAL();
}

/* FreeModbus master event pump. Must run in a task, not an ISR: the event
 * layer blocks on a queue. This is the change from NXP's demo, which called
 * eMBMasterPoll() from a TPM interrupt handler. */
static void dce_mbPollTask(
    void *pvParameters
)
{
    (void) pvParameters;

    for (;;) {
        (void) eMBMasterPoll();
    }
}

/* Assemble a big-endian 32-bit value from two consecutive registers.
 * The DCE's word ordering is High Then Low, so the first register is the
 * high half. */
static uint32_t dce_u32Get(
    const uint16_t *pRegs
)
{
    return ((uint32_t) pRegs[0] << 16) | (uint32_t) pRegs[1];
}

static void dce_task(
    void *pvParameters
)
{
    TickType_t tsWake = xTaskGetTickCount();
    uint16_t   regs[DCE_REG_BLOCK_NREGS];
    float      total    = 0.0f;
    float      flowRate = 0.0f;

    (void) pvParameters;

    for (;;) {
        eMBMasterReqErrCode eErr;

        /* >>> BENCH CHECK: confirm 0x01F8 appears on the wire, not 0x01F7 or
         * 0x01F9. Some FreeModbus forks apply a +/-1 conversion to the
         * request address. Reading one register off gives EFS and Count
         * instead of Count and CWratio, and the numbers will look almost
         * plausible. <<< */
        eErr = eMBMasterReqReadHoldingRegister(DCE_MODBUS_SLAVE_ADDR,
                                               DCE_REG_COUNT_ADDR,
                                               DCE_REG_BLOCK_NREGS,
                                               pdMS_TO_TICKS(DCE_REQ_WAIT_MS));

        if ((MB_MRE_NO_ERR == eErr) && mbcb_rawGet(regs, DCE_REG_BLOCK_NREGS)) {
            uint32_t count   = dce_u32Get(&regs[0]);   /* usMBReg_Count       */
            uint32_t cwratio = dce_u32Get(&regs[2]);   /* usMBReg_ReadCWratio */

            if (0u != cwratio) {
                flowRate = (float) count / (float) cwratio;

                /* TODO: totalization source is still undecided. Integrating
                 * the rate over the poll interval is a placeholder that
                 * behaves sensibly, but if usMBReg_Count is itself an
                 * accumulating counter then Count/CWratio IS the total and
                 * the rate needs a different source. See the note below. */
                total += flowRate * ((float) DCE_POLL_PERIOD_MS / 60000.0f);

                dce_dataSet(flowRate, total);
            }
            /* cwratio == 0 means the DCE is uncalibrated. Hold the previous
             * values and let the staleness timeout decide. */
        }

        /* Staleness check runs regardless of this poll's outcome. */
        if (dceData.valid) {
            uint32_t age = (uint32_t) xTaskGetTickCount() - dceData.tsLastGood;

            if (age > pdMS_TO_TICKS(DCE_STALE_TIMEOUT_MS)) {
                dce_dataInvalidate();
            }
        }

        vTaskDelayUntil(&tsWake, pdMS_TO_TICKS(DCE_POLL_PERIOD_MS));
    }
}

void dce_taskInit(
    void
)
{
    memset(&dceData, 0, sizeof(dceData));

    /* 38400 8-N-2. eMBMasterInit takes no slave address; that is per-request.
     * Port number is ignored by the retargeted modbus_serial.c, which always
     * uses LPUART10. */
    if (MB_ENOERR != eMBMasterInit(MB_RTU, 0, DCE_MODBUS_BAUDRATE, MB_PAR_NONE)) {
        return;
    }
    if (MB_ENOERR != eMBMasterEnable()) {
        return;
    }

    (void) xTaskCreate(dce_mbPollTask, "mbpoll", MB_POLL_TASK_STACK_SIZE,
                       NULL, MB_POLL_TASK_PRIORITY, NULL);

    (void) xTaskCreate(dce_task, "dce", DCE_TASK_STACK_SIZE,
                       NULL, DCE_TASK_PRIORITY, NULL);
}
