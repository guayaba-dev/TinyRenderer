#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <limits>
#include <utility>

#include "SDL2/SDL.h"
#include "geometry.h"
#include "model.h"

struct Color {
  int r;
  int g;
  int b;
  Color(const float& r, const float& g, const float& b) : r(r), g(g), b(b) {};
};

void drawLine(int x0, int y0, int x1, int y1, SDL_Renderer* renderer,
              Color color);
// void drawTriangle(Vec2i t0, Vec2i t1, Vec2i t2, SDL_Renderer* renderer,
// Color color);
Vec3f getBarycentric(Vec3i vertex[], Vec3i point);
void drawTriangle(Vec3i points[], float z_buffer[], SDL_Renderer* renderer,
                  Color color);

Model* model = NULL;
Color WHITE = Color(255, 255, 255);
Color RED = Color(255, 0, 0);
Color GREEN = Color(0, 255, 0);
const int WIDTH = 400;
const int HEIGHT = 400;

int main(int argc, char** argv) {
  if (2 == argc) {
    model = new Model(argv[1]);
  } else {
    model = new Model("obj/african_head.obj");
  }

  SDL_Window* window = nullptr;
  SDL_Renderer* renderer = nullptr;

  SDL_Init(SDL_INIT_VIDEO);
  SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer);

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  Vec3f lightCoords(0, 0, -1);

  float z_buffer[WIDTH * HEIGHT];

  for (int i = 0; i < WIDTH * HEIGHT; i++) {
    z_buffer[i] = std::numeric_limits<int>::min();
  }

  for (int i = 0; i < model->nfaces(); i++) {
    std::vector<int> face = model->face(i);

    Vec3f world_coords[3];
    Vec3i screen_coords[3];
    Vec3f n;

    for (int j = 0; j < 3; j++) {
      Vec3f v = model->vert(face[j]);

      // we obtein the xy part of our world_coords
      screen_coords[j] =
          Vec3i((v.x + 1.) * WIDTH / 2., (v.y + 1.) * HEIGHT / 2., v.z);

      world_coords[j] = v;
    }

    n = (world_coords[2] - world_coords[0]) ^
        (world_coords[1] - world_coords[0]);

    n.normalize();

    float intensity = std::max(lightCoords * n, 0.f);

    Color shadedColor =
        Color(intensity * 255, intensity * 255, intensity * 255);

    if (intensity > 0.)
      drawTriangle(screen_coords, z_buffer, renderer, shadedColor);
  }

  SDL_RenderPresent(renderer);
  SDL_Delay(5000);

  delete model;
  return 0;
}

void drawLine(int x0, int y0, int x1, int y1, SDL_Renderer* renderer,
              Color color) {
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);

  bool step = 0;

  if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
    std::swap(x0, y0);
    std::swap(x1, y1);
    step = true;
  }

  if (x0 > x1) {
    std::swap(x0, x1);
    std::swap(y0, y1);
  }

  int dy = y1 - y0;
  int dx = x1 - x0;
  int y = y0;
  float derror2 = std::abs(dy) * 2;
  float error2 = 0.0;

  for (int x = x0; x <= x1; x++) {
    if (step)
      SDL_RenderDrawPoint(renderer, y, HEIGHT - x);
    else
      SDL_RenderDrawPoint(renderer, x, HEIGHT - y);

    error2 += derror2;

    if (error2 > dx) {
      y = (y1 > y0) ? y + 1 : y - 1;
      error2 -= dx * 2.;
    }
  }
}

/*
void drawTriangle(Vec2i t0, Vec2i t1, Vec2i t2, SDL_Renderer* renderer,
              Color color) {
  // vertices sorted by height lower-to-upper t2 t1 t0

  if (t0.y > t2.y) std::swap(t0, t2);
  if (t0.y > t1.y) std::swap(t0, t1);
  if (t1.y > t2.y) std::swap(t2, t1);

  int triangleHeight = t2.y - t0.y;

  for (int y = t0.y; y <= t2.y; y++) {
    bool secondHalf = y >= t1.y;
    int segmentHeight = secondHalf ? t2.y - t1.y : t1.y - t0.y;

    float alpha = (float)(y - t0.y) / triangleHeight;
    float betha = (float)(y - (secondHalf ? (t1.y) : t0.y)) / segmentHeight;

    Vec2i A = t0 + (t2 - t0) * alpha;
    Vec2i B = secondHalf ? t1 + (t2 - t1) * betha : t0 + (t1 - t0) * betha;

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawPoint(renderer, A.x, HEIGHT - y);
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderDrawPoint(renderer, B.x, HEIGHT - y);

    if (B.x > A.x) std::swap(A, B);

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);

    for (int x = B.x; x <= A.x; x++) {
      SDL_RenderDrawPoint(renderer, x, HEIGHT - y);
    }
  }
}
*/

void drawTriangle(Vec3i points[], float z_buffer[], SDL_Renderer* renderer,
                  Color color) {
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);

  Vec2i bboxmin(WIDTH, HEIGHT);
  Vec2i bboxmax(0, 0);
  Vec2i clamp(WIDTH, HEIGHT);

  for (int i = 0; i < 3; i++) {
    bboxmin.x = std::max(0, std::min(bboxmin.x, points[i].x));
    bboxmin.y = std::max(0, std::min(bboxmin.y, points[i].y));

    bboxmax.x = std::min(clamp.x, std::max(bboxmax.x, points[i].x));
    bboxmax.y = std::min(clamp.y, std::max(bboxmax.y, points[i].y));
  }

  Vec3i P;

  for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
    for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
      Vec3f barycentric = getBarycentric(points, P);

      if (barycentric.x < 0. || barycentric.y < 0. || barycentric.z < 0.)
        continue;

      P.z = 0;

      // z coords aproximation

      P.z += points[0].z * barycentric.x + points[1].z * barycentric.y +
             points[2].z * barycentric.z;

      if (z_buffer[P.x + P.y * WIDTH] < P.z) {
        z_buffer[P.x + P.y * WIDTH] = P.z;
        SDL_RenderDrawPoint(renderer, P.x, HEIGHT - P.y);
      }
    }
  }
}

Vec3f getBarycentric(Vec3i vertex[], Vec3i point) {
  Vec3f x_vertex = Vec3f(vertex[2].x - vertex[0].x, vertex[1].x - vertex[0].x,
                         vertex[0].x - point.x);
  Vec3f y_vertex = Vec3f(vertex[2].y - vertex[0].y, vertex[1].y - vertex[0].y,
                         vertex[0].y - point.y);

  Vec3f u = x_vertex ^ y_vertex;
  if (abs(u.z) < 1) return Vec3f(-1, 1, 1);

  return Vec3f(1 - (u.x / u.z + u.y / u.z), u.x / u.z, u.y / u.z);
}
