"""One reversible live lighting check, retaining an exact config backup."""
import fcntl
import json
import os
from pathlib import Path
import struct
import subprocess
import sys
import time

root = Path(__file__).resolve().parents[1]
binary = root / 'build/aula-control'
node = sys.argv[1]

def read():
    return json.loads(subprocess.check_output([str(binary), '--read', node], text=True))

original = read()
config = bytes.fromhex(original['config_hex'])
(root / 'analysis/wired-original-config.json').write_text(json.dumps(original, indent=2) + '\n')
if original['mode'] not in range(1, 19):
    raise SystemExit('Live check requires a supported existing effect.')
brightness = 1 if original['brightness'] != 1 else 2
result = {'node': node, 'before': original, 'requested_brightness': brightness}
attempted = False
try:
    attempted = True
    process = subprocess.run([str(binary), '--apply', node, '--model', 'F75',
                              '--brightness', str(brightness)], capture_output=True, text=True)
    result['apply_returncode'] = process.returncode
    result['apply_stdout'] = process.stdout
    result['apply_stderr'] = process.stderr
    result['after_apply'] = read()
finally:
    if attempted:
        fd = os.open(node, os.O_RDWR)
        try:
            info = bytearray(8)
            fcntl.ioctl(fd, 0x80084803, info, True)
            bus, vendor, product = struct.unpack('IHH', info)
            if (vendor, product) != (0x258a, 0x010c):
                raise RuntimeError('Device identity changed; refusing restore')
            packet = bytearray(520)
            packet[:8] = bytes.fromhex('06 04 00 00 01 00 80 00')
            packet[8:136] = config
            time.sleep(.02)
            fcntl.ioctl(fd, 0xc0004806 | (520 << 16), packet, True)
        finally:
            os.close(fd)
        time.sleep(.1)
        restored = read()
        result['restored_exactly'] = restored['config_hex'] == original['config_hex']
        (root / 'analysis/wired-live-verification.json').write_text(json.dumps(result, indent=2) + '\n')
        print(json.dumps(result, indent=2))
        if not result['restored_exactly']:
            raise SystemExit('Original configuration read-back differs after restore')
if result['apply_returncode']:
    raise SystemExit(result['apply_returncode'])
