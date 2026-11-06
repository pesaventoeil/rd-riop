/*
 * Copyright 2025 NXP
 * RIOP retarget: LPUART10 + MAX3485, 8-N-2, hardware RS-485 driver enable.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Changes from the stock evkmimxrt1180 version:
 *   1. Honours ucDataBits / eParity instead of discarding them, and defaults
 *      to TWO stop bits. The stock file left LPUART_GetDefaultConfig()'s one
 *      stop bit in place, giving a 10-bit character where the DCE expects 11.
 *   2. Enables MODIR[TXRTSE] so the LPUART drives DE//RE with exact frame
 *      timing. Never toggle DE from software on a TDRE flag: the driver
 *      drops before the last character clears the shift register and the CRC
 *      is truncated on the wire.
 *   3. vMBMasterPortClose() actually shuts the peripheral down.
 */

#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

#include "fsl_lpuart.h"
#include "fsl_clock.h"
#include "fsl_iomuxc.h"
#include "clock_config.h"
#include "pin_mux.h"
#include "dce_app.h"
#include "port.h"

/* ----------------------- static functions ---------------------------------*/

static void prvvUARTTxReadyISR( void );
static void prvvUARTRxISR( void );

DCE_UART_DIAG_T dceUartDiag;
DCE_TIMING_T dceTiming;
static bool RxIsrFlag = 0;

/* G46: leading-byte filter.
 * Bench evidence (G44/G45 diag): right after the request is sent, a stray byte
 * (0xFF) shows up on LPUART10 RX. FreeModbus treats ANY first byte as the start
 * of the slave's reply: it cancels the response timeout, runs T3.5, finds a
 * 1-byte "frame" and fails the request ~4 ms after it started. The DCE's real
 * reply (5+2N bytes, complete, no framing errors) arrives a few ms later and is
 * thrown away; the next request then collides with the tail of it.
 * A valid reply always starts with the slave address, so drop any first byte
 * that is not the DCE address without telling the stack. */
static volatile bool    s_rxMidFrame = false;
static volatile uint8_t s_pendByte   = 0u;

/* Called from the TPM6 ISR whenever any Modbus timer (T3.5 / respond) expires. */
void dceRxFrameEnd(void)
{
    s_rxMidFrame = false;
}
static bool TxIsrFlag = 0;

void DEMO_LPUART_IRQHandler(void)
{
    if (RxIsrFlag) {
        uint32_t st = LPUART_GetStatusFlags(DEMO_LPUART);
        if (st & kLPUART_FramingErrorFlag)  { dceUartDiag.rxFE++; }
        if (st & kLPUART_NoiseErrorFlag)    { dceUartDiag.rxNF++; }
        if (st & kLPUART_RxOverrunFlag)     { dceUartDiag.rxOR++; }
        LPUART_ClearStatusFlags(DEMO_LPUART, st & (kLPUART_FramingErrorFlag | kLPUART_NoiseErrorFlag |
                                                   kLPUART_RxOverrunFlag | kLPUART_ParityErrorFlag));
        if (st & kLPUART_RxDataRegFullFlag) {
            uint8_t b = (uint8_t)LPUART_ReadByte(DEMO_LPUART);
            if (dceUartDiag.nFirst < 16u) {
                dceUartDiag.first[dceUartDiag.nFirst] = b;
                dceUartDiag.nFirst++;
            }
            dceUartDiag.rxBytes++;
            if (!s_rxMidFrame && (b != (uint8_t)DCE_MODBUS_SLAVE_ADDR)) {
                dceUartDiag.rxDropped++;
            } else {
                if (!s_rxMidFrame && !dceTiming.rxFirstValid) {
                    dceTiming.rxFirstTick  = (uint32_t)xTaskGetTickCountFromISR();
                    dceTiming.rxFirstValid = 1u;
                }
                if (dceUartDiag.nAcc < 4u) {
                    dceUartDiag.acc[dceUartDiag.nAcc] = b;
                }
                dceUartDiag.nAcc++;
                s_rxMidFrame = true;
                s_pendByte = b;
                prvvUARTRxISR();
            }
        }
        LPUART_ClearStatusFlags(DEMO_LPUART, kLPUART_RxDataRegFullFlag);
    }

    if (TxIsrFlag) {
        prvvUARTTxReadyISR();
        LPUART_ClearStatusFlags(DEMO_LPUART, kLPUART_TransmissionCompleteFlag);
    }

    SDK_ISR_EXIT_BARRIER;
}

static BOOL UartInit(uint32_t ulBaudRate, UCHAR ucDataBits, eMBParity eParity)
{
    lpuart_config_t config;

    LPUART_GetDefaultConfig(&config);
    config.baudRate_Bps = ulBaudRate;
    config.enableTx     = true;
    config.enableRx     = true;

    switch (eParity) {
        case MB_PAR_NONE:
            config.parityMode = kLPUART_ParityDisabled;
            /* Modbus RTU keeps an 11-bit character when parity is disabled,
             * so the missing parity bit is replaced by a second stop bit.
             * The DCE derives exactly this in mbs.c:842. */
            config.stopBitCount = kLPUART_TwoStopBit;
            break;
        case MB_PAR_EVEN:
            config.parityMode   = kLPUART_ParityEven;
            config.stopBitCount = kLPUART_OneStopBit;
            break;
        case MB_PAR_ODD:
            config.parityMode   = kLPUART_ParityOdd;
            config.stopBitCount = kLPUART_OneStopBit;
            break;
        default:
            return FALSE;
    }

    /* This LPUART offers only kLPUART_SevenDataBits and
     * kLPUART_EightDataBits -- there is no 9-bit mode, so with parity enabled
     * the parity bit would have to be carried inside the 8 data bits. The DCE
     * link is 8-N-2 and never uses parity, so 8 data bits is the only case
     * that has to work here. */
    if (ucDataBits != 8) {
        return FALSE;
    }
    config.dataBitsCount = kLPUART_EightDataBits;

    /* Enable the LPUART10 peripheral clock gate. The clock ROOT is configured
     * in clock_config.c, but the LPCG must also be on or the first register
     * access faults. */
    CLOCK_EnableClock(kCLOCK_Lpuart10);

    /* Mux only the LPUART10 pins (same settings as common/m7/board/pin_mux.c).
     * Do not call BOARD_InitBootPins() here: it also re-muxes LPUART1 and
     * GPIO_AD_27, which the M33 owns. */
    CLOCK_EnableClock(kCLOCK_Iomuxc1);
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_32_LPUART10_TX, 0U);
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_33_LPUART10_RX, 1U);
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_35_LPUART10_RTS_B, 0U);

    if (kStatus_Success != LPUART_Init(DEMO_LPUART, &config, DEMO_LPUART_CLK_FREQ)) {
        return FALSE;
    }

    /* Hardware RS-485 driver enable on the RTS pin (GPIO_AD_35 -> U35 DE//RE).
     * TXRTSE hands DE timing to the peripheral; TXRTSPOL selects the active
     * level. The net is named RTS_B by NXP convention, but the MAX3485 needs
     * DE HIGH to transmit, hence polarity set.
     *
     * >>> SCOPE THIS ON FIRST POWER-UP <<<
     * DE must sit LOW at idle and go HIGH for exactly the outbound frame. If
     * it is inverted, clear TXRTSPOL. */
    DEMO_LPUART->MODIR |= LPUART_MODIR_TXRTSE_MASK | LPUART_MODIR_TXRTSPOL_MASK;

    /* Reset priority 0 is above configMAX_SYSCALL_INTERRUPT_PRIORITY and
     * asserts in vPortValidateInterruptPriority. See DCE_MB_IRQ_PRIO. */
    NVIC_SetPriority(DEMO_LPUART_IRQn, DCE_MB_IRQ_PRIO);
    EnableIRQ(DEMO_LPUART_IRQn);
    return TRUE;
}

/* ----------------------- Start implementation -----------------------------*/
void
vMBMasterPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    /* G48: TX finished (driver released, receiver on) -> stamp for the reply-latency measurement. */
    if (xRxEnable && !xTxEnable && !dceTiming.txDoneValid && !dceTiming.rxFirstValid) {
        dceTiming.txDoneTick  = (uint32_t)xTaskGetTickCount();
        dceTiming.txDoneValid = 1u;
    }

    /* If xRXEnable enable serial receive interrupts. */
    if (xRxEnable) {
        RxIsrFlag = 1;
        LPUART_EnableInterrupts(DEMO_LPUART, kLPUART_RxDataRegFullInterruptEnable);
    } else {
        RxIsrFlag = 0;
        LPUART_DisableInterrupts(DEMO_LPUART, kLPUART_RxDataRegFullInterruptEnable);
    }
    LPUART_EnableRx(DEMO_LPUART, xRxEnable);

    /* Transmission-complete (not TDRE) is the correct trigger here: it fires
     * after the shift register empties, which is what hardware DE release is
     * synchronised to. Do not "optimise" this to TDRE. */
    if (xTxEnable) {
        TxIsrFlag = 1;
        LPUART_EnableInterrupts(DEMO_LPUART, kLPUART_TransmissionCompleteInterruptEnable);
    } else {
        TxIsrFlag = 0;
        LPUART_DisableInterrupts(DEMO_LPUART, kLPUART_TransmissionCompleteInterruptEnable);
    }
    LPUART_EnableTx(DEMO_LPUART, xTxEnable);
}

BOOL
xMBMasterPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
    (void)ucPORT;
    return UartInit(ulBaudRate, ucDataBits, eParity);
}

void vMBMasterPortClose(void)
{
    DisableIRQ(DEMO_LPUART_IRQn);
    LPUART_DisableInterrupts(DEMO_LPUART,
                             kLPUART_RxDataRegFullInterruptEnable |
                             kLPUART_TransmissionCompleteInterruptEnable);
    RxIsrFlag = 0;
    TxIsrFlag = 0;
    LPUART_Deinit(DEMO_LPUART);
}

BOOL
xMBMasterPortSerialPutByte( CHAR ucByte )
{
    dceUartDiag.txBytes++;
    s_rxMidFrame = false;
    LPUART_WriteByte(DEMO_LPUART, ucByte);
    return 1;
}

BOOL
xMBMasterPortSerialGetByte( CHAR * pucByte )
{
    *pucByte = (CHAR)s_pendByte;   /* byte already read (and filtered) by the ISR */
    return 1;
}

static void prvvUARTTxReadyISR( void )
{
  pxMBMasterFrameCBTransmitterEmpty(  );
}

static void prvvUARTRxISR( void )
{
  pxMBMasterFrameCBByteReceived(  );
}
