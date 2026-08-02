import sys
from setuptools import setup, Extension, find_packages

# İşletim sistemine göre derleme parametrelerini ayarla
extra_compile_args = []
if sys.platform == "win32":
    extra_compile_args = ["/O2", "/std:c11"] # Windows MSVC için derleme bayrakları
else:
    extra_compile_args = ["-O3", "-std=c99"] # Linux ve macOS için derleme bayrakları

c_module = Extension(
    'organoid_identifier._core',
    sources=[
        'src/main_bridge.c',
        'src/tiff_reader/tiff_header/tiff_header.c',
        'src/tiff_reader/tiff_header/tiff_header_bridge.c',
        'src/tiff_reader/tiff_tags/tiff_tags.c',
        'src/tiff_reader/tiff_tags/tiff_tags_bridge.c',
        'src/tiff_reader/tiff_format/tiff_format.c',
        'src/tiff_reader/tiff_format/tiff_format_bridge.c'
    ],
    include_dirs=[
        'src',
        'src/tiff_reader/tiff_header',
        'src/tiff_reader/tiff_tags',
        'src/tiff_reader/tiff_format'
    ],
    extra_compile_args=extra_compile_args
)

setup(
    name='organoid_identifier',
    version='0.4.4',
    description='High-performance zero-dependency C-powered Organoid Identifier',
    long_description=open('README.md', encoding='utf-8').read(),
    long_description_content_type='text/markdown',
    author='Murat Emre Öztürk',
    packages=find_packages(where='python'),
    package_dir={'': 'python'},
    ext_modules=[c_module],
    python_requires='>=3.8',
    classifiers=[
        'Development Status :: 4 - Beta',
        'Intended Audience :: Science/Research',
        'Topic :: Scientific/Engineering :: Bio-Informatics',
        'Topic :: Scientific/Engineering :: Image Processing',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: C',
        'Programming Language :: Python :: 3',
        'Operating System :: POSIX :: Linux',
        'Operating System :: Microsoft :: Windows',
        'Operating System :: MacOS',
    ],
)