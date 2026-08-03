# API Reference

This page explains every function of `organoid_identifier`, with real examples
and actual output.

---

## Printing functions

These functions **print** a nicely formatted report to your terminal.

### `inspect(file_path)`

Checks whether the file is a valid TIFF and shows basic header information.

| Field | Meaning |
|---|---|
| **File Path** | The file you gave us |
| **Status** | `VALID TIFF` or an error |
| **Byte Order** | How bytes are stored: `Little-Endian (II)` or `Big-Endian (MM)` |
| **Magic Number** | TIFF identifier; always `42` for valid files |
| **First IFD Offset** | Byte position of the tag table inside the file |

**Example**

```python
import organoid_identifier as oi

oi.inspect("example_data/Overlay_BK52_WT_BGR.tif")
```

**Output**

```
┌───────────────────────────────────────────────────────┐
│              TIFF FILE INSPECTION REPORT              │
├───────────────────────────────────────────────────────┤
│  • File Path         : example_data/Overlay_BK52_WT_BGR.tif │
│  • Status            : VALID TIFF                     │
│  • Byte Order        : Little-Endian (II)             │
│  • Magic Number      : 42                             │
│  • First IFD Offset  : 11297914                 bytes │
└───────────────────────────────────────────────────────┘
```

---

### `dimensions(file_path)`

Shows the size and color depth of the image.

| Field | Meaning |
|---|---|
| **Width (Pixels)** | Image width in pixels |
| **Height (Pixels)** | Image height in pixels |
| **Bits Per Sample** | Bits per color channel (8 = standard, 16 = high depth) |

**Example**

```python
import organoid_identifier as oi

oi.dimensions("example_data/Overlay_BK52_WT_BGR.tif")
```

**Output**

```
┌───────────────────────────────────────────────────────┐
│               TIFF IMAGE DIMENSIONS REPORT            │
├───────────────────────────────────────────────────────┤
│  • Width  (Pixels)   : 2323                           │
│  • Height (Pixels)   : 2253                           │
│  • Bits Per Sample   : 8                              │
└───────────────────────────────────────────────────────┘
```

---

### `format_info(file_path)`

Shows the color model, channel count and compression of the image.

| Field | Meaning |
|---|---|
| **Channels (Samples)** | Number of color channels (1 = grayscale, 3 = RGB) |
| **Color Space** | `RGB Color`, `Grayscale / Monochromatic`, or `Unknown` |
| **Compression** | `Uncompressed`, `LZW Compression`, or `Unknown` |

**Example**

```python
import organoid_identifier as oi

oi.format_info("example_data/Overlay_BK52_WT_BGR.tif")
```

**Output**

```
┌───────────────────────────────────────────────────────┐
│               TIFF FORMAT & COLOR REPORT              │
├───────────────────────────────────────────────────────┤
│  • Channels (Samples): 3                              │
│  • Color Space       : RGB Color                      │
│  • Compression       : LZW Compression                │
└───────────────────────────────────────────────────────┘
```

---

### `strip_info(file_path)`

Extracts raw pixel data offset (address) and total byte count from TIFF strip tags.

| Field | Meaning |
|---|---|
| **Strip Offset** | Byte position where the raw image payload starts |
| **Raw Pixel Data** | Total size of the image payload in bytes |

**Example**

```python
import organoid_identifier as oi

oi.strip_info("example_data/Overlay_BK52_WT_BGR.tif")
```

**Output**

```
┌───────────────────────────────────────────────────────┐
│              TIFF STRIP & PIXEL LOCATION              │
├───────────────────────────────────────────────────────┤
│  • Strip Offset      : 11298118   bytes (Address) │
│  • Raw Pixel Data    : 11307130   bytes (Length)  │
└───────────────────────────────────────────────────────┘
```

---

### `bits_info(file_path)`

Shows bit depth per sample and numeric representation data type.

| Field | Meaning |
|---|---|
| **Bits Per Sample** | Bits per channel (e.g. 8 bits, 16 bits) |
| **Data Format** | Data representation type (e.g. `Unsigned Integer`, `Signed Integer`, `Floating Point`) |

**Example**

```python
import organoid_identifier as oi

oi.bits_info("example_data/Overlay_BK52_WT_BGR.tif")
```

**Output**

```
┌───────────────────────────────────────────────────────┐
│             TIFF BIT DEPTH & DATA TYPE                │
├───────────────────────────────────────────────────────┤
│  • Bits Per Sample   : 8  bits                      │
│  • Data Format       : Unsigned Integer          │
└───────────────────────────────────────────────────────┘
```

---

## Native Image Viewer

### `show(file_path)`

Universal, zero-dependency native OS window viewer. Converts raw image payload directly into standard 24-bit BMP in memory and opens it using the default image viewer of your system (`xdg-open` on Linux, `open` on macOS, `os.startfile` on Windows).

```python
import organoid_identifier as oi

oi.show("example_data/Overlay_BK52_WT_BGR.tif")
```

**Output**

```
[+] Organoid görseli hazırlandı (2323x2253 px, BMP)
[+] Sistem penceresinde açılıyor...
```

---

## Return-value functions

These functions do the work in C and **return data structures or formatted report strings** instead of printing.

### `read_header(file_path)` → `str`

Same report as `inspect()`, returned as a string.

```python
import organoid_identifier as oi

report = oi.read_header("example_data/Overlay_BK52_WT_BGR.tif")
with open("report.txt", "w") as f:
    f.write(report)
```

### `read_tags(file_path)` → `str`

Same report as `dimensions()`, returned as a string.

### `read_format(file_path)` → `str`

Same report as `format_info()`, returned as a string.

### `read_strip(file_path)` → `str`

Same report as `strip_info()`, returned as a string.

### `read_bits(file_path)` → `str`

Same report as `bits_info()`, returned as a string.

### `read_pixels(file_path)` → `dict`

Extracts raw pixel bytes and dimensions directly into a Python dictionary.

| Key | Type | Description |
|---|---|---|
| `width` | `int` | Image width in pixels |
| `height` | `int` | Image height in pixels |
| `channels` | `int` | Number of color channels |
| `raw_bytes` | `bytes` | Raw uncompressed image pixel payload |

```python
import organoid_identifier as oi

data = oi.read_pixels("example_data/Overlay_BK52_WT_BGR.tif")
print(data["width"], data["height"], len(data["raw_bytes"]))
```

---

## Model Management & Cache

Isolated CellPose model downloader and local cache manager (`~/.organoid_identifier/models`).

### `download_model(model_name="cyto3", force=False)` → `str`

Downloads built-in CellPose models (`cyto3`, `cyto2`, `nuclei`, `organoid`) without external dependencies. If the model already exists locally, it skips re-downloading unless `force=True`.

```python
import organoid_identifier as oi

path = oi.download_model("cyto3")
# Update or force re-download
path = oi.download_model("cyto3", force=True)
```

### `list_local_models()` → `list`

Lists all locally installed models in the user cache directory with file sizes and absolute paths.

```python
import organoid_identifier as oi

oi.list_local_models()
```

### `remove_model(model_name="cyto3")` → `bool`

Deletes a specific model file from the local cache.

### `clear_models()` → `None`

Deletes all downloaded models to free disk space.

---

## Organoid Morphometry Metrics

### `calculate_metrics(mask_bytes, width, height)` → `dict`

High-performance C morphometry engine. Calculates area, perimeter, circularity, equivalent diameter, centroid, and bounding box for binary mask data.

```python
import organoid_identifier as oi

metrics = oi.calculate_metrics(mask_bytes, width, height)
print(metrics["area"], metrics["circularity"], metrics["centroid"])
```

---

## Error handling

If the file does not exist or is not a valid TIFF, the functions raise a **`ValueError`**:

```python
import organoid_identifier as oi

try:
    oi.inspect("not_a_real_file.tif")
except ValueError as e:
    print(f"Something went wrong: {e}")
```

**Output**

```
Something went wrong: Invalid TIFF header.
```

> 💡 Tip: wrap calls in a `try/except ValueError` when you loop over many files —
> so one bad file doesn't stop the whole script:

```python
import glob
import organoid_identifier as oi

for file in sorted(glob.glob("example_data/*.tif")):
    try:
        oi.format_info(file)
    except ValueError as e:
        print(f"Skipped {file}: {e}")
```