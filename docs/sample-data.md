# Sample Data

This repository includes **6 real microscopy images** in the [`example_data/`](https://github.com/mrtemroztrk/organoid-identifier/tree/main/example_data) folder.

They are **organoid overlay images** from the BK52 cell line — each file is a
fluorescence microscopy overlay where different color channels mark different
biological markers:

| File | What it is |
|---|---|
| `Overlay_BK52_WT_BGR.tif` | Organoid overlay — wild-type (WT) sample |
| `Overlay_BK52_CEA_BGR.tif` | Organoid overlay — CEA marker stained |
| `Overlay_BK52_CEA_9805_BGR.tif` | Organoid overlay — CEA + 9805 marker |
| `Overlay_BK52_BK_BGR.tif` | Organoid overlay — BK marker |
| `Overlay_BK52_WT_9805_BGR.tif` | Organoid overlay — WT + 9805 marker |
| `Overlay_BK52_9806_BGR.tif` | Organoid overlay — 9806 marker |

> **BGR** in the file names means the channels are stored in blue-green-red order
> (the common microscope capture order), which the TIFF reader detects correctly.

---

## Why are they useful?

They let you:

- **Try the library immediately** — no need to hunt for a TIFF file on the web
- **See realistic output** — these are true lab images with real TIFF metadata
- **Test your own scripts** — loop over them, filter by size, build reports

---

## How to use them

### Step 1 — Get the repository

```bash
git clone https://github.com/mrtemroztrk/organoid-identifier
cd organoid-identifier
```

### Step 2 — Run Python from the repository folder

The sample file paths in the examples below are relative to the repository root,
so run your script from there.

### Step 3 — Inspect the samples

**One file:**

```python
import organoid_identifier as oi

oi.inspect("example_data/Overlay_BK52_9806_BGR.tif")
oi.dimensions("example_data/Overlay_BK52_9806_BGR.tif")
oi.format_info("example_data/Overlay_BK52_9806_BGR.tif")
```

**All files at once:**

```python
import glob
import organoid_identifier as oi

for file in sorted(glob.glob("example_data/*.tif")):
    print(f"\n>>> {file}")
    oi.dimensions(file)
```

---

## What output should you expect?

The samples are 8-bit RGB TIFF images with LZW compression, so every file
produces the same kind of output — only the dimensions differ:

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

Here are the actual dimensions of every sample (measured by the library itself):

| File | Width | Height | Bits | Channels | Color | Compression |
|---|---|---|---|---|---|---|
| `Overlay_BK52_WT_BGR.tif` | 2323 | 2253 | 8 | 3 | RGB | LZW |
| `Overlay_BK52_CEA_BGR.tif` | 2323 | 2253 | 8 | 3 | RGB | LZW |
| `Overlay_BK52_CEA_9805_BGR.tif` | 2323 | 2253 | 8 | 3 | RGB | LZW |
| `Overlay_BK52_BK_BGR.tif` | 3004 | 2769 | 8 | 3 | RGB | LZW |
| `Overlay_BK52_WT_9805_BGR.tif` | 2322 | 2764 | 8 | 3 | RGB | LZW |
| `Overlay_BK52_9806_BGR.tif` | 2322 | 2763 | 8 | 3 | RGB | LZW |

---

## A small project idea

Build a simple inventory of all your microscopy images:

```python
import glob
import organoid_identifier as oi

results = []
for file in sorted(glob.glob("example_data/*.tif")):
    tags = oi.read_tags(file)        # get the dimensions report as text
    size = file.replace(".tif", "")  # file name without extension
    results.append((size, "OK"))

print(f"Checked {len(results)} files — all readable.")
```

> 💡 Remember: if a file is corrupt or not a TIFF, the library raises
> `ValueError`. Wrap the call in a `try/except` to keep going.
