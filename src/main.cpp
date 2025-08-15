#include <iostream>

#include "gl.h"
#include "gl_wrapper.hpp"
#include "l_matrix.h"
#include "model.h"
#include "s_WindowManager.h"

const int HEIGHT = 700;
const int WIDTH = 700;
const int DEPTH = 255;

Model* model = NULL;
Vec3f lighteye(1, 1, 1);
Vec3f eye(0, 0, 3);
Vec3f center(0, 0, 0);

zBufferShader shader2 =
    zBufferShader(lookat(center, lighteye, Vec3f(0, 1, 0)),
                  viewport(WIDTH, HEIGHT, 0, 0), projection(0));

int main(int argc, char** argv) {
  if (2 == argc)
    model = new Model(argv[1]);
  else
    model = new Model("obj/african_head.obj");

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

      running = false;
    }

    presentWindow(mainWindow);
  }

  deleteWindow(mainWindow);

  delete model;
  return 0;
}
