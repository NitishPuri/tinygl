
#include "model.h"
#include "tgaimage.h"
#include "utils.h"
#include <memory>
#include <string>

constexpr auto MODEL_PATH =
    "D:/tree/rendering/tinyrenderer/obj/african_head.obj";

constexpr auto OUTFILE_WIRE =
    "D:/tree/rendering/tinyrenderer/out/02_african_head_wireframe.tga";
constexpr auto OUTFILE_FLAT =
    "D:/tree/rendering/tinyrenderer/out/02_african_head_flat.tga";
constexpr auto OUTFILE_FLAT_LIGHT =
    "D:/tree/rendering/tinyrenderer/out/02_african_head_flat_light.tga";

constexpr int width = 800;
constexpr int height = 800;

int main() {
  TGAImage image(width, height, TGAImage::RGB);
  TGAImage image_flat(width, height, TGAImage::RGB);

  Model model(MODEL_PATH);

  auto map_to_screen = [=](auto v) {
    int x = int(floor((v[0] + 1.f) * width / 2.f + .5f));
    int y = int(floor((v[1] + 1.f) * height / 2.f + .5f));
    return Vec2i{x, y};
  };

  Vec3f light_dir{0, 0, -1};

  for (int f = 0; f < model.nfaces(); f++) {
    const auto &face = model.face(f);

    auto get_vertex = [&](auto vidx) { return model.vert(face[vidx].v_idx); };

    Vec3f n = ( get_vertex(2) - get_vertex(0) ) ^ ( get_vertex(1) - get_vertex(0) );

    std::array<Vec2i, 3> vertices{map_to_screen(get_vertex(0)),
                                  map_to_screen(get_vertex(1)),
                                  map_to_screen(get_vertex(2))};

    line(vertices[0], vertices[1], image, Colors::White);
    line(vertices[1], vertices[2], image, Colors::White);
    line(vertices[0], vertices[2], image, Colors::White);

    triangle(vertices, image_flat, Colors::White);
  }

  image.flip_vertically();
  image.write_tga_file(OUTFILE_WIRE);

  image_flat.flip_vertically();
  image_flat.write_tga_file(OUTFILE_FLAT);

  return 0;
}
