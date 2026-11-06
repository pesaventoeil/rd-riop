/* dce_poll_table.h - THE ONE FILE that defines what the RIOP polls on the DCE.
 *
 * G48. Edit ONLY this file to change page -> register scheduling.
 *
 * Concepts
 *   read    one FC03 transaction (start address, word count).
 *   set     a poll set chosen by the PC (asm 173; never a DCE write):
 *             0 STATUS  link/alarm minimum
 *             1 RUN     Run pages: F every cycle + one M group per cycle
 *             2 SETUP   today's full 8-block scan (boot default + selector timeout)
 *   cycle   one pass over a set's FIXED reads (every cycle) followed by ONE read
 *           taken round-robin from the set's RR list (entries whose needFlags
 *           bit is not set in the selector flags are skipped).
 *   group   label used for the per-group update counter in asm 172
 *           (F, M1..M5). A read that has no group is only visible via asm 170.
 *   image   the asm 170 register image (124 words, 8 legacy blocks). Every good
 *           read refreshes the image words it covers; words not refreshed in
 *           the active set simply age out (per-word 1000 ms quality in asm 170).
 *   fast    the asm 172 compact image (DCE_FAST_WORDS addresses, in order).
 */
#ifndef DCE_POLL_TABLE_H
#define DCE_POLL_TABLE_H

#include <stdint.h>

#define DCE_POLLSET_STATUS      0u
#define DCE_POLLSET_RUN         1u
#define DCE_POLLSET_SETUP       2u
#define DCE_POLLSET_COUNT       3u
#define DCE_POLLSET_DEFAULT     DCE_POLLSET_SETUP   /* boot + selector timeout */

/* Selector heartbeat: if asm 173 sel_seq does not change for this long the RIOP
 * falls back to DCE_POLLSET_DEFAULT. */
#define DCE_POLLSEL_TIMEOUT_MS  3000u
/* Selector flags (asm 173 flags byte) */
#define DCE_SELFLAG_Y           0x01u               /* poll Y[0..9] 1000..1009 */

/* Per-word freshness for asm 170 quality and link staleness. */
#define DCE_STALE_TIMEOUT_MS    1000u

/* ------------------------------------------------------------------ groups */
enum { DCE_G_F = 0, DCE_G_M1, DCE_G_M2, DCE_G_M3, DCE_G_M4, DCE_G_M5, DCE_G_COUNT, DCE_G_NONE = 0xFF };

/* ------------------------------------------------------------------- reads */
typedef struct { uint16_t start; uint8_t words; uint8_t group; } dce_rd_t;

enum {
    RD_F504 = 0, RD_F600, RD_M1, RD_M2, RD_M3, RD_M4, RD_M5, RD_STAT110, RD_Y1000,
    /* legacy full-scan blocks not already above */
    RD_L0, RD_L200, RD_L300, RD_L400, RD_L500, RD_L600, RD_COUNT
};

static const dce_rd_t dceReads[RD_COUNT] = {
    [RD_F504]   = {504u,  2u, DCE_G_F},    /* Count (ULONG)                          */
    [RD_F600]   = {600u,  4u, DCE_G_F},    /* Percent, PercentLinear (FLOAT x2)      */
    [RD_M1]     = {100u, 22u, DCE_G_M1},
    [RD_M2]     = {204u, 12u, DCE_G_M2},
    [RD_M3]     = {301u, 13u, DCE_G_M3},
    [RD_M4]     = {401u,  6u, DCE_G_M4},
    [RD_M5]     = {606u, 19u, DCE_G_M5},
    [RD_STAT110]= {110u,  5u, DCE_G_NONE}, /* AlarmState 110 .. ErrorCode 114        */
    [RD_Y1000]  = {1000u,10u, DCE_G_NONE}, /* Y[0..9] plot data                      */
    [RD_L0]     = {0u,   12u, DCE_G_NONE},
    [RD_L200]   = {200u, 16u, DCE_G_NONE},
    [RD_L300]   = {300u, 15u, DCE_G_NONE},
    [RD_L400]   = {400u, 12u, DCE_G_NONE},
    [RD_L500]   = {500u,  8u, DCE_G_NONE},
    [RD_L600]   = {600u, 29u, DCE_G_NONE},
};

/* -------------------------------------------------------------------- sets */
typedef struct { uint8_t rd; uint8_t needFlags; } dce_sched_t;

static const dce_sched_t dceSetStatusFixed[] = { {RD_STAT110, 0u}, {RD_F504, 0u} };

static const dce_sched_t dceSetRunFixed[]    = { {RD_F504, 0u}, {RD_F600, 0u} };
static const dce_sched_t dceSetRunRr[]       = {
    {RD_M1, 0u}, {RD_M2, 0u}, {RD_M3, 0u}, {RD_M4, 0u}, {RD_M5, 0u},
    {RD_Y1000, DCE_SELFLAG_Y},
};

/* SETUP = the G47 full scan, one block per cycle, 8 blocks, 124 words. */
static const dce_sched_t dceSetSetupRr[]     = {
    {RD_L0, 0u}, {RD_M1, 0u}, {RD_L200, 0u}, {RD_L300, 0u},
    {RD_L400, 0u}, {RD_L500, 0u}, {RD_L600, 0u}, {RD_Y1000, 0u},
};

typedef struct {
    const dce_sched_t *fixed; uint8_t nFixed;
    const dce_sched_t *rr;    uint8_t nRr;
} dce_set_t;

#define DCE_N(a) ((uint8_t)(sizeof(a) / sizeof((a)[0])))
static const dce_set_t dceSets[DCE_POLLSET_COUNT] = {
    [DCE_POLLSET_STATUS] = { dceSetStatusFixed, DCE_N(dceSetStatusFixed), 0, 0 },
    [DCE_POLLSET_RUN]    = { dceSetRunFixed,    DCE_N(dceSetRunFixed),    dceSetRunRr,   DCE_N(dceSetRunRr) },
    [DCE_POLLSET_SETUP]  = { 0, 0,                                        dceSetSetupRr, DCE_N(dceSetSetupRr) },
};

/* ------------------------------------------------------ asm 170 image map */
/* The 124-word asm 170 register image (DO NOT reorder: PC decoders and
 * dce_tag_table.h index into it). */
typedef struct { uint16_t startAddr; uint16_t words; uint16_t baseIndex; } dce_image_blk_t;
static const dce_image_blk_t dceImageBlocks[] = {
    {0u, 12u, 0u}, {100u, 22u, 12u}, {200u, 16u, 34u}, {300u, 15u, 50u},
    {400u, 12u, 65u}, {500u, 8u, 77u}, {600u, 29u, 85u}, {1000u, 10u, 114u},
};

/* ----------------------------------------------- asm 172 compact word list */
/* Holding-register address of each 16-bit word in asm 172 data area, in order.
 * Matches dce_hmi_replica/fastrun_plan.json (33 words, byte offset 28..93).
 * Words are refreshed whenever any read covers the address. Capacity is
 * DCE_FAST_WORDS (a _Static_assert in dce_task.c enforces it). */
#define DCE_FAST_WORDS 33u
static const uint16_t dceFastAddr[DCE_FAST_WORDS] = {
    504, 505, 600, 601, 602, 603,                        /* F  */
    100, 101, 103, 104, 105, 110, 114, 116, 119, 121,    /* M1 */
    204, 205, 213, 215,                                  /* M2 */
    301, 303, 306, 311, 312, 313,                        /* M3 */
    401, 402, 406,                                       /* M4 */
    606, 607, 623, 624,                                  /* M5 */
};

#endif /* DCE_POLL_TABLE_H */
