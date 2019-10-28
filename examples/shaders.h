#pragma once

#include <tinygl.h>

namespace Shaders {

struct NoShader : IShader {
  NoShader() { _name = "NoShader"; }
  const Model *model;

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

struct WireframeShader : IShader {
  WireframeShader() { _name = "WireframeShader"; }
  const Model *model;

  // uniform variables
  Color u_color;
  Matrix u_mvp; // model view projection matrix

  Vec3f vertex(int face_idx, int v_idx) override {
    auto v = model->vert(model->face(face_idx)[v_idx].v_idx);
    return utils::m2v(u_mvp * utils::v2m(v));
  }
  bool fragment(Vec3f bc, Color &color) override {
    constexpr auto eps = 0.3f;
    if (bc.x() < eps || bc.y() < eps || bc.z() < eps) {
      return false;
    }
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
    v_intensity = std::clamp(n * u_lightDir, 0.f, 1.f);
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
    v_intensity[v_idx] = std::clamp(n * u_lightDir * -1.f, 0.f, 1.f);
    return utils::m2v(u_mvp * utils::v2m(v));
  }
  bool fragment(Vec3f bc, Color &color) override {
    auto intensity = v_intensity * bc;
    color = u_color * intensity;
    return true;
  }
};

struct SmoothShader : IShader {
  SmoothShader() { _name = "SmoothShader"; }
  // vertex data
  const Model *model;

  // varying
  Vec3f v_intensity;

  // uniform variables
  Vec3f u_lightDir;
  Matrix u_mvp; // model view projection matrix

  Vec3f vertex(int face_idx, int v_idx) override {
    const auto &f = model->face(face_idx);
    auto v = model->vert(f[v_idx].v_idx);
    auto n = model->normal(f[v_idx].n_idx);
    v_intensity[v_idx] = std::max(0.f, n * u_lightDir * -1.f);
    return utils::m2v(u_mvp * utils::v2m(v));
  }
  bool fragment(Vec3f bc, Color &color) override {
    auto intensity = v_intensity * bc;
    color = Color(uchar(intensity * 255),
                  uchar((1 - intensity) * intensity * 255),
                  uchar(intensity * 255));
    return true;
  }
};

struct ToonShader : IShader {
  ToonShader() { _name = "ToonShader"; }
  // vertex data
  const Model *model;

  // varying
  Vec3f v_intensity;

  // uniform variables
  Vec3f u_lightDir;
  Matrix u_mvp; // model view projection matrix

  Vec3f vertex(int face_idx, int v_idx) override {
    const auto &f = model->face(face_idx);
    auto v = model->vert(f[v_idx].v_idx);
    auto n = model->normal(f[v_idx].n_idx);
    v_intensity[v_idx] = std::max(0.f, n * u_lightDir * -1.f);
    return utils::m2v(u_mvp * utils::v2m(v));
  }
  bool fragment(Vec3f bc, Color &color) override {
    double intensity = v_intensity * bc;
    if (intensity > .85)
      intensity = 1;
    else if (intensity > .60)
      intensity = .80;
    else if (intensity > .45)
      intensity = .60;
    else if (intensity > .30)
      intensity = .45;
    else if (intensity > .15)
      intensity = .30;
    else
      intensity = 0;
    color = Color(uchar(intensity * 255),
                  uchar((1 - intensity) * intensity * 200),
                  uchar(intensity * 255));
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
    v_intensity = std::max(0.0f, n * u_lightDir);
    return utils::m2v(u_mvp * utils::v2m(v));
  }
  bool fragment(Vec3f, Color &color) override {
    color = Color(uchar(v_intensity * 255),
                  uchar((1 - v_intensity) * v_intensity * 255),
                  uchar(v_intensity * 255));
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
    for (int i = 0; i < 3; i++) {
      t = t + (uv[i] * bc[i]);
    }
    color = u_diffuse->get(int(t.x() * u_diffuse->get_width()),
                           int((1 - t.y()) * u_diffuse->get_height()));
    return true;
  }
};

}