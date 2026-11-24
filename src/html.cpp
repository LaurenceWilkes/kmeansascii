#include <cstdint>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <array>
#include <unordered_map>
#include <vector>
#include <format>

#include "html.hpp"
#include "glyphMetrics.hpp"

namespace image_processor {

namespace {
  constexpr std::array<char, 10> kChars{
    '@', '%', '#', '*', '+', '=', '-', ':', '.', ' ',
  };

  std::uint32_t colourKey(const Colour& colour) {
    return (static_cast<std::uint32_t>(colour.y) << 16) |
           (static_cast<std::uint32_t>(colour.cb) << 8) |
           static_cast<std::uint32_t>(colour.cr);
  }

  char characterFor(const Colour& colour) {
    const int idx = std::round((kChars.size() - 1) * colour.y / 255.0);
    return kChars[idx];
  }

  std::string colourString(const Colour& colour) {
    RGBColour rgbc = colour.toRgb();
    return std::format("rgb({}, {}, {})",
      static_cast<int>(rgbc.r),
      static_cast<int>(rgbc.g),
      static_cast<int>(rgbc.b)
    );
  } // colourString

} // namespace

void writeHtml(const Image& image,
               const std::filesystem::path& outputFile,
               bool blackBackground) {
  const std::size_t imageSize = image.size();
  std::unordered_map<std::uint32_t, std::size_t> colourClasses;
  std::vector<Colour> colours;

  for (std::size_t i = 0; i < imageSize; ++i) {
    const Colour colour = image.colourAt(i);
    std::uint32_t key = colourKey(colour);
    if (!colourClasses.contains(key)) {
      colourClasses[key] = colours.size();
      colours.push_back(colour);
    }
  }

  std::ofstream output{outputFile};
  if (!output) {
    throw std::runtime_error("Could not open HTML output file");
  }

  Colour background{0, 128, 128};
  if (!blackBackground)
    background = image.averageColour(0, image.width(), 0, image.height());

  const double fontSizeFromWidth =
    100.0 / (glyphAdvanceEm * static_cast<double>(image.width()));
  const double fontSizeFromHeight =
    100.0 / (glyphLineHeightEm * static_cast<double>(image.height()));

  output << std::setprecision(12);
  output << "<!doctype html>\n"
         << "<html><head><meta charset=\"utf-8\">\n"
         << "<meta name=\"viewport\""
         << "content=\"width=device-width, initial-scale=1\">\n"
         << "<style>\n"
         << "* { box-sizing: border-box; }\n"
         << "html, body { width: 100%;"
         << "height: 100%; margin: 0; overflow: hidden; background: "
         << colourString(background)
         << "; }\n"
         << "body { display: grid; place-items: center; }\n"
         << "pre{\n"
         << "  margin: 0;\n"
         << "  color: #fff;\n"
         << "  background: transparent;\n"
         << "  font-family: \"Courier New\", Courier, monospace;\n"
         << "  font-size: min(" << fontSizeFromWidth << "vw, "
         << fontSizeFromHeight << "vh);\n"
         << "  font-weight: 700;\n"
         << "  line-height: " << glyphLineHeightEm << ";\n"
         << "  letter-spacing: 0;\n"
         << "  white-space: pre;\n"
         << "  text-align: left;\n"
         << "  text-rendering: geometricPrecision;\n"
         << "}\n";

  for (std::size_t i = 0; i < colours.size(); ++i) {
    output << ".c" << i << "{color:"
           << colourString(colours[i])
           << "; }\n";
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

    output << "\n";
  }

  output << "</pre></body></html>\n";
}

} // namespace image_processor
