#include <cstddef>
#include <random>
#include <algorithm>
#include <vector>
#include <unordered_set>
#include <cmath>

#include "image.hpp"
#include "discretise.hpp"

namespace image_processor {

namespace {
  constexpr double yWeight{1.15}, cWeight{1.0};
  constexpr double tol{0.01}; // maximum weighted squared movement
  constexpr std::size_t maxIter{50};
}

struct Centroid {
  double y;
  double cb;
  double cr;

  double dist(const Colour& c) const {
    double dy{y - c.y}, dcb{cb - c.cb}, dcr{cr - c.cr};
    return yWeight * dy * dy + cWeight * (dcb * dcb + dcr * dcr);
  }

  double dist(const Centroid& c) const {
    double dy{y - c.y}, dcb{cb - c.cb}, dcr{cr - c.cr};
    return yWeight * dy * dy + cWeight * (dcb * dcb + dcr * dcr);
  }

  Colour toColour() const {
    return Colour{d2uc(y), d2uc(cb), d2uc(cr)};
  }
}; // Centroid

struct Accumulator {
  double y{0.0};
  double cb{0.0};
  double cr{0.0};
  std::size_t count{0};

  void addColour(const Colour& c) {
    y += c.y;
    cb += c.cb;
    cr += c.cr;
    ++count;
  } // addColour

  Centroid centre() const {
    if (count == 0) throw std::out_of_range("Accumulator empty");
    return Centroid{y / (double) count, cb / (double) count, cr / (double) count};
  } // centre
}; // Accumulator

std::vector<std::size_t> chooseInitial(std::size_t n, std::size_t k) {
  if (k > n || n < 1 || k == 0) throw std::invalid_argument("Invalid inputs");

  std::mt19937 rng{std::random_device{}()};
  std::uniform_int_distribution<std::size_t> pick{0, n - 1};

  std::unordered_set<std::size_t> seen; seen.reserve(k);
  std::vector<std::size_t> selected; selected.reserve(k);

  while (selected.size() < k) {
    const auto index = pick(rng);
    if (seen.insert(index).second) selected.push_back(index);
  }

  return selected;
}

void kmeans(Image& image, std::size_t k) {
  std::vector<Centroid> centres(k);
  std::size_t imageSize = image.size();
  std::vector<std::size_t> initial{chooseInitial(imageSize, k)};

  for (std::size_t i{0}; i < k; ++i) {
    Colour c{image.colourAt(initial[i])};
    centres[i] = Centroid{
      static_cast<double>(c.y),
      static_cast<double>(c.cb),
      static_cast<double>(c.cr)
    };
  }

  auto minCentre = [&centres](const Colour& pixel) {
    double minDist{centres[0].dist(pixel)};
    std::size_t idx{0};
    for (std::size_t j{1}; j < centres.size(); ++j) {
      double d{centres[j].dist(pixel)};
      if (d < minDist) {
        minDist = d;
        idx = j;
      }
    }
    return idx;
  };

  double centreDisc{tol + 1};
  std::size_t iter{0};

  while (centreDisc > tol && iter < maxIter) {
    std::vector<Accumulator> accs(k);
    for (std::size_t i{0}; i < imageSize; ++i) {
      Colour pixel{image.colourAt(i)};
      accs[minCentre(pixel)].addColour(pixel);
    }

    std::vector<Centroid> newCentres(k);
    for (std::size_t i{0}; i < k; ++i) {
      if (accs[i].count == 0) newCentres[i] = centres[i];
      else newCentres[i] = accs[i].centre();
    }

    centreDisc = 0;

    for (std::size_t i{0}; i < k; ++i) {
      centreDisc = std::max(centreDisc, centres[i].dist(newCentres[i]));
    }

    centres = std::move(newCentres);
    ++iter;
  }

  for (std::size_t i{0}; i < imageSize; ++i) {
    Colour pixel{image.colourAt(i)};
    image.setColour(centres[minCentre(pixel)].toColour(), i);
  }

} // kmeans

}
