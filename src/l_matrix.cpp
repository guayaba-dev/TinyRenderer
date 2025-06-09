#include "l_matrix.h"
vec<3> crossProduct(vec<3> a, vec<3> b) {
  return vec<3>(a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2],
                a[0] * b[1] - a[1] * b[0]);
}
