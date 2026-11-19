#pragma once

#include "image.hpp"

namespace image_processor {

/* 
  This uses the one file stb_image header library to load 
  the image file to an Image object. 

  It should be noted that stb_image has been altered so as 
  to return the original YCbCr colour values of the pixels.
*/
Image loadImage(const char* fileName);

}
