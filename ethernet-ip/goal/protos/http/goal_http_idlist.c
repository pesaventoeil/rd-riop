/** @file
 *
 * @brief HTTP channel list handling
 *
 * @copyright
 * Copyright 2010-2024 port GmbH Halle/Saale.
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


#include <goal_includes.h>
#include "goal_http.h"
#include "goal_http_idlist.h"


/****************************************************************************/
/** get id of given channel
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_httpChannelId(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    GOAL_HTTP_CHN_T *pHttpChan,                 /**< channel pointer */
    uint32_t *pId                               /**< [out] channel id */
)
{
    GOAL_STATUS_T ret;                          /* result */

    if ((NULL == pInst) || (NULL == pHttpChan) || (NULL == pId)) {
        return GOAL_ERR_NULL_POINTER;
    }

    /* lock mutex */
    ret = goal_lockGet(pInst->lockChn, GOAL_LOCK_INFINITE);
    if (GOAL_RES_ERR(ret)) {
        goal_logErr("error acquiring channel lock");
        return ret;
    }

    *pId = pHttpChan->id;

    /* unlock mutex */
    goal_lockPut(pInst->lockChn);

    return ret;
}


/****************************************************************************/
/** get channel of given id
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_httpChannelEntryById(
    GOAL_HTTP_T *pInst,                         /**< instance pointer */
    uint32_t id,                                /**< channel id */
    GOAL_HTTP_CHN_T **ppEntry                   /**< [out] channel list entry */
)
{
    uint32_t cnt;                               /* loop counter */
    GOAL_STATUS_T res = GOAL_ERR_NOT_FOUND;     /* result */

    /* search channel handler */
    for (cnt = 0; cnt < pInst->cntChn; cnt++) {
        if (id == pInst->pChanHdl[cnt].id) {
            *ppEntry = &pInst->pChanHdl[cnt];
            res = GOAL_OK;
            break;
        }
    }

    return res;
}
