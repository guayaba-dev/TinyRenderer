#include "l_shaderDefinition.h"

#include <iostream>

#define DEPTH 255

Vec3f zBufferShader::vertex(Vec3f vert) {
  Vec4f gl_Vertex =
      (ViewPort * Projection * ModelView * embed<3, 4>(vert, 1.)).getCol(0);

  return proj<4, 3>(gl_Vertex / gl_Vertex[vW]);
}

bool zBufferShader::fragment(Vec4f bar, TGAColor& color) {
  float intensity = (ndc_tri * proj<4, 3>(bar)).getCol(0)[2] /
                    DEPTH;  // interpolate intensity for the current pixel

  color = TGAColor(255, 255, 255) * intensity;

  return false;
}

/*
Vec3f TexturingShader::vertex(Vec3f vertex) {
  varying_uv.setCol(model->textCoord(model->texture(face)[idVert]), idVert);

  Vec4f nrm =
      (uniform_MVIT *
       embed<3, 4>(model->vertexNomal(model->vertexNomalsIds(face)[idVert])))
          .getCol(0);
  varying_nrm.setCol(proj<4, 3>(nrm), idVert);

  Vec4f shadowVerts =
      (uniform_LMV * embed<3, 4>(model->vert(model->face(face)[idVert]), 1.))
          .getCol(0);
  varying_shadow_depth.setCol((shadowVerts / shadowVerts[3]), idVert);

  Vec4f glVertex = (Projection * ModelView *
                    embed<3, 4>(model->vert(model->face(face)[idVert]), 1.))
                       .getCol(0);
  varying_tri.setCol(glVertex, idVert);

  ndc_tri.setCol(proj<4, 3>(glVertex / glVertex[3]), idVert);
  glVertex = (ViewPort * glVertex).getCol(0);
  return proj<4, 3>(glVertex / glVertex[3]);
}

bool TexturingShader::fragment(Vec4f bar, TGAColor& color) {
  Vec3f normalBar = (varying_nrm * proj<4, 3>(bar)).getCol(0);
  Vec2f uvBar = (varying_uv * proj<4, 3>(bar)).getCol(0);
  Vec4f shadowMapBar = (varying_shadow_depth * bar).getCol(0);

  int idx = (int)shadowMapBar[0] +
            (int)shadowMapBar[1] * WIDTH;  // index in the shadowbuffer array

  float shadow = 0.3 + 0.7 * (z_ShadowBuffer[idx] < shadowMapBar[2] + 43.24);

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

  BTN.setCol(normalBar, 2);

  Vec3f normalMapped = (BTN * normalize(model->getNormal(uvBar))).getCol(0);

  float lightIntensity = std::max((normalMapped * lightDirection), 0.f);

  color = model->getDiffuse(uvBar) * shadow * lightIntensity;

  return false;
}
*/
