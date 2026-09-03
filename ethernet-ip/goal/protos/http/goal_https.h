/** @file
 *
 *  Http webserver Example
 *
 * This application runs a simple HTTP server that hosts a demo HTML page.
 * It can be accessed via web browser by browsing the IP address of this device.
 *
 * @copyright
 * Copyright 2010-2017 port GmbH Halle/Saale.
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


#ifndef GOAL_HTTPS_H
#define GOAL_HTTPS_H

#include <goal_includes.h>
#include "goal_media/goal_mi_tls.h"


/****************************************************************************/
/* Data types */
/****************************************************************************/


/****************************************************************************/
/* Functions */
/****************************************************************************/

GOAL_STATUS_T goal_httpsGetCert(
    GOAL_MI_TLS_CERT_T  *pHttpsCert             /**< pointer to certificate info */
);

GOAL_STATUS_T goal_httpsSetCertInfoCm(
    GOAL_MI_TLS_T *pMiTls                       /**< TLS MI handle */
);


#endif /* GOAL_HTTPS_H */

