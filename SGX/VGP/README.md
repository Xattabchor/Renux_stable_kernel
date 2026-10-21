# PhantomMask spherical-harmonics VGP port

`PhantomMask_SH.vp` is an IMG Vertex Program (VGP) port of the OGLES2 SDK
sample's `SHVertShader.vsh`. It evaluates the same nine-term second-order
spherical-harmonic lighting expression; it is a lighting shader, not a skinning
shader.

## Build

Use the SDK's `Utilities/VGPCompiler` for the full VGP target:

```sh
VGPCompiler -iPhantomMask_SH.vp -oPhantomMask_SH -target=generic
```

The compiler emits `PhantomMask_SH.bin` and `PhantomMask_SH.h`. Do not use
`-vgplite` when targeting a full VGP device.

## Host-side setup

The shader reads the MVP from `state.matrix.mvp`. Upload environment registers
`env[4..13]` with `glProgramEnvParameter4fvARB`:

| Register | Value |
|---|---|
| 4–6 | Model 3x3 normal/light-frame transform as three row vectors |
| 7–9 | `cAr`, `cAg`, `cAb` |
| 10–12 | `cBr`, `cBg`, `cBb` |
| 13 | `cC` in xyz (w unused) |

The SH coefficient packing/scaling is the same as the OGLES2 demo's
`ComputeAndSetSHIrradEnvMapConstants`; this VGP source expects those already
precomputed constants. The transformed normal and SH coefficients must use the
same coordinate frame. As in the OGLES2 sample, the transformed normal is not
renormalized in the shader.

The OGLES2 demo's fragment stage multiplies the SH colour by a texture. On the
IMG VGP path, pass the UV through `result.texcoord[0]` and configure the GLES
fixed-function texture environment for `GL_MODULATE` to get the equivalent
texture-times-light-colour operation.

## Vertex-generated glass refraction

`GlassRefraction.vp` produces a screen-space UV from clip coordinates and bends
it by the eye-space normal, with stronger distortion at grazing angles. It
outputs a glass tint/opacity as primary colour. The effect requires a 2D copy
of the opaque scene to be bound as the texture; draw the glass afterward with
blending enabled. Configure the texture environment for `GL_MODULATE` and use
`SRC_ALPHA, ONE_MINUS_SRC_ALPHA` blending. Use the texture matrix to flip Y if
the background texture is upside-down relative to the framebuffer.

Upload these registers with `glProgramEnvParameter4fvARB`:

| Register | Value |
|---|---|
| 8 | signed refraction scale in normalized screen UV (`x`, `y`) |
| 9 | glass tint and opacity (`r`, `g`, `b`, `alpha`) |

The distortion is evaluated per vertex, then interpolated; a dense glass mesh
looks smoother. It is an approximation, not per-pixel Snell refraction, and it
assumes a rigid/uniform model transform for the normal matrix. It does not
model thickness, internal reflections or dispersion.

Compile either source for the full VGP target with the SDK compiler:

```sh
VGPCompiler -iGlassRefraction.vp -oGlassRefraction -target=generic
```
