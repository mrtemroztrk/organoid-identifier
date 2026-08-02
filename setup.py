from setuptools import setup, Extension, find_packages

c_module = Extension(
    'organoid_identifier._core',
    sources=[
        'src/main_bridge.c',
        'src/tiff_reader/tiff_header/tiff_header.c',
        'src/tiff_reader/tiff_header/tiff_header_bridge.c',
        'src/tiff_reader/tiff_tags/tiff_tags.c',
        'src/tiff_reader/tiff_tags/tiff_tags_bridge.c',
        'src/tiff_reader/tiff_format/tiff_format.c',        # YENİ
        'src/tiff_reader/tiff_format/tiff_format_bridge.c' # YENİ
    ],
    include_dirs=[
        'src',
        'src/tiff_reader/tiff_header',
        'src/tiff_reader/tiff_tags',
        'src/tiff_reader/tiff_format'                     # YENİ
    ],
    extra_compile_args=['-O3', '-std=c99']
)

setup(
    name='organoid_identifier',
    version='0.4.1', # Yeni minör sürüm
    description='High-performance zero-dependency C-powered Organoid Identifier',
    long_description=open('README.md').read(),
    long_description_content_type='text/markdown',
    author='Murat Emre Öztürk',
    packages=find_packages(where='python'),
    package_dir={'': 'python'},
    ext_modules=[c_module],
    python_requires='>=3.8',
)