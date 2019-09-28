#include <image.h>

int main() {
  std::string base_path = "D:/tree/rendering/tinyrenderer/obj/";
  std::string filename = "african_head/african_head_nm_tangent";
  std::string in_file = base_path + filename + ".tga";
  std::string out_file = base_path + filename + ".jpg";

  Image test_img(in_file);
  test_img.write(out_file, false);
}
