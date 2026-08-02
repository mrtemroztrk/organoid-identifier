from ._core import read_header, read_tags

def inspect(file_path: str) -> None:
    """Prints formatted TIFF header report."""
    print(read_header(file_path))

def dimensions(file_path: str) -> None:
    """Prints formatted TIFF image dimensions & tags report."""
    print(read_tags(file_path))

__all__ = ["inspect", "dimensions", "read_header", "read_tags"]