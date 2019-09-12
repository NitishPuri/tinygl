#pragma once
#include "geometry.h"
#include "tgaimage.h"

#include <array>
#include <functional>
#include <vector>

Matrix viewport(int x, int y, int w, int h, int depth);
Matrix lookat(Vec3f eye, Vec3f center, Vec3f up);
Matrix projection(float coeff = 0.f); // coeff = -1/c

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

struct IShader {
  std::string _name;
  virtual Vec3f vertex(int face_idx, int v_idx) = 0;
  virtual bool fragment(Vec3f bc, TGAColor &color) = 0;
  virtual ~IShader() = default;
};

void triangle(const std::array<Vec3f, 3> &pts, std::vector<float> &zbuffer,
              TGAImage &image, IShader& shader);
