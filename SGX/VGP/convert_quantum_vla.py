#!/usr/bin/env python3
"""Convert a Quantum mode-9/13 .vla stream to RGB OBJ vertex colours.

A Quantum .vla file is a headerless byte stream. It contains per-corner light
values in mesh/polygon order, but no mesh names, face indices, or mode marker.
Consequently this converter requires an OBJ exported with the exact same polygon
order and triangle/quad arity, plus an explicit --mode 9 or --mode 13.
"""
from __future__ import annotations

import argparse
import sys
from pathlib import Path
from typing import Optional

from bake_vertex_lighting import read_obj, write_baked_obj


def decode_light_byte(data: bytes, offset: int) -> tuple[int, int]:
    if offset >= len(data):
        raise ValueError(f"truncated VLA data at byte {offset}")
    value = data[offset]
    signed = value if value < 128 else value - 256
    return signed + 128, offset + 1


def decode_vla(data: bytes, face_sizes: list[int], mode: int) -> list[list[tuple[float, float, float]]]:
    if mode not in (9, 13):
        raise ValueError("Quantum VLA mode must be 9 or 13")
    colors: list[list[tuple[float, float, float]]] = []
    offset = 0
    for face_index, count in enumerate(face_sizes):
        if count not in (3, 4):
            raise ValueError(f"OBJ face {face_index + 1} has {count} corners; VLA stores only triangles/quads")
        red = []
        for _ in range(count):
            value, offset = decode_light_byte(data, offset)
            red.append(value)
        if mode == 9:
            rgb = [(v / 255.0, v / 255.0, v / 255.0) for v in red]
        else:
            if offset >= len(data):
                raise ValueError(f"truncated VLA data before colored flag for face {face_index + 1}")
            colored = data[offset]
            offset += 1
            if colored not in (0, 1):
                raise ValueError(f"invalid VLA colored flag {colored} on face {face_index + 1}")
            if colored:
                green, blue = [], []
                for _ in range(count):
                    value, offset = decode_light_byte(data, offset)
                    green.append(value)
                for _ in range(count):
                    value, offset = decode_light_byte(data, offset)
                    blue.append(value)
                rgb = [(red[i] / 255.0, green[i] / 255.0, blue[i] / 255.0)
                       for i in range(count)]
            else:
                rgb = [(v / 255.0, v / 255.0, v / 255.0) for v in red]
        colors.append(rgb)
    if offset != len(data):
        raise ValueError(f"{len(data) - offset} trailing VLA byte(s); OBJ topology/order or mode likely does not match")
    return colors


def main(argv: Optional[list[str]] = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("obj", type=Path, help="matching Quantum mesh exported as OBJ")
    parser.add_argument("vla", type=Path, help="raw Quantum light data file")
    parser.add_argument("output", type=Path, help="OBJ output with VLA RGB in vertex colors")
    parser.add_argument("--mode", type=int, choices=(9, 13), required=True,
                        help="Quantum drawing mode used to produce the VLA file")
    args = parser.parse_args(argv)
    try:
        positions, _normals, faces, _libraries = read_obj(args.obj)
        colors = decode_vla(args.vla.read_bytes(), [len(face.corners) for face in faces], args.mode)
        write_baked_obj(args.obj, args.output, positions, faces, colors)
    except (OSError, ValueError, IndexError) as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 2
    print(f"Converted Quantum mode {args.mode} VLA colors to {args.output}", file=sys.stderr)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
