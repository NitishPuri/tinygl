#pragma once
#include "geometry.h"
#include "tgaimage.h"

#include <array>
#include <functional>
#include <vector>

namespace Colors {
constexpr TGAColor White(255, 255, 255, 255);
constexpr TGAColor Red(255, 0, 0, 255);
constexpr TGAColor Green(0, 255, 0, 255);
constexpr TGAColor Blue(0, 0, 255, 255);

TGAColor random() {
  return TGAColor(rand() % 255, rand() % 255, rand() % 255, 255);
}
} // namespace Colors

// Draw line segments.
// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
//
void line(Vec2i p, Vec2i q, TGAImage &image, const TGAColor &color);

// Rasterize triangle using line sweep.
void triangle_line_sweep(Vec2i a, Vec2i b, Vec2i c, TGAImage &image,
                         const TGAColor &color);

void triangle(const std::array<Vec2i, 3> &pts, TGAImage &image, TGAColor color);

void rasterize2D(Vec2i p, Vec2i q, TGAImage &image, TGAColor color,
                 std::vector<int> &ybuffer);

void triangle(const std::array<Vec3f, 3> &pts, std::vector<float> &zbuffer,
              TGAImage &image, std::function<TGAColor(Vec3f)> shader);
