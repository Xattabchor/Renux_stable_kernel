# MBX + VGP planar water

`MBXWater.vp` implements the water-surface vertex stage for the most advanced
practical MBX effect described here: a moving surface with projective reflection
and refraction textures, vertex Schlick Fresnel, and normal-based UV distortion.
The CPU wave helper is `MBXWaterWaves.h`; the two image captures and the final
reflection/refraction mix are fixed-function OGLES passes.

This is deliberately not presented as a fragment shader. MBX has two
fixed-function texture layers and can do DOT3 bump lighting, but cannot use a
normal-map sample to perform a dependent, per-pixel refraction lookup.

## Build

```sh
VGPCompiler -iSGX/VGP/MBXWater.vp -oSGX/VGP/build/MBXWater -target=generic
```

For an MBX VGP Lite target:

```sh
VGPCompiler -iSGX/VGP/MBXWater.vp -oSGX/VGP/build/MBXWaterLite -vgplite -target=generic
```

The tested compiler limits are 256 instructions for `generic` and 128 for
`vgplite`; the parameter and TEMP limits are 16 local vectors, 16 environment
vectors, and 14 TEMP registers. Check the generated program and runtime
extension support on the actual device. Not every MBX platform includes VGP.

## Required mesh data

Use a reasonably dense, regular XZ grid. The position array contains the
undisturbed grid positions (`y` is the mean water height). Enable texture
coordinate array 1 and provide four floats per vertex:

```text
vertex.texcoord[1] = (animatedNormal.x,
                      animatedNormal.y,
                      animatedNormal.z,
                      animatedHeightOffset)
```

`MBXUpdateWaterWaves()` fills this data from a small sum of directional sine
waves. Call it each frame and upload the updated array (for example, with a
streaming VBO). `vertex.texcoord[0]` is not used by this program. Use a rigid or
uniform model transform; the shader transforms the supplied normal with the
model-view 3x3.

Example wave setup:

```cpp
static const MBXWaterWave waves[] = {
    {  1.00f,  0.18f, 0.035f, 2.8f, 1.25f, 0.0f },
    { -0.32f,  0.95f, 0.018f, 1.4f, 1.85f, 1.1f },
    {  0.72f, -0.69f, 0.008f, 0.7f, 2.60f, 2.4f }
};

MBXWaterVertexWaveData *waveData = new MBXWaterVertexWaveData[vertexCount];
MBXUpdateWaterWaves(xzPositions, vertexCount, waves,
                    sizeof(waves) / sizeof(waves[0]), timeSeconds, waveData);
// Keep waveData allocated; update it each frame, then upload it to the VBO.
// delete[] waveData when the water mesh is destroyed.
```

The directions should be normalized. Amplitudes and wavelengths are in the
same object-space units as the mesh. Use a fine enough grid to avoid visible
faceting; the UV distortion and Fresnel term are interpolated per triangle.

## Render sequence

1. Render the scene from the mirrored camera into a reflection texture. Exclude
the water surface to avoid feedback. Clip geometry on the far side of the water
plane if the platform provides a suitable clipping path.
2. Render the scene below the water into a refraction texture, again excluding
the water surface. A small blue/green tint can be applied in this capture pass.
3. Bind the refraction image on texture unit 0 and reflection image on unit 1.
Draw the water with `MBXWater.vp`, depth testing enabled, and the two-layer
combiner below. Since the two captured images are mixed into an opaque surface,
ordinary alpha blending is not required for this pass.

The SDK `Demos/FilmTV/OGLES/OGLESFilmTV.cpp` demonstrates render-to-texture
setup using FBOs with a PBuffer fallback. Reflection and refraction can reuse
one render target sequentially, provided each result is retained in a separate
texture before the water draw.

## Program parameters

Upload each row as one `vec4` with `glProgramEnvParameter4fvARB`:

| Environment register | Contents |
|---|---|
| `env[0..3]` | Object-space to refraction-texture projective matrix rows |
| `env[4..7]` | Object-space to reflection-texture projective matrix rows |
| `env[8]` | Refraction UV distortion scale `xy`; reflection scale `zw` |
| `env[9].x` | Schlick F0; use about `0.02..0.04` for water |

Each projection matrix should be `bias * projection * view * model`, with a
clip-to-texture bias that maps projected XY to `[0,1]` and preserves Q for
projective texture sampling. Flip texture Y in the matrix if the render-target
orientation requires it. Distortion signs/scales depend on camera and texture
orientation; start with small magnitudes (roughly `0.005..0.03`) and tune for
your water grid and target resolution.

## Fixed-function two-texture mix

This sets stage 0 to refraction and stage 1 to `reflection * fresnel +
refraction * (1-fresnel)`. Confirm `GL_ARB_texture_env_combine` or the platform's
compatible texture-environment support before using it.

```cpp
glActiveTexture(GL_TEXTURE0);
glEnable(GL_TEXTURE_2D);
glBindTexture(GL_TEXTURE_2D, refractionTexture);
glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

glActiveTexture(GL_TEXTURE1);
glEnable(GL_TEXTURE_2D);
glBindTexture(GL_TEXTURE_2D, reflectionTexture);
glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_INTERPOLATE);
glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE);         // reflection
glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PREVIOUS);       // refraction
glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_PRIMARY_COLOR);   // Fresnel weight
glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_COLOR);
```

The VGP outputs Fresnel as grayscale primary color, which is the interpolation
factor. Set the stage alpha operation to replace from the texture if the render-
target formats carry meaningful alpha; otherwise RGB captures produce opaque
output.

To provide the wave vector as VGP input, select client texture unit 1 for the
array and use four `GL_FLOAT` components per vertex:

```cpp
glClientActiveTexture(GL_TEXTURE1);
glEnableClientState(GL_TEXTURE_COORD_ARRAY);
glTexCoordPointer(4, GL_FLOAT, sizeof(MBXWaterVertexWaveData),
                  &waveData[0].normalAndHeight[0]);
```

Restore client texture-array and active texture state after drawing the water.

## Soft projected shadows on the water

`MBXWaterShadow.vp` is an optional second water pass for a soft shadow mask.
The host should project opaque caster triangles onto the mean water plane from
the light's position/direction, rasterize the projected polygons as white with
alpha 1 into a low-resolution RGBA texture, and blur the alpha channel before
uploading it. For point lights, project each caster vertex along the ray from
the light through that vertex to the water plane; for directional lights,
project along the light-ray direction. This gives the correct hard-shadow
footprint for the planar receiver. Blur radius can be chosen from the light's
angular size and caster-to-water distance for a plausible penumbra.

`MBXWaterShadowBlur.h` provides a separable 5-tap Gaussian alpha blur for an
RGBA8 mask. Repeated blur or a smaller mask gives a wider, cheaper penumbra.
Clear the mask to alpha zero; draw the projected opaque triangles with alpha
one so their union remains opaque before filtering. Rebuild the mask when a
caster or light moves, not every frame if both are static.

For the overlay pass, upload `env[0..3]` as the water-object-to-mask projective
matrix and `env[4].x` as shadow strength. Bind the white-RGB/soft-alpha mask on
texture unit 0 with `GL_MODULATE`; enable blending with
`GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA`. Draw the same wave-updated grid after
the water composite, use `GL_LEQUAL`, and disable depth writes for this pass.
The displaced surface position warps the mask over the waves. This is a
projected planar shadow, not a shadow-map comparison; MBX has no programmable
PCF stage, so the soft penumbra is filtered/approximated rather than physically
integrated over an area light.

## What this achieves—and what it does not

- The reflected and refracted scenes are real render-to-texture captures.
- Fresnel blending and wave displacement/distortion are vertex-evaluated; a
  denser grid improves smoothness but costs more vertices.
- This does **not** sample a normal map and use it to offset the refraction
  texture independently for every pixel. That needs a programmable fragment
  stage or a different hardware path.
- DOT3 normal-map highlights can be added as a separate pass, but the two MBX
  texture layers are already occupied by reflection and refraction in the main
  mix. Extra lighting/material detail therefore costs a pass or a simplification.
- The CPU wave helper uses ordinary sine/cosine. It avoids relying on
  trigonometric VGP instructions and keeps the same shader usable on VGP Lite.
