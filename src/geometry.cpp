#include "geometry.h"

#include <cassert>

template <>
Vec4<float>::Vec4(Matrix a) {
  assert(a.getRows() == 4 && a.getColumns() == 1);

  x = a(0, 0);
  y = a(1, 0);
  z = a(2, 0);
  w = a(3, 0);
}
template <>
Vec4<int>::Vec4(Matrix a) {
  assert(a.getRows() == 4 && a.getColumns() == 1);

  x = (int)a(0, 0);
  y = (int)a(1, 0);
  z = (int)a(2, 0);
  w = (int)a(3, 0);
}

template <>
Vec3<float>::Vec3(Matrix a) {
  assert(a.getRows() == 3 && a.getColumns() == 1);

  x = a(0, 0);
  y = a(1, 0);
  z = a(2, 0);
}
template <>
Vec3<int>::Vec3(Matrix a) {
  assert(a.getRows() == 3 && a.getColumns() == 1);

  x = (int)a(0, 0);
  y = (int)a(1, 0);
  z = (int)a(2, 0);
}
