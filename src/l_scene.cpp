#include "l_scene.h"

struct transform {
  matrix<4, 4> m_transform;
};

Scene::Scene() { s_registry = entt::registry(); }

Scene::~Scene() { s_registry.clear(); }
