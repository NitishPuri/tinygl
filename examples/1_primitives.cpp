
#include "model.h"
#include "tinygl.h"
#include <string>

const std::string ROOT = ROOT_DIR;
const std::string OUTFILE = ROOT + "out/1_primitives.jpg";

int main() {
  constexpr auto width = 800;
  constexpr auto height = 800;
  Image image(width, height);

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

  std::vector<int> ybuffer(width, std::numeric_limits<int>::min());

  Image render(width, 1);
  rasterize2D(Vec2i(20, 34), Vec2i(744, 400), render, Colors::Red, ybuffer);
  rasterize2D(Vec2i(120, 434), Vec2i(444, 400), render, Colors::Green, ybuffer);
  rasterize2D(Vec2i(330, 463), Vec2i(594, 200), render, Colors::Blue, ybuffer);

  // fill image with render.
  for (int i = 0; i < width; i++) {
    image.set(i, 790, render.get(i, 0));
  }

  image.write(OUTFILE);

  system(OUTFILE.c_str());

  return 0;
}
