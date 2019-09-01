#include "model.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

Model::Model(const char *filename) : verts_(), faces_() {
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
        iss >> v.raw[i];
      verts_.push_back(v);
    } else if (!line.compare(0, 2, "f ")) {
      std::array<Vec3i, 3> f;
      Vec3i tmp;
      //int itrash, idx, tidx;
      iss >> trash;
      int f_count = 0;
      while (iss >> tmp[0] >> trash >> tmp[1] >> trash >> tmp[2]) {
        tmp = tmp - Vec3i{1, 1, 1}; // in wavefront obj all indices start at 1, not zero
        f[f_count++] = tmp;
      }
      faces_.push_back(f);
    } else if (!line.compare(0, 2, "vt")) {
      iss >> trash >> trash;
      Vec2i uv;
      iss >> uv.u >> uv.v;
      tex_coords_.emplace_back(uv);
    }
  }
  std::cerr << "# v# " << verts_.size() << " f# " << faces_.size() << std::endl;
}

Model::~Model() {}

int Model::nverts() const { return (int)verts_.size(); }

int Model::nfaces() const { return (int)faces_.size(); }

std::array<Vec3i, 3> Model::face(int idx) const { return faces_[idx]; }

Vec3f Model::vert(int i) const { return verts_[i]; }
