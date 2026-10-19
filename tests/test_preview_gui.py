"""Graphical animation regression: fake discovery, no keyboard commands."""
from pathlib import Path
import sys
import time
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'app'))
import gui

assert gui.Gtk.init_check()[0], 'A graphical session is required'
w = gui.ControlWindow('/no-device-backend')
commands = []

def backend(arguments, message, callback, timeout=20):
    commands.append(arguments)
    assert arguments == ['--list'], 'Preview must never invoke a hardware command'
    callback([])
w.run_backend = backend
w.model_combo.set_active_id('F75')
w.effect_combo.set_active(2)
w.brightness.set_value(9)
w.speed.set_value(4)

def pixels():
    return bytes(w.keyboard_preview.image.get_pixbuf().get_pixels())

def after(milliseconds):
    deadline = time.monotonic()+milliseconds/1000
    while time.monotonic() < deadline:
        while gui.Gtk.events_pending():
            gui.Gtk.main_iteration_do(False)
        time.sleep(.005)

after(120)
before = pixels()
after(350)
assert pixels() != before, 'Visible breathing preview must animate without Apply'
w.toggle_preview()
paused, phase = pixels(), w.preview_elapsed
after(160)
assert pixels() == paused and w.preview_elapsed == phase
w.toggle_preview()
after(80)
assert w.preview_elapsed > phase
w.navigate(None, 'keys')
phase = w.preview_elapsed
after(120)
assert w.preview_elapsed == phase, 'Hidden lighting preview must stop advancing'
w.navigate(None, 'lighting')
w.effect_combo.set_active(12)
w.preview_key_press(9)
assert w.local_preview_frame()[9] != (0, 0, 0)
assert w.preview_presses[-1][0] == 9
w.keyboard_preview.preview_typed(None, type('Event', (), {'keyval': gui.Gdk.KEY_b})())
assert w.preview_presses[-1][0] == 34

# Breathing used to start black for seconds at low speed and restart on speed
# changes. Color changes must also be visible when paused on a dark frame.
w.effect_combo.set_active(2)
w.speed.set_value(0)
w.preview_paused = True
w.set_color('#FF0000', True)
assert all(c == (1, 0, 0) for c in w.local_preview_frame().values())
phase = w.preview_phase()
w.speed.set_value(4)
assert w.preview_phase() == phase, 'Speed changes must preserve the current frame'
w.preview_elapsed = 0
w.set_color('#00FF00', True)
assert all(c == (0, 1, 0) for c in w.local_preview_frame().values())

# Complete hex input commits without Enter or changing focus; partial input
# keeps the last valid color. Every colored mode produces a visible first frame.
w.effect_combo.set_active(1)
for color, channel in (('#FF0000', 0), ('#00FF00', 1), ('#0000FF', 2)):
    w.set_color(color, True)
    image = w.keyboard_preview.image.get_pixbuf()
    _label, _index, x, y, _width, _height = next(b for b in w.keyboard_preview.bounds if b[1] == 9)
    offset = int(y+5)*image.get_rowstride()+int(x+5)*image.get_n_channels()
    pixel = image.get_pixels()[offset:offset+3]
    assert pixel[channel] > 130 and all(pixel[c] < 20 for c in range(3) if c != channel), pixel
w.color_entry.set_text('#0000FF')
assert w.color == '#0000FF' and set(w.local_preview_frame().values()) == {(0, 0, 1)}
w.color_entry.set_text('#12')
assert w.color == '#0000FF'
w.color_entry.set_text('#FF0000')
for mode in range(1, 19):
    w.effect_combo.set_active(mode)
    assert max(max(c) for c in w.local_preview_frame().values()) > .2, mode

# Picker changes update the visible frame while the actual modal dialog is open.
# Cancel restores the original settings and pending-field state.
w.effect_combo.set_active(1)
w.rainbow.set_active(True)
original = w.snapshot()
w.color_entered()
assert w.snapshot() == original, 'Leaving an unchanged hex field must preserve multicolor'
picker_errors = []
def edit_picker():
    dialog = next(d for d in gui.Gtk.Window.list_toplevels() if isinstance(d, gui.Gtk.ColorChooserDialog))
    rgba = gui.Gdk.RGBA()
    rgba.parse('#00FF00')
    try:
        dialog.set_rgba(rgba)
        assert w.color == '#00FF00' and not w.rainbow.get_active()
        assert set(w.local_preview_frame().values()) == {(0, 1, 0)}
    except Exception as exc:
        picker_errors.append(exc)
    finally:
        dialog.response(gui.Gtk.ResponseType.CANCEL)
    return False
gui.GLib.idle_add(edit_picker)
w.choose_color()
assert not picker_errors, picker_errors
assert w.snapshot() == original, (w.snapshot(), original)

# Palette edits must actually activate the multicolor path, and cancel restores it.
w.rainbow.set_active(False)
original = w.snapshot()
def edit_palette():
    dialog = next(d for d in gui.Gtk.Window.list_toplevels() if isinstance(d, gui.Gtk.ColorChooserDialog))
    rgba = gui.Gdk.RGBA()
    rgba.parse('#ABCDEF')
    try:
        dialog.set_rgba(rgba)
        assert w.rainbow.get_active() and w.palette[0] == '#ABCDEF'
    except Exception as exc:
        picker_errors.append(exc)
    finally:
        dialog.response(gui.Gtk.ResponseType.CANCEL)
    return False
gui.GLib.idle_add(edit_palette)
w.choose_palette(0)
assert not picker_errors, picker_errors
assert w.snapshot() == original

# Intentional dark states remain faithful but have a clear visible explanation.
w.effect_combo.set_active(0)
assert 'effect is Off' in w.preview_hint.get_text()
assert not w.quick_colors.get_sensitive()
w.effect_combo.set_active(3)
assert not w.color_button.get_sensitive() and not w.quick_colors.get_sensitive()
w.effect_combo.set_active(1)
assert w.color_button.get_sensitive() and w.quick_colors.get_sensitive()
w.brightness.set_value(0)
assert all(c == (0, 0, 0) for c in w.local_preview_frame().values())
assert 'brightness is zero' in w.preview_hint.get_text()
assert commands == [['--list']], commands
w.on_destroy()
assert w.preview_timer is None
w.hide()
print('Animation, dark-frame recovery, live hex/pickers, cancellation, pause and reactive keys passed with zero hardware operations')
