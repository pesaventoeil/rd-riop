# RIOP / EIP demo — project status for owner review

> **2026-10-26 16:30 update — read [`HANDOFF_2026-10-26.md`](HANDOFF_2026-10-26.md) first.** Board boots standalone (BOOT_MODE 100, no fuse burn), `6410533` EIP Class 1 green at DHCP `192.168.1.160`. §5 blockers 1–3 below are resolved/superseded.

**Audience:** Daniel + original RIOP / App Code Hub project owners  
**Tone:** Lab bring-up notes (soft conclusions; not a failure report)  
**As of:** 2026-10-26  
**Primary health check (when running):** PC EtherNet/IP Class 1 (`eip_rawstep.py`), not CODESYS

---

## 1. What this project is

Working tree for **NXP Remote IO Platform (RIOP)** EtherNet/IP sample plus a **CentriFeeder Modbus / DCE** path:

| Layer | Role |
|---|---|
| NXP `rd-riop` ACH **v1.8.0**, MCUX SDK **25.09.00**, GOAL EIP stack | Baseline industrial protocol sample on i.MX RT1180 (CM33 leader + CM7 follower) |
| CentriFeeder additions | FreeModbus RTU master on CM7 (`dce_task`) → overlays FlowRate / Totalization into EIP input assembly **100** (`adc15` / `adc16` @ bytes **89** / **93**) |

**Intended data path (when EIP is healthy and Modbus is enabled):**

```
DCE (RS-485, LPUART10, 38400 8-N-2, slave 1)
  → dce_task / FreeModbus FC03
  → dce_dataGet() → appl_eipTxPayloadPrepare()
  → MCTC → CM33 GOAL → PC Class 1 scanner @ 192.168.1.3
```

**Runtime gate:** `DCE_MODBUS_RUNTIME` defaults to **0** in `appl/ethernet-ip/riop/dce_task.h` — FreeModbus stays linked, idle (`DCE_MB_IDLE_LINKED`); no LPUART/TPM/`mbpoll` until EIP is green again.

**Known-good EIP milestone (git):** commit **`6410533`** — pure NXP baseline, Class 1 / MCTC historically healthy.

---

## 2. Asset map — where things live

### Application / source

| Asset | Absolute path |
|---|---|
| Active working tree | `C:\Users\dpesavento.EIL\Documents\GitHub\daniel-sandbox\rd-riop-EIP-demo` |
| Milestone worktree (`6410533`) | `C:\Users\dpesavento.EIL\Documents\GitHub\daniel-sandbox\rd-riop-EIP-6410533` |
| GOAL stack | `...\rd-riop-EIP-demo\ethernet-ip\goal\` |
| App + assemblies | `...\appl\ethernet-ip\riop\goal_appl.c` (CM7) |
| DCE Modbus task | `...\appl\ethernet-ip\riop\dce_task.[ch]` |
| FreeModbus port | `...\appl\ethernet-ip\riop\modbus\` |
| FreeModbus core | SDK `middleware/freemodbus/` (**not in repo**) |
| RIOP FlexSPI FCB (weekend experiments) | `...\common\m33\xip\riop_flexspi_nor_config.c` |
| Recovery / morning checklists | `...\RECOVERY.md`, `...\MORNING.md` |
| This status note | `...\PROJECT_STATUS.md` |

### Bench tooling (`mrlabjack`)

| Asset | Absolute path |
|---|---|
| Bench root | `C:\Users\dpesavento.EIL\mrlabjack` |
| Cursor ↔ Grok bridge | `...\CURSOR_TO_GROK.md`, `...\GROK_TO_CURSOR.md` |
| Claims protocol | `...\BENCH_CLAIMS.md`, `...\bench_claim.ps1`, `...\claims\` |
| Lua strap hold | `...\strap_hold_flexspi.lua`, `...\STRAP_HOLD_LUA.md`, `...\_lua_strap_hold.py` |
| PowerTail / cold POR notes | `...\POWER_TAIL_USB.md`, `...\ljpower.py` |
| LinkServer device JSON | `...\rt1180_b_qspi.json`, `...\rt1180_b_opi.json`, `...\rt1180_b_opi_nosdp.json` |
| SPT / bootable workspace | `...\spt_work\build\` (e.g. `bootable_spi_fcb_28000000.bin`, OPI/SPI/QSPI dumps) |
| NOR backup (weekend SPI-FCB leave) | `...\backup_nor_spi_fcb_20261024_003452.bin` |
| Fuse-burn prep (Daniel only) | `...\MONDAY_BT_FUSE_SEL.md` |
| Change rollup since `d300831` | `...\RIOP_CHANGES_SINCE_d300831.md` |
| SPT Port-B OPI notes | `...\SPT_RT1180_B_OPI.md` |

### Secure Provisioning / flashables

| Asset | Absolute path |
|---|---|
| SPT workspace (9/16-era settings) | `C:\Users\dpesavento.EIL\secure_provisioning0\` |
| `write_image` bat | `...\secure_provisioning0\write_image_win.bat` |
| Sept16 AHAB / `.simg` | `...\secure_provisioning0\bootable_images\riop_EIP_M33LEADER_cm33.bin` (+ `.simg`) |
| Flashloader FCB dumps | `mrlabjack\bootable_spt_write_28000000.bin` (OPI), `...\bootable_spt_qspi_write_28000000.bin` (QuadSPI) |
| LinkServer binary (typical) | `C:\nxp\LinkServer_26.6.137\LinkServer.exe` |
| Secure Provisioning Tool | `C:\nxp\SEC_Provi_26.03\` |

### EIP Class 1 host tools

| Asset | Absolute path |
|---|---|
| Primary poll CLI | `C:\Users\dpesavento.EIL\nextgen_eip\v2\eip_rawstep.py` |
| Assemblies (RIOP) | T2O **100**/97, O2T **150**/97, config **151**/10 — see `RECOVERY.md` §5 |

### Licensed / not in repo

| Asset | Note |
|---|---|
| **RIOP EIP App Glue v1.8.0** (`appl/common/`) | Licensed NXP deliverable — must be dropped in for CMake configure; absence looks like a missing submodule |
| FreeModbus core | Lives in MCUX SDK, not this git tree |

---

## 3. Bench topology (Daniel-PC)

| Item | Current lab practice |
|---|---|
| Boot straps | LabJack T7 (`labjack:470026553`) — **not** DIP eyeballing. Lua hold: `strap_hold_flexspi.lua` |
| FlexSPI **RUN** (today) | FIO0 `BOOT_MODE0` = **0**, FIO1 `BOOT_MODE2` = **1** → `BOOT_MODE` **100** |
| **ISP** | FIO0 = **1**, FIO1 = **0** → serial downloader; ROM mboot on UART |
| POR assist | FIO2 / Lua `por` pulse (~100–150 ms); prefer over PowerTail-only “cold” |
| Power | PowerTail (`ljpower.py`) switches wall 24V; **true cold** needs 24V + USB hub VBUS + probe unplugged (see `POWER_TAIL_USB.md`) |
| Console UART | CH340 → **COM13**, **115200** for app; ROM/mboot **COM13 @ 57600** |
| Debug / flash | LPC-Link2 (CMSIS-DAP) + LinkServer; claim `usb:jlink` |
| Ethernet | Goal IP **`192.168.1.3`**; PC side use **ETH4 only** for this RIOP subnet (avoid other NICs / HMI `.90` during bring-up) |
| HMI | **Off** during this arc — do not expect Crimson / `192.168.1.90` |
| Claims | `bench_claim.ps1` — check / claim / renew / release before LabJack, power, probe, or `.3` |

---

## 4. What has been tried (chronological)

Condensed from `CURSOR_TO_GROK.md` / weekend bridge. Soft outcomes only.

| When | Work | Outcome (short) |
|---|---|---|
| Pre–2026-10-23 | `6410533` EIP + MCTC; Modbus ladder through `d300831`; `DCE_MODBUS_RUNTIME=0` gate | Known-good EIP story; RS-485 deferred |
| 10/23 | Custom OPI / Spi2Xpi FCB + `XIP_BOOT_HEADER_ENABLE=1`; SPT/LinkServer program | After RUN: SFDP **0B** / quiet UART & `.3` |
| 10/23 | PowerTail “cold” + ISP reflash | ISP restores SFDP; RUN cold boot still quiet; OPI FCB re-wedges probe path |
| 10/23 | **SPI/QSPI BootROM FCB** (no Spi2Xpi) | SFDP stays **8MB** after RUN; app still quiet — BootROM does not hand off |
| G3 | GDB after RUN cold boot | PC in **BootROM** (`0x100xxxxx`), not app XIP |
| G5–G6 | AHAB/layout review; NOR backup; `6410533` source zip | Layout coherent; no flashable known-good Port-B reference yet |
| G7 | blhost on COM13 @57600 after RUN | ROM **serial-download fallback** after FlexSPI fail (not pin-selected ISP) |
| G8 | Sept16 / DEMO AHAB under same SPI FCB | Still BootROM-stuck — not a weekend-only container quirk alone |
| SPT0 | `secure_provisioning0\write_image_win.bat` over COM13 | Flashloader FCB + Sept16 OK; FCB is **OPI** → SFDP 0B after RUN; restored SPI FCB |
| G9 | Fuse read (no write) | `CONNECTION_SEL` already **Port B**; **`BT_FUSE_SEL=0`**; pin mode **100** = FlexSPI **Port A** per AN14589 |
| G10 | Flashloader **QuadSPI** Port-B option words | FCB generated OK; RUN cold boot still quiet (pin Port A) |
| G11–G12 | History vs strap map | No LabJack-reachable pin mode boots Port-B NOR without fuse path; G12 skipped |
| G13–G15 | Debugger / LinkServer Port-B QSPI start | Image readable; reach `SystemInit`; no healthy UART/IP yet |
| G16 | `DCE_MODBUS_RUNTIME=1` build-only | Builds; comms-lost path looks safe; gate restored to 0 |
| 10/26 | Worktree `rd-riop-EIP-6410533` + LinkServer Port-B load | Programs; debugger still stalls in/after `SystemInit` class |
| **G17** | Breakpoint map on `6410533` | Hang **#1:** `Prepare_CM7` → `InitCM7DMA` (DMA4 DONE). Hang **#2:** OPI `flexspi_init` vs SPI FCB leave image |
| 10/26 | **Dual-skip** (`Prepare_CM7` + `flexspi_init`) | **UART banner** seen (`Start the digital_io example…`); McInit / secondary-core prints in some runs; **`.3` still unreachable** in logged prep free-run |

---

## 5. Current leave state and blockers

### Leave state (lab)

- Flash: **`6410533`-class app** at `0x2800B000` with weekend **SPI Quad FCB** at `0x28000400` (recoverable via ISP + `bootable_spi_fcb_28000000.bin` / NOR backup).
- Straps: Lua **RUN** held (`BOOT_MODE` **100**); outlet typically **ON**.
- Cold FlexSPI boot from pin mode 100: still expected to miss Port-B NOR (see G9/G11).
- Debugger path: dual-skip proves **app can print**; full EIP / `.3` not restored in the logged free-runs.

### Blockers (for owners to weigh)

1. **Cold boot (deferred):** Pin `BOOT_MODE=100` selects FlexSPI1 **Port A**; RIOP MXIC is on FlexSPI1 **Port B**. Fuse fields already favor Port B but **`BT_FUSE_SEL` is still 0**, so fuse boot config is not applied. See `MONDAY_BT_FUSE_SEL.md` — **Daniel’s OTP call only**.
2. **Debugger bring-up (active path):** Without BootROM handoff, `Prepare_CM7`/DMA4 and runtime OPI `flexspi_init` vs SPI FCB need a durable recipe (clock DMA4, SPI-safe flexspi, or documented dual-skip → McInit → scheduler → NET). Banner ≠ Class 1 yet.
3. **EIP Class 1:** Not re-proven on this board since the boot arc; do not enable live Modbus (`DCE_MODBUS_RUNTIME=1`) or chase RS-485 until `.3` + `eip_rawstep` are green.

---

## 6. Explicit non-goals / deferred

| Item | Status |
|---|---|
| **`BT_FUSE_SEL` burn** | Documented for Monday decision; **do not** burn from automation |
| **DCE RS-485 / live Modbus** | Deferred until EIP Class 1 healthy with gate = 0 |
| **Crimson HMI / `.90`** | Out of scope for this bring-up arc |
| **Further FCB LUT thrash** | Stopped after OPI / SPI Quad / SPI1PAD / flashloader A/B — prefer fuse path or known-good Port-B cold boot |
| **CODESYS as primary scanner** | Optional secondary only; prefer PC Class 1 scripts |

---

## 7. Pointers for “original project” review

| Topic | What to tell / ask owners |
|---|---|
| **Milestone** | Git **`6410533`** = last confirmed EIP + MCTC healthy pure NXP baseline (`RECOVERY.md` §2). Worktree: `rd-riop-EIP-6410533`. |
| **App Glue** | **RIOP EIP App Glue v1.8.0** under `appl/common/` is **licensed and not in git** — clones fail configure without it. |
| **SDK / tools** | MCUX **25.09.00**, MCUXpresso for VS Code ≥ 25.03, ACH **rd-riop 1.8.0**, SPT/SPSDK **26.03**, LinkServer as installed under `C:\nxp\`. |
| **Boot header story change** | Pre-weekend: `XIP_BOOT_HEADER_ENABLE=0` — relied on **pre-programmed** NOR FCB + SPT AHAB-only exports. Weekend: in-image FCB experiments + merged bootables. |
| **Port A vs Port B** | Historical “it booted” on this bench may have been **debugger / LinkServer Port-B driver** rather than pin-mode cold FlexSPI. Worth confirming with owners how production RIOP boards are fused / strapped. |
| **Modbus status** | Port-layer fixes and runtime gate are in the working tree; **not** the current blocker for Class 1. |
| **Deep logs** | Prefer `CURSOR_TO_GROK.md` RESULT blocks and `RECOVERY.md` over raw `g*_*.log` dumps. |

---

## Quick links

- Bring-up / rebuild: [`RECOVERY.md`](RECOVERY.md)  
- Morning flash checklist: [`MORNING.md`](MORNING.md)  
- Agent bridge (facts & G1–G17): `C:\Users\dpesavento.EIL\mrlabjack\CURSOR_TO_GROK.md`  
- Fuse prep (owner only): `C:\Users\dpesavento.EIL\mrlabjack\MONDAY_BT_FUSE_SEL.md`
