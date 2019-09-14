
#include <ctime>
#include <string>

#include "model.h"
#include "tinygl.h"

#include "paths.h"

const auto MODEL = MODELS[0];
const auto PROJ_NO = "07_";

constexpr int width = 800;
constexpr int height = 800;
constexpr int depth = 10;

struct NoShader : IShader {
  NoShader() { _name = "NoShader"; }
  Model *model;

  // uniform variables
  Color u_color;
  Matrix u_mvp; // model view projection matrix

  Vec3f vertex(int face_idx, int v_idx) override {
    auto v = model->vert(model->face(face_idx)[v_idx].v_idx);
    return utils::m2v(u_mvp * utils::v2m(v));
  }
  bool fragment(Vec3f, Color &color) override {
    color = u_color;
    return true;
  }
};

struct FlatShader : IShader {
  FlatShader() { _name = "FlatShader"; }
  // vertex data
  const Model *model;

  // varying
  float v_intensity;

  // uniform variables
  Vec3f u_lightDir;
  Color u_color;
  Matrix u_mvp; // model view projection matrix

  Vec3f vertex(int face_idx, int v_idx) override {
    const auto &f = model->face(face_idx);
    auto v = model->vert(f[v_idx].v_idx);
    auto n = model->face_normal(face_idx);
    v_intensity = n * u_lightDir;
    return utils::m2v(u_mvp * utils::v2m(v));
  }
  bool fragment(Vec3f, Color &color) override {
    // auto intensity = (u_lightDir[0] + u_lightDir[1] + u_lightDir[2]) / 3.f;
    color = u_color * v_intensity;
    return true;
  }
};

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
    v_intensity[v_idx] = n * u_lightDir * -1.f;
    return utils::m2v(u_mvp * utils::v2m(v));
  }
  bool fragment(Vec3f bc, Color &color) override {
    auto intensity = v_intensity * bc;
    color = u_color * intensity;
    return true;
  }
};

struct DeepPurpleFlatShader : IShader {
  DeepPurpleFlatShader() { _name = "DeepPurpleFlatShader"; }
  // vertex data
  const Model *model;

  // varying
  float v_intensity;

  // uniform variables
  Vec3f u_lightDir;
  // Color u_color;
  Matrix u_mvp; // model view projection matrix

  Vec3f vertex(int face_idx, int v_idx) override {
    const auto &f = model->face(face_idx);
    auto v = model->vert(f[v_idx].v_idx);
    auto n = model->face_normal(face_idx);
    v_intensity = n * u_lightDir;
    return utils::m2v(u_mvp * utils::v2m(v));
  }
  bool fragment(Vec3f, Color &color) override {
    color = Color(unsigned char(v_intensity * 255),
                     unsigned char((1 - v_intensity) * v_intensity * 255),
                     unsigned char(v_intensity * 255));
    // color = Color(
    //    unsigned char(
    //        ((sin(utils::map(v_intensity, 0, 1, 0, 6.28f)) + 1) / .5) * 255),
    //    unsigned char((1 - v_intensity) * 255),
    //    unsigned char(v_intensity * 255), 255);
    return true;
  }
};

struct DiffuseTextureShader : IShader {
  DiffuseTextureShader() { _name = "DiffuseTextureShader"; }
  // vertex data
  const Model *model;

  // varying variables
  std::array<Vec2f, 3> uv;

  // uniform variables
  Vec3f u_lightDir;
  const Image *u_diffuse;
  Matrix u_mvp; // model view projection matrix

  Vec3f vertex(int face_idx, int v_idx) override {
    const auto &f = model->face(face_idx);
    auto v = model->vert(f[v_idx].v_idx);
    uv[v_idx] = model->tex(f[v_idx].t_idx);
    return utils::m2v(u_mvp * utils::v2m(v));
  }
  bool fragment(Vec3f bc, Color &color) override {
    Vec2f t;
    for (int i = 0; i < 3; i ++) {
      t = t + (uv[i] * bc[i]);
    }    
    color = u_diffuse->get(int(t.x() * u_diffuse->get_width()),
                           int((1 - t.y()) * u_diffuse->get_height()));
    return true;
  }
};

int main() {
  Image texture(GetDiffuseTexture(MODEL));
  Model model(GetObjPath(MODEL));

  Vec3f eye(0, 0, 100);
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
    // No shader

    auto shader = std::make_unique<NoShader>();
    shader->model = &model;
    shader->u_color = Colors::White;
    shader->u_mvp = ViewportProjectionView;
     shaders.emplace_back(shader.release());
  }

  {
    // Flat Shader
    auto shader = std::make_unique<FlatShader>();
    shader->model = &model;
    model.generateFaceNormals();
    shader->u_lightDir = Vec3f(0, 0, -1).normalize();
    shader->u_color = Color(200, 100, 150);
    shader->u_mvp = ViewportProjectionView;
     shaders.emplace_back(shader.release());
  }

  {
    // GouraudShader
    auto shader = std::make_unique<GouraudShader>();
    shader->model = &model;
    shader->u_lightDir = Vec3f(0, 0, -1).normalize();
    shader->u_color = Colors::White;
    shader->u_mvp = ViewportProjectionView;
     shaders.emplace_back(shader.release());
  }

  {
    // Flat Shader
    auto shader = std::make_unique<DeepPurpleFlatShader>();
    shader->model = &model;
    model.generateFaceNormals();
    shader->u_lightDir = Vec3f(0, 0, -1).normalize();
    shader->u_mvp = ViewportProjectionView;
    shaders.emplace_back(shader.release());
  }

  {
    // Diffuse Texture
    auto shader = std::make_unique<DiffuseTextureShader>();
    shader->model = &model;
    shader->u_diffuse = &texture;
    shader->u_mvp = ViewportProjectionView;
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
