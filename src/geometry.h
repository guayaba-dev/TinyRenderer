#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <assert.h>

#include <cmath>
#include <cstddef>
#include <iostream>
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
  vector<vector<float>> m;
  int rows;
  int collums;

 public:
  int getRows() { return rows; }
  int getCollums() { return collums; }

  Matrix(const Vec3f v) {
    m = vector<vector<float>>(4, vector<float>(1, 1.f));
    this->rows = 4;
    this->collums = 1;

    m[0][0] = v.x;
    m[1][0] = v.y;
    m[2][0] = v.z;
  }

  Matrix(const int rows = MAX_ALLOC, const int collums = MAX_ALLOC) {
    m = vector<vector<float>>(rows, vector<float>(collums, 0.f));
    this->rows = rows;
    this->collums = collums;
  }

  vector<float>& operator[](const int idx) {
    assert(idx >= 0 && idx < rows);
    return m[idx];
  }

  static Matrix identity(int dimensions) {
    Matrix result(dimensions, dimensions);

    for (int i = 0; i < dimensions; i++) {
      for (int j = 0; j < dimensions; j++) {
        result[i][j] = (i == j) ? 1.f : 0.f;
      }
    }

    return result;
  }

  Matrix operator*(const Matrix& a) {
    assert(collums == a.rows);

    Matrix result(rows, a.collums);

    for (int i = 0; i < rows; i++) {
      for (int j = 0; j < a.collums; j++) {
        result[i][j] = 0.f;
        for (int k = 0; k < collums; k++) {
          result[i][j] += m[i][k] * a.m[k][j];
        }
      }
    }

    return result;
  }

  Matrix transpose() {
    Matrix result(collums, rows);

    for (int i = 0; i < rows; i++) {
      for (int j = 0; j < collums; j++) {
        result[j][i] = m[i][j];
      }
    }

    return result;
  }

  void output() {
    std::cout << "----------------------------\n";
    for (int j = 0; j < rows; j++) {
      for (int i = 0; i < collums; i++) {
        std::cerr << "[ " << m[j][i] << "] ";
      }
      std::cerr << '\n';
    }
  }

  Vec3f getCollum(int colNum) {
    assert(getRows() >= 3);
    Vec3f a;
    for (int i = 0; i < 3; i++) a[i] = this->m[i][colNum];
    return a;
  };

  void setCollum(int colNum, Vec3f vector) {
    assert(this->rows >= 3);
    for (int i = 0; i < 3; i++) this->m[i][colNum] = vector[i];
  }

  void setCollum(int colNum, Vec2f vector) {
    assert(this->rows >= 2);
    for (int i = 0; i < 2; i++) this->m[i][colNum] = vector[i];
  }

  void setCollum(int colNum, Matrix vector) {
    assert(this->rows >= vector.getRows());
    for (int i = 0; i < vector.getRows(); i++)
      this->m[i][colNum] = vector[i][0];
  }

  Matrix cofactor(int p, int q, int dimension) {
    assert(this->getRows() == this->getCollums());

    // last collum and row end up in zero
    Matrix result(dimension, dimension);

    int i = 0;
    int j = 0;
    for (int row = 0; row < dimension; row++) {
      for (int collum = 0; collum < dimension; collum++) {
        if (row == p || collum == q) continue;

        result[i][j++] = this->m[row][collum];
        if (j == dimension - 1) {
          j = 0;
          i++;
        }
      }
    }
    return result;
  }

  float getDeterminant(int dimensions) {
    assert(this->getCollums() == this->getRows());

    if (dimensions == 1) return this->m[0][0];

    float determinant = 0.f;

    Matrix cofac(dimensions, dimensions);

    int sing = 1;
    for (int f = 0; f < getCollums(); f++) {
      cofac = this->cofactor(0, f, dimensions);
      determinant +=
          sing * this->m[0][f] * cofac.getDeterminant(dimensions - 1);
      sing = -sing;
    }

    return determinant;
  };

  Matrix adjunt() {
    int dimensions = this->getRows();

    Matrix result(dimensions, dimensions);

    for (int i = 0; i < dimensions; i++) {
      for (int j = 0; j < dimensions; j++) {
        int sing = ((i + j) % 2 == 0) ? 1 : -1;

        result[i][j] =
            this->cofactor(i, j, dimensions).getDeterminant(dimensions - 1) *
            sing;
      }
    }

    return result.transpose();
  }

  Matrix inverse() {
    float determinant = 0.f;

    determinant = this->getDeterminant(getRows());

    assert(determinant != 0.f);

    Matrix inverse = this->adjunt();

    for (int i = 0; i < getRows(); i++) {
      for (int j = 0; j < getCollums(); j++) {
        inverse[i][j] = inverse[i][j] / determinant;
      }
    }

    return inverse;
  }
};

#endif  //__GEOMETRY_H__
