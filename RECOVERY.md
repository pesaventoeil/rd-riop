# RECOVERY.md — rd-riop-EIP-demo

How to get this repo back to a known-good EIP-connected, Modbus-polling
state from nothing. Keep this file current: it is the thing you read when
the MCTC breaks again and you can't remember what was working.

Primary health check is the **PC EtherNet/IP Class 1 poll scripts** (not
CODESYS). CODESYS remains an optional secondary scanner.

Last updated: 2026-10-22. Morning flash checklist: see **MORNING.md**.

---

## 1. What this repo is

NXP RIOP (i.MX RT1180) EtherNet/IP sample (App Code Hub `rd-riop` v1.8.0,
SDK 25.09.00, GOAL EIP stack) plus the CentriFeeder additions:

| Piece | Where | Core |
|---|---|---|
| GOAL EtherNet/IP stack | `ethernet-ip/goal/` | CM33 (`M33LEADER`) |
| Application, assemblies | `appl/ethernet-ip/riop/goal_appl.c` | CM7 (`M7FOLLOWER`) |
| DCE Modbus RTU master | `appl/ethernet-ip/riop/dce_task.[ch]` | CM7 |
| FreeModbus port layer | `appl/ethernet-ip/riop/modbus/` | CM7 |
| FreeModbus core (master, RTU, FC03) | SDK `middleware/freemodbus/` — **not in repo** | CM7 |
| RIOP EIP App Glue v1.8.0 (`appl/common/`) | **licensed NXP deliverable, not in repo** | — |

Data path: DCE ──RS-485 (LPUART10, 38400 8-N-2, slave addr 1)──▶ `dce_task`
──▶ `dce_dataGet()` ──▶ `appl_eipTxPayloadPrepare()` writes FlowRate /
Totalization into input assembly 100, float slots `adc15_out` / `adc16_out`
(byte offsets 89 / 93) ──▶ MCTC ──▶ CM33 GOAL ──▶ PC Class 1 scanner
(or optional CODESYS).

---

## 2. Git history and the known-good points

Branch: `main`. All work is on this one branch.

| Commit | Meaning |
|---|---|
| `6410533` **Confirmed: EIP + MCTC healthy** | Pure NXP baseline. If everything else is on fire, `git checkout 6410533` and prove Class 1 (or optional CODESYS) before touching anything. |
| `71ce78e` Modbus stack compiles and links into M7 build; baseline still connects | FreeModbus core + port layer in the build but idle. |
| `2e8c22b` Add .gitignore; stop tracking build output | |
| `0e4c296` **Step 4: wire dce_task into assembly (dummy generator); adc15 shows 123.45** | End-to-end path through MCTC proven with fake data (historically via CODESYS). |
| `3912750` Set TPM6 source clock to 80 MHz to match baseline clock_config | |
| `f3cc42b` Reapply "Add LPUART10 RS-485 pin mux (AD_32/33/35) for DCE Modbus" | Pinmux only. |
| `d300831` **Step 5: real FC03 poll + dceDiag** | First commit that *starts* Modbus at runtime (LPUART10/TPM6 + mbpoll/dce). |

Tag the two milestones so they survive any future rebase:

```bash
git tag -a baseline-codesys-ok  6410533 -m "Pure NXP baseline, CODESYS connects, MCTC healthy"
git tag -a dummy-data-to-codesys 0e4c296 -m "dce_task dummy generator visible in CODESYS"
```

### Runtime gate (2026-10-22) — EIP recovery while keeping FreeModbus linked

`DCE_MODBUS_RUNTIME` defaults to **0** in `dce_task.h`. With that:

* FreeModbus core + port stay in `M7FOLLOWER/CMakeLists.txt` (same as `71ce78e`).
* `dce_taskInit()` only clears published data and sets `dceDiag.mbState = DCE_MB_IDLE_LINKED`.
* No `mbpoll` / `dce` tasks, no LPUART10/TPM6 init, no FC03 traffic.
* `goal_appl.c` still overlays `adc15`/`adc16` from `dce_dataGet()` → **0.0 / 0.0**.

Set `DCE_MODBUS_RUNTIME` to **1** only after PC EIP Class 1 connects again
and RS-485 is ready.

Known-good ladder for EIP (do not skip):

| State | Commit / flag | Expect |
|---|---|---|
| Pure NXP | `6410533` | PC EIP Class 1 Forward_Open OK |
| Modbus linked, idle | `71ce78e` or HEAD with `DCE_MODBUS_RUNTIME=0` | Class 1 OK; adc15/adc16 = 0.0 |
| Dummy floats | `0e4c296` | Class 1 shows 123.45 on adc15 |
| Live FC03 | `DCE_MODBUS_RUNTIME=1` | poll / timeouts; bus later |

Uncommitted local: `common/m7/source/init.c` may re-enable `BOARD_InitDebugConsole()` — leave it; unrelated to Modbus.

---

## 3. Rebuilding from a clean clone

1. Clone to a **short path** (`C:\rd-riop` was used originally; the current
   checkout at `...\GitHub\daniel-sandbox\rd-riop-EIP-demo` also works).
2. Drop the licensed **RIOP EIP App Glue v1.8.0** into `appl/common/`.
   Without it, CMake configure fails in a way that looks like a missing
   submodule. It is not a bug.
3. MCUXpresso for VS Code ≥ 25.03, SDK 25.09.00 imported as a repository,
   Arm GNU toolchain, `west`, `ninja`.
4. Open `ethernet-ip/M33LEADER` as the sysbuild project. The M7 image is
   built as `riop_EIP_M7FOLLOWER` under the M33 build dir and embedded as
   `core1_image.bin`. Preset: `flexspi_nor_release`.
5. If configure picks up stale IDE edits (`mcux_include.json`,
   `CMakePresets.json`), `git stash` them, configure, then decide what to
   keep. The extension will also silently add sources to the wrong
   `CMakeLists.txt` in `AUTO-ADDED BY MCUXPRESSO EXTENSION` blocks — grep
   for that string after any build that suddenly breaks.
6. Flash with LinkServer over the LPC-Link2 (CMSIS-DAP). Console: CH340E on
   LPUART1 → **COM9**, 115200.

### Things that are on purpose and look wrong

* `modbus/port.h` is a 3-line shim that includes `dce_port.h`. FreeModbus
  core hard-codes `#include "port.h"`; the shim keeps a generic `port.h`
  out of the include path while still satisfying the core. Do not delete it.
* `goal_appl.c` compiles into **M7**, not M33.
* `RIOP=1` comes from the build system, not from source.
* `MB_MASTER_TOTAL_SLAVE_NUM` is 2, not 1: some stack bounds checks are
  inclusive.
* The `mbpoll` task runs `eMBMasterPoll()`; NXP's demo ran it from a TPM
  ISR. This is deliberate — the event layer blocks on a queue.
* `usAddress` in `eMBMasterRegHoldingCB` is ignored: the stack passes
  address+1. Decoding is positional.

---

## 4. Modbus port layer — what was fixed vs. NXP's `modbus_rtu_client_cm33` demo

| File | Fix |
|---|---|
| `modbus_serial.c` | 8-N-2 (11-bit char) instead of the demo's 8-N-1; `MODIR[TXRTSE\|TXRTSPOL]` so the LPUART drives DE with exact frame timing; `vMBMasterPortClose()` really tears down; TX interrupt on Transmission Complete, not TDRE. |
| `modbus_timer.c` | Every arm re-programs prescaler + period (the demo's T35 inherited the 200 ms respond timeout → no end-of-frame detection); CNT reset on every arm (stale count → early expiry). |
| `portevent.c` | Rewritten for FreeRTOS: queue for protocol events, binary semaphore + result variable for request completion. Demo never cleared error flags, read an uninitialised local, and spun without yielding. |
| `modbus_port.c` | Real critical sections (PRIMASK save/restore with nesting). Demo's were empty. |
| `dce_port.h` | `MB_PORT_HAS_CLOSE=1`, master-only, critical-section macros wired. |
| `dce_app.h` | LPUART10 / TPM6 / 80 MHz / 32-bit TPM, with `_Static_assert` range checks. |

---

## 5. Prove EIP healthy again (PC poll scripts first)

Goal right now: **EIP + MCTC healthy with FreeModbus linked but idle.**
RS-485 is unwired; do not scope TX or chase Modbus poll success yet.
**Prefer the PC Class 1 tools below over CODESYS.**

### 5.0 PC EIP poll scripts (primary)

Preferred docs (CR3000 v5d pack README; same Class 1 tooling):

| Path | When |
|---|---|
| `/workspace/crimson-eip/v5d_zero_arm_test/README.md` | Cloud /agent workspace |
| `C:\Users\dpesavento.EIL\nextgen_eip\README.md` or `...\nextgen_eip\v2\README.md` | Local mirror (pack lives here) |

| Piece | Path |
|---|---|
| Class 1 CLI | `C:\Users\dpesavento.EIL\nextgen_eip\v2\eip_rawstep.py` (also root `nextgen_eip\`) |
| Class 1 library | `eip_class1.py` + `tag_map.py` + `eip_commander.py` (same folder) |
| In-repo CODESYS only | `ethernet-ip/plc-scripts/codesys/` — **optional secondary** |

No LabJack. Leave `DCE_MODBUS_RUNTIME=0` for this check.

RIOP assemblies (`appl/ethernet-ip/riop/goal_app_eip.h`) — override the
README’s CR3000 defaults (asm 2/3, 36/52):

| Direction | Instance | Size | Notes |
|---|---|---|---|
| T2O (adapter → PC) | **100** | **97** | input; `adc15` @ byte **89**, `adc16` @ **93** |
| O2T (PC → adapter) | **150** | **97** | output |
| Config | **151** | **10** | `--config-instance 151` |

Host: board IPv4 (GOAL sample default **`192.168.1.3`**). Only **one**
scanner — stop CODESYS if it holds Exclusive Owner. Inbound UDP **2222**
may be required (pack README firewall note).

```bat
cd C:\Users\dpesavento.EIL\nextgen_eip\v2
set PYTHONPATH=%CD%
python eip_rawstep.py --host 192.168.1.3 --o2t-size 97 --t2o-size 97 --o2t-instance 150 --t2o-instance 100 --config-instance 151 --value 0.0 --rpi-ms 500
```

Type `status` / `hold` on stdin; `stop` (or Ctrl+C) to Forward_Close.
Exit 0 = clean stop; 2 = Forward_Open rejected; 1 = connection lost.

**Success with `DCE_MODBUS_RUNTIME=0`:** Forward_Open OK, cyclic T2O
arrives (97 bytes), **adc15 / adc16 = 0.0 / 0.0** (not 123.45) — connection
healthy + idle DCE overlay. Optional GDB M7: `p dceDiag` → `mbState =
DCE_MB_IDLE_LINKED`, counters 0.

### 5.1 Bench checklist after flash

1. Confirm `DCE_MODBUS_RUNTIME` is **0** (`dce_task.h`).
2. Clean rebuild M33LEADER sysbuild, preset **`flexspi_nor_release`**.
   After configure, `git status` and grep `AUTO-ADDED BY MCUXPRESSO` in
   CMakeLists — stash IDE debris if present.
3. Flash via **SPT/OPI** (`mrlabjack\known_good_6410533\flash_spt_opi.ps1` or
   `mrlabjack\head_gate0\`), not LinkServer / in-image FCB
   (`XIP_BOOT_HEADER_ENABLE=0`). Then power-cycle: **preferred** LabJack
   PowerTail (`ljpower.py cycle`) when the probe is already unplugged;
   **fallback** true cold cycle (24V + RIOP USB + probe out until LED off)
   only if ListIdentity fails. Proven 2026-10-26 — see
   `HANDOFF_2026-10-26.md` and agent/personal store
   `RIOP_POWER_CYCLE.md` under
   `%LOCALAPPDATA%\Cursor\AgentStores\cursor_agent_stores\`
   `f0369756-0f84-467c-8591-15373493b82b\files\` (agent) and
   `t33809769-u458052730\files\` (personal).
4. Run §5.0 PC Class 1 poll. If Forward_Open fails:
   * GDB M7: `p dceDiag` → expect `DCE_MB_IDLE_LINKED`. If
     `DCE_MB_RUNNING` / `polls` climbing, the gate did not take effect.
   * If MCTC is dead again (~30 timeouts, `sizeXferRemote = 0`), see §6 —
     rebuild from `6410533` then re-apply Modbus with the gate still off.
5. Optional secondary: CODESYS RIOP scanner → **Running**, same adc15/adc16
   zeros (`ethernet-ip/plc-scripts/codesys/`).

When that passes, Modbus-in-image is safe. Enable live master later with
`DCE_MODBUS_RUNTIME=1` and only then do bus bring-up.

### 5.2 Live Modbus (after EIP is healthy; RS-485 next)

1. Set `DCE_MODBUS_RUNTIME` to 1, rebuild, flash, power-cycle.
2. GDB M7: `p dceDiag` — with no slave expect `mbState = DCE_MB_RUNNING`,
   `polls` ~10/s, `timeouts` tracking `polls`. Failures:
   * `DCE_MB_INIT_FAILED` → serial/timer init (clock gate, TPM root).
   * `DCE_MB_ENABLE_FAILED` → event queue / semaphore (heap).
   * `DCE_MB_CREATE_FAILED` → `xTaskCreate` failed (heap).
   * `otherErrors` climbing → run-resource; see `portevent.c`.
3. Wire A/B, enable Modbus on the DCE panel, then chase `good` / PC EIP
   adc15–adc16. Scope / register-map checks are deferred until then.

### 5.3 Open items (after the bus works)

* Totalization source: integrate rate vs. Count as total (TODO in `dce_task.c`).
* Priorities verified: mbpoll=5 (= GPIO), below AFE=7; GOAL Main on M7 ≈ 0.

---

## 6. If MCTC breaks again

Symptoms last time: ~30 timeouts / ~45 RPC attempts, `sizeXferRemote = 0`
on both cores, both stuck in the sync handshake, `goal_eipAddConnection`
returned `0x80000000`. Already ruled out: TRDC, MPU/cache, buffer
orientation, RPC length mismatch, stray RPMSG-Lite task.

What actually fixed it: throw the tree away and rebuild from a pristine NXP
import (`6410533`), then re-apply the Modbus work file by file, building
and re-checking PC EIP Class 1 (§5.0) after each step. Do not merge Modbus
work into a build tree that has ever been half-configured.

Re-apply order (each step is a commit above):

1. Pristine import → confirm PC EIP Class 1 connects (§5.0).
2. Add `modbus/` port files, not in build → still connects.
3. Add FreeModbus core + port to `M7FOLLOWER/CMakeLists.txt` → builds, still connects.
4. `dce_task` with dummy generator wired into `appl_eipTxPayloadPrepare` → adc15 shows 123.45.
5. TPM6 clock root 80 MHz; LPUART10 pin mux AD_32/33/35.
6. Real FC03 code linked, but keep `DCE_MODBUS_RUNTIME=0` until EIP is green.
7. Only then set `DCE_MODBUS_RUNTIME=1` for live poll.
