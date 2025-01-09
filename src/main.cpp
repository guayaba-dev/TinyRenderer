#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>

#include <algorithm>
#include <cstddef>
#include <utility>

#include "SDL2/SDL.h"
#include "geometry.h"
#include "model.h"

struct Color {
  int r;
  int g;
  int b;
  Color(const int& r, const int& g, const int& b) : r(r), g(g), b(b) {};
};

void line(int x0, int y0, int x1, int y1, SDL_Renderer* renderer, Color color);
void triangle(Vec2i t0, Vec2i t1, Vec2i t2, SDL_Renderer* renderer,
              Color color);

Model* model = NULL;
Color WHITE = Color(255, 255, 255);
Color RED = Color(255, 0, 0);
Color GREEN = Color(0, 255, 0);
const int WIDTH = 200;
const int HEIGHT = 200;

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

  /*
  for (int i = 0; i < model->nfaces(); i++) {
    std::vector<int> face = model->face(i);

    for (int j = 0; j < 3; j++) {
      Vec3f v0 = model->vert(face[j]);
      Vec3f v1 = model->vert(face[(j + 1) % 3]);

      int x0 = (v0.x + 1.) * WIDTH / 2.;
      int y0 = (v0.y + 1.) * HEIGHT / 2.;
      int x1 = (v1.x + 1.) * WIDTH / 2.;
      int y1 = (v1.y + 1.) * HEIGHT / 2.;
      line(x0, y0, x1, y1, renderer, WHITE);
    }
  }

  */

  Vec2i t0[3] = {Vec2i(10, 70), Vec2i(50, 160), Vec2i(70, 80)};
  Vec2i t1[3] = {Vec2i(180, 50), Vec2i(150, 1), Vec2i(70, 180)};
  Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)};

  triangle(t0[0], t0[1], t0[2], renderer, RED);
  triangle(t1[0], t1[1], t1[2], renderer, WHITE);
  triangle(t2[0], t2[1], t2[2], renderer, GREEN);

  SDL_RenderPresent(renderer);
  SDL_Delay(5000);

  delete model;
  return 0;
}

void line(int x0, int y0, int x1, int y1, SDL_Renderer* renderer, Color color) {
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

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, SDL_Renderer* renderer,
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
