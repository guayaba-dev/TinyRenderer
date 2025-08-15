#include "s_WindowManager.h"

// TODO: DEFINE if config file not found
#define WIDTH 700
#define HEIGHT 700

Window createWindow() {
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_Texture* canvas;

  SDL_Init(SDL_INIT_VIDEO);
  SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer);
  canvas = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                             SDL_TEXTUREACCESS_TARGET, WIDTH, HEIGHT);
  SDL_SetRenderTarget(renderer, canvas);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  return Window{window, renderer, canvas, {WIDTH, HEIGHT}};
}

void deleteWindow(Window& window) {
  SDL_DestroyWindow(window.window);
  SDL_DestroyRenderer(window.renderer);
  SDL_DestroyTexture(window.canvas);
  SDL_Quit();
};

void presentWindow(Window& window) {
  SDL_RenderClear(window.renderer);
  SDL_RenderCopy(window.renderer, window.canvas, NULL, NULL);
  SDL_RenderPresent(window.renderer);
  SDL_Delay(16);
}
