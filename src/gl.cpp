#include "gl.h"

#include <SDL2/SDL_render.h>

#include <cstdio>
#include <iostream>

#include "l_matrix.h"
#include "tgaimage.h"

const TGAColor ERR_COLOR = TGAColor(82, 0, 255);  //"#e30052"

IShader::~IShader() {}

Vec4f getBarycentric(Vec3f vertex[], Vec3f point) {
  // Obteins the x and y vectors respectively for bar calculus
  Vec3f x_vertex =
      Vec3f(vertex[1][vX] - vertex[0][vX], vertex[2][vX] - vertex[0][vX],
            vertex[0][vX] - (int)point[vX]);
  Vec3f y_vertex =
      Vec3f(vertex[1][vY] - vertex[0][vY], vertex[2][vY] - vertex[0][vY],
            vertex[0][vY] - (int)point[vY]);

  Vec3f u = crossProduct(x_vertex, y_vertex);

  if (abs(u[vZ]) < 1) return Vec4f(-1, 1, 1, 0);

  return Vec4f(1 - (u[vX] + u[vY]) / u[vZ], u[vX] / u[vZ], u[vY] / u[vZ], 0.f);
}

void drawTriangle(Vec3f points[], float z_buffer[], TGAImage* buffer,
                  IShader& shader, Vec2f windowDimensions) {
  Vec2f bboxmin(windowDimensions);
  Vec2f bboxmax(0, 0);
  Vec2f clamp(windowDimensions);

  for (int i = 0; i < 3; i++) {
    bboxmin[vX] = std::max(0., std::min(bboxmin[vX], points[i][vX]));
    bboxmin[vY] = std::max(0., std::min(bboxmin[vY], points[i][vY]));

    bboxmax[vX] = std::min(clamp[vX], std::max(bboxmax[vX], points[i][vX]));
    bboxmax[vY] = std::min(clamp[vY], std::max(bboxmax[vY], points[i][vY]));
  }

  Vec3f P;
  for (P[vY] = (int)bboxmin[vY]; P[vY] < bboxmax[vY]; P[vY]++) {
    for (P[vX] = (int)bboxmin[vX]; P[vX] < bboxmax[vX]; P[vX]++) {
      Vec4f barycentric = getBarycentric(points, P);

      if (barycentric[vX] < 0. || barycentric[vY] < 0. || barycentric[vZ] < 0.)
        continue;  // out of triangleBounds

      // z coords aproximation
      P[vZ] = 0;
      for (int i = 0; i < 3; i++)
        P[vZ] = P[vZ] + (points[i][vZ] * barycentric[i]);

      // buffer check
      if (P[vZ] < z_buffer[int(P[vX] + P[vY] * windowDimensions[vX])]) continue;

      z_buffer[int(P[vX] + P[vY] * windowDimensions[vX])] = P[vZ];

      TGAColor shadedColor;

      if (shader.fragment(barycentric, shadedColor)) continue;

      buffer->set(P[vX], P[vY], shadedColor);
    }
  }
}

void bufferToRender(SDL_Renderer* renderer, TGAImage* buffer) {
  buffer->flip_vertically();

  for (int j = 0; j < buffer->get_height(); j++)
    for (int i = 0; i < buffer->get_width(); i++) {
      TGAColor pixelColor = buffer->get(i, j);

      SDL_SetRenderDrawColor(renderer, pixelColor[vZ], pixelColor[vY],
                             pixelColor[vX], 255);
      SDL_RenderDrawPoint(renderer, i, j);
    }
}

matrix<4, 4> lookat(Vec3f center, Vec3f eye, Vec3f up) {
  Vec3f z = normalize(eye - center);
  Vec3f x = normalize(crossProduct(z, up));
  Vec3f y = normalize(crossProduct(x, z));

  matrix<4, 4> Minv = matrix<4, 4>::identity();
  matrix<4, 4> Traslation = matrix<4, 4>::identity();
  matrix<4, 4> result;

  for (int i = 0; i < 3; i++) {
    Minv(0, i) = x[i];
    Minv(1, i) = y[i];
    Minv(2, i) = z[i];
    Traslation(i, 3) = -center[i];
  }

  result = Minv * Traslation;
  std::cerr << "------------\n ModelView\n";
  return result;
}

matrix<4, 4> viewport(int w, int h, int x, int y) {
  matrix<4, 4> result = matrix<4, 4>::identity();

  result(0, 3) = x + w / 2.f;
  result(1, 3) = y + h / 2.f;
  result(2, 3) = 255.f / 2.f;

  result(0, 0) = w / 2.f;
  result(1, 1) = h / 2.f;
  result(2, 2) = 255.f / 2.f;

  std::cerr << "------------\n ViewPort\n";
  printMath(result);
  return result;
}

matrix<4, 4> projection(float coeff) {
  matrix<4, 4> result;
  result(3, 2) = coeff;
  result = matrix<4, 4>::identity();

  std::cerr << "------------\n Projection\n";
  printMath(result);

  return result;
}  // coeff = -1/c
