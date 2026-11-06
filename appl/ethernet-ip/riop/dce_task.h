#ifndef DCE_TASK_H
#define DCE_TASK_H

#include <stdint.h>
#include <stdbool.h>
#include "goal_app_eip.h"

/* Runtime gate: FreeModbus stays linked (M7FOLLOWER CMakeLists) whether this
 * is 0 or 1. When 0, dce_taskInit() only clears published floats — no mbpoll /
 * dce tasks, no LPUART10 / TPM6 bring-up. That matches the known-good "Modbus
 * in the image but idle" state (71ce78e) and keeps EIP/MCTC recoverable while
 * RS-485 is unwired. Set to 1 only after PC EIP Class 1 connects again
 * (see RECOVERY.md §5.0) and the bus is ready. Override from the build with
 * -DDCE_MODBUS_RUNTIME=1 if preferred. */
#ifndef DCE_MODBUS_RUNTIME
#define DCE_MODBUS_RUNTIME      1
#endif

/* Keep write execution compiled; mailbox-to-Modbus forwarding is still guarded
 * by a strict positive allowlist in dce_task.c. */
#ifndef DCE_MIRROR_WRITES
#define DCE_MIRROR_WRITES       1
#endif

/* Bench-only: remux LPUART10 RS-485 pads to GPIO and toggle DI+DE so a scope /
 * DS View / LabJack AIN can prove MCU→U35 drive. Do NOT drive AD_33 (RO).
 * When 1, Modbus is not started regardless of DCE_MODBUS_RUNTIME.
 * Keep at 0 for live FC03; M33 early-init bitbang also keys off this. */
#ifndef DCE_GPIO_BITBANG_TEST
#define DCE_GPIO_BITBANG_TEST   0
#endif
#ifndef DCE_GPIO_BITBANG_ON_M33
#define DCE_GPIO_BITBANG_ON_M33 0
#endif

/** CentriFeeder DCE process values, published by the Modbus RTU master. */
typedef struct {
    float    flowRate;      /**< engineering units per minute */
    float    totalization;  /**< engineering units, running total */
    uint32_t tsLastGood;    /**< ms timestamp of last successful poll */
    bool     valid;         /**< false until first good poll, or after timeout */
} DCE_DATA_T;

/** Lifecycle of the Modbus master stack, for bench diagnostics. */
typedef enum {
    DCE_MB_NOT_STARTED = 0, /**< dce_taskInit() not yet called            */
    DCE_MB_IDLE_LINKED,     /**< runtime gated off; FreeModbus linked only */
    DCE_MB_TASK_ENTERED,    /**< mbpoll task is running                    */
    DCE_MB_INIT_FAILED,     /**< eMBMasterInit() returned an error         */
    DCE_MB_ENABLE_FAILED,   /**< eMBMasterEnable() returned an error       */
    DCE_MB_CREATE_FAILED,   /**< xTaskCreate for mbpoll/dce failed         */
    DCE_MB_RUNNING          /**< stack up, dce task issuing FC03 polls     */
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

typedef enum {
    DCE_ACK_OK = 0u,
    DCE_ACK_REJECTED = 1u,
    DCE_ACK_NEEDS_ARM = 2u,
    DCE_ACK_SEQ_REPLAY = 3u,
    DCE_ACK_INVALID_ADDR = 4u,
    DCE_ACK_INVALID_LENGTH = 5u,
    DCE_ACK_WRITES_DISABLED = 6u,
    DCE_ACK_ALLOWLIST_REJECT = 7u
} DCE_MIRROR_ACK_T;

/* G48 ack_detail values >= 0x80 are LOCAL RIOP outcomes (never a DCE exception
 * code); 0x01..0x0B keep their Modbus exception meaning. */
#define DCE_ACK_DETAIL_LOCAL_REJECT   0x80u  /* master stack refused/failed the frame locally */
#define DCE_ACK_DETAIL_WRITE_PENDING  0x81u  /* previous mailbox write not executed yet        */
#define DCE_ACK_DETAIL_TIMEOUT        0x82u  /* DCE did not answer within the response timeout */
#define DCE_ACK_DETAIL_BAD_FRAME      0x83u  /* reply failed CRC/length/address checks         */
#define DCE_ACK_DETAIL_OTHER          0x84u  /* other master-stack error                        */

typedef struct __attribute__((packed)) {
    uint32_t seq;
    uint16_t op;
    uint16_t address;
    uint16_t lengthWords;
    uint16_t flags;
    uint16_t values[GOAL_APP_DCE_MIRROR_MAX_WRITE_WORDS];
} DCE_MIRROR_WRITE_REQ_T;

typedef struct {
    uint32_t scanCounter;
    uint32_t lastScanTicks;
    uint16_t commStatus;
    uint32_t ackSeq; /* Sequence echoed from the last mailbox ack. */
    uint16_t ackCode;
    uint16_t ackDetail; /* Modbus exception code / reject detail. */
    uint8_t quality[GOAL_APP_DCE_MIRROR_QUALITY_BYTES];
    uint16_t regWords[GOAL_APP_DCE_MIRROR_REG_WORDS];
} DCE_MIRROR_SNAPSHOT_T;

/** asm 173 (PC -> RIOP), explicit Set_Attribute_Single. Local setting only:
 *  NEVER produces a Modbus request. Little endian, 8 bytes. */
typedef struct __attribute__((packed)) {
    uint16_t selSeq;    /**< increments on every change AND as a >=1 Hz heartbeat */
    uint8_t  set;       /**< 0 STATUS, 1 RUN, 2 SETUP (see dce_poll_table.h)       */
    uint8_t  flags;     /**< bit0 = poll Y[0..9] (1000..1009) in RUN                */
    uint32_t reserved;  /**< 0                                                      */
} DCE_POLLSEL_REQ_T;

/** Apply a poll-set selection. Accepted only when selSeq differs from the last
 *  accepted one (heartbeat); unknown set id is ignored. 3 s without a new selSeq
 *  -> back to SETUP. */
void dce_pollSelectorSet(uint16_t selSeq, uint8_t set, uint8_t flags);

/** G49 bench knob, carried in asm 173 `reserved`: bit31 = valid, bits 7..0 =
 *  idle gap in ms inserted after every transaction (0..100). Local RIOP timing
 *  only, never a Modbus request. reserved == 0 leaves the gap unchanged. */
void dce_pollGapSet(uint32_t reserved);

/** Fill the asm 172 payload (len bytes, 128). Safe from any task. */
void dce_fastGetPayload(uint8_t *pOut, uint16_t len);

/** Create the DCE polling task. Call once during board init. */
void dce_taskInit(void);

/** Snapshot the latest values. Safe to call from any task context. */
void dce_dataGet(DCE_DATA_T *pOut);

/** Snapshot the diagnostics. Not locked; counters are monotonic and a torn
 *  read is harmless for a bench readout. */
void dce_diagGet(DCE_DIAG_T *pOut);

/** Snapshot all mirrored DCE registers and status for EIP assembly 170. */
void dce_mirrorGetSnapshot(DCE_MIRROR_SNAPSHOT_T *pOut);

/** Handle one mailbox write request from EIP assembly 171. */
void dce_mirrorHandleWriteRequest(const DCE_MIRROR_WRITE_REQ_T *pReq);

#endif /* DCE_TASK_H */
