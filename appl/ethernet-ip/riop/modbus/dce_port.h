/*
 * Copyright 2025 NXP
 * RIOP: critical sections implemented.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Location: appl\ethernet-ip\riop\modbus\dce_port.h
 */

#include <stdint.h>
#include <assert.h>

#ifndef _DCE_PORT_H
#define _DCE_PORT_H

#define	INLINE                    inline
#define PR_BEGIN_EXTERN_C         extern "C" {
#define	PR_END_EXTERN_C           }

/*! \brief If Modbus ASCII support is enabled. */
#define MB_ASCII_ENABLED                        (  0 )

 /*! \brief If Modbus Master RTU support is enabled. */
 #define MB_MASTER_RTU_ENABLED                  (  1 )

/*! \brief If Modbus RTU support is enabled. */
#define MB_RTU_ENABLED                          (  0 )

/*! \brief If Modbus TCP support is enabled. */
#define MB_TCP_ENABLED                          (  0 )

/*! \brief Enable the close hook so vMBMasterPortClose() is actually called.
 * Without this, mb_m.c sets pvMBMasterFrameCloseCur = NULL and the LPUART
 * teardown is dead code. */
#define MB_PORT_HAS_CLOSE                       (  1 )

/*! \brief The total slaves in Modbus Master system. Default 16.
 * \note : The slave ID must be continuous from 1.
 *
 * The DCE is the only device on this segment, at address 1. Lowering this
 * shrinks the master's per-slave bookkeeping, but some bounds checks in the
 * stack compare against it inclusively, so leave a little margin rather
 * than dropping straight to 1. */
#define MB_MASTER_TOTAL_SLAVE_NUM               (  2 )

/* ------------------------------------------------------------------------
 * Critical sections.
 *
 * The stock file left these empty, which is survivable in a bare-metal demo
 * and is not survivable here. The RTU state machine runs inside the LPUART
 * ISR (pxMBMasterFrameCBByteReceived), and it posts events that the poll
 * task consumes -- so xMBMasterPortEventPost's read-modify-write on the
 * event flags is a genuine race against xMBMasterPortEventGet.
 *
 * These CANNOT be taskENTER_CRITICAL()/taskEXIT_CRITICAL(). Those are not
 * safe to call from an ISR, and the stack calls this macro from both
 * contexts. PRIMASK save/restore with a nesting count is safe from either,
 * and the guarded regions are only a few instructions long.
 * ---------------------------------------------------------------------- */
void vMBPortEnterCritical(void);
void vMBPortExitCritical(void);

#define ENTER_CRITICAL_SECTION( )   vMBPortEnterCritical( )
#define EXIT_CRITICAL_SECTION( )    vMBPortExitCritical( )

typedef int8_t    BOOL;

typedef uint8_t   UCHAR;
typedef int8_t    CHAR;

typedef uint16_t  USHORT;
typedef int16_t   SHORT;

typedef uint32_t  ULONG;
typedef int32_t   LONG;

#ifndef TRUE
#define TRUE      1
#endif

#ifndef FALSE
#define FALSE     0
#endif

#endif /* _DCE_PORT_H */