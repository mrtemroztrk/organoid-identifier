# Organoid Identifier 

[![PyPI version](https://img.shields.io/badge/version-0.4.0-blue.svg)](https://test.pypi.org/project/organoid-identifier/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Python Version](https://img.shields.io/badge/python-3.8%2B-brightgreen.svg)](https://www.python.org/)
[![Build Status](https://img.shields.io/badge/build-passing-success.svg)](#)

A high-performance, zero-dependency Python library powered by pure C extensions for instant microscopy image inspection and organoid identifier analysis.

---

##  Features

-  **Ultra Fast:** Core metadata extraction runs natively in pure C at memory level $O(1)$.
-  **Zero Dependencies:** Pure C + Standard Python library. No heavy third-party bloat.
-  **Cross-Platform:** Pre-compiled standalone binary wheels (`manylinux`, `macOS`, `Windows`).
-  **Modular Architecture:** Fully isolated C components ensuring stability and modular extensions.

---

##  Installation

Install the pre-compiled binary package directly via `pip`:

```bash
pip install organoid-identifier
```

For more information, check out the GitHub Repo: https://github.com/mrtemroztrk/organoid-identifier


for beta testing, use:

```bash
pip install -i https://test.pypi.org/simple/ organoid-identifier==0.2.3 
```

