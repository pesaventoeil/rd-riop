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
| ENET RX-stall recover (`RIOP_ENET_RX_RECOVER`, default **0** since fix/n10-boot (was 1 in N10); timeout `RIOP_ENET_RX_RECOVER_SEC=5`, grace 15 s, cooldown 30 s) | TEMP | Runs on the M33 (NETC + lwIP owner), not the M7. If a switch port has seen RX and then no RX counter moves for N seconds, bounce the lwIP default netif (down/up + link down/up) and graceful-stop the switch-port MACs. The M7 only copies `enet_reinit_count` and flags from the OCRAM1 snapshot at 0x204FDF00. **Does not write NAFE or DCE config**, so a RAM-only SET_CH is left alone. Compiled out by setting `RIOP_ENET_RX_RECOVER` to 0. | Remove or harden after **two weeks with zero dropouts**, or replace with a handle-based NETC DMA-ring reinit once that path is proven safe. If recover runs and the board still stays silent, stop (do not tighten the timeout) and use the counters for root cause. | EI Software NextGen Dev |

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

---

## N10 boot failure (2026-11-12) - fix/n10-boot

N10 (7e6df68) flashed and verified at 13:40 ET 11/12 but never came up: no broadcast ListIdentity (bound to 192.168.1.33), no ping, no ARP, COM5 silent, after both a LabJack POR and a PowerTail cycle. Rolled back to N9 at 13:47 ET. Offline findings:

- Flash content is not the problem: `swd/postdumpN10_failed_20261112_1346.bin` SHA256 5DCD2E2A...2244 equals the N10 full image byte for byte; FCB/FCFB identical to N9; one AHAB container, one image at +0xA000, 550760 B, ends 0x38091768, well inside the 0xA0000 window.
- M7 image is statically clean: core1 261420 B in a 262144 B ITCM (724 B left); `.riop_diag_net` is NOLOAD and not in core1; `.noinit` (8 B) lands in DTCM after .bss; 5.6 KB between .bss end and the MSP stack limit; OCRAM1 0x204FDF00 is cacheable on the M7 and the reader invalidates before it reads; the SRC read is on the same AIPS (0x444x) as TRDC1, which the M7 already programs. The LED PWM adds no task or ISR.
- Leading suspect is the new M33 NETC poller (`riop_diag_m33.c`), which N9 does not have. It starts in the first scheduler slot, while the prio-31 GOAL init task may still be partway through NETC bring-up. It reads ENETC0_SI0 (an ENETC GOAL never uses) and ENETC1_SI1 (a VSI GOAL never enables). Its recover path graceful-stops every switch MAC with no restart and bounces the netif outside the tcpip thread.

| Item | Status | Purpose / evidence | Removal criterion | Owner |
|---|---|---|---|---|
| `RIOP_DIAG_M33_ENABLE` switch (`riop_EIP_M33LEADER.c`) | TEMP | Bisect: 0 = N10r (M33 behaves as N9, M7 identical to N10), 1 = N10f. | Remove the switch once N10f (or a later fix) passes a full day on the bench. | EI Software NextGen Dev |
| M33 poller start gate (netif up + link up + >=20 s, fallback 60 s) | KEEP | Ensures no NETC register access before GOAL has finished bringing up NETC and lwIP. | Permanent while the M33 poller exists. | EI Software NextGen Dev |
| ~~M33 SI list reduced to ENETC1 PSI0~~ | WITHDRAWN (N10g) | Wrong: RIOP is not in switch mode. GOAL builds only `GOAL_BOARD_ETH_PORT_4` (ENETC0 endpoint), so ENETC1 is never initialised. N10f read it and still killed the board. Replaced by the ENETC0_SI0-only poller below. | n/a | EI Software NextGen Dev |
| `RIOP_ENET_RX_RECOVER` = 0 | TEMP (disabled) | Not safe to re-enable as written: there is no MAC restart and the netif bounce runs outside the tcpip thread. | Re-enable only after it restarts the MACs and posts the bounce through `tcpip_callback()`, and after Daniel's go. | EI Software NextGen Dev |

Bisect images (built, **not flashed**): N10r = N10 minus the M33 poller; N10f = hardened poller; N10a = N9 + LED only (branch fix/n10-boot-ledonly). Flash N10r first.

---

## N10g: root cause and fix (2026-11-12 afternoon)

**Bench (14:0x-14:2x ET, hands-free via `swd_flash_test.ps1`):** N10r booted (broadcast ListIdentity bound to .33 answered about 35 s after power-on; asm 177 OK; reset_cause 0x00010001). N10f never answered across 8 broadcast tries from about 35 s to 3 min, so it was rolled back to N10r. The M33 poller's register accesses are the fault, not their timing.

**Root cause (offline, high confidence):** RIOP builds GOAL with **only the ENETC0 endpoint**: `GOAL_BOARD_ETH_PORT_4=1` and ports 0-3 = 0 under `#if RIOP` in `goal_target_board.c`. That means `kNETC_ENETC0EthPort`, RGMII, PHY address 3, `hasSwitch = FALSE` and `epConfig.si = kNETC_ENETC0PSI0`. GOAL never initialises the NETC switch or ENETC1. N10 and N10f both polled the **SW0 port 0-3 MAC PM0 counters** (64-bit registers on MAC blocks with no link clock or init) and **ENETC1 SI registers**. N10 also polled ENETC1_SI1. On an uninitialised NETC block that is a bus stall or bus error on the M33, which owns NETC, lwIP and GOAL, so all networking dies. No fault handler was recording, so it left no evidence. The earlier "switch-port" PHY link mask and the PHY addresses 2/5/4/7 in this register were wrong for RIOP; the only PHY is ENETC0 at address 3.

Secondary findings: OCRAM1 is **non-cacheable on the M33** (`CACHE_MODE_WRITE_THROUGH=1` MPU workaround), so no clean is needed. The N10 `DCACHE_CleanByRange(0x204FDF00)` was a no-op anyway, because the secure XCACHE table starts at 0x30000000. The RX-recover path (graceful MAC stop with no restart, netif bounce outside the tcpip thread) is deleted in N10g.

| Item | Status | Purpose / evidence | Removal criterion | Owner |
|---|---|---|---|---|
| M33 poller reads **only ENETC0_SI0** `SIRFRM0`/`SITFRM0` (32-bit), only after netif up + link + >=20 s, no time fallback | KEEP | The only NETC registers polled are those of the block GOAL itself brings up. They are read-only and have no read side effects. | Permanent while the poller exists. | EI Software NextGen Dev |
| `RIOP_DIAG_M33_MAC_COUNTERS` (ENETC0 port MAC `PM0_RERRN/TERRN/RDRPN`, low 32-bit word only, flag b6 MAC_CNT), default **0** | TEMP | Optional error counters from the endpoint MAC. Off until N10g has run clean for a day. | Turn on in a later build after N10g passes; KEEP if it stays clean for a week. | EI Software NextGen Dev |
| **M33 fault recorder** (`riop_fault_entry` + naked Hard/MemManage/Bus/Usage/Secure stubs in `semihost_hardfault.c`; SHCSR fault enables in `riop_fault_init`) | KEEP | Writes a 64 B magic-tagged 'N10F' record (count, type, CFSR, HFSR, BFAR, MMFAR, SFSR, SFAR, PC, LR, xPSR, EXC_RETURN, uptime) at OCRAM1 **0x204FDF40**, then `NVIC_SystemReset()`. Neither core's startup clears the window. The M7 copies count/CFSR/PC/BFAR into asm 177 v2. A crash now leaves evidence and reboots instead of hanging. The semihosting BKPT return path is unchanged. | Permanent. **Limit:** a pure bus *stall* never raises a fault, so it cannot be recorded. Pair with an M33 RTWDOG (not built yet) if one turns up. If OCRAM is not retained across `SYSRESETREQ` on this part, the record is lost, but `reset_cause` in asm 177 still shows the reset. | EI Software NextGen Dev |
| Poller self-disable after 2 recorded faults | KEEP | Stops a poller-induced reboot loop. The record is cleared only by power-off. | Permanent. | EI Software NextGen Dev |
| `RIOP_DIAG_M33_ENABLE` switch | TEMP | 0 = N10r/N10h (no poller), 1 = N10g. | Remove after N10g passes a full day. | EI Software NextGen Dev |

**asm 177 v2 = 96 B**; the first 80 B are unchanged, so v1 readers still work. Changes: version = 2; flags b5 `FAULT_SEEN`, b6 `MAC_CNT`; `phy_link_mask` bit0 = ENETC0 link. New fields: off 80 `fault_count` u32, 84 `fault_cfsr` u32, 88 `fault_pc` u32, 92 `fault_bfar` u32. `nafe_bench/riop_diag177.py` decodes v2 (nafe_bench commit d3b58f7, local).

Images (built, **not flashed**): **N10g** = fix (recorder + ENETC0_SI0-only poller), flash first. **N10h** = N10g with the poller off (recorder + asm 177 v2 only; branch fix/n10-boot-n10h) is the fallback if N10g does not answer.

### N10g bench result: PASS (2026-11-12 14:38 ET, RIOP B2)

- Broadcast ListIdentity bound to 192.168.1.33 was answered by 192.168.1.160.
- asm 177 v2 at uptime 259 s: rx_frames 2136, tx_frames 940, flags PHY_LINK, ENETC0 link up, **fault_count 0**, nafe_scan_count 244, reset_cause 0x00010001.
- ch0 4-wire PT100 SET_CH was rewritten and verified by readback. The day logger restarted (pid 42052, part6.csv).
- Root cause confirmed: the N10/N10f poller read the uninitialised switch and ENETC1. N10g reads only ENETC0_SI0 and boots clean. N10g is now the RIOP B2 firmware. The `RIOP_DIAG_M33_ENABLE` switch stays TEMP until a full day passes; `RIOP_DIAG_M33_MAC_COUNTERS` stays 0 for now.
