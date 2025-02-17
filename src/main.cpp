#include <SDL2/SDL_render.h>

#include <algorithm>
#include <iostream>
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
  Vec2f varying_texture_coords[3];
  float intensity[3];
  Vec3f normals[3];

  // Model view matrix
  Matrix uniform_MV = Matrix::identity(4);

  // Model view inverse traspose matrix
  Matrix uniform_MVIT = Matrix::identity(4);

  virtual Vec3f vertex(int face, int idVert) override {
    Vec3f v = model->vert(model->face(face)[idVert]);

    v = Vec3f(ViewPort * Projection * ModelView * Matrix(v));

    intensity[idVert] =
        lightDirection *
        model->vertexNomal(model->vertexNomalsIds(face)[idVert]);

    normals[idVert] = model->vertexNomal(model->vertexNomalsIds(face)[idVert]);

    varying_texture_coords[idVert] =
        model->textCoord(model->texture(face)[idVert]);

    return v;
  }

  virtual bool fragment(Vec3f bar, TGAColor& color) override {
    Vec2f text_Coord = Vec2f(0, 0);
    Vec3f normal = Vec3f(0, 0, 0);

    for (int i = 0; i < 3; i++) {
      text_Coord = text_Coord + varying_texture_coords[i] * bar[i];
    }

    for (int i = 0; i < 3; i++) {
      normal = normal + normals[i] * bar[i];
    }

    float intensityBar = 0.f;

    for (int i = 0; i < 3; i++) {
      intensityBar = intensityBar + intensity[i] * bar[i];
    }

    TGAColor textureColor = model->getDiffuse(text_Coord);

    Matrix normalMatrix = Matrix(normal);
    Matrix lightDirMatrix = Matrix(lightDirection);
    Vec3f l = Vec3f(uniform_MV * lightDirMatrix);
    Vec3f n = Vec3f(uniform_MVIT * normalMatrix);
    Vec3f r = (n * (n * l * 2.f) - l).normalize();  // reflected light

    float spec = pow(std::max(r.z, 0.0f), 10);

    TGAColor shadedColor;

    for (int i = 0; i < 3; i++)
      shadedColor[i] = std::max(
          0.f, std::min<float>(5 + textureColor[i] * (intensityBar + .6 * spec),
                               255));

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
    shader.uniform_MVIT = ModelView.inverse().transpose();

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
