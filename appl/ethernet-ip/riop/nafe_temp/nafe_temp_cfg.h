/* RIOP gateway build switches for the NAFE temperature path (included by temp_eip.h when RIOP=1). */
#ifndef NAFE_TEMP_CFG_H
#define NAFE_TEMP_CFG_H
#ifndef DEBUG_NAFE
#define DEBUG_NAFE 1          /* dev/bench build: asm 174 debug block + asm 175 enabled. Release: 0 */
#endif
#ifndef NAFE_TEMP_DEFAULT_PROFILE
#define NAFE_TEMP_DEFAULT_PROFILE 4 /* TEMP_PROF_BENCH0_NOEXC: no excitation on any pin */
#endif
/* N7: 1 = create the NAFE temp task; 0 = asm 174 header only (bisect build, no SPI traffic). */
#ifndef NAFE_TEMP_TASK_ENABLE
#define NAFE_TEMP_TASK_ENABLE 1
#endif
/* N7: FreeRTOS priority 0 (tskIDLE_PRIORITY). On this M7 the GOAL Main loop, which carries the whole
 * CM33<->M7 MCTC/RPC link (every EIP explicit request for asm 170-175), runs at FreeRTOS priority 0
 * (GOAL prio 1 * (11-1)/100 = 0) and never blocks. Any ready task above 0 starves it. At priority 0 with
 * time slicing (FreeRTOS default) this task round-robins with GOAL Main instead of preempting it.
 * N1-N6 used 2, which let one stuck SPI busy-wait stop all EIP explicit messaging. */
#define NAFE_TEMP_TASK_PRIORITY 0u
#define NAFE_TEMP_TASK_STACK_WORDS 1536u   /* static (DTCM .bss), not from the 180 KB FreeRTOS/GOAL heap */
#define NAFE_TEMP_START_DELAY_MS 10000u    /* let GOAL/MCTC sync and the DCE master settle first */
#define NAFE_TEMP_SPI_SPIN_MAX 100000u     /* bounded LPSPI flag wait (~1-3 ms); NXP HAL waits forever */
#define NAFE_TEMP_FAULT_BACKOFF_MS 10000u  /* after an SPI timeout or lost bus, retry the scan this much later */
#endif
