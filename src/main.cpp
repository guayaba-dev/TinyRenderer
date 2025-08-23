#include "gl.h"
#include "gl_wrapper.hpp"
#include "model.h"
#include "s_WindowManager.h"

int main(int argc, char** argv) {
  Model* model = NULL;
  {  // LOADS MODEL
    if (2 == argc)
      model = new Model(argv[1]);
    else
      model = new Model("obj/african_head.obj");
  }

  Window mainWindow = createWindow();
  setWindow(mainWindow);

  drawModel(model);

  SDL_SetRenderTarget(mainWindow.renderer, NULL);

  bool running = true;
  SDL_Event event;

  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) running = false;

      if (event.type != SDL_KEYDOWN) continue;
      if (event.key.keysym.sym != SDLK_ESCAPE) continue;
    }

    presentWindow(mainWindow);
  }

  deleteWindow(mainWindow);
  delete model;
  return 0;
}
