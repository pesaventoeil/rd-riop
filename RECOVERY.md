# RECOVERY.md — rd-riop-EIP-demo

How to get this repo back to a known-good, CODESYS-connected, Modbus-polling
state from nothing. Keep this file current: it is the thing you read when
the MCTC breaks again and you can't remember what was working.

Last updated: 2026-09-15.

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
(byte offsets 89 / 93) ──▶ MCTC ──▶ CM33 GOAL ──▶ CODESYS.

---

## 2. Git history and the known-good points

Branch: `main`. All work is on this one branch.

| Commit | Meaning |
|---|---|
| `6410533` **Confirmed: CODESYS connects, MCTC healthy** | Pure NXP baseline. If everything else is on fire, `git checkout 6410533` and prove the board still talks to CODESYS before touching anything. |
| `71ce78e` Modbus stack compiles and links into M7 build; baseline still connects | FreeModbus core + port layer in the build but idle. |
| `2e8c22b` Add .gitignore; stop tracking build output | |
| `0e4c296` **Step 4: wire dce_task into assembly (dummy generator); CODESYS shows 123.45/climbing** | End-to-end path through MCTC to CODESYS proven with fake data. |
| `3912750` Set TPM6 source clock to 80 MHz to match baseline clock_config | |
| `f3cc42b` Reapply "Add LPUART10 RS-485 pin mux (AD_32/33/35) for DCE Modbus" | Current HEAD as of this writing. |

Tag the two milestones so they survive any future rebase:

```bash
git tag -a baseline-codesys-ok  6410533 -m "Pure NXP baseline, CODESYS connects, MCTC healthy"
git tag -a dummy-data-to-codesys 0e4c296 -m "dce_task dummy generator visible in CODESYS"
```

### Uncommitted on 2026-09-15 (commit these first)

* `appl/ethernet-ip/riop/dce_task.c` — real FC03 poll replaces the dummy
  generator; `dceDiag` bench counters replace the `0xAA...` GDB markers.
* `appl/ethernet-ip/riop/dce_task.h` — `DCE_DIAG_T`, `dce_diagGet()`.
* `appl/ethernet-ip/riop/modbus/modbus_serial.c` — duplicate
  `LPUART_Init()` removed.
* `appl/ethernet-ip/riop/modbus/modbus_timer.c` — TPM arm-path fixes.
* `RECOVERY.md` — this file.

Both cores built clean against this tree at 15:01 local (`.ninja_log`), so
it is safe to commit as-is:

```bash
git add appl/ethernet-ip/riop RECOVERY.md
git commit -m "Step 5: real FC03 poll of DCE regs 504-507 + dceDiag bench counters"
git tag -a fc03-poll-untested HEAD -m "FC03 master compiled, not yet run against a DCE"
```

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

## 5. Bench procedure — CODESYS + real DCE

### 5.1 Before touching the DCE (EtherNet/IP only)

1. Build + flash the current tree. Power-cycle the board.
2. CODESYS: the RIOP device should go to **Running** (green) on the
   EtherNet/IP scanner, same as at `0e4c296`.
3. `adc15` / `adc16` in the input assembly should read **0.0 / 0.0** — not
   123.45. That proves the real `dce_task` is in the image and, with no DCE,
   `valid` stays false.
4. Attach GDB (LinkServer) to the **M7** ELF and read:
   ```
   p dceDiag
   ```
   Expect `mbState = DCE_MB_RUNNING`, `polls` climbing ~10/s, `timeouts`
   tracking `polls`, everything else 0. Anything else:
   * `DCE_MB_INIT_FAILED` → `xMBMasterPortSerialInit` or `xMBMasterPortTimersInit` returned FALSE (clock gate, TPM root).
   * `DCE_MB_ENABLE_FAILED` → event queue / semaphore creation failed (heap).
   * `polls` frozen → `dce` task never woke; check `dceMbReady`, task creation.
   * `otherErrors` climbing → `MB_MRE_MASTER_BUSY`/`ILL_ARG`: run-resource semaphore not released; look at `portevent.c`.
5. **Scope GPIO_AD_35 (DE) and GPIO_AD_32 (TX).** DE must idle LOW and go
   HIGH for exactly the 8-byte request burst (~2.3 ms at 38400), every
   100 ms. If DE is inverted, clear `LPUART_MODIR_TXRTSPOL_MASK` in
   `modbus_serial.c`. If DE never moves, the pin mux for AD_35 is not
   `LPUART10_RTS_B` (commit `f3cc42b`).
6. On the TX line, decode the request. It must be
   `01 03 01 F8 00 04 <crc> <crc>` — addr 1, FC03, **start 0x01F8 = 504**,
   4 regs. If it says `01 F7` or `01 F9` the fork applies an offset;
   adjust `DCE_REG_COUNT_ADDR`.

### 5.2 With the DCE

7. Wire A/B to the DCE's RS-485 (2-wire). Match polarity; a MAX3485 will
   happily receive nothing at all if A/B are swapped.
8. On the DCE front panel: Drop Number 1, 38400, Modbus **enabled**. The DCE
   only runs `eMBSPoll()` while enabled — silence is normal otherwise.
9. `p dceDiag` again. Expect `good` climbing with `polls`, `timeouts`
   static. `lastRegs` = `{Count_hi, Count_lo, CWratio_hi, CWratio_lo}`.
   * `rxErrors` climbing → framing: confirm 2 stop bits on the DCE side and
     that T3.5 (1750 µs) is actually expiring — scope TPM6 by toggling a
     spare pin in `prvvTIMERExpiredISR` if needed.
   * `exceptions` climbing → DCE rejected the request: wrong slave address
     or register range. Try a PC Modbus tool on a USB-RS485 dongle first to
     confirm the DCE answers FC03 @ 504 x4 at all.
10. CODESYS `adc15` should now show `Count / CWratio` and `adc16` a slowly
    climbing integral. Confirm the number against the CR3000 / Maple HMI
    display of the same DCE.

### 5.3 Open question to settle on the bench

`usMBReg_Count` (504) is either a rate proxy or an accumulating counter.
The DCE map also exposes `usMBReg_Percent` (600, float, % of full scale),
`usMBReg_PercentLinear` (602) and `usMBReg_EFS` (502, float, full-scale in
EU). If `Count / CWratio` climbs monotonically while feeding, it is the
**total**, and FlowRate should come from `Percent × EFS / 100` instead.
Watch `lastRegs` for ~30 s of feeding and decide. Then delete the TODO in
`dce_task.c`.

Also still to verify: `MB_POLL_TASK_PRIORITY` (5) vs. the GOAL EIP task
priority on M7 — grep `xTaskCreate` under `ethernet-ip/goal/plat/`.

---

## 6. If MCTC breaks again

Symptoms last time: ~30 timeouts / ~45 RPC attempts, `sizeXferRemote = 0`
on both cores, both stuck in the sync handshake, `goal_eipAddConnection`
returned `0x80000000`. Already ruled out: TRDC, MPU/cache, buffer
orientation, RPC length mismatch, stray RPMSG-Lite task.

What actually fixed it: throw the tree away and rebuild from a pristine NXP
import (`6410533`), then re-apply the Modbus work file by file, building
and re-checking CODESYS after each step. Do not merge Modbus work into a
build tree that has ever been half-configured.

Re-apply order (each step is a commit above):

1. Pristine import → confirm CODESYS connects.
2. Add `modbus/` port files, not in build → still connects.
3. Add FreeModbus core + port to `M7FOLLOWER/CMakeLists.txt` → builds, still connects.
4. `dce_task` with dummy generator wired into `appl_eipTxPayloadPrepare` → CODESYS shows 123.45.
5. TPM6 clock root 80 MHz; LPUART10 pin mux AD_32/33/35.
6. Real FC03 poll (this commit).
