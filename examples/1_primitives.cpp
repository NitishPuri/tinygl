
#include "model.h"
#include "tgaimage.h"
#include "utils.h"
#include <string>

constexpr auto OUTFILE = "D:/tree/rendering/tinyrenderer/out/01_primitives.tga";

int main() {
  constexpr auto width = 800;
  constexpr auto height = 800;
  TGAImage image(width, height, TGAImage::RGB);

  // Color a pixel.
  image.set(52, 41, Colors::Red);

  // triangle({Vec2i{10, 10}, Vec2i{150, 40}, Vec2i{600, 600}}, image,
  //         Colors::White);

  {
    line(Vec2i(20, 34), Vec2i(744, 400), image, Colors::Red);
    line(Vec2i(120, 434), Vec2i(444, 400), image, Colors::Green);
    line(Vec2i(330, 463), Vec2i(594, 200), image, Colors::Blue);

    line(Vec2i(10, 790), Vec2i(790, 790), image, Colors::White);
  }

  // TGAImage render(width, 16, TGAImage::RGB);
  std::vector<int> ybuffer(width, std::numeric_limits<int>::min());

  TGAImage render(width, 1, TGAImage::RGB);
  rasterize2D(Vec2i(20, 34), Vec2i(744, 400), render, Colors::Red, ybuffer);
  rasterize2D(Vec2i(120, 434), Vec2i(444, 400), render, Colors::Green, ybuffer);
  rasterize2D(Vec2i(330, 463), Vec2i(594, 200), render, Colors::Blue, ybuffer);

  // fill image with render.
  for (int i = 0; i < width; i++) {
    image.set(i, 790, render.get(i, 0));
  }

  image.flip_vertically(); // i want to have the origin at the left bottom
  image.write_tga_file(OUTFILE);
  return 0;
}
