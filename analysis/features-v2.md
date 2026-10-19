> Superseded by features-status.md for implementation and live charging-indicator findings. This file records the earlier static-only inventory.

# Original feature inventory and retained protocol evidence

This secondary investigation used only the extracted installer files and existing REA exports; no Windows execution or device writes. `application-evidence.json` retains the observed Ghidra pseudocode with evidence IDs; `oem-disassembly.txt` is a decoded machine-code cross-check. UI resources establish offered controls, not that every model enables them. Offsets below are into the 128-byte configuration payload, not the HID report; report payload begins at byte 8.

## Features established by resources

| Feature | Evidence | Scope / caveat |
|---|---|---|
| 18 effects, custom per-key lighting, lighting off | `extracted/app/Text/en/text.xml:199–218`; each `Dev/kb/*/KB.ini` LedOpt1–20 | Fixed, breathe, rainbow, flash, rain, wheel, ripples, stars, shadow, snake, neon, reaction, sine, scanning, windmill, waterfall, blossom, storm. |
| Color picker, custom palette, multicolor toggle | English XML:88–89,98–99; KB.ini RGBIndex=0,1,2; LedOpt last two capability columns | Random/multicolor and color are disabled for effects 3,15,16,17; off and custom have separate behavior. Fixed effect has no speed control. |
| Brightness / speed | English XML:96–97; KB.ini Light,LightHW,Speed,SpeedHW | F75 brightness0–9, F87/F990–4; speed0–4. |
| Per-key RGB and gaming color group | English XML:184–187; KB.ini GaoshouGroupNum=1, GaoshouKey1, GaoshouIndex=18, GameRGBIndex | Drag selection and one gaming group; transport distinct from regular effect palette. |
| Wireless idle sleep | English XML:108,110–111,123; F75/F99 KB.ini SleepUI/SleepHW | F75/F99 values60,90,120,180,240,300,600,900,1200 seconds map to2,3,4,6,8,10,20,30,40. F87 lacks explicit tables; parser defaults apply, so don't blindly reuse F75 values. |
| Debounce | English XML:128; all KB.ini ShowDebounce=0x04000101 | Native UI formats stored value plus1 (machine address4634ae). Exact list semantics should be checked before claiming milliseconds. |
| Key remapping and FN layers | English XML:92,102–107,184–197; KB.ini KEY/FN1 sections | Assign keyboard, mouse, multimedia, combination, command, disable, macro. F99 KeyNotToSet restricts1–12; F75 FnNotToSet=0. |
| Win lock / keyboard lock | English XML:144–145; skins/fun55x55/adv_lockwin.png and adv_lockall.png | Evidence is an assignable command category; a standalone global Win-lock config toggle is not established. |
| Macros | English XML:21–45,93; native macro serialization | Name/folder/create/delete/record/edit, timing, repeat count, repeat until release/other key, mouse motion, import/export. Full apply serializes matrix+macro pool. |
| Profiles and import/export | English XML:100–101,113–116 | Local profile format and device slot association still need reconstruction. |
| Audio-reactive lighting | English XML:228–248; model ET/*.txt | System sound, gain, automatic gain, smooth, effects, color. Host-side audio analysis and streaming required. |
| Factory reset / firmware update | English XML:12–19,95,124–125; F87 KB.ini CmdReset=0 | Reset behavior differs by configuration. Firmware update is separately destructive and has no justified Linux implementation from current evidence. |
| Charging indicator control | User-reported feature | **Not established in these shipped resource strings or retained native exports.** There is no English charge/charging label, or literal Chinese 充电/充電 in OemDrv.exe. Do not map an unrelated config field to it. |

## Native evidence useful for implementation

- `FUN_0049bc50`, evidence `ev_f7e0150938cc216ab2305ee008f274cd5c0ec2a65c667d3d2dc776e58218ebb8`: regular config. `cfg[9]` custom selector; `cfg[10]` hardware effect; `cfg[11]` gaming group index plus StartGp(default0x20); `cfg[0x38:0x3a]` bothff. Nineteen effect pairs at0x3a+2*effectIndex contain brightness then speed(high nibble)|colorIndex(low nibble). RandomVal(default7) supplies multicolor index; a disabled random flag produces0.
- Same fill helper: `cfg[3]` debounce raw state when ShowDebounce nonzero; `cfg[0x16]` Tap amount or0 when disabled; `cfg[0x18]` SleepHW lookup or0 when disabled. `cfg[0x17]` inverted automatic detection only when ShowAutoDetect==1. It is **not charging light control**. Parser evidence `ev_c07526289a07c502609b9aa530f742f628600b47d3dd2ae070a3248b0a966e38` explicitly associates device+0x2d58 with ShowAutoDetect(default0).
- Do not confuse low-delay fill helper `FUN_0049b960` (`ev_3c8acb55bfaadd073b597043eca012f0c75bfdddf5e501f6d47601563f6f2756`) with regular Fw24 layout. It stores debounce at2, Tap at8 and Sleep at9 and logs those labels. Branch choice is dev+0x2d8f, supplied by SubFw (default0); shipped models have no override.
- `FUN_0049c080`, captured via request0x49c158, evidence `ev_60b8a5582f8e061a6b13cf52c434b5f18ed4bf11a15fd4e2f376646070d372f5`: regular RGB palette is21 bytes per hardware effect (seven RGB triplets). All shipped models set RGBIndex=0,1,2. It builds0x1a4 bytes, then CRC1 pads to512 and places5a,a5 at510,511 before virtual+0x7c SetLedRgbTab. This whole-table initialization does not itself establish safe read-modify-write; use GetLedRgbTab to preserve unrelated palettes.
- `FUN_0049a290`, captured via0x49a414, evidence `ev_128f83eb39e251cd14fc8927f2c2d7c4873cff290715c830a4288a0e0675dd56`: GetLedRgbTab command0x8a, sub0, report6 for Fw24; up to512-byte chunks using existing520-byte feature transport. Palette write helper still needs active REA trace to confirm command.
- Same main apply: per-key game group has three planar color channels at offsets0,126,252, each logical matrix index mapped from KB.ini key data. It sends378 bytes through virtual+0x74. This is distinct from regular21-byte palette rows.
- Main apply writes up to4 keymap layers, each MatrixLen*4 bytes via virtual+0x5c, and then shared macro pool through+0x64. `FUN_0049b050`, evidence `ev_81982ede403656d063f9250896ef511057ab0cb8676bfdcef7665802a86292f7`, serializes name as length-prefixed UTF16LE, then4-byte actions. Action byte0 packs high4 delay bits, type bits, and0x80 for release; byte1/2 contain remaining delay; byte3 is HID/action value. Delay capped0xfffff. This alone is insufficient to implement matrix references/macros safely.
- `FUN_0049a490`, evidence `ev_8126c1c92c7de3e23edfc28edd7fed286c7c4e6a866d31a11d6dc5bf50f419b5`: generic reset command0x11, sub0, one-byte payload. F87 CmdReset=0 means main apply takes config-reset path; no Linux reset recommendation made.
- `FUN_0049a6c0`, captured via0x49a7d8, evidence `ev_147ab429ba4d13576ffddd809861d5e9894951f98d0b3c3bfe98f183e4903f03`: ReadPower command0x87, sub2,2-byte response. It reads battery/charge status and does **not** toggle charge LEDs. All shipped models set ShowPower=0.

## Unknowns requiring additional native evidence or device capture

1. Charging-indicator enable/disable: no proven byte, bit, command or persisted setting.
2. RGB write command, read response/header behavior, and physical color byte order should be confirmed against the actual wired keyboard.
3. Debounce UI bounds/unit; F87 sleep defaults; Tap semantics and bounds.
4. Matrix HID encoding, per-layer constraints, macro indexing/offset table; local import/export format.
5. Wireless handshake, receiver response assembly and write validation; firmware update behavior; audio/per-key streaming.
