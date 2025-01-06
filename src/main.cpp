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

Model* model = NULL;
Color WHITE = Color(255, 255, 255);
const int WIDTH = 700;
const int HEIGHT = 700;

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

  SDL_RenderPresent(renderer);
  SDL_Delay(5000);

  delete model;
  return 0;
}

void line(int x0, int y0, int x1, int y1, SDL_Renderer* renderer, Color color) {
  SDL_SetRenderDrawColor(renderer, color.r, color.b, color.g, 255);

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
