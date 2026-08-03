# API Reference

This page explains every function of `organoid_identifier`, with real examples
and actual output.

---

## Printing functions

These three functions **print** a nicely formatted report to your terminal.

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

## Return-value functions

The next three functions do **exactly the same work** as the ones above, but
they **return the report as a string** instead of printing it.

This is useful when you want to:

- Save the report to a file
- Send it somewhere (email, log, database)
- Keep your terminal clean

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

```python
import organoid_identifier as oi

report = oi.read_tags("example_data/Overlay_BK52_WT_BGR.tif")
print(report.upper())  # or do anything else with it
```

### `read_format(file_path)` → `str`

Same report as `format_info()`, returned as a string.

---

## Error handling

If the file does not exist or is not a valid TIFF, the functions raise a
**`ValueError`**:

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


### `strip_info(file_path)`

Extracts raw pixel data offset (address) and total byte count from TIFF strip tags.

| Field | Meaning |
|---|---|
| **Strip Offset** | Byte position where the raw image data starts |
| **Raw Pixel Data** | Total size of the image payload in bytes |

**Example**

```python
import organoid_identifier as oi

oi.strip_info("example_data/Overlay_BK52_WT_BGR.tif")
```