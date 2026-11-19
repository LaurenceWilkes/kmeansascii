#pragma once

#include "image.hpp"

namespace image_processor {

/* 
  This partitions the image into blocks of pixel height given by 
  gHeight and width provided to be close to the ratio 1229:2320.
  A new image is returned with each pixel representing a block in
  the original image. Its colour will be the average value over 
  the block.

  The ratio is the one used by the Courier New font and so each 
  image is prepared for the block being replaced by a character.
*/
Image reduce(const Image& fullImage, int gHeight);

}
