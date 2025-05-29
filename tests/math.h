#pragma once

#include <iostream>

#include "../src/math.h"

inline void foo() {
  vec<3> a;
  a[0] = 25;
  a[1] = 5;
  a[2] = 1;
  vec<3> b;
  b[0] = 64;
  b[1] = 8;
  b[2] = 1;
  vec<3> c;
  c[0] = 144;
  c[1] = 12;
  c[2] = 1;

  vec<3> d;
  d[0] = 106.8;
  d[1] = 177.2;
  d[2] = 279.2;

  matrix<3, 3> A;
  matrix<3, 3> L;
  matrix<3, 3> U;
  matrix<3, 3> R;

  matrix<3, 1> C;
  C.setCol(d, 0);
  A.setRow(a, 0);
  A.setRow(b, 1);
  A.setRow(c, 2);

  getLU(A, L, U);

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      std::cout << L(i, j) << " ";
    }
    std::cout << '\n';
  }

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      std::cout << U(i, j) << " ";
    }
    std::cout << '\n';
  }

  R = L * A;

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      std::cout << R(i, j) << " ";
    }
    std::cout << '\n';
  }

  matrix<3, 1> x;

  matrix<3, 1> Z;

  forwardGaussianMatrix(L, Z, C);

  backwardsGaussianMatrix(U, x, Z);
}
