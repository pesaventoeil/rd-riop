/** @file
 *
 * @brief Low Level Interface of PTP stack
 *
 * This provides the API to the target specific functions used by the PTP stack.
 *
 * @copyright
 * Copyright 2010-2023.
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

#include "goal_includes.h"
#include "goal_target_ptp.h"
#include "fsl_netc_timer.h"
#include "fsl_msgintr.h"


/****************************************************************************/
/* Local defines */
/****************************************************************************/
#define GOAL_DRV_PTP_INTR_CHAN                0 /**< interrupt channel used by PTP driver */
#define GOAL_DRV_PTP_INTR_ID                  0 /**< interrupt ID used by PTP driver */
#define GOAL_DRV_PTP_TASK_PRIO               95 /**< task priority for PTP Task */
#define GOAL_DRV_PTP_TASK_STACK_SIZE       1024 /**< stack size for PTP Tasks */
#define GOAL_DRV_PTP_SEC_IN_NSEC     1000000000 /**< seconds in ns */


/****************************************************************************/
/* Local functions */
/****************************************************************************/
static void goal_drvPtpMsiXCbExec(
    MSGINTR_Type *pBase,                        /**< MSI controller base */
    uint8_t channel,                            /**< channel ID */
    uint32_t pendingIntr                        /**< interrupt message data */
);

#if GOAL_CONFIG_TASK == 1
static GOAL_STATUS_T goal_drvPtpTask(
    void *pArg                                  /**< task argument */
);
#endif /* GOAL_CONFIG_TASK */
static void goal_drvPtpIrqHandler(
    void
);


/****************************************************************************/
/* Local variables */
/****************************************************************************/
static netc_timer_handle_t timerHdl;            /**< timer handle */
#if GOAL_CONFIG_TASK == 1
static GOAL_TASK_T *pIsrTskHandle = NULL;       /**< ISR Task handle */
#endif /* GOAL_CONFIG_TASK */
static GOAL_FUNC_NORET_T ptpIrqCb = NULL;       /**< PTP IRQ callback handler */
static void *pPtpIrqArg = NULL;                 /**< PTP IRQ callback handler argument */


/****************************************************************************/
/** Initialize Timestamp hardware
 *
 * This function sets up all hardware modules that are needed to generate
 * timestamps for Ethernet frames.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_targetPtpInit(
    void
)
{
    GOAL_STATUS_T res = GOAL_OK;                /* GOAL result */
    status_t resSdk;                            /* SDK result */
    netc_timer_config_t timerConfig;            /* timer config */
    netc_msix_entry_t msixEntry;                /* interrupt config */

    GOAL_MEMSET(&timerConfig, 0, sizeof(netc_timer_config_t));
    timerConfig.clockSelect = kNETC_TimerExtRefClk;
    timerConfig.refClkHz = CLOCK_GetRootClockFreq(kCLOCK_Root_Tmr_1588);
    timerConfig.enableTimer = true;
    timerConfig.msixEntry = &msixEntry;
    timerConfig.entryNum = 1;
    msixEntry.control = kNETC_MsixIntrMaskBit;
    msixEntry.msgAddr = MSGINTR_GetIntrSelectAddr(MSGINTR2, GOAL_DRV_PTP_INTR_CHAN);;
    msixEntry.msgData = GOAL_DRV_PTP_INTR_ID;

    resSdk = NETC_TimerInit(&timerHdl, &timerConfig);
    if (kStatus_Success != resSdk) {
        goal_logErr("failed to enable PTP clock");
        res = GOAL_ERROR;
    }

    if (GOAL_RES_OK(res)) {
        resSdk = MSGINTR_Init(MSGINTR2, &goal_drvPtpMsiXCbExec);
        if (kStatus_Success != resSdk) {
            goal_logErr("failed to register IRQ handler");
            res = GOAL_ERROR;
        }
    }

#if GOAL_CONFIG_TASK == 1
    if (GOAL_RES_OK(res)) {
        res = goal_taskCreate(&pIsrTskHandle,
                              "PTP Task",
                              goal_drvPtpTask,
                              NULL,
                              GOAL_DRV_PTP_TASK_PRIO,
                              GOAL_DRV_PTP_TASK_STACK_SIZE,
                              GOAL_TRUE);
    }
#endif /* GOAL_CONFIG_TASK */

    return res;
}


/****************************************************************************/
/** Update local clock frequency
 *
 * This function is called to correct the local clock frequency by given
 * factor.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_targetPtpUpdateFreq(
    double fac                                  /**< correction factor */
)
{
    static int32_t ppb = 0;                     /* current tick fraction in parts per billion */

    /* apply factor to factional part of clock tick */
    ppb += (int32_t) ((fac - 1.0) * 1000000000);

    NETC_TimerAdjustFreq(&timerHdl, ppb);

    return GOAL_OK;
}


/****************************************************************************/
/** Update local clock
 *
 * This function is called to correct the calculated offset from the master
 * clock to the local timer registers. Depending on @em add it either adds or
 * subtracts the offset.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_targetPtpUpdateTime(
    GOAL_BOOL_T add,                            /**< add offset to current time */
    int64_t secOffset,                          /**< offset of full seconds */
    int32_t nsecOffset                          /**< offset of sub-seconds */
)
{
    int64_t offset;                             /* offset in ns */

    offset = GOAL_TIME_S_TO_NS(secOffset) + nsecOffset;

    if (GOAL_FALSE == add) {
        offset *= -1;
    }

    NETC_TimerAddOffset(&timerHdl, offset);

    return GOAL_OK;
}


/****************************************************************************/
/** Set the system time
 *
 * This function sets the system time to the value specified in @em sec and
 * @em nsec. THe PTP stack uses this only if the clock is the grandmaster and a
 * management message was received that contained a new time value.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_targetPtpSetTime(
    uint64_t sec,                               /**< full seconds */
    uint32_t nsec                               /**< sub-seconds in ns */
)
{
    uint64_t now;                               /* current time in ns */
    uint64_t new;                               /* new time in ns */
    int64_t offset;                             /* offset between now and new */

    NETC_TimerGetCurrentTime(&timerHdl, &now);

    new = GOAL_TIME_S_TO_NS(sec) + nsec;

    offset = ((int64_t) new) - ((int64_t) now);

    NETC_TimerAddOffset(&timerHdl, offset);

    return GOAL_OK;
}


/****************************************************************************/
/** Get the current system time
 *
 * This function reads the current system time from the timer module that is
 * synchronized to the PTP domain.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
GOAL_STATUS_T goal_targetPtpGetTime(
    uint64_t *pSec,                             /**< full seconds */
    uint32_t *pNsec                             /**< sub-seconds in ns */
)
{
    uint64_t now;                               /* current time in ns */

    NETC_TimerGetCurrentTime(&timerHdl, &now);

    *pSec = GOAL_TIME_NS_TO_S(now);
    *pNsec = (uint32_t) (now - GOAL_TIME_S_TO_NS(*pSec));

    return GOAL_OK;
}


/****************************************************************************/
/** Check if the clock is considered stable
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_BOOL_T goal_targetPtpIsStable(
    void
)
{
    /* always true for this target */
    return GOAL_TRUE;
}


/****************************************************************************/
/** Start a callback that is periodically triggered by the PTP timer
 *
 * The cyclic timer must be delayed until @em startTime is reached.
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_targetPtpCycleCallbackStart(
    uint64_t startTime,                         /**< absolute start time of first cycle in ns */
    uint64_t cycleTime,                         /**< cycle time in ns */
    GOAL_FUNC_NORET_T cbFunc,                   /**< callback handler */
    void *pCbArg                                /**< callback handler argument */
)
{
    netc_timer_fiper_config_t fiperCfg;         /* FIPER config */
    netc_timer_fiper_t fiperCfg2;               /* FIPER config */
    netc_timer_alarm_t alarmCfg;                /* Alarm config */
    uint32_t clockPeriod;                       /* clock period in ns */

    if (NULL == cbFunc) {
        return GOAL_ERR_NULL_POINTER;
    }

    ptpIrqCb = cbFunc;
    pPtpIrqArg = pCbArg;

    /* Use the Alarm to set a start time for the FIPER that will generate a periodic
     * interrupt.
     */

    /* start FIPER after ALARM1 expired */
    fiperCfg.startCondition = true;
    fiperCfg.fiper1Loopback = false;
    fiperCfg.fiper2Loopback = false;
    fiperCfg.prescale = 0;
    NETC_TimerConfigureFIPER(&timerHdl, &fiperCfg);

    /* enable FIPER */
    fiperCfg2.enableInterrupt = true;
    fiperCfg2.pulseGenSync = false;
    fiperCfg2.pulseWidth = 1;
    /* FIPER interval is stated in ns - TCLK_PERIOD
      - convert clock frequency into period in ns
     */
    clockPeriod = GOAL_DRV_PTP_SEC_IN_NSEC / CLOCK_GetRootClockFreq(kCLOCK_Root_Tmr_1588);
    fiperCfg2.pulsePeriod = (uint32_t) cycleTime - clockPeriod;
    NETC_TimerStartFIPER(&timerHdl, kNETC_TimerFiper1, &fiperCfg2);

    /* set up aLarm timer that will start the FIPER */
    alarmCfg.enableInterrupt = false;
    alarmCfg.polarity = false;
    alarmCfg.pulseGenSync = true;
    alarmCfg.pulseWidth = 1;
    NETC_TimerConfigureAlarm(&timerHdl, kNETC_TimerAlarm1, &alarmCfg);
    NETC_TimerStartAlarm(&timerHdl, kNETC_TimerAlarm1, startTime);

    /* enable interrupt */
    NETC_TimerMsixSetEntryMask(&timerHdl, 0, false);

    return GOAL_OK;
}


/****************************************************************************/
/** Stop a callback that is periodically triggered by the PTP timer
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_targetPtpCycleCallbackStop(
    void
)
{
    /* disable interrupt */
    NETC_TimerMsixSetEntryMask(&timerHdl, 0, true);
    /* stop FIPER */
    NETC_TimerStopFIPER(&timerHdl, kNETC_TimerFiper1);
    /* stop Alarm */
    NETC_TimerStopAlarm(&timerHdl, kNETC_TimerAlarm1);

    return GOAL_OK;
}


/****************************************************************************/
/** PTP Timer interrupt handler
 *
 * This handler processes the MSI messages from the NETC module.
 */
static void goal_drvPtpMsiXCbExec(
    MSGINTR_Type *pBase,                        /**< MSI controller base */
    uint8_t channel,                            /**< channel ID */
    uint32_t pendingIntr                        /**< interrupt message data */
)
{
    UNUSEDARG(pBase);

    if (GOAL_DRV_PTP_INTR_CHAN == channel) {
        /* Transmit interrupt */
        if ((1 << GOAL_DRV_PTP_INTR_ID) == pendingIntr) {
            /* clear interrupt */
            NETC_TimerClearInterruptStatus(&timerHdl, ENETC_PF_TMR_TMR_TEVENT_PP1EN_MASK);
#if GOAL_CONFIG_TASK == 1
            goal_taskResume(pIsrTskHandle);
#else /* GOAL_CONFIG_TASK */
            goal_drvPtpIrqHandler();
#endif /* GOAL_CONFIG_TASK */
        }
        else {
            goal_logErr("unknown Interrupt ID 0x%"FMT_x32, pendingIntr);
        }
    }
    else {
        goal_logErr("unexpected Interrupt channel %u", channel);
    }
}


#if GOAL_CONFIG_TASK == 1
/****************************************************************************/
/** PTP ISR Task
 *
 * This task is woken up by the RTOS when an PTP IRQ was triggered.
 *
 * @retval GOAL_OK successful
 * @retval other failed
 */
static GOAL_STATUS_T goal_drvPtpTask(
    void *pArg                                  /**< task argument */
)
{
    GOAL_STATUS_T res;                          /* result */

    UNUSEDARG(pArg);

    /* loop until GOAL shutdown */
    while (!GOAL_flagShutdown) {
        /* suspend task until it is resumed by ISR */
        res = goal_taskSuspend(pIsrTskHandle);
        if (GOAL_RES_ERR(res)) {
            goal_logErr("Error suspending PTP task");
            continue;
        }

        goal_drvPtpIrqHandler();
    }

    return GOAL_OK;
}
#endif /* GOAL_CONFIG_TASK */


/****************************************************************************/
/** Handle a PTP Time IRQ
 */
static void goal_drvPtpIrqHandler(
    void
)
{
    if (NULL != ptpIrqCb) {
        ptpIrqCb(pPtpIrqArg);
    }
}
