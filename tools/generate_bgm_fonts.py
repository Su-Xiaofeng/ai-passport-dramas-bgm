#!/usr/bin/env python3
"""Rebuild fixed-text CJK subsets using the pinned LVGL converter 1.5.3."""
import argparse
import json
from pathlib import Path
import re
import subprocess

ROOT = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--converter", default="lv_font_conv")
args = parser.parse_args()
version = subprocess.check_output([args.converter, "--version"], text=True).strip()
if version != "1.5.3":
    parser.error("Requires lv_font_conv 1.5.3, got " + version)
tracks = json.loads((ROOT / "assets/music/playlist.json").read_text())["tracks"]
# Gather literal Chinese UI text, plus title metadata and every printable ASCII.
ui = (ROOT / "main/bgm_ui.c").read_text() + (ROOT / "main/queen_bgm.h").read_text()
strings = re.findall(r'"([^"\n]*)"', ui)
chars = sorted(set("".join(t["title"] for t in tracks) + "".join(strings) +
                   "".join(chr(i) for i in range(32, 127))))
font_dir = ROOT / "assets/fonts"
(font_dir / "bgm_chars.txt").write_text("".join(chars) + "\n")
(font_dir / "bgm_glyphs.h").write_text(
    "#pragma once\n#include <stdint.h>\nstatic const uint32_t bgm_glyphs[] = {\n" +
    ", ".join(hex(ord(c)) for c in chars) + "\n};\n")
for size in (12, 16, 20):
    subprocess.run([args.converter, "--font", "assets/fonts/NotoSansCJKsc-Regular.otf",
        "--symbols", "".join(chars), "--size", str(size), "--bpp", "4", "--format", "lvgl",
        "--no-compress", "--no-kerning", "--lv-font-name", f"bgm_font_{size}",
        "--lv-include", "lvgl.h", "--output", f"assets/fonts/bgm_font_{size}.c"], cwd=ROOT, check=True)
    generated = font_dir / f"bgm_font_{size}.c"
    generated.write_text(generated.read_text().rstrip() + "\n")
print("Generated and inventoried three CJK font sizes")
