/* RIOP gateway build switches for the NAFE temperature path (included by temp_eip.h when RIOP=1). */
#ifndef NAFE_TEMP_CFG_H
#define NAFE_TEMP_CFG_H
#ifndef DEBUG_NAFE
#define DEBUG_NAFE 1          /* dev/bench build: asm 174 debug block + asm 175 enabled. Release: 0 */
#endif
#ifndef NAFE_TEMP_DEFAULT_PROFILE
#define NAFE_TEMP_DEFAULT_PROFILE 4 /* TEMP_PROF_BENCH0_NOEXC: no excitation on any pin */
#endif
#define NAFE_TEMP_TASK_PRIORITY 2u  /* below dce (4), mbpoll (5), GOAL default (5), AFE_task (7) */
#define NAFE_TEMP_TASK_STACK_WORDS 1536u
#endif
