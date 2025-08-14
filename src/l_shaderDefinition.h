#pragma once

#include "l_matrix.h"
#include "model.h"
#include "tgaimage.h"

struct IShader {
  virtual ~IShader();

  virtual Vec3f vertex(Vec3f vert) = 0;  // Vertex processor

  virtual bool fragment(Vec4f bar, TGAColor& color) = 0;  // pixel processor
};

struct zBufferShader : public IShader {
  Model* model = NULL;
  matrix<3, 3> ndc_tri = matrix<3, 3>();
  matrix<4, 4> ViewPort = matrix<4, 4>();
  matrix<4, 4> Projection = matrix<4, 4>();
  matrix<4, 4> ModelView = matrix<4, 4>();

  zBufferShader(matrix<4, 4> model, matrix<4, 4> viewPort,
                matrix<4, 4> projection)
      : ModelView(model), ViewPort(viewPort), Projection(projection) {};

  Vec3f vertex(Vec3f vert) override;  // Vertex processor

  bool fragment(Vec4f bar,
                TGAColor& color) override;  // pixel processor
};

/*
struct TexturingShader : public IShader {
  Model* model = NULL;
  matrix<2, 3> varying_uv = matrix<2, 3>();            // uv coords
  matrix<4, 4> varying_tri = matrix<4, 4>();           // triangle ModelView
  matrix<3, 3> varying_nrm = matrix<3, 3>();           // normal per vertex
  matrix<4, 4> varying_shadow_depth = matrix<4, 4>();  // DEPTH per vertex
  matrix<3, 3> ndc_tri = matrix<3, 3>();       // triangle in device coordenates
  matrix<4, 4> uniform_LMV = matrix<4, 4>();   // Matrix to lightPos
  matrix<4, 4> uniform_MV = matrix<4, 4>();    // Model view matrix
  matrix<4, 4> uniform_MVIT = matrix<4, 4>();  // ModelView inverse traspose
  matrix<4, 4> ViewPort = matrix<4, 4>();
  matrix<4, 4> Projection = matrix<4, 4>();
  matrix<4, 4> ModelView = matrix<4, 4>();

  virtual Vec3f vertex(Vec3f vertex) override;

  virtual bool fragment(Vec4f bar, TGAColor& color) override;
};
*/
