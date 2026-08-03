import sys
from setuptools import setup, Extension, find_packages

extra_compile_args = []
if sys.platform == "win32":
    extra_compile_args = ["/O2", "/std:c11"]
else:
    extra_compile_args = ["-O3", "-std=c99"]

c_module = Extension(
    'organoid_identifier._core',
    sources=[
        'src/main_bridge.c',
        'src/tiff_reader/tiff_header/tiff_header.c',
        'src/tiff_reader/tiff_header/tiff_header_bridge.c',
        'src/tiff_reader/tiff_tags/tiff_tags.c',
        'src/tiff_reader/tiff_tags/tiff_tags_bridge.c',
        'src/tiff_reader/tiff_format/tiff_format.c',
        'src/tiff_reader/tiff_format/tiff_format_bridge.c',
        'src/tiff_reader/tiff_strip/tiff_strip.c',        
        'src/tiff_reader/tiff_strip/tiff_strip_bridge.c',
        'src/tiff_reader/tiff_bits/tiff_bits.c',
        'src/tiff_reader/tiff_bits/tiff_bits_bridge.c',
        'src/tiff_reader/tiff_pixels/tiff_pixels.c',
        'src/tiff_reader/tiff_pixels/tiff_pixels_bridge.c',
        'src/tiff_analyzer/organoid_metrics/organoid_metrics.c',
        'src/tiff_analyzer/organoid_metrics/organoid_metrics_bridge.c',
        'src/tiff_analyzer/organoid_segmenter/organoid_segmenter.c',
        'src/tiff_analyzer/organoid_segmenter/organoid_segmenter_bridge.c'
    ],
    include_dirs=[
        'src',
        'src/tiff_reader/tiff_header',
        'src/tiff_reader/tiff_tags',
        'src/tiff_reader/tiff_format',
        'src/tiff_reader/tiff_strip',
        'src/tiff_reader/tiff_bits',
        'src/tiff_reader/tiff_pixels',
        'src/tiff_analyzer/organoid_metrics',
        'src/tiff_analyzer/organoid_segmenter'
    ],
    extra_compile_args=extra_compile_args
)

setup(
    name='organoid_identifier',
    version='0.4.10',
    description='High-performance zero-dependency C-powered Organoid Identifier',
    long_description=open('README.md', encoding='utf-8').read(),
    long_description_content_type='text/markdown',
    author='Murat Emre Öztürk',
    packages=find_packages(where='python'),
    package_dir={'': 'python'},
    ext_modules=[c_module],
    python_requires='>=3.8',
)