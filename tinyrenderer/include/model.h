#pragma once

#include "geometry.h"
#include <array>
#include <vector>

struct VertexInfo {
public:
  VertexInfo() {}
  VertexInfo(Vec3i v) : v_idx(v[0]), t_idx(v[1]), n_idx(v[2]) {}
  int v_idx; // Vertex index
  int t_idx; // Texture index
  int n_idx; // Normal Index
};

class Model {
private:
  std::vector<Vec3f> verts_;
  std::vector<Vec3f> norms_;
  std::vector<std::array<VertexInfo, 3>> faces_;
  std::vector<Vec2f> tex_coords_;
  std::vector<Vec3f> face_norms;

public:
  Model(const std::string filename);
  ~Model();
  int nverts() const;
  int nfaces() const;
  Vec3f vert(int i) const;
  Vec2f tex(int i) const;
  Vec3f normal(int i) const;
  Vec3f face_normal(int i) const;
  std::array<VertexInfo, 3> face(int idx) const;
  void generateFaceNormals();
};
