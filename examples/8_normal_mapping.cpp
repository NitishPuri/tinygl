
#include <ctime>
#include <string>

#include "model.h"
#include "tinygl.h"

#include "paths.h"
#include "app_utils.h"

const auto MODEL = Paths::MODELS[0];
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
  NormalMappingShader() { _name = "NormalMappingShader"; }
  // vertex data
  const Model *model;

  // varying variables
  std::array<Vec2f, 3> v_uv;

  // uniform variables
  Vec3f u_lightDir;
  const Image *u_diffuse = nullptr;
  const Image *u_normal = nullptr;
  Matrix u_vmvp; // viewport model view projection matrix
  Matrix u_mvp;    // model view projection matrix
  Matrix u_mvp_IT; // model view projection matrix inverse transpose  
  void setup() override{
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
    n = utils::m2v( u_mvp_IT * utils::v2m(n) ).normalize();
    float intensity = std::max(0.f, n * u_lightDir);

    if (u_diffuse == nullptr) {
      color = Colors::White * intensity; // Without diffuse texture
    }
    else {
      color = utils::get_uv(*u_diffuse, uv) * intensity;
    }    

    return true;
  }
};

int main() {
  Image texture(Paths::GetDiffuseTexture(MODEL));
  Image normal_texture(Paths::GetNormalTexture(MODEL));
  Model model(Paths::GetObjPath(MODEL));

  App::Camera camera;
  camera.eye = Vec3f(5, 3, 10);
  camera.center = Vec3f(0, 0, 0);
  camera.up = Vec3f(0, 1, 0);
  camera.viewport_width = width;
  camera.viewport_height = height;
  camera.viewport_depth = depth;
  camera.initialize();

  std::vector<std::unique_ptr<IShader>> shaders;

  {
    // GouraudShader
    auto shader = std::make_unique<GouraudShader>();
    shader->model = &model;
    shader->u_lightDir = Vec3f(0, 0, -1).normalize();
    shader->u_color = Colors::White;
    shader->u_mvp = camera.ViewportProjectionView;
    //shaders.emplace_back(shader.release());
  }

  {
    // Diffuse Texture
    auto shader = std::make_unique<NormalMappingShader>();
    shader->model = &model;
    //shader->u_diffuse = &texture;
    shader->u_normal = &normal_texture;
    shader->u_vmvp = camera.ViewportProjectionView;
    shader->u_mvp = camera.Projection * camera.ModelView;
    shader->u_lightDir = Vec3f(0, 0, -1).normalize();
    shader->setup();
    shaders.emplace_back(shader.release());
  }

  {
    // Diffuse Texture
    auto shader = std::make_unique<NormalMappingShader>();
    shader->_name = "NormalMappingDiffuseShader";
    shader->model = &model;
    shader->u_diffuse = &texture;
    shader->u_normal = &normal_texture;
    shader->u_vmvp = camera.ViewportProjectionView;
    shader->u_mvp = camera.Projection * camera.ModelView;
    shader->u_mvp_IT = (camera.Projection * camera.ModelView).inverse().transpose();
    shader->u_lightDir = Vec3f(0, 0, -1).normalize();
    shader->setup();
    shaders.emplace_back(shader.release());
  }

  for (auto &shader : shaders) {
    App::render_and_show(model, shader, width, height, MODEL, PROJ_NO);
  }

  std::cout << "Press any key to exit...\n";
  getchar();

  return 0;
}
