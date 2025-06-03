#include <SDL2/SDL.h>

#include "l_matrix.h"
#include "tgaimage.h"

extern matrix<4, 4> Projection;
extern matrix<4, 4> ModelView;
extern matrix<4, 4> ViewPort;

struct IShader {
  virtual ~IShader();

  virtual Vec3f vertex(int face, int idVert) = 0;  // Vertex processor

  virtual bool fragment(Vec4f bar, TGAColor& color) = 0;  // pixel processor
};

void viewport(int w, int h, int x, int y);
void projection(float coeff = 0.f);  // coeff = -1/c
void lookat(Vec3f eye, Vec3f center, Vec3f up);

void drawTriangle(Vec3f points[], float z_buffer[], TGAImage* renderer,
                  IShader& shader, Vec2f windowDimensions);

void bufferToRender(SDL_Renderer* renderer, TGAImage* buffer);
