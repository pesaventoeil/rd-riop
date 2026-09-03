/** @file
 *
 * @brief common header for GOAL
 *
 * This header file must be included by all files that are using GOAL.
 *
 * @copyright
 * Copyright 2010-2019.
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

#ifndef GOAL_INCLUDES_H
#define GOAL_INCLUDES_H


/* C++ compatibility */
#ifdef __cplusplus
extern "C" {
#endif


/* system headers */
#include <inttypes.h>


/* GOAL Version */
#define GOAL_VER_MAJ    3                       /**< major version */
#define GOAL_VER_MIN    0                       /**< minor version */
#define GOAL_VER_SUB    1                       /**< sub version */

/* GOAL Unique IDs */
#include "goal_id.h"

/* GOAL headers */
#include "goal_config_def.h"

/* GOAL ID fallback (used if module does not set GOAL_ID before including this file) */
#ifndef GOAL_ID
#  define GOAL_ID GOAL_ID_APPL
#endif


/****************************************************************************/
/* GOAL Includes */
/****************************************************************************/
/* Warning: goal_config_def.h must always be the first included header file to
 * make sure the default-fallback logic for not-defined defines will work
 * correctly.
 */
#ifndef GOAL_CONFIG_DEF_H
#  error "goal_config_def.h must always be the first included file - for details see code comment"
#endif

#include "goal_target_types.h"
#include <arch/common/goal_arch_common.h>

#include "goal_types.h"

#include "goal_bus.h"
#include "goal_stat.h"

#include "goal_bm.h"
#include "goal_queue.h"
#include "goal_inst.h"
#include "goal_cm.h"
#include <goal_media/goal_ma_eth.h>
#include <goal_media/goal_mi_eth.h>
#include "goal_lock.h"
#include "goal_log.h"
#include "goal_main.h"
#include "goal_rb.h"
#include "goal_mbox.h"

#if GOAL_CONFIG_CSAP == 1
#  include "goal_csap.h"
#endif

#include "goal_timer.h"
#include "goal_reg.h"
#include "goal_util.h"
#if GOAL_CONFIG_TASK == 1
#  define GOAL_CONFIG_FEAT_UTHASH 1
#  include "goal_task.h"
#endif

/* GOAL target API and target system header */
#include "goal_target_api.h"
#include "goal_target.h"

#include "goal_lm.h"

/* GOAL global target default values */
#include <arch/common/goal_target_default.h>

#include "goal_alloc.h"

#if GOAL_CONFIG_TCPIP_STACK == 1 || GOAL_CONFIG_CTC_NET_AC == 1
#  include "goal_media/goal_mi_net.h"
#  include "goal_media/goal_ma_net.h"
#endif

#if GOAL_CONFIG_TCPIP_STACK == 1
#  include "goal_net_chan.h"
#  if GOAL_CONFIG_TCPIP_BSD_API == 1
#    include "goal_target_net.h"
#  endif /* GOAL_CONFIG_TCPIP_BSD_API */
#endif

#if GOAL_CONFIG_DHCP == 1
#  include "goal_net_dhcp.h"
#endif

#if GOAL_CONFIG_FEAT_UTHASH == 1
#  include "utlist.h"
#endif

#if GOAL_CONFIG_CLI == 1
#  include <protos/cli/goal_cli.h>
#  include "goal_eth_cli.h"
#endif

#if GOAL_CONFIG_DD == 1
#  include <protos/dd/goal_dd.h>
#endif

#if (GOAL_CONFIG_LIBRARY == 1) && (GOAL_CONFIG_MCTC_AC != 1)
#  include <goal_lib_util.h>
#endif

#if GOAL_CONFIG_LIBRARY == 1
int libgoal_run(
    int argc,                                   /**< argument count */
    char **argv                                 /**< arguments */
);
#endif


/* C++ compatibility */
#ifdef __cplusplus
}
#endif


#endif /* GOAL_INCLUDES_H */
