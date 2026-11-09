/** @file
 *
 * @brief Ethernet/IP application header
 *
 * This application defines some Assembly objects and connections. The first
 * byte of the Output Assembly is mapped to the LEDs. The first byte of the
 * Input Assembly contains a bitmap of the pressed buttons. Additionally, a
 * application specific CIP class is registered with parameters, listed in
 * the Instance Table.
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

#ifndef GOAL_APP_EIP_H
#define GOAL_APP_EIP_H


/****************************************************************************/
/* Local defines */
/****************************************************************************/
#define GOAL_APP_ASM_ID_INPUT        100        /**< Input Assembly ID */
#define GOAL_APP_ASM_ID_OUTPUT       150        /**< Output Assembly ID */
#define GOAL_APP_ASM_ID_CONFIG       151        /**< Config Assembly ID */
#define GOAL_APP_ASM_ID_HEARTBEAT_IO 152        /**< Heartbeat Assembly ID  (Input Only) */
#define GOAL_APP_ASM_ID_HEARTBEAT_LO 153        /**< Heartbeat Assembly ID  (Listen Only) */
#define GOAL_APP_ASM_ID_DCE_MIRROR_INPUT  170   /**< DCE mirror input assembly (RIOP -> scanner) */
#define GOAL_APP_ASM_ID_DCE_MIRROR_OUTPUT 171   /**< DCE write-mailbox output assembly (scanner -> RIOP) */
#define GOAL_APP_ASM_ID_DCE_FAST_INPUT   172   /**< DCE fast/compact input assembly (RIOP -> PC), explicit messaging */
#define GOAL_APP_ASM_ID_DCE_POLLSEL_OUTPUT 173 /**< DCE poll-set selector output assembly (PC -> RIOP), explicit messaging */
#define GOAL_APP_ASM_ID_NAFE_TEMP_INPUT  174  /**< NAFE temperature input assembly (RIOP -> PC), explicit messaging, read-only */
#define GOAL_APP_ASM_ID_NAFE_CFG_OUTPUT  175  /**< NAFE-only RAM config output (PC -> RIOP), DEBUG_NAFE builds only */
#define GOAL_APP_IOCON_NUM           6          /**< number of simultaneous connections for Input Only connections */
#define GOAL_APP_LOCON_NUM           6          /**< number of simultaneous connections for Listen Only connections */
#define GOAL_APP_ASM_SIZE_INPUT      97         /**< Input Assembly Data */
#define GOAL_APP_ASM_SIZE_OUTPUT     97         /**< Output Assembly Data */
#define GOAL_APP_ASM_SIZE_CONFIG     10         /**< Config Assembly Data */
#define GOAL_APP_DCE_MIRROR_REG_WORDS 124       /**< 8 FC03 block plan word count */
#define GOAL_APP_DCE_MIRROR_TAG_COUNT 106       /**< Number of mapped tags in dce_register_map.csv */
#define GOAL_APP_DCE_MIRROR_QUALITY_BYTES 14    /**< 106 bits rounded up to bytes */
#define GOAL_APP_DCE_MIRROR_MAX_WRITE_WORDS 2   /**< Largest mapped writable item is 2 words */
#define GOAL_APP_ASM_SIZE_DCE_MIRROR_INPUT 284  /**< Header + ack + quality + register image */
#define GOAL_APP_ASM_SIZE_DCE_FAST_INPUT 128    /**< asm 172: 96 B fastrun layout + 32 B diag */
#define GOAL_APP_ASM_SIZE_DCE_POLLSEL_OUTPUT 8  /**< asm 173: u16 sel_seq, u8 set, u8 flags, u32 reserved */
#define GOAL_APP_ASM_SIZE_NAFE_TEMP_INPUT TEMP_EIP_IN_LEN /**< asm 174: 96 B release + 136 B debug (DEBUG_NAFE) */
#define GOAL_APP_ASM_SIZE_NAFE_CFG_OUTPUT TEMP_EIP_CFG_LEN /**< asm 175: 32 B */
#define GOAL_APP_ASM_ID_NAFE_LOG_INPUT   176  /**< N9: NAFE lossless scan log (RIOP -> PC), explicit, read-only, DEBUG_NAFE */
#define GOAL_APP_ASM_SIZE_NAFE_LOG_INPUT TEMP_EIP_LOG_LEN /**< asm 176: 16 B header + 4 x 96 B records */
#define GOAL_APP_ASM_SIZE_DCE_MIRROR_OUTPUT 16  /**< seq/op/address/len/flags/values[2] */

#endif /* GOAL_APP_EIP_H */
