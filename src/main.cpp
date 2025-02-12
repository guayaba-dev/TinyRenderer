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
Vec3f lightDirection = Vec3f(1, -1, 1).normalize();
Vec3f eye(1, 1, -3);
Vec3f center(0, 0, 0);

struct TexturingShader : public IShader {
  TGAImage texture;
  Vec2f texture_coords[3];
  float intensity[3];

  virtual Vec3f vertex(int face, int idVert) override {
    Vec3f v = model->vert(model->face(face)[idVert]);
    Matrix a(v);
    v = Vec3f(ViewPort * Projection * ModelView * a);
    intensity[idVert] =
        lightDirection *
        model->vertexNomal(model->vertexNomalsIds(face)[idVert]);
    texture_coords[idVert] = model->textCoord(model->texture(face)[idVert]);
    return v;
  }

  virtual bool fragment(Vec3f bar, TGAColor& color) override {
    Vec2f text_Coord = Vec2f(0, 0);
    for (int i = 0; i < 3; i++) {
      text_Coord = text_Coord + texture_coords[i] * bar[i];
    }
    TGAColor textureColor = texture.get(text_Coord.x * texture.get_width(),
                                        text_Coord.y * texture.get_height());
    float intensityBar = 0;
    for (int i = 0; i < 3; i++) {
      intensityBar = intensityBar + intensity[i] * bar[i];
    }
    TGAColor shadedColor = textureColor * intensityBar;
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
    shader.texture.read_tga_file("texture/african_head_diffuse.tga");
    shader.texture.flip_vertically();
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
