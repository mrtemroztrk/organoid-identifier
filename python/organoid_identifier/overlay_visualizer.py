import os
import sys
import struct
import tempfile
import subprocess

def create_segmented_overlay_bmp(width: int, height: int, raw_bytes: bytes, channels: int, labels_bytes: bytes, outline_thickness: int = 2) -> bytes:
    """
    Zero-dependency pure Python overlay renderer with bold neon boundary highlights.
    Draws thick vibrant outlines and semi-transparent shaded interiors for every organoid object.
    """
    row_size = (width * 3 + 3) & ~3
    image_size = row_size * height
    file_size = 54 + image_size

    header = struct.pack('<2sIHHI', b'BM', file_size, 0, 0, 54)
    dib = struct.pack('<IiiHHIIIIII', 40, width, -height, 1, 24, 0, image_size, 2835, 2835, 0, 0)

    # Unpack label array (int32)
    labels = struct.unpack(f'<{width * height}i', labels_bytes)

    # Distinct high-contrast neon colors (BGR format)
    colors = [
        (0, 255, 0),     # Neon Green
        (0, 165, 255),   # Bright Orange
        (255, 255, 0),   # Bright Cyan
        (255, 0, 255),   # Bright Magenta
        (0, 255, 255),   # Neon Yellow
        (0, 128, 255),   # Deep Orange
        (255, 128, 0),   # Electric Blue
        (128, 255, 0)    # Lime
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

            if channels == 1:
                val = raw_bytes[src_idx]
                r, g, b = val, val, val
            elif channels >= 3:
                b, g, r = raw_bytes[src_idx], raw_bytes[src_idx + 1], raw_bytes[src_idx + 2]
            else:
                r, g, b = 128, 128, 128

            if lbl > 0:
                # Check neighborhood for thick boundary highlight
                is_boundary = False
                for dy in range(-outline_thickness, outline_thickness + 1):
                    for dx in range(-outline_thickness, outline_thickness + 1):
                        nx = x + dx
                        ny = y + dy
                        if nx < 0 or nx >= width or ny < 0 or ny >= height:
                            is_boundary = True
                            break
                        else:
                            nidx = ny * width + nx
                            if labels[nidx] != lbl:
                                is_boundary = True
                                break
                    if is_boundary:
                        break

                c_b, c_g, c_r = colors[(lbl - 1) % len(colors)]

                if is_boundary:
                    # Bold vibrant neon boundary highlight
                    out_pixels[dst_idx]     = c_b
                    out_pixels[dst_idx + 1] = c_g
                    out_pixels[dst_idx + 2] = c_r
                else:
                    # Soft 40% shaded blend inside organoid body
                    out_pixels[dst_idx]     = int(b * 0.6 + c_b * 0.4)
                    out_pixels[dst_idx + 1] = int(g * 0.6 + c_g * 0.4)
                    out_pixels[dst_idx + 2] = int(r * 0.6 + c_r * 0.4)
            else:
                out_pixels[dst_idx]     = b
                out_pixels[dst_idx + 1] = g
                out_pixels[dst_idx + 2] = r

    return header + dib + bytes(out_pixels)
