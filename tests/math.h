#pragma once

#include <chrono>
#include <iostream>

#include "../src/l_matrix.h"

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

  matrix<3, 3> A;
  A.setRow(a, 0);
  A.setRow(b, 1);
  A.setRow(c, 2);

  matrix<3, 3> B = A * matrix<3, 3>::identity();

  printMath(B);
}
