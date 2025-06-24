#pragma once

#include <cassert>
#include <cmath>
#include <iostream>
#include <type_traits>
#include <vector>

#define MAX_ALLOC 4

template <int s>
struct vec {
  double data[s] = {0};

  double& operator[](const int i) {
    assert(i < s && i >= 0);
    return data[i];
  }

  const double operator[](const int i) const {
    assert(i < s && i >= 0);
    return data[i];
  }

  template <typename... ARGS,
            typename = typename std::enable_if<sizeof...(ARGS) == s>::type>
  vec(ARGS... args) : data{static_cast<double>(args)...} {}

  vec() = default;
};

template <int n, int m>
class matrix {
  std::vector<float> data = std::vector<float>(n * m);

 public:
  static matrix<n, m> identity() {
    matrix<n, m> result;

    for (int i = 0; i < n; i++)
      for (int j = 0; j < m; j++)
        result(i, j) = (i == j);  // results in one when true

    return result;
  }

  float& operator()(const int& row, const int& collumn) {
    return data[collumn + row * m];
  }

  const float& operator()(const int& row, const int& collumn) const {
    return data[collumn + row * m];
  }

  vec<m> getRow(const int idx) {
    vec<m> res;
    for (int i = m; i--;) res[i] = data[i + idx * m];
    return res;
  }

  void setRow(const vec<m> vec, int idx) {
    for (int i = m; i--;) data[i + idx * m] = vec[i];
  }

  vec<n> getCol(const int idx) {
    vec<n> res;
    for (int i = n; i--;) res[i] = data[idx + i * m];
    return res;
  }

  void setCol(vec<n> vec, int idx) {
    for (int i = n; i--;) data[idx + i * m] = vec[i];
  }

  matrix<n, m> transpose() {
    matrix<m, n> res;

    for (int i = n - 1; i--;)
      for (int j = m - 1; j--; res(j, i) = (*this)(i, j));

    return res;
  }
};

/////////////////////////////////////////////////////////////////////////////

template <int s>
vec<s> operator+(const vec<s>& lhs, const vec<s>& rhs) {
  vec<s> ret = rhs;
  for (int i = 0; i < s; i++) ret[i] = lhs[i] + ret[i];
  return ret;
}

template <int s>
vec<s> operator-(const vec<s>& lhs, const vec<s>& rhs) {
  vec<s> ret = rhs;
  for (int i = 0; i < s; i++) ret[i] = lhs[i] - ret[i];
  return ret;
}

template <int s>
vec<s> operator*(const float& lhs, const vec<s>& rhs) {
  vec<s> ret = rhs;
  for (int i = 0; i < s; i++) ret[i] = lhs * ret[i];
  return ret;
}

template <int s>
float operator*(const vec<s>& lhs, const vec<s>& rhs) {
  float ret = 0;
  for (int i = 0; i < s; i++) ret += lhs[i] * rhs[i];

  return ret;
}

template <int s>
vec<s> operator/(const vec<s>& lhs, const float& rhs) {
  vec<s> ret = lhs;
  for (int i = 0; i < s; i++) ret[i] = ret[i] / rhs;
  return ret;
}

template <int s, int n>
vec<n> proj(const vec<s>& v) {
  vec<n> ret;
  for (int i = 0; i < n; i++) ret[i] = v[i];
  return ret;
}

template <int s, int n>
vec<n> embed(const vec<s>& v, const float& prefix = 0) {
  vec<n> ret;
  for (int i = 0; i < s; i++) ret[i] = v[i];
  for (int i = s; i < n; i++) ret[i] = prefix;
  return ret;
}

template <int r1, int c1, int c2>
matrix<r1, c2> operator*(matrix<r1, c1> mat1, matrix<c1, c2> mat2) {
  matrix<r1, c2> res;

  for (int i = r1; i--;)
    for (int j = c2; j--;) res(i, j) = mat1.getRow(i) * mat2.getCol(j);

  return res;
}

template <int r1, int c1>
matrix<r1, 1> operator*(matrix<r1, c1> mat1, vec<c1> vec) {
  matrix<r1, 1> res = matrix<r1, 1>();

  for (int i = r1; i--;) res(i, 0) = mat1.getRow(i) * vec;

  return res;
}

template <int s>
float norm(vec<s> v) {
  return std::sqrt(v * v);
};

template <int s>
vec<s> normalize(vec<s> v) {
  return v / norm(v);
};

typedef vec<2> Vec2f;
typedef vec<3> Vec3f;
typedef vec<4> Vec4f;

Vec3f crossProduct(Vec3f a, Vec3f b);

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
// LU sustitution optimization for inverse matrices
// Here will functions for that purpose only
// UP forward gauss jordan
// LOWER is coeficients for the UP jordan

template <int n, int m>
void getLU(matrix<n, m> A, matrix<n, m>& L, matrix<n, m>& U) {
  float coeff = 0.0;
  U = A;

  for (int i = 0; i < m; i++) {    // collumn
    for (int j = i; j < n; j++) {  // rows
      // UP Matrix calc

      coeff = U(j, i) / U(i, i);

      L(j, i) = coeff;

      if (coeff == 1) continue;

      U.setRow(U.getRow(j) - (coeff * U.getRow(i)), j);
    }
  }
}

template <int r1, int c1, int c2>
void backwardsGaussianMatrix(matrix<r1, c1>& U, matrix<c1, c2>& x,
                             matrix<c1, c2>& Z) {
  for (int i = 0; i < c2; i++)
    for (int j = r1; j--;) {
      x(j, i) = (Z(j, i) - U.getRow(j) * x.getCol(i)) / U(j, j);
    }
}

template <int r1, int c1, int c2>
void forwardGaussianMatrix(matrix<r1, c1>& L, matrix<c1, c2>& x,
                           matrix<c1, c2>& C) {
  for (int i = 0; i < c2; i++)
    for (int j = 0; j < r1; j++)
      x(j, i) = (C(j, i) - L.getRow(j) * x.getCol(i));
}

template <int n, int m>
void LUInverse(matrix<n, m>& A, matrix<n, m>& AI) {
  matrix<n, m> L, U, LI, I;

  I = matrix<n, m>::identity();

  getLU(A, L, U);

  forwardGaussianMatrix(L, LI, I);

  backwardsGaussianMatrix(U, AI, LI);
}

template <int n, int m>
void solve(matrix<n, m>& A, matrix<n, m>& X, matrix<n, m>& C) {
  matrix<n, m> L, U, Z;

  getLU(A, L, U);

  forwardGaussianMatrix(L, Z, C);

  backwardsGaussianMatrix(U, X, Z);
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
// Output functions for matrices and vectors
//
//

template <int n, int m>
void printMath(matrix<n, m>& A) {
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) std::cerr << A(i, j) << " ";
    std::cerr << "\n";
  }
}

template <int n>
void printMath(vec<n>& v) {
  for (int i = 0; i < n; i++) std::cerr << v[i] << "\n";
}
