#pragma once
#include <string>

#include "../.dependencies/entt/entt.hpp"
#include "l_matrix.h"
#include "model.h"

struct positon {
  float x, y;
};

struct model {};  // TODO: add model components

struct material {};  // TODO: add shader material

struct camera {};

class Scene {
 private:
  entt::registry s_registry;

 public:
  Scene();
  ~Scene();

  const entt::registry& getRegistry() { return s_registry; };
};
