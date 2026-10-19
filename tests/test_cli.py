import json
from pathlib import Path
import subprocess

binary = Path(__file__).resolve().parents[1] / "build/aula-control"

def run(*args, code=0):
    p = subprocess.run([str(binary), *args], capture_output=True, text=True)
    assert p.returncode == code, (args, p.returncode, p.stdout, p.stderr)
    return p.stdout

assert len(json.loads(run("--modes"))) == 19
preview = json.loads(run("--preview", "--model", "F75", "--mode", "18", "--brightness", "9", "--speed", "4"))
assert preview["read_packet_prefix"] == [6, 132, 0, 0, 1, 0, 128, 0]
assert {"offset": 92, "value": 9, "mask": 255} in preview["config_patches"]
run("--preview", "--model", "F99", "--brightness", "9", code=2)
run("--apply", "/dev/hidraw0", "--model", "F75", "--mode", "-1", code=2)
run("--apply", "/dev/hidraw0", "--model", "F75", "--speed", "2oops", code=2)
run("--apply", "/dev/hidraw0", "--model", "F75", code=2)
run("--read", "/tmp/not-a-device", code=1)
devices = json.loads(run("--list"))
assert isinstance(devices, list)
assert all(d["vendor"] in ["258a", "3554"] for d in devices)
print("CLI validation, preview and discovery passed")
