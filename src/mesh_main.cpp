
#include "model.h"
#include "tgaimage.h"
#include "utils.h"
#include <memory>
#include <string>

constexpr auto MODEL_PATH =
    "D:/tree/rendering/tinyrenderer/obj/african_head.obj";
constexpr auto OUTFILE = "D:/tree/rendering/tinyrenderer/african_head_flat.tga";

int main() {
  constexpr auto width = 800;
  constexpr auto height = 800;
  TGAImage image(width, height, TGAImage::RGB);
  // image.set(52, 41, Colors::Red);

  // triangle({Vec2i{10, 10}, Vec2i{150, 40}, Vec2i{600, 600}}, image,
  //         Colors::White);

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
    Vec3f n = (model.vert(face[2]) - model.vert(face[0])) ^
              (model.vert(face[1]) - model.vert(face[0]));
    n.normalize();
    float intensity = n * light_dir;
    triangle({map_to_screen(model.vert(face[0])),
              map_to_screen(model.vert(face[1])),
              map_to_screen(model.vert(face[2]))},
             zbuffer, image, TGAColor(char(intensity * 255.)));
  }

  image.flip_vertically();
  image.write_tga_file(OUTFILE);
  return 0;
}
