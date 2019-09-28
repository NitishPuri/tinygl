
#include <ctime>
#include <string>

#include "model.h"
#include "tinygl.h"

#include "app_utils.h"
#include "paths.h"
//#include "shaders.h"

const auto MODEL = Paths::MODELS[0];
const auto PROJ_NO = "08_";

constexpr int width = 800;
constexpr int height = 800;
constexpr int depth = 10;

struct NormalMappingShader : IShader {
  NormalMappingShader() { _name = "NormalMappingShader"; }
  // vertex data
  const Model *model;

  // varying variables
  std::array<Vec2f, 3> v_uv;

  // uniform variables
  Vec3f u_lightDir;
  const Image *u_diffuse = nullptr;
  const Image *u_normal = nullptr;
  Matrix u_vmvp;   // viewport model view projection matrix
  Matrix u_mvp;    // model view projection matrix
  Matrix u_mvp_IT; // model view projection matrix inverse transpose
  void setup() override {
    u_mvp_IT = (u_mvp).inverse().transpose();
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
    n = utils::m2v(u_mvp_IT * utils::v2m(n)).normalize();
    float intensity = std::max(0.f, n * u_lightDir);

    if (u_diffuse == nullptr) {
      color = Colors::White * intensity; // Without diffuse texture
    } else {
      color = utils::get_uv(*u_diffuse, uv) * intensity;
    }

    return true;
  }
};

int main() {

  auto make_camera = [&](Vec3f eye, auto name) {
    auto center = Vec3f{0, 0, 0};
    auto up = Vec3f{0, 1, 0};
    return App::make_camera(eye, center, up, width, height, depth, name);
  };

  auto make_NormalMappingShader = [](const App::Camera &camera,
                                     const Model *model,
                                     const Image *normal_map) {
    auto shader = std::make_unique<NormalMappingShader>();
    shader->model = model;
    shader->u_normal = normal_map;
    shader->u_vmvp = camera.ViewportProjectionView;
    shader->u_mvp = camera.Projection * camera.ModelView;
    shader->u_lightDir = Vec3f(0, 0, -1).normalize();
    shader->setup();
    return shader;
  };

  auto make_NormalMappingDiffuseTextureShader =
      [](const App::Camera &camera, const Model *model, const Image *texture,
         const Image *normal_map) {
        auto shader = std::make_unique<NormalMappingShader>();
        shader->_name = "NormalMappingDiffuseShader";
        shader->model = model;
        shader->u_diffuse = texture;
        shader->u_normal = normal_map;
        shader->u_vmvp = camera.ViewportProjectionView;
        shader->u_mvp = camera.Projection * camera.ModelView;
        shader->u_mvp_IT =
            (camera.Projection * camera.ModelView).inverse().transpose();
        shader->u_lightDir = Vec3f(0, 0, -1).normalize();
        shader->setup();
        return shader;
      };

  for (const auto &model_name : Paths::MODELS) {

    Image texture(Paths::GetDiffuseTexture(model_name));
    Image normal_texture(Paths::GetNormalTexture(model_name));
    Model model(Paths::GetObjPath(model_name));

    std::vector<App::Camera> cameras;
    cameras.emplace_back(make_camera({0, 0, 10}, "c_front"));
    cameras.emplace_back(make_camera({10, 5, 10}, "c_ob"));

    for (const auto &camera : cameras) {
      auto make_shader = [&](auto maker, auto... params) {
        return maker(camera, &model, params...);
      };

      std::vector<std::unique_ptr<IShader>> shaders;
      shaders.emplace_back(
          make_shader(make_NormalMappingShader, &normal_texture));
      shaders.emplace_back(make_shader(make_NormalMappingDiffuseTextureShader,
                                       &texture, &normal_texture));

      for (auto &shader : shaders) {
        App::render_and_show(model, shader, width, height, MODEL, PROJ_NO);
      }
    }
  }

  std::cout << "Press any key to exit...\n";
  getchar();

  return 0;
}
