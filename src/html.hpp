#pragma once

#include <filesystem>

#include "image.hpp"

namespace image_processor {

void writeHtml(const Image& image, const std::filesystem::path& outputFile);

}
