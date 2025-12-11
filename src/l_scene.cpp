#include "l_scene.h"

#include <string>

struct transform {
  matrix<4, 4> m_transform;
};

struct render {
  int x, y, z;
  std::string src;
};

Scene::Scene() = default;

Scene::~Scene() { s_registry.clear(); }
