/*
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
typedef struct GOAL_DRV_NET_CHANNEL_LIST_T {
    struct GOAL_DRV_NET_CHANNEL_LIST_T *pNext;   /* next channel */
    uint32_t id;                                /* channel id */
    GOAL_NET_CHAN_T *pChannel;                  /* channel handle */
    void *fnCallback;                           /* callback */

} GOAL_DRV_NET_CHANNEL_LIST_T;


GOAL_STATUS_T goal_drvCtcChannelId(
    GOAL_DRV_NET_CHANNEL_LIST_T *pListChan,     /**< channel list */
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    uint32_t *pId                               /**< [out] channel id */
);

GOAL_STATUS_T goal_drvCtcChannelEntry(
    GOAL_DRV_NET_CHANNEL_LIST_T *pListChan,     /**< channel list */
    GOAL_NET_CHAN_T *pChanHandle,               /**< channel handle */
    GOAL_DRV_NET_CHANNEL_LIST_T **ppEntry       /**< [out] channel list entry */
);

GOAL_STATUS_T goal_drvCtcChannelEntryById(
    GOAL_DRV_NET_CHANNEL_LIST_T *pListChan,     /**< channel list */
    uint32_t id,                                /**< channel id */
    GOAL_DRV_NET_CHANNEL_LIST_T **ppEntry       /**< [out] channel list entry */
);


