"""Exercise complete UI command routing with a fake backend; never writes hardware."""
from pathlib import Path
import sys
import tempfile
import threading
import time
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'app'))
import gui
assert gui.Gtk.init_check()[0], 'A graphical session is required'
w = gui.ControlWindow('/fake/aula-control')
commands = []
config = {'mode':1,'brightness':4,'speed':2,'color':'#334455','rainbow':False,
          'palette':gui.DEFAULT_PALETTE,'charging_light':True,'sleep_seconds':300,'debounce_ms':3}
def backend(arguments, message, callback, timeout=20):
    commands.append(arguments)
    operation = arguments[0]
    if operation=='--list':
        result=[{'node':'/dev/fake-aula','name':'AULA test','node_exists':True,'readable':True,'writable':True,'config_supported':True}]
    elif operation=='--read': result=dict(config)
    elif operation=='--perkey': result={'colors':['#123456']*126,'custom':True}
    elif operation=='--macro-list':
        result={'capacity':4096,'used':20,'macros':[{'index':0,'name':'Example','events':[{'kind':0,'code':4,'pressed':True,'delay_ms':10}]}]}
    elif operation=='--keymap': result={'layer':0,'keys':[{'index':9,'type':'combo','code':6,'code2':0,'modifiers':1}]}
    elif operation in ('--macro-add','--macro-update'):
        csv_path = arguments[arguments.index('--events')+1]
        assert Path(csv_path).read_text().strip() == '0,4,1,10'
        result={'index':0,'verified':True}
    elif operation=='--snapshot':
        result={'config_hex':'00'*128,'rgb_hex':'00'*512,'keymaps_hex':['00'*512]*4,'macros_hex':'00'*4096,'perkey_hex':'00'*378}
    else: result={'verified':True}
    callback(result)
w.run_backend = backend
w.refresh()
w.model_combo.set_active_id('F75')
w.read_device()
assert w.charging_light.get_sensitive() or 'charging_light' in w.known_fields
assert w.saved_snapshot == w.snapshot()
# Device-only edits preserve every lighting field, including custom mode21.
w.charging_light.set_active(False)
args=w.settings_arguments()
assert args == ['--model','F75','--charging-light','0'],args
w.apply()
assert commands[-1] == ['--apply','/dev/fake-aula']+args
config['mode']=21
w.read_device()
assert w.lighting_source.get_active_id()=='perkey'
w.charging_light.set_active(False)
w.apply()
assert commands[-1] == ['--apply','/dev/fake-aula','--model','F75','--charging-light','0']
# Per-key paint preserves all126 positions and uses a756-character RGB buffer.
w.read_perkey()
w.set_color('#00AAFF',True)
w.paint_key('A',9)
assert w.perkey_colors[9]=='#00AAFF'
w.apply()
assert commands[-1][0]=='--paint'
colors=commands[-1][commands[-1].index('--colors')+1]
assert len(colors)==756 and colors[9*6:10*6]=='00AAFF'
assert colors[:6]=='123456'
# Off and fixed rainbow modes must never receive single-color/palette flags.
w.lighting_source.set_active_id('effects')
for mode in (0,3,15,16,17):
    w.effect_combo.set_active(mode)
    w.touched_fields.update(('color','rainbow','palette'))
    args=w.settings_arguments()
    assert not any(flag in args for flag in ('--color','--rainbow','--palette')),(mode,args)
# Full key remapping uses matrix positions and preserves optional shortcut fields.
w.read_keymap()
w.select_key('A',9)
assert w.key_action.get_active_id()=='combo' and w.key_target.get_active_id()=='6'
w.apply_key()
assert commands[-1][:6]==['--remap','/dev/fake-aula','--layer','0','--key','9']
assert '--code2' in commands[-1] and '--modifiers' in commands[-1]
for action in ('key','combo','media','mouse','special','disable'):
    w.key_action.set_active_id(action)
    w.apply_key()
    assert commands[-1][commands[-1].index('--action')+1]==action
# Stored macros can be opened, edited in place, exported, or copied to another key.
w.read_macros()
w.open_macro()
assert w.edit_macro_index==0 and len(w.macro_events)==1
w.update_macro()
assert commands[-1][0]=='--macro-update'
w.save_macro()
assert commands[-1][0]=='--macro-add'
w.key_action.set_active_id('macro')
w.apply_key()
assert commands[-1][commands[-1].index('--action')+1]=='macro'
assert gui.ControlWindow.valid_macro_value(1,224)
assert not gui.ControlWindow.valid_macro_value(1,1)
assert not gui.ControlWindow.valid_macro_value(3,0)
# Profile and full backup persistence use temporary locations only.
with tempfile.TemporaryDirectory() as temp:
    w.profiles_path=Path(temp)/'profiles.json'
    w.profile_name.set_text('Test setup')
    w.save_profile()
    assert w.profiles_path.exists()
    w.load_profile()
    w.profile_file=lambda _save: str(Path(temp)/'full-backup.json')
    w.save_backup()
    assert (Path(temp)/'full-backup.json').exists()
    # Opt-in autostart writes and removes only the selected desktop file.
    w.autostart_path=Path(temp)/'autostart/aula-studio.desktop'
    w.autostart.set_active(True)
    assert w.autostart_path.exists() and '--gui' in w.autostart_path.read_text()
    w.autostart.set_active(False)
    assert not w.autostart_path.exists()
# Frame generation is pure until Start explicitly launches the native stream.
w.streaming=True
w.stream_stop=threading.Event()
w.stream_start_time=time.monotonic()
w.stream_positions=w.studio_key_positions()
w.stream_music_level=.5
w.stream_image_iter=None
for pattern in ('wave','breath','rain','ripple','music'):
    w.stream_pattern=pattern
    assert w.update_stream_frame()
    assert len(w.stream_frames)==378
    assert len(w.perkey_colors)==126
w.streaming=False
w.stream_stop=None
# All pages render using ordinary pycairo; no python3-gi-cairo dependency is needed.
for name in ('lighting','keys','macros','studio','device','profiles'):
    w.navigate(None,name)
    while gui.Gtk.events_pending(): gui.Gtk.main_iteration_do(False)
w.hide()
print('Extended GUI routing, persistence, custom-mode preservation, macros, and frame generation passed')
