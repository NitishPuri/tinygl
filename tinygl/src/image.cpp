#include "image.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

Image::Image(int width, int height) {
  _data = (unsigned char *) stbi__malloc(width * height * 3);
  if (_data) {
    std::fill(_data, _data + width * height * 3, uchar(0));
    _width = width;
    _height = height;
    _num_components = 3;  
  }
}

Image::Image(const std::string &filename) {
  int x, y, n;
  unsigned char *data = stbi_load(filename.c_str(), &x, &y, &n, 3);
  if (data == nullptr) {
    std::string error(stbi_failure_reason());
    std::cout << "Error loading file :: " << filename << " :: "<< error << '\n';
    return;
  }

  _width = x;
  _height = y;
  _num_components = 3;
  _data = data;
}

Image::~Image() {
  if (_data != NULL) {
    stbi_image_free(_data);
  }    
}

void Image::set(int x, int y, Color color) {
  if (!_data || x < 0 || y < 0 || x >= _width || y >= _height) {
    return;
  }

  memcpy(_data + (x + y * _width) * _num_components, color._rgb, _num_components);
}

Color Image::get(int x, int y) const { 
  return Color(_data + (x + y * _width) * _num_components);
}

int Image::get_width() const { return _width; }
int Image::get_height() const { return _height; }

void Image::write(const std::string &filename, bool flip) {
  auto get_extension = [](const std::string& filename) { 
    std::string ret;
    auto pos = filename.find('.');
    if (pos != std::string::npos) {
      ret = filename.substr(pos+1);
    }
    return ret;
  };

  
  stbi_flip_vertically_on_write(flip);

  auto ext = get_extension(filename);
  if (ext == "jpg" || ext == "jpeg") {
    stbi_write_jpg(filename.c_str(), _width, _height, _num_components, _data,
                   90);  
  } else if (ext == "png") {
    auto stride_in_bytes = _width * _num_components;
    stbi_write_png(filename.c_str(), _width, _height, _num_components, _data,
                   stride_in_bytes);
  } else {
    // Add more if required.
    std::cout << "File format :: " << ext << " not implemented.\n";
  }  
}

namespace Colors {
Color random() {
  return Color(uchar(rand() % 255), uchar(rand() % 255),
               uchar(rand() % 255));
}

} // namespace Colors
