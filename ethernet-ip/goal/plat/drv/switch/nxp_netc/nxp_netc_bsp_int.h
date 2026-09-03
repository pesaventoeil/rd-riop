/** @file
 *
 * @brief NXP NETC Switch Driver BSP glue code
 *
 * @copyright
 * Copyright 2022 port GmbH Halle/Saale.
 * This software is protected Intellectual Property and may only be used
 * according to the license agreement.
 */

#if defined(GOAL) && GOAL == 1
# define GOAL_ID GOAL_ID_TGT
# include <goal_includes.h>
# include <goal_target_common.h>
#else
typedef GOAL_STATUS {
    GOAL_OK = 0,
    GOAL_ERR_TARGET_INIT = 1,
} GOAL_STATUS_T;

#define GOAL_RES_OK(__res) (__res == GOAL_OK) ? 0 : -1)
#define GOAL_MEMCPY memcpy
#define GOAL_MEMSET memset

#endif

#include <fsl_netc_switch.h>

typedef struct NETC_BSP_INST {
    swt_handle_t *pSwitchHandle;                /**< swt driver handle */
    uint8_t portEnableMask;                     /**< port enable mask */
    GOAL_LOCK_T *pLockEpl;                      /**< lock for auto reply buffers */
} NETC_BSP_INST_T;


#define GOAL_SW_NETC_ENTRY_NULL 0xFFFFFFFF      /**< NULL entry ID */

#include "nxp_netc_bsp_api.h"
