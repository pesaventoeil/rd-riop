/** @file
 *
 * @brief GOAL Instance Module
 *
 * This module manages the creation of generic instances.
 *
 * @copyright
 * Copyright 2010-2017.
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
#ifndef GOAL_INST_H
#define GOAL_INST_H


/****************************************************************************/
/* Defines */
/****************************************************************************/
#define GOAL_INSTANCE_HEADER(x) \
    struct x *pNext;                            /**< next instance */ \
    unsigned int idInst;                        /**< instance id */ \
    const char *strInstDesc                     /**< instance description */

#define GOAL_INSTANCE_FOREACH(pList, pInst)     GOAL_LL_FOREACH(pList, *(GOAL_INSTANCE_T **) &pInst)

#define GOAL_ID_INVALID                         (UINT32_MAX)


/****************************************************************************/
/* Structures */
/****************************************************************************/
typedef struct GOAL_INSTANCE_T {
    struct GOAL_INSTANCE_T *pNext;              /**< next instance */
    unsigned int idInst;                        /**< instance id */
    const char *strInstDesc;                    /**< instance description */
} GOAL_INSTANCE_T;


typedef struct GOAL_INSTANCE_LIST_T {
    struct GOAL_INSTANCE_LIST_T *pNext;         /**< next instance list */
    uint32_t idType;                            /**< instance type (GOAL id) */
    GOAL_INSTANCE_T *pInst;                     /**< pointer to first instance */
} GOAL_INSTANCE_LIST_T;


/****************************************************************************/
/* Prototypes */
/****************************************************************************/
GOAL_STATUS_T goal_instListNew(
    GOAL_INSTANCE_LIST_T **ppList,              /**< [out] instance ptr ref */
    uint32_t idType                             /**< instance type */
);

GOAL_STATUS_T goal_instNew(
    GOAL_INSTANCE_T **ppInst,                   /**< [out] instance ptr ref */
    const unsigned int sizeInst,                /**< instance size in bytes */
    uint32_t idType,                            /**< instance type */
    unsigned int id,                            /**< instance id */
    const char *strDesc                         /**< instance description */
);

GOAL_STATUS_T goal_instFree(
    GOAL_INSTANCE_T **ppInst                    /**< [in/out] instance ptr ref */
);

GOAL_STATUS_T goal_instGetById(
    GOAL_INSTANCE_T **ppInst,                   /**< [out] instance ptr ref */
    uint32_t idType,                            /**< instance type */
    unsigned int id                             /**< instance id */
);

GOAL_STATUS_T goal_instListGetByType(
    GOAL_INSTANCE_LIST_T **ppInstList,          /**< [out] instance ptr ref */
    uint32_t idType                             /**< instance type */
);


#endif /* GOAL_INST_H */
