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
