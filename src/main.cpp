#include <cstddef>
#include <iostream>
#include <limits>

#include "SDL2/SDL.h"
#include "geometry.h"
#include "model.h"
#include "tgaimage.h"

struct Color {
  int r;
  int g;
  int b;
  Color(const float& r, const float& g, const float& b) : r(r), g(g), b(b) {};
};

const int HEIGHT = 700;
const int WIDTH = 700;
const int DEPTH = 255;

Model* model = NULL;
float* z_buffer = NULL;
TGAImage texture = TGAImage();
Vec3f lightDirection = Vec3f(1, -1, 1).normalize();
Vec3f eye(1, 1, 3);
Vec3f center(0, 0, 0);

Vec3f getBarycentric(Vec3i vertex[], Vec3i point) {
  Vec3f x_vertex = Vec3f(vertex[1].x - vertex[0].x, vertex[2].x - vertex[0].x,
                         vertex[0].x - point.x);
  Vec3f y_vertex = Vec3f(vertex[1].y - vertex[0].y, vertex[2].y - vertex[0].y,
                         vertex[0].y - point.y);

  Vec3f u = x_vertex ^ y_vertex;
  if (abs(u.z) < 1) return Vec3f(-1, 1, 1);

  return Vec3f(1 - (u.x / u.z + u.y / u.z), u.x / u.z, u.y / u.z);
}

void drawTriangle(Vec3i points[], Vec2f texture_coords[], float z_buffer[],
                  SDL_Renderer* renderer, float intensities[]) {
  Vec2i bboxmin(WIDTH, HEIGHT);
  Vec2i bboxmax(0, 0);
  Vec2i clamp(WIDTH, HEIGHT);

  for (int i = 0; i < 3; i++) {
    bboxmin.x = std::max(0, std::min(bboxmin.x, points[i].x));
    bboxmin.y = std::max(0, std::min(bboxmin.y, points[i].y));

    bboxmax.x = std::min(clamp.x, std::max(bboxmax.x, points[i].x));
    bboxmax.y = std::min(clamp.y, std::max(bboxmax.y, points[i].y));
  }

  Vec3i P;
  for (P.y = bboxmin.y; P.y < bboxmax.y; P.y++) {
    for (P.x = bboxmin.x; P.x < bboxmax.x; P.x++) {
      Vec3f barycentric = getBarycentric(points, P);

      if (barycentric.x < 0. || barycentric.y < 0. || barycentric.z < 0.)
        continue;  // out of triangleBounds

      P.z = 0;

      // z coords aproximation
      for (int i = 0; i < 3; i++) {
        P.z = P.z + points[i].z * barycentric[i];
      }

      if (z_buffer[P.x + P.y * WIDTH] < P.z) {
        z_buffer[P.x + P.y * WIDTH] = P.z;

        Vec2f text_Coord = Vec2f(0, 0);

        for (int i = 0; i < 3; i++) {
          text_Coord = text_Coord + texture_coords[i] * barycentric[i];
        }

        TGAColor textureColor =
            texture.get(text_Coord.x * texture.get_width(),
                        text_Coord.y * texture.get_height());

        float intensity = 0;

        for (int i = 0; i < 3; i++) {
          intensity = intensity + intensities[i] * barycentric[i];
        }

        TGAColor shadedColor = textureColor * intensity;

        SDL_SetRenderDrawColor(renderer, shadedColor[2], shadedColor[1],
                               shadedColor[0], 1);

        SDL_RenderDrawPoint(renderer, P.x, HEIGHT - P.y);
      }
    }
  }
}

Matrix lookAt(Vec3f center, Vec3f eye, Vec3f up) {
  Vec3f z = (eye - center).normalize();
  Vec3f x = (z ^ up).normalize();
  Vec3f y = (x ^ z).normalize();

  Matrix Minv = Matrix::identity(4);
  Matrix Traslation = Matrix::identity(4);

  for (int i = 0; i < 3; i++) {
    Minv[0][i] = x[i];
    Minv[1][i] = y[i];
    Minv[2][i] = z[i];
    Traslation[i][3] = -center[i];
  }

  return Minv * Traslation;
}

int main(int argc, char** argv) {
  if (2 == argc) {
    model = new Model(argv[1]);
  } else {
    model = new Model("obj/african_head.obj");
    texture.read_tga_file("texture/african_head_diffuse.tga");
    texture.flip_vertically();
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
    Matrix Projection = Matrix::identity(4);
    Matrix ModelView = lookAt(center, eye, Vec3f(0., 1., 0.));
    Projection[3][2] = -1 / (center - eye).norm();

    for (int i = 0; i < model->nfaces(); i++) {
      std::vector<int> face = model->face(i);
      std::vector<int> texture = model->texture(i);
      std::vector<int> vertexNormalsId = model->vertexNomalsIds(i);

      Vec3i screen_coords[3];
      Vec2f texture_coords[3];
      float intensity[3];
      Vec3f world_coords[3];

      for (int j = 0; j < 3; j++) {
        Vec3f v = model->vert(face[j]);

        Matrix a(v);

        a = Projection * ModelView * a;

        v = Vec3f(a);

        screen_coords[j] =
            Vec3i((v.x + 1.) * WIDTH / 2., (v.y + 1.) * HEIGHT / 2.,
                  (v.z + 1.) * DEPTH / 2.);

        intensity[j] = lightDirection * model->vertexNomal(vertexNormalsId[j]);
        texture_coords[j] = model->textCoord(texture[j]);
      }

      drawTriangle(screen_coords, texture_coords, z_buffer, renderer,
                   intensity);
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

  SDL_DestroyWindow(window);
  SDL_Quit();

  delete model;
  delete[] z_buffer;
  return 0;
}
