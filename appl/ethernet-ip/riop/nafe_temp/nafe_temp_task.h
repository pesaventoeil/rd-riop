#ifndef NAFE_TEMP_TASK_H
#define NAFE_TEMP_TASK_H
#include <stdint.h>
#include "dce_ng/temp_eip.h"
void nafe_tempTaskInit(void);
/* EIP callbacks (GOAL task context): copy-only, never touch SPI */
void nafe_tempGet174(uint8_t *buf, uint32_t len);
void nafe_tempPost175(const uint8_t *buf, uint32_t len);
void nafe_tempGet176(uint8_t *buf, uint32_t len); /* N9 asm 176 scan log */
#endif
