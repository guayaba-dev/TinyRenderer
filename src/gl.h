#include <SDL2/SDL.h>

#include "geometry.h"
#include "tgaimage.h"

extern Matrix Projection;
extern Matrix ModelView;
extern Matrix ViewPort;

struct IShader {
  virtual ~IShader();

  virtual Vec3f vertex(int face, int idVert) = 0;

  virtual bool fragment(Vec3f bar, TGAColor& color) = 0;
};

void viewport(int w, int h, int x, int y);
void projection(float coeff = 0.f);  // coeff = -1/c
void lookat(Vec3f eye, Vec3f center, Vec3f up);

void drawTriangle(Vec3f points[], float z_buffer[], SDL_Renderer* renderer,
                  IShader& shader, Vec2f windowDimensions);
