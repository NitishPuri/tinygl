
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

  for (int f = 0; f < model.nfaces(); f++) {
    const auto &face = model.face(f);
    auto get_vertex_of_face = [&](auto vidx) {
      return model.vert(face[vidx][0]);
    };
    Vec3f n = (get_vertex_of_face(2) - get_vertex_of_face(0)) ^
              (get_vertex_of_face(1) - get_vertex_of_face(0));
    n.normalize();
    float intensity = n * light_dir;
    triangle_flat({map_to_screen(get_vertex_of_face(0)),
                   map_to_screen(get_vertex_of_face(1)),
                   map_to_screen(get_vertex_of_face(2))},
                  zbuffer, image_flat, TGAColor(char(intensity * 255.)));
  }

  image_flat.flip_vertically();
  image_flat.write_tga_file(OUTFILE_FLAT);
  return 0;
}