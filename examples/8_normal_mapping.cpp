
#include <ctime>
#include <string>

#include "model.h"
#include "tinygl.h"

#include "paths.h"

const auto MODEL = MODELS[0];
const auto PROJ_NO = "08_";

constexpr int width = 800;
constexpr int height = 800;
constexpr int depth = 10;

struct GouraudShader : IShader {
  GouraudShader() { _name = "GouraudShader"; }
  // vertex data
  const Model *model;

  // varying
  Vec3f v_intensity;

  // uniform variables
  Vec3f u_lightDir;
  Color u_color;
  Matrix u_mvp; // model view projection matrix

  Vec3f vertex(int face_idx, int v_idx) override {
    const auto &f = model->face(face_idx);
    auto v = model->vert(f[v_idx].v_idx);
    auto n = model->normal(f[v_idx].n_idx);
    v_intensity[v_idx] = std::clamp(n * u_lightDir * -1.f, 0.f, 1.f);
    return utils::m2v(u_mvp * utils::v2m(v));
  }
  bool fragment(Vec3f bc, Color &color) override {
    auto intensity = v_intensity * bc;
    color = u_color * intensity;
    return true;
  }
};

struct NormalMappingShader : IShader {
  NormalMappingShader() { _name = "NormalMappingDiffuseShader"; }
  // vertex data
  const Model *model;

  // varying variables
  std::array<Vec2f, 3> v_uv;

  // uniform variables
  Vec3f u_lightDir;
  const Image *u_diffuse;
  const Image *u_normal;
  Matrix u_vmvp; // viewport model view projection matrix
  Matrix u_mvp;    // model view projection matrix
  Matrix u_mvp_IT; // model view projection matrix inverse transpose  
  void setup() override{
    u_lightDir = utils::m2v(u_mvp * utils::v2m(u_lightDir * -1.f)).normalize();
  }

  Vec3f vertex(int face_idx, int v_idx) override {
    const auto &f = model->face(face_idx);
    auto v = model->vert(f[v_idx].v_idx);
    v_uv[v_idx] = model->tex(f[v_idx].t_idx);
    return utils::m2v(u_vmvp * utils::v2m(v));
  }
  bool fragment(Vec3f bc, Color &color) override {
    Vec2f uv;
    for (int i = 0; i < 3; i++) {
      uv = uv + (v_uv[i] * bc[i]);
    }
    Color nc = utils::get_uv(*u_normal, uv);
    Vec3f n(nc.red(), nc.green(), nc.blue());
    n.normalize();
    n = utils::m2v( u_mvp_IT * utils::v2m(n) ).normalize();
    float intensity = std::max(0.f, n * u_lightDir);

    color = utils::get_uv(*u_diffuse, uv) * intensity;
    //color = Colors::White * intensity; // Without diffuse texture

    return true;
  }
};

int main() {
  Image texture(GetDiffuseTexture(MODEL));
  Image normal_texture(GetNormalTexture(MODEL));
  Model model(GetObjPath(MODEL));

  Vec3f eye(0, 0, 10);
  Vec3f center(0, 0, 0);
  Vec3f up(0, 1, 0);
  Vec3f view_dir = (eye - center).normalize();

  Vec3f light_dir{0, 0.5, -1};
  light_dir.normalize();

  Matrix Projection = projection((eye - center).norm());
  Matrix ViewPort =
      viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4, depth);

  Matrix ModelView = lookat(eye, center, up);

  Matrix ViewportProjectionView = ViewPort * Projection * ModelView;

  std::vector<std::unique_ptr<IShader>> shaders;

  {
    // GouraudShader
    auto shader = std::make_unique<GouraudShader>();
    shader->model = &model;
    shader->u_lightDir = Vec3f(0, 0, -1).normalize();
    shader->u_color = Colors::White;
    shader->u_mvp = ViewportProjectionView;
    //shaders.emplace_back(shader.release());
  }

  {
    // Diffuse Texture
    auto shader = std::make_unique<NormalMappingShader>();
    shader->model = &model;
    shader->u_diffuse = &texture;
    shader->u_normal = &normal_texture;
    shader->u_vmvp = ViewportProjectionView;
    shader->u_mvp = Projection * ModelView;
    shader->u_mvp_IT = (Projection * ModelView).inverse().transpose();
    shader->u_lightDir = Vec3f(0, 0, -1).normalize();
    shader->setup();
    shaders.emplace_back(shader.release());
  }

  for (auto &shader : shaders) {
    Image image(width, height);
    std::vector<float> zbuffer(width * height,
                               std::numeric_limits<float>::min());

    std::clock_t c_start = std::clock();

    for (int f = 0; f < model.nfaces(); f++) {
      std::array<Vec3f, 3> screen_coord{
          shader->vertex(f, 0), shader->vertex(f, 1), shader->vertex(f, 2)};

      triangle(screen_coord, zbuffer, image, *shader);
    }

    std::clock_t c_end = std::clock();
    std::cout << "Elapsed time for rendering with shader (" << shader->_name
              << ") :: " << 1000.0 * (c_end - c_start) / CLOCKS_PER_SEC
              << "ms\n";

    image.write(GetOutputPath(MODEL, PROJ_NO, shader->_name));

    system(GetOutputPath(MODEL, PROJ_NO, shader->_name).c_str());
  }

  std::cout << "Press any key to exit...\n";
  getchar();

  return 0;
}
