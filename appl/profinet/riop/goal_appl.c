/** @file
 *
 * @brief
 * PROFINET Simple I/O Example
 *
 * @details
 * This example provides a possible test environment for a simplified
 * conformance test.
 *
 * @copyright
 * Copyright 2025-2026 NXP
 *
 * NXP Confidential and Proprietary. This software is owned or controlled by NXP
 * and may only be used strictly in accordance with the applicable license
 * terms. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you
 * have read, and that you agree to comply with and are bound by, such license
 * terms. If you do not agree to be bound by the applicable license terms, then
 * you may not retain, install, activate or otherwise use the software.
 *
 */

#include <goal_includes.h>
#include <goal_pnio.h>
#include <goal_dd.h>
#include <goal_snmp.h>
#if RIOP
#include "gl.h"
#endif
/**-- ICC_PNIO_GEN_INCLUDES_START --**/
/* user code block */
extern void riop_appl_loop(void);
/**-- ICC_PNIO_GEN_INCLUDES_END --**/


/****************************************************************************/
/* Local Defines */
/****************************************************************************/
#define APPL_PNIO_ID        0                   /**< PROFINET instance id */
#define APPL_SNMP_ID        0                   /**< SNMP instance id */

#define APPL_SIZE_3_SUB_01_IN    1              /**< size for module 0x3 sub 0x01 input */
#define APPL_SIZE_3_SUB_02_IN    1              /**< size for module 0x3 sub 0x02 input */
#define APPL_SIZE_3_SUB_03_IN    1              /**< size for module 0x3 sub 0x03 input */
#define APPL_SIZE_3_SUB_04_IN    1              /**< size for module 0x3 sub 0x04 input */
#define APPL_SIZE_3_SUB_05_IN    1              /**< size for module 0x3 sub 0x05 input */
#define APPL_SIZE_3_SUB_06_IN    1              /**< size for module 0x3 sub 0x06 input */
#define APPL_SIZE_3_SUB_07_IN    1              /**< size for module 0x3 sub 0x07 input */
#define APPL_SIZE_3_SUB_08_IN    1              /**< size for module 0x3 sub 0x08 input */
#define APPL_SIZE_3_SUB_09_IN    1              /**< size for module 0x3 sub 0x09 input */
#define APPL_SIZE_3_SUB_0A_IN    1              /**< size for module 0x3 sub 0x0A input */
#define APPL_SIZE_3_SUB_0B_IN    1              /**< size for module 0x3 sub 0x0B input */
#define APPL_SIZE_3_SUB_0C_IN    1              /**< size for module 0x3 sub 0x0C input */
#define APPL_SIZE_3_SUB_0D_IN    1              /**< size for module 0x3 sub 0x0D input */
#define APPL_SIZE_3_SUB_0E_IN    1              /**< size for module 0x3 sub 0x0E input */
#define APPL_SIZE_3_SUB_0F_IN    1              /**< size for module 0x3 sub 0x0F input */
#define APPL_SIZE_3_SUB_10_IN    1              /**< size for module 0x3 sub 0x10 input */
#define APPL_SIZE_3_SUB_11_IN    1              /**< size for module 0x3 sub 0x11 input */
#define APPL_SIZE_3_SUB_12_IN    1              /**< size for module 0x3 sub 0x12 input */
#define APPL_SIZE_3_SUB_13_IN    1              /**< size for module 0x3 sub 0x13 input */
#define APPL_SIZE_3_SUB_14_IN    1              /**< size for module 0x3 sub 0x14 input */
#define APPL_SIZE_3_SUB_15_IN    1              /**< size for module 0x3 sub 0x15 input */
#define APPL_SIZE_3_SUB_16_IN    1              /**< size for module 0x3 sub 0x16 input */
#define APPL_SIZE_3_SUB_17_IN    1              /**< size for module 0x3 sub 0x17 input */
#define APPL_SIZE_3_SUB_18_IN    1              /**< size for module 0x3 sub 0x18 input */
#define APPL_SIZE_3_SUB_19_IN    1              /**< size for module 0x3 sub 0x19 input */
#define APPL_SIZE_3_SUB_1A_IN    1              /**< size for module 0x3 sub 0x1A input */
#define APPL_SIZE_3_SUB_1B_IN    1              /**< size for module 0x3 sub 0x1B input */
#define APPL_SIZE_3_SUB_1C_IN    1              /**< size for module 0x3 sub 0x1C input */
#define APPL_SIZE_3_SUB_1D_IN    1              /**< size for module 0x3 sub 0x1D input */
#define APPL_SIZE_3_SUB_1E_IN    1              /**< size for module 0x3 sub 0x1E input */
#define APPL_SIZE_3_SUB_1F_IN    1              /**< size for module 0x3 sub 0x1F input */
#define APPL_SIZE_3_SUB_20_IN    1              /**< size for module 0x3 sub 0x20 input */
#define APPL_SIZE_3_SUB_21_IN    1              /**< size for module 0x3 sub 0x21 input */
#define APPL_SIZE_4_SUB_01_IN    4              /**< size for module 0x4 sub 0x01 input */
#define APPL_SIZE_4_SUB_02_IN    4              /**< size for module 0x4 sub 0x02 input */
#define APPL_SIZE_4_SUB_03_IN    4              /**< size for module 0x4 sub 0x03 input */
#define APPL_SIZE_4_SUB_04_IN    4              /**< size for module 0x4 sub 0x04 input */
#define APPL_SIZE_4_SUB_05_IN    4              /**< size for module 0x4 sub 0x05 input */
#define APPL_SIZE_4_SUB_06_IN    4              /**< size for module 0x4 sub 0x06 input */
#define APPL_SIZE_4_SUB_07_IN    4              /**< size for module 0x4 sub 0x07 input */
#define APPL_SIZE_4_SUB_08_IN    4              /**< size for module 0x4 sub 0x08 input */
#define APPL_SIZE_4_SUB_09_IN    4              /**< size for module 0x4 sub 0x09 input */
#define APPL_SIZE_4_SUB_0A_IN    4              /**< size for module 0x4 sub 0x0A input */
#define APPL_SIZE_4_SUB_0B_IN    4              /**< size for module 0x4 sub 0x0B input */
#define APPL_SIZE_4_SUB_0C_IN    4              /**< size for module 0x4 sub 0x0C input */
#define APPL_SIZE_4_SUB_0D_IN    4              /**< size for module 0x4 sub 0x0D input */
#define APPL_SIZE_4_SUB_0E_IN    4              /**< size for module 0x4 sub 0x0E input */
#define APPL_SIZE_4_SUB_0F_IN    4              /**< size for module 0x4 sub 0x0F input */
#define APPL_SIZE_4_SUB_10_IN    4              /**< size for module 0x4 sub 0x10 input */
#define APPL_SIZE_5_SUB_01_OUT   1              /**< size for module 0x5 sub 0x01 output */
#define APPL_SIZE_5_SUB_02_OUT   1              /**< size for module 0x5 sub 0x02 output */
#define APPL_SIZE_5_SUB_03_OUT   1              /**< size for module 0x5 sub 0x03 output */
#define APPL_SIZE_5_SUB_04_OUT   1              /**< size for module 0x5 sub 0x04 output */
#define APPL_SIZE_5_SUB_05_OUT   1              /**< size for module 0x5 sub 0x05 output */
#define APPL_SIZE_5_SUB_06_OUT   1              /**< size for module 0x5 sub 0x06 output */
#define APPL_SIZE_5_SUB_07_OUT   1              /**< size for module 0x5 sub 0x07 output */
#define APPL_SIZE_5_SUB_08_OUT   1              /**< size for module 0x5 sub 0x08 output */
#define APPL_SIZE_5_SUB_09_OUT   1              /**< size for module 0x5 sub 0x09 output */
#define APPL_SIZE_5_SUB_0A_OUT   1              /**< size for module 0x5 sub 0x0A output */
#define APPL_SIZE_5_SUB_0B_OUT   1              /**< size for module 0x5 sub 0x0B output */
#define APPL_SIZE_5_SUB_0C_OUT   1              /**< size for module 0x5 sub 0x0C output */
#define APPL_SIZE_5_SUB_0D_OUT   1              /**< size for module 0x5 sub 0x0D output */
#define APPL_SIZE_5_SUB_0E_OUT   1              /**< size for module 0x5 sub 0x0E output */
#define APPL_SIZE_5_SUB_0F_OUT   1              /**< size for module 0x5 sub 0x0F output */
#define APPL_SIZE_5_SUB_10_OUT   1              /**< size for module 0x5 sub 0x10 output */
#define APPL_SIZE_5_SUB_11_OUT   1              /**< size for module 0x5 sub 0x11 output */
#define APPL_SIZE_5_SUB_12_OUT   1              /**< size for module 0x5 sub 0x12 output */
#define APPL_SIZE_5_SUB_13_OUT   1              /**< size for module 0x5 sub 0x13 output */
#define APPL_SIZE_5_SUB_14_OUT   1              /**< size for module 0x5 sub 0x14 output */
#define APPL_SIZE_5_SUB_15_OUT   1              /**< size for module 0x5 sub 0x15 output */
#define APPL_SIZE_5_SUB_16_OUT   1              /**< size for module 0x5 sub 0x16 output */
#define APPL_SIZE_5_SUB_17_OUT   1              /**< size for module 0x5 sub 0x17 output */
#define APPL_SIZE_5_SUB_18_OUT   1              /**< size for module 0x5 sub 0x18 output */
#define APPL_SIZE_5_SUB_19_OUT   1              /**< size for module 0x5 sub 0x19 output */
#define APPL_SIZE_5_SUB_1A_OUT   1              /**< size for module 0x5 sub 0x1A output */
#define APPL_SIZE_5_SUB_1B_OUT   1              /**< size for module 0x5 sub 0x1B output */
#define APPL_SIZE_5_SUB_1C_OUT   1              /**< size for module 0x5 sub 0x1C output */
#define APPL_SIZE_5_SUB_1D_OUT   1              /**< size for module 0x5 sub 0x1D output */
#define APPL_SIZE_5_SUB_1E_OUT   1              /**< size for module 0x5 sub 0x1E output */
#define APPL_SIZE_5_SUB_1F_OUT   1              /**< size for module 0x5 sub 0x1F output */
#define APPL_SIZE_5_SUB_20_OUT   1              /**< size for module 0x5 sub 0x20 output */
#define APPL_SIZE_5_SUB_21_OUT   1              /**< size for module 0x5 sub 0x21 output */
#define APPL_SIZE_6_SUB_01_OUT   4              /**< size for module 0x6 sub 0x01 output */
#define APPL_SIZE_6_SUB_02_OUT   4              /**< size for module 0x6 sub 0x02 output */
#define APPL_SIZE_6_SUB_03_OUT   4              /**< size for module 0x6 sub 0x03 output */
#define APPL_SIZE_6_SUB_04_OUT   4              /**< size for module 0x6 sub 0x04 output */
#define APPL_SIZE_6_SUB_05_OUT   4              /**< size for module 0x6 sub 0x05 output */
#define APPL_SIZE_6_SUB_06_OUT   4              /**< size for module 0x6 sub 0x06 output */
#define APPL_SIZE_6_SUB_07_OUT   4              /**< size for module 0x6 sub 0x07 output */
#define APPL_SIZE_6_SUB_08_OUT   4              /**< size for module 0x6 sub 0x08 output */
#define APPL_SIZE_6_SUB_09_OUT   4              /**< size for module 0x6 sub 0x09 output */
#define APPL_SIZE_6_SUB_0A_OUT   4              /**< size for module 0x6 sub 0x0A output */
#define APPL_SIZE_6_SUB_0B_OUT   4              /**< size for module 0x6 sub 0x0B output */
#define APPL_SIZE_6_SUB_0C_OUT   4              /**< size for module 0x6 sub 0x0C output */
#define APPL_SIZE_6_SUB_0D_OUT   4              /**< size for module 0x6 sub 0x0D output */
#define APPL_SIZE_6_SUB_0E_OUT   4              /**< size for module 0x6 sub 0x0E output */
#define APPL_SIZE_6_SUB_0F_OUT   4              /**< size for module 0x6 sub 0x0F output */
#define APPL_SIZE_6_SUB_10_OUT   4              /**< size for module 0x6 sub 0x10 output */

#define APPL_SLOT_1         0x1                   /**< slot 0x1 */
#define APPL_SLOT_1_SUB_01   0x01                   /**< subslot for slot 0x1 */
#define APPL_SLOT_1_SUB_02   0x02                   /**< subslot for slot 0x1 */
#define APPL_SLOT_1_SUB_03   0x03                   /**< subslot for slot 0x1 */
#define APPL_SLOT_1_SUB_04   0x04                   /**< subslot for slot 0x1 */
#define APPL_SLOT_1_SUB_05   0x05                   /**< subslot for slot 0x1 */
#define APPL_SLOT_1_SUB_06   0x06                   /**< subslot for slot 0x1 */
#define APPL_SLOT_1_SUB_07   0x07                   /**< subslot for slot 0x1 */
#define APPL_SLOT_1_SUB_08   0x08                   /**< subslot for slot 0x1 */
#define APPL_SLOT_1_SUB_09   0x09                   /**< subslot for slot 0x1 */
#define APPL_SLOT_1_SUB_0A   0x0A                   /**< subslot for slot 0x1 */
#define APPL_SLOT_1_SUB_0B   0x0B                   /**< subslot for slot 0x1 */
#define APPL_SLOT_1_SUB_0C   0x0C                   /**< subslot for slot 0x1 */
#define APPL_SLOT_1_SUB_0D   0x0D                   /**< subslot for slot 0x1 */
#define APPL_SLOT_1_SUB_0E   0x0E                   /**< subslot for slot 0x1 */
#define APPL_SLOT_1_SUB_0F   0x0F                   /**< subslot for slot 0x1 */
#define APPL_SLOT_1_SUB_10   0x10                   /**< subslot for slot 0x1 */
#define APPL_SLOT_1_SUB_11   0x11                   /**< subslot for slot 0x1 */
#define APPL_SLOT_1_SUB_12   0x12                   /**< subslot for slot 0x1 */
#define APPL_SLOT_1_SUB_13   0x13                   /**< subslot for slot 0x1 */
#define APPL_SLOT_1_SUB_14   0x14                   /**< subslot for slot 0x1 */
#define APPL_SLOT_1_SUB_15   0x15                   /**< subslot for slot 0x1 */
#define APPL_SLOT_1_SUB_16   0x16                   /**< subslot for slot 0x1 */
#define APPL_SLOT_1_SUB_17   0x17                   /**< subslot for slot 0x1 */
#define APPL_SLOT_1_SUB_18   0x18                   /**< subslot for slot 0x1 */
#define APPL_SLOT_1_SUB_19   0x19                   /**< subslot for slot 0x1 */
#define APPL_SLOT_1_SUB_1A   0x1A                   /**< subslot for slot 0x1 */
#define APPL_SLOT_1_SUB_1B   0x1B                   /**< subslot for slot 0x1 */
#define APPL_SLOT_1_SUB_1C   0x1C                   /**< subslot for slot 0x1 */
#define APPL_SLOT_1_SUB_1D   0x1D                   /**< subslot for slot 0x1 */
#define APPL_SLOT_1_SUB_1E   0x1E                   /**< subslot for slot 0x1 */
#define APPL_SLOT_1_SUB_1F   0x1F                   /**< subslot for slot 0x1 */
#define APPL_SLOT_1_SUB_20   0x20                   /**< subslot for slot 0x1 */
#define APPL_SLOT_1_SUB_21   0x21                   /**< subslot for slot 0x1 */
#define APPL_SLOT_2         0x2                   /**< slot 0x2 */
#define APPL_SLOT_2_SUB_01   0x01                   /**< subslot for slot 0x2 */
#define APPL_SLOT_2_SUB_02   0x02                   /**< subslot for slot 0x2 */
#define APPL_SLOT_2_SUB_03   0x03                   /**< subslot for slot 0x2 */
#define APPL_SLOT_2_SUB_04   0x04                   /**< subslot for slot 0x2 */
#define APPL_SLOT_2_SUB_05   0x05                   /**< subslot for slot 0x2 */
#define APPL_SLOT_2_SUB_06   0x06                   /**< subslot for slot 0x2 */
#define APPL_SLOT_2_SUB_07   0x07                   /**< subslot for slot 0x2 */
#define APPL_SLOT_2_SUB_08   0x08                   /**< subslot for slot 0x2 */
#define APPL_SLOT_2_SUB_09   0x09                   /**< subslot for slot 0x2 */
#define APPL_SLOT_2_SUB_0A   0x0A                   /**< subslot for slot 0x2 */
#define APPL_SLOT_2_SUB_0B   0x0B                   /**< subslot for slot 0x2 */
#define APPL_SLOT_2_SUB_0C   0x0C                   /**< subslot for slot 0x2 */
#define APPL_SLOT_2_SUB_0D   0x0D                   /**< subslot for slot 0x2 */
#define APPL_SLOT_2_SUB_0E   0x0E                   /**< subslot for slot 0x2 */
#define APPL_SLOT_2_SUB_0F   0x0F                   /**< subslot for slot 0x2 */
#define APPL_SLOT_2_SUB_10   0x10                   /**< subslot for slot 0x2 */
#define APPL_SLOT_3         0x3                   /**< slot 0x3 */
#define APPL_SLOT_3_SUB_01   0x01                   /**< subslot for slot 0x3 */
#define APPL_SLOT_3_SUB_02   0x02                   /**< subslot for slot 0x3 */
#define APPL_SLOT_3_SUB_03   0x03                   /**< subslot for slot 0x3 */
#define APPL_SLOT_3_SUB_04   0x04                   /**< subslot for slot 0x3 */
#define APPL_SLOT_3_SUB_05   0x05                   /**< subslot for slot 0x3 */
#define APPL_SLOT_3_SUB_06   0x06                   /**< subslot for slot 0x3 */
#define APPL_SLOT_3_SUB_07   0x07                   /**< subslot for slot 0x3 */
#define APPL_SLOT_3_SUB_08   0x08                   /**< subslot for slot 0x3 */
#define APPL_SLOT_3_SUB_09   0x09                   /**< subslot for slot 0x3 */
#define APPL_SLOT_3_SUB_0A   0x0A                   /**< subslot for slot 0x3 */
#define APPL_SLOT_3_SUB_0B   0x0B                   /**< subslot for slot 0x3 */
#define APPL_SLOT_3_SUB_0C   0x0C                   /**< subslot for slot 0x3 */
#define APPL_SLOT_3_SUB_0D   0x0D                   /**< subslot for slot 0x3 */
#define APPL_SLOT_3_SUB_0E   0x0E                   /**< subslot for slot 0x3 */
#define APPL_SLOT_3_SUB_0F   0x0F                   /**< subslot for slot 0x3 */
#define APPL_SLOT_3_SUB_10   0x10                   /**< subslot for slot 0x3 */
#define APPL_SLOT_3_SUB_11   0x11                   /**< subslot for slot 0x3 */
#define APPL_SLOT_3_SUB_12   0x12                   /**< subslot for slot 0x3 */
#define APPL_SLOT_3_SUB_13   0x13                   /**< subslot for slot 0x3 */
#define APPL_SLOT_3_SUB_14   0x14                   /**< subslot for slot 0x3 */
#define APPL_SLOT_3_SUB_15   0x15                   /**< subslot for slot 0x3 */
#define APPL_SLOT_3_SUB_16   0x16                   /**< subslot for slot 0x3 */
#define APPL_SLOT_3_SUB_17   0x17                   /**< subslot for slot 0x3 */
#define APPL_SLOT_3_SUB_18   0x18                   /**< subslot for slot 0x3 */
#define APPL_SLOT_3_SUB_19   0x19                   /**< subslot for slot 0x3 */
#define APPL_SLOT_3_SUB_1A   0x1A                   /**< subslot for slot 0x3 */
#define APPL_SLOT_3_SUB_1B   0x1B                   /**< subslot for slot 0x3 */
#define APPL_SLOT_3_SUB_1C   0x1C                   /**< subslot for slot 0x3 */
#define APPL_SLOT_3_SUB_1D   0x1D                   /**< subslot for slot 0x3 */
#define APPL_SLOT_3_SUB_1E   0x1E                   /**< subslot for slot 0x3 */
#define APPL_SLOT_3_SUB_1F   0x1F                   /**< subslot for slot 0x3 */
#define APPL_SLOT_3_SUB_20   0x20                   /**< subslot for slot 0x3 */
#define APPL_SLOT_3_SUB_21   0x21                   /**< subslot for slot 0x3 */
#define APPL_SLOT_4         0x4                   /**< slot 0x4 */
#define APPL_SLOT_4_SUB_01   0x01                   /**< subslot for slot 0x4 */
#define APPL_SLOT_4_SUB_02   0x02                   /**< subslot for slot 0x4 */
#define APPL_SLOT_4_SUB_03   0x03                   /**< subslot for slot 0x4 */
#define APPL_SLOT_4_SUB_04   0x04                   /**< subslot for slot 0x4 */
#define APPL_SLOT_4_SUB_05   0x05                   /**< subslot for slot 0x4 */
#define APPL_SLOT_4_SUB_06   0x06                   /**< subslot for slot 0x4 */
#define APPL_SLOT_4_SUB_07   0x07                   /**< subslot for slot 0x4 */
#define APPL_SLOT_4_SUB_08   0x08                   /**< subslot for slot 0x4 */
#define APPL_SLOT_4_SUB_09   0x09                   /**< subslot for slot 0x4 */
#define APPL_SLOT_4_SUB_0A   0x0A                   /**< subslot for slot 0x4 */
#define APPL_SLOT_4_SUB_0B   0x0B                   /**< subslot for slot 0x4 */
#define APPL_SLOT_4_SUB_0C   0x0C                   /**< subslot for slot 0x4 */
#define APPL_SLOT_4_SUB_0D   0x0D                   /**< subslot for slot 0x4 */
#define APPL_SLOT_4_SUB_0E   0x0E                   /**< subslot for slot 0x4 */
#define APPL_SLOT_4_SUB_0F   0x0F                   /**< subslot for slot 0x4 */
#define APPL_SLOT_4_SUB_10   0x10                   /**< subslot for slot 0x4 */

#define APPL_MOD_3          0x3                /**< module 0x3 */
#define APPL_MOD_3_SUB_01    0x01                /**< submodule for module 0x3 */
#define APPL_MOD_3_SUB_02    0x02                /**< submodule for module 0x3 */
#define APPL_MOD_3_SUB_03    0x03                /**< submodule for module 0x3 */
#define APPL_MOD_3_SUB_04    0x04                /**< submodule for module 0x3 */
#define APPL_MOD_3_SUB_05    0x05                /**< submodule for module 0x3 */
#define APPL_MOD_3_SUB_06    0x06                /**< submodule for module 0x3 */
#define APPL_MOD_3_SUB_07    0x07                /**< submodule for module 0x3 */
#define APPL_MOD_3_SUB_08    0x08                /**< submodule for module 0x3 */
#define APPL_MOD_3_SUB_09    0x09                /**< submodule for module 0x3 */
#define APPL_MOD_3_SUB_0A    0x0A                /**< submodule for module 0x3 */
#define APPL_MOD_3_SUB_0B    0x0B                /**< submodule for module 0x3 */
#define APPL_MOD_3_SUB_0C    0x0C                /**< submodule for module 0x3 */
#define APPL_MOD_3_SUB_0D    0x0D                /**< submodule for module 0x3 */
#define APPL_MOD_3_SUB_0E    0x0E                /**< submodule for module 0x3 */
#define APPL_MOD_3_SUB_0F    0x0F                /**< submodule for module 0x3 */
#define APPL_MOD_3_SUB_10    0x10                /**< submodule for module 0x3 */
#define APPL_MOD_3_SUB_11    0x11                /**< submodule for module 0x3 */
#define APPL_MOD_3_SUB_12    0x12                /**< submodule for module 0x3 */
#define APPL_MOD_3_SUB_13    0x13                /**< submodule for module 0x3 */
#define APPL_MOD_3_SUB_14    0x14                /**< submodule for module 0x3 */
#define APPL_MOD_3_SUB_15    0x15                /**< submodule for module 0x3 */
#define APPL_MOD_3_SUB_16    0x16                /**< submodule for module 0x3 */
#define APPL_MOD_3_SUB_17    0x17                /**< submodule for module 0x3 */
#define APPL_MOD_3_SUB_18    0x18                /**< submodule for module 0x3 */
#define APPL_MOD_3_SUB_19    0x19                /**< submodule for module 0x3 */
#define APPL_MOD_3_SUB_1A    0x1A                /**< submodule for module 0x3 */
#define APPL_MOD_3_SUB_1B    0x1B                /**< submodule for module 0x3 */
#define APPL_MOD_3_SUB_1C    0x1C                /**< submodule for module 0x3 */
#define APPL_MOD_3_SUB_1D    0x1D                /**< submodule for module 0x3 */
#define APPL_MOD_3_SUB_1E    0x1E                /**< submodule for module 0x3 */
#define APPL_MOD_3_SUB_1F    0x1F                /**< submodule for module 0x3 */
#define APPL_MOD_3_SUB_20    0x20                /**< submodule for module 0x3 */
#define APPL_MOD_3_SUB_21    0x21                /**< submodule for module 0x3 */
#define APPL_MOD_4          0x4                /**< module 0x4 */
#define APPL_MOD_4_SUB_01    0x01                /**< submodule for module 0x4 */
#define APPL_MOD_4_SUB_02    0x02                /**< submodule for module 0x4 */
#define APPL_MOD_4_SUB_03    0x03                /**< submodule for module 0x4 */
#define APPL_MOD_4_SUB_04    0x04                /**< submodule for module 0x4 */
#define APPL_MOD_4_SUB_05    0x05                /**< submodule for module 0x4 */
#define APPL_MOD_4_SUB_06    0x06                /**< submodule for module 0x4 */
#define APPL_MOD_4_SUB_07    0x07                /**< submodule for module 0x4 */
#define APPL_MOD_4_SUB_08    0x08                /**< submodule for module 0x4 */
#define APPL_MOD_4_SUB_09    0x09                /**< submodule for module 0x4 */
#define APPL_MOD_4_SUB_0A    0x0A                /**< submodule for module 0x4 */
#define APPL_MOD_4_SUB_0B    0x0B                /**< submodule for module 0x4 */
#define APPL_MOD_4_SUB_0C    0x0C                /**< submodule for module 0x4 */
#define APPL_MOD_4_SUB_0D    0x0D                /**< submodule for module 0x4 */
#define APPL_MOD_4_SUB_0E    0x0E                /**< submodule for module 0x4 */
#define APPL_MOD_4_SUB_0F    0x0F                /**< submodule for module 0x4 */
#define APPL_MOD_4_SUB_10    0x10                /**< submodule for module 0x4 */
#define APPL_MOD_5          0x5                /**< module 0x5 */
#define APPL_MOD_5_SUB_01    0x01                /**< submodule for module 0x5 */
#define APPL_MOD_5_SUB_02    0x02                /**< submodule for module 0x5 */
#define APPL_MOD_5_SUB_03    0x03                /**< submodule for module 0x5 */
#define APPL_MOD_5_SUB_04    0x04                /**< submodule for module 0x5 */
#define APPL_MOD_5_SUB_05    0x05                /**< submodule for module 0x5 */
#define APPL_MOD_5_SUB_06    0x06                /**< submodule for module 0x5 */
#define APPL_MOD_5_SUB_07    0x07                /**< submodule for module 0x5 */
#define APPL_MOD_5_SUB_08    0x08                /**< submodule for module 0x5 */
#define APPL_MOD_5_SUB_09    0x09                /**< submodule for module 0x5 */
#define APPL_MOD_5_SUB_0A    0x0A                /**< submodule for module 0x5 */
#define APPL_MOD_5_SUB_0B    0x0B                /**< submodule for module 0x5 */
#define APPL_MOD_5_SUB_0C    0x0C                /**< submodule for module 0x5 */
#define APPL_MOD_5_SUB_0D    0x0D                /**< submodule for module 0x5 */
#define APPL_MOD_5_SUB_0E    0x0E                /**< submodule for module 0x5 */
#define APPL_MOD_5_SUB_0F    0x0F                /**< submodule for module 0x5 */
#define APPL_MOD_5_SUB_10    0x10                /**< submodule for module 0x5 */
#define APPL_MOD_5_SUB_11    0x11                /**< submodule for module 0x5 */
#define APPL_MOD_5_SUB_12    0x12                /**< submodule for module 0x5 */
#define APPL_MOD_5_SUB_13    0x13                /**< submodule for module 0x5 */
#define APPL_MOD_5_SUB_14    0x14                /**< submodule for module 0x5 */
#define APPL_MOD_5_SUB_15    0x15                /**< submodule for module 0x5 */
#define APPL_MOD_5_SUB_16    0x16                /**< submodule for module 0x5 */
#define APPL_MOD_5_SUB_17    0x17                /**< submodule for module 0x5 */
#define APPL_MOD_5_SUB_18    0x18                /**< submodule for module 0x5 */
#define APPL_MOD_5_SUB_19    0x19                /**< submodule for module 0x5 */
#define APPL_MOD_5_SUB_1A    0x1A                /**< submodule for module 0x5 */
#define APPL_MOD_5_SUB_1B    0x1B                /**< submodule for module 0x5 */
#define APPL_MOD_5_SUB_1C    0x1C                /**< submodule for module 0x5 */
#define APPL_MOD_5_SUB_1D    0x1D                /**< submodule for module 0x5 */
#define APPL_MOD_5_SUB_1E    0x1E                /**< submodule for module 0x5 */
#define APPL_MOD_5_SUB_1F    0x1F                /**< submodule for module 0x5 */
#define APPL_MOD_5_SUB_20    0x20                /**< submodule for module 0x5 */
#define APPL_MOD_5_SUB_21    0x21                /**< submodule for module 0x5 */
#define APPL_MOD_6          0x6                /**< module 0x6 */
#define APPL_MOD_6_SUB_01    0x01                /**< submodule for module 0x6 */
#define APPL_MOD_6_SUB_02    0x02                /**< submodule for module 0x6 */
#define APPL_MOD_6_SUB_03    0x03                /**< submodule for module 0x6 */
#define APPL_MOD_6_SUB_04    0x04                /**< submodule for module 0x6 */
#define APPL_MOD_6_SUB_05    0x05                /**< submodule for module 0x6 */
#define APPL_MOD_6_SUB_06    0x06                /**< submodule for module 0x6 */
#define APPL_MOD_6_SUB_07    0x07                /**< submodule for module 0x6 */
#define APPL_MOD_6_SUB_08    0x08                /**< submodule for module 0x6 */
#define APPL_MOD_6_SUB_09    0x09                /**< submodule for module 0x6 */
#define APPL_MOD_6_SUB_0A    0x0A                /**< submodule for module 0x6 */
#define APPL_MOD_6_SUB_0B    0x0B                /**< submodule for module 0x6 */
#define APPL_MOD_6_SUB_0C    0x0C                /**< submodule for module 0x6 */
#define APPL_MOD_6_SUB_0D    0x0D                /**< submodule for module 0x6 */
#define APPL_MOD_6_SUB_0E    0x0E                /**< submodule for module 0x6 */
#define APPL_MOD_6_SUB_0F    0x0F                /**< submodule for module 0x6 */
#define APPL_MOD_6_SUB_10    0x10                /**< submodule for module 0x6 */
#define DEVICE_VENDORID               0x0CC2
#define DEVICE_VENDOR_NAME            "NXP"
#define DEVICE_NUM_SLOT_MAX           5
#define DEVICE_NUM_SUBSLOT_MAX        34
#define APPL_API            0                   /**< API 0 */

/**-- ICC_PNIO_GEN_DEFINE_START --**/
/* user code block */
#define APPL_TIMEOUT_TRIGGER_VAL GOAL_TIMER_SEC /**< timeout trigger in ms */

#if PNET_CONVERT_USRDATA
#define PNET_COPY_USR_DATA(var,ptr) do {\
	memcpy(&var,ptr,sizeof(var));\
	var = GOAL_htobe32(var);\
	ptr++;\
	} while(0)
#else
#define PNET_COPY_USR_DATA(var,ptr)  var = *ptr++;
#endif
/**-- ICC_PNIO_GEN_DEFINE_END --**/


/****************************************************************************/
/* Local Types */
/****************************************************************************/
/**-- ICC_PNIO_GEN_LOCALTYPES_START --**/
/* user code block */

/**-- ICC_PNIO_GEN_LOCALTYPES_END --**/


/****************************************************************************/
/* Local Prototypes */
/****************************************************************************/
static GOAL_STATUS_T main_callback(
    GOAL_PNIO_T *pPnio,                         /**< PROFINET handle */
    GOAL_PNIO_CB_ID_T id,                       /**< callback id */
    GOAL_PNIO_CB_DATA_T *pCb                    /**< callback parameters */
);

static GOAL_STATUS_T appl_pnioCbReadRecord(
    GOAL_PNIO_T *pPnio,                         /**< PROFINET handle */
    GOAL_PNIO_CB_DATA_T *pCb                    /**< callback parameters */
);

static GOAL_STATUS_T appl_pnioCbWriteRecord(
    GOAL_PNIO_T *pPnio,                         /**< PROFINET handle */
    GOAL_PNIO_CB_DATA_T *pCb                    /**< callback parameters */
);

/**-- ICC_PNIO_GEN_LOCALPROTO_START --**/
/* user code block */
/**-- ICC_PNIO_GEN_LOCALPROTO_END --**/


/****************************************************************************/
/* Local Variables */
/****************************************************************************/
static GOAL_BOOL_T flgAppReady = GOAL_FALSE;    /**< app ready flag */
static GOAL_PNIO_AR_ID_T idAr = 0;              /**< AR ID */
static GOAL_PNIO_T *pPnio;                      /**< GOAL PROFINET handle */

/**-- ICC_PNIO_GEN_LOCALVARS_START --**/
/* user code block */
static GOAL_TIMESTAMP_T mTsTout;                /**< timeout timestamp */
static GOAL_INSTANCE_SNMP_T *mpInstanceSnmp;    /**< GOAL_SNMP handle */
#if RIOP
extern int riop_appl_init(void);
#endif

/**-- ICC_PNIO_GEN_LOCALVARS_END --**/


/****************************************************************************/
/** Application Init
 *
 * Build up the device structure and initialize the Profinet stack.
 *
 * @retval GOAL_STATUS_T result
 */
GOAL_STATUS_T appl_init(
    void
)
{
    GOAL_STATUS_T res;                          /* result */
    
    /**-- ICC_PNIO_GEN_APPLINIT_BEFORE_START --**/
    /* user code block */
#if RIOP
    riop_appl_init();
#endif

    /* initialize device detection module */
    res = goal_ddInit();
    if (GOAL_RES_ERR(res)) {
	    goal_logErr("Initialization of GOAL dd failed");
	    return res;
    }

    /**-- ICC_PNIO_GEN_APPLINIT_BEFORE_END --**/


    /* initialize PROFINET */
    res = goal_pnioInit();
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Initialization of PROFINET failed");
    }
    
    /* initialize SNMP */
    res = goal_snmpInit();
    if (GOAL_RES_ERR(res)) {
        goal_logErr("Initialization of SNMP failed");
    }

    /**-- ICC_PNIO_GEN_APPLINIT_AFTER_START --**/
    /* user code block */
    /**-- ICC_PNIO_GEN_APPLINIT_AFTER_END --**/

    return res;
}

/****************************************************************************/
/** Application Setup
 *
 * Show the most common PROFINET configuration settings.
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T appl_setup(
    void
)
{
    GOAL_STATUS_T res;                          /* result */
    GOAL_INSTANCE_SNMP_T *pInstanceSnmp = NULL; /* GOAL_SNMP handle */
    GOAL_DD_T *pHdlDd;                          /* DD handle */
    
    /**-- ICC_PNIO_GEN_APPLSETUP_BEFORE_START --**/
    /* user code block */
    /**-- ICC_PNIO_GEN_APPLSETUP_BEFORE_END --**/
    /* Vendor Id */
    res = goal_pnioCfgVendorIdSet(DEVICE_VENDORID);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* Vendor Name */
    res = goal_pnioCfgVendorNameSet(DEVICE_VENDOR_NAME);
    if (GOAL_RES_ERR(res)) {
        return res;
    }
    /* Slot Maximum Count */
    res = goal_pnioCfgSlotMaxCntSet(DEVICE_NUM_SLOT_MAX);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* Subslot Maximum Count */
    res = goal_pnioCfgSubslotMaxCntSet(DEVICE_NUM_SUBSLOT_MAX);
    if (GOAL_RES_ERR(res)) {
        return res;
    }

    /* start goal dd with all features enabled */
    res = goal_ddNew(&pHdlDd, GOAL_DD_FEAT_ALL);
    if (GOAL_RES_ERR(res)) {
	    goal_logErr("error creating goal dd instance");
	    return res;
    }

    /* create SNMP instance */
    res = goal_snmpNew(&pInstanceSnmp, APPL_SNMP_ID);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create SNMP instance");
        return res;
    }
    
    /* set SNMP instance id for new PNIO instance */
    res = goal_pnioCfgSnmpIdSet(APPL_SNMP_ID);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to set SNMP instance id");
        return res;
    }
    /* enable MRP */
    res = goal_pnioCfgFlgMrpCfgSet(GOAL_TRUE);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to enable MRP");
        return res;
    }

    /* configure DCP Set reject feature (needed for Security Class 1) */
    res = goal_pnioCfgFlgDcpSetRejectSet(GOAL_TRUE);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to set DCP Reject Set Feature");
        return res;
    }

    /* configure SNMP adjust support (needed for Security Class 1) */
    res = goal_pnioCfgFlgSnmpAdjustSet(GOAL_TRUE);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to set SNMP Adjust Support");
        return res;
    }


    /* create new PROFINET instance */
    res = goal_pnioNew(&pPnio, APPL_PNIO_ID, main_callback);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to create a new PROFINET instance");
        return res;
    }

    goal_logInfo("Initializing device structure");

    /* create subslots */
    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_01, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x01");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_02, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x02");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_03, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x03");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_04, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x04");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_05, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x05");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_06, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x06");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_07, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x07");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_08, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x08");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_09, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x09");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_0A, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x0A");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_0B, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x0B");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_0C, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x0C");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_0D, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x0D");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_0E, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x0E");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_0F, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x0F");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_10, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x10");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_11, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x11");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_12, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x12");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_13, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x13");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_14, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x14");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_15, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x15");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_16, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x16");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_17, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x17");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_18, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x18");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_19, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x19");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_1A, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x1A");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_1B, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x1B");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_1C, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x1C");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_1D, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x1D");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_1E, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x1E");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_1F, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x1F");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_20, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x20");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_21, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x21");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_01, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x01");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_02, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x02");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_03, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x03");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_04, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x04");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_05, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x05");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_06, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x06");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_07, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x07");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_08, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x08");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_09, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x09");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_0A, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x0A");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_0B, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x0B");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_0C, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x0C");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_0D, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x0D");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_0E, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x0E");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_0F, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x0F");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_10, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x10");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_01, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x01");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_02, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x02");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_03, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x03");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_04, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x04");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_05, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x05");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_06, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x06");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_07, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x07");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_08, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x08");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_09, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x09");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_0A, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x0A");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_0B, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x0B");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_0C, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x0C");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_0D, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x0D");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_0E, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x0E");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_0F, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x0F");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_10, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x10");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_11, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x11");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_12, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x12");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_13, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x13");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_14, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x14");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_15, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x15");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_16, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x16");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_17, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x17");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_18, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x18");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_19, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x19");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_1A, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x1A");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_1B, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x1B");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_1C, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x1C");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_1D, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x1D");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_1E, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x1E");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_1F, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x1F");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_20, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x20");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_21, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x21");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_4, APPL_SLOT_4_SUB_01, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x01");
        return res;
    }
    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_4, APPL_SLOT_4_SUB_02, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x02");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_4, APPL_SLOT_4_SUB_03, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x03");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_4, APPL_SLOT_4_SUB_04, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x04");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_4, APPL_SLOT_4_SUB_05, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x05");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_4, APPL_SLOT_4_SUB_06, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x06");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_4, APPL_SLOT_4_SUB_07, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x07");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_4, APPL_SLOT_4_SUB_08, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x08");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_4, APPL_SLOT_4_SUB_09, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x09");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_4, APPL_SLOT_4_SUB_0A, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x0A");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_4, APPL_SLOT_4_SUB_0B, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x0B");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_4, APPL_SLOT_4_SUB_0C, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x0C");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_4, APPL_SLOT_4_SUB_0D, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x0D");
        return res;
    }
    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_4, APPL_SLOT_4_SUB_0E, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x0E");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_4, APPL_SLOT_4_SUB_0F, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x0F");
        return res;
    }

    res = goal_pnioSubslotNew(pPnio, APPL_API, APPL_SLOT_4, APPL_SLOT_4_SUB_10, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add subslot 0x10");
        return res;
    }

    /* create submodules */
    res = goal_pnioSubmodNew(pPnio, APPL_MOD_3, APPL_MOD_3_SUB_01, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_3_SUB_01_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x01");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_3, APPL_MOD_3_SUB_02, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_3_SUB_02_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x02");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_3, APPL_MOD_3_SUB_03, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_3_SUB_03_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x03");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_3, APPL_MOD_3_SUB_04, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_3_SUB_04_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x04");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_3, APPL_MOD_3_SUB_05, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_3_SUB_05_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x05");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_3, APPL_MOD_3_SUB_06, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_3_SUB_06_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x06");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_3, APPL_MOD_3_SUB_07, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_3_SUB_07_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x07");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_3, APPL_MOD_3_SUB_08, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_3_SUB_08_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x08");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_3, APPL_MOD_3_SUB_09, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_3_SUB_09_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x09");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_3, APPL_MOD_3_SUB_0A, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_3_SUB_0A_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x0A");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_3, APPL_MOD_3_SUB_0B, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_3_SUB_0B_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x0B");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_3, APPL_MOD_3_SUB_0C, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_3_SUB_0C_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x0C");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_3, APPL_MOD_3_SUB_0D, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_3_SUB_0D_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x0D");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_3, APPL_MOD_3_SUB_0E, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_3_SUB_0E_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x0E");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_3, APPL_MOD_3_SUB_0F, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_3_SUB_0F_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x0F");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_3, APPL_MOD_3_SUB_10, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_3_SUB_10_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x10");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_3, APPL_MOD_3_SUB_11, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_3_SUB_11_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x11");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_3, APPL_MOD_3_SUB_12, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_3_SUB_12_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x12");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_3, APPL_MOD_3_SUB_13, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_3_SUB_13_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x13");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_3, APPL_MOD_3_SUB_14, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_3_SUB_14_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x14");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_3, APPL_MOD_3_SUB_15, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_3_SUB_15_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x15");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_3, APPL_MOD_3_SUB_16, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_3_SUB_16_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x16");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_3, APPL_MOD_3_SUB_17, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_3_SUB_17_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x17");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_3, APPL_MOD_3_SUB_18, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_3_SUB_18_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x18");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_3, APPL_MOD_3_SUB_19, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_3_SUB_19_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x19");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_3, APPL_MOD_3_SUB_1A, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_3_SUB_1A_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x1A");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_3, APPL_MOD_3_SUB_1B, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_3_SUB_1B_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x1B");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_3, APPL_MOD_3_SUB_1C, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_3_SUB_1C_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x1C");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_3, APPL_MOD_3_SUB_1D, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_3_SUB_1D_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x1D");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_3, APPL_MOD_3_SUB_1E, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_3_SUB_1E_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x1E");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_3, APPL_MOD_3_SUB_1F, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_3_SUB_1F_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x1F");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_3, APPL_MOD_3_SUB_20, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_3_SUB_20_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x20");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_3, APPL_MOD_3_SUB_21, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_3_SUB_21_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x21");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_4, APPL_MOD_4_SUB_01, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_4_SUB_01_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x01");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_4, APPL_MOD_4_SUB_02, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_4_SUB_02_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x02");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_4, APPL_MOD_4_SUB_03, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_4_SUB_03_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x03");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_4, APPL_MOD_4_SUB_04, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_4_SUB_04_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x04");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_4, APPL_MOD_4_SUB_05, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_4_SUB_05_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x05");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_4, APPL_MOD_4_SUB_06, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_4_SUB_06_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x06");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_4, APPL_MOD_4_SUB_07, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_4_SUB_07_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x07");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_4, APPL_MOD_4_SUB_08, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_4_SUB_08_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x08");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_4, APPL_MOD_4_SUB_09, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_4_SUB_09_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x09");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_4, APPL_MOD_4_SUB_0A, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_4_SUB_0A_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x0A");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_4, APPL_MOD_4_SUB_0B, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_4_SUB_0B_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x0B");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_4, APPL_MOD_4_SUB_0C, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_4_SUB_0C_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x0C");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_4, APPL_MOD_4_SUB_0D, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_4_SUB_0D_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x0D");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_4, APPL_MOD_4_SUB_0E, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_4_SUB_0E_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x0E");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_4, APPL_MOD_4_SUB_0F, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_4_SUB_0F_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x0F");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_4, APPL_MOD_4_SUB_10, GOAL_PNIO_MOD_TYPE_INPUT, APPL_SIZE_4_SUB_10_IN, 0, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x10");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_5, APPL_MOD_5_SUB_01, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_5_SUB_01_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x01");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_5, APPL_MOD_5_SUB_02, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_5_SUB_02_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x02");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_5, APPL_MOD_5_SUB_03, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_5_SUB_03_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x03");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_5, APPL_MOD_5_SUB_04, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_5_SUB_04_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x04");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_5, APPL_MOD_5_SUB_05, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_5_SUB_05_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x05");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_5, APPL_MOD_5_SUB_06, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_5_SUB_06_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x06");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_5, APPL_MOD_5_SUB_07, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_5_SUB_07_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x07");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_5, APPL_MOD_5_SUB_08, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_5_SUB_08_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x08");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_5, APPL_MOD_5_SUB_09, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_5_SUB_09_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x09");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_5, APPL_MOD_5_SUB_0A, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_5_SUB_0A_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x0A");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_5, APPL_MOD_5_SUB_0B, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_5_SUB_0B_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x0B");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_5, APPL_MOD_5_SUB_0C, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_5_SUB_0C_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x0C");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_5, APPL_MOD_5_SUB_0D, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_5_SUB_0D_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x0D");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_5, APPL_MOD_5_SUB_0E, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_5_SUB_0E_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x0E");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_5, APPL_MOD_5_SUB_0F, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_5_SUB_0F_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x0F");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_5, APPL_MOD_5_SUB_10, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_5_SUB_10_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x10");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_5, APPL_MOD_5_SUB_11, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_5_SUB_11_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x11");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_5, APPL_MOD_5_SUB_12, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_5_SUB_12_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x12");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_5, APPL_MOD_5_SUB_13, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_5_SUB_13_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x13");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_5, APPL_MOD_5_SUB_14, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_5_SUB_14_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x14");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_5, APPL_MOD_5_SUB_15, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_5_SUB_15_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x15");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_5, APPL_MOD_5_SUB_16, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_5_SUB_16_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x16");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_5, APPL_MOD_5_SUB_17, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_5_SUB_17_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x17");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_5, APPL_MOD_5_SUB_18, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_5_SUB_18_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x18");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_5, APPL_MOD_5_SUB_19, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_5_SUB_19_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x19");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_5, APPL_MOD_5_SUB_1A, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_5_SUB_1A_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x1A");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_5, APPL_MOD_5_SUB_1B, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_5_SUB_1B_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x1B");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_5, APPL_MOD_5_SUB_1C, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_5_SUB_1C_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x1C");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_5, APPL_MOD_5_SUB_1D, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_5_SUB_1D_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x1D");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_5, APPL_MOD_5_SUB_1E, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_5_SUB_1E_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x1E");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_5, APPL_MOD_5_SUB_1F, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_5_SUB_1F_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x1F");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_5, APPL_MOD_5_SUB_20, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_5_SUB_20_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x20");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_5, APPL_MOD_5_SUB_21, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_5_SUB_21_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x21");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_6, APPL_MOD_6_SUB_01, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_6_SUB_01_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x01");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_6, APPL_MOD_6_SUB_02, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_6_SUB_02_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x02");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_6, APPL_MOD_6_SUB_03, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_6_SUB_03_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x03");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_6, APPL_MOD_6_SUB_04, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_6_SUB_04_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x04");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_6, APPL_MOD_6_SUB_05, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_6_SUB_05_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x05");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_6, APPL_MOD_6_SUB_06, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_6_SUB_06_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x06");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_6, APPL_MOD_6_SUB_07, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_6_SUB_07_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x07");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_6, APPL_MOD_6_SUB_08, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_6_SUB_08_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x08");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_6, APPL_MOD_6_SUB_09, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_6_SUB_09_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x09");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_6, APPL_MOD_6_SUB_0A, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_6_SUB_0A_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x0A");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_6, APPL_MOD_6_SUB_0B, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_6_SUB_0B_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x0B");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_6, APPL_MOD_6_SUB_0C, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_6_SUB_0C_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x0C");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_6, APPL_MOD_6_SUB_0D, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_6_SUB_0D_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x0D");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_6, APPL_MOD_6_SUB_0E, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_6_SUB_0E_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x0E");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_6, APPL_MOD_6_SUB_0F, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_6_SUB_0F_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x0F");
        return res;
    }

    res = goal_pnioSubmodNew(pPnio, APPL_MOD_6, APPL_MOD_6_SUB_10, GOAL_PNIO_MOD_TYPE_OUTPUT, 0, APPL_SIZE_6_SUB_10_OUT, GOAL_PNIO_FLG_AUTO_GEN);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to add submodule 0x10");
        return res;
    }


    /* plug modules into slots */
    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_01, APPL_MOD_3, APPL_MOD_3_SUB_01);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x01");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_02, APPL_MOD_3, APPL_MOD_3_SUB_02);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x02");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_03, APPL_MOD_3, APPL_MOD_3_SUB_03);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x03");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_04, APPL_MOD_3, APPL_MOD_3_SUB_04);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x04");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_05, APPL_MOD_3, APPL_MOD_3_SUB_05);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x05");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_06, APPL_MOD_3, APPL_MOD_3_SUB_06);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x06");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_07, APPL_MOD_3, APPL_MOD_3_SUB_07);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x07");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_08, APPL_MOD_3, APPL_MOD_3_SUB_08);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x08");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_09, APPL_MOD_3, APPL_MOD_3_SUB_09);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x09");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_0A, APPL_MOD_3, APPL_MOD_3_SUB_0A);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x0A");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_0B, APPL_MOD_3, APPL_MOD_3_SUB_0B);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x0B");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_0C, APPL_MOD_3, APPL_MOD_3_SUB_0C);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x0C");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_0D, APPL_MOD_3, APPL_MOD_3_SUB_0D);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x0D");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_0E, APPL_MOD_3, APPL_MOD_3_SUB_0E);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x0E");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_0F, APPL_MOD_3, APPL_MOD_3_SUB_0F);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x0F");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_10, APPL_MOD_3, APPL_MOD_3_SUB_10);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x10");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_11, APPL_MOD_3, APPL_MOD_3_SUB_11);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x11");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_12, APPL_MOD_3, APPL_MOD_3_SUB_12);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x12");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_13, APPL_MOD_3, APPL_MOD_3_SUB_13);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x13");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_14, APPL_MOD_3, APPL_MOD_3_SUB_14);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x14");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_15, APPL_MOD_3, APPL_MOD_3_SUB_15);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x15");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_16, APPL_MOD_3, APPL_MOD_3_SUB_16);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x16");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_17, APPL_MOD_3, APPL_MOD_3_SUB_17);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x17");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_18, APPL_MOD_3, APPL_MOD_3_SUB_18);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x18");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_19, APPL_MOD_3, APPL_MOD_3_SUB_19);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x19");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_1A, APPL_MOD_3, APPL_MOD_3_SUB_1A);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x1A");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_1B, APPL_MOD_3, APPL_MOD_3_SUB_1B);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x1B");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_1C, APPL_MOD_3, APPL_MOD_3_SUB_1C);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x1C");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_1D, APPL_MOD_3, APPL_MOD_3_SUB_1D);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x1D");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_1E, APPL_MOD_3, APPL_MOD_3_SUB_1E);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x1E");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_1F, APPL_MOD_3, APPL_MOD_3_SUB_1F);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x1F");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_20, APPL_MOD_3, APPL_MOD_3_SUB_20);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x20");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_21, APPL_MOD_3, APPL_MOD_3_SUB_21);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x21");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_01, APPL_MOD_4, APPL_MOD_4_SUB_01);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x01");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_02, APPL_MOD_4, APPL_MOD_4_SUB_02);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x02");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_03, APPL_MOD_4, APPL_MOD_4_SUB_03);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x03");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_04, APPL_MOD_4, APPL_MOD_4_SUB_04);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x04");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_05, APPL_MOD_4, APPL_MOD_4_SUB_05);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x05");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_06, APPL_MOD_4, APPL_MOD_4_SUB_06);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x06");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_07, APPL_MOD_4, APPL_MOD_4_SUB_07);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x07");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_08, APPL_MOD_4, APPL_MOD_4_SUB_08);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x08");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_09, APPL_MOD_4, APPL_MOD_4_SUB_09);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x09");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_0A, APPL_MOD_4, APPL_MOD_4_SUB_0A);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x0A");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_0B, APPL_MOD_4, APPL_MOD_4_SUB_0B);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x0B");
        return res;
    }
    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_0C, APPL_MOD_4, APPL_MOD_4_SUB_0C);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x0C");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_0D, APPL_MOD_4, APPL_MOD_4_SUB_0D);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x0D");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_0E, APPL_MOD_4, APPL_MOD_4_SUB_0E);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x0E");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_0F, APPL_MOD_4, APPL_MOD_4_SUB_0F);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x0F");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_10, APPL_MOD_4, APPL_MOD_4_SUB_10);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x10");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_01, APPL_MOD_5, APPL_MOD_5_SUB_01);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x01");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_02, APPL_MOD_5, APPL_MOD_5_SUB_02);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x02");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_03, APPL_MOD_5, APPL_MOD_5_SUB_03);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x03");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_04, APPL_MOD_5, APPL_MOD_5_SUB_04);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x04");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_05, APPL_MOD_5, APPL_MOD_5_SUB_05);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x05");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_06, APPL_MOD_5, APPL_MOD_5_SUB_06);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x06");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_07, APPL_MOD_5, APPL_MOD_5_SUB_07);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x07");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_08, APPL_MOD_5, APPL_MOD_5_SUB_08);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x08");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_09, APPL_MOD_5, APPL_MOD_5_SUB_09);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x09");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_0A, APPL_MOD_5, APPL_MOD_5_SUB_0A);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x0A");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_0B, APPL_MOD_5, APPL_MOD_5_SUB_0B);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x0B");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_0C, APPL_MOD_5, APPL_MOD_5_SUB_0C);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x0C");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_0D, APPL_MOD_5, APPL_MOD_5_SUB_0D);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x0D");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_0E, APPL_MOD_5, APPL_MOD_5_SUB_0E);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x0E");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_0F, APPL_MOD_5, APPL_MOD_5_SUB_0F);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x0F");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_10, APPL_MOD_5, APPL_MOD_5_SUB_10);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x10");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_11, APPL_MOD_5, APPL_MOD_5_SUB_11);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x11");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_12, APPL_MOD_5, APPL_MOD_5_SUB_12);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x12");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_13, APPL_MOD_5, APPL_MOD_5_SUB_13);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x13");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_14, APPL_MOD_5, APPL_MOD_5_SUB_14);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x14");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_15, APPL_MOD_5, APPL_MOD_5_SUB_15);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x15");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_16, APPL_MOD_5, APPL_MOD_5_SUB_16);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x16");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_17, APPL_MOD_5, APPL_MOD_5_SUB_17);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x17");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_18, APPL_MOD_5, APPL_MOD_5_SUB_18);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x18");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_19, APPL_MOD_5, APPL_MOD_5_SUB_19);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x19");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_1A, APPL_MOD_5, APPL_MOD_5_SUB_1A);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x1A");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_1B, APPL_MOD_5, APPL_MOD_5_SUB_1B);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x1B");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_1C, APPL_MOD_5, APPL_MOD_5_SUB_1C);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x1C");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_1D, APPL_MOD_5, APPL_MOD_5_SUB_1D);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x1D");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_1E, APPL_MOD_5, APPL_MOD_5_SUB_1E);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x1E");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_1F, APPL_MOD_5, APPL_MOD_5_SUB_1F);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x1F");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_20, APPL_MOD_5, APPL_MOD_5_SUB_20);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x20");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_21, APPL_MOD_5, APPL_MOD_5_SUB_21);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x21");
        return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_4, APPL_SLOT_4_SUB_01, APPL_MOD_6, APPL_MOD_6_SUB_01);
    if (GOAL_RES_ERR(res)) {
        goal_logErr("failed to plug submodule 0x01");
        return res;
    }
    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_4, APPL_SLOT_4_SUB_02, APPL_MOD_6, APPL_MOD_6_SUB_02);
    if (GOAL_RES_ERR(res)) {
	goal_logErr("failed to plug submodule 0x02");
	return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_4, APPL_SLOT_4_SUB_03, APPL_MOD_6, APPL_MOD_6_SUB_03);
    if (GOAL_RES_ERR(res)) {
	goal_logErr("failed to plug submodule 0x03");
	return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_4, APPL_SLOT_4_SUB_04, APPL_MOD_6, APPL_MOD_6_SUB_04);
    if (GOAL_RES_ERR(res)) {
	goal_logErr("failed to plug submodule 0x04");
	return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_4, APPL_SLOT_4_SUB_05, APPL_MOD_6, APPL_MOD_6_SUB_05);
    if (GOAL_RES_ERR(res)) {
	goal_logErr("failed to plug submodule 0x05");
	return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_4, APPL_SLOT_4_SUB_06, APPL_MOD_6, APPL_MOD_6_SUB_06);
    if (GOAL_RES_ERR(res)) {
	goal_logErr("failed to plug submodule 0x06");
	return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_4, APPL_SLOT_4_SUB_07, APPL_MOD_6, APPL_MOD_6_SUB_07);
    if (GOAL_RES_ERR(res)) {
	goal_logErr("failed to plug submodule 0x07");
	return res;
    }
    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_4, APPL_SLOT_4_SUB_08, APPL_MOD_6, APPL_MOD_6_SUB_08);
    if (GOAL_RES_ERR(res)) {
	goal_logErr("failed to plug submodule 0x08");
	return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_4, APPL_SLOT_4_SUB_09, APPL_MOD_6, APPL_MOD_6_SUB_09);
    if (GOAL_RES_ERR(res)) {
	goal_logErr("failed to plug submodule 0x09");
	return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_4, APPL_SLOT_4_SUB_0A, APPL_MOD_6, APPL_MOD_6_SUB_0A);
    if (GOAL_RES_ERR(res)) {
	goal_logErr("failed to plug submodule 0x0A");
	return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_4, APPL_SLOT_4_SUB_0B, APPL_MOD_6, APPL_MOD_6_SUB_0B);
    if (GOAL_RES_ERR(res)) {
	goal_logErr("failed to plug submodule 0x0B");
	return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_4, APPL_SLOT_4_SUB_0C, APPL_MOD_6, APPL_MOD_6_SUB_0C);
    if (GOAL_RES_ERR(res)) {
	goal_logErr("failed to plug submodule 0x0C");
	return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_4, APPL_SLOT_4_SUB_0D, APPL_MOD_6, APPL_MOD_6_SUB_0D);
    if (GOAL_RES_ERR(res)) {
	goal_logErr("failed to plug submodule 0x0D");
	return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_4, APPL_SLOT_4_SUB_0E, APPL_MOD_6, APPL_MOD_6_SUB_0E);
    if (GOAL_RES_ERR(res)) {
	goal_logErr("failed to plug submodule 0x0E");
	return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_4, APPL_SLOT_4_SUB_0F, APPL_MOD_6, APPL_MOD_6_SUB_0F);
    if (GOAL_RES_ERR(res)) {
	goal_logErr("failed to plug submodule 0x0F");
	return res;
    }

    res = goal_pnioSubmodPlug(pPnio, APPL_API, APPL_SLOT_4, APPL_SLOT_4_SUB_10, APPL_MOD_6, APPL_MOD_6_SUB_10);
    if (GOAL_RES_ERR(res)) {
	goal_logErr("failed to plug submodule 0x10");
	return res;
    }


    /* PROFINET configuration succesful */
    goal_logInfo("PROFINET ready");

    /**-- ICC_PNIO_GEN_APPLSETUP_AFTER_START --**/
    /* user code block */
    /* init trap incl. sending cold start trap */
    res = goal_snmpTrapsInit(mpInstanceSnmp, APPL_ADDR_TRAPSINK);
    if (GOAL_RES_ERR(res)) {
	    goal_logErr("Failed to init SNMP traps");
	    return res;
    }
    /**-- ICC_PNIO_GEN_APPLSETUP_AFTER_END --**/
    return res;
}


/****************************************************************************/
/** Profinet Callback Handler
 *
 * This function collects all callbacks from the stack and decides if the
 * callback must be handled.
 *
 * @retval GOAL_OK - successful
 */
static GOAL_STATUS_T main_callback(
    GOAL_PNIO_T *pHdlPnio,                      /**< PROFINET handle */
    GOAL_PNIO_CB_ID_T id,                       /**< callback id */
    GOAL_PNIO_CB_DATA_T *pCb                    /**< callback parameters */
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* return value */

    UNUSEDARG(pHdlPnio);

    /* handle callback IDs */
    switch (id) {

        case GOAL_PNIO_CB_ID_APPL_READY:
            /* application ready was confirmed, start data handling if not
             * already running */
            if (GOAL_TRUE != flgAppReady) {
                flgAppReady = GOAL_TRUE;
                idAr = pCb->data[0].idAr;
            }
            break;

        case GOAL_PNIO_CB_ID_RELEASE_AR:
            /* AR was released, stop data handling if it was the first AR */
            if (idAr == pCb->data[0].idAr) {
                flgAppReady = GOAL_FALSE;
                idAr = 0;
            }
            break;

        case GOAL_PNIO_CB_ID_BLINK:
            goal_targetSetLeds((pCb->data[1].stateDcpLight) ? UINT32_MAX : 0);
            break;

        case GOAL_PNIO_CB_ID_READ_RECORD:
            return appl_pnioCbReadRecord(pHdlPnio, pCb);

        case GOAL_PNIO_CB_ID_WRITE_RECORD:
            return appl_pnioCbWriteRecord(pHdlPnio, pCb);

        default:
            break;
    }

    return res;
}


/****************************************************************************/
/** GOAL PROFINET Callback - Read Record Data
 *
 * Indicates that record data that couldn't be handled by the stack itself
 * should be read. Before this callback is called, the stack checks if the
 * API, slot and subslot combination is valid.
 * If the function returns GOAL_OK the request will be denied with
 * "invalid index".
 * To handle the request the callback needs to return GOAL_OK_SUPPORTED.
 * It can either respond directly or later by calling goal_pnioRecReadFinish.
 * Also the now PROFINET status can be set through goal_pnioRecReadFinish.
 * If the response isn't GOAL_OK or GOAL_OK_SUPPORTED the stack will
 * automatically respond with application read error.
 * The parameter "sequence number" is used later when the response is send
 * to detect if the request has expired.
 *
 * Callback id: GOAL_PNIO_CB_ID_READ_RECORD
 *
 * @retval GOAL_OK - request denied
 * @retval GOAL_OK_SUPPORTED - handle request
 */
static GOAL_STATUS_T appl_pnioCbReadRecord(
    GOAL_PNIO_T *pPnio,                         /**< PROFINET handle */
    GOAL_PNIO_CB_DATA_T *pCb                    /**< callback parameters */
)
{
    /* Callback data:
     *
     *
     * pCb->data[0].pStatus unused (pointer to PROFINET status)
     * pCb->data[1].idAr    application relation id
     * pCb->data[2].u32     API
     * pCb->data[3].u16     slot
     * pCb->data[4].u16     subslot
     * pCb->data[5].u16     record index
     * pCb->data[6].pU8     unused (pointer to store record data)
     * pCb->data[7].u32     maximum record read data length
     * pCb->data[8].i32     record busy index for delaying
     * pCb->data[9].u32     sequence number
     */
     
    GOAL_STATUS_T res = GOAL_OK;                /* return value */

    /**-- ICC_PNIO_GEN_CB_READRECORD_START --**/
    /* user code block */
    /**-- ICC_PNIO_GEN_CB_READRECORD_END --**/
     
    return res;
}


/****************************************************************************/
/** GOAL PROFINET Callback - Write Record Data
 *
 * Indicates that record data that couldn't be handled by the stack
 * itself should be written. Before this callback is called, the
 * stack checks if the API, slot and subslot combination is valid.
 * If the function returns GOAL_OK the request will be denied with
 * "invalid index". To handle the request the callback needs to
 * return GOAL_OK_SUPPORTED. It can either respond directly or
 * later by calling goal_pnioRecWriteFinish. Also the now PROFINET
 * status can be set through goal_pnioRecWriteFinish. If the
 * response isn't GOAL_OK or GOAL_OK_SUPPORTED the stack will
 * automatically respond with application read error.
 * The parameter "sequence number" is used later when the response
 * is send to detect if the request has expired.
 *
 * Callback id: GOAL_PNIO_CB_ID_WRITE_RECORD
 *
 * @retval GOAL_OK - request denied
 * @retval GOAL_OK_SUPPORTED - handle request
 */
static GOAL_STATUS_T appl_pnioCbWriteRecord(
    GOAL_PNIO_T *pPnio,                         /**< PROFINET handle */
    GOAL_PNIO_CB_DATA_T *pCb                    /**< callback parameters */
)
{
    /* Callback data:
     *
     *
     * pCb->data[0].pStatus unused (pointer to PROFINET status)
     * pCb->data[1].idAr    application relation id
     * pCb->data[2].u32     API
     * pCb->data[3].u16     slot
     * pCb->data[4].u16     subslot
     * pCb->data[5].u16     record index
     * pCb->data[6].pCu8    pointer to read record data from
     * pCb->data[7].u32     record data length
     * pCb->data[8].i32     record busy index for delaying
     * pCb->data[9].valBool subslot locked status flag
     * pCb->data[10].u32    sequence numbers
     */

    GOAL_STATUS_T res = GOAL_OK;                /* return value */
    /**-- ICC_PNIO_GEN_CB_WRITERECORD_START --**/
    /* user code block */
    /**-- ICC_PNIO_GEN_CB_WRITERECORD_END --**/

    return res;
}


/****************************************************************************/
/** Application Loop
 *
 * This function must implement the application logic and must not block. It is
 * called in a loop (the GOAL loop) and if state tracking is necessary it
 * should use static or global variables.
 */
void appl_loop(
    void
)
{
    /**-- ICC_PNIO_GEN_APPLLOOP_START --**/
    GOAL_STATUS_T res;                          /* result */
    GOAL_TIMESTAMP_T tsCur;                     /* current timestamp */
    uint8_t iops;                               /* IO producer status */

    /* get current timestamp */
    tsCur = goal_timerTsGet();
    /* user code block */
    if ((GOAL_TRUE == flgAppReady) && (mTsTout <= tsCur)) {
#if RIOP

	uint8_t di;
	uint16_t * pi;
	uint8_t * pii;
	/*Local Variables to pass AFE digital inputs.*/
	uint8_t  di1_in, di2_in,di3_in,di4_in,di5_in,di6_in,di7_in,di8_in;
	uint8_t  di9_in, di10_in,di11_in,di12_in,di13_in,di14_in,di15_in;
	uint8_t  di17_in,di18_in,di19_in,di20_in,di21_in,di22_in,di23_in,di24_in;

	uint8_t  di1_out, di2_out,di3_out,di4_out,di5_out,di6_out,di7_out,di8_out;
	uint8_t di17_out,di18_out,di19_out,di20_out,di21_out,di22_out,di23_out,di24_out;
	uint8_t di25_out,di26_out,di27_out,di28_out,di29_out,di30_out,di31_out,di32_out;

	di=0;
	/*AFE Digital Inputs*/
	pi=&g_boardStatus.afe_status.afe_gpio_input_pins_status;
#define ATOMIC_OP(var, XXX) if(*pi&(uint16_t)XXX) var=1; else var=0
	ATOMIC_OP(di1_in, 0x0004);
	/* copy  to input module */
	res = goal_pnioDataInputSet(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_01, (char *) &di1_in, APPL_SIZE_3_SUB_01_IN, GOAL_PNIO_IOXS_GOOD);
	if (GOAL_RES_ERR(res)) {
	    return;
        }

        ATOMIC_OP(di2_in, 0x0008);
        res = goal_pnioDataInputSet(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_02, (char *) &di2_in, APPL_SIZE_3_SUB_02_IN, GOAL_PNIO_IOXS_GOOD);
        if (GOAL_RES_ERR(res)) {
            return;
        }

        ATOMIC_OP(di3_in, 0x0010);
        res = goal_pnioDataInputSet(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_03, (char *) &di3_in, APPL_SIZE_3_SUB_03_IN, GOAL_PNIO_IOXS_GOOD);
        if (GOAL_RES_ERR(res)) {
            return;
        }

        ATOMIC_OP(di4_in, 0x0020);
        res = goal_pnioDataInputSet(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_04, (char *) &di4_in, APPL_SIZE_3_SUB_04_IN, GOAL_PNIO_IOXS_GOOD);
        if (GOAL_RES_ERR(res)) {
            return;
        }

        ATOMIC_OP(di5_in, 0x0040);
        res = goal_pnioDataInputSet(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_05, (char *) &di5_in, APPL_SIZE_3_SUB_05_IN, GOAL_PNIO_IOXS_GOOD);
        if (GOAL_RES_ERR(res)) {
            return;
        }

        ATOMIC_OP(di6_in, 0x0080);
        res = goal_pnioDataInputSet(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_06, (char *) &di6_in, APPL_SIZE_3_SUB_06_IN, GOAL_PNIO_IOXS_GOOD);
        if (GOAL_RES_ERR(res)) {
            return;
        }

        ATOMIC_OP(di7_in, 0x0100);
        res = goal_pnioDataInputSet(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_07, (char *) &di7_in, APPL_SIZE_3_SUB_07_IN, GOAL_PNIO_IOXS_GOOD);
        if (GOAL_RES_ERR(res)) {
            return;
        }

        ATOMIC_OP(di8_in, 0x0200);
        res = goal_pnioDataInputSet(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_08, (char *) &di8_in, APPL_SIZE_3_SUB_08_IN, GOAL_PNIO_IOXS_GOOD);
        if (GOAL_RES_ERR(res)) {
            return;
        }
#undef ATOMIC_OP

#define ATOMIC_OP(var, XXX) if(*pii&(uint8_t)XXX) var=1; else var=0
        pii=&g_boardStatus.mcu_digital_input_pins_status;
        ATOMIC_OP(di9_in, 0x01);
        res = goal_pnioDataInputSet(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_09, (char *) &di9_in, APPL_SIZE_3_SUB_09_IN, GOAL_PNIO_IOXS_GOOD);
        if (GOAL_RES_ERR(res)) {
            return;
        }
        ATOMIC_OP(di10_in, 0x02);
        res = goal_pnioDataInputSet(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_0A, (char *) &di10_in, APPL_SIZE_3_SUB_0A_IN, GOAL_PNIO_IOXS_GOOD);
        if (GOAL_RES_ERR(res)) {
            return;
        }
        ATOMIC_OP(di11_in, 0x04);
        res = goal_pnioDataInputSet(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_0B, (char *) &di11_in, APPL_SIZE_3_SUB_0B_IN, GOAL_PNIO_IOXS_GOOD);
        if (GOAL_RES_ERR(res)) {
            return;
        }
        ATOMIC_OP(di12_in, 0x08);
        res = goal_pnioDataInputSet(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_0C, (char *) &di12_in, APPL_SIZE_3_SUB_0C_IN, GOAL_PNIO_IOXS_GOOD);
        if (GOAL_RES_ERR(res)) {
            return;
        }

        ATOMIC_OP(di13_in, 0x10);
        res = goal_pnioDataInputSet(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_0D, (char *) &di13_in, APPL_SIZE_3_SUB_0D_IN, GOAL_PNIO_IOXS_GOOD);
        if (GOAL_RES_ERR(res)) {
            return;
        }
        ATOMIC_OP(di14_in, 0x20);
        res = goal_pnioDataInputSet(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_0E, (char *) &di14_in, APPL_SIZE_3_SUB_0E_IN, GOAL_PNIO_IOXS_GOOD);
        if (GOAL_RES_ERR(res)) {
            return;
        }
        ATOMIC_OP(di15_in, 0x40);
        res = goal_pnioDataInputSet(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_0F, (char *) &di15_in, APPL_SIZE_3_SUB_0F_IN, GOAL_PNIO_IOXS_GOOD);
        if (GOAL_RES_ERR(res)) {
            return;
        }

	res = goal_pnioDataInputSet(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_10, (char *) &di, APPL_SIZE_3_SUB_10_IN, GOAL_PNIO_IOXS_GOOD);
        if (GOAL_RES_ERR(res)) {
            return;
        }

        pii=&g_boardStatus.mcu_digital_output_pins_status;

        ATOMIC_OP(di17_in, 0x01);
        res = goal_pnioDataInputSet(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_11, (char *) &di17_in, APPL_SIZE_3_SUB_11_IN, GOAL_PNIO_IOXS_GOOD);
        if (GOAL_RES_ERR(res)) {
            return;
        }
        ATOMIC_OP(di18_in, 0x02);
        res = goal_pnioDataInputSet(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_12, (char *) &di18_in, APPL_SIZE_3_SUB_12_IN, GOAL_PNIO_IOXS_GOOD);
        if (GOAL_RES_ERR(res)) {
            return;
        }
        ATOMIC_OP(di19_in, 0x04);
        res = goal_pnioDataInputSet(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_13, (char *) &di19_in, APPL_SIZE_3_SUB_13_IN, GOAL_PNIO_IOXS_GOOD);
        if (GOAL_RES_ERR(res)) {
            return;
        }
        ATOMIC_OP(di20_in, 0x08);
        res = goal_pnioDataInputSet(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_14, (char *) &di20_in, APPL_SIZE_3_SUB_14_IN, GOAL_PNIO_IOXS_GOOD);
        if (GOAL_RES_ERR(res)) {
            return;
        }

        ATOMIC_OP(di21_in, 0x10);
        res = goal_pnioDataInputSet(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_15, (char *) &di21_in, APPL_SIZE_3_SUB_15_IN, GOAL_PNIO_IOXS_GOOD);
        if (GOAL_RES_ERR(res)) {
            return;
        }
        ATOMIC_OP(di22_in, 0x20);
        res = goal_pnioDataInputSet(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_16, (char *) &di22_in, APPL_SIZE_3_SUB_16_IN, GOAL_PNIO_IOXS_GOOD);
        if (GOAL_RES_ERR(res)) {
            return;
        }
        ATOMIC_OP(di23_in, 0x40);
        res = goal_pnioDataInputSet(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_17, (char *) &di23_in, APPL_SIZE_3_SUB_17_IN, GOAL_PNIO_IOXS_GOOD);
        if (GOAL_RES_ERR(res)) {
            return;
        }
        ATOMIC_OP(di24_in, 0x80);
        res = goal_pnioDataInputSet(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_18, (char *) &di24_in, APPL_SIZE_3_SUB_18_IN, GOAL_PNIO_IOXS_GOOD);
        if (GOAL_RES_ERR(res)) {
            return;
        }
#undef ATOMIC_OP

        //LED: Fetch LED status

        res = goal_pnioDataInputSet(pPnio, APPL_API, APPL_SLOT_1, APPL_SLOT_1_SUB_21, (char *) &LED_status, APPL_SIZE_3_SUB_21_IN, GOAL_PNIO_IOXS_GOOD);
        if (GOAL_RES_ERR(res)) {
            return;
        }

        {
                uint32_t tt;
                tt=(adc_1);
		res = goal_pnioDataInputSet(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_01, (char *) &tt, APPL_SIZE_4_SUB_01_IN, GOAL_PNIO_IOXS_GOOD);
		if (GOAL_RES_ERR(res)) {
			return;
		}
		res = goal_pnioDataInputSet(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_02, (char *) &adc_1, APPL_SIZE_4_SUB_02_IN, GOAL_PNIO_IOXS_GOOD);
		if (GOAL_RES_ERR(res)) {
			return;
		}
	}
        {
                int32_t tt;
                tt=(adc_1);
                tt=-tt;
		res = goal_pnioDataInputSet(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_03, (char *) &tt, APPL_SIZE_4_SUB_03_IN, GOAL_PNIO_IOXS_GOOD);
		if (GOAL_RES_ERR(res)) {
			return;
		}
		res = goal_pnioDataInputSet(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_04, (char *) &tt, APPL_SIZE_4_SUB_04_IN, GOAL_PNIO_IOXS_GOOD);
		if (GOAL_RES_ERR(res)) {
			return;
		}
        }

        {
                float * pf;
#if PNET_CONVERT_USRDATA
                uint32_t tt;
#else
                float tt;
#endif
                tt=(adc_1);
                pf=&(g_boardStatus.afe_status.hvsig_status.channel_value[0]);
		PNET_COPY_USR_DATA(tt,pf);
		res = goal_pnioDataInputSet(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_05, (char *) &tt, APPL_SIZE_4_SUB_05_IN, GOAL_PNIO_IOXS_GOOD);
		if (GOAL_RES_ERR(res)) {
			return;
		}
		PNET_COPY_USR_DATA(tt,pf);
		res = goal_pnioDataInputSet(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_06, (char *) &tt, APPL_SIZE_4_SUB_06_IN, GOAL_PNIO_IOXS_GOOD);
		if (GOAL_RES_ERR(res)) {
			return;
		}
		PNET_COPY_USR_DATA(tt,pf);
		res = goal_pnioDataInputSet(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_07, (char *) &tt, APPL_SIZE_4_SUB_07_IN, GOAL_PNIO_IOXS_GOOD);
		if (GOAL_RES_ERR(res)) {
			return;
		}

                pf=&(g_boardStatus.afe_status.current_status.channel_value);
		PNET_COPY_USR_DATA(tt,pf);
		res = goal_pnioDataInputSet(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_08, (char *) &tt, APPL_SIZE_4_SUB_08_IN, GOAL_PNIO_IOXS_GOOD);
		if (GOAL_RES_ERR(res)) {
			return;
		}

                pf=&(g_boardStatus.afe_status.lvsig_status.channel_mean[0]);
		PNET_COPY_USR_DATA(tt,pf);
		res = goal_pnioDataInputSet(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_09, (char *) &tt, APPL_SIZE_4_SUB_09_IN, GOAL_PNIO_IOXS_GOOD);
		if (GOAL_RES_ERR(res)) {
			return;
		}
		PNET_COPY_USR_DATA(tt,pf);
		res = goal_pnioDataInputSet(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_0A, (char *) &tt, APPL_SIZE_4_SUB_0A_IN, GOAL_PNIO_IOXS_GOOD);
		if (GOAL_RES_ERR(res)) {
			return;
		}
		PNET_COPY_USR_DATA(tt,pf);
		res = goal_pnioDataInputSet(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_0B, (char *) &tt, APPL_SIZE_4_SUB_0B_IN, GOAL_PNIO_IOXS_GOOD);
		if (GOAL_RES_ERR(res)) {
			return;
		}
		PNET_COPY_USR_DATA(tt,pf);
		res = goal_pnioDataInputSet(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_0C, (char *) &tt, APPL_SIZE_4_SUB_0C_IN, GOAL_PNIO_IOXS_GOOD);
		if (GOAL_RES_ERR(res)) {
			return;
		}
		PNET_COPY_USR_DATA(tt,pf);
		res = goal_pnioDataInputSet(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_0D, (char *) &tt, APPL_SIZE_4_SUB_0D_IN, GOAL_PNIO_IOXS_GOOD);
		if (GOAL_RES_ERR(res)) {
			return;
		}

                pf=&(g_boardStatus.afe_status.temperature_status.channel_value);
		PNET_COPY_USR_DATA(tt,pf);
		res = goal_pnioDataInputSet(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_0E, (char *) &tt, APPL_SIZE_4_SUB_0E_IN, GOAL_PNIO_IOXS_GOOD);
		if (GOAL_RES_ERR(res)) {
			return;
		}
		res = goal_pnioDataInputSet(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_0F, (char *) &tt, APPL_SIZE_4_SUB_0F_IN, GOAL_PNIO_IOXS_GOOD);
		if (GOAL_RES_ERR(res)) {
			return;
		}
		res = goal_pnioDataInputSet(pPnio, APPL_API, APPL_SLOT_2, APPL_SLOT_2_SUB_10, (char *) &tt, APPL_SIZE_4_SUB_10_IN, GOAL_PNIO_IOXS_GOOD);
		if (GOAL_RES_ERR(res)) {
			return;
		}
        }

	/*Ouputs*/
        pi=&riop_command.afe_command.afe_gpio_command.gpio_output_value;
#define ATOMIC_OP(var, XXX) if(var) *pi|=(uint16_t)XXX; else *pi&=~(uint16_t)XXX

        /* read data from output module */
        res = goal_pnioDataOutputGet(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_01, (char *) &di1_out, APPL_SIZE_5_SUB_01_OUT, &iops);
        if (GOAL_RES_ERR(res)) {
            return;
        }
        ATOMIC_OP(di1_out, 0x0004);

        res = goal_pnioDataOutputGet(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_02, (char *) &di2_out, APPL_SIZE_5_SUB_02_OUT, &iops);
        if (GOAL_RES_ERR(res)) {
            return;
        }
	ATOMIC_OP(di2_out, 0x0008);

        res = goal_pnioDataOutputGet(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_03, (char *) &di3_out, APPL_SIZE_5_SUB_03_OUT, &iops);
        if (GOAL_RES_ERR(res)) {
            return;
        }
        ATOMIC_OP(di3_out, 0x0010);

        res = goal_pnioDataOutputGet(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_04, (char *) &di4_out, APPL_SIZE_5_SUB_04_OUT, &iops);
        if (GOAL_RES_ERR(res)) {
            return;
        }
        ATOMIC_OP(di4_out, 0x0020);

        res = goal_pnioDataOutputGet(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_05, (char *) &di5_out, APPL_SIZE_5_SUB_05_OUT, &iops);
        if (GOAL_RES_ERR(res)) {
            return;
        }
        ATOMIC_OP(di5_out, 0x0040);

	res = goal_pnioDataOutputGet(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_06, (char *) &di6_out, APPL_SIZE_5_SUB_06_OUT, &iops);
        if (GOAL_RES_ERR(res)) {
            return;
        }
        ATOMIC_OP(di6_out, 0x0080);
	
	res = goal_pnioDataOutputGet(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_07, (char *) &di7_out, APPL_SIZE_5_SUB_07_OUT, &iops);
        if (GOAL_RES_ERR(res)) {
            return;
        }
        ATOMIC_OP(di7_out, 0x0100);

	res = goal_pnioDataOutputGet(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_08, (char *) &di8_out, APPL_SIZE_5_SUB_08_OUT, &iops);
        if (GOAL_RES_ERR(res)) {
            return;
        }
        ATOMIC_OP(di8_out, 0x0200);
#undef ATOMIC_OP
	
#define ATOMIC_OP(var, XXX) if(var) *pii|=(uint8_t)XXX; else *pii&=~(uint8_t)XXX
        pii=&riop_command.mcu_digital_output_command.mcu_digital_output;

	res = goal_pnioDataOutputGet(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_11, (char *) &di17_out, APPL_SIZE_5_SUB_11_OUT, &iops);
        if (GOAL_RES_ERR(res)) {
            return;
        }
        ATOMIC_OP(di17_out, 0x01);

	res = goal_pnioDataOutputGet(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_12, (char *) &di18_out, APPL_SIZE_5_SUB_12_OUT, &iops);
        if (GOAL_RES_ERR(res)) {
            return;
        }
        ATOMIC_OP(di18_out, 0x02);

	res = goal_pnioDataOutputGet(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_13, (char *) &di19_out, APPL_SIZE_5_SUB_13_OUT, &iops);
        if (GOAL_RES_ERR(res)) {
            return;
        }
        ATOMIC_OP(di19_out, 0x04);

	res = goal_pnioDataOutputGet(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_14, (char *) &di20_out, APPL_SIZE_5_SUB_14_OUT, &iops);
        if (GOAL_RES_ERR(res)) {
            return;
        }
        ATOMIC_OP(di20_out, 0x08);

	res = goal_pnioDataOutputGet(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_15, (char *) &di21_out, APPL_SIZE_5_SUB_15_OUT, &iops);
        if (GOAL_RES_ERR(res)) {
            return;
        }
        ATOMIC_OP(di21_out, 0x10);

	res = goal_pnioDataOutputGet(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_16, (char *) &di22_out, APPL_SIZE_5_SUB_16_OUT, &iops);
        if (GOAL_RES_ERR(res)) {
            return;
        }
        ATOMIC_OP(di22_out, 0x20);

	res = goal_pnioDataOutputGet(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_17, (char *) &di23_out, APPL_SIZE_5_SUB_17_OUT, &iops);
        if (GOAL_RES_ERR(res)) {
            return;
        }
        ATOMIC_OP(di23_out, 0x40);

	res = goal_pnioDataOutputGet(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_18, (char *) &di24_out, APPL_SIZE_5_SUB_18_OUT, &iops);
        if (GOAL_RES_ERR(res)) {
            return;
        }
        ATOMIC_OP(di24_out, 0x80);
#undef ATOMIC_OP

#define ATOMIC_OP(var, XXX) if(var) riop_command.afe_command.afe_gpio_command.gpio_pin_direction |=XXX; \
                else riop_command.afe_command.afe_gpio_command.gpio_pin_direction &=~(uint16_t)XXX;

	res = goal_pnioDataOutputGet(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_19, (char *) &di25_out, APPL_SIZE_5_SUB_19_OUT, &iops);
        if (GOAL_RES_ERR(res)) {
            return;
        }
        ATOMIC_OP(di25_out, 0x0004);

	res = goal_pnioDataOutputGet(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_1A, (char *) &di26_out, APPL_SIZE_5_SUB_1A_OUT, &iops);
        if (GOAL_RES_ERR(res)) {
            return;
        }
        ATOMIC_OP(di26_out, 0x0008);

	res = goal_pnioDataOutputGet(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_1B, (char *) &di27_out, APPL_SIZE_5_SUB_1B_OUT, &iops);
        if (GOAL_RES_ERR(res)) {
            return;
        }
        ATOMIC_OP(di27_out, 0x0010);

	res = goal_pnioDataOutputGet(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_1C, (char *) &di28_out, APPL_SIZE_5_SUB_1C_OUT, &iops);
        if (GOAL_RES_ERR(res)) {
            return;
        }
        ATOMIC_OP(di28_out, 0x0020);

	res = goal_pnioDataOutputGet(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_1D, (char *) &di29_out, APPL_SIZE_5_SUB_1D_OUT, &iops);
        if (GOAL_RES_ERR(res)) {
            return;
        }
        ATOMIC_OP(di29_out, 0x0040);

	res = goal_pnioDataOutputGet(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_1E, (char *) &di30_out, APPL_SIZE_5_SUB_1E_OUT, &iops);
        if (GOAL_RES_ERR(res)) {
            return;
        }
	ATOMIC_OP(di30_out, 0x0080);

	res = goal_pnioDataOutputGet(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_1F, (char *) &di31_out, APPL_SIZE_5_SUB_1F_OUT, &iops);
        if (GOAL_RES_ERR(res)) {
            return;
        }
        ATOMIC_OP(di31_out, 0x0100);

	res = goal_pnioDataOutputGet(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_20, (char *) &di32_out, APPL_SIZE_5_SUB_20_OUT, &iops);
        if (GOAL_RES_ERR(res)) {
            return;
        }
        ATOMIC_OP(di32_out, 0x0200);

	res = goal_pnioDataOutputGet(pPnio, APPL_API, APPL_SLOT_3, APPL_SLOT_3_SUB_21, (char *) &LED_status, APPL_SIZE_5_SUB_21_OUT, &iops);
        if (GOAL_RES_ERR(res)) {
            return;
        }
       /* set LED */
	led_green = LED_status;
	res = goal_pnioDataOutputGet(pPnio, APPL_API, APPL_SLOT_4, APPL_SLOT_4_SUB_01, (char *)&fsm_mode, APPL_SIZE_6_SUB_01_OUT, &iops);
	if (GOAL_RES_ERR(res)) {
	    return;
	}
	fsm_mode = GOAL_be32toh(fsm_mode);
#undef ATOMIC_OP

	/* update timeout value */
	mTsTout = goal_timerTsGet() + APPL_TIMEOUT_TRIGGER_VAL;

#endif
    }
    riop_appl_loop();
    /**-- ICC_PNIO_GEN_APPLLOOP_END --**/
}
