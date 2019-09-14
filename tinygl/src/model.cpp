#include "model.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

Model::Model(const std::string filename) : verts_(), faces_() {
  std::ifstream in;
  in.open(filename, std::ifstream::in);
  if (in.fail())
    return;
  std::string line;
  while (!in.eof()) {
    std::getline(in, line);
    std::istringstream iss(line.c_str());
    char trash;
    if (!line.compare(0, 2, "v ")) {
      iss >> trash;
      Vec3f v;
      for (int i = 0; i < 3; i++)
        iss >> v[i];
      verts_.push_back(v);
    } else if (!line.compare(0, 2, "f ")) {
      std::array<VertexInfo, 3> f;
      Vec3i tmp;
      //int itrash, idx, tidx;
      iss >> trash;
      int f_count = 0;
      while (iss >> tmp[0] >> trash >> tmp[1] >> trash >> tmp[2]) {
        // in wavefront obj all indices start at 1, not zero
        f[f_count++] = {tmp - Vec3i(1, 1, 1)};
      }
      faces_.push_back(f);
    } else if (!line.compare(0, 2, "vt")) {
      iss >> trash >> trash;
      Vec2f uv;
      iss >> uv[0] >> uv[1];
      tex_coords_.emplace_back(uv);
    } else if (!line.compare(0, 2, "vn")) {
      iss >> trash >> trash;
      Vec3f norm;
      iss >> norm[0] >> norm[1] >> norm[2];
      norm.normalize();
      norms_.emplace_back(norm);
    }
  }
  std::cerr << "# v# " << verts_.size() << " f# " << faces_.size() << std::endl;
}

Model::~Model() {}

int Model::nverts() const { return (int)verts_.size(); }

int Model::nfaces() const { return (int)faces_.size(); }

std::array<VertexInfo, 3> Model::face(int idx) const { return faces_[idx]; }

Vec3f Model::vert(int i) const { return verts_[i]; }

Vec2f Model::tex(int i) const { return tex_coords_[i]; }

Vec3f Model::normal(int i) const { return norms_[i]; }

Vec3f Model::face_normal(int i) const { return face_norms[i]; }

void Model::generateFaceNormals()
{
  face_norms.clear();
  face_norms.reserve(faces_.size());
  for (const auto& face : faces_) {
    auto get_vertex = [&](auto vidx) { return vert(face[vidx].v_idx); };

    Vec3f face_normal = Vec3f(get_vertex(2) - get_vertex(0)) ^
                        Vec3f(get_vertex(1) - get_vertex(0));
    face_normal.normalize();
    face_norms.push_back(face_normal);
  }
}
