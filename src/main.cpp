
#include "tgaimage.h"
#include "utils.h"

namespace Colors{
constexpr TGAColor White(255, 255, 255, 255);
constexpr TGAColor Red(255, 0, 0, 255);
}

// Draw line segments.
void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
  loop(x, x0, x1+1, 1) {  
    auto t = (x - x0) / double(x1 - x0);
    auto y = static_cast<int>(y0 + (y1 - y0) * t);
    image.set(x, y, color);
  }
}

int main() {
  TGAImage image(800, 800, TGAImage::RGB);
  image.set(52, 41, Colors::Red);
  image.flip_vertically(); // i want to have the origin at the left bottom
                           // corner of the image

  // make a line
  line(10, 10, 600, 600, image, Colors::Red);
  line(500, 10, 600, 700, image, Colors::White);
  image.write_tga_file("output.tga");
  return 0;
}
