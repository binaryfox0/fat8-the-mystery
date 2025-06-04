import sys
import re

def parse_imd_header(f):
    header = f.readline().decode('ascii', errors='replace').strip()
    match = re.match(r'^IMD\s+([\d.]+):\s+(\d{2}/\d{2}/\d{4})\s+(\d{2}:\d{2}:\d{2})$', header)
    if not match:
        raise ValueError("Invalid header version/date/time format.")    
    print(f"IMD header: version={match.group(1)}, date={match.group(2)}, time={match.group(3)}")

    comment = bytearray()
    while True:
        b = f.read(1)
        if not b:
            raise ValueError("Unexpected EOF before 0x1A")
        if b[0] == 0x1A:
            break
        comment.append(b[0])
    print("Comment:\n" + comment.decode("ascii", "replace"))

    geometry = {
        "cylinders": set(),
        "heads": set(),
        "max_sector_count": 0,
        "max_sector_size": 0,
        "modes": set(),
    }

    while True:
        try:
            parse_track_header(f, geometry)
        except EOFError:
            break

    summarize_geometry(geometry)


def parse_track_header(f, geometry):
    header = f.read(5)
    if len(header) < 5:
        raise EOFError()

    _mode, cylinder, head, sectors_count, _sectors_size = header

    if _mode >= 6:
        raise ValueError("Invalid mode field.")
    if _sectors_size >= 7:
        raise ValueError("Invalid sector size field.")

    geometry["cylinders"].add(cylinder)
    geometry["heads"].add(head & 1)
    geometry["max_sector_count"] = max(geometry["max_sector_count"], sectors_count)
    geometry["max_sector_size"] = max(geometry["max_sector_size"], 128 << _sectors_size)
    geometry["modes"].add(_mode)

    # Sector numbering map
    sector_numbering_map = f.read(sectors_count)
    if len(sector_numbering_map) < sectors_count:
        raise ValueError("Unexpected EOF in sector number map")

    # Optional maps
    if head & 0x80:
        f.read(sectors_count)
    if head & 0x40:
        f.read(sectors_count)

    for _ in range(sectors_count):
        dtype = f.read(1)
        if not dtype:
            raise ValueError("Unexpected EOF in data type")
        dtype = dtype[0]

        if dtype in (0x01, 0x03, 0x05, 0x07):  # normal data
            f.read(128 << _sectors_size)
        elif dtype in (0x02, 0x04, 0x06, 0x08):  # compressed
            f.read(1)
        # 0x00 = missing (no data)


def summarize_geometry(geometry):
    print("\n=== Disk Geometry ===")
    print(f"Cylinders (Tracks): {len(geometry['cylinders'])}")
    print(f"Heads (Sides): {len(geometry['heads'])}")
    print(f"Max Sectors per Track: {geometry['max_sector_count']}")
    print(f"Max Sector Size: {geometry['max_sector_size']} bytes")

    mode_names = {
        0: "500 kbps FM",
        1: "300 kbps FM",
        2: "250 kbps FM",
        3: "500 kbps MFM",
        4: "300 kbps MFM",
        5: "250 kbps MFM"
    }
    print("Recording Modes Used:")
    for m in sorted(geometry["modes"]):
        print(f"  - Mode {m}: {mode_names.get(m, 'Unknown')}")


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print(f"Usage: python {sys.argv[0]} <file>")
        sys.exit(1)
    try:
        with open(sys.argv[1], "rb") as f:
            parse_imd_header(f)
    except ValueError as e:
        print(f"Error: {e}")
        sys.exit(1)
    except FileNotFoundError:
        print(f"File not found: {sys.argv[1]}")
        sys.exit(1)
    except OSError as e:
        print(f"Error reading file: {e}")
        sys.exit(1)
