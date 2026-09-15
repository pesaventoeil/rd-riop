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

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

#include "fsl_lpuart.h"
#include "fsl_clock.h"
#include "clock_config.h"
#include "dce_app.h"
#include "port.h"

/* ----------------------- static functions ---------------------------------*/

static void prvvUARTTxReadyISR( void );
static void prvvUARTRxISR( void );

static bool RxIsrFlag = 0;
static bool TxIsrFlag = 0;

void DEMO_LPUART_IRQHandler(void)
{
    if (RxIsrFlag) {
        prvvUARTRxISR();
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

    EnableIRQ(DEMO_LPUART_IRQn);
    return TRUE;
}

/* ----------------------- Start implementation -----------------------------*/
void
vMBMasterPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
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
    LPUART_WriteByte(DEMO_LPUART, ucByte);
    return 1;
}

BOOL
xMBMasterPortSerialGetByte( CHAR * pucByte )
{
    *pucByte = LPUART_ReadByte(DEMO_LPUART);
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
