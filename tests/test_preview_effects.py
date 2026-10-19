"""Headless checks of visible effect behavior; no USB, windows or subprocesses."""
from copy import deepcopy
from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'app'))
from gui import (ANIMATED_EFFECTS, DEFAULT_PALETTE, KEY_LAYOUTS, REACTIVE_EFFECTS,
                 lighting_effect_frame)

positions = [(y*10+x, x/9, y/5) for y in range(6) for x in range(10)]
presses = [(23, 3/9, 2/5, .2), (47, 7/9, 4/5, 1.2)]
original = deepcopy((positions, DEFAULT_PALETTE, presses))
frames = {}
for mode in range(19):
    sequence = [lighting_effect_frame(mode, t, positions, '#8055FF', DEFAULT_PALETTE,
                                      True, .8, presses) for t in (.3, .9, 1.6, 2.4)]
    for frame in sequence:
        assert set(frame) == {p[0] for p in positions}
        assert all(len(c) == 3 and all(0 <= v <= 1 for v in c) for c in frame.values())
    if mode in ANIMATED_EFFECTS:
        assert any(frame != sequence[0] for frame in sequence[1:]), mode
    else:
        assert all(frame == sequence[0] for frame in sequence), mode
    frames[mode] = tuple(tuple(round(v, 5) for c in f.values() for v in c) for f in sequence)
assert len(set(frames.values())) == 19, 'Effects must have distinct visible patterns'
assert (positions, DEFAULT_PALETTE, presses) == original

for mode in range(19):
    black = lighting_effect_frame(mode, .9, positions, '#FF0000', DEFAULT_PALETTE,
                                  True, 0, presses)
    assert all(c == (0, 0, 0) for c in black.values()), mode

fixed = lighting_effect_frame(1, 1, positions, '#FF0000', DEFAULT_PALETTE, False)
assert set(fixed.values()) == {(1, 0, 0)}
dim = lighting_effect_frame(1, 1, positions, '#FF0000', DEFAULT_PALETTE, False, .25)
assert set(dim.values()) == {(.25, 0, 0)}

for mode in REACTIVE_EFFECTS:
    dark = lighting_effect_frame(mode, .2, positions, '#FFFFFF', DEFAULT_PALETTE, False)
    hit = lighting_effect_frame(mode, .2, positions, '#FFFFFF', DEFAULT_PALETTE, False,
                                presses=[(23, 3/9, 2/5, .2)])
    assert all(c == (0, 0, 0) for c in dark.values())
    assert hit[23] == (1, 1, 1), mode
    expired = lighting_effect_frame(mode, 10, positions, '#FFFFFF', DEFAULT_PALETTE,
                                    False, presses=presses)
    assert all(c == (0, 0, 0) for c in expired.values())

# Every shipped model's geometry produces a complete frame, including F99's numpad.
for model, keys in KEY_LAYOUTS.items():
    actual = [(k[5], k[1]/700, k[2]/300) for k in keys if k[3] > k[1] and k[4] > k[2]]
    result = lighting_effect_frame(15, .7, actual, '#FFFFFF', DEFAULT_PALETTE, False)
    assert set(result) == {k[0] for k in actual}, model
print('All 19 preview effects, color/brightness, reactive input and model layouts passed')
