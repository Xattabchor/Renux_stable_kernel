#include "application.h"

#include <e32std.h>
#include <EGL/egl.h>
#include <GLES/gl.h>

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "symbian/api/display/window_surface.h"

namespace {
using symbian::api::display::WindowInputKind;
using symbian::api::display::WindowKey;
using symbian::api::display::WindowSurface;

struct Glyph {
  char character;
  std::uint8_t rows[5];
};

// Compact 3x5 pixel font. Lowercase glyphs are kept distinct so extension
// names remain legible with their original spelling and case.
constexpr Glyph kGlyphs[] = {
    {' ', {0, 0, 0, 0, 0}}, {'A', {2, 5, 7, 5, 5}}, {'B', {6, 5, 6, 5, 6}},
    {'C', {3, 4, 4, 4, 3}}, {'D', {6, 5, 5, 5, 6}}, {'E', {7, 4, 6, 4, 7}},
    {'F', {7, 4, 6, 4, 4}}, {'G', {3, 4, 5, 5, 3}}, {'H', {5, 5, 7, 5, 5}},
    {'I', {7, 2, 2, 2, 7}}, {'J', {1, 1, 1, 5, 2}}, {'K', {5, 5, 6, 5, 5}},
    {'L', {4, 4, 4, 4, 7}}, {'M', {5, 7, 7, 5, 5}}, {'N', {5, 7, 7, 7, 5}},
    {'O', {2, 5, 5, 5, 2}}, {'P', {6, 5, 6, 4, 4}}, {'Q', {2, 5, 5, 7, 3}},
    {'R', {6, 5, 6, 5, 5}}, {'S', {3, 4, 2, 1, 6}}, {'T', {7, 2, 2, 2, 2}},
    {'U', {5, 5, 5, 5, 7}}, {'V', {5, 5, 5, 5, 2}}, {'W', {5, 5, 7, 7, 5}},
    {'X', {5, 5, 2, 5, 5}}, {'Y', {5, 5, 2, 2, 2}}, {'Z', {7, 1, 2, 4, 7}},
    {'a', {0, 3, 1, 3, 5}}, {'b', {4, 4, 6, 5, 6}}, {'c', {0, 3, 4, 4, 3}},
    {'d', {1, 1, 3, 5, 3}}, {'e', {2, 5, 6, 4, 3}}, {'f', {1, 2, 7, 2, 2}},
    {'g', {3, 5, 3, 1, 6}}, {'h', {4, 4, 6, 5, 5}}, {'i', {2, 0, 2, 2, 2}},
    {'j', {1, 0, 1, 5, 2}}, {'k', {4, 5, 6, 5, 5}}, {'l', {6, 2, 2, 2, 7}},
    {'m', {0, 6, 7, 5, 5}}, {'n', {0, 6, 5, 5, 5}}, {'o', {0, 2, 5, 5, 2}},
    {'p', {6, 5, 6, 4, 4}}, {'q', {3, 5, 3, 1, 1}}, {'r', {0, 5, 6, 4, 4}},
    {'s', {3, 4, 2, 1, 6}}, {'t', {2, 7, 2, 2, 1}}, {'u', {0, 5, 5, 5, 3}},
    {'v', {0, 5, 5, 5, 2}}, {'w', {0, 5, 5, 7, 5}}, {'x', {0, 5, 2, 2, 5}},
    {'y', {5, 5, 3, 1, 6}}, {'z', {0, 7, 1, 2, 7}},
    {'0', {7, 5, 5, 5, 7}}, {'1', {2, 6, 2, 2, 7}}, {'2', {6, 1, 2, 4, 7}},
    {'3', {6, 1, 2, 1, 6}}, {'4', {5, 5, 7, 1, 1}}, {'5', {7, 4, 6, 1, 6}},
    {'6', {3, 4, 6, 5, 2}}, {'7', {7, 1, 2, 2, 2}}, {'8', {2, 5, 2, 5, 2}},
    {'9', {2, 5, 3, 1, 6}}, {'_', {0, 0, 0, 0, 7}}, {':', {0, 2, 0, 2, 0}},
    {'.', {0, 0, 0, 0, 2}}, {'-', {0, 0, 7, 0, 0}}, {'/', {1, 1, 2, 4, 4}},
    {'=', {0, 7, 0, 7, 0}}, {'+', {0, 2, 7, 2, 0}}, {'(', {1, 2, 2, 2, 1}},
    {')', {4, 2, 2, 2, 4}}, {'?', {6, 1, 2, 0, 2}}, {',', {0, 0, 0, 2, 4}},
};

const Glyph& FindGlyph(char character) {
  for (const Glyph& glyph : kGlyphs) {
    if (glyph.character == character) return glyph;
  }
  for (const Glyph& glyph : kGlyphs) {
    if (glyph.character == '?') return glyph;
  }
  return kGlyphs[0];
}

bool HasProc(const char* name) {
  return eglGetProcAddress(name) != nullptr;
}

// The SDK's GlesWindowContext deliberately creates only OpenGL ES 2 contexts.
// This probe targets Symbian handsets such as the Nokia N95, whose PowerVR
// MBX driver exposes OpenGL ES 1.1. Use the EGL 1.x-compatible ES1 path
// directly: no EGL 1.2 eglBindAPI or ES2-only config/context attributes.
class Es1WindowContext final {
 public:
  Es1WindowContext() = default;
  Es1WindowContext(const Es1WindowContext&) = delete;
  Es1WindowContext& operator=(const Es1WindowContext&) = delete;
  ~Es1WindowContext() { Close(); }

  bool Open(WindowSurface* window) {
    if (window == nullptr || window->NativeWindowHandle() == nullptr) {
      return Fail("native window", EGL_BAD_NATIVE_WINDOW);
    }
    display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display_ == EGL_NO_DISPLAY) return Fail("eglGetDisplay", eglGetError());

    EGLint major = 0;
    EGLint minor = 0;
    if (!eglInitialize(display_, &major, &minor)) {
      return Fail("eglInitialize", eglGetError());
    }
    initialized_ = true;

    // EGL_RENDERABLE_TYPE and eglBindAPI are EGL 1.2 additions. The N95's
    // legacy EGL 1.x implementation is ES-only, so request just a window
    // surface with RGB565 and let eglCreateContext select its native ES API.
    const EGLint config_attributes[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 5,
        EGL_GREEN_SIZE, 6,
        EGL_BLUE_SIZE, 5,
        EGL_NONE};
    EGLConfig config = 0;
    EGLint config_count = 0;
    if (!eglChooseConfig(display_, config_attributes, &config, 1,
                         &config_count)) {
      return Fail("eglChooseConfig", eglGetError());
    }
    if (config_count == 0) return Fail("no RGB565 EGL config", EGL_SUCCESS);

    surface_ = eglCreateWindowSurface(
        display_, config,
        static_cast<EGLNativeWindowType>(window->NativeWindowHandle()),
        nullptr);
    if (surface_ == EGL_NO_SURFACE) {
      return Fail("eglCreateWindowSurface", eglGetError());
    }
    const EGLint context_attributes[] = {EGL_NONE};
    context_ = eglCreateContext(display_, config, EGL_NO_CONTEXT,
                                context_attributes);
    if (context_ == EGL_NO_CONTEXT) {
      return Fail("eglCreateContext", eglGetError());
    }
    if (!eglMakeCurrent(display_, surface_, surface_, context_)) {
      return Fail("eglMakeCurrent", eglGetError());
    }
    return true;
  }

  bool Swap() {
    if (display_ != EGL_NO_DISPLAY && surface_ != EGL_NO_SURFACE &&
        eglSwapBuffers(display_, surface_)) {
      return true;
    }
    return Fail("eglSwapBuffers", eglGetError());
  }

  const std::string& error_message() const { return error_message_; }

 private:
  bool Fail(const char* stage, EGLint error) {
    char formatted_error[16];
    std::snprintf(formatted_error, sizeof(formatted_error), "0x%04x",
                  static_cast<unsigned int>(error));
    error_message_ = std::string(stage) + " failed (EGL " + formatted_error + ")";
    Close();
    return false;
  }

  void Close() {
    if (display_ != EGL_NO_DISPLAY) {
      if (context_ != EGL_NO_CONTEXT) {
        eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE,
                       EGL_NO_CONTEXT);
        eglDestroyContext(display_, context_);
      }
      if (surface_ != EGL_NO_SURFACE) eglDestroySurface(display_, surface_);
      if (initialized_) eglTerminate(display_);
    }
    display_ = EGL_NO_DISPLAY;
    surface_ = EGL_NO_SURFACE;
    context_ = EGL_NO_CONTEXT;
    initialized_ = false;
  }

  EGLDisplay display_ = EGL_NO_DISPLAY;
  EGLSurface surface_ = EGL_NO_SURFACE;
  EGLContext context_ = EGL_NO_CONTEXT;
  bool initialized_ = false;
  std::string error_message_;
};

bool HasExtension(std::string_view list, std::string_view wanted) {
  std::size_t start = 0;
  while (start < list.size()) {
    while (start < list.size() && list[start] == ' ') ++start;
    std::size_t end = list.find(' ', start);
    if (end == std::string_view::npos) end = list.size();
    if (list.substr(start, end - start) == wanted) return true;
    start = end + 1;
  }
  return false;
}

void WrapAndAppend(std::vector<std::string>* lines, std::string_view text,
                   std::size_t max_chars) {
  if (!lines || max_chars == 0) return;
  while (text.size() > max_chars) {
    std::size_t split = text.rfind(' ', max_chars);
    if (split == std::string_view::npos || split == 0) split = max_chars;
    lines->emplace_back(text.substr(0, split));
    text.remove_prefix(split);
    while (!text.empty() && text.front() == ' ') text.remove_prefix(1);
  }
  lines->emplace_back(text);
}

void AddQuad(std::vector<GLfloat>* vertices, float left, float top,
             float right, float bottom) {
  const GLfloat quad[] = {left, top, right, top, right, bottom,
                          left, top, right, bottom, left, bottom};
  vertices->insert(vertices->end(), quad, quad + 12);
}

void AddText(std::vector<GLfloat>* vertices, std::string_view text, int x,
             int y, int scale) {
  for (std::size_t index = 0; index < text.size(); ++index) {
    const Glyph& glyph = FindGlyph(text[index]);
    for (int row = 0; row < 5; ++row) {
      for (int column = 0; column < 3; ++column) {
        if ((glyph.rows[row] & (1u << (2 - column))) == 0) continue;
        const float left = static_cast<float>(x + index * 4 * scale +
                                              column * scale);
        const float top = static_cast<float>(y + row * scale);
        AddQuad(vertices, left, top, left + scale, top + scale);
      }
    }
  }
}

void SetRgb565Pixel(symbian::api::display::Rgb565Frame* frame, int x, int y,
                    std::uint16_t color) {
  if (frame == nullptr || x < 0 || y < 0 || x >= frame->size.width ||
      y >= frame->size.height) {
    return;
  }
  const std::size_t offset = static_cast<std::size_t>(y) * frame->pitch_bytes +
                             static_cast<std::size_t>(x) * sizeof(color);
  std::memcpy(frame->pixels.data() + offset, &color, sizeof(color));
}

void DrawSoftwareText(symbian::api::display::Rgb565Frame* frame,
                      std::string_view text, int x, int y, int scale,
                      std::uint16_t color) {
  for (std::size_t index = 0; index < text.size(); ++index) {
    const Glyph& glyph = FindGlyph(text[index]);
    for (int row = 0; row < 5; ++row) {
      for (int column = 0; column < 3; ++column) {
        if ((glyph.rows[row] & (1u << (2 - column))) == 0) continue;
        for (int dy = 0; dy < scale; ++dy) {
          for (int dx = 0; dx < scale; ++dx) {
            SetRgb565Pixel(frame,
                           x + static_cast<int>(index) * 4 * scale +
                               column * scale + dx,
                           y + row * scale + dy, color);
          }
        }
      }
    }
  }
}

int ShowEglFailure(WindowSurface* window, std::string_view detail) {
  using symbian::api::display::Rgb565Frame;
  using symbian::api::display::WindowInputKind;
  using symbian::api::display::WindowKey;
  if (window == nullptr) return KErrNotSupported;

  const auto draw = [&]() -> bool {
    auto frame_result = window->CreateRgb565Frame();
    if (!frame_result.ok()) return false;
    Rgb565Frame frame = *frame_result;
    for (int y = 0; y < frame.size.height; ++y) {
      for (int x = 0; x < frame.size.width; ++x) {
        SetRgb565Pixel(&frame, x, y, 0x0841);
      }
    }
    const int scale = frame.size.width >= 220 ? 2 : 1;
    const int margin = 6 * scale;
    const std::size_t chars_per_line = static_cast<std::size_t>(
        std::max(8, (frame.size.width - 2 * margin) / (4 * scale)));
    std::vector<std::string> lines;
    WrapAndAppend(&lines, "GLES 1.x context failed", chars_per_line);
    WrapAndAppend(&lines, detail, chars_per_line);
    WrapAndAppend(&lines, "Send this EGL error to the developer", chars_per_line);
    WrapAndAppend(&lines, "Press Escape to close", chars_per_line);
    for (std::size_t index = 0; index < lines.size(); ++index) {
      DrawSoftwareText(&frame, lines[index], margin,
                       margin + static_cast<int>(index) * 7 * scale,
                       scale, 0xffff);
    }
    return window->Present().ok();
  };

  if (!draw()) return KErrNotSupported;
  bool running = true;
  while (running) {
    for (int count = 0; count < 32; ++count) {
      auto event = window->PollInput();
      if (!event.ok()) return KErrNone;
      if (!event->has_value()) break;
      const auto& input = **event;
      if (input.kind == WindowInputKind::kCloseRequested ||
          (input.kind == WindowInputKind::kKeyDown &&
           input.key == WindowKey::kEscape)) {
        running = false;
        break;
      }
      if (input.kind == WindowInputKind::kDisplayChanged && !draw()) {
        return KErrNone;
      }
    }
    if (running) User::After(100000);
  }
  return KErrNone;
}

struct PageLayout {
  int width = 1;
  int height = 1;
  int scale = 1;
  int margin = 5;
  int line_height = 7;
  std::vector<std::vector<GLfloat>> pages;
};

std::vector<std::string> MakeReport() {
  const char* version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
  const char* renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
  const char* vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
  const char* raw_extensions =
      reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
  const std::string_view extensions =
      raw_extensions ? std::string_view(raw_extensions) : std::string_view();
  std::vector<std::string> extension_names;
  std::size_t start = 0;
  while (start < extensions.size()) {
    while (start < extensions.size() && extensions[start] == ' ') ++start;
    if (start == extensions.size()) break;
    std::size_t end = extensions.find(' ', start);
    if (end == std::string_view::npos) end = extensions.size();
    extension_names.emplace_back(extensions.substr(start, end - start));
    start = end + 1;
  }

  std::vector<std::string> lines;
  lines.emplace_back("GL extension probe (runtime context)");
  lines.emplace_back(std::string("Version: ") + (version ? version : "(null)"));
  lines.emplace_back(std::string("Renderer: ") + (renderer ? renderer : "(null)"));
  lines.emplace_back(std::string("Vendor: ") + (vendor ? vendor : "(null)"));
  lines.emplace_back("Extensions reported: " +
                     std::to_string(extension_names.size()));
  lines.emplace_back(std::string("GL_IMG_vertex_program: ") +
                     (HasExtension(extensions, "GL_IMG_vertex_program")
                          ? "advertised"
                          : "not advertised"));
  lines.emplace_back("EGL proc-address tests:");
  lines.emplace_back(std::string("glGenProgramsARB: ") +
                     (HasProc("glGenProgramsARB") ? "yes" : "no"));
  lines.emplace_back(std::string("glBindProgramARB: ") +
                     (HasProc("glBindProgramARB") ? "yes" : "no"));
  lines.emplace_back(std::string("glProgramStringARB: ") +
                     (HasProc("glProgramStringARB") ? "yes" : "no"));
  lines.emplace_back(std::string("glProgramEnvParameter4fvARB: ") +
                     (HasProc("glProgramEnvParameter4fvARB")
                          ? "yes"
                          : "no"));
  lines.emplace_back(std::string("glDeleteProgramsARB: ") +
                     (HasProc("glDeleteProgramsARB") ? "yes" : "no"));
  lines.emplace_back("GL_OES_matrix_palette procs:");
  lines.emplace_back(std::string("glCurrentPaletteMatrixOES: ") +
                     (HasProc("glCurrentPaletteMatrixOES")
                          ? "yes"
                          : "no"));
  lines.emplace_back(std::string("glLoadPaletteFromModelViewMatrixOES: ") +
                     (HasProc("glLoadPaletteFromModelViewMatrixOES")
                          ? "yes"
                          : "no"));
  lines.emplace_back(std::string("glMatrixIndexPointerOES: ") +
                     (HasProc("glMatrixIndexPointerOES")
                          ? "yes"
                          : "no"));
  lines.emplace_back(std::string("glWeightPointerOES: ") +
                     (HasProc("glWeightPointerOES") ? "yes" : "no"));
  lines.emplace_back("Runtime GL_EXTENSIONS tokens:");
  for (const std::string& extension : extension_names) {
    lines.push_back(extension);
  }
  return lines;
}

PageLayout MakePageLayout(const std::vector<std::string>& report,
                          symbian::api::display::WindowSize size) {
  PageLayout layout;
  layout.width = std::max(1, size.width);
  layout.height = std::max(1, size.height);
  layout.scale = layout.width >= 220 ? 2 : 1;
  layout.margin = 5 * layout.scale;
  layout.line_height = 7 * layout.scale;
  const std::size_t chars_per_line = static_cast<std::size_t>(
      std::max(8, (layout.width - 2 * layout.margin) / (4 * layout.scale)));

  std::vector<std::string> wrapped;
  for (const std::string& line : report) {
    WrapAndAppend(&wrapped, line, chars_per_line);
  }
  const std::size_t lines_per_page = static_cast<std::size_t>(
      std::max(1, (layout.height - 3 * layout.margin) / layout.line_height));
  const std::size_t page_count =
      std::max<std::size_t>(1, (wrapped.size() + lines_per_page - 1) /
                                  lines_per_page);
  layout.pages.reserve(page_count);
  for (std::size_t page = 0; page < page_count; ++page) {
    const std::size_t first_line = page * lines_per_page;
    const std::size_t last_line =
        std::min(wrapped.size(), first_line + lines_per_page);
    std::vector<GLfloat> vertices;
    for (std::size_t line = first_line; line < last_line; ++line) {
      AddText(&vertices, wrapped[line], layout.margin,
              layout.margin + static_cast<int>(line - first_line) *
                                  layout.line_height,
              layout.scale);
    }
    const std::string footer = "LEFT/RIGHT OR TAP: PAGE " +
                               std::to_string(page + 1) + "/" +
                               std::to_string(page_count);
    AddText(&vertices, footer, layout.margin,
            layout.height - 2 * layout.margin, layout.scale);
    layout.pages.push_back(std::move(vertices));
  }
  return layout;
}

int RunWindow(WindowSurface* window) {
  Es1WindowContext context;
  if (!context.Open(window)) {
    return ShowEglFailure(window, context.error_message());
  }

  const std::vector<std::string> report = MakeReport();
  PageLayout layout = MakePageLayout(report, window->size());
  bool running = true;
  std::size_t page = 0;
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
      if (input.kind == WindowInputKind::kKeyDown) {
        if (input.key == WindowKey::kRight || input.key == WindowKey::kDown ||
            input.key == WindowKey::kEnter || input.key == WindowKey::kSelect) {
          ++page;
        } else if (input.key == WindowKey::kLeft ||
                   input.key == WindowKey::kUp ||
                   input.key == WindowKey::kBackspace) {
          if (page > 0) --page;
        }
      } else if (input.kind == WindowInputKind::kPointerDown) {
        if (input.x >= window->size().width / 2) {
          ++page;
        } else if (page > 0) {
          --page;
        }
      } else if (input.kind == WindowInputKind::kDisplayChanged) {
        layout = MakePageLayout(report, window->size());
        page = 0;
      }
    }
    if (!running) break;

    const std::size_t page_count = layout.pages.size();
    if (page_count == 0) break;
    page %= page_count;
    const int width = layout.width;
    const int height = layout.height;
    const std::vector<GLfloat>& vertices = layout.pages[page];

    glViewport(0, 0, width, height);
    glClearColor(0.015f, 0.025f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_TEXTURE_2D);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrthof(0.0f, static_cast<GLfloat>(width),
             static_cast<GLfloat>(height), 0.0f, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glColor4f(0.45f, 1.0f, 0.68f, 1.0f);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, vertices.empty() ? nullptr : vertices.data());
    glDrawArrays(GL_TRIANGLES, 0,
                 static_cast<GLsizei>(vertices.size() / 2));
    glDisableClientState(GL_VERTEX_ARRAY);
    if (!context.Swap()) break;
    User::After(100000);
  }
  return KErrNone;
}
}  // namespace

int RunApplication() {
  WindowSurface window;
  if (!window.Open("GLES Extension Probe").ok()) {
    return KErrNotSupported;
  }
  return RunWindow(&window);
}
