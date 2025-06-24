#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>

#include <iostream>

#include "gl.h"
#include "l_matrix.h"
#include "model.h"
#include "tgaimage.h"

const int HEIGHT = 700;
const int WIDTH = 700;
const int DEPTH = 255;

Model* model = NULL;
float* z_buffer = NULL;
float* z_ShadowBuffer = NULL;
Vec3f lightDirection = Vec3f(1., 1., 1);  // light
Vec3f lighteye(1, 1, 1);
Vec3f eye(0, 0, 5);
Vec3f center(0, 0, 0);

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* canvas = nullptr;
struct TexturingShader : public IShader {
  matrix<2, 3> varying_uv = matrix<2, 3>();            // uv coords
  matrix<4, 4> varying_tri = matrix<4, 4>();           // triangle ModelView
  matrix<3, 3> varying_nrm = matrix<3, 3>();           // normal per vertex
  matrix<4, 4> varying_shadow_depth = matrix<4, 4>();  // DEPTH per vertex
  matrix<3, 3> ndc_tri = matrix<3, 3>();       // triangle in device coordenates
  matrix<4, 4> uniform_LMV = matrix<4, 4>();   // Matrix to lightPos
  matrix<4, 4> uniform_MV = matrix<4, 4>();    // Model view matrix
  matrix<4, 4> uniform_MVIT = matrix<4, 4>();  // ModelView inverse traspose
  TGAImage uniform_shadowMap;

  virtual Vec3f vertex(int face, int idVert) override {
    varying_uv.setCol(model->textCoord(model->texture(face)[idVert]), idVert);

    Vec4f nrm =
        (uniform_MVIT *
         embed<3, 4>(model->vertexNomal(model->vertexNomalsIds(face)[idVert])))
            .getCol(0);

    varying_nrm.setCol(proj<4, 3>(nrm), idVert);
    Vec4f shadowVerts =
        (uniform_LMV * embed<3, 4>(model->vert(model->face(face)[idVert]), 1.))
            .getCol(0);
    varying_shadow_depth.setCol((shadowVerts / shadowVerts[2]), idVert);
    Vec4f glVertex = (Projection * ModelView *
                      embed<3, 4>(model->vert(model->face(face)[idVert]), 1.))
                         .getCol(0);
    varying_tri.setCol(glVertex, idVert);
    ndc_tri.setCol(proj<4, 3>(glVertex / glVertex[3]), idVert);
    glVertex = (ViewPort * glVertex).getCol(0);
    return proj<4, 3>(glVertex / glVertex[3]);
  }

  virtual bool fragment(Vec4f bar, TGAColor& color) override {
    Vec3f normalBar = (varying_nrm * proj<4, 3>(bar)).getCol(0);
    Vec2f uvBar = (varying_uv * proj<4, 3>(bar)).getCol(0);
    Vec4f shadowMapBar = (varying_shadow_depth * bar).getCol(0);

    int idx = int(shadowMapBar[0]) +
              int(shadowMapBar[1]) * WIDTH;  // index in the shadowbuffer array

    uniform_shadowMap.get(shadowMapBar[0], shadowMapBar[1]);

    float shadow = 0.3 + 0.7 * (z_ShadowBuffer[idx] < shadowMapBar[2] + 47.61);

    matrix<3, 3> A = matrix<3, 3>();

    A.setRow(ndc_tri.getCol(1) - ndc_tri.getCol(0), 0);
    A.setRow(ndc_tri.getCol(2) - ndc_tri.getCol(0), 1);
    A.setRow(normalize(normalBar), 2);

    matrix<3, 3> B = matrix<3, 3>();

    B.setCol(Vec3f(varying_uv(0, 1) - varying_uv(0, 0),
                   varying_uv(0, 2) - varying_uv(0, 0), 0),
             0.);
    B.setCol(Vec3f(varying_uv(1, 1) - varying_uv(1, 0),
                   varying_uv(1, 2) - varying_uv(1, 0), 0),
             0.);

    matrix<3, 3> BTN = matrix<3, 3>();

    solve(A, BTN, B);

    Vec3f normalMapped = (BTN * normalize(model->getNormal(uvBar))).getCol(0);

    float lightIntensity = std::max((normalMapped * lightDirection), 0.f);

    color = model->getDiffuse(uvBar) * lightIntensity * shadow;

    return false;
  }
};

struct zBufferShader : public IShader {
  matrix<4, 4> ndc_tri = matrix<4, 4>();

  virtual Vec3f vertex(int face, int idVert) override {
    Vec4f gl_Vertex = (ViewPort * Projection * ModelView *
                       embed<3, 4>(model->vert(model->face(face)[idVert]), 1.))
                          .getCol(0);

    ndc_tri.setCol(gl_Vertex / gl_Vertex[3], idVert);

    return proj<4, 3>(gl_Vertex / gl_Vertex[3]);
  }

  virtual bool fragment(Vec4f bar, TGAColor& color) override {
    float intensity = (ndc_tri * bar).getCol(0)[2] /
                      DEPTH;  // interpolate intensity for the current pixel

    color = TGAColor(255, 255, 255) * intensity;
    return false;
  }
};

TexturingShader shader;
zBufferShader shader2;

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
    lookat(center, lighteye, Vec3f(0., 1., 0.));
    viewport(WIDTH, HEIGHT, 0, 0);
    projection(0);

    TGAImage* z_shadedBuffer = new TGAImage(WIDTH, HEIGHT, TGAImage::RGBA);
    TGAImage* finalRender = new TGAImage(WIDTH, HEIGHT, TGAImage::RGBA);

    for (int i = 0; i < model->nfaces(); i++) {
      Vec3f screen_coords[3];
      for (int j = 0; j < 3; j++) {
        screen_coords[j] = shader2.vertex(i, j);
      }

      drawTriangle(screen_coords, z_ShadowBuffer, z_shadedBuffer, shader2,
                   Vec2f(WIDTH, HEIGHT));
    }

    /*
    shader.uniform_LMV = ViewPort * Projection * ModelView;

    lookat(center, eye, Vec3f(0., 1., 0.));
    viewport(WIDTH, HEIGHT, 0, 0);
    projection(-1.f / norm(eye - center));

    shader.uniform_MV = Projection * ModelView;
    shader.uniform_MVIT(4, 4);
    LUInverse(shader.uniform_MV, shader.uniform_MVIT);
    shader.uniform_MVIT = shader.uniform_MVIT.transpose();
    lightDirection = proj<4, 3>(
        (Projection * ModelView * embed<3, 4>(lightDirection)).getCol(0));

    for (int i = 0; i < model->nfaces(); i++) {
      Vec3f screen_coords[3];
      for (int j = 0; j < 3; j++) {
        screen_coords[j] = shader.vertex(i, j);
      }

      drawTriangle(screen_coords, z_buffer, finalRender, shader,
                   Vec2f(WIDTH, HEIGHT));
    }
    */

    bufferToRender(renderer, z_shadedBuffer);
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
