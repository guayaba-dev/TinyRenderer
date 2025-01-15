#include "model.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "geometry.h"

Model::Model(const char *filename)
    : verts_(), faces_(), tex_coords_(), textures_() {
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

    else if (!line.compare(0, 2, "f ")) {
      std::vector<int> f;
      std::vector<int> ft;

      int itrash, idx, tidx;
      iss >> trash;
      while (iss >> idx >> trash >> tidx >> trash >> itrash) {
        idx--;  // in wavefront obj all indices start at 1, not zero
        tidx--;
        f.push_back(idx);
        ft.push_back(tidx);
      }
      faces_.push_back(f);
      textures_.push_back(ft);
    }
  }
  std::cerr << "# v# " << verts_.size() << " f# " << faces_.size() << std::endl;
}

Model::~Model() {}

int Model::nverts() { return (int)verts_.size(); }

int Model::nfaces() { return (int)faces_.size(); }

std::vector<int> Model::face(int idx) { return faces_[idx]; }

std::vector<int> Model::texture(int tidx) { return textures_[tidx]; }

Vec3f Model::vert(int i) { return verts_[i]; }

Vec2f Model::textCoord(int i) { return tex_coords_[i]; }
