
#include "model.h"
#include "tgaimage.h"
#include "utils.h"
#include <string>

constexpr auto MODEL_PATH =
    "D:/tree/rendering/tinyrenderer/obj/african_head.obj";
constexpr auto OUTFILE = "D:/tree/rendering/tinyrenderer/african_head_flat.tga";

int main() {
  auto width = 800;
  auto height = 800;
  TGAImage image(width, height, TGAImage::RGB);
  // image.set(52, 41, Colors::Red);
  image.flip_vertically(); // i want to have the origin at the left bottom
  //                         // corner of the image

  // triangle({Vec2i{10, 10}, Vec2i{150, 40}, Vec2i{600, 600}}, image,
  //         Colors::White);

  Model model(MODEL_PATH);

  Vec3f light_dir{0, 0, -1};
  auto map_to_screen = [=](auto v) {
    int x = int((v.x + 1.) * width / 2.);
    int y = int(height - (v.y + 1.) * height / 2.);
    return Vec2i{x, y};
  };

  for (int f = 0; f < model.nfaces(); f++) {
    const auto &face = model.face(f);
    Vec3f n = (model.vert(face[2]) - model.vert(face[0])) ^
              (model.vert(face[1]) - model.vert(face[0]));
    n.normalize();
    float intensity = n * light_dir;
    triangle({map_to_screen(model.vert(face[0])),
              map_to_screen(model.vert(face[1])),
              map_to_screen(model.vert(face[2]))},
             image, TGAColor(char(intensity * 255.)));
  }

  image.write_tga_file(OUTFILE);
  return 0;
}
