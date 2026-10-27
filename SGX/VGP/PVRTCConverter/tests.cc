#include "converter.h"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace {

void Check(bool condition, const char* message) {
  if (!condition) {
    std::cerr << "FAIL: " << message << '\n';
    std::exit(1);
  }
}

pvrtc_converter::Image MakeGradient(std::uint32_t width, std::uint32_t height) {
  pvrtc_converter::Image image;
  image.width = width;
  image.height = height;
  image.rgba.resize(static_cast<std::size_t>(width) * height * 4);
  for (std::uint32_t y = 0; y < height; ++y) {
    for (std::uint32_t x = 0; x < width; ++x) {
      const std::size_t offset = (static_cast<std::size_t>(y) * width + x) * 4;
      image.rgba[offset] = static_cast<std::uint8_t>(x * 255 / (width - 1));
      image.rgba[offset + 1] = static_cast<std::uint8_t>(y * 255 / (height - 1));
      image.rgba[offset + 2] = static_cast<std::uint8_t>((x + y) * 255 / (width + height - 2));
      image.rgba[offset + 3] = static_cast<std::uint8_t>(64 + (x * 191 / (width - 1)));
    }
  }
  return image;
}

void TestMode(unsigned bpp) {
  pvrtc_converter::Image input = MakeGradient(19, 11);
  pvrtc_converter::Texture encoded;
  std::string error;
  Check(pvrtc_converter::Encode(input, bpp, &encoded, &error), error.c_str());
  Check(encoded.width >= input.width && encoded.height >= input.height,
        "encoder must pad to PVRTC block/power-of-two dimensions");
  Check(encoded.bits_per_pixel == bpp, "encoder must retain selected bit rate");

  const std::string pvr_path = std::string("/tmp/pvrtc_converter_") +
                               std::to_string(bpp) + ".pvr";
  Check(pvrtc_converter::WritePvr(pvr_path, encoded, &error), error.c_str());
  pvrtc_converter::Texture loaded;
  Check(pvrtc_converter::ReadPvr(pvr_path, &loaded, &error), error.c_str());
  Check(loaded.width == encoded.width && loaded.height == encoded.height &&
            loaded.source_width == input.width && loaded.source_height == input.height,
        "PVR v3 header and crop metadata must survive save/load");
  pvrtc_converter::Image decoded;
  Check(pvrtc_converter::Decode(loaded, &decoded, &error), error.c_str());
  Check(decoded.width == input.width && decoded.height == input.height,
        "decoder must restore original image dimensions");
  Check(decoded.rgba.size() == input.rgba.size(), "decoded RGBA buffer size");

  long long error_sum = 0;
  for (std::size_t i = 0; i < input.rgba.size(); ++i) {
    error_sum += std::abs(static_cast<int>(input.rgba[i]) - decoded.rgba[i]);
  }
  const double mean_error = static_cast<double>(error_sum) / input.rgba.size();
  std::cout << bpp << "bpp gradient mean absolute RGBA error: " << mean_error << '\n';
  Check(mean_error < 100.0, "encoder/decoder round trip must produce bounded image error");
  std::remove(pvr_path.c_str());
}

void TestBmp() {
  pvrtc_converter::Image image = MakeGradient(13, 7);
  const std::string path = "/tmp/pvrtc_converter_test.bmp";
  std::string error;
  Check(pvrtc_converter::WriteImage(path, image, &error), error.c_str());
  pvrtc_converter::Image loaded;
  Check(pvrtc_converter::ReadImage(path, &loaded, &error), error.c_str());
  Check(loaded.width == image.width && loaded.height == image.height,
        "BMP dimensions must round trip");
  Check(loaded.rgba == image.rgba, "32-bit BMP RGBA data must round trip exactly");
  std::remove(path.c_str());
}

void TestPng() {
  pvrtc_converter::Image image = MakeGradient(9, 5);
  const std::string path = "/tmp/pvrtc_converter_test.png";
  std::string error;
  Check(pvrtc_converter::WriteImage(path, image, &error), error.c_str());
  pvrtc_converter::Image loaded;
  Check(pvrtc_converter::ReadImage(path, &loaded, &error), error.c_str());
  Check(loaded.width == image.width && loaded.height == image.height,
        "PNG dimensions must round trip");
  Check(loaded.rgba == image.rgba, "PNG RGBA data must round trip exactly");
  std::remove(path.c_str());
}

void PutU32(std::vector<std::uint8_t>* bytes, std::size_t offset,
            std::uint32_t value) {
  (*bytes)[offset] = static_cast<std::uint8_t>(value);
  (*bytes)[offset + 1] = static_cast<std::uint8_t>(value >> 8);
  (*bytes)[offset + 2] = static_cast<std::uint8_t>(value >> 16);
  (*bytes)[offset + 3] = static_cast<std::uint8_t>(value >> 24);
}

void TestLegacyPvrV2() {
  pvrtc_converter::Image image = MakeGradient(16, 8);
  pvrtc_converter::Texture texture;
  std::string error;
  Check(pvrtc_converter::Encode(image, 4, &texture, &error), error.c_str());
  std::vector<std::uint8_t> legacy(52 + texture.blocks.size(), 0);
  PutU32(&legacy, 0, 52);
  PutU32(&legacy, 4, texture.height);
  PutU32(&legacy, 8, texture.width);
  PutU32(&legacy, 16, 25);  // OGL_PVRTC4 in PVR v2.
  PutU32(&legacy, 20, static_cast<std::uint32_t>(texture.blocks.size()));
  PutU32(&legacy, 24, 4);
  PutU32(&legacy, 44, 0x21525650u);  // "PVR!"
  PutU32(&legacy, 48, 1);
  std::copy(texture.blocks.begin(), texture.blocks.end(), legacy.begin() + 52);
  const std::string path = "/tmp/pvrtc_converter_legacy.pvr";
  FILE* file = std::fopen(path.c_str(), "wb");
  Check(file != nullptr, "create legacy PVR fixture");
  Check(std::fwrite(legacy.data(), 1, legacy.size(), file) == legacy.size(),
        "write legacy PVR fixture");
  std::fclose(file);
  pvrtc_converter::Texture loaded;
  Check(pvrtc_converter::ReadPvr(path, &loaded, &error), error.c_str());
  Check(loaded.bits_per_pixel == 4 && loaded.width == texture.width,
        "legacy PVR v2 format and dimensions must be recognized");
  pvrtc_converter::Image decoded;
  Check(pvrtc_converter::Decode(loaded, &decoded, &error), error.c_str());
  Check(decoded.width == image.width && decoded.height == image.height,
        "legacy PVR v2 payload must decode");
  std::remove(path.c_str());
}

}  // namespace

int main() {
  TestMode(2);
  TestMode(4);
  TestBmp();
  TestPng();
  TestLegacyPvrV2();
  std::cout << "PVRTC converter tests passed.\n";
  return 0;
}
