#pragma once
#include "geometry.h"
#include "tgaimage.h"

namespace Colors {
constexpr TGAColor White(255, 255, 255, 255);
constexpr TGAColor Red(255, 0, 0, 255);
} // namespace Colors

// Draw line segments.
// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
//
void line(Vec2i p, Vec2i q, TGAImage &image, const TGAColor &color) {
  auto steep = false;
  // transpose in case the line is steep.
  if (std::abs(p.x - q.x) < std::abs(p.y - q.y)) {
    std::swap(p.x, p.y);
    std::swap(q.x, q.y);
    steep = true;
  }
  // start from the lower x position.
  if (p.x > q.x) {
    std::swap(p, q);
  }
  auto dx = (p.x - q.x);
  auto dy = (p.y - q.y);
  double derror = std::abs(dy / double(dx));
  double error = 0;
  int y = p.y;
  for (auto x = p.x; x <= q.x; x++) {
    if (steep) {
      image.set(y, x, color);
    } else {
      image.set(x, y, color);
    }
    error += derror;
    if (error > .5) {
      y += (q.y > p.y ? 1 : -1);
      error -= 1;
    }
  }
}

void triangle(Vec2i a, Vec2i b, Vec2i c, TGAImage& image, const TGAColor& color) {
  line(a, b, image, color);
  line(b, c, image, color);
  line(a, c, image, color);
}

// MACROS
#define loop(variable, initial, final, incr)                                   \
  for (auto variable = initial; variable < final; variable += incr)
