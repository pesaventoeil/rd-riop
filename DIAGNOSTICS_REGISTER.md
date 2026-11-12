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

---

## N10w: M33 hardware watchdog (2026-11-12, fix/n10-wdog, NOT flashed)

Daniel's go (t420u). Built offline from N10g (f104aa2). Covers what the N10g fault recorder cannot: a stalled bus access, IRQs masked, or a starved scheduler. In those cases no fault is ever raised, so the board would hang. The watchdog makes it reset and leave a record.

**How it works.**
- **Instance.** `RTWDOG1` is owned by the M33. NXP's RT1180 SDK pairs RTWDOG1 with the CM33 and RTWDOG3 with the CM7.
- **Reset type.** A timeout is a SRC global *system* reset: `SRMASK.WDOG1_MASK` is cleared, and `LP_HANDSHAKE` is not touched. Both cores, NETC and the TCMs restart. The ROM boots the M33, which reloads and releases the M7 exactly as after power-on, so the M7 needs no separate recovery path.
- **Clock and timeouts.** LPO / 256 (expected 128 Hz). The rate is measured against the core clock at boot. If it is out of range, the watchdog is left disarmed (`CLK_BAD`) instead of risking a reset loop.
- **ERR051578.** Per NXP's RT1180 rtwdog example, the first timeout only raises the IRQ, and the reset comes one TOVAL later. So:
  - Run phase: TOVAL 2 s gives the IRQ at 2 s and the reset at about 4 s.
  - Boot phase: TOVAL 15 s gives the IRQ at 15 s and the reset at about 30 s.
  - If this silicon does *not* have the erratum, the reset comes at the IRQ time (2 s / 15 s). Bench step 3 shows which.
- **Settings.** No window. `DBG=0`: the counter stops while the core is halted. `WAIT=1`, `STOP=0`. `UPDATE=1` stays set, so it can still be reconfigured.
- **Kicks.** Only from `riop_wdog` (`tskIDLE_PRIORITY+1`). Never from an ISR. A kick needs all three:
  - **GOAL main loop advancing.** A GOAL loop hook is registered at `GOAL_STAGE_GOAL` in `00_rpc_cc/goal_appl.c`. The GOAL main task runs at FreeRTOS priority 0 and yields, so any starvation stops it first.
  - **Diag poller publishing**, if it started.
  - **lwIP default netif up.** It is set up with the link forced up, so pulling the cable does not reset the board.
- **Boot phase.** No kicks until GOAL init is done, the hook has run and the netif is up. Then the task switches to the 4 s run timeout.
- **IRQ.** Priority 0, so it still runs inside FreeRTOS critical sections. In the run phase, once the IRQ fires the task stops kicking and the reset is certain. In the boot phase the IRQ only records.
- **Record.** `riop_diag_wdog_t`, 80 B, magic `'N10W'`, at OCRAM1 **0x204FDF80** (diag window +0x80, after the `N10F` fault record).
  - Every 250 ms: uptime + health flags breadcrumb.
  - The IRQ adds the interrupted PC, LR and the first 8 characters of the task name.
  - On the next boot the M33 moves these into `last_*`, sets `last_reason`, and bumps `wdog_count` and `consec`.
  - It is re-initialised on POR, or if it was not retained.
- **SRSR.** The M33 now snapshots `SRSR` into `boot_srsr` and clears it (W1C) before releasing the M7. `reset_cause` in asm 177 is therefore **this boot's** cause; before, it was cumulative since POR.
- **M7 SystemInit.** It would disable all RTWDOGs after the M33 armed RTWDOG1, so the M7 is now built with `-DDISABLE_WDOG=0`. The M33 SystemInit still disables everything at reset. The health task also re-arms RTWDOG1 if it ever finds `EN` cleared (state `REARM`).
- **SWD / ISP.** Under ISP straps the ROM serial downloader runs instead of this image, so LinkServer flashing never meets an armed watchdog. An SWD halt of the running image pauses the counter (`DBG=0`).
- **M7 size cost.** `.text` 245348 â†’ 245156 B (âˆ’192 B, because the M7 SystemInit no longer carries the wdog-disable code). `.bss` +32 B (two 16 B asm 177 buffers). M33 image `.text` +2912 B (528296 â†’ 531208, includes the embedded M7 image).

| Item | Status | Purpose / evidence | Removal criterion | Owner |
|---|---|---|---|---|
| **M33 RTWDOG1 health watchdog** (`riop_wdog_m33.c`, switch `RIOP_M33_WDOG` default 1 in `riop_diag_net.h`), with GOAL loop hook, IRQ record and per-boot `reset_cause` | KEEP | Turns a silent hang (bus stall, masked IRQs, starved scheduler, dead GOAL loop) into a ~4 s reset with evidence in asm 177 v3. | Permanent. Revisit the 2 s / 15 s TOVALs only if bench step 3 shows this silicon does not have ERR051578. | EI Software NextGen Dev |
| M7 built with `-DDISABLE_WDOG=0` | KEEP | Without it, the M7 SystemInit disables the M33-owned RTWDOG1. | Permanent while the M33 owns a watchdog. | EI Software NextGen Dev |
| **Boot-loop guard tuning**: `RIOP_WDOG_LOOP_N` = 3 consecutive WDOG resets, each within `RIOP_WDOG_LOOP_WINDOW_S` = 600 s of boot, put the next boot in safe mode. In safe mode the diag poller is off and the watchdog stays on. In safe mode the run phase starts after GOAL ready or 25 s. `consec` clears after 600 s of clean running or on any non-WDOG reset. | TEMP | Stops a bad poller or init path from reset-looping forever. N and M are first guesses. | Re-tune or confirm N/M after the N10wt bench run and two weeks of field data. Then promote the guard to KEEP with final values, and remove the TEMP note. | EI Software NextGen Dev |
| Boot TOVAL 15 s (reset ~30 s) | TEMP | Covers slow GOAL / M7 bring-up. | Measure real boot-to-run time from asm 177 (`wdog_state.RUN`, uptime) over 10 boots, then set to 3x the worst case and KEEP. | EI Software NextGen Dev |
| **N10wt stall test** (`RIOP_WDOG_STALL_TEST=1`; separate image, never default) | TEMP | Proves the reset path, the IRQ record and the guard with no write at all. After each power-on, at uptime 60 s it runs:<br>(1) a BASEPRI critical-section spin (expect `RUN_ISR` + PC)<br>(2) a PRIMASK spin, the same as a bus stall to the watchdog (expect `RUN_NOISR`, PC 0)<br>(3) a BASEPRI spin again, which is the 3rd consecutive reset, so the next boot is SAFE<br>Then it stops. | Remove the switch once N10wt has passed on the bench and N10w has run a day. | EI Software NextGen Dev |

**asm 177 v3 (112 B, first 96 B = v2).**

| Offset | Type | Field |
|---|---|---|
| 96 | u16 | wdog_reset_count (since POR) |
| 98 | u8 | wdog_last_reason: 0 none, 1 RUN_ISR, 2 RUN_NOISR, 3 BOOT_ISR, 4 BOOT_NOISR, 5 UNKNOWN (record not retained) |
| 99 | u8 | wdog_consec (loop guard) |
| 100 | u16 | wdog_state: b0 EN, b1 RUN, b2 SAFE, b3 CLK_BAD, b4 REARM, b5 TEST, b6 WDOG_BOOT, b7 FRESH |
| 102 | u8 | wdog_test_stage |
| 103 | u8 | reserved |
| 104 | u32 | wdog_last_pc (0 = IRQ did not run) |
| 108 | u32 | wdog_last_uptime_ms |

New `flags` bits: b7 `WDOG_EN`, b8 `WDOG_BOOT`, b9 `SAFE_MODE`. Decoder: `nafe_bench/riop_diag177.py` (local commit 1e74294).

**Limits.**
- If OCRAM1 is not retained across the WDOG system reset, `wdog_last_reason` reads 5 and the guard cannot count. `reset_cause` still shows WDOG1, and N10wt step 3 shows SAFE never appearing.
- A stall *inside* the IRQ path (IRQ priority 0 blocked by PRIMASK or a stuck bus) gives `RUN_NOISR` with no PC. That is the expected signature of a bus stall.
