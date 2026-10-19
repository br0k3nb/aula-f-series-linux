# Wired keymaps and macros

Native evidence: `new-49a970.c` is REA's recovered key assignment converter, `new-49b420.c` builds matrix layers and macro memory, and `application-evidence.json` includes macro serializer `FUN_0049b050`, evidence `ev_81982ede403656d063f9250896ef511057ab0cb8676bfdcef7665802a86292f7`. `oem-disassembly.txt` cross-checks converter49a970, VK→HID4821d0 and macro serializer49b050. This is reconstructed protocol behavior, not original vendor source. The parser `FUN_00411800` sets MacroBufferSize default4096; the shipped models have no override. The apply stack's0x2800 upper bound is not the model memory size.

Matrix read command83 / write03, subcommand layer0..3. Each layer is512 bytes,128 four-byte records; last record is reserved with final bytes5a,a5. Physical indexes derive from the final KB.ini KEY value; do not use K-number as matrix index. API rejects index127, preserves all other records and verifies exact whole-layer readback. Model-specific non-remappable keys and unavailable layers should be disabled in the UI.

Assignment bytes in transfer order:

| Assignment | Four bytes |
|---|---|
| Keyboard key / two-key combination | `00 modifierMask HID2 HID1` |
| Modifier-only combination | `00 modifierMask 00 00` |
| Consumer/media usage | `02 00 usageHi usageLo` |
| Disable | `00 00 00 00` |
| Macro | `03 trigger repeat index` |
| Original native special | canonical word big-endian, e.g Win lock `07 00 00 01` |

ModifierMask is USB standard, bits0..7 leftCtrl, leftShift, leftAlt, leftGUI, rightCtrl, rightShift, rightAlt, rightGUI. Ordinary HID usages include A04, Z1d, Esc29, Enter28, F13a. Left/right modifier usagesE0..E7 may also be assigned as individual keys. Media examples play/pauseCD, nextB5, previousB6, stopB7, volume+E9, volume−EA, muteE2, brightness+6F, brightness−70, calculator192, email18A, browserHome223. Read decoder returns type,code,code2,modifiers,raw.

Original mouse minor actions:11left→`01 01 01 00`,12right→`01 03 01 00`,13middle→`01 02 01 00`,14back→`01 04 01 00`,15forward→`01 05 01 00`,16double-left→`01 01 02 32`,17triple-left→`01 01 03 32`,19scrollUp→`01 08 00 00`,1ascrollDown→`01 09 00 00`,1b→`01 07 ff 32`,1c→`01 06 ff 32`. Native Mac mode swapsback/forward; Linux follows Windows-mode converter.

Macro trigger1=count,2=until released,4=until any key pressed. Repeat1..255 is meaningful only for count; other modes store1. Index is zero-based and bounded0..99 by the native temporary offset-table size.

Macro memory read85 / write05, sub0,4096 bytes in eight512-byte chunks. The firstLE16 offset equals4*macroCount; table entries areLE16 offset,LE16 blobLength, followed by contiguous blobs. A blob is one-byte nameLength, UTF16LE name(max30 code units), then4-byte actions. Empty memory has zero first table entry. Appending reconstructs table offsets shifted4 while preserving every old blob and index. It refuses unknown/malformed layouts or exhausted memory. No automatic retry, erase or rollback.

Action byte0=(kind<<4)|(delay>>16), with releasebit80 for kinds0..2. Bytes1/2 are remaining delay high/low; byte3 is value. Delay is20-bit, maximum1048575ms. Kinds:0keyboard HID,1modifierHID(E0..E7),2mousebuttonmask1..31,3signedX,4signedY,5signedwheel. Signed motion ranges−128..127, nonzero. E.g A-down then100ms A-up: `00 00 00 04 80 00 64 04`.

Implemented files: `src/keymap.h`, `src/keymap.c`, `src/keymap_cli.h`, `src/keymap_cli.c`; isolated tests `tests/test_keymap.c`, `tests/test_keymap_cli.c`. Generic framing comes from lighting_ext.

CLI:

- `--keymap NODE --layer N` reads a layer asJSON withkeys andraw_hex.
- `--remap NODE --layer N --key IDX --action key|combo|media|mouse|special|disable|macro --code NUMBER [--code2 HID] [--modifiers MASK] [--repeat N] [--trigger once|hold|toggle]`. Special codes restricted to corroborated shipped native commands. Disable takes no code. Macro index must already exist.
- `--macro-list NODE` returnsJSON macros,index,name,events(kind,code,pressed,delay_ms),used,capacity.
- `--macro-add NODE --name NAME --events FILE --layer N --key IDX [--repeat N] [--trigger once|hold|toggle]`. File isCSV rows `kind,code,pressed(0/1),delay_ms`, comments beginning# accepted. New memory verified before assigning the key. If key write fails afterward, error identifies the stored index for recovery.
- `--macro-update NODE --index IDX --name NAME --events FILE` edits/renames an existing macro in place. It rebuilds the table while preserving its count, every macro index, all unrelated macro blobs, and all keymap layers. It rejects out-of-range indexes or insufficient memory before writing; exact pool readback verifies the update. Playback trigger/repeat values remain in key assignments and are unchanged.
- `--macro-delete NODE --index IDX` verifies all four keymap layers before removal. It refuses deletion while the chosen or any later macro is assigned; this preserves indices of assigned macros. Unassign them before deletion. Remaining unassigned macro indices aboveIDX decrease1.

All reads open writable handles because a native feature query usesSET_FEATURE followed byGET_FEATURE; this queries configuration and does not read typed input. No hardware mutations were performed by this agent.
