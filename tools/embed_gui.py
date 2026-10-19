"""Embed UTF-8 Python source in the ELF; no runtime source files needed."""
import pathlib
import sys

source = pathlib.Path(sys.argv[1]).read_bytes() + b"\0"
symbol = sys.argv[3] if len(sys.argv) > 3 else "aula_gui_source"
lines = ["/* Generated Python source. */", f"static const char {symbol}[] = {{"]
for offset in range(0, len(source), 24):
    lines.append(",".join(str(x) for x in source[offset:offset + 24]) + ",")
lines.append("};\n")
pathlib.Path(sys.argv[2]).write_text("\n".join(lines))
