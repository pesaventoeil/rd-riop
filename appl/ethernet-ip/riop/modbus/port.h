/* port.h - shim for the FreeModbus core.
 *
 * The FreeModbus core sources (mb_m.c, mbrtu_m.c, mbfuncholding_m.c,
 * mbutils.c, mbcrc.c) hardcode  #include "port.h"  and cannot be edited.
 * This file exists only to satisfy that include and redirect to the real
 * port header, which is renamed dce_port.h so a generic "port.h" never
 * shadows another port.h elsewhere on the M7 include path.
 *
 * Location: appl\ethernet-ip\riop\modbus\port.h
 */

#ifndef _PORT_H
#define _PORT_H
#include "dce_port.h"
#endif /* _PORT_H */
