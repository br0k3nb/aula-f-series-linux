# REA investigation: AULA F Series Driver v2.0

## Live correction and validation — version 0.1.1

The user reported that Apply stayed disabled even with a USB cable. Live host
inspection identified wired258a:010c at hidraw4/5, with config report6 on hidraw5.
Both nodes were0600 root:root; the installed older rule covered receivers only.
Installed `/etc/udev/rules.d/70-aula-f-series.rules` through authenticated pkexec,
reloaded udev rules, and triggered only the connected wired AULA interfaces.
The ordinary desktop user now has read/write access. A built-in setup action
performs this operation on other installations, and the GUI explains disabled
states, prefers the wired control interface and expands actual errors.

The first live read uncovered a second bug: HIDIOCGFEATURE returns136 actual
bytes, not the declared520-byte report size. The exact header matches the
vendor request and128 configuration bytes end in5a,a5. The reader now accepts
136–520 bytes while retaining header and marker validation. Raw response:
`wired-config-read.json`; regression fixture: `../tests/fixtures/wired-config-read.hex`.

Live read showed mode6, brightness0, speed4. A CLI brightness change0→1 returned
success and the keyboard read back the expected configuration exactly. The test
then restored all128 original bytes and verified equality. Evidence:
`wired-original-config.json` and `wired-live-verification.json`. This establishes
configuration transport/read-back on the connected firmware; perceived lighting
appearance and other firmware/models are not established by that check.

GUI integration checks confirmed permission/model transitions, automatic wired
control-interface selection, a real config read and an enabled Apply button.
`gui-wired-fixed.png` records that state. Current effect brightness remains0
because the live test restored the user's original configuration. Choose a
nonzero brightness in the app to illuminate it.

The original static investigation follows; statements about absent live testing
and the original full-size response check below describe the superseded0.1 build.

## Scope and result

Analyzed the supplied installer and its extracted OemDrv.exe without executing
either Windows artifact. Produced a new native Linux x86-64 ELF, source, tests
and GTK interface for a **partial implementation: wired lighting settings**.
Full vendor-driver parity and live hardware validation remain unresolved.

## Artifact identities

| Artifact | SHA-256 |
|---|---|
| Supplied installer | c2b3161f7da0a04c6f9d2efc6c7b124b29aa54618dbc84b2471c7bf7545d0952 |
| Extracted app/OemDrv.exe | 4091d39ef765b38c26fe77e3afb6f4378f52900edd3a144adb50c661238852ec |

REA selected Ghidra 12.1.4 for x86 PE analysis. Installer Evidence
`ev_d3424c985ebfcb6c3d9cf22898bbeb495745d2ee67d67afb3db826c6788cab77`
reports 781 identified procedures and 385 defined strings. These counts cover
Ghidra's inventory, not proof of all possible code/text. Evidence
`ev_ee98e5cc7cc587357e8d741e2234ce916801ff16e6e984af9f0bc9f4de950cf0`
identifies Inno Setup data 5.3.3 (Unicode). Extracted with portable innoextract1.9
from [the upstream distribution](https://constexpr.org/innoextract/), preserving
the original installer. All findings below are static observations/inferences,
unless explicitly described as host discovery or tests.

## Protocol findings and implementation boundary

| Finding | Evidence |
|---|---|
| Windows imports HidD_SetFeature/GetFeature | `ev_b86a515746ce7db8bd2248edd33c0d6d0e2a7549ce9290cca134d3cbff2346b8` |
| G5 keyboard sends 520-byte buffers, report 6 when Fw=24, payload chunks≤512 | FUN_004994c0, `ev_77847deb0e685c3024efd6d1f84d24b7b18407474e16b8c6d55a14effde33974` |
| Read LED config command0x84; payload begins byte8 | FUN_00499bf0, `ev_211a4d5df4cb9d9a8ed42016d9bfc9bf8f2ae0dbe9bb4b717f1a82847738d996` |
| Write LED command4, sub0 | FUN_00499ba0, `ev_35522f640b3ad9202da8180297ea382cfbebe23f529ff3d6d0b9e9f636d5fbf9` |
| Main apply reads128 config bytes; checks trailing5a,a5 before patch/write | FUN_0049c080, `ev_60b8a5582f8e061a6b13cf52c434b5f18ed4bf11a15fd4e2f376646070d372f5` |
| Regular lighting layout: normal/custom selector byte9; mode byte10; effect pairs at0x3a, speed high nibble, brightness first byte;0x38/39 setff | FUN_0049bc50, `ev_f7e0150938cc216ab2305ee008f274cd5c0ec2a65c667d3d2dc776e58218ebb8` |
| SubFw defaults0 and CRC defaults1; speed/brightness mapping tables | Config parser, `ev_c07526289a07c502609b9aa530f742f628600b47d3dd2ae070a3248b0a966e38` |

For one 128-byte chunk the implemented read header is
`06 84 00 00 01 00 80 00`; write header is `06 04 00 00 01 00 80 00`.
Data starts at byte8; remaining bytes through519 are zero. This path has no
additional packet checksum in the observed helper; the config marker is not a
computed checksum. Other device-class helpers use different framing/checksums.

All shipped KB.ini files use Fw24, no SubFw override, CRC1 and matching USB IDs;
see resources.md for paths/lines. Brightness/speed mappings are identity tables
for these models. Effects1–18 map to hardware modes1–18; off is0. This is an
inference connecting the shipped table parser, regular fill helper and resources.
Color writes require additional table operations and are intentionally absent.

The Linux client additionally requires an exact echoed reply header and exact
read-back equality. Those are conservative implementation choices, **not observed
firmware behavior**. The vendor reader copies the response payload without this
header check. Valid firmware that changes response headers will be rejected and
needs a captured response to refine compatibility. There is no model handshake
in this first implementation; explicit model choice and descriptor gating narrow
the target, but do not establish that every device sharing the VID/PID uses this
firmware layout.

## Wireless findings

Host sysfs on this machine exposes 3554:fa09 with two hidraw collections. The
second descriptor includes report19 input/output (20 total bytes), plus feature6
(8 total bytes) on vendor pageff04. The unrelated25a7:fa07 receiver is not in this
installer's model configuration and is excluded from the new utility.

The earlier aula-f75-control README claimed no vendor collection or usable
lighting protocol. The descriptor and code provide contrary evidence; lack of
spontaneous feature reports does not establish lack of a host-initiated command.
That earlier project was left unchanged.

REA observations of the separate CDev3632 path:

- FUN_00493da0 (`ev_a4057f0338024a07866223041d9c37910dd3659f7971df68d1efaa76de4dd40c`)
  constructs report19, command, chunk count/index, nibble-packed length/sub,
  14-byte payload and additive checksum in the last byte.
- FUN_00494c50 (`ev_7bb28e46ef0caba8c36daa4074a9c86396e1089072c3baa5c70c775242d94f81`)
  sends20-byte output reports with WriteFile, not the 520-byte feature helper.
- FUN_00494640 (`ev_8eaddd64b034a1073fd84e498c52c530fcbb00084e49e47ad136157983ef1f2c`)
  reads LED settings with command44h. FUN_004945f0
  (`ev_5e9891cb9a6bc2d8648171a8d35e3ebf051acd614c5cf5d771e1690768df23d6`)
  writes with command4.
- FUN_00494130 (`ev_a30ca03a491e1a69065bb795059be55446e1d9e68531e926f5f3150c1ceb294a`)
  sends a read request and assembles multiple input reports; online-state,
  discovery/model handshake and asynchronous response processing still require
  further reconstruction and physical validation before enabling wireless writes.

Battery-reading routines exist in the generic classes, while all three shipped
models set ShowPower0. Their presence does not establish working battery support
on these keyboards; none is claimed by the Linux executable.

## Verification and remaining work

- Built static x86-64 ELF with C11 and warnings-as-errors.
- Descriptor fixture tests cover report sizing, multiple pages, malformed input.
- Protocol fixtures test known headers, markers, field bounds, byte preservation,
  response failures, verification mismatch and wireless rejection before I/O.
- CLI tests verify JSON output, no-device preview and invalid-input rejection.
- GTK3 interface rendered on the actual desktop, completed discovery, and closed
  automatically; `gui-preview.png` records the window. This was a UI test, not
  a hardware protocol test.
- Sandbox exposes sysfs but hides device nodes; desktop test outside the sandbox
  sees readable/writable receiver nodes. No AULA keyboard config writes were made.
- Remaining: physical wired read/response capture; verify report size and header
  compatibility; validate lighting apply/read-back; reconstruct wireless handshake;
  then color/per-key RGB and remapping/macros if full parity is required.

Full evidence is retained in installer-evidence.json and application-evidence.json;
application-snapshot.json preserves the application session. Open REA targets were
closed after export. Extracted binaries are analysis inputs, not Linux runtime
dependencies. Recovered pseudocode is not original vendor source.
