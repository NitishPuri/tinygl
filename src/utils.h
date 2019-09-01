#pragma once
#include "geometry.h"
#include "tgaimage.h"

#include <array>
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
  for (int y = a.y; y <= c.y; y++) {
    auto first_seg = (y <= b.y);
    int segment_height = first_seg ? (b.y - a.y + 1) : (c.y - b.y + 1);
    auto alpha = (float)(y - a.y) / total_height;
    auto beta = (float)(y - (first_seg ? a.y : b.y)) /
                segment_height; // be careful with divisions by zero
    Vec2i A = a + (c - a) * alpha;
    Vec2i B = a + (b - a) * beta;
    if (A.x > B.x)
      std::swap(A, B);
    for (int j = A.x; j <= B.x; j++) {
      image.set(j, y, color); // attention, due to int casts t0.y+i != A.y
    }
  }
}

template <typename vec>
Vec3f barycentric(const std::array<vec, 3> &pts, vec P) {
  Vec3f s[2];
  for (int i = 0; i <= 1; i++) {
    s[i][0] = float(pts[2][i] - pts[0][i]);
    s[i][1] = float(pts[1][i] - pts[0][i]);
    s[i][2] = float(pts[0][i] - P[i]);
  }
  Vec3f u = cross(s[0], s[1]);
  if (std::abs(u[2]) > 1e-2) // dont forget that u[2] is integer. If it is zero
                             // then triangle ABC is degenerate
    return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
  return Vec3f(-1, 1, 1); // in this case generate negative coordinates, it will
                          // be thrown away by the rasterizator
}

void triangle(const std::array<Vec2i, 3> &pts, TGAImage &image,
              TGAColor color) {
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

void rasterize2D(Vec2i p, Vec2i q, TGAImage &image, TGAColor color,
                 std::vector<int> &ybuffer) {
  if (p.x > q.x) {
    std::swap(p, q);
  }

  for (int x = p.x; x <= q.x; x++) {
    float t = (x - p.x) / (float)(q.x - p.x);
    int y = static_cast<int>(p.y * (1. - t) + q.y * t);
    if (ybuffer[x] < y) {
      ybuffer[x] = y;
      image.set(x, 0, color);
    }
  }
}

void triangle_flat(const std::array<Vec3f, 3> &pts, std::vector<float> &zbuffer,
                   TGAImage &image, TGAColor color) {
  Vec2f bboxmin(std::numeric_limits<float>::max(),
                std::numeric_limits<float>::max());
  Vec2f bboxmax(-std::numeric_limits<float>::max(),
                -std::numeric_limits<float>::max());
  Vec2f clamp(image.get_width() - 1.f, image.get_height() - 1.f);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 2; j++) {
      bboxmin[j] = std::max(0.f, std::min(bboxmin[j], pts[i][j]));
      bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
    }
  }
  Vec3f P;
  for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
    for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
      Vec3f bc_screen = barycentric(pts, P);
      if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)
        continue;
      P.z = 0;
      for (int i = 0; i < 3; i++)
        P.z += pts[i][2] * bc_screen[i];
      if (zbuffer[int(P.x + P.y * image.get_width())] < P.z) {
        zbuffer[int(P.x + P.y * image.get_width())] = P.z;
        image.set(int(P.x), int(P.y), color);
      }
    }
  }
}

void triangle_color_interp(const std::array<Vec3f, 3> &pts,
                           const std::array<TGAColor, 3> &colors,
                           std::vector<float> &zbuffer, TGAImage &image) {
  Vec2f bboxmin(std::numeric_limits<float>::max(),
                std::numeric_limits<float>::max());
  Vec2f bboxmax(-std::numeric_limits<float>::max(),
                -std::numeric_limits<float>::max());
  Vec2f clamp(image.get_width() - 1.f, image.get_height() - 1.f);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 2; j++) {
      bboxmin[j] = std::max(0.f, std::min(bboxmin[j], pts[i][j]));
      bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
    }
  }
  Vec3f P;
  for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
    for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
      Vec3f bc_screen = barycentric(pts, P);
      if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)
        continue;
      P.z = 0;
      TGAColor color;
      for (int i = 0; i < 3; i++) {
        P.z += pts[i][2] * bc_screen[i];
        color = (color + (colors.at(i) * bc_screen[i]));
      }
      if (zbuffer[int(P.x + P.y * image.get_width())] < P.z) {
        zbuffer[int(P.x + P.y * image.get_width())] = P.z;
        image.set(int(P.x), int(P.y), color);
      }
    }
  }
}

void triangle_tex_interp(const std::array<Vec3f, 3> &pts,
                         const std::array<Vec2f, 3> &tex_coords,
                         const TGAImage &texture, std::vector<float> &zbuffer,
                         TGAImage &image) {
  Vec2f bboxmin(std::numeric_limits<float>::max(),
                std::numeric_limits<float>::max());
  Vec2f bboxmax(-std::numeric_limits<float>::max(),
                -std::numeric_limits<float>::max());
  Vec2f clamp(image.get_width() - 1.f, image.get_height() - 1.f);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 2; j++) {
      bboxmin[j] = std::max(0.f, std::min(bboxmin[j], pts[i][j]));
      bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
    }
  }
  Vec3f P;
  for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
    for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
      Vec3f bc_screen = barycentric(pts, P);
      if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)
        continue;
      P.z = 0;
      Vec2f uv;

      for (int i = 0; i < 3; i++) {
        P.z += pts[i][2] * bc_screen[i];
        uv = (uv + (tex_coords[i] * bc_screen[i]));
      }
      auto color = texture.get(int(uv.u * texture.get_width()),
                               int((1. - uv.v) * texture.get_height()));
      if (zbuffer[int(P.x + P.y * image.get_width())] < P.z) {
        zbuffer[int(P.x + P.y * image.get_width())] = P.z;
        image.set(int(P.x), int(P.y), color);
      }
    }
  }
}
