#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>

#include "geometry.h"
#include "tgaimage.h"

class Model {
 private:
  TGAImage diffusemap_;
  std::vector<Vec3f> verts_;
  std::vector<Vec2f> tex_coords_;
  std::vector<Vec3f> vertexNomals;
  std::vector<std::vector<int> > faces_;     // id to all verts
  std::vector<std::vector<int> > textures_;  // id to all tex_coords_
  std::vector<std::vector<int> > vertexNomalsIds_;

 public:
  Model(const char *filename);
  ~Model();
  int nverts();
  int nfaces();
  Vec3f vert(int i);
  Vec2f textCoord(int i);
  Vec3f vertexNomal(int i);
  std::vector<int> face(int idx);
  std::vector<int> texture(int tidx);
  std::vector<int> vertexNomalsIds(int nidx);
  void load_texture(std::string filename, const char *suffix, TGAImage &img);
  TGAColor getDiffuse(Vec2f uvf);
};

#endif  //__MODEL_H__
