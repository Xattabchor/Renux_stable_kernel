# PolyBumpN95

A Nokia N95 / Symbian adaptation of the Native SDK 3.5 OGLES PolyBump
technique. It demonstrates per-pixel bump lighting with the GLES 1.1 fixed-
function texture combiner: texture unit 0 performs `GL_DOT3_RGB` against an
object-space normal map, then texture unit 1 modulates the result with a diffuse
map.

The original example's POD/PVR assets and PVRTools/Print3D framework are not
required here. A small deformed head mesh and 128x128 RGB diffuse/normal maps
are generated at startup, keeping the SIS self-contained and avoiding runtime
asset loading. The normal map is derived from the same procedural surface as
the mesh, with a fine height field added for the bump detail.

## Target and runtime

- Nokia N95-class Symbian S60 device with EGL and OpenGL ES 1.1 fixed function.
- ARMv5T, soft-float; no VFP instructions and no OpenGL ES 2.0 APIs.
- Runtime GLES/EGL symbols are resolved from the Symbian combined
  `libgles_cm.dll`.
- EGL requests the native window color depth and a 16-bit depth buffer; no
  stencil plane is required.
- The app checks for two texture units and a GLES 1.1 implementation, then
  displays a Symbian information note if the required DOT3 path is unavailable.

The mesh slowly rotates under an orbiting directional light. Escape closes the
application.

## Build

This directory follows the repository's Symbian CLI project layout. The
`poly-bump-n95.yml` GitHub Actions workflow builds the E32 executable for
ARMv5T and packages the SIS.
