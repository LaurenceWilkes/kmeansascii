#include <cstddef>
#include <stdexcept>
#include <utility>
#include <array>
#include <cmath>

#include "image.hpp"
#include "reduce.hpp"

namespace image_processor {

namespace {
  constexpr std::array<int, 18> hs {
      7, 9, 10, 11, 13, 15, 17, 19, 30,
      32, 34, 36, 38, 45, 47, 51, 68, 83,
  };

  constexpr int widthForHeight(int height) {
    constexpr int fh{2320}, fw{1229};
    return (height * fw + fh / 2) / fh;
  }
}

Image reduce(const Image& fullImage, int gHeight) {
  int gWidth;
  if (std::find(hs.begin(), hs.end(), gHeight) != hs.end()) {
    gWidth = widthForHeight(gHeight);
  } else {
    throw std::out_of_range("Unsupported glyph height value");
  }

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
