/* dce_task.c - CentriFeeder DCE interface
 *
 * PLUMBING-TEST VERSION (dummy generator, no Modbus/hardware).
 *
 * Produces a flat FlowRate of 123.45 and a Totalization that integrates it
 * over the poll interval, so the full path
 *   dce_task -> dce_dataGet() -> appl_eipTxPayloadPrepare -> adc15/adc16 -> PLC
 * can be validated before the RS-485 hardware is involved.
 *
 * The real FreeModbus FC03 version is preserved as dce_task_modbus.c; swap it
 * back in once CODESYS shows 123.45 / climbing total through this path.
 *
 * Location: appl\ethernet-ip\riop\dce_task.c
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include "dce_task.h"

#define DCE_POLL_PERIOD_MS      100u
#define DCE_TASK_PRIORITY       4u
#define DCE_TASK_STACK_SIZE     (configMINIMAL_STACK_SIZE + 256)

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

static void dce_task(
    void *pvParameters
)
{
    TickType_t tsWake = xTaskGetTickCount();
    float      total  = 0.0f;

    (void) pvParameters;

    for (;;) {
        /* Dummy: flat rate, integrate for a climbing total.
         * 123.45 units/min * (100 ms / 60000 ms) ~= 0.2058 per tick. */
        total += 123.45f * ((float) DCE_POLL_PERIOD_MS / 60000.0f);
        dce_dataSet(123.45f, total);

        vTaskDelayUntil(&tsWake, pdMS_TO_TICKS(DCE_POLL_PERIOD_MS));
    }
}

void dce_taskInit(
    void
)
{
    memset(&dceData, 0, sizeof(dceData));

    (void) xTaskCreate(dce_task, "dce", DCE_TASK_STACK_SIZE,
                       NULL, DCE_TASK_PRIORITY, NULL);
}
