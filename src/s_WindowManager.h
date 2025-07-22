#pragma once
#include "SDL2/SDL.h"

struct Window {
  SDL_Window* window = nullptr;
  SDL_Renderer* renderer = nullptr;
  SDL_Texture* canvas = nullptr;
};

Window createWindow();

void deleteWindow(Window& window);

void presentWindow();
