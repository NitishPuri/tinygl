
#include <string>
#include <ctime>

#include "model.h"
#include "tinygl.h"

#include "paths.h"

using namespace Paths;

const auto MODEL = MODELS[2];
const auto PROJ_NO = "04_";

constexpr int width = 800;
constexpr int height = 800;
constexpr int depth = 255;

int main() {
  Image image_1(width, height);
  Image image_2(width, height);

  Image texture(GetDiffuseTexture(MODEL));

  Model model(GetObjPath(MODEL));

  Vec3f light_dir{0, 0, -1};

  std::vector<float> zbuffer_1(width * height,
                               std::numeric_limits<float>::min());
  std::vector<float> zbuffer_2(width * height,
                               std::numeric_limits<float>::min());

  auto map_to_screen = [=](auto v) {
    auto x = floorf((v[0] + 1.f) * width / 2.f + .5f);
    auto y = floorf((v[1] + 1.f) * height / 2.f + .5f);
    auto z = floor((v.z() + 1.f) * depth / 2.f + .5f);
    return Vec3f{x, y, z};
  };

  std::clock_t c_start = std::clock();

  for (int f = 0; f < model.nfaces(); f++) {
    const auto &face = model.face(f);

    auto get_vertex = [&](auto vidx) { return model.vert(face[vidx].v_idx); };
    auto get_tex = [&](auto vidx) { return model.tex(face[vidx].t_idx); };

    auto get_color = [&](auto vidx) {
      auto tex = get_tex(vidx);
      return texture.get(int(tex[0] * texture.get_width()),
                         int((1. - tex[1]) * texture.get_height()));
    };

    // light intensity
    Vec3f n = Vec3f(get_vertex(2) - get_vertex(0)) ^
              Vec3f(get_vertex(1) - get_vertex(0));
    n.normalize();
    float intensity = n * light_dir;

    if (intensity < 0)
      continue;

    std::array<Vec3f, 3> vertices{map_to_screen(get_vertex(0)),
                                  map_to_screen(get_vertex(1)),
                                  map_to_screen(get_vertex(2))};

    // Flat shading
    auto color_flat = [intensity](auto) {
      return Color(char(intensity * 255.));
    };

    // Color interpolation
    auto color_interp = [&get_color](auto bc_screen) {
      Color color;
      for (int i = 0; i < 3; i++) {
        color = (color + (get_color(i) * bc_screen[i]));
      }
      return color;
    };
    triangle(vertices, zbuffer_1, image_1, color_interp);

    // Texture interpolation
    auto tex_interp = [&get_tex, &texture, intensity](auto bc_screen) {
      Vec2f uv;
      for (int i = 0; i < 3; i++) {
        uv = (uv + (get_tex(i) * bc_screen[i]));
      }
      auto color = texture.get(int(uv[0] * texture.get_width()),
                               int((1. - uv[1]) * texture.get_height()));

      return color;
    };

    triangle(vertices, zbuffer_2, image_2, tex_interp);
  }

  std::clock_t c_end = std::clock();
  std::cout << "Elapsed time for rendering :: "
            << 1000.0 * (c_end - c_start) / CLOCKS_PER_SEC << "ms\n";


  image_1.write(GetOutputPath(MODEL, PROJ_NO, "tex_01"));

  image_2.write(GetOutputPath(MODEL, PROJ_NO, "tex_02"));

  std::cout << "Press any key to exit...\n";
  getchar();


  return 0;
}
