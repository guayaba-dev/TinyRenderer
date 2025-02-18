#include "geometry.h"

#include <cassert>

template <>
Vec3<float>::Vec3(Matrix a) {
  assert(a.getRows() == 4 && a.getCollums() == 1);

  x = (a[0][0] / a[3][0]);
  y = (a[1][0] / a[3][0]);
  z = (a[2][0] / a[3][0]);
}
template <>
Vec3<int>::Vec3(Matrix a) {
  assert(a.getRows() == 4 && a.getCollums() == 1);

  x = (int)(a[0][0] / a[3][0]);
  y = (int)(a[1][0] / a[3][0]);
  z = (int)(a[2][0] / a[3][0]);
}

template <>
Vec2<float>::Vec2(Matrix a) {
  assert(a.getRows() == 4 && a.getCollums() == 1);
  x = (a[0][0] / a[3][0]);
  y = (a[1][0] / a[3][0]);
}
template <>
Vec2<int>::Vec2(Matrix a) {
  assert(a.getRows() == 4 && a.getCollums() == 1);
  x = (int)(a[0][0] / a[3][0]);
  y = (int)(a[1][0] / a[3][0]);
}
