
#include "tgaimage.h"
#include "utils.h"

int main() {
  TGAImage image(800, 800, TGAImage::RGB);
  image.set(52, 41, Colors::Red);
  image.flip_vertically(); // i want to have the origin at the left bottom
                           // corner of the image

  for (auto i = 0; i < 1000000; i++) {
    line(10, 10, 600, 600, image, Colors::Red);
    line(500, 10, 600, 700, image, Colors::White);
  }

  image.write_tga_file("output.tga");
  return 0;
}
