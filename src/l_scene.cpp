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

Scene::~Scene() { registry.clear(); }

void Scene::addModel(std::string src) {
  auto entity = registry.create();
  registry.emplace<render>(entity, 0., 0., 0., src);
}

auto& Scene::getRenderizable() {
  auto renderizable = registry.view<render>();
  return renderizable;
}
