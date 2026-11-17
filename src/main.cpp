#include <iostream>
#include "image.hpp"
#include "image_io.hpp"
#include "reduce.hpp"
#include "discretise.hpp"
#include "display.hpp"

using namespace image_processor;

int main(int argc, char *argv[]) {
  int kColours;
  if (argc < 2) {std::cerr << "File name missing"; return 1;}
  if (argc < 3) {
    kColours = 5;
  } else {
    kColours = atoi(argv[2]);
  }

  Image original{loadImage(argv[1])};
  display(original);
//  Image reduced{reduce(original, 11)};
//  display(reduced);
  kmeans(original, kColours);
  display(original);

  return 0;
}
