#include "application.h"

#include <e32std.h>
#include <GLES/gl.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "symbian/api/display/gles_window_context.h"
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
using symbian::api::display::GlesWindowContext;
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

template <typename Function>
Function LoadProcedure(const char* name) {
  return reinterpret_cast<Function>(GlesWindowContext::GetProcAddress(name));
}

VgpApi LoadVgpApi() {
  return {
      LoadProcedure<GenPrograms>("glGenProgramsARB"),
      LoadProcedure<BindProgram>("glBindProgramARB"),
      LoadProcedure<ProgramString>("glProgramStringARB"),
      LoadProcedure<ProgramEnvParameter4fv>("glProgramEnvParameter4fvARB"),
      LoadProcedure<DeletePrograms>("glDeleteProgramsARB"),
  };
}

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

bool LoadPvrV2Texture(const std::uint8_t* data, std::size_t size,
                      GLuint* texture) {
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

  glGenTextures(1, texture);
  glBindTexture(GL_TEXTURE_2D, *texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  std::size_t offset = 0;
  level_width = pvr.width;
  level_height = pvr.height;
  for (unsigned level = 0; level < pvr.mip_count; ++level) {
    const std::size_t level_bytes = Pvrtc4LevelBytes(level_width, level_height);
    glCompressedTexImage2D(
        GL_TEXTURE_2D, static_cast<GLint>(level),
        kCompressedRgbPvrtc4BppV1Img, static_cast<GLsizei>(level_width),
        static_cast<GLsizei>(level_height), 0,
        static_cast<GLsizei>(level_bytes), pvr.pixels + offset);
    if (glGetError() != GL_NO_ERROR) {
      glDeleteTextures(1, texture);
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

void DrawBackground(GLuint texture, bool vgp_active) {
  static const GLfloat positions[] = {
      -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
  };
  static const GLfloat texcoords[] = {
      0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
  };
  if (vgp_active) glDisable(kVertexProgram);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glDisable(GL_LIGHTING);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glEnable(GL_TEXTURE_2D);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glVertexPointer(2, GL_FLOAT, 0, positions);
  glTexCoordPointer(2, GL_FLOAT, 0, texcoords);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void DrawPhantomMask(const float* camera, const float* world, int width,
                     int height, GLuint texture, bool vgp_active,
                     const VgpApi& api, GLuint program) {
  float view[16];
  float projection[16];
  float model_view[16];
  const float fov = camera[6] * static_cast<float>(height) /
                    static_cast<float>(width);
  MakeViewMatrix(camera, view);
  MakeProjectionMatrix(fov, static_cast<float>(width) / height, projection);
  MultiplyMat4(view, world, model_view);

  glMatrixMode(GL_PROJECTION);
  glLoadMatrixf(projection);
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixf(model_view);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  const GLsizei stride = static_cast<GLsizei>(8 * sizeof(GLfloat));
  glVertexPointer(3, GL_FLOAT, stride, phantom_mask_scene::kVertices);
  glNormalPointer(GL_FLOAT, stride, phantom_mask_scene::kVertices + 3);
  glTexCoordPointer(2, GL_FLOAT, stride, phantom_mask_scene::kVertices + 6);

  if (vgp_active) {
    api.bind(kVertexProgram, program);
    UploadModelRows(api, world);
    glEnable(kVertexProgram);
    glDisable(GL_LIGHTING);
  } else {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    const GLfloat ambient[4] = {0.32f, 0.32f, 0.32f, 1.0f};
    const GLfloat diffuse[4] = {0.78f, 0.78f, 0.78f, 1.0f};
    const GLfloat light_position[4] = {0.2f, 0.8f, 1.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    const GLfloat material[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, material);
  }

  glDrawElements(GL_TRIANGLES, phantom_mask_scene::kIndexCount,
                 GL_UNSIGNED_SHORT, phantom_mask_scene::kIndices);

  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
  if (!vgp_active) {
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
  }
}

bool InitializeVgp(const VgpApi& api, GLuint* program) {
  const auto* extensions = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
  if (!HasExtension(extensions, "GL_IMG_vertex_program") || !api.gen ||
      !api.bind || !api.load || !api.set_env || !api.remove) {
    return false;
  }
  api.gen(1, program);
  api.bind(kVertexProgram, *program);
  api.load(kVertexProgram, kProgramFormatBinaryImg,
           static_cast<GLsizei>(AssetByteCount(phantom_vgp_program_begin,
                                               phantom_vgp_program_end)),
           phantom_vgp_program_begin);
  if (glGetError() != GL_NO_ERROR) {
    api.remove(1, program);
    *program = 0;
    return false;
  }
  const ShCoefficients sh = BuildShCoefficients();
  UploadShCoefficients(api, sh);
  if (glGetError() != GL_NO_ERROR) {
    api.remove(1, program);
    *program = 0;
    return false;
  }
  return true;
}

int RunWindow(WindowSurface* window) {
  GlesWindowContext context;
  if (!context.Open(window, 1).ok() || !context.MakeCurrent().ok()) {
    return KErrNotSupported;
  }
  const char* extensions = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
  if (!HasExtension(extensions, "GL_IMG_texture_compression_pvrtc")) {
    return KErrNotSupported;
  }

  GLuint room_texture = 0;
  GLuint mask_texture = 0;
  if (!LoadPvrV2Texture(phantom_room_still_pvr_begin,
                        static_cast<std::size_t>(phantom_room_still_pvr_end -
                                                 phantom_room_still_pvr_begin),
                        &room_texture) ||
      !LoadPvrV2Texture(phantom_mask_main_pvr_begin,
                        static_cast<std::size_t>(phantom_mask_main_pvr_end -
                                                 phantom_mask_main_pvr_begin),
                        &mask_texture)) {
    if (room_texture) glDeleteTextures(1, &room_texture);
    if (mask_texture) glDeleteTextures(1, &mask_texture);
    return KErrCorrupt;
  }

  const VgpApi api = LoadVgpApi();
  GLuint program = 0;
  const bool vgp_active = InitializeVgp(api, &program);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    DrawBackground(room_texture, vgp_active);

    const std::int64_t now_ns =
        symbian::api::time::MonotonicClock::NowNanoseconds();
    const std::uint64_t elapsed_frames = static_cast<std::uint64_t>(
        std::max<std::int64_t>(0, now_ns - start_ns) * kDemoFrameRate /
        kNanosecondsPerSecond);
    const unsigned frame =
        static_cast<unsigned>(elapsed_frames % phantom_mask_scene::kFrameCount);
    const float* camera = phantom_mask_scene::kCameraFrames + frame * 7;
    const float* world = phantom_mask_scene::kModelMatrices + frame * 16;
    DrawPhantomMask(camera, world, width, height, mask_texture, vgp_active, api,
                    program);

    if (!context.Swap().ok()) break;
    User::After(33333);
  }

  if (vgp_active && program) api.remove(1, &program);
  glDeleteTextures(1, &mask_texture);
  glDeleteTextures(1, &room_texture);
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
