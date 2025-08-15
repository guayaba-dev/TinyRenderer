#include "gl_wrapper.hpp"

#include <iostream>

#include "gl.h"
#include "l_matrix.h"
#include "l_shaderDefinition.h"

// const definitions for drawing

Window currentWindow;

// Temporal soluction to camera, and scene lacking at the moment
const vec<3> UP(0., 1., 0.);
const vec<3> eye(1, 1, 1);
const Vec3f center(0, 0, 0);

void setWindow(Window& window) { currentWindow = window; }

void drawModel(Model* model) {
  int windowArea = currentWindow.size[0] * currentWindow.size[1];

  zBufferShader shader2 = zBufferShader(
      lookat(center, eye, UP),
      viewport(currentWindow.size[0], currentWindow.size[1], 0, 0),
      projection(0));

  float* z_ShadowBuffer = new float[windowArea];
  for (int i = 0; i < windowArea; i++)
    z_ShadowBuffer[i] = std::numeric_limits<int>::min();

  TGAImage* z_shadedBuffer = new TGAImage(
      currentWindow.size[0], currentWindow.size[1], TGAImage::RGBA);

  for (int i = 0; i < model->nfaces(); i++) {
    std::vector<int> faceID = model->face(i);
    Vec3f screen_coords[3];

    for (int j = 0; j < 3; j++) {
      screen_coords[j] = shader2.vertex(model->vert(faceID[j]));
      shader2.ndc_tri.setCol(screen_coords[j], j);
    }

    drawTriangle(screen_coords, z_ShadowBuffer, z_shadedBuffer, shader2,
                 Vec2f(currentWindow.size[0], currentWindow.size[1]));
  }

  bufferToRender(currentWindow.renderer, z_shadedBuffer);
  delete z_shadedBuffer;
}
