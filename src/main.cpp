#include <vector>

#include "gl.h"
#include "l_matrix.h"
#include "model.h"
#include "s_WindowManager.h"
#include "tgaimage.h"

const int HEIGHT = 700;
const int WIDTH = 700;
const int DEPTH = 255;

Model* model = NULL;
float* z_buffer = NULL;
float* z_ShadowBuffer = NULL;
Vec3f lightDirection = Vec3f(1., 1., 1);  // light
Vec3f lighteye(1, 1, 1);
Vec3f eye(0, 0, 3);
Vec3f center(0, 0, 0);

zBufferShader shader2 = zBufferShader();

int main(int argc, char** argv) {
  if (2 == argc) {
    model = new Model(argv[1]);
  } else {
    model = new Model("obj/african_head.obj");
  }

  z_buffer = new float[WIDTH * HEIGHT];
  for (int i = 0; i < WIDTH * HEIGHT; i++) {
    z_buffer[i] = std::numeric_limits<int>::min();
  }

  z_ShadowBuffer = new float[WIDTH * HEIGHT];
  for (int i = 0; i < WIDTH * HEIGHT; i++) {
    z_buffer[i] = std::numeric_limits<int>::min();
  }

  Window mainWindow = createWindow();

  {  // draw model Logic
    lookat(center, lighteye, Vec3f(0., 1., 0.));
    viewport(WIDTH, HEIGHT, 0, 0);
    projection(0);

    // temp
    shader2.Projection = Projection;
    shader2.ModelView = ModelView;
    shader2.ViewPort = ViewPort;

    TGAImage* z_shadedBuffer = new TGAImage(WIDTH, HEIGHT, TGAImage::RGBA);

    for (int i = 0; i < model->nfaces(); i++) {
      std::vector<int> faceID = model->face(i);
      Vec3f screen_coords[3];

      for (int j = 0; j < 3; j++) {
        screen_coords[j] = shader2.vertex(model->vert(faceID[j]));
        shader2.ndc_tri.setCol(screen_coords[j], j);
      }

      drawTriangle(screen_coords, z_ShadowBuffer, z_shadedBuffer, shader2,
                   Vec2f(WIDTH, HEIGHT));
    }

    bufferToRender(mainWindow.renderer, z_shadedBuffer);
    delete z_shadedBuffer;
  }

  SDL_SetRenderTarget(mainWindow.renderer, NULL);

  bool running = true;
  SDL_Event event;

  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      }

      if (event.type != SDL_KEYDOWN) continue;
      if (event.key.keysym.sym != SDLK_ESCAPE) continue;

      running = false;
    }

    presentWindow(mainWindow);
  }

  deleteWindow(mainWindow);

  delete model;
  delete[] z_buffer;
  delete[] z_ShadowBuffer;
  return 0;
}
