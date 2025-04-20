#include "model.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "geometry.h"
#include "tgaimage.h"

Model::Model(const char *filename)
    : verts_(),
      tex_coords_(),
      vertexNomals(),
      faces_(),
      textures_(),
      vertexNomalsIds_() {
  std::ifstream in;
  in.open(filename, std::ifstream::in);
  if (in.fail()) return;
  std::string line;
  while (!in.eof()) {
    std::getline(in, line);
    std::istringstream iss(line.c_str());
    char trash;
    if (!line.compare(0, 2, "v ")) {
      iss >> trash;
      Vec3f v;
      for (int i = 0; i < 3; i++) iss >> v.raw[i];
      verts_.push_back(v);

    } else if (!line.compare(0, 3, "vt ")) {
      iss >> trash >> trash;
      Vec2f tuv;
      for (int i = 0; i < 2; i++) iss >> tuv.raw[i];
      tex_coords_.push_back(tuv);
    }

    else if (!line.compare(0, 3, "vn ")) {
      iss >> trash >> trash;
      Vec3f vn;
      for (int i = 0; i < 3; i++) iss >> vn.raw[i];
      vertexNomals.push_back(vn);
    }

    else if (!line.compare(0, 2, "f ")) {
      std::vector<int> f;
      std::vector<int> ft;
      std::vector<int> fvn;

      int idx, tidx, vnidx;
      iss >> trash;
      while (iss >> idx >> trash >> tidx >> trash >> vnidx) {
        idx--;  // in wavefront obj all indices start at 1, not zero
        tidx--;
        vnidx--;
        f.push_back(idx);
        ft.push_back(tidx);
        fvn.push_back(vnidx);
      }
      faces_.push_back(f);
      textures_.push_back(ft);
      vertexNomalsIds_.push_back(fvn);
    }
  }
  std::cerr << "# v# " << verts_.size() << " f# " << faces_.size() << std::endl;
  // load_texture(filename, "_diffuse.tga", diffusemap_);
  load_texture(filename, "_diffuse.tga", diffusemap_);
  // load_texture(filename, "_grid.tga", diffusemap_);
  load_texture(filename, "_nm_tangent.tga", normalmap_);
}

void Model::load_texture(std::string filename, const char *suffix,
                         TGAImage &img) {
  std::string texfile(filename);
  size_t dot = texfile.find_last_of(".");
  if (dot != std::string::npos) {
    texfile = texfile.substr(0, dot) + std::string(suffix);
    std::cerr << "texture file " << texfile << " loading "
              << (img.read_tga_file(texfile.c_str()) ? "ok" : "failed")
              << std::endl;
    img.flip_vertically();
  }
}

TGAColor Model::getDiffuse(Vec2f uvf) {
  Vec2i uv(uvf.x * diffusemap_.get_width(), uvf.y * diffusemap_.get_height());
  return diffusemap_.get(uv.x, uv.y);
}

Vec3f Model::getNormal(Vec2f uvf) {
  Vec2i uv(uvf.x * normalmap_.get_width(), uvf.y * normalmap_.get_height());
  TGAColor normalmap_Color = normalmap_.get(uv.x, uv.y);
  return Vec3f(normalmap_Color[0] / 255.f, normalmap_Color[1] / 255.f,
               normalmap_Color[2] / 255.f) *
             2.f -
         Vec3f(1., 1., 1.);
}

Model::~Model() {}

int Model::nverts() { return (int)verts_.size(); }

int Model::nfaces() { return (int)faces_.size(); }

std::vector<int> Model::face(int idx) { return faces_[idx]; }

std::vector<int> Model::texture(int tidx) { return textures_[tidx]; }

std::vector<int> Model::vertexNomalsIds(int vnidx) {
  return vertexNomalsIds_[vnidx];
}

Vec3f Model::vert(int i) { return verts_[i]; }

Vec2f Model::textCoord(int i) { return tex_coords_[i]; }

Vec3f Model::vertexNomal(int i) { return vertexNomals[i]; }
