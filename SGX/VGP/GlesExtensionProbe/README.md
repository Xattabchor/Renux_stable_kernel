# GLES Extension Probe (Symbian)

A small on-device OpenGL ES 1.x diagnostic app. It targets the ARMv5T E32
image profile for Symbian OS 9.x handset-loader compatibility, while remaining
valid on ARM11-class N95 hardware. It dynamically loads the legacy
`libGLES_CM.dll` and resolves its EGL/GLES 1.1 entry points, following
the same compatibility path used by the Quake III Symbian port. It displays
`GL_VERSION`, `GL_RENDERER`, `GL_VENDOR`, every space-delimited token returned
by `glGetString(GL_EXTENSIONS)`, and the result of the driver's
`eglGetProcAddress` for IMG VGP and OES matrix-palette entry points. The EGL
config requests a 32-bit color buffer with 16-bit depth and avoids
ES2-only attributes/import DLLs.

The report is paginated. Use left/right or up/down keys; tap the left/right
half of the screen to change pages; Escape closes the app.

**Important:** this reports extensions advertised by the active GLES context.
It does not scan the protected `libgles_cm.dll` file for every embedded string.
A name found in the DLL's string table is not necessarily exposed to a running
context. The entry-point checks are also performed after `MakeCurrent()`.

## Build and package with Symbian SDK 0.3.0

The project expects `sdk-location.json` to point at the SDK root. Its checked-in
`../..` path is intended for the project's normal location under
`$SDK_ROOT/examples/GlesExtensionProbe`:

```sh
SDK_ROOT=/path/to/symbian-sdk/install
PROJECT="$SDK_ROOT/examples/GlesExtensionProbe"
mkdir -p "$PROJECT"
cp -a SGX/VGP/GlesExtensionProbe/. "$PROJECT/"
symbian app build --project "$PROJECT"
symbian package \
  --project "$PROJECT" \
  --artifact "$PROJECT/.symbian/build/glesextprobe.exe" \
  --output /tmp/gles-extension-probe-sis
```
