# PVRTC1 Converter for Symbian

Native ARMv6 utility for converting between PVRTC1 2bpp/4bpp (`.pvr`) and PNG/BMP. It reads PVR v2 and v3 files and writes PVR v3. The decoder uses the PowerVR SDK's `PVRTDecompress.cpp`; the encoder is a new portable CPU implementation that writes PVRTC1 block data without relying on the host-only `pvrtc.a`/`PVRTexLib` binaries.

## On-device use

Copy a file to `C:\Data\PVRTCConverter\` and launch the app:

- Tap **ENCODE**, choose **PVRTC 2 BPP** or **PVRTC 4 BPP**, choose the preferred PNG/BMP input, then tap **RUN**. The app reads `input.png` or `input.bmp` (falling back to the other format if needed) and writes `output.pvr`.
- Tap **DECODE**, choose PNG or BMP output, then tap **RUN**. The app reads `input.pvr` and writes `output.png` or `output.bmp`.
- **CLOSE** exits the utility. On keypad-only devices, keys `1`–`5` select Encode, Decode, rate, image type, and Run; `0` or Escape exits.

PNG images use the SDK's portable libpng. BMP reading accepts uncompressed 24-bit and 32-bit images, including standard BGRA bitfields; BMP output is 32-bit BGRA with alpha.

The encoder edge-pads arbitrary input dimensions to the next PVRTC-compatible power-of-two dimensions and writes the original dimensions in a private PVR v3 metadata record. This converter uses that metadata to crop decoded images back to the source dimensions; other PVR readers may display the padded size. PVRTC1 requires minimum encoded dimensions of 8x8 for 4bpp and 16x8 for 2bpp.

The encoder prioritizes portability and bounded runtime over high-end compressor quality. PVRTC is lossy; inspect output on the target GPU, particularly for textures with sharp alpha edges. The input alpha channel is preserved as PVRTC RGBA data.

## Host tests

With a C++17 compiler and libpng development package installed:

```sh
cmake -S SGX/VGP/PVRTCConverter/tests -B /tmp/pvrtc-converter-tests
cmake --build /tmp/pvrtc-converter-tests
ctest --test-dir /tmp/pvrtc-converter-tests --output-on-failure
```

The host tests cover PNG/BMP round trips, PVR v2/v3 payload handling, and 2bpp/4bpp encode/decode error bounds.

## Build and package (SDK 0.3.0)

```sh
SDK_ROOT=/path/to/symbian-sdk/install
PROJECT="$SDK_ROOT/examples/PVRTCConverter"
mkdir -p "$PROJECT"
cp -a SGX/VGP/PVRTCConverter/. "$PROJECT/"
symbian app build --project "$PROJECT"
symbian package \
  --project "$PROJECT" \
  --artifact "$PROJECT/.symbian/build/pvrtc_converter.exe" \
  --output /tmp/pvrtc-converter-sis
```
