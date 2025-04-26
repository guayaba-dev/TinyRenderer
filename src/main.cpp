#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>

#include <algorithm>
#include <cassert>
#include <cstddef>

#include "SDL2/SDL.h"
#include "geometry.h"
#include "gl.h"
#include "model.h"
#include "tgaimage.h"

int samples = 0;

const int HEIGHT = 700;
const int WIDTH = 700;
const int DEPTH = 255;
Model* model = NULL;
float* z_buffer = NULL;
Vec3f lightDirection = Vec3f(1., 1., -1);  // light
Vec3f eye(1, 1, 3);
Vec3f center(0, 0, 0);

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* canvas = nullptr;

struct TexturingShader : public IShader {
  Matrix varying_uv = Matrix(4, 4);   // uv coords
  Matrix varying_tri = Matrix(4, 4);  // triangle ModelView
  Matrix varying_nrm = Matrix(4, 4);  // normal per vertex
  Matrix ndc_tri = Matrix(4, 4);      // triangle in device coordenates

  Matrix uniform_MV = Matrix(4, 4);    // Model view matrix
  Matrix uniform_MVIT = Matrix(4, 4);  // ModelView inverse traspose

  virtual Vec3f vertex(int face, int idVert) override {
    varying_uv.setColumn(
        idVert, Vec4f(model->textCoord(model->texture(face)[idVert]), 0.));

    Matrix nrm =
        Vec4f(model->vertexNomal(model->vertexNomalsIds(face)[idVert]), 0.);
    nrm = uniform_MVIT * nrm;
    varying_nrm.setColumn(idVert, nrm);

    Vec4f glVertex = Projection * ModelView *
                     Matrix(Vec4f(model->vert(model->face(face)[idVert]), 1));

    varying_tri.setColumn(idVert, glVertex);

    ndc_tri.setColumn(idVert, glVertex.hogenize());

    glVertex = ViewPort * glVertex;

    return glVertex.hogenize().xyz();
  }

  virtual bool fragment(Vec4f bar, TGAColor& color) override {
    Vec4f normalBar = (varying_nrm * Matrix(bar));
    Vec4f uvBar = varying_uv * Matrix(bar);

    Matrix A = Matrix::identity(3);

    A.setColumn(0, ndc_tri.getColumn(1) - ndc_tri.getColumn(0));
    A.setColumn(1, ndc_tri.getColumn(2) - ndc_tri.getColumn(0));
    A.setColumn(2, normalBar.normalize());
    A = A.transpose();

    Matrix AI(3, 3);
    A.inverse(AI);

    Vec4f i = AI * Matrix(Vec3f(varying_uv(0, 1) - varying_uv(0, 0),
                                varying_uv(0, 2) - varying_uv(0, 0), 0.));

    Vec4f j = AI * Matrix(Vec3f(varying_uv(1, 1) - varying_uv(1, 0),
                                varying_uv(1, 2) - varying_uv(1, 0), 0.));

    Vec4f ud = Vec4f(varying_uv(0, 1) - varying_uv(0, 0),
                     varying_uv(0, 2) - varying_uv(0, 0), 0., 0.);
    Vec4f vd = Vec4f(varying_uv(1, 1) - varying_uv(1, 0),
                     varying_uv(1, 2) - varying_uv(1, 0), 0., 0.);

    Matrix BTN = Matrix(4, 4);

    BTN.setColumn(0, i.normalize());
    BTN.setColumn(1, j.normalize());
    BTN.setColumn(2, normalBar);

    Vec3f normalMapped =
        (BTN * (Vec4f(model->getNormal(uvBar.xy()), 0)).normalize())
            .getColumn(0)
            .xyz();

    float lightIntensity = std::max((normalMapped * lightDirection), 0.f);

    color = model->getDiffuse(uvBar.xy()) * lightIntensity;

    return false;
  }
};

/*
struct TexturingShader : public IShader {
  Vec3f varying_intensity;

  virtual Vec3f vertex(int face, int idVert) override {
    varying_intensity[idVert] =
        std::max(0.f, model->vertexNomal(model->vertexNomalsIds(face)[idVert]) *
                          lightDirection);  // get diffuse lighting intensity

    Vec4f gl_Vertex = ViewPort * Projection * ModelView *
                      Vec4f(model->vert(model->face(face)[idVert]), 1.);

    return gl_Vertex.hogenize().xyz();
  }

  virtual bool fragment(Vec4f bar, TGAColor& color) override {
    float intensity = Vec4f(varying_intensity, 0.f) *
                      bar;  // interpolate intensity for the current pixel
    color = TGAColor(255, 255, 255) * intensity;  // well duh
    return false;
  }
};
*/

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

  {  // window set up
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer);
    canvas = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                               SDL_TEXTUREACCESS_TARGET, WIDTH, HEIGHT);
    SDL_SetRenderTarget(renderer, canvas);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
  }

  {  // draw model Logic
    lookat(center, eye, Vec3f(0., 1., 0.));
    viewport(WIDTH, HEIGHT, 0, 0);
    projection(-1.f / (eye - center).norm());

    shader.uniform_MV = Projection * ModelView;
    shader.uniform_MVIT(4, 4);
    shader.uniform_MV.inverse(shader.uniform_MVIT);
    shader.uniform_MVIT = shader.uniform_MVIT.transpose();
    lightDirection =
        Vec4f(Projection * ModelView * Vec4f(lightDirection, 0.)).xyz();

    for (int i = 0; i < model->nfaces(); i++) {
      Vec3f screen_coords[3];
      for (int j = 0; j < 3; j++) {
        screen_coords[j] = shader.vertex(i, j);
      }

      drawTriangle(screen_coords, z_buffer, renderer, shader,
                   Vec2f(WIDTH, HEIGHT));
    }
  }

  SDL_SetRenderTarget(renderer, NULL);

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

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, canvas, NULL, NULL);
    SDL_RenderPresent(renderer);
    SDL_Delay(16);
  }

  SDL_DestroyTexture(canvas);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  delete model;
  delete[] z_buffer;
  return 0;
}
