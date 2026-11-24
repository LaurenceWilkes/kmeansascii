#include <cstddef>
#include <stdexcept>
#include <utility>
#include <array>
#include <cmath>

#include "image.hpp"
#include "reduce.hpp"

namespace image_processor {

namespace {
  constexpr int widthForHeight(int height) {
    constexpr int fh{2320}, fw{1229};
    return (height * fw + fh / 2) / fh;
  }
}

Image reduce(const Image& fullImage, int gHeight) {
  int gWidth;
  if (gHeight <= 6)
    throw std::out_of_range("Glyph height too small for accurate image");

  gWidth = widthForHeight(gHeight);

  std::size_t imageWidth{fullImage.width()};
  std::size_t imageHeight{fullImage.height()};

  // Calculated with floor -- The resulting image will lose the right and bottom edge...
  std::size_t reducedWidth{imageWidth / gWidth};
  std::size_t reducedHeight{imageHeight / gHeight};

  if (reducedWidth == 0 || reducedHeight == 0)
    throw std::out_of_range("The glyph is larger than the image");

  Image reducedImage(reducedWidth, reducedHeight);

  for (std::size_t i{0}; i < reducedWidth; ++i) {
    for (std::size_t j{0}; j < reducedHeight; ++j) {
      Colour ac{fullImage.averageColour(
          gWidth * i, gWidth * (i + 1),
          gHeight * j, gHeight * (j + 1)
      )};
      reducedImage.yAt(i, j)  = ac.y;
      reducedImage.cbAt(i, j) = ac.cb;
      reducedImage.crAt(i, j) = ac.cr;
    }
  }

  return reducedImage;

}

}
