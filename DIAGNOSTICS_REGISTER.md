# RIOP / NAFE diagnostics register

N10 (2026-11-12). Owner column is the bot that maintains the hook. Nothing here is a flash or a push instruction.

Status: **KEEP** = part of the standing scheme. **TEMP** = added to catch a problem; remove when the removal criterion is met.

EIP assembly **177** is a new 80-byte little-endian read-only input (explicit Get Attribute Single, class 0x04, attribute 3). It does not use or overlap 171. Failure to create 177 does not block 100–176.

Dump tool: `nafe_bench/riop_diag177.py --bind 192.168.1.33 --select 192.168.1.160`.

Architecture: the EIP application (asm 177, NAFE, LED, session counts) runs on the M7. NETC, EMDIO and lwIP run on the M33. N10 therefore polls MAC/SI counters and the recover path on the M33 (`ethernet-ip/M33LEADER/source/riop_diag_m33.c`) and publishes a 64-byte snapshot at reserved OCRAM1 `0x204FDF00` (256 bytes below the RPMSG window). The M7 copies that snapshot into asm 177. Do not call `fsl_netc` or bounce the netif from the M7 image; those drivers are not linked there.

---

## Firmware counters (asm 177)

| Item | Status | Purpose / evidence | Removal criterion | Owner |
|---|---|---|---|---|
| `uptime_s` | KEEP | Seconds since this boot. Distinguishes a hung board from a recent reset when a dropout is reported. | Permanent. | EI Software NextGen Dev |
| `boot_count` | TEMP | Increments in a `.noinit` word so a warm reset is visible. Starts at 1 after POR or if `.noinit` is not kept by the linker. | Promote to KEEP once a warm-reset increment is proven on the bench, or replace with a real NVS counter. Remove the `.noinit` hack if it proves noisy after POR. | EI Software NextGen Dev |
| `reset_cause` | KEEP | Raw `SRC_GENERAL->SRSR` snapshot (POR, WDOGx, CM7/CM33 request or lockup, JTAG, etc.). Tells us whether a dropout ended in a reset and why. | Permanent. | EI Software NextGen Dev |
| `rx_frames` / `tx_frames` | KEEP | Sum of NETC switch-port MAC valid frames and station-interface frame counts. If TX keeps climbing and RX freezes, the stall is inbound. | Permanent. | EI Software NextGen Dev |
| `rx_errors` / `tx_errors` | KEEP | MAC FCS/error and TX bad-FCS counts. A burst of errors at the dropout points at PHY/cable/EMI rather than the stack. | Permanent. | EI Software NextGen Dev |
| `dma_stall` | KEEP | SI “lack of Rx BDs” plus program/bus errors. A rising count with a live PHY is a DMA/ring stall, which is the leading theory for the 11/11–11/12 dropouts. | Permanent. | EI Software NextGen Dev |
| `dma_overflow` | KEEP | MAC `rxDiscard`. Distinguishes “frames dropped at the MAC” from “frames never arrived”. | Permanent. | EI Software NextGen Dev |
| `last_rx_tick_ms` / `last_tx_tick_ms` | KEEP | `xTaskGetTickCount` in milliseconds of the last observed increment of the RX or TX counters. Together with `uptime_s`, this is the quiet-time that arms self-recovery. | Permanent. | EI Software NextGen Dev |
| PHY link mask + `link_change_count` | TEMP (MDIO) / KEEP (mask) | Switch-port link inferred on the M33 from "port has seen a good RX frame". A dedicated Clause-22 BMSR poll (PHY addrs 2/5/4/7) is not wired yet because GOAL already owns EMDIO on the M33; opening a second NETC_MDIOInit handle would race the stack. `flags.MDIO_OK` stays clear until that hook uses GOAL's existing PHY poll. | Promote MDIO to KEEP when the M33 poller reads BMSR through GOAL's handle (no second init). Revisit PHY addresses if Electrical Dev maps ETH2 differently. | EI Software NextGen Dev (PHY map: EI Electrical Dev if a mismatch shows up) |
| `lwip_pbuf_err` / `lwip_mem_err` | TEMP | Copied from `lwip_stats` on the M33 when `LWIP_STATS` is on. In the N10 build `GOAL_CONFIG_IP_STATS` is off (M7 `m_data` is 97.8% full; do not turn it on just for these counters), so these read back 0 and `flags.LWIP_STATS` is clear. | Enable `GOAL_CONFIG_IP_STATS` (or a narrower MEM/MEMP hook) on the M33 once there is data-RAM headroom, then KEEP. | EI Software NextGen Dev |
| `eip_session_count` | KEEP | Cumulative I/O connection `FORWARD_OPEN` events. A dropout that leaves this unchanged happened after the scanner was already connected (or before any connection). | Permanent. | EI Software NextGen Dev |
| `last_cip_tick_ms` | KEEP | Last CIP activity: I/O connection event or a Get of asm 177. If this freezes while `uptime_s` runs, the EIP stack stopped taking requests. | Permanent. | EI Software NextGen Dev |
| `nafe_scan_count` | KEEP | Same `s_rt.scan_count` already published in asm 174/176. If it keeps rising after EIP looks dead, the M7 app is still running and the failure is the network path. | Permanent (already part of the NAFE path). | EI Software NextGen Dev |
| `enet_reinit_count` | TEMP | How many times the N10 self-recovery ran. | Keep until the dropout root cause is closed; then either remove the counter with the recover path, or KEEP it as a rare-event health field. | EI Software NextGen Dev |

`flags` bits: `LWIP_STATS` (0), `RECOVER_EN` (1), `PHY_LINK` (2), `RECOVERED` (3), `MDIO_OK` (4).

---

## Self-recovery hook

| Item | Status | Purpose / evidence | Removal criterion | Owner |
|---|---|---|---|---|
| ENET RX-stall recover (`RIOP_ENET_RX_RECOVER`, default **1**; timeout `RIOP_ENET_RX_RECOVER_SEC=5`, grace 15 s, cooldown 30 s) | TEMP | Runs on the M33 (NETC + lwIP owner), not the M7. If a switch port has seen RX and then no RX counter moves for N seconds, bounce the lwIP default netif (down/up + link down/up) and graceful-stop the switch-port MACs. The M7 only copies `enet_reinit_count` and flags from the OCRAM1 snapshot at 0x204FDF00. **Does not write NAFE or DCE config**, so a RAM-only SET_CH is left alone. Compiled out by setting `RIOP_ENET_RX_RECOVER` to 0. | Remove or harden after **two weeks with zero dropouts**, or replace with a handle-based NETC DMA-ring reinit once that path is proven safe. If recover runs and the board still stays silent, stop (do not tighten the timeout) and use the counters for root cause. | EI Software NextGen Dev |

This is deliberately not a full `goal_drvEthNetcShutdown` + re-init: that path frees the RX/TX rings and is not safe to call at runtime from the GOAL loop.

---

## Test tool

| Item | Status | Purpose / evidence | Removal criterion | Owner |
|---|---|---|---|---|
| `nafe_bench/riop_diag177.py` | KEEP | Read-only dump of asm 177 (broadcast ListIdentity, then Get). Use after every dropout and after the first flash of N10. | Permanent companion to asm 177. | EI Software NextGen Dev |

---

## Bench tools already in use (11/11–11/12)

| Item | Status | Purpose / evidence | Removal criterion | Owner |
|---|---|---|---|---|
| RIOP dropout watchdog (`nafe_bench/riop_watchdog.py` + policy) | TEMP | Detects .160 going quiet, logs the dropout, and (when not paused) can LabJack-cycle power and re-apply the ch0 4-wire SET_CH. Auto-cycle is **paused** until Daniel says go. Claim-aware; the LabJack outlet also feeds the CR3000 and Legacy's DCE. | Turn auto-cycle off for good once the dropout root cause is fixed and N10 recover (or a later DMA reinit) holds for **two weeks with zero drops**. Keep the detect-only logger as KEEP if it stays useful. | EI Software NextGen Dev |
| PT100 day logger (`nafe_day_log.py`) + `live_plot_day.py` | TEMP (this characterization run) / KEEP (the tools) | Day-long 4-wire PT100 log and the live plot. Proves the NAFE path is stable across hours and timestamps the last good sample at a dropout (22:03:38 ET on 11/11). Today's file: `N9_logs/pt100_4wire_ch0_day_20261112.csv`. | Stop the daily run when the dropout work is closed. Keep the scripts. | EI Software NextGen Dev |
| `nafe_gui.py` | KEEP | Read-mostly bench GUI (ListIdentity, live 174, optional verified 175). Running it is not a write unless someone clicks Write. | Permanent bench tool. | EI Software NextGen Dev |
| OVERRIDE flag relabel (asm 174) | KEEP | Status 0x41 is VALID\|OVERRIDE; bit 0x40 is OVERRIDE (forced/config override), not overload. Header bit 0x10 is OVERRIDES_ACTIVE. A misread here looks like a sensor fault. | Permanent. Do not treat OVERRIDE as a NAFE error. | EI Software NextGen Dev |

---

## Layout of asm 177 (80 bytes, little-endian)

| Off | Type | Field |
|---:|---|---|
| 0 | u16 | version (1) |
| 2 | u16 | flags |
| 4 | u32 | uptime_s |
| 8 | u32 | boot_count |
| 12 | u32 | reset_cause |
| 16 | u32 | rx_frames |
| 20 | u32 | tx_frames |
| 24 | u32 | rx_errors |
| 28 | u32 | tx_errors |
| 32 | u32 | dma_stall |
| 36 | u32 | dma_overflow |
| 40 | u32 | last_rx_tick_ms |
| 44 | u32 | last_tx_tick_ms |
| 48 | u8 | phy_link_mask |
| 49 | u8 | reserved |
| 50 | u16 | link_change_count |
| 52 | u32 | lwip_pbuf_err |
| 56 | u32 | lwip_mem_err |
| 60 | u32 | eip_session_count |
| 64 | u32 | last_cip_tick_ms |
| 68 | u32 | nafe_scan_count |
| 72 | u32 | enet_reinit_count |
| 76 | u32 | reserved |

N10 image is built only. It is **not flashed** until Daniel holds the ISP straps and gives the go.
