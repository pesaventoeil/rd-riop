/*
 * app.h - RIOP (RT1180) board glue for the NXP FreeModbus RTU master example.
 *
 * Replaces the evkmimxrt1180 board header that ships in
 *   examples/_boards/evkmimxrt1180/modbus_examples/rtu/modbus_client/
 *
 * Target: CentriFeeder DCE board, Modbus RTU slave address 1, 38400 8-N-2,
 *         2-wire RS-485 via MAX3485 (U35) on LPUART10.
 */

#ifndef _APP_H_
#define _APP_H_

#include "fsl_device_registers.h"
#include "fsl_clock.h"

/* =========================================================================
 * RS-485 / LPUART10
 *
 * Schematic (RIOP):
 *   GPIO_AD_32 (J17) -> LPUART10_TXD -> U35 pin 4  (DI)
 *   GPIO_AD_33 (J16) -> LPUART10_RXD <- U35 pin 1  (RO)
 *   GPIO_AD_35 (H17) -> LPUART10_RTS_B -> SJ6 -> U35 pin 3 (DE)
 *                                      -> SJ8 -> U35 pin 2 (/RE)
 *   R310 10K pulldown holds the transceiver in receive while the pad is Hi-Z.
 *
 * DE and /RE are tied together, so RTS high = drive, RTS low = listen.
 * The receiver is muted during transmit: there is no local echo to discard.
 * ========================================================================= */
#define DCE_LPUART                  LPUART10
#define DCE_LPUART_IRQn             LPUART10_IRQn
#define DCE_LPUART_IRQHandler       LPUART10_IRQHandler

/* LPUART9 and LPUART10 share one clock root (24 MHz, SYS_PLL3_DIV2 / 10).
 * 24000000 / 38400 = 625 = OSR 25 x SBR 25 -> exact baud, 0% error. */
#define DCE_LPUART_CLK_ROOT         kCLOCK_Root_Lpuart0910
#define DCE_LPUART_CLK_FREQ         CLOCK_GetRootClockFreq(DCE_LPUART_CLK_ROOT)

/* Names the stock example sources expect. */
#define DEMO_LPUART                 DCE_LPUART
#define DEMO_LPUART_IRQn            DCE_LPUART_IRQn
#define DEMO_LPUART_IRQHandler      DCE_LPUART_IRQHandler
#define DEMO_LPUART_CLK_FREQ        DCE_LPUART_CLK_FREQ

/* =========================================================================
 * Modbus link parameters (must match the DCE)
 *
 * MBS_SLAVE_ADDRESS   == 1        (front panel "Drop Number" 1 in Crimson)
 * MBS_SERIAL_BAUDRATE == 38400
 * MBS_PARITY          == MB_PAR_NONE
 * Two stop bits are NOT configured on the DCE; they are derived in mbs.c:
 *     ucStopBits = MB_PAR_NONE == eParity ? 2 : 1;
 * so the wire format is 8-N-2 (11 bits per character).
 * ========================================================================= */
#define DCE_MODBUS_SLAVE_ADDR       (1U)
#define DCE_MODBUS_BAUDRATE         (38400U)

/* Register block to poll: xMBRegHolding protocol addresses 504..507.
 *   504,505  usMBReg_Count        ULONG, read-only
 *   506,507  usMBReg_ReadCWratio  ULONG, read-only
 * Contiguous, so one FC03 returns both operands from the same scan -- the
 * numerator and denominator can never be torn across polls.
 * Word order is High Then Low (confirmed in the Crimson device page), and
 * each register is big-endian on the wire, so each 32-bit value is simply
 * four big-endian bytes in the response buffer. */
#define DCE_REG_COUNT_ADDR          (504U)
#define DCE_REG_BLOCK_NREGS         (4U)

/* =========================================================================
 * TPM timer for RTU timing
 *
 * TPM6 is deliberate. The six instances split across two memory mixes:
 *   TPM1 0x44310000, TPM2 0x44320000                -> AONMIX
 *   TPM3 0x424E0000 .. TPM6 0x42510000              -> WAKEUPMIX
 * LPUART10 is at 0x42D80000, also WAKEUPMIX. Keeping the timer and the UART
 * in the same mix means one TRDC instance and one domain grant to get right
 * rather than two, and AONMIX is where the CM33 boot core is most likely to
 * hold ownership.
 *
 * Of the WAKEUPMIX instances, TPM6 is the one whose clock root is confirmed
 * to exist as kCLOCK_Root_Tpm6. The TPM roots are not one-per-instance --
 * kCLOCK_Root_Tpm3 does not exist -- the same way LPUART9 and LPUART10 share
 * kCLOCK_Root_Lpuart0910.
 *
 * IRQ numbering follows the split: 36/37 for the AONMIX pair, 75-78 for
 * WAKEUPMIX, so TPM6_IRQn is 78.
 *
 * The stock example also used a second TPM to call eMBMasterPoll() from an
 * ISR. That is dropped here: eMBMasterPoll() moves into a FreeRTOS task so
 * the event layer can block, which also keeps the register callbacks out of
 * interrupt context.
 * ========================================================================= */
#define BOARD_TPM                   TPM6
#define BOARD_TPM_IRQ_NUM           TPM6_IRQn
#define BOARD_TPM_HANDLER           TPM6_IRQHandler
#define BOARD_TPM_CLK_ROOT          kCLOCK_Root_Tpm6

/* Must equal the configured root frequency. Used in _Static_assert below,
 * so it has to be a literal rather than a CLOCK_GetRootClockFreq() call. */
#define TPM_SOURCE_CLOCK_HZ         (24000000UL)
#define TPM_SOURCE_CLOCK            TPM_SOURCE_CLOCK_HZ

/* The RT1180 TPM has a 32-BIT counter: PERI_TPM.h defines CNT and MOD as
 * uint32_t with 0xFFFFFFFF masks. This is not the 16-bit TPM found on older
 * Kinetis parts. Consequences:
 *   - No need to split prescalers across timer modes; one value covers the
 *     whole range from 1750 us to seconds.
 *   - Running undivided gives the best T3.5 resolution (41.7 ns) and still
 *     leaves ~179 s of range before MOD overflows.
 *
 * fsl_tpm prescale enums are log2 of the divider; modbus_timer.c relies on
 * this when it computes (1U << tpmInfo.prescale). */
#define TPM_PRESCALE_LOG2           (0)                          /* /1 -> 24 MHz */
#define TPM_PRESCALER               kTPM_Prescale_Divide_1
#define TPM_PRESCALER_RESPOND       kTPM_Prescale_Divide_1
#define TPM_PRESCALER_CONVERT       kTPM_Prescale_Divide_1

/* T3.5 inter-frame gap. Above 19200 baud the spec fixes this at 1750 us
 * rather than computing 3.5 character times. The DCE's own RTU layer makes
 * the same >19200 distinction (mbsrtu.c:245), and so does mbrtu_m.c. */
#define TIME_PERIOD_US              (1750UL)

/* Response timeout. The 32-bit counter means Crimson's 2000 ms is reachable,
 * but there is no reason to wait that long: an FC03 response is 13 bytes,
 * about 3.7 ms at 38400. A shorter timeout just means the dce task notices a
 * silent DCE sooner. 200 ms is generous and keeps the task responsive
 * relative to its 100 ms poll period. */
#define MB_MASTER_TIMEOUT_US_RESPOND (200000UL)

/* Turnaround delay between transactions. Mirrors the 10 ms "Comms Delay"
 * the Red Lion used, which paces the half-duplex bus so we do not start
 * driving before the DCE has released its transceiver. */
#define MB_MASTER_DELAY_US_CONVERT  (10000UL)

/* ------------------------------------------------------------------------
 * Compile-time range checks against the 32-bit MOD register.
 *
 * NOTE the evaluation order: (FREQ / 1000000) * us, NOT us * FREQ. The
 * latter overflows 32-bit unsigned arithmetic for timeouts above ~178 ms
 * and would silently evaluate to garbage inside the assert.
 * ---------------------------------------------------------------------- */
#define TPM_TICKS(us, presc_log2) \
    (((TPM_SOURCE_CLOCK_HZ / 1000000UL) * (us)) / (1UL << (presc_log2)))

_Static_assert(TPM_TICKS(TIME_PERIOD_US, TPM_PRESCALE_LOG2) <= 0xFFFFFFFFUL,
               "T3.5 period overflows the 32-bit TPM counter");
_Static_assert(TPM_TICKS(TIME_PERIOD_US, TPM_PRESCALE_LOG2) >= 100UL,
               "T3.5 resolution too coarse; reduce the prescaler");
_Static_assert(TPM_TICKS(MB_MASTER_TIMEOUT_US_RESPOND, TPM_PRESCALE_LOG2) <= 0xFFFFFFFFUL,
               "Response timeout overflows the 32-bit TPM counter");
_Static_assert(TPM_TICKS(MB_MASTER_DELAY_US_CONVERT, TPM_PRESCALE_LOG2) <= 0xFFFFFFFFUL,
               "Convert delay overflows the 32-bit TPM counter");

#endif /* _APP_H_ */
