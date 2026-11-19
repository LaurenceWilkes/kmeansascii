#include <iostream>
#include <filesystem>
#include <stdexcept>
#include "image.hpp"
#include "image_io.hpp"
#include "reduce.hpp"
#include "discretise.hpp"
#include "display.hpp"
#include "html.hpp"

using namespace image_processor;

int main(int argc, char *argv[]) {
  int kColours, gHeight;
  bool blackBackground{false};
  if (argc < 2) {std::cerr << "File name missing"; return 1;}
  if (argc < 3) {
    kColours = 5;
  } else {
    kColours = atoi(argv[2]);
    if (kColours < 1)
      throw std::out_of_range("Invalid character size");
  }
  if (argc < 4) {
    gHeight = 19;
  } else {
    gHeight = atoi(argv[3]);
    if (gHeight < 1)
      throw std::out_of_range("Invalid character size");
  }
  if (argc < 5) {
    blackBackground = false;
  } else {
    blackBackground = true;
  }

  Image original{loadImage(argv[1])};
//  display(original);
  Image reduced{reduce(original, gHeight)};
//  display(reduced);
  kmeans(reduced, kColours);

  const std::filesystem::path htmlPath =
      std::filesystem::path{"html"} /
      (std::filesystem::path{argv[1]}.stem().string() + ".html");
  std::filesystem::create_directories(htmlPath.parent_path());

  writeHtml(reduced, htmlPath, blackBackground);

  return 0;
}
