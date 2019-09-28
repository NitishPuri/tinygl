#pragma once

#include <array>
#include <string>

struct Color {
  using uchar = unsigned char;

public:
  constexpr Color() : _rgb{0, 0, 0} {}
  constexpr Color(uchar r, uchar g, uchar b) : _rgb{r, g, b} {}
  constexpr Color(uchar g) : _rgb{g, g, g} {}
  //constexpr Color(rgb_t color) : _color(color) {}
  Color(uchar* raw) { 
    _rgb[0] = raw[0];
    _rgb[1] = raw[1];
    _rgb[2] = raw[2];
  }

  Color operator*(float f) const {
    return {uchar(_rgb[0] * f),
            uchar(_rgb[1] * f),
            uchar(_rgb[2] * f)};
  }

  Color operator+(const Color &c) const {
    return {uchar(_rgb[0] + c._rgb[0]),
            uchar(_rgb[1] + c._rgb[1]),
            uchar(_rgb[2] + c._rgb[2])};
  }

  uchar red()   const { return _rgb[0]; }
  uchar green() const { return _rgb[1]; }
  uchar blue()  const { return _rgb[2]; }

  uchar _rgb[3];
};

class Image {
public:
  Image(int width, int height);
  Image(const std::string &filename);

  ~Image();

  void set(int x, int y, Color color);
  Color get(int x, int y) const;

  int get_width() const;
  int get_height() const;

  void write(const std::string &filename, bool flip = true);

private:
  unsigned char *_data = nullptr;
  int _width = 0;
  int _height = 0;
  int _num_components = 0;
  // bitmap_image _image;
};

namespace Colors {
constexpr Color White{255, 255, 255};
constexpr Color Red{255, 0, 0};
constexpr Color Green{0, 255, 0};
constexpr Color Blue{0, 0, 255};
Color random();
} // namespace Colors
