#include "application.h"

#include <e32std.h>
#include <w32std.h>
#include <EGL/egl.h>
#include <GLES/gl.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "symbian/api/display/window_surface.h"

namespace {
using symbian::api::display::WindowInputKind;
using symbian::api::display::WindowKey;
using symbian::api::display::WindowSurface;

// This application intentionally resolves the Symbian 9.x combined EGL/GLES
// library at runtime. Older N95 ROMs provide libgles_cm.dll rather than the
// newer split libegl/libglesv1_cm imports used by desktop SDK samples.
class GlesApi final {
 public:
  using EglChooseConfigFn = decltype(&eglChooseConfig);
  using EglCreateContextFn = decltype(&eglCreateContext);
  using EglCreateWindowSurfaceFn = decltype(&eglCreateWindowSurface);
  using EglDestroyContextFn = decltype(&eglDestroyContext);
  using EglDestroySurfaceFn = decltype(&eglDestroySurface);
  using EglGetDisplayFn = decltype(&eglGetDisplay);
  using EglGetErrorFn = decltype(&eglGetError);
  using EglInitializeFn = decltype(&eglInitialize);
  using EglMakeCurrentFn = decltype(&eglMakeCurrent);
  using EglSwapBuffersFn = decltype(&eglSwapBuffers);
  using EglTerminateFn = decltype(&eglTerminate);
  using GlBlendFuncFn = decltype(&glBlendFunc);
  using GlClearFn = decltype(&glClear);
  using GlClearColorFn = decltype(&glClearColor);
  using GlClearStencilFn = decltype(&glClearStencil);
  using GlColor4fFn = decltype(&glColor4f);
  using GlColorMaskFn = decltype(&glColorMask);
  using GlColorPointerFn = decltype(&glColorPointer);
  using GlCullFaceFn = decltype(&glCullFace);
  using GlDepthFuncFn = decltype(&glDepthFunc);
  using GlDepthMaskFn = decltype(&glDepthMask);
  using GlDisableFn = decltype(&glDisable);
  using GlDisableClientStateFn = decltype(&glDisableClientState);
  using GlDrawArraysFn = decltype(&glDrawArrays);
  using GlEnableFn = decltype(&glEnable);
  using GlEnableClientStateFn = decltype(&glEnableClientState);
  using GlFrontFaceFn = decltype(&glFrontFace);
  using GlFrustumfFn = decltype(&glFrustumf);
  using GlGetIntegervFn = decltype(&glGetIntegerv);
  using GlLoadIdentityFn = decltype(&glLoadIdentity);
  using GlMatrixModeFn = decltype(&glMatrixMode);
  using GlRotatefFn = decltype(&glRotatef);
  using GlStencilFuncFn = decltype(&glStencilFunc);
  using GlStencilMaskFn = decltype(&glStencilMask);
  using GlStencilOpFn = decltype(&glStencilOp);
  using GlTranslatefFn = decltype(&glTranslatef);
  using GlVertexPointerFn = decltype(&glVertexPointer);
  using GlViewportFn = decltype(&glViewport);

  GlesApi() = default;
  GlesApi(const GlesApi&) = delete;
  GlesApi& operator=(const GlesApi&) = delete;
  ~GlesApi() { Close(); }

  bool Load() {
    const TInt result = library_.Load(_L("libgles_cm.dll"));
    if (result != KErrNone) {
      error_ = "RLibrary.Load(libgles_cm.dll) failed";
      return false;
    }
    loaded_ = true;
    // Ordinals are from SymbianSource opengles11u.def (the combined
    // EGL/GLES 1.1 DLL), not from the separate GLES-only proxy table.
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
           Bind(&gl_blend_func, 30, "glBlendFunc") &&
           Bind(&gl_clear, 31, "glClear") &&
           Bind(&gl_clear_color, 32, "glClearColor") &&
           Bind(&gl_clear_stencil, 36, "glClearStencil") &&
           Bind(&gl_color4f, 38, "glColor4f") &&
           Bind(&gl_color_mask, 40, "glColorMask") &&
           Bind(&gl_color_pointer, 41, "glColorPointer") &&
           Bind(&gl_cull_face, 46, "glCullFace") &&
           Bind(&gl_depth_func, 48, "glDepthFunc") &&
           Bind(&gl_depth_mask, 49, "glDepthMask") &&
           Bind(&gl_disable, 52, "glDisable") &&
           Bind(&gl_disable_client_state, 53, "glDisableClientState") &&
           Bind(&gl_draw_arrays, 54, "glDrawArrays") &&
           Bind(&gl_enable, 56, "glEnable") &&
           Bind(&gl_enable_client_state, 57, "glEnableClientState") &&
           Bind(&gl_front_face, 64, "glFrontFace") &&
           Bind(&gl_frustumf, 65, "glFrustumf") &&
           Bind(&gl_get_integerv, 69, "glGetIntegerv") &&
           Bind(&gl_load_identity, 82, "glLoadIdentity") &&
           Bind(&gl_matrix_mode, 90, "glMatrixMode") &&
           Bind(&gl_rotatef, 109, "glRotatef") &&
           Bind(&gl_stencil_func, 117, "glStencilFunc") &&
           Bind(&gl_stencil_mask, 118, "glStencilMask") &&
           Bind(&gl_stencil_op, 119, "glStencilOp") &&
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
  GlBlendFuncFn gl_blend_func = nullptr;
  GlClearFn gl_clear = nullptr;
  GlClearColorFn gl_clear_color = nullptr;
  GlClearStencilFn gl_clear_stencil = nullptr;
  GlColor4fFn gl_color4f = nullptr;
  GlColorMaskFn gl_color_mask = nullptr;
  GlColorPointerFn gl_color_pointer = nullptr;
  GlCullFaceFn gl_cull_face = nullptr;
  GlDepthFuncFn gl_depth_func = nullptr;
  GlDepthMaskFn gl_depth_mask = nullptr;
  GlDisableFn gl_disable = nullptr;
  GlDisableClientStateFn gl_disable_client_state = nullptr;
  GlDrawArraysFn gl_draw_arrays = nullptr;
  GlEnableFn gl_enable = nullptr;
  GlEnableClientStateFn gl_enable_client_state = nullptr;
  GlFrontFaceFn gl_front_face = nullptr;
  GlFrustumfFn gl_frustumf = nullptr;
  GlGetIntegervFn gl_get_integerv = nullptr;
  GlLoadIdentityFn gl_load_identity = nullptr;
  GlMatrixModeFn gl_matrix_mode = nullptr;
  GlRotatefFn gl_rotatef = nullptr;
  GlStencilFuncFn gl_stencil_func = nullptr;
  GlStencilMaskFn gl_stencil_mask = nullptr;
  GlStencilOpFn gl_stencil_op = nullptr;
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

    // Request the native window's color depth (N95 normally uses EColor64K),
    // plus depth and an 8-bit stencil plane. No ES2 context attributes/APIs.
    EGLint buffer_bits = 32;
    switch (static_cast<RWindow*>(window->NativeWindowHandle())->DisplayMode()) {
      case EColor4K:
        buffer_bits = 12;
        break;
      case EColor64K:
        buffer_bits = 16;
        break;
      case EColor16M:
        buffer_bits = 24;
        break;
      default:
        buffer_bits = 32;
        break;
    }
    const EGLint config_attributes[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BUFFER_SIZE, buffer_bits,
        EGL_DEPTH_SIZE, 16,
        EGL_STENCIL_SIZE, 8,
        EGL_NONE};
    EGLConfig config = 0;
    EGLint config_count = 0;
    if (!api_.egl_choose_config(display_, config_attributes, &config, 1,
                                &config_count)) {
      return Fail("eglChooseConfig");
    }
    if (config_count == 0) {
      error_ = "Stencil buffer unavailable (no matching EGL config)";
      Close();
      return false;
    }

    surface_ = api_.egl_create_window_surface(
        display_, config,
        static_cast<EGLNativeWindowType>(window->NativeWindowHandle()), nullptr);
    if (surface_ == EGL_NO_SURFACE) return Fail("eglCreateWindowSurface");
    // A null attribute list creates the default fixed-function GLES 1.x context.
    context_ = api_.egl_create_context(display_, config, EGL_NO_CONTEXT, nullptr);
    if (context_ == EGL_NO_CONTEXT) return Fail("eglCreateContext");
    if (!api_.egl_make_current(display_, surface_, surface_, context_)) {
      return Fail("eglMakeCurrent");
    }
    GLint stencil_bits = 0;
    api_.gl_get_integerv(GL_STENCIL_BITS, &stencil_bits);
    if (stencil_bits < 1) {
      error_ = "EGL config created without a usable stencil buffer";
      Close();
      return false;
    }
    return true;
  }

  bool Swap() {
    if (display_ != EGL_NO_DISPLAY && surface_ != EGL_NO_SURFACE &&
        api_.egl_swap_buffers(display_, surface_)) {
      return true;
    }
    return Fail("eglSwapBuffers");
  }

  void Close() {
    if (display_ != EGL_NO_DISPLAY) {
      if (context_ != EGL_NO_CONTEXT) {
        api_.egl_make_current(display_, EGL_NO_SURFACE, EGL_NO_SURFACE,
                              EGL_NO_CONTEXT);
        api_.egl_destroy_context(display_, context_);
      }
      if (surface_ != EGL_NO_SURFACE) {
        api_.egl_destroy_surface(display_, surface_);
      }
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
    char error_code[16];
    const EGLint code = api_.egl_get_error != nullptr ? api_.egl_get_error() : 0;
    std::snprintf(error_code, sizeof(error_code), "0x%04x",
                  static_cast<unsigned int>(code));
    error_ = std::string(stage) + " failed (EGL " + error_code + ")";
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

struct Vec3 {
  float x;
  float y;
  float z;
};

Vec3 Add(Vec3 a, Vec3 b) { return {a.x + b.x, a.y + b.y, a.z + b.z}; }
Vec3 Sub(Vec3 a, Vec3 b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
Vec3 Mul(Vec3 a, float s) { return {a.x * s, a.y * s, a.z * s}; }
float Dot(Vec3 a, Vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
Vec3 Cross(Vec3 a, Vec3 b) {
  return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
          a.x * b.y - a.y * b.x};
}
float Length(Vec3 v) { return std::sqrt(Dot(v, v)); }
Vec3 Normalize(Vec3 v) {
  const float length = Length(v);
  return length > 0.0001f ? Mul(v, 1.0f / length) : Vec3{0, 0, 1};
}

struct Triangle {
  int a;
  int b;
  int c;
};

struct Edge {
  int a0;
  int b0;
  int triangle0;
  int a1;
  int b1;
  int triangle1;
};

struct GearTopology {
  std::vector<Vec3> vertices;
  std::vector<Triangle> triangles;
  std::vector<Edge> edges;
  GLubyte red;
  GLubyte green;
  GLubyte blue;
};

struct WorldMesh {
  const GearTopology* topology;
  std::vector<Vec3> vertices;
};

GearTopology MakeGear(int tooth_count, float root_radius, float tip_radius,
                      float half_depth, GLubyte red, GLubyte green,
                      GLubyte blue) {
  GearTopology mesh;
  mesh.red = red;
  mesh.green = green;
  mesh.blue = blue;
  const float step = 6.28318530718f / static_cast<float>(tooth_count);
  const float fractions[] = {0.0f, 0.16f, 0.24f, 0.54f, 0.62f, 0.86f};
  const float radii[] = {root_radius, root_radius, tip_radius,
                         tip_radius, root_radius, root_radius};
  const int outline_count = tooth_count * 6;
  mesh.vertices.reserve(outline_count * 2 + 2);
  for (int side = 0; side < 2; ++side) {
    const float y = side == 0 ? half_depth : -half_depth;
    for (int tooth = 0; tooth < tooth_count; ++tooth) {
      for (int point = 0; point < 6; ++point) {
        const float angle = (static_cast<float>(tooth) + fractions[point]) * step;
        mesh.vertices.push_back(
            {radii[point] * std::cos(angle), y, radii[point] * std::sin(angle)});
      }
    }
  }
  const int top_center = outline_count * 2;
  const int bottom_center = top_center + 1;
  mesh.vertices.push_back({0, half_depth, 0});
  mesh.vertices.push_back({0, -half_depth, 0});

  const int bottom_offset = outline_count;
  for (int point = 0; point < outline_count; ++point) {
    const int next = (point + 1) % outline_count;
    // Top and bottom caps are wound outwards; the perimeter is shared with
    // the vertical tooth faces so the caster remains a closed manifold.
    mesh.triangles.push_back({top_center, next, point});
    mesh.triangles.push_back({bottom_center, bottom_offset + point,
                              bottom_offset + next});
    mesh.triangles.push_back({point, next, bottom_offset + next});
    mesh.triangles.push_back({point, bottom_offset + next,
                              bottom_offset + point});
  }

  std::map<std::pair<int, int>, int> edge_indices;
  for (std::size_t triangle_index = 0;
       triangle_index < mesh.triangles.size(); ++triangle_index) {
    const Triangle triangle = mesh.triangles[triangle_index];
    const int edge_a[] = {triangle.a, triangle.b, triangle.c};
    const int edge_b[] = {triangle.b, triangle.c, triangle.a};
    for (int edge_index = 0; edge_index < 3; ++edge_index) {
      const int a = edge_a[edge_index];
      const int b = edge_b[edge_index];
      const std::pair<int, int> key =
          a < b ? std::make_pair(a, b) : std::make_pair(b, a);
      const auto found = edge_indices.find(key);
      if (found == edge_indices.end()) {
        const int index = static_cast<int>(mesh.edges.size());
        edge_indices.emplace(key, index);
        mesh.edges.push_back({a, b, static_cast<int>(triangle_index), -1, -1,
                              -1});
      } else {
        Edge& shared = mesh.edges[found->second];
        shared.a1 = a;
        shared.b1 = b;
        shared.triangle1 = static_cast<int>(triangle_index);
      }
    }
  }
  return mesh;
}

WorldMesh TransformMesh(const GearTopology& topology, Vec3 position,
                        float angle_radians) {
  WorldMesh world{&topology, {}};
  world.vertices.reserve(topology.vertices.size());
  const float cosine = std::cos(angle_radians);
  const float sine = std::sin(angle_radians);
  for (const Vec3 local : topology.vertices) {
    world.vertices.push_back(
        {position.x + local.x * cosine + local.z * sine,
         position.y + local.y,
         position.z - local.x * sine + local.z * cosine});
  }
  return world;
}

struct SceneVertex {
  GLfloat x;
  GLfloat y;
  GLfloat z;
  GLubyte r;
  GLubyte g;
  GLubyte b;
  GLubyte a;
};

void AppendSceneTriangle(std::vector<SceneVertex>* vertices, Vec3 a, Vec3 b,
                         Vec3 c, GLubyte r, GLubyte g, GLubyte blue) {
  const SceneVertex points[] = {
      {a.x, a.y, a.z, r, g, blue, 255},
      {b.x, b.y, b.z, r, g, blue, 255},
      {c.x, c.y, c.z, r, g, blue, 255},
  };
  vertices->insert(vertices->end(), points, points + 3);
}

GLubyte ShadeChannel(GLubyte channel, float illumination) {
  return static_cast<GLubyte>(std::min(255.0f, channel * illumination));
}

void AppendLitMesh(std::vector<SceneVertex>* scene, const WorldMesh& mesh,
                   Vec3 light) {
  const GearTopology& topology = *mesh.topology;
  for (const Triangle triangle : topology.triangles) {
    const Vec3 a = mesh.vertices[triangle.a];
    const Vec3 b = mesh.vertices[triangle.b];
    const Vec3 c = mesh.vertices[triangle.c];
    const Vec3 normal = Normalize(Cross(Sub(b, a), Sub(c, a)));
    const Vec3 center = Mul(Add(Add(a, b), c), 1.0f / 3.0f);
    const float diffuse = std::max(0.0f, Dot(normal, Normalize(Sub(light, center))));
    const float illumination = 0.32f + 0.68f * diffuse;
    AppendSceneTriangle(scene, a, b, c,
                        ShadeChannel(topology.red, illumination),
                        ShadeChannel(topology.green, illumination),
                        ShadeChannel(topology.blue, illumination));
  }
}

std::vector<SceneVertex> BuildScene(const WorldMesh& large_gear,
                                    const WorldMesh& small_gear, Vec3 light) {
  std::vector<SceneVertex> vertices;
  vertices.reserve(1100);
  for (int z = -4; z < 4; ++z) {
    for (int x = -4; x < 4; ++x) {
      const GLubyte shade = ((x + z) & 1) ? 68 : 86;
      const float dx = static_cast<float>(x) - light.x;
      const float dz = static_cast<float>(z) - light.z;
      const GLubyte lit = static_cast<GLubyte>(std::min(
          126.0f, shade + 38.0f / (1.0f + 0.10f * (dx * dx + dz * dz))));
      const Vec3 a{static_cast<float>(x), 0, static_cast<float>(z)};
      const Vec3 b{static_cast<float>(x + 1), 0, static_cast<float>(z)};
      const Vec3 c{static_cast<float>(x + 1), 0, static_cast<float>(z + 1)};
      const Vec3 d{static_cast<float>(x), 0, static_cast<float>(z + 1)};
      AppendSceneTriangle(&vertices, a, c, b, lit, lit, lit);
      AppendSceneTriangle(&vertices, a, d, c, lit, lit, lit);
    }
  }
  AppendLitMesh(&vertices, large_gear, light);
  AppendLitMesh(&vertices, small_gear, light);
  return vertices;
}

void AppendTriangle(std::vector<GLfloat>* output, Vec3 a, Vec3 b, Vec3 c) {
  const GLfloat triangle[] = {a.x, a.y, a.z, b.x, b.y, b.z, c.x, c.y, c.z};
  output->insert(output->end(), triangle, triangle + 9);
}

void AppendShadowVolume(std::vector<GLfloat>* output, const WorldMesh& mesh,
                        Vec3 light) {
  constexpr float kExtrusionDistance = 48.0f;
  constexpr float kEpsilon = 0.0001f;
  const GearTopology& topology = *mesh.topology;
  std::vector<GLubyte> facing(topology.triangles.size(), 0);

  for (std::size_t index = 0; index < topology.triangles.size(); ++index) {
    const Triangle triangle = topology.triangles[index];
    const Vec3 a = mesh.vertices[triangle.a];
    const Vec3 b = mesh.vertices[triangle.b];
    const Vec3 c = mesh.vertices[triangle.c];
    const Vec3 normal = Cross(Sub(b, a), Sub(c, a));
    const Vec3 center = Mul(Add(Add(a, b), c), 1.0f / 3.0f);
    facing[index] = Dot(normal, Sub(light, center)) > kEpsilon ? 1 : 0;
    if (facing[index] == 0) continue;

    // The light-facing triangles form the near cap; the reversed projected
    // copy is the far cap needed for the two-sided z-fail stencil pass.
    AppendTriangle(output, a, b, c);
    const Vec3 far_a = Add(a, Mul(Normalize(Sub(a, light)), kExtrusionDistance));
    const Vec3 far_b = Add(b, Mul(Normalize(Sub(b, light)), kExtrusionDistance));
    const Vec3 far_c = Add(c, Mul(Normalize(Sub(c, light)), kExtrusionDistance));
    AppendTriangle(output, far_c, far_b, far_a);
  }

  for (const Edge& edge : topology.edges) {
    if (edge.triangle1 < 0 ||
        facing[edge.triangle0] == facing[edge.triangle1]) {
      continue;
    }
    const bool first_is_lit = facing[edge.triangle0] != 0;
    const int index_a = first_is_lit ? edge.a0 : edge.a1;
    const int index_b = first_is_lit ? edge.b0 : edge.b1;
    const Vec3 a = mesh.vertices[index_a];
    const Vec3 b = mesh.vertices[index_b];
    const Vec3 far_a = Add(a, Mul(Normalize(Sub(a, light)), kExtrusionDistance));
    const Vec3 far_b = Add(b, Mul(Normalize(Sub(b, light)), kExtrusionDistance));
    AppendTriangle(output, a, far_a, far_b);
    AppendTriangle(output, a, far_b, b);
  }
}

std::vector<GLfloat> BuildShadowVolumes(const WorldMesh& large_gear,
                                        const WorldMesh& small_gear,
                                        Vec3 light) {
  std::vector<GLfloat> volume;
  volume.reserve(18000);
  AppendShadowVolume(&volume, large_gear, light);
  AppendShadowVolume(&volume, small_gear, light);
  return volume;
}

void DrawScene(GlesApi& gl, const std::vector<SceneVertex>& vertices) {
  gl.gl_enable_client_state(GL_VERTEX_ARRAY);
  gl.gl_enable_client_state(GL_COLOR_ARRAY);
  gl.gl_vertex_pointer(3, GL_FLOAT, sizeof(SceneVertex), &vertices[0].x);
  gl.gl_color_pointer(4, GL_UNSIGNED_BYTE, sizeof(SceneVertex), &vertices[0].r);
  gl.gl_draw_arrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));
  gl.gl_disable_client_state(GL_COLOR_ARRAY);
  gl.gl_disable_client_state(GL_VERTEX_ARRAY);
}

void DrawVolume(GlesApi& gl, const std::vector<GLfloat>& vertices) {
  if (vertices.empty()) return;
  gl.gl_enable_client_state(GL_VERTEX_ARRAY);
  gl.gl_vertex_pointer(3, GL_FLOAT, 0, vertices.data());
  gl.gl_draw_arrays(GL_TRIANGLES, 0,
                    static_cast<GLsizei>(vertices.size() / 3));
  gl.gl_disable_client_state(GL_VERTEX_ARRAY);
}

void DrawDarkOverlay(GlesApi& gl) {
  const GLfloat fullscreen[] = {
      -1, -1, 1, -1, 1, 1,
      -1, -1, 1, 1, -1, 1,
  };
  gl.gl_disable(GL_DEPTH_TEST);
  gl.gl_depth_mask(GL_FALSE);
  gl.gl_stencil_mask(0x00);
  gl.gl_stencil_func(GL_NOTEQUAL, 0, 0xff);
  gl.gl_stencil_op(GL_KEEP, GL_KEEP, GL_KEEP);
  gl.gl_enable(GL_BLEND);
  gl.gl_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  gl.gl_color4f(0.015f, 0.02f, 0.035f, 0.62f);
  gl.gl_matrix_mode(GL_PROJECTION);
  gl.gl_load_identity();
  gl.gl_matrix_mode(GL_MODELVIEW);
  gl.gl_load_identity();
  gl.gl_enable_client_state(GL_VERTEX_ARRAY);
  gl.gl_vertex_pointer(2, GL_FLOAT, 0, fullscreen);
  gl.gl_draw_arrays(GL_TRIANGLES, 0, 6);
  gl.gl_disable_client_state(GL_VERTEX_ARRAY);
  gl.gl_disable(GL_BLEND);
  gl.gl_disable(GL_STENCIL_TEST);
  gl.gl_depth_mask(GL_TRUE);
}

void RenderFrame(GlesApi& gl, const WindowSurface& window, float angle) {
  const int width = std::max(1, window.size().width);
  const int height = std::max(1, window.size().height);
  const float aspect = static_cast<float>(width) / static_cast<float>(height);
  const Vec3 light{2.6f + 1.4f * std::sin(angle * 0.7f), 4.2f,
                   1.8f + 1.0f * std::cos(angle * 0.43f)};
  static const GearTopology large_topology =
      MakeGear(14, 1.02f, 1.32f, 0.22f, 172, 98, 50);
  static const GearTopology small_topology =
      MakeGear(9, 0.55f, 0.76f, 0.17f, 72, 132, 172);
  const WorldMesh large_gear = TransformMesh(
      large_topology, {-1.22f, 0.22f, 0.0f}, angle);
  const WorldMesh small_gear = TransformMesh(
      small_topology, {1.05f, 0.17f, 0.0f}, -angle * (14.0f / 9.0f));
  const std::vector<SceneVertex> scene = BuildScene(large_gear, small_gear, light);
  const std::vector<GLfloat> volume =
      BuildShadowVolumes(large_gear, small_gear, light);

  gl.gl_viewport(0, 0, width, height);
  gl.gl_clear_color(0.075f, 0.11f, 0.16f, 1.0f);
  gl.gl_clear_stencil(0);
  gl.gl_stencil_mask(0xff);
  gl.gl_clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  gl.gl_enable(GL_DEPTH_TEST);
  gl.gl_depth_func(GL_LEQUAL);
  gl.gl_disable(GL_BLEND);
  gl.gl_disable(GL_TEXTURE_2D);
  gl.gl_enable(GL_CULL_FACE);
  gl.gl_front_face(GL_CCW);
  gl.gl_cull_face(GL_BACK);
  gl.gl_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  gl.gl_depth_mask(GL_TRUE);

  gl.gl_matrix_mode(GL_PROJECTION);
  gl.gl_load_identity();
  const float half_height = 0.86f;
  gl.gl_frustumf(-half_height * aspect, half_height * aspect,
                 -half_height, half_height, 1.0f, 100.0f);
  gl.gl_matrix_mode(GL_MODELVIEW);
  gl.gl_load_identity();
  gl.gl_translatef(0.0f, -2.0f, -9.3f);
  gl.gl_rotatef(17.0f, 1.0f, 0.0f, 0.0f);
  DrawScene(gl, scene);

  // This is the z-fail version of the source sample's stencil pass. The
  // CPU-built caps make it robust when a shadow volume crosses the near plane;
  // no OES matrix-palette or VBO extension is required to extrude vertices.
  gl.gl_enable(GL_STENCIL_TEST);
  gl.gl_color_mask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  gl.gl_depth_mask(GL_FALSE);
  gl.gl_stencil_mask(0xff);
  gl.gl_stencil_func(GL_ALWAYS, 0, 0xff);
  gl.gl_cull_face(GL_BACK);
  gl.gl_stencil_op(GL_KEEP, GL_INCR, GL_KEEP);
  DrawVolume(gl, volume);
  gl.gl_cull_face(GL_FRONT);
  gl.gl_stencil_op(GL_KEEP, GL_DECR, GL_KEEP);
  DrawVolume(gl, volume);
  gl.gl_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  gl.gl_cull_face(GL_BACK);
  DrawDarkOverlay(gl);
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
  bool running = true;
  float angle = 0.0f;
  while (running) {
    for (int count = 0; count < 32; ++count) {
      auto event = window->PollInput();
      if (!event.ok()) {
        running = false;
        break;
      }
      if (!event->has_value()) break;
      const auto& input = **event;
      if (input.kind == WindowInputKind::kCloseRequested ||
          (input.kind == WindowInputKind::kKeyDown &&
           input.key == WindowKey::kEscape)) {
        running = false;
        break;
      }
      if (input.kind == WindowInputKind::kDisplayChanged) {
        // Viewport and projection are recalculated during the next frame.
      }
    }
    if (!running) break;
    RenderFrame(gl, *window, angle);
    if (!context.Swap()) {
      ShowFailure(context.error().c_str());
      break;
    }
    angle += 0.018f;
    if (angle > 6.2831853f) angle -= 6.2831853f;
    User::After(18000);
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
