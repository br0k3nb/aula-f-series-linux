"""Run with /usr/bin/python3 under a graphical session; does not change hardware."""
from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'app'))
import gui

assert gui.Gtk.init_check()[0], 'A graphical session is required'
w = gui.ControlWindow(str(Path(__file__).resolve().parents[1] / 'build/aula-control'))
# Model alone cannot unlock a root-only wired control interface.
w.devices = [{'node': '/dev/hidraw5', 'name': 'Test wired keyboard',
              'node_exists': True, 'config_supported': True, 'readable': False,
              'writable': False, 'transport': 'USB cable'}]
w.device_combo.append_text('Wired control')
w.device_combo.set_active(0)
w.model_combo.set_active_id('F75')
w.device_changed()
assert not w.apply_button.get_sensitive()
assert w.access_button.get_visible() and w.access_button.get_sensitive()
assert 'Access blocked' in w.device_status.get_text()
# Successful setup unlocks read and apply without requiring a restart.
w.devices[0].update(readable=True, writable=True)
w.device_changed()
assert w.read_button.get_sensitive() and w.apply_button.get_sensitive()
assert not w.access_button.get_visible()
# Read works without choosing a model; Apply explains that remaining requirement.
w.model_combo.set_active_id('')
assert w.read_button.get_sensitive() and not w.apply_button.get_sensitive()
assert 'Choose your model' in w.device_status.get_text()
w.hide()
print('GUI permission and model transitions passed')
