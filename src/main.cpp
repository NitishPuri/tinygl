
#include "model.h"
#include "tgaimage.h"
#include "utils.h"
#include <string>

constexpr auto MODEL_PATH =
    "D:/tree/rendering/tinyrenderer/obj/african_head.obj";
constexpr auto OUTFILE =
    "D:/tree/rendering/tinyrenderer/african_head.tga";


int main() {
  auto width = 800;
  auto height = 800;
  TGAImage image(width, height, TGAImage::RGB);
  // image.set(52, 41, Colors::Red);
  image.flip_vertically(); // i want to have the origin at the left bottom
  //                         // corner of the image

  triangle({Vec2i{10, 10}, Vec2i{150, 40}, Vec2i{600, 600}}, image,
           Colors::White);

  /*
  Model model(MODEL_PATH);
  auto draw_line = [=, &image](auto v0, auto v1) {
    int x0 = int((v0.x + 1.) * width / 2.);
    int y0 = int((v0.y + 1.) * height / 2.);
    int x1 = int((v1.x + 1.) * width / 2.);
    int y1 = int((v1.y + 1.) * height / 2.);
    line({x0, y0}, {x1, y1}, image, Colors::White);
  };

  for (int f = 0; f < model.nfaces(); f++) {
    const auto &face = model.face(f);
    auto v1 = model.vert(face[0]);
    auto v2 = model.vert(face[1]);
    auto v3 = model.vert(face[2]);
    draw_line(v1, v2);
    draw_line(v2, v3);
    draw_line(v1, v2);
  }*/

  image.write_tga_file(OUTFILE);
  return 0;
}
