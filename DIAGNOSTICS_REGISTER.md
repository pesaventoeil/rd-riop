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

## N10w: M33 hardware watchdog (2026-11-12, fix/n10-wdog; N10w/N10wt FAILED on the bench, superseded by N10w2, see below)

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
| ~~M7 built with `-DDISABLE_WDOG=0`~~ | REMOVED in N10w2 | Prime suspect for the N10w no-boot (see N10w2). The M7 is back on the SDK default and disables RTWDOG1..5 at its reset, as in N10g. | n/a | EI Software NextGen Dev |
| **Boot-loop guard tuning**: `RIOP_WDOG_LOOP_N` = 3 consecutive WDOG resets, each within `RIOP_WDOG_LOOP_WINDOW_S` = 600 s of boot, put the next boot in safe mode. In safe mode the diag poller is off and the watchdog stays on. In safe mode the run phase starts after GOAL ready or 25 s. `consec` clears after 600 s of clean running or on any non-WDOG reset. | TEMP | Stops a bad poller or init path from reset-looping forever. N and M are first guesses. | Re-tune or confirm N/M after the N10wt bench run and two weeks of field data. Then promote the guard to KEEP with final values, and remove the TEMP note. | EI Software NextGen Dev |
| Boot TOVAL 15 s (reset ~30 s); N10w2: 30 s after arming (reset ~60 s), safe mode 300 s | TEMP | Covers slow GOAL / M7 bring-up. | Measure real boot-to-run time from asm 177 (`wdog_state.RUN`, uptime) over 10 boots, then set to 3x the worst case and KEEP. | EI Software NextGen Dev |
| **N10wt stall test** (`RIOP_WDOG_STALL_TEST=1`; separate image, never default) | TEMP | Proves the reset path, the IRQ record and the guard with no write at all. After each power-on, at uptime 60 s it runs:<br>(1) a BASEPRI critical-section spin (expect `RUN_ISR` + PC)<br>(2) a PRIMASK spin, the same as a bus stall to the watchdog (expect `RUN_NOISR`, PC 0)<br>(3) a BASEPRI spin again, which is the 3rd consecutive reset, so the next boot is SAFE<br>Then it stops. | Remove the switch once N10wt has passed on the bench and N10w has run a day. | EI Software NextGen Dev |

**asm 177 v3 (112 B, first 96 B = v2).**

| Offset | Type | Field |
|---|---|---|
| 96 | u16 | wdog_reset_count (since POR) |
| 98 | u8 | wdog_last_reason: 0 none, 1 RUN_ISR, 2 RUN_NOISR, 3 BOOT_ISR, 4 BOOT_NOISR, 5 UNKNOWN (record not retained) |
| 99 | u8 | wdog_consec (loop guard) |
| 100 | u16 | wdog_state: b0 EN, b1 RUN, b2 SAFE, b3 CLK_BAD, b4 REARM, b5 TEST, b6 WDOG_BOOT, b7 FRESH |
| 102 | u8 | wdog_test_stage |
| 103 | u8 | wdog_cal_err (N10w3+; 0 before): 0 OK/armed, 1 ULK_TMO, 2 RCS_TMO, 3 STUCK (32 kHz not counting), 4 WRAP, 5 RANGE, 6 ARM_FAIL, 0xFF setup not run yet |
| 104 | u32 | wdog_last_pc (0 = IRQ did not run) |
| 108 | u32 | wdog_last_uptime_ms |

N10w3+ also use the old v1 `reserved` u32 at offset 76 (always 0 before): 76 u16 `wdog_clk_hz` (measured RTWDOG1 tick, expect ~128), 78 u16 `wdog_cs_found` (RTWDOG1 CS as the M33 found it, before its first write). Same version (3) and size (112 B); all three read 0 on N10w/N10w2.

New `flags` bits: b7 `WDOG_EN`, b8 `WDOG_BOOT`, b9 `SAFE_MODE`. Decoder: `nafe_bench/riop_diag177.py` (v3 1e74294; N10w3 fields in the N10w3 decoder commit).

**Limits.**
- If OCRAM1 is not retained across the WDOG system reset, `wdog_last_reason` reads 5 and the guard cannot count. `reset_cause` still shows WDOG1, and N10wt step 3 shows SAFE never appearing.
- A stall *inside* the IRQ path (IRQ priority 0 blocked by PRIMASK or a stuck bus) gives `RUN_NOISR` with no PC. That is the expected signature of a bus stall.

---

## N10w2: fix for the N10w no-boot (2026-11-12, fix/n10-wdog, NOT flashed)

**Bench result (Daniel's bench run, 15:22-15:57 ET).** N10wt (twice, ~9.5 min) and N10w (~6 min) never answered ICMP to .160. The monitor pings about once a second and logs every change, and it never logged a single `True` until the N10g rollback. N10g answers ping about 8 s after power-on. So the board never reached the netif even briefly: a fast reset loop or an early hang, not the 15 s / 30 s boot timeout firing late. `swd_flash_test.ps1` rolled back to N10g each time. No dead-state capture was taken: the capture step needs the DCE HMI bot warned first, and this build agent has no channel to it.

**What N10w changed that runs before the netif** (diff f104aa2..fe28718), and how N10w2 handles each one:

| N10w change | Runs | Risk | N10w2 |
|---|---|---|---|
| M7 built with `-DDISABLE_WDOG=0`. Checked in the ELFs: the N10w/N10wt M7 `SystemInit` has 62 instructions and no RTWDOG writes; N10g and N10w2 have 188, with all five RTWDOG bases and key `0xD928C520`. | M7 reset, right after the M33 releases it (async MCMGR start, pre-OS on the M33) | **Prime suspect.** Every NXP SystemInit for both cores disables all five RTWDOGs because they can be live out of reset. RTWDOG3/4/5 sit in WAKEUPMIX with the M7, and NXP's own CM7 rtwdog example gets a reset from RTWDOG3 without touching SRMASK. If releasing the M7 leaves any of them running, the chip resets within its default timeout, before the netif, forever. The N10w loop guard only counted WDOG1, so it never reached safe mode. | Flag removed. The M7 `SystemInit` is byte-identical in function to N10g's. |
| `riop_wdog_boot()` armed RTWDOG1 pre-OS in `BOARD_InitHardware`: LPCG on, `SRMASK.WDOG1` unmasked **before** a 100 ms busy-wait calibration with EN=1, then IRQ prio 0 enabled. | M33 pre-OS, before `goal_targetBoardMcInit` | Secondary. A wrong clock during calibration could reset the chip. The arming also overlapped the async M7 start. | Pre-OS now only does record bookkeeping, the SRSR snapshot and the safe-mode decision. `riop_wdog_task` calibrates and arms 3 s after the scheduler starts, with `SRMASK.WDOG1` **masked** during calibration (unmasked only after a successful arm), using `vTaskDelay` rather than a busy-wait. |
| Boot TOVAL 15 s | after arming | It could not explain "never pinged". Without ERR051578 it would fire just before GOAL ready (~16 s), though. | 30 s after arming (~33 s after boot). Safe mode: 300 s. |
| Loop guard counted only `SRSR.WDOG1` | M33 pre-OS | A WDOG3/4/5 loop never reaches safe mode. | It counts any `SRSR.WDOG1..5` bit. The record also keeps `srsr_hist` (OR of every boot's SRSR since POR), `boot_seq`, `arm_s` and `srmask_boot` (bytes 68..79, in the former `reserved2`) for an SWD dump. |
| `goal_appl.c` stage hook | GOAL init | Reviewed: `goal_mainStageReg` returns `GOAL_ERR_WRONG_STATE` if the stage has passed, and `goal_mainLoopReg` returns `GOAL_ERR_ACCESS` once alloc is denied. Both are handled (HOOK_MISSING path). It cannot hang init. | Unchanged. |
| OCRAM record at 0x204FDF80 | | Reviewed: both linker scripts reserve the full 256 B `m_diag` window (M33: `m_cache_end` minus `m_diag_size` 0x100; M7: `m_diag_start` below `m_rpmsg`). Nothing else is placed there. | Unchanged. |
| asm 177 v3 (112 B) | M7 GOAL/CIP setup | No assembly size limit found in GOAL. Ping failing points below EIP. | Unchanged (v3, 112 B). If N10w2 still fails, build a 96 B variant to rule this out. |

**Images** (`N10_logs\fix_n10_boot_images.sha256`):
- `N10w2`: default (`RIOP_WDOG_STALL_TEST=0`).
- `N10wt2`: same tree with `RIOP_WDOG_STALL_TEST=1`, bench test only.
- The first N10w2 build attempt was stale: the M7 CMake re-run failed because `ARMGCC_DIR` was unset, and the old core1 image was reused. It was renamed `build\N10w2_flash_INVALID_stale_m7_cmake_failed`, then rebuilt with `ARMGCC_DIR` set. In the rebuild the M7 recompiled `system_MIMXRT1189_cm7.c` and core1 grew 261316 -> 261636 B.
- `make_full_img.py` builds the full SWD image from the N10g header page [0:0x1000], the unsparsed simg and 0xFF padding to 0xA0000. Checked: it reproduces the N10w full image byte for byte.
- Side note: nxpimage's sparse output marks all-zero 1 KB blocks as don't-care, so they are flashed as 0xFF. This was already true for N10g. Inside the embedded M7 `.data` image it only hits the idle, timer and main stack arrays, so it is harmless.

| Item | Status | Purpose / evidence | Removal criterion | Owner |
|---|---|---|---|---|
| RTWDOG1 armed from `riop_wdog_task` (`RIOP_WDOG_ARM_DELAY_MS` 3000), with `SRMASK.WDOG1` masked during calibration | KEEP (part of the watchdog) | Keeps the watchdog away from the async M7 start and from any pre-OS path. | Permanent. | EI Software NextGen Dev |
| Loop guard counting `SRSR.WDOG1..5`; record fields `srsr_hist` / `boot_seq` / `arm_s` / `srmask_boot` | KEEP (`srsr_hist`, `boot_seq`) / TEMP (`arm_s`, `srmask_boot`) | Turns a reset loop from any watchdog into SAFE mode, and leaves SWD-readable evidence. | Drop `arm_s` and `srmask_boot` after N10w2 has passed the bench and run a day. | EI Software NextGen Dev |
| `RIOP_WDOG_ARM_DELAY_MS` 3000 | TEMP | Margin after scheduler start. | Confirm `arm_s` <= 5 s over 10 boots, then KEEP. | EI Software NextGen Dev |

---

## N10w3: why N10w2 never armed (CLK_BAD) and the fix (2026-11-16, fix/n10-wdog, NOT flashed)

**N10w2 bench result (Daniel, flashed 09:32 ET 11/16, flash+verify True).** Boots fine: asm 177 v3 112 B, fault_count 0, ENETC0 up, reset_count 0, reason none. But `wdog_state` = 0x0088 (CLK_BAD, FRESH) and `WDOG_EN` clear: setup refused to arm. (The 09:24 ET attempt did not flash; the board kept N10g. `swd_flash_test.ps1` now reports `flashed=False` for that case.)

**Root cause: the unlock key, not the clock.**
- The SDK `SystemInit` on both cores (`DISABLE_WDOG=1`) disables RTWDOG1..5. It checks `CS[CMD32EN]`, which is 0 out of reset, so it unlocks with the 16-bit pair `0xC520`, `0xD928` and writes CS without setting CMD32EN (`system_MIMXRT1189_cm33.c` lines 79-89). CMD32EN stays 0.
- `wdog_write_cfg()` in N10w/N10w2 always wrote the 32-bit key `0xD928C520`. RT1180 RM rev 9, WDOG 74.3.2.3: the 32-bit unlock works only if CMD32EN = 1; with CMD32EN = 0 only the 16-bit or 8-bit writes unlock, and "an improper unlock sequence causes WDOG to reset".
- So `CS[ULK]` never set. `wdog_write_cfg()` timed out after 200000 polls, `wdog_calibrate()` returned 0 without measuring anything, and `wdog_setup()` set CLK_BAD. Its fail-path disarm used the same key, so it failed too, harmlessly. `SRMASK.WDOG1` was masked during calibration, so the WDOG1 reset request from the improper unlock went nowhere. That matches reset_count 0.
- **Probably also the N10w no-boot.** N10w did the same 32-bit unlock pre-OS in `BOARD_InitHardware`, but it cleared `SRMASK.WDOG1` *before* calibrating (fe28718 line 348, then 350). An improper unlock would then be a global system reset on every boot, before the netif. That fits "never answered ping". Not proven by a dump; the `-DDISABLE_WDOG=0` M7 is the other candidate.

**Clock (answer to "which clock").** RM rev 9 table 1084 (CS[CLK] options, chip-specific): 00 reserved, **01 = 32 kHz clock**, 10 reserved, 11 = 24 MHz RC. CCM table 133: WDOGn `lpo_clk` = OSC_32K, `ext_clk` = OSC_RC_24M. PRES = 1 divides by 256 (74.3.5). N10w2's CLK = 01 with PRES gives 32768 / 256 = **128 Hz**, so the clock choice, the prescaler and the 96..160 Hz window were right. Bus clock and 1 MHz are not selectable for RTWDOG on RT1180 (1 MHz is an EWM option). With the 24 MHz RC source the longest TOVAL is 0.7 s, too short for this design.

**Fix (N10w3).**
- `wdog_unlock()` and `wdog_kick()` follow `fsl_rtwdog` `RTWDOG_Unlock` / `RTWDOG_Refresh`: the 32-bit key when `CS[CMD32EN]` = 1, else the 16-bit pair (refresh pair with IRQs off). Checked in the ELF: `wdog_unlock` tests bit 13 and writes `0xC520`, `0xD928` or `0xD928C520` to `0x442D0004`.
- `wdog_write_cfg()` returns which step failed. Calibration window 200 -> 500 ms (~64 ticks, +-2 %). A counter that does not move is reported as STUCK (32 kHz clock dead), not as a bad rate.
- Evidence from hardware goes to the record (cal_err at 15, cs_found at 18, clk_hz at 16) and on to asm 177 (76 `wdog_clk_hz`, 78 `wdog_cs_found`, 103 `wdog_cal_err`). Same version and size.
- Unchanged from N10w2: arm 3 s after the task starts, WDOG1 reset masked during calibration, boot TOVAL 30 s, run TOVAL 2 s, safe 300 s, loop guard.

**Images** (`N10_logs\fix_n10_boot_images.sha256`), built 2026-11-16 09:41-09:42 ET with `ARMGCC_DIR` set. The M7 recompiled `riop_diag.c` and `goal_appl.c` (core1 261636 -> 261628 B), and core1 is embedded in both M33 images (embed check OK):
- `N10w3`: default (`RIOP_WDOG_STALL_TEST=0`).
- `N10wt3`: same tree with `RIOP_WDOG_STALL_TEST=1`, bench test only.
- Tree check: a rebuild of N10w3 differs from it only in the AHAB image hash (32 B) and the `__TIME__` banner (2 B).

**Expected asm 177 (N10w3, after a power-on):**
- Until setup runs (3.5 s after the scheduler starts): `wdog_state` 0x0080 (FRESH), `wdog_cal_err` 0xFF.
- After setup: `flags` include `WDOG_EN` (0x0084 with PHY_LINK). `wdog_state` is 0x0081 (EN, FRESH), then 0x0083 (EN, RUN, FRESH) once GOAL is ready and the netif is up (~16-20 s). `wdog_cal_err` 0, `wdog_clk_hz` 126..130.
- `wdog_cs_found` should show CMD32EN = 0 and EN = 0, about 0x0520 (RCS, UPDATE, CLK = 32 kHz). If it shows CMD32EN = 1, the root cause above is wrong.
- `wdog_reset_count`, `wdog_last_reason`, `wdog_consec`, `wdog_test_stage`, `wdog_last_pc` and `wdog_last_uptime_ms` all 0.
- If instead `wdog_cal_err` = 3 (STUCK, `wdog_clk_hz` 0), OSC_32K is not running on RIOP B2 (32.768 kHz crystal or BBSM). The watchdog stays disarmed and the board runs normally.

**Expected asm 177 (N10wt3, POR then hands-free):** boot 1 as N10w3, but `wdog_state` 0x00A3 (with TEST). At uptime 60 s the stall test starts and the reset lands ~4 s later (ERR051578):
- Boot 2: `reset_count` 1, reason 1 RUN_ISR, consec 1, stage 1, `last_pc` != 0, `last_uptime_ms` ~60000, state 0x0063.
- Boot 3: count 2, reason 2 RUN_NOISR, consec 2, stage 2, `last_pc` 0, state 0x0063.
- Boot 4: count 3, reason 1, consec 3, stage 3, state 0x0067 (SAFE), `flags` 0x0384 (WDOG_EN, WDOG_BOOT, SAFE_MODE, PHY_LINK). No further resets.
- `reset_cause` includes WDOG1 (bit 1) on boots 2-4. Every boot shows `clk_hz` ~128, `cal_err` 0. Reaching SAFE takes about 4 min from power-on.

| Item | Status | Purpose / evidence | Removal criterion | Owner |
|---|---|---|---|---|
| CMD32EN-aware unlock/refresh (`wdog_unlock`, `wdog_kick`) | KEEP (part of the watchdog) | Without it RTWDOG1 can never be reconfigured after SystemInit. | Permanent. | EI Software NextGen Dev |
| asm 177 `wdog_clk_hz` (76), `wdog_cs_found` (78), `wdog_cal_err` (103) and record `cal_err` / `cs_found` | KEEP `wdog_cal_err` / TEMP `wdog_clk_hz`, `wdog_cs_found` | Hardware evidence for the clock and the CMD32EN state. | Drop `wdog_cs_found` (and `clk_hz` from asm 177, keep it in the record) after N10w3 has armed on the bench over 10 power-ons with `clk_hz` 120..136. | EI Software NextGen Dev |
| Calibration window 500 ms | TEMP | +-2 % rate check. | Keep unless boot timing needs the 300 ms back. | EI Software NextGen Dev |

