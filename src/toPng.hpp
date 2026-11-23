#pragma once

#include <filesystem>

#include "image.hpp"

namespace image_processor {

void writePng(const Image& image, 
              const std::filesystem::path& outputFile);

}
