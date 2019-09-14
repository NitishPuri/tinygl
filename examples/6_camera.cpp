
#include <ctime>
#include <string>

#include "model.h"
#include "tinygl.h"

#include "paths.h"

const auto MODEL = MODELS[2];
const auto PROJ_NO = "06_";

constexpr int width = 800;
constexpr int height = 800;
constexpr int depth = 255;

int main() {
  Image image_flat(width, height);
  Image image_tex(width, height);

  Image texture(GetDiffuseTexture(MODEL));

  Model model(GetObjPath(MODEL));


  std::vector<float> zbuffer_1(width * height,
                               std::numeric_limits<float>::min());
  std::vector<float> zbuffer_2(width * height,
                               std::numeric_limits<float>::min());

  //Vec3f eye(5, 2, 10);
  Vec3f eye(10, 0, 1000);
  Vec3f center(0, 0, 0);
  Vec3f up(0, 1, 0);
  Vec3f view_dir = (eye - center).normalize() * -1;

  Vec3f light_dir{0, 0.5, -1};
  light_dir.normalize();

  Matrix Projection = projection((eye - center).norm());
  Matrix ViewPort =
      viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4, depth);

  Matrix ModelView = lookat(eye, center, up);

  Matrix ViewportProjectionView = ViewPort * Projection * ModelView;

  auto map_to_screen = [=](auto v) {
    return utils::m2v(ViewportProjectionView * utils::v2m(v));
  };

  std::clock_t c_start = std::clock();

  for (int f = 0; f < model.nfaces(); f++) {
    const auto &face = model.face(f);

    auto get_vertex = [&](auto vidx) { return model.vert(face[vidx].v_idx); };
    auto get_tex = [&](auto vidx) { return model.tex(face[vidx].t_idx); };
    auto get_norm = [&](auto vidx) {
      return model.normal(face[vidx].n_idx) * -1;
    };

    auto get_color = [&](auto vidx) {
      auto tex = get_tex(vidx);
      return texture.get(int(tex[0] * texture.get_width()),
                         int((1. - tex[1]) * texture.get_height()));
    };

    // light intensity
    Vec3f face_normal = Vec3f(get_vertex(2) - get_vertex(0)) ^
                        Vec3f(get_vertex(1) - get_vertex(0));

    //Vec3f face_normal = (get_norm(0) + get_norm(1) + get_norm(2)) * (1.f/ 3.f);

    face_normal.normalize();
    float face_intensity = face_normal * light_dir;

    //if (view_dir * face_normal < 0)
    //  continue;

    std::array<Vec3f, 3> vertices{map_to_screen(get_vertex(0)),
                                  map_to_screen(get_vertex(1)),
                                  map_to_screen(get_vertex(2))};


    auto face_color = Color(char(face_intensity * 255.));

    // Flat shading
    auto color_flat = [face_intensity, face_color](auto) {
      return face_color;
    };

    // Texture interpolation
    auto tex_interp = [&get_tex, &texture, face_intensity](auto bc_screen) {
      Vec2f uv;
      for (int i = 0; i < 3; i++) {
        uv = (uv + (get_tex(i) * bc_screen[i]));
      }
      auto color = texture.get(int(uv[0] * texture.get_width()),
                               int((1. - uv[1]) * texture.get_height()));

      return color;
    };

    triangle(vertices, zbuffer_1, image_flat, color_flat);

    triangle(vertices, zbuffer_2, image_tex, tex_interp);
  }

  std::clock_t c_end = std::clock();
  std::cout << "Elapsed time for rendering :: "
            << 1000.0 * (c_end - c_start) / CLOCKS_PER_SEC << "ms\n";


  image_flat.write(GetOutputPath(MODEL, PROJ_NO, "flat"));

  image_tex.write(GetOutputPath(MODEL, PROJ_NO, "tex"));

  std::cout << "Press any key to exit...\n";
  getchar();

  return 0;
}
