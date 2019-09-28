
#include <ctime>
#include <string>

#include "model.h"
#include "tinygl.h"

#include "paths.h"
#include "app_utils.h"
#include "shaders.h"

const auto MODEL = Paths::MODELS[0];
const auto PROJ_NO = "07_ob_";

constexpr int width = 800;
constexpr int height = 800;
constexpr int depth = 10;

int main() {
  Image texture(Paths::GetDiffuseTexture(MODEL));
  Model model(Paths::GetObjPath(MODEL));

  App::Camera camera;
  camera.eye = Vec3f(10, 5, 10);
  camera.center = Vec3f(0, 0, 0);
  camera.up = Vec3f(0, 1, 0);
  camera.viewport_width = width;
  camera.viewport_height = height;
  camera.viewport_depth = depth;
  camera.initialize();

  //Vec3f light_dir{0, 0.5, -1};
  //light_dir.normalize();

  auto make_NoShader = [&]() {
    auto shader = std::make_unique<Shaders::NoShader>();
    shader->model = &model;
    shader->u_color = Colors::White;
    shader->u_mvp = camera.ViewportProjectionView;
    return shader;
  };

  auto make_WireframeShader = [&]() {
    auto shader = std::make_unique<Shaders::WireframeShader>();
    shader->model = &model;
    shader->u_color = Colors::White;
    shader->u_mvp = camera.ViewportProjectionView;
    return shader;
  };

  auto make_FlatShader = [&]() {
    auto shader = std::make_unique<Shaders::FlatShader>();
    shader->model = &model;
    model.generateFaceNormals();
    shader->u_lightDir = Vec3f(0, 0, -1).normalize();
    shader->u_color = Colors::White;
    shader->u_mvp = camera.ViewportProjectionView;
    return shader;
  };

  auto make_DeepPurpleFlatShader = [&]() {
    auto shader = std::make_unique<Shaders::DeepPurpleFlatShader>();
    shader->model = &model;
    model.generateFaceNormals();
    shader->u_lightDir = Vec3f(0, 0, -1).normalize();
    shader->u_mvp = camera.ViewportProjectionView;
    return shader;
  };

  auto make_GouraudShader = [&]() {
    auto shader = std::make_unique<Shaders::GouraudShader>();
    shader->model = &model;
    shader->u_lightDir = Vec3f(0, 0, -1).normalize();
    shader->u_color = Colors::White;
    shader->u_mvp = camera.ViewportProjectionView;
    return shader;
  };

  auto make_SmoothShader = [&]() {
    auto shader = std::make_unique<Shaders::SmoothShader>();
    shader->model = &model;
    shader->u_lightDir = Vec3f(0, 0, -1).normalize();
    shader->u_mvp = camera.ViewportProjectionView;
    return shader;
  };

  auto make_ToonShader = [&]() {
    auto shader = std::make_unique<Shaders::ToonShader>();
    shader->model = &model;
    shader->u_lightDir = Vec3f(0, 0, -1).normalize();
    shader->u_mvp = camera.ViewportProjectionView;
    return shader;
  };

  auto make_DiffuseTextureShader = [&]() {
    auto shader = std::make_unique<Shaders::DiffuseTextureShader>();
    shader->model = &model;
    shader->u_diffuse = &texture;
    shader->u_mvp = camera.ViewportProjectionView;
    return shader;
  };

  std::vector<std::unique_ptr<IShader>> shaders;
  shaders.emplace_back(make_NoShader());
  shaders.emplace_back(make_FlatShader());
  shaders.emplace_back(make_WireframeShader());
  shaders.emplace_back(make_DeepPurpleFlatShader());
  shaders.emplace_back(make_GouraudShader());
  shaders.emplace_back(make_SmoothShader());
  shaders.emplace_back(make_ToonShader());
  shaders.emplace_back(make_DiffuseTextureShader());

  for (auto &shader : shaders) {
    App::render_and_show(model, shader, width, height, MODEL, PROJ_NO);
  }

  std::cout << "Press any key to exit...\n";
  getchar();

  return 0;
}
