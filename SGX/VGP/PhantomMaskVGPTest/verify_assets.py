#!/usr/bin/env python3
"""Fail the build if PhantomMask textures are not the original PVRTC1 assets."""

from hashlib import sha256
from pathlib import Path
import struct

ROOT = Path(__file__).resolve().parent
ASSETS = {
    "MaskMain.pvr": {
        "width": 256,
        "height": 256,
        "mipmaps": 8,
        "size": 43828,
        "sha256": "ce2069c594ec7eda31285f91baad155104f9bbc8406e30d95916e0344fb4c49b",
    },
    "RoomStill.pvr": {
        "width": 512,
        "height": 512,
        "mipmaps": 9,
        "size": 174900,
        "sha256": "d4d32026d7dc1f41177b20bd2b2c26870a4029afff6e6736bed39d6e7802e694",
    },
}
PVR_V2_HEADER_BYTES = 52
PVR_V2_TAG = 0x21525650
PVR_OGL_PVRTC4 = 0x19  # PVRTC1 4bpp, not the PVRTC2 formats.
VGP_BINARY_SHA256 = "486203809ee3e42e90c37db9ebb06d950d27a8e13e1b11eef33a5fccb0ce1d40"


def level_bytes(width: int, height: int) -> int:
    return max(32, max(width, 8) * max(height, 8) // 2)


def verify(name: str, expected: dict[str, object]) -> None:
    data = (ROOT / "assets" / name).read_bytes()
    if len(data) != expected["size"]:
        raise SystemExit(f"{name}: unexpected size {len(data)}")
    fields = struct.unpack_from("<13I", data)
    header_bytes, height, width, mipmaps, flags, data_size, bpp = fields[:7]
    alpha_mask, pvr_tag, surfaces = fields[10], fields[11], fields[12]
    if (
        header_bytes != PVR_V2_HEADER_BYTES
        or width != expected["width"]
        or height != expected["height"]
        or mipmaps != expected["mipmaps"]
        or flags & 0xFF != PVR_OGL_PVRTC4
        or bpp != 4
        or alpha_mask != 0
        or pvr_tag != PVR_V2_TAG
        or surfaces != 1
    ):
        raise SystemExit(f"{name}: not the expected RGB PVRTC1 4bpp PVR v2 texture")
    total = 0
    w, h = width, height
    for _ in range(mipmaps + 1):
        total += level_bytes(w, h)
        w, h = max(1, w // 2), max(1, h // 2)
    if total != data_size or header_bytes + data_size != len(data):
        raise SystemExit(f"{name}: invalid PVRTC1 mip payload size")
    digest = sha256(data).hexdigest()
    if digest != expected["sha256"]:
        raise SystemExit(f"{name}: SHA-256 differs from the original SDK asset")
    print(f"{name}: PVRTC1 4bpp, {width}x{height}, {mipmaps + 1} mip levels, verified")


if __name__ == "__main__":
    for filename, metadata in ASSETS.items():
        verify(filename, metadata)
    vgp = (ROOT / "assets" / "PhantomMask_SH.bin").read_bytes()
    if len(vgp) != 1412 or not vgp.startswith(b"VGP ") or sha256(vgp).hexdigest() != VGP_BINARY_SHA256:
        raise SystemExit("PhantomMask_SH.bin: not the tracked compiled IMG VGP binary")
    print("PhantomMask_SH.bin: compiled VGP binary verified and embedded unchanged")
