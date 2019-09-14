
#include "model.h"
#include "tinygl.h"
#include <string>
#include <ctime>

#include "paths.h"

const auto MODEL = MODELS[1];
const auto PROJ_NO = "03_";

constexpr int width = 800;
constexpr int height = 800;
constexpr int depth = 255;

int main() {
  Image image(width, height);

  Model model(GetObjPath(MODEL));

  auto map_to_screen = [=](auto v) {
    auto x = floor((v.x() + 1.f) * width / 2.f + .5f);
    auto y = floor((v.y() + 1.f) * height / 2.f + .5f);
    auto z = floor((v.z() + 1.f) * depth / 2.f + .5f);
    return Vec3f{x, y, z};
  };

  Vec3f light_dir{0, 0, -1};
  std::vector<float> zbuffer(width * height, std::numeric_limits<float>::min());

  std::clock_t c_start = std::clock();

  for (int f = 0; f < model.nfaces(); f++) {
    const auto &face = model.face(f);

    auto get_vertex = [&](auto vidx) { return model.vert(face[vidx].v_idx); };
    auto get_tex = [&](auto vidx) { return model.tex(face[vidx].t_idx); };

    // light intensity
    Vec3f n = (get_vertex(2) - get_vertex(0)) ^ (get_vertex(1) - get_vertex(0));
    n.normalize();
    float intensity = n * light_dir;

    if (intensity < 0)
      continue;

    std::array<Vec3f, 3> vertices{map_to_screen(get_vertex(0)),
                                  map_to_screen(get_vertex(1)),
                                  map_to_screen(get_vertex(2))};

    // Flat shading
    auto color_flat = [intensity](Vec3f) {
      return Color(char(intensity * 255.));
      // return Colors::White;
    };

    triangle(vertices, zbuffer, image, color_flat);
  }

  std::clock_t c_end = std::clock();
  std::cout << "Elapsed time for rendering :: "
            << 1000.0 * (c_end - c_start) / CLOCKS_PER_SEC << "ms\n";

  image.write(GetOutputPath(MODEL, PROJ_NO, "z_buffered"));

  Image z_img(width, height);
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      if (zbuffer[i + j * width] > 0) {
        z_img.set(i, j, Color(unsigned char(zbuffer[i + j * width])));
      }
    }
  }

  z_img.write(GetOutputPath(MODEL, PROJ_NO, "z_buffer"));

  std::cout << "Press any key to exit...\n";
  getchar();

  return 0;
}
