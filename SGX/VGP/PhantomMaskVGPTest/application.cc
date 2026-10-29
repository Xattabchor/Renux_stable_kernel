#include "application.h"

#include <e32std.h>
#include <EGL/egl.h>
#include <GLES/gl.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <string>
#include <string_view>

#include "symbian/api/display/window_surface.h"
#include "symbian/api/time/monotonic_clock.h"
#include "phantom_mask_scene_data.h"

extern "C" {
extern const std::uint8_t phantom_vgp_program_begin[];
extern const std::uint8_t phantom_vgp_program_end[];
extern const std::uint8_t phantom_mask_main_pvr_begin[];
extern const std::uint8_t phantom_mask_main_pvr_end[];
extern const std::uint8_t phantom_room_still_pvr_begin[];
extern const std::uint8_t phantom_room_still_pvr_end[];
}

namespace {
using symbian::api::display::WindowInputKind;
using symbian::api::display::WindowKey;
using symbian::api::display::WindowSurface;

constexpr GLenum kVertexProgram = 0x8620;
constexpr GLenum kProgramFormatBinaryImg = 0x8C0B;
constexpr GLenum kCompressedRgbPvrtc4BppV1Img = 0x8C00;
constexpr std::uint32_t kPvrV2Tag = 0x21525650;
constexpr std::uint32_t kPvrOglPvrtc4 = 0x19;
constexpr std::int64_t kNanosecondsPerSecond = 1000000000LL;
constexpr float kNearPlane = 50.0f;
constexpr float kFarPlane = 5000.0f;
constexpr float kDemoFrameRate = 30.0f;
constexpr float kShModelRotationY = -1.047197f;

using GenPrograms = void (*)(GLsizei, GLuint*);
using BindProgram = void (*)(GLenum, GLuint);
using ProgramString = void (*)(GLenum, GLenum, GLsizei, const void*);
using ProgramEnvParameter4fv = void (*)(GLenum, GLuint, const GLfloat*);
using DeletePrograms = void (*)(GLsizei, const GLuint*);

struct VgpApi {
  GenPrograms gen = nullptr;
  BindProgram bind = nullptr;
  ProgramString load = nullptr;
  ProgramEnvParameter4fv set_env = nullptr;
  DeletePrograms remove = nullptr;
};

struct PvrV2View {
  const std::uint8_t* pixels = nullptr;
  std::size_t total_bytes = 0;
  unsigned width = 0;
  unsigned height = 0;
  unsigned mip_count = 0;
  unsigned data_bytes = 0;
};

struct ShCoefficients {
  GLfloat c_ar[4];
  GLfloat c_ag[4];
  GLfloat c_ab[4];
  GLfloat c_br[4];
  GLfloat c_bg[4];
  GLfloat c_bb[4];
  GLfloat c_c[4];
};

// Nokia Symbian 9.x firmware exposes its EGL 1.1 and GLES 1.1 entry points
// through libGLES_CM.dll. Load that combined driver dynamically instead of
// importing the newer SDK's separate libegl.dll/libglesv1_cm.dll proxies.
class SymbianGlesApi final {
 public:
#define GLES_FN(name) using name##Fn = decltype(&name); name##Fn name##_fn = nullptr
  GLES_FN(eglGetDisplay);
  GLES_FN(eglGetError);
  GLES_FN(eglGetProcAddress);
  GLES_FN(eglInitialize);
  GLES_FN(eglChooseConfig);
  GLES_FN(eglCreateWindowSurface);
  GLES_FN(eglCreateContext);
  GLES_FN(eglMakeCurrent);
  GLES_FN(eglSwapBuffers);
  GLES_FN(eglDestroyContext);
  GLES_FN(eglDestroySurface);
  GLES_FN(eglTerminate);
  GLES_FN(glGenTextures);
  GLES_FN(glBindTexture);
  GLES_FN(glTexParameterf);
  GLES_FN(glCompressedTexImage2D);
  GLES_FN(glGetError);
  GLES_FN(glDeleteTextures);
  GLES_FN(glGetString);
  GLES_FN(glClearColor);
  GLES_FN(glClear);
  GLES_FN(glDisable);
  GLES_FN(glEnable);
  GLES_FN(glCullFace);
  GLES_FN(glDepthFunc);
  GLES_FN(glMatrixMode);
  GLES_FN(glLoadIdentity);
  GLES_FN(glLoadMatrixf);
  GLES_FN(glColor4f);
  GLES_FN(glEnableClientState);
  GLES_FN(glDisableClientState);
  GLES_FN(glVertexPointer);
  GLES_FN(glNormalPointer);
  GLES_FN(glTexCoordPointer);
  GLES_FN(glDrawArrays);
  GLES_FN(glDrawElements);
  GLES_FN(glTexEnvf);
  GLES_FN(glLightfv);
  GLES_FN(glMaterialfv);
  GLES_FN(glViewport);
#undef GLES_FN

  bool Load() {
    const TInt result = library_.Load(_L("libGLES_CM.dll"));
    if (result != KErrNone) {
      error_ = "RLibrary.Load(libGLES_CM.dll) failed (" +
               std::to_string(result) + ")";
      return false;
    }
    loaded_ = true;
    // Symbian GLES 1.1 Common ordinal table; EGL exports precede GLES exports.
    return LoadFunction(&eglChooseConfig_fn, 1, "eglChooseConfig") &&
           LoadFunction(&eglCreateContext_fn, 3, "eglCreateContext") &&
           LoadFunction(&eglCreateWindowSurface_fn, 6, "eglCreateWindowSurface") &&
           LoadFunction(&eglDestroyContext_fn, 7, "eglDestroyContext") &&
           LoadFunction(&eglDestroySurface_fn, 8, "eglDestroySurface") &&
           LoadFunction(&eglGetDisplay_fn, 15, "eglGetDisplay") &&
           LoadFunction(&eglGetError_fn, 16, "eglGetError") &&
           LoadFunction(&eglGetProcAddress_fn, 17, "eglGetProcAddress") &&
           LoadFunction(&eglInitialize_fn, 18, "eglInitialize") &&
           LoadFunction(&eglMakeCurrent_fn, 19, "eglMakeCurrent") &&
           LoadFunction(&eglSwapBuffers_fn, 24, "eglSwapBuffers") &&
           LoadFunction(&eglTerminate_fn, 25, "eglTerminate") &&
           LoadFunction(&glBindTexture_fn, 60, "glBindTexture") &&
           LoadFunction(&glClear_fn, 62, "glClear") &&
           LoadFunction(&glClearColor_fn, 63, "glClearColor") &&
           LoadFunction(&glColor4f_fn, 69, "glColor4f") &&
           LoadFunction(&glCompressedTexImage2D_fn, 73, "glCompressedTexImage2D") &&
           LoadFunction(&glCullFace_fn, 77, "glCullFace") &&
           LoadFunction(&glDeleteTextures_fn, 78, "glDeleteTextures") &&
           LoadFunction(&glDepthFunc_fn, 79, "glDepthFunc") &&
           LoadFunction(&glDisable_fn, 83, "glDisable") &&
           LoadFunction(&glDisableClientState_fn, 84, "glDisableClientState") &&
           LoadFunction(&glDrawArrays_fn, 85, "glDrawArrays") &&
           LoadFunction(&glDrawElements_fn, 86, "glDrawElements") &&
           LoadFunction(&glEnable_fn, 87, "glEnable") &&
           LoadFunction(&glEnableClientState_fn, 88, "glEnableClientState") &&
           LoadFunction(&glGenTextures_fn, 98, "glGenTextures") &&
           LoadFunction(&glGetError_fn, 99, "glGetError") &&
           LoadFunction(&glGetString_fn, 101, "glGetString") &&
           LoadFunction(&glLightfv_fn, 108, "glLightfv") &&
           LoadFunction(&glLoadIdentity_fn, 113, "glLoadIdentity") &&
           LoadFunction(&glLoadMatrixf_fn, 114, "glLoadMatrixf") &&
           LoadFunction(&glMaterialfv_fn, 118, "glMaterialfv") &&
           LoadFunction(&glNormalPointer_fn, 128, "glNormalPointer") &&
           LoadFunction(&glTexCoordPointer_fn, 151, "glTexCoordPointer") &&
           LoadFunction(&glTexEnvf_fn, 152, "glTexEnvf") &&
           LoadFunction(&glTexParameterf_fn, 157, "glTexParameterf") &&
           LoadFunction(&glVertexPointer_fn, 162, "glVertexPointer") &&
           LoadFunction(&glViewport_fn, 163, "glViewport");
  }

  void Close() {
    if (loaded_) library_.Close();
    loaded_ = false;
  }

  template <typename Function>
  Function LoadProcedure(const char* name) const {
    return eglGetProcAddress_fn == nullptr
               ? nullptr
               : reinterpret_cast<Function>(eglGetProcAddress_fn(name));
  }

  const std::string& error_message() const { return error_; }

 private:
  template <typename Function>
  bool LoadFunction(Function* output, TInt ordinal, const char* name) {
    const TLibraryFunction address = library_.Lookup(ordinal);
    if (address == nullptr) {
      error_ = std::string("libGLES_CM.dll missing ") + name + " at ordinal " +
               std::to_string(ordinal);
      Close();
      return false;
    }
    *output = reinterpret_cast<Function>(address);
    return true;
  }

  RLibrary library_;
  bool loaded_ = false;
  std::string error_;
};

VgpApi LoadVgpApi(const SymbianGlesApi& gl) {
  return {
      gl.LoadProcedure<GenPrograms>("glGenProgramsARB"),
      gl.LoadProcedure<BindProgram>("glBindProgramARB"),
      gl.LoadProcedure<ProgramString>("glProgramStringARB"),
      gl.LoadProcedure<ProgramEnvParameter4fv>("glProgramEnvParameter4fvARB"),
      gl.LoadProcedure<DeletePrograms>("glDeleteProgramsARB"),
  };
}

class LegacyEs1Context final {
 public:
  LegacyEs1Context() = default;
  LegacyEs1Context(const LegacyEs1Context&) = delete;
  LegacyEs1Context& operator=(const LegacyEs1Context&) = delete;
  ~LegacyEs1Context() { Close(); }

  bool Open(WindowSurface* window) {
    if (!api_.Load()) {
      error_ = api_.error_message();
      return false;
    }
    if (window == nullptr || window->NativeWindowHandle() == nullptr) {
      return Fail("no native window", EGL_BAD_NATIVE_WINDOW);
    }
    display_ = api_.eglGetDisplay_fn(EGL_DEFAULT_DISPLAY);
    if (display_ == EGL_NO_DISPLAY) return Fail("eglGetDisplay", api_.eglGetError_fn());
    EGLint major = 0;
    EGLint minor = 0;
    if (!api_.eglInitialize_fn(display_, &major, &minor)) {
      return Fail("eglInitialize", api_.eglGetError_fn());
    }
    initialized_ = true;
    // Match the working Quake III Symbian EGL 1.1 setup for N95: 16-bit color,
    // a 16-bit depth buffer, and no EGL 1.2 renderable-type/client-version tags.
    const EGLint attributes[] = {EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                                 EGL_BUFFER_SIZE, 16, EGL_DEPTH_SIZE, 16,
                                 EGL_NONE};
    EGLConfig config = 0;
    EGLint count = 0;
    if (!api_.eglChooseConfig_fn(display_, attributes, &config, 1, &count)) {
      return Fail("eglChooseConfig", api_.eglGetError_fn());
    }
    if (count == 0) return Fail("no 16-bit EGL config", EGL_SUCCESS);
    surface_ = api_.eglCreateWindowSurface_fn(
        display_, config,
        static_cast<EGLNativeWindowType>(window->NativeWindowHandle()), nullptr);
    if (surface_ == EGL_NO_SURFACE) {
      return Fail("eglCreateWindowSurface", api_.eglGetError_fn());
    }
    context_ = api_.eglCreateContext_fn(display_, config, EGL_NO_CONTEXT, nullptr);
    if (context_ == EGL_NO_CONTEXT) {
      return Fail("eglCreateContext", api_.eglGetError_fn());
    }
    if (!api_.eglMakeCurrent_fn(display_, surface_, surface_, context_)) {
      return Fail("eglMakeCurrent", api_.eglGetError_fn());
    }
    return true;
  }

  bool Swap() {
    if (display_ != EGL_NO_DISPLAY && surface_ != EGL_NO_SURFACE &&
        api_.eglSwapBuffers_fn(display_, surface_)) return true;
    return Fail("eglSwapBuffers", api_.eglGetError_fn());
  }
  const std::string& error_message() const { return error_; }
  const SymbianGlesApi& api() const { return api_; }
  void Close() {
    if (display_ != EGL_NO_DISPLAY) {
      if (context_ != EGL_NO_CONTEXT) {
        api_.eglMakeCurrent_fn(display_, EGL_NO_SURFACE, EGL_NO_SURFACE,
                               EGL_NO_CONTEXT);
        api_.eglDestroyContext_fn(display_, context_);
      }
      if (surface_ != EGL_NO_SURFACE) api_.eglDestroySurface_fn(display_, surface_);
      if (initialized_) api_.eglTerminate_fn(display_);
    }
    display_ = EGL_NO_DISPLAY;
    surface_ = EGL_NO_SURFACE;
    context_ = EGL_NO_CONTEXT;
    initialized_ = false;
    api_.Close();
  }

 private:
  bool Fail(const char* stage, EGLint error) {
    char code[16];
    std::snprintf(code, sizeof(code), "0x%04x", static_cast<unsigned>(error));
    error_ = std::string(stage) + " failed (EGL " + code + ")";
    Close();
    return false;
  }
  SymbianGlesApi api_;
  EGLDisplay display_ = EGL_NO_DISPLAY;
  EGLSurface surface_ = EGL_NO_SURFACE;
  EGLContext context_ = EGL_NO_CONTEXT;
  bool initialized_ = false;
  std::string error_;
};

bool HasExtension(const char* extension_list, const char* wanted) {
  if (!extension_list || !wanted || !*wanted || std::strchr(wanted, ' ')) {
    return false;
  }
  const std::size_t wanted_length = std::strlen(wanted);
  const char* cursor = extension_list;
  while ((cursor = std::strstr(cursor, wanted)) != nullptr) {
    const bool begins_token = cursor == extension_list || cursor[-1] == ' ';
    const char following = cursor[wanted_length];
    if (begins_token && (following == '\0' || following == ' ')) {
      return true;
    }
    cursor += wanted_length;
  }
  return false;
}

struct StatusGlyph {
  char character;
  std::uint8_t rows[5];
};

constexpr StatusGlyph kStatusGlyphs[] = {
    {' ', {0, 0, 0, 0, 0}}, {'A', {2, 5, 7, 5, 5}}, {'B', {6, 5, 6, 5, 6}},
    {'C', {3, 4, 4, 4, 3}}, {'D', {6, 5, 5, 5, 6}}, {'E', {7, 4, 6, 4, 7}},
    {'F', {7, 4, 6, 4, 4}}, {'G', {3, 4, 5, 5, 3}}, {'H', {5, 5, 7, 5, 5}},
    {'I', {7, 2, 2, 2, 7}}, {'J', {1, 1, 1, 5, 2}}, {'K', {5, 5, 6, 5, 5}},
    {'L', {4, 4, 4, 4, 7}}, {'M', {5, 7, 7, 5, 5}}, {'N', {5, 7, 7, 7, 5}},
    {'O', {2, 5, 5, 5, 2}}, {'P', {6, 5, 6, 4, 4}}, {'Q', {2, 5, 5, 7, 3}},
    {'R', {6, 5, 6, 5, 5}}, {'S', {3, 4, 2, 1, 6}}, {'T', {7, 2, 2, 2, 2}},
    {'U', {5, 5, 5, 5, 7}}, {'V', {5, 5, 5, 5, 2}}, {'W', {5, 5, 7, 7, 5}},
    {'X', {5, 5, 2, 5, 5}}, {'Y', {5, 5, 2, 2, 2}}, {'Z', {7, 1, 2, 4, 7}},
};

const StatusGlyph& FindStatusGlyph(char character) {
  for (const StatusGlyph& glyph : kStatusGlyphs) {
    if (glyph.character == character) return glyph;
  }
  return kStatusGlyphs[0];
}

void SetStatusPixel(symbian::api::display::Rgb565Frame* frame, int x, int y,
                    std::uint16_t color) {
  if (frame == nullptr || x < 0 || y < 0 || x >= frame->size.width ||
      y >= frame->size.height) {
    return;
  }
  const std::size_t offset = static_cast<std::size_t>(y) * frame->pitch_bytes +
                             static_cast<std::size_t>(x) * sizeof(color);
  std::memcpy(frame->pixels.data() + offset, &color, sizeof(color));
}

void DrawStatusText(symbian::api::display::Rgb565Frame* frame,
                    std::string_view text, int x, int y, int scale,
                    std::uint16_t color) {
  for (std::size_t index = 0; index < text.size(); ++index) {
    const StatusGlyph& glyph = FindStatusGlyph(text[index]);
    for (int row = 0; row < 5; ++row) {
      for (int column = 0; column < 3; ++column) {
        if ((glyph.rows[row] & (1u << (2 - column))) == 0) continue;
        for (int dy = 0; dy < scale; ++dy) {
          for (int dx = 0; dx < scale; ++dx) {
            SetStatusPixel(frame,
                           x + static_cast<int>(index) * 4 * scale +
                               column * scale + dx,
                           y + row * scale + dy, color);
          }
        }
      }
    }
  }
}

bool DrawVgpStatus(WindowSurface* window, std::string_view status) {
  auto frame_result = window->CreateRgb565Frame();
  if (!frame_result.ok()) return false;
  auto frame = *frame_result;
  for (int y = 0; y < frame.size.height; ++y) {
    for (int x = 0; x < frame.size.width; ++x) {
      SetStatusPixel(&frame, x, y, 0x0841);
    }
  }
  const int scale = frame.size.width >= 220 ? 2 : 1;
  const int lines_height = 4 * 7 * scale;
  const int first_y = std::max(4, (frame.size.height - lines_height) / 2);
  const auto draw_centered = [&](std::string_view text, int y,
                                 std::uint16_t color) {
    const int text_width = static_cast<int>(text.size()) * 4 * scale - scale;
    const int x = std::max(2, (frame.size.width - text_width) / 2);
    DrawStatusText(&frame, text, x, y, scale, color);
  };
  draw_centered("VERTEX PROGRAM", first_y, 0xffff);
  draw_centered(status, first_y + 7 * scale, 0xffe0);
  draw_centered("USING FIXED LIGHTING", first_y + 14 * scale, 0xffff);
  draw_centered("PRESS ANY KEY", first_y + 21 * scale, 0x07ff);
  return window->Present().ok();
}

bool ShowVgpStatus(WindowSurface* window, std::string_view status) {
  if (!DrawVgpStatus(window, status)) return true;
  while (true) {
    for (int count = 0; count < 32; ++count) {
      auto event = window->PollInput();
      if (!event.ok()) return true;
      if (!event->has_value()) break;
      const auto& input = **event;
      if (input.kind == WindowInputKind::kCloseRequested) return false;
      if (input.kind == WindowInputKind::kKeyDown) return true;
      if (input.kind == WindowInputKind::kDisplayChanged &&
          !DrawVgpStatus(window, status)) {
        return true;
      }
    }
    User::After(50000);
  }
}

std::size_t AssetByteCount(const std::uint8_t* begin,
                           const std::uint8_t* end) {
  return static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(end) -
                                  reinterpret_cast<std::uintptr_t>(begin));
}

std::uint32_t ReadU32LE(const std::uint8_t* bytes) {
  return static_cast<std::uint32_t>(bytes[0]) |
         (static_cast<std::uint32_t>(bytes[1]) << 8) |
         (static_cast<std::uint32_t>(bytes[2]) << 16) |
         (static_cast<std::uint32_t>(bytes[3]) << 24);
}

bool ParsePvrV2(const std::uint8_t* data, std::size_t size, PvrV2View* out) {
  if (!data || !out || size < 52) {
    return false;
  }
  const std::uint32_t header_bytes = ReadU32LE(data + 0);
  const std::uint32_t height = ReadU32LE(data + 4);
  const std::uint32_t width = ReadU32LE(data + 8);
  const std::uint32_t mipmaps = ReadU32LE(data + 12);
  const std::uint32_t flags = ReadU32LE(data + 16);
  const std::uint32_t data_bytes = ReadU32LE(data + 20);
  const std::uint32_t bits_per_pixel = ReadU32LE(data + 24);
  const std::uint32_t alpha_mask = ReadU32LE(data + 40);
  const std::uint32_t tag = ReadU32LE(data + 44);
  const std::uint32_t surfaces = ReadU32LE(data + 48);
  if (header_bytes != 52 || width == 0 || height == 0 ||
      (flags & 0xffu) != kPvrOglPvrtc4 || bits_per_pixel != 4 ||
      alpha_mask != 0 || tag != kPvrV2Tag || surfaces != 1 ||
      data_bytes > size - header_bytes) {
    return false;
  }
  *out = {data + header_bytes, size, width, height, mipmaps + 1, data_bytes};
  return true;
}

std::size_t Pvrtc4LevelBytes(unsigned width, unsigned height) {
  const std::size_t pixels =
      static_cast<std::size_t>(std::max(width, 8u)) * std::max(height, 8u);
  return std::max<std::size_t>(32, pixels / 2);
}

bool LoadPvrV2Texture(const SymbianGlesApi& gl, const std::uint8_t* data,
                      std::size_t size, GLuint* texture) {
  PvrV2View pvr;
  if (!texture || !ParsePvrV2(data, size, &pvr)) {
    return false;
  }
  std::size_t expected_bytes = 0;
  unsigned level_width = pvr.width;
  unsigned level_height = pvr.height;
  for (unsigned level = 0; level < pvr.mip_count; ++level) {
    expected_bytes += Pvrtc4LevelBytes(level_width, level_height);
    level_width = std::max(1u, level_width >> 1);
    level_height = std::max(1u, level_height >> 1);
  }
  if (expected_bytes != pvr.data_bytes) {
    return false;
  }

  gl.glGenTextures_fn(1, texture);
  gl.glBindTexture_fn(GL_TEXTURE_2D, *texture);
  gl.glTexParameterf_fn(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLfloat>(GL_LINEAR_MIPMAP_LINEAR));
  gl.glTexParameterf_fn(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLfloat>(GL_LINEAR));
  gl.glTexParameterf_fn(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLfloat>(GL_REPEAT));
  gl.glTexParameterf_fn(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLfloat>(GL_REPEAT));

  std::size_t offset = 0;
  level_width = pvr.width;
  level_height = pvr.height;
  for (unsigned level = 0; level < pvr.mip_count; ++level) {
    const std::size_t level_bytes = Pvrtc4LevelBytes(level_width, level_height);
    gl.glCompressedTexImage2D_fn(
        GL_TEXTURE_2D, static_cast<GLint>(level),
        kCompressedRgbPvrtc4BppV1Img, static_cast<GLsizei>(level_width),
        static_cast<GLsizei>(level_height), 0,
        static_cast<GLsizei>(level_bytes), pvr.pixels + offset);
    if (gl.glGetError_fn() != GL_NO_ERROR) {
      gl.glDeleteTextures_fn(1, texture);
      *texture = 0;
      return false;
    }
    offset += level_bytes;
    level_width = std::max(1u, level_width >> 1);
    level_height = std::max(1u, level_height >> 1);
  }
  return true;
}

ShCoefficients BuildShCoefficients() {
  // The original PhantomMask demo enables only the environment SH lighting.
  const GLfloat red[9] = {
      1.2961891f, -0.42659417f, -0.10065936f, -8.4035477e-005f,
      -0.00021227333f, 0.10019236f, 0.011847760f, 0.00016783635f,
      -0.10584830f};
  const GLfloat green[9] = {
      1.2506844f, -0.12775756f, 0.33325988f, -8.7283181e-005f,
      -0.00015105936f, -0.025249202f, -0.048718069f, 0.00026852929f,
      -0.28519103f};
  const GLfloat blue[9] = {
      1.6430428f, 0.098693930f, 0.071262904f, 0.00044371662f,
      0.00027166531f, 0.056100018f, -0.23762819f, -0.00015725456f,
      -0.49318397f};
  const GLfloat* channels[3] = {red, green, blue};
  ShCoefficients result{};
  const GLfloat sqrt_pi = 1.772453850905516f;
  const GLfloat c0 = 1.0f / (2.0f * sqrt_pi);
  const GLfloat c1 = 1.732050807568877f / (3.0f * sqrt_pi);
  const GLfloat c2 = 3.872983346207417f / (8.0f * sqrt_pi);
  const GLfloat c3 = 2.236067977499790f / (16.0f * sqrt_pi);
  const GLfloat c4 = 0.5f * c2;
  GLfloat* a[3] = {result.c_ar, result.c_ag, result.c_ab};
  GLfloat* b[3] = {result.c_br, result.c_bg, result.c_bb};
  for (int channel = 0; channel < 3; ++channel) {
    a[channel][0] = -c1 * channels[channel][3];
    a[channel][1] = -c1 * channels[channel][1];
    a[channel][2] = c1 * channels[channel][2];
    a[channel][3] = c0 * channels[channel][0] - c3 * channels[channel][6];
    b[channel][0] = c2 * channels[channel][4];
    b[channel][1] = -c2 * channels[channel][5];
    b[channel][2] = 3.0f * c3 * channels[channel][6];
    b[channel][3] = -c2 * channels[channel][7];
    result.c_c[channel] = c4 * channels[channel][8];
  }
  result.c_c[3] = 0.0f;
  return result;
}

void UploadShCoefficients(const VgpApi& api, const ShCoefficients& sh) {
  api.set_env(kVertexProgram, 7, sh.c_ar);
  api.set_env(kVertexProgram, 8, sh.c_ag);
  api.set_env(kVertexProgram, 9, sh.c_ab);
  api.set_env(kVertexProgram, 10, sh.c_br);
  api.set_env(kVertexProgram, 11, sh.c_bg);
  api.set_env(kVertexProgram, 12, sh.c_bb);
  api.set_env(kVertexProgram, 13, sh.c_c);
}

void UploadModelRows(const VgpApi& api, const float* world) {
  const float cosine = std::cos(kShModelRotationY);
  const float sine = std::sin(kShModelRotationY);
  // PVRTMat3(world) * RotationY(angle), represented column-major.
  const float rotation[9] = {
      cosine, 0.0f, -sine,
      0.0f, 1.0f, 0.0f,
      sine, 0.0f, cosine,
  };
  float world3[9] = {
      world[0], world[1], world[2],
      world[4], world[5], world[6],
      world[8], world[9], world[10],
  };
  float normal_frame[9] = {};
  for (int column = 0; column < 3; ++column) {
    for (int row = 0; row < 3; ++row) {
      for (int k = 0; k < 3; ++k) {
        normal_frame[column * 3 + row] +=
            world3[k * 3 + row] * rotation[column * 3 + k];
      }
    }
  }
  const GLfloat rows[3][4] = {
      {normal_frame[0], normal_frame[3], normal_frame[6], 0.0f},
      {normal_frame[1], normal_frame[4], normal_frame[7], 0.0f},
      {normal_frame[2], normal_frame[5], normal_frame[8], 0.0f},
  };
  for (GLuint row = 0; row < 3; ++row) {
    api.set_env(kVertexProgram, 4 + row, rows[row]);
  }
}

void MultiplyMat4(const float* left, const float* right, float* output) {
  for (int column = 0; column < 4; ++column) {
    for (int row = 0; row < 4; ++row) {
      float value = 0.0f;
      for (int k = 0; k < 4; ++k) {
        value += left[k * 4 + row] * right[column * 4 + k];
      }
      output[column * 4 + row] = value;
    }
  }
}

void MakeViewMatrix(const float* camera, float* view) {
  float forward[3] = {camera[3] - camera[0], camera[4] - camera[1],
                      camera[5] - camera[2]};
  const float forward_length =
      std::sqrt(forward[0] * forward[0] + forward[1] * forward[1] +
                forward[2] * forward[2]);
  if (forward_length > 0.0f) {
    for (float& value : forward) value /= forward_length;
  }
  const float up[3] = {0.0f, 1.0f, 0.0f};
  float side[3] = {forward[1] * up[2] - forward[2] * up[1],
                   forward[2] * up[0] - forward[0] * up[2],
                   forward[0] * up[1] - forward[1] * up[0]};
  const float side_length =
      std::sqrt(side[0] * side[0] + side[1] * side[1] + side[2] * side[2]);
  if (side_length > 0.0f) {
    for (float& value : side) value /= side_length;
  }
  const float corrected_up[3] = {
      side[1] * forward[2] - side[2] * forward[1],
      side[2] * forward[0] - side[0] * forward[2],
      side[0] * forward[1] - side[1] * forward[0],
  };
  view[0] = side[0]; view[1] = corrected_up[0]; view[2] = -forward[0]; view[3] = 0.0f;
  view[4] = side[1]; view[5] = corrected_up[1]; view[6] = -forward[1]; view[7] = 0.0f;
  view[8] = side[2]; view[9] = corrected_up[2]; view[10] = -forward[2]; view[11] = 0.0f;
  view[12] = -(side[0] * camera[0] + side[1] * camera[1] + side[2] * camera[2]);
  view[13] = -(corrected_up[0] * camera[0] + corrected_up[1] * camera[1] + corrected_up[2] * camera[2]);
  view[14] = forward[0] * camera[0] + forward[1] * camera[1] + forward[2] * camera[2];
  view[15] = 1.0f;
}

void MakeProjectionMatrix(float fov, float aspect, float* projection) {
  for (int i = 0; i < 16; ++i) projection[i] = 0.0f;
  const float tangent = std::tan(fov * 0.5f);
  if (tangent <= 0.0f || aspect <= 0.0f) return;
  projection[0] = 1.0f / (aspect * tangent);
  projection[5] = 1.0f / tangent;
  projection[10] = -(kFarPlane + kNearPlane) / (kFarPlane - kNearPlane);
  projection[11] = -1.0f;
  projection[14] = -(2.0f * kFarPlane * kNearPlane) /
                   (kFarPlane - kNearPlane);
}

void DrawBackground(const SymbianGlesApi& gl, GLuint texture,
                    bool vgp_active) {
  static const GLfloat positions[] = {
      -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
  };
  static const GLfloat texcoords[] = {
      0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
  };
  if (vgp_active) gl.glDisable_fn(kVertexProgram);
  gl.glDisable_fn(GL_DEPTH_TEST);
  gl.glDisable_fn(GL_CULL_FACE);
  gl.glDisable_fn(GL_LIGHTING);
  gl.glMatrixMode_fn(GL_PROJECTION);
  gl.glLoadIdentity_fn();
  gl.glMatrixMode_fn(GL_MODELVIEW);
  gl.glLoadIdentity_fn();
  gl.glColor4f_fn(1.0f, 1.0f, 1.0f, 1.0f);
  gl.glBindTexture_fn(GL_TEXTURE_2D, texture);
  gl.glTexEnvf_fn(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, static_cast<GLfloat>(GL_REPLACE));
  gl.glEnable_fn(GL_TEXTURE_2D);
  gl.glEnableClientState_fn(GL_VERTEX_ARRAY);
  gl.glEnableClientState_fn(GL_TEXTURE_COORD_ARRAY);
  gl.glVertexPointer_fn(2, GL_FLOAT, 0, positions);
  gl.glTexCoordPointer_fn(2, GL_FLOAT, 0, texcoords);
  gl.glDrawArrays_fn(GL_TRIANGLE_STRIP, 0, 4);
  gl.glDisableClientState_fn(GL_TEXTURE_COORD_ARRAY);
  gl.glDisableClientState_fn(GL_VERTEX_ARRAY);
  gl.glTexEnvf_fn(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, static_cast<GLfloat>(GL_MODULATE));
}

void DrawPhantomMask(const SymbianGlesApi& gl, const float* camera,
                     const float* world, int width, int height, GLuint texture,
                     bool vgp_active, const VgpApi& api, GLuint program) {
  float view[16];
  float projection[16];
  float model_view[16];
  const float fov = camera[6] * static_cast<float>(height) /
                    static_cast<float>(width);
  MakeViewMatrix(camera, view);
  MakeProjectionMatrix(fov, static_cast<float>(width) / height, projection);
  MultiplyMat4(view, world, model_view);

  gl.glMatrixMode_fn(GL_PROJECTION);
  gl.glLoadMatrixf_fn(projection);
  gl.glMatrixMode_fn(GL_MODELVIEW);
  gl.glLoadMatrixf_fn(model_view);
  gl.glEnable_fn(GL_DEPTH_TEST);
  gl.glDepthFunc_fn(GL_LEQUAL);
  gl.glEnable_fn(GL_CULL_FACE);
  gl.glCullFace_fn(GL_BACK);
  gl.glEnable_fn(GL_TEXTURE_2D);
  gl.glBindTexture_fn(GL_TEXTURE_2D, texture);
  gl.glTexEnvf_fn(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, static_cast<GLfloat>(GL_MODULATE));

  gl.glEnableClientState_fn(GL_VERTEX_ARRAY);
  gl.glEnableClientState_fn(GL_NORMAL_ARRAY);
  gl.glEnableClientState_fn(GL_TEXTURE_COORD_ARRAY);
  const GLsizei stride = static_cast<GLsizei>(8 * sizeof(GLfloat));
  gl.glVertexPointer_fn(3, GL_FLOAT, stride, phantom_mask_scene::kVertices);
  gl.glNormalPointer_fn(GL_FLOAT, stride, phantom_mask_scene::kVertices + 3);
  gl.glTexCoordPointer_fn(2, GL_FLOAT, stride, phantom_mask_scene::kVertices + 6);

  if (vgp_active) {
    api.bind(kVertexProgram, program);
    UploadModelRows(api, world);
    gl.glEnable_fn(kVertexProgram);
    gl.glDisable_fn(GL_LIGHTING);
  } else {
    gl.glEnable_fn(GL_LIGHTING);
    gl.glEnable_fn(GL_LIGHT0);
    const GLfloat ambient[4] = {0.32f, 0.32f, 0.32f, 1.0f};
    const GLfloat diffuse[4] = {0.78f, 0.78f, 0.78f, 1.0f};
    const GLfloat light_position[4] = {0.2f, 0.8f, 1.0f, 0.0f};
    gl.glLightfv_fn(GL_LIGHT0, GL_AMBIENT, ambient);
    gl.glLightfv_fn(GL_LIGHT0, GL_DIFFUSE, diffuse);
    gl.glLightfv_fn(GL_LIGHT0, GL_POSITION, light_position);
    const GLfloat material[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    gl.glMaterialfv_fn(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, material);
  }

  gl.glDrawElements_fn(GL_TRIANGLES, phantom_mask_scene::kIndexCount,
                 GL_UNSIGNED_SHORT, phantom_mask_scene::kIndices);

  gl.glDisableClientState_fn(GL_TEXTURE_COORD_ARRAY);
  gl.glDisableClientState_fn(GL_NORMAL_ARRAY);
  gl.glDisableClientState_fn(GL_VERTEX_ARRAY);
  if (!vgp_active) {
    gl.glDisable_fn(GL_LIGHT0);
    gl.glDisable_fn(GL_LIGHTING);
  }
}

bool InitializeVgp(const SymbianGlesApi& gl, const VgpApi& api,
                   GLuint* program, std::string* status) {
  const auto* extensions =
      reinterpret_cast<const char*>(gl.glGetString_fn(GL_EXTENSIONS));
  if (!HasExtension(extensions, "GL_IMG_vertex_program") || !api.gen ||
      !api.bind || !api.load || !api.set_env || !api.remove) {
    if (status) *status = "NOT SUPPORTED";
    return false;
  }
  api.gen(1, program);
  api.bind(kVertexProgram, *program);
  api.load(kVertexProgram, kProgramFormatBinaryImg,
           static_cast<GLsizei>(AssetByteCount(phantom_vgp_program_begin,
                                               phantom_vgp_program_end)),
           phantom_vgp_program_begin);
  if (gl.glGetError_fn() != GL_NO_ERROR) {
    api.remove(1, program);
    *program = 0;
    if (status) *status = "LOAD FAILED";
    return false;
  }
  const ShCoefficients sh = BuildShCoefficients();
  UploadShCoefficients(api, sh);
  if (gl.glGetError_fn() != GL_NO_ERROR) {
    api.remove(1, program);
    *program = 0;
    if (status) *status = "PARAMETER ERROR";
    return false;
  }
  if (status) status->clear();
  return true;
}

int RunWindow(WindowSurface* window) {
  LegacyEs1Context context;
  if (!context.Open(window)) return KErrNotSupported;
  const SymbianGlesApi& gl = context.api();
  const char* extensions =
      reinterpret_cast<const char*>(gl.glGetString_fn(GL_EXTENSIONS));
  if (!HasExtension(extensions, "GL_IMG_texture_compression_pvrtc")) {
    return KErrNotSupported;
  }

  GLuint room_texture = 0;
  GLuint mask_texture = 0;
  if (!LoadPvrV2Texture(gl, phantom_room_still_pvr_begin,
                        static_cast<std::size_t>(phantom_room_still_pvr_end -
                                                 phantom_room_still_pvr_begin),
                        &room_texture) ||
      !LoadPvrV2Texture(gl, phantom_mask_main_pvr_begin,
                        static_cast<std::size_t>(phantom_mask_main_pvr_end -
                                                 phantom_mask_main_pvr_begin),
                        &mask_texture)) {
    if (room_texture) gl.glDeleteTextures_fn(1, &room_texture);
    if (mask_texture) gl.glDeleteTextures_fn(1, &mask_texture);
    return KErrCorrupt;
  }

  const VgpApi api = LoadVgpApi(gl);
  GLuint program = 0;
  std::string vgp_status;
  const bool vgp_active = InitializeVgp(gl, api, &program, &vgp_status);
  if (!vgp_active && !ShowVgpStatus(window, vgp_status)) {
    gl.glDeleteTextures_fn(1, &mask_texture);
    gl.glDeleteTextures_fn(1, &room_texture);
    return KErrNone;
  }
  gl.glClearColor_fn(0.0f, 0.0f, 0.0f, 1.0f);
  const std::int64_t start_ns =
      symbian::api::time::MonotonicClock::NowNanoseconds();
  bool running = true;
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
    }
    if (!running) break;

    const auto size = window->size();
    const int width = std::max(1, size.width);
    const int height = std::max(1, size.height);
    gl.glViewport_fn(0, 0, width, height);
    gl.glClear_fn(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    DrawBackground(gl, room_texture, vgp_active);

    const std::int64_t now_ns =
        symbian::api::time::MonotonicClock::NowNanoseconds();
    const std::uint64_t elapsed_frames = static_cast<std::uint64_t>(
        std::max<std::int64_t>(0, now_ns - start_ns) * kDemoFrameRate /
        kNanosecondsPerSecond);
    const unsigned frame =
        static_cast<unsigned>(elapsed_frames % phantom_mask_scene::kFrameCount);
    const float* camera = phantom_mask_scene::kCameraFrames + frame * 7;
    const float* world = phantom_mask_scene::kModelMatrices + frame * 16;
    DrawPhantomMask(gl, camera, world, width, height, mask_texture, vgp_active, api,
                    program);

    if (!context.Swap()) break;
    User::After(33333);
  }

  if (vgp_active && program) api.remove(1, &program);
  gl.glDeleteTextures_fn(1, &mask_texture);
  gl.glDeleteTextures_fn(1, &room_texture);
  context.Close();
  return KErrNone;
}
}  // namespace

int RunApplication() {
  WindowSurface window;
  if (!window.Open("PhantomMask").ok()) {
    return KErrNotSupported;
  }
  return RunWindow(&window);
}
