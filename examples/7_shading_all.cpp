
#include <ctime>
#include <string>

#include "model.h"
#include "tinygl.h"

#include "app_utils.h"
#include "paths.h"
#include "shaders.h"

const auto PROJ_NO = "07_";

constexpr int width = 800;
constexpr int height = 800;
constexpr int depth = 10;

// Composable callable structures
template <class... Ts> struct MakeShader : Ts... { using Ts::operator()...; };
template <class... Ts> MakeShader(Ts...)->MakeShader<Ts...>;

int main() {

  std::vector<App::Camera> cameras;
  auto make_camera = [&](Vec3f eye, auto name) { 
    auto center = Vec3f{0, 0, 0};
    auto up = Vec3f{0, 1, 0};
    return App::make_camera(eye, center, up, width, height, depth, name);
  };

  cameras.emplace_back(make_camera({0, 0, 10}, "c_front"));
  cameras.emplace_back(make_camera({10, 5, 10}, "c_ob"));

  auto make_NoShader = [](const App::Camera &camera, const Model *model) {
    auto shader = std::make_unique<Shaders::NoShader>();
    shader->model = model;
    shader->u_color = Colors::White;
    shader->u_mvp = camera.ViewportProjectionView;
    return shader;
  };

  auto make_WireframeShader = [](const App::Camera &camera,
                                  const Model *model) {
    auto shader = std::make_unique<Shaders::WireframeShader>();
    shader->model = model;
    shader->u_color = Colors::White;
    shader->u_mvp = camera.ViewportProjectionView;
    return shader;
  };

  auto make_FlatShader = [](const App::Camera &camera, const Model *model) {
    auto shader = std::make_unique<Shaders::FlatShader>();
    shader->model = model;
    shader->u_lightDir = Vec3f(0, 0, -1).normalize();
    shader->u_color = Colors::White;
    shader->u_mvp = camera.ViewportProjectionView;
    return shader;
  };

  auto make_DeepPurpleFlatShader = [](const App::Camera &camera,
                                       const Model *model) {
    auto shader = std::make_unique<Shaders::DeepPurpleFlatShader>();
    shader->model = model;
    shader->u_lightDir = Vec3f(0, 0, -1).normalize();
    shader->u_mvp = camera.ViewportProjectionView;
    return shader;
  };

  auto make_GouraudShader = [](const App::Camera &camera, const Model *model) {
    auto shader = std::make_unique<Shaders::GouraudShader>();
    shader->model = model;
    shader->u_lightDir = Vec3f(0, 0, -1).normalize();
    shader->u_color = Colors::White;
    shader->u_mvp = camera.ViewportProjectionView;
    return shader;
  };

  auto make_SmoothShader = [](const App::Camera &camera, const Model *model) {
    auto shader = std::make_unique<Shaders::SmoothShader>();
    shader->model = model;
    shader->u_lightDir = Vec3f(0, 0, -1).normalize();
    shader->u_mvp = camera.ViewportProjectionView;
    return shader;
  };

  auto make_ToonShader = [](const App::Camera &camera, const Model *model) {
    auto shader = std::make_unique<Shaders::ToonShader>();
    shader->model = model;
    shader->u_lightDir = Vec3f(0, 0, -1).normalize();
    shader->u_mvp = camera.ViewportProjectionView;
    return shader;
  };

  auto make_DiffuseTextureShader = [](const App::Camera &camera,
                                       const Model *model,
                                       const Image * texture) {
    auto shader = std::make_unique<Shaders::DiffuseTextureShader>();
    shader->model = model;
    shader->u_diffuse = texture;
    shader->u_mvp = camera.ViewportProjectionView;
    return shader;
  };

  for (const auto &model_name : Paths::MODELS) {
    Image texture(Paths::GetDiffuseTexture(model_name));
    Model model(Paths::GetObjPath(model_name));
    model.generateFaceNormals();

    for (const auto &camera : cameras) {
      std::vector<std::unique_ptr<IShader>> shaders;

      // Composable shader maker lambda.
      MakeShader make_shader{ 
        [&](auto maker) { return maker(camera, &model); },
        [&](auto maker, auto... params) {
          return maker(camera, &model, params...);
      }};

      shaders.emplace_back(make_shader(make_NoShader));
      shaders.emplace_back(make_shader(make_FlatShader));
      shaders.emplace_back(make_shader(make_WireframeShader));
      shaders.emplace_back(make_shader(make_DeepPurpleFlatShader));
      shaders.emplace_back(make_shader(make_GouraudShader));
      shaders.emplace_back(make_shader(make_SmoothShader));
      shaders.emplace_back(make_shader(make_ToonShader));
      shaders.emplace_back(make_shader(make_DiffuseTextureShader, &texture));

      for (auto &shader : shaders) {
        App::render_and_show(model, shader, width, height, model_name,
                             PROJ_NO + camera._name);
      }
    }
  }

  std::cout << "Press any key to exit...\n";
  getchar();

  return 0;
}
