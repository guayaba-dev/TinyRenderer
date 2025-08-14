#include <SDL2/SDL.h>

#include "l_matrix.h"
#include "l_shaderDefinition.h"
#include "tgaimage.h"

matrix<4, 4> viewport(int w, int h, int x, int y);
matrix<4, 4> projection(float coeff = 0.f);  // coeff = -1/c
matrix<4, 4> lookat(Vec3f eye, Vec3f center, Vec3f up);

void setRedererWindow(Vec2f& size);

void drawTriangle(Vec3f points[], float z_buffer[], TGAImage* renderer,
                  IShader& shader, Vec2f windowDimensions);

void bufferToRender(SDL_Renderer* renderer, TGAImage* buffer);
