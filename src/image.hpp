#pragma once

#include <cstddef>
#include <utility>
#include <stdexcept>
#include <memory>
#include <new>
#include <cstdlib>
#include <cmath>
#include <algorithm>

namespace image_processor {

inline unsigned char d2uc(double d) {
  return static_cast<unsigned char>(std::round(std::clamp(d, 0.0, 255.0)));
}

struct RGBColour {
  unsigned char r;
  unsigned char g;
  unsigned char b;
}; // RGBColour

struct Colour {
  unsigned char y;
  unsigned char cb;
  unsigned char cr;

  RGBColour toRgb() const {
    const double tcb = static_cast<double>(cb) - 128.0;
    const double tcr = static_cast<double>(cr) - 128.0;
    return RGBColour{
      d2uc(y + 1.402 * tcr),
      d2uc(y - 0.344136 * tcb - 0.714136 * tcr),
      d2uc(y + 1.772 * tcb)
    };
  }
}; // Colour

/*
  This holds the image data in a pixel buffer similar to the one used by 
  stb_image. The values for each pixel are stored in the YCbCr format and 
  can be accessed as a colour with colourAt or individually with yAt, cbAt, 
  and crAt.
*/
class Image {
  public: 
    using PixelBuffer = std::unique_ptr<unsigned char, void (*)(void*)>;

    Image(std::size_t w, std::size_t h, PixelBuffer image_data) 
      : width_(w), height_(h), image_data_(std::move(image_data)) {}

    Image(std::size_t w, std::size_t h) : Image(w, h, PixelBuffer{
      static_cast<unsigned char*>(std::malloc(w * h * 3)), 
      std::free 
    }) {
      if (w == 0 || h == 0)
        throw std::out_of_range("Image dimensions must be non-zero");
      if (!image_data_) {
        throw std::bad_alloc{};
      }
    }

    std::size_t width() const {
      return width_;
    }

    std::size_t height() const {
      return height_;
    }

    std::size_t size() const {
      return width_ * height_;
    }

    unsigned char& yAt(std::size_t x, std::size_t y) {
#ifdef DEBUG_MODE
      if (x >= width_ || y >= height_) 
        throw std::out_of_range("Coordinates not in range");
#endif
      return image_data_.get()[3 * (y * width_ + x)];
    }

    const unsigned char& yAt(std::size_t x, std::size_t y) const {
#ifdef DEBUG_MODE
      if (x >= width_ || y >= height_) 
        throw std::out_of_range("Coordinates not in range");
#endif
      return image_data_.get()[3 * (y * width_ + x)];
    }

    unsigned char& cbAt(std::size_t x, std::size_t y) {
#ifdef DEBUG_MODE
      if (x >= width_ || y >= height_) 
        throw std::out_of_range("Coordinates not in range");
#endif
      return image_data_.get()[3 * (y * width_ + x) + 1];
    }

    const unsigned char& cbAt(std::size_t x, std::size_t y) const {
#ifdef DEBUG_MODE
      if (x >= width_ || y >= height_) 
        throw std::out_of_range("Coordinates not in range");
#endif
      return image_data_.get()[3 * (y * width_ + x) + 1];
    }

    unsigned char& crAt(std::size_t x, std::size_t y) {
#ifdef DEBUG_MODE
      if (x >= width_ || y >= height_) 
        throw std::out_of_range("Coordinates not in range");
#endif
      return image_data_.get()[3 * (y * width_ + x) + 2];
    }

    const unsigned char& crAt(std::size_t x, std::size_t y) const {
#ifdef DEBUG_MODE
      if (x >= width_ || y >= height_) 
        throw std::out_of_range("Coordinates not in range");
#endif
      return image_data_.get()[3 * (y * width_ + x) + 2];
    }

    Colour colourAt(std::size_t loc) const {
#ifdef DEBUG_MODE
      if (loc >= width_ * height_) 
        throw std::out_of_range("Location not in range");
#endif
      return Colour{
        image_data_.get()[3 * loc],
        image_data_.get()[3 * loc + 1],
        image_data_.get()[3 * loc + 2]
      };
    }

    void setColour(const Colour& colour, std::size_t loc) {
#ifdef DEBUG_MODE
      if (loc >= width_ * height_) 
        throw std::out_of_range("Location not in range");
#endif
      image_data_.get()[3 * loc] = colour.y;
      image_data_.get()[3 * loc + 1] = colour.cb;
      image_data_.get()[3 * loc + 2] = colour.cr;
    } // setColour

    // Returns the colour in the region which is closest to the average.
    Colour averageColour(std::size_t xl, std::size_t xu, 
                         std::size_t yl, std::size_t yu) const {
#ifdef DEBUG_MODE
      if (xl >= xu || xu > width_ || yl >= yu || yu > height_) 
        throw std::out_of_range("Indices not in range");
#endif
      double ySum{0.0}, cbSum{0.0}, crSum{0.0};
      for (std::size_t y{yl}; y < yu; ++y) {
        for (std::size_t x{xl}; x < xu; ++x) {
          ySum  += yAt(x, y);
          cbSum += cbAt(x, y);
          crSum += crAt(x, y);
        }
      }
      double rangeSize{static_cast<double>((xu - xl) * (yu - yl))};
      Colour avgColour{
        d2uc(ySum / rangeSize),
        d2uc(cbSum / rangeSize),
        d2uc(crSum / rangeSize)
      };
      int minDist{195075}; // 3 * 255 * 255
      unsigned char ay{avgColour.y}, acb{avgColour.cb}, acr{avgColour.cr};
      for (std::size_t y{yl}; y < yu; ++y) {
        for (std::size_t x{xl}; x < xu; ++x) {
          int dy{(int) avgColour.y - yAt(x, y)};
          int dcb{(int) avgColour.cb - cbAt(x, y)};
          int dcr{(int) avgColour.cr - crAt(x, y)};
          int d = dy * dy + dcb * dcb + dcr * dcr;  
          if (d < minDist) {
            minDist = d;
            ay = yAt(x, y);
            acb = cbAt(x, y);
            acr = crAt(x, y);
          }
        }
      }
      return Colour{ay, acb, acr};
    }

  private:
    std::size_t width_;
    std::size_t height_;
    PixelBuffer image_data_;
}; // image class

} 
