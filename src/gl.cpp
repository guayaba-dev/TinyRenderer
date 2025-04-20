#include "gl.h"

#include "geometry.h"
#include "tgaimage.h"

Matrix ModelView;
Matrix ViewPort;
Matrix Projection;

IShader::~IShader() {}

Vec4f getBarycentric(Vec3f vertex[], Vec3i point) {
  Vec3f x_vertex = Vec3f(vertex[1].x - vertex[0].x, vertex[2].x - vertex[0].x,
                         vertex[0].x - point.x);
  Vec3f y_vertex = Vec3f(vertex[1].y - vertex[0].y, vertex[2].y - vertex[0].y,
                         vertex[0].y - point.y);

  Vec3f u = x_vertex ^ y_vertex;

  if (abs(u.z) < 1) return Vec4f(-1, 1, 1, 0);

  return Vec4f(1 - (u.x / u.z + u.y / u.z), u.x / u.z, u.y / u.z, 0.f);
}

void drawTriangle(Vec3f points[], float z_buffer[], SDL_Renderer* renderer,
                  IShader& shader, Vec2f windowDimensions) {
  Vec2f bboxmin(windowDimensions);
  Vec2f bboxmax(0, 0);
  Vec2f clamp(windowDimensions);

  for (int i = 0; i < 3; i++) {
    bboxmin.x = std::max(0.f, std::min(bboxmin.x, points[i].x));
    bboxmin.y = std::max(0.f, std::min(bboxmin.y, points[i].y));

    bboxmax.x = std::min(clamp.x, std::max(bboxmax.x, points[i].x));
    bboxmax.y = std::min(clamp.y, std::max(bboxmax.y, points[i].y));
  }

  Vec3i P;
  for (P.y = bboxmin.y; P.y < bboxmax.y; P.y++) {
    for (P.x = bboxmin.x; P.x < bboxmax.x; P.x++) {
      Vec4f barycentric = getBarycentric(points, P);

      if (barycentric.x < 0. || barycentric.y < 0. || barycentric.z < 0.)
        continue;  // out of triangleBounds

      P.z = 0;

      // z coords aproximation
      for (int i = 0; i < 3; i++) {
        P.z = P.z + points[i].z * barycentric[i];
      }

      if (z_buffer[P.x + P.y * (int)windowDimensions.x] < P.z) {
        z_buffer[P.x + P.y * (int)windowDimensions.x] = P.z;

        TGAColor shadedColor;

        if (shader.fragment(barycentric, shadedColor)) {
          continue;
        }

        SDL_SetRenderDrawColor(renderer, shadedColor[2], shadedColor[1],
                               shadedColor[0], 100);

        SDL_RenderDrawPoint(renderer, windowDimensions.x - P.x,
                            windowDimensions.y - P.y);
      }
    }
  }
}

void lookat(Vec3f center, Vec3f eye, Vec3f up) {
  Vec3f z = (eye - center).normalize();
  Vec3f x = (z ^ up).normalize();
  Vec3f y = (x ^ z).normalize();

  Matrix Minv = Matrix::identity(4);
  Matrix Traslation = Matrix::identity(4);

  for (int i = 0; i < 3; i++) {
    Minv(0, i) = x[i];
    Minv(1, i) = y[i];
    Minv(2, i) = z[i];
    Traslation(i, 3) = -center[i];
  }

  ModelView = Minv * Traslation;
}

void viewport(int w, int h, int x, int y) {
  Matrix result = Matrix::identity(4);

  result(0, 3) = x + w / 2.f;
  result(1, 3) = y + h / 2.f;
  result(2, 3) = 255.f / 2.f;

  result(0, 0) = w / 2.f;
  result(1, 1) = h / 2.f;
  result(2, 2) = 255.f / 2.f;

  ViewPort = result;
}

void projection(float coeff) {
  Projection = Matrix::identity(4);
  Projection(3, 2) = coeff;
}  // coeff = -1/c
