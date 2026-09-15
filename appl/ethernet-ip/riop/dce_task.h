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

/** Lifecycle of the Modbus master stack, for bench diagnostics. */
typedef enum {
    DCE_MB_NOT_STARTED = 0, /**< dce_taskInit() not yet called          */
    DCE_MB_TASK_ENTERED,    /**< mbpoll task is running                  */
    DCE_MB_INIT_FAILED,     /**< eMBMasterInit() returned an error        */
    DCE_MB_ENABLE_FAILED,   /**< eMBMasterEnable() returned an error      */
    DCE_MB_RUNNING          /**< stack up, dce task issuing FC03 polls    */
} DCE_MB_STATE_T;

/** Bench diagnostics. Read this over GDB ("p dceDiag") to see what the
 *  link is doing without a serial console:
 *    polls == good            -> DCE answering every request
 *    timeouts climbing        -> no response: wiring, DE polarity, baud,
 *                                or DCE Modbus not enabled on its panel
 *    rxErrors climbing        -> bytes arrive but frames fail CRC/length:
 *                                stop bits, T3.5, or noise
 *    exceptions climbing      -> DCE replied with a Modbus exception:
 *                                wrong slave address or register range
 *    good > 0 but rawMismatch -> callback staged a different reg count
 */
typedef struct {
    DCE_MB_STATE_T mbState;
    uint32_t polls;         /**< FC03 requests issued                     */
    uint32_t good;          /**< requests that returned MB_MRE_NO_ERR     */
    uint32_t timeouts;      /**< MB_MRE_TIMEDOUT                          */
    uint32_t rxErrors;      /**< MB_MRE_REV_DATA                          */
    uint32_t exceptions;    /**< MB_MRE_EXE_FUN                           */
    uint32_t otherErrors;   /**< any other code (ILL_ARG, MASTER_BUSY..)  */
    uint32_t rawMismatch;   /**< good reply but mbcb_rawGet() refused it  */
    uint32_t cwratioZero;   /**< good reply with CWratio == 0             */
    uint32_t staleEvents;   /**< times valid was cleared by the timeout   */
    int32_t  lastErr;       /**< most recent eMBMasterReqErrCode          */
    uint16_t lastRegs[4];   /**< raw registers 504..507 from last good    */
} DCE_DIAG_T;

/** Create the DCE polling task. Call once during board init. */
void dce_taskInit(void);

/** Snapshot the latest values. Safe to call from any task context. */
void dce_dataGet(DCE_DATA_T *pOut);

/** Snapshot the diagnostics. Not locked; counters are monotonic and a torn
 *  read is harmless for a bench readout. */
void dce_diagGet(DCE_DIAG_T *pOut);

#endif /* DCE_TASK_H */
