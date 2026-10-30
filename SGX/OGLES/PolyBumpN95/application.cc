#include "application.h"

#include <e32std.h>
#include <w32std.h>
#include <EGL/egl.h>
#include <GLES/gl.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

#include "symbian/api/display/window_surface.h"

namespace {
using symbian::api::display::WindowInputKind;
using symbian::api::display::WindowKey;
using symbian::api::display::WindowSurface;

// Resolve the Symbian 9.x combined EGL/GLES 1.1 library at runtime. N95 ROMs
// expose libgles_cm.dll; this deliberately avoids newer split EGL imports.
class GlesApi final {
 public:
  using EglChooseConfigFn = decltype(&eglChooseConfig);
  using EglCreateContextFn = decltype(&eglCreateContext);
  using EglCreateWindowSurfaceFn = decltype(&eglCreateWindowSurface);
  using EglDestroyContextFn = decltype(&eglDestroyContext);
  using EglDestroySurfaceFn = decltype(&eglDestroySurface);
  using EglGetErrorFn = decltype(&eglGetError);
  using EglInitializeFn = decltype(&eglInitialize);
  using EglMakeCurrentFn = decltype(&eglMakeCurrent);
  using EglSwapBuffersFn = decltype(&eglSwapBuffers);
  using EglTerminateFn = decltype(&eglTerminate);
  using GlActiveTextureFn = decltype(&glActiveTexture);
  using GlBindTextureFn = decltype(&glBindTexture);
  using GlClearFn = decltype(&glClear);
  using GlClearColorFn = decltype(&glClearColor);
  using GlColor4fFn = decltype(&glColor4f);
  using GlDisableFn = decltype(&glDisable);
  using GlDisableClientStateFn = decltype(&glDisableClientState);
  using GlDrawElementsFn = decltype(&glDrawElements);
  using GlEnableFn = decltype(&glEnable);
  using GlEnableClientStateFn = decltype(&glEnableClientState);
  using GlFrustumfFn = decltype(&glFrustumf);
  using GlGenTexturesFn = decltype(&glGenTextures);
  using GlGetIntegervFn = decltype(&glGetIntegerv);
  using GlGetStringFn = decltype(&glGetString);
  using GlLoadIdentityFn = decltype(&glLoadIdentity);
  using GlMatrixModeFn = decltype(&glMatrixMode);
  using GlClientActiveTextureFn = decltype(&glClientActiveTexture);
  using GlNormalPointerFn = decltype(&glNormalPointer);
  using GlRotatefFn = decltype(&glRotatef);
  using GlTexCoordPointerFn = decltype(&glTexCoordPointer);
  using GlTexEnvfFn = decltype(&glTexEnvf);
  using GlTexImage2DFn = decltype(&glTexImage2D);
  using GlTexParameterfFn = decltype(&glTexParameterf);
  using GlTranslatefFn = decltype(&glTranslatef);
  using GlVertexPointerFn = decltype(&glVertexPointer);
  using GlViewportFn = decltype(&glViewport);

  GlesApi() = default;
  GlesApi(const GlesApi&) = delete;
  GlesApi& operator=(const GlesApi&) = delete;
  ~GlesApi() { Close(); }

  bool Load() {
    if (library_.Load(_L("libgles_cm.dll")) != KErrNone) {
      error_ = "RLibrary.Load(libgles_cm.dll) failed";
      return false;
    }
    loaded_ = true;
    // Ordinals are from SymbianSource opengles11u.def, the combined 1.1 DLL.
    return Bind(&egl_choose_config, 1, "eglChooseConfig") &&
           Bind(&egl_create_context, 3, "eglCreateContext") &&
           Bind(&egl_create_window_surface, 6, "eglCreateWindowSurface") &&
           Bind(&egl_destroy_context, 7, "eglDestroyContext") &&
           Bind(&egl_destroy_surface, 8, "eglDestroySurface") &&
           Bind(&egl_get_display, 14, "eglGetDisplay") &&
           Bind(&egl_get_error, 15, "eglGetError") &&
           Bind(&egl_initialize, 17, "eglInitialize") &&
           Bind(&egl_make_current, 18, "eglMakeCurrent") &&
           Bind(&egl_swap_buffers, 22, "eglSwapBuffers") &&
           Bind(&egl_terminate, 23, "eglTerminate") &&
           Bind(&gl_active_texture, 26, "glActiveTexture") &&
           Bind(&gl_bind_texture, 29, "glBindTexture") &&
           Bind(&gl_clear, 31, "glClear") &&
           Bind(&gl_clear_color, 32, "glClearColor") &&
           Bind(&gl_client_active_texture, 37, "glClientActiveTexture") &&
           Bind(&gl_color4f, 38, "glColor4f") &&
           Bind(&gl_disable, 52, "glDisable") &&
           Bind(&gl_disable_client_state, 53, "glDisableClientState") &&
           Bind(&gl_draw_elements, 55, "glDrawElements") &&
           Bind(&gl_enable, 56, "glEnable") &&
           Bind(&gl_enable_client_state, 57, "glEnableClientState") &&
           Bind(&gl_frustumf, 65, "glFrustumf") &&
           Bind(&gl_gen_textures, 67, "glGenTextures") &&
           Bind(&gl_get_integerv, 69, "glGetIntegerv") &&
           Bind(&gl_get_string, 70, "glGetString") &&
           Bind(&gl_load_identity, 82, "glLoadIdentity") &&
           Bind(&gl_matrix_mode, 90, "glMatrixMode") &&
           Bind(&gl_normal_pointer, 97, "glNormalPointer") &&
           Bind(&gl_rotatef, 109, "glRotatef") &&
           Bind(&gl_tex_coord_pointer, 120, "glTexCoordPointer") &&
           Bind(&gl_tex_envf, 121, "glTexEnvf") &&
           Bind(&gl_tex_image_2d, 125, "glTexImage2D") &&
           Bind(&gl_tex_parameterf, 126, "glTexParameterf") &&
           Bind(&gl_translatef, 129, "glTranslatef") &&
           Bind(&gl_vertex_pointer, 131, "glVertexPointer") &&
           Bind(&gl_viewport, 132, "glViewport");
  }

  void Close() {
    if (loaded_) library_.Close();
    loaded_ = false;
  }

  template <typename Function>
  bool Bind(Function* output, TInt ordinal, const char* symbol) {
    const TLibraryFunction address = library_.Lookup(ordinal);
    if (address == nullptr) {
      error_ = std::string("libgles_cm.dll missing ") + symbol +
               " at ordinal " + std::to_string(ordinal);
      Close();
      return false;
    }
    *output = reinterpret_cast<Function>(address);
    return true;
  }

  std::string error_;
  RLibrary library_;
  bool loaded_ = false;
  EGLDisplay (EGLAPIENTRY *egl_get_display)(EGLNativeDisplayType) = nullptr;
  EglGetErrorFn egl_get_error = nullptr;
  EglInitializeFn egl_initialize = nullptr;
  EglChooseConfigFn egl_choose_config = nullptr;
  EglCreateWindowSurfaceFn egl_create_window_surface = nullptr;
  EglCreateContextFn egl_create_context = nullptr;
  EglMakeCurrentFn egl_make_current = nullptr;
  EglSwapBuffersFn egl_swap_buffers = nullptr;
  EglDestroyContextFn egl_destroy_context = nullptr;
  EglDestroySurfaceFn egl_destroy_surface = nullptr;
  EglTerminateFn egl_terminate = nullptr;
  GlActiveTextureFn gl_active_texture = nullptr;
  GlBindTextureFn gl_bind_texture = nullptr;
  GlClearFn gl_clear = nullptr;
  GlClearColorFn gl_clear_color = nullptr;
  GlClientActiveTextureFn gl_client_active_texture = nullptr;
  GlColor4fFn gl_color4f = nullptr;
  GlDisableFn gl_disable = nullptr;
  GlDisableClientStateFn gl_disable_client_state = nullptr;
  GlDrawElementsFn gl_draw_elements = nullptr;
  GlEnableFn gl_enable = nullptr;
  GlEnableClientStateFn gl_enable_client_state = nullptr;
  GlFrustumfFn gl_frustumf = nullptr;
  GlGenTexturesFn gl_gen_textures = nullptr;
  GlGetIntegervFn gl_get_integerv = nullptr;
  GlGetStringFn gl_get_string = nullptr;
  GlLoadIdentityFn gl_load_identity = nullptr;
  GlMatrixModeFn gl_matrix_mode = nullptr;
  GlNormalPointerFn gl_normal_pointer = nullptr;
  GlRotatefFn gl_rotatef = nullptr;
  GlTexCoordPointerFn gl_tex_coord_pointer = nullptr;
  GlTexEnvfFn gl_tex_envf = nullptr;
  GlTexImage2DFn gl_tex_image_2d = nullptr;
  GlTexParameterfFn gl_tex_parameterf = nullptr;
  GlTranslatefFn gl_translatef = nullptr;
  GlVertexPointerFn gl_vertex_pointer = nullptr;
  GlViewportFn gl_viewport = nullptr;
};

class Es1Context final {
 public:
  Es1Context() = default;
  Es1Context(const Es1Context&) = delete;
  Es1Context& operator=(const Es1Context&) = delete;
  ~Es1Context() { Close(); }

  bool Open(WindowSurface* window) {
    if (!api_.Load()) {
      error_ = api_.error_;
      return false;
    }
    if (window == nullptr || window->NativeWindowHandle() == nullptr) {
      error_ = "native Symbian window is unavailable";
      return false;
    }
    display_ = api_.egl_get_display(EGL_DEFAULT_DISPLAY);
    if (display_ == EGL_NO_DISPLAY) return Fail("eglGetDisplay");
    EGLint major = 0;
    EGLint minor = 0;
    if (!api_.egl_initialize(display_, &major, &minor)) {
      return Fail("eglInitialize");
    }
    initialized_ = true;

    EGLint buffer_bits = 32;
    switch (static_cast<RWindow*>(window->NativeWindowHandle())->DisplayMode()) {
      case EColor4K: buffer_bits = 12; break;
      case EColor64K: buffer_bits = 16; break;
      case EColor16M: buffer_bits = 24; break;
      default: buffer_bits = 32; break;
    }
    // PolyBump requires depth testing but not the stencil plane used by the
    // ShadowVolume sample. Keep the color request matched to the native mode.
    const EGLint attributes[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BUFFER_SIZE, buffer_bits,
        EGL_DEPTH_SIZE, 16,
        EGL_NONE};
    EGLConfig config = 0;
    EGLint count = 0;
    if (!api_.egl_choose_config(display_, attributes, &config, 1, &count)) {
      return Fail("eglChooseConfig");
    }
    if (count == 0) {
      error_ = "No EGL window config with native color depth and 16-bit depth";
      Close();
      return false;
    }
    surface_ = api_.egl_create_window_surface(
        display_, config,
        static_cast<EGLNativeWindowType>(window->NativeWindowHandle()), nullptr);
    if (surface_ == EGL_NO_SURFACE) return Fail("eglCreateWindowSurface");
    context_ = api_.egl_create_context(display_, config, EGL_NO_CONTEXT, nullptr);
    if (context_ == EGL_NO_CONTEXT) return Fail("eglCreateContext");
    if (!api_.egl_make_current(display_, surface_, surface_, context_)) {
      return Fail("eglMakeCurrent");
    }
    return true;
  }

  bool Swap() {
    if (display_ != EGL_NO_DISPLAY && surface_ != EGL_NO_SURFACE &&
        api_.egl_swap_buffers(display_, surface_)) return true;
    return Fail("eglSwapBuffers");
  }

  void Close() {
    if (display_ != EGL_NO_DISPLAY) {
      if (context_ != EGL_NO_CONTEXT) {
        api_.egl_make_current(display_, EGL_NO_SURFACE, EGL_NO_SURFACE,
                              EGL_NO_CONTEXT);
        api_.egl_destroy_context(display_, context_);
      }
      if (surface_ != EGL_NO_SURFACE) api_.egl_destroy_surface(display_, surface_);
      if (initialized_) api_.egl_terminate(display_);
    }
    display_ = EGL_NO_DISPLAY;
    surface_ = EGL_NO_SURFACE;
    context_ = EGL_NO_CONTEXT;
    initialized_ = false;
    api_.Close();
  }

  GlesApi& api() { return api_; }
  const std::string& error() const { return error_; }

 private:
  bool Fail(const char* stage) {
    char code[16];
    const EGLint value = api_.egl_get_error != nullptr ? api_.egl_get_error() : 0;
    std::snprintf(code, sizeof(code), "0x%04x", static_cast<unsigned int>(value));
    error_ = std::string(stage) + " failed (EGL " + code + ")";
    Close();
    return false;
  }
  GlesApi api_;
  EGLDisplay display_ = EGL_NO_DISPLAY;
  EGLSurface surface_ = EGL_NO_SURFACE;
  EGLContext context_ = EGL_NO_CONTEXT;
  bool initialized_ = false;
  std::string error_;
};

struct Vec3 { float x, y, z; };
Vec3 Add(Vec3 a, Vec3 b) { return {a.x + b.x, a.y + b.y, a.z + b.z}; }
Vec3 Sub(Vec3 a, Vec3 b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
Vec3 Mul(Vec3 a, float s) { return {a.x * s, a.y * s, a.z * s}; }
float Dot(Vec3 a, Vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
Vec3 Cross(Vec3 a, Vec3 b) {
  return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
          a.x * b.y - a.y * b.x};
}
Vec3 Normalize(Vec3 v) {
  const float length = std::sqrt(Dot(v, v));
  return length > 0.000001f ? Mul(v, 1.0f / length) : Vec3{0, 1, 0};
}

constexpr float kPi = 3.14159265358979323846f;
constexpr int kMeshSlices = 48;
constexpr int kMeshRings = 24;
constexpr int kTextureSize = 128;

Vec3 Surface(float u, float v) {
  const float phi = 2.0f * kPi * u;
  const float theta = kPi * std::max(0.0f, std::min(1.0f, v));
  const float st = std::sin(theta);
  const float ct = std::cos(theta);
  const float cp = std::cos(phi);
  const float sp = std::sin(phi);
  const float x = st * cp;
  const float y = ct;
  const float front = std::pow(std::max(0.0f, sp), 8.0f);

  float z = 0.72f * st * sp;
  const float nose = std::exp(-(x * x) / 0.025f - ((y - 0.03f) * (y - 0.03f)) / 0.12f);
  const float left_cheek = std::exp(-((x + 0.39f) * (x + 0.39f)) / 0.055f -
                                    ((y + 0.08f) * (y + 0.08f)) / 0.09f);
  const float right_cheek = std::exp(-((x - 0.39f) * (x - 0.39f)) / 0.055f -
                                     ((y + 0.08f) * (y + 0.08f)) / 0.09f);
  const float left_eye = std::exp(-((x + 0.25f) * (x + 0.25f)) / 0.018f -
                                  ((y - 0.28f) * (y - 0.28f)) / 0.035f);
  const float right_eye = std::exp(-((x - 0.25f) * (x - 0.25f)) / 0.018f -
                                   ((y - 0.28f) * (y - 0.28f)) / 0.035f);
  const float mouth = std::exp(-(x * x) / 0.045f - ((y + 0.31f) * (y + 0.31f)) / 0.008f);
  z += front * (0.37f * nose + 0.045f * (left_cheek + right_cheek) -
                0.045f * (left_eye + right_eye) - 0.025f * mouth);
  return {0.91f * x, 1.18f * y, z};
}

Vec3 SurfaceNormal(float u, float v) {
  if (v <= 0.0001f) return {0.0f, 1.0f, 0.0f};
  if (v >= 0.9999f) return {0.0f, -1.0f, 0.0f};
  constexpr float e = 0.0008f;
  const Vec3 du = Sub(Surface(u + e, v), Surface(u - e, v));
  const float vm = std::max(0.0f, v - e);
  const float vp = std::min(1.0f, v + e);
  const Vec3 dv = Sub(Surface(u, vp), Surface(u, vm));
  const Vec3 n = Normalize(Cross(du, dv));
  // At the very tips the UV parameterization collapses; use an ellipsoid normal.
  if (std::fabs(n.x) + std::fabs(n.y) + std::fabs(n.z) < 0.01f) {
    const Vec3 p = Surface(u, v);
    return Normalize({p.x / 0.91f, p.y / 1.18f, p.z / 0.72f});
  }
  return n;
}

struct Vertex {
  GLfloat x, y, z;
  GLfloat nx, ny, nz;
  GLfloat u, v;
};
struct Mesh {
  std::vector<Vertex> vertices;
  std::vector<GLushort> indices;
};

Mesh MakeHead() {
  Mesh mesh;
  mesh.vertices.reserve((kMeshSlices + 1) * (kMeshRings + 1));
  mesh.indices.reserve(kMeshSlices * kMeshRings * 6);
  for (int ring = 0; ring <= kMeshRings; ++ring) {
    const float v = static_cast<float>(ring) / kMeshRings;
    for (int slice = 0; slice <= kMeshSlices; ++slice) {
      const float u = static_cast<float>(slice) / kMeshSlices;
      const Vec3 p = Surface(u, v);
      const Vec3 n = SurfaceNormal(u, v);
      mesh.vertices.push_back({p.x, p.y, p.z, n.x, n.y, n.z, u, v});
    }
  }
  const int row = kMeshSlices + 1;
  for (int ring = 0; ring < kMeshRings; ++ring) {
    for (int slice = 0; slice < kMeshSlices; ++slice) {
      const GLushort a = static_cast<GLushort>(ring * row + slice);
      const GLushort d = static_cast<GLushort>(a + 1);
      const GLushort b = static_cast<GLushort>(a + row);
      const GLushort c = static_cast<GLushort>(b + 1);
      // UV order u then v yields outward-facing triangles for this surface.
      mesh.indices.push_back(a); mesh.indices.push_back(d); mesh.indices.push_back(b);
      mesh.indices.push_back(d); mesh.indices.push_back(c); mesh.indices.push_back(b);
    }
  }
  return mesh;
}

float BumpHeight(float u, float v) {
  const float a = 2.0f * kPi * (19.0f * u + 0.6f * std::sin(9.0f * v));
  const float b = 2.0f * kPi * (27.0f * v + 0.4f * std::sin(13.0f * u));
  const float c = 2.0f * kPi * (37.0f * u - 31.0f * v);
  return 0.014f * std::sin(a) * std::cos(b) + 0.006f * std::sin(c);
}

std::vector<GLubyte> MakeNormalMap() {
  std::vector<GLubyte> pixels(kTextureSize * kTextureSize * 3);
  const float e = 0.0012f;
  for (int y = 0; y < kTextureSize; ++y) {
    const float v = (static_cast<float>(y) + 0.5f) / kTextureSize;
    const float vm = std::max(0.0f, v - e);
    const float vp = std::min(1.0f, v + e);
    for (int x = 0; x < kTextureSize; ++x) {
      const float u = (static_cast<float>(x) + 0.5f) / kTextureSize;
      const Vec3 pu = Mul(Sub(Surface(u + e, v), Surface(u - e, v)), 1.0f / (2.0f * e));
      const Vec3 pv = Mul(Sub(Surface(u, vp), Surface(u, vm)), 1.0f / std::max(2.0f * e, vp - vm));
      const Vec3 n = SurfaceNormal(u, v);
      const float dhdu = (BumpHeight(u + e, v) - BumpHeight(u - e, v)) / (2.0f * e);
      const float dhdv = (BumpHeight(u, vp) - BumpHeight(u, vm)) / std::max(2.0f * e, vp - vm);
      const Vec3 perturbed = Normalize(Cross(Add(pu, Mul(n, dhdu)), Add(pv, Mul(n, dhdv))));
      const int offset = (y * kTextureSize + x) * 3;
      pixels[offset] = static_cast<GLubyte>(std::max(0, std::min(255, static_cast<int>((perturbed.x * 0.5f + 0.5f) * 255.0f))));
      pixels[offset + 1] = static_cast<GLubyte>(std::max(0, std::min(255, static_cast<int>((perturbed.y * 0.5f + 0.5f) * 255.0f))));
      pixels[offset + 2] = static_cast<GLubyte>(std::max(0, std::min(255, static_cast<int>((perturbed.z * 0.5f + 0.5f) * 255.0f))));
    }
  }
  return pixels;
}

std::vector<GLubyte> MakeDiffuseMap() {
  std::vector<GLubyte> pixels(kTextureSize * kTextureSize * 3);
  for (int y = 0; y < kTextureSize; ++y) {
    const float v = (static_cast<float>(y) + 0.5f) / kTextureSize;
    const float theta = kPi * v;
    const float sy = std::cos(theta);
    const float st = std::sin(theta);
    for (int x = 0; x < kTextureSize; ++x) {
      const float u = (static_cast<float>(x) + 0.5f) / kTextureSize;
      const float phi = 2.0f * kPi * u;
      const float sx = st * std::cos(phi);
      const float front = std::pow(std::max(0.0f, std::sin(phi)), 8.0f);
      const float pore = 4.0f * std::sin(83.0f * phi + 17.0f * theta) *
                         std::sin(61.0f * theta - 11.0f * phi);
      const float eye_l = std::exp(-((sx + 0.25f) * (sx + 0.25f)) / 0.009f -
                                   ((sy - 0.28f) * (sy - 0.28f)) / 0.014f);
      const float eye_r = std::exp(-((sx - 0.25f) * (sx - 0.25f)) / 0.009f -
                                   ((sy - 0.28f) * (sy - 0.28f)) / 0.014f);
      const float eye = std::max(eye_l, eye_r) * front;
      const float mouth = std::exp(-(sx * sx) / 0.035f - ((sy + 0.31f) * (sy + 0.31f)) / 0.004f) * front;
      const float variation = 6.0f * std::sin(5.0f * theta) + pore;
      const float r = 190.0f + variation - 90.0f * eye - 85.0f * mouth;
      const float g = 137.0f + variation * 0.72f - 82.0f * eye - 50.0f * mouth;
      const float b = 111.0f + variation * 0.55f - 65.0f * eye - 43.0f * mouth;
      const int offset = (y * kTextureSize + x) * 3;
      pixels[offset] = static_cast<GLubyte>(std::max(0.0f, std::min(255.0f, r)));
      pixels[offset + 1] = static_cast<GLubyte>(std::max(0.0f, std::min(255.0f, g)));
      pixels[offset + 2] = static_cast<GLubyte>(std::max(0.0f, std::min(255.0f, b)));
    }
  }
  return pixels;
}

GLuint UploadTexture(GlesApi& gl, const std::vector<GLubyte>& pixels) {
  GLuint texture = 0;
  gl.gl_gen_textures(1, &texture);
  gl.gl_bind_texture(GL_TEXTURE_2D, texture);
  gl.gl_tex_parameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  gl.gl_tex_parameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  gl.gl_tex_parameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  gl.gl_tex_parameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  gl.gl_tex_image_2d(GL_TEXTURE_2D, 0, GL_RGB, kTextureSize, kTextureSize, 0,
                     GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
  return texture;
}

void SetArrayPointers(GlesApi& gl, const Mesh& mesh) {
  const Vertex* vertices = mesh.vertices.data();
  gl.gl_vertex_pointer(3, GL_FLOAT, sizeof(Vertex), &vertices[0].x);
  gl.gl_normal_pointer(GL_FLOAT, sizeof(Vertex), &vertices[0].nx);
  gl.gl_client_active_texture(GL_TEXTURE0);
  gl.gl_enable_client_state(GL_TEXTURE_COORD_ARRAY);
  gl.gl_tex_coord_pointer(2, GL_FLOAT, sizeof(Vertex), &vertices[0].u);
  gl.gl_client_active_texture(GL_TEXTURE1);
  gl.gl_enable_client_state(GL_TEXTURE_COORD_ARRAY);
  gl.gl_tex_coord_pointer(2, GL_FLOAT, sizeof(Vertex), &vertices[0].u);
  gl.gl_client_active_texture(GL_TEXTURE0);
}

void ConfigureDot3(GlesApi& gl, GLuint normal_texture, GLuint diffuse_texture) {
  // Stage 0: DOT3(normal-map RGB, primary-color RGB/light vector).
  gl.gl_active_texture(GL_TEXTURE0);
  gl.gl_enable(GL_TEXTURE_2D);
  gl.gl_bind_texture(GL_TEXTURE_2D, normal_texture);
  gl.gl_tex_envf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
  gl.gl_tex_envf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_DOT3_RGB);
  gl.gl_tex_envf(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_TEXTURE);
  gl.gl_tex_envf(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_PREVIOUS);
  gl.gl_tex_envf(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
  gl.gl_tex_envf(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
  // Stage 1: modulate the per-pixel DOT3 result by the diffuse color map.
  gl.gl_active_texture(GL_TEXTURE1);
  gl.gl_enable(GL_TEXTURE_2D);
  gl.gl_bind_texture(GL_TEXTURE_2D, diffuse_texture);
  gl.gl_tex_envf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
  gl.gl_tex_envf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
  gl.gl_tex_envf(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_TEXTURE);
  gl.gl_tex_envf(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_PREVIOUS);
  gl.gl_tex_envf(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
  gl.gl_tex_envf(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
  gl.gl_active_texture(GL_TEXTURE0);
}

void RenderFrame(GlesApi& gl, const WindowSurface& window, const Mesh& mesh,
                 GLuint normal_texture, GLuint diffuse_texture,
                 float angle) {
  const auto size = window.size();
  const int width = std::max(1, size.width);
  const int height = std::max(1, size.height);
  const float aspect = static_cast<float>(width) / static_cast<float>(height);
  const float light_angle = angle * 0.63f;
  const Vec3 light_view = Normalize({1.6f * std::cos(light_angle), 0.85f,
                                     2.2f + 1.5f * std::sin(light_angle)});
  // The head rotates about Y; transform the view-space light back into head
  // space because the normal map stores object-space normals.
  const float c = std::cos(angle);
  const float s = std::sin(angle);
  const Vec3 light_local = Normalize({c * light_view.x - s * light_view.z,
                                       light_view.y,
                                       s * light_view.x + c * light_view.z});

  gl.gl_viewport(0, 0, width, height);
  gl.gl_clear_color(0.045f, 0.065f, 0.10f, 1.0f);
  gl.gl_clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  gl.gl_enable(GL_DEPTH_TEST);
  gl.gl_enable(GL_CULL_FACE);
  gl.gl_disable(GL_BLEND);
  gl.gl_matrix_mode(GL_PROJECTION);
  gl.gl_load_identity();
  const float half_height = 0.72f;
  gl.gl_frustumf(-half_height * aspect, half_height * aspect,
                 -half_height, half_height, 1.0f, 30.0f);
  gl.gl_matrix_mode(GL_MODELVIEW);
  gl.gl_load_identity();
  gl.gl_translatef(0.0f, 0.0f, -4.25f);
  gl.gl_color4f(light_local.x * 0.5f + 0.5f,
                light_local.y * 0.5f + 0.5f,
                light_local.z * 0.5f + 0.5f, 1.0f);
  gl.gl_rotatef(angle * (180.0f / kPi), 0.0f, 1.0f, 0.0f);

  gl.gl_active_texture(GL_TEXTURE0);
  gl.gl_enable(GL_TEXTURE_2D);
  gl.gl_active_texture(GL_TEXTURE1);
  gl.gl_enable(GL_TEXTURE_2D);
  gl.gl_active_texture(GL_TEXTURE0);
  SetArrayPointers(gl, mesh);
  ConfigureDot3(gl, normal_texture, diffuse_texture);
  gl.gl_enable_client_state(GL_VERTEX_ARRAY);
  gl.gl_enable_client_state(GL_NORMAL_ARRAY);
  gl.gl_draw_elements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()),
                      GL_UNSIGNED_SHORT, mesh.indices.data());
  gl.gl_disable_client_state(GL_NORMAL_ARRAY);
  gl.gl_disable_client_state(GL_VERTEX_ARRAY);
  gl.gl_client_active_texture(GL_TEXTURE1);
  gl.gl_disable_client_state(GL_TEXTURE_COORD_ARRAY);
  gl.gl_client_active_texture(GL_TEXTURE0);
  gl.gl_disable_client_state(GL_TEXTURE_COORD_ARRAY);
}

void ShowFailure(const char* message) {
  TBuf<64> text;
  for (const char* character = message;
       *character != '\0' && text.Length() < text.MaxLength(); ++character) {
    text.Append(TChar(static_cast<TUint8>(*character)));
  }
  User::InfoPrint(text);
  User::After(2500000);
}

int Run(WindowSurface* window) {
  Es1Context context;
  if (!context.Open(window)) {
    ShowFailure(context.error().c_str());
    return KErrNotSupported;
  }
  GlesApi& gl = context.api();
  GLint texture_units = 0;
  gl.gl_get_integerv(GL_MAX_TEXTURE_UNITS, &texture_units);
  if (texture_units < 2) {
    ShowFailure("PolyBump needs two GLES texture units");
    return KErrNotSupported;
  }
  const GLubyte* version = gl.gl_get_string(GL_VERSION);
  if (version == nullptr || std::string(reinterpret_cast<const char*>(version)).find("1.1") == std::string::npos) {
    ShowFailure("PolyBump needs the GLES 1.1 DOT3 texture combiner");
    return KErrNotSupported;
  }

  const Mesh mesh = MakeHead();
  const std::vector<GLubyte> normal_pixels = MakeNormalMap();
  const std::vector<GLubyte> diffuse_pixels = MakeDiffuseMap();
  const GLuint normal_texture = UploadTexture(gl, normal_pixels);
  const GLuint diffuse_texture = UploadTexture(gl, diffuse_pixels);
  bool running = true;
  float angle = 0.0f;
  while (running) {
    for (int count = 0; count < 32; ++count) {
      auto event = window->PollInput();
      if (!event.ok()) { running = false; break; }
      if (!event->has_value()) break;
      const auto& input = **event;
      if (input.kind == WindowInputKind::kCloseRequested ||
          (input.kind == WindowInputKind::kKeyDown && input.key == WindowKey::kEscape)) {
        running = false;
        break;
      }
    }
    if (!running) break;
    RenderFrame(gl, *window, mesh, normal_texture, diffuse_texture, angle);
    if (!context.Swap()) {
      ShowFailure(context.error().c_str());
      break;
    }
    angle += 0.012f;
    if (angle > 2.0f * kPi) angle -= 2.0f * kPi;
    User::After(22000);
  }
  return KErrNone;
}
}  // namespace

int RunApplication() {
  WindowSurface window;
  const auto opened = window.Open();
  if (!opened.ok()) {
    ShowFailure("Could not open Symbian window");
    return KErrNotSupported;
  }
  return Run(&window);
}
