# FilmTVN95

A Nokia N95 / Symbian adaptation of the Native SDK 3.5 Advanced **FilmTV**
OGLES example. FilmTV demonstrates render-to-texture: the original scene is
rendered to alternating framebuffer objects, and the previous frame is shown
on the television screen while the next one is being drawn.

This self-contained version keeps that GLES 1.1/OES framebuffer ping-pong
technique, but replaces the original POD/PVRTools/Print3D assets with a compact
procedural television and animated miniature scene. The miniature scene
contains a second TV that displays the previous render, preserving the
feedback effect without external files or the desktop SDK framework.

## Target and runtime

- Nokia N95-class Symbian S60 device with EGL and OpenGL ES 1.1 fixed function.
- ARMv5T, soft-float; no VFP instructions and no OpenGL ES 2.0 APIs.
- Runtime GLES/EGL symbols are resolved from `libgles_cm.dll`.
- EGL requests the native window color depth and a 16-bit depth buffer.
- Uses `GL_OES_framebuffer_object` when available, otherwise falls back to
  texture-bound EGL 1.1 PBuffer surfaces. It checks the GLES and EGL runtime
  capabilities and shows a Symbian information note only if neither path can
  be created.

Escape closes the application.

## Build

The dedicated `.github/workflows/film-tv-n95.yml` workflow builds the ARMv5T
E32 executable and packages the SIS.
