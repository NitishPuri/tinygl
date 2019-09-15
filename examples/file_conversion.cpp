#include <image.h>

int main() {
  std::string base_path = "D:/tree/rendering/tinyrenderer/obj/";
  std::string filename = "diablo3_pose/diablo3_pose_diffuse";
  std::string in_file = base_path + filename + ".tga";
  std::string out_file = base_path + filename + ".png";

  Image test_img(in_file);
  test_img.write(out_file);
}
