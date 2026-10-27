# PhantomMask VGP / GLES 1.1 test SIS

This is a small Symbian test application, not a port of the full PhantomMask demo. It requests an OpenGL ES 1.1 context, loads the precompiled IMG VGP SH vertex program embedded in the PowerVR SDK's `OGLESPhantomMask` demo, supplies a minimal set of spherical-harmonic environment parameters, and draws one triangle.

The VGP program is embedded in `vgp_program.h` (1328 bytes; `VGP ` binary signature). It was extracted from the `vgp_SHL_VGP` object in the SDK 4.03 `OGLES/SDKPackage/Binaries/CommonRaw/Demos/PhantomMask/OGLESPhantomMask` ELF. This uses a known compiled SDK program; it does not compile `SGX/VGP/PhantomMask_SH.vp` or claim that the shader's original assets/game renderer have been ported.

On a device with `GL_IMG_vertex_program` and the compatible VGP binary format, the app uses the SH VGP program and shows the triangle on a blue-black background. If the extension or procedure lookup is unavailable, it falls back to the GLES 1.1 fixed-function pipeline and shows a red-black background. Escape or the close request exits.

## Build

The `symbian-platform` 0.3.0 SDK archive used for this test was obtained from the user's Google Drive. Its SHA-256 matches the upstream release asset exactly: `9be830884a5fd1b9315ea1c11571810f7a50fa673851fa392bb397c6759fec3d`.

The GitHub Actions workflow downloads that exact archive, verifies the SHA-256, builds the ARMv6 E32 executable, creates an unsigned SIS with application-menu registration, and uploads the SIS plus the package report as a workflow artifact. The SIS is an experimental SDK output: neither Symbian-loader acceptance nor execution on a physical VGP device is verified by compilation or packaging.
