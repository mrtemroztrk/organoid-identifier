# Organoid Identifier

[![PyPI version](https://img.shields.io/badge/version-0.4.6-blue.svg)](https://test.pypi.org/project/organoid-identifier/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Python Version](https://img.shields.io/badge/python-3.8%2B-brightgreen.svg)](https://www.python.org/)
[![Build Status](https://img.shields.io/badge/build-passing-success.svg)](#)

**Organoid Identifier** is a fast and simple Python library that reads and inspects
microscopy **TIFF files** (like organoid images) without installing any heavy
dependencies. The heavy lifting is done by a tiny, pure **C** engine, so it works
instantly on any computer.

> 🧠 **In short:** you give it a TIFF file, it tells you everything about the file —
> size, dimensions, color space, compression — in a nice readable report.

---

## Why use it?

| Problem | Solution |
|---|---|
| "Which TIFF format is this file?" | `inspect()` tells you in milliseconds |
| "How big is this image?" | `dimensions()` shows width, height, bit depth |
| "Is it RGB? Compressed? How many channels?" | `format_info()` answers all of these |

- ⚡ **Fast** — metadata is read natively in C, no image decoding, results in microseconds
- 🧹 **Zero dependencies** — pure C + standard Python library. Nothing else to install
- 🖥️ **Cross-platform** — works on Linux, macOS and Windows
- 📦 **No compilation needed** — pre-built binary wheels are installed by `pip`

---

## Installation

You only need **Python 3.8 or newer**.

### Option 1: From PyPI (recommended)

```bash
pip install organoid-identifier
```

### Option 2: Beta testing

```bash
pip install -i https://test.pypi.org/simple/ organoid-identifier==0.4.6
```

### Option 3: Build from source

```bash
git clone https://github.com/mrtemroztrk/organoid-identifier
cd organoid-identifier
pip install .
```

> A C compiler is only needed for the *source build*. The normal `pip install`
> gives you a ready-to-use binary — no compiler required.

---

## Quick start (30 seconds)

```python
import organoid_identifier as oi

# Pick any TIFF file on your computer
oi.inspect("my_image.tif")
```

That's it! You will see a report like this:

```
┌───────────────────────────────────────────────────────┐
│              TIFF FILE INSPECTION REPORT              │
├───────────────────────────────────────────────────────┤
│  • File Path         : my_image.tif                   │
│  • Status            : VALID TIFF                     │
│  • Byte Order        : Little-Endian (II)             │
│  • Magic Number      : 42                             │
│  • First IFD Offset  : 11297914                 bytes │
└───────────────────────────────────────────────────────┘
```

---

## Try it with the sample data

This repository includes **6 real microscopy TIFF files** in the
[`example_data/`](example_data/) folder. They are organoid overlay images, so you
can test the library immediately without hunting for files.

```python
import organoid_identifier as oi

sample = "example_data/Overlay_BK52_WT_BGR.tif"

oi.inspect(sample)       # file-level info
oi.dimensions(sample)    # width, height, bit depth
oi.format_info(sample)   # channels, color space, compression
```

Real output from the sample data:

```
┌───────────────────────────────────────────────────────┐
│               TIFF IMAGE DIMENSIONS REPORT            │
├───────────────────────────────────────────────────────┤
│  • Width  (Pixels)   : 2323                           │
│  • Height (Pixels)   : 2253                           │
│  • Bits Per Sample   : 8                              │
└───────────────────────────────────────────────────────┘

┌───────────────────────────────────────────────────────┐
│               TIFF FORMAT & COLOR REPORT              │
├───────────────────────────────────────────────────────┤
│  • Channels (Samples): 3                              │
│  • Color Space       : RGB Color                      │
│  • Compression       : LZW Compression                │
└───────────────────────────────────────────────────────┘
```

> 📚 Want the full details on the sample files and what they mean?
> See [Sample Data Guide](docs/sample-data.md).

---

## API at a glance

| Function | What it does | Returns |
|---|---|---|
| `oi.inspect(path)` | TIFF file validation + header info | prints report |
| `oi.dimensions(path)` | Width, height, bits per sample | prints report |
| `oi.format_info(path)` | Channels, color space, compression | prints report |
| `oi.strip_info(path)` | Strip offset and raw pixel payload size | prints report |
| `oi.bits_info(path)` | Bit depth and data format type | prints report |
| `oi.show(path)` | Native OS window viewer for organoid images | opens window |
| `oi.read_header(path)` | Same as `inspect`, but returns report as a string | `str` |
| `oi.read_tags(path)` | Same as `dimensions`, but returns report as a string | `str` |
| `oi.read_format(path)` | Same as `format_info`, but returns report as a string | `str` |
| `oi.read_strip(path)` | Same as `strip_info`, but returns report as a string | `str` |
| `oi.read_bits(path)` | Same as `bits_info`, but returns report as a string | `str` |
| `oi.read_pixels(path)` | Extracts raw pixel payload and dimensions | `dict` |

The `read_*` variants are useful if you want to save or process the report
yourself instead of printing it.

---

## How it works (no math, promise)

A TIFF file starts with a small **header** (byte order + magic number), followed by
an **IFD** — a table of **tags** that describe the image (width, height, color, …).
Organoid Identifier reads this metadata instantly with its native C engine. See [Architecture](docs/architecture.md) for details.

---

## Documentation

Full documentation is available at: **https://mrtemroztrk.github.io/organoid-identifier**

- [Getting Started](docs/index.md)
- [API Reference](docs/api.md)
- [Sample Data Guide](docs/sample-data.md)
- [Architecture](docs/architecture.md)

---

## Project structure

```
organoid-identifier/
├── python/organoid_identifier/   # Python package (public API)
├── src/                          # Pure C engine (TIFF reader)
│   └── tiff_reader/
│       ├── tiff_header/          # Header & validation
│       ├── tiff_tags/            # Width / height / bit depth
│       ├── tiff_format/          # Color space / channels / compression
│       ├── tiff_strip/           # Strip offsets & byte counts
│       ├── tiff_bits/            # Bit depth & data representation
│       └── tiff_pixels/          # Raw pixel extraction
├── example_data/                 # 6 sample organoid TIFF images
├── tests/                        # Usage examples & tests
└── docs/                         # Documentation
```

---

## License

Distributed under the MIT License.

Developed with ❤️ by Murat Emre Öztürk.
