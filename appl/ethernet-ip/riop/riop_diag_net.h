/* Shared M33->M7 NETC/lwIP diagnostic snapshot.
 * Lives at a reserved 256-byte OCRAM1 window just below the RPMSG region
 * (0x204FDF00 when __multicore__ shmem is 8K). M33 writes, M7 reads.
 * OCRAM1 is non-cacheable on the M33 (MPU) and cacheable on the M7 (M7 invalidates).
 */
#ifndef RIOP_DIAG_NET_H
#define RIOP_DIAG_NET_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RIOP_DIAG_NET_MAGIC  0x4E31304Eu /* 'N10N' */
#define RIOP_DIAG_NET_ADDR   0x204FDF00u
#define RIOP_DIAG_NET_LEN    64u

typedef struct {
	uint32_t magic;
	uint32_t seq;
	uint32_t rx_frames;
	uint32_t tx_frames;
	uint32_t rx_errors;
	uint32_t tx_errors;
	uint32_t dma_stall;
	uint32_t dma_overflow;
	uint32_t lwip_pbuf_err;
	uint32_t lwip_mem_err;
	uint32_t enet_reinit_count;
	uint32_t flags;
	uint8_t  phy_link_mask;
	uint8_t  reserved0;
	uint16_t link_change_count;
	uint32_t reserved1[3];
} riop_diag_net_t;

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
_Static_assert(sizeof(riop_diag_net_t) == RIOP_DIAG_NET_LEN, "riop_diag_net_t must be 64 bytes");
#endif

/* N10g: M33 fault record, 64 B at window +0x40. Written by the M33 fault
 * handlers just before NVIC_SystemReset(); survives the warm reset (neither
 * core's startup touches the window); read by the M7 into asm 177 v2. */
#define RIOP_DIAG_FAULT_ADDR   0x204FDF40u
#define RIOP_DIAG_FAULT_MAGIC  0x4E313046u /* 'N10F' */
#define RIOP_FAULT_HARD    1u
#define RIOP_FAULT_MEM     2u
#define RIOP_FAULT_BUS     3u
#define RIOP_FAULT_USAGE   4u
#define RIOP_FAULT_SECURE  5u

typedef struct {
	uint32_t magic;
	uint32_t count;      /* faults since this record was last valid (power-off clears) */
	uint32_t type;       /* RIOP_FAULT_* */
	uint32_t cfsr;
	uint32_t hfsr;
	uint32_t bfar;
	uint32_t mmfar;
	uint32_t sfsr;
	uint32_t sfar;
	uint32_t pc;
	uint32_t lr;
	uint32_t xpsr;
	uint32_t exc_return;
	uint32_t uptime_ms;
	uint32_t reserved0;
	uint32_t reserved1;
} riop_diag_fault_t;

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
_Static_assert(sizeof(riop_diag_fault_t) == 64, "riop_diag_fault_t must be 64 bytes");
#endif

#ifdef __cplusplus
}
#endif

#endif /* RIOP_DIAG_NET_H */
