# GLES Extension Probe (Symbian)

A small on-device OpenGL ES 1.x diagnostic app. It creates a GLES context with
`GlesWindowContext`, then displays the context's `GL_VERSION`, `GL_RENDERER`,
`GL_VENDOR`, every space-delimited token returned by `glGetString(GL_EXTENSIONS)`,
and the result of `GlesWindowContext::GetProcAddress` for IMG VGP and OES matrix
palette entry points.

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
