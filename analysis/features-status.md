# Feature coverage — AULA Studio0.2

This checklist supersedes the earlier limited0.1 resource inventory. It does not
claim full vendor parity. All listed implemented write paths require physical
verification beyond the existing0.1 brightness test; mocked tests do not prove
firmware compatibility or perceived color/animation output.

| Original capability | Implementation | Verification / remaining limit |
|---|---|---|
| Built-in effects, off, brightness/speed | Native config read/modify/write | Earlier live brightness test passed; fixture tests |
| RGB picker and palettes | cmd8a/0a, seven RGB triplets per mode | Live read; actual palette marker at506; write test pending |
| Charging indicator | cfg0x12 boolean | User shortcut changed exactly01→00; user confirmed off; new write pending |
| Per-key custom colors | cmd86/06,126-entry planar RGB, custom mode21 | Live read; mocked byte preservation; write pending |
| Normal/FN key remapping | cmd83/03, four512-byte layer maps | All four live reads; key/media/chord/mouse/special/disable/macro constructors tested |
| Macros | cmd85/05,4096-byte pool | Live empty-pool read; table/UTF8/events/create/edit/delete fixtures; writes pending |
| Sleep/debounce | cfg24=seconds/30; cfg3=ms-1 | Live values300s/2ms; native debounce range0..4 corroborated; F87 sleep unknown |
| Profiles | Local JSON named presets/import/export | UI logic tests; not vendor binary profile format |
| Backups | Full config/RGB/maps/macros/custom-color snapshot/restore | Complete live snapshot saved; restoration writes pending |
| Music and host lighting | cmd08 interleaved RGB, paced streaming | Native framing and shutdown-restoration failure fixtures; live streaming pending |
| Image/animation import | GTK image/GIF sampling into Studio frames | GUI path implemented; output verification pending |
| Startup preference | User-toggle autostart desktop file | Local preference path; not enabled automatically |
| Battery view | Not implemented | Generic code exists but packaged models set ShowPower0 |
| Wireless / Bluetooth configuration | Not implemented | Different transport and unknown device-specific validation |
| Firmware updates | Not implemented | No verified firmware image/update workflow supplied |
| Language switching / tray | Not implemented | Modern interface currently English; no tray component |
| Exact vendor host-animation repertoire | Partial | Studio provides replacement patterns, not a claimed reproduction of every OEM algorithm |

## Evidence

- `features-native-evidence.json`: retained REA observations, including key-record
  converter FUN_0049a970 and matrix/macro builder FUN_0049b420, palette writer
  FUN_0049a240 and settings UI helpers.
- `indicator-before.json` / `indicator-after.json`: configuration reads around
  the human-operated Fn+LeftShift shortcut. The only changed payload byte is18,
 1→0; the user confirmed the indicator is off. This supersedes the prior unknown.
- `extended-read-capture.json`: exact feature replies. RGB table has5a,a5 at
  payload506/507; keymaps have it at510/511; custom colors return378 bytes.
- `v2-before-snapshot.json`: complete read-only keyboard snapshot, all regions.
- `v2-state-read.json`: current config/palette/indicator/sleep/debounce read.
- `studio-ui-preview.png`: rendered native interface after successful real reads.
- `remap-macro-spec.md`: native record encodings and CLI contract.

The requested combined reversible write test was rejected by automatic approval
review due to broad multi-feature writes without explicit test authorization.
The rejected command did not execute. `tools/verify_features.py` now documents
that exact test as a reviewable local artifact. No workaround or split version
of the rejected write test has been executed. Read-only hardware checks and
mocked/software checks continued independently.

External cross-checks were read from the authors' protocol notes in
[Aula-Manager](https://github.com/free-soldier28/Aula-Manager/blob/main/docs/PROTOCOL.md)
and [keylux](https://github.com/SibteProf/keylux/blob/main/docs/PROTOCOL.md).
Some details differ across those notes, so the implementation follows the
supplied native artifact and current keyboard captures. No external repository
code was copied into this project.
