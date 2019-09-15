#include <image.h>

int main() {  
  Image newImg(800, 800);

  for (int i = 0; i < 800; i++) {
    for (int j = 0; j < 800; j++) {
      if (i + j % 60 == 0) {
        newImg.set(i, j, {255, 10, 20});
      } else if (i + j % 10 == 0) {
        newImg.set(i, j, {25, 100, 200});
      } else {
        newImg.set(i, j, {Color::uchar(i * 10 % 255),
                          Color::uchar(j * 10 % 255),
                          Color::uchar((i + j) * 10 % 255)});
      }
    }
  }

  std::string base_path = "D:/tree/rendering/tinyrenderer/obj/";
  newImg.write(base_path + "new.jpg");
}
