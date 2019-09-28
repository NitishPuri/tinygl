#pragma once

#include <model.h>
#include <tinygl.h>

#include "paths.h"

namespace App {
void render_and_show(const Model &model, const std::unique_ptr<IShader> &shader,
                     const int width, const int height,
                     const std::string &model_name,
                     const std::string &proj_prefix) {
  Image image(width, height);
  std::vector<float> zbuffer(width * height, std::numeric_limits<float>::min());

  std::clock_t c_start = std::clock();

  for (int f = 0; f < model.nfaces(); f++) {
    std::array<Vec3f, 3> screen_coord{
        shader->vertex(f, 0), shader->vertex(f, 1), shader->vertex(f, 2)};

    triangle(screen_coord, zbuffer, image, *shader);
  }

  std::clock_t c_end = std::clock();
  std::cout << "Elapsed time for rendering with shader (" << shader->_name
            << ") :: " << 1000.0 * (c_end - c_start) / CLOCKS_PER_SEC << "ms\n";

  auto outpath = Paths::GetOutputPath(model_name, proj_prefix, shader->_name);
  image.write(outpath);
  system(outpath.c_str());
}

struct Camera {
  Vec3f eye;
  Vec3f center;
  Vec3f up;
  int viewport_width;
  int viewport_height;
  int viewport_depth;

  Matrix Projection;
  Matrix ViewPort;
  Matrix ModelView;
  Matrix ViewportProjectionView;

  void initialize() {
    Vec3f view_dir = (eye - center).normalize();

    Vec3f light_dir{0, 0.5, -1};
    light_dir.normalize();

    Projection = projection((eye - center).norm());
    ViewPort = viewport(viewport_width / 8, viewport_height / 8,
                        viewport_width * 3 / 4, viewport_height * 3 / 4,
                        viewport_depth);
    ModelView = lookat(eye, center, up);
    ViewportProjectionView = ViewPort * Projection * ModelView;
  }
};

}

