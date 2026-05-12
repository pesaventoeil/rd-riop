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
#include "goal_eip.h"

int riop_appl_init(void)
{
    PRINTF("RIOP Application Initialization..\r\n");
    return 0;
}

int BOARD_InitHardware(void)
{
    /* Init board hardware.*/
    BOARD_ConfigMPU();
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();
    flexspi_init();

#ifndef BUILD_NAME
#define BUILD_NAME "ETHERNET/IP"
#endif

    PRINTF("Start the digital_io example. (" BUILD_NAME ") [" __DATE__ " " __TIME__ "]\r\n");

    return 0;
}

