#include <tgaimage.h>
#include <image.h>

int main() {
  std::string base_path = "D:/tree/rendering/tinyrenderer/obj/";
  std::string filename = "diablo3_pose/diablo3_pose_diffuse";
  std::string in_file = base_path + filename + ".tga";
  std::string out_file = base_path + filename + ".bmp";

  TGAImage tga_image;
  tga_image.read_tga_file(in_file);

  Image bmp_image(tga_image.get_width(), tga_image.get_height());
  for (int i = 0; i < tga_image.get_width(); i++) {
    for (int j = 0; j < tga_image.get_height(); j++) {
      auto color = tga_image.get(i, j);
      bmp_image.set(i, j, {color.r, color.g, color.b});
    }
  }

  bmp_image.write(out_file, false);
}
