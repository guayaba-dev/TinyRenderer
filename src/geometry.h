#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <assert.h>

#include <cmath>
#include <cstddef>
#include <iostream>
#include <memory>
#include <ostream>
#include <vector>

using std::vector;

const int MAX_ALLOC = 4;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Matrix;

template <class t>
struct Vec2 {
  union {
    struct {
      t u, v;
    };
    struct {
      t x, y;
    };
    t raw[2];
  };
  Vec2() : u(0), v(0) {}
  Vec2(t _u, t _v) : u(_u), v(_v) {}
  Vec2<t>(Matrix m);
  inline Vec2<t> operator+(const Vec2<t>& V) const {
    return Vec2<t>(u + V.u, v + V.v);
  }
  inline Vec2<t> operator-(const Vec2<t>& V) const {
    return Vec2<t>(u - V.u, v - V.v);
  }
  inline Vec2<t> operator*(float f) const { return Vec2<t>(u * f, v * f); }

  inline t& operator[](const int i) { return raw[i]; }
  const inline t& operator[](const int i) const { return raw[i]; }

  template <class>
  friend std::ostream& operator<<(std::ostream& s, Vec2<t>& v);
};

template <class t>
struct Vec3 {
  union {
    struct {
      t x, y, z;
    };
    struct {
      t ivert, iuv, inorm;
    };
    t raw[3];
  };
  Vec3<t>() : x(0), y(0), z(0) {}
  Vec3<t>(t _x, t _y, t _z) : x(_x), y(_y), z(_z) {}
  Vec3<t>(Matrix m);

  inline Vec3<t> operator^(const Vec3<t>& v) const {
    return Vec3<t>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
  }
  inline Vec3<t> operator+(const Vec3<t>& v) const {
    return Vec3<t>(x + v.x, y + v.y, z + v.z);
  }
  inline Vec3<t> operator-(const Vec3<t>& v) const {
    return Vec3<t>(x - v.x, y - v.y, z - v.z);
  }
  inline Vec3<t> operator*(float f) const {
    return Vec3<t>(x * f, y * f, z * f);
  }
  inline t operator*(const Vec3<t>& v) const {
    return x * v.x + y * v.y + z * v.z;
  }

  inline t& operator[](const int i) { return raw[i]; }
  const inline t& operator[](const int i) const { return raw[i]; }

  float norm() const { return std::sqrt(x * x + y * y + z * z); }
  Vec3<t>& normalize(t l = 1) {
    *this = (*this) * (l / norm());
    return *this;
  }
  template <class>
  friend std::ostream& operator<<(std::ostream& s, Vec3<t>& v);
};

typedef Vec2<float> Vec2f;
typedef Vec2<int> Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int> Vec3i;

template <class t>
std::ostream& operator<<(std::ostream& s, Vec2<t>& v) {
  s << "(" << v.x << ", " << v.y << ")\n";
  return s;
}

template <class t>
std::ostream& operator<<(std::ostream& s, Vec3<t>& v) {
  s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
  return s;
}

class Matrix {
 private:
  vector<float> m;
  int rows = 0;
  int columns = 0;

 public:
  const int& getRows() const { return rows; }
  const int& getColumns() const { return columns; }

  Matrix(const int rows = MAX_ALLOC, const int columns = MAX_ALLOC) {
    m = vector<float>(rows * columns, 0.f);
    this->rows = rows;
    this->columns = columns;
  }

  Matrix(const Vec3f& v, int vertex) {
    m = vector<float>(4, vertex);
    this->rows = 4;
    this->columns = 1;

    for (int i = 0; i < 3; i++) {
      (*this)(i, 0) = v[i];
    }
  }

  float& operator()(int row, int col) { return m[row * getColumns() + col]; }

  const float& operator()(int row, int col) const {
    return m[row * getColumns() + col];
  }

  static Matrix identity(int dimension) {
    Matrix result(dimension, dimension);

    for (int i = 0; i < result.getRows(); i++) {
      for (int j = 0; j < result.getColumns(); j++) {
        if (i != j) continue;
        result(i, j) = 1.0;
      }
    }

    return result;
  }

  Matrix operator*(const Matrix& a) const {
    assert((*this).getColumns() == a.getRows());

    Matrix result((*this).getRows(), a.getColumns());

    for (int i = 0; i < (*this).getRows(); i++) {
      for (int j = 0; j < a.getColumns(); j++) {
        for (int k = 0; k < (*this).getColumns(); k++) {
          result(i, j) += (*this)(i, k) * a(k, j);
        }
      }
    }
    return result;
  }

  Matrix transpose() const {
    Matrix result((*this).getColumns(), (*this).getRows());

    for (int i = 0; i < (*this).getRows(); i++) {
      for (int j = 0; j < (*this).getColumns(); j++) {
        result(j, i) = (*this)(i, j);
      }
    }

    return result;
  }

  void output() const {
    std::cerr << "----------------------------\n";
    for (int i = 0; i < (*this).getRows(); i++) {
      for (int j = 0; j < (*this).columns; j++) {
        std::cerr << "[ " << (*this)(i, j) << "] ";
      }
      std::cerr << '\n';
    }
  }

  // TODO: Add vec4
  void setColumn(const int col, const Vec3f& vector) {
    assert((*this).getRows() >= 3);

    for (int i = 0; i < 3; i++) {
      (*this)(i, col) = vector[i];
    }
  }

  void setColumn(const int col, const Vec2f& vector) {
    assert((*this).getRows() >= 2);

    for (int i = 0; i < 2; i++) {
      (*this)(i, col) = vector[i];
    }
  }

  // TODO: add vec4
  Vec3f getColumn(const int col) const {
    Vec3f result;
    for (int i = 0; i < 3; i++) result[i] = (*this)(i, col);
    return result;
  }

  void getCofac(const int& p, const int& q, Matrix& cofac) const {
    int row = 0, col = 0;

    for (int i = 0; i < (*this).getRows(); i++) {
      if (i == p) continue;

      for (int j = 0; j < (*this).getColumns(); j++) {
        if (j == q) continue;

        cofac(row, col++) = (*this)(i, j);

        if (col == cofac.getColumns()) {
          row++;
          col = 0;
        }
      }
    }
  }

  float getDeterminant(const int dimension) {
    assert((*this).getColumns() == (*this).getRows());

    if (dimension == 1) return (*this)(0, 0);

    int sing = 1;
    float result = 0.f;
    Matrix cofac(dimension - 1, dimension - 1);

    for (int f = 0; f < (*this).getColumns(); f++) {
      getCofac(0, 0, cofac);
      result += sing * (*this)(0, f) * cofac.getDeterminant(dimension - 1);
      sing = -sing;
    }

    return result;
  }

  void adjugate(Matrix& adjugate, const float& determinant) {
    float detInv = 1.0 / determinant;
    int dimensions = adjugate.getColumns();
    Matrix cofac(dimensions - 1, dimensions - 1);
    int sing = 0;

    for (int i = 0; i < adjugate.getRows(); i++) {
      for (int j = 0; j < adjugate.getColumns(); j++) {
        sing = ((i + j) % 2 == 0) ? 1 : -1;
        (*this).getCofac(i, j, cofac);
        adjugate(i, j) = cofac.getDeterminant(dimensions - 1) * sing * detInv;
      }
    }
  }

  void inverse(Matrix& inverse) {
    assert((*this).getColumns() == (*this).getRows());

    int dimension = (*this).getColumns();
    float determinant = (*this).getDeterminant(dimension);

    assert(determinant != 0.0);

    adjugate(inverse, determinant);
  }
};

#endif  //__GEOMETRY_H__
