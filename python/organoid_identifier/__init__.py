import os
import sys
import struct
import tempfile
import subprocess
from ._core import (
    read_header, 
    read_tags, 
    read_format, 
    read_strip, 
    read_bits, 
    read_pixels
)

def inspect(file_path: str) -> None:
    print(read_header(file_path))

def dimensions(file_path: str) -> None:
    print(read_tags(file_path))

def format_info(file_path: str) -> None:
    print(read_format(file_path))

def strip_info(file_path: str) -> None:
    print(read_strip(file_path))

def bits_info(file_path: str) -> None:
    print(read_bits(file_path))

def _raw_to_bmp(width: int, height: int, raw_bytes: bytes, channels: int = 3) -> bytes:
    """Zero-dependency pure Python raw pixel to standard 24-bit BMP encoder."""
    # BMP satır boyutu 4 baytın katı olmak zorundadır (Padding)
    row_size = (width * 3 + 3) & ~3
    image_size = row_size * height
    file_size = 54 + image_size

    # 1. BMP File Header (14 bytes)
    header = struct.pack('<2sIHHI', b'BM', file_size, 0, 0, 54)

    # 2. DIB Header / BITMAPINFOHEADER (40 bytes)
    dib = struct.pack('<IiiHHIIIIII', 40, width, -height, 1, 24, 0, image_size, 2835, 2835, 0, 0)

    # 3. Piksel Verisini Hazırla
    src_stride = width * channels
    padding = b'\x00' * (row_size - width * 3)

    if channels == 3:
        if not padding and src_stride == width * 3 and len(raw_bytes) >= image_size:
            bmp_pixels = raw_bytes[:image_size]
        else:
            rows = []
            for y in range(height):
                src_idx = y * src_stride
                rows.append(raw_bytes[src_idx:src_idx + width * 3])
                if padding:
                    rows.append(padding)
            bmp_pixels = b''.join(rows)
    elif channels == 1:
        rows = []
        for y in range(height):
            src_idx = y * width
            row_gray = raw_bytes[src_idx:src_idx + width]
            bgr_row = bytearray(width * 3)
            bgr_row[0::3] = row_gray
            bgr_row[1::3] = row_gray
            bgr_row[2::3] = row_gray
            rows.append(bgr_row)
            if padding:
                rows.append(padding)
        bmp_pixels = b''.join(rows)
    else:
        bmp_pixels = bytearray(image_size)
        for y in range(height):
            src_row = y * src_stride
            dst_row = y * row_size
            for x in range(width):
                src_idx = src_row + x * channels
                dst_idx = dst_row + x * 3
                if src_idx + 2 < len(raw_bytes):
                    bmp_pixels[dst_idx]     = raw_bytes[src_idx]     # B
                    bmp_pixels[dst_idx + 1] = raw_bytes[src_idx + 1] # G
                    bmp_pixels[dst_idx + 2] = raw_bytes[src_idx + 2] # R
        bmp_pixels = bytes(bmp_pixels)

    return header + dib + bmp_pixels

def show(file_path: str) -> None:
    """Zero-dependency universal native OS window viewer using standard BMP."""
    data = read_pixels(file_path)
    width = data["width"]
    height = data["height"]
    channels = data.get("channels", 3)
    raw_bytes = data["raw_bytes"]

    # Ham piksel verisini standart 24-bit BMP formatına dönüştür
    bmp_data = _raw_to_bmp(width, height, raw_bytes, channels)

    # Temp klasörüne .bmp olarak kaydediyoruz
    temp_dir = tempfile.gettempdir()
    temp_bmp_path = os.path.join(temp_dir, "organoid_preview.bmp")

    with open(temp_bmp_path, "wb") as f:
        f.write(bmp_data)

    print(f"\n[+] Organoid görseli hazırlandı ({width}x{height} px, BMP)")
    print(f"[+] Sistem penceresinde açılıyor...")

    if sys.platform.startswith('linux'):
        subprocess.Popen(['xdg-open', temp_bmp_path], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    elif sys.platform == 'darwin':
        subprocess.Popen(['open', temp_bmp_path])
    elif sys.platform == 'win32':
        os.startfile(temp_bmp_path)

__all__ = [
    "inspect", "dimensions", "format_info", "strip_info", "bits_info", "show",
    "read_header", "read_tags", "read_format", "read_strip", "read_bits", "read_pixels"
]