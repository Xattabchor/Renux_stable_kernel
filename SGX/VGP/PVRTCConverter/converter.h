#ifndef RENUX_PVRTC_CONVERTER_CONVERTER_H_
#define RENUX_PVRTC_CONVERTER_CONVERTER_H_

#include <cstdint>
#include <string>
#include <vector>

namespace pvrtc_converter {

struct Image {
  std::uint32_t width = 0;
  std::uint32_t height = 0;
  // Straight (not premultiplied) RGBA, row-major, top-to-bottom.
  std::vector<std::uint8_t> rgba;
};

struct Texture {
  std::uint32_t width = 0;
  std::uint32_t height = 0;
  std::uint32_t source_width = 0;
  std::uint32_t source_height = 0;
  unsigned bits_per_pixel = 0;
  bool has_alpha = true;
  std::vector<std::uint8_t> blocks;
};

// Input image dimensions need not be powers of two. Encoding edge-pads to the
// next PVRTC-compatible power-of-two size and records the original size in PVR
// metadata so this converter can crop it again when decoding.
bool Encode(const Image& image, unsigned bits_per_pixel, Texture* texture,
            std::string* error);
bool Decode(const Texture& texture, Image* image, std::string* error);

bool ReadPvr(const std::string& path, Texture* texture, std::string* error);
bool WritePvr(const std::string& path, const Texture& texture,
              std::string* error);
bool ReadImage(const std::string& path, Image* image, std::string* error);
bool WriteImage(const std::string& path, const Image& image,
                std::string* error);

}  // namespace pvrtc_converter

#endif  // RENUX_PVRTC_CONVERTER_CONVERTER_H_
