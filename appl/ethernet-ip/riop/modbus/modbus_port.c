/* modbus_port.c - critical section primitives for the DCE Modbus master.
 *
 * Location: C:\rd-riop\appl\ethernet-ip\riop\modbus\modbus_port.c
 *
 * Called from both task context (the mbpoll task, via the event layer) and
 * ISR context (the LPUART10 and TPM handlers, via the RTU state machine),
 * so this must not use taskENTER_CRITICAL().
 *
 * PRIMASK is used rather than BASEPRI deliberately: the guarded regions are
 * a handful of instructions around event flags, and PRIMASK save/restore is
 * simpler to reason about than interacting with FreeRTOS's BASEPRI nesting.
 * Note this does briefly mask GOAL's Ethernet interrupts -- acceptable at
 * this duration, but do not lengthen these regions.
 */

#include <stdint.h>

#include "fsl_device_registers.h"
#include "port.h"

static volatile uint32_t ulMbCriticalNesting = 0u;
static volatile uint32_t ulMbSavedPrimask    = 0u;

void vMBPortEnterCritical(void)
{
    uint32_t ulPrimask = __get_PRIMASK();

    __disable_irq();

    if (0u == ulMbCriticalNesting) {
        ulMbSavedPrimask = ulPrimask;
    }
    ulMbCriticalNesting++;
}

void vMBPortExitCritical(void)
{
    if (ulMbCriticalNesting > 0u) {
        ulMbCriticalNesting--;

        /* Only re-enable if interrupts were enabled when we first nested.
         * Restoring unconditionally would enable interrupts inside an outer
         * critical section that had disabled them. */
        if ((0u == ulMbCriticalNesting) && (0u == ulMbSavedPrimask)) {
            __enable_irq();
        }
    }
}
