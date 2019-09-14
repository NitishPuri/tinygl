#pragma once

#include "bitmap_image.hpp"

struct Color {
  using uchar = unsigned char;

public:
  constexpr Color() : _color{0, 0, 0} {}
  constexpr Color(uchar r, uchar g, uchar b) : _color{r, g, b} {}
  constexpr Color(uchar g) : _color{g, g, g} {}
  constexpr Color(rgb_t color) : _color(color) {}
  rgb_t _color;
};

class Image {
public:
  Image(int width, int height);

  void set(int x, int y, Color color);
  Color get(int x, int y) const;

  int get_width() const;
  int get_height() const;

  void write(const std::string &filename);

private:
  bitmap_image _image;
};

namespace Colors {
constexpr Color White{255, 255, 255};
constexpr Color Red{255, 0, 0};
constexpr Color Green{0, 255, 0};
constexpr Color Blue{0, 0, 255};
Color random();
} // namespace Colors
