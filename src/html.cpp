#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "html.hpp"

namespace image_processor {

namespace {
  constexpr std::string_view kChars{"@%#*+=-:. "};

  std::uint32_t colourKey(const Colour& colour) {
    return (static_cast<std::uint32_t>(colour.y) << 16) |
           (static_cast<std::uint32_t>(colour.cb) << 8) |
           static_cast<std::uint32_t>(colour.cr);
  }

  char characterFor(const Colour& colour) {
    const std::size_t index = (kChars.size() - 1) * static_cast<std::size_t>(colour.y) / 255;
    return kChars[index];
  }

  void writeColourCss(std::ofstream& output, std::size_t index, const Colour& colour) {
    Colour rgbc = colour.toRgb();
    output << ".c" << index << "{color:rgb("
           << static_cast<int>(rgbc.y) << ','
           << static_cast<int>(rgbc.cb) << ','
           << static_cast<int>(rgbc.cr) << ");}\n";
  }
} // namespace

void writeHtml(const Image& image, const std::filesystem::path& outputFile) {
  const std::size_t imageSize = image.size();
  std::unordered_map<std::uint32_t, std::size_t> colourClasses;
  std::vector<Colour> colours;

  for (std::size_t i = 0; i < imageSize; ++i) {
    const Colour colour = image.colourAt(i);
    const auto [entry, inserted] =
        colourClasses.try_emplace(colourKey(colour), colours.size());
    if (inserted) {
      colours.push_back(colour);
    }
  }

  std::ofstream output{outputFile};
  if (!output) {
    throw std::runtime_error("Could not open HTML output file");
  }

  Colour background{image.averageColour(0, image.width(), 0, image.height()).toRgb()};
  constexpr double characterWidthInEm = 0.6;
  const double fontSizeFromWidth =
      100.0 / (characterWidthInEm * static_cast<double>(image.width()));
  const double fontSizeFromHeight = 100.0 / static_cast<double>(image.height());

  output << "<!doctype html>\n"
         << "<html><head><meta charset=\"utf-8\">\n"
         << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
         << "<style>\n"
         << "* { box-sizing: border-box; }\n"
         << "html, body { width: 100%; height: 100%; margin: 0; overflow: hidden; background: rgb("
         << static_cast<int>(background.y) << ", "
         << static_cast<int>(background.cb) << ", "
         << static_cast<int>(background.cr) << "); }\n"
         << "body { display: grid; place-items: center; }\n"
         << "pre{\n"
         << "  margin: 0;\n"
         << "  color: #fff;\n"
         << "  background: transparent;\n"
         << "  font-family: \"Courier New\", Courier, monospace;\n"
         << "  font-size: min(" << fontSizeFromWidth << "vw, "
         << fontSizeFromHeight << "vh);\n"
         << "  font-weight: 700;\n"
         << "  line-height: 1;\n"
         << "  letter-spacing: 0;\n"
         << "  white-space: pre;\n"
         << "  text-align: left;\n"
         << "  text-rendering: geometricPrecision;\n"
         << "}\n";

  for (std::size_t i = 0; i < colours.size(); ++i) {
    writeColourCss(output, i, colours[i]);
  }

  output << "</style>\n</head><body><pre>";

  for (std::size_t y = 0; y < image.height(); ++y) {
    std::size_t activeClass = colours.size();

    for (std::size_t x = 0; x < image.width(); ++x) {
      const Colour colour = image.colourAt(y * image.width() + x);
      const std::size_t colourClass = colourClasses.at(colourKey(colour));

      if (colourClass != activeClass) {
        if (activeClass != colours.size()) {
          output << "</span>";
        }
        output << "<span class=\"c" << colourClass << "\">";
        activeClass = colourClass;
      }

      output.put(characterFor(colour));
    }

    if (activeClass != colours.size()) {
      output << "</span>";
    }
    if (y + 1 < image.height()) {
      output.put('\n');
    }
  }

  output << "</pre></body></html>\n";
}

} // namespace image_processor
