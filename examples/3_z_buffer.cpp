
#include "model.h"
#include "tgaimage.h"
#include "utils.h"
#include <memory>
#include <string>

constexpr auto MODEL_PATH =
    "D:/tree/rendering/tinyrenderer/obj/african_head.obj";

constexpr auto OUTFILE_Z =
    "D:/tree/rendering/tinyrenderer/out/03_african_head_z_buffered.tga";

constexpr int width = 800;
constexpr int height = 800;

int main() {
  TGAImage image(width, height, TGAImage::RGB);

  Model model(MODEL_PATH);

  auto map_to_screen = [=](auto v) {
    auto x = floor((v.x() + 1.f) * width / 2.f + .5f);
    auto y = floor((v.y() + 1.f) * height / 2.f + .5f);
    return Vec3f{x, y, v.z()};
  };

  Vec3f light_dir{0, 0, -1};
  std::vector<float> zbuffer(width * height, std::numeric_limits<float>::min());

  for (int f = 0; f < model.nfaces(); f++) {
    const auto &face = model.face(f);

    auto get_vertex = [&](auto vidx) { return model.vert(face[vidx].v_idx); };
    auto get_tex = [&](auto vidx) { return model.tex(face[vidx].t_idx); };

    // light intensity
    Vec3f n = (get_vertex(2) - get_vertex(0)) ^ (get_vertex(1) - get_vertex(0));
    n.normalize();
    float intensity = n * light_dir;

    std::array<Vec3f, 3> vertices{map_to_screen(get_vertex(0)),
                                  map_to_screen(get_vertex(1)),
                                  map_to_screen(get_vertex(2))};


    // Flat shading
    auto color_flat = [intensity](Vec3f) {
      return TGAColor(char(intensity * 255.));
    };

    triangle(vertices, zbuffer, image, color_flat);
  }

  image.flip_vertically();
  image.write_tga_file(OUTFILE_Z);

  return 0;
}
