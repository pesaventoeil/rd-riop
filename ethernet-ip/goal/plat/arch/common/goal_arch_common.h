/** @file
 *
 * @brief Generic Compiler/OS Definitions
 *
 * All commonly used Compiler/OS specific settings are included here.
 *
 * @copyright
 * Copyright 2020 port GmbH Halle/Saale.
 * This software is protected Intellectual Property and may only be used
 * according to the license agreement.
 *
 * Copyright 2025 NXP
 *
 * NXP Confidential and Proprietary. This software is owned or controlled by NXP
 * and may only be used strictly in accordance with the applicable license
 * terms. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you
 * have read, and that you agree to comply with and are bound by, such license
 * terms. If you do not agree to be bound by the applicable license terms, then
 * you may not retain, install, activate or otherwise use the software.
 */

#ifndef GOAL_ARCH_COMMON_H
#define GOAL_ARCH_COMMON_H


/****************************************************************************/
/* Headers */
/****************************************************************************/
#include <limits.h>


/****************************************************************************/
/* Helper macros */
/****************************************************************************/
#define STR_META(x) #x
#define STR(x) STR_META(x)


/****************************************************************************/
/* Standard command mappings */
/****************************************************************************/
#ifndef GOAL_MEMCPY
#  define GOAL_MEMCPY                   memcpy
#endif

#ifndef GOAL_MEMSET
#  define GOAL_MEMSET                   memset
#endif

#ifndef GOAL_MEMCMP
#  define GOAL_MEMCMP                   memcmp
#endif

#ifndef GOAL_MEMMOVE
#  define GOAL_MEMMOVE                  memmove
#endif

#ifndef GOAL_STRCMP
#  define GOAL_STRCMP                   strcmp
#endif

#ifndef GOAL_STRNCMP
#  define GOAL_STRNCMP                  strncmp
#endif

#ifndef GOAL_STRLEN
#  define GOAL_STRLEN                   strlen
#endif

#ifndef GOAL_STRNLEN
#  define GOAL_STRNLEN                  strnlen
#endif

#ifndef GOAL_STRNCPY
#  define GOAL_STRNCPY                  strncpy
#endif

#ifndef GOAL_ASSERT
#  define GOAL_ASSERT(x)                { if (!(x)) { goal_logErr("Assert: %s", #x); goal_targetHalt(); } }
#endif

#ifndef GOAL_SNPRINTF
#  define GOAL_SNPRINTF                 snprintf
#endif

#ifndef GOAL_VSNPRINTF
#  define GOAL_VSNPRINTF                vsnprintf
#endif

#ifndef GOAL_ISALNUM
#  define GOAL_ISALNUM                  isalnum
#endif

#ifndef GOAL_ISDIGIT
#  define GOAL_ISDIGIT                  isdigit
#endif

#ifndef GOAL_STRSTR
#  include <string.h>
#  define GOAL_STRSTR                   strstr
#endif

#ifndef GOAL_STRTOL
#  include <stdlib.h>
#  define GOAL_STRTOL                   strtol
#endif

#ifndef GOAL_STRTOUL
#  include <stdlib.h>
#  define GOAL_STRTOUL                  strtoul
#endif

#ifndef GOAL_ERRNO
#  include <errno.h>
#  define GOAL_ERRNO                    errno
#endif

#ifndef GOAL_VA_START
#  include <stdarg.h>
#  define GOAL_VA_START                 va_start
#  define GOAL_VA_END                   va_end
#endif

#ifndef GOAL_SSCANF
#  include <stdio.h>
#  define GOAL_SSCANF                   sscanf
#endif


/****************************************************************************/
/* Format defines */
/****************************************************************************/
#ifndef FMT_d32
#  define FMT_d32       PRId32
#endif

#ifndef FMT_i32
#  define FMT_i32       PRIi32
#endif

#ifndef FMT_u32
#  define FMT_u32       PRIu32
#endif

#ifndef FMT_x32
#  define FMT_x32       PRIx32
#endif

#ifndef FMT_d64
#  define FMT_d64       PRId64
#endif

#ifndef FMT_i64
#  define FMT_i64       PRIi64
#endif

#ifndef FMT_u64
#  define FMT_u64       PRIu64
#endif

#ifndef FMT_x64
#  define FMT_x64       PRIx64
#endif

#ifndef FMT_size_t
#  define FMT_size_t    "zd"
#endif

#ifndef FMT_ptr
#  define FMT_ptr       "p"
#endif

#ifndef FMT_ptrdiff
#  define FMT_ptrdiff   "td"
#endif


/****************************************************************************/
/* Alignment */
/****************************************************************************/
#ifndef GOAL_TARGET_MEM_ALIGN_CPU
#  define GOAL_TARGET_MEM_ALIGN_CPU     4
#endif

#ifndef GOAL_TARGET_MEM_ALIGN_NET
#  define GOAL_TARGET_MEM_ALIGN_NET     4
#endif

#ifndef GOAL_TARGET_MEM_ALIGN_STACK
#  define GOAL_TARGET_MEM_ALIGN_STACK   8
#endif


/****************************************************************************/
/* Endianness */
/****************************************************************************/
#ifndef GOAL_CONFIG_TARGET_LITTLE_ENDIAN
#  define GOAL_CONFIG_TARGET_LITTLE_ENDIAN 0
#endif

#ifndef GOAL_CONFIG_TARGET_BIG_ENDIAN
#  define GOAL_CONFIG_TARGET_BIG_ENDIAN 0
#endif

#if (GOAL_CONFIG_TARGET_LITTLE_ENDIAN == 0) && (GOAL_CONFIG_TARGET_BIG_ENDIAN == 0)
#  ifdef __BYTE_ORDER
#    if __BYTE_ORDER == __LITTLE_ENDIAN
#      undef GOAL_CONFIG_TARGET_LITTLE_ENDIAN
#      define GOAL_CONFIG_TARGET_LITTLE_ENDIAN 1
#    elif __BYTE_ORDER == __BIG_ENDIAN
#      undef GOAL_CONFIG_TARGET_BIG_ENDIAN
#      define GOAL_CONFIG_TARGET_BIG_ENDIAN 1
#    endif
#  elif defined(__BYTE_ORDER__)
#    if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#      undef GOAL_CONFIG_TARGET_LITTLE_ENDIAN
#      define GOAL_CONFIG_TARGET_LITTLE_ENDIAN 1
#    elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#      undef GOAL_CONFIG_TARGET_BIG_ENDIAN
#      define GOAL_CONFIG_TARGET_BIG_ENDIAN 1
#    endif
#  endif
#endif

#if (GOAL_CONFIG_TARGET_LITTLE_ENDIAN == 0) && (GOAL_CONFIG_TARGET_BIG_ENDIAN == 0)
#  error "Byte order not detected and not set."
#endif


/****************************************************************************/
/* Compiler specific defines
 *
 *   GOAL_TARGET_CC_FORMAT_PRINTF_PRE   - printf-format attribute before definition
 *   GOAL_TARGET_CC_FORMAT_PRINTF       - printf-format attribute after definition
 *   GOAL_TARGET_INLINE                 - inline attribute
 *   GOAL_TARGET_NORETURN               - no-return attribute after definition
 *   GOAL_TARGET_NORETURN_PRE           - no-return attribute before definition
 *   GOAL_TARGET_PACKED                 - packed attribute before typedef name
 *   GOAL_TARGET_PACKED_PRE             - packed attribute before struct keyword,
 *                                        but after typedef keyword
 *   GOAL_TARGET_PACKED_STRUCT_PRE      - packed attribute before struct definition
 *   GOAL_TARGET_PACKED_STRUCT_POST     - packed attribute after struct definition
 *
 *   Struct example:
 *   GOAL_TARGET_PACKED_STRUCT_PRE
 *   typedef GOAL_TARGET_PACKED_PRE struct NAME_OF_STRUCT {
 *       ...
 *   } GOAL_TARGET_PACKED NAME_OF_STRUCT;
 *   GOAL_TARGET_PACKED_STRUCT_POST
 *
 * Hint: for new compiler defines see http://sourceforge.net/p/predef/wiki/Compilers
 */
/****************************************************************************/
/* GNU C Compiler */
#if defined(__GNUC__)
#  define GOAL_TARGET_CC_FORMAT_PRINTF      __attribute__((format(printf, 4, 5)))
#  define GOAL_TARGET_CC_FORMAT_PRINTF_PRE
#  define GOAL_TARGET_CC_FORMAT_SCANF       __attribute__((format(scanf, 2, 3)))
#  define GOAL_TARGET_CC_FORMAT_SCANF_PRE
#  define GOAL_TARGET_INLINE                inline
#  define GOAL_TARGET_NORETURN              __attribute__((noreturn))
#  define GOAL_TARGET_NORETURN_PRE
#  define GOAL_TARGET_PACKED                __attribute__((packed))
#  define GOAL_TARGET_PACKED_PRE
#  define GOAL_TARGET_PACKED_STRUCT_PRE
#  define GOAL_TARGET_PACKED_STRUCT_POST

/* handle fallthrough for all clang versions */
#  if defined(__clang__)
#    if defined(__has_attribute)
#      if __has_attribute(fallthrough)
#        define GOAL_TARGET_FALLTHROUGH     __attribute__((fallthrough))
#      endif
#    endif
#  endif

#  if (7 <= __GNUC__)
#    define GOAL_TARGET_FALLTHROUGH         __attribute__((fallthrough))
#  endif

#  ifndef GOAL_TARGET_FALLTHROUGH
#    define GOAL_TARGET_FALLTHROUGH         /* fallthrough */
#  endif

#  define GOAL_TARGET_WEAK                  __attribute__((weak))
#  if defined(__GNUC_PATCHLEVEL__)
#    define GOAL_TARGET_VERSION             "GCC C/C++ V" STR(__GNUC__) "." STR(__GNUC_MINOR__) "." STR(__GNUC_PATCHLEVEL__)
#  else
#    define GOAL_TARGET_VERSION             "GCC C/C++ V" STR(__GNUC__) "." STR(__GNUC_MINOR__)
#  endif
#  define GOAL_TARGET_NOT_INITIALIZED       __attribute__((section("non_initialized")))

/* IAR Compiler */
#elif defined(__IAR_SYSTEMS_ICC__)
#  define GOAL_TARGET_CC_FORMAT_PRINTF
#  define GOAL_TARGET_CC_FORMAT_PRINTF_PRE  _Pragma("__printf_args")
#  define GOAL_TARGET_CC_FORMAT_SCANF
#  define GOAL_TARGET_CC_FORMAT_SCANF_PRE   _Pragma("__scanf_args")
#  define GOAL_TARGET_INLINE                inline
#  define GOAL_TARGET_NORETURN
#  define GOAL_TARGET_NORETURN_PRE          __noreturn
#  define GOAL_TARGET_PACKED
#  define GOAL_TARGET_PACKED_PRE            __packed
#  define GOAL_TARGET_PACKED_STRUCT_PRE
#  define GOAL_TARGET_PACKED_STRUCT_POST
#  define GOAL_TARGET_FALLTHROUGH
#  define GOAL_TARGET_VERSION               "IAR C/C++ V" STR(__VER__)
#  define GOAL_TARGET_WEAK                  __weak
#  define GOAL_TARGET_NOT_INITIALIZED       __attribute__((section(".noinit")))

#  if GOAL_CONFIG_TGT_TRACE == 1
#    if GOAL_CONFIG_TGT_TRACE_ITM == 1
#      if GOAL_CONFIG_TGT_TRACE_ITM_WITHOUT_PC == 1 && defined(__ICCARM__)
#        include <arm_itm.h>
#        define GOAL_TGT_TRACE8(_chan, _data)     ITM_EVENT8(_chan, _data)
#        define GOAL_TGT_TRACE16(_chan, _data)    ITM_EVENT16(_chan, _data)
#        define GOAL_TGT_TRACE32(_chan, _data)    ITM_EVENT32(_chan, _data)
#      else
#        define GOAL_TGT_TRACE8(_chan, _data)     ITM_EVENT8_WITH_PC(_chan, _data)
#        define GOAL_TGT_TRACE16(_chan, _data)    ITM_EVENT16_WITH_PC(_chan, _data)
#        define GOAL_TGT_TRACE32(_chan, _data)    ITM_EVENT32_WITH_PC(_chan, _data)
#      endif /* GOAL_CONFIG_TGT_TRACE_ITM_WITHOUT_PC == 1 */
#    endif /* GOAL_CONFIG_TGT_TRACE_ITM == 1 */
#  endif /* GOAL_CONFIG_TGT_TRACE == 1 && defined(__ICCARM__) */

/* Keil ARMCC Compiler */
#elif defined(__ARMCC_VERSION)
#  define GOAL_TARGET_CC_FORMAT_PRINTF
#  define GOAL_TARGET_CC_FORMAT_PRINTF_PRE
#  define GOAL_TARGET_CC_FORMAT_SCANF
#  define GOAL_TARGET_CC_FORMAT_SCANF_PRE
#  define GOAL_TARGET_INLINE                __inline
#  define GOAL_TARGET_NORETURN
#  define GOAL_TARGET_NORETURN_PRE          __attribute__((noreturn))
#  define GOAL_TARGET_PACKED
#  define GOAL_TARGET_PACKED_PRE
#  define GOAL_TARGET_PACKED_STRUCT_PRE     __packed
#  define GOAL_TARGET_PACKED_STRUCT_POST
#  define GOAL_TARGET_FALLTHROUGH
#  define GOAL_TARGET_VERSION                  "Keil ARMCC C/C++ V" STR(__VERSION__)
#  define GOAL_TARGET_WEAK                       __weak
#  define GOAL_TARGET_NOT_INITIALIZED     __attribute__((section("NoInit"), zero_init))

/* Renesas RX Compiler */
#elif defined(__RX)
/* checkbox must be set in project properties (Compiler/CPU/Advanced) */
#  pragma packoption
#  define GOAL_TARGET_CC_FORMAT_PRINTF
#  define GOAL_TARGET_CC_FORMAT_PRINTF_PRE
#  define GOAL_TARGET_CC_FORMAT_SCANF
#  define GOAL_TARGET_CC_FORMAT_SCANF_PRE
#  define GOAL_TARGET_NORETURN
#  define GOAL_TARGET_NORETURN_PRE
#  define GOAL_TARGET_PACKED
#  define GOAL_TARGET_PACKED_PRE
#  define GOAL_TARGET_PACKED_STRUCT_PRE
#  define GOAL_TARGET_PACKED_STRUCT_POST
#  define GOAL_TARGET_FALLTHROUGH
#  define GOAL_TARGET_VERSION               "Renesas RX Compiler"

/* Microsoft Visual C++ */
#elif defined(_MSC_VER)
#  define _CRT_SECURE_NO_WARNINGS           1
#  define GOAL_TARGET_CC_FORMAT_PRINTF
#  define GOAL_TARGET_CC_FORMAT_PRINTF_PRE
#  define GOAL_TARGET_CC_FORMAT_SCANF
#  define GOAL_TARGET_CC_FORMAT_SCANF_PRE
#  define GOAL_TARGET_INLINE                _inline
#  define GOAL_TARGET_NORETURN
#  define GOAL_TARGET_NORETURN_PRE
#  define GOAL_TARGET_PACKED
#  define GOAL_TARGET_PACKED_PRE
#  define GOAL_TARGET_PACKED_STRUCT_PRE     __pragma(pack(push, 1))
#  define GOAL_TARGET_PACKED_STRUCT_POST    __pragma(pack(pop))
#  define GOAL_TARGET_FALLTHROUGH
#  define GOAL_TARGET_VERSION               "Microsoft Visual C/C++ V" STR(_MSC_VER)

/* Texas Instruments C Compiler with GCC Extensions
 * See: Build/ARM Compiler/Advanced Options/Language Options
 *      [x] Enable support for GCC extensions
 */
#elif defined(__TI_COMPILER_VERSION__)
#  define GOAL_TARGET_CC_FORMAT_PRINTF      __attribute__((format(printf, 4, 5)))
#  define GOAL_TARGET_CC_FORMAT_PRINTF_PRE
#  define GOAL_TARGET_CC_FORMAT_SCANF       __attribute__((format(scanf, 2, 3)))
#  define GOAL_TARGET_CC_FORMAT_SCANF_PRE
#  define GOAL_TARGET_INLINE                inline
#  define GOAL_TARGET_NORETURN              __attribute__((noreturn))
#  define GOAL_TARGET_NORETURN_PRE
#  define GOAL_TARGET_PACKED                __attribute__((packed))
#  define GOAL_TARGET_PACKED_PRE
#  define GOAL_TARGET_PACKED_STRUCT_PRE
#  define GOAL_TARGET_PACKED_STRUCT_POST
#  define GOAL_TARGET_FALLTHROUGH           /* fall through */
#  define GOAL_TARGET_VERSION               "TI C V" STR(__TI_COMPILER_VERSION__)
#  define GOAL_TARGET_WEAK                  __attribute__((weak))

#else
#  error "Unknown compiler type. Please add defines to goal_arch_common.h"
#endif

#if GOAL_CONFIG_TGT_TRACE == 1
#  if GOAL_CONFIG_TGT_TRACE_PIN == 1
#    define GOAL_TGT_TRACE8(_chan, _data) { \
                for (unsigned int _bit = 0; _bit < 8; _bit++) { \
                    goal_tgtBoardPinSet(_bit, GOAL_BIT_IS_SET(_data, _bit)); \
                } \
            }
#    define GOAL_TGT_TRACE16(_chan, _data) { \
                for (unsigned int _bit = 0; _bit < 16; _bit++) { \
                    goal_tgtBoardPinSet(_bit, GOAL_BIT_IS_SET(_data, _bit)); \
                } \
            }
#    define GOAL_TGT_TRACE32(_chan, _data) { \
                for (unsigned int _bit = 0; _bit < 32; _bit++) { \
                    goal_tgtBoardPinSet(_bit, GOAL_BIT_IS_SET(_data, _bit)); \
                } \
            }
#    define GOAL_TGT_TRACE_BIT_SET(_chan, _bit) goal_tgtBoardPinSet(_bit, GOAL_TRUE)
#    define GOAL_TGT_TRACE_BIT_CLR(_chan, _bit) goal_tgtBoardPinSet(_bit, GOAL_FALSE)
#  endif
#endif


#if !defined(GOAL_TGT_TRACE8)
#  define GOAL_TGT_TRACE8(_chan, _data)
#endif

#if !defined(GOAL_TGT_TRACE16)
#  define GOAL_TGT_TRACE16(_chan, _data)
#endif

#if !defined(GOAL_TGT_TRACE32)
#  define GOAL_TGT_TRACE32(_chan, _data)
#endif

#if !defined(GOAL_TGT_TRACE_BIT_SET)
#  define GOAL_TGT_TRACE_BIT_SET(_chan, _bit)
#endif

#if !defined(GOAL_TGT_TRACE_BIT_CLR)
#  define GOAL_TGT_TRACE_BIT_CLR(_chan, _bit)
#endif

#endif /* GOAL_ARCH_COMMON_H */
