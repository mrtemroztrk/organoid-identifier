import os
import sys
import struct
import tempfile
import subprocess

def create_segmented_overlay_bmp(width: int, height: int, raw_bytes: bytes, channels: int, labels_bytes: bytes) -> bytes:
    """
    Zero-dependency pure Python overlay renderer.
    Draws shaded/hatched contours around organoid objects over original TIFF image.
    """
    row_size = (width * 3 + 3) & ~3
    image_size = row_size * height
    file_size = 54 + image_size

    header = struct.pack('<2sIHHI', b'BM', file_size, 0, 0, 54)
    dib = struct.pack('<IiiHHIIIIII', 40, width, -height, 1, 24, 0, image_size, 2835, 2835, 0, 0)

    # Unpack label array (int32)
    labels = struct.unpack(f'<{width * height}i', labels_bytes)

    # Distinct vibrant colors for organoid outlines (BGR format)
    colors = [
        (0, 255, 0),     # Bright Green
        (0, 165, 255),   # Bright Orange
        (255, 255, 0),   # Cyan
        (255, 0, 255),   # Magenta
        (0, 255, 255),   # Yellow
        (50, 100, 255)   # Coral
    ]

    out_pixels = bytearray(image_size)

    for y in range(height):
        src_row = y * width * channels
        dst_row = y * row_size

        for x in range(width):
            idx = y * width + x
            lbl = labels[idx]

            src_idx = src_row + x * channels
            dst_idx = dst_row + x * 3

            # Default base image color
            if channels == 1:
                val = raw_bytes[src_idx]
                r, g, b = val, val, val
            elif channels >= 3:
                b, g, r = raw_bytes[src_idx], raw_bytes[src_idx + 1], raw_bytes[src_idx + 2]
            else:
                r, g, b = 128, 128, 128

            if lbl > 0:
                # Check 4-connectivity for boundary
                is_boundary = False
                if x == 0 or x == width - 1 or y == 0 or y == height - 1:
                    is_boundary = True
                else:
                    if labels[idx - 1] != lbl or labels[idx + 1] != lbl or \
                       labels[idx - width] != lbl or labels[idx + width] != lbl:
                        is_boundary = True

                c_b, c_g, c_r = colors[(lbl - 1) % len(colors)]

                if is_boundary:
                    # Solid vibrant outline
                    out_pixels[dst_idx]     = c_b
                    out_pixels[dst_idx + 1] = c_g
                    out_pixels[dst_idx + 2] = c_r
                else:
                    # Shaded / hatched interior (50% blend with outline color)
                    out_pixels[dst_idx]     = (b + c_b) // 2
                    out_pixels[dst_idx + 1] = (g + c_g) // 2
                    out_pixels[dst_idx + 2] = (r + c_r) // 2
            else:
                out_pixels[dst_idx]     = b
                out_pixels[dst_idx + 1] = g
                out_pixels[dst_idx + 2] = r

    return header + dib + bytes(out_pixels)
