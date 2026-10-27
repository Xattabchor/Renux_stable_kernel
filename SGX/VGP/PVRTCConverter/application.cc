#include "application.h"

#include <e32std.h>
#include <gdi.h>
#include <w32std.h>

#include <algorithm>
#include <string>

#include "converter.h"

namespace {

constexpr TInt kWindowHandle = 2;
constexpr TInt kControlEncode = 0;
constexpr TInt kControlDecode = 1;
constexpr TInt kControlRate = 2;
constexpr TInt kControlOutput = 3;
constexpr TInt kControlRun = 4;
constexpr TInt kControlClose = 5;

struct UiState {
  bool encode = true;
  unsigned bpp = 4;
  bool bmp_format = false;
  bool attempted = false;
  TInt result = KErrNone;
  TBuf<128> status;
};

void Fill(CWindowGc* gc, const TRect& rect, TUint color) {
  gc->SetPenStyle(CGraphicsContext::ENullPen);
  gc->SetBrushStyle(CGraphicsContext::ESolidBrush);
  gc->SetBrushColor(TRgb((color >> 16) & 255, (color >> 8) & 255, color & 255));
  gc->DrawRect(rect);
}

void DrawWrapped(CWindowGc* gc, const TDesC& text, TInt x, TInt y,
                 TInt max_width, TInt line_height) {
  TBuf<64> line;
  TInt start = 0;
  while (start < text.Length()) {
    TInt length = 0;
    line.Zero();
    while (start + length < text.Length() && length < line.MaxLength()) {
      const TChar ch = text[start + length];
      line.Append(ch);
      ++length;
      if (line.Length() > 1 && line.Length() * 7 > max_width) {
        line.Delete(line.Length() - 1, 1);
        --length;
        break;
      }
    }
    if (length == 0) break;
    gc->DrawText(line, TPoint(x, y));
    start += length;
    while (start < text.Length() && text[start] == ' ') ++start;
    y += line_height;
  }
}

TRect ButtonRect(const TSize& size, TInt index) {
  const TInt margin = 8;
  const TInt gap = 5;
  const TInt height = 34;
  const TInt row = index / 2;
  const TInt column = index % 2;
  const TInt top = size.iHeight - 3 * (height + gap) - margin + row * (height + gap);
  const TInt mid = size.iWidth / 2;
  return column == 0 ? TRect(margin, top, mid - gap / 2, top + height)
                     : TRect(mid + gap / 2, top, size.iWidth - margin, top + height);
}

void Draw(CWsScreenDevice* screen, CWindowGc* gc, const TSize& size,
          const UiState& state) {
  CFont* font = nullptr;
  TFontSpec font_spec(_L("Series 60 Sans"), 12);
  if (screen->GetNearestFontInPixels(font, font_spec) != KErrNone) return;
  gc->UseFont(font);
  Fill(gc, TRect(TPoint(0, 0), size), 0x00131d2a);
  Fill(gc, TRect(8, 8, size.iWidth - 8, 40), 0x002b6683);
  const TInt status_top = size.iHeight - 3 * (34 + 5) - 8 - 50;
  Fill(gc, TRect(8, 48, size.iWidth - 8, status_top - 8), 0x00212e3f);
  gc->SetPenStyle(CGraphicsContext::ESolidPen);
  gc->SetPenColor(TRgb(255, 255, 255));
  gc->DrawText(_L("PVRTC1 IMAGE CONVERTER"), TPoint(16, 29));

  DrawWrapped(gc, _L("C:\\Data\\PVRTCConverter"), 16, 68,
              size.iWidth - 32, 16);
  DrawWrapped(gc, _L("Encode image to output.pvr"), 16, 91,
              size.iWidth - 32, 16);
  DrawWrapped(gc, _L("Decode input.pvr to image"), 16, 114,
              size.iWidth - 32, 16);

  Fill(gc, TRect(8, status_top, size.iWidth - 8, status_top + 42),
       !state.attempted ? 0x003b526c : state.result == KErrNone ? 0x002e7d62 : 0x009d4b52);
  if (state.attempted) {
    DrawWrapped(gc, state.status, 16, status_top + 17, size.iWidth - 32, 16);
  } else {
    DrawWrapped(gc, _L("Ready. Choose an operation and press RUN."), 16,
                status_top + 17, size.iWidth - 32, 16);
  }

  const TUint colors[] = {0x002b6683, 0x002b6683, 0x005c557e,
                          0x005c557e, 0x002e7d62, 0x009d4b52};
  TBuf<24> labels[6];
  labels[kControlEncode].Copy(_L("ENCODE"));
  labels[kControlDecode].Copy(_L("DECODE"));
  if (state.bpp == 2) {
    labels[kControlRate].Copy(_L("PVRTC 2 BPP"));
  } else {
    labels[kControlRate].Copy(_L("PVRTC 4 BPP"));
  }
  if (state.encode) {
    if (state.bmp_format) {
      labels[kControlOutput].Copy(_L("INPUT BMP"));
    } else {
      labels[kControlOutput].Copy(_L("INPUT PNG"));
    }
  } else if (state.bmp_format) {
    labels[kControlOutput].Copy(_L("OUTPUT BMP"));
  } else {
    labels[kControlOutput].Copy(_L("OUTPUT PNG"));
  }
  labels[kControlRun].Copy(_L("RUN"));
  labels[kControlClose].Copy(_L("CLOSE"));
  for (TInt index = 0; index < 6; ++index) {
    const TRect rect = ButtonRect(size, index);
    const TUint color = index < 2 && ((index == kControlEncode) == state.encode)
                            ? 0x003e8e70
                            : colors[index];
    Fill(gc, rect, color);
    gc->DrawText(labels[index], TPoint(rect.iTl.iX + 8, rect.iTl.iY + 22));
  }
  gc->DiscardFont();
  screen->ReleaseFont(font);
}

TInt Convert(UiState* state) {
  if (!state) return KErrArgument;
  const TFileName directory(_L("C:\\Data\\PVRTCConverter\\"));
  RFs file_server;
  TInt result = file_server.Connect();
  if (result != KErrNone) {
    state->status.Copy(_L("Cannot connect to file server."));
    return result;
  }
  result = file_server.MkDirAll(directory);
  file_server.Close();
  if (result != KErrNone && result != KErrAlreadyExists) {
    state->status.Copy(_L("Cannot create converter folder."));
    return result;
  }

  std::string error;
  bool ok = false;
  if (state->encode) {
    pvrtc_converter::Image input;
    const std::string png_path = "C:\\Data\\PVRTCConverter\\input.png";
    const std::string bmp_path = "C:\\Data\\PVRTCConverter\\input.bmp";
    const std::string& preferred_path = state->bmp_format ? bmp_path : png_path;
    const std::string& fallback_path = state->bmp_format ? png_path : bmp_path;
    if (!pvrtc_converter::ReadImage(preferred_path, &input, &error) &&
        !pvrtc_converter::ReadImage(fallback_path, &input, &error)) {
      state->status.Copy(_L("Missing input.png or input.bmp."));
      return KErrNotFound;
    }
    pvrtc_converter::Texture texture;
    ok = pvrtc_converter::Encode(input, state->bpp, &texture, &error);
    if (ok) ok = pvrtc_converter::WritePvr("C:\\Data\\PVRTCConverter\\output.pvr", texture, &error);
  } else {
    pvrtc_converter::Texture texture;
    ok = pvrtc_converter::ReadPvr("C:\\Data\\PVRTCConverter\\input.pvr", &texture, &error);
    if (ok) {
      pvrtc_converter::Image output;
      ok = pvrtc_converter::Decode(texture, &output, &error);
      if (ok) {
        const std::string output_path = state->bmp_format
            ? "C:\\Data\\PVRTCConverter\\output.bmp"
            : "C:\\Data\\PVRTCConverter\\output.png";
        ok = pvrtc_converter::WriteImage(output_path, output, &error);
      }
    }
  }
  if (ok) {
    state->status.Copy(_L("Done. See the output file."));
    return KErrNone;
  }
  TPtrC8 detail(reinterpret_cast<const TUint8*>(error.c_str()),
                static_cast<TInt>(std::min<std::size_t>(error.size(), 52)));
  state->status.Copy(detail);
  return KErrCorrupt;
}

TInt Display(RWsSession* session, CWsScreenDevice* screen, CWindowGc* gc) {
  const TSize size = screen->SizeInPixels();
  RWindowGroup group(*session);
  TInt result = group.Construct(1, ETrue);
  if (result != KErrNone) return result;
  RWindow window(*session);
  result = window.Construct(group, kWindowHandle);
  if (result != KErrNone) {
    group.Close();
    return result;
  }
  group.SetOrdinalPosition(0);
  window.SetExtent(TPoint(0, 0), size);
  window.SetVisible(ETrue);
  window.Activate();

  UiState state;
  bool running = true;
  TRequestStatus event_status;
  TRequestStatus redraw_status;
  session->EventReady(&event_status);
  session->RedrawReady(&redraw_status);
  window.Invalidate();
  session->Flush();
  while (running) {
    User::WaitForRequest(event_status, redraw_status);
    if (event_status != KRequestPending) {
      result = event_status.Int();
      if (result != KErrNone) break;
      TWsEvent event;
      session->GetEvent(event);
      if (event.Handle() == kWindowHandle && event.Type() == EEventPointer &&
          event.Pointer()->iType == TPointerEvent::EButton1Down) {
        const TPoint point = event.Pointer()->iPosition;
        for (TInt index = 0; index < 6; ++index) {
          if (!ButtonRect(size, index).Contains(point)) continue;
          if (index == kControlEncode) {
            state.encode = true;
            state.attempted = false;
          } else if (index == kControlDecode) {
            state.encode = false;
            state.attempted = false;
          } else if (index == kControlRate) {
            state.bpp = state.bpp == 2 ? 4 : 2;
            state.attempted = false;
          } else if (index == kControlOutput) {
            state.bmp_format = !state.bmp_format;
            state.attempted = false;
          } else if (index == kControlRun) {
            state.attempted = true;
            state.result = Convert(&state);
          } else {
            running = false;
          }
          window.Invalidate();
          break;
        }
      } else if (event.Type() == EEventKey) {
        const TInt code = event.Key()->iCode;
        if (code == '1') {
          state.encode = true;
          state.attempted = false;
        } else if (code == '2') {
          state.encode = false;
          state.attempted = false;
        } else if (code == '3') {
          state.bpp = state.bpp == 2 ? 4 : 2;
          state.attempted = false;
        } else if (code == '4') {
          state.bmp_format = !state.bmp_format;
          state.attempted = false;
        } else if (code == '5' || code == EKeyEnter) {
          state.attempted = true;
          state.result = Convert(&state);
        } else if (code == '0' || code == EKeyEscape) {
          running = false;
        }
        window.Invalidate();
      }
      if (running) session->EventReady(&event_status);
    }
    if (running && redraw_status != KRequestPending) {
      result = redraw_status.Int();
      if (result != KErrNone) break;
      TWsRedrawEvent redraw;
      session->GetRedraw(redraw);
      if (redraw.Handle() == kWindowHandle) {
        window.BeginRedraw(redraw.Rect());
        gc->Activate(window);
        Draw(screen, gc, size, state);
        gc->Deactivate();
        window.EndRedraw();
      }
      session->RedrawReady(&redraw_status);
    }
    session->Flush();
  }
  session->EventReadyCancel();
  session->RedrawReadyCancel();
  if (event_status == KRequestPending) User::WaitForRequest(event_status);
  if (redraw_status == KRequestPending) User::WaitForRequest(redraw_status);
  window.Close();
  group.Close();
  session->Flush();
  return result;
}

}  // namespace

int RunApplication() {
  RWsSession session;
  TInt result = session.Connect();
  if (result != KErrNone) return result;
  {
    CWsScreenDevice screen(session);
    result = screen.Construct();
    if (result == KErrNone) {
      CWindowGc gc(&screen);
      result = gc.Construct();
      if (result == KErrNone) result = Display(&session, &screen, &gc);
    }
  }
  session.Close();
  return result;
}
