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
#include "fsl_clock.h"
#include "fsl_iomuxc.h"
#include "fsl_rgpio.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "mcmgr.h"
#include "flexspi_ops.h"

#include "goal_includes.h"
#include "goal_eip.h"

/* Keep in sync with appl/ethernet-ip/riop/dce_task.h (M33 may not include that). */
#ifndef DCE_GPIO_BITBANG_TEST
#define DCE_GPIO_BITBANG_TEST   0
#endif
#ifndef DCE_GPIO_BITBANG_ON_M33
#define DCE_GPIO_BITBANG_ON_M33 0
#endif

#if DCE_GPIO_BITBANG_TEST && DCE_GPIO_BITBANG_ON_M33
/* MCU → U35: AD_32=DI, AD_35=DE//RE, AD_33=RO (input only).
 * 1 Hz in-phase square for LabJack AIN0/AIN1 (expect ~0 / ~3.3 V). */
#define DCE_BB_GPIO           RGPIO5
#define DCE_BB_PIN_DI         0u  /* GPIO5_IO00 = GPIO_AD_32 */
#define DCE_BB_PIN_RO         1u  /* GPIO5_IO01 = GPIO_AD_33 */
#define DCE_BB_PIN_DE         3u  /* GPIO5_IO03 = GPIO_AD_35 */
#define DCE_BB_HALF_PERIOD_MS 500u
#define DCE_BB_PAD_CFG        0x06u

static void dce_m33_gpioBitbangTask(void *arg)
{
    rgpio_pin_config_t outCfg = { .pinDirection = kRGPIO_DigitalOutput, .outputLogic = 0u };
    rgpio_pin_config_t inCfg  = { .pinDirection = kRGPIO_DigitalInput,  .outputLogic = 0u };
    (void)arg;

    CLOCK_EnableClock(kCLOCK_Iomuxc1);
    CLOCK_EnableClock(kCLOCK_Gpio5);

    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_32_GPIO5_IO00, 0U);
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_33_GPIO5_IO01, 1U);
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_35_GPIO5_IO03, 0U);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_32_GPIO5_IO00, DCE_BB_PAD_CFG);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_35_GPIO5_IO03, DCE_BB_PAD_CFG);

    RGPIO_PinInit(DCE_BB_GPIO, DCE_BB_PIN_DI, &outCfg);
    RGPIO_PinInit(DCE_BB_GPIO, DCE_BB_PIN_DE, &outCfg);
    RGPIO_PinInit(DCE_BB_GPIO, DCE_BB_PIN_RO, &inCfg);

    PRINTF("DCE GPIO bitbang on M33: AD_32/DI + AD_35/DE @ 1 Hz\r\n");

    for (;;) {
        RGPIO_PinWrite(DCE_BB_GPIO, DCE_BB_PIN_DE, 1U);
        RGPIO_PinWrite(DCE_BB_GPIO, DCE_BB_PIN_DI, 1U);
        vTaskDelay(pdMS_TO_TICKS(DCE_BB_HALF_PERIOD_MS));
        RGPIO_PinWrite(DCE_BB_GPIO, DCE_BB_PIN_DE, 0U);
        RGPIO_PinWrite(DCE_BB_GPIO, DCE_BB_PIN_DI, 0U);
        vTaskDelay(pdMS_TO_TICKS(DCE_BB_HALF_PERIOD_MS));
    }
}

static void dce_m33_gpioBitbangStart(void)
{
    if (pdPASS != xTaskCreate(dce_m33_gpioBitbangTask, "dce_bb",
                              configMINIMAL_STACK_SIZE + 256, NULL, 4u, NULL)) {
        PRINTF("DCE GPIO bitbang: xTaskCreate FAILED\r\n");
    }
}
#endif /* DCE_GPIO_BITBANG_TEST && DCE_GPIO_BITBANG_ON_M33 */

int riop_appl_init(void)
{
    /* Note: EIP GOAL calls riop_appl_init from the M7 image (common/m7/init.c).
     * This M33 copy is not the Modbus/GOAL path — bitbang starts in BOARD_InitHardware. */
    PRINTF("RIOP Application Initialization..\r\n");
    return 0;
}

extern void riop_diag_m33_start(void);

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

#if DCE_GPIO_BITBANG_TEST && DCE_GPIO_BITBANG_ON_M33
    /* xTaskCreate before the scheduler is fine; task runs after vTaskStartScheduler. */
    dce_m33_gpioBitbangStart();
#endif

    riop_diag_m33_start();
    return 0;
}
