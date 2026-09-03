/** @file
 *
 * @brief GOAL NVS Driver Common Header
 *
 * @copyright
 * Copyright 2020.
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
#ifndef GOAL_DRV_NVS_H
#define GOAL_DRV_NVS_H

#include <goal_includes.h>


/****************************************************************************/
/* Pre-defined flash configurations */
/****************************************************************************/
/**< Cypress Semiconductor - S25FL032P */
#define GOAL_DRV_NVS_CHIP_CS_S25FL032P \
    "Cypress Semiconductor - S25FL032P",        /**< flash name */ \
    0x00400000,                                 /**< memory size: 4 MB */ \
    0x00010000,                                 /**< sector size: 64 kB */ \
    0x00000100,                                 /**< page size: 256 Byte */ \
    GOAL_TRUE                                   /**< memory mapped read: yes */

/**< Macronix - MX25L8006E */
#define GOAL_DRV_NVS_CHIP_MACRONIX_MX25L8006E \
    "Macronix - MX25L8006E",                    /**< flash name */ \
    0x00100000,                                 /**< memory size: 1 MB */ \
    0x00001000,                                 /**< sector size: 4 kB */ \
    0x00000100,                                 /**< page size: 256 Byte */ \
    GOAL_TRUE                                   /**< memory mapped read: yes */

/**< Macronix - MX25L4006E */
#define GOAL_DRV_NVS_CHIP_MACRONIX_MX25L4006E \
    "Macronix - MX25L4006E",                    /**< flash name */ \
    0x00080000,                                 /**< memory size: 512 kB */ \
    0x00001000,                                 /**< sector size: 4 kB */ \
    0x00000100,                                 /**< page size: 256 Byte */ \
    GOAL_TRUE                                   /**< memory mapped read: yes */

/**< Macronix - MX29GL512F */
#define GOAL_DRV_NVS_CHIP_MACRONIX_MX29GL512F \
    "Macronix - MX29GL512F",                    /**< flash name */ \
    0x04000000,                                 /**< memory size: 64 MB */ \
    0x00010000,                                 /**< sector size: 64 kB */ \
    0x00000100,                                 /**< page size: 256 Byte */ \
    GOAL_TRUE                                   /**< memory mapped read: yes */

/**< Macronix - MX25L25635F */
#define GOAL_DRV_NVS_CHIP_MACRONIX_MX25L25635F \
    "Macronix - MX25L25635F",                   /**< flash name */ \
    0x01000000,                                 /**< memory size: 16 MB */ \
    0x00001000,                                 /**< sector size: 4 kB */ \
    0x00000100,                                 /**< page size: 256 Byte */ \
    GOAL_TRUE                                   /**< memory mapped read: yes */

/**< Macronix - MX25UR51245G */
#define GOAL_DRV_NVS_CHIP_MACRONIX_MX25UR51245G \
    "Macronix - MX25UR51245G",                  /**< flash name */ \
    0x04000000,                                 /**< memory size: 64 MB */ \
    0x00001000,                                 /**< sector size: 4 kB */ \
    0x00000040,                                 /**< page size: 64 Byte */ \
    GOAL_TRUE                                   /**< memory mapped read: yes */

/**< Renesas - R1EX24016A (EEPROM) */
#define GOAL_DRV_NVS_CHIP_RENESAS_R1EX24016A \
    "Renesas - R1EX24016A",                     /**< NVS name */ \
    0x00000800,                                 /**< memory size: 2 kB */ \
    0x00000800,                                 /**< sector size: 2 kB */ \
    0x00000010,                                 /**< page size: 16 byte */ \
    GOAL_TRUE                                   /**< memory mapped read: yes */

#define GOAL_DRV_NVS_CHIP_ATMEL_AT24CS64 \
    "Atmel - AT24CS64",                         /**< NVS name */ \
    0x00010000,                                 /**< memory size: 64 kB */ \
    0x00010000,                                 /**< sector size: 64 kB */ \
    0x00000020,                                 /**< page size: 32 byte */ \
    GOAL_TRUE                                   /**< memory mapped read: yes */

/**< Macronix - MX25L51245G */
#define GOAL_DRV_NVS_CHIP_MACRONIX_MX25L51245G \
    "Macronix - MX25L51245G",                   /**< NVS name */ \
    0x04000000,                                 /**< memory size: 64 MB */ \
    0x00001000,                                 /**< sector size: 64 kB */ \
    0x00000100,                                 /**< page size: 256 Byte */ \
    GOAL_TRUE                                   /**< memory mapped read: yes */

/**< Macronix - MX25L51245G */
#define GOAL_DRV_NVS_CHIP_SHANGHAI_FM24C128D \
    "Shanghai - FM24C128D ",                    /**< NVS name */ \
    0x00004000,                                 /**< memory size: 64 kB */ \
    0x00004000,                                 /**< sector size: 64 kB */ \
    0x00000040,                                 /**< page size: 256 Byte */ \
    GOAL_TRUE                                   /**< memory mapped read: yes */

/**< Renesas - R1EX24032ASAS0I (EEPROM) */
#define GOAL_DRV_NVS_CHIP_RENESAS_R1EX24032ASAS0I \
    "Renesas - R1EX24032ASAS0I",                /**< NVS name */ \
    0x00001000,                                 /**< memory size: 4 kB */ \
    0x00001000,                                 /**< sector size: 4 kB */ \
    0x00000020,                                 /**< page size: 32 byte */ \
    GOAL_TRUE                                   /**< memory mapped read: yes */

/**< Renesas - R1EX24016ASAS0I (EEPROM) */
#define GOAL_DRV_NVS_CHIP_RENESAS_R1EX24016ASAS0I \
    "Renesas - R1EX24016ASAS0I",                /**< NVS name */ \
    0x00000800,                                 /**< memory size: 2 kB */ \
    0x00000800,                                 /**< sector size: 2 kB */ \
    0x00000010,                                 /**< page size: 16 byte */ \
    GOAL_TRUE                                   /**< memory mapped read: yes */

/**< Winbond - W25Q128JWSIQ */
#define GOAL_DRV_NVS_CHIP_WINBOND_W25Q128JWSIQ \
    "Winbond - W25Q128JWSIQ",                   /**< flash name */ \
    0x01000000,                                 /**< memory size: 16 MB */ \
    0x00001000,                                 /**< sector size: 4 kB */ \
    0x00000100,                                 /**< page size: 256 Byte */ \
    GOAL_TRUE                                   /**< memory mapped read: yes */

#if RIOP
/**< Macronix - MX25UW6345GXDI00 */
#define GOAL_DRV_NVS_CHIP_MACRONIX_MX25UW6345GXDI00 \
    "Macronix - Octal 8MB",                   /**< flash name */ \
    0x00800000,                                 /**< memory size: 8 MB */ \
    0x00001000,                                 /**< sector size: 4 kB */ \
    0x00000100,                                 /**< page size: 256 Byte */ \
    GOAL_TRUE                                   /**< memory mapped read: yes */
#endif

/**< Renesas - R5F572MNDxBD (internal data flash) */
#define GOAL_DRV_NVS_CHIP_RENESAS_R5F572MNDXBD_DF \
    "Renesas - R5F572MNDxBD - DF",              /**< NVS name */ \
    0x00008000,                                 /**< memory size: 32 kB */ \
    0x00001000,                                 /**< sector size: 4 kB */ \
    0x00000040,                                 /**< page size: 64 byte */ \
    GOAL_TRUE                                   /**< memory mapped read: yes */

#endif /* GOAL_DRV_NVS_H */
