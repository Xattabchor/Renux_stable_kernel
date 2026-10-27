# PhantomMask — Symbian GLES 1.1 / VGP port

This project packages the actual PhantomMask scene, not a geometry smoke test. The app renders the animated mask mesh from the PowerVR POD scene, draws the original room panorama behind it, applies the original nine-term SH coefficients through the compiled IMG VGP program, and runs the 451-frame scene animation at 30 fps. It requests an OpenGL ES 1.1 context.

`phantom_mask_scene_data.h` contains the mask mesh (2238 vertices, 4480 triangles), camera data, and per-frame model transforms generated from the SDK PhantomMask POD scene. `assets/MaskMain.pvr` and `assets/RoomStill.pvr` are the original PVR v2 texture files. Both were checked against the original files embedded in the OGLES1.1 SDK executable: `flags & 0xff == 0x19` (`OGL_PVRTC4`, PVRTC1 4 bpp), with their original mip chains and SHA-256 values. The build runs `verify_assets.py`; texture data is embedded unchanged and is not transcoded to PVRTC2.

The vertex shader is loaded from the repository's already-compiled `SGX/VGP/build/PhantomMask_SH.bin`, copied unchanged to `assets/PhantomMask_SH.bin` and embedded in the executable. Its `VGP ` signature, 1412-byte size, and SHA-256 are verified before compiling.

The SIS is built with `symbian-platform` 0.3.0 using the SDK archive obtained from Google Drive. The archive SHA-256 is verified against the release asset before the build. The output is unsigned and marked as an experimental E32 executable by the SDK; successful packaging does not establish Symbian loader acceptance or runtime compatibility. Physical-device VGP rendering still needs testing.
