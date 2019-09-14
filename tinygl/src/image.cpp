#include "image.h"

Image::Image(int width, int height) : _image(width, height) {}

void Image::set(int x, int y, Color color) { _image.set_pixel(x, y, color._color); }

Color Image::get(int x, int y) const { return Color {_image.get_pixel(x, y)}; }

int Image::get_width() const { return _image.width(); }
int Image::get_height() const { return _image.height(); }

void Image::write(const std::string &filename) {
  _image.vertical_flip();
  _image.save_image(filename);
}

namespace Colors {
Color random() {
  return Color(unsigned char(rand() % 255),
               unsigned char(rand() % 255),
               unsigned char(rand() % 255));
}

}