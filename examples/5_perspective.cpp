
#include <ctime>
#include <string>

#include "model.h"
#include "tinygl.h"

#include "paths.h"

using namespace Paths;

const auto MODEL = MODELS[2];
const auto PROJ_NO = "05_";

constexpr int width = 800;
constexpr int height = 800;
constexpr int depth = 255;

int main() {
  Image image(width, height);

  Image texture(GetDiffuseTexture(MODEL));

  Model model(GetObjPath(MODEL));

  Vec3f light_dir{0, 0, -1};
  Vec3f camera(0, 0, 3);

  std::vector<float> zbuffer(width * height, std::numeric_limits<float>::min());

  Matrix Projection = projection(camera[2]);
  //Matrix ViewPort =
  //    viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4, depth);
  Matrix ViewPort =
      viewport(0, 0, width , height , depth);

  Matrix ViewProjection = ViewPort * Projection;

  auto map_to_screen = [=](auto v) {
    // int x = int(floorf((v[0] + 1.f) * width / 2.f + .5f));
    // int y = int(floorf((v[1] + 1.f) * height / 2.f + .5f));
    // return Vec3f{float(x), float(y), v[2]};

    return utils::m2v(ViewProjection * utils::v2m(v));
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
    triangle(vertices, zbuffer, image, tex_interp);
  }

  std::clock_t c_end = std::clock();
  std::cout << "Elapsed time for rendering :: "
            << 1000.0 * (c_end - c_start) / CLOCKS_PER_SEC << "ms\n";


  image.write(GetOutputPath(MODEL, PROJ_NO, "perspective"));

  Image z_img(width, height);
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      if (zbuffer[i + j * width] > 0) {
        z_img.set(i, j, Color(unsigned char(zbuffer[i + j * width])));
      }
    }
  }

  z_img.write(GetOutputPath(MODEL, PROJ_NO, "z_buffer"));

  return 0;
}
