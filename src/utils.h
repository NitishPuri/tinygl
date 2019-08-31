#pragma once
#include "tgaimage.h"

namespace Colors {
constexpr TGAColor White(255, 255, 255, 255);
constexpr TGAColor Red(255, 0, 0, 255);
} // namespace Colors


// Draw line segments.
// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
//
void line(int x0, int y0, int x1, int y1, TGAImage &image,
          const TGAColor &color) {
  auto steep = false;
  // transpose in case the line is steep.
  if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
    std::swap(x0, y0);
    std::swap(x1, y1);
    steep = true;
  }
  // start from the lower x position.
  if (x0 > x1) {
    std::swap(x0, x1);
    std::swap(y0, y1);
  }
  int dx = (x1 - x0);
  int dy = (y1 - y0);
  double derror = std::abs(dy / double(dx));
  double error = 0;
  int y = y0;
  for (auto x = x0; x <= x1; x++) {
    if (steep) {
      image.set(y, x, color);
    } else {
      image.set(x, y, color);
    }
    error += derror;
    if (error > .5) {
      y += (y1 > y0 ? 1 : -1);
      error -= 1;
    }
  }
}

// MACROS
#define loop(variable, initial, final, incr) \
  for(auto variable = initial; variable < final; variable+=incr)
