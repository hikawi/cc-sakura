# This is a tool-file, meant to transform the provided level text file
# into a .map file for easier loading in game.

import os
import sys
import re
import struct
from math import isnan
from typing import IO

TOOL_VERSION = 1

def split_coords(coord):
    m = re.match(r"(\d+),(\d+)", coord)
    return (int(m[1]), int(m[2])) if m else (-1, -1)

def partition(lines: list[str]):
    map = {}
    groups = []
    tag = ""
    cur = []

    for line in lines:
        if line == "":
            if len(cur) > 0:
                groups.append(cur)
                cur = []
        else:
            cur.append(line)

    if len(cur) > 0:
        groups.append(cur)

    for group in groups:
        if len(group) == 0:
            print(f"Syntax error parsing {group}")
            sys.exit(1)

        matcher = re.findall(r"\[(.+)\]", group[0])
        if len(matcher) == 0:
            print(f"Syntax error parsing {group[0]}. A header must be in the form [name]")
            sys.exit(1)

        tag = matcher[0]
        for node in group[1:]:
            matcher = re.match(r"(.+)=(.+)", node)
            if matcher is None:
                print(f"Syntax error parsing {node}. A node must be in the form key=value")
                sys.exit(1)
            map['.'.join([tag, matcher[1]])] = matcher[2]
    return map

def write_coords(coord: str, f: IO):
    x, y = split_coords(coord)
    if x >= 0 and y >= 0:
        print(f"Writing coords {x}, {y}")
        f.write(struct.pack("<I", x))
        f.write(struct.pack("<I", y))

def parse_meta_region(map: dict, f: IO):
    name = map["meta.name"]

    if not name:
        print("name missing in meta region")
        sys.exit(1)

    print(f"Writing map {name}")
    f.write(struct.pack("<I", len(name)))
    f.write(struct.pack(f"{len(name)}s", bytearray(name, 'utf-8')))

def parse_map_region(map: dict, f: IO):
    # Get out the coords
    coords_list = []
    max = (0, 0)
    for k, v in map.items():
        if not k.startswith("map."):
            continue

        coords = k[4:]
        x, y = split_coords(coords)

        if x < 0 or y < 0 or isnan(int(v)):
            print(f"Invalid syntax at {k}={v}")
            sys.exit(1)

        coords_list.append((x, y, int(v)))
        max_x, max_y = max
        if x > max_x:
            max_x = x
        if y > max_y:
            max_y = y
        max = (max_x, max_y)

    # Write the max size of the grid first
    f.write(struct.pack("<I", max[0] + 1))
    f.write(struct.pack("<I", max[1] + 1))

    # Then write the length of coords.
    f.write(struct.pack("<I", len(coords_list)))

    # Then write the coords
    for x, y, v in coords_list:
        f.write(struct.pack("<I", x))
        f.write(struct.pack("<I", y))
        f.write(struct.pack("<I", v))
    print(f"Written {len(coords_list)} coords")

def main():
    if len(sys.argv) < 2:
        print("Not enough arguments. Use python map_to_bin.py levelfile")
        return

    try:
        with open(sys.argv[1], 'r') as f:
            lines = [l.strip() for l in f.readlines() if not l.startswith("#")]
            sprname, _ = os.path.splitext(sys.argv[1])

        # Open a new file for writing at the same path of the provided sheet
        outpath = f"{sprname}.map"
        print(outpath)
        with open(outpath, 'wb') as f:
            groups = partition(lines)
            f.write(struct.pack("<I", TOOL_VERSION))
            print(f"Using v{TOOL_VERSION} of map_to_bin.")
            parse_meta_region(groups, f)
            parse_map_region(groups, f)
    except OSError:
        print("Can't open provided files.")

if __name__ == "__main__":
    main()
