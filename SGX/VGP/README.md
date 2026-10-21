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

## MBX VGP planar water and soft shadow overlay

`MBXWater.vp` combines projective reflection/refraction coordinates, vertex
Fresnel blending, and CPU-generated wave displacement/normals. The host-side
wave helper is `MBXWaterWaves.h`; see `MBXWater.md` for the render-to-texture
sequence and MBX two-layer combiner setup. `MBXWaterShadow.vp` projects a
filtered planar shadow mask onto the animated surface, and
`MBXWaterShadowProjection.h` projects caster triangles onto the mean water
plane, and `MBXWaterShadowBlur.h` provides a small CPU alpha blur. These shadows
have a soft filtered penumbra, not programmable PCF.

Build the water shaders for full VGP as follows:

```sh
VGPCompiler -iMBXWater.vp -oMBXWater -target=generic
VGPCompiler -iMBXWaterShadow.vp -oMBXWaterShadow -target=generic
```

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

## Dynamic shot flash and flashlight lighting

`DynamicShotFlashLighting.vp` evaluates one transient point light (for a muzzle
flash or hit event) and one spotlight (for a flashlight) at each vertex. It
uses no `glLight*` calls and no baked vertex colours. The host updates `env[8]`
through `env[14]` with `glProgramEnvParameter4fvARB` each frame; see the source
header for the complete layout. For an inactive shot, set `env[9].w` to zero.
Use the ordinary texture stage with `GL_MODULATE` to apply the dynamic vertex
illumination to the material texture. The flashlight and shot positions, plus
the flashlight direction, must be supplied in eye space.

This computes per-vertex illumination, not a texture lightmap. A dense mesh
improves smoothness; the number of sources is intentionally limited to fit VGP
resource limits. Compile for full VGP with the SDK compiler, for example:

```sh
VGPCompiler -iDynamicShotFlashLighting.vp -oDynamicShotFlashLighting -target=generic
```

### Combine baked vertex illumination with dynamic lights

`DynamicBakedShotFlashLighting.vp` is the companion variant. It reads baked
static illumination from `vertex.color.rgb`, then adds the same live shot and
flashlight contributions before modulating the material texture. Enable the
GLES color array and fill its RGB channels with baked illumination (not the
material albedo); alpha is ignored. The live shot/flashlight parameters remain
in `env[8..14]` and are updated by the host. Thus dark baked areas can still be
lit by a shot or flashlight, while the baked contribution remains when those
lights are off.
