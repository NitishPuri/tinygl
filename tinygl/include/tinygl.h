#pragma once
#include "geometry.h"
#include "image.h"

#include <array>
#include <functional>
#include <vector>

Matrix viewport(int x, int y, int w, int h, int depth);
Matrix lookat(Vec3f eye, Vec3f center, Vec3f up);
Matrix projection(float coeff = 0.f); // coeff = -1/c

// Draw line segments.
// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
//
void line(Vec2i p, Vec2i q, Image &image, const Color &color);

// Rasterize triangle using line sweep.
void triangle_line_sweep(Vec2i a, Vec2i b, Vec2i c, Image &image,
                         const Color &color);

void triangle(const std::array<Vec2i, 3> &pts, Image &image, Color color);

void rasterize2D(Vec2i p, Vec2i q, Image &image, Color color,
                 std::vector<int> &ybuffer);

void triangle(const std::array<Vec3f, 3> &pts, std::vector<float> &zbuffer,
              Image &image, std::function<Color(Vec3f)> shader);

struct IShader {
  std::string _name;
  virtual Vec3f vertex(int face_idx, int v_idx) = 0;
  virtual bool fragment(Vec3f bc, Color &color) = 0;
  virtual void setup() {};
  virtual ~IShader() = default;
};

void triangle(const std::array<Vec3f, 3> &pts, std::vector<float> &zbuffer,
              Image &image, IShader& shader);
