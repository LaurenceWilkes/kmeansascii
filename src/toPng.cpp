#include <filesystem>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "toPng.hpp"

namespace image_processor {

void writePng(const Image& image,
              const std::filesystem::path& outputFile) {
  std::vector<unsigned char> rgb(image.size() * 3);
  for (std::size_t i = 0; i < image.size(); ++i) {
    const RGBColour colour = image.colourAt(i).toRgb();
    rgb[3 * i] = colour.r;
    rgb[3 * i + 1] = colour.g;
    rgb[3 * i + 2] = colour.b;
  }

  const std::string filename = outputFile.string();
  if (!stbi_write_png(filename.c_str(), static_cast<int>(image.width()),
                      static_cast<int>(image.height()), 3, rgb.data(), 0))
    throw std::runtime_error("Could not write file");
}

}
