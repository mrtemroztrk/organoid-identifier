from ._core import read_header, read_tags, read_format, read_strip

def inspect(file_path: str) -> None:
    print(read_header(file_path))

def dimensions(file_path: str) -> None:
    print(read_tags(file_path))

def format_info(file_path: str) -> None:
    print(read_format(file_path))

def strip_info(file_path: str) -> None:
    print(read_strip(file_path))

__all__ = [
    "inspect", 
    "dimensions", 
    "format_info", 
    "strip_info", 
    "read_header", 
    "read_tags", 
    "read_format", 
    "read_strip"
]