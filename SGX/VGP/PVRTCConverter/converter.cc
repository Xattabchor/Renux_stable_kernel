#include "converter.h"

#include <png.h>

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <limits>
#include <string>
#include <vector>

#include "PVRTDecompress.h"

namespace pvrtc_converter {
namespace {

constexpr std::uint32_t kPvr3Magic = 0x03525650u;
constexpr std::uint32_t kRenuxFourCc = 0x554e4552u;  // "RENU" little-endian.
constexpr std::uint32_t kOriginalSizeKey = 1;
constexpr std::uint32_t kMaxDimension = 16384;
constexpr std::uint64_t kMaxImageBytes = 512ull * 1024ull * 1024ull;

bool Fail(std::string* error, const char* message) {
  if (error) *error = message;
  return false;
}

std::uint32_t ReadU32(const std::uint8_t* p) {
  return static_cast<std::uint32_t>(p[0]) |
         (static_cast<std::uint32_t>(p[1]) << 8) |
         (static_cast<std::uint32_t>(p[2]) << 16) |
         (static_cast<std::uint32_t>(p[3]) << 24);
}

void AppendU32(std::vector<std::uint8_t>* out, std::uint32_t value) {
  out->push_back(static_cast<std::uint8_t>(value));
  out->push_back(static_cast<std::uint8_t>(value >> 8));
  out->push_back(static_cast<std::uint8_t>(value >> 16));
  out->push_back(static_cast<std::uint8_t>(value >> 24));
}

bool CheckedImageSize(std::uint32_t width, std::uint32_t height,
                      std::size_t* bytes) {
  if (!width || !height || width > kMaxDimension || height > kMaxDimension)
    return false;
  const std::uint64_t count = static_cast<std::uint64_t>(width) * height * 4;
  if (count > kMaxImageBytes || count > std::numeric_limits<std::size_t>::max())
    return false;
  *bytes = static_cast<std::size_t>(count);
  return true;
}

std::uint32_t NextPowerOfTwo(std::uint32_t value) {
  std::uint32_t result = 1;
  while (result < value && result < kMaxDimension) result <<= 1;
  return result;
}

std::uint32_t MortonIndex(std::uint32_t height, std::uint32_t width,
                          std::uint32_t y, std::uint32_t x) {
  const std::uint32_t min_dimension = std::min(height, width);
  const std::uint32_t max_value = height < width ? x : y;
  std::uint32_t result = 0;
  std::uint32_t src_bit = 1;
  std::uint32_t dst_bit = 1;
  unsigned shifts = 0;
  while (src_bit < min_dimension) {
    if (y & src_bit) result |= dst_bit;
    if (x & src_bit) result |= dst_bit << 1;
    src_bit <<= 1;
    dst_bit <<= 2;
    ++shifts;
  }
  result |= (max_value >> shifts) << (2 * shifts);
  return result;
}

struct Color {
  int r, g, b, a;
};

Color Add(Color a, Color b) {
  return {a.r + b.r, a.g + b.g, a.b + b.b, a.a + b.a};
}
Color Sub(Color a, Color b) {
  return {a.r - b.r, a.g - b.g, a.b - b.b, a.a - b.a};
}
Color Scale(Color a, int factor) {
  return {a.r * factor, a.g * factor, a.b * factor, a.a * factor};
}

Color Pixel(const Image& image, std::uint32_t x, std::uint32_t y) {
  const std::size_t offset = (static_cast<std::size_t>(y) * image.width + x) * 4;
  return {image.rgba[offset], image.rgba[offset + 1], image.rgba[offset + 2],
          image.rgba[offset + 3]};
}

void Include(Color color, Color* low, Color* high) {
  low->r = std::min(low->r, color.r);
  low->g = std::min(low->g, color.g);
  low->b = std::min(low->b, color.b);
  low->a = std::min(low->a, color.a);
  high->r = std::max(high->r, color.r);
  high->g = std::max(high->g, color.g);
  high->b = std::max(high->b, color.b);
  high->a = std::max(high->a, color.a);
}

std::uint16_t PackA(Color c, unsigned mode) {
  if (c.a >= 250) {
    return static_cast<std::uint16_t>(0x8000 | ((c.r >> 3) << 10) |
                                      ((c.g >> 3) << 5) |
                                      (((c.b >> 4) & 0x0f) << 1) | (mode & 1));
  }
  return static_cast<std::uint16_t>(((c.a * 7 / 255) << 12) |
                                    ((c.r >> 4) << 8) |
                                    ((c.g >> 4) << 4) |
                                    ((c.b >> 5) << 1) | (mode & 1));
}

std::uint16_t PackB(Color c) {
  if (c.a >= 250) {
    return static_cast<std::uint16_t>(0x8000 | ((c.r >> 3) << 10) |
                                      ((c.g >> 3) << 5) | (c.b >> 3));
  }
  return static_cast<std::uint16_t>(((c.a * 15 / 255) << 12) |
                                    ((c.r >> 4) << 8) |
                                    ((c.g >> 4) << 4) | (c.b >> 4));
}

// Expand endpoint words using the same bit replication convention as the
// PowerVR reference decoder. This keeps modulation fitting close to runtime.
Color UnpackA(std::uint16_t word) {
  const unsigned raw = word & 0xfffeu;
  if (raw & 0x8000u) {
    unsigned b = raw & 0x1fu;
    b |= b >> 4;
    return {static_cast<int>(((raw >> 10) & 31) * 255 / 31),
            static_cast<int>(((raw >> 5) & 31) * 255 / 31),
            static_cast<int>(b * 255 / 31), 255};
  }
  unsigned r = (raw >> 8) & 0xfu;
  unsigned g = (raw >> 4) & 0xfu;
  unsigned b = (raw >> 1) & 7u;
  unsigned a = (raw >> 12) & 7u;
  r = (r << 1) | (r >> 3);
  g = (g << 1) | (g >> 3);
  b = (b << 2) | (b >> 1);
  a = (a << 1);
  return {static_cast<int>(r * 255 / 31), static_cast<int>(g * 255 / 31),
          static_cast<int>(b * 255 / 31), static_cast<int>(a * 17)};
}

Color UnpackB(std::uint16_t raw) {
  if (raw & 0x8000u) {
    return {static_cast<int>(((raw >> 10) & 31) * 255 / 31),
            static_cast<int>(((raw >> 5) & 31) * 255 / 31),
            static_cast<int>((raw & 31) * 255 / 31), 255};
  }
  unsigned r = (raw >> 8) & 0xfu;
  unsigned g = (raw >> 4) & 0xfu;
  unsigned b = raw & 0xfu;
  unsigned a = (raw >> 12) & 0xfu;
  r = (r << 1) | (r >> 3);
  g = (g << 1) | (g >> 3);
  b = (b << 1) | (b >> 3);
  return {static_cast<int>(r * 255 / 31), static_cast<int>(g * 255 / 31),
          static_cast<int>(b * 255 / 31), static_cast<int>(a * 17)};
}

Color Interpolate(const Color& p, const Color& q, const Color& r,
                  const Color& s, int u, int v, int x_scale) {
  // The reference decoder uses fixed-point coordinates centred between blocks.
  const int denominator = x_scale * 4;
  Color result;
  result.r = (p.r * x_scale + u * (q.r - p.r)) * 4 +
             v * ((r.r * x_scale + u * (s.r - r.r)) -
                  (p.r * x_scale + u * (q.r - p.r)));
  result.g = (p.g * x_scale + u * (q.g - p.g)) * 4 +
             v * ((r.g * x_scale + u * (s.g - r.g)) -
                  (p.g * x_scale + u * (q.g - p.g)));
  result.b = (p.b * x_scale + u * (q.b - p.b)) * 4 +
             v * ((r.b * x_scale + u * (s.b - r.b)) -
                  (p.b * x_scale + u * (q.b - p.b)));
  result.a = (p.a * x_scale + u * (q.a - p.a)) * 4 +
             v * ((r.a * x_scale + u * (s.a - r.a)) -
                  (p.a * x_scale + u * (q.a - p.a)));
  return {result.r / denominator, result.g / denominator,
          result.b / denominator, result.a / denominator};
}

int Dot(Color a, Color b) {
  return a.r * b.r + a.g * b.g + a.b * b.b + 2 * a.a * b.a;
}

unsigned ChooseModulation(Color pixel, Color a, Color b, unsigned bpp) {
  static const int k4Weights[] = {0, 4, 4, 8};
  static const int k2Weights[] = {0, 3, 5, 8};
  const int* weights = bpp == 2 ? k2Weights : k4Weights;
  const Color difference = Sub(b, a);
  const Color target8 = Scale(pixel, 8);
  int best_error = std::numeric_limits<int>::max();
  unsigned best_index = 0;
  for (unsigned index = 0; index < 4; ++index) {
    const Color predicted = Add(Scale(a, 8), Scale(difference, weights[index]));
    const Color error = Sub(target8, predicted);
    const int score = Dot(error, error);
    if (score < best_error) {
      best_error = score;
      best_index = index;
    }
  }
  return best_index;
}

std::size_t BlockOffset(std::uint32_t width_blocks,
                        std::uint32_t height_blocks, std::uint32_t bx,
                        std::uint32_t by) {
  return static_cast<std::size_t>(MortonIndex(height_blocks, width_blocks, by,
                                               bx)) * 8;
}

Color InterpolatedEndpoint(const std::vector<std::uint16_t>& endpoint_a,
                           const std::vector<std::uint16_t>& endpoint_b,
                           std::uint32_t width_blocks,
                           std::uint32_t height_blocks, std::uint32_t bx,
                           std::uint32_t by, unsigned bpp, unsigned x,
                           unsigned y, bool use_b) {
  const unsigned block_width = bpp == 2 ? 8 : 4;
  const unsigned x_half = block_width / 2;
  const int u = static_cast<int>(x < x_half ? x + x_half : x - x_half);
  const int v = static_cast<int>(y < 2 ? y + 2 : y - 2);
  const std::uint32_t left = (bx + width_blocks - (x < x_half ? 1 : 0)) % width_blocks;
  const std::uint32_t top = (by + height_blocks - (y < 2 ? 1 : 0)) % height_blocks;
  const std::uint32_t right = (left + 1) % width_blocks;
  const std::uint32_t bottom = (top + 1) % height_blocks;
  const std::vector<std::uint16_t>& packed = use_b ? endpoint_b : endpoint_a;
  const auto get = [&](std::uint32_t xx, std::uint32_t yy) {
    const std::size_t index = static_cast<std::size_t>(yy) * width_blocks + xx;
    return use_b ? UnpackB(packed[index]) : UnpackA(packed[index]);
  };
  return Interpolate(get(left, top), get(right, top), get(left, bottom),
                     get(right, bottom), u, v, block_width);
}

void WriteWord(std::vector<std::uint8_t>* data, std::size_t offset,
               std::uint32_t value) {
  (*data)[offset + 0] = static_cast<std::uint8_t>(value);
  (*data)[offset + 1] = static_cast<std::uint8_t>(value >> 8);
  (*data)[offset + 2] = static_cast<std::uint8_t>(value >> 16);
  (*data)[offset + 3] = static_cast<std::uint8_t>(value >> 24);
}

bool IsPowerOfTwo(std::uint32_t value) {
  return value && !(value & (value - 1));
}

bool ReadBmp(const std::string& path, Image* image, std::string* error) {
  FILE* file = std::fopen(path.c_str(), "rb");
  if (!file) return Fail(error, "Cannot open BMP file.");
  std::uint8_t header[54];
  if (std::fread(header, 1, sizeof(header), file) != sizeof(header) ||
      header[0] != 'B' || header[1] != 'M') {
    std::fclose(file);
    return Fail(error, "Invalid or truncated BMP header.");
  }
  const std::uint32_t pixel_offset = ReadU32(header + 10);
  const std::uint32_t dib_size = ReadU32(header + 14);
  const std::int32_t width_signed = static_cast<std::int32_t>(ReadU32(header + 18));
  const std::int32_t height_signed = static_cast<std::int32_t>(ReadU32(header + 22));
  const std::uint16_t planes = static_cast<std::uint16_t>(header[26] | (header[27] << 8));
  const std::uint16_t bpp = static_cast<std::uint16_t>(header[28] | (header[29] << 8));
  const std::uint32_t compression = ReadU32(header + 30);
  if (dib_size < 40 || width_signed <= 0 || height_signed == 0 ||
      height_signed == std::numeric_limits<std::int32_t>::min() || planes != 1 ||
      (bpp != 24 && bpp != 32) || (compression != 0 && !(bpp == 32 && compression == 3))) {
    std::fclose(file);
    return Fail(error, "BMP must be uncompressed 24/32-bit (or 32-bit bitfields).");
  }
  const std::uint32_t width = static_cast<std::uint32_t>(width_signed);
  const std::uint32_t height = static_cast<std::uint32_t>(height_signed < 0 ? -height_signed : height_signed);
  std::size_t bytes;
  if (!CheckedImageSize(width, height, &bytes)) {
    std::fclose(file);
    return Fail(error, "BMP dimensions exceed the supported image limit.");
  }
  std::uint32_t red_mask = 0x00ff0000u, green_mask = 0x0000ff00u;
  std::uint32_t blue_mask = 0x000000ffu, alpha_mask = 0;
  if (compression == 3) {
    if (dib_size >= 52) {
      std::uint8_t masks[16] = {};
      const long mask_position = 14 + 40;
      std::fseek(file, mask_position, SEEK_SET);
      std::fread(masks, 1, dib_size >= 56 ? 16 : 12, file);
      red_mask = ReadU32(masks);
      green_mask = ReadU32(masks + 4);
      blue_mask = ReadU32(masks + 8);
      alpha_mask = dib_size >= 56 ? ReadU32(masks + 12) : 0;
    } else {
      std::uint8_t masks[12];
      if (std::fseek(file, 54, SEEK_SET) != 0 ||
          std::fread(masks, 1, sizeof(masks), file) != sizeof(masks)) {
        std::fclose(file);
        return Fail(error, "Truncated BMP color masks.");
      }
      red_mask = ReadU32(masks);
      green_mask = ReadU32(masks + 4);
      blue_mask = ReadU32(masks + 8);
    }
    if (red_mask != 0x00ff0000u || green_mask != 0x0000ff00u ||
        blue_mask != 0x000000ffu) {
      std::fclose(file);
      return Fail(error, "BMP bitfields must use standard BGRA channel masks.");
    }
  }
  const std::size_t row_bytes = ((static_cast<std::size_t>(width) * bpp + 31) / 32) * 4;
  if (row_bytes > kMaxImageBytes || std::fseek(file, static_cast<long>(pixel_offset), SEEK_SET) != 0) {
    std::fclose(file);
    return Fail(error, "Invalid BMP pixel-data offset.");
  }
  image->width = width;
  image->height = height;
  image->rgba.assign(bytes, 255);
  std::vector<std::uint8_t> row(row_bytes);
  for (std::uint32_t file_y = 0; file_y < height; ++file_y) {
    if (std::fread(row.data(), 1, row.size(), file) != row.size()) {
      std::fclose(file);
      return Fail(error, "Truncated BMP pixel data.");
    }
    const std::uint32_t y = height_signed < 0 ? file_y : height - 1 - file_y;
    for (std::uint32_t x = 0; x < width; ++x) {
      const std::size_t src = static_cast<std::size_t>(x) * (bpp / 8);
      const std::size_t dst = (static_cast<std::size_t>(y) * width + x) * 4;
      image->rgba[dst + 0] = row[src + 2];
      image->rgba[dst + 1] = row[src + 1];
      image->rgba[dst + 2] = row[src + 0];
      image->rgba[dst + 3] = bpp == 32 && alpha_mask ? row[src + 3] : 255;
    }
  }
  std::fclose(file);
  return true;
}

bool WriteBmp(const std::string& path, const Image& image, std::string* error) {
  std::size_t image_bytes;
  if (!CheckedImageSize(image.width, image.height, &image_bytes) || image.rgba.size() != image_bytes)
    return Fail(error, "Invalid image buffer for BMP output.");
  const std::uint64_t pixel_bytes = static_cast<std::uint64_t>(image.width) * image.height * 4;
  const std::uint64_t total_bytes = 14 + 108 + pixel_bytes;
  if (total_bytes > std::numeric_limits<std::uint32_t>::max())
    return Fail(error, "BMP output is too large.");
  FILE* file = std::fopen(path.c_str(), "wb");
  if (!file) return Fail(error, "Cannot create BMP file.");
  std::uint8_t header[122] = {};
  header[0] = 'B'; header[1] = 'M';
  auto put = [&](std::size_t offset, std::uint32_t value) {
    header[offset] = static_cast<std::uint8_t>(value);
    header[offset + 1] = static_cast<std::uint8_t>(value >> 8);
    header[offset + 2] = static_cast<std::uint8_t>(value >> 16);
    header[offset + 3] = static_cast<std::uint8_t>(value >> 24);
  };
  put(2, static_cast<std::uint32_t>(total_bytes));
  put(10, 122);
  put(14, 108);  // BITMAPV4HEADER
  put(18, image.width);
  put(22, image.height);
  header[26] = 1;
  header[28] = 32;
  put(30, 3);  // BI_BITFIELDS
  put(34, static_cast<std::uint32_t>(pixel_bytes));
  put(54, 0x00ff0000u);  // red
  put(58, 0x0000ff00u);  // green
  put(62, 0x000000ffu);  // blue
  put(66, 0xff000000u);  // alpha
  put(70, 0x73524742u);  // sRGB
  bool ok = std::fwrite(header, 1, sizeof(header), file) == sizeof(header);
  std::vector<std::uint8_t> row(static_cast<std::size_t>(image.width) * 4);
  for (std::uint32_t y = image.height; ok && y > 0; --y) {
    const std::uint32_t source_y = y - 1;
    for (std::uint32_t x = 0; x < image.width; ++x) {
      const std::size_t src = (static_cast<std::size_t>(source_y) * image.width + x) * 4;
      const std::size_t dst = static_cast<std::size_t>(x) * 4;
      row[dst] = image.rgba[src + 2];
      row[dst + 1] = image.rgba[src + 1];
      row[dst + 2] = image.rgba[src];
      row[dst + 3] = image.rgba[src + 3];
    }
    ok = std::fwrite(row.data(), 1, row.size(), file) == row.size();
  }
  ok = std::fclose(file) == 0 && ok;
  return ok ? true : Fail(error, "Failed while writing BMP data.");
}

}  // namespace

bool Encode(const Image& source, unsigned bpp, Texture* texture,
            std::string* error) {
  if (!texture || (bpp != 2 && bpp != 4)) return Fail(error, "Choose PVRTC1 2bpp or 4bpp.");
  std::size_t source_bytes;
  if (!CheckedImageSize(source.width, source.height, &source_bytes) || source.rgba.size() != source_bytes)
    return Fail(error, "Invalid source image or unsupported dimensions.");
  const unsigned block_width = bpp == 2 ? 8 : 4;
  const std::uint32_t min_width = bpp == 2 ? 16 : 8;
  const std::uint32_t width = std::max(min_width, NextPowerOfTwo(source.width));
  const std::uint32_t height = std::max(8u, NextPowerOfTwo(source.height));
  if (width > kMaxDimension || height > kMaxDimension)
    return Fail(error, "Image dimensions exceed the supported PVRTC size.");
  const std::uint32_t width_blocks = width / block_width;
  const std::uint32_t height_blocks = height / 4;
  const std::size_t block_count = static_cast<std::size_t>(width_blocks) * height_blocks;
  if (block_count > kMaxImageBytes / 8) return Fail(error, "PVRTC output is too large.");

  Image padded;
  padded.width = width;
  padded.height = height;
  padded.rgba.resize(static_cast<std::size_t>(width) * height * 4);
  for (std::uint32_t y = 0; y < height; ++y) {
    const std::uint32_t sy = std::min(y, source.height - 1);
    for (std::uint32_t x = 0; x < width; ++x) {
      const std::uint32_t sx = std::min(x, source.width - 1);
      const Color c = Pixel(source, sx, sy);
      const std::size_t p = (static_cast<std::size_t>(y) * width + x) * 4;
      padded.rgba[p] = static_cast<std::uint8_t>(c.r);
      padded.rgba[p + 1] = static_cast<std::uint8_t>(c.g);
      padded.rgba[p + 2] = static_cast<std::uint8_t>(c.b);
      padded.rgba[p + 3] = static_cast<std::uint8_t>(c.a);
    }
  }

  std::vector<std::uint16_t> endpoints_a(block_count);
  std::vector<std::uint16_t> endpoints_b(block_count);
  for (std::uint32_t by = 0; by < height_blocks; ++by) {
    for (std::uint32_t bx = 0; bx < width_blocks; ++bx) {
      const std::size_t linear = static_cast<std::size_t>(by) * width_blocks + bx;
      Color low = {255, 255, 255, 255};
      Color high = {0, 0, 0, 0};
      for (unsigned y = 0; y < 4; ++y) {
        for (unsigned x = 0; x < block_width; ++x) {
          Include(Pixel(padded, bx * block_width + x, by * 4 + y), &low, &high);
        }
      }
      endpoints_a[linear] = PackA(low, bpp == 2 ? 1 : 0);
      endpoints_b[linear] = PackB(high);
    }
  }

  texture->width = width;
  texture->height = height;
  texture->source_width = source.width;
  texture->source_height = source.height;
  texture->bits_per_pixel = bpp;
  texture->has_alpha = true;
  texture->blocks.assign(block_count * 8, 0);

  for (std::uint32_t by = 0; by < height_blocks; ++by) {
    for (std::uint32_t bx = 0; bx < width_blocks; ++bx) {
      std::uint32_t modulation = 0;
      unsigned bit = 0;
      for (unsigned y = 0; y < 4; ++y) {
        for (unsigned x = 0; x < block_width; ++x) {
          if (bpp == 2 && ((x ^ y) & 1u)) continue;
          const Color pixel = Pixel(padded, bx * block_width + x, by * 4 + y);
          const Color a = InterpolatedEndpoint(endpoints_a, endpoints_b,
              width_blocks, height_blocks, bx, by, bpp, x, y, false);
          const Color b = InterpolatedEndpoint(endpoints_a, endpoints_b,
              width_blocks, height_blocks, bx, by, bpp, x, y, true);
          const unsigned index = ChooseModulation(pixel, a, b, bpp);
          modulation |= index << bit;
          bit += 2;
        }
      }
      const std::size_t offset = BlockOffset(width_blocks, height_blocks, bx, by);
      WriteWord(&texture->blocks, offset, modulation);
      const std::uint32_t colors = endpoints_a[static_cast<std::size_t>(by) * width_blocks + bx] |
          (static_cast<std::uint32_t>(endpoints_b[static_cast<std::size_t>(by) * width_blocks + bx]) << 16);
      WriteWord(&texture->blocks, offset + 4, colors);
    }
  }
  return true;
}

bool Decode(const Texture& texture, Image* image, std::string* error) {
  if (!image || (texture.bits_per_pixel != 2 && texture.bits_per_pixel != 4) ||
      !IsPowerOfTwo(texture.width) || !IsPowerOfTwo(texture.height))
    return Fail(error, "Invalid PVRTC dimensions or bit rate.");
  const unsigned block_width = texture.bits_per_pixel == 2 ? 8 : 4;
  if (texture.width < (texture.bits_per_pixel == 2 ? 16u : 8u) || texture.height < 8 ||
      texture.width % block_width || texture.height % 4)
    return Fail(error, "PVRTC texture is smaller than the format minimum.");
  const std::size_t block_count = static_cast<std::size_t>(texture.width / block_width) * (texture.height / 4);
  if (texture.blocks.size() != block_count * 8)
    return Fail(error, "PVRTC payload size does not match its dimensions.");
  std::size_t bytes;
  if (!CheckedImageSize(texture.width, texture.height, &bytes))
    return Fail(error, "PVRTC dimensions exceed the supported image limit.");
  std::vector<std::uint8_t> decoded(bytes);
  PVRTDecompressPVRTC(texture.blocks.data(), texture.bits_per_pixel == 2 ? 1 : 0,
                      texture.width, texture.height, decoded.data());
  const std::uint32_t output_width = texture.source_width ? texture.source_width : texture.width;
  const std::uint32_t output_height = texture.source_height ? texture.source_height : texture.height;
  if (output_width > texture.width || output_height > texture.height || !output_width || !output_height)
    return Fail(error, "Invalid original image size in PVR metadata.");
  std::size_t output_bytes;
  if (!CheckedImageSize(output_width, output_height, &output_bytes))
    return Fail(error, "Decoded image dimensions exceed the supported limit.");
  image->width = output_width;
  image->height = output_height;
  image->rgba.resize(output_bytes);
  for (std::uint32_t y = 0; y < output_height; ++y) {
    std::memcpy(image->rgba.data() + static_cast<std::size_t>(y) * output_width * 4,
                decoded.data() + static_cast<std::size_t>(y) * texture.width * 4,
                static_cast<std::size_t>(output_width) * 4);
  }
  return true;
}

bool ReadPvr(const std::string& path, Texture* texture, std::string* error) {
  if (!texture) return Fail(error, "Missing PVR output object.");
  FILE* file = std::fopen(path.c_str(), "rb");
  if (!file) return Fail(error, "Cannot open PVR file.");
  if (std::fseek(file, 0, SEEK_END) != 0) { std::fclose(file); return Fail(error, "Cannot seek PVR file."); }
  const long file_size = std::ftell(file);
  if (file_size < 52 || file_size > static_cast<long>(kMaxImageBytes)) { std::fclose(file); return Fail(error, "Invalid PVR file size."); }
  std::rewind(file);
  std::vector<std::uint8_t> data(static_cast<std::size_t>(file_size));
  if (std::fread(data.data(), 1, data.size(), file) != data.size()) { std::fclose(file); return Fail(error, "Cannot read PVR file."); }
  std::fclose(file);
  if (ReadU32(data.data()) != kPvr3Magic) {
    // Legacy PVR v2 stores a 52-byte header and the compression type in the
    // low byte of flags (12/24 = PVRTC1 2bpp, 13/25 = PVRTC1 4bpp).
    const std::uint32_t header_size = ReadU32(data.data());
    const std::uint32_t legacy_height = ReadU32(data.data() + 4);
    const std::uint32_t legacy_width = ReadU32(data.data() + 8);
    const std::uint32_t flags = ReadU32(data.data() + 16);
    const std::uint32_t declared_data_size = ReadU32(data.data() + 20);
    const std::uint32_t tag = ReadU32(data.data() + 44);
    const unsigned pixel_type = flags & 0xffu;
    const unsigned legacy_bpp =
        (pixel_type == 12 || pixel_type == 24) ? 2
        : (pixel_type == 13 || pixel_type == 25) ? 4
                                                  : 0;
    const unsigned legacy_block_width = legacy_bpp == 2 ? 8 : 4;
    if (header_size < 52 || header_size > data.size() || tag != 0x21525650u ||
        !legacy_bpp || !IsPowerOfTwo(legacy_width) || !IsPowerOfTwo(legacy_height) ||
        legacy_width < (legacy_bpp == 2 ? 16u : 8u) || legacy_height < 8 ||
        legacy_width % legacy_block_width || legacy_height % 4) {
      return Fail(error, "File is neither supported PVR v3 nor PVRTC1 PVR v2.");
    }
    const std::size_t base_level_size = static_cast<std::size_t>(legacy_width / legacy_block_width) *
        (legacy_height / 4) * 8;
    if (declared_data_size < base_level_size ||
        declared_data_size > data.size() - header_size ||
        base_level_size > data.size() - header_size) {
      return Fail(error, "Legacy PVR payload length does not match its dimensions.");
    }
    texture->width = legacy_width;
    texture->height = legacy_height;
    texture->source_width = legacy_width;
    texture->source_height = legacy_height;
    texture->bits_per_pixel = legacy_bpp;
    texture->has_alpha = true;
    texture->blocks.assign(data.begin() + header_size,
                           data.begin() + header_size + base_level_size);
    return true;
  }
  const std::uint64_t format = static_cast<std::uint64_t>(ReadU32(data.data() + 8)) |
      (static_cast<std::uint64_t>(ReadU32(data.data() + 12)) << 32);
  const std::uint32_t width = ReadU32(data.data() + 28);
  const std::uint32_t height = ReadU32(data.data() + 24);
  const std::uint32_t meta_size = ReadU32(data.data() + 48);
  const unsigned type = static_cast<unsigned>(format & 0xffffffffu);
  unsigned bpp = (type == 0 || type == 1) ? 2 : (type == 2 || type == 3) ? 4 : 0;
  if (!bpp || format >> 32 != 0 || meta_size > data.size() - 52)
    return Fail(error, "PVR v3 format is not PVRTC1 2bpp/4bpp.");
  const unsigned block_width = bpp == 2 ? 8 : 4;
  if (!IsPowerOfTwo(width) || !IsPowerOfTwo(height) || width < (bpp == 2 ? 16u : 8u) || height < 8 ||
      width % block_width || height % 4)
    return Fail(error, "PVR dimensions are not valid PVRTC1 dimensions.");
  const std::size_t payload_size = static_cast<std::size_t>(width / block_width) * (height / 4) * 8;
  if (payload_size != data.size() - 52 - meta_size)
    return Fail(error, "PVR payload length does not match the image dimensions.");
  texture->width = width;
  texture->height = height;
  texture->source_width = width;
  texture->source_height = height;
  texture->bits_per_pixel = bpp;
  texture->has_alpha = type == 1 || type == 3;
  texture->blocks.assign(data.begin() + 52 + meta_size, data.end());
  std::size_t position = 52;
  const std::size_t metadata_end = 52 + meta_size;
  while (position + 12 <= metadata_end) {
    const std::uint32_t vendor = ReadU32(data.data() + position);
    const std::uint32_t key = ReadU32(data.data() + position + 4);
    const std::uint32_t length = ReadU32(data.data() + position + 8);
    position += 12;
    if (length > metadata_end - position) return Fail(error, "Invalid PVR metadata length.");
    if (vendor == kRenuxFourCc && key == kOriginalSizeKey && length == 8) {
      texture->source_width = ReadU32(data.data() + position);
      texture->source_height = ReadU32(data.data() + position + 4);
    }
    position += length;
  }
  if (position != metadata_end) return Fail(error, "Malformed PVR metadata section.");
  return true;
}

bool WritePvr(const std::string& path, const Texture& texture, std::string* error) {
  if ((texture.bits_per_pixel != 2 && texture.bits_per_pixel != 4) ||
      !IsPowerOfTwo(texture.width) || !IsPowerOfTwo(texture.height))
    return Fail(error, "Invalid PVRTC texture for PVR output.");
  const unsigned block_width = texture.bits_per_pixel == 2 ? 8 : 4;
  const std::size_t expected = static_cast<std::size_t>(texture.width / block_width) * (texture.height / 4) * 8;
  if (texture.blocks.size() != expected || !texture.source_width || !texture.source_height ||
      texture.source_width > texture.width || texture.source_height > texture.height)
    return Fail(error, "PVRTC payload or source dimensions are invalid.");
  std::vector<std::uint8_t> data;
  data.reserve(52 + 20 + texture.blocks.size());
  AppendU32(&data, kPvr3Magic);
  AppendU32(&data, 0);  // flags
  AppendU32(&data, texture.bits_per_pixel == 2 ? (texture.has_alpha ? 1 : 0) : (texture.has_alpha ? 3 : 2));
  AppendU32(&data, 0);  // high word of pixel format
  AppendU32(&data, 0);  // linear RGB
  AppendU32(&data, 0);  // unsigned normalized byte channels
  AppendU32(&data, texture.height);
  AppendU32(&data, texture.width);
  AppendU32(&data, 1);  // depth
  AppendU32(&data, 1);  // surfaces
  AppendU32(&data, 1);  // faces
  AppendU32(&data, 1);  // mip levels
  AppendU32(&data, 20); // one custom metadata record
  AppendU32(&data, kRenuxFourCc);
  AppendU32(&data, kOriginalSizeKey);
  AppendU32(&data, 8);
  AppendU32(&data, texture.source_width);
  AppendU32(&data, texture.source_height);
  data.insert(data.end(), texture.blocks.begin(), texture.blocks.end());
  FILE* file = std::fopen(path.c_str(), "wb");
  if (!file) return Fail(error, "Cannot create PVR file.");
  const bool wrote_all = std::fwrite(data.data(), 1, data.size(), file) == data.size();
  const bool closed = std::fclose(file) == 0;
  return wrote_all && closed ? true : Fail(error, "Failed while writing PVR data.");
}

bool ReadImage(const std::string& path, Image* image, std::string* error) {
  if (!image) return Fail(error, "Missing image output object.");
  const std::size_t dot = path.find_last_of('.');
  std::string ext = dot == std::string::npos ? "" : path.substr(dot + 1);
  std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
  if (ext == "bmp") return ReadBmp(path, image, error);
  png_image png = {};
  png.version = PNG_IMAGE_VERSION;
  if (!png_image_begin_read_from_file(&png, path.c_str())) return Fail(error, "Cannot read PNG file.");
  png.format = PNG_FORMAT_RGBA;
  std::size_t bytes;
  if (!CheckedImageSize(png.width, png.height, &bytes)) { png_image_free(&png); return Fail(error, "PNG dimensions exceed the supported image limit."); }
  image->width = png.width;
  image->height = png.height;
  image->rgba.resize(bytes);
  if (!png_image_finish_read(&png, nullptr, image->rgba.data(), 0, nullptr)) {
    png_image_free(&png);
    return Fail(error, "PNG decode failed.");
  }
  png_image_free(&png);
  return true;
}

bool WriteImage(const std::string& path, const Image& image, std::string* error) {
  const std::size_t dot = path.find_last_of('.');
  std::string ext = dot == std::string::npos ? "" : path.substr(dot + 1);
  std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
  if (ext == "bmp") return WriteBmp(path, image, error);
  std::size_t bytes;
  if (!CheckedImageSize(image.width, image.height, &bytes) || image.rgba.size() != bytes)
    return Fail(error, "Invalid image buffer for PNG output.");
  png_image png = {};
  png.version = PNG_IMAGE_VERSION;
  png.width = image.width;
  png.height = image.height;
  png.format = PNG_FORMAT_RGBA;
  if (!png_image_write_to_file(&png, path.c_str(), 0, image.rgba.data(), 0, nullptr)) {
    png_image_free(&png);
    return Fail(error, "PNG encode failed.");
  }
  png_image_free(&png);
  return true;
}

}  // namespace pvrtc_converter
