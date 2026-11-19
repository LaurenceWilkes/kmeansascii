#pragma once

#include <filesystem>

#include "image.hpp"

namespace image_processor {

/*
  Each pixel in the image is replaced with an ASCII character 
  with those with a larger Y value having a glyph which uses 
  more pixels. The background is either black or calculated 
  using the average over the image. The result is written to 
  a html file for display. 

  The image is intended to have been processed by the K-means 
  algorithm prior to this as otherwise the resulting html will 
  be vast.
*/
void writeHtml(const Image& image, 
               const std::filesystem::path& outputFile, 
               bool blackBackground);

}
