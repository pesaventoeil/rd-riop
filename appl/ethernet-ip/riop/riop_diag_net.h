/* Shared M33->M7 NETC/lwIP diagnostic snapshot.
 * Lives at a reserved 256-byte OCRAM1 window just below the RPMSG region
 * (0x204FDF00 when __multicore__ shmem is 8K). M33 writes, M7 reads.
 * Cache clean (M33) / invalidate (M7) required; OCRAM1 is cacheable.
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

#ifdef __cplusplus
}
#endif

#endif /* RIOP_DIAG_NET_H */
