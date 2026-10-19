"""Reviewable hardware test. Default: print plan only. --run requires explicit consent.

This temporarily changes lighting, device settings and one assignment on layer3,
adds a test macro, and exercises streaming. It saves a complete backup first and
attempts to restore every region in finally. Restore failure remains possible.
"""
import argparse
import json
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
PLAN = [
    'Save config, RGB palettes, four key layers, macro memory and custom colors.',
    'Switch charging indicator on; set solid violet at brightness4.',
    'Set debounce5ms and sleep600s; verify configuration read-back.',
    'Assign Ctrl+A to matrix88 on layer3; verify that key layer.',
    'Create a two-event A-key test macro and assign it to the same layer3 key.',
    'Write a solid violet per-key image; verify its stored color table.',
    'Stream two RGB frames, then verify the stream restores its starting state.',
    'Restore the complete original backup and compare all regions byte-for-byte.',
]

def restore_arguments(node, state):
    args = ['--restore', node, '--config', state['config_hex'], '--rgb', state['rgb_hex'],
            '--macros', state['macros_hex'], '--perkey', state['perkey_hex']]
    for layer, value in enumerate(state['keymaps_hex']):
        args += ['--map' + str(layer), value]
    return args

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('node', nargs='?', default='/dev/hidraw5')
    parser.add_argument('--run', action='store_true', help='Perform live writes only after explicit user authorization')
    args = parser.parse_args()
    if not args.run:
        print(json.dumps({'live_writes': False, 'plan': PLAN}, indent=2))
        return
    binary = str(ROOT / 'build/aula-control')
    def run(*command, data=None):
        result = subprocess.run([binary, *command], input=data, capture_output=True, timeout=25)
        if result.returncode:
            raise RuntimeError(result.stderr.decode(errors='replace').strip())
        return json.loads(result.stdout)
    node = args.node
    original = run('--snapshot', node)
    (ROOT / 'analysis/v2-test-original.json').write_text(json.dumps(original, indent=2) + '\n')
    outcomes = {}
    try:
        outcomes['indicator'] = run('--apply', node, '--model', 'F75', '--charging-light', '1')['charging_light']
        state = run('--apply', node, '--model', 'F75', '--mode', '1', '--brightness', '4', '--color', '7855ff', '--rainbow', '0')
        outcomes['color'] = state['color'] == '#7855ff'
        state = run('--apply', node, '--model', 'F75', '--debounce', '5', '--sleep', '600')
        outcomes['settings'] = state['debounce_ms'] == 5 and state['sleep_seconds'] == 600
        outcomes['remap'] = run('--remap', node, '--layer', '3', '--key', '88', '--action', 'combo', '--code', '4', '--modifiers', '1')['verified']
        with tempfile.NamedTemporaryFile('w') as events:
            events.write('0,4,1,0\n0,4,0,50\n')
            events.flush()
            outcomes['macro'] = run('--macro-add', node, '--name', 'AULA smoke test', '--events', events.name, '--layer', '3', '--key', '88')['verified']
        outcomes['perkey'] = run('--paint', node, '--model', 'F75', '--colors', '7855ff' * 126)['verified']
        outcomes['stream'] = run('--stream-stdin', node, data=bytes.fromhex('7855ff' * 126) + bytes(378))['restored']
    finally:
        try:
            outcomes['restore'] = run(*restore_arguments(node, original))['verified']
            outcomes['exact_original_restored'] = run('--snapshot', node) == original
        finally:
            (ROOT / 'analysis/v2-live-verification.json').write_text(json.dumps(outcomes, indent=2) + '\n')
            print(json.dumps(outcomes, indent=2))
    if not all(outcomes.values()):
        raise SystemExit('At least one hardware check failed. Inspect the saved results and backup.')

if __name__ == '__main__':
    main()
