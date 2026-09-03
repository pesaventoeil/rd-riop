/** @file
 *
 * @brief GOAL Register Handling
 *
 * This module implements a generic register handling interface.
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

#define GOAL_ID GOAL_ID_REG
#include <goal_includes.h>


/****************************************************************************/
/** Get register value
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_regGet(
    GOAL_REG_T *pTable,                         /**< register table */
    GOAL_BUS_T *pBus,                           /**< bus handle */
    uint32_t addr,                              /**< address */
    uint32_t reg,                               /**< register/offset */
    uint32_t *pVal                              /**< value ptr */
)
{
    GOAL_STATUS_T res;                          /* result */

    /* read register */
    res = pBus->read(pBus, addr, pTable[reg].reg, (void *) pVal, sizeof(uint32_t));
    if (GOAL_OK != res) {
        return res;
    }

    /* shift and mask valid bits */
    (*pVal) >>= pTable[reg].shift;
    (*pVal) &= pTable[reg].mask;

    return GOAL_OK;
}


/****************************************************************************/
/** Set register value
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_regSet(
    GOAL_REG_T *pTable,                         /**< register table */
    GOAL_BUS_T *pBus,                           /**< bus handle */
    uint32_t addr,                              /**< address */
    uint32_t reg,                               /**< register/offset */
    uint32_t val                                /**< value */
)
{
    GOAL_STATUS_T res;                          /* result */
    uint32_t regValue;                          /* content of the switch register */

    if (!(pTable[reg].flags & GOAL_REG_FLG_RW)) {
        /* GG_LOG ID:233: */
        goal_lmLog(GOAL_ID_REG, 233, 188, 4, 0, GOAL_LOG_SEV_ERROR, "register is read only: $1");
        goal_lmLogParamUINT32(reg);
        goal_lmLogFinish();
        return GOAL_ERR_ACCESS;
    }

    /* read register to keep current content */
    res = pBus->read(pBus, addr, pTable[reg].reg, (void *) &regValue, sizeof(regValue));
    if (GOAL_OK != res) {
        return res;
    }

    /* shift and set value */
    regValue &= ~(pTable[reg].mask << pTable[reg].shift);
    regValue |= ((val & pTable[reg].mask) << pTable[reg].shift);

    /* write to register */
    return pBus->write(pBus, addr, pTable[reg].reg, (void *) &regValue, sizeof(regValue));
}


/****************************************************************************/
/** Get/Set register value by GOAL command mapping
 *
 * @retval GOAL_OK success
 * @retval other fail
 */
GOAL_STATUS_T goal_regCmd(
    GOAL_REG_T *pTable,                         /**< register table */
    GOAL_BUS_T *pBus,                           /**< bus handle */
    GOAL_REG_CMD_T *pTableCmd,                  /**< command map table */
    uint32_t addr,                              /**< address */
    uint32_t cmd,                               /**< command ID */
    GOAL_BOOL_T wrFlag,                         /**< write flag */
    uint32_t *pVal                              /**< value ptr */
)
{
    GOAL_REG_CMD_T *pCmdMap;                    /* current mapped command */

    /* iterate over all command map entries */
    for (pCmdMap = pTableCmd; pCmdMap->cmd; pCmdMap++) {
        if (cmd == pCmdMap->cmd) {
            break;
        }
    }

    if (!pCmdMap->cmd) {
        /* GG_LOG ID:234: */
        goal_lmLog(GOAL_ID_REG, 234, 189, 4, 0, GOAL_LOG_SEV_WARNING, "unsupported command received: $1");
        goal_lmLogParamUINT32(cmd);
        goal_lmLogFinish();
        return GOAL_ERR_UNSUPPORTED;
    }

    /* get/set register */
    if (GOAL_FALSE == wrFlag) {
        return goal_regGet(pTable, pBus, addr, pCmdMap->reg, pVal);
    } else {
        return goal_regSet(pTable, pBus, addr, pCmdMap->reg, *pVal);
    }
}
