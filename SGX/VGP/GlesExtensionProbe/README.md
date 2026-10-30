# GLES Extension Probe (Symbian)

A small on-device OpenGL ES 1.x diagnostic app. It targets the ARMv5T E32
image profile for Symbian OS 9.x handset-loader compatibility, while remaining
valid on ARM11-class N95 hardware. It dynamically loads the legacy
`libgles_cm.dll` and resolves its EGL/GLES 1.1 entry points using the combined
EGL/GLES ordinals from Symbian's public [`opengles11u.def`](https://github.com/SymbianSource/oss.FCL.sf.os.graphics/blob/ff133bc50e6158bfb08cc093b0f0055321dcde99/opengles/openglesinterface/bwins/opengles11u.def),
not the separately numbered GLES-only proxy. This matches the combined-driver
path used by the Quake III Symbian port. It displays
`GL_VERSION`, `GL_RENDERER`, `GL_VENDOR`, every space-delimited token returned
by `glGetString(GL_EXTENSIONS)`, and the result of the driver's
`eglGetProcAddress` for IMG VGP and OES matrix-palette entry points. The EGL
config requests the color depth reported by the native Symbian window, with 16-bit depth, and avoids
ES2-only attributes/import DLLs.

The project selects the SDK's `LegacyEka2` runtime profile for older EKA2 ROMs.
This avoids importing newer EUSER atomic ordinals that are absent from devices
such as the Nokia 5320 RM-409 emulator image. Packaging success still does not
prove runtime compatibility; verify launch on the target emulator/device.

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
