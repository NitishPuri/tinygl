
#include "model.h"
#include "tgaimage.h"
#include "tinygl.h"
#include <memory>
#include <string>

constexpr auto MODEL_PATH =
    "D:/tree/rendering/tinyrenderer/obj/african_head.obj";
constexpr auto TEXTURE =
    "D:/tree/rendering/tinyrenderer/obj/african_head_diffuse.tga";

constexpr auto OUTFILE_TEX_1 =
    "D:/tree/rendering/tinyrenderer/out/04_african_head_tex_1.tga";
constexpr auto OUTFILE_TEX_2 =
    "D:/tree/rendering/tinyrenderer/out/04_african_head_tex_2.tga";

constexpr int width = 800;
constexpr int height = 800;

int main() {
  TGAImage image_1(width, height, TGAImage::RGB);
  TGAImage image_2(width, height, TGAImage::RGB);

  TGAImage texture;
  texture.read_tga_file(TEXTURE);

  Model model(MODEL_PATH);

  Vec3f light_dir{0, 0, -1};

  std::vector<float> zbuffer_1(width * height,
                               std::numeric_limits<float>::min());
  std::vector<float> zbuffer_2(width * height,
                               std::numeric_limits<float>::min());

  auto map_to_screen = [=](auto v) {
    int x = int(floorf((v[0] + 1.f) * width / 2.f + .5f));
    int y = int(floorf((v[1] + 1.f) * height / 2.f + .5f));
    return Vec3f{float(x), float(y), v[2]};
  };

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
      return TGAColor(char(intensity * 255.));
    };

    // Color interpolation
    auto color_interp = [&get_color](auto bc_screen) {
      TGAColor color;
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

  image_1.flip_vertically();
  image_1.write_tga_file(OUTFILE_TEX_1);

  image_2.flip_vertically();
  image_2.write_tga_file(OUTFILE_TEX_2);

  return 0;
}
