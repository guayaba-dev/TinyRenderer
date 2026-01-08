#include "gl.h"

#include <SDL2/SDL_render.h>

#include <iostream>

#include "l_matrix.h"
#include "tgaimage.h"

matrix<4, 4> ModelView;
matrix<4, 4> ViewPort;
matrix<4, 4> Projection;

IShader::~IShader() {}

Vec4f getBarycentric(Vec3f vertex[], Vec3f point) {
  Vec3f x_vertex =
      Vec3f(vertex[1][0] - vertex[0][0], vertex[2][0] - vertex[0][0],
            vertex[0][0] - (int)point[0]);
  Vec3f y_vertex =
      Vec3f(vertex[1][1] - vertex[0][1], vertex[2][1] - vertex[0][1],
            vertex[0][1] - (int)point[1]);

  Vec3f u = crossProduct(x_vertex, y_vertex);

  if (abs(u[2]) < 1)
    return Vec4f(-1, 1, 1, 0);

  return Vec4f(1 - (u[0] + u[1]) / u[2], u[0] / u[2], u[1] / u[2], 0.f);
}

void drawTriangle(Vec3f points[], float z_buffer[], TGAImage *buffer,
                  IShader &shader, Vec2f windowDimensions) {
  Vec2f bboxmin(windowDimensions);
  Vec2f bboxmax(0, 0);
  Vec2f clamp(windowDimensions);

  for (int i = 0; i < 3; i++) {
    bboxmin[0] = std::max(0., std::min(bboxmin[0], points[i][0]));
    bboxmin[1] = std::max(0., std::min(bboxmin[1], points[i][1]));

    bboxmax[0] = std::min(clamp[0], std::max(bboxmax[0], points[i][0]));
    bboxmax[1] = std::min(clamp[1], std::max(bboxmax[1], points[i][1]));
  }

  Vec3f P;
  for (P[1] = (int)bboxmin[1]; P[1] < bboxmax[1]; P[1]++) {
    for (P[0] = (int)bboxmin[0]; P[0] < bboxmax[0]; P[0]++) {
      Vec4f barycentric = getBarycentric(points, P);

      if (barycentric[0] < 0. || barycentric[1] < 0. || barycentric[2] < 0.)
        continue; // out of triangleBounds

      P[2] = 0;

      // z coords aproximation
      for (int i = 0; i < 3; i++)
        P[2] = P[2] + (points[i][2] * barycentric[i]);

      if (P[2] < z_buffer[int(P[0] + P[1] * windowDimensions[0])])
        continue;

      z_buffer[int(P[0] + P[1] * windowDimensions[0])] = P[2];

      TGAColor shadedColor;

      if (shader.fragment(barycentric, shadedColor))
        continue;

      buffer->set(P[0], P[1], shadedColor);
    }
  }
}

void bufferToRender(SDL_Renderer *renderer, TGAImage *buffer) {
  buffer->flip_vertically();

  for (int j = 0; j < buffer->get_height(); j++) {
    for (int i = 0; i < buffer->get_width(); i++) {
      TGAColor pixelColor = buffer->get(i, j);

      SDL_SetRenderDrawColor(renderer, pixelColor[2], pixelColor[1],
                             pixelColor[0], 255);
      SDL_RenderDrawPoint(renderer, i, j);
    }
  }
}

void lookat(Vec3f center, Vec3f eye, Vec3f up) {
  Vec3f z = normalize(eye - center);
  Vec3f x = normalize(crossProduct(z, up));
  Vec3f y = normalize(crossProduct(x, z));

  matrix<4, 4> Minv = matrix<4, 4>::identity();
  matrix<4, 4> Traslation = matrix<4, 4>::identity();

  for (int i = 0; i < 3; i++) {
    Minv(0, i) = x[i];
    Minv(1, i) = y[i];
    Minv(2, i) = z[i];
    Traslation(i, 3) = -center[i];
  }

  ModelView = Minv * Traslation;
  std::cerr << "------------\n ModelView\n";
  printMath(ModelView);
}

void viewport(int w, int h, int x, int y) {
  matrix<4, 4> result = matrix<4, 4>::identity();

  result(0, 3) = x + w / 2.f;
  result(1, 3) = y + h / 2.f;
  result(2, 3) = 255.f / 2.f;

  result(0, 0) = w / 2.f;
  result(1, 1) = h / 2.f;
  result(2, 2) = 255.f / 2.f;

  ViewPort = result;

  std::cerr << "------------\n ViewPort\n";
  printMath(ViewPort);
}

void projection(float coeff) {
  Projection = matrix<4, 4>::identity();
  Projection(3, 2) = coeff;

  std::cerr << "------------\n Projection\n";
  printMath(Projection);

} // coeff = -1/c
