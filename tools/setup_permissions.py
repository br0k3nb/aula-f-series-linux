"""Admin-only installation of the narrow wired AULA access rule."""
import json
import os
from pathlib import Path
import subprocess
import tempfile

RULE = ('# AULA F series wired configuration access for the active desktop user.\n'
        'SUBSYSTEM=="hidraw", ATTRS{idVendor}=="258a", ATTRS{idProduct}=="010c", TAG+="uaccess"\n')

def main():
    if os.geteuid() != 0:
        raise SystemExit("Administrator authentication is required to install the wired keyboard access rule.")
    directory = Path('/etc/udev/rules.d')
    directory.mkdir(mode=0o755, parents=True, exist_ok=True)
    target = directory / '70-aula-f-series.rules'
    if target.exists() and target.read_text() != RULE:
        raise SystemExit(f"An existing rule differs: {target}. Review it before replacing it.")
    fd, temporary = tempfile.mkstemp(prefix='.aula-', dir=directory)
    try:
        with os.fdopen(fd, 'w') as handle:
            handle.write(RULE)
            os.fchmod(handle.fileno(), 0o644)
        os.replace(temporary, target)
    finally:
        if os.path.exists(temporary):
            os.unlink(temporary)
    subprocess.run(['/usr/bin/udevadm', 'control', '--reload-rules'], check=True)
    nodes = []
    for node in Path('/sys/class/hidraw').glob('hidraw*'):
        fields = dict(line.split('=', 1) for line in
                      (node / 'device/uevent').read_text().splitlines() if '=' in line)
        if fields.get('HID_ID') == '0003:0000258A:0000010C':
            subprocess.run(['/usr/bin/udevadm', 'trigger', '--action=add', '--settle', str(node)], check=True)
            nodes.append('/dev/' + node.name)
    print(json.dumps({'installed': str(target), 'refreshed_nodes': nodes}))

if __name__ == '__main__':
    main()
