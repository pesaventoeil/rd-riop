/*
* Copyright 2025-2026 NXP
*
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly
* in accordance with the applicable license terms.  By expressly accepting such terms or by downloading,
* installing, activating and/or otherwise using the software, you are agreeing that you have read,
* and that you agree to comply with and are bound by, such license terms.  If you do not agree to be bound by
* the applicable license terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "FreeRTOS.h"
#include "task.h"

#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "mcmgr.h"
#include "flexspi_ops.h"

#include "goal_includes.h"
int BOARD_InitHardware(void)
{
    /* Init board hardware.*/
    BOARD_ConfigMPU();
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();
    flexspi_init();
    return 0;
}

static int Reset_PHY(void)
{
    PRINTF("Reset ETH4 PHY?\r\n");
    RGPIO_PinWrite(NETC_ETH4_RESET_GPIO, NETC_ETH4_RESET_GPIO_PIN, 0);
    SDK_DelayAtLeastUs(150000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
    RGPIO_PinWrite(NETC_ETH4_RESET_GPIO, NETC_ETH4_RESET_GPIO_PIN, 1);
    SDK_DelayAtLeastUs(150000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
    return 0;
}

int riop_appl_init(void)
{
    PRINTF("RIOP Application Initialization..\r\n");
    Reset_PHY();
    //Network_Init(CLOCK_GetRootClockFreq(kCLOCK_Root_Netc)); // TODO
    return 0;
}
