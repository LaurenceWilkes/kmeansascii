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

struct Colour {
  unsigned char y;
  unsigned char cb;
  unsigned char cr;
}; // Colour

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

    unsigned char& yAt(std::size_t x, std::size_t y) {
#ifdef DEBUG_MODE
      if (x >= width_ || y >= height_) throw std::out_of_range("Coordinates not in range");
#endif
      return image_data_.get()[3 * (y * width_ + x)];
    }

    const unsigned char& yAt(std::size_t x, std::size_t y) const {
#ifdef DEBUG_MODE
      if (x >= width_ || y >= height_) throw std::out_of_range("Coordinates not in range");
#endif
      return image_data_.get()[3 * (y * width_ + x)];
    }

    unsigned char& cbAt(std::size_t x, std::size_t y) {
#ifdef DEBUG_MODE
      if (x >= width_ || y >= height_) throw std::out_of_range("Coordinates not in range");
#endif
      return image_data_.get()[3 * (y * width_ + x) + 1];
    }

    const unsigned char& cbAt(std::size_t x, std::size_t y) const {
#ifdef DEBUG_MODE
      if (x >= width_ || y >= height_) throw std::out_of_range("Coordinates not in range");
#endif
      return image_data_.get()[3 * (y * width_ + x) + 1];
    }

    unsigned char& crAt(std::size_t x, std::size_t y) {
#ifdef DEBUG_MODE
      if (x >= width_ || y >= height_) throw std::out_of_range("Coordinates not in range");
#endif
      return image_data_.get()[3 * (y * width_ + x) + 2];
    }

    const unsigned char& crAt(std::size_t x, std::size_t y) const {
#ifdef DEBUG_MODE
      if (x >= width_ || y >= height_) throw std::out_of_range("Coordinates not in range");
#endif
      return image_data_.get()[3 * (y * width_ + x) + 2];
    }

    Colour colourAt(std::size_t loc) const {
#ifdef DEBUG_MODE
      if (loc >= width_ * height_) throw std::out_of_range("Location not in range");
#endif
      return Colour{
        image_data_.get()[3 * loc],
        image_data_.get()[3 * loc + 1],
        image_data_.get()[3 * loc + 2]
      };
    }

    void setColour(const Colour& colour, std::size_t loc) {
#ifdef DEBUG_MODE
      if (loc >= width_ * height_) throw std::out_of_range("Location not in range");
#endif
      image_data_.get()[3 * loc] = colour.y;
      image_data_.get()[3 * loc + 1] = colour.cb;
      image_data_.get()[3 * loc + 2] = colour.cr;
    } // setColour

  private:
    std::size_t width_;
    std::size_t height_;
    PixelBuffer image_data_;
}; // image class

} 
