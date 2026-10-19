# AULA Studio for Linux — 0.2.1

A native Linux x86-64 keyboard utility reconstructed from the supplied **AULA F
Series Driver Setup v2.0 20251127.exe** using REA/Ghidra, packaged resources and
read-only/reversible checks on the connected keyboard.

## Run

Open `build/aula-control`, or run `./build/aula-control`. Select your model and
USB control interface, then click **Read keyboard**. Changes reach the keyboard
only through an explicit Apply, Save assignment, macro action, Restore, or Studio
Start action. Configurations can persist in keyboard memory.

The executable embeds its interface and uses no Windows binaries at runtime.
Its C backend is statically linked. The desktop interface requires system Python3,
PyGObject, GTK3 and pycairo (Debian/Ubuntu: `python3-gi`, `gir1.2-gtk-3.0`,
`python3-cairo`). All are already available on the development machine.

## Features in this build

- Modern native interface with a clickable keyboard preview and separate Lighting,
  Keys, Macros, Studio, Device and Profiles pages.
- Eighteen built-in effects plus off, brightness and speed; RGB/hex color picker,
  seven-color effect palettes and single-color/multicolor selection. Effects with
  firmware-defined colors disable the corresponding controls.
- Live local previews for every built-in effect, with color, brightness and speed
  changes visible before Apply. Reactive effects include demo keypresses; click
  preview keys or focus the preview and type to try them. Pause/resume is available,
  and animation suspends when the Lighting page is hidden. These are visual
  simulations of the named effects; exact firmware timing is unverified. Preview
  playback sends no keyboard commands.
- Live color-picker and valid hex-input previews, with Cancel restoring the
  preceding settings. Breathing starts visibly lit; speed adjustments preserve
  the animation position. Dark previews explain Off, zero brightness or black
  color settings, and key surfaces preserve the selected RGB hue.
- Custom per-key RGB painting, fill/clear and persistent custom-color storage.
- Charging-indicator on/off, sleep timeout and debounce1–5ms. The indicator offset
  was confirmed by comparing reads before/after the user's Fn+LeftShift shortcut.
- Four key-assignment layers: normal keys, two-key combinations, modifiers, media,
  mouse actions, firmware functions, disabled keys and macros. Only documented
  action families are exposed. Physical firmware restrictions can still apply.
- Macro recording inside this window and manual keyboard/modifier/mouse/movement/
  wheel events with delays, repeats and once/hold/toggle triggers. Create, edit,
  rename, assign, list, import/export and delete. Existing indices and unrelated
  data are preserved. Deletion requires unassigning the selected and any later
  macros first, to avoid invalidating stored references.
- Live procedural lighting, output-audio-reactive lighting, and image/animated-GIF
  playback. Music mode requires `pw-record` from PipeWire and captures a playback
  sink monitor by default. Streaming stops before other device operations and
  attempts to restore the preceding lighting state.
- Named local JSON settings profiles, import/export, full keyboard backups and
  verified restoration, plus optional start-on-login preference.

## Verification status and remaining OEM gaps

The earlier brightness write/read-back passed on the connected wired keyboard.
Version0.2 can read valid configuration, RGB palettes, four key layers, macro memory
and per-key colors. Native fixture/mocked tests and graphical state tests pass.
The expanded version0.2 write paths have **not yet passed the combined physical
hardware test**: automatic approval review rejected that broad test pending
explicit user authorization. See `tools/verify_features.py` for the exact plan;
without `--run` it only prints the plan. The test backs up all regions and attempts
exact restoration afterward; restoration failure remains possible.

This is **not yet complete OEM feature parity**. Remaining gaps include wireless
configuration, firmware updating, battery display, original binary-profile format
compatibility, UI language selection, and the vendor's exact host-animation set.
F87 sleep mapping is unconfirmed, so this build rejects changes to it. Bluetooth
configuration is not implemented. No speculative firmware-write commands are used.
See `analysis/features-status.md` for evidence and the explicit coverage checklist.

## Device access

The wired device is `258a:010c`; the receiver is `3554:fa09`. A data cable and wired
mode are required for configuration. If access is blocked, use **Enable wired
keyboard access…** in Device, or `./build/aula-control --setup-permissions`.
Authenticate once in the system dialog. The helper installs the narrow wired
udev rule and refreshes matching interfaces. Run normal operations as your user.
The rule is already installed on the development machine.

## CLI examples

```sh
./build/aula-control --list
./build/aula-control --read /dev/hidrawN
./build/aula-control --apply /dev/hidrawN --model F75 --mode 1 --brightness 5 --color 7855ff --rainbow 0
./build/aula-control --apply /dev/hidrawN --model F75 --charging-light 0 --debounce 2 --sleep 300
./build/aula-control --keymap /dev/hidrawN --layer 0
./build/aula-control --remap /dev/hidrawN --layer 1 --key 88 --action combo --code 4 --modifiers 1
./build/aula-control --macro-list /dev/hidrawN
./build/aula-control --snapshot /dev/hidrawN > keyboard-backup.json
```

Replace `hidrawN` with the wired interface whose `--list` result says
`config_supported: true`. Brightness is0–9 on F75 and0–4 on F87/F99; speed0–4.
Omitted fields stay unchanged. Read operations send vendor read requests. Writes
verify read-back; failures may occur after settings were accepted. Rollback is
attempted for multi-region failures and any restoration failure is reported.

`--macro-add NODE --name NAME --events FILE --layer N --key INDEX` creates and
assigns a macro. `--macro-update NODE --index N --name NAME --events FILE` edits
one without changing its assignments. Event files use `kind,code,pressed,delay_ms`
CSV. See `analysis/remap-macro-spec.md` for event kinds and the protocol.

## Build and tests

```sh
make
make test
make package
# Under a graphical session, without changing hardware:
/usr/bin/python3 tests/test_gui_extended.py
/usr/bin/python3 tests/test_preview_gui.py
```

Building requires a C11 compiler, Linux headers, static libc and Python3.
Use `make LDFLAGS=` if static libc is unavailable. The desktop source is embedded
in the ELF during compilation. `dist/aula-control-linux-x86_64.tar.gz` contains
the executable, README and optional rule; vendor binaries and skins are excluded.

Source and evidence remain alongside the executable. Start with
`analysis/features-status.md`, `analysis/report.md`, `analysis/resources.md` and
`analysis/features-native-evidence.json` for observations and remaining unknowns.
