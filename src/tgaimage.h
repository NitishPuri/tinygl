#pragma once

#include <fstream>

#pragma pack(push, 1)
struct TGA_Header {
  char idlength;

  char colormaptype;
  char datatypecode;
  short colormaporigin;
  short colormaplength;
  char colormapdepth;
  short x_origin;
  short y_origin;
  short width;
  short height;
  char bitsperpixel;
  char imagedescriptor;
};
#pragma pack(pop)

struct TGAColor {
  union {
    struct {
      unsigned char b, g, r, a;
#pragma warning(suppress : 4201)
    };
    unsigned char raw[4];
    unsigned int val;
  };
  int bytespp;

  TGAColor() : val(0), bytespp(1) {}

  constexpr TGAColor(unsigned char R, unsigned char G, unsigned char B,
                     unsigned char A)
      : b(B), g(G), r(R), a(A), bytespp(4) {}

  constexpr TGAColor(unsigned char g) : b(g), g(g), r(g), a(255), bytespp(4) {}

  TGAColor(int v, int bpp) : val(v), bytespp(bpp) {}

  TGAColor(const TGAColor &c) : val(c.val), bytespp(c.bytespp) {}

  TGAColor(const unsigned char *p, int bpp) : val(0), bytespp(bpp) {
    for (int i = 0; i < bpp; i++) {
      raw[i] = p[i];
    }
  }

  TGAColor &operator=(const TGAColor &c) {
    if (this != &c) {
      bytespp = c.bytespp;
      val = c.val;
    }
    return *this;
  }

  TGAColor operator*(float f) const {
    return {unsigned char(b * f), unsigned char(g * f), unsigned char(r * f),
            unsigned char(a * f)};
  }

  TGAColor operator+(const TGAColor &c) const {
    return {unsigned char(b + c.b), unsigned char(g + c.g),
            unsigned char(r + c.r), unsigned char(a + c.a)};
  }
};

class TGAImage {
protected:
  unsigned char *data;
  int width;
  int height;
  int bytespp;

  bool load_rle_data(std::ifstream &in);
  bool unload_rle_data(std::ofstream &out) const;

public:
  enum Format { GRAYSCALE = 1, RGB = 3, RGBA = 4 };

  TGAImage();
  TGAImage(int w, int h, int bpp);
  TGAImage(const TGAImage &img);
  bool read_tga_file(const char *filename);
  bool write_tga_file(const char *filename, bool rle = true) const;
  bool flip_horizontally();
  bool flip_vertically();
  bool scale(int w, int h);
  TGAColor get(int x, int y) const;
  bool set(int x, int y, const TGAColor &c);
  ~TGAImage();
  TGAImage &operator=(const TGAImage &img);
  int get_width() const;
  int get_height() const;
  int get_bytespp() const;
  unsigned char *buffer();
  void clear();
};
