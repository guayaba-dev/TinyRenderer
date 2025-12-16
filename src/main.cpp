#include "../.dependencies/entt/entt.hpp"
#include "gl.h"
#include "gl_wrapper.hpp"
#include "l_matrix.h"
#include "l_scene.h"
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

  Scene scene1 = Scene();

  auto entity = scene1.getRegistry().create();

  scene1.getRegistry().emplace<Transform>(
      entity, Vec3f(0, 0, 0), Vec3f(0, 0, 0), matrix<4, 4>().identity());

  scene1.getRegistry().emplace<mesh>(entity, model);

  auto camera_entt = scene1.getRegistry().create();

  scene1.getRegistry().emplace<camera>(camera_entt, vec<3>(0., 1., 0.),
                                       vec<3>(1, 1, 1), Vec3f(0, 0, 0));

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
