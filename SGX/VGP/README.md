# PhantomMask spherical-harmonics VGP port

## Maximum affine bone-skinning palette

`build/BoneSkinningMax.vp` is a full-VGP linear-blend skinning shader compiled
by this SDK's `VGPCompiler` for `-target=generic`. It processes four influences
sequentially while reusing four TEMP registers. Each bone is an object-space
3x4 affine matrix stored as three `vec4` rows; the shader accumulates weighted
position and normal, normalizes the result, transforms position by the MVP, and
writes the normalized normal as debug vertex colour.

The tested compiler ceiling is **10 bones** for this representation. The
accepted program uses 30 parameter rows: `program.local[0..15]` for the first
16 rows and `program.env[0..13]` for the remaining 14. Eleven bones require 33
rows and fail because parameter index 16 is out of range (valid bank indices
are 0 through 15). The test used 4 declared TEMP registers, so the 10-bone
limit is the parameter-bank ceiling, not a TEMP-register failure. The actual
compiler log and first rejected candidate are preserved in
`build/BoneSkinning_search.log`; `build/BoneSkinningMax.txt` summarizes them.

Vertex inputs are `position`, `normal`, `texcoord[0]`, four integer-valued
bone indices in `attrib[1]` (range 0..9), and four weights in `attrib[2]`.
Weights should sum to 1.0. Upload each bone's three rows consecutively: rows
0..15 with `glProgramLocalParameter4fvARB`, then rows 16..29 with
`glProgramEnvParameter4fvARB`. Use rigid or uniform-scale bone transforms; the
shader applies each bone's upper 3x3 directly to normals before renormalizing.
The generated `build/BoneSkinningMax.bin` is the compiled program and
`build/BoneSkinningMax.h` is the compiler's C array form.

Reproduce the sequential compiler probe on Linux with the SDK's 32-bit runtime
installed (`libc6-i386`, `lib32gcc-s1`, and `lib32stdc++6`):

```sh
python3 SGX/VGP/generate_bone_skinning.py --max-try 64
```

The generator retains the largest passing shader, `.bin`, `.h`, and a complete
search log under `build/`. This is compile validation against the generic VGP
target; it does not assert that a particular device/runtime exposes the same
register-bank capacities.

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

## Full-VGP custom lighting: SH + sun/moon + local lights

`CustomLighting.vp` combines nine-term SH diffuse irradiance, separately
controlled directional sun and moon, one point light, one spotlight, optional
fill, and baked vertex RGB. Every contribution is computed in the VGP; the
shader does not read or require any `glLight*` state. Texture colour is applied
later by the fixed-function texture stage with `GL_MODULATE`.

This version targets the full VGP (`-target=generic`), not VGP Lite. It uses
`env[7..13]` for precomputed SH irradiance coefficients and `local[0..10]` for
the direct sources and fill. Directions, positions, normals, and SH coefficients
must share eye space; upload the sun/moon directions and point/spot positions in
eye space, and rotate SH coefficients into eye space when the camera rotates.
Set a source's intensity (`w`) to zero to disable it. The shader expects
`vertex.color.rgb` to contain baked illumination; if no baked term is used, set
the current vertex colour to black.

```sh
VGPCompiler -iSGX/VGP/CustomLighting.vp -oSGX/VGP/build/CustomLighting -target=generic
```

The output is diffuse vertex lighting, so it is interpolated across triangles.
It does not calculate specular highlights or pixel-level normal-map lighting;
use MBX DOT3 as an additional fixed-function pass if needed. More point/spot
sources require duplicating their instruction blocks and remain subject to the
full-VGP instruction and parameter limits.

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

### Quantum draw mode 7 screen distortion

`QuantumMode7Glass.vp` reproduces mode 7's screen-copy warp more closely than
`GlassRefraction.vp`: Quantum's mode 7 uses the sampled texture's red/green
channels as a per-pixel screen-space offset, centered at 128. The VGP has no
fragment texture lookup, so the host must sample that distortion map at each
mesh vertex and place normalized R/G values in `vertex.color.rg`. The shader
then creates projective framebuffer coordinates and applies the same
resolution-scaled offset. A dense glass mesh reduces the per-vertex
approximation error.

Bind the opaque scene copy as the only texture, use `GL_MODULATE` with white
primary colour, and draw the glass after the opaque scene with blending disabled
for Quantum-like replacement sampling. Upload `env[8]` using the formula in
the shader header; flip the Y scale or texture matrix if the framebuffer copy
has the opposite vertical orientation. The asset/render host must supply the
per-vertex distortion colours; the VGP cannot read Quantum's distortion texture
itself.

```sh
VGPCompiler -iSGX/VGP/QuantumMode7Glass.vp -oSGX/VGP/build/QuantumMode7Glass -target=generic
```

This matches mode 7's basic distortion-map effect, not the software rasterizer's
exact per-pixel result. It does not model refraction thickness or material tint.

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

### Quantum-style mode 13: RGB baked lighting, AO/GI and runtime lights

Quantum modes 9 and 13 store baked illumination per polygon vertex and
interpolate it while rasterizing the textured triangle. Mode 9 is monochrome;
mode 13 keeps separate red, green and blue illumination. `LightMapper` bakes
static sky/sun, local lights, AO and optional GI. The runtime shot flash and
flashlight are then added to the vertex illumination before it modulates the
material texture.

`DynamicBakedShotFlashLighting.vp` follows that mode-13 data path: `vertex.color.rgb`
is the precomputed RGB illumination, while a transient point flash and a
spotlight are evaluated at each vertex at runtime. AO and GI belong in the
baked vertex RGB, not in the material albedo. GLES interpolates the vertex
colour perspective-correctly across each triangle; the texture stage should use
`GL_MODULATE`. For a monochrome mode-9 bake, pass `--monochrome`; the baker
writes the same luminance into all three baked channels. Use white dynamic
light colours if the runtime contribution should also remain grayscale.

The standalone offline helper `bake_vertex_lighting.py` can bake OBJ meshes
without external Python packages. It writes the common OBJ vertex-colour
extension (`v x y z r g b`) and supports hemispheric sky/ground, shadowed
sunlight, repeatable static point lights, cosine-sampled AO, and configurable
multi-bounce diffuse GI. At every diffuse hit it uses the material's `Kd`
reflectance; `Ke` contributes emissive light. Both come from referenced MTL
files, with `--default-albedo` as the missing-`Kd` fallback. Example:

```sh
python3 SGX/VGP/bake_vertex_lighting.py scene.obj scene_baked.obj \
  --sky 0.28 0.34 0.45 --ground 0.045 0.035 0.025 \
  --sun-dir 0.35 0.8 0.25 --sun-color 0.8 0.72 0.58 \
  --point-light 1 2 0 1.0 0.35 0.12 2.0 5.0 \
  --ao-samples 32 --ao-distance 2.0 --ao-strength 0.75 \
  --gi-samples 32 --gi-bounces 8 --gi-strength 0.65 --rr-depth 3
```

The color values are linear and clamped to 0..1. Increase AO/GI sample counts
for smoother, less noisy bakes; set `--gi-samples 0` to disable GI and raise
`--gi-bounces` for deeper indirect lighting. Russian roulette (`--rr-depth`)
keeps long paths practical. The CPU cost and Monte Carlo noise grow with both
sample count and bounce depth. MTL `Kd`/`Ke` values are supported, but texture
maps such as `map_Kd` are not sampled; `--default-albedo` is used when no `Kd`
is available. After baking, the asset importer must copy the OBJ vertex RGB
into the GLES vertex-color array; this repository does not contain the game's
mesh importer or bind that array for you. Adjust intensity/radius and bake
settings to the scene's unit scale. The runtime VGP still requires the
`env[8..14]` upload described above.

### Import Quantum `.vla` light data

A VGP shader cannot read Quantum's `.vla` byte stream directly. Quantum writes
it as an unlabelled sequence of per-corner bytes; it contains no mesh IDs,
face indices, topology, or mode marker. `convert_quantum_vla.py` is the bridge:
it decodes mode 9 grayscale or mode 13 RGB and writes the values into OBJ
vertex colors.

```sh
python3 SGX/VGP/convert_quantum_vla.py scene.obj lightmap.vla scene_vgp.obj --mode 13
```

The OBJ must have exactly the same lighted triangle/quad sequence and polygon
corner order as the meshes used to generate the VLA. Faces skipped by Quantum's
serializer must also be absent from the OBJ. The converter catches impossible
lengths, but a different ordering with the same byte layout cannot be detected
because VLA has no topology metadata. Once converted, import the OBJ vertex RGB into
`vertex.color.rgb` and use `DynamicBakedShotFlashLighting.vp` for baked mode-13
colors plus live shot/flashlight lighting. The shader itself does not parse
`.vla`; the conversion must happen offline or in the host-side asset loader.

### Combine baked vertex illumination with dynamic lights

Use `DynamicBakedShotFlashLighting.vp` for the combined path documented above.
Enable the GLES color array and upload baked illumination (not material albedo)
in `vertex.color.rgb`; alpha is ignored. The shader and register layout are
unchanged when switching between RGB mode 13 and grayscale mode 9.
