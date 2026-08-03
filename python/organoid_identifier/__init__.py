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
    read_pixels,
    calculate_metrics,
    calculate_multi_metrics,
    segment_organoids
)
from .model_manager import (
    download_model,
    list_local_models,
    remove_model,
    clear_models,
    get_model_path
)
from .overlay_visualizer import create_segmented_overlay_bmp

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

def analyze(file_path: str) -> dict:
    """Calculates and returns single organoid morphometry & pixel-wise signal metrics in 1 line."""
    data = read_pixels(file_path)
    width = data["width"]
    height = data["height"]
    channels = data.get("channels", 1)
    raw_bytes = data["raw_bytes"]

    mask = bytearray(width * height)
    stride = channels
    for i in range(width * height):
        p_idx = i * stride
        if any(raw_bytes[p_idx + c] > 0 for c in range(stride)):
            mask[i] = 255

    return calculate_metrics(bytes(mask), width, height, raw_bytes, channels)

def metrics_info(file_path: str) -> None:
    """Calculates and prints single organoid morphometry and pixel-wise signal intensity report in 1 line."""
    metrics = analyze(file_path)
    print(f"\n┌───────────────────────────────────────────────────────┐")
    print(f"│           ORGANOID MORPHOMETRY & INTENSITY            │")
    print(f"├───────────────────────────────────────────────────────┤")
    print(f"│  • File Path         : {file_path:<30} │")
    print(f"│  • Area (Pixels)     : {metrics['area']:<30} │")
    print(f"│  • Perimeter (Px)    : {metrics['perimeter']:<30} │")
    print(f"│  • Circularity Index : {metrics['circularity']:<30.4f} │")
    print(f"│  • Equiv. Diameter   : {metrics['equivalent_diameter']:<30.2f} │")
    print(f"│  • Centroid (x, y)   : ({metrics['centroid'][0]:.1f}, {metrics['centroid'][1]:.1f}){'':<18} │")
    print(f"├───────────────────────────────────────────────────────┤")
    print(f"│  • Mean Intensity    : {metrics['mean_intensity']:<30.2f} │")
    print(f"│  • Integrated Signal : {metrics['integrated_intensity']:<30.2f} │")
    print(f"│  • Min Intensity     : {metrics['min_intensity']:<30.2f} │")
    print(f"│  • Max Intensity     : {metrics['max_intensity']:<30.2f} │")
    print(f"│  • Heterogeneity (SD): {metrics['std_intensity']:<30.2f} │")
    print(f"└───────────────────────────────────────────────────────┘\n")

def segment(file_path: str, min_size: int = 100, min_circularity: float = 0.20) -> list:
    """Performs multi-object circular organoid segmentation and returns metric list for all objects in 1 line."""
    data = read_pixels(file_path)
    width = data["width"]
    height = data["height"]
    channels = data.get("channels", 1)
    raw_bytes = data["raw_bytes"]

    seg_res = segment_organoids(raw_bytes, channels, width, height, min_size, min_circularity)
    object_count = seg_res["object_count"]
    labels_bytes = seg_res["labels_bytes"]

    if object_count == 0:
        return []

    return calculate_multi_metrics(labels_bytes, object_count, width, height, raw_bytes, channels)

def segment_info(file_path: str, min_size: int = 100, min_circularity: float = 0.20) -> list:
    """Performs circular organoid segmentation, prints multi-object report table with metrics for every object in 1 line."""
    objects = segment(file_path, min_size, min_circularity)
    print(f"\n┌────────────────────────────────────────────────────────────────────────────────────────────────────────┐")
    print(f"│                               ORGANOID MULTI-OBJECT SEGMENTATION REPORT                                │")
    print(f"├──────┬──────────────┬────────────┬─────────────┬─────────────┬─────────────┬─────────────┬─────────────┤")
    print(f"│  ID  │ Area (Pixels)│ Perimeter  │ Circularity │ Diameter(px)│ Mean Int.   │ Total Int.  │ Hetero (SD) │")
    print(f"├──────┼──────────────┼────────────┼─────────────┼─────────────┼─────────────┼─────────────┼─────────────┤")

    if not objects:
        print(f"│  -   │  No circular organoids detected matching min_size >= {min_size:<8} & min_circularity >= {min_circularity:<6.2f} │")
    else:
        for obj in objects:
            print(f"│ #{obj['id']:<3} │ {obj['area']:<12} │ {obj['perimeter']:<10} │ {obj['circularity']:<11.4f} │ {obj['equivalent_diameter']:<11.2f} │ {obj['mean_intensity']:<11.2f} │ {obj['integrated_intensity']:<11.1f} │ {obj['std_intensity']:<11.2f} │")
    print(f"└──────┴──────────────┴────────────┴─────────────┴─────────────┴─────────────┴─────────────┴─────────────┘\n")
    return objects

def save_segmentation(file_path: str, output_path: str = "segmentation_overlay.bmp", min_size: int = 100, min_circularity: float = 0.20) -> str:
    """Performs circular organoid segmentation and saves highlighted overlay image with object numbers (#1, #2...) to disk in 1 line."""
    data = read_pixels(file_path)
    width = data["width"]
    height = data["height"]
    channels = data.get("channels", 1)
    raw_bytes = data["raw_bytes"]

    seg_res = segment_organoids(raw_bytes, channels, width, height, min_size, min_circularity)
    labels_bytes = seg_res["labels_bytes"]
    object_count = seg_res["object_count"]

    bmp_data = create_segmented_overlay_bmp(width, height, raw_bytes, channels, labels_bytes)

    dest_path = os.path.abspath(output_path)
    os.makedirs(os.path.dirname(dest_path), exist_ok=True) if os.path.dirname(dest_path) else None

    with open(dest_path, "wb") as f:
        f.write(bmp_data)

    print(f"[✓] Saved highlighted organoid segmentation image ({object_count} circular objects with #IDs) to: {dest_path}")
    return dest_path

def show_segmentation(file_path: str, min_size: int = 100, min_circularity: float = 0.20) -> None:
    """Performs circular organoid segmentation and opens native OS window with shaded outlines and #ID numbers in 1 line."""
    dest_path = save_segmentation(file_path, os.path.join(tempfile.gettempdir(), "organoid_segmentation_preview.bmp"), min_size, min_circularity)
    open_image(dest_path)

def open_image(image_path: str) -> None:
    """Dedicated 1-line command to open any image file in the native system viewer."""
    abs_path = os.path.abspath(image_path)
    if not os.path.exists(abs_path):
        raise FileNotFoundError(f"Image file not found: {abs_path}")

    print(f"\n[+] Opening image in native system viewer: {abs_path}")
    if sys.platform.startswith('linux'):
        subprocess.Popen(['xdg-open', abs_path], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    elif sys.platform == 'darwin':
        subprocess.Popen(['open', abs_path])
    elif sys.platform == 'win32':
        os.startfile(abs_path)

def _raw_to_bmp(width: int, height: int, raw_bytes: bytes, channels: int = 3) -> bytes:
    """Zero-dependency pure Python raw pixel to standard 24-bit BMP encoder."""
    row_size = (width * 3 + 3) & ~3
    image_size = row_size * height
    file_size = 54 + image_size

    header = struct.pack('<2sIHHI', b'BM', file_size, 0, 0, 54)
    dib = struct.pack('<IiiHHIIIIII', 40, width, -height, 1, 24, 0, image_size, 2835, 2835, 0, 0)

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

    bmp_data = _raw_to_bmp(width, height, raw_bytes, channels)

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
    "inspect", "dimensions", "format_info", "strip_info", "bits_info", "show", "metrics_info", "analyze",
    "segment", "segment_info", "show_segmentation", "save_segmentation", "open_image",
    "read_header", "read_tags", "read_format", "read_strip", "read_bits", "read_pixels",
    "download_model", "list_local_models", "remove_model", "clear_models", "get_model_path",
    "calculate_metrics", "calculate_multi_metrics", "segment_organoids"
]