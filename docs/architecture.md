# How it works (architecture)

This page explains the internals of `organoid-identifier` — in plain words.

---

## The big idea

Reading a whole TIFF image takes time and memory. But **you usually don't need
the pixels** — you need the *description* of the file: size, bit depth, colors,
compression. That description lives in a few small places at the **beginning of
the file**, and reading it is nearly instant.

`organoid-identifier` reads only that metadata, using a tiny C engine. That is
why it is so fast and uses almost no memory.

---

## Anatomy of a TIFF file

A TIFF file is made of three parts:

```
┌─────────────────────────────────────────┐
│ 1. HEADER   (8 bytes)                   │
│    • Byte order (II or MM)              │
│    • Magic number (always 42)           │
│    • Offset to the IFD table            │
├─────────────────────────────────────────┤
│ 2. IFD  (Image File Directory)          │
│    A table of numbered tags:            │
│    • Tag 256 → width                    │
│    • Tag 257 → height                   │
│    • Tag 258 → bits per sample          │
│    • Tag 259 → compression              │
│    • Tag 262 → photometric (color)      │
│    • Tag 277 → samples per pixel        │
├─────────────────────────────────────────┤
│ 3. PIXEL DATA  (the heavy part)         │
│    ─ we never touch this!               │
└─────────────────────────────────────────┘
```

Our library reads part 1 and part 2, and **skips part 3** completely. That is
the entire secret.

---

## The modular C engine components

The engine is split into six small, isolated C components — one per job:

| Module | What it reads | Exposed as |
|---|---|---|
| `tiff_header` | Byte order, magic number, IFD offset — and validates the file | `inspect()` / `read_header()` |
| `tiff_tags` | Width, height, bits per sample | `dimensions()` / `read_tags()` |
| `tiff_format` | Photometric interpretation, samples per pixel, compression | `format_info()` / `read_format()` |
| `tiff_strip` | Strip offsets and byte counts | `strip_info()` / `read_strip()` |
| `tiff_bits` | Bit depth per sample and numeric sample format | `bits_info()` / `read_bits()` |
| `tiff_pixels` | Raw pixel payload extraction from strip addresses | `read_pixels()` / `show()` |

Each module is completely independent. If one needs to be changed or extended
in the future, the others are not affected.

```
┌────────────┐   ┌────────────┐   ┌────────────┐
│ tiff_header│──▶│ tiff_tags  │──▶│ tiff_format│
└────────────┘   └────────────┘   └────────────┘
      │                 │                │
      ▼                 ▼                ▼
┌────────────┐   ┌────────────┐   ┌────────────┐
│ tiff_strip │──▶│ tiff_bits  │──▶│ tiff_pixels│
└────────────┘   └────────────┘   └────────────┘
```

The Python layer (`python/organoid_identifier/`) is a thin wrapper: it passes
your file path to the C code and prints (or returns) the formatted report.

---

## Why is it fast?

- **O(1) time metadata reading** — it reads a fixed number of bytes at a known file offset. A 1 MB and a 10 GB TIFF take the same time for inspection.
- **O(1) memory footprint** — no heavy pixel data is loaded during inspection.
- **Native C speed** — no interpreter overhead in the hot path; compiled machine code executes directly.

---

## Why zero dependencies?

Everything is implemented with:

- the **C standard library** (for the engine), and
- the **Python standard library** (for the package wrapper and native BMP viewer).

No numpy, no Pillow, no OpenCV, no scipy. This means:

- `pip install` succeeds even in restricted environments
- no version conflicts with your other scientific packages
- the library is small and fast to install

---

## How it is built

The C code is compiled into a Python extension (`_core`), and shipped as
pre-built **binary wheels** for Linux, macOS and Windows. When you
`pip install organoid-identifier`, you get the compiled library directly —
you never need a C compiler on your machine.

Source layout:

```
src/
└── tiff_reader/
    ├── tiff_header/
    │   ├── tiff_header.c          # C logic (validation, header parsing)
    │   ├── tiff_header.h          # C header
    │   └── tiff_header_bridge.c      # Python ⇄ C bridge
    ├── tiff_tags/                 # Width, height, bit depth
    ├── tiff_format/               # Color space, channels, compression
    ├── tiff_strip/                # Strip offset and payload size
    ├── tiff_bits/                 # Bit depth & data format representation
    └── tiff_pixels/               # Raw pixel data buffer extraction
```

Each component is a `(logic + header + bridge)` triple, which keeps the Python
interface and the C implementation neatly separated.
