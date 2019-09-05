
#include "model.h"
#include "tgaimage.h"
#include "utils.h"
#include <memory>
#include <string>

constexpr auto MODEL_PATH =
    "D:/tree/rendering/tinyrenderer/obj/african_head.obj";
constexpr auto TEXTURE =
    "D:/tree/rendering/tinyrenderer/obj/african_head_diffuse.tga";

constexpr auto OUTFILE =
    "D:/tree/rendering/tinyrenderer/out/06_african_head_flat.tga";
constexpr auto OUTFILE_2 =
    "D:/tree/rendering/tinyrenderer/out/06_african_head_goroud.tga";

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

Matrix lookat(Vec3f eye, Vec3f center, Vec3f up) {
  Vec3f z = (eye - center).normalize();
  Vec3f x = cross(up, z).normalize();
  Vec3f y = cross(z, x).normalize();
  Matrix Minv = Matrix::identity(4);
  Matrix Tr = Matrix::identity(4);
  for (int i = 0; i < 3; i++) {
    Minv[0][i] = x[i];
    Minv[1][i] = y[i];
    Minv[2][i] = z[i];
    Tr[i][3] = -center[i];
  }
  return Minv * Tr;
}

int main() {
  TGAImage image_flat(width, height, TGAImage::RGB);
  TGAImage image_goroud(width, height, TGAImage::RGB);

  TGAImage texture;
  texture.read_tga_file(TEXTURE);

  Model model(MODEL_PATH);

  Vec3f light_dir{0, 0, -1};
  Vec3f camera(0, 0, 3);

  std::vector<float> zbuffer_1(width * height,
                               std::numeric_limits<float>::min());
  std::vector<float> zbuffer_2(width * height,
                               std::numeric_limits<float>::min());

  Matrix Projection = Matrix::identity(4);
  Projection[3][2] = -1.f / camera[2];

  Matrix ViewPort =
      viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);

  Vec3f eye(5, 3, 10);
  Vec3f center(0, 0, 0);
  Vec3f up(0, 1, 0);
  Matrix View = lookat(eye, center, up);

  Matrix ViewportProjectionView = ViewPort * Projection * View;

  auto map_to_screen = [=](auto v) {
    // int x = int(floorf((v[0] + 1.f) * width / 2.f + .5f));
    // int y = int(floorf((v[1] + 1.f) * height / 2.f + .5f));
    // return Vec3f{float(x), float(y), v[2]};
    return m2v(ViewportProjectionView * v2m(v));
  };

  for (int f = 0; f < model.nfaces(); f++) {
    const auto &face = model.face(f);

    auto get_vertex = [&](auto vidx) { return model.vert(face[vidx].v_idx); };
    auto get_tex = [&](auto vidx) { return model.tex(face[vidx].t_idx); };
    auto get_norm = [&](auto vidx) {
      return model.normal(face[vidx].n_idx) * -1.f;
    };

    auto get_color = [&](auto vidx) {
      auto tex = get_tex(vidx);
      return texture.get(int(tex[0] * texture.get_width()),
                         int((1. - tex[1]) * texture.get_height()));
    };

    // light intensity
    Vec3f face_normal = Vec3f(get_vertex(2) - get_vertex(0)) ^
                        Vec3f(get_vertex(1) - get_vertex(0));
    face_normal.normalize();
    float face_intensity = face_normal * light_dir;

    float v_intensity[3];
    for (int i = 0; i < 3; i++) {
      v_intensity[i] = get_norm(i) * light_dir;
    }

    if (face_intensity < 0)
      continue;

    std::array<Vec3f, 3> vertices{map_to_screen(get_vertex(0)),
                                  map_to_screen(get_vertex(1)),
                                  map_to_screen(get_vertex(2))};

    // auto color =
    //    TGAColor(unsigned char(rand() % 255), unsigned char(rand() % 255),
    //             unsigned char(rand() % 255), 255);

    auto get_color_at_intensity = [](auto intensity) {
      // return TGAColor(unsigned char(intensity * 255),
      //                unsigned char(intensity * intensity  * 255),
      //                unsigned char(intensity * 255),
      //                255);

      return TGAColor(
          unsigned char(
              ((sin(utils::map(intensity, 0, 1, 0, 6.28f)) + 1) / .5) * 255),
          unsigned char((1 - intensity) * 255), unsigned char(intensity * 255),
          255);
    };

    auto face_color = get_color_at_intensity(face_intensity);

    // Flat shading
    auto color_flat = [face_intensity, &get_norm, face_color](auto) {
      // return TGAColor(char(face_intensity * 255.));
      return face_color;
    };

    // Goroud shading
    auto color_goroud = [v_intensity, &get_color_at_intensity](auto bc_screen) {
      float intensity = 0.0f;
      for (int i = 0; i < 3; i++) {
        intensity += (v_intensity[i] * bc_screen[i]);
      }
      //return TGAColor(char(intensity * 255.));
      return get_color_at_intensity(intensity);

      // TGAColor color(0, 0, 0, 0);
      // for (int i = 0; i < 3; i++) {
      //  color = color + (get_color_at_intensity(v_intensity[i]) *
      //  bc_screen[i]);
      //}

      // return color;
    };

    triangle(vertices, zbuffer_1, image_flat, color_flat);
    triangle(vertices, zbuffer_2, image_goroud, color_goroud);
  }

  image_flat.flip_vertically();
  image_flat.write_tga_file(OUTFILE);

  image_goroud.flip_vertically();
  image_goroud.write_tga_file(OUTFILE_2);

  return 0;
}
