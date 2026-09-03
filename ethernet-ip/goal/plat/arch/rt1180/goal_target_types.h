/** @file
 *
 * @brief GOAL - target specific data type definitions
 *
 * This header contains data type definitions which are used by GOAL and
 * external OS-dependent modules.
 *
 * @copyright
 * Copyright 2010-2021.
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

#ifndef GOAL_TARGET_TYPES_H
#define GOAL_TARGET_TYPES_H


#include "psnprintf.h"


/****************************************************************************/
/* Standard command mappings */
/****************************************************************************/
#define GOAL_STRNLEN                    pstrnlen
#define GOAL_SNPRINTF                   psnprintf
#define GOAL_VSNPRINTF                  pvsnprintf

#ifdef POSIX_API_SUP
/* use BSD API of uNet3 */
#define GOAL_ERRNO                      unet_errno
#endif /* POSIX_API_SUP */


/****************************************************************************/
/* Format defines */
/****************************************************************************/
#ifndef PRIi64
#  define PRIi64 "lli"
#endif

#ifndef PRId64
#  define PRId64 "lld"
#endif

#ifndef PRIx64
#  define PRIx64 "llx"
#endif

#ifndef PRIu64
#  define PRIu64 "llu"
#endif


/****************************************************************************/
/* Endianness */
/****************************************************************************/
#define GOAL_CONFIG_TARGET_LITTLE_ENDIAN 1


/****************************************************************************/
/* Portable data type definitions */
/****************************************************************************/
typedef uint32_t PtrCast;                       /**< pointer casting helper */


#endif /* GOAL_TARGET_TYPES_H */
