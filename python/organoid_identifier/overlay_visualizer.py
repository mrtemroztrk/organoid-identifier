import os
import sys
import struct
import tempfile
import subprocess

FONT_5X7 = {
    '#': [0x14, 0x3E, 0x14, 0x3E, 0x14],
    '0': [0x3E, 0x41, 0x41, 0x3E],
    '1': [0x42, 0x7F, 0x40],
    '2': [0x62, 0x51, 0x49, 0x46],
    '3': [0x22, 0x41, 0x49, 0x36],
    '4': [0x1C, 0x12, 0x7F, 0x10],
    '5': [0x27, 0x45, 0x45, 0x39],
    '6': [0x3E, 0x49, 0x49, 0x32],
    '7': [0x01, 0x71, 0x09, 0x07],
    '8': [0x36, 0x49, 0x49, 0x36],
    '9': [0x26, 0x49, 0x49, 0x3E]
}

def _draw_char(pixels: bytearray, width: int, height: int, row_size: int, start_x: int, start_y: int, char: str, color_bgr: tuple):
    bits = FONT_5X7.get(char, FONT_5X7['0'])
    c_b, c_g, c_r = color_bgr

    for col_idx, col_byte in enumerate(bits):
        x = start_x + col_idx
        for bit_idx in range(7):
            y = start_y + bit_idx
            if (col_byte >> bit_idx) & 1:
                if 0 <= x < width and 0 <= y < height:
                    # Draw 1px black shadow border for high contrast
                    for sy in range(-1, 2):
                        for sx in range(-1, 2):
                            nx, ny = x + sx, y + sy
                            if 0 <= nx < width and 0 <= ny < height:
                                n_dst = ny * row_size + nx * 3
                                pixels[n_dst] = 0
                                pixels[n_dst + 1] = 0
                                pixels[n_dst + 2] = 0

                    dst_idx = y * row_size + x * 3
                    pixels[dst_idx] = c_b
                    pixels[dst_idx + 1] = c_g
                    pixels[dst_idx + 2] = c_r

def _draw_label_text(pixels: bytearray, width: int, height: int, row_size: int, center_x: int, center_y: int, text: str):
    char_w = 4
    total_w = len(text) * (char_w + 1)
    start_x = center_x - total_w // 2
    start_y = center_y - 3

    curr_x = start_x
    for char in text:
        _draw_char(pixels, width, height, row_size, curr_x, start_y, char, (255, 255, 255))
        curr_x += char_w + 1

def create_segmented_overlay_bmp(
    width: int, 
    height: int, 
    raw_bytes: bytes, 
    channels: int, 
    labels_bytes: bytes, 
    centroids: dict = None, 
    outline_thickness: int = 1
) -> bytes:
    """
    Zero-dependency pure Python contour ring renderer.
    Draws ONLY sharp, precise contour rings matching organoid shapes (0% interior fill/shading)
    and small object numbers (#1, #2, #3...) at centroids.
    """
    row_size = (width * 3 + 3) & ~3
    image_size = row_size * height
    file_size = 54 + image_size

    header = struct.pack('<2sIHHI', b'BM', file_size, 0, 0, 54)
    dib = struct.pack('<IiiHHIIIIII', 40, width, -height, 1, 24, 0, image_size, 2835, 2835, 0, 0)

    labels = struct.unpack(f'<{width * height}i', labels_bytes)

    # Distinct vibrant colors for precise contour rings (BGR format)
    colors = [
        (0, 255, 0),     # Bright Green
        (0, 165, 255),   # Bright Orange
        (255, 255, 0),   # Cyan
        (255, 0, 255),   # Magenta
        (0, 255, 255),   # Yellow
        (0, 128, 255),   # Deep Orange
        (255, 128, 0),   # Electric Blue
        (128, 255, 0)    # Lime
    ]

    out_pixels = bytearray(image_size)

    if centroids is None:
        obj_sums = {}
        for y in range(height):
            for x in range(width):
                idx = y * width + x
                lbl = labels[idx]
                if lbl > 0:
                    if lbl not in obj_sums:
                        obj_sums[lbl] = [0, 0, 0]
                    obj_sums[lbl][0] += x
                    obj_sums[lbl][1] += y
                    obj_sums[lbl][2] += 1
        centroids = {lbl: (vals[0] // vals[2], vals[1] // vals[2]) for lbl, vals in obj_sums.items() if vals[2] > 0}

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
                    # Precise contour ring matching shape
                    out_pixels[dst_idx]     = c_b
                    out_pixels[dst_idx + 1] = c_g
                    out_pixels[dst_idx + 2] = c_r
                else:
                    # ZERO interior fill/shading — preserve 100% original pixel color
                    out_pixels[dst_idx]     = b
                    out_pixels[dst_idx + 1] = g
                    out_pixels[dst_idx + 2] = r
            else:
                out_pixels[dst_idx]     = b
                out_pixels[dst_idx + 1] = g
                out_pixels[dst_idx + 2] = r

    # Draw small object numbers (#1, #2, #3...) over centroids
    for lbl, (cx, cy) in centroids.items():
        _draw_label_text(out_pixels, width, height, row_size, cx, cy, f"#{lbl}")

    return header + dib + bytes(out_pixels)
