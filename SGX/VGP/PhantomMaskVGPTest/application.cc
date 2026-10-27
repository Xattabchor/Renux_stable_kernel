#include "application.h"

#include <e32std.h>
#include <GLES/gl.h>

#include <cstdint>
#include <cstring>
#include <string>
#include <string_view>

#include "symbian/api/display/gles_window_context.h"
#include "symbian/api/display/window_surface.h"
#include "vgp_program.h"

namespace {
using symbian::api::display::GlesWindowContext;
using symbian::api::display::WindowInputKind;
using symbian::api::display::WindowKey;
using symbian::api::display::WindowSurface;

constexpr GLenum kVertexProgram = 0x8620;
constexpr GLenum kProgramFormatBinaryImg = 0x8C0B;

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

bool InstallVgpProgram(const VgpApi& api, GLuint* program) {
  const auto* extensions = glGetString(GL_EXTENSIONS);
  if (!extensions || !api.gen || !api.bind || !api.load || !api.set_env ||
      !api.remove) {
    return false;
  }
  const char* extension_text = reinterpret_cast<const char*>(extensions);
  bool has_img_vgp = false;
  for (const char* cursor = extension_text;
       (cursor = std::strstr(cursor, "GL_IMG_vertex_program")); cursor += 21) {
    const bool left = cursor == extension_text || cursor[-1] == ' ';
    const char after = cursor[21];
    if (left && (after == '\0' || after == ' ')) {
      has_img_vgp = true;
      break;
    }
  }
  if (!has_img_vgp) {
    return false;
  }

  api.gen(1, program);
  api.bind(kVertexProgram, *program);
  api.load(kVertexProgram, kProgramFormatBinaryImg,
           static_cast<GLsizei>(phantom_mask_vgp::kProgramSize),
           phantom_mask_vgp::kProgram);
  if (glGetError() != GL_NO_ERROR) {
    api.remove(1, program);
    *program = 0;
    return false;
  }

  const GLfloat model_rows[3][4] = {
      {1.0f, 0.0f, 0.0f, 0.0f},
      {0.0f, 1.0f, 0.0f, 0.0f},
      {0.0f, 0.0f, 1.0f, 0.0f},
  };
  for (GLuint row = 0; row < 3; ++row) {
    api.set_env(kVertexProgram, 4 + row, model_rows[row]);
  }
  const GLfloat c_ar[4] = {0.0f, 0.0f, 0.0f, 0.82f};
  const GLfloat c_ag[4] = {0.0f, 0.0f, 0.0f, 0.48f};
  const GLfloat c_ab[4] = {0.0f, 0.0f, 0.0f, 0.18f};
  const GLfloat zero[4] = {0.0f, 0.0f, 0.0f, 0.0f};
  api.set_env(kVertexProgram, 7, c_ar);
  api.set_env(kVertexProgram, 8, c_ag);
  api.set_env(kVertexProgram, 9, c_ab);
  api.set_env(kVertexProgram, 10, zero);
  api.set_env(kVertexProgram, 11, zero);
  api.set_env(kVertexProgram, 12, zero);
  api.set_env(kVertexProgram, 13, zero);
  glEnable(kVertexProgram);
  return glGetError() == GL_NO_ERROR;
}

int RunWindow(WindowSurface* window) {
  GlesWindowContext context;
  if (!context.Open(window, 1).ok() || !context.MakeCurrent().ok()) {
    return KErrNotSupported;
  }

  const VgpApi api = LoadVgpApi();
  GLuint program = 0;
  const bool vgp_active = InstallVgpProgram(api, &program);
  if (!vgp_active) {
    glDisable(kVertexProgram);
  }

  constexpr GLfloat positions[] = {
      -0.78f, -0.68f, 0.0f,
       0.78f, -0.68f, 0.0f,
       0.00f,  0.78f, 0.0f,
  };
  constexpr GLfloat normals[] = {
      0.0f, 0.0f, 1.0f,
      0.0f, 0.0f, 1.0f,
      0.0f, 0.0f, 1.0f,
  };
  constexpr GLfloat texcoords[] = {0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f};
  glVertexPointer(3, GL_FLOAT, 0, positions);
  glNormalPointer(GL_FLOAT, 0, normals);
  glTexCoordPointer(2, GL_FLOAT, 0, texcoords);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  glClearColor(vgp_active ? 0.015f : 0.16f,
               vgp_active ? 0.025f : 0.012f,
               vgp_active ? 0.04f : 0.012f, 1.0f);

  bool running = true;
  while (running) {
    for (int count = 0; count < 32; ++count) {
      auto event = window->PollInput();
      if (!event.ok()) {
        running = false;
        break;
      }
      if (!event->has_value()) {
        break;
      }
      const auto& input = **event;
      if (input.kind == WindowInputKind::kCloseRequested ||
          (input.kind == WindowInputKind::kKeyDown &&
           input.key == WindowKey::kEscape)) {
        running = false;
        break;
      }
      if (input.kind == WindowInputKind::kFocusLost) {
        User::After(80000);
      }
    }
    if (!running) {
      break;
    }
    const auto size = window->size();
    glViewport(0, 0, size.width, size.height);
    glClear(GL_COLOR_BUFFER_BIT);
    if (!vgp_active) {
      glColor4f(0.95f, 0.2f, 0.08f, 1.0f);
    }
    glDrawArrays(GL_TRIANGLES, 0, 3);
    if (!context.Swap().ok()) {
      break;
    }
    User::After(16000);
  }

  if (vgp_active) {
    api.remove(1, &program);
  }
  context.Close();
  return KErrNone;
}
}  // namespace

int RunApplication() {
  WindowSurface window;
  if (!window.Open("PhantomMask VGP Test").ok()) {
    return KErrNotSupported;
  }
  return RunWindow(&window);
}
