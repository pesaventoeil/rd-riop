/* modbus_cb.h - staging buffer accessor for the DCE Modbus master.
 *
 * Location: C:\rd-riop\appl\ethernet-ip\riop\modbus\modbus_cb.h
 */

#ifndef MODBUS_CB_H
#define MODBUS_CB_H

#include <stdint.h>
#include <stdbool.h>

/** Discard any staged response. Call before issuing a request so a failed
 *  transaction cannot leave the previous response looking fresh. */
void mbcb_rawInvalidate(void);

/** Copy the most recent holding-register response.
 *  Returns false if the last transaction failed or returned a different
 *  register count than expected. Call only after
 *  eMBMasterReqReadHoldingRegister() has returned MB_MRE_NO_ERR. */
bool mbcb_rawGet(uint16_t *pOut, uint8_t nRegs);

#endif /* MODBUS_CB_H */
