#include <SDL2/SDL_render.h>

#include <iostream>
#include <utility>

#include "SDL2/SDL.h"

struct Color {
  int r;
  int g;
  int b;
  Color(const int& r, const int& g, const int& b) : r(r), g(g), b(b) {};
};

void line(int x0, int y0, int x1, int y1, SDL_Renderer* renderer, Color color);

int main() {
  SDL_Window* window = nullptr;
  SDL_Renderer* renderer = nullptr;

  SDL_Init(SDL_INIT_VIDEO);
  SDL_CreateWindowAndRenderer(100, 100, 0, &window, &renderer);

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  line(13, 20, 80, 40, renderer, Color(255, 255, 255));
  line(20, 13, 40, 80, renderer, Color(255, 0, 0));
  line(80, 40, 13, 20, renderer, Color(255, 0, 0));

  SDL_RenderPresent(renderer);
  SDL_Delay(5000);

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
      SDL_RenderDrawPoint(renderer, y, 100 - x);
    else
      SDL_RenderDrawPoint(renderer, x, 100 - y);

    error2 += derror2;

    if (error2 > dx) {
      y = (y1 > y0) ? y + 1 : y - 1;
      error2 -= dx * 2.;
    }
  }
}
