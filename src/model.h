#pragma once

#include "geometry.h"
#include <vector>
#include <array>

class Model {
private:
  std::vector<Vec3f> verts_;
  std::vector<std::array<Vec3i, 3>> faces_;
  std::vector<Vec2i> tex_coords_;

public:
  Model(const char *filename);
  ~Model();
  int nverts() const;
  int nfaces() const;
  Vec3f vert(int i) const;
  std::array<Vec3i, 3> face(int idx) const;
};
