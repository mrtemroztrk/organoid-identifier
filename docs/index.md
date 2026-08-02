# Welcome to Organoid Identifier

A fast, zero-dependency Python library that inspects **microscopy TIFF files** —
like organoid images — in a fraction of a second.

No numpy. No OpenCV. No compilation. Just a small pure-C engine and a few lines
of Python.

---

## What can you do with it?

Ask any TIFF file three simple questions:

1. **Is this file a valid TIFF?** → `inspect()`
2. **How big is the image?** → `dimensions()`
3. **What is the image format?** → `format_info()`

Each function answers instantly and prints a clean, readable report.

---

## Installation

Requires **Python 3.8+**. Then just:

```bash
pip install organoid-identifier
```

That's the whole installation. No extra packages are ever installed — the library
has **zero dependencies**.

---

## Quick start

```python
import organoid_identifier as oi

oi.inspect("path/to/your/image.tif")
```

And you get something like this:

```
┌───────────────────────────────────────────────────────┐
│              TIFF FILE INSPECTION REPORT              │
├───────────────────────────────────────────────────────┤
│  • File Path         : path/to/your/image.tif         │
│  • Status            : VALID TIFF                     │
│  • Byte Order        : Little-Endian (II)             │
│  • Magic Number      : 42                             │
│  • First IFD Offset  : 11297914                 bytes │
└───────────────────────────────────────────────────────┘
```

**All three functions:**

```python
import organoid_identifier as oi

file = "example_data/Overlay_BK52_WT_BGR.tif"

oi.inspect(file)      # 1. File info
oi.dimensions(file)   # 2. Image size
oi.format_info(file)  # 3. Color & compression
```

---

## Don't have a TIFF file? Use the sample data!

This repository ships with **6 real organoid microscopy images** in the
[`example_data/`](https://github.com/mrtemroztrk/organoid-identifier/tree/main/example_data)
folder — so you can try everything right away.

```bash
git clone https://github.com/mrtemroztrk/organoid-identifier
cd organoid-identifier
python your_script.py   # run from this folder so the paths match
```

Then explore every sample with one loop:

```python
import glob
import organoid_identifier as oi

for file in sorted(glob.glob("example_data/*.tif")):
    print(f"\n=== {file} ===")
    oi.dimensions(file)
```

📖 The complete guide to the sample files (what they are, what output to expect)
is on the [Sample Data](sample-data.md) page.

---

## What's next?

| Page | What you'll find |
|---|---|
| [API Reference](api.md) | Every function, full explanation, real outputs |
| [Sample Data](sample-data.md) | The included example images, explained |
| [Architecture](architecture.md) | How the C engine works under the hood |
