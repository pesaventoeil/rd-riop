/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef __CC_H__
#define __CC_H__

#if GOAL_CONFIG_TCPIP_BSD_API == 1
/* disable inclusion of lwIP socket header */
#define LWIP_HDR_SOCKETS_H
#endif /* GOAL_CONFIG_TCPIP_BSD_API */

#include <goal_includes.h>

#if GOAL_CONFIG_TCPIP_BSD_API == 1
/* reenable inclusion of lwIP socket header */
#undef LWIP_HDR_SOCKETS_H
#endif /* GOAL_CONFIG_TCPIP_BSD_API */


/* configure assert */
#if GOAL_FEAT_LWIP_ASSERT == 0
#  define LWIP_NOASSERT
#endif


/** additional defintion of datatype bool */
#ifndef bool
typedef enum {
    false = 0,                                  /**< false */
    true = 1                                    /**< true */
} bool;
#endif

typedef uint8_t u8_t;
typedef int8_t s8_t;
typedef uint16_t u16_t;
typedef int16_t s16_t;
typedef uint32_t u32_t;
typedef int32_t s32_t;
typedef PtrCast mem_ptr_t;
typedef s32_t sys_prot_t;

/** tell lwIP that integers have already been defined here */
#define LWIP_NO_STDINT_H 1                      /**< disable redefinition of integers by lwIP */

#define U16_F "hu"
#define S16_F "d"
#define X16_F "hx"
#define U32_F "u"
#define S32_F "d"
#define X32_F "x"
#define SZT_F "uz"


/* define compiler specific symbols */
#define PACK_STRUCT_BEGIN         GOAL_TARGET_PACKED_STRUCT_PRE
#define PACK_STRUCT_STRUCT        GOAL_TARGET_PACKED
#define PACK_STRUCT_END           GOAL_TARGET_PACKED_STRUCT_POST
#define PACK_STRUCT_FIELD(x)      x
/* needed to avoid errors by script "check_packing": GOAL_TARGET_PACKED_PRE */

/* assert */
#define LWIP_PLATFORM_ASSERT(x)   do { goal_logErr("Assert: " x); goal_targetHalt(); } while (0)

/* diagnostic output */
#define LWIP_PLATFORM_DIAG(...)   goal_logDbg(__VA_ARGS__)

/* endianness */
#ifndef BYTE_ORDER
#  if GOAL_CONFIG_TARGET_LITTLE_ENDIAN == 1
#    define BYTE_ORDER             LITTLE_ENDIAN
#  else
#    define BYTE_ORDER             BIG_ENDIAN
#  endif
#endif

#endif /* __CC_H__ */
