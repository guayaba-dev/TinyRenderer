#pragma once

#include <chrono>
#include <iostream>

#include "../src/geometry.h"
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
  A.setRow(a, 0);
  A.setRow(b, 1);
  A.setRow(c, 2);

  matrix<3, 3> AI;

  Matrix BI(3, 3);
  Matrix B(3, 3);

  B(0, 0) = 1;
  B(0, 1) = 1 / 2.;
  B(0, 2) = 1 / 3.;

  B(1, 0) = 1 / 2.;
  B(1, 1) = 1 / 3.;
  B(1, 2) = 1 / 4.;

  B(2, 0) = 1 / 3.;
  B(2, 1) = 1 / 4.;
  B(2, 2) = 1 / 5.;
}
