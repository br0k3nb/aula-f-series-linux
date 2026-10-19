# Extracted resource evidence

All paths below are relative to `analysis/extracted/app`. Line numbers refer to decoded text lines. `Cfg.ini`, F87/F99 `KB.ini`, and language XML carry UTF-16 BOMs; F75 `KB.ini` is ANSI/ASCII-compatible. Resources describe configuration and presentation, and alone do **not** establish HID packet offsets, report IDs, endpoint selection, CRC algorithms, or safe write sequences.

## Models and identifiers

Only three device directories are present, each with a `KB.ini`:

| Model | Wired VID:PID | Wireless VID:PID | `Psd` literal | Evidence |
|---|---|---|---|---|
| AULA F75 | 258a:010c | 3554:fa09 | `3,0,0,0,3,C2` | `Dev/kb/F75/KB.ini:2–11` |
| AULA F87 | 258a:010c | 3554:fa09 | `3,0,0,0,3,C1` | `Dev/kb/F87/KB.ini:2–10` |
| AULA F99 | 258a:010c | 3554:fa09 | `3,0,0,0,3,C3` | `Dev/kb/F99/KB.ini:2–11` |

All three set `Fw=24`, `CRC=1`, `ChannelMask=3`, and `ShowPower=0`. Shared USB IDs cannot independently distinguish these models. The significance of `Psd` bytes must be confirmed from executable code before using them for communication.

F75 and F99 explicitly set `MatrixLen=128` at line 9. F87 does not declare it. Key entries include visual rectangle coordinates, three function values, and a final integer; named key comments associate entries with physical keys. There are 82 F75 entries (`[KEY]` line 69 through 233), 88 F87 entries (line 65 through 241), and 100 F99 entries (line 68 through 268). These include alternate-layout entries; do not mistake those counts for physical product key counts. Highest final integers are 89, 101, and 112 respectively. FN defaults occur in `[FN1]` at F75 line 235, F87 line 243, F99 line 270, with 38, 39, and 40 entries respectively.

## Lighting configuration

The `LedOptN` tables are identical across models: F75 lines 46–65, F87 lines 43–62, F99 lines 45–64. Their preceding comment labels seven columns `hw effect speed light direct random color`. The following is the literal configuration plus English label keyed by `tc_kb_ledN` in `Text/en/text.xml:199–218`. Association with the hardware protocol still requires native-code confirmation.

| N | English label | Literal LedOptN |
|---|---|---|
| 1 | Fixed_on | `1,1,0,1,0,1,1` |
| 2 | Respire | `2,3,1,1,0,1,1` |
| 3 | Rainbow | `3,2,1,1,0,0,0` |
| 4 | Flash_away | `4,19,1,1,0,1,1` |
| 5 | Raindrops | `5,15,1,1,0,1,1` |
| 6 | Rainbow_wheel | `6,13,1,1,0,1,1` |
| 7 | Ripples_shining | `7,20,1,1,0,1,1` |
| 8 | Stars_twinkle | `8,16,1,1,0,1,1` |
| 9 | Shadow_disappear | `9,18,1,1,0,1,1` |
| 10 | Retro_snake | `10,5,1,1,0,1,1` |
| 11 | Neon_stream | `11,7,1,1,0,1,1` |
| 12 | Reaction | `12,17,1,1,0,1,1` |
| 13 | Sine_wave | `13,12,1,1,0,1,1` |
| 14 | Retinue scanning | `14,8,1,1,0,1,1` |
| 15 | Rotating windmill | `15,28,1,1,0,0,0` |
| 16 | Colorful waterfall | `16,30,1,1,0,0,0` |
| 17 | Blossoming | `17,14,1,1,0,0,0` |
| 18 | Rotating storm | `18,29,1,1,0,1,1` |
| 19 | Self-define | `21,0,0,0,0,0,0` |
| 20 | OFF | `0,0,0,0,0,0,0` |

All declare `DefLedIndex=10`, `LedMask=0x22120`, `GaoshouIndex=18`, `RGBIndex=0,1,2`, `GameRGBIndex=0,1,2`, one Gaoshou group, and the same Gaoshou key list. F75 brightness lists (`Light` and `LightHW`) have values 0–9 (`KB.ini:28–29`); F87 has 0–4 (lines 28–29), F99 0–4 (27–28). All speed lists have values 0–4. The English resources call these controls Brightness, Speed, Color, Colourful (`Text/en/text.xml:96–99`).

## Other settings and app capabilities evidenced by resources

- F75 `KB.ini:39–40` and F99 `KB.ini:38–39` pair `SleepUI=60,90,120,180,240,300,600,900,1200` with `SleepHW=2,3,4,6,8,10,20,30,40`. Their `SleepTime=300` is line 17; F87 has only commented `;SleepTime=120` at line 17. English text line 108 describes idle sleep in wireless mode.
- `ShowDebounce=0x04000101` occurs at F75 line 23 and F87/F99 line 22; the English Debounce label is line 128. No reliable numerical debounce packet semantics can be obtained from these strings.
- F87 explicitly sets `CmdReset=0` at line 24. F75 has `FnNotToSet=0` at line 67; F99 has `KeyNotToSet=1,2,3,4,5,6,7,8,9,10,11,12` at line 67. Interpretations require the parser/code.
- English resources expose key assignment, profiles, macro limit messaging, import/export, factory reset, upgrade, music-reactive lighting, FN1/FN2 labels and automatic detection (`Text/en/text.xml:92–128`, `184–197`, `228–248`). UI strings alone do not prove every feature is enabled for every model.
- `Cfg.ini:4–6` sets title `AULA F Series Driver`, version `0x200`, app directory `BYCOMBO4`; lines 29–37 include `ApplyNow=0`, `Rotating=1`, `GoWithNoDev=0`, `UpdateByLocal=0`, `EnableYun=0`, `MacLayout=1`, `ShowDevList=1`. Languages are English and Português(Brasil), lines 41–42.

## Animation resources

Every model has `ET/audiobar.txt`, `ET/happy.txt`, `ET/flower.txt`, and `ET/stream_h.txt`, whose comma/colon-separated key names form spatial animation groups. F75/F99 Cartoon directories include `fixon_unit.txt` and `breath_unit.txt`; F99 additionally has `flower_unit.txt` and `stream_unit.txt`; F87 also has `flower_unit.txt`, `windmill_unit.txt`, `paoma_unit.txt`, and `wheel_unit.txt`.

The GB18030 comments at `Dev/kb/F75/Cartoon/fixon_unit.txt:1–2` and `breath_unit.txt:1–2` say each line is a key unit, the file must be ANSI, key names are case-insensitive, and unspecified unit colors receive automatically calculated transitional colors. For example `fixon_unit.txt:3` is `o:255,0,0`. These are host-side visual grouping/color resources, not evidence of device report layouts.
