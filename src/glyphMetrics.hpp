#pragma once

namespace image_processor {

// Courier New and Courier New Bold both advance 1229 units per character.
// Their height is 2320 units in a 2048 unit em.
inline constexpr double glyphAdvanceEm = 1229.0 / 2048.0;
inline constexpr double glyphLineHeightEm = 2320.0 / 2048.0;

} // namespace image_processor
