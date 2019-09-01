
#include "model.h"
#include "tgaimage.h"
#include "utils.h"
#include <memory>
#include <string>

constexpr auto MODEL_PATH =
    "D:/tree/rendering/tinyrenderer/obj/african_head.obj";
constexpr auto TEXTURE =
    "D:/tree/rendering/tinyrenderer/obj/african_head_diffuse.tga";

constexpr auto OUTFILE_FLAT =
    "D:/tree/rendering/tinyrenderer/african_head_flat.tga";
constexpr auto OUTFILE_COLOR_INTERP =
    "D:/tree/rendering/tinyrenderer/african_head_color_interp.tga";
constexpr auto OUTFILE_TEX_INTERP =
    "D:/tree/rendering/tinyrenderer/african_head_texture_interp.tga";

int main() {
  constexpr auto width = 800;
  constexpr auto height = 800;
  TGAImage image_flat(width, height, TGAImage::RGB);
  TGAImage image_color(width, height, TGAImage::RGB);
  TGAImage image_tex(width, height, TGAImage::RGB);

  TGAImage texture;
  texture.read_tga_file(TEXTURE);

  Model model(MODEL_PATH);

  Vec3f light_dir{0, 0, -1};
  auto map_to_screen = [=](auto v) {
    int x = int(floorf((v.x + 1.f) * width / 2.f + .5f));
    int y = int(floorf((v.y + 1.f) * height / 2.f + .5f));
    return Vec3f{float(x), float(y), v.z};
  };

  std::vector<float> zbuffer(width * height, std::numeric_limits<float>::min());
  std::vector<float> zbuffer_col(width * height,
                                 std::numeric_limits<float>::min());
  std::vector<float> zbuffer_tex(width * height,
                                 std::numeric_limits<float>::min());

  for (int f = 0; f < model.nfaces(); f++) {
    const auto &face = model.face(f);

    auto get_vertex = [&](auto vidx) { return model.vert(face[vidx][0]); };
    auto get_tex = [&](auto vidx) { return model.tex(face[vidx][1]); };

    auto get_color = [&](auto vidx) {
      auto tex = get_tex(vidx);
      return texture.get(int(tex.u * texture.get_width()),
                         int((1. - tex.v) * texture.get_height()));
    };

    // light intensity
    Vec3f n = (get_vertex(2) - get_vertex(0)) ^ (get_vertex(1) - get_vertex(0));
    n.normalize();
    float intensity = n * light_dir;

    std::array<Vec3f, 3> vertices{map_to_screen(get_vertex(0)),
                                  map_to_screen(get_vertex(1)),
                                  map_to_screen(get_vertex(2))};

    // Flat shading
    auto color_flat = [intensity](auto) {
      return TGAColor(char(intensity * 255.));
    };
    triangle(vertices, zbuffer, image_flat, color_flat);

    // Color interpolation
    auto color_interp = [&get_color](auto bc_screen) {
      TGAColor color;
      for (int i = 0; i < 3; i++) {
        color = (color + (get_color(i) * bc_screen[i]));
      }
      return color;
    };
    triangle(vertices, zbuffer_col, image_color, color_interp);

    // Texture interpolation
    auto tex_interp = [&get_tex, &texture](auto bc_screen) {
      Vec2f uv;
      for (int i = 0; i < 3; i++) {
        uv = (uv + (get_tex(i) * bc_screen[i]));
      }
      auto color = texture.get(int(uv.u * texture.get_width()),
                               int((1. - uv.v) * texture.get_height()));

      return color;
    };
    triangle(vertices, zbuffer_tex, image_tex, tex_interp);
  }

  image_flat.flip_vertically();
  image_flat.write_tga_file(OUTFILE_FLAT);

  image_color.flip_vertically();
  image_color.write_tga_file(OUTFILE_COLOR_INTERP);

  image_tex.flip_vertically();
  image_tex.write_tga_file(OUTFILE_TEX_INTERP);

  return 0;
}
