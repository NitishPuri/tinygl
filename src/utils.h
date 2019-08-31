#pragma once
#include "geometry.h"
#include "tgaimage.h"

#include <array>

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

// Rasterize triangle using line sweep.
void triangle_line_sweep(Vec2i a, Vec2i b, Vec2i c, TGAImage &image,
                         const TGAColor &color) {
  if (a.y > b.y)
    std::swap(a, b);
  if (a.y > c.y)
    std::swap(a, c);
  if (b.y > c.y)
    std::swap(b, c);

  int total_height = c.y - a.y;
  for (int y = a.y; y <= b.y; y++) {
    int segment_height = b.y - a.y + 1;
    auto alpha = (float)(y - a.y) / total_height;
    auto beta =
        (float)(y - a.y) / segment_height; // be careful with divisions by zero
    Vec2i A = a + (c - a) * alpha;
    Vec2i B = a + (b - a) * beta;
    if (A.x > B.x)
      std::swap(A, B);
    for (int j = A.x; j <= B.x; j++) {
      image.set(j, y, color); // attention, due to int casts t0.y+i != A.y
    }
  }
  for (int y = b.y; y <= c.y; y++) {
    int segment_height = c.y - b.y + 1;
    float alpha = (float)(y - a.y) / total_height;
    float beta =
        (float)(y - b.y) / segment_height; // be careful with divisions by zero
    Vec2i A = a + (c - a) * alpha;
    Vec2i B = b + (c - b) * beta;
    if (A.x > B.x)
      std::swap(A, B);
    for (int j = A.x; j <= B.x; j++) {
      image.set(j, y, color); // attention, due to int casts t0.y+i != A.y
    }
  }
}

Vec3f barycentric(const std::array<Vec2i,3>& pts, Vec2i P) {
  Vec3f u = cross(
      Vec3f(float(pts[2][0] - pts[0][0]), float(pts[1][0] - pts[0][0]), float(pts[0][0] - P[0])),
      Vec3f(float(pts[2][1] - pts[0][1]), float(pts[1][1] - pts[0][1]), float(pts[0][1] - P[1])));
  /* `pts` and `P` has integer value as coordinates
     so `abs(u[2])` < 1 means `u[2]` is 0, that means
     triangle is degenerate, in this case return something with negative
     coordinates */
  if (std::abs(u[2]) < 1)
    return Vec3f(-1, 1, 1);
  return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

void triangle(const std::array<Vec2i, 3>& pts, TGAImage &image, TGAColor color) {
  Vec2i bboxmin(image.get_width() - 1, image.get_height() - 1);
  Vec2i bboxmax(0, 0);
  Vec2i clamp(image.get_width() - 1, image.get_height() - 1);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 2; j++) {
      bboxmin[j] = std::max(0, std::min(bboxmin[j], pts[i][j]));
      bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
    }
  }
  Vec2i P;
  for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
    for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
      Vec3f bc_screen = barycentric(pts, P);
      if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)
        continue;
      image.set(P.x, P.y, color);
    }
  }
}
