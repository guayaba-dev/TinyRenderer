#include "gl_wrapper.hpp"

#include "gl.h"
#include "l_matrix.h"
#include "l_shaderDefinition.h"
#include "s_WindowManager.h"

// const definitions for drawing
// Temporal soluction to camera, and scene lacking at the moment
const vec<3> UP(0., 1., 0.);
const vec<3> eye(1, 1, 1);
const Vec3f center(0, 0, 0);

void drawModel(Model *model) {
  // zBufferShader shader2 =
  // zBufferShader(lookat(center, eye, Vec3f(0, 1, 0)),
  // viewport(, HEIGHT, 0, 0), projection(0));
}
