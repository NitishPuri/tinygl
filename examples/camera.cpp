
#include "model.h"
#include "tgaimage.h"
#include "utils.h"
#include <memory>
#include <string>

constexpr auto MODEL_PATH =
    "D:/tree/rendering/tinyrenderer/obj/african_head.obj";
constexpr auto TEXTURE =
    "D:/tree/rendering/tinyrenderer/obj/african_head_diffuse.tga";

constexpr auto OUTFILE_WIRE = "D:/tree/rendering/tinyrenderer/african_head.tga";

constexpr int width = 800;
constexpr int height = 800;
constexpr int depth = 255;

// Model *model = NULL;
// int *zbuffer = NULL;
// Vec3f light_dir(0, 0, -1);
// Vec3f camera(0, 0, 3);

Vec3f m2v(Matrix m) {
  return Vec3f(m[0][0] / m[3][0], m[1][0] / m[3][0], m[2][0] / m[3][0]);
}

Matrix v2m(Vec3f v) {
  Matrix m(4, 1);
  m[0][0] = v[0];
  m[1][0] = v[1];
  m[2][0] = v[2];
  m[3][0] = 1.f;
  return m;
}

Matrix viewport(int x, int y, int w, int h) {
  Matrix m = Matrix::identity(4);
  m[0][3] = x + w / 2.f;
  m[1][3] = y + h / 2.f;
  m[2][3] = depth / 2.f;

  m[0][0] = w / 2.f;
  m[1][1] = h / 2.f;
  m[2][2] = depth / 2.f;
  return m;
}

int main() {
  TGAImage image(width, height, TGAImage::RGB);

  TGAImage texture;
  texture.read_tga_file(TEXTURE);

  Model model(MODEL_PATH);

  Vec3f light_dir{0, 0, -1};
  Vec3f camera(0, 0, 3);

  std::vector<float> zbuffer(width * height, std::numeric_limits<float>::min());

  Matrix Projection = Matrix::identity(4);
  Matrix ViewPort =
      viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
  Projection[3][2] = -1.f / camera[2];

  auto map_to_screen = [=](auto v) {
    //int x = int(floorf((v[0] + 1.f) * width / 2.f + .5f));
    //int y = int(floorf((v[1] + 1.f) * height / 2.f + .5f));
    //return Vec3f{float(x), float(y), v[2]};

    return m2v(ViewPort * Projection * v2m(v));
  };

  for (int f = 0; f < model.nfaces(); f++) {
    const auto &face = model.face(f);

    auto get_vertex = [&](auto vidx) { return model.vert(face[vidx].v_idx); };
    auto get_tex = [&](auto vidx) { return model.tex(face[vidx].t_idx); };

    auto get_color = [&](auto vidx) {
      auto tex = get_tex(vidx);
      return texture.get(int(tex[0] * texture.get_width()),
                         int((1. - tex[1]) * texture.get_height()));
    };

    // light intensity
    Vec3f n = Vec3f(get_vertex(2) - get_vertex(0)) ^
              Vec3f(get_vertex(1) - get_vertex(0));
    n.normalize();
    float intensity = n * light_dir;

    if (intensity < 0)
      continue;

    std::array<Vec3f, 3> vertices{map_to_screen(get_vertex(0)),
                                  map_to_screen(get_vertex(1)),
                                  map_to_screen(get_vertex(2))};

    // Flat shading
    auto color_flat = [intensity](auto) {
      return TGAColor(char(intensity * 255.));
    };

    // Color interpolation
    auto color_interp = [&get_color](auto bc_screen) {
      TGAColor color;
      for (int i = 0; i < 3; i++) {
        color = (color + (get_color(i) * bc_screen[i]));
      }
      return color;
    };

    // Texture interpolation
    auto tex_interp = [&get_tex, &texture, intensity](auto bc_screen) {
      Vec2f uv;
      for (int i = 0; i < 3; i++) {
        uv = (uv + (get_tex(i) * bc_screen[i]));
      }
      auto color = texture.get(int(uv[0] * texture.get_width()),
                               int((1. - uv[1]) * texture.get_height()));

      return color;
    };
    triangle(vertices, zbuffer, image, tex_interp);
  }

  image.flip_vertically();
  image.write_tga_file(OUTFILE_WIRE);

  return 0;
}
