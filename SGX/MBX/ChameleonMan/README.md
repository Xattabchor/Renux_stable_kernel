# Chameleon Man — MBX / OpenGL ES 1.1 port

This directory contains an OGLES 1.1 version of the PowerVR SDK Chameleon Man demo. It retains the POD character animation, three skinned materials, moving wall/lamp/background, and a runtime DOT3-per-pixel-lighting toggle.

## Rendering path

- Character geometry is expanded into the POD's bone batches and animated with `GL_OES_matrix_palette`, following the OGLES matrix-palette sample in the SDK. Batch-local matrix indices remain valid and stay within the MBX palette limit.
- The original normal maps store their tangent-space Y component in red and X in green. Each frame the port skins the position and tangent frame on the CPU to update the primary-color light vector; the fixed-function texture environment evaluates the normal-map DOT3 and diffuse modulation per pixel. This avoids requiring programmable shaders on MBX while preserving the original DOT3 effect.
- Press **Action 1** to switch to the per-vertex diffuse-light fallback. DOT3 is disabled automatically if the device does not expose a compatible fixed-function DOT3 extension.
- Static environment meshes use the original baked wall diffuse texture and animated sky/lamp textures.

The rendering code is built for floating-point `VERTTYPE` data (`Common=1`); the mesh values passed to GL use ordinary `GL_FLOAT` arrays. The `CommonLite` fixed-point configuration is not supported by this implementation.

## Assets

The SDK's Chameleon Man POD and PVR files remain SDK-supplied content and are not duplicated in this repository. From this directory, run:

```sh
./prepare_assets.sh
```

or pass the extracted `GFX_Linux_SDK` path:

```sh
./prepare_assets.sh /path/to/GFX_Linux_SDK
```

The script copies the POD and nine textures into `OGLES/Media/`. Keep these SDK assets out of source control if the SDK license does not permit redistribution.

## Build

The makefile targets the SDK's Linux OMAP3 OGLES backend (the supplied 4.03 tree does not contain a LinuxGeneric platform makefile). Build after staging the assets:

```sh
cd OGLES/Build/LinuxOMAP3
make Common=1 TOOLCHAIN=/path/to/arm-none-linux-gnueabi
```

Omit `TOOLCHAIN` if the SDK's `arm-none-linux-gnueabi-g++` tools are already on `PATH`; override `SDKDIR` if the extracted SDK is elsewhere. For another MBX board/OS, substitute its PVRShell platform backend and GLES link libraries. Launch the executable with its working directory or PVRShell read path set to `OGLES/Media` (for example, set `prefReadPath` to `../../Media` when launching from `OGLES/Build/LinuxOMAP3`).

## Port constraints

The POD is expected to contain the original node order (body, legs, belt, wall, background, lights), triangle-list skin meshes with tangents/binormals, and bone batches. Startup validates these assumptions and reports a shell error rather than drawing malformed data. `GL_OES_matrix_palette` is required; fixed-function DOT3 is optional and has the vertex-light fallback.
