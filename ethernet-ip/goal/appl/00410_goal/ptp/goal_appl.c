/** @file
 *
 * @brief
 * PTP Example Application
 *
 * @details
 * This module implements a basic application that tests the PTP stack.
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

#include <goal_includes.h>
#include <ptp_includes.h>


/****************************************************************************/
/* Local functions */
/****************************************************************************/
void goal_ptpCallback(
    PTP_CB_ID_T cbId,                           /**< callback ID */
    void *pCbData                               /**< callback data */
);


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static PTP_TIMEPROPERTIES_DS_T myTimeProps;     /**< time properties of this device */
static GOAL_TIMESTAMP_T tsNext;                 /**< next iteration time */


/****************************************************************************/
/** Application Init
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T appl_init(
    void
)
{
    /* initialize PTP stack */
    return ptp_init(goal_ptpCallback);
}


/****************************************************************************/
/** Application Setup
 *
 * @returns GOAL_STATUS_T result
 */
GOAL_STATUS_T appl_setup(
    void
)
{
    /* initialize time properties, these values are application specific */
    myTimeProps.utcOffset = 33;
    myTimeProps.utcOffsetValid = GOAL_TRUE;
    myTimeProps.leap59 = GOAL_FALSE;
    myTimeProps.leap61 = GOAL_FALSE;
    myTimeProps.timeTracable = GOAL_FALSE;
    myTimeProps.freqTracable = GOAL_FALSE;
    myTimeProps.ptpTimescale = GOAL_TRUE;
    myTimeProps.timeSrc = PTP_TIME_SRC_INTERNAL_OSC;

    /* initialize timestamp */
    tsNext = goal_timerTsGet() + GOAL_TIMER_SEC;

    return GOAL_OK;
}


/****************************************************************************/
/** PTP application callback
 *
 * This function is called by the PTP stack to inform the application about
 * special events that might require the application's attention.
 */
void goal_ptpCallback(
    PTP_CB_ID_T cbId,                           /**< callback ID */
    void *pCbData                               /**< callback data */
)
{
    PTP_TIMEPROPERTIES_DS_T *pTimeProperties;   /* timePropertiesDS data set */
    PTP_TIMESTAMP_T *pTs;                       /* new system time */

    switch (cbId) {
        case PTP_CB_TIME_UPDATE:
            /* the local clock was updated by the PTP stack */
            pTs = (PTP_TIMESTAMP_T *) pCbData;
            goal_logInfo("new system time: %"FMT_u64" s %"FMT_u32" ns",
                         pTs->sec, pTs->nsec);
            UNUSEDARG(pTs); /* if logging is disabled */
            break;

        case PTP_CB_GRANDMASTER:
            /* device is grandmaster of this domain, state the properties of
             * its time source (synchronized to it by non-PTP means)
             * pCbData points to a PTP_TIMEPROPERTIES_DS_T struct
             */
            GOAL_MEMCPY(pCbData, (void *) &myTimeProps, sizeof(PTP_TIMEPROPERTIES_DS_T));
            break;

        case PTP_CB_SYNC:
            /* The local clock is synchronized to a master. The application
             * should do all necessary steps to synchronize to the new time and
             * then call ptp_syncFinished().
             */
            ptp_syncFinished();
            break;

        case PTP_CB_UTC_PROP_UPDATE:
            /* a management node requested to change the device's UTC properties,
             * application must determine if the new values are applicable
             */
            pTimeProperties = (PTP_TIMEPROPERTIES_DS_T *) pCbData;
            myTimeProps.utcOffset = pTimeProperties->utcOffset;
            myTimeProps.leap61 = pTimeProperties->leap61;
            myTimeProps.leap59 = pTimeProperties->leap59;
            myTimeProps.utcOffsetValid = pTimeProperties->utcOffsetValid;
            break;

        case PTP_CB_TRACABLE_UPDATE:
            /* a management node requested to change the device's traceability
             * properties, application must determine if the new values are
             * applicable
             */
            pTimeProperties = (PTP_TIMEPROPERTIES_DS_T *) pCbData;
            myTimeProps.timeTracable = pTimeProperties->timeTracable;
            myTimeProps.freqTracable = pTimeProperties->freqTracable;
            break;

        case PTP_CB_TIMESCALE_UPDATE:
            /* a management node requested to change the device's time scale
             * properties, application must determine if the new values are
             * applicable
             */
            pTimeProperties = (PTP_TIMEPROPERTIES_DS_T *) pCbData;
            myTimeProps.ptpTimescale = pTimeProperties->ptpTimescale;
            myTimeProps.timeSrc = pTimeProperties->timeSrc;
            break;

        case PTP_CB_IN_SYNC:
            goal_logInfo("synchronization successful");
            break;

        case PTP_CB_OUT_OF_SYNC:
            goal_logInfo("lost synchronization");
            break;
    }
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
    uint32_t button;                            /* pressed buttons */

    /* map buttons to LEDs each second */
    if (goal_timerTsGet() > tsNext) {
        tsNext = goal_timerTsGet() + GOAL_TIMER_SEC;
        button = goal_targetGetButtons();
        goal_logDbg("buttons: 0x%"FMT_x32, button);
        goal_targetSetLeds(button);
    }
}
