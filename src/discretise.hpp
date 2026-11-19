#pragma once

#include "image.hpp"

namespace image_processor {

/*
   The K-means algorithm is performed on the colours in the 
   image to reduce the number of unique colours used to a set 
   of size at most k.
*/
void kmeans(Image& image, std::size_t k);

}

