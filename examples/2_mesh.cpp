
#include "model.h"
#include "tgaimage.h"
#include "tinygl.h"
#include <string>
#include <iostream>

#include "paths.h"

const auto MODEL = MODELS[2];
const auto PROJ_NO = "02_";

constexpr int width = 800;
constexpr int height = 800;

int main() {
  TGAImage image(width, height, TGAImage::RGB);
  TGAImage image_flat(width, height, TGAImage::RGB);
  TGAImage image_light(width, height, TGAImage::RGB);

  std::cout << ROOT_DIR << '\n';

  Model model(GetObjPath(MODEL));

  auto map_to_screen = [=](auto v) {
    int x = int(floor((v.x() + 1.f) * width / 2.f + .5f));
    int y = int(floor((v.y() + 1.f) * height / 2.f + .5f));
    return Vec2i{x, y};
  };

  Vec3f light_dir{0, 0, -1};

  for (int f = 0; f < model.nfaces(); f++) {
    const auto &face = model.face(f);

    auto get_vertex = [&](auto vidx) { return model.vert(face[vidx].v_idx); };

    Vec3f n = (get_vertex(2) - get_vertex(0)) ^ (get_vertex(1) - get_vertex(0));
    n.normalize();

    float intensity = n * light_dir;

    std::array<Vec2i, 3> vertices{map_to_screen(get_vertex(0)),
                                  map_to_screen(get_vertex(1)),
                                  map_to_screen(get_vertex(2))};

    // wireframe
    line(vertices[0], vertices[1], image, Colors::White);
    line(vertices[1], vertices[2], image, Colors::White);
    line(vertices[0], vertices[2], image, Colors::White);

    // uniform flat shading without light
    triangle(vertices, image_flat, Colors::White);

    // flat shading with light
    triangle(vertices, image_light, TGAColor(char(intensity * 255)));
  }

  image.flip_vertically();
  image.write_tga_file(GetOutputPath(MODEL, PROJ_NO, "wireframe"));

  image_flat.flip_vertically();
  image_flat.write_tga_file(GetOutputPath(MODEL, PROJ_NO, "flat"));

  image_light.flip_vertically();
  image_light.write_tga_file(GetOutputPath(MODEL, PROJ_NO, "flat_light"));

  auto f = GetOutputPath(MODEL, PROJ_NO, "wireframe");

  system(f.c_str());
  getchar();

  return 0;
}
