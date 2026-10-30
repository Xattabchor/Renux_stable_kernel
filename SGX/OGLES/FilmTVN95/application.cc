#include "application.h"

#include <e32std.h>
#include <w32std.h>
#include <EGL/egl.h>
#include <GLES/gl.h>
#include <GLES/glext.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <string>

#include "symbian/api/display/window_surface.h"

namespace {
using symbian::api::display::WindowInputKind;
using symbian::api::display::WindowKey;
using symbian::api::display::WindowSurface;

// Resolve the combined Symbian EGL/GLES 1.1 library used by N95 firmware.
// No ES2 or post-EGL-1.1 entry points are imported by this application.
class GlesApi final {
 public:
  using EglChooseConfigFn = decltype(&eglChooseConfig);
  using EglCreateContextFn = decltype(&eglCreateContext);
  using EglCreateWindowSurfaceFn = decltype(&eglCreateWindowSurface);
  using EglDestroyContextFn = decltype(&eglDestroyContext);
  using EglDestroySurfaceFn = decltype(&eglDestroySurface);
  using EglGetErrorFn = decltype(&eglGetError);
  using EglGetProcAddressFn = decltype(&eglGetProcAddress);
  using EglInitializeFn = decltype(&eglInitialize);
  using EglMakeCurrentFn = decltype(&eglMakeCurrent);
  using EglSwapBuffersFn = decltype(&eglSwapBuffers);
  using EglTerminateFn = decltype(&eglTerminate);
  using GlBindTextureFn = decltype(&glBindTexture);
  using GlClearFn = decltype(&glClear);
  using GlClearColorFn = decltype(&glClearColor);
  using GlColor4fFn = decltype(&glColor4f);
  using GlDeleteTexturesFn = decltype(&glDeleteTextures);
  using GlDisableFn = decltype(&glDisable);
  using GlDisableClientStateFn = decltype(&glDisableClientState);
  using GlDrawArraysFn = decltype(&glDrawArrays);
  using GlEnableFn = decltype(&glEnable);
  using GlEnableClientStateFn = decltype(&glEnableClientState);
  using GlGenTexturesFn = decltype(&glGenTextures);
  using GlGetStringFn = decltype(&glGetString);
  using GlLoadIdentityFn = decltype(&glLoadIdentity);
  using GlMatrixModeFn = decltype(&glMatrixMode);
  using GlOrthofFn = decltype(&glOrthof);
  using GlTexCoordPointerFn = decltype(&glTexCoordPointer);
  using GlTexEnvfFn = decltype(&glTexEnvf);
  using GlTexImage2DFn = decltype(&glTexImage2D);
  using GlTexParameterfFn = decltype(&glTexParameterf);
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
    return Bind(&egl_choose_config, 1, "eglChooseConfig") &&
           Bind(&egl_create_context, 3, "eglCreateContext") &&
           Bind(&egl_create_window_surface, 6, "eglCreateWindowSurface") &&
           Bind(&egl_destroy_context, 7, "eglDestroyContext") &&
           Bind(&egl_destroy_surface, 8, "eglDestroySurface") &&
           Bind(&egl_get_display, 14, "eglGetDisplay") &&
           Bind(&egl_get_error, 15, "eglGetError") &&
           Bind(&egl_get_proc_address, 16, "eglGetProcAddress") &&
           Bind(&egl_initialize, 17, "eglInitialize") &&
           Bind(&egl_make_current, 18, "eglMakeCurrent") &&
           Bind(&egl_swap_buffers, 22, "eglSwapBuffers") &&
           Bind(&egl_terminate, 23, "eglTerminate") &&
           Bind(&gl_bind_texture, 29, "glBindTexture") &&
           Bind(&gl_clear, 31, "glClear") &&
           Bind(&gl_clear_color, 32, "glClearColor") &&
           Bind(&gl_color4f, 38, "glColor4f") &&
           Bind(&gl_delete_textures, 47, "glDeleteTextures") &&
           Bind(&gl_disable, 52, "glDisable") &&
           Bind(&gl_disable_client_state, 53, "glDisableClientState") &&
           Bind(&gl_draw_arrays, 54, "glDrawArrays") &&
           Bind(&gl_enable, 56, "glEnable") &&
           Bind(&gl_enable_client_state, 57, "glEnableClientState") &&
           Bind(&gl_gen_textures, 67, "glGenTextures") &&
           Bind(&gl_get_string, 70, "glGetString") &&
           Bind(&gl_load_identity, 82, "glLoadIdentity") &&
           Bind(&gl_matrix_mode, 90, "glMatrixMode") &&
           Bind(&gl_ortho, 98, "glOrthof") &&
           Bind(&gl_tex_coord_pointer, 120, "glTexCoordPointer") &&
           Bind(&gl_tex_envf, 121, "glTexEnvf") &&
           Bind(&gl_tex_image_2d, 125, "glTexImage2D") &&
           Bind(&gl_tex_parameterf, 126, "glTexParameterf") &&
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
  EglGetProcAddressFn egl_get_proc_address = nullptr;
  EglInitializeFn egl_initialize = nullptr;
  EglChooseConfigFn egl_choose_config = nullptr;
  EglCreateWindowSurfaceFn egl_create_window_surface = nullptr;
  EglCreateContextFn egl_create_context = nullptr;
  EglMakeCurrentFn egl_make_current = nullptr;
  EglSwapBuffersFn egl_swap_buffers = nullptr;
  EglDestroyContextFn egl_destroy_context = nullptr;
  EglDestroySurfaceFn egl_destroy_surface = nullptr;
  EglTerminateFn egl_terminate = nullptr;
  GlBindTextureFn gl_bind_texture = nullptr;
  GlClearFn gl_clear = nullptr;
  GlClearColorFn gl_clear_color = nullptr;
  GlColor4fFn gl_color4f = nullptr;
  GlDeleteTexturesFn gl_delete_textures = nullptr;
  GlDisableFn gl_disable = nullptr;
  GlDisableClientStateFn gl_disable_client_state = nullptr;
  GlDrawArraysFn gl_draw_arrays = nullptr;
  GlEnableFn gl_enable = nullptr;
  GlEnableClientStateFn gl_enable_client_state = nullptr;
  GlGenTexturesFn gl_gen_textures = nullptr;
  GlGetStringFn gl_get_string = nullptr;
  GlLoadIdentityFn gl_load_identity = nullptr;
  GlMatrixModeFn gl_matrix_mode = nullptr;
  GlOrthofFn gl_ortho = nullptr;
  GlTexCoordPointerFn gl_tex_coord_pointer = nullptr;
  GlTexEnvfFn gl_tex_envf = nullptr;
  GlTexImage2DFn gl_tex_image_2d = nullptr;
  GlTexParameterfFn gl_tex_parameterf = nullptr;
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
    EGLint major = 0, minor = 0;
    if (!api_.egl_initialize(display_, &major, &minor)) return Fail("eglInitialize");
    initialized_ = true;

    EGLint buffer_bits = 32;
    switch (static_cast<RWindow*>(window->NativeWindowHandle())->DisplayMode()) {
      case EColor4K: buffer_bits = 12; break;
      case EColor64K: buffer_bits = 16; break;
      case EColor16M: buffer_bits = 24; break;
      default: buffer_bits = 32; break;
    }
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

struct FboApi {
  using GenFn = void (GL_APIENTRY *)(GLsizei, GLuint*);
  using DeleteFn = void (GL_APIENTRY *)(GLsizei, const GLuint*);
  using BindFn = void (GL_APIENTRY *)(GLenum, GLuint);
  using AttachFn = void (GL_APIENTRY *)(GLenum, GLenum, GLenum, GLuint, GLint);
  using StatusFn = GLenum (GL_APIENTRY *)(GLenum);
  GenFn gen = nullptr;
  DeleteFn remove = nullptr;
  BindFn bind = nullptr;
  AttachFn attach_texture = nullptr;
  StatusFn status = nullptr;

  template <typename Function>
  bool Resolve(GlesApi& gl, Function* out, const char* name) {
    const __eglMustCastToProperFunctionPointerType address =
        gl.egl_get_proc_address(name);
    if (address == nullptr) return false;
    *out = reinterpret_cast<Function>(address);
    return true;
  }

  bool Load(GlesApi& gl) {
    return Resolve(gl, &gen, "glGenFramebuffersOES") &&
           Resolve(gl, &remove, "glDeleteFramebuffersOES") &&
           Resolve(gl, &bind, "glBindFramebufferOES") &&
           Resolve(gl, &attach_texture, "glFramebufferTexture2DOES") &&
           Resolve(gl, &status, "glCheckFramebufferStatusOES");
  }
};

bool HasExtension(const char* list, const char* extension) {
  if (list == nullptr || extension == nullptr || std::strchr(extension, ' ')) return false;
  const std::size_t length = std::strlen(extension);
  const char* current = list;
  while ((current = std::strstr(current, extension)) != nullptr) {
    const bool starts_token = current == list || current[-1] == ' ';
    const char after = current[length];
    if (starts_token && (after == '\0' || after == ' ')) return true;
    current += length;
  }
  return false;
}

constexpr float kPi = 3.14159265358979323846f;
constexpr int kTextureSize = 128;

void Set2D(GlesApi& gl, int width, int height) {
  const float aspect = static_cast<float>(std::max(1, width)) /
                       static_cast<float>(std::max(1, height));
  gl.gl_matrix_mode(GL_PROJECTION);
  gl.gl_load_identity();
  gl.gl_ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
  gl.gl_matrix_mode(GL_MODELVIEW);
  gl.gl_load_identity();
}

void DrawQuad(GlesApi& gl, float left, float bottom, float right, float top,
              const GLfloat color[4], GLuint texture = 0) {
  const GLfloat vertices[] = {left, bottom, left, top, right, bottom, right, top};
  static const GLfloat texcoords[] = {0, 0, 0, 1, 1, 0, 1, 1};
  gl.gl_color4f(color[0], color[1], color[2], color[3]);
  gl.gl_vertex_pointer(2, GL_FLOAT, 0, vertices);
  if (texture != 0) {
    gl.gl_enable(GL_TEXTURE_2D);
    gl.gl_bind_texture(GL_TEXTURE_2D, texture);
    gl.gl_enable_client_state(GL_TEXTURE_COORD_ARRAY);
    gl.gl_tex_coord_pointer(2, GL_FLOAT, 0, texcoords);
  } else {
    gl.gl_disable(GL_TEXTURE_2D);
    gl.gl_disable_client_state(GL_TEXTURE_COORD_ARRAY);
  }
  gl.gl_draw_arrays(GL_TRIANGLE_STRIP, 0, 4);
}

void DrawCircle(GlesApi& gl, float cx, float cy, float radius,
                const GLfloat color[4], float phase) {
  const int segments = 24;
  GLfloat vertices[(segments + 2) * 2];
  vertices[0] = cx;
  vertices[1] = cy;
  for (int i = 0; i <= segments; ++i) {
    const float angle = phase + 2.0f * kPi * static_cast<float>(i) /
                                     static_cast<float>(segments);
    vertices[(i + 1) * 2] = cx + radius * std::cos(angle);
    vertices[(i + 1) * 2 + 1] = cy + radius * std::sin(angle);
  }
  gl.gl_color4f(color[0], color[1], color[2], color[3]);
  gl.gl_disable(GL_TEXTURE_2D);
  gl.gl_disable_client_state(GL_TEXTURE_COORD_ARRAY);
  gl.gl_vertex_pointer(2, GL_FLOAT, 0, vertices);
  gl.gl_draw_arrays(GL_TRIANGLE_FAN, 0, segments + 2);
}

GLuint MakeTargetTexture(GlesApi& gl) {
  GLuint texture = 0;
  gl.gl_gen_textures(1, &texture);
  gl.gl_bind_texture(GL_TEXTURE_2D, texture);
  gl.gl_tex_parameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  gl.gl_tex_parameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  gl.gl_tex_parameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  gl.gl_tex_parameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  gl.gl_tex_image_2d(GL_TEXTURE_2D, 0, GL_RGB, kTextureSize, kTextureSize, 0,
                     GL_RGB, GL_UNSIGNED_SHORT_5_6_5, nullptr);
  return texture;
}

bool CreateTargets(GlesApi& gl, FboApi& fbo, GLuint textures[2],
                   GLuint framebuffers[2], std::string* error) {
  fbo.gen(2, framebuffers);
  for (int i = 0; i < 2; ++i) {
    textures[i] = MakeTargetTexture(gl);
    fbo.bind(GL_FRAMEBUFFER_OES, framebuffers[i]);
    fbo.attach_texture(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES,
                       GL_TEXTURE_2D, textures[i], 0);
    if (fbo.status(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES) {
      *error = "FilmTV framebuffer is incomplete on this GLES driver";
      fbo.bind(GL_FRAMEBUFFER_OES, 0);
      return false;
    }
  }
  fbo.bind(GL_FRAMEBUFFER_OES, 0);
  return true;
}

void RenderFilmTexture(GlesApi& gl, const FboApi& fbo,
                       const GLuint framebuffers[2], const GLuint textures[2],
                       int current, int previous, unsigned int frame,
                       float time) {
  fbo.bind(GL_FRAMEBUFFER_OES, framebuffers[current]);
  gl.gl_viewport(0, 0, kTextureSize, kTextureSize);
  Set2D(gl, kTextureSize, kTextureSize);
  gl.gl_disable(GL_DEPTH_TEST);
  gl.gl_disable(GL_CULL_FACE);
  gl.gl_clear_color(0.06f, 0.19f, 0.34f, 1.0f);
  gl.gl_clear(GL_COLOR_BUFFER_BIT);
  gl.gl_enable_client_state(GL_VERTEX_ARRAY);
  gl.gl_disable(GL_TEXTURE_2D);

  const GLfloat sky[4] = {0.07f, 0.24f, 0.43f, 1.0f};
  const GLfloat floor[4] = {0.24f, 0.16f, 0.09f, 1.0f};
  const GLfloat horizon[4] = {0.95f, 0.58f, 0.16f, 1.0f};
  const GLfloat dark[4] = {0.055f, 0.07f, 0.10f, 1.0f};
  const GLfloat light[4] = {0.83f, 0.86f, 0.80f, 1.0f};
  const float aspect = 1.0f;
  DrawQuad(gl, -aspect, -0.22f, aspect, 1.0f, sky);
  DrawQuad(gl, -aspect, -1.0f, aspect, -0.22f, floor);
  DrawQuad(gl, -aspect, -0.24f, aspect, -0.20f, horizon);

  // A tiny television in the off-screen scene shows the previous render.
  // Ping-pong targets avoid sampling from the texture currently being rendered.
  DrawQuad(gl, -0.53f, -0.60f, 0.53f, 0.47f, dark);
  if (frame != 0) {
    DrawQuad(gl, -0.46f, -0.49f, 0.46f, 0.39f, light, textures[previous]);
  } else {
    const GLfloat first_picture[4] = {0.10f, 0.58f, 0.82f, 1.0f};
    DrawQuad(gl, -0.46f, -0.49f, 0.46f, 0.39f, first_picture);
  }
  const GLfloat stand[4] = {0.19f, 0.14f, 0.10f, 1.0f};
  DrawQuad(gl, -0.19f, -0.73f, 0.19f, -0.60f, stand);
  DrawQuad(gl, -0.36f, -0.78f, 0.36f, -0.73f, stand);

  // Colorful drifting shapes make the off-screen image visibly animate.
  const GLfloat red[4] = {0.94f, 0.18f, 0.12f, 1.0f};
  const GLfloat yellow[4] = {1.0f, 0.72f, 0.12f, 1.0f};
  const GLfloat green[4] = {0.18f, 0.78f, 0.46f, 1.0f};
  const float x = 0.69f * std::sin(time * 1.10f);
  const float y = -0.53f + 0.12f * std::sin(time * 1.70f);
  DrawCircle(gl, x, y, 0.14f, red, time * 0.7f);
  DrawCircle(gl, -0.76f * std::sin(time * 0.73f + 1.2f), 0.66f,
             0.09f, yellow, 0.0f);
  DrawCircle(gl, 0.73f * std::cos(time * 0.81f), -0.09f,
             0.07f, green, time);
  (void)aspect;
  fbo.bind(GL_FRAMEBUFFER_OES, 0);
}

void RenderTelevision(GlesApi& gl, const WindowSurface& window,
                      GLuint screen_texture, float time) {
  const auto size = window.size();
  const int width = std::max(1, size.width);
  const int height = std::max(1, size.height);
  const float aspect = static_cast<float>(width) / static_cast<float>(height);
  gl.gl_viewport(0, 0, width, height);
  Set2D(gl, width, height);
  gl.gl_disable(GL_DEPTH_TEST);
  gl.gl_disable(GL_CULL_FACE);
  gl.gl_clear_color(0.025f, 0.045f, 0.075f, 1.0f);
  gl.gl_clear(GL_COLOR_BUFFER_BIT);
  gl.gl_enable_client_state(GL_VERTEX_ARRAY);

  const GLfloat floor[4] = {0.09f, 0.12f, 0.16f, 1.0f};
  const GLfloat shadow[4] = {0.018f, 0.025f, 0.036f, 1.0f};
  const GLfloat case_color[4] = {0.17f, 0.20f, 0.24f, 1.0f};
  const GLfloat edge[4] = {0.33f, 0.38f, 0.42f, 1.0f};
  const GLfloat black[4] = {0.018f, 0.025f, 0.031f, 1.0f};
  const GLfloat metal[4] = {0.57f, 0.62f, 0.64f, 1.0f};
  const GLfloat amber[4] = {0.95f, 0.53f, 0.12f, 1.0f};
  const float half_width = std::min(0.82f, aspect * 0.83f);
  const float body_left = -half_width;
  const float body_right = half_width;
  DrawQuad(gl, -aspect, -1.0f, aspect, -0.78f, floor);
  DrawQuad(gl, body_left - 0.06f, -0.73f, body_right + 0.06f, -0.65f, shadow);
  DrawQuad(gl, body_left, -0.60f, body_right, 0.61f, case_color);
  DrawQuad(gl, body_left, 0.55f, body_right, 0.61f, edge);

  const float screen_left = body_left + half_width * 0.08f;
  const float screen_right = body_left + half_width * 1.34f;
  const float screen_bottom = -0.48f;
  const float screen_top = 0.46f;
  DrawQuad(gl, screen_left - 0.045f, screen_bottom - 0.045f,
           screen_right + 0.045f, screen_top + 0.045f, black);
  DrawQuad(gl, screen_left, screen_bottom, screen_right, screen_top,
           metal, screen_texture);

  // A simple speaker grille and tuning dials evoke the original TV model.
  const float panel_left = screen_right + 0.09f;
  const float panel_right = body_right - half_width * 0.08f;
  DrawQuad(gl, panel_left, -0.48f, panel_right, 0.46f, black);
  for (int i = 0; i < 8; ++i) {
    const float y = -0.34f + static_cast<float>(i) * 0.075f;
    DrawQuad(gl, panel_left + 0.025f, y, panel_right - 0.025f, y + 0.018f, metal);
  }
  DrawCircle(gl, (panel_left + panel_right) * 0.5f, 0.31f,
             0.055f, amber, time);
  DrawCircle(gl, (panel_left + panel_right) * 0.5f, 0.15f,
             0.043f, metal, -time * 0.5f);
  DrawQuad(gl, body_left + half_width * 0.25f, -0.79f,
           body_left + half_width * 0.39f, -0.60f, case_color);
  DrawQuad(gl, body_right - half_width * 0.39f, -0.79f,
           body_right - half_width * 0.25f, -0.60f, case_color);
  gl.gl_disable_client_state(GL_VERTEX_ARRAY);
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
  const GLubyte* version = gl.gl_get_string(GL_VERSION);
  if (version == nullptr ||
      std::strstr(reinterpret_cast<const char*>(version), "1.1") == nullptr) {
    ShowFailure("FilmTV needs an OpenGL ES 1.1 context");
    return KErrNotSupported;
  }
  const char* extensions = reinterpret_cast<const char*>(gl.gl_get_string(GL_EXTENSIONS));
  if (!HasExtension(extensions, "GL_OES_framebuffer_object")) {
    ShowFailure("FilmTV needs GL_OES_framebuffer_object");
    return KErrNotSupported;
  }
  FboApi fbo;
  if (!fbo.Load(gl)) {
    ShowFailure("FBO entry points unavailable via EGL");
    return KErrNotSupported;
  }

  GLuint textures[2] = {0, 0};
  GLuint framebuffers[2] = {0, 0};
  std::string error;
  if (!CreateTargets(gl, fbo, textures, framebuffers, &error)) {
    ShowFailure(error.c_str());
    return KErrNotSupported;
  }
  gl.gl_tex_envf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  gl.gl_enable_client_state(GL_VERTEX_ARRAY);
  bool running = true;
  unsigned int frame = 0;
  float time = 0.0f;
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
    const int current = static_cast<int>(frame & 1u);
    const int previous = 1 - current;
    RenderFilmTexture(gl, fbo, framebuffers, textures, current, previous, frame, time);
    RenderTelevision(gl, *window, textures[current], time);
    if (!context.Swap()) {
      ShowFailure(context.error().c_str());
      break;
    }
    ++frame;
    time += 0.035f;
    if (time > 1000.0f) time -= 1000.0f;
    User::After(18000);
  }
  fbo.bind(GL_FRAMEBUFFER_OES, 0);
  fbo.remove(2, framebuffers);
  gl.gl_delete_textures(2, textures);
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
