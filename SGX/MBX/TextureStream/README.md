# IMG texture-stream demo for OGLES 1.1 / MBX

This is a PVRShell OGLES 1.1 port of the SDK's `gles1_texture_stream` sample. It discovers IMG stream device 0, queries its name and dimensions/format/buffer count, creates a `GL_TEXTURE_STREAM_IMG` texture, binds rotating device-buffer offsets with `glTexBindStreamIMG`, and draws the stream on a full-screen quad.

## Producer and device requirements

The demo is a consumer plus the SDK's sample test-pattern producer. It expects:

- A driver exposing `GL_IMG_texture_stream` and at least one stream device.
- The SDK `bc_example` buffer-class kernel driver loaded, with `/dev/bc_example` accessible to the user running the demo. The demo calls the driver's two public ioctls: get buffer count and fill buffer. The kernel driver generates the moving test pattern; the GL extension reads its buffers without a CPU texture upload.
- The buffer count reported by `bc_example` to match that of stream device 0, as in the SDK reference demo.

This is not a general video-file decoder. To use a camera/decoder instead, retain the texture-stream discovery/bind/draw path and replace the `bc_example` ioctl producer with the platform's capture/decoder queue. Texture-stream device names, formats, and offsets are driver-specific.

## Build and run

```sh
cd OGLES/Build/LinuxOMAP3
make Common=1 TOOLCHAIN=/path/to/arm-none-linux-gnueabi
```

Omit `TOOLCHAIN` if the SDK cross-compiler is on `PATH`. The project uses the SDK's Linux OMAP3 PVRShell backend as a reference build; for another MBX system, select its matching PVRShell/GLES backend. Run with access to `/dev/bc_example` and the matching kernel module. A successful C++ build alone cannot confirm that the runtime driver advertises `GL_IMG_texture_stream` or provides device 0.

The matching kernel buffer-class example and ioctl ABI are in `SGX/SDK_BIN/Graphics_SDK_setuplinux_4_03_00_02/GFX_Linux_KM/services4/3rdparty/bufferclass_example/`.
