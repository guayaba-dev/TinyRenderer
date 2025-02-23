#include <SDL2/SDL_render.h>

#include <limits>

#include "SDL2/SDL.h"
#include "geometry.h"
#include "gl.h"
#include "model.h"
#include "tgaimage.h"

const int HEIGHT = 700;
const int WIDTH = 700;
const int DEPTH = 255;

Model* model = NULL;
float* z_buffer = NULL;
Vec3f lightDirection = Vec3f(1, 1, 1).normalize();
Vec3f eye(0, -1, 3);
Vec3f center(0, 0, 0);

struct TexturingShader : public IShader {
  Matrix varying_uv = Matrix::identity(4);   // uv coords
  Matrix varying_tri = Matrix::identity(4);  // triangle ModelView
  Matrix varying_nrm = Matrix::identity(4);  // normal per vertex
  Matrix ndc_tri = Matrix::identity(4);      // triangle in device coordenates

  Matrix uniform_MV = Matrix::identity(4);    // Model view matrix
  Matrix uniform_MVIT = Matrix::identity(4);  // ModelView inverse traspose

  virtual Vec3f vertex(int face, int idVert) override {
    Matrix v = ViewPort * Projection * ModelView *
               Matrix(model->vert(model->face(face)[idVert]), 1);

    Vec3f glVertex = v;

    varying_nrm.setColumn(
        idVert,
        Vec3f(uniform_MVIT *
              Matrix(model->vertexNomal(model->vertexNomalsIds(face)[idVert]),
                     0.)));

    varying_uv.setColumn(idVert,
                         model->textCoord(model->texture(face)[idVert]));

    ndc_tri.setColumn(idVert, Vec3f(v));

    varying_tri.setColumn(idVert, glVertex);

    return glVertex;
  }

  virtual bool fragment(Vec3f bar, TGAColor& color) override {
    Vec2f uvBar = varying_uv * Matrix(bar, 0);
    Vec3f normalBar = varying_nrm * Matrix(bar, 0);

    TGAColor textureColor = model->getDiffuse(uvBar);

    Matrix A = Matrix::identity(4);

    A.setColumn(0, ndc_tri.getColumn(2) - ndc_tri.getColumn(0));
    A.setColumn(1, ndc_tri.getColumn(1) - ndc_tri.getColumn(0));
    A.setColumn(2, normalBar);
    A = A.transpose();
    Matrix AI(4, 4);
    A.inverse(AI);

    Vec3f i = AI * Matrix(Vec3f(varying_uv(0, 1) - varying_uv(0, 0),
                                varying_uv(0, 2) - varying_uv(0, 0), 0.f),
                          0);

    Vec3f j = AI * Matrix(Vec3f(varying_uv(1, 1) - varying_uv(1, 0),
                                varying_uv(1, 2) - varying_uv(1, 0), 0.f),
                          0);

    Matrix BTN = Matrix::identity(4);

    BTN.setColumn(0, i.normalize());
    BTN.setColumn(1, j.normalize());
    BTN.setColumn(2, normalBar);

    Vec3f normalMapped = (BTN * Matrix(model->getNormal(uvBar), 0));
    TGAColor shadedColor = textureColor * (normalMapped * lightDirection);
    color = shadedColor;
    return false;
  }
};

TexturingShader shader;

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

  SDL_Window* window = nullptr;
  SDL_Renderer* renderer = nullptr;

  {  // window set up
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
  }

  {  // draw model Logic
    lookat(center, eye, Vec3f(0., 1., 0.));
    viewport(WIDTH, HEIGHT, 0, 0);
    projection(-1.f / (eye - center).norm());

    shader.uniform_MV = ModelView;
    shader.uniform_MV.output();
    shader.uniform_MVIT(4, 4);
    ModelView.inverse(shader.uniform_MVIT);
    shader.uniform_MVIT.transpose();
    shader.uniform_MVIT.output();

    for (int i = 0; i < model->nfaces(); i++) {
      Vec3f screen_coords[3];
      for (int j = 0; j < 3; j++) {
        screen_coords[j] = shader.vertex(i, j);
      }

      drawTriangle(screen_coords, z_buffer, renderer, shader,
                   Vec2f(WIDTH, HEIGHT));
    }
  }

  bool running = true;
  SDL_Event event;

  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      } else if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_ESCAPE) {
          running = false;
        }
      }
    }

    SDL_RenderPresent(renderer);
    SDL_Delay(16);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  delete model;
  delete[] z_buffer;
  return 0;
}
