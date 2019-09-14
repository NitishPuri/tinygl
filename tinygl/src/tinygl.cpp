#include "tinygl.h"

Matrix viewport(int x, int y, int w, int h, int depth) {
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

Matrix projection(float coeff) // coeff = -1/c
{
  Matrix P = Matrix::identity(4);
  P[3][2] = -1.f / coeff;

  return P;
}




// Draw line segments.
// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
//
void line(Vec2i p, Vec2i q, Image &image, const Color &color) {
  auto steep = false;
  // transpose in case the line is steep.
  if (std::abs(p[0] - q[0]) < std::abs(p[1] - q[1])) {
    std::swap(p[0], p[1]);
    std::swap(q[0], q[1]);
    steep = true;
  }
  // start from the lower x position.
  if (p[0] > q[0]) {
    std::swap(p, q);
  }
  auto dx = (p[0] - q[0]);
  auto dy = (p[1] - q[1]);
  double derror = std::abs(dy / double(dx));
  double error = 0;
  int y = p[1];
  for (auto x = p[0]; x <= q[0]; x++) {
    if (steep) {
      image.set(y, x, color);
    } else {
      image.set(x, y, color);
    }
    error += derror;
    if (error > .5) {
      y += (q[1] > p[1] ? 1 : -1);
      error -= 1;
    }
  }
}

// Rasterize triangle using line sweep.
void triangle_line_sweep(Vec2i a, Vec2i b, Vec2i c, Image &image,
                         const Color &color) {
  if (a[1] > b[1])
    std::swap(a, b);
  if (a[1] > c[1])
    std::swap(a, c);
  if (b[1] > c[1])
    std::swap(b, c);

  int total_height = c[1] - a[1];
  for (int y = a[1]; y <= c[1]; y++) {
    auto first_seg = (y <= b[1]);
    int segment_height = first_seg ? (b[1] - a[1] + 1) : (c[1] - b[1] + 1);
    auto alpha = (float)(y - a[1]) / total_height;
    auto beta = (float)(y - (first_seg ? a[1] : b[1])) /
                segment_height; // be careful with divisions by zero
    Vec2i A = a + (c - a) * alpha;
    Vec2i B = a + (b - a) * beta;
    if (A[0] > B[0])
      std::swap(A, B);
    for (int j = A[0]; j <= B[0]; j++) {
      image.set(j, y, color); // attention, due to int casts t0[1]+i != A[1]
    }
  }
}


template <typename vec>
Vec3f barycentric(const std::array<vec, 3> &pts, vec P) {
  Vec3f s[2];
  //s[0] = 
  for (int i = 0; i <= 1; i++) {
    s[i][0] = float(pts[2][i] - pts[0][i]); // c - a
    s[i][1] = float(pts[1][i] - pts[0][i]); // b - a
    s[i][2] = float(pts[0][i] - P[i]);      // a - p
  }
  Vec3f u = cross(s[0], s[1]);
  if (std::abs(u[2]) > 1e-2) // dont forget that u[2] is integer. If it is zero
                             // then triangle ABC is degenerate
    return Vec3f(1.f - (u[0] + u[1]) / u[2], u[1] / u[2], u[0] / u[2]);

  return Vec3f(-1, 1, 1); // in this case generate negative coordinates, it will
                          // be thrown away by the rasterizator
}

void triangle(const std::array<Vec2i, 3> &pts, Image &image,
              Color color) {
  Vec2i bboxmin(image.get_width() - 1, image.get_height() - 1);
  Vec2i bboxmax(0, 0);
  Vec2i clamp(image.get_width() - 1, image.get_height() - 1);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 2; j++) {
      bboxmin[j] = std::max(0, std::min(bboxmin[j], pts[i][j]));
      bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
    }
  }
  Vec2i P;
  for (P[0] = bboxmin[0]; P[0] <= bboxmax[0]; P[0]++) {
    for (P[1] = bboxmin[1]; P[1] <= bboxmax[1]; P[1]++) {
      Vec3f bc_screen = barycentric(pts, P);
      if (bc_screen[0] < 0 || bc_screen[1] < 0 || bc_screen[2] < 0)
        continue;
      image.set(P[0], P[1], color);
    }
  }
}

void rasterize2D(Vec2i p, Vec2i q, Image &image, Color color,
                 std::vector<int> &ybuffer) {
  if (p[0] > q[0]) {
    std::swap(p, q);
  }

  for (int x = p[0]; x <= q[0]; x++) {
    float t = (x - p[0]) / (float)(q[0] - p[0]);
    int y = static_cast<int>(p[1] * (1. - t) + q[1] * t);
    if (ybuffer[x] < y) {
      ybuffer[x] = y;
      image.set(x, 0, color);
    }
  }
}

void triangle(const std::array<Vec3f, 3> &pts, std::vector<float> &zbuffer,
              Image &image, std::function<Color(Vec3f)> shader) {
  Vec2f bboxmin(std::numeric_limits<float>::max(),
                std::numeric_limits<float>::max());
  Vec2f bboxmax(-std::numeric_limits<float>::max(),
                -std::numeric_limits<float>::max());
  Vec2f clamp(image.get_width() - 1.f, image.get_height() - 1.f);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 2; j++) {
      bboxmin[j] = std::max(0.f, std::min(bboxmin[j], pts[i][j]));
      bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
    }
  }
  // int P;
  for (int x = int(bboxmin[0] + 0.5f); x <= int(bboxmax[0] + 0.5f); x++) {
    for (int y = int(bboxmin[1] + 0.5f); y <= int(bboxmax[1] + 0.5f); y++) {
      Vec3f bc_screen = barycentric(pts, Vec3f(float(x), float(y), 0.f));
      if (bc_screen[0] < 0 || bc_screen[1] < 0 || bc_screen[2] < 0)
        continue;
      float z = 0;
      for (int i = 0; i < 3; i++)
        z += pts[i][2] * bc_screen[i];
      auto zidx = x + y * image.get_width();
      if (zbuffer[zidx] < z) 
      {
        zbuffer[zidx] = z;
        image.set(x, y, shader(bc_screen));
      }
    }
  }
}

void triangle(const std::array<Vec3f, 3> &pts, std::vector<float> &zbuffer,
              Image &image, IShader& shader) {
  Vec2f bboxmin(std::numeric_limits<float>::max(),
                std::numeric_limits<float>::max());
  Vec2f bboxmax(-std::numeric_limits<float>::max(),
                -std::numeric_limits<float>::max());
  Vec2f clamp(image.get_width() - 1.f, image.get_height() - 1.f);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 2; j++) {
      bboxmin[j] = std::max(0.f, std::min(bboxmin[j], pts[i][j]));
      bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
    }
  }
  // int P;
  for (int x = int(bboxmin[0] + 0.5f); x <= int(bboxmax[0] + 0.5f); x++) {
    for (int y = int(bboxmin[1] + 0.5f); y <= int(bboxmax[1] + 0.5f); y++) {
      Vec3f bc_screen = barycentric(pts, Vec3f(float(x), float(y), 0.f));
      if (bc_screen[0] < 0 || bc_screen[1] < 0 || bc_screen[2] < 0)
        continue;
        Color color;
      if (shader.fragment(bc_screen, color)) {
        float z = 0;
        for (int i = 0; i < 3; i++) {
          z += pts[i][2] * bc_screen[i];
        }          
        auto zidx = x + y * image.get_width();
        if (zbuffer[zidx] < z) 
        {
          zbuffer[zidx] = z;
          image.set(x, y, color);
        }
      }
    }
  }
}
