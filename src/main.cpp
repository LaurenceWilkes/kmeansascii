#include <iostream>
#include <filesystem>
#include <stdexcept>
#include "image.hpp"
#include "image_io.hpp"
#include "reduce.hpp"
#include "discretise.hpp"
#include "html.hpp"

using namespace image_processor;

void processImage(const char* file, int kColours, int gHeight, bool blackBg) {
  Image original{loadImage(file)};
  Image reduced{reduce(original, gHeight)};
  kmeans(reduced, kColours);

  const std::filesystem::path htmlPath =
    std::filesystem::path{"html"} /
      (std::filesystem::path{file}.stem().string() + "_averaged.html");
  std::filesystem::create_directories(htmlPath.parent_path());

  writeHtml(reduced, htmlPath, blackBg);
} // processImage

int main(int argc, char *argv[]) {
  int kColours, gHeight;
  bool blackBg{false};

  if (argc < 2) { std::cerr << "File name missing"; return 1; }

  if (argc >= 3) {
    kColours = atoi(argv[2]);
    if (kColours < 1)
      throw std::out_of_range("Too few colours");
  } else kColours = 5;

  if (argc >= 4) {
    gHeight = atoi(argv[3]);
    if (gHeight < 1)
      throw std::out_of_range("Invalid character size");
  } else gHeight = 19;

  if (argc >= 5) blackBg = true;
  else blackBg = false;

  // Process image
  processImage(argv[1], kColours, gHeight, blackBg);

  return 0;
}
