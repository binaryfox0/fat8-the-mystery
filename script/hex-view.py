import argparse
import sys

def bytes_to_str(data: bytes) -> str:
    return ' '.join(f'{b:02X}' for b in data)

def bytes_to_ascii(data: bytes) -> str:
    return ''.join(chr(b) if 0x20 <= b <= 0x7E else '.' for b in data)

def format_hex_line(offset: int, data: bytes, width: int = 16) -> str:
    hex_part = bytes_to_str(data)
    padding = ' ' * ((width - len(data)) * 3 + 3)
    ascii_part = bytes_to_ascii(data)
    return f"{offset:08X} {hex_part}{padding}{ascii_part}"


def read_and_display_hex(
    filename: str,
    start_offset: int,
    length: int,
    line_width: int = 16
) -> None:
    try:
        with open(filename, "rb") as f:
            file_size = f.seek(0, 2)
            if start_offset < 0 or start_offset >= file_size:
                print(f"Error: offset {start_offset} is out of file bounds (0 to {file_size-1}).")
                sys.exit(1)
            f.seek(start_offset)

            bytes_remaining = min(length, file_size - start_offset)
            offset = start_offset

            while bytes_remaining > 0:
                chunk_size = min(line_width, bytes_remaining)
                chunk = f.read(chunk_size)
                print(format_hex_line(offset, chunk, line_width))
                offset += chunk_size
                bytes_remaining -= chunk_size

    except FileNotFoundError:
        print(f"File not found: {filename}")
        sys.exit(1)
    except OSError as e:
        print(f"Error reading file: {e}")
        sys.exit(1)

def main() -> None:
    parser = argparse.ArgumentParser(description="Hex view for markdown with LaTeX-safe output")
    parser.add_argument("file", type=str, help="Input binary file")
    parser.add_argument("offset", type=lambda x: int(x, 0), help="Start offset in bytes")
    parser.add_argument("length", type=lambda x: int(x, 0), help="Number of bytes to display")
    args = parser.parse_args()

    if args.length <= 0:
        print("Length must be positive.")
        sys.exit(1)

    read_and_display_hex(args.file, args.offset, args.length)

if __name__ == "__main__":
    main()
