#pragma once

#include <cassert>
#include <cmath>
#include <functional>
#include <iostream>
#include <numeric>

////////////////////////////////////////////////////////////////////////////////////
// Vector
template <typename T, size_t Size> struct Vec {
  Vec(){};
  Vec(const std::array<T, Size> &x) : data(std::move(x)) {}

  Vec<T, Size> &operator=(const Vec<T, Size> &V) {
    if (this != &V) {
      data = V.data;
    }
    return *this;
  }

  // inline Vec<t, Size> operate(const Vec<t, Size> &V,
  //                            std::function<t, (t, t)> op) {
  //  return std::inn
  //}

  inline T operator[](int i) const { return data[i]; }
  inline T &operator[](int i) { return data[i]; }

  inline Vec<T, Size> operator+(const Vec<T, Size> &V) const {
    Vec<T, Size> ret;
    for (auto i = 0; i < Size; i++) {
      ret[i] = (data[i] + V[i]);
    }
    return ret;
  }
  inline Vec<T, Size> operator-(const Vec<T, Size> &V) const {
    Vec<T, Size> ret;
    for (auto i = 0; i < Size; i++) {
      ret[i] = (data[i] - V[i]);
    }
    return ret;
  }
  inline Vec<T, Size> operator*(float f) const {
    Vec<T, Size> ret;
    for (auto i = 0; i < Size; i++) {
      ret[i] = static_cast<T>(data[i] * f);
    }
    return ret;
  }
  inline T operator*(const Vec<T, Size> &v) const {
    T ret = 0;
    for (auto i = 0; i < Size; i++) {
      ret += data[i] * v[i];
    }
    return ret;
  }

  float norm() const {
    return std::sqrtf(
        std::accumulate(std::begin(data), std::end(data), 0.0f,
                        [](auto a, auto b) { return a + b * b; }));
  }
  Vec<T, Size> &normalize(T l = 1) {
    *this = (*this) * (l / norm());
    return *this;
  }

  template <typename T, size_t Size>
  friend std::ostream &operator<<(std::ostream &s, Vec<T, Size> &v);

private:
  std::array<T, Size> data;
};

template <typename T> class Vec2 : public Vec<T, 2> {
public:
  Vec2() : Vec({0, 0}){};
  Vec2(T x, T y) : Vec({x, y}){};
  Vec2(const Vec<T, 2> &obj) : Vec(obj) {}
};

template <typename T> class Vec3 : public Vec<T, 3> {
public:
  Vec3() : Vec({0, 0, 0}){};
  Vec3(T x, T y, T z) : Vec({x, y, z}){};  
  Vec3(const Vec<T, 3> &obj) : Vec(obj) {}
  inline Vec3<T> operator^(const Vec3<T> &v) const {
    const auto &x = operator[](0), y = operator[](1), z = operator[](2);
    const auto &x2 = v[0], y2 = v[1], z2 = v[2];
    return {y * z2 - z * y2, z * x2 - x * z2, x * y2 - y * x2};
  }
};

template <typename T> Vec3<T> cross(Vec3<T> lhs, Vec3<T> rhs) {
  return lhs ^ rhs;
}

using Vec2f = Vec2<float>;
using Vec2i = Vec2<int>;
using Vec3f = Vec3<float>;
using Vec3i = Vec3<int>;

template <class t, size_t Size>
std::ostream &operator<<(std::ostream &s, Vec<t, Size> &v) {
  for (s << "(", int i = 0; i < Size; i++) {
    s << v[i] << (i < (Size - 1)) ? ", " : ")\n";
  }
  return s;
}

class Matrix {
public:
  Matrix(int r = 4, int c = 4);
  inline int nrows();
  inline int ncols();

  static Matrix identity(int dimensions);
  std::vector<float> &operator[](int i);
  Matrix operator*(const Matrix &a) const;

  Matrix transpose() const;
  Matrix inverse() const;

  friend std::ostream &operator<<(std::ostream &s, Matrix &m);

private:
  std::vector<std::vector<float>> m;
  int rows, cols;
};
