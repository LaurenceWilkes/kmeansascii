#define STB_IMAGE_IMPLEMENTATION
#define STB_ALWAYS_YCBCR
#include "stb_image.h"

#include "image_io.hpp"
#include "image.hpp"

#include <stdexcept>
#include <cstddef>
#include <memory>
#include <vector>
#include <format>

namespace image_processor {

Image loadImage(const char* fileName) {
  int width, height, channels;

  Image::PixelBuffer raw{
    stbi_load(fileName, &width, &height, &channels, 3),
    stbi_image_free
  };

  if (!raw) {
      throw std::runtime_error(std::format("Failed to load image: {}",
                                           stbi_failure_reason()));
  }

  return Image(
    static_cast<std::size_t>(width),
    static_cast<std::size_t>(height),
    std::move(raw)
  );

} // loadImage

} // image_processor namespace
