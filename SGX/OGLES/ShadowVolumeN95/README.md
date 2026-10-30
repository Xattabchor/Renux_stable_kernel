# Stencil Shadow Volumes for Symbian / Nokia N95

A standalone Symbian OS 9.x ARMv5T demonstration of **real stencil shadow
volumes** using EGL 1.1 and fixed-function OpenGL ES 1.1. Two procedural gear
casters rotate above a checker floor under a moving point light. Each frame the
CPU classifies lit faces, builds silhouette sides and both volume caps, then
the GPU performs a two-sided z-fail stencil pass and darkens pixels with a
nonzero stencil value.

## OGLES 1 reference and N95 adaptation

The reference is the OGLES 1 example the user supplied:
[`Native_SDK/Examples/Intermediate/ShadowVolume/OGLES`](https://github.com/robocon2011/Native_SDK/tree/3.5/Examples/Intermediate/ShadowVolume/OGLES).
That sample uses PVRShell/PVRTools, POD/PVR assets, VBOs, and
`GL_OES_matrix_palette` to extrude the shadow mesh. This port keeps the same
stencil-volume approach but replaces those dependencies with a small procedural
closed gear mesh and CPU extrusion. That keeps the N95 build on core GLES 1.1
entry points and avoids requiring matrix-palette or VBO extensions. It is not
the OGLES2 sample and contains no shaders, framebuffer objects, or ES2 calls.

The EGL config requests the native window's color-buffer depth, a 16-bit depth
buffer, and an 8-bit stencil buffer. It deliberately does not use EGL 1.2
renderable-type attributes or context-version attributes. If the driver cannot
provide stencil, startup displays the EGL/config failure instead of silently
falling back to projected shadows. EGL/GLES symbols are resolved at runtime
from the combined `libgles_cm.dll` using the combined Symbian
`opengles11u.def` ordinals; split EGL/GLES imports are not linked.

## Build

Install Symbian SDK 0.3.0 and select its root in `sdk-location.json` (the
relative path is intended for the project under
`$SDK_ROOT/examples/ShadowVolumeN95`), then run:

```sh
symbian app build --project "$SDK_ROOT/examples/ShadowVolumeN95"
symbian package \
  --project "$SDK_ROOT/examples/ShadowVolumeN95" \
  --artifact "$SDK_ROOT/examples/ShadowVolumeN95/.symbian/build/shadowvolumen95.exe" \
  --output /tmp/shadow-volume-sis
```

The GitHub Actions workflow builds the ARMv5T E32 executable and packages a
SIS artifact. A successful build/package does not prove that a particular N95
ROM exposes an EGL stencil config; verify on the target device. Escape or the
native close event exits the demo.
