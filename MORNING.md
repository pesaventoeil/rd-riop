# Morning checklist — RIOP after overnight prep

> **2026-10-26:** Flash via `mrlabjack\known_good_6410533\flash_spt_opi.ps1` (NOT LinkServer), then a TRUE power cycle. Board IP is DHCP (was `.160`), not `.3`. See [`HANDOFF_2026-10-26.md`](HANDOFF_2026-10-26.md).

## Already in the tree
- `DCE_MODBUS_RUNTIME` defaults to **0** in `appl/ethernet-ip/riop/dce_task.h`  
  FreeModbus stays linked; no LPUART/TPM/`mbpoll` until you set it to `1`.
- Prefer PC health check over CODESYS — see `RECOVERY.md` §5.
- Bot quiet file: `C:\Users\dpesavento.EIL\mrlabjack\GROKGRUNT_QUIET.json`  
  (delete when finished so GrokGrunt can use LabJack / `.3` again)

## 1. Boot straps (LabJack, not DIP eyeballing)
With Kipling closed and `GROKGRUNT_QUIET.json` present:

| FIO | Net | FlexSPI RUN (`BOOT=100`) |
|-----|-----|---------------------------|
| FIO0 | BOOT_MODE0 | Output **0** |
| FIO1 | BOOT_MODE2 | Output **1** |
| FIO2 | POR (optional) | pulse low ~100 ms, then Hi-Z |

Keep the LJM handle open while pins must stay Outputs (or they float ~0.55 V as Inputs).

## 2. Flash
- Preset: **flexspi_nor_release** (M33LEADER sysbuild embeds M7).
- LinkServer / LPC-Link2 (CMSIS-DAP). Binary may be at `C:\nxp\LinkServer_26.6.137\LinkServer.exe`.
- Power-cycle or POR after program.

## 3. Health check (PC scripts — primary)
```bat
cd C:\Users\dpesavento.EIL\nextgen_eip\v2
set PYTHONPATH=%CD%
python eip_rawstep.py --host 192.168.1.3 --o2t-size 97 --t2o-size 97 --o2t-instance 150 --t2o-instance 100 --config-instance 151 --value 0.0 --rpi-ms 500
```

**Success with runtime gate = 0:** Forward_Open OK, T2O cycling, **adc15/adc16 (offsets 89/93) = 0.0 / 0.0**.

Optional GDB on M7: `p dceDiag` → `mbState = DCE_MB_IDLE_LINKED`.

## 4. After green
Only then set `DCE_MODBUS_RUNTIME` to `1`, rebuild/flash, wire RS-485, bring up DCE.
Delete `mrlabjack\GROKGRUNT_QUIET.json` when overnight claim is over.
