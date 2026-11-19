#pragma once

#include "image.hpp"

namespace image_processor {

/*
  The native windows display libraries are used to display the image.
  The operation is suspended till the image is closed.

  This code is LLM generated.
*/
void display(const Image& image);

}
