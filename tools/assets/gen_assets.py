#!/usr/bin/env python3
from __future__ import annotations

import csv
import json
import os
import re
import struct
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, List, Tuple, Optional
import shutil


ROOT = Path(__file__).resolve().parents[2]  # .../SUNNY_OS
MANIFEST = ROOT / "resources" / "manifest.json"
OUT_DIR = ROOT / "resources" / "generated"
OUT_BIN = OUT_DIR / "bin"
OUT_C = OUT_DIR / "sunny_resources.c"
OUT_H = OUT_DIR / "sunny_resources.h"


def _err(msg: str) -> None:
    raise SystemExit(msg)


def _c_ident(name: str) -> str:
    s = re.sub(r"[^a-zA-Z0-9_]", "_", name)
    if not s or s[0].isdigit():
        s = "_" + s
    return s


def _macro_name(name: str) -> str:
    return re.sub(r"[^A-Z0-9_]", "_", name.upper())


def _parse_int(s: str) -> int:
    s = s.strip()
    if s.lower().startswith("0x"):
        return int(s, 16)
    return int(s, 10)


@dataclass
class BmpImage:
    w: int
    h: int
    # 行优先、从上到下的 RGBA8888 字节序列
    rgba: bytes


def load_bmp(path: Path) -> BmpImage:
    data = path.read_bytes()
    if len(data) < 54:
        _err(f"BMP too small: {path}")

    if data[0:2] != b"BM":
        _err(f"Not a BMP file: {path}")

    # BITMAPFILEHEADER
    file_size = struct.unpack_from("<I", data, 2)[0]
    off_bits = struct.unpack_from("<I", data, 10)[0]
    if file_size != 0 and file_size > len(data):
        _err(f"BMP size mismatch: {path}")

    # BITMAPINFOHEADER（按常见的 40 字节头处理）
    dib_size = struct.unpack_from("<I", data, 14)[0]
    if dib_size < 40:
        _err(f"Unsupported DIB header (size={dib_size}): {path}")

    w = struct.unpack_from("<i", data, 18)[0]
    h = struct.unpack_from("<i", data, 22)[0]
    planes = struct.unpack_from("<H", data, 26)[0]
    bpp = struct.unpack_from("<H", data, 28)[0]
    compression = struct.unpack_from("<I", data, 30)[0]

    if planes != 1:
        _err(f"Unsupported BMP planes={planes}: {path}")
    # BI_RGB = 0, BI_BITFIELDS = 3 (commonly used for 32bpp with channel masks, incl. alpha).
    if compression not in (0, 3):
        _err(f"Unsupported BMP compression={compression} (need BI_RGB or BI_BITFIELDS): {path}")
    if bpp not in (24, 32):
        _err(f"Unsupported BMP bpp={bpp} (need 24 or 32): {path}")
    if compression == 3 and bpp != 32:
        _err(f"Unsupported BMP BITFIELDS bpp={bpp} (need 32): {path}")
    if w <= 0 or h == 0:
        _err(f"Unsupported BMP dimensions {w}x{h}: {path}")

    top_down = h < 0
    height = -h if top_down else h
    width = w

    bytes_per_px = bpp // 8
    row_stride = ((width * bytes_per_px + 3) // 4) * 4

    if off_bits + row_stride * height > len(data):
        _err(f"BMP pixel data out of range: {path}")

    r_mask = g_mask = b_mask = a_mask = 0
    if compression == 3:
        # For BITMAPV4/V5 headers the masks are stored in the DIB header at offset 14+40.
        if dib_size < 56:
            _err(f"Unsupported BITFIELDS DIB header (size={dib_size}): {path}")
        r_mask = struct.unpack_from("<I", data, 14 + 40 + 0)[0]
        g_mask = struct.unpack_from("<I", data, 14 + 40 + 4)[0]
        b_mask = struct.unpack_from("<I", data, 14 + 40 + 8)[0]
        a_mask = struct.unpack_from("<I", data, 14 + 40 + 12)[0]
        if r_mask == 0 or g_mask == 0 or b_mask == 0:
            _err(f"Invalid BITFIELDS masks: {path}")

        def _ctz(x: int) -> int:
            n = 0
            while x and (x & 1) == 0:
                x >>= 1
                n += 1
            return n

        def _popcount(x: int) -> int:
            c = 0
            while x:
                x &= x - 1
                c += 1
            return c

        r_shift, g_shift, b_shift = _ctz(r_mask), _ctz(g_mask), _ctz(b_mask)
        a_shift = _ctz(a_mask) if a_mask else 0
        r_bits, g_bits, b_bits = _popcount(r_mask), _popcount(g_mask), _popcount(b_mask)
        a_bits = _popcount(a_mask) if a_mask else 0

        def _scale(v: int, bits: int) -> int:
            if bits <= 0:
                return 255
            if bits == 8:
                return v & 0xFF
            maxv = (1 << bits) - 1
            return (v * 255 + (maxv // 2)) // maxv

    rgba = bytearray(width * height * 4)

    for row in range(height):
        src_row = row if top_down else (height - 1 - row)
        src_off = off_bits + src_row * row_stride
        dst_off = row * width * 4
        for col in range(width):
            if bytes_per_px == 3:
                b = data[src_off + col * 3 + 0]
                g = data[src_off + col * 3 + 1]
                r = data[src_off + col * 3 + 2]
                a = 255
            else:
                if compression == 0:
                    b = data[src_off + col * 4 + 0]
                    g = data[src_off + col * 4 + 1]
                    r = data[src_off + col * 4 + 2]
                    a = data[src_off + col * 4 + 3]
                else:
                    px = struct.unpack_from("<I", data, src_off + col * 4)[0]
                    r = _scale((px & r_mask) >> r_shift, r_bits)
                    g = _scale((px & g_mask) >> g_shift, g_bits)
                    b = _scale((px & b_mask) >> b_shift, b_bits)
                    a = _scale((px & a_mask) >> a_shift, a_bits) if a_mask else 255
            rgba[dst_off + col * 4 + 0] = r
            rgba[dst_off + col * 4 + 1] = g
            rgba[dst_off + col * 4 + 2] = b
            rgba[dst_off + col * 4 + 3] = a

    return BmpImage(w=width, h=height, rgba=bytes(rgba))


def _find_magick() -> Optional[str]:
    return shutil.which("magick")


def rasterize_svg(svg_path: Path, px: int, color: str = "#FFFFFF") -> BmpImage:
    """
    Rasterize an SVG to RGBA8888 using ImageMagick (magick).

    Notes:
    - Many SVG icons use `currentColor`. We replace it with `color` so the
      generated bitmap is monochrome (useful for LVGL recolor).
    - Use `-size` (not just `-resize`) to avoid 1em SVG defaulting to 12px.
    """
    if px <= 0:
        _err(f"Invalid svg px={px}: {svg_path}")

    magick = _find_magick()
    if not magick:
        _err("SVG rasterization requires ImageMagick on PATH (magick).")

    svg_text = svg_path.read_text(encoding="utf-8")
    if color:
        svg_text = svg_text.replace("currentColor", color)

    # Use workspace temp dir (system temp may be blocked by sandbox/ACL on some setups).
    # Avoid tempfile directories on some Windows setups where Python-created folders may get a
    # restrictive ACL. Using fixed temp files under OUT_DIR is reliable and build-friendly.
    # Use per-process temp files under OUT_DIR (some Windows setups block Python-created temp dirs).
    pid = os.getpid()
    tmp_svg = OUT_DIR / f"_tmp_svg_{pid}.svg"
    tmp_bmp = OUT_DIR / f"_tmp_svg_{pid}.bmp"
    try:
        tmp_svg.write_text(svg_text, encoding="utf-8")

        cmd = [
            magick,
            "-background",
            "none",
            "-size",
            f"{px}x{px}",
            str(tmp_svg),
            "-alpha",
            "on",
            # Keep alpha channel for transparent background; ImageMagick will usually emit 32bpp BMP
            # using BI_BITFIELDS (compression=3), which our BMP loader supports.
            str(tmp_bmp),
        ]
        proc = subprocess.run(cmd, capture_output=True, text=True)
        if proc.returncode != 0:
            out = (proc.stdout or "") + (proc.stderr or "")
            _err(f"Failed to rasterize SVG via ImageMagick: {svg_path}\n{out.strip()}")

        if not tmp_bmp.exists():
            out = (proc.stdout or "") + (proc.stderr or "")
            _err(f"ImageMagick did not produce output BMP for {svg_path}\n{out.strip()}")

        return load_bmp(tmp_bmp)
    finally:
        try:
            if tmp_svg.exists():
                tmp_svg.unlink()
        except OSError:
            pass
        try:
            if tmp_bmp.exists():
                tmp_bmp.unlink()
        except OSError:
            pass


def expand_image_dirs(manifest: Dict[str, object]) -> List[Dict[str, object]]:
    out: List[Dict[str, object]] = []
    dirs = manifest.get("image_dirs", [])
    if dirs is None:
        return out
    if not isinstance(dirs, list):
        _err("manifest.image_dirs must be a list")

    for spec in dirs:
        if not isinstance(spec, dict):
            _err("manifest.image_dirs entries must be objects")
        dir_rel = str(spec.get("dir", "")).strip()
        if not dir_rel:
            _err("manifest.image_dirs[].dir is required")
        glob_pat = str(spec.get("glob", "*")).strip() or "*"

        strip_prefix = str(spec.get("strip_prefix", "") or "")
        name_prefix = str(spec.get("name_prefix", "") or "")
        with_alpha = bool(spec.get("with_alpha", True))
        px = int(spec.get("px", 24))
        svg_color = str(spec.get("svg_color", "#FFFFFF") or "#FFFFFF")

        abs_dir = (ROOT / dir_rel).resolve()
        if not abs_dir.exists():
            _err(f"Image dir not found: {abs_dir}")

        paths = sorted([p for p in abs_dir.glob(glob_pat) if p.is_file()])
        for p in paths:
            stem = p.stem
            if strip_prefix and stem.startswith(strip_prefix):
                stem = stem[len(strip_prefix) :]
            name = f"{name_prefix}{stem}"
            out.append(
                {
                    "name": name,
                    "file": p.relative_to(ROOT).as_posix(),
                    "with_alpha": with_alpha,
                    "px": px,
                    "svg_color": svg_color,
                }
            )
    return out


def rgba_to_rgb565(r: int, g: int, b: int) -> int:
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3)


def lvgl_img_bytes(img: BmpImage, with_alpha: bool) -> bytes:
    out = bytearray()
    px = img.rgba
    for i in range(0, len(px), 4):
        r = px[i + 0]
        g = px[i + 1]
        b = px[i + 2]
        a = px[i + 3]
        c565 = rgba_to_rgb565(r, g, b)
        out.append(c565 & 0xFF)
        out.append((c565 >> 8) & 0xFF)
        if with_alpha:
            out.append(a)
    return bytes(out)


@dataclass
class Glyph:
    codepoint: int
    adv_w: int
    box_w: int
    box_h: int
    ofs_x: int
    ofs_y: int
    bpp: int
    bitmap_off: int


def load_glyphs_csv(path: Path) -> List[Dict[str, str]]:
    rows: List[Dict[str, str]] = []
    with path.open("r", encoding="utf-8-sig", newline="") as f:
        reader = csv.DictReader(f)
        for row in reader:
            if not row:
                continue
            rows.append({k.strip(): (v.strip() if isinstance(v, str) else "") for k, v in row.items() if k})
    return rows


def build_bitmap_font(atlas: BmpImage, glyph_rows: List[Dict[str, str]]) -> Tuple[List[Glyph], bytes]:
    # 将图集像素转换为 8bpp alpha（0..255），使用 max(R,G,B) * A/255
    w, h = atlas.w, atlas.h
    px = atlas.rgba

    def atlas_alpha(x: int, y: int) -> int:
        i = (y * w + x) * 4
        r = px[i + 0]
        g = px[i + 1]
        b = px[i + 2]
        a = px[i + 3]
        v = max(r, g, b)
        return (v * a) // 255

    glyphs: List[Glyph] = []
    bitmaps = bytearray()

    for row in glyph_rows:
        for need in ("codepoint", "x", "y", "w", "h", "adv_w", "ofs_x", "ofs_y"):
            if need not in row or row[need] == "":
                _err(f"Missing column '{need}' in {row} (file: {MANIFEST})")

        cp = _parse_int(row["codepoint"])
        x = _parse_int(row["x"])
        y = _parse_int(row["y"])
        gw = _parse_int(row["w"])
        gh = _parse_int(row["h"])
        adv_w = _parse_int(row["adv_w"])
        ofs_x = int(row["ofs_x"])
        ofs_y = int(row["ofs_y"])

        if gw <= 0 or gh <= 0:
            continue
        if x < 0 or y < 0 or x + gw > w or y + gh > h:
            _err(f"Glyph out of atlas bounds: codepoint={cp} rect=({x},{y},{gw},{gh}) atlas={w}x{h}")

        off = len(bitmaps)
        # 存储 8bpp alpha bitmap（行优先）
        for yy in range(y, y + gh):
            for xx in range(x, x + gw):
                bitmaps.append(atlas_alpha(xx, yy))

        glyphs.append(
            Glyph(
                codepoint=cp,
                adv_w=adv_w,
                box_w=gw,
                box_h=gh,
                ofs_x=ofs_x,
                ofs_y=ofs_y,
                bpp=8,
                bitmap_off=off,
            )
        )

    glyphs.sort(key=lambda g: g.codepoint)
    return glyphs, bytes(bitmaps)


def write_c_array(f, name: str, data: bytes) -> None:
    f.write(f"static const unsigned char {name}[] = {{\n")
    for i in range(0, len(data), 12):
        chunk = data[i : i + 12]
        f.write("  " + ", ".join(f"0x{b:02X}" for b in chunk) + ",\n")
    f.write("};\n\n")


def main() -> int:
    if not MANIFEST.exists():
        _err(f"Manifest not found: {MANIFEST}")

    manifest = json.loads(MANIFEST.read_text(encoding="utf-8"))
    images = manifest.get("images", [])
    fonts = manifest.get("fonts", [])
    strings = manifest.get("strings", [])

    if not isinstance(images, list):
        _err("manifest.images must be a list")
    images = list(images) + expand_image_dirs(manifest)

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    OUT_BIN.mkdir(parents=True, exist_ok=True)

    c_lines: List[str] = []
    h_lines: List[str] = []

    h_lines.append("#pragma once\n")
    h_lines.append('#include "lvgl.h"\n')
    h_lines.append("\n// 自动生成文件，请勿手工修改。\n\n")
    # 通用辅助宏：SUNNY_IMG(LOGO)、SUNNY_FONT(MYFONT_16)、SUNNY_STR(APP_TITLE)
    h_lines.append("#define SUNNY_IMG(name) (&sunny_img_##name)\n")
    h_lines.append("#define SUNNY_FONT(name) (&sunny_font_##name)\n")
    h_lines.append("#define SUNNY_STR(name) (sunny_str_##name)\n\n")

    c_lines.append('#include "sunny_resources.h"\n')
    c_lines.append('#include "lvgl.h"\n\n')

    # 字符串
    if strings:
        h_lines.append("// 字符串\n")
        for s in strings:
            nm = _macro_name(s["name"])
            val = s["value"]
            h_lines.append(f'#define sunny_str_{nm} "{val}"\n')
            h_lines.append(f'#define SUNNY_STR_{nm} sunny_str_{nm}\n')
        h_lines.append("\n")

    # 图片
    if images:
        h_lines.append("// 图片\n")
        for img in images:
            name = _macro_name(img["name"])
            ident = _c_ident(img["name"])
            in_path = (ROOT / img["file"]).resolve()
            if not in_path.exists():
                _err(f"Image file not found: {in_path}")
            with_alpha = bool(img.get("with_alpha", True))

            ext = in_path.suffix.lower()
            if ext == ".bmp":
                bmp = load_bmp(in_path)
            elif ext == ".svg":
                px = int(img.get("px", 24))
                svg_color = str(img.get("svg_color", "#FFFFFF") or "#FFFFFF")
                bmp = rasterize_svg(in_path, px=px, color=svg_color)
            else:
                _err(f"Unsupported image type: {in_path} (need .bmp or .svg)")
            raw = lvgl_img_bytes(bmp, with_alpha)

            # 可选 bin 输出（LVGL 原始像素字节，便于后续打包/烧录/外部加载）
            bin_path = OUT_BIN / f"img_{ident}.bin"
            bin_path.write_bytes(raw)

            map_name = f"sunny_img_{ident}_map"
            dsc_name = f"sunny_img_{name}"

            # 写入 C 数组
            c_lines.append(f"// 图片: {img['name']} ({bmp.w}x{bmp.h})\n")
            c_lines.append(f"// 源文件: {in_path.as_posix()}\n")
            c_lines.append(f"// Bin: {bin_path.as_posix()}\n")
            # Generate array
            c_lines.append(f"static const unsigned char {map_name}[] = {{\n")
            for i in range(0, len(raw), 12):
                chunk = raw[i : i + 12]
                c_lines.append("  " + ", ".join(f"0x{b:02X}" for b in chunk) + ",\n")
            c_lines.append("};\n\n")

            cf = "LV_IMG_CF_TRUE_COLOR_ALPHA" if with_alpha else "LV_IMG_CF_TRUE_COLOR"
            c_lines.append(f"const lv_img_dsc_t {dsc_name} = {{\n")
            c_lines.append(f"  .header = {{ .cf = {cf}, .w = {bmp.w}, .h = {bmp.h} }},\n")
            c_lines.append(f"  .data_size = {len(raw)},\n")
            c_lines.append(f"  .data = {map_name},\n")
            c_lines.append("};\n\n")

            h_lines.append(f"extern const lv_img_dsc_t {dsc_name};\n")
            h_lines.append(f"#define SUNNY_IMG_{name} (&{dsc_name})\n")
        h_lines.append("\n")

    # 字体（bitmap）
    if fonts:
        h_lines.append("// 字体\n")
        for ft in fonts:
            name = _macro_name(ft["name"])
            ident = _c_ident(ft["name"])
            atlas_path = (ROOT / ft["atlas_bmp"]).resolve()
            csv_path = (ROOT / ft["glyphs_csv"]).resolve()
            if not atlas_path.exists():
                _err(f"Font atlas not found: {atlas_path}")
            if not csv_path.exists():
                _err(f"Font glyph csv not found: {csv_path}")

            line_height = int(ft["line_height"])
            base_line = int(ft["base_line"])

            atlas = load_bmp(atlas_path)
            glyph_rows = load_glyphs_csv(csv_path)
            glyphs, bitmaps = build_bitmap_font(atlas, glyph_rows)

            bin_path = OUT_BIN / f"font_{ident}_bitmap.bin"
            bin_path.write_bytes(bitmaps)

            c_lines.append(f"// 字体: {ft['name']} glyphs={len(glyphs)}\n")
            c_lines.append(f"// 图集: {atlas_path.as_posix()}\n")
            c_lines.append(f"// CSV: {csv_path.as_posix()}\n")
            c_lines.append(f"// Bin: {bin_path.as_posix()}\n\n")

            bmp_name = f"sunny_font_{ident}_bitmap"
            c_lines.append(f"static const unsigned char {bmp_name}[] = {{\n")
            for i in range(0, len(bitmaps), 12):
                chunk = bitmaps[i : i + 12]
                c_lines.append("  " + ", ".join(f"0x{b:02X}" for b in chunk) + ",\n")
            c_lines.append("};\n\n")

            glyph_struct = f"sunny_font_{ident}_glyph_t"
            glyph_arr = f"sunny_font_{ident}_glyphs"

            c_lines.append(f"typedef struct {glyph_struct} {{\n")
            c_lines.append("  uint32_t codepoint;\n")
            c_lines.append("  uint16_t adv_w;\n")
            c_lines.append("  uint16_t box_w;\n")
            c_lines.append("  uint16_t box_h;\n")
            c_lines.append("  int16_t ofs_x;\n")
            c_lines.append("  int16_t ofs_y;\n")
            c_lines.append("  uint8_t bpp;\n")
            c_lines.append("  uint32_t bitmap_off;\n")
            c_lines.append(f"}} {glyph_struct};\n\n")

            c_lines.append(f"static const {glyph_struct} {glyph_arr}[] = {{\n")
            for g in glyphs:
                c_lines.append(
                    f"  {{ {g.codepoint}u, {g.adv_w}u, {g.box_w}u, {g.box_h}u, {g.ofs_x}, {g.ofs_y}, {g.bpp}u, {g.bitmap_off}u }},\n"
                )
            c_lines.append("};\n\n")

            dsc_fn = f"sunny_font_{ident}_get_dsc"
            bmp_fn = f"sunny_font_{ident}_get_bitmap"

            c_lines.append(f"static bool {dsc_fn}(const lv_font_t* font, lv_font_glyph_dsc_t* dsc, uint32_t letter, uint32_t letter_next)\n")
            c_lines.append("{\n")
            c_lines.append("  (void)font; (void)letter_next;\n")
            c_lines.append(f"  const int n = (int)(sizeof({glyph_arr}) / sizeof({glyph_arr}[0]));\n")
            c_lines.append("  int lo = 0, hi = n - 1;\n")
            c_lines.append("  while(lo <= hi) {\n")
            c_lines.append("    int mid = (lo + hi) / 2;\n")
            c_lines.append(f"    uint32_t cp = {glyph_arr}[mid].codepoint;\n")
            c_lines.append("    if(cp == letter) {\n")
            c_lines.append(f"      dsc->adv_w = {glyph_arr}[mid].adv_w;\n")
            c_lines.append(f"      dsc->box_w = {glyph_arr}[mid].box_w;\n")
            c_lines.append(f"      dsc->box_h = {glyph_arr}[mid].box_h;\n")
            c_lines.append(f"      dsc->ofs_x = {glyph_arr}[mid].ofs_x;\n")
            c_lines.append(f"      dsc->ofs_y = {glyph_arr}[mid].ofs_y;\n")
            c_lines.append(f"      dsc->bpp = {glyph_arr}[mid].bpp;\n")
            c_lines.append("      dsc->is_placeholder = 0;\n")
            c_lines.append("      dsc->resolved_font = font;\n")
            c_lines.append("      return true;\n")
            c_lines.append("    }\n")
            c_lines.append("    if(cp < letter) lo = mid + 1; else hi = mid - 1;\n")
            c_lines.append("  }\n")
            c_lines.append("  return false;\n")
            c_lines.append("}\n\n")

            c_lines.append(f"static const uint8_t* {bmp_fn}(const lv_font_t* font, uint32_t letter)\n")
            c_lines.append("{\n")
            c_lines.append("  (void)font;\n")
            c_lines.append(f"  const int n = (int)(sizeof({glyph_arr}) / sizeof({glyph_arr}[0]));\n")
            c_lines.append("  int lo = 0, hi = n - 1;\n")
            c_lines.append("  while(lo <= hi) {\n")
            c_lines.append("    int mid = (lo + hi) / 2;\n")
            c_lines.append(f"    uint32_t cp = {glyph_arr}[mid].codepoint;\n")
            c_lines.append("    if(cp == letter) {\n")
            c_lines.append(f"      return &{bmp_name}[{glyph_arr}[mid].bitmap_off];\n")
            c_lines.append("    }\n")
            c_lines.append("    if(cp < letter) lo = mid + 1; else hi = mid - 1;\n")
            c_lines.append("  }\n")
            c_lines.append("  return NULL;\n")
            c_lines.append("}\n\n")

            c_lines.append(f"const lv_font_t sunny_font_{name} = {{\n")
            c_lines.append(f"  .get_glyph_dsc = {dsc_fn},\n")
            c_lines.append(f"  .get_glyph_bitmap = {bmp_fn},\n")
            c_lines.append(f"  .line_height = {line_height},\n")
            c_lines.append(f"  .base_line = {base_line},\n")
            c_lines.append("  .subpx = LV_FONT_SUBPX_NONE,\n")
            c_lines.append("  .underline_position = 0,\n")
            c_lines.append("  .underline_thickness = 0,\n")
            c_lines.append("  .dsc = NULL,\n")
            c_lines.append("  .fallback = NULL,\n")
            c_lines.append("};\n\n")

            h_lines.append(f"extern const lv_font_t sunny_font_{name};\n")
            h_lines.append(f"#define SUNNY_FONT_{name} (&sunny_font_{name})\n")
        h_lines.append("\n")

    # Use UTF-8 with BOM to keep Windows tools (e.g. Notepad/PowerShell) happy.
    OUT_H.write_text("".join(h_lines), encoding="utf-8-sig")
    OUT_C.write_text("".join(c_lines), encoding="utf-8-sig")

    return 0


if __name__ == "__main__":
    sys.exit(main())
