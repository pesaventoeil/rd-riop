/* modbus_cb.c - FreeModbus master callbacks for the CentriFeeder DCE link.
 *
 * Replaces the stub register callbacks in NXP's modbus_client.c. That file is
 * NOT added to the build: it defines main(), which collides with the GOAL
 * application's.
 *
 * NOTE: the four request-result callbacks (vMBMasterCBRequestScuuess and the
 * three vMBMasterErrorCB*) are NOT here. They belong to the OS signalling
 * layer in portevent.c -- they are what eMBMasterWaitRequestFinish() waits
 * on, not user hooks. Defining them here would block every request until its
 * wait timeout regardless of outcome.
 *
 * Location: C:\rd-riop\appl\ethernet-ip\riop\modbus\modbus_cb.c
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "mb.h"
#include "mb_m.h"
#include "mbport.h"

#include "dce_app.h"
#include "modbus_cb.h"

/* Raw response staging. Written from the FreeModbus poll task, read by
 * dce_task after eMBMasterReqReadHoldingRegister() returns success. The
 * master stack serialises requests behind its own resource semaphore and we
 * are the only requester, so no additional locking is needed here. */
static uint16_t prvRawRegs[DCE_REG_BLOCK_NREGS];
static uint8_t  prvRawNRegs;

void mbcb_rawInvalidate(void)
{
    prvRawNRegs = 0u;
}

bool mbcb_rawGet(uint16_t *pOut, uint8_t nRegs)
{
    if (prvRawNRegs != nRegs) {
        return false;
    }
    memcpy(pOut, prvRawRegs, nRegs * sizeof(uint16_t));
    return true;
}

/* ------------------------------------------------------------------------
 * Holding registers - this is where Count and CWratio arrive.
 *
 * pucRegBuffer is raw wire order: two bytes per register, high byte first.
 * The DCE's word ordering is High Then Low (confirmed on the Crimson device
 * page), so each 32-bit value is simply four big-endian bytes. We stage the
 * registers here and let dce_task do the arithmetic.
 *
 * usAddress is ignored deliberately, and this is now confirmed rather than
 * assumed: eMBMasterFuncReadHoldingRegister() in mbfuncholding_m.c re-reads
 * the address out of the send buffer and does usRegAddress++ before calling
 * us. A request for 504 therefore arrives here as 505. Positional decoding
 * sidesteps the off-by-one entirely.
 *
 * The stack has already verified that 2 * usNRegs matches the response byte
 * count before calling us, so a short frame cannot reach this point.
 * ---------------------------------------------------------------------- */
eMBErrorCode eMBMasterRegHoldingCB(UCHAR *pucRegBuffer, USHORT usAddress,
                                   USHORT usNRegs, eMBRegisterMode eMode)
{
    USHORT i;

    (void)usAddress;

    if (MB_REG_READ != eMode) {
        return MB_ENOREG;
    }
    if (usNRegs > DCE_REG_BLOCK_NREGS) {
        return MB_ENOREG;
    }

    for (i = 0; i < usNRegs; i++) {
        prvRawRegs[i] = (uint16_t)((pucRegBuffer[i * 2] << 8) | pucRegBuffer[(i * 2) + 1]);
    }
    prvRawNRegs = (uint8_t)usNRegs;

    return MB_ENOERR;
}

/* Unused function codes. MB_ENOREG makes an accidental call fail loudly
 * rather than silently succeeding with garbage. */
eMBErrorCode eMBMasterRegInputCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNRegs)
{
    (void)pucRegBuffer; (void)usAddress; (void)usNRegs;
    return MB_ENOREG;
}

eMBErrorCode eMBMasterRegCoilsCB(UCHAR *pucRegBuffer, USHORT usAddress,
                                 USHORT usNCoils, eMBRegisterMode eMode)
{
    (void)pucRegBuffer; (void)usAddress; (void)usNCoils; (void)eMode;
    return MB_ENOREG;
}

eMBErrorCode eMBMasterRegDiscreteCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNDiscrete)
{
    (void)pucRegBuffer; (void)usAddress; (void)usNDiscrete;
    return MB_ENOREG;
}
