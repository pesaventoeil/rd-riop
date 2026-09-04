#ifndef DCE_TASK_H
#define DCE_TASK_H

#include <stdint.h>
#include <stdbool.h>

/** CentriFeeder DCE process values, published by the Modbus RTU master. */
typedef struct {
    float    flowRate;      /**< engineering units per minute */
    float    totalization;  /**< engineering units, running total */
    uint32_t tsLastGood;    /**< ms timestamp of last successful poll */
    bool     valid;         /**< false until first good poll, or after timeout */
} DCE_DATA_T;

/** Create the DCE polling task. Call once during board init. */
void dce_taskInit(void);

/** Snapshot the latest values. Safe to call from any task context. */
void dce_dataGet(DCE_DATA_T *pOut);

#endif /* DCE_TASK_H */
